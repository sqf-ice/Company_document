/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: dns模块
* file name		: dns.c
* Author 			: linwl
* version			: 
* DATE				: 20150705
* directory 	: 
* description	: dns模块总入口,本模块的公共函数与公共定义也可以放于此处
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
#define	TESTMODULE	"dns模块"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: dns
* functional description 	: 以太网模块入口主函数
* input parameter	 	:
* output parameter	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  						linwl		   20150705	 		created
*****************************************************************/
void dns(PF* list, unsigned char id[])
{
#if STABAR_ENABLE
	int ret=-1;
#endif
	float testtime = 0;
#if K21_ENABLE
	char str[32] = {0};
#endif
	//以下为函数体
	cls_show_msg1(1, "%s测试...", TESTMODULE);
	if(conf_conn_DNS()!=SUCC)
	{
		cls_show_msg("line %d:网络配置失败!!!\n任意键退出配置...", __LINE__);
		return;
	}
	
#if STABAR_ENABLE
	if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME/*|STATUSBAR_DISP_ETH*/))!=NDK_OK)
	{
		cls_show_msg("状态栏显示失败!(%d)", ret);
		return;
	}
#endif

	lib_StartStopwatch();
	process(list, id);
	testtime = lib_ReadStopwatch();
	
#if STABAR_ENABLE	
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif

#if K21_ENABLE
	memset(str,0,sizeof(str));
	ftos(testtime,str);
	send_result("%s测试时间为%ss秒", TESTMODULE,str);
#else
	send_result("%s测试时间为%f秒", TESTMODULE,testtime);
#endif
	
	//模块测试结束
	return;
}

