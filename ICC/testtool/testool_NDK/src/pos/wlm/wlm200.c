/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: wlm通讯模块
* file name			: 
* Author 			: liny
* version			: 
* DATE				: 20171201
* directory 			: 
* description			: 测试wlm 通讯模块随机组合情况
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"
#define	WLM_TIMEOUT	2000
#if CPU5810X_ENABLE||defined ME68||defined ME50NX
#define	CHECK_TIMEOUT	2000
#endif

/*---------------constants/macro definition---------------------*/

#define	TESTAPI	"wlm模块内随机组合"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
  	WlInit,
	WlModemGetSQ,
	WlSendATCmd,	
	WlModemReset,
	WlModemClose,
	WlGetInfo,
	WlGetStationInfo,
#if CPU5810X_ENABLE||defined ME68||defined ME50NX
    	WLCheckSIM,
#endif
#if !K21_ENABLE //若不支持此函数可自行关闭
    	WlGetStationInfoGeneral,
#endif
	MaxNum,
}EM_WLM;
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: wlm 通讯模块随机组合函数
* functional description 	: 测试wlm 通讯模块随机组合情况
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*                                 linying      20180211       cteated
*****************************************************************/
static int wlm_randtest(int funnum)
{ 
    EM_WLM_STATUS emStatus = -1;
	ST_MOBILE_STATION_INFO pstinfo;
	int sq =0;
	ST_ATCMD_PACK STatcmdpack = {0};
	char szOutbuf[1024] = {0};
	char pszvalue[25] = {0};
	uchar AtCmdNo[]={ WLM_CMD_E0,WLM_CMD_CSQ,WLM_CMD_CREG,WLM_CMD_CPIN,WLM_CMD_CPIN0,WLM_CMD_CGATT0,WLM_CMD_CGATT1,WLM_CMD_DIAL,WLM_CMD_D2,WLM_CMD_COPS,WLM_CMD_CGMR,WLM_CMD_CGSN,WLM_CMD_CCID,				/**<获取无线SIM卡序列号*/
    	WLM_CMD_END,WLM_CMD_UNDEFINE,WLM_CMD_PUBLIC};
	uchar WLM_TYPE_INFO[]={ WLM_INFO_CCID,WLM_INFO_IMSI,WLM_INFO_IMEI};
#if !K21_ENABLE
	uchar STATION_TYPE[]={STATION_TYPE_DEFAULT,STATION_TYPE_GSM,STATION_TYPE_LTE,STATION_TYPE_TDSCDMA,STATION_TYPE_WCDMA,STATION_TYPE_CDMAHDR};
#endif

	srand((unsigned)time(NULL));
    	switch(funnum)
	{	
	    case WlInit:
			NDK_WlInit(WLM_TIMEOUT,NULL,&emStatus);
			break;
		case WlModemGetSQ:
			NDK_WlModemGetSQ(&sq);
			break;
		case WlSendATCmd:
			memset(szOutbuf, 0, sizeof(szOutbuf));
			STatcmdpack.AtCmdNo = AtCmdNo[rand()%sizeof(AtCmdNo)];
		        STatcmdpack.pcAddParam = NULL;
		        STatcmdpack.pcAtCmd = NULL;
		        NDK_WlSendATCmd(&STatcmdpack, szOutbuf,sizeof(szOutbuf), 30000,&emStatus);	
			break;
		case WlModemReset:
			NDK_WlModemReset();
			break;
		case WlModemClose:
			NDK_WlModemClose();
			break;
		case WlGetInfo:
			memset(pszvalue,0,sizeof(pszvalue));
	        	NDK_WlGetInfo(WLM_TYPE_INFO[rand()%3],pszvalue,sizeof(pszvalue));
			break;
		case WlGetStationInfo:
			NDK_WlGetStationInfo(&pstinfo);
			break;
#if CPU5810X_ENABLE||defined ME68||defined ME50NX
        case WLCheckSIM:
			NDK_WLCheckSIM("1234",CHECK_TIMEOUT);
			break;
#endif

#if !K21_ENABLE  //若不支持此函数可自行关闭
		case WlGetStationInfoGeneral:
			NDK_WlGetStationInfoGeneral(STATION_TYPE[rand()%sizeof(STATION_TYPE)],&pstinfo);
#endif
    }
    return SUCC;
}

void wlm200(void)
{
	/*private & local definition*/
	int num[50] ={0}, i = 0, bak = 0;
	int ret = -1,cnt =50, len = 0,j=0;//cnt 待定
#if CPU5810X_ENABLE||defined ME68||defined ME50NX
	char *wlmstr[] = {"NDK_WlInit","NDK_WlModemGetSQ","NDK_WlSendATCmd","NDK_WlModemReset","NDK_WlModemClose","NDK_WlGetInfo","NDK_WlGetStationInfo","NDK_WLCheckSIM"};	
#endif
#if !K21_ENABLE
    char *wlmstr[] = {"NDK_WlInit","NDK_WlModemGetSQ","NDK_WlSendATCmd","NDK_WlModemReset","NDK_WlModemClose","NDK_WlGetInfo","NDK_WlGetStationInfo","NDK_WlGetStationInfoGeneral"};	
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
			if((ret = wlm_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",wlmstr[num[j]]);
				goto ERR;
			}
		}	
		
	}
	send_result("%s测试通过", TESTAPI);
ERR:
	NetDown();
	NDK_WlModemReset();//应开发建议增加复位要求，防止模块关闭后，测试人员做其他无线操作，出现误报bug的现象20180329 linying added
	return;
}


