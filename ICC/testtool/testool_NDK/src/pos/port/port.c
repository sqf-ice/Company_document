/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 串口模块
* file name		: port.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 通讯模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"串口模块"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: port
* functional description 	: 通讯模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  						chensl		   20130105	 		created
*****************************************************************/
void port(PF* list, unsigned char id[])
{
	//以下为函数体
	if(auto_flag==1)
	{
		g_UCID = 0;
		send_result("%s不支持自动测试，请使用手动测试验证", TESTMODULE);
		return;
	}
	cls_show_msg1(1, "%s测试...", TESTMODULE);
	process(list, id);
	
	//模块测试结束
	return;
}

