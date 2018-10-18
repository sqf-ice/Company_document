/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: WIFI AP综合
* file name		: systest83.c
* Author 			: 
* version			: V1.0
* DATE			: 20141210
* directory 		: 
* description		: WIFI AP综合
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
#define	TESTITEM	"WIFI AP综合"

#define	DEFAULT_CNT_STR	"10"//"1000"
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
static void open_wifi_ap(int flag)
{
	/*private & local definition*/
	ST_WIFIAP_PARAM stAPParam = {0};
	int ret=-1;
	uint h_tcp = -1;
	
	/*process body*/
	//打开共享网络，建立ETH或者WLM连接
	if(NetUp()!=SUCC)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:NetUp失败", __LINE__);
		return;
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
		return;
	}
	//cls_show_msg("DEV:%d MODE:%d\nAPIP:%s\nAPKEY:%s\nAPSSID:%s\nHIDDEN:%d",stAPParam.emShareDev, stAPParam.emSecMode,stAPParam.szAPIP,stAPParam.szKey,stAPParam.szSsid,stAPParam.uchidden);
	cls_show_msg_record(FILENAME ,FUNCNAME ,"WIFI AP打开成功\nAPSSID:%s\nAPKEY:%s\nAPIP:%s\n任意键继续", stAPParam.szSsid, stAPParam.szKey, stAPParam.szAPIP);

	if(flag==1)
	{
		if(TransUp(&h_tcp)!=SUCC)
		{
			NetDown();
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:TransUp失败", __LINE__);
			return;
		}
		send_recv_press(h_tcp);
		TransDown(h_tcp);
	}
	cls_show_msg_record(FILENAME ,FUNCNAME ,"WIFI AP打开成功\nAPSSID:%s\nAPKEY:%s\nAPIP:%s\n任意键退出", stAPParam.szSsid, stAPParam.szKey, stAPParam.szAPIP);
	return;
}

static void close_wifi_ap(void)
{
	/*private & local definition*/
	int ret = -1;
	
	/*process body*/
	//关闭WIFI热点
	if((ret=NDK_WiFiAPClose())!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:%s失败(ret=%d)", __LINE__, TESTITEM, ret);
		return;
	}
	//断开共享网络
	NetDown();
	cls_show_msg_record(FILENAME ,FUNCNAME ,"WIFI AP关闭成功,任意键退出\n");
	return;
}

static void test_wifi_AP(void)
{
	/*private & local definition*/
	int ret = '0', apcommflag = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	while (1)
	{	
		if(auto_flag==1)
		{
			if(( ++ret == '4')||( cls_show_msg1( 2, "即将进行%sAP压力测试,ESC退出",TESTITEM))==ESC)
				return;
		}
		else
		{
			ret = cls_show_msg("1.打开AP共享网络\n"
								"2.关闭AP共享网络\n"
								"3.设置AP数通标志\n");
		}
		switch(ret)
		{
		case '1':
			open_wifi_ap(apcommflag);
			break;
		case '2':
			close_wifi_ap();
			break;
		case '3':
			if(cls_show_msg1(10, "是否进行AP本地数据收发测试:是按【确认】,否则按其它键\n")==ENTER)
				apcommflag=1;
			else
				apcommflag=0;
			break;
		case ESC:
			cls_show_msg("退出测试程序前请进行WIFI AP关闭操作\n");
			return;
			break;
		default:
			continue;
			break;
		}
	}
	return;
}

static void ap_abnormity(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	while (1)
	{
		keyin = cls_show_msg("1.多台设备接入AP\n" 
							"2.休眠测试\n"
							"3.长时间静置\n"
							"4.断电或重启\n");
		switch (keyin)
		{
		case '1':
			cls_show_msg("按任意键打开AP,在AP打开成功界面使用多台设备接入AP，均应能正常数据传输");
			break;
		case '2':
			cls_show_msg("按任意键打开AP,在AP打开成功界面等待AP进入休眠,休眠唤醒后，使用其他设备连接AP应该能正常数据传输");
			break;
		case '3':
			cls_show_msg("按任意键打开AP,在AP打开成功界面长时间放置(至少30分钟)后使用其他设备连接AP应该能正常数据传输");
			break;
		case '4':
			cls_show_msg("按任意键打开AP,在AP打开成功界面使用其他设备连接AP后将POS重启或断电后再进行AP正常测试应该成功");
			break;
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
		open_wifi_ap(0);
		cls_printf("关闭AP共享网络中...");
		close_wifi_ap();
	}
	return;
}


void systest83(void)
{
	/*private & local definition*/
	int ret = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	if(auto_flag==1)
	{
		//自动化测试内容
		//自动配置参数，失败则退出
		//无线配置
		if(conf_conn_AP()!=SUCC)
		{
			cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试,共享网络配置失败,请检查配置文件",TESTITEM);
			return;
		}
		//AP参数设置
		conf_wifi_AP();
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行AP压力测试
		test_wifi_AP();
			
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{	
		ret = cls_show_msg("1.AP压力测试\n"
							"2.AP异常测试\n"
							"7.AP参数设置\n"
							"8.共享网络设置\n");
		
		switch(ret)
		{
		case '1':
			test_wifi_AP();
			break;
		case '2':
			ap_abnormity();
			break;
		case '7':
			conf_wifi_AP();
			break;
		case '8':
			switch(conf_conn_AP())
			{
			case SUCC:
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
			return;
			break;
		default:
			continue;
			break;
		}
	}
	
	return;
}

