/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线模块
* file name		: wlm6.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_WlModemReset能否复位无线模块，NDK_WlModemClose能否关闭无线模块
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if !(CPU5810X_ENABLE||defined ME50NX||defined ME68)
#define TESTAPI	"NDK_WlModemReset,NDK_WlModemClose"	
#else
#define TESTAPI	"NDK_WlModemReset"	
#endif

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlm6(void)
* functional description 	: 关闭无线模块
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void wlm6(void)
{
	//以下为局部变量
	int nRet = 0,nSq=0;
	LinkType type = getlinktype();
	
	//以下为函数体
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	NDK_WlModemClose();

	//case1:测试无线复位后能够正常获取信号
	if((nRet = NDK_WlModemReset())!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		GOTOERR;
	}
	NDK_SysDelay(150/*100*/);//复位时间,由原来的100修改成150,原因 CDMA模块复位后需要约11秒才能获取到信号,模块复位较慢 20140212 zhengjd
	if((nRet = NDK_WlModemGetSQ(&nSq))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		GOTOERR;
	}
	if(((type==GPRS||type==CDMA) && (nSq>SQ_2G_MAX||nSq<SQ_2G_MIN))
	||(type==TD && (nSq>SQ_3G_MAX||nSq<SQ_3G_MIN)))
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nSq);
		GOTOERR;
	}
	cls_show_msg1(3, "获取到的信号数:(%d)",nSq);

#if !(CPU5810X_ENABLE||defined ME50NX||defined ME68)
	//case2:测试关闭无线后获取信号应该失败
	if((nRet = NDK_WlModemClose())!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		GOTOERR;
	}
	if((nRet = NDK_WlModemGetSQ(&nSq))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		GOTOERR;
	}
#endif

	//case3:测试重新复位后能够正常获取信号
	if((nRet = NDK_WlModemReset())!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		GOTOERR;
	}
	NDK_SysDelay(150/*100*/);//复位时间,由原来的100修改成150,原因 CDMA模块复位后需要约11秒才能获取到信号,模块复位较慢 20140212 zhengjd
	if((nRet = NDK_WlModemGetSQ(&nSq))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		GOTOERR;
	}
	if(((type==GPRS||type==CDMA) && (nSq>SQ_2G_MAX||nSq<SQ_2G_MIN))
	||(type==TD && (nSq>SQ_3G_MAX||nSq<SQ_3G_MIN)))
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nSq);
		goto ERR;
	}
	cls_show_msg1(3, "获取到的信号数:(%d)",nSq);
	
	//测试结束	
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_WlModemReset();
	return;
}
