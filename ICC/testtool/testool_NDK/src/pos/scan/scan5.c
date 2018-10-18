/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 扫描
* file name			: 5.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_ScanSet摄像头相关参数配置配置(非阻塞式)
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_ScanSet"
#define 	MAXTIME     60
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: scan5
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*							zhengry			20160526		created
*
*****************************************************************/
void scan5(void)
{
	//测试前置
	int ret=0;

	//case1:未初始化摄像头，应返回摄像头未初始化
	if((ret=NDK_ScanSet(CAMERA_SETTYPE_IMAGE, 0))!=NDK_ERR_CAMERA_INIT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return; 
	}
	
	if((ret=NDK_ScanInit())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	//case2:异常参数测试
	if((ret=NDK_ScanSet(CAMERA_SETTYPE_IMAGE-1, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScanSet(CAMERA_SETTYPE_GAMMA+1, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScanSet(CAMERA_SETTYPE_IMAGE, -1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_ScanSet(CAMERA_SETTYPE_IMAGE, 2))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//待续
	
	//case3:摄像头显示CAMERA_SETTYPE_IMAGE分别为0和1的时候，扫描时摄像头应相应的会打开或者关闭
	if((ret=NDK_ScanSet(CAMERA_SETTYPE_IMAGE, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=lib_doscan(0, MAXTIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(cls_show_msg("扫描过程中摄像头处于关闭状态,是[ENTER],否则[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI);
		RETURN;
	}

	if((ret=NDK_ScanSet(CAMERA_SETTYPE_IMAGE, 1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=lib_doscan(1, MAXTIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScanClose())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:摄像头增益CAMERA_SETTYPE_SATURATION分别为XX和XX的时候，扫描时摄像头应相应的会...

	//case5:摄像头曝光CAMERA_SETTYPE_SHARPNESS分别为XX和XX的时候，扫描时摄像头应相应的会...

	//case6:摄像头帧率CAMERA_SETTYPE_GAMMA分别为0和1的时候，扫描时摄像头应相应的会...
	
	//结束测试
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_ScanClose();
	return;
	
}
