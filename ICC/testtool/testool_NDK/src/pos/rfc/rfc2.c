/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name		: rfc2.c
* Author 			: chenfm
* version			: 
* DATE			: 20130108
* directory 		: 
* description		: 测试NDK_RfidPiccType接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_RfidPiccType"

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
*			  		    chenfm	        20130108	 	created
*****************************************************************/
void rfc2(void)
{
	//以下为局部变量
	uchar psPicctype=0x00, psDatabuf[256]={0}, ucPicctype=0;  
	int ret= -1, pnDatalen=0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_RfidPiccDeactivate(0);

	//case1:未知类型
	if((ret=NDK_RfidPiccType(0xff))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case2:设置卡类型为A，使用B卡操作（寻卡），应该失败返回
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_A))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	cls_show_msg("请在感应区放置B标准卡，按任意键继续...");
	if((ret=NDK_RfidPiccDetect(&ucPicctype))!=NDK_ERR_RFID_NOCARD)//NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_ERR_RFID_NOTDETE)//NDK_ERR_RFID_NOCARD)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:设备卡类型为A,使用A卡操作（寻卡），应该成功返回
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_A))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	cls_show_msg("请在感应区放置A标准卡，按任意键继续...");
	if((ret=NDK_RfidPiccDetect(&ucPicctype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=RFCARD_TYPE_A)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		GOTOERR;
	}
	NDK_RfidPiccDeactivate(0);//激活成功之后应下电，才能进行后续测试

	//case4:设备卡类型为B,使用B卡操作（寻卡），应该成功返回
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_B))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	cls_show_msg("请在感应区放置B标准卡，按任意键继续...");
	if((ret=NDK_RfidPiccDetect(&ucPicctype))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=RFCARD_TYPE_B)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		GOTOERR;
	}
	NDK_RfidPiccDeactivate(0);//激活成功之后应下电，才能进行后续测试

	//case5:设备卡类型为B,使用A卡操作（寻卡），应该成功返回
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_B))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	cls_show_msg("请在感应区放置A标准卡，按任意键继续...");
	if((ret=NDK_RfidPiccDetect(&ucPicctype))!=NDK_ERR_RFID_NOCARD)//NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_ERR_RFID_NOTDETE)//NDK_ERR_RFID_NOCARD)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//case6:设备卡类型为AB,使用A或B卡操作（寻卡），都应该成功返回
	//此用例放到其他用例附带测试

	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);
	return;
}

