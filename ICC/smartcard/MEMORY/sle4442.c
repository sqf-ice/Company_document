#include "sle4442.h"
#include "memory.h"
static unsigned char sle4442_read1byte(void)
{
	unsigned char i = 0,byte = 0;
	tda8035_set_io_type(input);
	for(i = 0;i < 8;i++)
	{
		delay_us(10);
		byte >>= 1;
		if(tda8035_get_io())
			byte |= 0x80;
		tda8035_set_clk(1);
		delay_us(10);
		tda8035_set_clk(0);
	}
	return byte;
}
static void sle4442_write1byte(unsigned char byte)
{
	unsigned char i = 0,temp = 0;
	temp = byte;
	for(i = 0;i < 8;i++)
	{
		tda8035_set_io(temp & 0x01);
		temp >>= 1;
		delay_us(10);
		tda8035_set_clk(1);
		delay_us(10);
		tda8035_set_clk(0);
	}
}
int sle4442_poweron(unsigned char *atrbuf)
{
	unsigned char i = 0;
	//set_sc0_gpio();
	set_sc0_gpio_tda8035vcc5v();
	delay_ms(1);
	//tda8035_poweron_clk();
	#if 1
	if(!check_mem_insert())
	{
		mem_printk("no card\n");
		return -1;
	}
	#endif
	tda8035_set_vcc_onoff(1);
	tda8035_set_io_type(output);
	tda8035_set_io(1);
	tda8035_set_clk(0);
	tda8035_set_vcc_onoff(0);
	tda8035_set_io_type(input);
	//tda8035_poweron_clk();
	delay_ms(3);
	delay_us(100);
	tda8035_poweron_clk();
	tda8035_set_rst(1);
	delay_us(10);
	tda8035_set_clk(1);
	delay_us(10);
	if(!tda8035_get_io())
	{
		mem_printk("io err\n");
		return -1;
	}
	tda8035_set_clk(0);
	delay_us(10);
	tda8035_set_rst(0);
	delay_us(10);
	for( i = 1;i < 5;i++)
	{
		atrbuf[i] = sle4442_read1byte();
	}
	delay_us(10);
	tda8035_set_io_type(output);
	tda8035_set_io(1);
	delay_us(10);
	atrbuf[0] = 0x06;
	atrbuf[5] = 0x90;
	atrbuf[6] = 0x00;
	return 0;	
}
/*clk =0 rst = 1 */
static void sle4442_break(void)
{
	tda8035_set_clk(1);
	delay_us(10);
	tda8035_set_clk(0);
	delay_us(10);
	//tda8035_poweron_clk();
	tda8035_set_rst(1);
	delay_us(10);
	tda8035_set_rst(0);
}
/*clk =1 io = 1--0*/
static void sle4442_start(void)
{
	tda8035_set_clk(0);
	delay_us(10);
	tda8035_set_io_type(output);
	tda8035_set_io(1);
	delay_us(20);
	tda8035_set_clk(1);
	delay_us(10);
	tda8035_set_io(0);
	delay_us(10);
	tda8035_set_clk(0);
	
}
static void sle4442_stop(void)
{
	tda8035_set_io(0);
	delay_us(10);
	tda8035_set_clk(1);
	delay_us(10);
	tda8035_set_io(1);
	delay_us(10);
	tda8035_set_io_type(input);
	tda8035_set_clk(0);
}

static int sle4442_senddata(unsigned char cmd,unsigned char addr,unsigned data)
{
	unsigned int i = 0;
	sle4442_break();
	sle4442_start();
	sle4442_write1byte(cmd);
	sle4442_write1byte(addr);
	sle4442_write1byte(data);
	sle4442_stop(); 
	/*write commad send clk */
	if((cmd == WR_MAIN_RAM) || (cmd == WR_P_RAM) || (cmd == WR_PSC_RAM) || (cmd == COMP_PSC_RAM))
	{
		for(i = 0;i < 256;i++)
		{
			delay_us(10);
			tda8035_set_clk(1);
			delay_us(10);
			tda8035_set_clk(0);
			if(tda8035_get_io())
				return 0;
		}
		return -1;
	}
	return 0;
}
static void sle4442_rd_mainmem(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned char i = 0,addr = 0,rlen = 0;
	addr = sendbuf[3];
	rlen = sendbuf[4];
	if((rlen == 0) || (( addr + rlen ) > 256) || (rlen > MEM_RW_MAX_LEN))
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return ;
	}
	sle4442_senddata(RD_MAIN_RAM,addr,rlen);//rlen no effect
	for(i = 0;i < rlen;i++)
	{
		recebuf[i] = sle4442_read1byte();
	}
	/*After the command entry the IFD has to supply
      *sufficient clock pulses. The number of clocks
      *is m = (256 – N)/8 + 1
      ****Datasheet page 16.
      *这就是下面这个for的用意
      */
     for(i = 0; i < (~(addr + rlen - 1));i++)
     {
     	sle4442_read1byte();
     }
     delay_us(10);
     tda8035_set_clk(1);
     delay_us(10);
     tda8035_set_clk(0);
     recebuf[rlen] = 0x90;
     recebuf[rlen + 1] = 0x00;
     *rcvlen = rlen +2 ;
      
}
static void sle4442_rd_p_psc_mem( unsigned char cmd,const unsigned char *sendbuf,
			unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned char i =0;
	if(sendbuf[3] != 0)
	{
		recebuf[0] = 0x6a;
		recebuf[1] = 0x86;
		*rcvlen = 2;
	}
	if(sendbuf[4] != 4)
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
	}
	sle4442_senddata(cmd, 0, 4);
	for(i = 0;i < 4;i++)
	{
		recebuf[i] = sle4442_read1byte();
	}
	delay_us(10);
    tda8035_set_clk(1);
    delay_us(10);
    tda8035_set_clk(0);
    recebuf[4] = 0x90;
    recebuf[5] = 0x00;
    *rcvlen = 6 ;
}
static int sle4442_pro_read(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	switch(sendbuf[2])
	{
		case 0x00:
			sle4442_rd_mainmem(sendbuf, rcvlen, recebuf);
		break;
		case 0xC0:
			sle4442_rd_p_psc_mem(RD_PSC_RAM, sendbuf, rcvlen, recebuf);
		break;
		case 0x80:
			sle4442_rd_p_psc_mem(RD_P_RAM, sendbuf, rcvlen, recebuf);
		break;
		default :
			recebuf[0] = 0x6a;
			recebuf[1] = 0x82;
			*rcvlen = 2;
			return -1;
		break;
	}
	return 0;
}
static void sle4442_wr_mainmem(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned char i = 0,addr = 0,rlen = 0;
	addr = sendbuf[3];
	rlen = sendbuf[4];
	if((rlen == 0) || (( addr + rlen ) > 256) || (rlen > MEM_RW_MAX_LEN)) 
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return ;
	}
	for(i = 0;i < rlen;i++)
	{
		if(sle4442_senddata(WR_MAIN_RAM,addr + i,sendbuf[i + 5]) == -1)
		{
			delay_ms(5);
			recebuf[0] = 0x63;
			recebuf[1] = 0xc3;
			*rcvlen = 2;
			return ;
		}
		delay_ms(1);
		
	}
    delay_ms(5); 
    recebuf[0] = 0x90;
    recebuf[1] = 0x00;
    *rcvlen = 2 ;
      
}
static void sle4442_wr_p_psc_mem(unsigned char cmd, const unsigned char *sendbuf,
unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned char i = 0;
	if(cmd == WR_PSC_RAM)
	{
		if(sendbuf[3] != 1)
		{
			recebuf[0] = 0x6a;
			recebuf[1] = 0x86;
			*rcvlen =2;
			return ;
		}
		if(sendbuf[4] != 3)
		{
			recebuf[0] = 0x6a;
			recebuf[1] = 0x81;
			*rcvlen = 2;
			return ;
		}
	}
	else if(cmd == WR_P_RAM)
	{
		if((sendbuf[3] > 0x1f) || ((sendbuf[3]+sendbuf[4]-1) > 0x1f))
	    {
	        recebuf[0] = 0x6a;
			recebuf[1] = 0x86;
			*rcvlen =2;
			return ;
	    }
	    if (sendbuf[4]==0) 
	    {
	        recebuf[0] = 0x67;
			recebuf[1] = 0x00;
			*rcvlen =2;
			return ;
	    }
	}
	for(i = 0;i < sendbuf[4];i++)
	{
		if(sle4442_senddata(cmd, i + sendbuf[3],sendbuf[i + 5]) == -1)
		{
			delay_ms(5);
			recebuf[0] = 0x63;
			recebuf[1] = 0xc3;
			*rcvlen = 2;
			return ;
		}
		delay_ms(1);
		
	}
    delay_ms(5); 
    recebuf[0] = 0x90;
    recebuf[1] = 0x00;
    *rcvlen = 2 ;
      
}
static int sle4442_pro_write(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	switch(sendbuf[2])
	{
		case 0x00:
			sle4442_wr_mainmem(sendbuf, rcvlen, recebuf);
		break;
		case 0xC0:
			sle4442_wr_p_psc_mem(WR_PSC_RAM, sendbuf, rcvlen, recebuf);
		break;
		case 0x80:
			sle4442_wr_p_psc_mem(WR_P_RAM, sendbuf, rcvlen, recebuf);
		break;
		default :
			recebuf[0] = 0x6a;
			recebuf[1] = 0x86;
			*rcvlen = 2;
			return -1;
		break;
	}
	return 0;
}
static int sle4442_pro_verification(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned char i =0,tmp = 0;
	if((sendbuf[2] != 0) && (sendbuf[3] != 0))
	{
		recebuf[0] = 0x6a;
		recebuf[1] = 0x86;
		*rcvlen = 2;
		return -1;
	}
	if(sendbuf[4]!= 3)
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return -1;
	}
	sle4442_senddata(RD_PSC_RAM,0,0);
	tmp = sle4442_read1byte();
	i = sle4442_read1byte();
	i = sle4442_read1byte();
	i = sle4442_read1byte();
	if(tmp == 0)
	{
		recebuf[0] = 0x69;
		recebuf[1] = 0x83;
		*rcvlen = 2;
		return -1;
	}
	if(tmp & 0x01)
		tmp &= 0xfe;
	else
	{
		if(tmp & 0x02)
			tmp &= 0xfd;
		else
		{
			if(tmp & 0x04)
				tmp &= 0xfb;
		}
	}
	if(sle4442_senddata(WR_PSC_RAM,0,tmp) == -1)
	{
		recebuf[0] = 0x63;
		recebuf[1] = 0xC3;
		*rcvlen = 2;
		return -1;
	}
	if(sle4442_senddata(COMP_PSC_RAM,1,sendbuf[5]) == -1)
	{
		recebuf[0] = 0x63;
		recebuf[1] = 0xC3;
		*rcvlen = 2;
		return -1;
	}
	if(sle4442_senddata(COMP_PSC_RAM,2,sendbuf[6]) == -1)
	{
		recebuf[0] = 0x63;
		recebuf[1] = 0xC3;
		*rcvlen = 2;
		return -1;
	}
	if(sle4442_senddata(COMP_PSC_RAM,3,sendbuf[7]) == -1)
	{
		recebuf[0] = 0x63;
		recebuf[1] = 0xC3;
		*rcvlen = 2;
		return -1;
	}
	if(sle4442_senddata(WR_PSC_RAM,0,0xff) == -1)
	{
		recebuf[0] = 0x63;
		recebuf[1] = 0xC3;
		*rcvlen = 2;
		return -1;
	}
	sle4442_senddata(RD_PSC_RAM,0,0);
	tmp = sle4442_read1byte();
	i = sle4442_read1byte();
	i = sle4442_read1byte();
	i = sle4442_read1byte();
	if(tmp != 0x07)
	{
		i = 0;
		if(tmp & 0x01)
			++i;
		tmp >>= 1;
		if(tmp & 0x01)
			++i;
		tmp >>= 1;
		if(tmp & 0x01)
			++i;
		recebuf[0] = 0x63;
		recebuf[1] = 0xc0 |i;
		*rcvlen = 2;
		return -1;
	}
	recebuf[0] = 0x90;
	recebuf[1] = 0x00;
	*rcvlen = 2;
	return 0;
}
int sle4442_pro_main(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
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
	mem_printk("sendbuf[1] %02x \n",sendbuf[1]);
	switch(sendbuf[1])
	{
		case 0x20:
			ret = sle4442_pro_verification(sendbuf,rcvlen,recebuf);
		break;
		case 0xB0:
			ret = sle4442_pro_read(sendbuf,rcvlen,recebuf);
		break;
		case 0xD0:
			ret = sle4442_pro_write(sendbuf,rcvlen,recebuf);
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


