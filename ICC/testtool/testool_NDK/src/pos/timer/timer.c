/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: timer模块
* file name		: 
* Author 			: huangbs
* version			: 
* DATE			: 20120830
* directory 		: 
* description		: timer模块总入口
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

#define	TESTMODULE	"timer模块"

/****************************************************************
* function name 	 	: timer
* functional description 	: timer模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	  huangbs         20120905	created
*				  	
*****************************************************************/
void timer(PF *list, unsigned char id[])
{
	if(auto_flag==1)//不支持自动测试
	{
		g_UCID = 0;
		send_result("%s不支持自动测试，请使用手动测试验证", TESTMODULE);
		return;
	}
	cls_show_msg1(1, "%s测试...", TESTMODULE); 
	process(list, id);
	return;
}

