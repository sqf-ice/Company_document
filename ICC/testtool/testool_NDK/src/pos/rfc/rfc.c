/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name		: 
* Author 			: chenfm
* version			: 
* DATE			: 20130107
* directory 		: 
* description		: rfc模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"rfc模块"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: rfc
* functional description 	:rfc模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	chenfm      20130107	created
*				  	
*****************************************************************/
void rfc(PF *list, unsigned char id[])//入口函数名就是主文件名,也是模块名
{
	//以下为局部变量

	//以下为函数体
	if(auto_flag==1)//不支持自动测试
	{
		g_UCID = 0;
		send_result("%s不支持自动测试，请使用手动测试验证", TESTMODULE);
		return;
	}
	cls_show_msg1(1, "%s测试...", TESTMODULE);

	if(NDK_RfidInit(NULL)!=NDK_OK)
	{
		cls_show_msg("射频接口器件初始化失败,任意键退出...");
		return;
	}
	if(NDK_RfidOpenRf()!=NDK_OK)
	{
		cls_show_msg("开射频输出失败,任意键退出...");
		return;
	}

	process(list, id);

	NDK_RfidPiccDeactivate(0);
	if(NDK_RfidCloseRf()!=NDK_OK)
	{
		cls_show_msg("关射频输出失败,任意键退出...");
		return;
	}
	return;
}

