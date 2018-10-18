/************************************************************************
*
* copyright(c)2012-2015 Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name			: rfc16.c
* Author 			:  
* version			: 
* DATE				:  
* directory 			: 
* description			: 测试NDK_Iso15693_Inventory能否正常寻卡防冲突(Iso15693协议)
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_Iso15693_Inventory"

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
*			  		    	 jiangym			20180127		created
*****************************************************************/
void rfc16(void)
{
	int ret = -1, i = 0, j = 0;
	uint cardcount = 0, maxcards = 20;
	iso15693ProximityCard_t cards[20];		//定义最多支持20张ISO1569卡的读写
	
	//case1:未初始化返回失败
	if((ret=NDK_Iso15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,&cardcount,cards)) != NDK_ERR_RFID_INITSTA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//初始化
	if((ret=NDK_Iso15693_init()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:参数异常,返回失败
	if((ret=NDK_Iso15693_Inventory(ISO15693_NUM_SLOTS_1-1,maxcards,&cardcount,cards)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_Iso15693_Inventory( ISO15693_NUM_SLOTS_16+1,maxcards,&cardcount,cards)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_Iso15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,NULL,cards)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_Iso15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,&cardcount,NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:未放置卡片应该返回无卡
	if((ret=NDK_Iso15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,&cardcount,cards)) != NDK_ERR_RFID_NOCARD)//NDK_ERR_MI_TRANSERR
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:正常测试:放置卡片,寻到卡输出内容
	cls_show_msg("放置一张15693卡片,任意键继续");
	if((ret=NDK_Iso15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,&cardcount,cards)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
/*	for(i = 0; i < 20; i++)
	{
		for(j = 0; j < ISO15693_UID_LENGTH; j++)
		{
			send_result("%02x ",cards[i].uid[j]);
		}
	}*/
	//case5:放置多张卡片,应能检测出多张卡片
	cls_show_msg("放置多张15693卡片,任意键继续");
	if((ret=NDK_Iso15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,&cardcount,cards)) != NDK_ERR_RFID_MULTICARD)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_Iso15693_Deinit();
	return;
}


