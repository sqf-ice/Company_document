#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <mach/regaccess.h>
#include <mach/pmb.h>
#include <mach/reg_gpio.h>
#include <linux/module.h>

#include "sam.h"
#include "board.h"
#include "icc_define.h"
#define SAM_DEBUG
#undef SAM_DEBUG
#ifdef SAM_DEBUG
#define sam_printk(fmt, arg...)  printk(fmt, ##arg)
#else
#define sam_printk(fmt, arg...)    
#endif

icc_ctrl_t *ctrl;
icc_ctrl_t ctrl_mem[5];
sam_stat_t sam_stat[5];
static void sam_ctrl_init(void)
{
	ctrl->timer_factor = TIMER_FACTOR;
	ctrl->rec_byte = 0;
    ctrl->D = 1;
    ctrl->F = 372;
    ctrl->t1 = 0;
    ctrl->IFSC = 0x20;
    ctrl->GTR = 0;
    ctrl->NAD = 0;
    ctrl->CWT = 11 + (1 << 13);
    ctrl->BWT = 11L + (1L << 4L) * 960L;
    ctrl->etu_count = (372 * ctrl->timer_factor);
}
int active_sam(uint8_t warm,uint8_t samnum)
{
	uint32_t start_cnt = 0;
	sam_select_num(samnum);
	sam_ctrl_init();
	sam_start_timer_cnt(0xFFFFFFFF);
	warm ? sam_warm_rst(samnum) : sam_cold_rst(samnum);
	start_cnt = sam_read_timer_value();
	start_cnt -= 100000 * ctrl->timer_factor;/*要乘以timer_factor ，42000 外面使用发现一张卡时间要求长*/
	while(sam_get_io()){
		if(start_cnt > sam_read_timer_value())
			return ERR_ATR_START_TIMEOUT;
	}
	ctrl->wwt_cnt = ctrl->start_cnt = sam_read_timer_value();
	return 0 ;
}
int atr_next_start(void)
{
    uint32_t cnt = 0;
    while(sam_get_io()) {
        cnt = sam_read_timer_value();
        if((ctrl->start_cnt - cnt) >  20160 * ctrl->etu_count)
            return ERR_ATR_TIMEOUT;
        if((ctrl->wwt_cnt - cnt) >  10080 * ctrl->etu_count)
            return ERR_ATR_BYTE_TIMEOUT;
    }
    ctrl->wwt_cnt =  sam_read_timer_value();
    return 0;
}
void get_atr_first_byte(void)
{
    uint8_t rbyte = 0,parity = 0, i = 0;
    uint32_t ti = 0;
    uint32_t timer_cnt = 0;
    uint32_t timer = 0;
    uint32_t start_cnt = 0;

    start_cnt = sam_read_timer_value();    
    while(!sam_get_io()){ /*wait for first bit H*/
		ti++;
		if(ti > 0x10000) return;
	}
    timer_cnt =  sam_read_timer_value();
    timer = (ctrl->wwt_cnt - timer_cnt) / ctrl->timer_factor;
    if(timer & 0x100) i = 1;
    else if(timer & 0x80) i = 2;
    else i = 4;

    if (i != 1) {
        ctrl->D = i;
        ctrl->etu_count = (372 / ctrl->D * ctrl->timer_factor);
    }
    
    rbyte = 1;
    parity = 1;
    ti = ctrl->etu_count + (ctrl->etu_count >> 2);/*first bit */
    for(i = 1;i < 8;i++) {
        ti += ctrl->etu_count;
        while(sam_read_timer_value()  >= (start_cnt- ti));
        rbyte <<= 1;
        if(sam_get_io()) {
            rbyte |= 0x01;
            parity ++;
        }
    }
    /* Even bit */
    ti += ctrl->etu_count;
    while (sam_read_timer_value() >= (start_cnt- ti));
    if(sam_get_io()) parity ++;
    if(ctrl->bit_reverse) ctrl->parity_count = 9 - parity;
    else ctrl->parity_count = parity;
    ctrl->rec_byte = rbyte;
    /* cross event bit */
    ti += ctrl->etu_count;
    while (sam_read_timer_value() >= (start_cnt- ti));
}
void get_byte(void)
{
    uint8_t rbyte = 0;
    uint8_t parity = 0;
    uint8_t i = 0;
    uint32_t ti = 0;
    uint32_t start_cnt = 0;
    start_cnt = sam_read_timer_value();
    ti = (ctrl->etu_count) >> 2;
    while (sam_read_timer_value() >= (start_cnt - ti));
    for (i = 0; i < 8; i++) {
        ti += ctrl->etu_count;
        while (sam_read_timer_value() >= (start_cnt - ti));
        rbyte <<= 1;
        if (sam_get_io()) {
            rbyte |= 0x01;
            parity ++;
        }
    }
    ti += ctrl->etu_count;
    while (sam_read_timer_value() >= (start_cnt - ti));
    if (sam_get_io()) {
        parity ++;
    }
    if(ctrl->bit_reverse)
        ctrl->parity_count = 9 - parity;
    else
        ctrl->parity_count = parity;
    ctrl->rec_byte = rbyte;
    ti += ctrl->etu_count;
    while(sam_read_timer_value()  >= (start_cnt - ti));
}
static void decode_byte(void)
{
    if (ctrl->bit_reverse) {
        ctrl->rec_byte = (~ctrl->rec_byte) & 0xFF;
    }
    else {
        uint8_t tmp = 0, tmp1 = ctrl->rec_byte;
        int i;
        for (i = 0; i < 8; i ++) {
            tmp >>= 1;
            if (tmp1 & 0x80) tmp |= 0x80;
            tmp1 <<= 1;
        }
        ctrl->rec_byte = tmp;
    }
}
int sam_atr_sequence(uint8_t warm, uint8_t * atrstr, int buflen,uint8_t samnum,icc_standard_t standard)
{
    int ret, index = 0, done = 0;
    unsigned char works[256];
    unsigned char check = 0, cur_byte;
    unsigned char historicalBytes = 0;
    unsigned char expectedCharacters = 0, no_expect_flag = 0;
    unsigned char reject_atr=0;
    unsigned char  TC2 = 0x0A,TB1=0,TA1 = 1, atrmode = NEGOTIABLE_MODE;
    unsigned int TB3 = -1;
    unsigned char interfaceIteration = 1;
	sam_stat[samnum].power_stat = 0;
	sam_stat[samnum].atr_stat = 0;
    if ((ret = active_sam(warm,samnum)) < 0){
        return ret;
    }
	get_atr_first_byte();
    if ((ctrl->rec_byte == 0xDC)) {
        cur_byte = 0x3B;
        ctrl->bit_reverse = 0;
    }
    else if ((ctrl->rec_byte == 0xC0)) {
        cur_byte = 0x3F;
        ctrl->bit_reverse = 1;
    }
    else return ERR_ATR_TS;
    works[index ++] = cur_byte;
    while (!done){
        if ((ret = atr_next_start()) < 0) return ret;
        get_byte();
        if (ctrl->parity_count & 0x01) return ERR_ATR_PARITY;
        decode_byte();
        cur_byte = ctrl->rec_byte;
        works[index ++] = cur_byte;
        check ^= cur_byte;
        if (index == 2)   /* T0 Character */{
            historicalBytes = cur_byte & 0x0F;      /* Find out historical bytes */
            expectedCharacters = cur_byte & 0xF0;   /* Figure out upcoming protocol conf bytes */
            if(expectedCharacters == 0) no_expect_flag = 1;
        }
        else {
            if (expectedCharacters & 0x10) /* TA */{
                expectedCharacters &= 0xE0;
                switch (interfaceIteration) {
                case 1:
                    /* TA1 : [Fi:Di]
                    If TA2 is return with b5 = 0, accept 0x11<= TA1 <= 0x13,
                    or reject it unless we are able to
                    If TA2 isn't returned, use default
                    */
                    TA1 = cur_byte;
                    break;
                case 2: /* TA2 */
                    if ((cur_byte & 0x0F) != ctrl->t1) reject_atr = 1; /* indicates the protocol to be used */
                    else if (cur_byte & 0x10) reject_atr = 1; /* b5 should = 0 */
                    else if (TA1 != 1 ) /* if TA1 is returned */ {
                        /* put TA1 test here, to see if TA2 is present */
                        if (TA1 < 0x11 || TA1 > 0x13) reject_atr = 1;
                        else {
                            if ((TA1 &= 0x0F) == 3) TA1 = 4;
                            atrmode = SPECIFIC_MODE_USE_TA1_F_D; /* use specific F/D */
                        }
                    }
                    break;
                case 3: /* TA3 */
                    /* The terminal shall reject an ATR containing TA3 having a
                    value in the range '00' to '0F' or a value of 'FF', default 0x20 */
                    if ((cur_byte < 0x10)|| (cur_byte == 0xFF)) {
                        reject_atr = 1;
                    }
                    ctrl->IFSC = cur_byte;
                    break;
                }
            }
            else if (expectedCharacters & 0x20) /* TB */ {
                expectedCharacters &= 0xD0;
                if (interfaceIteration == 1)   /* TB1 */ {
                    /* code reset only accept TB1 = 0x00 */
                    if (!warm && cur_byte != 0) reject_atr = 1;
                    TB1 = 1;
                }
                else if (interfaceIteration == 3) TB3 = cur_byte; /* TB3 checked below */
                else if (interfaceIteration == 2) reject_atr = 1; /* The terminal shall reject an ATR containing TB2 */
            }
            else if (expectedCharacters & 0x40) /* TC */{
                expectedCharacters &= 0xB0;
                switch (interfaceIteration) {
                case 1: /* TC1 */
                    if (!warm && !TB1) reject_atr = 1;
                    else ctrl->GTR = cur_byte;
                    break;
                case 2: /* TC2 */
                    if (cur_byte != 0x0A) reject_atr = 1;
                    else TC2 = cur_byte; /* accept an ATR containing TC2 = '0A' */
                    break;
                case 3: /* TC3 */
                    if (cur_byte != 0x00) reject_atr = 1; /* only 0x00 accpeted */
                    break;
                }
            }
            else if (expectedCharacters == 0x80) /* TD */ {
                expectedCharacters = (cur_byte & 0xF0);
                if (interfaceIteration == 1) {
                    if((cur_byte & 0xF) > 1) reject_atr = 1; /* the l.s. nibble having a value of '0' or '1' */
                    if((!warm) && (!TB1)) reject_atr = 1;
                    ctrl->t1 = cur_byte & 0x0F;
                }
                else if (interfaceIteration == 2) {
                    if((((cur_byte & 0x0F) | ctrl->t1) != 0xE) &&
                        ((cur_byte & 0x0F) != 1)) reject_atr = 1;
                }
                interfaceIteration ++;
            }
            else if (expectedCharacters == 0x02) /* TCK */ {
                done = 1;
            }
            else if (expectedCharacters == 0x01) /* Historical Bytes */{
                if (-- historicalBytes == 0) {
                    if (ctrl->t1 == 0) done = 1; /* only T=1 require checksum */
                    else expectedCharacters = 0x02; /* checksum, require TCK */
                }
            }
            /* if no historical bytes required, check if TCK needed */
            if (expectedCharacters == 0) {
                if (historicalBytes == 0) {
                    if (ctrl->t1 == 0) done = 1; /* only T=1 require checksum */
                    else expectedCharacters = 0x02; /* checksum, require TCK */
                }
                else {
    				if(no_expect_flag == 1) {
    					if (-- historicalBytes == 0) {
    						if (ctrl->t1 == 0) done = 1; /* only T=1 require checksum */
    						else expectedCharacters = 0x02; /* checksum, require TCK */
    					}
    				}
                	expectedCharacters = 0x01;
                }
            }
        }
    }
    if(ctrl->t1 == 1) {
        if ((check != 0) && (reject_atr == 0)) 
            return ERR_ATR_TCK_T1; /* FCS error */
        if ((ret = ctrl->GTR) == 0xFF) ret = -1;
        if((TB3 == -1) || ((TB3 & 0x0F) > 0x05) ||
                ((TB3 & 0xF0) > 0x40) || 
                ((1 << (TB3 & 0x0f)) <= (ret + 1))) {
            reject_atr = 1;
        }
    }
    if ((standard == emv) && (reject_atr)) return 0;
    if (atrmode == SPECIFIC_MODE_USE_TA1_F_D) {
        ctrl->D = TA1;
        ctrl->etu_count = ctrl->F / ctrl->D * ctrl->timer_factor;
    }
    ctrl->WWT = ctrl->D * (TC2 * 960L + 480L);
    if (TB3 != -1) {
        ctrl->CWT = 11 + (1 << (TB3 & 0x0F));
        ctrl->BWT = 11L + (1L << ((TB3 & 0xF0) >> 4)) * 960L * ctrl->D;
    }
    if(ctrl->t1 == 1) {
        ctrl->BWT += 960L * ctrl->D;
        ctrl->CWT += 4;
        ctrl->IFD_Seq = ctrl->ICC_Seq = ctrl->IFSD_Negotiated = 0;
        ctrl->BGT = 22 ;
        if (ctrl->GTR == 0xFF) {
            ctrl->GTR = 0;
            ctrl->GTR = (1 + ctrl->GTR);
        }
        else  {
            ctrl->GTR = (2 + ctrl->GTR) ;
        }
    }
    else {
        if (ctrl->GTR == 0xFF) ctrl->GTR = 0;
        ctrl->BGT = 16 ;
        ctrl->GTR = (1 + ctrl->GTR);
    }
    if (atrstr) {
        if (buflen < index) index = buflen;
        memcpy(atrstr, works, index);
    }
	if(samnum != g_ccm3310_num) {
		sam_stat[samnum].power_stat = 1;
		sam_stat[samnum].atr_stat = 3;
	}

    return index;
}

int send_byte(uint8_t byte,uint8_t last)
{
    uint8_t buf[10] = {0},even = 0;
    uint8_t i = 0,j = 0;
    uint32_t ti = 0;
    uint32_t start_cnt = 0;
    buf[0] = 0;
    if (ctrl->bit_reverse) {
        for (i = 1; i < 9; i ++) {
            if (byte & 0x80) {
                buf[i] = 0;
                even ++;
            }
            else buf[i] = 1;
            byte <<= 1;
        }
        buf[9] = (even & 0x01) ^ 0x01;
    }
    else {
        for (i = 1; i < 9; i ++) {
            if (byte & 0x01) {
                buf[i] = 1;
                even ++;
            }
            else buf[i] = 0;
            byte >>= 1;
        }
        buf[9] = even & 0x01;
    }

    for (i = 0; i < 5; i ++) {
        sam_set_io_type(output);
        start_cnt = sam_read_timer_value();
        ti = 0;
        for (j = 0; j < 10; j ++) {
            ti += ctrl->etu_count;
            sam_send_io(buf[j]);
            while(sam_read_timer_value() >= (start_cnt- ti));
        }
        sam_set_io_type(input);;
        if (ctrl->t1) goto end; /* no parity */
        ti += ctrl->etu_count;
        while(sam_read_timer_value()  >= (start_cnt- ti)); /* 11 etu */
        if (sam_get_io()) goto end;
        ti += ((ctrl->etu_count) << 1);
        while(sam_read_timer_value()  >= (start_cnt- ti)); /* 13 etu */
    }
    return ERR_SEND_PARITY;
end:
    if (!last) {
        sam_wait_etu_cnt(ctrl->GTR);
    }
    return 0;
}
#define wait_BGT()   sam_wait_etu_cnt(ctrl->BGT)
#define write_t0_byte(byte) send_byte(byte, 0)

static int write_t0_last_byte(uint8_t byte)
{
    int ret = 0;
    ctrl->wwt_cnt = sam_read_timer_value();
    if ((ret = send_byte(byte,1)) < 0)return ret;
    return 0 ;
}

static int read_t0_byte(void)
{
    uint8_t i= 0;
    uint32_t ti = 0;
    uint32_t cnt = 0;
    for (i = 0; i < 5; i ++) {
        sam_set_io_type(input);
        while(1) {
            cnt = sam_read_timer_value();
            if((ctrl->wwt_cnt - cnt) >  ctrl->WWT * ctrl->etu_count) {
                return ERR_RECV_TIMEOUT;
            }
            if (!sam_get_io()) break;
        }
        get_byte();
        if (ctrl->parity_count & 0x01) {
            cnt = sam_read_timer_value();
            ti = (ctrl->etu_count ) >> 2;
            /* at 10.5 etus */
            while(sam_read_timer_value()  >= (cnt - ti));
            sam_set_io_type(output);
            sam_send_io(0);
            ti += (ctrl->etu_count+ (ctrl->etu_count >> 1));
            while(sam_read_timer_value()  >= (cnt - ti));
        }
        else {
            decode_byte();
            sam_start_timer_cnt(0xffffffff);
            ctrl->wwt_cnt = sam_read_timer_value();
            return ctrl->rec_byte;
        }
    }
    return ERR_RECV_PARITY;
}

static int sam_send_apdu0(uint8_t *buffer, uint32_t length, uint8_t *rbuffer)
{
    int index, retval;
    uint8_t * hdr = buffer;
    uint8_t INS, P3 = 0, wait_for_proc_byte;
    uint8_t SW1=0, SW2=0, warning_status_byte = 0;
    uint8_t case4_cmd = 0, get_resp = 0, inverse_INS;
    
    uint8_t  GET_RESPONSE[5] = {0,0xC0,0,0,0};
	uint8_t case_t0 = 0;
	uint32_t rindex = 0, Lc = 0, Le = 0, val;
    /* identify command type */
	if(length == 4) case_t0 = 1;
	else if(length == 5) case_t0 = 2;
	else if(buffer[4] == length - 5) case_t0 = 3;
	else case_t0 = 4;
    sam_start_timer_cnt(0xffffffff);
    if (length > 5) {
        Lc = buffer[4];
        if(length > Lc + 5) {
            Le = buffer[length - 1];
            case4_cmd = 1;
        }
        P3 = Lc;
    }
    else if(length == 5) {
        Le = buffer[4];
        P3 = Le;
    }
    /* Follow BGT time before sending data at opposite direction */
    wait_BGT();
    while (1) {
        sam_start_timer_cnt(0xffffffff);
        INS = hdr[1];
        inverse_INS = ~INS;
        wait_for_proc_byte = 1;

        /* write 5 byte command header */
        for (index = 0; index < 4;){
            if ((retval= write_t0_byte(hdr[index ++])) < 0){
                return retval;
            }
        }
        if ((retval = write_t0_last_byte(P3)) < 0) {
            return retval;
        }
        index = 5;
        while (wait_for_proc_byte) {
            if ((retval = read_t0_byte()) < 0) {
                return retval;
            }
            val = retval;
            switch (val) {
            case 0x61: /* Card wants GET Response to get APDU response */
                get_resp = 1;
            case 0x6C: /* Card wants to repeat APDU header with expect APDU response byte */
                Lc = 0;
                hdr = (val == 0x61) ? GET_RESPONSE:buffer;
                P3 = Le = retval = read_t0_byte ();
                Le += rindex;
                if(retval < 0) return retval;
				/*add by guoc 2014-01-13 if case 1 &3 return val not get respone*/
				if(case_t0 == 1 || case_t0 == 3){
					rbuffer[rindex ++] = val;
					rbuffer[rindex ++] = retval;
					return rindex;
				}
				if(Le > (RECEIVE_DATA_MAX - 2)) {
					rbuffer[rindex ++] = val;
					rbuffer[rindex ++] = retval;
					return rindex;
				}
                wait_for_proc_byte = 0;
                wait_BGT();
            case 0x60: /* Card wants to have additional WWT */
                break;
            default:
                if((val == INS) || (val == inverse_INS))   /* all remaining data or next data byte */
                {
                    if (Lc + 5 == index)   /* LC have been send out */ {
                        /* receive data from ICC */
                        while((Le > rindex) && ((retval = read_t0_byte()) >= 0)) {
                            rbuffer[rindex ++] = retval;
                            if(val == inverse_INS) {
                                break;
                            }
                        }
                        if (retval < 0) return retval;
                        break; /* wait for next proc byte or status byte */
                    }
                    wait_BGT();

                    /* if we received INS byte, send all remaining bytes, otherwise, send only one byte */
                    if(val == INS) {
                        for (; index < (Lc + 4); index ++) {
                            if ((retval = write_t0_byte(buffer[index])) < 0) return retval;
                        }
                    }
                    if ((retval = write_t0_last_byte(buffer[index ++])) < 0) return retval;
                    break; /* wait for next proc byte or status byte */
                }
                if (((val & 0xf0) != 0x60) && ((val & 0xf0) != 0x90)) return ERR_INVALID_STATUS_BYTE;
                if ((retval = read_t0_byte()) < 0) return retval; /* SW2 */
                if (!warning_status_byte) {
                    SW2 = retval;
                    SW1 = val;
                    /* Check status byte to see whether card has more data to read */
                    if((((SW1 == 0x62) || (SW1 == 0x63)) ||
                            (((SW1 & 0x90) ==0x90) && (!((SW1 == 0x90) && (SW2 == 0))))) &&
                            ((Lc + 5) == index) && (case4_cmd) && (get_resp == 0)){
                        warning_status_byte = 1;

                        Lc = 0;
                        hdr = buffer = GET_RESPONSE;
                        P3 = Le = 0;
                        Le += rindex;
        				if(Le > (RECEIVE_DATA_MAX - 2)) {
        					rbuffer[rindex ++] = val;
        					rbuffer[rindex ++] = retval;
        					return rindex;
        				}
                        wait_for_proc_byte = 0;
                        wait_BGT();
                    }
                    else    /* Card completed APDU response, send response with Status code */{
                        rbuffer[rindex ++] = SW1;
                        rbuffer[rindex ++] = SW2;
                        return rindex;

                    }
                }
                else {
                    /*case 4 received 0x62 xx send not 90 00 send data 62 xx add by guoc */
                    if((rindex == 0)||(warning_status_byte == 1)) {
                        /* If card did not send any response, send first warning status code to application */
                        rbuffer[rindex ++] = SW1;
                        rbuffer[rindex ++] = SW2;
                    }
                    else {
                        /* Send APDU response with latest warning status code */
                        rbuffer[rindex ++] = val;
                        rbuffer[rindex ++] = retval;
                    }
                    return rindex;
                }
                break;
            }
        }
    }
}
#define T1_BLOCK_RETRY    	(3)     		/* Number of retries */
#define NONIBLOCK   		(0x80)        /* Non I Block indicator */
#define SBLOCK      		(0x40)        /* S Block indicator     */
#define MOREBIT     		(0x20)        /* More bit indicator    */

#define SBLOCKFUNCMASK		(0x3F)    	/* S-block function mask */
#define CODESBLOCK(x)		(uint8_t)((x) | NONIBLOCK | SBLOCK)  /* Generates SBLOCK PCB BYTE */
#define CODERBLOCK(nr,rc)   (uint8_t)(NONIBLOCK | ((nr) << 4) | (rc)) /* Generates RBLOCK PCB BYTE */
#define CODEIBLOCK(ns,mb)   (uint8_t)(((ns) << 6) | ((mb) << 5)) /* Generates IBLOCK PCB BYTE */
#define ISIBLOCK(pcb)    	(!((pcb) & NONIBLOCK))       /* Returns TRUE if pcb belongs to I BLOCK */
#define ISRBLOCK(pcb)		((pcb & NONIBLOCK) && (!(pcb & SBLOCK)) && (!(pcb & MOREBIT)))  /* Returns TRUE if pcb belongs to R BLOCK */
#define ISSBLOCK(pcb)		(((pcb) & NONIBLOCK) && ((pcb) & SBLOCK)) /* Returns TRUE if pcb belongs to S BLOCK */
#define NR(pcb)				(((pcb) & 0x10) >> 4)		/* Retrieves R_Block Sequence number from pcb*/
#define NS(pcb)				(((pcb) & 0x40) >> 6)		/* Retrieves I_Block Sequence number from pcb*/
#define RERR(pcb)			((pcb) & 0x0F)			/* Retrieves Error field in R-blck pcb  */
#define MORE(pcb)			(((pcb) & MOREBIT) >> 5)	/* Retrieves More data bit info from pcb*/
#define SBLOCKFUNC(pcb)		((pcb) & SBLOCKFUNCMASK)	/* Retrieves S-block function code */
#define SREQUEST_ABORT		(0xC2) /* S-block Abort operation function code */
#define SREQUEST_IFS		(0xC1) /* S-block IFS Request function code */
#define SREQUEST_WTX		(0xC3) /* S-block WTX Request function code */
#define SRESPONSE_IFS		(0xE1) /* S-block IFS Response function code */
#define SRESPONSE_WTX		(0xE3) /* S-block WTX Response function code */
#define SRESPONSE_ABORT		(0xE2) /* S-block Abort Response function code */
#define generateEDC(onebyte, value) (value ^ onebyte)
static uint8_t t1_parity_error = 0;
static uint8_t t1_sblock_sent = 0;

static int read_t1_byte(uint8_t first)
{
    volatile uint32_t cnt = 0;
    uint32_t compar_cnt = 0;
    uint32_t timeout = 0;
    if (ctrl->wtx > 0) {
        timeout = (1L * ctrl->BWT * ctrl->wtx);
        ctrl->wtx = 0; /*clear*/
    }
    else timeout = ctrl->BWT;
    compar_cnt = first ? timeout * ctrl->etu_count :ctrl->CWT * ctrl->etu_count;
    sam_set_io_type(input);
    while(1) {
        cnt = sam_read_timer_value();
        if(cnt < (ctrl->start_cnt - compar_cnt)){
            return ERR_RECV_TIMEOUT;
        }
        if (!sam_get_io()) break;
    }
    get_byte();
    if (ctrl->parity_count & 0x01) {
        t1_parity_error = 1;
    }
    decode_byte();
    ctrl->start_cnt = sam_read_timer_value();
    return ctrl->rec_byte;
}

static void send_block(uint8_t NAD, uint8_t PCB, uint8_t length, uint8_t *buffer)
{
    uint8_t i = 0;
    uint8_t EDC = 0;
    uint8_t len = 0;
    wait_BGT();
    len = length;
    sam_start_timer_cnt(0xffffffff);
    EDC = generateEDC(NAD, EDC);
    EDC = generateEDC(PCB, EDC);
    EDC = generateEDC(len, EDC);

    for(i = 0; i < length; i++) {
        EDC = generateEDC(buffer[i], EDC);
    }
    send_byte(NAD, 0);
    send_byte(PCB, 0);
    send_byte(len, 0);
    for(i = 0; i < length; i++) {
        send_byte(buffer[i], 0);
    }
    send_byte(EDC, 1);
    ctrl->start_cnt = sam_read_timer_value();
}

static int receive_block(uint8_t *rNAD, uint8_t *rPCB, 
    uint8_t *rLEN, uint8_t * buffer)
{
    int retval;
    int index = 0;
    int i;
    uint8_t EDC;

    t1_parity_error = 0;

    /* Get NAD, PCB, and Length */
    if ((retval = read_t1_byte(1)) < 0)
        goto exit_from_receiveblock;

    *rNAD = retval;

    if ((retval = read_t1_byte(0)) < 0)
        goto exit_from_receiveblock;

    *rPCB = retval;

    if ((retval = read_t1_byte(0)) < 0)
        goto exit_from_receiveblock;

    *rLEN = retval;
    /* Add additional byte if using CRC
    Get all data bytes plus EDC (1 byte at end) */
    for (i = 0; i <= *rLEN; i++) {
        if ((retval = read_t1_byte(0)) < 0)
            goto exit_from_receiveblock;
        buffer[index ++] = retval;
    }
    /* compute LRC of received block */
    EDC = 0;
    EDC = generateEDC(*rNAD,EDC);
    EDC = generateEDC(*rPCB,EDC);
    EDC = generateEDC(*rLEN,EDC);
    for (i = 0; i <= *rLEN; i++) {
        EDC = generateEDC(buffer[i], EDC);
    }
    if(t1_parity_error)   /* check if there is any parity error */ {
        retval = ERR_RECV_PARITY;
        goto exit_from_receiveblock;
    }
    if (EDC != 0) {
        retval = ERR_RECV_LRC; /* Check if there is any LRC error */
        goto exit_from_receiveblock;
    }
    if (*rNAD != 0)   /* Check the received T1 block format */ {
        retval = ERR_INVALID_NAD;
        goto exit_from_receiveblock;
    }
    if((ISRBLOCK(*rPCB)&&(*rLEN!=0)) ||
        (ISSBLOCK(*rPCB)&&(*rLEN>1))) {
        retval= ERR_INVALID_LEN;
    }
    else {
        retval=*rLEN;
    }
exit_from_receiveblock:
    /* if length is FF, abort APDU transaction */
    if(*rLEN==0xFF) {
        return ERR_ABORT_T1_TRANSACTION;
    }

    return retval;
}

static int send_sblock_IFSD(uint8_t IFSD)
{
    uint8_t buffer = IFSD;
    uint8_t rNAD, rPCB, rLEN;
    uint8_t rbuffer[256];
    uint8_t count = T1_BLOCK_RETRY;
    do {
        /*  S(IFS request) block with PCB = 'C1' and with IFSD = 254 */
        send_block(0, 0xC1, 1, &buffer);
        if((receive_block(&rNAD, &rPCB, &rLEN, rbuffer)) < 0) {
            continue;
        }
        /* The PCB of the S(IFS response) block sent
           in response shall have the value 'E1', and the INF field shall have the same
           value as the INF field of the block requesting the change. */
        if ((rPCB == 0xE1) && (rLEN == 1) && (rbuffer[0] == IFSD)) {
            return 0;
        }
    }
    while(--count);

    return ERR_SETIFSD_FAILURE;
}
#define set_NAD(nad)	ctrl->NAD = (nad)
static int receive_sblock(uint8_t *rNAD, uint8_t *rPCB,
        uint8_t *rLEN, uint8_t * rbuffer)
{
    int ret;
    int Responsed;

    t1_sblock_sent = 0;

    do {
        Responsed = 0;
        if((ret = receive_block(rNAD, rPCB, rLEN, rbuffer)) < 0) {
            return ret;
        }

        switch (*rPCB) {
        case SREQUEST_ABORT :
        {
            /* Card requests to ABORT, send ABORT error code to abort transaction */
            return ERR_ABORT_T1_TRANSACTION;
        }
        case SREQUEST_IFS :
            /* Card sends IFSC request, Check if it is valid IFSC request */
            if((*rbuffer < 0x10) || (*rbuffer == 0xff)) {
                return ERR_INVALID_INF;
            }
            /* /Send IFSC response */
            send_block(0, SRESPONSE_IFS, 1, rbuffer);
            ctrl->IFSC = *rbuffer;   /* Store new IFSC value in buffer */
            Responsed = 1; 	/* Set t1 status flags to maintain protocol state */
            t1_sblock_sent = 1;
            break;

        case SREQUEST_WTX :
            /* Card sends WTX Request */
            ctrl->wtx = *rbuffer;       /* Store new WTX value in wtx buffer */
            send_block(0, SRESPONSE_WTX, 1, rbuffer); /* Send wtx response to card */
            /* After the ICC responds,
            BWT is again used as the time allowed for the ICC to process the I-block */
            //ctrl->wtx = 0;
            Responsed = 1;
            t1_sblock_sent = 1;
            break;
        }

    } while (Responsed);
    return ret;
}

static int sam_send_apdu1(uint8_t *req_buf, uint16_t req_len, uint8_t *resp_buf)
{
    int ret;
    int currentIBlockLen, more, retry, i_block_retry;
    int totalResLength;
    uint8_t rNAD, rblock_sent=0;
    int cardiblock_err=0;
    uint8_t rPCB;
    uint8_t rLEN;
    uint8_t *IFDSeq, *ICCSeq, works[256];
    IFDSeq = &(ctrl->IFD_Seq);
    ICCSeq = &(ctrl->ICC_Seq);
    sam_start_timer_cnt(0xffffffff);
    /* Make sure IFSD Negotiation takes place once before any APDU exchange for T1 protocol */
    if (ctrl->IFSD_Negotiated == 0) {
        if (send_sblock_IFSD(0xFE) < 0) return -1;
        ctrl->IFSD_Negotiated = 1;
    }
    /* Send APDU request */
    while (req_len) {

        /* We do chaining if request length is greater than IFSC */
        currentIBlockLen = (req_len < ctrl->IFSC) ? req_len : ctrl->IFSC;
        req_len -= currentIBlockLen;
        more = req_len ? 1 : 0;

        retry = T1_BLOCK_RETRY;

        i_block_retry = 1;
        cardiblock_err = 0;

        send_block(0, CODEIBLOCK(*IFDSeq, more), currentIBlockLen,req_buf);
        /* make sure block gets sent successfully within three times */
        while (retry--) {
            /* receive response from card for I-block */
            if ((ret = receive_sblock(&rNAD, &rPCB, &rLEN, works)) >= 0) {
                if((ISRBLOCK(rPCB)))   /* Check whether card sent R-Block */ {
                    /* Check IFDseq to see whether wants to re-transmit previous block */
                    if((NR(rPCB) == *IFDSeq) && (!t1_sblock_sent)) {
                        if(((++i_block_retry) > 3)) {
                            retry = -1;
                            ret = -2;
                            break;
                        }
                        send_block(0,CODEIBLOCK(*IFDSeq,more), currentIBlockLen, req_buf);
                        retry ++;
                        continue;
                    }
                    else {
                        /* Card sent ACK for previous i-block, send next chained i-block */
                        if (more) {
                            *IFDSeq = 1 - *IFDSeq;
                            break;
                        }
                        else if(((!rblock_sent) && (retry < 2))) {
                            /* there could be error in icc response to last i-block */
                            send_block(0,
                                       CODERBLOCK(*ICCSeq, ((cardiblock_err == ERR_RECV_LRC) ||
                                       (cardiblock_err==ERR_RECV_PARITY)) ? 1 : 2), 0,(void*)0);
                            retry ++;
                            rblock_sent = 1;
                            continue;
                        }
                    }
                }
                /* Card started sending APDU response, getinto receive APDU state machine */
                else if (ISIBLOCK(rPCB)) {
                    if (((NS(rPCB) == *ICCSeq) && (more==0))) {
                        *IFDSeq = 1 - *IFDSeq;
                        break;
                    }
                }
            }
            /* Card wants to abort T1 transaction, return with Abort Error Code */
            if(ret== ERR_ABORT_T1_TRANSACTION) return ret;
            if(retry) {
                if((cardiblock_err))
                    ret = cardiblock_err;
                /* send R block saying we got invalid block */
                send_block(0,CODERBLOCK(*ICCSeq, 
                ((ret==ERR_RECV_LRC)||(ret==ERR_RECV_PARITY)) ? 1 : 2), 0,(void*)0);
                cardiblock_err =ret;
            }
        }
        /* The transaction errors exceeded EMV limit, so, return error code */
        if (retry < 0) return ERR_MAX_RETRIED;
        req_buf += currentIBlockLen;
    }
    totalResLength = 0;

    do {
    	if((totalResLength + rLEN) > RECEIVE_DATA_MAX) return ERR_RECEIVE_DATA_OVER;
        memcpy(resp_buf, works, rLEN);
        resp_buf += rLEN;
        totalResLength += rLEN;

        *ICCSeq = 1 - *ICCSeq;
        /* Check whether there are more chained APDU responses blocks */
        if (MORE(rPCB)) {
            send_block(0,CODERBLOCK(*ICCSeq, 0), 0, (void*)0); /* Send ACK for received I-block */
            retry = T1_BLOCK_RETRY;

            while (retry) {
                ret = receive_sblock(&rNAD, &rPCB, &rLEN, works); /* Receive Next Block from Card */

                if(ret >= 0) {
                    if (ISRBLOCK(rPCB))   /* Card did not receive our ACK properly, resend it */ {
                        if (!(NR(rPCB) != *IFDSeq)) {
                            send_block(0,CODERBLOCK(*ICCSeq, 0), 0,(void*)0);
                            continue;
                        }
                    }
                    else if (ISIBLOCK(rPCB)) {
                        if (NS(rPCB) == *ICCSeq) break;
                    }
                }

                if(ret== ERR_ABORT_T1_TRANSACTION) return ret;

                if(--retry)  send_block(0,CODERBLOCK(*ICCSeq, 0), 0,(void*)0);
            }

            if (!retry) return ERR_MAX_RETRIED;
        }
        else {
            break;
        }
    }
    while (1);
    return totalResLength;
}
int sam_send_apdu(uint8_t *buffer, uint32_t length, uint8_t *rbuffer,uint8_t samnum)
{
	int ret = -1;
	if(samnum > (g_sam_card_num + 1))
		return -4;
	ctrl = &ctrl_mem[samnum];
	sam_select_num(samnum);
	switch (ctrl->t1) {
	case 0:
		ret = sam_send_apdu0(buffer,length,rbuffer);
		break;
	case 1:
		ret = sam_send_apdu1(buffer,length,rbuffer);
		break;
	default:
		break;
	}
	if(ret < 0)
	{
		sam_power_down(samnum);/*error power down*/
		sam_printk("apdu err %d \n",ret);
	}
	return ret;
}
int sam_powerup(uint8_t *atrstr,uint8_t len,uint8_t samnum,icc_standard_t standard)
{
	int ret = 0;
	if(samnum > (g_sam_card_num + 1))
		return -4;
	ctrl = &ctrl_mem[samnum];
	sam_select_num(samnum);
	if((ret = sam_atr_sequence(0, atrstr, len,samnum,standard)) < 0)
	{
		sam_power_down(samnum);
		sam_printk("sam cold  reset err %d\n",ret);
		return ret;
	}
	if(ret == 0)
	{
		/*goto warm rst*/
		//sam_printk("sam go warm reset \n");
		if((ret = sam_atr_sequence(1, atrstr, len,samnum,standard)) <= 0)/*=0 powerdown*/
		{
			sam_power_down(samnum);
			sam_printk("sam warm reset err %d \n",ret);
			return -2;
		}
	}
	return ret;
}
#include <linux/spinlock.h>
#include "sc_linux.h"
extern spin_lock sc_lock;
int ccm3310_powerup(uint8_t *atrstr, uint8_t len) 
{
	int ret = 0;
	if(g_ccm3310_num == 0) return -1;
	spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
    ret = sam_powerup(atrstr, len, g_ccm3310_num, iso7816);
    spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
    return ret;
}
int ccm3310_apdu(uint8_t *buffer, uint32_t length, uint8_t *rbuffer)
{
	int ret = 0;
	if(g_ccm3310_num == 0) return -1;
	spin_lock_irqsave(&sc_lock.lock, sc_lock.flag);
    ret = sam_send_apdu(buffer, length, rbuffer, g_ccm3310_num);
    spin_unlock_irqrestore(&sc_lock.lock, sc_lock.flag);
    return ret;
}
int ccm3310_powerdown(void)
{
	if(g_ccm3310_num == 0) return -1;
	sam_power_down(g_ccm3310_num);
	return 0;
}
EXPORT_SYMBOL(ccm3310_powerup);
EXPORT_SYMBOL(ccm3310_powerdown);
EXPORT_SYMBOL(ccm3310_apdu);
