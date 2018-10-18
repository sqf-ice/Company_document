/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys32.c
* Author 			:  
* version			: 
* DATE				:  
* directory 		: 
* description		: 测试NDk_COS相关函数设置mode获取mode及数据交互的功能(本用例由于条件有限未实际测试,无可验证的设备和固件)
* related document	: NDK.chm
*
************************************************************************/ 
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDk_COS相关"	 

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
static int _NDK_CosReset()
{
#if ANDIROD_ENABLE
	int fd = -1;
	int ret;
	char path[] = "/sys/class/paymodule_k21/power_status";
	fd = open(path, O_RDWR);
	if (fd < 0) {
		cls_printf("open thk88 power failed! [%s]", path);
		return NDK_ERR;
	}

	lseek(fd, 0, SEEK_SET);

	//power down
	write(fd, "0", 1);
	usleep(100*1000);

	//power up
	write(fd, "1", 1);
	usleep(50*1000);

	return 0;
#else
	return NDK_CosReset();
#endif
}

/****************************************************************
* function name 	 	: 
* functional description: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	       jiangym  	20160614   created
*****************************************************************/
void sys33(void)
{
	//以下为局部变量
	int ret = 0;
	uchar cos_mode = -1, recvlen = 0;
	char erase_cmd[] ={0x20,0x03,0x00,0x02,0x00,0x00,0x42};
	char write_cmd[265] = {0x02,0x03,0x00,0x81,0x80,0x00,0xFF,0x10,0x9A,0x9D,0x59,0xD2,0x13,0xF5,0xBE,0x32,0xD2,0xDA,0x48,0x74,0xF8,0xA0,0x20,0x48,0x97,0x67,0xB7,0x63,0x42,0x86,0xEB,0xFD,0xB7,0x3B,0xD1,0xB7,0xA3,0xDD,0xD7,0x5F,0x5C,0xC5,0x67,0x92,0x9E,0x2D,0xD6,0x12,0x2B,0x9B,0x56,0xCD,0x11,0xEC,0x9B,0xB1,0x5A,0x4F,0xAB,0x82,0x54,0x7C,0x8C,0x86,0x4B,0xE9,0x82,0x40,0x99,0xBC,0x10,0x23,0x00,0x45,0x67,0x77,0x91,0xFD,0xFE,0x02,0x1D,0x47,0x4E,0xD8,0x53,0x4D,0x72,0xF6,0x87,0x11,0xC3,0x39,0x96,0xE6,0x6C,0xB3,0xD9,0xD1,0x1A,0xA0,0xCE,0x80,0xDC,0xAF,0x2C,0x03,0xCF,0x8E,0x8B,0x3C,0x38,0xCD,0x9E,0x6B,0x70,0x00,0x00,0x00};
	char verify_cmd[] = {0x62,0x03,0x00,0x82,0x00,0x00,0x12,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x17,0x7F,0xE5};
	char rec_cmd[64] = {0};
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//case1:参数异常测试(  底层各种参数还未处理)
	if((ret=NDK_CosSetMode(-1)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);//-1
		RETURN;
	}
	if((ret=NDK_CosSetMode(PORT_INPUT+1)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);//-1
		RETURN;
	}
	if((ret=NDK_CosGetMode(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_CosCmdRW(0, erase_cmd, recvlen, rec_cmd)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);//0
		RETURN;
	}
/*	if((ret=NDK_CosCmdRW(strlen(erase_cmd), NULL, recvlen, rec_cmd)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	*///这里会跑飞 
	if((ret=NDK_CosCmdRW(strlen(erase_cmd), erase_cmd, recvlen, NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	

	//case2:正常流程测试
	if((ret=NDK_CosSetMode(PORT_INPUT)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=_NDK_CosReset()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_CosSetMode(PORT_SPICOS)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_CosGetMode(&cos_mode)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if (cos_mode == SPI_BOOT)
		cls_printf("处于BOOT模式下...");
	else if (cos_mode == SPI_COS) 
	{
		NDK_CosSetMode(PORT_SPIBL);
		//需要两次复位:第一次重启切换状态,第二次重启进入boot
		_NDK_CosReset();
		_NDK_CosReset();
		NDK_CosSetMode(PORT_SPICOS);
		NDK_CosGetMode(&cos_mode);
		if (cos_mode != SPI_BOOT) 
		{
			cls_show_msg("切换到BOOT模式失败,任意键退出");
			return;
		}
	} 
	else 
	{
		cls_show_msg("获取cos模式失败,任意键退出");
		return;
	}
	//处在boot模式下进行擦除\ 写 \验证    
	if((ret=NDK_CosCmdRW(strlen(erase_cmd), erase_cmd, recvlen, rec_cmd)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	if((ret=NDK_CosCmdRW(strlen(write_cmd), write_cmd, recvlen, rec_cmd)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	if((ret=NDK_CosCmdRW(strlen(verify_cmd), verify_cmd, recvlen, rec_cmd)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	

	send_result("%s测试通过", TESTAPI);
	return;
}

