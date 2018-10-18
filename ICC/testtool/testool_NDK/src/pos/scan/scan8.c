/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 扫描
* file name			: 8.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试NDK_ScanExit是否可以正常退出扫描
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_ScanExit"
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
*					 	  sull			   20180409		    created
*****************************************************************/
void scan8(void)
{
	/*private & local definition*/
	int ret = 0, plen = 0;
	char pvalue[500] = {0};
		
	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	#if 0//开发没有明确说明此函数使用情况故暂时不测此点
	//case1:没有初始化，退出应该失败
	if((ret = NDK_ScanExit()) == NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	#endif

	//case2:退出后，应不能正常扫描
	if((ret = NDK_ScanInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_ScanExit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_printf("请在10秒内进行扫描");
	if((ret = NDK_ScanDoScan(MAXTIMEOUT,pvalue,&plen)) != NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:退出后，再进行初始化，应能成功扫描
	if((ret = NDK_ScanInit()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	plen=0;
	memset(pvalue,0,sizeof(pvalue));
	cls_printf("请在20秒内进行扫描");
	if((ret=NDK_ScanDoScan(MAXTIMEOUT*2,pvalue,&plen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(cls_show_msg("扫描到的长度为:%d的内容:%s,确认键通过,其它键失败", plen, pvalue)!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//结束测试
	send_result("%s测试通过", TESTAPI);	
	
ERR:	
	NDK_ScanExit();
	return;
}


