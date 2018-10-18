/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: socket模块
* file name		: sckt.c
* Author 			: chensl
* version			: 
* DATE				: 20130105
* directory 	: 
* description	: socket模块总入口,本模块的公共函数与公共定义也可以放于此处
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
#define	TESTMODULE	"socket模块"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: sckt
* functional description 	: socket模块入口主函数
* input parameter	 	:
* output parameter	: 
* return value		 	:
* history 		 		: 
*****************************************************************/
void sckt(PF* list, unsigned char id[])
{
	//以下为函数体
	cls_show_msg1(1, "%s测试...", TESTMODULE);
	
	//测试前置
	switch(conf_conn_sock())
	{
	case SUCC:
		break;
	case FAIL:
		cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
	case NDK_ERR_QUIT://用户放弃
	default:
		return;
		break;
	}
	
	process(list, id);
	
	//模块测试结束
	return;
}

