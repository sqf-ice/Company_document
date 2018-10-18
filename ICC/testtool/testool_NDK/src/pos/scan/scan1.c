/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 扫描
* file name			: 1.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_ScanInit是否可以进行扫描头初始化
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_ScanInit"
#define    MAXTIMEOUT  10

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: scan1
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*										jiangym			20130917		created
*
*****************************************************************/
void scan1(void)
{
	/*private & local definition*/
	int ret = 0, plen = 0;
	char pvalue[500] = {0};
		
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:初始化应该成功,初始化包括管脚电平初始化，出厂默认设置。 
	if((ret=NDK_ScanInit())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_printf("请进行扫描");
	if((ret=NDK_ScanDoScan(MAXTIMEOUT,pvalue,&plen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//出厂默认设置(照明对焦为闪烁、灵敏度为11))
	if(cls_show_msg("扫描过程中照明对焦灯为闪烁,[Enter]通过,其他失败")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//结束测试
	send_result("%s测试通过", TESTAPI);	
ERR:
#if SCAN_SOFT_ENABLE	
	NDK_ScanExit();
#endif
	return;
}

