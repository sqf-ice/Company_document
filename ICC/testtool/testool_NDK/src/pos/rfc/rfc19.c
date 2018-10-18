/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name			: rfc19.c
* Author 			:  
* version			: 
* DATE				:  
* directory 			: 
* description		: 测试NDK_MifareActive寻卡、防碰撞、选卡接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_MifareActive"

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
*			  		    jiangym			    20180110		created  
*****************************************************************/
void rfc19(void)
{
	//以下为局部变量
	int ret = -1;
	uchar UIDLen = 0,UIDLen1 = 0, sak;
	uchar UID[LEN_UID]={0}, UID1[LEN_UID]={0};

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_RfidPiccDeactivate(0);
	//case1:异常测试，错误参数	     
	if((ret=NDK_MifareActive(REQA, NULL, &UIDLen,&sak))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MifareActive(REQA, UID, NULL,&sak))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:未放置卡片,寻卡防冲突失败返回无卡-2030
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,&sak))!=NDK_ERR_MI_NOTAGERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:放置错误的卡片,返回失败 
	cls_show_msg("请在感应区放置1张标准B卡,任意键继续...");
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,&sak))!=NDK_ERR_MI_NOTAGERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:放置两张卡应该返回多卡冲突 -2048 
	cls_show_msg("请在感应区放置2张M0卡,任意键继续...");
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,&sak))!=NDK_ERR_MI_COLLERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:正常放置一张M卡,应该返回成功
	cls_show_msg("请在感应区放置1张M0卡,任意键继续...");
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,&sak))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//需要先下电再调用寻卡才有效
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6:同一张卡获取的uid应该相同
	if((ret=NDK_MifareActive(REQA, UID1, &UIDLen,&sak))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(UID, UID1, sizeof(UID)))
	{
		send_result("line %d:同张卡UID不一致", __LINE__);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case7:不同一张卡获取的uid应该不同
	cls_show_msg("请在感应区放置另外一张M0卡,任意键继续...");
	if((ret=NDK_MifareActive(REQA, UID1, &UIDLen1,&sak))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(!memcmp(UID, UID1, sizeof(UID)))
	{
		send_result("line %d:不同卡UID应不一致", __LINE__);
		GOTOERR;
	}
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

