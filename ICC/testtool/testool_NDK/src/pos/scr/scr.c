/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示模块
* file name		: 
* Author 			: zhuq
* version			: 
* DATE			: 20121224
* directory 		: 
* description		: 显示模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"显示模块"
#define TESTBGPIC "test_bg.png"	//根据开发要求将测试文件放到main文件所在路径，这样可以一起打包下载 20160513

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: scr
* functional description 	: 显示模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        zhuq         20121224  	   created
*****************************************************************/
void scr(PF* list, unsigned char id[])
{
	/*private & local definition*/
	if(auto_flag==1)//液显模块支持自动测试，但需要手动测试辅助
	{
		g_UCID = 0;
		send_result("%s自动测试不能作为最终测试结果，请结合手动测试验证", TESTMODULE);
		//return;
	}
	
#if !K21_ENABLE
	int ret = -1;
#endif
#if STABAR_ENABLE
	int nkey = 0;
	/*process body*/
	nkey=cls_show_msg1(5,"是否要打开状态栏:[确认]:是,[其它]:否", TESTMODULE);	
	NDK_ScrClrs();
	NDK_ScrRefresh();
	if(nkey == ENTER||nkey == 0)
		NDK_ScrStatusbar(STATUSBAR_DISP_ALL);
#endif	

#if !K21_ENABLE
	//20140611 linwl added 补充背景图片验证有背景图片情况下液显函数调用不会引起背景图片显示异常
	if(cls_show_msg1(5, "是否设置背景图片:是[确认],否[其它]")==ENTER)
	{
		cls_show_msg("请先下载%s背景图片后按任意键继续",TESTBGPIC);
		if((ret=NDK_ScrSetbgPic(TESTBGPIC))!=NDK_OK)
		{
			g_UCID = 0;
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTMODULE, ret);
			return;
		}
	}
#endif

	cls_show_msg1(1, "%s测试...", TESTMODULE);
	process(list, id);
	
#if !K21_ENABLE
	NDK_ScrClrbgPic();
#endif

#if STABAR_ENABLE
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif
	return;
}

