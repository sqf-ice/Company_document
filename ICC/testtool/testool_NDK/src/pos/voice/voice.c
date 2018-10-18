/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 音频模块
* file name			: 
* Author 			: jiangym
* version			: 
* DATE				: 20131028
* directory 			: 
* description			: 音频模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"音频模块"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: voice
* functional description 	: 扫描模块入口主函数,可根据需要扩展
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	        jiangym           20131028       created
*****************************************************************/
void voice(PF* list, unsigned char id[])
{
	/*private & local definition*/

	/*process body*/
	if(auto_flag==1)//音频模块支持自动测试，但需要手动测试辅助
	{
		g_UCID = 0;
		send_result("%s自动测试不能作为最终测试结果，请结合手动测试验证", TESTMODULE);
		//return;
	}
	
	cls_show_msg1(1, "%s测试...", TESTMODULE);	
	process(list, id);
	
	return;
}

