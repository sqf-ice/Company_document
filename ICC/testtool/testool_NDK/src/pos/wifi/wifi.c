/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 新wifi模块
* file name			: 
* Author 			:  
* version			: 
* DATE				:  
* directory 			: 
* description			: WIFI模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	: 程序员参考手册
*
************************************************************************
* log			: 
*  Revision 1.0  2011.09.11 bcat
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"WIFI模块"
#define MAXWAITTIME 30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
int scan_get_mac(uchar outmac[6])
{
	/*private & local definition*/
	int ret = 0, i = 0;
	ST_WIFI_AP_LIST list_scan;
	ST_WIFI_AP_INFO ap[32];
	char ssidout[32][33];
	uint ssidoutlen = 0;

	/*process body*/
	//cls_printf("扫描AP中,请稍候...");
	list_scan.num = 32;
	list_scan.list = ap;
	memset(ap,0,sizeof(ap));
	if ((ret=NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan)) != NDK_OK) 
	{
		cls_show_msg("line %d:获取wifi扫描结果失败(%d)", __LINE__,ret);
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
				cls_show_msg("line %d:utf8转换gbk失败(%d)", __LINE__, ret);
				return FAIL;
			}
		}
		if(cls_show_msg("AP%d:%s(%02x:%02x:%02x:%02x:%02x:%02x)是否为测试AP,是[确认],否[其它]", i, ssidout[i],ap[i].mac.octet[0],ap[i].mac.octet[1],ap[i].mac.octet[2],ap[i].mac.octet[3],ap[i].mac.octet[4],ap[i].mac.octet[5])==ENTER)//选择SSID后
		{
			memcpy(outmac, ap[i].mac.octet, 6);
			break;
		}
	}
	if(i>=list_scan.num)
	{
		cls_show_msg("line %d:未扫描到测试用AP请重新扫描", __LINE__);
		return FAIL;
	}
	return SUCC;
}

/****************************************************************
* function name 	 		: WIFI
* functional description 	: WIFI模块入口主函数,可根据需要扩展
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks	
*			  	 	       jiangym		 20160223      created 
*****************************************************************/
void wifi(PF* list, unsigned char id[])
{
	/*private & local definition*/
	int ret=-1, retime = -1;
	
	/*process body*/
	if(cls_show_msg1(MAXWAITTIME, "%s测试...请确保按要求搭建好测试环境(详见搭建文档)...ESC退出本模块测试", TESTMODULE)==ESC)//时间长些,以保证用户有时间操作
		return;
	//恢复自动重连的默认值 :不重连
	retime = -1;
	if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_RELINK_TIME,&retime)) != NDK_OK)
	{
		cls_show_msg("line %d: 设置失败(%d)", __LINE__, ret);
		return;
	}
/*	switch(conf_conn_WLAN())//整个用例的测试环境写死,不用配置
	{
	case SUCC:
		break;
	case FAIL:
		cls_show_msg("line %d:网络未连通!!!\n任意键退出配置...", __LINE__);
		return;
		break;
	default:
		return;
		break;
	}
	*/
	
	//执行测试用例
#if STABAR_ENABLE
	if((ret=NDK_ScrStatusbar(STATUSBAR_DISP_TIME|STATUSBAR_DISP_WIFI))!=NDK_OK)
	{
		cls_show_msg("状态栏显示失败!(%d)", ret);
		return;
	}
#endif
	process(list, id);
#if STABAR_ENABLE	
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);
#endif
	return;
}

