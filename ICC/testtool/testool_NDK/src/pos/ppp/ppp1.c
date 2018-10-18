/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: ppp模块
* file name		: ppp1.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PppSetCfg,NDK_PppDial,NDK_PppHangup接口功能
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
#define TESTAPI	"NDK_PppSetCfg,NDK_PppDial,NDK_PppHangup"
#define MAXTIMEOUT	60

/*----------global variables declaration("extern")--------------*/

/*------------global variables definition-----------------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
//发送atstr并等待响应(预期res)
static int WLM_get_rsp(EM_WLM_CMD AtCmdNo, char* atstr, EM_WLM_STATUS rsp, char *rspbuf)
{
	/*private & local definition*/
	ST_ATCMD_PACK st_ATCMD_PACK;
	char szOutput[1024]={0};
	EM_WLM_STATUS emStatus=0;

	/*process body*/
	memset(&st_ATCMD_PACK,0,sizeof(ST_ATCMD_PACK));
	st_ATCMD_PACK.AtCmdNo=AtCmdNo;
	strcpy(st_ATCMD_PACK.pcAtCmd,atstr);
	st_ATCMD_PACK.pcAddParam=NULL;

	if(NDK_WlSendATCmd(&st_ATCMD_PACK,szOutput,sizeof(szOutput),30000,&emStatus)!=NDK_OK)
		return FAIL;
	if(rsp!=emStatus)
		return FAIL;
	if(rspbuf!=NULL && !strstr(szOutput, rspbuf))
		return FAIL;

	return SUCC;
}

//增强型WLM_get_rsp,增加了重试/超时机制
static int WLM_get_rsp_Ex(EM_WLM_CMD AtCmdNo, char* atstr, EM_WLM_STATUS rsp, char *rspbuf)
{
	/*private & local definition*/
	int ret = FAIL;
	time_t diff = 0, oldtime = time(NULL);//不使用不可重入函数StartStopwatch()

	/*process body*/
	while((ret=WLM_get_rsp(AtCmdNo, atstr, rsp, rspbuf))!=SUCC)
	{
		if((diff=time(NULL)-oldtime)>MAXTIMEOUT)//if(ReadStopwatch()>MAXTIMEOUT)
			break;
		show_stopwatch(ENABLE, diff);
		//NDK_SysDelay(10);// 20140505linwl 将延时动作移动到show_stopwatch函数中
	}

	show_stopwatch(DISABLE, 0);
	return ret;
}

static int set_apn(void)
{
	/*private & local definition*/
	char ApnStr[96] = {0}; 
	
	/*process body*/
	sprintf(ApnStr, "+CGDCONT=1,\"IP\",\"%s\"", WLMAPN);//设置错误的APN,也可能返回OK,而在之后的PPP连接中报错

	return WLM_get_rsp(WLM_CMD_UNDEFINE, ApnStr, WLM_STATUS_OK, NULL);
}

static int WLMDial(void)
{	
	time_t diff = 0, oldtime = 0;
	LinkType type = getlinktype();

	//建立GPRS,CDMA连接
	cls_printf("建立%s连接中...\n", (type==GPRS)?"GPRS":((type==CDMA)?"CDMA":"TD"));

	//在链路层设置必要的参数(供之后的网络层使用)
	//不是必须的!WLM_get_res(WM_CGATT1, NULL, OK);
	if((type==GPRS||type==TD) && set_apn()!=SUCC)
	{
		cls_show_msg1(10, "%s, line %d:设置APN失败!", __FILE__, __LINE__);
		return FAIL;
	}
	
	NDK_ScrDispString(0,16,"查询卡PIN中...  ",0);//16个字符
	if(WLM_get_rsp_Ex(WLM_CMD_UNDEFINE, "+CPIN?", WLM_STATUS_OK, "READY")!=SUCC)
	{
		cls_show_msg1(10, "%s, line %d:查询卡PIN失败!", __FILE__, __LINE__);
		return FAIL;
	}

	//做DIAL前,要确保模块已附着到网络上
	NDK_ScrDispString(0,16,"附着网络中...  ",0);
	oldtime = time(NULL);//不使用不可重入函数StartStopwatch()
	while(((type==GPRS||type==TD) && WLM_get_rsp(WLM_CMD_UNDEFINE, "+CREG?", WLM_STATUS_OK, "1")!=SUCC && WLM_get_rsp(WLM_CMD_UNDEFINE, "+CREG?", WLM_STATUS_OK, "5")!=SUCC)
		||(type==CDMA && WLM_get_rsp(WLM_CMD_UNDEFINE, "+CAD?", WLM_STATUS_OK, "1")!=SUCC))
	{
		if((diff=time(NULL)-oldtime)>MAXTIMEOUT)//if(ReadStopwatch()>MAXTIMEOUT)
		{
			cls_show_msg1(10, "%s, line %d:无线模块附着网络失败!", __FILE__, __LINE__);
			return FAIL;
		}
		show_stopwatch(ENABLE, diff);
		//NDK_SysDelay(10);// 20140505linwl 将延时动作移动到show_stopwatch函数中
	}
	show_stopwatch(DISABLE, 0);

	NDK_ScrDispString(0,16,"拔号中...         ",0);
	if(WLM_get_rsp_Ex(WLM_CMD_DIAL, NULL, WLM_STATUS_CONNECT, "CONNECT")!=SUCC)//"D*99***1#" "D#777",API底层会自动选择
	{
		cls_show_msg1(10, "%s, line %d:建立%s连接失败!", __FILE__, __LINE__, (type==GPRS)?"GPRS":((type==CDMA)?"CDMA":"TD"));
		return FAIL;
	}
	
	return SUCC;
}

/****************************************************************
* function name 	 				: void ppp1(void)
* functional description 	: 配置PPP参数,拨号，挂断
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void ppp1(void)
{
	/*private & local definition*/
	int nRet = 0;
	ulong ulLocalAddr=0;
	EM_PPP_STATUS emStatus=0;
	ST_PPP_CFG stPPPCfg;
	char szIpAddr[20]={0};
	LinkType type = getlinktype();

	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	//下面如果不是为了测试NetPPPHangup,一般挂断一般选用阻塞式挂断NetPPPHangup(1),使用非阻塞式挂断时要判断PPP状态
	//默认是只争对无线的ppp的,有线MODEM的PPP要设置拔号函数,因动作不太一样,故分不同的分支去测试

	//ModemDial的说明(20130319):
	//底层改为linux后,应用传的函数地址,底层无法调到相应的函数了.故对无线而言,即使ModemDial非NULL,其实对底层而言,效果与传NULL一样.ModemDial成为一个兼容参数(实际上无用了)
	//对无线和有线异步猫而言,传NULL时,底层会帮应用进行链路层的拔号连接,而异步猫而言,由于ModemDial传递的地址,底层调不到,所以,必须
	//额外地,先在应用层进行链路层的拔号调用才行,而不能期望通过传递ModemDial,让底层来调用.

	memset((void *)&stPPPCfg, 0, sizeof(stPPPCfg));
	
	switch(type)
	{	
	case CDMA:
		//case1:错误的USR/PWD
		if((nRet=NDK_PppDial("newland_chensj", "newland_chensj"))!=NDK_OK/*NDK_ERR_PARA*/)//NDK中的实现只是负责将账号，密码传送出去，具体是由PPPserv处理的，后面的WaitPPPState是不会成功的 20131022 涂志广解释
		{	
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
		if((nRet = WaitPPPState(PPP_STATUS_CONNECTED, NULL))==SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
	case GPRS:
	case TD:
		//case2
		//验证:在调用NetPPPDial前不调用NetPPPCfg，底层默认在pppoPen前做拨号动作，采用阻塞式挂断，应成功
		//故不必单独linkup,交由NetPPPDial去完成
		if((nRet=NDK_PppDial(PPPUSR, PPPPWD))!=NDK_OK)//GPRS网与用户密码无关,故共用card/card
		{	
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
		if((nRet = WaitPPPState(PPP_STATUS_CONNECTED, NULL)) != SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
		
		//进一步看申请动态IP是否成功(以示处于数通态) 
		if((nRet = lib_GetLocalAddr(szIpAddr,NULL,NULL,NULL))!=SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;		
		}
#if  !(CPU5810X_ENABLE||defined ME50NX||defined ME68) //50仅支持非阻塞挂断
		//case4:测试NetPPPHangup(1)能否成功阻塞式挂断
		if((nRet = NDK_PppHangup(1)) !=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
		//阻塞式挂断后,应立即检测到PPP_STATUS_DISCONNECT,不要用PPP_STATUS_DISCONNECT
		if((nRet = NDK_PppCheck(&emStatus, NULL))!=NDK_OK||emStatus!=PPP_STATUS_DISCONNECT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;	
		}
#else
		if((nRet = NDK_PppHangup(0)) !=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
		//非阻塞式挂断等会后会完全挂断
		if((nRet = WaitPPPState(PPP_STATUS_DISCONNECT, NULL))!=SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;	
		}
#endif		
		//case5(异常测试尽量放在case5中扩展)
		//测试NetPPPDial前调用NetPPPCfg，传入自己的拨号函数，那么底层还是默认在pppoPen前做拨号动作．采用非阻塞式挂断，应成功
		//设置拨号信息
		if(type==GPRS||type==TD)
		{
			strcpy(stPPPCfg.szDailNum, "*99***1#");
		}
		else if(type==CDMA)
		{
			strcpy(stPPPCfg.szDailNum, "#777");
		}
			
		stPPPCfg.nDevType = 0;//0表示 无线,1为有线
#if !(CPU5810X_ENABLE||defined ME50NX||defined ME68)	
		stPPPCfg.nPPPFlag = LCP_PPP_KEEP;
		sprintf(stPPPCfg.szApn, "+CGDCONT=1,\"IP\",\"%s\"", WLMAPN);//sprintf(stPPPCfg.szApn, "\"%s\"", WLMAPN);
#else 
		stPPPCfg.nPPPFlag = LCP_PPP_UNAUTOCONNECT; //nPPPFlag参数定义为:自动重连标志,单元测试不自动重连,自动重连功能在综合的异常用例中测试
		sprintf(stPPPCfg.szApn, WLMAPN);	//低端PPP设置APN只需直接设置,无需增加CGD等字符串
#endif
		stPPPCfg.ModemDial = WLMDial;
		stPPPCfg.PPPIntervalTimeOut = 600;

		//case6:NDK_PppSetCfg异常参数
		if((nRet = NDK_PppSetCfg(NULL,0))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
#if  !(CPU5810X_ENABLE||defined ME50NX||defined ME68) //低端产品长度参数无效,不做测试
		if((nRet = NDK_PppSetCfg(&stPPPCfg, sizeof(stPPPCfg)-1))!=NDK_ERR_PPP_PARAM)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
#endif
		if((nRet = NDK_PppSetCfg(&stPPPCfg, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
		
		//case7:NDK_PppDial异常参数
		if((nRet = NDK_PppDial(NULL, NULL))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
		
		//拨号
		NDK_PppDial(PPPUSR, PPPPWD);//这里不测NetPPPDial了,故不判返回
		if((nRet = WaitPPPState(PPP_STATUS_CONNECTED, NULL))!=SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;	
		}
		
		//进一步看申请动态IP是否成功(以示处于数通态) 
		if((nRet = lib_GetLocalAddr(szIpAddr,NULL,NULL,NULL))!=SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;		
		}	

		//case3:测试NetPPPHangup 异常参数
		if((nRet = NDK_PppHangup(-1)) !=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
		if((nRet = NDK_PppHangup(10)) !=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
		
		//case8:测试NetPPPHangup(0)能否成功非阻塞挂断
		if((nRet = NDK_PppHangup(0)) !=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}

		//case9:测试非阻塞挂断取状态应为正在挂断
#if !(defined ME50NX ||defined ME68) //ME50NX只支持已连接，未连接，还有正在连接，正在挂断还不支持 sull 20171124
		if((nRet = NDK_PppCheck(&emStatus, NULL))!=NDK_OK||emStatus!=PPP_STATUS_DISCONNECTING)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI,nRet,emStatus);
			goto ppp_err;	
		}
#endif

		//非阻塞式挂断等会后会完全挂断
		if((nRet = WaitPPPState(PPP_STATUS_DISCONNECT, NULL))!=SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;	
		}
#if  !(CPU5810X_ENABLE||defined ME50NX||defined ME68)  //50N 的 nPPPFlag定义为是否支持自动重连 在综合异常测试中测
		//case10:测试nPPPFlag能否支持短链接
		if(type==GPRS||type==TD)
		{
			strcpy(stPPPCfg.szDailNum, "*99***1#");
		}	
		else if(type==CDMA)
		{
			strcpy(stPPPCfg.szDailNum, "#777");
		}
		stPPPCfg.nDevType = 0;//0表示 无线,1为有线
		stPPPCfg.nPPPFlag = LCP_PPP_UNKEEPED;//不维持长链接LCP_PPP_UNKEEPED
		sprintf(stPPPCfg.szApn, "+CGDCONT=1,\"IP\",\"%s\"", WLMAPN);//sprintf(stPPPCfg.szApn, "\"%s\"", WLMAPN);
	//	strcpy(stPPPCfg. szUserName,"card");
	//	strcpy(stPPPCfg. szUserPasswd,"card");
		
		stPPPCfg.ModemDial = NULL;//如果之前已配置过,此时再配置NULL,则还是使用之前的拔号函数
		stPPPCfg.PPPIntervalTimeOut = 600;

		if((nRet = NDK_PppSetCfg (&stPPPCfg, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;
		}
		
		nRet = NDK_PppDial(PPPUSR, PPPPWD);//这里不测NetPPPDial了,故不判返回
		if((nRet = WaitPPPState(PPP_STATUS_CONNECTED, NULL))!=SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;	
		}
		
		//进一步看申请动态IP是否成功(以示处于数通态) 
		if((nRet = lib_GetLocalAddr(szIpAddr,NULL,NULL,NULL))!=SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;		
		}
		NDK_PppHangup(1);
		if((nRet = NDK_PppCheck(&emStatus, NULL))!=NDK_OK||emStatus!=PPP_STATUS_DISCONNECT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;	
		}		
		
		//case11:测试nPPPFlag能否支持长链接
		if(type==GPRS||type==TD)
		{
			strcpy(stPPPCfg.szDailNum, "*99***1#");
		}	
		else if(type==CDMA)
		{
			strcpy(stPPPCfg.szDailNum, "#777");
		}
		stPPPCfg.nDevType = 0;//0表示 无线,1为有线
		stPPPCfg.nPPPFlag = LCP_PPP_KEEP;//维持长链接
		sprintf(stPPPCfg.szApn, "+CGDCONT=1,\"IP\",\"%s\"", WLMAPN);//sprintf(stPPPCfg.szApn, "\"%s\"", WLMAPN);
	//	strcpy(stPPPCfg. szUserName,"card");
	//	strcpy(stPPPCfg. szUserPasswd,"card");
		
		stPPPCfg.ModemDial = NULL;
		stPPPCfg.PPPIntervalTimeOut = 600;

		if((nRet=NDK_PppSetCfg (&stPPPCfg, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;
		}

		nRet = NDK_PppDial(PPPUSR, PPPPWD);	//这里不测NetPPPDial了,故不判返回
		if((nRet = WaitPPPState(PPP_STATUS_CONNECTED, NULL))!=SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;		
		}
		
		//进一步看申请动态IP是否成功(以示处于数通态) 
		if((nRet = NDK_PppGetAddr(&ulLocalAddr,NULL))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;	
		}
		NDK_PppHangup(1);
		if((nRet = NDK_PppCheck(&emStatus, NULL))!=NDK_OK||emStatus!=PPP_STATUS_DISCONNECT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;	
		}		
#endif
		//test over
		send_result("%s测试通过", TESTAPI);
		goto ppp_err;
		break;
	case ASYN:
		//底层默认不帮忙做有线PPP拨号函数功能，如果需要应用层人员自己传入拨号函数．
		//case12:测试调用 了cfg但未传入拔号函数时,NetPPPDiaL应失败(目前的设计(拔号是非阻塞的)限制使得行为是NetPPPDiaL成功,但之后 取状态应取不到CNNTED态)
		strcpy(stPPPCfg.szDailNum, MDMDialStr);
		stPPPCfg.nDevType = 1;//0表示 无线,1为有线
		//stPPPCfg.nPPPFlag = 1;
		//strcpy(stPPPCfg.szApn, "CMNET");
		//strcpy(stPPPCfg. szUserName,"card");
		//strcpy(stPPPCfg. szUserPasswd,"card");
		
		stPPPCfg.ModemDial = NULL;
		stPPPCfg.PPPIntervalTimeOut = 600;

		if((nRet=NDK_PppSetCfg (&stPPPCfg, sizeof(stPPPCfg)/*0*/))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;
		}

		if((nRet=NDK_PppDial(DEFAULT_ASYN_USR, DEFAULT_ASYN_PWD))!=NDK_OK)//>=0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;
		}
		if((nRet = WaitPPPState(PPP_STATUS_CONNECTED, NULL))==SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;		
		}
		
		//case13:测试调用 了cfg且传入拔号函数时,NetPPPDiaL应成功(测试设计改为应用先进行拔号,再调用NDK_PppDial.by zhangnw20130319)
		strcpy(stPPPCfg.szDailNum, MDMDialStr);
		stPPPCfg.nDevType = 1;//0表示 无线,1为有线
		//stPPPCfg.nPPPFlag = 1;
		//strcpy(stPPPCfg.szApn,"CMNET");
		//strcpy(stPPPCfg. szUserName,"card");
		//strcpy(stPPPCfg. szUserPasswd,"card");
		
		stPPPCfg.ModemDial = NULL;//ASYNDial;
		stPPPCfg.PPPIntervalTimeOut = 600;

		if((nRet=NDK_PppSetCfg (&stPPPCfg, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;
		}

		if((nRet=ASYNDial4PPP())!=SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;		
		}
		if((nRet=WM_PPP_16300_DIAL())<0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;		
		}
		
		if((nRet=NDK_PppDial(DEFAULT_ASYN_USR, DEFAULT_ASYN_PWD))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;
		}
		if((nRet = WaitPPPState(PPP_STATUS_CONNECTED, NULL))!=SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;		
		}
		if((nRet=NDK_PppDial(DEFAULT_ASYN_USR, DEFAULT_ASYN_PWD))!=NDK_ERR_PPP_OPEN)//已经连接之后再去调用NDK_PppDial应该返回NDK_ERR_PPP_OPEN
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;
		}

		//进一步看申请动态IP是否成功(以示处于数通态) 
		if((nRet = NDK_PppGetAddr(&ulLocalAddr,NULL))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;		
		}
		//测试NetPPPHangup(1)能否成功阻塞式挂断
		if((nRet=NDK_PppHangup(1))!=SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;
		}
#if 1
		if((nRet = NDK_PppCheck(&emStatus, NULL))!=NDK_OK||emStatus!=PPP_STATUS_DISCONNECT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			goto ppp_err;	
		}
#else		
		//zhangnw201105add
		if((nRet = WaitPPPState(PPP_STATUS_DISCONNECT , NULL))!=SUCC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
			goto ppp_err;		
		}
#endif

#if CPU5892_ENABLE||CPU5830X_ENABLE||OVERSEAS_ENABLE
		NDK_MdmHangup();//对应上面的ASYNDial调用
#endif		
		//test over
		send_result("%s测试通过", TESTAPI);
		goto ppp_err;
		break;
	default:
		send_result("%s测试失败:未知的链路类型(%d)", TESTAPI, type);
		goto ppp_err;
		break;
	}
ppp_err:
#if !(CPU5810X_ENABLE||defined ME50NX||defined ME68)
	NDK_PppHangup(1);
#else  //50N仅支持非阻塞挂断
	NDK_PppHangup(0);
#endif
	PppSetUsrCfg();
#if CPU5892_ENABLE||CPU5830X_ENABLE||OVERSEAS_ENABLE
	if(type==ASYN)
		NDK_MdmHangup();//对应上面的ASYNDial调用	
#endif	
	return;
}

