
/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 测试程序配置一族函数库
* file name		: conf.c
* Author 			: zhangnw
* version			: V1.0
* DATE			: 20101217
* directory 		: .\SRC\main\
* description		: 
* related document	: 
* 
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	MAXWAITTIME 60
#define	WAITTIME  5
/*---------------functions declaration--------------------------*/
#if !(defined ME15CHX||defined ME15B||defined ME66)//ME15C为节省代码空间进行条件编译 20170213 linwl
static int conf_WLAN_lnk(void);
int use_wifi_conf(int ApID);
#endif
/*--------------------通讯模块对应的查找表--------------------------*/
uint g_conf_flag = 0; //为1的时候使用配置文件

/*---------------functions definition---------------------------*/
#if !(defined ME15C||defined ME15CHX||defined ME20||defined ME15B||defined ME66)	//ME15C为节省应用空间不编译不用的函数
#if PRN_ENABLE
#if PRNGREY_ENABLE
uint g_PrnGrey = DEFAULT_GREY_VLE;
static int conf_prn_grey(void)//注意:打印机复位不应将灰度参数恢复
{
	/*private & local definition*/
	char grey[4] = {0};
	int ret = 0;
	uint len = 0;

	/*process body*/
	if(ENTER!=cls_show_msg1(10, "重新配置打印灰度吗?\n"                                                   
											"[确认]是[其它]否\n"))
		return SUCC;
	do
	{
		cls_printf("请输入灰度值(%d-%d):", GREY_MINVALID, GREY_MAXVALID);
		sprintf(grey, "%d", g_PrnGrey); 
		if ((ret=lib_kbgetinput(grey, 0, sizeof(grey)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			g_PrnGrey = DEFAULT_GREY_VLE;
		else
			g_PrnGrey = atoi(grey);
	}while(g_PrnGrey<GREY_MINVALID || g_PrnGrey>GREY_MAXVALID);
/*	
	if ((ret=NDK_PrnSetGreyScale(g_PrnGrey))!=NDK_OK)
	{
		cls_show_msg1(10, "%s, line %d:设置打印灰度失败", __FILE__, __LINE__);
		return ret;
	}
*/	
	cls_show_msg1(2, "设置打印灰度(%d级)成功!", g_PrnGrey);
	return SUCC;
}
#else
static int conf_prn_grey(void){return FAIL;}
#endif   //对应PRNGREY_ENABLE

#if PRNUNDERLINE_EN
EM_PRN_UNDERLINE_STATUS g_PrnUnderline = PRN_UNDERLINE_STATUS_CLOSE;//0
static void conf_prn_underline(void)
{
	/*private & local definition*/
	int nKeyin = 0;//, ret = 0;

	/*process body*/
	nKeyin = cls_show_msg1(10, "是否开启打印下划线功能?\n"                                                   
											"[确认]是[其它]否\n");
	g_PrnUnderline = (nKeyin==ENTER)?PRN_UNDERLINE_STATUS_OPEN:PRN_UNDERLINE_STATUS_CLOSE;
/*
	if ((ret=NDK_PrnSetUnderLine(g_PrnUnderline))!=NDK_OK)
	{
		cls_show_msg1(10, "%s, line %d:设置打印下划线功能失败", __FILE__, __LINE__);
		return;
	}
*/	
	cls_show_msg1(2, "%s打印下划线功能成功!", g_PrnUnderline==PRN_UNDERLINE_STATUS_OPEN?"开启":"关闭");
	return;
}
#else
static void conf_prn_underline(void){}
#endif   //对应PRNUNDERLINE_EN

uint g_PrnSwitch = 0;//打印边送边打开关,默认关闭
static void conf_prn_switch(void)
{
	/*private & local definition*/

	/*process body*/
	g_PrnSwitch = (cls_show_msg1(10, "是否开启边送边打功能?\n[确认]是[其它]否\n")==ENTER)?1:0;
	return;
}

void conf_prn(void)
{
	/*private & local definition*/

	/*process body*/
	conf_prn_switch();
	conf_prn_grey();
	conf_prn_underline();
}

int lib_initprn(uint unSwitch)
{
	int ret = 0;

	//初始化
	if((ret=NDK_PrnInit(unSwitch)) != NDK_OK)
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:打印初始化失败(%d)", __FILE__, __LINE__, ret);
		return ret;
	}	
	//设置下划线属性
#if PRNUNDERLINE_EN
	if ((ret=NDK_PrnSetUnderLine(g_PrnUnderline))!=NDK_OK)
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:设置打印下划线功能%d失败(%d)", __FILE__, __LINE__, g_PrnUnderline,ret);
		return ret;
	}
#endif
	//设置灰度
#if PRNGREY_ENABLE
	if ((ret=NDK_PrnSetGreyScale(g_PrnGrey))!=NDK_OK)
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s, line %d:设置打印灰度%d失败(%d)", __FILE__, __LINE__, g_PrnGrey,ret);
		return ret;
	}
#endif
	return ret;
}

#else
void conf_prn(void){return;}
int lib_initprn(uint unSwitch){return SUCC;}
#endif

#if PPP_ENABLE
int PppSetUsrCfg(void)
{
	/*private & local definition*/
	//int ret = 0;
	ST_PPP_CFG tmp_stPPPCfg;
	LinkType type = getlinktype();	

	/*process body*/
	memset((void *)&tmp_stPPPCfg, 0, sizeof(tmp_stPPPCfg));
	if(type==ASYN)
	{	
		memset(&tmp_stPPPCfg,0,sizeof(tmp_stPPPCfg));
		strcpy(tmp_stPPPCfg.szDailNum, MDMDialStr);
		tmp_stPPPCfg.nDevType = 1;//0表示 无线,1为有线
		//tmp_stPPPCfg.nPPPFlag = 1;
		//strcpy(tmp_stPPPCfg.szApn, "CMNET");
		tmp_stPPPCfg.ModemDial = NULL;//ASYNDial;//底层默认不帮忙做有线PPP拨号函数功能，如果需要应用层人员自己传入拨号函数．
		tmp_stPPPCfg.PPPIntervalTimeOut = 600;

		//目前在conf_MDM_lnk及conf_PPP中均无对以下2个异步PPP参数的配置,暂在此处写死
		strcpy(PPPUSR, DEFAULT_ASYN_USR);
		strcpy(PPPPWD, DEFAULT_ASYN_PWD);
	}
	else
	{
		memset(&tmp_stPPPCfg,0,sizeof(tmp_stPPPCfg));//20141201 3G模块支持后修改应用层必须传入正确拨号串，如果不串则使用上次的拨号串 
		if(type==CDMA)
			strcpy(tmp_stPPPCfg.szDailNum, "#777");
		else
			strcpy(tmp_stPPPCfg.szDailNum, "*99***1#");
		tmp_stPPPCfg.nDevType = 0;//0表示 无线,1为有线
#if !(CPU5810X_ENABLE||defined ME50NX||defined ME68)//低端支持PPP功能仅支持APN设置,其它设置无效 20160527
		tmp_stPPPCfg.nPPPFlag = LCP_PPP_UNKEEPED;//短连接
		sprintf(tmp_stPPPCfg.szApn, "+CGDCONT=1,\"IP\",\"%s\"", WLMAPN);//sprintf(tmp_stPPPCfg.szApn, "\"%s\"", WLMAPN);
#else
		tmp_stPPPCfg.nPPPFlag = LCP_PPP_UNAUTOCONNECT; //设置不自动重连
		sprintf(tmp_stPPPCfg.szApn, WLMAPN);
#endif
		tmp_stPPPCfg.ModemDial = NULL;
		//tmp_stPPPCfg.PPPIntervalTimeOut = 600;
	}
	
#if 0
	if((ret=NDK_PppSetCfg(&tmp_stPPPCfg, sizeof(tmp_stPPPCfg)))!=NDK_OK)//if((ret=NetPPPCfg(&tmp_stPPPCfg, 0))<0)
	{
		//cls_show_msg1(10, "%s, line %d:链路层参数配置失败(%d)", __FILE__, __LINE__, ret);
		return ret;//FAIL;
	}
#else
	return NDK_PppSetCfg(&tmp_stPPPCfg, sizeof(tmp_stPPPCfg));
#endif
}

//APN,USR/PWD是在拔入PPP时使用的参数
//APN为GPRS、TD拔入时认证使用,USR/PWD为CDMA、ASYN拔入时认证使用
static int conf_PPP(void)
{
	/*private & local definition*/
	int ret = 0;
	uint len = 0;
	LinkType type = getlinktype();	
	char pInfo[64] = {0};
	char wlm_apn[64] = {0}, ppp_usr[8] = {0}, ppp_pwd[8] = {0};

	/*process body*/
	if(g_conf_flag)
	{
		if(type==GPRS||type==TD)
		{
			if(GetParaInfo("WLMAPN",pInfo)!=SUCC)
				return FAIL;
			if(trim_string(pInfo,wlm_apn)!=SUCC)
				return FAIL;
			strcpy(WLMAPN,wlm_apn);
		}
		if(type!=ASYN)
		{
			if(GetParaInfo("PPPUSR",pInfo)!=SUCC)
				return FAIL;
			if(trim_string(pInfo,ppp_usr)!=SUCC)
				return FAIL;
			strcpy(PPPUSR,ppp_usr);
			if(GetParaInfo("PPPPWD",pInfo)!=SUCC)
				return FAIL;
			if(trim_string(pInfo,ppp_pwd)!=SUCC)
				return FAIL;
			strcpy(PPPPWD,ppp_pwd);
		}
		cls_show_msg1(1,"PPP参数配置成功");
	}
	else
	{
		if(type==GPRS||type==TD)
		{
			cls_printf("当前APN:\n");
			//if ((ret=getreturnline(gprsAPN, sizeof(gprsAPN)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default    
			if ((ret=lib_kbgetinput(WLMAPN, 0, sizeof(WLMAPN)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT/*INPUT_CONTRL_LIMIT*/))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
				strcpy(WLMAPN, DEFAULT_WLM_APN);//strcpy(gprsAPN, DEFAULT_WLM_APN);
		}
		//以下为PPP配置共有的
		if(type!=ASYN)//ASYN还是暂不配置,直接写死.即使配置的话,在PppSetCfg中也会被写死
		{
		cls_printf("当前PPP用户名:\n");
		//if ((ret=getreturnline(gprsusr, sizeof(gprsusr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default    
		if ((ret=lib_kbgetinput(PPPUSR, 0, sizeof(PPPUSR)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT/*INPUT_CONTRL_LIMIT*/))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			strcpy(PPPUSR, (type==ASYN)?DEFAULT_ASYN_USR:DEFAULT_PPP_USR);//strcpy(gprsusr, DEFAULT_WLM_USR);

		cls_printf("当前PPP密码:\n");
		//if ((ret=getreturnline(gprspwd, sizeof(gprspwd)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default    
		if ((ret=lib_kbgetinput(PPPPWD, 0, sizeof(PPPPWD)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT/*INPUT_CONTRL_LIMIT*/))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			strcpy(PPPPWD, (type==ASYN)?DEFAULT_ASYN_PWD:DEFAULT_PPP_PWD);//strcpy(gprspwd, DEFAULT_WLM_PWD);
		}
	}
	if((ret=PppSetUsrCfg())!=SUCC)
	{
		cls_show_msg1(10, "%s, line %d:PPP参数配置失败(%d)", __FILE__, __LINE__, ret);
		return ret;//FAIL;
	}
		
	return SUCC;
}

int conf_conn_PPP(void)
{
	/*process body*/
	int nKeyin = 0, nkeyin1 = ENTER, i = 0;
	LinkType type[] = {GPRS, CDMA, ASYN, TD};
	char *typestr[] = {"GPRS", "CDMA", "ASYN", "TD"};
	int enable_flag[] ={PPP_ENABLE, PPP_ENABLE, ASYN_ENABLE, TD_ENABLE};
	int conf_flag = 0;	
	char pInfo[64] = {0}, temptype[16] = {0};
	
	/*process body*/	
	if((nkeyin1=cls_show_msg1(WAITTIME,"[Enter]使用参数配置文件,[其他]手动配置"))==ENTER || nkeyin1==0)
	{
		g_conf_flag = 1;
		if(GetParaInfo("WLMType",pInfo)!=SUCC)
			return FAIL;
		if(trim_string(pInfo,temptype)!=SUCC)
				return FAIL;
		for(i=0;i<SZ_ARRAY(typestr);i++)
		{
			if(!strcmp(temptype,typestr[i]))
			{
				if(type[i]==ASYN)
				{
					setlinktype(ASYN);
					strcpy(MDMDialStr, DEFAULT_MDM_DIAL_STR);//直接写死,不必调用conf_link
				}
				else
				{
					if(conf_link(type[i])!=SUCC)
						return FAIL;
				}
				break;
			}
		}
		if(i==SZ_ARRAY(typestr))
		{
			cls_show_msg("file:%s,line:%d参数配置文件中通讯类型配置错误)",__FILE__ ,__LINE__);
			return FAIL;
		}
    }
	else
	{
		g_conf_flag = 0;
		do
		{
			nKeyin = cls_show_msg("选择连接方式:\n"
									"1.GPRS 2.CDMA\n"
									"3.ASYN 4.TD\n");
#define MAXMENUITEM ('4')

			conf_flag = enable_flag[nKeyin-'1'];//赋值配置标志
			switch (nKeyin)
			{
			//case 0://0为超时,默认启用GPRS
			case '1':
			case '2':
			case MAXMENUITEM://'4':
				if(conf_flag)//支持标志为1说明支持则开始配置,否则直接提示不支持后退出
				{
					if(conf_link(type[nKeyin-'1'])!=SUCC)
						return FAIL;
					break;
				}
				else
				{
					cls_show_msg1(10, "本产品不支持本连接方式,请重新选择");
					return NDK_ERR_QUIT;
				}
			case '3':
				if(conf_flag)
				{
					setlinktype(type[nKeyin-'1']);//linktype = ASYN;
					strcpy(MDMDialStr, DEFAULT_MDM_DIAL_STR);//直接写死,不必调用conf_link
					break;
				}
				else
				{
					cls_show_msg1(10, "本产品不支持本连接方式,请重新选择");
					return NDK_ERR_QUIT;
				}
			case ESC:
				return NDK_ERR_QUIT;//QUIT;
				break;
			default:
				cls_show_msg1(1, "若要退出,请按ESC...");
				continue;
				break;
			}
		}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM	//使用完就近释放定义
	}
	return SUCC;
}
#else
int PppSetUsrCfg(void){return FAIL;}
static int conf_PPP(void){return FAIL;}
int conf_conn_PPP(void){return FAIL;}
#endif

#if (SDLC_ENABLE||ASYN_ENABLE)
static int conf_MDM_lnk(void)
{
	/*private & local definition*/
	int ret = 0;
	uint len = 0;
	char tmp[2] = {0};
	char pInfo[64] = {0}, dialstr[32] = {0};
	int typenum = 0;
	LinkType type = getlinktype();

	/*process body*/

	if(g_conf_flag)
	{	
		if(GetParaInfo("MDMPatchType",pInfo)!=SUCC)
				return FAIL;
		if(trim_string2int(pInfo,&typenum)!=SUCC)
				return FAIL;
		MDMPatchType=typenum;
		if(type==SYNC)
		{
			if(GetParaInfo("SYNCDialStr",pInfo)!=SUCC)
				return FAIL;
			if(trim_string(pInfo,dialstr)!=SUCC)
				return FAIL;
			strcpy(MDMDialStr,dialstr);
		}
		else
		{
			if(GetParaInfo("ASYNDialStr",pInfo)!=SUCC)
				return FAIL;
			if(trim_string(pInfo,dialstr)!=SUCC)
				return FAIL;
			strcpy(MDMDialStr,dialstr);
		}
		cls_show_msg1(1,"MDM配置成功");
	}
	else
	{
		// 1.国家选择/补丁选择
		cls_printf("当前补丁号(%d-%d):\n", MDM_PatchType1, MDM_PatchType5);
		sprintf(tmp, "%d", MDMPatchType);
		//if ((ret=getnumstr(tmp, sizeof(tmp)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default
		if ((ret=lib_kbgetinput(tmp, 0, sizeof(tmp)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			MDMPatchType = MDM_PatchType5;
		else
			MDMPatchType = atoi(tmp);
		
		// 2.设置拔号串
		cls_printf("当前拨号串:\n");
		//if ((ret=getreturnline(MDMDialStr, sizeof(MDMDialStr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default   //由使用getnumstr改用getline/getreturnline,以可以输入字母
		if ((ret=lib_kbgetinput(MDMDialStr, 0, sizeof(MDMDialStr)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			strcpy(MDMDialStr, DEFAULT_MDM_DIAL_STR);
	}
	// 3.(异步)PPP参数配置
	//由于这些参数配置只用于异步网络应用中(且可用参数一般是固定的),目前暂不放在此处配置,在需要的地方直接写死即可

	return SUCC;
}

int sdlc_init_flag = 0;
int g_sdlc_bps = EM_MDM_BPS_1200;
#if defined SP50||defined SP80||defined GP720//SP5080的环境已经更新
int set_sdlc_bps(void)
{
	/*private & local definition*/
	int nKeyin = 0;
	char pInfo[64] = {0};
	
	/*process body*/
	if(g_conf_flag==1)
	{
		if(GetParaInfo("MDMBps",pInfo)!=SUCC)
			return FAIL;
		if(trim_string2int(pInfo,&g_sdlc_bps)!=SUCC)
			return FAIL;
		if(g_sdlc_bps<EM_MDM_BPS_1200||g_sdlc_bps>EM_MDM_BPS_9600)
			g_sdlc_bps=EM_MDM_BPS_1200;
	}
	else
	{
		nKeyin = cls_show_msg("设置的波特率\n"
								"1.1200\n"
								"2.2400\n"
								"3.9600\n");
		switch(nKeyin)
		{
		case '1':
		case '2':
		case '3':
			g_sdlc_bps = nKeyin-'0';
			break;
		default:
		case ESC:
			g_sdlc_bps = EM_MDM_BPS_1200;
			break;
		}
	}
	
	return SUCC;
}
#else
int set_sdlc_bps(void)
{
	return SUCC;
}
#endif

int conf_conn_MDM(void)
{
	/*private & local definition*/
	int nKeyin = 0, nkeyin1 = ENTER;
	LinkType type[] = {SYNC, ASYN};
	char pInfo[64] = {0}, temptype[16] = {0};
	
	/*process body*/
	if((nkeyin1=cls_show_msg1(WAITTIME,"[Enter]使用参数配置文件,[其他]手动配置"))==ENTER || nkeyin1==0)
	{
		g_conf_flag = 1;
		if(GetParaInfo("MDMType",pInfo)!=SUCC)
			return FAIL;
		if(trim_string(pInfo,temptype)!=SUCC)
			return FAIL;
		if(!strcmp(temptype,"SYNC"))
		{
			if(conf_link(SYNC)!=SUCC)
				return FAIL;
		}
		else if(!strcmp(temptype,"ASYN"))
		{
			if(conf_link(ASYN)!=SUCC)
				return FAIL;
		}
		else
		{
			cls_show_msg("file:%s,line:%dModem配置类型错误)",__FILE__ ,__LINE__);
			return FAIL; 
		}
	}
	else
	{
		g_conf_flag = 0;
		do
	    {
	        nKeyin = cls_show_msg("选择连接方式:\n"
									"1.同步MODEM\n"
									"2.异步MODEM\n");
#define MAXMENUITEM ('2')
			switch(nKeyin) 
			{
			case '1':
			case MAXMENUITEM://'2':
				if(conf_link(type[nKeyin-'1'])!=SUCC)//配置MODEM 拨号参数
					return FAIL;
				break;
			case ESC:
				return NDK_ERR_QUIT;//QUIT;
				break;
			default:
				continue;
				break;
			}
		}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM

		nkeyin1 = 0;
		if((nkeyin1=cls_show_msg1(WAITTIME,"[Enter]默认初始化函数,[其他]调用带波特率设置的初始化函数"))==ENTER || nkeyin1==0)
			sdlc_init_flag=0;
		else
			sdlc_init_flag=1;
			
	}

	if(getlinktype()==SYNC&&sdlc_init_flag==1)
		set_sdlc_bps();
	
	return SUCC;
}
#else
static int conf_MDM_lnk(void){return FAIL;}
int conf_conn_MDM(void){return FAIL;}
#endif
#endif

#if !defined ME66 //ME66 空间小 不编译
int select_serial(void)
{
	/*private & local definition*/
	int nKeyin = 0, port = 0;

	/*process body*/
	while(1)
	{
#if COMNUM==3
#if defined GP710
	nKeyin = cls_show_msg("请选择串口:\n①手机串口1\n②座机串口1\n③座机串口2\n");
#else
	nKeyin = cls_show_msg("请选择串口:\n①串口1 ②串口2\n③串口3\n");
#endif
#elif COMNUM==2
	nKeyin = cls_show_msg("请选择串口:\n①串口1 ②串口2\n");
#else //默认只有一个串口
#if defined ME15C
	nKeyin = cls_show_msg("请选择串口:\n①串口2\n");
#else
	nKeyin = cls_show_msg("请选择串口:\n①串口1\n");
#endif
#endif

	switch (nKeyin)
	{
	case '1':
#if defined ME15C
		port = PORT_NUM_COM2;
#else
		port = PORT_NUM_COM1;		
#endif
		break;
#if COMNUM>1
	case '2':
		port = PORT_NUM_COM2;
		break;
#endif
#if COMNUM>2
	case '3':
		port = PORT_NUM_COM3;
		break;
#endif
	default:
		continue;
		break;
	}
	return port;
	}
}

char *conf_serial(int *bpsid)
{
	/*private & local definition*/
	/*const */char *menulst[] =
	{
		"1.300   2.1200\n3.2400  4.4800\n5.9600  6.19200\n7.38400 8.翻页\n",
		"1.57600 2.115200\n3.230400\n\n        8.翻页\n"
	};
	/*const */char *baud[] = 
	{	
		"300","1200","2400","4800","9600","19200","38400","57600","115200","230400"
	};
	int page = 0, baudid = 0, maxpage = SZ_ARRAY(menulst),  nKeyin = 0;

	/*process body*/
	while(1)
	{
		nKeyin = cls_show_msg1(MAXWAITTIME, menulst[page]);
		if (nKeyin=='8')
		{
			if (++page==maxpage)
				page = 0;
		}
		else if (nKeyin>='1' && nKeyin<='7')
		{
			if ((baudid=page*(8-1)+nKeyin-'0'-1)>=SZ_ARRAY(baud))
				continue;
						
			break;
		}
		else if (nKeyin==ESC||nKeyin==0)//超时for自动测试
		{
			baudid = 4;//9600 id
			break;
		}
		else
			continue;
	}
	
	if(bpsid!=NULL)
		*bpsid = baudid;
	cls_show_msg1(2, "BPS将设置为%s...", baud[baudid]);
	return baud[baudid];//return baudlst[baudid];//g_bps = baudlst[baudid];
}

#if !(defined ME15C||defined ME15CHX||defined ME20||defined ME15B)	//ME15C为节省应用空间不编译不用的函数
/*
conf一族函数的设计(另外,conf一族函数仅做配置动作,改变linktype的事不由conf负责):
step1:先判断type,不合理/不支持,直接return(提高了执行效率)
step2:对合理的type,再次提示并确认后,转到各分支处理
*/
int conf_link(LinkType type)
{
	/*private & local definition*/
	int bpsid = 0;
#if DOUBLESIM_ENABLE
	int ret = -1;
	char pInfo[128] = {0};
	int simslot = 0;
#endif

	
	/*process body*/
	setlinktype(type);
	switch(type)
	{
	case ETH://暂无需配置,直接返回SUCC
#if defined ME50C
		NDK_NetAddRouterTable(COMM_TYPE_ETH,"0.0.0.0");
#endif
		return SUCC;
		break;
	//需要配置的都放到此行始,再次确认后,转各自处理
	case GPRS:
	case CDMA:
#if DOUBLESIM_ENABLE
		if(g_conf_flag==1)
		{
			//读配置文件中的SIM卡槽标志
			if(GetParaInfo("SIMSLOT",pInfo)!=SUCC)
				return FAIL;
			if(trim_string2int(pInfo,&simslot)!=SUCC)
				return FAIL;
			SIMSLOT = simslot;
		}
		else
		{
			//手动配置sim 卡槽标志，并调用set函数
			ret = cls_show_msg("选择SIM卡槽:\n1.SIM1卡槽\n2.SIM2卡槽\n3.不设置");
			if(ret=='1')
				SIMSLOT = SIM_SLOT1;
			else if(ret=='2')
				SIMSLOT = SIM_SLOT2;
			else if(ret=='3')
				SIMSLOT = SIM_SLOT_UNKNOW;
		}
		if(SIMSLOT==SIM_SLOT1||SIMSLOT==SIM_SLOT2)
		{
			cls_printf("切换到无线卡槽%d中", SIMSLOT+1);
			if((ret=NDK_WlSetSimSlot(SIMSLOT))!=NDK_OK)
			{
				cls_show_msg1(10, "%s, line %d:设置无线无线卡槽%d失败(ret=%d)", __FILE__, __LINE__, SIMSLOT+1, ret);
				return FAIL;
			}
		}
#endif
	case TD:
#if defined E90
		cls_printf("双模切换中...");
		//双模切换不需要是否配置的确认,故放在配置确认之前
		//也不必先取状态再决定是否设置,因为若当前已是欲设置的状态了,即使去设置一下,也会很快返回,不影响效率
		if((type==GPRS && (WLM_SetNetMode(E_NETWORK_TYPE_GPRS_E)!=SUCC ||(ret=WLM_GetNetStatus_Ex(E_MMI_GMMREG_RAT_GPRS))!=E_MMI_GMMREG_RAT_GPRS))
		||(type==TD && (WLM_SetNetMode(E_NETWORK_TYPE_TD_E)!=SUCC||(ret=WLM_GetNetStatus_Ex(E_MMI_GMMREG_RAT_3G))!=E_MMI_GMMREG_RAT_3G)))
		{
			cls_show_msg1(10, "%s, line %d:设置无线网络模式失败(%d)", __FILE__, __LINE__, ret);
			return FAIL;
		}
#endif
	case ASYN:
	case SYNC:
	case SERIAL:
	case BT:
	case WLAN:
		if(!g_conf_flag)
		{
			if(ENTER!=cls_show_msg1(MAXWAITTIME, "重新配置链路参数吗?\n"                                                   
													"[确认]是[其它]否\n"))
				return SUCC;	
		}
		switch(type)//再次根据type进行分发处理
		{
		case ASYN:
			if(conf_MDM_lnk()!=SUCC)
				return FAIL;
			return PppSetUsrCfg();//return conf_PPP();
			break;			
		case SYNC:
			return conf_MDM_lnk();
			break;
		case SERIAL:
			strcpy(g_bps, conf_serial(&bpsid));//g_bps = conf_serial();
			break;
		case BT:
			return conf_bt();
			break;
		case GPRS:
		case CDMA:
		case TD:
#if CPU5810X_ENABLE||defined ME50NX||defined ME68//50和50N需要调用该函数设置路由表
			NDK_NetAddRouterTable(COMM_TYPE_PPP,"0.0.0.0");
#endif
			return conf_PPP();
			break;
		case WLAN:
#if CPU5810X_ENABLE||defined ME50NX||defined ME68
			NDK_NetAddRouterTable(COMM_TYPE_WIFI,"0.0.0.0");
#endif
			if(g_conf_flag)
				return use_wifi_conf(0);
			else
				return conf_WLAN_lnk();
			break;
		default://never reach here
			break;
		}
		return SUCC;
		break;
	default:
		cls_show_msg1(10, "%s, line %d:未知的链路类型(%d)", __FILE__, __LINE__, type);
		return FAIL;
		break;
	}

	return SUCC;
}
#else
int conf_link(LinkType type)//ME15C空间限制且仅支持串口和蓝牙,故缩减配置函数
{
	/*private & local definition*/
	int bpsid = 0;
	
	/*process body*/
	setlinktype(type);
	switch(type)
	{
		case SERIAL:
		case BT:
			if(!g_conf_flag)
			{
				if(ENTER!=cls_show_msg1(MAXWAITTIME, "重新配置链路参数吗?\n"                                                   
														"[确认]是[其它]否\n"))
					return SUCC;	
			}
			switch(type)//再次根据type进行分发处理
			{
				case SERIAL:
					strcpy(g_bps, conf_serial(&bpsid));//g_bps = conf_serial();
					break;
				case BT:
					return conf_bt();
					break;
				default://never reach here
					break;
			}
			return SUCC;
			break;
		default:
			cls_show_msg1(10, "%s, line %d:未知的链路类型(%d)", __FILE__, __LINE__, type);
			return FAIL;
			break;
	}
	return SUCC;
}
#endif
#endif

#if !(defined ME15C||defined ME15CHX||defined ME20||defined ME15B||defined ME66)	//ME15C为节省应用空间不编译不用的函数
int conf_local(void)
{
	/*private & local definition*/
	int ret = 0;
	uint len = 0;
	LinkType type = getlinktype();
	char pInfo[64] = {0};
	char localip[16] = {0}, localmask[16] = {0}, localgateway[16] = {0};
	int dhcpflag = 0;

	/*process body*/
	switch(type)
	{
	//需要配置的都放到此行始,再次确认后,转各自处理
	case ETH:
	case WLAN:
		if(g_conf_flag)//从配置文件中读取并导入
		{
			if(type == ETH)
			{
				if(GetParaInfo("EthDHCPenable",pInfo)!=SUCC)
					return FAIL;
				if(trim_string2int(pInfo,&dhcpflag)!=SUCC)
					return FAIL;
				if(dhcpflag)//为1 使能 使用自动分配本地地址
				{
					netsetting.DHCPenable = ENABLE;
				}
				else  //为0需要从配置文件中读取本地地址
				{
					netsetting.DHCPenable = DISABLE;
					if(GetParaInfo("EthLocalIP",pInfo)!=SUCC)
						return FAIL;
					if(trim_string(pInfo,localip)!=SUCC)
						return FAIL;
					strcpy(netsetting.LocalIP.Str,localip);
					if(GetParaInfo("EthLocalMask",pInfo)!=SUCC)
						return FAIL;
					if(trim_string(pInfo,localmask)!=SUCC)
						return FAIL;
					strcpy(netsetting.LocalMask.Str,localmask);
					if(GetParaInfo("EthLocalGatway",pInfo)!=SUCC)
						return FAIL;
					if(trim_string(pInfo,localgateway)!=SUCC)
						return FAIL;
					strcpy(netsetting.LocalGateway.Str,localgateway);		
					netsetting.LocalIP.value.data = INET_ADDR(netsetting.LocalIP.Str);
					netsetting.LocalMask.value.data = INET_ADDR(netsetting.LocalMask.Str);
					netsetting.LocalGateway.value.data = INET_ADDR(netsetting.LocalGateway.Str);
					cls_show_msg1(1,"本地网络参数配置成功");
				}
			}
			else
			{
				if(GetParaInfo("WifiDHCPenable",pInfo)!=SUCC)
					return FAIL;
				if(trim_string2int(pInfo,&dhcpflag)!=SUCC)
					return FAIL;
				if(dhcpflag)//为1 使能 使用自动分配本地地址
				{
					netsetting.DHCPenable = ENABLE;
				}
				else  //为0需要从配置文件中读取本地地址
				{
					netsetting.DHCPenable = DISABLE;
					if(GetParaInfo("WifiLocalIP",pInfo)!=SUCC)
						return FAIL;
					if(trim_string(pInfo,localip)!=SUCC)
						return FAIL;
					strcpy(netsetting.LocalIP.Str,localip);
					if(GetParaInfo("WifiLocalMask",pInfo)!=SUCC)
						return FAIL;
					if(trim_string(pInfo,localmask)!=SUCC)
						return FAIL;
					strcpy(netsetting.LocalMask.Str,localmask);
					if(GetParaInfo("WifiLocalGatway",pInfo)!=SUCC)
						return FAIL;
					if(trim_string(pInfo,localgateway)!=SUCC)
						return FAIL;
					strcpy(netsetting.LocalGateway.Str,localgateway);	
					netsetting.LocalIP.value.data = INET_ADDR(netsetting.LocalIP.Str);
					netsetting.LocalMask.value.data = INET_ADDR(netsetting.LocalMask.Str);
					netsetting.LocalGateway.value.data = INET_ADDR(netsetting.LocalGateway.Str);
					cls_show_msg1(1,"本地网络参数配置成功");
				}
			}
		}
		else  //手动配置
		{
			if(ENTER!=cls_show_msg1(MAXWAITTIME, "重新配置本地网络参数吗?\n"
													"[确认]是[其它]否\n"))
				return SUCC;
			if(ENTER==cls_show_msg1(MAXWAITTIME, "_选择配置方式_\n"
													"[确认]静态配置\n"
													"[其它]动态配置\n"))
			{
				cls_printf("当前本地IP:\n");
				//if ((ret=getnumstr(netsetting.LocalIP.Str, sizeof(netsetting.LocalIP.Str)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default      
				if ((ret=lib_kbgetinput(netsetting.LocalIP.Str, 0, sizeof(netsetting.LocalIP.Str)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
					strcpy(netsetting.LocalIP.Str, DEFAULT_LOCAL_IP_STR);       

				netsetting.LocalIP.value.data = INET_ADDR(netsetting.LocalIP.Str);
									
				cls_printf("当前本地掩码:\n");
				//if ((ret=getnumstr(netsetting.LocalMask.Str, sizeof(netsetting.LocalMask.Str)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default    
				if ((ret=lib_kbgetinput(netsetting.LocalMask.Str, 0, sizeof(netsetting.LocalMask.Str)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
					strcpy(netsetting.LocalMask.Str, DEFAULT_LOCAL_MASK_STR);
				
				netsetting.LocalMask.value.data = INET_ADDR(netsetting.LocalMask.Str);
								
				cls_printf("当前本地网关:\n");
				//if ((ret=getnumstr(netsetting.LocalGateway.Str, sizeof(netsetting.LocalGateway.Str)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default 
				if ((ret=lib_kbgetinput(netsetting.LocalGateway.Str, 0, sizeof(netsetting.LocalGateway.Str)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
					strcpy(netsetting.LocalGateway.Str, DEFAULT_LOCAL_GATEWAY_STR);
			 
				netsetting.LocalGateway.value.data = INET_ADDR(netsetting.LocalGateway.Str);
				netsetting.DHCPenable = DISABLE;			
			}
			else
				netsetting.DHCPenable = ENABLE;
		}
		return SUCC;
		break;
	//自动获取(NETUP时),无需手工配置,直接return
	case GPRS:
	case CDMA:
	case TD:
	case ASYN:
		return SUCC;
		break;
	case SERIAL:
	case BT:
	case SYNC:
		return FAIL;
		break;
	default:
		cls_show_msg("%s, line %d:未知的本地网络类型(%d)", __FILE__, __LINE__, type);
		return FAIL;
		break;
	}

	return SUCC;
}

int conf_server(void)
{
	/*private & local definition*/
	char tmp[8] = {0};
	int ret = 0;
	uint len = 0;
	LinkType type = getlinktype();	
	char pInfo[64] = {0}, svrip[16] = {0};
	int tcpport = 0, udpport = 0;  

	/*process body*/
	switch(type)
	{
	//需要配置的都放到此行始,再次确认后,转各自处理
	case GPRS:
	case CDMA:
	case TD:
	case ETH:
	case ASYN:
	case WLAN:
		if(g_conf_flag)
		{
			if(type==GPRS || type==CDMA ||type==TD || type==ASYN)
			{
				if(GetParaInfo("WlmSvrIP",pInfo)!=SUCC)
					return FAIL;
				if(trim_string(pInfo,svrip)!=SUCC)
					return FAIL;
				strcpy(netsetting.SvrIP.Str,svrip);
				if(GetParaInfo("WlmSvrTCP_PORT",pInfo)!=SUCC)
					return FAIL;
				if(trim_string2int(pInfo,&tcpport)!=SUCC)
					return FAIL;
				netsetting.SvrTcpPort=(ushort)tcpport;
				if(GetParaInfo("WlmSvrUDP_PORT",pInfo)!=SUCC)
					return FAIL;
				if(trim_string2int(pInfo,&udpport)!=SUCC)
					return FAIL;
				netsetting.SvrUdpPort=(ushort)udpport;
			}
			else if(type==ETH)
			{
				if(GetParaInfo("EthSvrIP",pInfo)!=SUCC)
					return FAIL;
				if(trim_string(pInfo,svrip)!=SUCC)
					return FAIL;
				strcpy(netsetting.SvrIP.Str,svrip);
				if(GetParaInfo("EthSvrTCP_PORT",pInfo)!=SUCC)
					return FAIL;
				if(trim_string2int(pInfo,&tcpport)!=SUCC)
					return FAIL;
				netsetting.SvrTcpPort=(ushort)tcpport;
				if(GetParaInfo("EthSvrUDP_PORT",pInfo)!=SUCC)
					return FAIL;
				if(trim_string2int(pInfo,&udpport)!=SUCC)
					return FAIL;
				netsetting.SvrUdpPort=(ushort)udpport;
			}
			else 
			{
				if(GetParaInfo("WifiSvrIP",pInfo)!=SUCC)
					return FAIL;
				if(trim_string(pInfo,svrip)!=SUCC)
					return FAIL;
				strcpy(netsetting.SvrIP.Str,svrip);
				if(GetParaInfo("WifiSvrTCP_PORT",pInfo)!=SUCC)
					return FAIL;
				if(trim_string2int(pInfo,&tcpport)!=SUCC)
					return FAIL;
				netsetting.SvrTcpPort=(ushort)tcpport;
				if(GetParaInfo("WifiSvrUDP_PORT",pInfo)!=SUCC)
					return FAIL;
				if(trim_string2int(pInfo,&udpport)!=SUCC)
					return FAIL;
				netsetting.SvrUdpPort=(ushort)udpport;
			}			
			cls_show_msg1(1,"服务器参数配置成功");
		}
		else
		{
			if(ENTER!=cls_show_msg1(MAXWAITTIME, "重新配置服务器参数吗?\n"                                                   
													"[确认]是[其它]否\n"))
				return SUCC;                                                                                                                       
			
			//网络公共配置
			cls_printf("当前服务器IP:\n");
			//if ((ret=getnumstr(netsetting.SvrIP.Str, sizeof(netsetting.SvrIP.Str)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default
			if ((ret=lib_kbgetinput(netsetting.SvrIP.Str, 0, sizeof(netsetting.SvrIP.Str)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
				strcpy(netsetting.SvrIP.Str, DEFAULT_SVR_IP_STR);
			
			netsetting.SvrIP.value.data = INET_ADDR(netsetting.SvrIP.Str);
					
			//itoa(netsetting.SvrPort, tmp, 10);                                         
			cls_printf("当前服务器TCP端口:\n");
			sprintf(tmp, "%d", netsetting.SvrTcpPort);
			//if ((ret=getnumstr(tmp, sizeof(tmp)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default
			if ((ret=lib_kbgetinput(tmp, 0, sizeof(tmp)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
				netsetting.SvrTcpPort = DEFAULT_SVR_TCP_PORT;	                     
			else                                                                         
				netsetting.SvrTcpPort = atoi(tmp);
		                                                                                                     
			cls_printf("当前服务器UDP端口:\n");
			sprintf(tmp, "%d", netsetting.SvrUdpPort);
			//if ((ret=getnumstr(tmp, sizeof(tmp)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default
			if ((ret=lib_kbgetinput(tmp, 0, sizeof(tmp)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
				netsetting.SvrUdpPort = DEFAULT_SVR_UDP_PORT;
			else                                                                         
				netsetting.SvrUdpPort = atoi(tmp);
		}
		return SUCC;
		break;
	case SERIAL:
	case BT:
	case SYNC:	
		return FAIL;
		break;
	default:
		cls_show_msg1(10, "%s, line %d:未知的网络类型(%d)", __FILE__, __LINE__, type);
		return FAIL;
		break;
	}
	
	return SUCC;
}

int conf_network(void)
{
 	/*private & local definition*/

 	/*process body*/
	if(conf_local()!=SUCC)
 		return FAIL;
 	if(conf_server()!=SUCC)
 		return FAIL;

	return SUCC;
}

#if NDKSSL_ENABLE
static void conf_SSL(void)
{
 	/*private & local definition*/
 	int nKeyin = 0;

 	/*process body*/
	//EM_SSL_HANDSHAKE_OPT
	do
	{
		nKeyin = cls_show_msg("SSL握手选项:\n"
								"1.SSLv2 2.SSLv23\n"
								"3.SSLv3 4.TLSv1\n");
#define MAXMENUITEM ('4')
		
		switch (nKeyin)
		{
		case ESC:
		case '1':
			cls_show_msg1(2, "将使用SSLv2...");
			netsetting.SSLHsOpt = HANDSHAKE_SSLv2;
			break;
		case '2':
			cls_show_msg1(2, "将使用SSLv23...");
			netsetting.SSLHsOpt = HANDSHAKE_SSLv23;
			break;
		case '3':
			cls_show_msg1(2, "将使用SSLv3...");
			netsetting.SSLHsOpt = HANDSHAKE_SSLv3;
			break;
		case MAXMENUITEM://'4':
			cls_show_msg1(2, "将使用TLSv1...");
			netsetting.SSLHsOpt = HANDSHAKE_TLSv1;
			break;
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;
		}
	}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM

 	//EM_SSL_AUTH_OPT
	do
	{
		nKeyin = cls_show_msg("SSL认证选项:\n"
								"1.AUTH_NONE\n"
								"2.AUTH_CLIENT\n"
								//不开放配置"3.AUTH_CLI_FAIL\n"
								);
#define MAXMENUITEM ('2')		
		
		switch (nKeyin)
		{
		case ESC:
		case '1':
			cls_show_msg1(2, "将使用AUTH_NONE...");
			netsetting.SSLAuthOpt = SSL_AUTH_NONE;
			break;
		case MAXMENUITEM://'2':
			cls_show_msg1(2, "将使用AUTH_CLIENT...");
			netsetting.SSLAuthOpt = SSL_AUTH_CLIENT;
			break;
#if 0
		case '3':
			cls_show_msg1(2, "将使用AUTH_CLIENT_FAIL...");
			netsetting.SSLAuthOpt = SSL_AUTH_CLIENT_FAIL;
			break;
#endif
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;
		}
	}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM	
 	
 #if 0
 	//EM_SSL_FILE_FORMAT
	do
	{
		nKeyin = cls_show_msg("SSL证书格式:\n"
								"1.PEM 2.DER\n");
		
		switch (nKeyin)
		{
		case ESC:
		case '1':
			cls_show_msg1(2, "将使用PEM格式...");
			netsetting.SSLFileFmt = FILE_PEM;
			break;
		case '2':
			cls_show_msg1(2, "将使用DER格式...");
			netsetting.SSLFileFmt = FILE_DER;
			break;
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;
		}
	}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>'2'));
#else	//死成,不影响测试
	netsetting.SSLFileFmt = SSL_FILE_PEM;
 #endif
}
#else
static void conf_SSL(void){}
#endif

//从设计上看,conf_trans一般多用于综合测试,而很少用于单元测试.因为它的层次太高了
#if (NDKSOCK_ENABLE||NDKSSL_ENABLE)
int conf_trans(void)
{
 	/*private & local definition*/
 	char *sockname[] = {"NDKTCP", "NDKUDP", "NDKSSL", NULL};
	sock_t socktype[] = {SOCK_NDKTCP, SOCK_NDKUDP, SOCK_NDKSSL};
 	int i = 0, nKeyin = 0;
 	LinkType type = getlinktype();
	char pInfo[64] = {0}, temptype[8] = {0};

 	/*process body*/
	switch(type)
	{
	//需要配置的都放到此行始,再次确认后,转各自处理
	case ETH:
	case GPRS:
	case CDMA:
	case TD:
	case ASYN:
	case WLAN:
		if(g_conf_flag)
		{
			if(GetParaInfo("SockType",pInfo)!=SUCC)
				return FAIL;
			if(trim_string(pInfo,temptype)!=SUCC)
				return FAIL;
			for(i=0;i<(SZ_ARRAY(sockname)-1);i++)
			{
				if(!strcmp(temptype,sockname[i]))
				{
					netsetting.socktype = socktype[i];
				}
				break;
			}
			if(i==(SZ_ARRAY(sockname)-1))
			{
				
				cls_show_msg("file:%s,line:%d参数配置文件中传输层参数配置类型错误)",__FILE__ ,__LINE__);
				return FAIL; 
			}
			cls_show_msg1(1,"传输层参数配置成功");
		}
		else
		{
			if(ENTER!=cls_show_msg1(MAXWAITTIME, "重新配置传输层参数?\n"                                                   
													"[确认]是[其它]否\n"))
				return SUCC;

			do
			{
				NDK_ScrClrs();//clrscr();
				i = 0;
				while(sockname[i])
				{
					if(netsetting.socktype==(i))
						NDK_ScrSetAttr(TEXT_ATTRIBUTE_REVERSE, NULL);//settextattr(REVERSE_TEXT);
					
					NDK_ScrPrintf("%d.%s\n", i+1, sockname[i]);//printf("%d.%s\n", i+1, sockname[i]);

					if(netsetting.socktype==(i))
						NDK_ScrSetAttr(TEXT_ATTRIBUTE_NORMAL, NULL);//settextattr(NORMAL_TEXT);
					
					i++;
				}
				NDK_ScrRefresh();

				nKeyin = lib_getch();//getch();
				if(nKeyin>'0' && nKeyin<'0'+SZ_ARRAY(sockname))
					netsetting.socktype = (sock_t)(nKeyin-'1');
			}while(nKeyin!=ESC && nKeyin!=ENTER);
		}

		if(netsetting.socktype==SOCK_NDKSSL&& ENTER==cls_show_msg1(MAXWAITTIME, "重新配置SSL层参数?\n"                                                   
												"[确认]是[其它]否\n"))
			conf_SSL();
		
		return SUCC;
		break;
	case SERIAL:
	case BT:
	case SYNC:
		return FAIL;
		break;
	default:
		cls_show_msg1(10, "%s, line %d:未知的本地网络类型(%d)", __FILE__, __LINE__, type);
		return FAIL;
		break;
	}

	return SUCC;
}
#else
int conf_trans(void){return FAIL;}
#endif

#if ETH_ENABLE
int conf_conn_ETH(int IsTransConfed)
{
	/*private & local definition*/
	int nkeyin = ENTER;
	
	/*process body*/
	if((nkeyin=cls_show_msg1(WAITTIME,"[Enter]使用参数配置文件,[其他]手动配置"))==ENTER || nkeyin==0)
	{	
		if(NDK_FsExist(PARACONF)!=NDK_OK)
		{	
			cls_show_msg1(10,"无参数配置文件,将进行手动配置");
			g_conf_flag = 0;
		}
		else
			g_conf_flag = 1;
	}	
	else
		g_conf_flag = 0;
	//手工配置链路参数
	if(conf_link(ETH)!=SUCC)//setlinktype(ETH);//linktype = ETH;
		return FAIL;
	if(cls_show_msg1(5,"请确保打开TCP/UDP后台!!!\n")==ESC)
		return NDK_ERR_QUIT;//QUIT;
	//手工配置网络参数
	if(conf_network()!=SUCC)
		return FAIL;
	if(IsTransConfed==TRUE)
	{
		if(conf_trans()!=SUCC)
			return FAIL;
	}
	
	//配置完毕,要测试一下网络连通性
#if 0
	//配置完毕,要测试一下网络连通性
	return (IsNetConnected()==TRUE)?SUCC:FAIL;
#else
	return SUCC;
#endif

}
#else
int conf_conn_ETH(int IsTransConfed){return FAIL;}
#endif

#if (NDKSOCK_ENABLE||NDKSSL_ENABLE)
int conf_conn_sock(void)
{
 	/*private & local definition*/
	int nKeyin = 0, nkeyin1 = ENTER, i = 0;
	LinkType type[] = {GPRS, CDMA, ASYN, ETH, TD, WLAN};
	char *typestr[] = {"GPRS", "CDMA", "ASYN", "ETH", "TD", "WLAN"};
	int enable_flag[] ={PPP_ENABLE, PPP_ENABLE, ASYN_ENABLE, ETH_ENABLE, TD_ENABLE, WLAN_ENABLE};
	int conf_flag = 0;
	char pInfo[64] = {0}, temptype[16] = {0};

 	/*process body*/	
	if((nkeyin1=cls_show_msg1(WAITTIME,"[Enter]使用参数配置文件,[其他]手动配置"))==ENTER || nkeyin1==0)
	{
		g_conf_flag = 1;
		if(GetParaInfo("COMMType",pInfo)!=SUCC)
			return FAIL;
		if(trim_string(pInfo,temptype)!=SUCC)
			return FAIL;
		for(i=0;i<SZ_ARRAY(typestr);i++)
		{
			if(!strcmp(temptype,typestr[i]))
			{			
				if(conf_link(type[i])!=SUCC)
					return FAIL;
				if(getlinktype()==ASYN)//若为异步,做进一步修正
				{
					strcpy(MDMDialStr, DEFAULT_MDM_DIAL_STR);	
					//追加对PPPUSR,PPPPWD的配置
					strcpy(PPPUSR, DEFAULT_ASYN_USR);
					strcpy(PPPPWD, DEFAULT_ASYN_PWD);
				}
				break;
			}
			
		}
		if(i==SZ_ARRAY(typestr))
		{
			cls_show_msg("file:%s,line:%d参数配置文件中socket通讯类型配置错误)",__FILE__ ,__LINE__);
			return FAIL;
		}
	}
	else
	{
		g_conf_flag = 0;
		do
		{
			nKeyin = cls_show_msg("选择连接方式:\n"
									"1.GPRS 2.CDMA\n"
									"3.ASYN 4.Eth\n"
									"5.TD 6.WLAN\n");
#define MAXMENUITEM ('6')

			conf_flag = enable_flag[nKeyin-'1'];
			switch (nKeyin)
			{
			//case 0://0为超时,默认启用GPRS
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case MAXMENUITEM://'6':
				if(conf_flag)
				{
					if(conf_link(type[nKeyin-'1'])!=SUCC)
						return FAIL;
					if(getlinktype()==ASYN)//若为异步,做进一步修正
					{
						strcpy(MDMDialStr, DEFAULT_MDM_DIAL_STR);	
						//追加对PPPUSR,PPPPWD的配置
						strcpy(PPPUSR, DEFAULT_ASYN_USR);
						strcpy(PPPPWD, DEFAULT_ASYN_PWD);
					}
					break;
				}
				else
				{
					cls_show_msg1(10, "本产品不支持本连接方式,请重新选择");
					return NDK_ERR_QUIT;
				}
			case ESC:
				return NDK_ERR_QUIT;//QUIT;
				break;
			default:
				cls_show_msg1(1, "若要退出,请按ESC...");
				continue;
				break;
			}
		}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM
	}

 	if(cls_show_msg1(MAXWAITTIME,"请确保打开sock或者ssl测试后台!!!\n")==ESC)
		return NDK_ERR_QUIT;//QUIT;
	if(conf_network()!=SUCC)
		return FAIL;

#if NEW_WIFI_ENABLE
	if(getlinktype()==WLAN)
	{
		cls_show_msg1(5,"保存AP列表...");
		//保存全局参数到AP列表文件中,同时设置好列表
		if(set_addedapinfo_fromgloabvar()!= SUCC)
		{
			cls_show_msg1(10, "%s, line %d:AP列表设置失败!(%d)", __FILE__, __LINE__);
			return FAIL;
		}
	}
#endif

#if 0
	//配置完毕,要测试一下网络连通性
	return (IsNetConnected()==TRUE)?SUCC:FAIL;
#else
	return SUCC;
#endif
}
#else
int conf_conn_sock(void){return FAIL;}
#endif

#if (WLM_ENABLE && PPP_ENABLE)
int conf_conn_WLM(int IsCnncted)
{
	/*private & local definition*/
	int nKeyin = 0, nkeyin1 = ENTER, i = 0;
	LinkType type[] = {GPRS, CDMA, TD};
	char *typestr[] = {"GPRS", "CDMA", "TD"};
	int enable_flag[] ={PPP_ENABLE, PPP_ENABLE, TD_ENABLE};
	int conf_flag = 0;
	char pInfo[64] = {0}, temptype[16] = {0};
	
	/*process body*/	
	if((nkeyin1=cls_show_msg1(WAITTIME,"[Enter]使用参数配置文件,[其他]手动配置"))==ENTER || nkeyin1==0)
	{
		g_conf_flag = 1;
		if(GetParaInfo("WLMType",pInfo)!=SUCC)
			return FAIL;
		if(trim_string(pInfo,temptype)!=SUCC)
			return FAIL;
		for(i=0;i<SZ_ARRAY(typestr);i++)
		{
			if(!strcmp(temptype,typestr[i]))
			{
				if(conf_link(type[i])!=SUCC)
					return FAIL;
				break;
			}
		}
		if(i==SZ_ARRAY(typestr))
		{
			cls_show_msg("file:%s,line:%d参数配置文件中无线类型配置错误)",__FILE__ ,__LINE__);
			return FAIL;
		}
	}
	else
	{
		g_conf_flag = 0;
		do
		{	
			nKeyin = cls_show_msg("选择连接方式:\n"
								"1.GPRS 2.CDMA\n"
								"3.TD\n");
#define MAXMENUITEM ('3')

			conf_flag = enable_flag[nKeyin-'1'];
			switch(nKeyin)
			{
			//case 0://0为超时,默认启用GPRS
			case '1':
			case '2':
			case MAXMENUITEM://'3':
				if(conf_flag)
				{
					if(conf_link(type[nKeyin-'1'])!=SUCC)
						return FAIL;
					break;
				}
				else
				{
					cls_show_msg1(10, "本产品不支持本连接方式,请重新选择");
					return NDK_ERR_QUIT;
				}	
			case ESC:
				return NDK_ERR_QUIT;//QUIT;
				break;
			default:
				continue;
				break;
			}
		}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM
	}
	if(IsCnncted==TRUE)//IsCnncted==FALSE的话,只配置本地链路参数
	{
		if(cls_show_msg1(5,"请确保打开TCP/UDP后台!!!\n")==ESC)
			return NDK_ERR_QUIT;//QUIT;
		if(conf_network()!=SUCC)
			return FAIL;
		if(conf_trans()!=SUCC)
			return FAIL;
		
#if 0
		//配置完毕,要测试一下网络连通性
		if(!IsNetConnected())
			return FAIL;
#endif
	}

	return SUCC;
}
#else
int conf_conn_WLM(int IsCnncted){return FAIL;}
#endif

int conf_conn_DNS(void)
{
#if 1
	char pInfo[64] = {0};
	int type = 0, keyin = 0, ret = 0;

	if((keyin=cls_show_msg1(10,"dns测试方式选择:[Enter]使用自动配置,[其他]手动配置"))==ENTER || keyin==0)
	{
		if(GetParaInfo("DNSType",pInfo)!=SUCC)
			return FAIL;
		if(trim_string2int(pInfo,&type)!=SUCC)
			return FAIL;
		if(type==1)
			keyin='1';
		else if(type==2)
			keyin='2';
		else if(type==3)
			keyin='3';
		else
			return FAIL;
	}
	else
		keyin=cls_show_msg("请选择dns测试方式\n"
					"1.ETH\n"
					"2.PPP\n"
					"3.WIFI");

	switch(keyin) 
	{
		case '1':
			ret=conf_conn_ETH(FALSE);//ETH和WIFI请分别设置动态IP和静态IP进行测试
			break;
		case '2':
			ret=conf_conn_PPP();
			break;
		case '3':
			ret=conf_conn_WLAN();
			break;
		default:
			ret=conf_conn_ETH(FALSE);
			break;
	}

	return ret;
#else
	/*private & local definition*/
	int nKeyin = 0;
	LinkType type[] = {GPRS, CDMA, ASYN, ETH, TD, WLAN};
	
	/*process body*/
	do
	{
		nKeyin = cls_show_msg("选择连接方式:\n"
								"1.GPRS 2.CDMA\n"
								"3.ASYN 4.Eth\n"
								"5.TD 6.WLAN\n");
#define MAXMENUITEM ('6')

		switch (nKeyin)
		{
		//case 0://0为超时,默认启用GPRS
		case '1':
		case '2':
		case '3':			
		case '4':
		case '5':
		case MAXMENUITEM://'6':
			if(conf_link(type[nKeyin-'1'])!=SUCC)
				return FAIL;
			if(getlinktype()==ASYN)//若为异步,做进一步修正
			{
				strcpy(MDMDialStr, DEFAULT_MDM_DIAL_STR);//直接写死,不必调用conf_link,另外,不需要调用conf_network来设置DNS服务器
				//追加对PPPUSR,PPPPWD的配置
				strcpy(PPPUSR, DEFAULT_ASYN_USR);
				strcpy(PPPPWD, DEFAULT_ASYN_PWD);
			}			
			if(conf_local()!=SUCC)
		 		return FAIL;		
			break;
		case ESC:
			return NDK_ERR_QUIT;//QUIT;
			break;
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;
		}
	}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM

	return SUCC;
#endif
}

#if  WLAN_ENABLE
//从配置文件中导入wifi的参数
int use_wifi_conf(int ApID)
{
	/*private & local definition*/
	char pInfo[128] = {0},  wifissid[128] = {0}, wifiPwd[65] = {0};
#if NEW_WIFI_ENABLE
	char wifimactemp[64] = {0};
	uchar wifimac[12] = {0};
	int macflag, hiddenflag;
#else
	int encmode, keytype, eapmode;
	char Identity[128] = {0};
#endif
	char tmp[32] = {0}, tmp_left[32] = {0};

	//获取ssid
	memset(tmp,0,sizeof(tmp));
	sprintf(tmp, "WlanEssid%d", ApID);
	if(GetParaInfo(tmp,pInfo)!=SUCC)
		return FAIL;
	//新增取=左边的内容与ssid进行比较,若是这个ssid再继续,若不是出错提示20171101 add by jym
	trim_left_string(pInfo,tmp_left);
	if(strcmp(tmp,tmp_left))
	{	
		cls_show_msg1_record(FILENAME,FUNCNAME, 3, "配置文件中无%s配置项", tmp);
		return FAIL;
	}
	if(trim_string(pInfo,wifissid)!=SUCC)
		return FAIL;
	strcpy(WlanEssid,wifissid);
	
#if !NEW_WIFI_ENABLE
	//获取加密模式
	memset(tmp,0,sizeof(tmp));
	sprintf(tmp, "WlanEncMode%d", ApID);
	if(GetParaInfo(tmp,pInfo)!=SUCC)
		return FAIL;
	if(trim_string2int(pInfo,&encmode)!=SUCC)
		return FAIL;
	WlanEncMode = encmode;
	//加密模式为802.1X则继续获取EAP模式和认证用户名
	if(WlanEncMode==WIFI_NET_SEC_WPA_EAP)
	{
		//获取EAP模式
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp, "Wlan8021XEapMode%d", ApID);
		if(GetParaInfo(tmp,pInfo)!=SUCC)
			return FAIL;
		if(trim_string2int(pInfo,&eapmode)!=SUCC)
			return FAIL;
		Wlan8021XEapMode = eapmode;
		//获取认证用户名
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp, "Wlan8021XIdentity%d", ApID);
		if(GetParaInfo(tmp,pInfo)!=SUCC)
			return FAIL;
		if(trim_string(pInfo,Identity)!=SUCC)
			return FAIL;
		strcpy(Wlan8021XIdentity,Identity);
	}
	//获取密码类型
	memset(tmp,0,sizeof(tmp));
	sprintf(tmp, "WlanKeyType%d", ApID);
	if(GetParaInfo(tmp,pInfo)!=SUCC)
		return FAIL;
	if(trim_string2int(pInfo,&keytype)!=SUCC)
		return FAIL;
	WlanKeyType = keytype;
	//获取密码值
	if(WlanKeyType) //增加一个判断,若设置无密码就不获取密码值,有设置才获取20170925
#endif		
	{//新WiFi是直接获取密码值无需判断20170925
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp, "WlanPwd%d", ApID);
		if(GetParaInfo(tmp,pInfo)!=SUCC)
			return FAIL;
		if(trim_string(pInfo,wifiPwd)!=SUCC)
			;//return FAIL;
		//低端WiFi允许为空就不做判断20171107modify
		strcpy(WlanPwd,wifiPwd);
	}
#if NEW_WIFI_ENABLE
	//增加MAC地址,允许为空 
	if(strcmp(WlanPwd,"")) //密码为空时允许Mac为空就不去获取Mac 20171107 modify
	{
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp, "WifiMac%d", ApID);
		if(GetParaInfo(tmp,pInfo)!=SUCC)
			return FAIL;
		if(trim_string(pInfo,wifimactemp)!=SUCC)
			return FAIL;
		if(trim_char2uchar(wifimactemp,wifimac)!=SUCC)
			return FAIL;
		memcpy((char *)WifiMac,(char *)wifimac,6);
		//有MAC 就把mac 标志位置1
		memset(tmp,0,sizeof(tmp));
		sprintf(tmp, "WifiMacFlag%d", ApID);
		if(GetParaInfo(tmp,pInfo)!=SUCC)
			return FAIL;
		if(trim_string2int(pInfo,&macflag)!=SUCC)
			return FAIL;
		WifiMacFlag = macflag;
	}
	//增加优先级  可以后面加吧?如果需要优先级时，一定是存在2个AP,那么可能改动的地方会多吧
	//获取隐藏AP标志位
	memset(tmp,0,sizeof(tmp));
	sprintf(tmp, "WifiHiddenFlag%d", ApID);
	if(GetParaInfo(tmp,pInfo)!=SUCC)
		return FAIL;
	if(trim_string2int(pInfo,&hiddenflag)!=SUCC)
		return FAIL;
	WifiHiddenFlag = hiddenflag;
#endif
	cls_show_msg1(1,"WLAN参数配置成功");		
	return SUCC;
	
}

//与conf_WLAN_lnk的区别在于不进行AP扫描与选择,仅进行一些纯软参数的配置
void conf_WLAN_lnk1(void)
{
	/*private & local definition*/
	int nKeyin = 0;
	uint unLen=0;
#if !NEW_WIFI_ENABLE
	char *tip[] = {"WIFI_NET_SEC_NONE", "WIFI_NET_SEC_WEP_OPEN", "WIFI_NET_SEC_WEP_SHARED", "WIFI_NET_SEC_WPA", "WIFI_NET_SEC_WPA2", "WIFI_NET_SEC_WPA_EAP"}, *tip1[] = {"NOKEY", "HEX", "ASCII"};

	/*process body*/
	// 2.加密模式选择
	do
	{
		nKeyin = cls_show_msg("WLAN加密模式:\n"
								"0.NONE 1.OPEN\n"
								"2.SHARED 3.WPA\n"
								"4.WPA2 5.802.1x\n");//标号从0始,以为了保持与枚举值一致

#define MAXMENUITEM ('5')

		switch (nKeyin)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
			WlanEncMode= nKeyin-'0';
			cls_show_msg1(2, "将设置%s加密模式...", tip[WlanEncMode]);
			break;
		case MAXMENUITEM:
			WlanEncMode= WIFI_NET_SEC_WPA_EAP;
			cls_show_msg1(2, "将设置%s加密模式...", tip[nKeyin-'0']);
			break;
		//case 0://超时,ESC,均使用默认
		case ESC:
			cls_show_msg1(2, "将使用%s加密模式...", tip[WlanEncMode]);
			break;
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;
		}
	}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'0'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM

	//加密模式是802.1x认证模式时需设置EAP模式和用户名
	if(WlanEncMode==WIFI_NET_SEC_WPA_EAP)
	{
		//设置802.1x的EAP模式
		do
		{
			nKeyin = cls_show_msg("EAP模式:\n"
									"0.EAP_MODE_MD5\n"
									"1.EAP_MODE_PEAP\n"
									"2.EAP_MODE_TTLS\n");//标号从0始,以为了保持与枚举值一致

#define MAXMENUITEM ('2')

			switch (nKeyin)
			{
			case '0':
			case '1':
			case MAXMENUITEM:
				Wlan8021XEapMode= nKeyin-'0';
				cls_show_msg1(2, "将设置%s加密模式...", tip[Wlan8021XEapMode]);
				break;
			case ESC:
				cls_show_msg1(2, "将使用%s加密模式...", tip[Wlan8021XEapMode]);
				break;
			default:
				cls_show_msg1(1, "若要退出,请按ESC...");
				continue;
				break;
			}
		}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'0'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM
		
		//设置802.1x认证用户名
		cls_printf("当前802.1x认证用户名:\n");
		if ((nKeyin=lib_kbgetinput(Wlan8021XIdentity, 0,sizeof(Wlan8021XIdentity)-1,&unLen, INPUTDISP_OTHER, 0,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||nKeyin==NDK_ERR_TIMEOUT)
			strcpy(Wlan8021XIdentity, DEFAULT_WLAN_8021XIDENTITY);
	}
	
	// 3.密码类型选择
	do
	{
		nKeyin = cls_show_msg("WLAN密码类型:\n"
								"0.无密码1.HEX\n"
								"2.ASCII\n");//标号从0始,以为了保持与枚举值一致
#define MAXMENUITEM ('2')

		switch (nKeyin)
		{
		case '0':
		case '1':
		case MAXMENUITEM://'2':
			WlanKeyType= nKeyin-'0';
			cls_show_msg1(2, "将设置密码类型:%s...", tip1[WlanKeyType]);
			break;
		//case 0://超时,ESC,均使用默认
		case ESC:
			cls_show_msg1(2, "将使用密码类型:%s...", tip1[WlanKeyType]);
			break;
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;
		}	
	}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'0'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM
#endif

	// 4.WLAN密码输入
#if NEW_WIFI_ENABLE
	cls_printf("当前WLAN密码:\n");
	if ((nKeyin=lib_kbgetinput(WlanPwd, 0,sizeof(WlanPwd)-1,&unLen, INPUTDISP_OTHER, 0,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||nKeyin==NDK_ERR_TIMEOUT)//按取消,则使用default
		strcpy(WlanPwd, DEFAULT_WLAN_PWD);//不允许空密码,故若取消输入,也要给个默认值
#else
	if(WlanKeyType==0)
		strcpy(WlanPwd, DEFAULT_WLAN_PWD);
	else
	{
		cls_printf("当前WLAN密码:\n");
		if ((nKeyin=lib_kbgetinput(WlanPwd, 0,sizeof(WlanPwd)-1,&unLen, INPUTDISP_OTHER, 0,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||nKeyin==NDK_ERR_TIMEOUT)//按取消,则使用default
			strcpy(WlanPwd, DEFAULT_WLAN_PWD);//不允许空密码,故若取消输入,也要给个默认值
	}
#endif
	return;
}

#if NEW_WIFI_ENABLE
int set_addedapinfo_fromgloabvar(void)
{
	int ret = 0;
	uint len =2, dhcpflag = 0, macflag= 0, priflag = 0, hiddenflag = 0;
	char tmp[4] = {0};
	ST_WIFI_PROFILE_LIST stored_list;
	ST_WIFI_AP_ENTRY_T store_ap;
	stored_list.stored_ap = &store_ap;
	
	//将全局变量中的SSID\密码保存到可链接AP列表中的ssid[WIFI_SSID_MAX]
	strcpy(stored_list.stored_ap->ssid, WlanEssid);
	strcpy(stored_list.stored_ap->pwd, WlanPwd);
	dhcpflag = netsetting.DHCPenable&0x01;
	if(dhcpflag==0)
	{
		strcpy(stored_list.stored_ap->ip.ip,netsetting.LocalIP.Str);	
		strcpy(stored_list.stored_ap->ip.netmask,netsetting.LocalMask.Str);	
		strcpy(stored_list.stored_ap->ip.gateway,netsetting.LocalGateway.Str);
		strcpy(stored_list.stored_ap->ip.dns_primary, netsetting.DNSHost[0]);
		strcpy(stored_list.stored_ap->ip.dns_secondary, netsetting.DNSHost[1]);
	}

	//根据全局变量的标志决定是否保存MAC
	if(WifiMacFlag)
	{
		memcpy(stored_list.stored_ap->mac.octet, WifiMac, 6);
		macflag = WIFI_AP_FLAG_MAC_SET;
	}
	else
	{
		memset(stored_list.stored_ap->mac.octet, 0, 6);
		macflag = 0;
	}
	//不使用配置文件时手工设置
	if(!g_conf_flag)
	{
		if(ENTER==cls_show_msg1(MAXWAITTIME, "是否设置优先级\n"
											"[确认]设置\n"
											"[其它]不设置\n"))
		{
			priflag = WIFI_AP_FLAG_PRI_SET;

			cls_printf("请设置连接优先级,范围(0~255)数值越大越高:\n");
			sprintf(tmp, "%d", 0);
			if ((ret=lib_kbgetinput(tmp, 0, sizeof(tmp)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
				stored_list.stored_ap->priority = 1;
			else
				stored_list.stored_ap->priority = atoi(tmp);

			if(stored_list.stored_ap->priority>=255)
				stored_list.stored_ap->priority=255;
		}
		else
			priflag = 0;

		if(ENTER==cls_show_msg1(MAXWAITTIME, "连接的AP是否为隐藏AP\n"
											"[确认]是\n"
											"[其它]不是\n"))
		{
			hiddenflag = WIFI_AP_FLAG_HIDDEN_SET;
		}
		else
			hiddenflag = 0;
	}
	else
	{
		if(WifiHiddenFlag)
			hiddenflag = WIFI_AP_FLAG_HIDDEN_SET;
		else
			hiddenflag = 0;
	}
	//将4个标志位组合成状态标志位flag
	stored_list.stored_ap->flag = dhcpflag|macflag|priflag|hiddenflag;
	stored_list.stored_num = 1;
	if((ret = NDK_WifiProfileSet(&stored_list)) != NDK_OK)
	{
		cls_show_msg("%s line %d:设置AP列表失败(%d)", __FILE__, __LINE__, ret);
		return FAIL;
	}
	return SUCC;
}
#endif

//选择通过扫描的方式输入测试的SSID名
static int select_scan_SSID(void)
{
	/*private & local definition*/
	int ret = 0, i = 0, j = 0;
#if NEW_WIFI_ENABLE
	ST_WIFI_AP_LIST list_scan;
	ST_WIFI_AP_INFO ap[32];
	ST_WIFI_STATUS status;
	char ssidout[32][33];
	uint ssidoutlen = 0;
#else
	int signal = 0, numList = 0;
	char *ESSIDlist[64];
	char ESSIDlist1[64][64];
	ST_WIFI_APINFO ESSIDlist_info[64];
#endif
	
	/*process body*/
	cls_printf("扫描AP中,请稍候...");
#if NEW_WIFI_ENABLE
	//NDK_WifiGetFunc功能需要在start(wifi驱动开启)后且状态要为稳定(包括空、失败、成功三种状态)时才能用
	NDK_WifiStart();
	if((ret = lib_getwifistatus(&status)) != NDK_OK)
	{
		cls_show_msg("%s line %d:获取wifi状态失败(%d)", __FILE__, __LINE__,status.status);
		return FAIL;
	}
	list_scan.num = 32;
	list_scan.list = ap;
	for (j=0; j<3; j++) 
	{
		if ((NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan)) == NDK_OK) 
		{
			if (list_scan.num > 0) 
				break;
			else
			{
				cls_show_msg("%s line %d:未扫描到AP", __FILE__, __LINE__);
				return FAIL;
			}
		}
		sleep(1);
	}
	if (j == 3) 
	{
		cls_show_msg("%s line %d:WIFI扫描失败", __FILE__, __LINE__);
		return FAIL;
	}
	for (i=0; i<list_scan.num; i++)
	{
		if(!strcmp(ap[i].ssid,""))//如果是隐藏ap  直接传给输出buf
			strcpy(ssidout[i],ap[i].ssid);
		else
		{
			if((ret=NDK_Utf8ToGbk((uchar *)ap[i].ssid,strlen(ap[i].ssid),(uchar *)ssidout[i],&ssidoutlen))!=NDK_OK)
			{
				cls_show_msg("%s line %d:utf8转换gbk失败(%d)", __FILE__, __LINE__, ret);
				return FAIL;
			}
		}
		if(cls_show_msg("是否选AP%d:%s(%02x:%02x:%02x:%02x:%02x:%02x)为测试AP,是[确认],否[其它]", i, ssidout[i],ap[i].mac.octet[0],ap[i].mac.octet[1],ap[i].mac.octet[2],ap[i].mac.octet[3],ap[i].mac.octet[4],ap[i].mac.octet[5])==ENTER)//选择SSID后
			break;
	}
	if(i>=list_scan.num)
	{
		cls_show_msg("%s line %d:未扫描到测试用AP请重新扫描", __FILE__, __LINE__);
		return FAIL;
	}
	strcpy(WlanEssid, ap[i].ssid);
	
	//增加保存MAC地址的操作 ,新增全局变量和初始化默认值等
	if(cls_show_msg("是否保存AP:%s的MAC地址%02x:%02x:%02x:%02x:%02x:%02x", ap[i].ssid,ap[i].mac.octet[0],ap[i].mac.octet[1],ap[i].mac.octet[2],ap[i].mac.octet[3],ap[i].mac.octet[4],ap[i].mac.octet[5])==ENTER)
	{
		memcpy(WifiMac, ap[i].mac.octet, 6);
		WifiMacFlag = 1;
	}
	else
	{
		memset(WifiMac, 0, sizeof(WifiMac));
		WifiMacFlag = 0;
	}
	NDK_WifiStop();
#else
	memset(ESSIDlist_info,0,sizeof(ESSIDlist_info));
	for (i=0; i<64; i++)
		ESSIDlist[i]=ESSIDlist1[i];
	if ((ret=NDK_WiFiInit()) != NDK_OK)
	{
		cls_show_msg("%s line %d:WIFI初始化失败ret=%d", __FILE__, __LINE__, ret);
		return FAIL;
	}
/*	
	//获取到所有的AP信息
	for (j=0; j<3; j++)
	{
		if (NDK_WiFiGetNetInfo(ESSIDlist_info,64, &numList) == NDK_OK)
		{
			if (numList > 0)
			{
				for (i=0; i<numList; i++)
				    cls_show_msg("Line=%d\n AP[%d]->Essid:%s\n sSignal:%s", __LINE__,i, ESSIDlist_info[i].sEssid,ESSIDlist_info[i].sSignal);
				break;
			}
		}
		sleep(1);
	}	
*/	
	//扫描AP
	for (j=0; j<3; j++) 
	{
		if (NDK_WiFiGetNetList(ESSIDlist, &numList) == NDK_OK) 
		{
			if (numList > 0) 
				break;
			else
			{
				cls_show_msg("%s line %d:未扫描到AP", __FILE__, __LINE__);
				NDK_WiFiShutdown();
				return FAIL;
			}
		}
		sleep(1);
	}
	if (j == 3) 
	{
		cls_show_msg("%s line %d:WIFI扫描失败", __FILE__, __LINE__);
		return FAIL;
	}
	for (i=0; i<numList; i++)
	{
		
		ret=NDK_WiFiSignalCover(ESSIDlist1[i], &signal);
		if(cls_show_msg("共%d个AP,是否选择AP[%d]:%s信号为:%d作为测试AP,是按[确认],否则按其它", numList, i, ESSIDlist1[i],signal)==ENTER)//选择SSID后
			break;
	}
	if(i>=numList)
	{
		cls_show_msg("%s line %d:未扫描到测试用AP请重新扫描", __FILE__, __LINE__);
		NDK_WiFiShutdown();
		return FAIL;
	}
	strcpy(WlanEssid, ESSIDlist1[i]);
	if((ret=NDK_WiFiShutdown()) != NDK_OK)
	{
		cls_show_msg("%s line %d:NDK_WiFiShutdown失败ret=%d", __FILE__, __LINE__, ret);
		return FAIL;
	}
#endif

	return SUCC;
}

static int conf_WLAN_lnk(void)
{
	/*private & local definition*/
	int ret = 0, nKeyin = 0;
	uint unLen=0;
#if NEW_WIFI_ENABLE
	char wifimactemp[18]={0};
#endif
	/*process body*/
	// 1.扫描并选择接入的AP
	//扫描前,需要先切换到wifi设备//当然了,通常来说,SelectNetDev不要出现在conf.c中,而放到layer中
	do
	{
		nKeyin = cls_show_msg("SSID输入方法:\n"
								"1.手输\n"
								"2.扫描\n");
		switch(nKeyin)
		{
		case '1':
			cls_printf("当前SSID:\n");
			if((ret=lib_kbgetinput(WlanEssid,0, sizeof(WlanEssid)-1, &unLen, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
				strcpy(WlanEssid, DEFAULT_WLAN_ESSID);//不允许空SSID,故若取消输入,也要给个默认值
#if NEW_WIFI_ENABLE
			//如果需要则:增加MAC设置
			if(cls_show_msg("是否设置路由器MAC,是按[确认],其它键跳过")==ENTER)
			{
				cls_printf("请输入MAC字符串(格式XXXXXXXXXXXX):\n");
				if((ret=lib_kbgetinput(wifimactemp,0, sizeof(wifimactemp)-1, &unLen, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
					strcpy(wifimactemp, "A8:57:4E:95:2E:88");
				
				if(trim_char2uchar(wifimactemp,WifiMac)!=SUCC)
				{
					cls_show_msg("%s line %d:MAC设置错误", __FILE__, __LINE__);
					return FAIL;
				}
				WifiMacFlag = 1;
			}
			else
			{
				memset(WifiMac, 0, sizeof(WifiMac));
				WifiMacFlag = 0;
			}
#endif			
			break;
		case '2':
			if(select_scan_SSID()!=SUCC)
				return FAIL;
			break;
		case ESC:
			cls_show_msg1(2, "将使用默认SSID:%s...", DEFAULT_WLAN_ESSID);		
			strcpy(WlanEssid, DEFAULT_WLAN_ESSID);//不允许空SSID,故若取消输入,也要给个默认值
			memset(WifiMac, 0, sizeof(WifiMac));
			WifiMacFlag = 0;
			break;	
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;
		}
	}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>'2'));
	
	conf_WLAN_lnk1();
	return SUCC;
}

int conf_conn_WLAN(void)
{
	/*private & local definition*/
	int nkeyin = ENTER;
	
	/*process body*/
	if((nkeyin=cls_show_msg1(WAITTIME,"[Enter]使用参数配置文件,[其他]手动配置"))==ENTER || nkeyin==0)
	{	
		if(NDK_FsExist(PARACONF)!=NDK_OK)
		{	
			cls_show_msg1(10,"无参数配置文件,将进行手动配置");
			g_conf_flag = 0;
		}
		else
			g_conf_flag = 1;
	}	
	else
		g_conf_flag = 0;
	
	//手工配置链路参数
	if(conf_link(WLAN)!=SUCC)
		return FAIL;
	if(cls_show_msg1(5,"请确保打开TCP/UDP后台!!!\n")==ESC)
		return NDK_ERR_QUIT;//QUIT;
	//手工配置网络参数
	if(conf_network()!=SUCC)
		return FAIL;
	
#if NEW_WIFI_ENABLE
	//保存全局参数到AP列表文件中,同时设置好列表
	if(set_addedapinfo_fromgloabvar()!= SUCC)
	{
		cls_show_msg1(10, "%s, line %d:AP列表设置失败!(%d)", __FILE__, __LINE__);
		return FAIL;
	}
#endif

	if(conf_trans()!=SUCC)
		return FAIL;

//	if(cls_show_msg1(5, "是否要测试下网络连通性，首次运行wifi模块必须要测一下")!=ENTER)
//		return SUCC;
		
	//配置完毕,要测试一下网络连通性
#if 0
	//配置完毕,要测试一下网络连通性
	return (IsNetConnected()==TRUE)?SUCC:FAIL;
#else
	return SUCC;
#endif

}
#else
int use_wifi_conf(int ApID) {return FAIL;}
void conf_WLAN_lnk1(void){}
#if NEW_WIFI_ENABLE
int set_addedapinfo_fromgloabvar(void){return FAIL;}
#endif
static int conf_WLAN_lnk(void){return FAIL;}
int conf_conn_WLAN(void){return FAIL;}
#endif

#if WIFIAP_ENABLE
//设置WIFI AP功能参数
int conf_wifi_AP(void)  
{
	/*private & local definition*/
	int nKeyin = 0,ret = 0, nkeyin1 = ENTER;
	uint unLen=0;
	char *tip[] = {"WIFI_NET_SEC_NONE", "WIFI_NET_SEC_WEP_OPEN", "WIFI_NET_SEC_WEP_SHARED", "WIFI_NET_SEC_WPA", "WIFI_NET_SEC_WPA2"};
	char pInfo[70] = {0}, apssid[33] = {0}, apkey[65] = {0}, apip[16] = {0};
	int apsecmode = 0, aphideden = 0;

	/*process body*/	
	if((nkeyin1=cls_show_msg1(WAITTIME,"[Enter]使用参数配置文件,[其他]手动配置"))==ENTER || nkeyin1==0)
	{
		//加密模式
		if(GetParaInfo("WifiApSecMode",pInfo)!=SUCC)
			return FAIL;
		if(trim_string2int(pInfo,&apsecmode)!=SUCC)
			return FAIL;
		WifiApSecMode = apsecmode;
		//密码
		if(GetParaInfo("WifiApKey",pInfo)!=SUCC)
			return FAIL;
		if(trim_string(pInfo,apkey)!=SUCC)
			return FAIL;
		strcpy(WifiApKey,apkey);
		// wifi ap ssid
		if(GetParaInfo("WifiApSsid",pInfo)!=SUCC)
			return FAIL;
		if(trim_string(pInfo,apssid)!=SUCC)
			return FAIL;
		strcpy(WifiApSsid,apssid);
		//是否隐藏
		if(GetParaInfo("WifiApHidden",pInfo)!=SUCC)
			return FAIL;
		if(trim_string2int(pInfo,&aphideden)!=SUCC)
			return FAIL;
		WifiApHidden = (uchar)aphideden;
		//热点IP
		if(GetParaInfo("WifiApIp",pInfo)!=SUCC)
			return FAIL;
		if(trim_string(pInfo,apip)!=SUCC)
			return FAIL;
		strcpy(netsetting.WifiApIp.Str,apip);
		netsetting.WifiApIp.value.data = INET_ADDR(netsetting.WifiApIp.Str);
		cls_show_msg1(1,"AP参数配置成功");
	}
	else
	{
	//2.加密模式选择
		do
		{
			nKeyin = cls_show_msg("WIFI AP加密模式:\n"
									"0.NONE 1.OPEN\n"
									"2.SHARED 3.WPA\n"
									"4.WPA2\n");//标号从0始,以为了保持与枚举值一致

#define MAXMENUITEM ('4')

			switch (nKeyin)
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case MAXMENUITEM:
				WifiApSecMode = nKeyin-'0';
				cls_show_msg1(2, "将设置%s加密模式...", tip[WifiApSecMode]);
				break;
			case ESC:
				cls_show_msg1(2, "将使用%s加密模式...", tip[WifiApSecMode]);
				break;
			default:
				cls_show_msg1(1, "若要退出,请按ESC...");
				continue;
				break;
			}
		}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'0'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM

		//3.根据不同的加密模式写死相应长度的密码
		if(WifiApSecMode==WIFI_NET_SEC_NONE)
			memset(WifiApKey,0,sizeof(WifiApKey));
		if(WifiApSecMode==WIFI_NET_SEC_WEP_OPEN)//WIFI_NET_SEC_WEP_OPEN:5个ASCII字符或者10个十六进制字符（0-9和A-F）
			strcpy(WifiApKey, "01234");
		if(WifiApSecMode==WIFI_NET_SEC_WEP_SHARED)//WIFI_NET_SEC_WEP_ SHARED: 13个ASCII字符或者26个十六进制字符（0-9和A-F）
			strcpy(WifiApKey, "0123456789012");
		if(WifiApSecMode==WIFI_NET_SEC_WPA||WifiApSecMode==WIFI_NET_SEC_WPA2)//WIFI_NET_SEC_WPA或者WIFI_NET_SEC_WPA2：可以输入8-64个任意字符。
			strcpy(WifiApKey, DEFAULT_WLANAP_KEY);

		//4.设置WIFI热点的SSID
		cls_printf("当前AP:\n");
		if((ret=lib_kbgetinput(WifiApSsid,0, sizeof(WifiApSsid)-1, &unLen,INPUTDISP_OTHER, 0,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			strcpy(WifiApSsid, DEFAULT_WLANAP_SSID);//不允许空SSID,故若取消输入,也要给个默认值
		
		//5.设置是否隐藏ssid
		do
		{
			nKeyin = cls_show_msg("是否隐藏SSID:\n"
									"0.不隐藏\n"
									"1.隐藏\n");//标号从0始,以为了保持与枚举值一致
#define MAXMENUITEM ('1')

			switch (nKeyin)
			{
			case '0':
				WifiApHidden = 0;
				cls_show_msg1(1, "将设置SSID不隐藏");
				break;
			case MAXMENUITEM:
				WifiApHidden = 1;
				cls_show_msg1(1, "将设置SSID隐藏");
				break;
			case ESC:
				WifiApHidden = 0;
				cls_show_msg1(1, "将设置SSID不隐藏");
				break;
			default:
				cls_show_msg1(1, "若要退出,请按ESC...");
				continue;
				break;
			}	
		}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'0'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM

		//设置WiFi热点IP
		cls_printf("当前WiFi热点IP:\n");
		if ((ret=lib_kbgetinput(netsetting.WifiApIp.Str, 0, sizeof(netsetting.WifiApIp.Str)-1, &unLen, INPUTDISP_OTHER, 0, INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			strcpy(netsetting.WifiApIp.Str, DEFAULT_WIFIAP_IP_STR);  
		
		netsetting.WifiApIp.value.data = INET_ADDR(netsetting.WifiApIp.Str);
	}
	return SUCC;
}

int conf_conn_AP(void)
{
	/*private & local definition*/
	int nKeyin = 0, nkeyin1 = ENTER;
	char *tip[] = {"ETH", "WLM"};
	char pInfo[64] = {0};
	int apsharedev = 0;
	
	/*process body*/	
	//1.WiFi热点接入的网络方式选择
	if((nkeyin1=cls_show_msg1(WAITTIME,"[Enter]使用参数配置文件,[其他]手动配置"))==ENTER || nkeyin1==0)
	{
		g_conf_flag = 1;
		if(GetParaInfo("WifiApShareDev",pInfo)!=SUCC)
			return FAIL;
		if(trim_string2int(pInfo,&apsharedev)!=SUCC)
			return FAIL;
		WifiApShareDev = apsharedev;
		cls_show_msg1(1,"AP接入方式配置成功");
	}
	else
	{
		g_conf_flag = 0;
		do
		{
			nKeyin = cls_show_msg("WIFI AP接入方式:\n"
									"0.ETH方式\n"
									"1.WLM方式\n");//标号从0始,以为了保持与枚举值一致
#define MAXMENUITEM ('1')

			switch (nKeyin)
			{
			case '0':
			case MAXMENUITEM:
				WifiApShareDev = nKeyin-'0';
				cls_show_msg1(2, "将设置WIFI AP接入方式:%s...", tip[WifiApShareDev]);
				break;
			case ESC:
				cls_show_msg1(2, "将使用WIFI AP接入方式:%s...", tip[WifiApShareDev]);
				break;
			default:
				cls_show_msg1(1, "若要退出,请按ESC...");
				continue;
				break;
			}	
		}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'0'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM
	}

	//根据共享网络类型设置链路层
	if(WifiApShareDev==WIFIAP_SHARE_DEV_ETH)
	{
		switch(conf_conn_ETH(TRUE))
		{
		case SUCC:
			break;
		case FAIL:
			cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
			break;
		case NDK_ERR_QUIT://用户放弃
		default:
			break;
		}
	}
	else if(WifiApShareDev==WIFIAP_SHARE_DEV_PPP)
	{
		switch(conf_conn_WLM(TRUE))
		{
		case SUCC:
			cls_show_msg1(2, "网络配置成功!");
			break;
		case FAIL:
			cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);	
			//return;
			break;
		case NDK_ERR_QUIT://用户放弃
		default:
			//return;
			break;
		}
	}
	return SUCC;
}
#else
int conf_wifi_AP(void){return FAIL;}
int conf_conn_AP(void){return FAIL;}
#endif

#if USB_ENABLE||SD_ENABLE
static int g_disktype = UDISK;

//g_disktype数据的操作方法
void set_disktype(int type){g_disktype = type;}
int get_disktype(void){return g_disktype;}

int conf_disktype(void)//增加设置disk设备类型,用于usb和sd卡测试,linwl 20130311
{
	int nKeyin = 0, nkeyin1 = ENTER;
	char pInfo[64] = {0};
	int type = 0;
	
	/*process body*/
	if((nkeyin1=cls_show_msg1(WAITTIME,"[Enter]使用参数配置文件,[其他]手动配置"))==ENTER || nkeyin1==0)
	{
		if(GetParaInfo("DiskType",pInfo)!=SUCC)
			return FAIL;
		if(trim_string2int(pInfo,&type)!=SUCC)
			return FAIL;
		if(type==1)
		{	
			set_disktype(SDDISK);
			cls_show_msg1(2, "选择设备类型为SD卡");
		}
		else
		{
			set_disktype(UDISK);
			cls_show_msg1(2, "选择设备类型为U盘");
		}
	}
	else
	{
		do
		{
			nKeyin = cls_show_msg("选择设备类型:\n"
									"1.USB 2.SDCard\n");

#define MAXMENUITEM ('2')		
			switch (nKeyin)
			{
			case '1':
				set_disktype(UDISK);
				cls_show_msg1(2, "选择设备类型为U盘");
				break;
			case MAXMENUITEM://'2':
				set_disktype(SDDISK);
				cls_show_msg1(2, "选择设备类型为SD卡");
				break;
			case ESC:
				return NDK_ERR_QUIT;//QUIT;
				break;
			default:
				cls_show_msg1(2, "若要退出,请按ESC...");
				continue;
				break;
			}
		}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>MAXMENUITEM));
#undef MAXMENUITEM
	}
	return SUCC;
}

#else
void set_disktype(int type){}
int get_disktype(void){return FAIL;}
int conf_disktype(void){return FAIL;}
#endif
#endif

#if !defined ME66 //ME66 空间小 不支持蓝牙 不编译
#if BT_ENABLE
int conf_conn_BT(void)
{
#if !(defined ME15CHX||defined ME15B)//ME15C极简版不支持文件系统 20170213 linwl
	/*private & local definition*/
	int nkeyin = ENTER;
	/*process body*/
	if((nkeyin=cls_show_msg1(WAITTIME,"[Enter]使用参数配置文件,[其他]手动配置"))==ENTER || nkeyin==0)
	{	
		if(NDK_FsExist(PARACONF)!=NDK_OK)
		{	
			cls_show_msg1(10,"无参数配置文件,将进行手动配置");
			g_conf_flag = 0;
		}
		else
			g_conf_flag = 1;
	}	
	else
		g_conf_flag = 0;
#endif
	//手工配置链路参数
	if(conf_link(BT)!=SUCC)
		return FAIL;
	
	return SUCC;
}

int g_at_enable = 0;//附初始值
int g_pair_mode = 0;
int g_btdiscoverflag = 0;//手动开启广播标志位 1:表示需要手动开启广播  0:表示不需要手动开启广播
int conf_bt(void)
{
	/*private & local definition*/
	int ret = -1;//, nkeyin1 = ENTER;
	uint unLen = 0;
	int nkey = 0;
	char sBuf[65]={0};
#if !(defined ME15CHX||defined ME15B)//ME15C极简版不支持文件系统 20170213 linwl	
	char pInfo[32] = {0}, btname[16] = {0}, btpin[7] = {0};
#endif
	char *pairmodestr[]={"Just Work","PIN Code","SSP PIN","PassKey","Just Work SC"};

	/*process body*/
	//配置蓝牙串口波特率
	//strcpy(btbps, conf_serial(&bpsid));

	//获取蓝牙模块
	memset(sBuf,0,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_HARDWARE_INFO, &unLen, sBuf)) != NDK_OK)
	{
		cls_show_msg("file:%s,line:%d获取蓝牙模块失败(ret=%d)",__FILE__ ,__LINE__, ret);
		return FAIL;
	}
	switch(sBuf[13])
	{
#if K21_ENABLE   //中低端蓝牙模块有区别 20180815 modify sull
		case 0x01:
			cls_printf("蓝牙模块为BM77");//目前仅BM77模块支持AT命令模式,要进行命令收发需先调用NDK_BTEnterCommand进入AT命令模式,NDK_BTExitCommand退出命令模式进入数据模式进行数据收发
			g_at_enable = 1;//AT命令模式标志位
			break;
		case 0x02:
			cls_printf("蓝牙模块为BT24");
			g_at_enable = 0;
			break;
		case 0x03:
			cls_printf("蓝牙模块为AP6210B");
			g_at_enable = 0;
			break;
		case 0x04:
			cls_printf("蓝牙模块为ALLTEC20706");
			g_at_enable = 0;
			break;
		case 0x05:
			cls_printf("蓝牙模块为YC1021");
			g_at_enable = 0;
			g_btdiscoverflag = 1;
			break;
		case 0x06:
			//cls_printf("蓝牙模块为DA14681");
			cls_printf("蓝牙模块为TC35661");
			g_at_enable = 0;
			break;
		case 0x07:
			cls_printf("蓝牙模块为20707-A2");
			g_at_enable = 0;
			break;
#else
		case 0x01:
			cls_printf("蓝牙模块为SD8787");
			g_at_enable = 0;
			break;
		case 0x02:
			cls_printf("蓝牙模块为AP6210");
			g_at_enable = 0;
			break;
		case 0x03:
			cls_printf("蓝牙模块为AP6210B");
			g_at_enable = 0;
			break;
		case 0x04:
			cls_printf("蓝牙模块为AP6212");
			g_at_enable = 0;
			break;
		case 0x05:
			cls_printf("蓝牙模块为AP6236");
			g_at_enable = 0;
			break;
		case 0x06:
			cls_printf("蓝牙模块为AP6255");
			g_at_enable = 0;
 			break;
#endif
 		default:
			cls_printf("未知的蓝牙模块");
			g_at_enable = 0;
			return FAIL;
			break;
	}
	//打开蓝牙串口
	//if((ret=NDK_PortOpen(PORT_NUM_BT,btbps))!=NDK_OK)
	if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
		cls_show_msg("file:%s,line:%d打开蓝牙串口失败(ret=%d)",__FILE__ ,__LINE__, ret);
		return FAIL;
	}
	//进入蓝牙命令模式
	if((ret=NDK_BTEnterCommand())!=NDK_OK)
	{
		cls_show_msg("file:%s,line:%d进入蓝牙命令模式失败(ret=%d)",__FILE__ ,__LINE__, ret);
		NDK_PortClose(PORT_NUM_BT);
		return FAIL;
	}
	
#if !(defined ME15CHX||defined ME15B)//ME15C极简版不支持文件系统 20170213 linwl
	//if((nkeyin1=cls_show_msg1(WAITTIME,"[Enter]使用参数配置文件,[其他]手动配置"))==ENTER || nkeyin1==0)
	if(g_conf_flag==1)
	{
		if(GetParaInfo("BtName",pInfo)!=SUCC)
			return FAIL;
		if(trim_string(pInfo,btname)!=SUCC)
			return FAIL;
		strcpy(BtName,btname);
		if(GetParaInfo("BtPin",pInfo)!=SUCC)
			return FAIL;
		if(trim_string(pInfo,btpin)!=SUCC)
			return FAIL;
		strcpy(BtPin,btpin);
		if(GetParaInfo("BtPairMode",pInfo)!=SUCC)
			return FAIL;
		if(trim_string2int(pInfo,&g_pair_mode)!=SUCC)
			return FAIL;
		if(g_pair_mode<0||g_pair_mode>4)//配置文件设置的配对模式不合法时修正为Just Work模式
			g_pair_mode=0;
		cls_show_msg1(1,"蓝牙设置成功");
	}
	else//手动设置
#endif
	{
		//设置蓝牙名称
		cls_printf("当前蓝牙名称:\n");
		if ((ret=lib_kbgetinput(BtName,0, sizeof(BtName)-1, &unLen,INPUTDISP_OTHER, 0,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			strcpy(BtName, DEFAULT_BT_NAME);//不允许空BtName,故若取消输入,也要给个默认值
		
		//设置蓝牙PIN
		cls_printf("当前蓝牙PIN:\n");
		if ((ret=lib_kbgetinput(BtPin,0, sizeof(BtPin)-1, &unLen,INPUTDISP_OTHER, 0,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			strcpy(BtPin, DEFAULT_BT_PIN);//不允许空BtPin,故若取消输入,也要给个默认值	

		//设置蓝牙配对模式
		cls_show_msg1(2,"请设置蓝牙配对模式(提示:苹果手机6s以下版本只能设置just work模式)");
		do
		{
			nkey = cls_show_msg(  "1.Just Work\n"
								"2.PIN Code\n"
								"3.SSP PIN\n"
								"4.PassKey\n"
								"5.Jusk Work SC"
								);
			switch(nkey)
			{	
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':	
					g_pair_mode=nkey-'1';
					break;
				case ESC:
				default:
#if BTPCI_ENABLE
					g_pair_mode=2;
#else
					g_pair_mode=0;
#endif
					break;
			}
		}while(nkey!=0 && nkey!=ESC && (nkey<'1'||nkey>'5'));
		
	}
	//cls_show_msg("设置蓝牙名称:%s,蓝牙PIN:%s,为%s模式",BtName ,BtPin ,pairmodestr[g_pair_mode]);
	
	//设置蓝牙名称
	if((ret=NDK_BTSetLocalName(BtName))!=NDK_OK)
	{
		cls_show_msg("file:%s,line:%d设置蓝牙名称失败(ret=%d,name=%s)",__FILE__ ,__LINE__, ret, BtName);
		NDK_BTExitCommand();
		NDK_PortClose(PORT_NUM_BT);
		return FAIL;
	}
#if !(defined ME20|| BTPCI_ENABLE)//ME20 仅支持BLE模式该函数返回-18 //PCI认证版本不支持此函数
	//设置蓝牙PIN
	if((ret=NDK_BTSetPIN(BtPin))!=NDK_OK)
	{
		cls_show_msg("file:%s,line:%d设置蓝牙PIN失败(ret=%d,pin=%s)",__FILE__ ,__LINE__, ret, BtPin);
		NDK_BTExitCommand();
		NDK_PortClose(PORT_NUM_BT);
		return FAIL;
	}
#endif
	//设置配对模式
#if JUSTWORK_FLAG
	g_pair_mode = 0;//只支持JustWork的设备，直接修改配对模式为JUSTWORK
#endif
	NDK_BTSetPairingMode(g_pair_mode);
	cls_show_msg1(1,"设置为%s模式",pairmodestr[g_pair_mode]);
	
	//退出蓝牙命令模式
	if((ret=NDK_BTExitCommand())!=NDK_OK)
	{
		cls_show_msg("file:%s,line:%d退出蓝牙命令模式失败(ret=%d)",__FILE__ ,__LINE__, ret);
		NDK_PortClose(PORT_NUM_BT);
		return FAIL;
	}
	NDK_BTDisconnect();
	if((ret=NDK_PortClose(PORT_NUM_BT))!=NDK_OK)
	{
		cls_show_msg("file:%s,line:%d关闭蓝牙串口失败(ret=%d)",__FILE__ ,__LINE__, ret);
		return FAIL;
	}
	
	return SUCC;
}
#else
int conf_conn_BT(void) {return FAIL;}
int conf_bt(void) {return FAIL;}
#endif
#endif

#if !(defined ME15C||defined ME15CHX||defined ME20||defined ME15B||defined ME66)	//ME15C为节省应用空间不编译不用的函数 20170213 linwl
#if NFC_ENABLE
int  (*Nfc_Activate)(uchar *psRecebuf,int *pnRecvlen,int nSeekCnt) = NDK_NfcUMS_Activate; 
void conf_nfc_datatype(void)
{
	/*private & local definition*/
	int nKeyin = 0;

	/*process body*/
	do
	{
		nKeyin = cls_show_msg("选择NFC数据格式:\n"
								"1.NDK_NfcUMS_Activate\n"
								"2.NDK_Nfc_Activate\n");
		
		switch (nKeyin)
		{
		case ESC:
		case '1':
			cls_show_msg1(2, "将使用NDK_NfcUMS_Activate...");
			Nfc_Activate = NDK_NfcUMS_Activate;
			break;
		case '2':
			cls_show_msg1(2, "将使用NDK_Nfc_Activate...");
			Nfc_Activate = NDK_Nfc_Activate;
			break;
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;
		}
	}while(nKeyin!=0 && nKeyin!=ESC && (nKeyin<'1'||nKeyin>'2'));

	return;
}
#else
void conf_nfc_datatype(void){cls_show_msg("不支持NFC");return;}
#endif
#endif

#if !(defined ME15C||defined ME15CHX||defined ME20||defined ME15B)	//ME15C为节省应用空间不编译不用的函数 20170213 linwl
int g_ScanType=0;
#if CAMERA_ENABLE
int g_ShowImage = 1, g_Saturation = 10, g_Sharpness = 5, g_Contrast = 12;
//摄像头参数设置函数
void conf_camera(void)
{
	int ret=0;
	uint unLen=0;
	char CntStr[8]={0};

	/*//本调用移动到systest33
	cls_printf("选择扫描模式:0.同步扫描 1.异步扫描:");
	strcpy(CntStr, "1");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		g_ScanType = 1;
	else
		g_ScanType = atoi(CntStr);*/
	
	cls_printf("是否打开摄像头(0为关闭,1为打开):");
	strcpy(CntStr, "1");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		g_ShowImage = 1;
	else
		g_ShowImage = atoi(CntStr);
	
	cls_printf("设置摄像头增益(有效值X~X):");
	strcpy(CntStr, "10");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		g_Saturation = 10;
	else
		g_Saturation = atoi(CntStr);
	
	cls_printf("设置摄像头曝光(有效值X~X):");
	strcpy(CntStr, "5");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		g_Sharpness = 5;
	else
		g_Sharpness = atoi(CntStr);
	
	cls_printf("设置摄像头帧率(有效值X~X):");
	strcpy(CntStr, "12");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		g_Contrast = 12;
	else
		g_Contrast = atoi(CntStr);

	return;
}
#else
void conf_camera(void){cls_show_msg("该产品不支持摄像头");return;}
#endif

#if SCAN_ENABLE||SCANASYCN_ENABLE
int g_Floodlight = 0, g_Focuslight = 0, g_Sensitivity = 11;
//扫描头参数设置函数
void conf_scan(void)
{
	/*private & local definition*/
	int ret=0;
	uint unLen=0;
	char CntStr[8]={0};

	/*process body*/
	g_ScanType = 0;//扫描头仅支持阻塞式调用
	if(cls_show_msg("是否默认设置,是[ENTER],否则[其他]")==ENTER)
	{
		NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT,11);
		g_Floodlight = 0;
		g_Focuslight = 0;
		g_Sensitivity = 11;
	}
	else
	{
		cls_printf("设置照明灯(0.闪烁 1.无照明 2.读码常亮):");
		strcpy(CntStr, "0");
		if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			g_Floodlight = 0;
		else
			g_Floodlight = atoi(CntStr);
			
		cls_printf("设置对焦灯(0.闪烁 1.无对焦 2.感应):");
		strcpy(CntStr, "0");
		if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			g_Focuslight = 0;
		else
			g_Focuslight = atoi(CntStr);
		
		cls_printf("设置灵敏度(1-20):");
		strcpy(CntStr, "11");
		if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 30, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			g_Sensitivity = 11;
		else
			g_Sensitivity = atoi(CntStr);
	}

	return;
}
#else
void conf_scan(void){cls_show_msg("该产品不支持扫描头");return;}
#endif
#endif

