/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：    产品开发部
* 日    期：    2012-08-17
* 版    本：    V1.00
* 最后修改人：
* 最后修改日期：
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <dbus/dbus-glib.h>

#include "NDK_Sys.h"
#include "NDK.h"
#include "NDK_debug.h"
#include "../public/delay.h"
#include "libss.h"

#define NDK_LIBVERSION "5.0.7"

#define CHARGING_UP     1
static int led_reset_flag = 0;
static struct timeval ndk_StartTime, ndk_StopTime;
struct cpld_val ndk_ledctlval;

#define NDK_APP_PUBKEY_FILE_DBG "/etc/pubkey_dbg"


const t_HardwareInfo HardwareList[]= {
    { SYS_HWINFO_GET_POS_TYPE ,             ndk_getPOSType  },
    { SYS_HWINFO_GET_HARDWARE_INFO ,        ndk_getHardWareInfo },
    { SYS_HWINFO_GET_BIOS_VER ,             ndk_getBIOSVer  },/**<硬件信息*/
    { SYS_HWINFO_GET_POS_USN ,              ndk_getPOSUsn   },
    { SYS_HWINFO_GET_POS_PSN ,              ndk_getPOSPsn   },
    { SYS_HWINFO_GET_BOARD_VER ,            ndk_getBoardVer},/**<驱动版本*/
    { SYS_HWINFO_GET_CREDITCARD_COUNT ,     ndk_getCreditCardCount },/**<统计*/
    { SYS_HWINFO_GET_PRN_LEN ,              ndk_getPrnLen   },
    { SYS_HWINFO_GET_POS_RUNTIME ,          ndk_getPOSRuntime   },
    { SYS_HWINFO_GET_KEY_COUNT ,            ndk_getKeyCount },
    { SYS_HWINFO_GET_CPU_TYPE ,             ndk_getCpuType  },
    { SYS_HWINFO_GET_BOOT_VER ,             ndk_getBootVer  },
    { SYS_HWINFO_GET_PATCH_VER ,            ndk_getPatchVer },
    { SYS_HWINFO_GET_PUBKEY_VER,            ndk_getPubkeyinfo   },
    { -1 , NULL }
};

const st_ConfigInfo syscfginfo_list[]= {
    { SYS_CONFIG_LANGUAGE ,   "language"    },
    { SYS_CONFIG_SLEEP_ENABLE , "enable_sleep"},
    { SYS_CONFIG_SLEEP_TIME ,   "suspend_time"  },
    { SYS_CONFIG_SLEEP_MODE ,   "sleep_mode"    },
    { SYS_CONFIG_APP_AUTORUN ,  "autorun"   },
    { -1 , NULL }
};

extern int SDK_PMIRebootSystem(void);
extern int SDK_PMIShutDown(void);
extern int SDK_PMISetSuspend(unsigned char flag);
extern int SDK_PMIGoSuspend(void);
extern int SDK_PMIGetPowerStatus(PM_STATUS *pw_status);
extern int SDK_PMISetDuration(unsigned int duration);
//extern int SDK_PMISetPowerMode(unsigned char mode);
extern int SDK_PMIGetConfig(char * confname, cfgValueType type, void * confvalue);
extern int SDK_PMISetConfig(char *confname, cfgValueType type, const void *confvalue);
extern int ss_methodcall(int method_id,ST_SS_INPUT* ss_arg,ST_SS_OUTPUT* ss_out,int *ret);

extern int App_GetCallBack(CallbackMock *NDK_EventMain);
extern int App_GetEventFile(char *pEventFile);
extern int sysdelay(int unDelayTime,int units);

static int ndk_SetAnyLedStatus(unsigned char LedNum, unsigned char state);
#ifdef CONFIG_NLGP730_L1
static void ndk_LedContrl(void);
#endif



/**
 *@brief    获取NDK库版本号
 *@retval   pszVer  版本号字符串,缓冲大小不低于8字节
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(pszVer为NULL)
*/
NEXPORT int NDK_Getlibver(char *pszVer)
{
    if(pszVer==NULL)
        return NDK_ERR_PARA;
    strcpy(pszVer,NDK_LIBVERSION);
    return NDK_OK;
}


/**
 *@brief        系统初始化(暂屏蔽该API)
 *@details
 *@return
 *@li    NDK_OK             操作成功
 *@li   其它NDK_ERRCODE     操作失败
*/
NEXPORT int NDK_SysInit(void)
{
#if 1
    FILE *fp = NULL;
    struct stat f_stat;
    char psEvenFile[256]= {0};
    char szInBuff[1024+8]= {0};
    char Buff[1024]= {0},*pInBuf;
    void *pOut = Buff;
    int nSize = 0,nOutLen = 0,nModuleID = 0,nInLen = 0;
    int iret = -1,nResult = 0 ;

    CallbackMock NDK_EventMain;
    App_GetEventFile(psEvenFile);
    if(stat(psEvenFile, &f_stat) == -1) {
        return NDK_OK;
    }
    if ((nSize = f_stat.st_size) == 0) {
        return NDK_OK;
    }
    fp = fopen(psEvenFile, "r");
    if(fp == NULL) {
        return NDK_OK;
    }
    fseek(fp, 0, SEEK_SET);
    if((nSize != fread(szInBuff,1,nSize,fp))||(nSize<=8)) {
        nResult = NDK_ERR_READ;
        goto ERR;
    }
    memcpy(&nModuleID,szInBuff,4);
    memcpy(&nInLen,szInBuff+4,4);
    pInBuf = szInBuff +8;
    iret = App_GetCallBack(&NDK_EventMain);
    if(iret != NDK_OK) {
        nResult = NDK_ERR;
        goto ERR;
    }
    iret = NDK_EventMain(nModuleID,pInBuf,nInLen,pOut,&nOutLen);
    if(iret != 0) {
        nResult =iret;
        goto ERR;
    }
    fclose(fp);
    fp = fopen(psEvenFile, "w+");
    if(fp == NULL) {
        nResult = NDK_ERR_WRITE;
        goto ERR;
    }
    fseek(fp, 0, SEEK_SET);
    fwrite(&nResult,1,1,fp);
    fwrite(pOut,nOutLen,1,fp);
    fclose(fp);
    exit(0);
    return NDK_OK;
ERR:
    fclose(fp);
    fp = fopen(psEvenFile, "w+");
    if(fp == NULL) {
        return NDK_ERR_WRITE;
    }
    fseek(fp, 0, SEEK_SET);
    fwrite(&nResult,1,1,fp);
    fclose(fp);
    exit(0);
    return NDK_ERR;
#endif
}

/**
 *@brief        系统退出
 *@details
 *@return
 *@li    NDK_OK             操作成功
 *@li   其它NDK_ERRCODE     操作失败
*/
NEXPORT int NDK_SysExit(int nErrcode)
{
    exit(nErrcode);
    return NDK_OK;
}

/**
 *@brief        POS重启
 *@details
 *@return
 *@li    NDK_OK             操作成功
 *@li   其它NDK_ERRCODE     操作失败
*/
NEXPORT int NDK_SysReboot(void)
{
	if(SDK_PMIRebootSystem())
		return NDK_ERR;
    return NDK_OK;
}


/**
 *@brief        POS关机
 *@details
 *@return
 *@li    NDK_OK             操作成功
 *@li   其它NDK_ERRCODE     操作失败
*/
NEXPORT int NDK_SysShutDown(void)
{
    if(SDK_PMIShutDown())
		return NDK_ERR;
    return NDK_OK;
}

int (*g_callback_shutdown)(void);
int runbeforeshutdown()
{
	int ret;
	ret = g_callback_shutdown();
	remove("/tmp/.disable_powerdown");
	return 0;
	
}
int NDK_SysCallBackOnShutDown(int (*callback)(void) )
{
	int fd;

	if(callback==NULL)
		return NDK_ERR_PARA;
	fd = open("/tmp/.disable_powerdown",O_RDWR|O_CREAT);
	if(fd<0)
		return NDK_ERR;
	g_callback_shutdown = callback;
	signal(SIGTERM, runbeforeshutdown);
	close(fd);
	return NDK_OK;
}
/**
 *@brief        Beep只响一声，如果要连续响多声，可以在中间加延时
 *@details
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_OPEN_DEV       打开设备文件失败
 *@li    NDK_ERR_IOCTL          驱动调用错误
*/
NEXPORT int NDK_SysBeep(void)
{
    int fd,ret;

    fd = open(BEEP_DEV, O_RDWR);
    if (fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }
    ret = ioctl(fd, BUZZ_BEEP,0);
    if(ret < 0) {
        close(fd);
        return NDK_ERR_IOCTL;
    }
    close(fd);
    return NDK_OK;
}


/**
 *@brief        设置beep的音量
 *@details
 *@param        unVolumn    所要设置的音量的参数，参数范围为0~5，不设置底层默认为5
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA       参数错误(unVolumn非法)
 *@li    NDK_ERR_OPEN_DEV   打开设备文件失败
*/
NEXPORT int NDK_SysSetBeepVol(uint unVolumn)
{

    int fd, ret;
    int val;
    if(unVolumn < 0||unVolumn > 5)
        return NDK_ERR_PARA;
    val=unVolumn*2;
    fd = open(BEEP_DEV, O_RDWR);
    if (fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }
    //fprintf(stderr,"set-vol:%d,%d,%d\n",val,&val,*(&val));
    ret = ioctl(fd, BUZZ_IOC_SETVOLUMN, val);

    //if(ret < 0){
    //  close(fd);
    //  fprintf(stderr,"ioctl err\n");
    //  return NDK_ERR_IOCTL;
    //  }
    close(fd);
    return NDK_OK;
}


/**
 *@brief        取beep的音量
 *@details
 *@retval         punVolumn    所要设置的音量的参数
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA       参数错误(punVolumn非法)
 *@li    NDK_ERR_OPEN_DEV   打开设备文件失败
*/
NEXPORT int NDK_SysGetBeepVol(uint *punVolumn)
{

    int fd, ret;
    int vol,*p=&vol;

    if(punVolumn==NULL)
        return NDK_ERR_PARA;
    fd = open(BEEP_DEV, O_RDWR);
    if (fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }
    ret = ioctl(fd, BUZZ_IOC_GETVOLUMN, p);
    //if(ret < 0){
    //  close(fd);
    //  return NDK_ERR_IOCTL;
    //  }
    *punVolumn =( (*p)/2);
    close(fd);
    return NDK_OK;
}

/**
 *@brief        按一定的频率响一定的时间
 *@details
 *@param       unFrequency声音的频率，单位:Hz    ，范围为0 < unFrequency <=4000
 *@param       unMsSeconds声音持续的时间，单位:ms   ，范围为unMsSeconds > 0
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA       参数错误(unFrequency非法、unMsSeconds小于0)
 *@li    NDK_ERR_OPEN_DEV   打开设备文件失败
 *@li    NDK_ERR_IOCTL          驱动调用错误
*/
NEXPORT int NDK_SysTimeBeep(uint unFrequency,uint unMsSeconds)
{
    int fd, ret;
    if(unFrequency <= 0||unFrequency >4000||unMsSeconds <= 0)
        return NDK_ERR_PARA;

    fd = open(BEEP_DEV, O_RDWR);
    if (fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }


    if ((ret = ioctl(fd, BUZZ_START, unFrequency)) < 0) {
        close(fd);
        return NDK_ERR_IOCTL;
    }

    ndk_udelay(unMsSeconds * 1000);

    if ((ret = ioctl(fd, BUZZ_STOP, 0)) < 0) {
        close(fd);
        return NDK_ERR_IOCTL;
    }

    close(fd);
    return NDK_OK;

}


/**
 *@brief        设置是否允许自动进入休眠
 *@param        unFlag  0:不允许自动进入休眠，1:允许自动进入休眠，其他值参数不合法
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA       参数错误(unFlag非法)
 *@li    NDK_ERR    操作失败
*/
NEXPORT int NDK_SysSetSuspend(uint unFlag)
{
    if((unFlag != 0) && (unFlag != 1))
        return NDK_ERR_PARA;
    if(SDK_PMISetSuspend(unFlag) < 0)
        return NDK_ERR;
    else
        return NDK_OK;
}
/**
 *@brief        设置是否立即进入休眠
 *@details       设置是否自动进入休眠开关对此函数无影响。只要调用机器立即进入休眠
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR    操作失败
*/
NEXPORT int NDK_SysGoSuspend(void)
{
    int ret = -1000;
    ret=SDK_PMIGoSuspend();
    if(ret==0)
        return NDK_OK;
    switch(ret) {
        case -100:
            return NDK_ERR_RFID_BUSY;
        case -200:
            return NDK_ERR_ICCARD_BUSY;
        case -300:
            return NDK_ERR_PRN_BUSY;
        case -400:
            return NDK_ERR_USB_BUSY;
        case -500:
            return NDK_ERR_WLM_BUSY;
        case -600:
            return NDK_ERR_MAG_BUSY;
		case -700:
			return NDK_ERR_PIN_BUSY;
		case -800:
			return NDK_ERR_BT_BUSY;
		case -2:
			return NDK_OK;
        default:
            return NDK_ERR;
    }
}
/**
 *@brief        取电源电量
 *@retval        punVol  如果有插电源则为0，否则返回电池电量
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA       参数非法(punVol为NULL)
 *@li    NDK_ERR    操作失败
*/
NEXPORT int NDK_SysGetPowerVol(uint *punVol)
{
    PM_STATUS status;
    if(punVol == NULL)
        return NDK_ERR_PARA;
    if(SDK_PMIGetPowerStatus(&status) < 0)
        return NDK_ERR;
    *punVol = status.adc_volt;
    if(punVol!=NULL)
        NDK_LOG_INFO(NDK_LOG_MODULE_SYS,"%s succ,current power voltage is %d\n",__func__,*punVol);
    return NDK_OK;
}
#if 0
/**
 *@brief        设置电源模式
 *@param        unMode
 *@return
 *@li    NDK_OK             操作成功
 *@li   其它NDK_ERRCODE     操作失败
*/
int NDK_SysSetPowerMode(uint unMode)
{
    //
    //if(SDK_PMISetPowerMode(unMode) < 0)
    //  return NDK_ERR;
    //else
    return NDK_OK;
}

#endif

/**
 *@brief   设置休眠自动唤醒的时间,最小设置时间为60秒,589X平台（SP60机型）定时唤醒的精度较低，误差在128秒左右。
 *@retval  unSec  单位:秒
 *@return
 *@li    NDK_OK             操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"         参数非法(unSec小于60)
 *@li   \ref NDK_ERR "NDK_ERR"       操作失败
*/
NEXPORT int NDK_SysSetSuspendDuration(uint unSec)
{
    if((unSec<60)&&(unSec!=0))
        return NDK_ERR_PARA;
    if(SDK_PMISetDuration(unSec) < 0)
        return NDK_ERR;
    return NDK_OK;
}

/**
 *@brief        单位延时( 单位时间为0.1s)
 *@details
 *@param       unDelayTime 延时时间，范围unDelayTime > 0
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA       参数非法(unDelayTime小于0)
*/
NEXPORT int NDK_SysDelay(uint unDelayTime)
{
    if (unDelayTime <= 0) {
        return NDK_ERR_PARA;
    }

    return sysdelay(unDelayTime,100*1000);
}

/**
 *@brief        单位延时 (单位时间为1ms)
 *@details
 *@param       unDelayTime 延时时间，范围unDelayTime > 0
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA       参数非法(unDelayTime<=0)
*/
NEXPORT int NDK_SysMsDelay(uint unDelayTime)
{
    if (unDelayTime <= 0) {
        return NDK_ERR_PARA;
    }

    return sysdelay(unDelayTime,1000);
}


/**
 *@brief        取POS当前时间
 *@details
 *@param       pstTime  传入tm结构体类型的指针，返回当前pos时间
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA       参数非法(pstTime为NULL)
*/
NEXPORT int NDK_SysGetPosTime(struct tm *pstTime)
{

    time_t t;
    struct tm *ltm;

    if (pstTime == NULL) {
        return NDK_ERR_PARA;
    }
    t = time(NULL);
    ltm = localtime(&t);
    *pstTime=*ltm;
    return NDK_OK;

}


static int check_date(int year, int month, int day)
{
    time_t time_new;

    if (year < 1900 || month <= 0 || month > 12 || day <= 0 || day > 31) {
        return NDK_ERR;
    }

    //form time
    struct tm tm_new;

    tm_new.tm_year = year - 1900;

    tm_new.tm_mon = month - 1;

    tm_new.tm_mday = day;

    tm_new.tm_hour = 0;

    tm_new.tm_min = 0;

    tm_new.tm_sec = 0;

    time_new = mktime(&tm_new);

    localtime_r(&time_new, &tm_new);

    if (tm_new.tm_year != year - 1900 || tm_new.tm_mon != month - 1 || tm_new.tm_mday != day) {
        return NDK_ERR;
    } else {
        return NDK_OK;
    }
}


/**
 *@brief        设置POS当前时间
 *@details
 *@param       stTime  传入tm结构体类型的变量，设置pos时间为变量time的时间
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA       参数非法(stTime非法)
 *@li    NDK_ERR        操作失败(调用mktime()/stime()失败返回)
*/
NEXPORT int NDK_SysSetPosTime(struct tm stTime)
{

    time_t t;
    struct tm *ltm;

    if((stTime.tm_sec < 0 )||(stTime.tm_sec > 59)) {
        return NDK_ERR_PARA;
    }
    if((stTime.tm_min < 0 )||(stTime.tm_min > 59)) {
        return NDK_ERR_PARA;
    }
    if((stTime.tm_hour < 0 )||(stTime.tm_hour > 23)) {
        return NDK_ERR_PARA;
    }
    if((stTime.tm_mday < 1 )||(stTime.tm_mday >31)) {
        return NDK_ERR_PARA;
    }
    if((stTime.tm_mon < 0 )||(stTime.tm_mon > 11)) {
        return NDK_ERR_PARA;
    }
    if((stTime.tm_year < 0 )||(stTime.tm_year > 137)) {  //时间在40年左右会溢出？
        return NDK_ERR_PARA;
    }
    if(check_date(stTime.tm_year+1900,stTime.tm_mon+1,stTime.tm_mday)!=NDK_OK) {
        return NDK_ERR_PARA;
    }

    ltm = &stTime;
    t = mktime( ltm );
    if (t == -1) {
        return NDK_ERR;
    }

    if (stime(&t) < 0) {
        return NDK_ERR;
    }
    return ndk_write_rtc(t, 0);
}


/**
 *@brief        读取指定字库的内容(接口暂定，未实现)
 *@details      根据偏移取指定字库的n个字节的内容
 *@param      pPath 字库所在的路径
 *@param      unOffset 字库偏移量
 *@param      unLen 要取字库字节数
 *@retval       psBuf 用于存储取出来的字库内容信息
 *@return
 *@li    NDK_OK             操作成功
 *@li   其它NDK_ERRCODE     操作失败
*/
NEXPORT int NDK_SysReadFont(const char * pPath,uint unOffset,char *psBuf,uint unLen)
{
    return NDK_ERR;
}


/**
 *@brief        设置POS上面所有led灯的亮灭情况
 *@details
 *@param      emStatus    枚举类型的变量，控制各个灯的亮灭，不同的各个灯之间可通过相或进行控制。
                    如果相应灯的枚举变量为0(即不或上相应的值)，则相对应的灯的状态不变，如:
                    NDK_LedStatus(LED_RFID_RED_ON|LED_RFID_YELLOW_FLICK),该设置为设置红灯亮，黄灯闪，其他等状态不变。
                    所以控制玩相应的灯之后需要注意是否恢复。
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA   /ref "NDK_ERR_PARA" 参数非法(emStatus)
 *@li    NDK_ERR        /ref "NDK_ERR"      操作失败
*/
NEXPORT int NDK_LedStatus(EM_LED emStatus)
{
    int ret = 0;
    char ledbuf[30] = {0};
    
    if((emStatus & (~0xfff)) != 0)
        return NDK_ERR_PARA;

    if(NDK_SysGetPosInfo(SYS_HWINFO_GET_HARDWARE_INFO,NULL,ledbuf)!=NDK_OK||ledbuf[SYS_HWTYPE_RFIDLED]!=0x01){
        return NDK_ERR_NO_DEVICES;
    }
    
    if((emStatus & 0x03) == LED_RFID_RED_ON)
        ret |= ndk_SetAnyLedStatus(LED1_Red, Led_On);

    if((emStatus & 0x03) == LED_RFID_RED_OFF)
        ret |= ndk_SetAnyLedStatus(LED1_Red, Led_Off);

    if((emStatus & 0x03) == LED_RFID_RED_FLICK)
        ret |= ndk_SetAnyLedStatus(LED1_Red, Led_Flick);

    if((emStatus & 0x0c) == LED_RFID_YELLOW_ON)
        ret |= ndk_SetAnyLedStatus(LED1_Yellow, Led_On);

    if((emStatus & 0x0c) == LED_RFID_YELLOW_OFF)
        ret |= ndk_SetAnyLedStatus(LED1_Yellow, Led_Off);

    if((emStatus & 0x0c) == LED_RFID_YELLOW_FLICK)
        ret |= ndk_SetAnyLedStatus(LED1_Yellow, Led_Flick);

    if((emStatus & 0x30) == LED_RFID_GREEN_ON)
        ret |= ndk_SetAnyLedStatus(LED1_Green, Led_On);

    if((emStatus & 0x30) == LED_RFID_GREEN_OFF)
        ret |= ndk_SetAnyLedStatus(LED1_Green, Led_Off);

    if((emStatus & 0x30) == LED_RFID_GREEN_FLICK)
        ret |= ndk_SetAnyLedStatus(LED1_Green, Led_Flick);


    if((emStatus & 0xc0) == LED_RFID_BLUE_ON)
        ret |= ndk_SetAnyLedStatus(LED1_Blue, Led_On);

    if((emStatus & 0xc0) == LED_RFID_BLUE_OFF)
        ret |= ndk_SetAnyLedStatus(LED1_Blue, Led_Off);

    if((emStatus & 0xc0) == LED_RFID_BLUE_FLICK)
        ret |= ndk_SetAnyLedStatus(LED1_Blue, Led_Flick);

    if((emStatus & 0x300) == LED_COM_ON)
        ret |= ndk_SetAnyLedStatus(LED2_COM, Led_On);

    if((emStatus & 0x300) == LED_COM_OFF)
        ret |= ndk_SetAnyLedStatus(LED2_COM, Led_Off);

    if((emStatus & 0x300) == LED_COM_FLICK)
        ret |= ndk_SetAnyLedStatus(LED2_COM, Led_Flick);

    if((emStatus & 0xc00) == LED_ONL_ON)
        ret |= ndk_SetAnyLedStatus(LED2_ONL, Led_On);

    if((emStatus & 0xc00) == LED_ONL_OFF)
        ret |= ndk_SetAnyLedStatus(LED2_ONL, Led_Off);

    if((emStatus & 0xc00) == LED_ONL_FLICK)
        ret |= ndk_SetAnyLedStatus(LED2_ONL, Led_Flick);
    if(ret != 0)
        return NDK_ERR;

    return NDK_OK;
}

/**
 *@brief        启动跑表，开始计时。由NDK_SysStartWatch()和NDK_SysStopWatch()配合使用。精度在1毫秒以内
 *@details
 *@return
 *@li    NDK_OK             操作成功
*/
NEXPORT int NDK_SysStartWatch(void)
{
    gettimeofday(&ndk_StartTime, NULL);
    return NDK_OK;
}

/**
 *@brief        停止跑表并保存计数值
 *@details
 *@retval         punTime 跑表结束时的计数值
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA   /ref "NDK_ERR_PARA" 参数非法(punTime为NULL)
*/
NEXPORT int NDK_SysStopWatch(uint *punTime)
{
    ulong ret;

    if(punTime == NULL)
        return NDK_ERR_PARA;
    gettimeofday(&ndk_StopTime, NULL);
    ret = (ndk_StopTime.tv_sec - ndk_StartTime.tv_sec)* 1000000;
    ret += (ndk_StopTime.tv_usec - ndk_StartTime.tv_usec);
    ret /= 1000;
    *punTime=ret;
    return NDK_OK;
}



/**
 *@brief        读取pos硬件信息接口
 *@details  如果传入的取硬件信息的索引emFlag不在范围内，则返回参数错误，如果没取到版本信息返回NDK_ERR
            传入的参数的数组大小可暂定为100。api只返回前100个字节的信息
 *@param       emFlag 所要读取设备信息的索引号
 *@retval       punLen 返回传回的buf信息的长度
 *@retval       psBuf用于存储返回的信息
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA   /ref "NDK_ERR_PARA" 参数非法(psBuf为NULL)
 *@li    NDK_ERR    /ref "NDK_ERR"  操作失败
*/
NEXPORT int NDK_SysGetPosInfo(EM_SYS_HWINFO emFlag,uint *punLen,char *psBuf)
{
    char tmpbuf[100];
    int ret,len;

    if(psBuf == NULL)
        return NDK_ERR_PARA;

    memset(tmpbuf,0x00,sizeof(tmpbuf));
    ret = 0;
    for( ret = 0 ; ; ret++ ) {
        if(HardwareList[ret].HDRNo== -1)
            return NDK_ERR_PARA;
        if( HardwareList[ret].HDRNo == emFlag) {
            len =HardwareList[ret].pHDRver(tmpbuf);
            if(punLen != NULL)
                *punLen = len;
            if(len > 0)
                strncpy(psBuf,tmpbuf,len);
            else
                return NDK_ERR;
            return NDK_OK;
        }
    }

}


/**
 *@brief    读取系统配置信息
 *@param    emConfig 所要读取配置信息的索引号
 *@retval   pnvalue 返回的配置值
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA   /ref "NDK_ERR_PARA" 参数非法(pnvalue为NULL)
 *@li    NDK_ERR    /ref "NDK_ERR"  操作失败
*/
NEXPORT int NDK_SysGetConfigInfo(EM_SYS_CONFIG emConfig,int *pnvalue)
{
    int ret;

    if(pnvalue==NULL)
        return NDK_ERR_PARA;
    for( ret = 0 ; ; ret++ ) {
        if(syscfginfo_list[ret].ConfigNo== -1)
            return NDK_ERR_PARA;
        if( syscfginfo_list[ret].ConfigNo == emConfig) {
            if((syscfginfo_list[ret].ConfigNo>=SYS_CONFIG_SLEEP_ENABLE)&&(syscfginfo_list[ret].ConfigNo<=SYS_CONFIG_SLEEP_MODE)) {
                if(SDK_PMIGetConfig(syscfginfo_list[ret].Configname,CFG_INT,pnvalue)<0)
                    return NDK_ERR;
            } else {

                if(ndk_getconfig("sys",syscfginfo_list[ret].Configname,CFG_INT,pnvalue)<0)
                    return NDK_ERR;
            }
            return NDK_OK;
        }
    }
}
static int ndk_LedReset()
{
    int fd;
    int ret;
    fd = open("/dev/led", O_RDWR);
    if (fd < 0)
        return -1;
    ret = ioctl(fd, LED_IOCS_RESET, NULL);
    if (ret < 0) {
        close(fd);
        return -2;
    }
    close(fd);
    return 0;
}
static int ndk_SetAnyLedFlick(unsigned char LedNum, ST_LED_FLICK stFlickParam)
{
    int fd;
    int ret;
    struct __flickparam {
        int lednum;
        ST_LED_FLICK flickparam;
    };
    struct __flickparam tmpflickparam;

    tmpflickparam.lednum=LedNum;
    tmpflickparam.flickparam=stFlickParam;

    fd = open("/dev/led", O_RDWR);
    if (fd < 0)
        return -1;


    ret = ioctl(fd, LED_IOCS_FLICK, &tmpflickparam);
    if (ret < 0) {
        close(fd);
        return -2;
    }
    close(fd);
    return 0;
}
static int ndk_SetAnyLedStatus(unsigned char LedNum, unsigned char state)
{
    int fd;
    int ret;
    int value=0;

//  if((LedNum<LED1_Yellow)||(LedNum>LED1_Blue)||(state>Led_Flick)||(state<Led_On))
//      return -1;

    fd = open("/dev/led", O_RDWR);
    if (fd < 0)
        return -1;

    value |= (LedNum<<4)|state; /*前四位为操作的灯号，后四位为操作的类型*/

    ret = ioctl(fd, LED_IOCS_STATE, value);
    if (ret < 0) {
        close(fd);
        return -2;
    }
    close(fd);
    return 0;
}

/**
 *@brief    清除统计信息
 *@param
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR    /ref "NDK_ERR"  操作失败(统计服务dbus通讯失败)
*/
int NDK_SysInitStatisticsData(void)
{
    NDK_LOG_INFO(NDK_LOG_MODULE_SYS,"call %s\n",__func__);
    int ret;
    ret=SS_InitStatisticsData();
    if(ret!=0)
        return NDK_ERR;
    return NDK_OK;
}

/**
 *@brief    获取统计信息
 *@param  emDevId   要查询的设备ID,参考\ref EM_SS_DEVID "EM_SS_DEVID".
 *@retval   pulValue    统计值
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR    /ref "NDK_ERR"  操作失败(统计服务dbus通讯失败)
 *@li    NDK_ERR_PARA   /ref "NDK_ERR_PARA" 参数非法(pulValue为NULL、emDevId不在枚举范围内)
*/
int NDK_SysGetStatisticsData(EM_SS_DEV_ID emDevId,ulong *pulValue)
{
    int nRet;
    ulong tempvalue = 0;
    char buf[20] = {0};

    if(pulValue==NULL)
        return NDK_ERR_PARA;
    if ((nRet=NDK_SysGetPosInfo(1,NULL,buf))<0) {
        NDK_LOG_DEBUG(NDK_LOG_MODULE_PRINT, "call NDK_SysGetPosInfo err\r\n");
        return nRet;
    }
    if(emDevId<SS_KEYBOARD_0_ID||emDevId>SS_POWERUP_TIME_ID)
        return NDK_ERR_PARA;
    nRet=SS_GetStatisticsOffData(emDevId,&tempvalue);
    if(nRet!=0)
        return NDK_ERR;
    else {
        switch (emDevId) {                                              //单位转换
            case SS_PRT_PAPER_ID:
                if(buf[SYS_HWTYPE_PRINTER]&0x80)
                    *pulValue = tempvalue/(24.0)*4.235;
                else
                    *pulValue = tempvalue/SB_PRT_LEN;
                break;
            case SS_PRT_HEAT_ID:
                *pulValue = tempvalue;
                break;
            case SS_MODEM_SDLCTIME_ID:
                *pulValue = tempvalue*SB_MODEM_SDLCTIME;
                break;
            case SS_MODEM_ASYNTIME_ID:
                *pulValue = tempvalue*SB_MODEM_ASYNTIME;
                break;
            case SS_WLS_PPPTIME_ID:
                *pulValue = tempvalue*SB_WLS_PPPTIME;
                break;
            case SS_WIFI_TIME_ID:
                *pulValue = tempvalue*SB_WIFI_TIME;
                break;
            default:
                *pulValue = tempvalue;
                break;
        }
    }
    return NDK_OK;
}

/**
 *@brief    获取固件类型
 *@retval   emFWinfo    返回的固件类型,参考\ref EM_SYS_FWINFO "EM_SYS_FWINFO".
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA   \ref "NDK_ERR_PARA" 参数非法(emFWinfo为NULL)
*/
int NDK_SysGetFirmwareInfo(EM_SYS_FWINFO *emFWinfo)
{
    if(emFWinfo==NULL) {
        return NDK_ERR_PARA;
    }
    if(access(NDK_APP_PUBKEY_FILE_DBG, F_OK)>=0) {
        *emFWinfo = SYS_FWINFO_DEV;
    } else
        *emFWinfo = SYS_FWINFO_PRO;
    return NDK_OK;
}
/**
 *@brief        获取POS当前时间单位为秒
 *@details  获取的时间以秒单位，从1970年1月1日0时0分0秒开始计算到现在经过了多少秒的时间。
 *@retval   ulTime  返回所经过的秒
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA   \ref "NDK_ERR_PARA" 参数非法(ulTime为NULL)
*/
int NDK_SysTime(ulong *ulTime)
{
    time_t t;

    if (ulTime == NULL) {
        return NDK_ERR_PARA;
    }
    t = time(NULL);
    *ulTime=t;
    return NDK_OK;
}

/**
 *@brief    获取电源信息
 *@param   pstPowerInfo    电源信息结构，参考\ref ST_POWER_INFO "ST_POWER_INFO ".
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR    /ref "NDK_ERR"  操作失败
 *@li    NDK_ERR_PARA   /ref "NDK_ERR_PARA" 参数非法(pstPowerInfo为NULL)
*/

int NDK_SysGetPowerInfo(ST_POWER_INFO *pstPowerInfo)
{
    PM_STATUS pms = {0};

    if(pstPowerInfo==NULL)
        return NDK_ERR_PARA;
    memset(pstPowerInfo,0,sizeof(pstPowerInfo));
    if(SDK_PMIGetPowerStatus(&pms) < 0)
        return NDK_ERR;

    if(pms.pmType == ADAPTER) {
        pstPowerInfo->unPowerType=0x03;
        if(pms.charging != CHARGING_UP) { //充电完成
            pstPowerInfo->unIsCharging= 0;
        } else { //充电中
            pstPowerInfo->unIsCharging = 1;
        }
    } else if(pms.pmType == ADAPTERONLY) { //仅外电
        pstPowerInfo->unPowerType=0x01;
        pstPowerInfo->unIsCharging=0;
    }else if(pms.pmType == USBONLY )  {
        pstPowerInfo->unPowerType=0x04;
        pstPowerInfo->unIsCharging=0;			
	}else if(pms.pmType == BATTERY) { //仅电池
        pstPowerInfo->unPowerType=0x02;
        pstPowerInfo->unIsCharging=0;
    } else {
        pstPowerInfo->unPowerType=0xff;
        pstPowerInfo->unIsCharging=0xff;
        pstPowerInfo->unBattryPercent=0xff;
        return NDK_OK;
    }

    pstPowerInfo->unBattryPercent=pms.rel_state_of_charge;
    return NDK_OK;
}

/**
 *@brief  设置led灯的闪烁参数
 *@details
 *@param      emStatus    枚举类型的变量，只对FLICK有效，如LED_RFID_RED_FLICK ，LED_RFID_YELLOW_FLICK，可指定不同LED灯配置各自的闪烁参数
                   param  ST_LED_FLICK 类型，设置闪烁属性
 *@return
 *@li  NDK_OK 操作成功
 *@li  NDK_ERR_PARA /ref "NDK_ERR_PARA" 参数非法(emStatus)
 *@li  NDK_ERR        /ref "NDK_ERR" 操作失败
*/
int NDK_LedSetFlickParam(EM_LED emStatus,ST_LED_FLICK stFlickParam)
{
    int ret = 0;

    if((emStatus & (~0xfff)) != 0)
        return NDK_ERR_PARA;

    if(led_reset_flag==0)
        ndk_LedReset();

    if((emStatus & 0x03) == LED_RFID_RED_FLICK)
        ret |= ndk_SetAnyLedFlick(LED1_Red, stFlickParam);

    if((emStatus & 0x0c) == LED_RFID_YELLOW_FLICK)
        ret |= ndk_SetAnyLedFlick(LED1_Yellow, stFlickParam);

    if((emStatus & 0x30) == LED_RFID_GREEN_FLICK)
        ret |= ndk_SetAnyLedFlick(LED1_Green, stFlickParam);

    if((emStatus & 0xc0) == LED_RFID_BLUE_FLICK)
        ret |= ndk_SetAnyLedFlick(LED1_Blue, stFlickParam);

    if((emStatus & 0x300) == LED_COM_FLICK)
        ret |= ndk_SetAnyLedFlick(LED2_COM, stFlickParam);

    if((emStatus & 0xc00) == LED_ONL_FLICK)
        ret |= ndk_SetAnyLedFlick(LED2_ONL, stFlickParam);
    if(ret != 0)
        return NDK_ERR;
    led_reset_flag=1;
    return NDK_OK;
}
/**
 *@brief        标准C库函数signal
 *@details	设置某一信号的对应动作
 *@param	nSigNum	指明了所要处理的信号类型(仅支持SIG_USER1、SIG_USER2)
 *@param	pHandler	描述了与信号关联的动作
 *@return
 *@li   返回先前的信号处理函数指针	成功
 *@li   SIG_ERR(-1)  出错(signum为不支持直接返回SIG_ERR)
*/
sighandler_t NDK_Signal(int nSigNum, sighandler_t pHandler)
{
	if(SIGUSR1!=nSigNum||SIGUSR2!=nSigNum)
		return SIG_ERR;
    return signal(nSigNum,pHandler);
}
/* End of this file */


