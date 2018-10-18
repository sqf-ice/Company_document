/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys100.c
* Author 			: linwl
* version			: 
* DATE				: 20140621
* directory 		: 
* description		: “弹窗和屏幕显示交叉”测试用例
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"弹窗和屏幕显示交叉"

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
*			  		        linwl	           20140621	 	created
*****************************************************************/
void sys100(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//以下为局部变量
	int nCode = 0;
	
	//以下为函数体
	NDK_ScrAutoUpdate(0,NULL);
	NDK_ScrStatusbar(STATUSBAR_DISP_ALL);
	while (1)
	{
		cls_printf("国家国家国家国国国家国家国家国国国家国家国家国国家国家国家国国家国家国家国国家国家国家国国家国家国家国国家国家国家国国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家国家");
		NDK_SysMsDelay(10);
		cls_printf("我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我我");
		NDK_SysMsDelay(10);
		if(NDK_KbHit(&nCode)==NDK_OK)
		{
			if(nCode==ESC)
				return;
		}
	}
}

