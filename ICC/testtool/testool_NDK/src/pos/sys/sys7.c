/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys7.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_SysSetBeepVol,NDK_SysGetBeepVol接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysSetBeepVol,NDK_SysGetBeepVol"

#define MAXVOL 5
#define	MAXWAITTIME 30

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
void sys7(void)
{
	if(auto_flag==1)
		send_result("%s自动测试不能作为最终测试结果，请结合手动测试验证", TESTAPI);

	//以下为局部变量
	int i,ret = -1,nKeyin;
	unsigned int unVolumn=0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//case1: 异常测试，错误参数
	if((ret=NDK_SysSetBeepVol(MAXVOL*10)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysGetBeepVol(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3:设置音量为4,再设置音量为1,获取音量看是否为1 20140603 added by jiangym
	if((ret=NDK_SysSetBeepVol(4)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysSetBeepVol(1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysGetBeepVol(&unVolumn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(unVolumn != 1)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2: 设置音量为0到5
	for (i=0; i<=MAXVOL; i++)
	{
		if((ret=NDK_SysSetBeepVol(i)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret=NDK_SysGetBeepVol(&unVolumn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if(unVolumn != i)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		NDK_SysBeep();
		NDK_SysMsDelay(200);
	}

	nKeyin = cls_show_msg1(MAXWAITTIME, "听到响声从小到大,[确认]通过,[其它]失败");
	if (nKeyin==ENTER || nKeyin==0)
		send_result("%s测试通过", TESTAPI);
	else
		send_result("%s测试失败", TESTAPI);
	return;
}

