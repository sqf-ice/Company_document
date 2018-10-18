/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 扫描模块
* file name			: 
* Author 			: jiangym
* version			: 
* DATE				: 20130911
* directory 			: 
* description			: 扫描模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"扫描模块"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: scan
* functional description 	: 扫描模块入口主函数,可根据需要扩展
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        jiangym           20130911       created
*****************************************************************/
void scan(PF* list, unsigned char id[])
{
	/*private & local definition*/

	/*process body*/
	if(auto_flag==1)
	{
		g_UCID = 0;
		send_result("%s不支持自动测试，请使用手动测试验证", TESTMODULE);
		return;
	}
	
	cls_show_msg1(1, "%s测试...", TESTMODULE);	
	process(list, id);
	
	return;
}

