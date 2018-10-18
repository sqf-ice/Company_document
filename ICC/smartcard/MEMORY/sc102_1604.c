#include "memory.h"
#include "sc102_1604.h"
/*fus = c4 = auxio1.pgm=c8 = auxio2*/
int sc102_1604_powon(unsigned char *atrbuf)
{
	set_sc0_gpio();
	//delay_ms(1);
	#if 1
		if(!check_mem_insert())
		{
			mem_printk("no card\n");
			return -1;
		}
	#endif

	tda8035_set_auxio1(1);
	tda8035_set_auxio2(1);
	//tda8035_set_vcc_onoff(1);
	tda8035_set_io_type(output);
	tda8035_set_io(1);
	tda8035_set_clk(0);
	tda8035_set_vcc_onoff(0);
	tda8035_set_io_type(input);
	tda8035_set_rst(1);
	delay_ms(4);
	delay_us(100);
	tda8035_poweron_clk();
	tda8035_set_clk(0);
	delay_us(10);
	tda8035_set_rst(0);
	tda8035_set_io_type(input);
	delay_us(2);
	if(!tda8035_get_io())
	{
		return -1;
	}
	tda8035_set_auxio2(0);
	atrbuf[0] = 0x02;
	atrbuf[1] = 0x90;
	atrbuf[2] = 0x00;
	return 0;
}
void sc0204_setaddr(unsigned int addr)
{
	unsigned int i = 0;
	unsigned char j = 0;
	tda8035_set_auxio1(1);
	tda8035_set_rst(1);
	tda8035_set_io(1);
	tda8035_poweron_clk();
	tda8035_set_clk(0);
	delay_us(10);
	tda8035_set_rst(0);
	delay_us(10);
	for(i = 0 ; i< addr;i++)
	{
		for(j = 0;j < 8;j++)
		{
			tda8035_set_clk(1);
			delay_us(10);
			tda8035_set_clk(0);
			delay_us(10);
		}
	}
	tda8035_set_io_type(input);
}
void sc0204_readdata(unsigned char len, unsigned char *recebuf)
{
	unsigned char i = 0, j = 0,tmp =0;
	tda8035_set_io_type(input);
	for(i = 0;i < len;i++)
	{
		tmp = 0;
		for(j = 0;j <8;j++)
		{
			tmp <<= 1;
			tda8035_set_clk(1);
			delay_us(10);
			if(tda8035_get_io())
				tmp += 1;
			tda8035_set_clk(0);
			delay_us(10);	
		}
		recebuf[i] = tmp;
	}
}
int sc0204_writebyte(unsigned char data)
{
	unsigned char i =0,tmp =0,ch = 0;
	tmp = data;
	mem_printk("data %x \n",tmp);
	for(i = 0;i < 8;i++)
	{
		tda8035_set_io_type(input);
		ch = tmp & 0x80;
		if(ch == 0)//write 0
		{
			tda8035_set_auxio2(1);
			delay_us(5);
			tda8035_set_io(0);
			delay_us(5);
			tda8035_set_clk(1);
			delay_us(5);
			tda8035_set_auxio2(0);
			delay_ms(4);
			tda8035_set_clk(0);
			delay_us(5);
			
			tda8035_set_io_type(input);	
			delay_us(5);
			
		}
		tmp <<= 1;
		tda8035_set_clk(1);//addr inc
		delay_us(10);
		if(tda8035_get_io() != ch)//read data
		{
			//return 0;
			;
		}
		tda8035_set_clk(0);
		delay_us(10);
		//delay_ms(1);
	}
	return 1;
}
int sc0204_erasebyte(void)
{
	unsigned char i =0;
	
			//tda8035_set_auxio2(0);
			//delay_us(5);
			//tda8035_set_clk(0);
			//delay_us(5);
			tda8035_set_auxio2(1);
			delay_us(5);
			tda8035_set_io(1);
			delay_us(5);
			tda8035_set_clk(1);
			delay_us(5);
			tda8035_set_auxio2(0);
			delay_us(5);
			tda8035_set_io_type(input);
			delay_ms(4);
			tda8035_set_clk(0);
			delay_us(10);
		
	
		if(tda8035_get_io() != 1)//read data
			return 0;
		for(i = 0;i < 8;i++)
		{
			tda8035_set_clk(1);//addr inc
			delay_us(10);
			tda8035_set_clk(0);
			delay_us(10);
		}
		return 1;
}
void sc0204_checkbytes(CHECK_SC_DATA *sc_pdata,const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned char i = 0,tmp = 0,j = 0,ch = 0;
	unsigned char try_times = 0;
	try_times = sc_pdata->try_times;
	sc0204_setaddr(sc_pdata->addr);
	for(i = 0;i < sc_pdata->len;i++)
	{
		tmp = sendbuf[i + 5];
		for(j = 0;j < 8;j++)
		{
			ch = tmp & 0x80 ? 1 : 0;
			tda8035_set_io(ch);
			tmp <<= 1;
			delay_us(3);
			tda8035_set_clk(1);
			delay_us(3);
			tda8035_set_clk(0);
			delay_us(3);
		}
	}
	tda8035_set_io(1);
	tda8035_set_io_type(input);
	if(try_times == 0)
	{
		/*write bit to 1*/
		//tda8035_set_auxio2(0);
		//delay_us(5);
		//tda8035_set_clk(0);
		//delay_us(5);
		tda8035_set_auxio2(1);
		delay_us(5);
		tda8035_set_io(1);
		delay_us(5);
		tda8035_set_clk(1);
		delay_us(5);
		tda8035_set_auxio2(0);
		delay_us(5);
		tda8035_set_io_type(input);
		delay_ms(4);
		tda8035_set_clk(0);
		delay_us(5);

		tda8035_set_clk(1);//addr inc
		delay_us(10);
		tda8035_set_clk(0);
		delay_us(10);

		recebuf[0] = 0x90;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return;
	}
	while(!tda8035_get_io() && (try_times > 0))
	{
		tda8035_set_clk(1);//addr inc
		delay_us(10);
		tda8035_set_clk(0);
		delay_us(10);
		try_times--;
	}
	if(try_times == 0)
	{
		recebuf[0] = 0x69;
		recebuf[1] = 0x83;
		*rcvlen = 2;
		return;
	}
	/*write bit to 0*/
	//tda8035_set_auxio2(0);
	//delay_us(5);
	//tda8035_set_clk(0);
	//delay_us(5);
	tda8035_set_auxio2(1);
	delay_us(5);
	tda8035_set_io(0);
	delay_us(5);
	tda8035_set_clk(1);
	delay_us(5);
	tda8035_set_auxio2(0);
	delay_us(5);
	tda8035_set_io_type(input);
	delay_ms(4);
	//tda8035_set_clk(1);
	//delay_us(5);
	tda8035_set_clk(0);
	delay_us(5);
/*write bit to 0*/
	//tda8035_set_auxio2(0);
	//delay_us(5);
	//tda8035_set_clk(0);
	//delay_us(5);
	tda8035_set_auxio2(1);
	delay_us(5);
	tda8035_set_io(1);
	delay_us(5);
	tda8035_set_clk(1);
	delay_us(5);
	tda8035_set_auxio2(0);
	delay_us(5);
	tda8035_set_io_type(input);
	delay_ms(4);
	//tda8035_set_clk(1);
	//delay_us(5);
	tda8035_set_clk(0);
	delay_us(5);
		
	if(tda8035_get_io() != 1)//read data
	{
		try_times--;
		if(try_times == 0)
		{
			recebuf[0] = 0x69;
			recebuf[1] = 0x83;
			*rcvlen = 2;
			return;
		}
		else
		{
			recebuf[0] = 0x63;
			recebuf[1] = try_times;
			*rcvlen = 2;
			return;
		}
	}	
	recebuf[0] = 0x90;
	recebuf[1] = 0x00;
	*rcvlen = 2;
	
}
void at88sc102_write(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned int addr = 0;
	unsigned char i = 0,len = 0;
	addr = (sendbuf[2] << 8)|sendbuf[3];
	len = sendbuf[4];
	if(len == 0 || (sendbuf[2] > 0) || ((addr + len) > 178))
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return ;
	}
	sc0204_setaddr(addr);
	for(i = 0;i < len;i++)
	{
		if(!sc0204_writebyte(sendbuf[i+5]))
		{
			delay_ms(5);
			recebuf[0] = 0x65;
			recebuf[1] = 0x81;
			*rcvlen = 2;
			return ;
		}
	}
	delay_ms(5);
	recebuf[0] = 0x90;
	recebuf[1] = 0x00;
	*rcvlen = 2;
	return ;
}
void at88sc102_read(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned int addr = 0;
	unsigned char len = 0;
	addr = sendbuf[2] << 8 |sendbuf[3];
	len = sendbuf[4];

	tda8035_set_auxio1(0);
	tda8035_set_auxio2(0);
	delay_us(10);
	if(len == 0 || (sendbuf[2] > 0) || ((addr + len) > 178))
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return ;
	}
	sc0204_setaddr(addr);
	sc0204_readdata(len,recebuf);
	*rcvlen = len + 2;
	recebuf[len] = 0x90;
	recebuf[len + 1] = 0x00;
	return;
}
void at88sc102_erase(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned int addr = 0;
	unsigned char i = 0,len = 0,erase_sc = 0;
	addr = sendbuf[2] << 8 |sendbuf[3];
	len = sendbuf[4];

	if(len == 0 || (sendbuf[2] > 0) || ((addr + len) > 178))
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return ;
	}
	if((addr & 0x01) || (len & 0x01))
	{
		recebuf[0] = 0x6a;
		recebuf[1] = 0x86;
		*rcvlen = 2;
		return ;
	}
	erase_sc = (addr == 0x0a) ? 1 : 0;
	sc0204_setaddr(addr);
	for(i = 0;i < len;i++)
	{
		if((!erase_sc) && (!sc0204_erasebyte()))
		{
			recebuf[0] = 0x65;
			recebuf[1] = 0x81;
			*rcvlen = 2;
			return ;
		}
	}
	recebuf[0] = 0x90;
	recebuf[1] = 0x00;
	*rcvlen = 2;
	return ;
	
}
void at88sc102_checksc(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned char p1_data = 0,p2_data = 0;
	unsigned char len = 0;
	CHECK_SC_DATA sc_data;
	p1_data = sendbuf[2];
	p2_data = sendbuf[3];
	len = sendbuf[4];
	switch(p1_data)
	{
		case 0:
			if(p2_data != 0)
			{
				recebuf[0] = 0x6a;
				recebuf[1] = 0x86;
				*rcvlen = 2;
				return ;
			}
			sc_data.addr = 0x0a;
			sc_data.len = 2;
			sc_data.try_times = 4;
		break;
		case 1:
			switch(p2_data)
			{
				case 1:
					sc_data.addr = 86;
					sc_data.len = 6;
					sc_data.try_times = 0;
				break;
				case 2:
					sc_data.addr = 156;
					sc_data.len = 4;
					sc_data.try_times = 128;
				break;
				default:
					recebuf[0] = 0x6a;
					recebuf[1] = 0x86;
					*rcvlen = 2;
					return ;	
				break;
			}
		break;
		default:
			recebuf[0] = 0x6a;
			recebuf[1] = 0x86;
			*rcvlen = 2;
			return ;	
		break;
	}
	sc0204_checkbytes(&sc_data, sendbuf,rcvlen, recebuf);
}
int at88sc102_pro_main(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
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
		case 0x0e:
			at88sc102_erase( sendbuf,rcvlen, recebuf);
		break;
		case 0x20:
			at88sc102_checksc(sendbuf,rcvlen,recebuf);
		break;
		case 0xB0:
			at88sc102_read(sendbuf,rcvlen,recebuf);
		break;
		case 0xD0:
			 at88sc102_write(sendbuf,rcvlen,recebuf);
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
void at88sc1604_write(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned int addr = 0;
	unsigned char i = 0,len = 0;
	unsigned char write_sc_flag = 0;
	addr = (sendbuf[2] << 8)|sendbuf[3];
	len = sendbuf[4];
	if(len == 0 || (addr > 0x800) || ( len > MEM_RW_MAX_LEN))
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return ;
	}
	if ((addr==0x0a)||(addr==0x15) ||(addr==0x18) ||(addr==0x4c6) || (addr==0x4c8)||(addr==0x5cb)||\
        (addr==0x5cd)||(addr==0x6d0)||(addr==0x6d2)) 
    {
         write_sc_flag=1;
    }else{
        write_sc_flag=0;
    }
    
	sc0204_setaddr(addr);
	for(i = 0;i < len;i++)
	{
		if((!write_sc_flag)&&(!sc0204_writebyte(sendbuf[i+5])))
		{
			delay_ms(5);
			recebuf[0] = 0x65;
			recebuf[1] = 0x81;
			*rcvlen = 2;
			return ;
		}
	}
	delay_ms(5);
	recebuf[0] = 0x90;
	recebuf[1] = 0x00;
	*rcvlen = 2;
	return ;
}
void at88sc1604_read(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned int addr = 0;
	unsigned char len = 0;
	addr = sendbuf[2] << 8 |sendbuf[3];
	len = sendbuf[4];

	tda8035_set_auxio1(0);
	tda8035_set_auxio2(0);
	delay_us(10);
	if(len == 0 || (addr > 0x800) || ( len > MEM_RW_MAX_LEN))
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return ;
	}
	sc0204_setaddr(addr);
	sc0204_readdata(len,recebuf);
	*rcvlen = len + 2;
	recebuf[len] = 0x90;
	recebuf[len + 1] = 0x00;
	return;
}
void at88sc1604_erase(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned int addr = 0;
	unsigned char i = 0,len = 0,erase_sc = 0;
	addr = sendbuf[2] << 8 |sendbuf[3];
	len = sendbuf[4];

	if(len == 0 || (addr > 0x800) || ( len > MEM_RW_MAX_LEN))
	{
		recebuf[0] = 0x67;
		recebuf[1] = 0x00;
		*rcvlen = 2;
		return ;
	}
	if((addr & 0x01) || (len & 0x01))
	{
		recebuf[0] = 0x6a;
		recebuf[1] = 0x86;
		*rcvlen = 2;
		return ;
	}
	if ((addr==0x0a)||(addr==0x15) ||(addr==0x18) ||(addr==0x4c6) || (addr==0x4c8)||(addr==0x5cb)||\
        (addr==0x5cd)||(addr==0x6d0)||(addr==0x6d2)) 
    {
         erase_sc=1;
    }else{
        erase_sc=0;
    }
	sc0204_setaddr(addr);
	for(i = 0;i < len;i++)
	{
		if((!erase_sc) && (!sc0204_erasebyte()))
		{
			recebuf[0] = 0x65;
			recebuf[1] = 0x81;
			*rcvlen = 2;
			return ;
		}
	}
	recebuf[0] = 0x90;
	recebuf[1] = 0x00;
	*rcvlen = 2;
	return ;
	
}
void at88sc1604_checksc(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
{
	unsigned int p1_data = 0,p2_data = 0;
	unsigned char len = 0;
	CHECK_SC_DATA sc_data;
	p1_data = sendbuf[2];
	p2_data = sendbuf[3];
	len = sendbuf[4];
	sc_data.len = 2;
	switch(p1_data)
	{
		case 0:
			switch(p2_data)
			{
				case 0:
					sc_data.addr = 0x0a;
					sc_data.try_times = 8;
				break;
				case 1:
					sc_data.addr = 0x15;
					sc_data.try_times = 8;
				break;
				case 2:
					sc_data.addr = 0x4c6;
					sc_data.try_times = 0;
				break;
				case 3:
					sc_data.addr = 0x5cb;
					sc_data.try_times = 0;
				break;
				case 4:
					sc_data.addr = 0x6d0;
					sc_data.try_times = 0;
				break;
				default:
					recebuf[0] = 0x6a;
					recebuf[1] = 0x86;
					*rcvlen = 2;
					return ;
				break;
			}
		break;
		case 1:
			sc_data.try_times = 8;
			switch(p2_data)
			{
				case 1:
					sc_data.addr = 0x18;
				break;
				case 2:
					sc_data.addr = 0x4c8;
				break;
				case 3:
					sc_data.addr = 0x5cd;
				break;
				case 4:
					sc_data.addr = 0x6d2;
				break;
				default:
					recebuf[0] = 0x6a;
					recebuf[1] = 0x86;
					*rcvlen = 2;
					return ;	
				break;
			}
		break;
		default:
			recebuf[0] = 0x6a;
			recebuf[1] = 0x86;
			*rcvlen = 2;
			return ;	
		break;
	}
	sc0204_checkbytes(&sc_data, sendbuf,rcvlen, recebuf);
}
int at88sc1604_pro_main(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf)
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
		case 0x0e:
			 at88sc1604_erase( sendbuf,rcvlen, recebuf);
		break;
		case 0x20:
			 at88sc1604_checksc(sendbuf,rcvlen,recebuf);
		break;
		case 0xB0:
			 at88sc1604_read(sendbuf,rcvlen,recebuf);
		break;
		case 0xD0:
			 at88sc1604_write(sendbuf,rcvlen,recebuf);
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


