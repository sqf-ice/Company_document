/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module						: prnt打印模块
* file name					: 
* Author 						: chensj
* version						: 
* DATE							: 
* directory 				: 
* description				: prnt打印模块总入口
* related document	: DNKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"prnt模块"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				:prnt
* functional description 	: prnt模块入口主函数,可根据需要扩展
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						: author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt(PF* list, unsigned char id[])
{
	/*private & local definition*/
	if(auto_flag==1)//不支持自动测试
	{
		g_UCID = 0;
		send_result("%s不支持自动测试，请使用手动测试验证", TESTMODULE);
		return;
	}
	/*process body*/
	cls_show_msg1(1, "%s测试...", TESTMODULE);

	//调用process进入模块测试前,要先对链路的类型linktype赋值
	conf_prn();
	lib_initprn(g_PrnSwitch);//NDK_PrnInit(g_PrnSwitch);
	process(list, id);
	
	//模块测试结束
	return;
}

