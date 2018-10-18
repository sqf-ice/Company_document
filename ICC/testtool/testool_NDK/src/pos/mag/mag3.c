/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: mag模块
* file name		: mag3.c
* Author 			: chenfm
* version			: 
* DATE			: 20121220
* directory 		: 
* description		: 测试NDK_MagSwiped接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"NDK_MagSwiped"

/****************************************************************
* function name 	 	: void mag3(void)
* functional description 	: 判断是否刷过卡
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	  chenfm     20121220	created
*****************************************************************/
void mag3(void)
{
	int ret=-1;
	uchar pcSwiped=-1;

	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_MagClose();
#if !(defined ME30MH ||defined ME32||defined N920)
	//case1:未打开磁卡设备后，调用该接口应返回失败。
	if((ret=NDK_MagSwiped(&pcSwiped))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case2:打开磁卡设备，判断刷卡结构应返回0（未刷卡）。
	if((ret=NDK_MagOpen())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MagSwiped(&pcSwiped))!=NDK_OK||pcSwiped!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pcSwiped);
		GOTOERR;
	}  

	//case3:进行刷卡，返回已刷卡
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

	//case4:传错误的参数，应失败返回。
	if((ret=NDK_MagSwiped(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_MagClose();
	return;
}

