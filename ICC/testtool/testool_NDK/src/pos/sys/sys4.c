/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys4.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_SysReboot接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysReboot"

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
void sys4(void)
{
	if(auto_flag==2)
		return;
	//case1:功能测试
#if !K21_ENABLE //根据会议讨论结果 K21硬件上需要插着电源线才能重启
	send_result("请在测试主菜单按[<-]进行%s验证", TESTAPI);
#else
	send_result("请插上电源线后,在测试主菜单按[退格键]进行%s验证", TESTAPI);
#endif
	return;
}

