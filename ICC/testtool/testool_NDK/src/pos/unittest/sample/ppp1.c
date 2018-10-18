/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: PPP通讯
* file name		: 1.c
* Author 			: jiangwx
* version			: V1.0
* DATE			: 20061026
* directory 		: .\SRC\net\
* description		: 测试pppOpen能否开辟一个新的PPP通道
* related document	: 程序员参考手册
*
************************************************************************
* log			: 
* Revision 1.0  2006.10.26 jiangwx
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  UCID  1
#define	TESTAPI		"pppOpen"
#define	MAXTIMEOUT	60

/*------------global variables definition-----------------------*/
static float fTimeElapsed = 0;
static int (*ppp_subcase)(void) = NULL;

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
extern int IsPPPTimeout(int ph, int state);

static int subcase_1(void);
static int subcase_2(void);
static int subcase_3(void);
static int subcase_4(void);

/*---------------functions definition---------------------------*/
//第1号subcase:测试创建新PPP通道-1	       
static int subcase_1(void)
{
	/*private & local definition*/
	int p = 0;
	
	/*process body*/
	//构造PPP测试前环境
	//注意:pppClose后会连物理链路也断开,每次调用pppClose后,再调用pppOpen都要重新LinkUp
	if(LinkUp()!=SUCC)
	{
		send_result("line %d:创建链路失败", __LINE__);
		return FAIL;
	}

	if ((p=pppOpen(-1))<0)                                                                                           	                                                                                                                                                        	
	{
		send_result("line %d:%s测试失败(err=%d)", __LINE__, TESTAPI, getPPPerrorcode(p));
		LinkDown();
		return FAIL;                                                                                                  	                                                                                                                                                        	
	}
	ppp_subcase = subcase_2;//设置下一节点
	return SUCC;
}

//测试创建新PPP通道0	          
static int subcase_2(void)
{
	/*private & local definition*/
	int p = 0;
	fTimeElapsed = 0;
	
	/*process body*/
	//构造PPP测试前环境
	//注意:pppClose后会连物理链路也断开,每次调用pppClose后,再调用pppOpen都要重新LinkUp
	if(LinkUp()!=SUCC)
	{
		send_result("line %d:创建链路失败", __LINE__);
		return FAIL;
	}

	StartStopwatch();
	if ((p=pppOpen(0))<0)                                                                                            	                                                                                                                                                        	
	{
		send_result("line %d:%s测试失败(err=%d)", __LINE__, TESTAPI, getPPPerrorcode(p));
		LinkDown();
		return FAIL;                                                                                          	                                                                                                                                                        	
	}
	fTimeElapsed = ReadStopwatch();
	ppp_subcase = subcase_3;//设置下一节点
	return SUCC;
}

//测试创建新PPP通道1024	      	          
static int subcase_3(void)
{
	/*private & local definition*/
	int p = 0;
	
	/*process body*/
	//构造PPP测试前环境
	//注意:pppClose后会连物理链路也断开,每次调用pppClose后,再调用pppOpen都要重新LinkUp
	if(LinkUp()!=SUCC)
	{
		send_result("line %d:创建链路失败", __LINE__);
		return FAIL;
	}
			
	if ((p=pppOpen(1024))<0)                                                                                         	                                                                                                                                                        	
	{
		send_result("line %d:%s测试失败(err=%d)", __LINE__, TESTAPI, getPPPerrorcode(p));
		LinkDown(); 
		return FAIL;                                                                                                  	                                                                                                                                                        	
	}	                                         
	ppp_subcase = subcase_4;//设置下一节点
	return SUCC;
}

//测试最大通道数,目前为1           	          
static int subcase_4(void)
{
	/*private & local definition*/
	int p = 0, p1 = 0;//目前只支持一个PPP通道,系统句柄一般为0
	
	/*process body*/
	//构造PPP测试前环境
	//注意:pppClose后会连物理链路也断开,每次调用pppClose后,再调用pppOpen都要重新LinkUp
	if(LinkUp()!=SUCC)
	{
		send_result("line %d:创建链路失败", __LINE__);
		return FAIL;
	}
				
	if ((p=pppOpen(1))<0)                                                                                            	                                                                                                                                                        	
	{
		send_result("line %d:%s测试失败(err=%d)", __LINE__, TESTAPI, getPPPerrorcode(p));
		LinkDown();
		return FAIL;                                                                                                  	                                                                                                                                                        	
	}	                                                                                                         	                                                                                                                                                        	
	if ((p1=pppOpen(1024))>=0)                                                                                        	                                                                                                                                                        	
	{                                                                         	                                                                                                                                                        	
		send_result("line %d:%s测试失败(err=%d, hd=%d)", __LINE__, TESTAPI, getPPPerrorcode(p1), p1);
		LinkDown(); 
		return FAIL;                                                                                                	                                                                                                                                                        	
	}	                                            
	ppp_subcase = NULL;//设置下一节点
	return SUCC;
}

static int resume(void)
{
	/*private & local definition*/
	
	/*process body*/
	LinkDown();    
	send_result("%s测试通过(耗时%fs)", TESTAPI, fTimeElapsed);                                                               	                                                                                                                                                        	                                                                                                  	                                                                                                                                                        	
	return SUCC;	
}

static int handle_subcase(void)
{
	/*private & local definition*/

	/*process body*/
	if(ppp_subcase!=NULL)
		return ppp_subcase();
	else	//test over
	{
		resume();
		return (-2);
	}
}

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  			  jiangwx		   2006.9.26	 	created
*						lwl		         20080922		recreated
*****************************************************************/
void ppp1(void)
{
	/*private & local definition*/
	int subcase = 0, p = 0, ret = 0;
	int tpstatus = PHASE_DEAD;
	ppp_subcase = subcase_1;//测试前恢复首节点
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);

	while (1)
	{
		delay(10);//加延时是为了降低连续读状态对芯片的压力
		tpstatus = GetPPPphase(p);

		switch (tpstatus)
		{
		case PHASE_INITIALIZE:  //PPP正在初始化                                                                                                  	
			cls_show_msg1(2, "PPP初始化中...\n");
		case PHASE_DEAD://ppp关闭                                                                                                                	
			cls_show_msg1(2, "PPP处于初始态(%d#子用例)...\n", ++subcase);			                                                                                         	                                                                                                                  	

			ret = handle_subcase();
			if(ret==FAIL)
				return;
			else if(ret==SUCC)
				;//break;
			else	//-2
				return;
			
			break;                                                                                                                           	                                                                                                                       	
		case PHASE_ESTABLISH: //PPP正在链接                                                                                                      	
			cls_show_msg1(2, "PPP已创建,获取网络层信息中...\n");
			if(IsPPPTimeout(p, PHASE_ESTABLISH))
			{
				send_result("line %d:状态机错误,一直处于创建态(subcase:%d,  err=%d)", __LINE__, subcase, getPPPerrorcode(p));
				pppClose(p);//简单错误处理一下,不必判是否成功,反正已有问题了                                                             	
				LinkDown();
				return;     				
			}
			break;                                                                                                                           	
		case PHASE_AUTHENTICATE://PPP正在认证用户名及密码                                                                                        	
			cls_show_msg1(2, "PPP认证用户名及密码中...\n");
			if(IsPPPTimeout(p, PHASE_AUTHENTICATE))
			{
				send_result("line %d:状态机错误,一直处于认证态(subcase:%d,  err=%d)", __LINE__, subcase, getPPPerrorcode(p));
				pppClose(p);//简单错误处理一下,不必判是否成功,反正已有问题了                                                             	
				LinkDown();
				return;     				
			}
			break;                                                                                                                           	
		case PHASE_CALLBACK: //PPP回复呼叫                                                                                                       	
			cls_show_msg1(2, "PPP回复呼叫中...\n");
			break;                                                                                                                           	
		case PHASE_NETWORK://PPP正在请求IP地址                                                                                                   	
			cls_show_msg1(2, "PPP已接入网络,获取IP中...\n");
			if(IsPPPTimeout(p, PHASE_NETWORK))
			{
				send_result("line %d:本地获取动态IP失败(subcase:%d, err=%d)", __LINE__, subcase, getPPPerrorcode(p));
				pppClose(p);//简单错误处理一下,不必判是否成功,反正已有问题了                                                             	
				LinkDown();
				return;                                                                                                                  	
			}
			//成功的话,关闭PPP,开始新一轮的测试                                                                                          	
                     //这里close的成功与否会影响下一次的open,所以判一下close是否成功                                                          	
			if ((ret=pppClose(p))<0)                                                                                               	
			{
				send_result("line %d:pppClose失败(0x%, subcase:%d,  err=%d)", __LINE__, ret, subcase, getPPPerrorcode(p));
				LinkDown();
				return;
			}

			//PPPCLOSE后,本端已处于LINKDOWN状态,大约40s,对端才会完全挂断
			//本端若不LinkDown一下,状态不能与对端同步,会影响下一轮测试
			LinkDown();
			break;	                                                                                                                         	
		case PHASE_TERMINATE://ppp正在断开                                                                                                       	
			cls_show_msg1(2, "ppp正在断开...");                                                                                                        	                                                                                                   	
			break;                                                                                                                           	
		default://未知状态，判为错	
			send_result("line %d: UNKNOWN STATE(tpstatus=%d, err=%d)", __LINE__, tpstatus, getPPPerrorcode(p));
			pppClose(0);//为了防止open成功后,读到未知状态,在这里return,把已open开的句柄(目前只能打开一个且值为0)                      	
			LinkDown();
			return;                
			break;
		}
	}
}

