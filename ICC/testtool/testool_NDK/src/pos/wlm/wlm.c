/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module						: wlm无线通讯模块
* file name					: 
* Author 						: chensj
* version						: 
* DATE						: 
* directory 					: 
* description					: wlm无线通讯模块总入口
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
#define	TESTMODULE	"WLM模块"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: wlm
* functional description 	: wlm模块入口主函数,可根据需要扩展
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						: author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void wlm(PF* list, unsigned char id[])
{
	/*private & local definition*/
#if STABAR_ENABLE
	int ret=-1;
#endif

	/*process body*/
#if STABAR_ENABLE
	if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_ALL))!=NDK_OK)
	{
		cls_show_msg("状态栏显示失败!(%d)", ret);
		return;
	}
#endif
	cls_printf("%s测试...", TESTMODULE);

	if(conf_conn_WLM(FALSE)!=SUCC)//==QUIT)
		return;

	cls_printf("初始化无线MODEM中...\n请稍候...\n");

//	NDK_WlShowSignalQuality(-1,-1);//进入无线(AT命令)测试前,先关信号显示(关闭控制台,避免两个进程冲突)

	process(list, id);
	
//	NDK_WlShowSignalQuality(0, 0);//进入无线(AT命令)测试前,先关信号显示(关闭控制台,避免两个进程冲突)
#if STABAR_ENABLE
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif

//模块测试结束
	return;
}

