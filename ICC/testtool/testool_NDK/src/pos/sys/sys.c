/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: sys模块总入口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

#define	TESTMODULE	"sys模块"

/****************************************************************
* function name 	 	: sys
* functional description 	: sys模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	      linwei        20130121	created
*				  	
*****************************************************************/
void sys(PF *list, unsigned char id[])
{
#if STABAR_ENABLE
	NDK_ScrStatusbar(STATUSBAR_DISP_ALL);
#endif
	cls_show_msg1(1, "%s测试...", TESTMODULE);
	process(list, id);
#if STABAR_ENABLE	
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif
	return;
}

