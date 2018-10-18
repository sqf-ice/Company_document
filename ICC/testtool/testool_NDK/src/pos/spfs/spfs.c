/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: spfs模块
* file name		: 
* Author 			: linying
* version			: 
* DATE			: 20180828
* directory 		: 
* description		:高端产品 fs模块总入口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

#define	TESTMODULE	"spfs模块"

/****************************************************************
* function name 	 	: spfs
* functional description 	:高端产品K21端fs模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	        linying       20180828	created
*				  	
*****************************************************************/
void spfs(PF *list, unsigned char id[])
{
	cls_show_msg1(1, "%s测试...", TESTMODULE); 
	process(list, id);
	return;
}

