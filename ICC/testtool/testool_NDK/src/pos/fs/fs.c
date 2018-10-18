/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name		: 
* Author 			: chenfm
* version			: 
* DATE			: 20121217
* directory 		: 
* description		: fs模块总入口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

#define	TESTMODULE	"fs模块"

/****************************************************************
* function name 	 	: fs
* functional description 	: fs模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	  chenfm         20121217	created
*				  	
*****************************************************************/
void fs(PF *list, unsigned char id[])
{
	cls_show_msg1(1, "%s测试...", TESTMODULE); 
	process(list, id);
	return;
}

