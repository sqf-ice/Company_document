/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: PPP 通讯模块
* file name			: 
* Author 			: liny
* version			: 
* DATE				: 
* directory 			: 
* description			: 测试PPP 通讯模块随机组合情况
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/

#define	TESTAPI	"PPP模块内随机组合"
#define MAXTIMEOUT 60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
    PppSetCfg,
    PppDial,
    PppHangup,
    PppCheck,
#if !K21_ENABLE
    PppAddrChange,
    PppGetAddr,
    NetGetAddr,    
#endif
#if CPU5810X_ENABLE
    PppGetNetAddr,
#endif
	MaxNum,
}EM_PPP;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: 
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*
*****************************************************************/
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
	
	NDK_ScrDispString(0,16,"查询卡PIN中...	",0);//16个字符
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

	NDK_ScrDispString(0,16,"拔号中...		  ",0);
	if(WLM_get_rsp_Ex(WLM_CMD_DIAL, NULL, WLM_STATUS_CONNECT, "CONNECT")!=SUCC)//"D*99***1#" "D#777",API底层会自动选择
	{
		cls_show_msg1(10, "%s, line %d:建立%s连接失败!", __FILE__, __LINE__, (type==GPRS)?"GPRS":((type==CDMA)?"CDMA":"TD"));
		return FAIL;
	}
	
	return SUCC;
}

static int ppp_randtest(int funnum)
{ 
	ST_PPP_CFG stPPPCfg;
	EM_PPP_STATUS emStatus=0;
	int nErr=100;
#if !K21_ENABLE 
	ulong ulIpAddr=0, ulHostAddr=0, ulLocalAddr=0;
	char szGateway[16] = {0}, szMaskaddr[16] = {0}, pszDns[16] = {0}, szIPaddr[16] = {0};
#elif CPU5810X_ENABLE
	char LocalIPAddr[17] = {0};
#endif
	LinkType type = getlinktype();

    	switch(funnum)
	{	
		case PppSetCfg:  
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
			sprintf(stPPPCfg.szApn, "+CGDCONT=1,\"IP\",\"%s\"", WLMAPN);
#else 
			stPPPCfg.nPPPFlag = LCP_PPP_UNAUTOCONNECT; //nPPPFlag参数定义为:自动重连标志,单元测试不自动重连,自动重连功能在综合的异常用例中测试
			sprintf(stPPPCfg.szApn, WLMAPN);	//低端PPP设置APN只需直接设置,无需增加CGD等字符串
#endif
			stPPPCfg.ModemDial = WLMDial;
			stPPPCfg.PPPIntervalTimeOut = 600;
			NDK_PppSetCfg(&stPPPCfg, 0);
			break;
		case PppDial:
			NDK_PppDial(PPPUSR, PPPPWD);
			break;
		case PppHangup:
			NDK_PppHangup(rand()&2);
			break;
		case PppCheck:
			NDK_PppCheck(&emStatus, &nErr);
			break;

#if !K21_ENABLE
        	case PppAddrChange:
		        NDK_PppAddrChange("192.168.2.1",&ulIpAddr);
			break;
		case PppGetAddr:
			NDK_PppGetAddr(&ulLocalAddr, &ulHostAddr);
			break;
		case NetGetAddr:    
			NDK_NetGetAddr(COMM_TYPE_PPP, szIPaddr, szMaskaddr, szGateway, pszDns);
			break;
#endif

#if CPU5810X_ENABLE
        case PppGetNetAddr:
			NDK_PppGetNetAddr(LocalIPAddr);
			break;
#endif		

	}
	return SUCC;
}

void ppp200(void)
{
	/*private & local definition*/
	int num[50] ={0}, i = 0, bak = 0;
	int ret = -1,cnt =50, len = 0,j=0;//cnt 待定
#if CPU5810X_ENABLE
		char *pppstr[] = {"NDK_PppSetCfg","NDK_PppDial","NDK_PppHangup","NDK_PppCheck","NDK_PppGetNetAddr"};
#endif
#if defined ME68||defined ME50NX
		char *pppstr[] = {"NDK_PppSetCfg","NDK_PppDial","NDK_PppHangup","NDK_PppCheck"};
#endif
#if !K21_ENABLE
		char *pppstr[] = {"NDK_PppSetCfg","NDK_PppDial","NDK_PppHangup","NDK_PppAddrChange","NDK_PppGetAddr","NDK_PppCheck"};
#endif		

	/*process body*/
	len = (MaxNum>20)?MaxNum:20;
	while(cnt)
	{	
		cnt--;
		bak++;

		//产生随机数
		srand((unsigned)time(NULL));
		for(i=0;i<len;i++)
		{	
			num[i] = rand()%MaxNum;
		}
		cls_printf("第%d次随机顺序:",bak);
		for(i=0;i<len;i++)
		{
			NDK_ScrPrintf("%d ", num[i]);
			NDK_ScrRefresh();
		}
			
		//进行随机测试
		for(i=0;i<len;i++)
		{
			if((ret = ppp_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",pppstr[num[j]]);
				goto ERR;
			}
		}	
	}
	send_result("%s测试通过", TESTAPI);
ERR:
	LinkDown();
	return;
}


