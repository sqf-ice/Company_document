#include "sle4442.h"
#include "sle4428.h"
#include "sc102_1604.h"
#include "at88sc153.h"
#include "at88sc1608.h"
#include "ic_memory.h"
#include "memory.h"
#include "at24xx.h"
int mem_powon_flag = 0;
int memory_poweron(unsigned char mem_type,unsigned char *atrbuf)
{
	int ret = 0;
	//mem_timer_init();
	switch(mem_type)
	{
		case ic0sle44x8:
			ret = sle4428_poweron(atrbuf);
			break;

		case ic0at24c01:
		case ic0at24c02:
		case ic0at24c04:
		case ic0at24c08:
		case ic0at24c16:
		case ic0at24c32:
		case ic0at24c64:
			ret = at24cxx_poweron(atrbuf);
			break;

		case ic0sle44x2:
			ret = sle4442_poweron(atrbuf);
			break;

		case ic0at88sc102:
		case ic0at88sc1604:
			ret = sc102_1604_powon(atrbuf);
			break;

		case ic0at88sc1608:
			ret = at88sc1608_poweron(atrbuf);
			break;

		case ic0at45d041_3v:
			
			break;

		case ic0at88sc153:
			ret = at88sc153_poweron(atrbuf);
			break;
			
		default :
			ret = -1;
			break;
	}
	if(ret == 0) mem_powon_flag = 1;
	return ret;
}
int memory_rw(unsigned char mem_type,const unsigned char *sendbuf,
				  unsigned char *rcvlen,unsigned char *recebuf)
{
	int ret = 0;
	switch(mem_type)
	{
		case ic0sle44x8:
			ret = sle4428_pro_main(sendbuf, rcvlen, recebuf);
			break;

		case ic0at24c01:
		case ic0at24c02:
		case ic0at24c04:
		case ic0at24c08:
		case ic0at24c16:
		case ic0at24c32:
		case ic0at24c64:
			ret = at24cxx_pro_main(mem_type,sendbuf, rcvlen, recebuf);
			break;

		case ic0sle44x2:
			ret = sle4442_pro_main(sendbuf, rcvlen, recebuf);
			break;

		case ic0at88sc102:
			ret = at88sc102_pro_main(sendbuf, rcvlen, recebuf);
			break;

		case ic0at88sc1604:
			ret = at88sc1604_pro_main(sendbuf,rcvlen, recebuf);
			break;

		case ic0at88sc1608:
			ret = at88sc1608_pro_main(sendbuf,rcvlen, recebuf);
			break;

		case ic0at45d041_3v:
			
			break;

		case ic0at88sc153:
			ret = at88sc153_pro_main(sendbuf,rcvlen, recebuf);
			break;
			
		default :
			ret = -1;
			break;
	}
	return ret;
}

