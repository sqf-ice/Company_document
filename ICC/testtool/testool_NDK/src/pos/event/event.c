/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: event模块
* file name			: event.c
* Author 			: linwl
* version			: 
* DATE				: 20160912
* directory 		: 
* description		: event模块总入口
* related document	: 高端平台统一API动态链接库编程手册V2.00.doc
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

#define	TESTMODULE	"event模块"

/****************************************************************
* function name 	 	: event
* functional description 	: event模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	      linwl        20160912	created
*				  	
*****************************************************************/
void event(PF *list, unsigned char id[])
{
	cls_show_msg1(1, "%s测试...", TESTMODULE);
	process(list, id);
	
	return;
}
