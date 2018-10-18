/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys2.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_SysInit接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysInit"//int NDK_SysInit(void):系统初始化(暂未使用) ,fd 操作成功,其它EM_NDK_ERR 操作失败 

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
*			  		      linwei	        20130121	 	created
*****************************************************************/
void sys2(void)
{
	//case1:函数未启用
	send_result("%s暂未使用", TESTAPI);
	return;
}

