#ifndef __NDK_SYS_H__
#define	__NDK_SYS_H__

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
#include <sys/utsname.h>
#include <ctype.h>
#include <getopt.h>
#include "NDK.h"
#include "NDK_debug.h"
#include "../public/config.h"

//#define NEXPORT		/**<NDK 对外提供函数、宏、结构等信息，需要在行头部增加该定义,解析工具将自动生成文本*/


#define BEEP_DEV	"/dev/buzz"

#if 0
#define BEEP_IOC_MAGIC 'N'
#define BUZZ_START 			_IOW(BEEP_IOC_MAGIC, 0, int)
#define BUZZ_STOP  			_IO(BEEP_IOC_MAGIC, 1)
#define BUZZ_PWM_SET_PERIOD _IOW(BEEP_IOC_MAGIC, 1, int)
#define BUZZ_PWM_SET_DUTY   _IOW(BEEP_IOC_MAGIC, 2, int)
#define BUZZ_PWM_SET_DIV    _IOW(BEEP_IOC_MAGIC, 3, int)

#define BUZZ_IOCS	_IOW(BEEP_IOC_MAGIC, 4, int)
#define BUZZ_IOCG	_IOR(BEEP_IOC_MAGIC, 5, int)
#define BUZZ_BEEP	_IO(BEEP_IOC_MAGIC, 2)

//LinuxPOS
//#define BEEP_IOC_MAGIC		(0xc2<<8 )
#define BEEP_IOC_SETVOLUMN 	_IO( BEEP_IOC_MAGIC, 3)		 //modify by linrq
#define BEEP_IOC_GETVOLUMN 	_IO( BEEP_IOC_MAGIC, 4)
#define BEEP_IOCG_VER 	   		_IO( BEEP_IOC_MAGIC, 5)
#define BEEP_IOC_MAXNR		3

#define MAX_VOLUMN	2
#define MIN_VOLUMN	0

typedef struct _BUZZ_PARAM {
    int pulse_width;
    int beep_time;
} BUZZ_PARAM;

#endif
/* IOCTL definitions */
#define BUZZ_IOC_MAGIC 'N'
#define BUZZ_START 				_IOW(BUZZ_IOC_MAGIC, 0, int)
#define BUZZ_STOP  				_IO(BUZZ_IOC_MAGIC, 1)
#define BUZZ_PWM_SET_PERIOD 	_IOW(BUZZ_IOC_MAGIC, 1, int)
#define BUZZ_PWM_SET_DUTY   	_IOW(BUZZ_IOC_MAGIC, 2, int)
#define BUZZ_PWM_SET_DIV    	_IOW(BUZZ_IOC_MAGIC, 3, int)

#define BUZZ_IOCS				_IOW(BUZZ_IOC_MAGIC, 4, int)
#define BUZZ_IOCG				_IOR(BUZZ_IOC_MAGIC, 5, int)
#define BUZZ_BEEP				_IO(BUZZ_IOC_MAGIC, 2)

#define BUZZ_IOC_SETVOLUMN 	_IO( BUZZ_IOC_MAGIC, 3)		/*add by linrq*/
#define BUZZ_IOC_GETVOLUMN 	_IO( BUZZ_IOC_MAGIC, 4)
#define BUZZ_IOCG_VER 	   		_IO( BUZZ_IOC_MAGIC, 5)
#define BUZZ_IOCS_RESET		_IO( BUZZ_IOC_MAGIC, 6)		/*for spire SDK接口*/
#define BUZZ_IOCS_QUIET		_IO( BUZZ_IOC_MAGIC, 7)		/*for spire SDK接口*/
#define BUZZ_IOCS_SDKSTART		_IO( BUZZ_IOC_MAGIC, 8)		/*for spire SDK接口*/
#define BUZZ_IOCS_SDKNOWAIT	_IO( BUZZ_IOC_MAGIC, 9)		/*非阻塞调用*/


#define MIN_BUZ_FEQ				100
#define MAX_BUZ_FEQ			4000
typedef struct _BUZZ_PARAM {
    int pulse_width;
    int beep_time;
} BUZZ_PARAM;

typedef struct _SDK_BUZZ_PARAM {
    int beep_time;
    int quiet_time;
    int beep_num;
} SDK_BUZZ_PARAM;





#define LED_IOC_MAGIC 'L'
#define LED_IOCG_VER 	   _IO(LED_IOC_MAGIC, 0)
#define LED_IOCS_STATE	   _IO(LED_IOC_MAGIC, 1)
#define LED_IOCS_FLICK	   _IO(LED_IOC_MAGIC, 4)
#define LED_IOCS_RESET	    _IO(LED_IOC_MAGIC, 5)


/* 设备名称 */
#define POWER_DEV_STRING	"power"

/* 次设备号 */
#define POWER_DEV_MINOR	245

/* IOCTL definitions */
#define POWER_IOC_MAGIC 'P'
#define POWER_SUSPEND  		        _IOW(POWER_IOC_MAGIC, 0, int)
#define POWER_GET_STATUS	        _IO(POWER_IOC_MAGIC, 1)
#define POWER_SET_PID		        _IOW(POWER_IOC_MAGIC, 2, pid_t)
#define POWER_DOWN			        _IO(POWER_IOC_MAGIC, 3)
#define POWER_USB_SUSPEND  		    _IO(POWER_IOC_MAGIC, 4)
#define POWER_USB_RESUME  		    _IO(POWER_IOC_MAGIC, 5)
#define POWER_USB_FREQ  	        _IO(POWER_IOC_MAGIC, 6)
#define POWER_SET_ALARM             _IO(POWER_IOC_MAGIC, 7) //by hanjf 2011-03-31
#define POWER_REBOOT				_IO(POWER_IOC_MAGIC, 8)
#define POWER_SET_SLEEP_DELAY		        _IOW(POWER_IOC_MAGIC, 9, int)
#define POWER_SET_SLEEP_SWITCH		        _IOW(POWER_IOC_MAGIC, 10, int)
#define POWER_SET_SLEEP_MODE		        _IOW(POWER_IOC_MAGIC, 11, int)
#define POWER_DISABLE_POWEROFF		_IO(POWER_IOC_MAGIC, 12)
#define POWER_ENABLE_POWEROFF		_IO(POWER_IOC_MAGIC, 13)


enum power_source {ADAPTER, BATTERY,ADAPTERONLY,USBONLY};

struct power_status {
    enum power_source source;
    int adc_volt;
};

/* Copied from linux/rtc.h to eliminate the kernel dependency */
struct linux_rtc_time {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};


#define RTC_SET_TIME   _IOW('p', 0x0a, struct linux_rtc_time) /* Set RTC time    */
#define RTC_RD_TIME    _IOR('p', 0x09, struct linux_rtc_time) /* Read RTC time   */


typedef struct t_HardwareInfo{
	 int HDRNo;
	 int (*pHDRver)(char *);
}t_HardwareInfo;

typedef struct st_ConfigInfo{
	 int ConfigNo;
	 char *Configname;
}st_ConfigInfo;

typedef struct {
	unsigned char pmType;				/* 电源类型0：交流电1：电池 */
	unsigned char charging;				/* 充电标志0：非充电状态1：充电状态 */
	unsigned char couple;				/* 手机放到底座标志0：未放到底座1：已放到底座 */
	unsigned int adc_volt;				/* 电池电量 */
	unsigned char show_adc;			/* 转换后的电池电量 */
	unsigned int max_volt;			/*当前电池最大电压值*/
	unsigned char rel_state_of_charge;			/*当前电池电压百分比*/
	unsigned short cycle_counter;	/*当前充电次数*/
}PM_STATUS;


typedef enum {
	LED1_Yellow,			/*RFID射频状态灯(黄灯)*/
	LED1_Green,			/*RFID射频状态灯(绿灯)*/
	LED1_Red,			/*RFID射频状态灯(红灯)*/
	LED1_Blue,			/*RFID射频状态灯(蓝灯)*/
	LED2_COM,		/*通讯状态指示灯*/
	LED3_IC,		/*IC卡状态指示*/
	LED3_MAG,		/*磁卡状态指示*/
	LED2_ONL		/*联机状态指示灯*/
}LED_NUM;

typedef enum{
	Led_On,			/*亮*/
	Led_Off,			/*暗*/
	Led_Flick		/*闪烁*/
}LED_STATE;

typedef  struct{
	unsigned char m_ucLen;	/* 字段长度 */
	char m_cData[30];		/* 字段内容 */
    char m_cReserve[3];		/* 保留字段 */
}TField;	/*字段结构*/

typedef struct{
	unsigned char lenl;    /* 产权信息长度低位 */
    unsigned char lenh;    /* 产权信息长度高位 */
	char m_cData[512];
}TProperty;

typedef struct{
    unsigned char lenl;    /* 图标长度低位 */
    unsigned char lenh;    /* 图标长度低位 */
    unsigned char width;   /* 图标宽度 */
	unsigned char high;    /* 图标高度 */
	char icondata[1024];
}TICON;

typedef  struct{
	char m_cType;			/* 类型 */
	char m_cTime[6];		/* 时间（年月日时分秒－BCD）*/
	char m_cReserve[5];		/* 保留字段 */
}TLog;		/*信息结构*/

typedef struct{
    unsigned char bps;      /* 波特率 */
    unsigned char aux;      /* 辅口号 */
    unsigned char flags;    /* 停止位 */

}TIMEI;

typedef  struct{
	TField  m_TFProduce;	/* 生产机号 */
	TField  m_TFUser;		/* 客户序列号 */
	TLog    m_TLInfo[10];	/* 下载信息记录 */
	char    m_cReserve[8];	/* 保留字段 */
}TSNManage;		/* POS序列号信息管理结构 */


typedef struct{
	int lrc;				      /* 扇区异或值 */
	unsigned char  UserLen;       /* 客户序列号长度 */
	unsigned char Propertylenl;   /* 产权信息长度低位 */
	unsigned char Propertylenh;   /* 产权信息长度高位 */
    unsigned char  Producelen;    /* 生产机号长度 */
}TFlag;

typedef struct{
	unsigned char iconflag;       /* 图标有效标志 */
	unsigned char iconwidth;	  /* 图标宽 */
	unsigned char iconhigh;       /* 图标高 */
	unsigned char iconreserve[5]; /* 保留 */

}TIconindex;

typedef struct{
	TFlag m_flag;
	char  m_TFUser[64];           /* 客户序列号 */
	char  m_Property[512];        /* 产权信息 */
	TIconindex m_IconIndex[3];	  /* 图标索引信息 */
	char  m_TFProduce[24];        /* 生产机号 */
	TLog  m_TLInfo[10];		      /* 下载信息记录 */
}NEWTSNManage;

enum cpld_cs {
    PIO_nWE0 = 16, 	//PA16
    PIO_nWE1,
    PRN_nWE0,
    PRN_nWE1,
    PIO_nWE2 = 31 	//PB31
};

struct cpld_val {
    int cs;
    unsigned char mask;
    unsigned char val;
};


#define SERIALNOFILE   "/mnt/hwinfo/serialno"
#define SNFILE	"/mnt/hwinfo/sn"
#define PNFILE	"/mnt/hwinfo/pn"
#define JIHAOFILE   "/mnt/hwinfo/jihao"
#define PUBKEY_USER_FILE "/etc/key/pubkey_user"

//单位转换
#define SB_PRT_LEN 8	   //热敏
#define SB_PRT_HEAT 1000
#define SB_MODEM_SDLCTIME 10
#define SB_MODEM_ASYNTIME 10
#define SB_WLS_PPPTIME 10
#define SB_WIFI_TIME 10
/* 统一错误码定义 */
#define  SN_EMPTY   (-10)


extern int ndk_getHardWareInfo(char *pinfo);
extern int ndk_getBIOSVer(char *pinfo);
extern int ndk_getPatchVer(char *pinfo);
extern int ndk_getBootVer(char * pinfo);
extern int ndk_getPOSUsn(char *pinfo);
extern int ndk_getPOSPsn(char *pinfo);
extern int ndk_getCreditCardCount(char *pinfo);
extern int ndk_getPOSRuntime(char *pinfo);
extern int ndk_getKeyCount(char *pinfo);
extern int ndk_getPrnLen(char *pinfo);
extern int ndk_getBoardVer(char *pinfo);
extern int ndk_getPOSType(char *pinfo);
extern int ndk_getCpuType(char *pinfo);
extern int ndk_getPubkeyinfo(char *pinfo);
extern time_t ndk_read_rtc(int utc);
extern int ndk_write_rtc(time_t t, int utc);

NEXPORT int NDK_SysGetPosInfo(EM_SYS_HWINFO emFlag,uint *punLen,char *psBuf);

#endif

