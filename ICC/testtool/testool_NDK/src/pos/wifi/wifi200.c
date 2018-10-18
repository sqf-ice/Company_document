/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module				: wifi模块 
* file name				: 
* Author 				: 
* version				: 
* DATE					: 
* directory 				: 
* description			:wifi模块内随机组合测试(低端WiFi)
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI	"wifi模块内随机组合"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
	WifiProfileGet,
	WifiProfileSet,
	WifiStart,
	WifiStop,
	WifiStatus,
	WifiGetFunc,
	WifiSetFunc,
	NetAddRouterTable,
	MaxNum,//新增枚举值时在此值前面加,要保证此值为最后一个值
}EM_WIFI;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: 
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						 sull				20180212		created
*****************************************************************/
static int wifi_randtest(int funnum)
{
	int ret = -1, data = 0;
	ST_WIFI_PROFILE_LIST list_g;
	ST_WIFI_PROFILE_LIST list_s;
	ST_WIFI_AP_LIST list_scan;
	ST_WIFI_STATUS status;
	ST_WIFI_AP_INFO ap[32];
	list_scan.list = ap;
	ST_WIFI_AP_ENTRY_T set_ap[32], get_ap[32];
	memset(set_ap,0,sizeof(set_ap));//清零
	memset(get_ap,0,sizeof(get_ap));
	list_s.stored_ap = set_ap;
	list_g.stored_ap = get_ap;
	list_scan.num = 32;
	
	switch(funnum)
	{
		case WifiProfileGet:
			memset(get_ap,0,sizeof(get_ap));
			//if((ret = NDK_WifiProfileGet(&list_g)) != NDK_OK)
				//return ret;
			NDK_WifiProfileGet(&list_g);
			break;
		case WifiProfileSet:
			memset(set_ap,0,sizeof(set_ap));
			strcpy(list_s.stored_ap->ssid,"AP2&4same");
			strcpy(list_s.stored_ap->pwd,"1234567890");
			list_s.stored_ap->flag = WIFI_AP_FLAG_DHCP_SET;
			list_s.stored_ap->priority = 0;
			list_s.stored_num = 1;
			//if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
				//return ret;
			NDK_WifiProfileSet(&list_s);	
			break;
		case WifiStart:
			NDK_WifiStart();
			sleep(15); //如果没有延时直接就跑到NDK_NetAddRouterTable 就会有影响导致其他地方不能用 乐鑫WiFi特有 为保持一致其他产品也同步
			break;
		case WifiStop:
			//if((ret = NDK_WifiStop()) != NDK_OK)
				//return ret;
			NDK_WifiStop();
			break;
		case WifiStatus:
			NDK_WifiStatus(&status);
			break;
		case WifiGetFunc:
			memset(ap, 0, sizeof(ap));
			NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan);
			break;
		case WifiSetFunc:
			data = rand() %2;
			//if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_AUTO_START,&data)) != NDK_OK)
				//return ret;
			NDK_WifiSetFunc(WIFI_SET_FUNC_AUTO_START,&data);
			break;
		case NetAddRouterTable:
			NDK_NetAddRouterTable(rand()%4, "0.0.0.0");
			break;		
	}		
	return SUCC;
}

void wifi200(void)
{
	/*private & local definition*/
	int  i = 0, bak = 0, num[30] ={0}, j = 0,data = 0;; 
	int ret = -1,cnt =50, len = 0;//cnt 待定
	char *wifistr[] = {"NDK_WifiProfileGet", "NDK_WifiProfileSet", "NDK_WifiStart", "NDK_WifiStop", "NDK_WifiStatus","NDK_WifiGetFunc", "NDK_WifiSetFunc", "NDK_NetAddRouterTable"};

	/*process body*/
	len = (MaxNum>20)?MaxNum:20;
	while(cnt)
	{	
		cnt--;
		bak++;
		
		//产生随机数
		cls_printf("第%d次随机顺序:",bak);
		srand((unsigned)time(NULL));
		for(i=0;i<len;i++)
		{	
			num[i] = rand()%MaxNum;
			NDK_ScrPrintf("%d ", num[i]);
			NDK_ScrRefresh();
		}
		
		//进行随机测试
		for(i=0;i<len;i++)
		{
			if((ret = wifi_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",wifistr[num[j]]);
				goto ERR;
			}
		}	
	}

	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_WifiSetFunc(WIFI_SET_FUNC_AUTO_START,&data);//恢复默认情况
	NDK_WifiStop();
	return;
}

