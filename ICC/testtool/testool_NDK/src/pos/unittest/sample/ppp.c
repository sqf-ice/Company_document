/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: PPP通讯模块
* file name		: 
* Author 			: jiangwx
* version			: V1.0
* DATE			: 200601026
* directory 		: .\SRC\Net\
* description		: PPP通讯模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	: 程序员参考手册
*
************************************************************************
* log			: 
* Revision 1.0  200601026 14:10 jiangwx
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
int IsPPPTimeout(int ph, int state)
{
	/*private & local definition*/
	time_t diff = 0, oldtime = time(NULL);//StartStopwatch();
	
	/*process body*/
	//当state==PHASE_NETWORK时,需进一步看申请动态IP是否成功(可能一次取不到,多取几次)   
	//getLocalAddr()==0表示未取到本地地址(即未申请成功)  
	if(state==PHASE_NETWORK)
		return (GetLocalAddr()==0?TRUE:FALSE);
	else
	{
		while(GetPPPphase(ph)==state)
		{
			//超时还一直处于某个状态态,说明FSM出问题了!
			if((diff=time(NULL)-oldtime)>MAXTIMEOUT)//if(ReadStopwatch()>MAXTIMEOUT)
				return TRUE;
			show_stopwatch(ENABLE, diff);
			delay(10);//加延时是为了降低连续读状态对芯片的压力
		}

		show_stopwatch(DISABLE, 0);
		return FALSE;	
	}
}

/****************************************************************
* function name 	 	: ppp
* functional description 	: ppp模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  			  jiangwx		   20061016 		created
*					linwl				20080923		modify
*****************************************************************/
void ppp(PF* list, unsigned char id[])
{
	/*private & local definition*/
	
	/*process body*/
	cls_show_msg1(1, "%s测试...", TESTMODULE);

	//调用process进入模块测试前,要先对链路的类型linktype赋值
	if(conf_conn_PPP()!=SUCC)
		return;
	process(list, id);
}

