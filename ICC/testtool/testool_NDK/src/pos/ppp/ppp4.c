/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: ppp模块
* file name		: ppp4.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PppCheck接口功能
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
#define TESTAPI	"NDK_PppCheck"	
#define MAXTIMEOUT	60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void ppp3(void)
* functional description 	: 获取PPP状态
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void ppp4(void)
{
	/*private & local definition*/
	int nRet = 0;
	EM_PPP_STATUS emStatus=0;
	char szIpAddr[30]={0};
	int nErr=100;
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI);
		
	//构造PPP测试前环境
	if((getlinktype()!=ASYN&&LinkUp()!=SUCC)
	||(getlinktype()==ASYN&&LinkUp4AsynPPP()!=SUCC))//if(LinkUp()!=SUCC)
	{
		send_result("line %d:创建链路失败", __LINE__);
		return;
	}
	if((nRet = PppSetUsrCfg())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;	
	}

	//status参数为NULL情况下返回参数错误
#if CPU5810X_ENABLE||defined ME50NX||defined ME68
	if((nRet=NDK_PppCheck(NULL, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;	
	}
#endif
	
	//函数的几个状态要在底层中才能检测到,所以测试程序只测试在应用层能检测到的两个状态PHASE_DEAD和PHASE_NETWORK
	//zhangnw201105add 未打开PPP,应得到PPP_STATUS_DISCONNECT
	if((nRet=NDK_PppCheck(&emStatus, &nErr))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;	
	}
#if !(defined ME50NX||defined ME68||CPU5810X_ENABLE) //5810平台nErr参数不用  20180408 sull modify
	if(emStatus!=PPP_STATUS_DISCONNECT||nErr==100)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, emStatus, nErr);
		goto ppp_err;	
	}	
#else  //夏新50N没有err值
	if(emStatus!=PPP_STATUS_DISCONNECT)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, emStatus, nErr);
		goto ppp_err;	
	}
#endif
	if(getlinktype()==ASYN&&(nRet=WM_PPP_16300_DIAL())<0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;
	}	

	//测试PPP_STATUS_CONNECTING、PPP_STATUS_CONNECTED态
	if((nRet = NDK_PppDial(PPPUSR,PPPPWD))!=NDK_OK)
	{    
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;             
	}

	if((nRet=NDK_PppCheck(&emStatus, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;	
	}
#if defined ME68 || defined ME50NX //ac35平台只支持已连接和未连接状态
	if(emStatus!=PPP_STATUS_DISCONNECT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, emStatus);
		goto ppp_err;	
	}
#else
	if(emStatus!=PPP_STATUS_CONNECTING)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, emStatus);
		goto ppp_err;	
	}
#endif

	//本用例为getPPPphase的单元测试,可通过GetPPPphase来测试,但不要通过WaitPPPState来测试
	//单元测试的原则:尽量直接使用API接口,不要使用自行封装的函数
	lib_StartStopwatch();
	while(1)
	{	
		NDK_SysDelay(10);//加延时是为了降低连续读状态对芯片的压力
		if((nRet=NDK_PppCheck(&emStatus, NULL))!=NDK_OK)
		{    
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;             
		}
		if(PPP_STATUS_CONNECTED==emStatus)
			break;
			
		if(lib_ReadStopwatch()>MAXTIMEOUT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, emStatus);
			goto ppp_err;
		}
	}

	//进一步看申请动态IP是否成功(可能一次取不到,多取几次)         
	if((nRet = lib_GetLocalAddr( szIpAddr, NULL, NULL, NULL))!=SUCC)
	{    
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;             
	}

	//紧接上一步测试后的环境作为此时的测试前置,测试PHASE_DEAD
	if((nRet=NDK_PppHangup(0))!=NDK_OK)//使用非阻塞式挂断
	{    
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;             
	}

	if((nRet=NDK_PppCheck(&emStatus, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;	
	}
#if !(defined ME50NX ||defined ME68)//ME50NX只支持已连接，未连接，还有正在连接，正在挂断还不支持 sull 20171124
	if(emStatus!=PPP_STATUS_DISCONNECTING)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, emStatus);
		goto ppp_err;	
	}	
#endif
	
	//有的机型返回快,有的慢,于是干脆一直读状态直到得到PHASE_DEAD,否则超时报错
	//p句柄已关闭,状态应该回归初始态
	lib_StartStopwatch();
	while(1)
	{	
		NDK_SysDelay(10);//加延时是为了降低连续读状态对芯片的压力
		if((nRet=NDK_PppCheck(&emStatus, NULL))!=NDK_OK)
		{    
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;             
		}
		if(PPP_STATUS_DISCONNECT==emStatus)
			break;
			
		if(lib_ReadStopwatch()>MAXTIMEOUT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, emStatus);
			goto ppp_err;
		}
	}
	
	//PPP FSM中状态跃迁的顺序正确性与否暂不测试
	//上面已测

	//test over
	send_result("%s测试通过", TESTAPI);	
ppp_err:
#if !(CPU5810X_ENABLE||defined ME50NX||defined ME68)
	NDK_PppHangup(1);
#else  //50N仅支持非阻塞挂断
	NDK_PppHangup(0);
#endif
	LinkDown();
	return;
}

