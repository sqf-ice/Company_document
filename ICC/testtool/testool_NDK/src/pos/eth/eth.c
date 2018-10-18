/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 以太网模块
* file name		: eth.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: 以太网模块总入口,本模块的公共函数与公共定义也可以放于此处
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
#define	TESTMODULE	"以太网模块"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: eth
* functional description 	: 以太网模块入口主函数
* input parameter	 	:
* output parameter	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  						chensl		   20130105	 		created
*****************************************************************/
void eth(PF* list, unsigned char id[])
{
	int ret=-1;
	
	//以下为函数体
	cls_show_msg1(1, "%s测试...", TESTMODULE);

	switch(conf_conn_ETH(FALSE))
	{
	case SUCC:
		break;
	case FAIL:
		cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
	default:
		return;
		break;
	}
	
#if STABAR_ENABLE
	if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME/*|STATUSBAR_DISP_ETH*/))!=NDK_OK)
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

