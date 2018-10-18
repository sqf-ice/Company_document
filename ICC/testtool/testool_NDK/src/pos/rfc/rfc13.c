/************************************************************************
*
* copyright(c)2012-2015 Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name			: rfc13.c
* Author 			:  zhengry
* version			: 
* DATE				:  20150811
* directory 			: 
* description			: 测试NDK_RfidFelicaPoll对felica卡寻卡 
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_RfidFelicaPoll"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		       zhengry	        20150811	 	created
*****************************************************************/
void rfc13(void)
{
	int ret=0, pUIDlen=0;
	uchar UID[300]={0};
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//case1:参数错误
	if((ret=NDK_RfidFelicaPoll(NULL, &pUIDlen))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidFelicaPoll(UID, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:无卡：将felica卡放置在场强范围外
	cls_show_msg("请在感应区不放置任何卡，按任意键继续...");
	if((ret=NDK_RfidFelicaPoll(UID, &pUIDlen))!=NDK_ERR_RFID_NOCARD)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:正常测试：正常放置felica卡,寻卡应正常
	cls_show_msg("请在感应区放置Felica卡，按任意键继续...");
	if((ret=NDK_RfidFelicaPoll(UID, &pUIDlen))!=NDK_OK)
	{
		send_result("line %d:%s寻卡失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:异常测试：拿不是felica卡寻卡应该失败
	cls_show_msg("请在感应区放置标准A卡，按任意键继续...");
	if((ret=NDK_RfidFelicaPoll(UID, &pUIDlen))!=NDK_ERR_RFID_NOCARD)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:异常测试：多卡：取一张A卡和一张felica卡寻卡不应跑飞
	cls_show_msg("请在感应区放置标准A卡和Felica卡，按任意键继续...");
	if((ret=NDK_RfidFelicaPoll(UID, &pUIDlen))!=NDK_OK)
	{
		send_result("line %d:%s寻卡失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//下电
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);
	return;

}

