/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: mag模块
* file name		: mag2.c
* Author 			: chenfm
* version			: 
* DATE			: 20121220
* directory 		: 
* description		: 测试NDK_MagClose接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"NDK_MagClose"

/****************************************************************
* function name 	 	: void mag2(void)
* functional description 	: 关闭磁卡设备
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	  chenfm     20121220	created
*****************************************************************/
void mag2(void)
{
	int ret=-1;

	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_MagClose();
#if !(defined ME30MH ||defined ME32||defined N920)
	//case1 :在没打开磁卡设备前，调用该接口关闭磁卡设备，应失败返回。
	if((ret=NDK_MagClose())!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case2:打开磁卡设备，再关闭磁卡设备应该成功返回。
	if((ret=NDK_MagOpen())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MagClose())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if !(defined ME30MH ||defined ME32||defined N920)
	//case3:关闭已关闭的磁卡设备，应该失败返回。
	if((ret=NDK_MagClose())!=NDK_ERR)
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

