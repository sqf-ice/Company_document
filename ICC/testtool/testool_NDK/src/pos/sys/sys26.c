/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys26.c
* Author 			: linwl
* version			: 
* DATE				: 20150509
* directory 		: 
* description		: 本例测试NDK_SysSetPosInfo设置pos硬件信息接口功能(仅K21支持)
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SysSetPosInfo"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 	: 
* functional description: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	        linwl         20150509	created
*****************************************************************/
void sys26(void)
{
	if(auto_flag==2)
		return;
	//以下为局部变量
	int ret = -1;
	unsigned int unLen=0;
	char usnbuf[128] = {0}, psnbuf[128] = {0}, boardver[128] = {0}, boardnum[128] = {0}, sBuf[128] = {0};
#if ANDIROD_ENABLE  //高端产品新增设置和获取非接芯片类型  20180503 sull add
	char rfidtype[128] = {0};
#endif

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//测试前置:获取测试前配置，测试完后恢复
	memset(usnbuf,0,sizeof(usnbuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_USN, &unLen, usnbuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	memset(psnbuf,0,sizeof(psnbuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_PSN, &unLen, psnbuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	memset(boardver,0,sizeof(boardver));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_BOARD_VER, &unLen, boardver)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	memset(boardnum,0,sizeof(boardnum));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_BOARD_NUMBER, &unLen, boardnum)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#if ANDIROD_ENABLE //高端产品新增设置和获取非接芯片类型  20180503 sull add
	memset(rfidtype,0,sizeof(rfidtype));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_RFID_CHIP_TYPE, &unLen, rfidtype)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	//case1: 异常测试，错误参数，psBuf为空指针
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_BOARD_NUMBER, NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2: 异常测试，错误参数，emFlag为不支持的类型应该返回失败
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_BIOS_VER, "300")) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_KEY_COUNT, "300")) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_POS_TYPE, "ME30")) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_HARDWARE_INFO, "300")) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_CREDITCARD_COUNT, "300")) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_PRN_LEN, "300")) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_POS_RUNTIME, "300")) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_CPU_TYPE, "300")) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//高端产品新增设置和获取非接芯片类型,支持传入的配置有AS3911、MH1608和NORFID  20180503 sull add
#if ANDIROD_ENABLE
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_RFID_CHIP_TYPE, "300")) != NDK_ERR_PARA)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if !CPU5810X_ENABLE	//20170111 SP610新版本出于安全考虑关闭设置PN/SN的功能，返回不支持，后续其它低端产品要陆续根据需求导入 linwl linshengxin
	//case3:设置SYS_HWINFO_GET_POS_USN，设置后获取应该和设置的一致  //ME50NX和ME68只支持设置一次，设置之后再设置会返回-18 20180416 sull
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_POS_USN, "Us2n!@序列")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_USN, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(strcmp(sBuf, "Us2n!@序列"))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, sBuf);
		GOTOERR;
	}
#else
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_POS_USN, "Us2n!@序列")) != NDK_ERR_NOT_SUPPORT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if !CPU5810X_ENABLE
	//case4:设置SYS_HWINFO_GET_POS_PSN   //ME50NX和ME68只支持设置一次，设置之后再设置会返回-18 20180416 sull
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_POS_PSN, "ps2n!@中国")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_PSN, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(strcmp(sBuf, "ps2n!@中国"))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, sBuf);
		GOTOERR;
	}
#else
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_POS_PSN, "ps2n!@中国")) != NDK_ERR_NOT_SUPPORT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case5:设置SYS_HWINFO_GET_BOARD_VER
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_BOARD_VER, "ps2n!@主板号")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_BOARD_VER, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(strcmp(sBuf, "ps2n!@主板号"))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, sBuf);
		GOTOERR;
	}

	//case6:设置SYS_HWINFO_GET_BOARD_NUMBER
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_BOARD_NUMBER, "ps2n!@主板编号")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_BOARD_NUMBER, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(strcmp(sBuf, "ps2n!@主板编号"))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, sBuf);
		goto ERR;
	}

	//case7:高端产品新增设置和获取非接芯片类型  20180503 sull add 
#if ANDIROD_ENABLE
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysSetPosInfo(SYS_HWINFO_GET_RFID_CHIP_TYPE, rfidtype)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_RFID_CHIP_TYPE, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(strcmp(sBuf, rfidtype))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, sBuf);
		goto ERR;
	}
#endif

	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_SysSetPosInfo(SYS_HWINFO_GET_POS_USN, usnbuf);
	NDK_SysSetPosInfo(SYS_HWINFO_GET_POS_PSN, psnbuf);
	NDK_SysSetPosInfo(SYS_HWINFO_GET_BOARD_VER, boardver);
	NDK_SysSetPosInfo(SYS_HWINFO_GET_BOARD_NUMBER, boardnum);
#if ANDIROD_ENABLE
	NDK_SysSetPosInfo(SYS_HWINFO_GET_RFID_CHIP_TYPE, rfidtype);
#endif
	return;
}

