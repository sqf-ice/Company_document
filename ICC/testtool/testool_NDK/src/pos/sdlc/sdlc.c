/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: 
* Author 			: chenjings
* version			: 
* DATE			: 20130106
* directory 		: 
* description		: sdlc模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"同步MODEM"
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 	: sdlc
* functional description 	: sdlc模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	chenjings      20130106	created
*****************************************************************/
void sdlc(PF* list, unsigned char id[])//入口函数名就是主文件名,也是模块名
{
	//以下为局部变量
	int ret=-1;	
	
	//以下为函数体
	cls_show_msg1(5,"%s测试,请打开后台并接连自检头,任意键继续...", TESTMODULE);
 	if(auto_flag==1)//自动测试使能导入配置文件的标志后调用配置函数就会使用配置文件里的内容
 		g_conf_flag=1;
	else
		g_conf_flag=0;
	if(conf_link(SYNC)!=SUCC)
	{
		cls_show_msg("设置链接类型为同步猫失败!");
		return;
	}
	//调用process进入模块单元测试
#if STABAR_ENABLE
	if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME|STATUSBAR_DISP_MODEM))!=NDK_OK)
	{
		cls_show_msg("状态栏显示失败!(%d)", ret);
		return;
	}
#endif
	process(list, id);
#if STABAR_ENABLE	
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif

	//模块测试结束
	return;
}

