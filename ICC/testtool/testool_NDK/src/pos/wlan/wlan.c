/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: WLAN模块
* file name		: 
* Author 			: chensj
* version			: 
* DATE			: 20130513
* directory 		: 
* description		: WLAN模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	: 程序员参考手册
*
************************************************************************
* log			: 
*  Revision 1.0  2011.09.11 bcat
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"WLAN模块"
#define MAXWAITTIME 30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
extern void wlan13(void);

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: Wlan
* functional description 	: WLAN模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks	
*			  	 	   
*****************************************************************/
void wlan(PF* list, unsigned char id[])
{
	/*private & local definition*/
	int ret=-1;
	PF tmplist[] = {wlan13, NULL};
	uchar tmpid[] = {13, 0}; 
	
	/*process body*/
	if(cls_show_msg1(MAXWAITTIME, "%s测试...请确保打开所有AP...ESC退出本模块测试", TESTMODULE)==ESC)//时间长些,以保证用户有时间操作
		return;
	
	switch(conf_conn_WLAN())
	{
	case SUCC:
		break;
	case FAIL:
		cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
		return;
		break;
//	case QUIT://用户放弃
	default:
		return;
		break;
	}
	
	//执行测试用例
#if STABAR_ENABLE
	if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME|STATUSBAR_DISP_WIFI))!=NDK_OK)
	{
		cls_show_msg("状态栏显示失败!(%d)", ret);
		return;
	}
#endif
	if(linksetting.WLANEncMode!=WIFI_NET_SEC_WPA_EAP)
		process(list, id);
	else
		process(tmplist, tmpid);
		
#if STABAR_ENABLE	
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif
	return;
}

