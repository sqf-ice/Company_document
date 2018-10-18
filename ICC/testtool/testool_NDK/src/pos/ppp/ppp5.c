/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: ppp模块
* file name		: ppp3.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_NetGetAddr接口功能
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_NetGetAddr"	
#define MAXTIMEOUT 60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
extern int IsPPPTimeout(int state, int *err);

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void ppp3(void)
* functional description 	: 获取本地地址和主机地址 
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void ppp5(void)
{
	//以下为局部变量
	int nRet;
	EM_PPP_STATUS nStatus=PPP_STATUS_DISCONNECT;
	ulong ulLocalAddr=0;
	int tpstatus=0;
	int times=0;
	char szIPaddr[16] = {0}, szMaskaddr[16] = {0}, szGateway[16] = {0};
	char tmpipstr[16] = {0};
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	if((getlinktype()!=ASYN&&LinkUp()!=SUCC)
	||(getlinktype()==ASYN&&LinkUp4AsynPPP()!=SUCC))//if(LinkUp()!=SUCC)
	{
		send_result("line %d:创建链路失败", __LINE__);
		goto ppp_err;
	}
	if((nRet = PppSetUsrCfg())!=SUCC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;	
	}
	
	//case1:未拨号取地址函数调用返回失败 
	if((nRet=NDK_NetGetAddr(COMM_TYPE_PPP, szIPaddr, szMaskaddr, szGateway, NULL))!=NDK_ERR_NET_GETADDR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;
	}

	if(getlinktype()==ASYN&&(nRet=WM_PPP_16300_DIAL())<0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;
	}

	while (1)
	{
		NDK_SysDelay(10);//加延时是为了降低连续读状态对芯片的压力
		if((tpstatus = NDK_PppCheck(&nStatus,NULL))!=NDK_OK)
		{
	    		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,tpstatus);
	    		goto ppp_err;
		}		
		
		switch (nStatus)
		{   
		case PPP_STATUS_DISCONNECT ://ppp关闭                                                                           	
			cls_show_msg1(2, "PPP未连接\n");                                                                                                	
			if(++times==3)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ppp_err;                                                                                                          	
			}
			if ((nRet=NDK_PppDial(PPPUSR, PPPPWD))!=NDK_OK) 
			{
			    send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			    goto ppp_err;
			}                                                                                                             	
			break;
		case PPP_STATUS_CONNECTING: //PPP正在链接                                                                 	                                     	
			cls_show_msg1(2, "PPP正在连接中...\n");
			if(IsPPPTimeout(PPP_STATUS_CONNECTING, NULL))
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ppp_err; 			
			}
			break;                                                                                      	                                     	                                                                          	
		case PPP_STATUS_CONNECTED://PPP已连接
			cls_show_msg1(2, "PPP已接入网络,获取IP中...\n");                                                                                                    	
			//进一步看申请动态IP是否成功(可能一次取不到,多取几次)         
			if(IsPPPTimeout(PPP_STATUS_CONNECTED,NULL))
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ppp_err;                                                                                                             	
			}

			//case2:正常获取本地及主机地址
			if ((nRet=NDK_NetGetAddr(COMM_TYPE_PPP, szIPaddr, szMaskaddr, szGateway, NULL))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
				goto ppp_err;
			}
			//获取到的主机地址和NDK_PppGetAddr函数获取到的应该一致
			NDK_PppGetAddr(&ulLocalAddr, NULL);
			INET_NTOA(HTONL(ulLocalAddr), tmpipstr);
			if(strcmp(tmpipstr, szIPaddr))
			{
				send_result("line %d:%s测试失败(%s,%s)", __LINE__, TESTAPI, tmpipstr, szIPaddr);
				goto ppp_err;
			}
			
			//成功的话,关闭PPP,开始新一轮的测试
			if((nRet=NDK_PppHangup(1))!=SUCC)//使用阻塞式挂断                                                                                 	
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
				goto ppp_err;                                                                                                         	
			}

#if 0
			//zhangnw201105 add
			//由于使用pppClose是非阻塞的,故这里要等待一下
			//当然,实际上LinkDown中也会等待DEAD态,但为了结构对称,在pppClose之后,就先等一下DEAD态
			if(WaitPPPState(PPP_STATUS_CONNECTED, NULL)!=SUCC)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);   
				goto ppp_err;
			}
#endif 

			//case3:挂断后,应取不到地址
			if ((nRet=NDK_NetGetAddr(COMM_TYPE_PPP, szIPaddr, szMaskaddr, szGateway, NULL))!=NDK_ERR_NET_GETADDR)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
				goto ppp_err;
			}
			
			//test over
			send_result("%s测试通过", TESTAPI);	                                    		                                                            	
			goto ppp_err;   
			break;			                                                            	
		case PPP_STATUS_DISCONNECTING://ppp正在断开                                                                                                       	
			cls_show_msg1(2, "ppp正在断开...");                                                                                                        	
			break;                                                                                                                           	
		default://未知状态，判为错		
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nStatus);
			goto ppp_err;
			break;
		}
	}
	
ppp_err:
	NDK_PppHangup(1);
	LinkDown();
	return;
}

