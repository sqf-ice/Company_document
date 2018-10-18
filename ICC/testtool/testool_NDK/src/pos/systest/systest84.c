/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: WIFI/AP交叉
* file name		: systest84.c
* Author 			: 
* version			: V1.0
* DATE			: 20141210
* directory 		: 
* description		: WIFI/AP交叉
* related document	: NL_POS程序员参考手册
*				  
************************************************************************
* log			: 
* Revision 1.0  20130120 chenfm
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"WIFI/AP交叉"

#define	DEFAULT_CNT_STR	"10"
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	:
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 				: author			date			remarks
*						linwl		20141210		created
*****************************************************************/
static int test_AP(LinkType type)
{
	/*private & local definition*/
	ST_WIFIAP_PARAM stAPParam = {0};
	int ret=-1;
	
	/*process body*/
	//打开共享网络，建立ETH或者WLM连接
	setlinktype(type);
	if(NetUp()!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:NetUp失败", __LINE__);
		NetDown();
		return FAIL;
	}
	
	//获取全局WiFi热点参数
	memset(&stAPParam, 0,sizeof(stAPParam));
	stAPParam.emShareDev = WifiApShareDev;
	stAPParam.emSecMode = WifiApSecMode;
	strcpy(stAPParam.szAPIP, netsetting.WifiApIp.Str);
	strcpy(stAPParam.szKey,WifiApKey);
	strcpy(stAPParam.szSsid , WifiApSsid);
	stAPParam.uchidden = WifiApHidden;
	
	//打开WIFI热点
	if((ret=NDK_WiFiAPOpen(stAPParam))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:%s失败(ret=%d)", __LINE__, TESTITEM, ret);
		NDK_WiFiAPClose();
		NetDown();
		return FAIL;
	}
	//cls_show_msg("DEV:%d MODE:%d\nAPIP:%s\nAPKEY:%s\nAPSSID:%s\nHIDDEN:%d",stAPParam.emShareDev, stAPParam.emSecMode,stAPParam.szAPIP,stAPParam.szKey,stAPParam.szSsid,stAPParam.uchidden);
	cls_show_msg_record(FILENAME ,FUNCNAME ,"WIFI AP打开成功\nAPSSID:%s\nAPKEY:%s\nAPIP:%s\n使用另外一台设备进行一次WIFI数据通信后任意键继续", stAPParam.szSsid, stAPParam.szKey, stAPParam.szAPIP);
	
	//关闭WIFI热点
	if((ret=NDK_WiFiAPClose())!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:%s失败(ret=%d)", __LINE__, TESTITEM, ret);
		NDK_WiFiAPClose();
		NetDown();
		return FAIL;
	}
	//断开共享网络
	NetDown();
	return SUCC;
}

static int test_wifi(void)
{
	/*private & local definition*/
	int slen = 0, rlen = 0, timeout = SO_TIMEO;
	char buf[PACKMAXLEN] = {0}, rbuf[PACKMAXLEN] = {0};
	uint h_tcp = -1;
	
	/*process body*/
	setlinktype(WLAN);
	if(NetUp()!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:NetUp失败", __LINE__);
		NetDown();
		return FAIL;
	}
	
	//UP
	if(TransUp(&h_tcp)!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransUp失败", __LINE__);
		NetDown();
		return FAIL;
	}
		
	//发送数据
	if((slen=sock_send(h_tcp, buf, PACKMAXLEN, timeout))!=PACKMAXLEN)
	{
		cls_show_msg1(g_keeptime, "line %d:发送失败(实际%d, 预期%d)", __LINE__, slen, PACKMAXLEN);
		goto ERR;
	}
	//接收数据
	memset(rbuf, 0, sizeof(rbuf));
	if ((rlen=sock_recv(h_tcp, rbuf, PACKMAXLEN, timeout))!=slen)
	{
		cls_show_msg1(g_keeptime, "line %d:接收失败(实际%d, 预期%d)", __LINE__, rlen, slen);
		goto ERR;
	}
	//比较收发
	if (MemCmp(buf, rbuf, PACKMAXLEN))
	{
		cls_show_msg1(g_keeptime, "line %d:校验失败", __LINE__);
		goto ERR;
	}

	if (TransDown(h_tcp)!=SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:TransDown失败", __LINE__);
		goto ERR;
	}
	NetDown();
	return SUCC;
ERR:
	TransDown(h_tcp);
	NetDown();
	return FAIL;
}

void WLAN_AP_cross(LinkType type)
{
	/*private & local definition*/
	int ret = 0, succ = 0, i = 0, cnt = DEFAULT_CNT_VLE;
	char CntStr[8] = DEFAULT_CNT_STR;
	uint unLen=0;
		
	/*process body*/
	cls_printf("默认压力测试次数:");
	if ((ret= lib_kbgetinput(CntStr,2, sizeof(CntStr)-1, &unLen,2, 5,INPUT_CONTRL_NOLIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)//按取消,则使用default    
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	
	//进行WIFI数据通信一次
	while(cnt)
	{
		if (ESC==cls_show_msg1(2, "%s测试中\n还剩%d次(已成功%d次),ESC中断测试", TESTITEM, cnt, succ))
			break;
		cnt--;
		i++;

		//进行WIFI AP功能测试
		Local2Global(type);
		if(test_AP(type)!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败", __LINE__, i, TESTITEM);
			continue;
		}
		Global2Local(type);
		
		//进行WIFI功能测试
		Local2Global(WLAN);
		if(test_wifi()!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次%s测试失败", __LINE__, i, TESTITEM);
			continue;
		}
		Global2Local(WLAN);
		succ++;
	}

	cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试完成\n总共%d次(成功%d次)", TESTITEM, cnt, succ);
	return;
}

void systest84(void)
{
	/*private & local definition*/
	int ret = 0, tmpflag = 0;
	LinkType apsharelinktype = ETH;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	tmpflag = GetWifiStopflag();//将全局wifi断开的开关保存到临时
	if(tmpflag!=1)
		SetWifiStopflag(1);//确保本用例测试时wifi每次都调用stop去挂断

	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		//AP参数设置
		conf_wifi_AP();
		//共享网络配置
		if(conf_conn_AP()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,共享网络配置失败,请检查配置文件",TESTITEM);
			SetWifiStopflag(tmpflag);
			return;
		}
		apsharelinktype = getlinktype();//配置好共享网络后将共享网络类型(ETH,GPRS,CDMA)保持后在打开共享网络时使用
		Global2Local(apsharelinktype);//备份共享网络的参数
		//WLAN配置
		if(conf_conn_WLAN()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,WLAN配置失败,请检查配置文件",TESTITEM);
			SetWifiStopflag(tmpflag);
			return;
		}
		Global2Local(WLAN);

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行AP压力测试
		WLAN_AP_cross(apsharelinktype);
			
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		SetWifiStopflag(tmpflag);
		return;
	}
	while (1)
	{	
		ret = cls_show_msg("1.WIFI/AP交叉\n"
							"7.AP参数设置\n"
							"8.共享网络设置\n"
							"9.WLAN配置\n");
		
		switch(ret)
		{
		case '1':
			WLAN_AP_cross(apsharelinktype);
			break;
		case '7':
			conf_wifi_AP();
			break;
		case '8':
			switch(conf_conn_AP())
			{
			case SUCC:
				apsharelinktype = getlinktype();//配置好共享网络后将共享网络类型(ETH,GPRS,CDMA)保持后在打开共享网络时使用
				Global2Local(apsharelinktype);//备份共享网络的参数
				break;
			case FAIL:
				cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
				break;
			case NDK_ERR_QUIT:
			default:
				break;
			}
			
			break;
		case '9':
			switch(conf_conn_WLAN())
			{
			case SUCC:
				//备份wlan参数
				Global2Local(WLAN);
				break;
			case FAIL:
				cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
				break;
			case NDK_ERR_QUIT:
			default:
				break;
			}
			break;
		case ESC:
			SetWifiStopflag(tmpflag);
			return;
			break;
		default:
			continue;
			break;
		}
	}

	SetWifiStopflag(tmpflag);
	return;
}

