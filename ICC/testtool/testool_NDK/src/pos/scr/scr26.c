/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示
* file name		: scr26.c
* Author 			: huangjianb
* version			: 
* DATE				: 20140617
* directory 	: 
* description	: 测试NDK_ScrTSCalibrate函数触屏校验功能
* 
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		 "NDK_ScrTSCalibrate"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr26
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	  	 huangjianb   20140617 	created
*
*****************************************************************/
void scr26(void)
{
	int ret = -1;
	unsigned int unLen=0;
	char szBuf[24]={0};

	if(auto_flag==1)
	{
		send_result("%s不支持自动测试,请重新手动测试", TESTAPI);
		return;
	}
	//测试前置
	//测试前置要修改触屏参数文件，将参数修改成触屏不能正常使用，参数文件修改后需要重启才能生效
	if(cls_show_msg("测试前请确保已进入/guiapp/etc路径修改pointercal文件修改成触屏不准确:是[确认],否[其它]")!=ENTER)
		return;
	
	//读取pos硬件信息接口 
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_HARDWARE_INFO, &unLen, szBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//case1：异常测试,非触屏机型应返回NDK_ERR_NO_DEVICES
	if (szBuf[11] == 0xFF)
	{
		if((ret = NDK_ScrTSCalibrate())!=NDK_ERR_NO_DEVICES)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		send_result("%s测试通过(本设备为非触屏设备)", TESTAPI);
		return;
	}

	//测试过程中不允许进行休眠,根据yuyanfei反馈开发确认的20180608 modify
	NDK_SysSetSuspend (0);
	//case2：触屏机型,校准过程中,用户取消键退出
	cls_show_msg("校准过程中请按ESC键应能够退出校准程序,任意键开始测试");
	if((ret = NDK_ScrTSCalibrate())!=NDK_ERR_QUIT)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3：触屏机在校准过程中应禁止桌面弹窗功能，可以换上21%的电池，或者在校准界面按关机键
	cls_show_msg("请在触屏校准过程中按关机键或者构造低电弹窗,任意键开始测试");
	if((ret = NDK_ScrTSCalibrate())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("校准过程中是否出现弹窗:是[确认],否[其它]")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case4：触屏机器应支持正常触屏校验
	cls_show_msg("请进行正常触屏校验,任意键开始测试");
	if((ret = NDK_ScrTSCalibrate())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:测试校准过程中退出，不影响屏幕准确性(即校准中退出，会使用上一次校准的结果)
	cls_show_msg("校准过程中请按ESC键应能够退出校准程序,任意键开始测试");
	if((ret = NDK_ScrTSCalibrate())!=NDK_ERR_QUIT)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//测试结束
	send_result("%s测试通过,触屏应该能正常使用", TESTAPI);
ERR:	
	NDK_SysSetSuspend (1);
	return;
}
