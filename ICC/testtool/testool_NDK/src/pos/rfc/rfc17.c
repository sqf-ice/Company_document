/************************************************************************
*
* copyright(c)2012-2015 Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name			: rfc17.c
* Author 			:  
* version			: 
* DATE				:  
* directory 			: 
* description			: 测试NDK_Iso15693SelectPicc,NDK_Iso15693GetPicc_SystemInfo能否正常进行防冲突之后的选卡激活操作并进行获取卡片相关信息(Iso15693协议)
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_Iso15693SelectPicc,NDK_Iso15693GetPicc_SystemInfo"

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
void rfc17(void)
{
	int ret = -1, i = 0, j = 0;
	uint cardcount = 0, maxcards = 20;
	iso15693ProximityCard_t cards[20];		//定义最多支持20张ISO1569卡的读写
	iso15693PiccSystemInformation_t sysinfo;

	//初始化
	if((ret=NDK_Iso15693_init()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case1:未进行防冲突应该返回失败
	if((ret=NDK_Iso15693SelectPicc(cards[0])) != NDK_ERR_RFID_SEEKING)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	cls_show_msg("放置一张15693卡片,任意键继续");
	if((ret=NDK_Iso15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,&cardcount,cards)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	//for(i = 0; i < 20; i++)
	{
		for(j = 0; j < ISO15693_UID_LENGTH; j++)
		{
			send_result("%02x ",cards[0].uid[j]);
		}
	}
	//case2:防冲突成功后未放置卡片应该返回无卡
	cls_show_msg("把15693卡片移开");//??
	if((ret=NDK_Iso15693SelectPicc(cards[0])) != NDK_ERR_RFID_NOCARD)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	
	//case3:正常测试:放置卡片,寻到卡输出内容激活成功后获取卡片内容
	cls_show_msg("放置一张15693卡片,任意键继续");
	if((ret=NDK_Iso15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,&cardcount,cards)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_Iso15693SelectPicc(cards[0])) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case4:参数异常
	if((ret=NDK_Iso15693GetPicc_SystemInfo(cards[0],NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	
	//case5:正常获取卡片内容
	if((ret=NDK_Iso15693GetPicc_SystemInfo(cards[0],&sysinfo)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	//输出卡片内容
	send_result("flag:%d,uid:%02x",sysinfo.flags,sysinfo.uid);

	//测试结束
	send_result("%s测试通过", TESTAPI);
	
ERR:
	NDK_Iso15693_Deinit();
	return;
}

