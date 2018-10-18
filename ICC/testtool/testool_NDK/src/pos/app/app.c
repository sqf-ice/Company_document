/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: 
* Author 			: daitt
* version			: 
* DATE			: 20130228
* directory 		: 
* description		: 
* related document	: NDK.chm
*
************************************************************************/
/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 应用管理模块
* file name		: 
* Author 			: daitao
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 应用管理模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"app模块"

/*------------global variables definition-----------------------*/
#define MAXWAITTIME 30

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr
* functional description 	: 显示模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        daitao         20121224  	   created
*****************************************************************/
void app(PF *list, unsigned char id[])
{
	/*private & local definition*/

	/*process body*/
#if !K21_ENABLE
	if(cls_show_msg1(MAXWAITTIME, "%s测试...请在POS/appfs/download目录下载测试NLD包,ESC退出本模块测试", TESTMODULE)==ESC)
#else
	if(cls_show_msg1(MAXWAITTIME, "%s测试...请在POS/appfs目录下载测试NLD包,ESC退出本模块测试", TESTMODULE)==ESC)
#endif
		return;
	process(list, id);
	return;
}

