/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: mag模块
* file name		: mag1.c
* Author 			: chenfm
* version			: 
* DATE			: 20121220
* directory 		: 
* description		: 测试NDK_MagOpen接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"NDK_MagOpen"

/****************************************************************
* function name 	 	: void mag1(void)
* functional description 	: 打开磁卡设备
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	  chenfm     20121220	created
*****************************************************************/
void mag1(void)
{
	int ret= -1, pnErrorCode=0;
	uchar pcSwiped= -1;
	char pszTk1[MAXTRACKLEN]={0}, pszTk2[MAXTRACKLEN]={0}, pszTk3[MAXTRACKLEN]={0};

	cls_printf("测试%s...", TESTAPI); 

	//测试前置：
	NDK_MagClose();
#if !(defined ME30MH ||defined ME32||defined N920) //ME30MH磁卡是厂家设计 不能对异常情况做判断
	//case1:未打开磁卡，刷卡，读数据应该读不到。
	cls_show_msg("请刷卡后按[ENTER]继续.....");
	if((ret=NDK_MagSwiped(&pcSwiped))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pcSwiped);
		GOTOERR;
	}
	if((ret=NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &pnErrorCode))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例测试通过!");
#endif
	//case2 :第一次打开磁卡设备，刷卡，读数据，应成功读到数据。
	if((ret=NDK_MagOpen())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !(defined ME15CHX||defined ME30MH||defined ME32||defined ME15B||defined N920)
	cls_show_msg("请刷卡后按[ENTER]继续..");
	if((ret=NDK_MagSwiped(&pcSwiped))!=NDK_OK||pcSwiped!=1)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pcSwiped);
		GOTOERR;
	}
#else
	cls_printf("请刷卡..");
	while(1)
	{
		NDK_MagSwiped(&pcSwiped);
		if(pcSwiped==1)
			break;
	}
#endif
	if((ret=NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &pnErrorCode))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
#if !(defined ME30MH ||defined ME32||defined N920) //ME30MH磁卡是厂家设计 不能对异常情况做判断
	//case3:重复打开，应打开失败。
	if((ret=NDK_MagOpen())!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
#endif
	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_MagClose();
}

