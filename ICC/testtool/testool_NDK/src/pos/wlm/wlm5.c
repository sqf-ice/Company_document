/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线模块
* file name		: wlm5.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_WlCloseRF接口功能
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
#define TESTAPI	"NDK_WlCloseRF"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlm2(void)
* functional description 	: 关闭射频 
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void wlm5(void)
{
	//以下为局部变量
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	//...
	
	//case1:测试能否正常关闭射频
	//...
	
	//测试结束	
	return;
}

