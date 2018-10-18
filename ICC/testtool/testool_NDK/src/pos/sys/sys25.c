/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys25.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 测试NDK_SysGetPowerVolRange接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"
//#if defined ME30||defined ME30MH
//#define VOL_LOWEST  1650 //30S还是保持和其他产品不一样的1650
//#else
//#define VOL_LOWEST  1750//20150116 范益龙回复 开发硬件设计修改30不再使用1650，和其它产品一样统一成1750
//#endif
//#define VOL_FULL    2100
#if ANDIROD_ENABLE
#define VOL_LOWEST  380//20160222 修改
#define VOL_FULL    469
#elif defined ME30MH||defined ME30GX||defined ME32||defined ME32GX||defined ME65
#define VOL_LOWEST  3300
#define VOL_FULL    4200
#else
#define VOL_LOWEST  1750//20150116 范益龙回复 开发硬件设计修改30不再使用1650，和其它产品一样统一成1750
#define VOL_FULL    2100
#endif

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysGetPowerVolRange"

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
*			  		     jiangym		20140530	created
*****************************************************************/
void sys25(void)
{
	if(auto_flag==2)
		return;
	//以下为局部变量
	int nRet = 0;
	uint punmax = 0, punmin = 0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//case1:异常测试
	if((nRet = NDK_SysGetPowerVolRange(NULL,&punmin))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	if((nRet = NDK_SysGetPowerVolRange(&punmax,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}

	//case2:正常调用
	if((nRet = NDK_SysGetPowerVolRange(&punmax,&punmin)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	if(((punmin != VOL_LOWEST)&&(punmin != 1650)) || (punmax != VOL_FULL))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, punmax, punmin);
		RETURN;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

