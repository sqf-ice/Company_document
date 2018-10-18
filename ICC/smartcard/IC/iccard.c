#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <mach/reg_gpio.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/module.h>
#include "bcm5892sc_define.h"
#include "iccard.h"
#include "icc_define.h"
#include "board.h"
#include "bcm5892_define.h"
#include "sam.h"
#define SC_HW_DEBUG 1
//#undef SC_HW_DEBUG

#if SC_HW_DEBUG
#define sc_printk(fmt, arg...) printk(fmt, ##arg)
#else
#define sc_printk(fmt, arg...)       
#endif
//#define sc_printk(fmt, arg...) printk(fmt, ##arg)
#if SC_HW_DEBUG
	#define SC_ATR_ERR_DEBUG_EN        1
	#define SC_ATR_DATA_DEBUG_EN       1
	#define SC_T0_ERR_DEBUG_EN         1
	#define SC_T0_SDATA_DEBUG_EN       1
	#define SC_T0_RDATA_DEBUG_EN       1
	#define SC_T1_ERR_DEBUG_EN         1
	#define SC_T1_SDATA_DEBUG_EN       1
	#define SC_T1_RDATA_DEBUG_EN       1
	#define SC_POWON_DEBUG_EN          1
#endif
	
#if 0
#define print_log(fmt, args) printk("*<in %s at line %d>*"fmt"\n", __FILE__, __LINE__, ##args)
#endif

/*atr全局变量，保存接收的数据*/
atr_t g_atr;
 /*t1发送s块标志位*/
static uint8_t t1_sblock_sent = 0;
unsigned long g_iccard_insert_cnt = 0;
#define SC_4_SPEED
//#undef SC_4_SPEED
#define EMV_TEST
/******************************************************************
 函数名：void ic_atr_init(uint8_t d)
 输入 :atr初始化的速率。
 输出 : no
 功能描述：atr参数初始化
 作者：guoc
 日期：2013-02-01
 版　本：1.00
 ******************************************************************/
static void ic_atr_init(uint8_t d)
{
	/* default value */
	g_atr.F = 372;
	g_atr.D = d;
	g_atr.IFSC = 0x20;
	g_atr.NAD = 0;
	g_atr.CWT = 11 + (1 << 13);
	g_atr.BWT = 11L + (1L << 4L) * 960L;
	g_atr.GTR = 0;
	g_atr.T = 0;
	g_atr.cwi = 0;
	g_atr.ATROK = 0;
	g_atr.Vcc = 0;
}
 /******************************************************************
  函数名：int atr_get_first_byte(void)
  输入 :no
  输出 : int 接收到的字符或者错误值
  功能描述：获取ATR第一个字符，在冷热复位后调用
  并设置好ATR时模块。
  作者：guoc
  日期：2013-02-01
  版　本：1.00
  ******************************************************************/
static int atr_get_first_byte(void)
{
	uint8_t value = 0,retval = 0;
	timer_ctrl_type timer_ctrl;
	uint32_t wwt = 0,i = 0;
	value =reg_read8(SMARTCARD_BASE + SC_P_UART_CMD_1);
	value |= SC_UART_CMD_1_get_atr_MASK;
	value |= SC_UART_CMD_1_io_en_MASK;
	value &= (~SC_UART_CMD_1_t_r_MASK);
	value &= (~SC_UART_CMD_1_RESERVED_MASK);
	reg_write8(SMARTCARD_BASE + SC_P_UART_CMD_1,value);
	/*42000 clk rece atr first bit*/
	timer_ctrl.cnt = 42250;
	timer_ctrl.mode = start_immediate;
	timer_ctrl.src = clk;
	timer_ctrl.onoff = enable;
	ctrl_ic_timer(&timer_ctrl);
	/*set wwt time wwt+1*/
	wwt = 10081;
	reg_write8(SMARTCARD_BASE + SC_P_WAIT_1,(wwt & 0xff));
	reg_write8(SMARTCARD_BASE + SC_P_WAIT_2,(wwt & 0xff00) >> 8);
	reg_write8(SMARTCARD_BASE + SC_P_WAIT_3,(wwt & 0xff0000) >> 16);
	
	value = reg_read8(SMARTCARD_BASE + SC_P_TIMER_CMD);
	value |= SC_TIMER_CMD_wait_en_MASK;
	value &= (~SC_TIMER_CMD_wait_mode_MASK);/*wwt mode*/
	reg_write8(SMARTCARD_BASE + SC_P_TIMER_CMD,value);
	value = reg_read8(SMARTCARD_BASE + SC_P_INTR_STAT_2);/*clear wwt time out*/
	do{
		value = __raw_readb(SMARTCARD_BASE + SC_P_INTR_STAT_2);
		if(check_ic_timer_over()){
			#if SC_ATR_ERR_DEBUG_EN
			sc_printk("first\n");
			#endif
			return ERR_ATR_START_TIMEOUT;
		}
			
	}while((value & SC_INTR_STAT_2_atrs_intr_MASK) != SC_INTR_STAT_2_atrs_intr_MASK);
	timer_ctrl.cnt = 20160;
	timer_ctrl.src = etu;
	ctrl_ic_timer(&timer_ctrl);
	i = 0;
	do{
		value = reg_read8(SMARTCARD_BASE + SC_P_INTR_STAT_2);
		i++;
		if(i > 0xfff) return ERR_ATR_TIMEOUT;
	}while((value & SC_INTR_STAT_2_rcv_intr_MASK) != SC_INTR_STAT_2_rcv_intr_MASK);
	retval = reg_read8(SMARTCARD_BASE + SC_P_RECEIVE);
	value = reg_read8(SMARTCARD_BASE + SC_P_STATUS_2);
	if((value & SC_STATUS_2_rpar_err_MASK) == SC_STATUS_2_rpar_err_MASK){	
		#if SC_ATR_ERR_DEBUG_EN
		sc_printk("a.par %02x\n",(int)retval);
		#endif
		return ERR_ATR_PARITY;
	}
	#if SC_ATR_DATA_DEBUG_EN
	sc_printk("%02x ",(int)retval);
	#endif 
	return (int)retval;
}
 /******************************************************************
  函数名： int atr_get_next_byte(void)
  输入 :no
  输出 : int 接收到的字符或者错误值
  功能描述：获取ATR下一个字符，用于ATR解析
  作者：guoc
  日期：2013-02-01
  版　本：1.00
  ******************************************************************/
static int atr_get_next_byte(void)
{
	uint8_t value = 0,retval = 0;
	do{
		value = reg_read8(SMARTCARD_BASE + SC_P_INTR_STAT_2);
		if((value & SC_INTR_STAT_2_wait_intr_MASK) == SC_INTR_STAT_2_wait_intr_MASK){
			#if SC_ATR_ERR_DEBUG_EN
			sc_printk("wwt\n");
			#endif
			return ERR_ATR_BYTE_TIMEOUT;
		}
		if(check_ic_timer_over())
		{
			#if SC_ATR_ERR_DEBUG_EN
			sc_printk("atr\n");
			#endif
			return ERR_ATR_TIMEOUT;
		}
	}while((value & SC_INTR_STAT_2_rcv_intr_MASK) != SC_INTR_STAT_2_rcv_intr_MASK);
	retval = reg_read8(SMARTCARD_BASE + SC_P_RECEIVE);
	value = reg_read8(SMARTCARD_BASE + SC_P_STATUS_2);
	if((value & SC_STATUS_2_rpar_err_MASK) == SC_STATUS_2_rpar_err_MASK)
	{	
	#if SC_ATR_ERR_DEBUG_EN
		sc_printk("2.par %02x\n",(int)retval);
	#endif
		return ERR_ATR_PARITY;
	}

	#if SC_ATR_DATA_DEBUG_EN
	sc_printk("%02x ",(int)retval);
	#endif
	return (int)retval;
}

 /******************************************************************
函数名：int icc_atr_sequence(unsigned char warm, unsigned char * atrstr, int buflen)
输入 :warm = 0 冷复位=1热复位, atrstr，接收指针, buflen 接收长度
输出 : int  ATR解析收到的字符长度或者出错
功能描述：ATR解析，读取ATR字符并解析存入g_atr结构体
******删除了T0卡的接收tck,gtr变化取消*******
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
 int icc_atr_sequence(uint8_t warm,uint8_t d, uint8_t * atrstr, int buflen,icc_standard_t standard)
{
	
	uint8_t cur_byte, done = 0, index = 0, check = 0;
	uint8_t historicalBytes = 0;
	uint8_t expectedCharacters = 0, no_expect_flag = 0;
	uint8_t reject_atr=0;
	uint8_t TCK, TC2 = 0x0A, TA1 = 1,TB1=0, atrmode = NEGOTIABLE_MODE;
	uint8_t interfaceIteration = 1;
	uint8_t works[256];
	int result;
	uint32_t TB3 = -1;
	ic_atr_init(d);/*init param */
	if ((result = atr_get_first_byte()) < 0) {
		//if(result == ERR_ATR_PARITY)
		return result; /* TS Character */
	}
	cur_byte = result;
	if (cur_byte != 0x3B && cur_byte != 0x3F) {
		wait_ic_etu_cnt(1);
		//printk("ts %x \n",cur_byte);
		return ERR_ATR_TS;
	}
	works[index ++] = cur_byte;
	while (!done) {
 		if ((result = atr_get_next_byte()) < 0) {
			return result;
 		}
 		if (index==1 && (result == 0x3F || result == 0x3B)) { /* we maybe receive a invalid byte which is the same with*/
 			continue;                                           /* the first byte when T1 mode changed into T0 mode */
 		}
		cur_byte = result;
		works[index ++] = cur_byte;
		check ^= cur_byte;/*TCK*/

		if (index != 2) {
			if (expectedCharacters & 0x10) /* TA */ {
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
						if ((cur_byte & 0x0F) != g_atr.T) reject_atr = 1; /* indicates the protocol to be used */
						else if (cur_byte & 0x10) reject_atr = 1; /* b5 should = 0 */
						else if (TA1 !=1 ) /* if TA1 is returned */{
							/* put TA1 test here, to see if TA2 is present */
							if (TA1 < 0x11 || TA1 > 0x13) {
								reject_atr = 1;
							}
							else {
								if ((TA1 &= 0x0F) == 3) TA1 = 4;
								atrmode = SPECIFIC_MODE_USE_TA1_F_D; /* use specific F/D */
							}
						}
						break;
					case 3: /* TA3 */
						/* The terminal shall reject an ATR containing TA3 having a
							value in the range '00' to '0F' or a value of 'FF', default 0x20 */
						if((cur_byte < 0x10)|| (cur_byte == 0xff)){
							reject_atr = 1;
						}
						g_atr.IFSC = cur_byte;
						break;
				}
			}
			else if (expectedCharacters & 0x20) /* TB */ {
				expectedCharacters &= 0xD0;
				if (interfaceIteration == 1) { /* TB1 */
					/* code reset only accept TB1 = 0x00 */
					if (!warm && cur_byte != 0) reject_atr = 1;
					TB1 = 1;
				}
				else if (interfaceIteration == 3) TB3 = cur_byte; /* TB3 checked below */
				else if (interfaceIteration == 2) reject_atr = 1; /* The terminal shall reject an ATR containing TB2 */
			}
			else if (expectedCharacters & 0x40) /* TC */ {
				 expectedCharacters &= 0xB0;
				 switch (interfaceIteration) {
				 	case 1: /* TC1 */
						if (!warm && !TB1) reject_atr = 1;
						else g_atr.GTR = cur_byte;
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
				 	g_atr.T = cur_byte & 0x0F;
				 }
				 else if (interfaceIteration == 2) {
				 	if((((cur_byte & 0x0F) | g_atr.T) != 0xE) && ((cur_byte & 0x0F) != 1)) reject_atr = 1;
				 }
				 interfaceIteration ++;
			}
			else if (expectedCharacters == 0x02) /* TCK */ {
				TCK = cur_byte;
				done = 1;
			}
			else if (expectedCharacters == 0x01) /* Historical Bytes */ {
				if (-- historicalBytes == 0) {
					if (g_atr.T == 0) done = 1; /* only T=1 require checksum */
					else expectedCharacters = 0x02; /* checksum, require TCK */
				}
			}
			/* if no historical bytes required, check if TCK needed */
			if (expectedCharacters == 0) {
				if (historicalBytes == 0) {
					if (g_atr.T == 0) done = 1; /* only T=1 require checksum */
					else expectedCharacters = 0x02; /* checksum, require TCK */
				}
				else{
					if(no_expect_flag == 1) {
						if (-- historicalBytes == 0) {
							if (g_atr.T == 0) done = 1; /* only T=1 require checksum */
							else expectedCharacters = 0x02; /* checksum, require TCK */
						}
					}
					expectedCharacters = 0x01;
				}
			}
		}
		else{ /* T0 Character */
			historicalBytes = cur_byte & 0x0F;            /* Find out historical bytes */
			expectedCharacters = cur_byte & 0xF0;         /* Figure out upcoming protocol conf bytes */
			if(expectedCharacters == 0) no_expect_flag = 1;
		}
	}
	if (g_atr.T == 1) {
		if ((check != 0) && (reject_atr == 0)) return ERR_POWERUP_ATR_CRC_FAILURE; /* FCS error */
		if ((result = g_atr.GTR) == 0xFF) result = -1;
		if((TB3 == -1) || ((TB3 & 0x0F) > 0x05) ||
			((TB3 & 0xF0) > 0x40) || ((1 << (TB3 & 0x0f)) <= (result + 1))) {
			reject_atr=1;
		}
	}
	if((standard == emv) && (reject_atr == 1)) {
		return 0;
	}
	if (atrmode == SPECIFIC_MODE_USE_TA1_F_D) {
		g_atr.D = TA1;
		ctrl_ic_clk(g_atr.D);
	}
	
	g_atr.WWT = g_atr.D * (TC2 * 960L + 480L + 230L);
	g_atr.WWT += 1;//add 2013-2-25;
	if (TB3 != -1) {
		g_atr.cwi = (TB3 & 0x0F);
		g_atr.CWT = 11 + (1 << (TB3 & 0x0F));
		g_atr.BWT = 11L + (1L << ((TB3 & 0xF0) >> 4)) * 960L * g_atr.D;
	}
	if(g_atr.T == 1) {
		g_atr.BWT += 960L * g_atr.D;
		g_atr.CWT += 4;
		g_atr.IFD_Seq = g_atr.ICC_Seq = g_atr.IFSD_Negotiated = 0;
	}
	if (atrstr) {
		if (buflen < index) index = buflen;
		memcpy(atrstr, works, index);
	}
	g_atr.ATROK = 1;
	wait_ic_etu_cnt(12);
#ifdef CONFIG_SPU90
	sam_stat[SC_SAM1].power_stat = 1;
	sam_stat[SC_SAM1].atr_stat = 1;
#endif
	return index;
}
/******************************************************************
函数名：void set_sc_t0_mode(void)
输入 :no
输出 : no
功能描述：t0模式apdu设置，设置wwt和重复次数
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static void set_sc_t0_mode(void)
{
	uint8_t value = 0;
	/*rbuf_rst tbuf_rst*/
	value = reg_read8(SMARTCARD_BASE + SC_P_PROTO_CMD);
	value |= SC_PROTO_CMD_rbuf_rst_MASK | SC_PROTO_CMD_tbuf_rst_MASK;
	reg_write8(SMARTCARD_BASE + SC_P_PROTO_CMD,value);
	/*set rpat_retry tpat_retry*/
	value = reg_read8(SMARTCARD_BASE + SC_P_UART_CMD_2);
	value &= 0x40;
	value |= (RPAR_RETRY_CNT << 3);
	value |= TPAR_RETRY_CNT;
	reg_write8(SMARTCARD_BASE + SC_P_UART_CMD_2,value);
	/*set gtr gtr = 11+x but T0 GTR=12+x so we do it*/
	if(g_atr.GTR > 0){
		if((g_atr.GTR < 254) || (g_atr.GTR == 255)){
			reg_write8(SMARTCARD_BASE + SC_P_TGUARD,g_atr.GTR + 1 );
		}
		else reg_write8(SMARTCARD_BASE + SC_P_TGUARD,g_atr.GTR );
	}
	else reg_write8(SMARTCARD_BASE + SC_P_TGUARD,0 );
	/*set wwt time*/
	reg_write8(SMARTCARD_BASE + SC_P_WAIT_1,(g_atr.WWT & 0xff));
	reg_write8(SMARTCARD_BASE + SC_P_WAIT_2,(g_atr.WWT & 0xff00) >> 8);
	reg_write8(SMARTCARD_BASE + SC_P_WAIT_3,(g_atr.WWT & 0xff0000) >> 16);
	
	value = reg_read8(SMARTCARD_BASE + SC_P_TIMER_CMD);
	value |= SC_TIMER_CMD_wait_en_MASK;
	value &= (~SC_TIMER_CMD_wait_mode_MASK);/*wwt mode*/
	reg_write8(SMARTCARD_BASE + SC_P_TIMER_CMD,value);
	value = reg_read8(SMARTCARD_BASE + SC_P_INTR_STAT_2);/*clear wwt time out*/
}
 /******************************************************************
函数名：void disable_t0_wwt(void)
输入 :no
输出 : no
功能描述：失效wwt
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
 void disable_t0_wwt(void)
{
	uint8_t value = 0;
	value = reg_read8(SMARTCARD_BASE + SC_P_TIMER_CMD);
	value &= ~SC_TIMER_CMD_wait_en_MASK;
	reg_write8(SMARTCARD_BASE + SC_P_TIMER_CMD,value);
}
 /******************************************************************
 函数名：static int write_t0_bytes(unsigned char *buf,unsigned int len)
 输入 :buf 发送数据的指针，len发送长度
 输出 : no
 功能描述：t0发送字符串
 作者：guoc
 日期：2013-02-01
 版　本：1.00
 ******************************************************************/

static int write_t0_bytes(uint8_t *buf,uint16_t len)
{
	uint8_t value = 0;
	uint16_t i=0;
	value = reg_read8(SMARTCARD_BASE + SC_P_PROTO_CMD);
	value |= SC_PROTO_CMD_rbuf_rst_MASK | SC_PROTO_CMD_tbuf_rst_MASK;
	reg_write8(SMARTCARD_BASE + SC_P_PROTO_CMD,value);
#if SC_T0_SDATA_DEBUG_EN
	sc_printk("\n");
	for(i=0;i<len;i++){
		sc_printk("%02x ",(int)buf[i]);
	}
	sc_printk("t\n");
#endif
	for(i=0;i<len;i++){
		reg_write8(SMARTCARD_BASE + SC_P_TRANSMIT,buf[i]);/*write data*/
	}
	reg_write8(SMARTCARD_BASE + SC_P_TLEN_1,(len & 0xff));/*len*/
	reg_write8(SMARTCARD_BASE + SC_P_TLEN_2,(len &0xff00)>>8);
	value = reg_read8(SMARTCARD_BASE + SC_P_UART_CMD_1);
	value |= SC_UART_CMD_1_t_r_MASK |SC_UART_CMD_1_xmit_go_MASK |\
			SC_UART_CMD_1_io_en_MASK | SC_UART_CMD_1_auto_rcv_MASK;
	reg_write8(SMARTCARD_BASE + SC_P_UART_CMD_1,value);/*start send data*/
	do{
		value = reg_read8(SMARTCARD_BASE + SC_P_INTR_STAT_1);/*3次发送校验出错*/
		if((value & SC_INTR_STAT_1_retry_intr_MASK) == SC_INTR_STAT_1_retry_intr_MASK){
			#if SC_T0_ERR_DEBUG_EN
			sc_printk("tpar\n");
			#endif
			return ERR_SEND_PARITY;
		}
	}while((value & SC_INTR_STAT_1_tdone_intr_MASK) != SC_INTR_STAT_1_tdone_intr_MASK);
	return 0;
}

/******************************************************************
函数名：static int read_t0_byte(void)
输入 :no
输出 : int 接收到的字符或者错误信息
功能描述：t0接收一个字符
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static int read_t0_byte(void)
{
	volatile uint8_t value = 0,value1 = 0,retval = 0;
	while(1){
		value = reg_read8(SMARTCARD_BASE + SC_P_INTR_STAT_2);/*wwt time out*/
		if((value & SC_INTR_STAT_2_wait_intr_MASK) == SC_INTR_STAT_2_wait_intr_MASK){
			#if SC_T0_ERR_DEBUG_EN
			sc_printk("0wwt");
			#endif
			return ERR_RECV_TIMEOUT;
		}
		if((value & SC_INTR_STAT_2_rcv_intr_MASK) != SC_INTR_STAT_2_rcv_intr_MASK)/*no data*/
			continue;
	
		retval = reg_read8(SMARTCARD_BASE + SC_P_RECEIVE);
		value1 = reg_read8(SMARTCARD_BASE + SC_P_INTR_STAT_1);
		/*3次接收校验出错*/
		if((value1 & SC_INTR_STAT_1_retry_intr_MASK) == SC_INTR_STAT_1_retry_intr_MASK){
			#if SC_T0_ERR_DEBUG_EN
			sc_printk("3rpar\n");
			#endif
			return ERR_RECV_PARITY;
		}
		value1 = reg_read8(SMARTCARD_BASE + SC_P_STATUS_2);
		if((value1 & SC_STATUS_2_rpar_err_MASK) ==  SC_STATUS_2_rpar_err_MASK){
			#if SC_T0_ERR_DEBUG_EN
			sc_printk("0rpar\n");
			#endif
			continue;
		}
		#if SC_T0_RDATA_DEBUG_EN
		sc_printk("R%02x ",(int)retval);
		#endif
		return (int)retval;
	}	
}
/******************************************************************
函数名：void wait_t0_BGT(void)
输入 :no
输出 : no
功能描述：t0等待BGT 软件延时，等待16etu
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static void wait_t0_BGT(void)
{
	wait_ic_etu_cnt(16);
}
/******************************************************************
函数名：int icc_send_apdu0(unsigned char *buffer, unsigned int length, unsigned char *rbuffer)
输入 :buffer 发送字符指针length发送长度rbuffer 接收字符长度
输出 : int apdu接收长度或者出错代码
功能描述：t0卡apdu处理，包括4种情况，
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static int icc_send_apdu0(uint8_t *buffer, uint32_t length, uint8_t *rbuffer)
{
	int index, retval;
	uint8_t* hdr = buffer;
	uint8_t INS, P3 = 0, wait_for_proc_byte;
	uint8_t SW1=0, SW2=0, warning_status_byte = 0,case4_cmd = 0, get_resp = 0, inverse_INS;
	
	uint8_t  GET_RESPONSE[5] = {0,0xC0,0,0,0};
	uint8_t tbuf_tmp[5]={0,0,0,0,0};
	uint8_t case_t0 = 0;
	uint32_t rindex = 0, Lc = 0, Le = 0, val;
	/* identify command type */
#if 0
	for(index = 0;index<length;index++)
	{
		printk("%02x ",buffer[index]);
	}
	printk("apdu\n");
	index =0;
#endif
	set_sc_t0_mode();//add guoc
	if(length == 4) case_t0 = 1;
	else if(length == 5) case_t0 = 2;
	else if(buffer[4] == length - 5) case_t0 = 3;
	else case_t0 = 4;
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
 	wait_t0_BGT();
	while (1) {
		INS = hdr[1];
  		inverse_INS = ~INS;
		wait_for_proc_byte = 1;
		/* write 5 byte command header */
		for (index = 0; index < 4;) {
		    tbuf_tmp[index] = hdr[index];
		    index ++;
		}
		tbuf_tmp[4] = P3;
		if ((retval = write_t0_bytes(tbuf_tmp,5)) < 0) {
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
					wait_t0_BGT();
				case 0x60: /* Card wants to have additional WWT */
					break;
				default:
					if((val == INS) || (val == inverse_INS)) { /* all remaining data or next data byte */
						if (Lc + 5 == index) { /* LC have been send out */
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
						wait_t0_BGT();
						/* if we received INS byte, send all remaining bytes, otherwise, send only one byte */
						if(val == INS) {
							retval = write_t0_bytes(&(buffer[index]),(Lc+5-index));
							index = Lc +5;
							if(retval < 0)
								return retval;
						}
						if(val == inverse_INS){						
							retval = write_t0_bytes(&(buffer[index]),1);
							index++;
							if(retval <0)
								return retval;
						}
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
							((Lc + 5) == index) && (case4_cmd) && (get_resp == 0)) {
							warning_status_byte = 1;
							Lc = 0;
							hdr = buffer = GET_RESPONSE;
							P3 = Le = 0;
							Le += rindex;
							//if(Le > RECEIVE_DATA_MAX) return ERR_RECEIVE_DATA_OVER;
							if(Le > (RECEIVE_DATA_MAX - 2)) {
								rbuffer[rindex ++] = val;
								rbuffer[rindex ++] = retval;
								return rindex;
							}
							wait_for_proc_byte = 0;
							wait_t0_BGT();
						}
						else {  /* Card completed APDU response, send response with Status code */
							rbuffer[rindex ++] = SW1;
							rbuffer[rindex ++] = SW2;
							return rindex;

						}
					}
					else {
					/*case 4 received 0x62 xx send not 90 00 send data 62 xx add by guoc */
							if((rindex == 0)||(warning_status_byte == 1)){
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
/******************************************************************
函数名：void set_sc_t1_mode(void)
输入 :no
输出 : no
功能描述：t1卡模式设置，设置cwt bwt、bgt，使能lrc
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static void set_sc_t1_mode(void)
{
	uint8_t value =0 ;
	value = reg_read8(SMARTCARD_BASE + SC_P_PROTO_CMD);
	value |= SC_PROTO_CMD_rbuf_rst_MASK | SC_PROTO_CMD_tbuf_rst_MASK;
	value &= (~SC_PROTO_CMD_crc_lrc_MASK);
	value |= (SC_PROTO_CMD_edc_en_MASK);
	value &= 0xf0;/*set 0-3 =0*/
	value |= g_atr.cwi;
	reg_write8(SMARTCARD_BASE + SC_P_PROTO_CMD,value);
	reg_write8(SMARTCARD_BASE + SC_P_TGUARD,g_atr.GTR);

	value = reg_read8(SMARTCARD_BASE + SC_P_UART_CMD_2);
	value &= (1 << 6);/*tpar rpar =0*/
	reg_write8(SMARTCARD_BASE + SC_P_UART_CMD_2,value);

	value = reg_read8(SMARTCARD_BASE + SC_P_TIMER_CMD);
	value |= SC_TIMER_CMD_wait_en_MASK | SC_TIMER_CMD_wait_mode_MASK |SC_TIMER_CMD_cwt_en_MASK;
	reg_write8(SMARTCARD_BASE + SC_P_TIMER_CMD,value);

	reg_write8(SMARTCARD_BASE + SC_P_WAIT_1,((g_atr.BWT + 1) & 0xff));
	reg_write8(SMARTCARD_BASE + SC_P_WAIT_2,((g_atr.BWT + 1) & 0xff00) >> 8);
	reg_write8(SMARTCARD_BASE + SC_P_WAIT_3,((g_atr.BWT + 1) & 0xff0000) >> 16);
	
	reg_write8(SMARTCARD_BASE + SC_P_EVENT1_CMP, 5);/*emv2000 cwt= cwt+4*/
	reg_write8(SMARTCARD_BASE + SC_P_EVENT1_CMD_3, SC_P_CWT_INTR_EVENT_SRC);
	reg_write8(SMARTCARD_BASE + SC_P_EVENT1_CMD_2, SC_P_RX_ETU_TICK_EVENT_SRC);
	reg_write8(SMARTCARD_BASE + SC_P_EVENT1_CMD_1, SC_P_RX_START_BIT_EVENT_SRC);
	value = SC_EVENT1_CMD_4_event_en_MASK | SC_EVENT1_CMD_4_intr_after_compare_MASK |\
			SC_EVENT1_CMD_4_run_after_reset_MASK;
	value &= ~(SC_EVENT1_CMD_4_intr_after_reset_MASK | SC_EVENT1_CMD_4_run_after_compare_MASK);
	reg_write8(SMARTCARD_BASE + SC_P_EVENT1_CMD_4,value);


	value = reg_read8(SMARTCARD_BASE + SC_P_INTR_STAT_1);/*clear bgt cwt bwt timer err*/
	value = reg_read8(SMARTCARD_BASE + SC_P_INTR_STAT_2);
}
/******************************************************************
函数名：void disable_t1_cwi_bwt_bgt(void)
输入 :no
输出 : no
功能描述：t1卡cwt、bwt、bgt失效
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static void disable_t1_cwi_bwt_bgt(void)
{
	uint8_t value =0;
	value = reg_read8(SMARTCARD_BASE + SC_P_TIMER_CMD);/*cwt bwt*/
	value &= ~(SC_TIMER_CMD_cwt_en_MASK | SC_TIMER_CMD_wait_en_MASK);
	reg_write8(SMARTCARD_BASE + SC_P_TIMER_CMD,value);
	
	value = reg_read8(SMARTCARD_BASE + SC_P_EVENT1_CMD_4);/*event*/
	value &= ~SC_EVENT1_CMD_4_event_en_MASK;
	reg_write8(SMARTCARD_BASE + SC_P_EVENT1_CMD_4,value);
	
	value = reg_read8(SMARTCARD_BASE + SC_P_BGT);/*bgt*/
	value &= ~(SC_BGT_r2t_MASK | SC_BGT_t2r_MASK); 
	reg_write8(SMARTCARD_BASE + SC_P_BGT,value);
}
/******************************************************************
函数名：void set_sc_bwt(unsigned int bwt)
输入 :int bwt
输出 : no
功能描述：t1卡bwt设置
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static void set_sc_bwt(uint32_t bwt)
{
	//add by qiut in 2017.06.13 for avoiding the test case failed under limiting conditions
	bwt += 1;
	/*set wwt time*/
	reg_write8(SMARTCARD_BASE + SC_P_WAIT_1,(bwt & 0xff));
	reg_write8(SMARTCARD_BASE + SC_P_WAIT_2,(bwt & 0xff00) >> 8);
	reg_write8(SMARTCARD_BASE + SC_P_WAIT_3,(bwt & 0xff0000) >> 16);
}
static void wait_t1_bgt(void)
{
	wait_ic_etu_cnt(22);
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
/******************************************************************
函数名：int read_t1_byte(void)
输入 :no
输出 : int 读取的字节或者错误码
功能描述：t1卡bwt设置
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static int read_t1_byte(void)
{
	volatile uint8_t value = 0,retval = 0;
	retval = reg_read8(SMARTCARD_BASE + SC_P_RECEIVE);
	value = reg_read8(SMARTCARD_BASE + SC_P_STATUS_2);
	if((value & SC_STATUS_2_rpar_err_MASK) == SC_STATUS_2_rpar_err_MASK){
		#if SC_T1_ERR_DEBUG_EN
		sc_printk("1par %02x\n",(int)retval);
		#endif
		return ERR_RECV_PARITY;
	}
	if((value & SC_STATUS_2_edc_err_MASK) == SC_STATUS_2_edc_err_MASK){
		#if SC_T1_ERR_DEBUG_EN
		sc_printk("1lrc\n");
		#endif
		return ERR_RECV_LRC;
	}
	#if SC_T1_RDATA_DEBUG_EN
	sc_printk("R%02x ",retval);
	#endif
	return (int)retval;
}
/******************************************************************
函数名：void send_block(unsigned char NAD,unsigned char PCB,unsigned char length,unsigned char *buffer)
输入 :NAD PCB length 发送长度 buffer发送字节，参照emv2000，
输出 : no
功能描述：t1卡发送一个块数据
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static void send_block(uint8_t NAD,uint8_t PCB,uint8_t length,uint8_t *buffer)
{
	uint8_t i = 0;
	uint8_t value = 0;
	if(g_atr.wtx > 0){
		set_sc_bwt(g_atr.BWT * g_atr.wtx);/*set wtx time*/
	}
	else{
		set_sc_bwt(g_atr.BWT);
	}
	#if SC_T1_SDATA_DEBUG_EN
	for(i = 0; i < length; i++){
		sc_printk("%02x ",buffer[i]);
	}
	sc_printk("T \n");
	#endif 
	/*enable bwt cwt event1 */
	value = reg_read8(SMARTCARD_BASE + SC_P_TIMER_CMD);
	value |= SC_TIMER_CMD_wait_en_MASK | SC_TIMER_CMD_wait_mode_MASK |SC_TIMER_CMD_cwt_en_MASK;
	reg_write8(SMARTCARD_BASE + SC_P_TIMER_CMD,value);
	value = reg_read8(SMARTCARD_BASE + SC_P_EVENT1_CMD_4);
	value |= SC_EVENT1_CMD_4_event_en_MASK; 
	reg_write8(SMARTCARD_BASE + SC_P_EVENT1_CMD_4,value);

	value = reg_read8(SMARTCARD_BASE + SC_P_PROTO_CMD);
	value |=SC_PROTO_CMD_tbuf_rst_MASK | SC_PROTO_CMD_rbuf_rst_MASK;
	reg_write8(SMARTCARD_BASE + SC_P_PROTO_CMD,value);/*rst tbuf rbuf*/
	reg_write8(SMARTCARD_BASE + SC_P_TRANSMIT, NAD);
	reg_write8(SMARTCARD_BASE + SC_P_TRANSMIT, PCB);
	reg_write8(SMARTCARD_BASE + SC_P_TRANSMIT, length);
	for(i = 0; i < length; i++)
	{
		reg_write8(SMARTCARD_BASE + SC_P_TRANSMIT,buffer[i]);
	}
	/*send edc by hardware*/
	value = reg_read8(SMARTCARD_BASE + SC_P_PROTO_CMD);
	value |= SC_PROTO_CMD_edc_en_MASK ;
	value &= ~SC_PROTO_CMD_crc_lrc_MASK;
	reg_write8(SMARTCARD_BASE + SC_P_PROTO_CMD,value);
	
	reg_write8(SMARTCARD_BASE + SC_P_TLEN_1,(length + 4)&0xff);
	reg_write8(SMARTCARD_BASE + SC_P_TLEN_2,((length + 4)&0xff00)>>8);
	wait_t1_bgt();
	value = reg_read8(SMARTCARD_BASE + SC_P_UART_CMD_1);
	value |= SC_UART_CMD_1_io_en_MASK | SC_UART_CMD_1_auto_rcv_MASK |\
			SC_UART_CMD_1_t_r_MASK | SC_UART_CMD_1_xmit_go_MASK;
	reg_write8(SMARTCARD_BASE + SC_P_UART_CMD_1,value);/*send data*/
	do
	{
		value = reg_read8(SMARTCARD_BASE +SC_P_INTR_STAT_1);
	}while((value & SC_INTR_STAT_1_tdone_intr_MASK) != SC_INTR_STAT_1_tdone_intr_MASK);/*wait for send over*/
}
/******************************************************************
函数名：signed short receive_block(unsigned char *rNAD, unsigned char *rPCB, unsigned char *rLEN, unsigned char * buffer)
输入 :*rNAD *rPCB *rLen 接收长度*rbuffer接收数据buf，参照emv2000，
输出 : no
功能描述：t1卡接收一个块数据
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static int receive_block(uint8_t *rNAD, uint8_t *rPCB, uint8_t *rLEN, uint8_t * buffer)
{
	uint8_t value = 0,value1 = 0;
	uint8_t len1 = 0,len2 = 0;
	uint32_t len = 0,i =0;
	uint8_t rece_buf[264]={0};
	int errcode = 0;
	int retval = 0;
	do{
		value = reg_read8(SMARTCARD_BASE +SC_P_INTR_STAT_1);
		/*cwt+4 so set event1=cwt+4*/
		if((value & SC_INTR_STAT_1_event1_intr_MASK) == SC_INTR_STAT_1_event1_intr_MASK){
			retval = ERR_CWT_ERROR ;
			#if SC_T1_ERR_DEBUG_EN
			sc_printk("cwt\n");
			#endif
			goto exit_from_receiveblock;
		}
		value = reg_read8(SMARTCARD_BASE + SC_P_INTR_STAT_2);
		if((value & SC_INTR_STAT_2_rready_intr_MASK) != SC_INTR_STAT_2_rready_intr_MASK){
			if((value & SC_INTR_STAT_2_rlen_intr_MASK) == SC_INTR_STAT_2_rlen_intr_MASK){
				retval = ERR_RLEN_ERROR;
				#if SC_T1_ERR_DEBUG_EN
				sc_printk("rlen\n");
				#endif
				goto exit_from_receiveblock;
			}
			if((value & SC_INTR_STAT_2_wait_intr_MASK) == SC_INTR_STAT_2_wait_intr_MASK){
				retval = ERR_BWT_ERROR;
				#if SC_T1_ERR_DEBUG_EN
				sc_printk("bwt\n");
				#endif
				goto exit_from_receiveblock;
			}
		}
		value1 = reg_read8(SMARTCARD_BASE + SC_P_STATUS_2);
		if((value1 & SC_STATUS_2_roverflow_MASK) == SC_STATUS_2_roverflow_MASK){
			retval = ERR_ROVERFLOW_ERROR;
			#if SC_T1_ERR_DEBUG_EN
			sc_printk("rflow\n");
			#endif
			goto exit_from_receiveblock;
		}
		
	}while((value & SC_INTR_STAT_2_rready_intr_MASK) != SC_INTR_STAT_2_rready_intr_MASK);
	/*receive block over disable bwt cwt*/
	value = reg_read8(SMARTCARD_BASE + SC_P_TIMER_CMD);
	value &= ~(SC_TIMER_CMD_wait_en_MASK | SC_TIMER_CMD_wait_mode_MASK |SC_TIMER_CMD_cwt_en_MASK);
	reg_write8(SMARTCARD_BASE + SC_P_TIMER_CMD,value);
	len1 = reg_read8(SMARTCARD_BASE + SC_P_RLEN_1);
	len2 = reg_read8(SMARTCARD_BASE + SC_P_RLEN_2);
	len = (((unsigned short int)len2 << 8) |len1)& 0x1ff;/*max 264*/
	#if SC_T1_RDATA_DEBUG_EN
	sc_printk("len is %d \n",len);
	#endif
	/*最后一个校验是否再长度之内，需要测试*/
	for(i = 0;i < len;i++){
		if((retval = read_t1_byte()) < 0){
			errcode = retval;
		}
		else{
			rece_buf[i]=(unsigned char)retval;
		}
	}
	if(errcode < 0){/*no err*/
		retval = errcode;
		goto exit_from_receiveblock;
	}
	else{
		*rNAD = rece_buf[0];
		*rPCB = rece_buf[1];
		*rLEN = rece_buf[2];
		memcpy(buffer, &(rece_buf[3]), *rLEN);
	}
	if (*rNAD != 0) { /* Check the received T1 block format */
		retval = ERR_INVALID_NAD;
		goto exit_from_receiveblock;
	}
	if((ISRBLOCK(*rPCB)&&(*rLEN!=0))||(ISSBLOCK(*rPCB)&&(*rLEN>1))) {
		retval= ERR_INVALID_LEN;
	}
	else {
		retval=*rLEN;
	}
				
exit_from_receiveblock:
	value = reg_read8(SMARTCARD_BASE + SC_P_PROTO_CMD);
	value |=SC_PROTO_CMD_rbuf_rst_MASK ;
	reg_write8(SMARTCARD_BASE + SC_P_PROTO_CMD,value);
	/*disable event1 cnt*/
	value = reg_read8(SMARTCARD_BASE + SC_P_EVENT1_CMD_4);
	value &= ~SC_EVENT1_CMD_4_event_en_MASK; 
	reg_write8(SMARTCARD_BASE + SC_P_EVENT1_CMD_4,value);
	/* if length is FF, abort APDU transaction */
	if(*rLEN==0xFF) {
		/*if len=0xff send r block*/
		//return ERR_ABORT_T1_TRANSACTION;
		return ERR_INVALID_LEN;/*power down modify to send R-block modify by guoc*/
	}

	return retval;
}
/******************************************************************
函数名：void send_block(unsigned char NAD,unsigned char PCB,unsigned char length,unsigned char *buffer)
输入 :IFSD 
输出 : short 返回成功结果0或者失败错误码
功能描述：t1卡发送一个IFSD数据块内容，接收回复数据
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static int send_sblock_IFSD(uint8_t IFSD)
{
	uint8_t buffer = IFSD;
	uint8_t rNAD, rPCB, rLEN;
	uint8_t rbuffer[256];
	uint8_t count = T1_BLOCK_RETRY;
	do{
		/*  S(IFS request) block with PCB = 'C1' and with IFSD = 254 */
		send_block(0, 0xC1, 1, &buffer);
		if((receive_block(&rNAD, &rPCB, &rLEN, rbuffer)) < 0) {
		  continue;
		}
		/* The PCB of the S(IFS response) block sent
		   in response shall have the value 'E1', and the INF field shall have the same
		   value as the INF field of the block requesting the change. */
		if ((rPCB == 0xE1) && (rLEN == 1) && (rbuffer[0] == IFSD))  {
		  return 0;
		}
	}while(--count);
	return ERR_SETIFSD_FAILURE;
}

#define set_NAD(nad)	g_atr.NAD = (nad)
/******************************************************************
函数名：int receive_sblock(unsigned char *rNAD, unsigned char *rPCB, unsigned char *rLEN, unsigned char * rbuffer)
输入 :*rNAD *rPCB *rLen 接收长度*rbuffer接收数据buf，参照emv2000，
输出 : no
功能描述：t1卡接收一个S块数据
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static int receive_sblock(uint8_t *rNAD, uint8_t *rPCB, uint8_t *rLEN, uint8_t *rbuffer)
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
		case SREQUEST_ABORT : {
			/* Card requests to ABORT, send ABORT error code to abort transaction */
			return ERR_ABORT_T1_TRANSACTION;
		}
		case SREQUEST_IFS :
			/* Card sends IFSC request, Check if it is valid IFSC request */
			if((*rbuffer<0x10)||(*rbuffer==0xff)) {
				return ERR_INVALID_INF;
			}
			/* /Send IFSC response */
			send_block(0, SRESPONSE_IFS, 1, rbuffer);
			g_atr.IFSC = *rbuffer;   /* Store new IFSC value in buffer */
			Responsed = 1; 	/* Set t1 status flags to maintain protocol state */
			t1_sblock_sent = 1;
			break;

		case SREQUEST_WTX :
			/* Card sends WTX Request */
			g_atr.wtx = *rbuffer;       /* Store new WTX value in wtx buffer */
			send_block(0, SRESPONSE_WTX, 1, rbuffer); /* Send wtx response to card */
			/* After the ICC responds,
			BWT is again used as the time allowed for the ICC to process the I-block */
			g_atr.wtx = 0;
			Responsed = 1;
			t1_sblock_sent = 1;
			break;
		}
	} while (Responsed);
	return ret;
}
/******************************************************************
函数名：int icc_send_apdu1(unsigned char *req_buf, unsigned short req_len, unsigned char *resp_buf)
输入 : *req_buf发送数据, req_len发送数据长度, *resp_buf接收数据
输出 : int 长度或者返回错误码，
功能描述：t1卡APDU流程
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
static int icc_send_apdu1(uint8_t *req_buf, uint32_t req_len, uint8_t *resp_buf)
{
	uint8_t rNAD, rblock_sent=0;
	uint8_t rPCB;
	uint8_t rLEN;
	uint8_t *IFDSeq, *ICCSeq, works[256];
	int cardiblock_err=0;
	int ret;
	int currentIBlockLen, more, retry, i_block_retry;
	int totalResLength;
	IFDSeq = &(g_atr.IFD_Seq);
	ICCSeq = &(g_atr.ICC_Seq);
	set_sc_t1_mode();//add guoc
	/* Make sure IFSD Negotiation takes place once before any APDU exchange for T1 protocol */
	if (g_atr.IFSD_Negotiated == 0) {
		if (send_sblock_IFSD(0xFE) < 0) return -1;
		g_atr.IFSD_Negotiated = 1;
	}
	/* Send APDU request */
	while (req_len) {
		/* We do chaining if request length is greater than IFSC */
		currentIBlockLen = (req_len < g_atr.IFSC) ? req_len : g_atr.IFSC;
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
				if((ISRBLOCK(rPCB))) { /* Check whether card sent R-Block */
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
					else  {
						/* Card sent ACK for previous i-block, send next chained i-block */
						if (more) {
							*IFDSeq = 1 - *IFDSeq;
							break;
						}
						else if(((!rblock_sent) && (retry < 2))) {
							/* there could be error in icc response to last i-block */
							send_block(0,
								CODERBLOCK(*ICCSeq, ((cardiblock_err == ERR_RECV_LRC) ||(cardiblock_err==ERR_RECV_PARITY)) ? 1 : 2), 0,(void*)0);
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
			if(ret == ERR_ABORT_T1_TRANSACTION) return ret;
			if(retry) {
				if((cardiblock_err))
					ret = cardiblock_err;
				/* send R block saying we got invalid block */
				send_block(0,CODERBLOCK(*ICCSeq, ((ret==ERR_RECV_LRC)||(ret==ERR_RECV_PARITY)) ? 1 : 2), 0,(void*)0);
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
					if (ISRBLOCK(rPCB)) { /* Card did not receive our ACK properly, resend it */
						if (!(NR(rPCB) != *IFDSeq)) {
							send_block(0,CODERBLOCK(*ICCSeq, 0), 0,(void*)0);
							continue;
						}
					}
					else if (ISIBLOCK(rPCB)) {
						if (NS(rPCB) == *ICCSeq) break;
					}
				}
				if(ret == ERR_ABORT_T1_TRANSACTION) return ret;

				if(--retry)  send_block(0,CODERBLOCK(*ICCSeq, 0), 0,(void*)0);
			}
			if (!retry) return ERR_MAX_RETRIED;
		}
		else
		{
			break;
		}
	} while (1);
	return totalResLength;
}
/******************************************************************
函数名： icc_send_apdu(unsigned char *buffer, unsigned int length, unsigned char *rbuffer)
输入 : *buffer发送数据, length发送数据长度, *rbuffer接收数据
输出 : int 长度或者返回错误码，
功能描述：IC卡APDU流程
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
int icc_send_apdu(uint8_t *buffer, uint32_t length, uint8_t *rbuffer)
{
	int ret = -1;
	//sc_printk("icc_send_apdu for T=%d\n", g_atr.T);
	switch (g_atr.T) {
	case 0:
		ret = icc_send_apdu0(buffer,length,rbuffer);
		disable_t0_wwt();
		break;
	case 1:
		ret = icc_send_apdu1(buffer,length,rbuffer);
		disable_t1_cwi_bwt_bgt();
		break;
	default:
		break;
	}
	if(ret < 0)
	{
		ic_power_down();/*error power down*/
	}
	return ret;
}
/******************************************************************
函数名： int icc_powerup(unsigned char *atrstr,unsigned char len)
输入 : atrstr atr数据指针，atr长度
输出 : int 长度或者返回错误码，
功能描述：IC卡上电复位操作
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
#ifndef EMV_TEST
#ifdef SC_4_SPEED
int icc_powerup(uint8_t *atrstr,uint8_t len,icc_standard_t standard)
{
	int ret = 0;
	int i = 1;
	int j = 0;
	tda8035_vcc_type vcc[2] = {VCC3V,VCC5V};
#if SC_POWON_DEBUG_EN
	sc_printk("sc pow up %d\n",standard);
#endif
	if(check_iccard_pres()){
		for(j = 0; j < 2; j++){
			i = 1;
			do{
				//printk("pow up vcc is %d \n",j);
				ic_cold_reset(vcc[j],i);
				if((ret = icc_atr_sequence(0,i, atrstr, len,standard)) < 0){
					if((ret == ERR_ATR_TS) || (ret == ERR_ATR_PARITY)){ /*for speed 2 or 4*/
						i = i << 1;/*i = 2 ,4*/
						ic_power_down();
						#if SC_POWON_DEBUG_EN
						sc_printk("sc speed=%d \n",i);
						#endif
						mdelay(1);/*wait for pow down */
						continue;
					}
					else{
						ic_power_down();
						disable_t0_wwt();
						#if SC_POWON_DEBUG_EN
						sc_printk("sc cold  reset err %d\n",ret);
						#endif
						break;
					}
				}
				
				if(ret == 0){
					/*warm rst */
					#if SC_POWON_DEBUG_EN
					sc_printk("sc go warm reset \n");
					#endif
					ic_warm_reset();
					if((ret = icc_atr_sequence(1,i, atrstr, len,standard))<= 0){/*=0 powerdown*/
						ic_power_down();
						disable_t0_wwt();
						#if SC_POWON_DEBUG_EN
						sc_printk("sc warm reset err %d\n",ret);
						#endif
						return -2;
					}
					else{
						break;
					}
				}
				else{
					break;
				}
			}while(i < 5);
			if(ret > 0) break;
		}
		disable_t0_wwt();
	}
	else{
		return -3;
	}
	if(g_iccard_insert_cnt < 65535) g_iccard_insert_cnt++;
	else g_iccard_insert_cnt = 0;
	return ret;
}
#else
int icc_powerup(uint8_t *atrstr,uint8_t len,icc_standard_t standard)
{
	int ret = 0;
	int j = 0;
	tda8035_vcc_type vcc[2] = {VCC3V,VCC5V};
	#if SC_POWON_DEBUG_EN
	sc_printk("sc pow up \n");
	#endif
	if(check_iccard_pres()){
		for(j = 0; j < 2; j++) {
			//printk("pow up vcc is %d \n",j);
			ic_cold_reset(vcc[j],1);
			if((ret = icc_atr_sequence(0,1,atrstr, len,standard)) < 0){
				ic_power_down();
				disable_t0_wwt();
				#if SC_POWON_DEBUG_EN
				sc_printk("sc cold  reset err %d\n",ret);
				#endif
				//return ret;
			}
			if(ret == 0){
				/*goto warm rst*/
				#if SC_POWON_DEBUG_EN
				sc_printk("sc go warm reset \n");
				#endif
				ic_warm_reset();
				if((ret = icc_atr_sequence(1,1, atrstr, len,standard)) <= 0){/*=0 powerdown*/
					ic_power_down();
					disable_t0_wwt();
					#if SC_POWON_DEBUG_EN
					sc_printk("sc warm reset err %d\n",ret);
					#endif
					return -2;
				}
			}
			disable_t0_wwt();
			if(ret > 0) break;
		}
	}
	else{
		return -3;
	}
	if(g_iccard_insert_cnt < 65535) g_iccard_insert_cnt++;
	else g_iccard_insert_cnt = 0;
	return ret;
}

#endif
#else
int icc_powerup(uint8_t *atrstr,uint8_t len,icc_standard_t standard)
{
	
	int ret = 0;
	#if SC_POWON_DEBUG_EN
	sc_printk("sc pow up \n");
	#endif
	if(check_iccard_pres()){
		ic_cold_reset(VCC5V,1);
//		ic_cold_reset(VCC3V,1);	//旧版本tda8035只能过电气3V测试
		if((ret = icc_atr_sequence(0,1,atrstr, len,emv)) < 0){
			ic_power_down();
			disable_t0_wwt();
			#if SC_POWON_DEBUG_EN
			sc_printk("sc cold  reset err %d\n",ret);
			#endif
				//return ret;
		}
		if(ret == 0){
			/*goto warm rst*/
			#if SC_POWON_DEBUG_EN
			sc_printk("sc go warm reset \n");
			#endif
			ic_warm_reset();
			if((ret = icc_atr_sequence(1,1, atrstr, len,emv)) <= 0){/*=0 powerdown*/
				ic_power_down();
				disable_t0_wwt();
				#if SC_POWON_DEBUG_EN
				sc_printk("sc warm reset err %d\n",ret);
				#endif
				return -2;
			}
		}
		disable_t0_wwt();		
	}
	else{
		return -3;
	}
	return ret;
}
#endif
/******************************************************************
函数名：int get_sc_state(void)
输入 : no
输出 : ic的状态可读，在卡槽，不再卡槽
功能描述：IC卡的状态
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
int get_sc_state(void)
{
	int ret = 0;
	if(check_iccard_pres()){
		if(g_atr.ATROK){
			ret = SC_State_Ready;
		}
		else{
			ret = SC_State_CardInsert;
		}
	}
	else{
		ret = SC_State_CardPull;
	}
	return ret ;
}
/******************************************************************
函数名：int get_sc_state_c(void)
输入 : no
输出 : ic卡的上电状态，0x03 上电 0x01没上电
功能描述：IC卡的状态
作者：guoc
日期：2013-02-01
版　本：1.00
******************************************************************/
int get_sc_state_c(void)
{
	int ret = 0;
	if(check_iccard_pres()){
		if(check_iccard_vccon()){
			ret = 0x03;
		}
		else{
			ret = 0x01;
		}
	}
	if(sam_stat[SC_SAM1].power_stat)
		ret |= (1 << 4);
	if(sam_stat[SC_SAM2].power_stat)
		ret |= (1 << 5);
	if(sam_stat[SC_SAM3].power_stat)
		ret |= (1 << 6);
	return ret ;	
}
irqreturn_t icc_isr(int irq, void *dev_id)
{
	uint8_t value = 0;
	value = reg_read8(SMARTCARD_BASE + SC_P_INTR_STAT_1);
	if(value & SC_INTR_STAT_1_pres_intr_MASK) {
		if(!check_iccard_pres()) {
			ic_power_down();
			//printk("icc src pow down \n");
		}
	}
	return IRQ_HANDLED;
}

int sc_atr_sequence(int warm, unsigned char * atrstr, int buflen)
{
	int ret = 0;
	if(buflen > 255) buflen = 255;
	ret = icc_powerup(atrstr, buflen,iso7816);
	return ret;
}
void sc_deactivate(void)
{
	ic_power_down();
}
int sc_present(void)
{
	return check_iccard_pres();
}
int sc_apdu(uint8_t * inbuf, uint32_t len, uint8_t *outbuf)
{
	if(!check_iccard_pres()) return -1;
	return icc_send_apdu(inbuf, len, outbuf);
}
EXPORT_SYMBOL(sc_atr_sequence);
EXPORT_SYMBOL(sc_deactivate);
EXPORT_SYMBOL(sc_present);
EXPORT_SYMBOL(sc_apdu);

