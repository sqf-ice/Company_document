/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys17.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_SysGetConfigInfo接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysGetConfigInfo"

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
*			  		        linwei	           20130121	 	created
*****************************************************************/
void sys17(void)
{
	if(auto_flag==2)
		return;
	//以下为局部变量
	int nvalue,ret= -1;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
#if !K21_ENABLE //K21只支持SYS_CONFIG_LANGUAGE
	//case1:异常测试，错误参数     
	if((ret=NDK_SysGetConfigInfo(SYS_CONFIG_SLEEP_ENABLE,NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysGetConfigInfo(16,&nvalue)) != NDK_ERR_PARA)
	{//16大于EM_SYS_CONFIG枚举最大
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:读取系统配置信息
#if SLEEP_ENABLE	
	//case2.1:读取休眠使能
	if((ret=NDK_SysGetConfigInfo(SYS_CONFIG_SLEEP_ENABLE, &nvalue))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("休眠使能:%s,请判断是否正确" ,nvalue==0?"禁止":(nvalue == 1?"启用":"未知"));
	
	//case2.2:读取进入休眠时间前待机时间
	if((ret=NDK_SysGetConfigInfo(SYS_CONFIG_SLEEP_TIME, &nvalue))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("进入休眠时间前待机时间:%d,请判断是否正确",nvalue);
	
	//case2.3:读取休眠模式
	if((ret=NDK_SysGetConfigInfo(SYS_CONFIG_SLEEP_MODE, &nvalue))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("休眠模式:%s,请判断是否正确" ,nvalue==1?"浅休眠":(nvalue == 2?"深休眠":"未知"));
	
#endif	
#endif

	//case2.4:读取获取系统语言
	if((ret=NDK_SysGetConfigInfo(SYS_CONFIG_LANGUAGE, &nvalue))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#if !K21_ENABLE//K21平台底层0代表英文 1代表中文(与其他平台相反)20140515 linwl
	send_result("获取系统语言:%s,请判断是否正确",nvalue==0?"中文":(nvalue == 1?"english":"未知"));
#else
	send_result("获取系统语言:%s,请判断是否正确",nvalue==0?"english":(nvalue == 1?"中文":"未知"));
#endif
	

#if !K21_ENABLE
	//case2.5:读取开机自动运行主控程序
	if((ret=NDK_SysGetConfigInfo(SYS_CONFIG_APP_AUTORUN, &nvalue))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	send_result("开机自动运行主控程序:%s,请判断是否正确",nvalue==0?"禁用":(nvalue == 1?"启用":"未知"));
#endif

	//测试结束
	send_result("%s测试通过(请修改设置参数重测本例)", TESTAPI);
	return;
}

