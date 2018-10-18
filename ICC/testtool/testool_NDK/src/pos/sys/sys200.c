/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 系统模块
* file name			: 
* Author 			: liny
* version			: 
* DATE				: 20171107
* directory 			: 
* description			: 测试系统模块随机组合情况
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"
#if CPU5892_ENABLE||CPU5830X_ENABLE||OVERSEAS_ENABLE	//589X平台（SP60机型）定时唤醒的精度较低，误差在128秒左右。 所以需要设置大于128的时间才能有效,小于128的时间效果随机
#define WAKETIME	(128)
#else
#define WAKETIME	(60)
#endif

/*---------------constants/macro definition---------------------*/

#define	TESTAPI	"系统模块内随机组合"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
	Getlibvers ,// 0
	SysGetPosTime,
	SysSetPosTime,
	LedStatus,
	SysGetPosInf,
	SysGetConfigInfo,
	SysTime,
#if !K21_ENABLE
	SysSetBeepVol,
	SysGetBeepVol,
	SysSetSuspend,
	SysGetFirmwareInfo,
	SysInitStatisticsData,
	SysGetStatisticsData,
	SysGetPowerInfo,
	LedSetFlickParam,
	SysSetSuspendDuration,
#endif
#if !ANDIROD_ENABLE
	SysBeep,
	SysTimeBeep,
	SysGoSuspend,
	SysGetPowerVol,
	SysStopWatch,
	SysStartWatch,
	SysDelay,
	SysMsDelay,
#endif
#if K21_ENABLE&&!ANDIROD_ENABLE
	SysGetPowerVolRange,
	SysSetPosInfo,
#if !(defined ME50NX||defined ME68||defined ME30S||CPU5810X_ENABLE||defined ME32||defined ME32THM)
	SysGetRemainRAMInfo,
#endif
#endif
#if ANDIROD_ENABLE
	SysGetK21Version ,
#endif
	MaxNum,
}EM_SYS;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: 
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*                                linying              20180211           created
*****************************************************************/
static int sys_randtest(int funnum)
{
	char szVer[16]={0},old[32]={0}; 
	struct tm stGetPosTime;
	struct tm stSetPosTime;
	unsigned int unLen=0, unVol=0, unTime=0;
	char sBuf[128]={0},oldbuf[128]={0};
	int nvalue= -1,ret=-1;
	ulong ultime=0;
	EM_SYS_HWINFO SYS_HWINFO[] = {SYS_HWINFO_GET_POS_TYPE,SYS_HWINFO_GET_HARDWARE_INFO,SYS_HWINFO_GET_BIOS_VER,SYS_HWINFO_GET_POS_USN,SYS_HWINFO_GET_POS_PSN,SYS_HWINFO_GET_BOARD_VER,SYS_HWINFO_GET_CREDITCARD_COUNT,	
	SYS_HWINFO_GET_PRN_LEN,	SYS_HWINFO_GET_POS_RUNTIME,SYS_HWINFO_GET_KEY_COUNT,SYS_HWINFO_GET_CPU_TYPE};
	EM_LED LED[]={LED_RED_X_ON,LED_RED_X_OFF,LED_RED_X_FLICK,LED_YELLOW_X_ON ,LED_YELLOW_X_OFF,LED_YELLOW_X_FLICK,LED_GREEN_X_ON ,LED_GREEN_X_OFF,LED_GREEN_X_FLICK,LED_BLUE_X_ON ,LED_BLUE_X_OFF,
	LED_BLUE_X_FLICK,LED_RFID_RED_ON,LED_RFID_RED_OFF,LED_RFID_RED_FLICK,LED_RFID_YELLOW_ON,LED_RFID_YELLOW_OFF,LED_RFID_YELLOW_FLICK ,LED_RFID_GREEN_ON ,LED_RFID_GREEN_OFF,LED_RFID_GREEN_FLICK,
	LED_RFID_BLUE_ON,LED_RFID_BLUE_OFF,LED_RFID_BLUE_FLICK,LED_COM_ON,LED_COM_OFF,LED_COM_FLICK ,LED_ONL_ON,LED_ONL_OFF,LED_ONL_FLICK,LED_DETECTOR_ON ,LED_DETECTOR_OFF,LED_DETECTOR_FLICK,LED_MAG_ON,
	LED_MAG_OFF,LED_MAG_FLICK};  
#if !K21_ENABLE
	ulong stOutPut = 0;
	unsigned int unVolumn=0;
	EM_SYS_FWINFO emFWinfo;
	ST_LED_FLICK flick_param;
	ST_POWER_INFO power_info;
	EM_SYS_CONFIG SYS_CONFIG[]={SYS_CONFIG_SLEEP_ENABLE,SYS_CONFIG_SLEEP_TIME,SYS_CONFIG_SLEEP_MODE,SYS_CONFIG_LANGUAGE,SYS_CONFIG_APP_AUTORUN};
	EM_LED LEDPARAM[]={LED_RFID_RED_ON ,LED_RFID_RED_OFF ,LED_RFID_RED_FLICK ,LED_RFID_YELLOW_ON ,LED_RFID_YELLOW_OFF,LED_RFID_YELLOW_FLICK,LED_RFID_GREEN_ON,LED_RFID_GREEN_OFF ,LED_RFID_GREEN_FLICK,        	/**<   控制射频绿色灯闪			*/
	LED_RFID_BLUE_ON,LED_RFID_BLUE_OFF,LED_RFID_BLUE_FLICK,LED_COM_ON,LED_COM_OFF,LED_COM_FLICK ,LED_ONL_ON ,LED_ONL_OFF,LED_ONL_FLICK,LED_DETECTOR_ON ,LED_DETECTOR_OFF,LED_DETECTOR_FLICK,
    	LED_MAG_ON,LED_MAG_OFF ,LED_MAG_FLICK};
	EM_SS_DEV_ID devid[] = {SS_KEYBOARD_ENTER, SS_KEYBOARD_BASP,SS_KEYBOARD_ESC,SS_KEYBOARD_ZMK,SS_KEYBOARD_ZERO,SS_KEYBOARD_DOT,\
		SS_KEYBOARD_NINE,SS_KEYBOARD_EIGHT,SS_KEYBOARD_SEVEN,SS_KEYBOARD_SIX,SS_KEYBOARD_FIVE,SS_KEYBOARD_FOUR,SS_KEYBOARD_THREE,\
		SS_KEYBOARD_TWO,SS_KEYBOARD_ONE,SS_KEYBOARD_F1,SS_KEYBOARD_F2,SS_KEYBOARD_F3,SS_KEYBOARD_F4,SS_KEYBOARD_0_ID,SS_KEYBOARD_1_ID,\
		SS_KEYBOARD_2_ID,SS_KEYBOARD_3_ID,/*SS_KEYBOARD_4_ID,SS_KEYBOARD_5_ID,SS_KEYBOARD_6_ID,SS_KEYBOARD_7_ID,SS_KEYBOARD_11_ID,\
		SS_KEYBOARD_15_ID,SS_KEYBOARD_19_ID,SS_KEYBOARD_27_ID,*/SS_KEYBOARD_TOTAL,SS_PRT_PAPER_ID,SS_PRT_HEAT_ID,SS_PRT_STITCH_ID,SS_MAG_TIMES_ID,\
		SS_ICCARD_BASE_ID,SS_RFID_TIMES_ID,SS_MODEM_TIMES_ID,SS_MODEM_FAILTIMES_ID,SS_MODEM_SDLCTIME_ID,SS_MODEM_ASYNTIME_ID,\
		SS_WLS_TIMES_ID,SS_WLS_FAILTIMES_ID,SS_WLS_PPPTIME_ID,SS_WIFI_TIMES_ID,SS_WIFI_TIME_ID,SS_POWER_TIMES_ID,SS_POWERUP_TIME_ID};
#elif ANDIROD_ENABLE
	char verstr[32]={0};
#else
	uint punmin=0, punmax=0;
#if !(defined ME50NX||defined ME68||defined ME30S||CPU5810X_ENABLE)
	uint ram_size=0, ram_addr=0;
#endif
#endif

	srand((unsigned)time(NULL));
    switch(funnum)
	{	
		case Getlibvers:	
			if((ret=NDK_Getlibver(szVer)) != NDK_OK)
				return ret;
			break;
		case SysGetPosTime:  
			if((ret=NDK_SysGetPosTime(&stGetPosTime)) != NDK_OK)
				return ret;
			break;
		case SysSetPosTime:
			if((ret=NDK_SysGetPosTime(&stGetPosTime)) != NDK_OK)
			        return ret;
		        memset(&stSetPosTime, 0, sizeof(stSetPosTime));
		        stSetPosTime.tm_year = 2012 - 1900;
		        stSetPosTime.tm_mon = 2 - 1;
		        stSetPosTime.tm_mday = 29;
		        stSetPosTime.tm_hour = 11;
		        stSetPosTime.tm_min = 11;
		        stSetPosTime.tm_sec = 11;
		        if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_OK)
			        return ret;
			if((ret=NDK_SysSetPosTime(stGetPosTime)) != NDK_OK)//恢复pos最初的时间
			        return ret;
			break;
		case LedStatus:			
			NDK_LedStatus(LED[rand()%36]);
			break;
		case SysGetPosInf:
			NDK_SysGetPosInfo(SYS_HWINFO[rand()%11], &unLen, sBuf);
			break;
		case SysGetConfigInfo:
#if !K21_ENABLE
			if((ret=NDK_SysGetConfigInfo(SYS_CONFIG[rand()%5], &nvalue))!=NDK_OK)
				return ret;
#else
		        if((ret=NDK_SysGetConfigInfo(SYS_CONFIG_LANGUAGE, &nvalue))!=NDK_OK)
				return ret;
#endif
            		break;
        case SysTime:
			if((ret = NDK_SysTime(&ultime)) != NDK_OK)//返回经过的秒
			        return ret;
			break;
#if !K21_ENABLE
        case SysSetBeepVol:
			//if((ret=NDK_SysGetBeepVol(&unVolumn)) != NDK_OK)
			       // return ret;
			       NDK_SysGetBeepVol(&unVolumn);
			//if((ret=NDK_SysSetBeepVol(rand()%6)) != NDK_OK)
			       // return ret;
			       NDK_SysSetBeepVol(rand()%6);
		      // if((ret=NDK_SysSetBeepVol(unVolumn)) != NDK_OK)//恢复最初的beep音量
			      //  return ret;
			      NDK_SysSetBeepVol(unVolumn);
			break;
		case SysGetBeepVol:
		   	if((ret=NDK_SysGetBeepVol(&unVolumn)) != NDK_OK)
				return ret;
			break;
		case SysSetSuspend:
		   	if((ret=NDK_SysSetSuspend(rand()%2)) != NDK_OK)
		       		return ret;
			NDK_SysSetSuspend(1);
			break;
		case SysGetFirmwareInfo: 
		    	if((ret = NDK_SysGetFirmwareInfo(&emFWinfo)) != NDK_OK||(emFWinfo!=SYS_FWINFO_PRO&&emFWinfo!=SYS_FWINFO_DEV))
		        	return ret;
			break;
		case SysInitStatisticsData:
			NDK_SysInitStatisticsData();//部分海外产品不支持统计函数，会返回失败
			break;
		case SysGetStatisticsData:
			NDK_SysGetStatisticsData(devid[rand()%41],&stOutPut);//部分海外产品不支持统计函数，会返回失败
			break;
		case SysGetPowerInfo:
			if((ret=NDK_SysGetPowerInfo(&power_info))!=NDK_OK)
		         	return ret;
			break;
		case LedSetFlickParam:
			NDK_LedSetFlickParam(LEDPARAM[rand()%24],flick_param);
			break;
		case SysSetSuspendDuration:
		  	if((ret=NDK_SysSetSuspendDuration(WAKETIME))!=NDK_OK)
		        	return ret;
			break;
#endif
#if !ANDIROD_ENABLE
        	case SysBeep:			
			NDK_SysBeep();
			break;
		case SysTimeBeep:	
			//if((ret=NDK_SysTimeBeep(2000, 5000))!=NDK_OK)
		       		//return ret;
		    NDK_SysTimeBeep(2000, 5000);
			break;
		case SysGoSuspend:
			 if((ret=NDK_SysGoSuspend())!=NDK_OK )
				 return ret;
			break;
		case SysGetPowerVol:
			if((ret=NDK_SysGetPowerVol(&unVol))!=NDK_OK )
				return ret;
			break;
		case SysStopWatch:
		   	if((ret=NDK_SysStopWatch(&unTime)) != NDK_OK)
		        	return ret;
			break;
		case SysStartWatch:
		  	if((ret=NDK_SysStartWatch()) != NDK_OK)
		        	return ret;
			break;
		case SysDelay:
			if((ret=NDK_SysDelay(20)) != NDK_OK)
			        return ret;
			break;
		case SysMsDelay:
		  	if((ret=NDK_SysMsDelay(2000)) != NDK_OK)
		        	return ret;
			break;
	#endif

#if K21_ENABLE && !ANDIROD_ENABLE
        	case SysGetPowerVolRange:
			//if((ret = NDK_SysGetPowerVolRange(&punmax,&punmin)) != NDK_OK)
				//return ret;
				NDK_SysGetPowerVolRange(&punmax,&punmin);
				break;
			case SysSetPosInfo:
			//NDK_SysSetPosInfo(SYS_HWINFO[rand()%11], "ps2n!@");
	        	memset(oldbuf,0,sizeof(oldbuf));
			//获取测试前配置，测试完后恢复
		       NDK_SysGetPosInfo(SYS_HWINFO_GET_BIOS_VER, &unLen,oldbuf);
		       NDK_SysSetPosInfo(SYS_HWINFO_GET_BIOS_VER, "ps2n!@");
			   NDK_SysSetPosInfo(SYS_HWINFO_GET_BIOS_VER, oldbuf);
			break;
#if !(defined ME50NX||defined ME68||defined ME30S||CPU5810X_ENABLE||defined ME32||defined ME32THM||defined ME65)
		case SysGetRemainRAMInfo:
			if((ret=NDK_SysGetRemainRAMInfo(&ram_addr, &ram_size))!=NDK_OK)
		        	return ret;
			break;
#endif
#endif
#if ANDIROD_ENABLE
		case SysGetK21Version:
		  if((ret=NDk_SysGetK21Version(verstr))!=NDK_OK)
		       	return ret;
		break;
#endif		
	}
	return SUCC;
}

void sys200(void)
{
	/*private & local definition*/
	int num[50] ={0}, i = 0, bak = 0;
	int ret = -1,cnt =50, len = 0,j=0;//cnt 待定	
#if ANDIROD_ENABLE
	char *sysstr[] = {"NDK_Getlibvers" ,"NDK_SysGetPosTime","NDK_SysSetPosTime","NDK_LedStatus","NDK_SysGetPosInfo","NDK_SysGetConfigInfo","NDK_SysTime","NDk_SysGetK21Version "};
#elif K21_ENABLE && !(ANDIROD_ENABLE||defined ME50NX||defined ME68||CPU5810X_ENABLE||defined ME32||defined ME32THM||defined ME30THM)
	char *sysstr[] = {"NDK_Getlibvers" ,"NDK_SysGetPosTime","NDK_SysSetPosTime","NDK_LedStatus","NDK_SysGetPosInfo","NDK_SysGetConfigInfo","NDK_SysTime","NDK_SysBeep","NDK_SysTimeBeep",
		"NDK_SysGoSuspend","NDK_SysGetPowerVol","NDK_SysStoptWatch","NDK_SysStartWatch","NDK_SysDelay","NDK_SysMsDelay","NDK_SysGetPowerVolRange","NDK_SysSetPosInfo""NDK_SysGetRemainRAMInfo"};
#elif defined ME50NX||defined ME68||CPU5810X_ENABLE||defined ME32||defined ME32THM||defined ME30THM
	char *sysstr[] = {"NDK_Getlibvers" ,"NDK_SysGetPosTime","NDK_SysSetPosTime","NDK_LedStatus","NDK_SysGetPosInfo","NDK_SysGetConfigInfo","NDK_SysTime","NDK_SysBeep","NDK_SysTimeBeep",
		"NDK_SysGoSuspend","NDK_SysGetPowerVol","NDK_SysStoptWatch","NDK_SysStartWatch","NDK_SysDelay","NDK_SysMsDelay","NDK_SysGetPowerVolRange","NDK_SysSetPosInfo"};
#else
        char *sysstr[] = {"NDK_Getlibvers" ,"NDK_SysGetPosTime","NDK_SysSetPosTime","NDK_LedStatus","NDK_SysGetPosInfo","NDK_SysGetConfigInfo","NDK_SysTime","NDK_SysSetBeepVol","NDK_SysGetBeepVol",
        	"NDK_SysSetSuspend","NDK_SysGetFirmwareInfo","NDK_SysInitStatisticsData","NDK_SysGetStatisticsData","NDK_SysGetPowerInfo","NDK_LedSetFlickParam","NDK_SysSetSuspendDuration","NDK_SysBeep","NDK_SysTimeBeep",
		"NDK_SysGoSuspend","NDK_SysGetPowerVol","NDK_SysStopWatch","NDK_SysStartWatch","NDK_SysDelay","NDK_SysMsDelay"};
#endif

	/*process body*/
#if !K21_ENABLE
	NDK_SysSetSuspendDuration(WAKETIME); //测试前置设置休眠唤醒时间 以防进入休眠后程序不再运行而是能够唤醒再运行
#endif
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
			if((ret = sys_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",sysstr[num[j]]);
				goto ERR;
			}
		}					
	}
	send_result("%s测试通过", TESTAPI);
ERR:
#if !K21_ENABLE
	NDK_SysSetSuspendDuration(0);
#endif
	NDK_LedStatus(LED_RFID_RED_OFF | LED_RFID_YELLOW_OFF | LED_RFID_GREEN_OFF | LED_RFID_BLUE_OFF); //关闭LED灯
	return;
}


