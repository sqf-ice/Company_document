/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: ssl模块
* file name		: 
* Author 			: chenfm
* version			: 
* DATE			: 20120905
* directory 		: 
* description		: ssl模块总入口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

#define	TESTMODULE	"ssl模块"

/****************************************************************
* function name 	 	: ssl
* functional description 	: ssl模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	  chenfm         20120905	created
*				  	
*****************************************************************/
void ssl(PF *list, unsigned char id[])
{
	cls_show_msg1(1, "%s测试...", TESTMODULE); 

	switch(conf_conn_sock())//port=1111,服务器密码123456
	{
  	case SUCC:
		break;
  	case FAIL:
		cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
  	case NDK_ERR_QUIT:
		//用户放弃
  	default:
		return;
		break;
	}

	process(list, id);
	return;
}

