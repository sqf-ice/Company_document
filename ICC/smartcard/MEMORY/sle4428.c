#include "sle4428.h"
#include "memory.h"
static void sle4428_write1byte(unsigned char byte)
{
	unsigned char i = 0,temp = 0;
	temp = byte;
	for(i = 0;i < 8;i++)
	{
		tda8035_set_io(temp & 0x01);
		temp >>= 1;
		delay_us(9);// 20 change to 10 2013-08-29
		tda8035_set_clk(1);
		delay_us(9);
		tda8035_set_clk(0);
		delay_us(5);//add 2013-08-29
	}
}/*read protect bit*/
static unsigned int sle4428_read1byte(unsigned char rd_pflag)
{
	unsigned char i = 0;
	unsigned int byte = 0;
	tda8035_set_io_type(input);
	for(i = 0;i < 8;i++)
	{
		delay_us(9);
		byte >>= 1;
		if(tda8035_get_io())
			byte |= 0x80;
		tda8035_set_clk(1);
		delay_us(9);
		tda8035_set_clk(0);
	}
	if(rd_pflag)
	{
		delay_us(9);
		if(tda8035_get_io())
			byte |= 0x100;
		tda8035_set_clk(1);
		delay_us(9);
		tda8035_set_clk(0);
	}
	return byte;
}
int sle4428_poweron(unsigned char *atrbuf)
{

	unsigned char i = 0;
	//set_sc0_gpio();
	set_sc0_gpio_tda8035vcc5v();
	delay_ms(1);
	mem_printk("sle4428\n");
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
	delay_us(20);
	tda8035_set_clk(1);
	delay_us(20);
	if(!tda8035_get_io())
	{
		mem_printk("io err\n");
		return -1;
	}
	tda8035_set_clk(0);
	delay_us(20);
	tda8035_set_rst(0);
	delay_us(20);
	for( i = 1;i < 5;i++)
	{
		atrbuf[i] = (unsigned char)sle4428_read1byte(0);
	}
	delay_us(20);
	tda8035_set_io_type(output);
	tda8035_set_io(1);
	delay_us(20);
	atrbuf[0] = 0x06;
	atrbuf[5] = 0x90;
	atrbuf[6] = 0x00;
	return 0;	
}
static void sle4428_sendcmd(unsigned char cmd,unsigned int addr,unsigned char data)
{
	unsigned char h_addr = 0,l_addr = 0;
	l_addr = addr & 0xff;
	h_addr = (addr >> 2) & 0xc0;
	tda8035_set_clk(1);
	delay_us(9);
	tda8035_set_clk(0);
	delay_us(9);//add 2013-08-29
	tda8035_set_io_type(output);
	tda8035_set_rst(1);
	delay_us(5);
	
	sle4428_write1byte(cmd | h_addr);
	sle4428_write1byte(l_addr);
	sle4428_write1byte(data);

	tda8035_set_io(0);
	tda8035_set_io_type(input);
	tda8035_set_rst(0);
	delay_us(9);
	tda8035_set_clk(1);
	delay_us(9);
	tda8035_set_clk(0);
	return ;
}
static int sle4428_readdata(unsigned char rd_pflag,unsigned int addr,unsigned char len,unsigned char *recebuf)
{
	unsigned char i = 0,command = 0,tmp_len = 0;
	unsigned int tmp = 0;
	if(rd_pflag)
	{
		command = 0x0c;
		tmp_len = len << 1;/*len *2*/
	}
	else
	{
	 	command = 0x0e;
	 	tmp_len = len ;
	 }
	 sle4428_sendcmd(command,addr,0);
	 for(i = 0; i < tmp_len ; i++ )
	 {
		tmp = sle4428_read1byte(rd_pflag);
		recebuf[i] = tmp & 0xff;
		if(rd_pflag)
		{
			recebuf[++i] = (tmp >> 8)& 0x01;
		}
	 }
	 return i ;
}
static int sle4428_pro_read(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned char i = 0,len = 0,rd_pflag = 0,h_addr = 0;
	unsigned int addr = 0;
	h_addr = sendbuf[2] & 0x7f;
	len = sendbuf[4];
	addr = sendbuf[3] | (h_addr << 8);
	rd_pflag = (sendbuf[2] & 0x80 ? 1 : 0);
	mem_printk("rd_pflag %d \n",rd_pflag);
	if((h_addr > 3) ||((h_addr ==  3)&&(sendbuf[3] + len > 256)))
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return -1;
	}
	if((len == 0) || ((rd_pflag == 1) && (len > MEM_RW_MAX_LEN/2)) || (len > MEM_RW_MAX_LEN))
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return -1;
	}
	i = sle4428_readdata(rd_pflag,addr,len,recebuf);
	recebuf[i] = 0x90;
	recebuf[i+1] = 0x00;
	*rcvlen = i+2;
	return 0;
}
static int sle4428_writedata(unsigned char cmd,unsigned int addr ,unsigned char data)
{
	unsigned char i = 0;
	sle4428_sendcmd( cmd,addr, data);
	for(i = 0; i < 250;i++ )
	{
		delay_us(9);
		tda8035_set_clk(1);
		delay_us(9);
		tda8035_set_clk(0);
		if(!tda8035_get_io())
			return 0;
	}
	return -1;
}
static int sle4428_pro_write(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned char i = 0,len = 0,h_addr = 0,cmd = 0;
	unsigned int addr = 0;
	h_addr = sendbuf[2] & 0x3f;
	len = sendbuf[4];
	addr = sendbuf[3] | (h_addr << 8);
	if((h_addr > 3) ||((h_addr ==  3)&&(sendbuf[3] + len > 256)))
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return -1;
	}
	if((len == 0) || (len > MEM_RW_MAX_LEN))
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return -1;
	}
	switch( sendbuf[2] >> 6)
	{
		case 0 ://写数据，不写保护位		00
			cmd = 0x33;
		break;
		case 2 ://写数据和保护位			10
			cmd = 0x31;
		break;
		case 3 : //比较数据和写保护位		11
			cmd = 0x30;
		break;
		default :
			recebuf[0] = 0x6a;
			recebuf[1] = 0x86;
			*rcvlen = 2;
			return -1;
		break;
	}
	for( i = 0 ;i < len;i++)
	{
		if(sle4428_writedata(cmd, addr,sendbuf[i+5]) == -1)
		{
			delay_ms(5);
			recebuf[0] = 0x63;
			recebuf[1] = 0xc8;
			*rcvlen = 2;
			return -1;
		}
		//delay_ms(1);
		addr++;
	}
	//delay_ms(5);
	recebuf[0] = 0x90;
	recebuf[1] = 0x00;
	*rcvlen = 2;
	return 0;
}
static int sle4428_pro_verification(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned char i = 0,j = 0,tmp = 0;
	if((sendbuf[2] != 0) && (sendbuf[3] != 0))
	{
		recebuf[0] = 0x6a;
		recebuf[1] = 0x86;
		*rcvlen = 2;
		return -1;
	}
	if(sendbuf[4]!= 2)
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return -1;
	}
	sle4428_readdata(0,1021,1,recebuf);
	tmp =recebuf[0];
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
	if(sle4428_writedata(0x32, 1021, tmp)== -1)
	{
		recebuf[0] = 0x63;
		recebuf[1] = 0xc8;
		*rcvlen = 2;
		return -1;
	}
	if(sle4428_writedata(0x0d, 1022, sendbuf[5])== -1)
	{
		recebuf[0] = 0x63;
		recebuf[1] = 0xc8;
		*rcvlen = 2;
		return -1;
	}
	if(sle4428_writedata(0x0d, 1023, sendbuf[6])== -1)
	{
		recebuf[0] = 0x63;
		recebuf[1] = 0xc8;
		*rcvlen = 2;
		return -1;
	}
	sle4428_writedata(0x33, 1021, 0xff);
	sle4428_readdata(0,1021,1,recebuf);
	tmp =recebuf[0];
	if(tmp != 0xff)
	{
		j = 0;
		for(i = 0;i < 8;i++)
		{
			if(tmp & 0x01)
				j++;
			tmp >>= 1;
		}
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
int sle4428_pro_main(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
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
	switch(sendbuf[1])
	{
		case 0x20:
			ret = sle4428_pro_verification(sendbuf,rcvlen,recebuf);
		break;
		case 0xB0:
			ret = sle4428_pro_read(sendbuf,rcvlen,recebuf);
		break;
		case 0xD0:
			ret = sle4428_pro_write(sendbuf,rcvlen,recebuf);
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


