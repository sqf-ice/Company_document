#include"memory.h"
#include"at24xx.h"
#include "ic_memory.h"
int at24cxx_poweron(unsigned char *atrbuf)
{
	set_sc0_gpio();
	delay_ms(1);
	//tda8035_poweron_clk();
	//printk("at24cxx\n");
	#if 1
	if(!check_mem_insert())
	{
		mem_printk("no card\n");
		//printk("no card\n");
		return -1;
	}
	#endif
	tda8035_set_auxio1(0);
	tda8035_set_auxio2(0);
	tda8035_set_io(1);
	delay_ms(5);
	tda8035_set_vcc_onoff(0);
	delay_us(5);
	tda8035_poweron_clk();
	delay_ms(1);
	tda8035_set_io_type(input);
	atrbuf[0] = 0x02;
	atrbuf[1] = 0x90;
	atrbuf[2] = 0x00;
	return 0;
}
void iic_start(void)
{
	tda8035_set_io(1);
	delay_us(3);
	tda8035_set_clk(1);
	delay_us(10);
	tda8035_set_io(0);
	delay_us(10);
	tda8035_set_clk(0);
	delay_us(3);
}
void iic_stop(void)
{
	tda8035_set_io(0);
	delay_us(3);
	tda8035_set_clk(1);
	delay_us(10);
	tda8035_set_io(1);
	delay_us(10);
	tda8035_set_clk(0);
	delay_us(3);
}
int iic_writebyte(unsigned char data)
{
	unsigned char i = 0;
	int ret = 0;
	for(i = 0;i < 8;i++)
	{
		if(data & 0x80)
			tda8035_set_io(1);
		else
			tda8035_set_io(0);
		data <<= 1;
		delay_us(10);
		tda8035_set_clk(1);
		delay_us(10);
		tda8035_set_clk(0);
		delay_us(1);
	}
	tda8035_set_io_type(input);
	tda8035_set_clk(1);
	delay_us(10);
	if(tda8035_get_io())ret = -1;
	else ret = 0;
	tda8035_set_clk(0);
	delay_us(10);
	return ret;
}
unsigned char iic_readbyte(void)
{
	unsigned char i = 0, data = 0;
	tda8035_set_io_type(input);
	//tda8035_set_clk(0);
	//delay_us(10);
	for(i = 0;i < 8;i++)
	{
		tda8035_set_clk(1);
		delay_us(10);
		data <<= 1;
		if(tda8035_get_io())data |= 0x01;
		tda8035_set_clk(0);
		delay_us(10);
	}
	return data;
}
unsigned char set_chipaddr(unsigned char card_type,unsigned char apdu_p1)
{
	if((card_type == ic0at24c32) || (card_type == ic0at24c64))
		return 0xa0;
	else
		return ((apdu_p1 << 1) | 0xa0); 
}
int wr24cxx_addr(unsigned char card_type,unsigned char chipaddrin,unsigned char apdu_p1,unsigned char apdu_p2)
{
    unsigned char i;
    
    for (i=0; i<80; i++)
    {
        iic_start();
        if (iic_writebyte(chipaddrin) == 0){
            break;  //写设备地址
        }
    }
    if (i==80){
        return -1; //失败返回
    }

    if ( (card_type == ic0at24c32) || (card_type == ic0at24c64) )  //at24c32 at24c64
    {
        if ( iic_writebyte(apdu_p1) == -1)
        {
            for (; i<80; i++)
            {
                iic_start();
                if ( iic_writebyte(apdu_p1) == 0){
                    break;
                }
            }
        }
    }
    if (i==80){
        return -1; //失败返回
    }

    if ( iic_writebyte(apdu_p2) == -1 )
    {
        for (; i<80; i++)
        {
            iic_start();
            if ( iic_writebyte(apdu_p2) == 0){
                break;
            }
        }
    }
    if (i==80){
        return -1; //失败返回
    }

    return 0; //成功返回
}

int check_apdu(unsigned char card_type,const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned char apdu_p1,apdu_p2,apdu_lc;
        
    apdu_p1=sendbuf[2];
    apdu_p2=sendbuf[3];
    apdu_lc=sendbuf[4];

    if ( (apdu_lc==0)  || (apdu_lc>rw_memorycard_maxlength) )  //2005-11-15  由252改为249
    {
        goto APDU_LcLeErr;                //因为加上头尾会超过256
    }               
    switch(card_type)
    {
    case ic0at24c01:
        if (apdu_p1!=0) goto APDU_LcLeErr;
        if (apdu_p2>127) goto APDU_LcLeErr;
        if ( (apdu_lc==0) || (apdu_lc>128) )
            goto APDU_LcLeErr;
        if ((apdu_p2+apdu_lc)>128)  //2006-2-14增加
            goto APDU_LcLeErr;
        break;
    case ic0at24c02:
        if ( (apdu_p1!=0) ||
             ((apdu_p2+apdu_lc)>256) ) //2006-2-14增加
            goto APDU_LcLeErr;
        break;
    case ic0at24c04:
        if ( (apdu_p1>1) ||
             ((apdu_p1==1)&&((apdu_p2+apdu_lc)>256)) ) //2006-2-14增加
            goto APDU_LcLeErr;
        break;
    case ic0at24c08:
        if ( (apdu_p1>3) ||
             ((apdu_p1==3)&&((apdu_p2+apdu_lc)>256)) ) //2006-2-14增加
            goto APDU_LcLeErr;
        break;    
    case ic0at24c16:
        if ( (apdu_p1>7) ||
             ((apdu_p1==7)&&((apdu_p2+apdu_lc)>256)) ) //2006-2-14增加
            goto APDU_LcLeErr;
        break;
    case ic0at24c32:
        if ( (apdu_p1>15) ||
             ((apdu_p1==15)&&((apdu_p2+apdu_lc)>256)) ) //2006-2-14增加
            goto APDU_LcLeErr;
        break;
    case ic0at24c64:
        if ( (apdu_p1>31) ||
             ((apdu_p1==31)&&((apdu_p2+apdu_lc)>256)) ) //2006-2-14增加
            goto APDU_LcLeErr;
        break;
    default:
   		recebuf[0] = 0x6d;
		recebuf[1] = 0x00;
		*rcvlen = 2;
        return 0;
    }
    
    return 0; //成功返回

APDU_LcLeErr:
    recebuf[0] = 0x67;
	recebuf[1] = 0x00;
	*rcvlen = 2;
    return -1;
}
int at24cxx_write(unsigned char type_card,const unsigned char *sendbuf, int *rcvlen, unsigned char *recebuf)
{
    unsigned char i,page_mark=0;
    unsigned char apdu_p1,apdu_p2,apdu_lc;
        
    apdu_p1=sendbuf[2];
    apdu_p2=sendbuf[3];
    apdu_lc=sendbuf[4];

    if(type_card==ic0at24c01)
        page_mark=0x03;
	else 
		if(type_card==ic0at24c02)
			page_mark=0x07;
		else
			if ( (type_card==ic0at24c04) || (type_card==ic0at24c08) || (type_card==ic0at24c16) )
				page_mark=0x0f;
			else
				if ( (type_card==ic0at24c32) || (type_card==ic0at24c64) )
					page_mark=0x1f;

    for (i=0; i<apdu_lc; )
    {
        if ( wr24cxx_addr(type_card,set_chipaddr(type_card,apdu_p1),apdu_p1,apdu_p2) == -1)
            goto at24WrIc1OpErr;
        do 
        {    
            if ( iic_writebyte(sendbuf[i+5]) == -1)
                goto at24WrIc1OpErr;  //2005-11-8 +!
            apdu_p2++;
            if (apdu_p2==0){
                apdu_p1++;
            }
            i++;
        }while( (i<apdu_lc) && ((apdu_p2 & page_mark) != 0) );
        
        iic_stop();
        delay_ms(10);
    }
	delay_ms(10);
    recebuf[0] = 0x90;
	recebuf[1] = 0x00;
	*rcvlen = 2;
	return 0;

at24WrIc1OpErr:
    iic_stop();
	delay_ms(10);
	recebuf[0] = 0x65;
	recebuf[1] = 0x81;
	*rcvlen = 2;
	return -1;
}
int at24cxx_read(unsigned char type_card,const unsigned char *sendbuf, int *rcvlen, unsigned char *recebuf) 
{
    int i;
    unsigned char apdu_p1,apdu_p2,apdu_lc;
        
    apdu_p1=sendbuf[2];
    apdu_p2=sendbuf[3];
    apdu_lc=sendbuf[4];

    if (wr24cxx_addr(type_card,set_chipaddr(type_card,apdu_p1),apdu_p1,apdu_p2) == -1)
        goto rdAt24Ic1_err;
    
    iic_start();
    if ( iic_writebyte( set_chipaddr(type_card,apdu_p1) | 0x01) == -1)
        goto rdAt24Ic1_err;  //bit0＝1，读模式
    for (i=0;i<apdu_lc;i++)
    {
    	recebuf[i] = iic_readbyte();
        if (i==apdu_lc-1)
		{      //2005-11-11最后一个clk没有ACK
            tda8035_set_io(1);
        }
		else
		{
            tda8035_set_io(0);
        }
        delay_us(10);
		tda8035_set_clk(1);
		delay_us(10); 
		tda8035_set_clk(0);
		delay_us(1);
		tda8035_set_io_type(input);
    }
    iic_stop(); 
    recebuf[i]   = 0x90;
    recebuf[i+1] = 0x00;
    *rcvlen = apdu_lc+2;
    return 0;
    
rdAt24Ic1_err:
    iic_stop();
    recebuf[0]   = 0x65;
    recebuf[1] = 0x81;
    *rcvlen = 2;
	return -1;
}
int at24cxx_pro_main(unsigned char type_card,const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	int ret = 0;
	#if 1
		if(!check_mem_insert())
		{
			mem_printk("no card\n");
			return -1;
		}
	#endif
	tda8035_set_io(0);
	if ( check_apdu(type_card, sendbuf, rcvlen, recebuf) == -1)	//读
	{
		return -1;
	}
	switch(sendbuf[1])
	{
		case 0xB0:
			ret = at24cxx_read(type_card, sendbuf,rcvlen,recebuf);
		break;
		case 0xD0:
			ret = at24cxx_write(type_card, sendbuf,rcvlen,recebuf);
		break;
		default:
		recebuf[0] = 0x6d;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		ret = -1;
		break;
	}
	return ret;
}


