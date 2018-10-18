/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module						: PPP通讯模块
* file name					: 
* Author 						: chensj
* version						: 
* DATE							: 
* directory 				: 
* description				: PPP通讯模块总入口,本模块的公共函数与公共定义也可以放于此处
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
#define	TESTMODULE	"PPP模块"
#define	MAXTIMEOUT	60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
int IsPPPTimeout(int state, int *err)
{
	/*private & local definition*/
	time_t diff = 0, oldtime = time(NULL);
	EM_PPP_STATUS emStatus=0;
	char szIpaddr[20]={0};
	
	/*process body*/
	//当state==PHASE_NETWORK时,需进一步看申请动态IP是否成功(可能一次取不到,多取几次)   
	//getLocalAddr()==0表示未取到本地地址(即未申请成功)  
	if(state==PPP_STATUS_CONNECTED )
		return (lib_GetLocalAddr(szIpaddr, NULL, NULL, NULL)!=SUCC?TRUE:FALSE);
	else
	{
		while(1)
		{
			if(NDK_PppCheck(&emStatus, err)!=NDK_OK)
				return TRUE;
			if(emStatus!=state)
				break;
			//超时还一直处于某个状态态,说明FSM出问题了!
			if((diff=time(NULL)-oldtime)>MAXTIMEOUT)//if(ReadStopwatch()>MAXTIMEOUT)
				return TRUE;
			show_stopwatch(ENABLE, diff);
			//NDK_SysDelay(10);// 20140505linwl 将延时动作移动到show_stopwatch函数中
		}

		show_stopwatch(DISABLE, 0);
		return FALSE;
	}
}

/****************************************************************
* function name 	 				: ppp
* functional description 	: ppp模块入口主函数,可根据需要扩展
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						: author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void ppp(PF* list, unsigned char id[])
{
	/*private & local definition*/
	if(auto_flag==2) 
	{
		g_UCID = 0;
		send_result("%s可自动测试，请使用自动测试验证", TESTMODULE);
		return;
	}
	/*process body*/
#if STABAR_ENABLE
	NDK_ScrStatusbar(STATUSBAR_DISP_ALL);
#endif
	cls_show_msg1(1, "%s测试...", TESTMODULE);
	if(conf_conn_PPP()!=SUCC)
		return;
	//调用process进入模块测试前,要先对链路的类型linktype赋值
	process(list, id);
#if STABAR_ENABLE	
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif	
	//模块测试结束
	return;
}

