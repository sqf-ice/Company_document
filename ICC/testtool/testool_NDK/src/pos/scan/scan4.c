/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 扫描
* file name			: 4.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_ScanInit摄像头初始化和NDK_ScanClose摄像头关闭功能(非阻塞式)
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_ScanInit、NDK_ScanClose"
#define 	MAXTIME     60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: scan4
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*							zhengry			20160526		created
*
*****************************************************************/
void scan4(void)
{
	/*private & local definition*/
	int ret=0, plen = 0;
	char pvalue[50] = {0};

	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:未初始化摄像头，应返回摄像头未初始化NDK_ERR_CAMERA_INIT
	if((ret=NDK_ScanClose())!=NDK_ERR_CAMERA_INIT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_ScanDoScan(20, pvalue, &plen))!=NDK_ERR_CAMERA_INIT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		//return; ret=0
	}

	//case2:正常测试:不设置任何东西
	if((ret=NDK_ScanInit())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=lib_doscan(1, MAXTIME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:关闭摄像头后，应无法扫描内容
	if((ret=NDK_ScanClose())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScanStart()) != NDK_ERR_CAMERA_INIT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_ScanDoScan(20, pvalue, &plen))!=NDK_ERR_CAMERA_INIT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//NDK_ScanInit: NDK_ERR_CAMERA_LOAD_DRV 驱动加载失败 
	//NDK_ScanInit: NDK_ERR_CAMERA_ENUMERATE  驱动枚举失败 
	//NDK_ScanInit和NDK_ScanClose: NDK_ERR_CAMERA_SYSCMD 系统命令执行出错
	
	//结束测试
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_ScanClose();
	return;
}
