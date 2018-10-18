/*
* �´�½��˾ ��Ȩ����(c) 2011-2015
*
* ͳһƽ̨NDK API
* ��    �ߣ�    ��Ʒ������
* ��    �ڣ�    2012-08-17
* ��    ����    V1.00
* ����޸��ˣ�
* ����޸����ڣ�
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
    { SYS_HWINFO_GET_BIOS_VER ,             ndk_getBIOSVer  },/**<Ӳ����Ϣ*/
    { SYS_HWINFO_GET_POS_USN ,              ndk_getPOSUsn   },
    { SYS_HWINFO_GET_POS_PSN ,              ndk_getPOSPsn   },
    { SYS_HWINFO_GET_BOARD_VER ,            ndk_getBoardVer},/**<�����汾*/
    { SYS_HWINFO_GET_CREDITCARD_COUNT ,     ndk_getCreditCardCount },/**<ͳ��*/
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
 *@brief    ��ȡNDK��汾��
 *@retval   pszVer  �汾���ַ���,�����С������8�ֽ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszVerΪNULL)
*/
NEXPORT int NDK_Getlibver(char *pszVer)
{
    if(pszVer==NULL)
        return NDK_ERR_PARA;
    strcpy(pszVer,NDK_LIBVERSION);
    return NDK_OK;
}


/**
 *@brief        ϵͳ��ʼ��(�����θ�API)
 *@details
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li   ����NDK_ERRCODE     ����ʧ��
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
 *@brief        ϵͳ�˳�
 *@details
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li   ����NDK_ERRCODE     ����ʧ��
*/
NEXPORT int NDK_SysExit(int nErrcode)
{
    exit(nErrcode);
    return NDK_OK;
}

/**
 *@brief        POS����
 *@details
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li   ����NDK_ERRCODE     ����ʧ��
*/
NEXPORT int NDK_SysReboot(void)
{
	if(SDK_PMIRebootSystem())
		return NDK_ERR;
    return NDK_OK;
}


/**
 *@brief        POS�ػ�
 *@details
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li   ����NDK_ERRCODE     ����ʧ��
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
 *@brief        Beepֻ��һ�������Ҫ������������������м����ʱ
 *@details
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_OPEN_DEV       ���豸�ļ�ʧ��
 *@li    NDK_ERR_IOCTL          �������ô���
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
 *@brief        ����beep������
 *@details
 *@param        unVolumn    ��Ҫ���õ������Ĳ�����������ΧΪ0~5�������õײ�Ĭ��Ϊ5
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA       ��������(unVolumn�Ƿ�)
 *@li    NDK_ERR_OPEN_DEV   ���豸�ļ�ʧ��
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
 *@brief        ȡbeep������
 *@details
 *@retval         punVolumn    ��Ҫ���õ������Ĳ���
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA       ��������(punVolumn�Ƿ�)
 *@li    NDK_ERR_OPEN_DEV   ���豸�ļ�ʧ��
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
 *@brief        ��һ����Ƶ����һ����ʱ��
 *@details
 *@param       unFrequency������Ƶ�ʣ���λ:Hz    ����ΧΪ0 < unFrequency <=4000
 *@param       unMsSeconds����������ʱ�䣬��λ:ms   ����ΧΪunMsSeconds > 0
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA       ��������(unFrequency�Ƿ���unMsSecondsС��0)
 *@li    NDK_ERR_OPEN_DEV   ���豸�ļ�ʧ��
 *@li    NDK_ERR_IOCTL          �������ô���
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
 *@brief        �����Ƿ������Զ���������
 *@param        unFlag  0:�������Զ��������ߣ�1:�����Զ��������ߣ�����ֵ�������Ϸ�
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA       ��������(unFlag�Ƿ�)
 *@li    NDK_ERR    ����ʧ��
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
 *@brief        �����Ƿ�������������
 *@details       �����Ƿ��Զ��������߿��ضԴ˺�����Ӱ�졣ֻҪ���û���������������
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR    ����ʧ��
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
 *@brief        ȡ��Դ����
 *@retval        punVol  ����в��Դ��Ϊ0�����򷵻ص�ص���
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA       �����Ƿ�(punVolΪNULL)
 *@li    NDK_ERR    ����ʧ��
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
 *@brief        ���õ�Դģʽ
 *@param        unMode
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li   ����NDK_ERRCODE     ����ʧ��
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
 *@brief   ���������Զ����ѵ�ʱ��,��С����ʱ��Ϊ60��,589Xƽ̨��SP60���ͣ���ʱ���ѵľ��Ƚϵͣ������128�����ҡ�
 *@retval  unSec  ��λ:��
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"         �����Ƿ�(unSecС��60)
 *@li   \ref NDK_ERR "NDK_ERR"       ����ʧ��
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
 *@brief        ��λ��ʱ( ��λʱ��Ϊ0.1s)
 *@details
 *@param       unDelayTime ��ʱʱ�䣬��ΧunDelayTime > 0
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA       �����Ƿ�(unDelayTimeС��0)
*/
NEXPORT int NDK_SysDelay(uint unDelayTime)
{
    if (unDelayTime <= 0) {
        return NDK_ERR_PARA;
    }

    return sysdelay(unDelayTime,100*1000);
}

/**
 *@brief        ��λ��ʱ (��λʱ��Ϊ1ms)
 *@details
 *@param       unDelayTime ��ʱʱ�䣬��ΧunDelayTime > 0
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA       �����Ƿ�(unDelayTime<=0)
*/
NEXPORT int NDK_SysMsDelay(uint unDelayTime)
{
    if (unDelayTime <= 0) {
        return NDK_ERR_PARA;
    }

    return sysdelay(unDelayTime,1000);
}


/**
 *@brief        ȡPOS��ǰʱ��
 *@details
 *@param       pstTime  ����tm�ṹ�����͵�ָ�룬���ص�ǰposʱ��
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA       �����Ƿ�(pstTimeΪNULL)
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
 *@brief        ����POS��ǰʱ��
 *@details
 *@param       stTime  ����tm�ṹ�����͵ı���������posʱ��Ϊ����time��ʱ��
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA       �����Ƿ�(stTime�Ƿ�)
 *@li    NDK_ERR        ����ʧ��(����mktime()/stime()ʧ�ܷ���)
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
    if((stTime.tm_year < 0 )||(stTime.tm_year > 137)) {  //ʱ����40�����һ������
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
 *@brief        ��ȡָ���ֿ������(�ӿ��ݶ���δʵ��)
 *@details      ����ƫ��ȡָ���ֿ��n���ֽڵ�����
 *@param      pPath �ֿ����ڵ�·��
 *@param      unOffset �ֿ�ƫ����
 *@param      unLen Ҫȡ�ֿ��ֽ���
 *@retval       psBuf ���ڴ洢ȡ�������ֿ�������Ϣ
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li   ����NDK_ERRCODE     ����ʧ��
*/
NEXPORT int NDK_SysReadFont(const char * pPath,uint unOffset,char *psBuf,uint unLen)
{
    return NDK_ERR;
}


/**
 *@brief        ����POS��������led�Ƶ��������
 *@details
 *@param      emStatus    ö�����͵ı��������Ƹ����Ƶ����𣬲�ͬ�ĸ�����֮���ͨ�������п��ơ�
                    �����Ӧ�Ƶ�ö�ٱ���Ϊ0(����������Ӧ��ֵ)�������Ӧ�ĵƵ�״̬���䣬��:
                    NDK_LedStatus(LED_RFID_RED_ON|LED_RFID_YELLOW_FLICK),������Ϊ���ú�������Ƶ�����������״̬���䡣
                    ���Կ�������Ӧ�ĵ�֮����Ҫע���Ƿ�ָ���
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA   /ref "NDK_ERR_PARA" �����Ƿ�(emStatus)
 *@li    NDK_ERR        /ref "NDK_ERR"      ����ʧ��
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
 *@brief        �����ܱ���ʼ��ʱ����NDK_SysStartWatch()��NDK_SysStopWatch()���ʹ�á�������1��������
 *@details
 *@return
 *@li    NDK_OK             �����ɹ�
*/
NEXPORT int NDK_SysStartWatch(void)
{
    gettimeofday(&ndk_StartTime, NULL);
    return NDK_OK;
}

/**
 *@brief        ֹͣ�ܱ��������ֵ
 *@details
 *@retval         punTime �ܱ����ʱ�ļ���ֵ
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA   /ref "NDK_ERR_PARA" �����Ƿ�(punTimeΪNULL)
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
 *@brief        ��ȡposӲ����Ϣ�ӿ�
 *@details  ��������ȡӲ����Ϣ������emFlag���ڷ�Χ�ڣ��򷵻ز����������ûȡ���汾��Ϣ����NDK_ERR
            ����Ĳ����������С���ݶ�Ϊ100��apiֻ����ǰ100���ֽڵ���Ϣ
 *@param       emFlag ��Ҫ��ȡ�豸��Ϣ��������
 *@retval       punLen ���ش��ص�buf��Ϣ�ĳ���
 *@retval       psBuf���ڴ洢���ص���Ϣ
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA   /ref "NDK_ERR_PARA" �����Ƿ�(psBufΪNULL)
 *@li    NDK_ERR    /ref "NDK_ERR"  ����ʧ��
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
 *@brief    ��ȡϵͳ������Ϣ
 *@param    emConfig ��Ҫ��ȡ������Ϣ��������
 *@retval   pnvalue ���ص�����ֵ
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA   /ref "NDK_ERR_PARA" �����Ƿ�(pnvalueΪNULL)
 *@li    NDK_ERR    /ref "NDK_ERR"  ����ʧ��
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

    value |= (LedNum<<4)|state; /*ǰ��λΪ�����ĵƺţ�����λΪ����������*/

    ret = ioctl(fd, LED_IOCS_STATE, value);
    if (ret < 0) {
        close(fd);
        return -2;
    }
    close(fd);
    return 0;
}

/**
 *@brief    ���ͳ����Ϣ
 *@param
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR    /ref "NDK_ERR"  ����ʧ��(ͳ�Ʒ���dbusͨѶʧ��)
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
 *@brief    ��ȡͳ����Ϣ
 *@param  emDevId   Ҫ��ѯ���豸ID,�ο�\ref EM_SS_DEVID "EM_SS_DEVID".
 *@retval   pulValue    ͳ��ֵ
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR    /ref "NDK_ERR"  ����ʧ��(ͳ�Ʒ���dbusͨѶʧ��)
 *@li    NDK_ERR_PARA   /ref "NDK_ERR_PARA" �����Ƿ�(pulValueΪNULL��emDevId����ö�ٷ�Χ��)
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
        switch (emDevId) {                                              //��λת��
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
 *@brief    ��ȡ�̼�����
 *@retval   emFWinfo    ���صĹ̼�����,�ο�\ref EM_SYS_FWINFO "EM_SYS_FWINFO".
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA   \ref "NDK_ERR_PARA" �����Ƿ�(emFWinfoΪNULL)
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
 *@brief        ��ȡPOS��ǰʱ�䵥λΪ��
 *@details  ��ȡ��ʱ�����뵥λ����1970��1��1��0ʱ0��0�뿪ʼ���㵽���ھ����˶������ʱ�䡣
 *@retval   ulTime  ��������������
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA   \ref "NDK_ERR_PARA" �����Ƿ�(ulTimeΪNULL)
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
 *@brief    ��ȡ��Դ��Ϣ
 *@param   pstPowerInfo    ��Դ��Ϣ�ṹ���ο�\ref ST_POWER_INFO "ST_POWER_INFO ".
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR    /ref "NDK_ERR"  ����ʧ��
 *@li    NDK_ERR_PARA   /ref "NDK_ERR_PARA" �����Ƿ�(pstPowerInfoΪNULL)
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
        if(pms.charging != CHARGING_UP) { //������
            pstPowerInfo->unIsCharging= 0;
        } else { //�����
            pstPowerInfo->unIsCharging = 1;
        }
    } else if(pms.pmType == ADAPTERONLY) { //�����
        pstPowerInfo->unPowerType=0x01;
        pstPowerInfo->unIsCharging=0;
    }else if(pms.pmType == USBONLY )  {
        pstPowerInfo->unPowerType=0x04;
        pstPowerInfo->unIsCharging=0;			
	}else if(pms.pmType == BATTERY) { //�����
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
 *@brief  ����led�Ƶ���˸����
 *@details
 *@param      emStatus    ö�����͵ı�����ֻ��FLICK��Ч����LED_RFID_RED_FLICK ��LED_RFID_YELLOW_FLICK����ָ����ͬLED�����ø��Ե���˸����
                   param  ST_LED_FLICK ���ͣ�������˸����
 *@return
 *@li  NDK_OK �����ɹ�
 *@li  NDK_ERR_PARA /ref "NDK_ERR_PARA" �����Ƿ�(emStatus)
 *@li  NDK_ERR        /ref "NDK_ERR" ����ʧ��
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
 *@brief        ��׼C�⺯��signal
 *@details	����ĳһ�źŵĶ�Ӧ����
 *@param	nSigNum	ָ������Ҫ������ź�����(��֧��SIG_USER1��SIG_USER2)
 *@param	pHandler	���������źŹ����Ķ���
 *@return
 *@li   ������ǰ���źŴ�����ָ��	�ɹ�
 *@li   SIG_ERR(-1)  ����(signumΪ��֧��ֱ�ӷ���SIG_ERR)
*/
sighandler_t NDK_Signal(int nSigNum, sighandler_t pHandler)
{
	if(SIGUSR1!=nSigNum||SIGUSR2!=nSigNum)
		return SIG_ERR;
    return signal(nSigNum,pHandler);
}
/* End of this file */


