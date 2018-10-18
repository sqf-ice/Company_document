/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: mag模块
* file name		: mag5.c
* Author 			: chenfm
* version			: 
* DATE			: 20121220
* directory 		: 
* description		: 测试NDK_MagReset接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"NDK_MagReset"

/****************************************************************
* function name 	 	: void mag5(void)
* functional description 	: 复位磁头
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	  chenfm     20121220	created
*****************************************************************/
void mag5(void)
{
	int ret=-1, pnErrorCode=0;
	uchar pcSwiped=-1;
	char pszTk1[MAXTRACKLEN]={0}, pszTk2[MAXTRACKLEN]={0}, pszTk3[MAXTRACKLEN]={0};

	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_MagClose();
#if !(defined ME30MH ||defined ME32||defined N920)
	//case1:在没打开磁卡设备前，复位应失败返回。
	if((ret=NDK_MagReset())!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case2:打开磁卡设备，复位，应成功返回。
	if((ret=NDK_MagOpen())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MagReset())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:刷卡，读数据，再刷卡，复位，再读数据，刷卡状态应未刷卡，应读不到数据
#if !(defined ME15CHX||defined ME30MH||defined ME32||defined ME15B||defined N920)
	cls_show_msg("请刷123磁道正常卡后，按[ENTER]继续..");
	if((ret=NDK_MagSwiped(&pcSwiped))!=NDK_OK||pcSwiped!=1)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pcSwiped);
		GOTOERR;
	}
#else
	cls_printf("请刷123磁道正常卡");
	while(1)
	{
		NDK_MagSwiped(&pcSwiped);
		if(pcSwiped==1)
			break;
	}
#endif
	if((ret=NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &pnErrorCode))!=NDK_OK||!strlen(pszTk1)||!strlen(pszTk2)||!strlen(pszTk3))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例测试通过!");
	cls_show_msg("请刷123磁道正常卡后，按[ENTER]继续..");  
	if((ret=NDK_MagReset())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MagSwiped(&pcSwiped))!=NDK_OK||pcSwiped!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pcSwiped);
		GOTOERR;
	} 	
	if((ret=NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &pnErrorCode))!=NDK_ERR_NOSWIPED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	} 
	
#if !(defined ME30MH||defined ME32||defined N920)
	//case4:关闭磁卡设备，复位应失败返回。
	if((ret=NDK_MagClose())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MagReset())!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
#endif

	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_MagClose();
	return;
}

