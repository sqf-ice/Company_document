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
#include <sys/utsname.h>
#include <ctype.h>
#include <getopt.h>
#include<sys/mount.h>
#include <sys/sysinfo.h>
#include <linux/input.h>
#include "NDK.h"
#include "NDK_debug.h"
#include "devmgr.h"
#include "NDK_Sys.h"
#include "libconfig.h"


config_t ndk_devconf;
extern int NDK_PrnGetPrintedLen(uint *punLen);


static NEWTSNManage *c_pTSNManageInfo ;
static int snReadUSN(TField *pTFUSN);
static int snReadPSN(TField *pTFPSN);

/*
 *ndk_cpu_state = 1 代表5892,0表示za9l，-1表示未取过cpu类型
 */
static int ndk_cpu_state = -1;


typedef struct {
    char Bootloader[30];  ///< Boot Loader version ID (Format: BOOTL_SS.SS_XXXXXXXX)
    char SP_FW[33];       ///< Spire Firmware ID (Format: SPSYS_SS.SS_FF_XXXXXXXX)
    char KLA[21];         ///< KLA version ID (Format: SPKLA_SS.SS_XXXXXXXX)
} PCI_FW_ID;

static char ndk_getdevinfo(char * devname)
{
    char str[16];
    char c;
    int val;
    get_dev_info(devname, "type",str);
    if(strcmp(str,"")==0)
        return 0xff;
    val = atoi(str);
    if(val == 0)
        return 0xff;
    else {
        c=val&0xff;
        return c;
    }
}

static char ndk_get_tmp_devinfo(config_t *cfg_file,char * devname)
{
    extern int ndk_getconfig_customize(config_t * cfg_file,const char * optname, const char * confname, cfgValueType type, void * confvalue);
    char *str;
    char c;
    int val;

    if(ndk_getconfig_customize(cfg_file,devname, "type",CFG_STRING,&str)<0)
        return 0xff;
    if(strcmp(str,"")==0)
        return 0xff;
    val = atoi(str);
    if(val == 0)
        return 0xff;
    else {
        c=val&0xff;
        return c;
    }
}
static int ndk_get_tmp_POSType_flag = 0;
static char gpostype[256]= {0};
static int ndk_get_tmp_POSType(char *pinfo)
{
    char *str;

    if(!ndk_get_tmp_POSType_flag) {
        ndk_initconfig_customize("/mnt/hwinfo/devmgr.conf",&ndk_devconf);
        if(ndk_getconfig_customize(&ndk_devconf,DEV_CLASS_MACHINE_TYPE, "type",CFG_STRING,&str)<0) {
            ndk_destoryconfig_customize(&ndk_devconf);
            return -1;
        }
        if(strcmp(str,"")==0) {
            ndk_destoryconfig_customize(&ndk_devconf);
            return -1;
        }
        strcpy(pinfo,str);
        ndk_destoryconfig_customize(&ndk_devconf);
        strcpy(gpostype,pinfo);
        ndk_get_tmp_POSType_flag=1;
    } else {
        strcpy(pinfo,gpostype);
    }
    return strlen(pinfo);
}
/*
* 获取POS硬件信息每一个字节代表不同的模块
*同一字节内不同的值代表同一模块的不芯片类型
*至少支持63个外部设备，应用传入参数数组不小于64
*返回的该数组以0x00表示设备类型结束。0x00之前每一字节表示一个模块
*    pinfo[0]:无线模块
*    0xFF     :没有无线模块
*    0x01     :SIM300
*    0x02    :M72
*    0x03    :DTGS800
*    0x04    :SM700
*    0x05    :MC39I
*    0x06    :DTM228C
*    0x07    :MC8331
*    0x08    :EM200
*    pinfo[1]:射频模块
*    0xFF     :没有射频模块
*    0x01     :RC531
*    0x02    :PN512
*    pinfo[2]:磁卡模块
*    0xFF     :没有磁卡模块
*    0x01     :mesh
*    0x02     :giga
*    pinfo[3]:扫描头模块
*    0xFF     :没有磁卡模块
*    0x01     :EM1300
*    0x02     :EM3000
*    0x03     :SE955
*    pinfo[4]:是否支持外接密码键盘
*    0xFF     :不支持
*    0x01     :支持
*    pinfo[5]:串口个数
*    0xFF     :无
*    0x01     :1个
*    0x02     :2个
*    pinfo[6]:是否支持USB
*    0xFF     :否
*    0x01     :是
*    pinfo[7]:MODEM模块
*    0xFF     :否
*    0x01     :是
*    pinfo[8]:wifi模块
*    0xFF     :无wifi模块
*    0x01     :是"USI WM-G-MR-09"模块
*    pinfo[9]:是否支持以太网
*    0xFF     :否
*    0x01     :dm9000
*    0x02     :bcm589xcore
*    pinfo[10]:打印模块
*    0xFF     :无打印模块
*    0x01     :热敏
*    0x02     :针打
*    pinfo[11]:是否支持触屏
*    0xFF     :否
*    0x01     :是
*    pinfo[12]:是否有射频LED灯
*    0xFF     :否
*    0x01     :是
*    pinfo[13]:是否有蓝牙	
*    0xFF     :否	
*    0x01     :是	 
*    pinfo[14]:是否有NFC	
*    0xFF     :否	
*    0x01     :是	  
*    pinfo[15]:是否有国密芯片	
*    0xFF     :否	
*    0x01     :THK88	  
*/
static int ndk_gethardwareinfo_flag = 0;
static char ghardwareinfo[SYS_HWTYPE_MAX]= {0xff};
int ndk_getHardWareInfo(char *pinfo)
{
    if(!ndk_gethardwareinfo_flag) {
        ndk_initconfig_customize("/mnt/hwinfo/devmgr.conf",&ndk_devconf);
        pinfo[SYS_HWTYPE_WIRELESS_MODEM]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_WIRELESS_MODEM);
        pinfo[SYS_HWTYPE_RFID]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_RFID);
        pinfo[SYS_HWTYPE_MAG_CARD]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_MAG_CARD);
        pinfo[SYS_HWTYPE_SCANNER]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_SCANNER);
        pinfo[SYS_HWTYPE_PINPAD]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_PINPAD);
        pinfo[SYS_HWTYPE_AUX]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_AUX);
        pinfo[SYS_HWTYPE_USB]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_USB);
        pinfo[SYS_HWTYPE_MODEM]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_MODEM);
        pinfo[SYS_HWTYPE_WIFI]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_WIFI);
        pinfo[SYS_HWTYPE_ETHERNET]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_ETHERNET);
        pinfo[SYS_HWTYPE_PRINTER]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_PRINTER);
        pinfo[SYS_HWTYPE_TOUCHSCREEN]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_TOUCHSCREEN);
        pinfo[SYS_HWTYPE_RFIDLED]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_RFIDLED);
        pinfo[SYS_HWTYPE_BT]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_BT);
		pinfo[SYS_HWTYPE_NFC]=0xff;//NFC目前不支持
        pinfo[SYS_HWTYPE_GM]=ndk_get_tmp_devinfo(&ndk_devconf,DEV_CLASS_GM);
        ndk_destoryconfig_customize(&ndk_devconf);
        memcpy(ghardwareinfo,pinfo,SYS_HWTYPE_MAX);
        ndk_gethardwareinfo_flag=1;
    } else
        memcpy(pinfo,ghardwareinfo,SYS_HWTYPE_MAX);

    return SYS_HWTYPE_MAX;
}
static int CalculateStringLen(const char *pStr)
{
    int nCnt = 0;
    while(*pStr) {
        if(*pStr == '\r' || *pStr == '\n')
            return nCnt;
        nCnt++;
        pStr++;
    }
    return nCnt;
}
static int GetPCI_FW_ID(PCI_FW_ID *pInfoPCI)
{
    FILE *fp = NULL;
    int nLen = 0;
    char sBuf[64] = {0}, *pIndex = NULL;
    char sDevType[32] = {0};
    char sFilePath[64] = {0};

    if(get_dev_info("machine", "type", sDevType)) {
        return -1;
    }

    strcpy(sFilePath, "/etc/pci_fw_id.");
    strcat(sFilePath, sDevType);
    if((fp = fopen(sFilePath, "r")) == NULL) {
        return -1;
    }

    if(pInfoPCI)
        memset(pInfoPCI, 0, sizeof(PCI_FW_ID));

    if(fgets(sBuf, 63, fp) != NULL) {
        pIndex = strstr(sBuf, "=");
        nLen = CalculateStringLen(pIndex);
        pIndex[nLen] = 0;
        strcpy(pInfoPCI->Bootloader, pIndex+1);
    }

    memset(sBuf, 0, sizeof(sBuf));
    if(fgets(sBuf, 63, fp) != NULL) {
        pIndex = strstr(sBuf, "=");
        nLen = CalculateStringLen(pIndex);
        pIndex[nLen] = 0;
        strcpy(pInfoPCI->SP_FW, pIndex+1);
    }

    memset(sBuf, 0, sizeof(sBuf));
    if(fgets(sBuf, 63, fp) != NULL) {
        pIndex = strstr(sBuf, "=");
        nLen = CalculateStringLen(pIndex);
        pIndex[nLen] = 0;
        strcpy(pInfoPCI->KLA, pIndex+1);
    }
    fclose(fp);
    return 0;
}
int  ndk_getBIOSVer(char *pinfo)
{
    char *p, *q;
    int i, n, digits,ret;
    char buf[3];
    PCI_FW_ID pci_fw_id;

    if(pinfo==NULL)
        return -1;
    ret = GetPCI_FW_ID(&pci_fw_id);
    if (ret < 0)
        return -1;
    p = strrchr(pci_fw_id.SP_FW, '_');
    p++;
    q = pinfo;
    for (i = 0; i < 3; i++, p += 2) {
        buf[0] = p[0];
        buf[1] = p[1];
        buf[2] = '\0';
        n = atoi(buf);
        if (n < 9)
            digits = 1;
        else
            digits = 2;
        sprintf(q, "%d", n);
        if (i < 2) {
            q += digits;
            *(q++) = '.';
        }
    }
    return strlen(pinfo);

}
int ndk_getBootVer(char *pinfo)
{
    char *p;
    int ret,fd;
    PCI_FW_ID pci_fw_id;
	char cmdline[256];

    if(pinfo==NULL)
        return -1;
    ret = GetPCI_FW_ID(&pci_fw_id);
    if (ret < 0)
        return -1;
    p = strrchr(pci_fw_id.Bootloader, '_');
    p++;
    sprintf(pinfo, "P0%d", atoi(p));

	fd = open("/proc/cmdline",O_RDONLY);
	if(fd>0)
	{
		memset(cmdline,0,sizeof(cmdline));
		read(fd,cmdline,sizeof(cmdline));
		close(fd);
		if((p = strstr(cmdline,"bootver="))!=NULL)
		{
			if(strstr(p,"PD"))
			{
				p= strstr(p,"*");
				if(p!=NULL)
				{
					sprintf(pinfo, "01.00_01_0000%02d",atoi(p+1));
				}
			}
			else
			{
				p= strstr(p,"*");
				if(p!=NULL)
				{
					sprintf(pinfo, "01.00_00_0000%02d",atoi(p+1));
				}
			}
		}
	}
    return strlen(pinfo);
}
int  ndk_getPatchVer(char *pinfo)
{
    char *p, *q;
    int i,n,digits,ret;
    char buf[4] = {0};
    PCI_FW_ID pci_fw_id;

    if(pinfo==NULL)
        return -1;
    ret = GetPCI_FW_ID(&pci_fw_id);
    if (ret < 0)
        return -1;
    p = strrchr(pci_fw_id.SP_FW, '_');
    p++;
    q = pinfo;
    for (i = 0; i < 4; i++, p += 2) {
        if(i==3) {
            buf[0] = p[0];
            buf[1] = p[1];
            buf[2] = '\0';
            if(NDK_IsDigitStr((const uchar*)buf)==NDK_OK) {
                n = atoi(buf);
                if (n < 9)
                    digits = 1;
                else
                    digits = 2;
                sprintf(q, "%d", n);
            } else {
                strcat(q,buf);
            }
        }
    }
    return strlen(pinfo);
}

int ndk_getPubkeyinfo(char *pinfo)
{
    char keyowner[32]= {0};
    char keyver[16]= {0};
    char flagbuf[16]= {0};
    char buf[64]= {0};
    int FileFd;

    if(pinfo==NULL)
        return -1;
    if(access(PUBKEY_USER_FILE,F_OK)<0) {
        strcpy(pinfo,"NewlandPayment v2.0");
    } else {
        if ((FileFd=open(PUBKEY_USER_FILE, O_RDONLY))<0) {
            fprintf(stderr,"%s,%d,open  file fail!\n",__FUNCTION__,__LINE__);
            return -1;
        }
        lseek(FileFd,-64,SEEK_END);
        if(read(FileFd,flagbuf,16)==-1) {
            fprintf(stderr,"%s,%d,read flag fail!\n",__FUNCTION__,__LINE__);
            close(FileFd);
            return -1;
        }
        if(memcmp(flagbuf,"newland",7)==0) {
            if(read(FileFd,keyowner,32)==-1) {
                fprintf(stderr,"%s,%d,read keyowner fail!\n",__FUNCTION__,__LINE__);
                close(FileFd);
                return -1;
            }
            if(read(FileFd,keyver,16)==-1) {
                fprintf(stderr,"%s,%d,read keyver fail!\n",__FUNCTION__,__LINE__);
                close(FileFd);
                return -1;
            }
            sprintf(buf,"%s_%s",keyowner,keyver);
            strcpy(pinfo,buf);
        } else {
            strcpy(pinfo,"unknown");
        }
        close(FileFd);
    }
    return strlen(pinfo);
}

int ndk_getPOSUsn(char *pinfo)
{
    TField message;
    int ret;
//  if (mount("/dev/mtdblock3", "/mnt/hwinfo", "yaffs2", 0, NULL) != 0)
//      return -1;
    ret = snReadUSN(&message);
//  if(umount("/mnt/hwinfo")!=0)
//      return -2;
    if(ret<0)
        return ret;
    strncpy(pinfo,message.m_cData,message.m_ucLen);
    return message.m_ucLen;
}

int ndk_getPOSPsn(char *pinfo)
{
    TField message;
    int ret;
//  if (mount("/dev/mtdblock3", "/mnt/hwinfo", "yaffs2", 0, NULL) != 0)
//      return -1;
    ret = snReadPSN(&message);
//  if(umount("/mnt/hwinfo")!=0)
//      return -2;
    if(ret<0)
        return ret;
    strncpy(pinfo,message.m_cData,message.m_ucLen);
    return message.m_ucLen;
}

#define MAG_IOC_MAGIC 'M'
#define MAG_IOCG_TOTAL_SWIPED   _IO(MAG_IOC_MAGIC, 4)

int ndk_getCreditCardCount(char *pinfo)
{
    char POSCardCount[16];
    int fd;
    int total_swiped = 0;

    fd = open("/dev/mag", O_RDONLY);
    if (fd > 0) {
        ioctl(fd, MAG_IOCG_TOTAL_SWIPED, &total_swiped);
        close(fd);
    }

    memset(POSCardCount, 0x00, sizeof(POSCardCount));
    sprintf(POSCardCount, "%d", total_swiped);
    strncpy(pinfo,POSCardCount,strlen(POSCardCount));

    return strlen(POSCardCount);
}

int ndk_getPOSRuntime(char *pinfo)
{
    char POSRuntime[64];
    struct sysinfo info;
    sysinfo(&info);
    memset(POSRuntime, 0x00, sizeof(POSRuntime));
    sprintf(POSRuntime, "%ld", info.uptime);
    strncpy(pinfo,POSRuntime,strlen(POSRuntime));

    return strlen(POSRuntime);

}

#define EVIOCGFF _IOC(_IOC_READ, 'E', 0x80, sizeof(struct ff_effect))
#define KEYPAD_IOCG_KEYCOUNT    _IO('K', 10)    /*SP60 获取按键总数接口*/

int ndk_getKeyCount(char *pinfo)
{
    int ret;
    char POSKeyCount[16];
    struct ff_effect effect;
    int fd;
    int total_key_pressed = 0;

    /*   sp60 bcm5892 cpu 走不同分支       */
    if(ndk_cpu_type() == 1) {
        fd = open("/dev/keypad",O_RDONLY);
        if(fd < 0) {
            fprintf(stderr," %s  %d  %d\n",__FUNCTION__,__LINE__,errno);
            return -1;
        } else {
            ret = ioctl(fd,KEYPAD_IOCG_KEYCOUNT,&total_key_pressed);
            if(ret < 0) {
                return -1;
            }
        }
    } else {
        fd = open("/dev/event0", O_RDONLY);
        if (fd > 0) {
            ioctl(fd, EVIOCGFF, &effect);
            total_key_pressed = effect.u.constant.level;
            close(fd);
        }
    }
    memset(POSKeyCount, 0x00, sizeof(POSKeyCount));
    sprintf(POSKeyCount,"%d",total_key_pressed);
    strncpy(pinfo,POSKeyCount,strlen(POSKeyCount));

    return strlen(POSKeyCount);

}

int ndk_getPrnLen(char *pinfo)
{
    char POSPrnLen[16];
    uint len;
    int ret;
    ret=NDK_PrnGetPrintedLen(&len);
    if(ret != NDK_OK)
        return -1;
    memset(POSPrnLen, 0x00, sizeof(POSPrnLen));
    sprintf(POSPrnLen,"%d",len);
    strncpy(pinfo,POSPrnLen,strlen(POSPrnLen));
    return strlen(POSPrnLen);

}
int ndk_getBoardVer(char *pinfo)
{
    if(get_dev_info(DEV_CLASS_MACHINE_TYPE, "hardware",pinfo)<0)
        return -1;
    return strlen(pinfo);
}
int ndk_getCpuType(char *pinfo)
{
    if(get_dev_info(DEV_CLASS_MACHINE_TYPE, "cputype",pinfo)<0)
        return -1;
    return strlen(pinfo);
}

int ndk_getPOSType(char *pinfo)
{
#if 0
    char str[16];
    int ret,exist;
    memset(str,0x00,sizeof(str));
    if(get_dev_info(DEV_CLASS_MACHINE_TYPE, "type",str)<0)
        return -1;
    if(strcmp(str,"")==0)
        return -1;
    strncpy(pinfo,str,sizeof(str));
    return strlen(pinfo);
#else
    return ndk_get_tmp_POSType(pinfo);
#endif
}



time_t ndk_read_rtc(int utc)
{
    int rtc=-1;
    struct tm tm;
    char *oldtz = 0;
    time_t t = 0;
    int ret;

    if (( rtc = open ( "/dev/rtc", O_RDONLY )) < 0 ) {
        if (( rtc = open ( "/dev/misc/rtc", O_RDONLY )) < 0 ) {
            goto do_exit;
        }
    }
    memset ( &tm, 0, sizeof( struct tm ));
    if ( (ret = ioctl ( rtc, RTC_RD_TIME, &tm )) < 0 )
        goto do_exit;
    tm. tm_isdst = -1;

    if ( utc ) {
        oldtz = getenv ( "TZ" );
        setenv ( "TZ", "UTC 0", 1 );
        tzset ( );
    }

    t = mktime ( &tm );

    if ( utc ) {
        if ( oldtz )
            setenv ( "TZ", oldtz, 1 );
        else
            unsetenv ( "TZ" );
        tzset ( );
    }

do_exit:
    if (rtc>=0) close ( rtc );

    return t;
}

int ndk_write_rtc(time_t t, int utc)
{
#ifdef PCI_CONFIG
    struct tm tm;

    tm = *( utc ? gmtime ( &t ) : localtime ( &t ));
    tm. tm_isdst = 0;
	return NDK_SecSetRtcTime(&tm);
#else
    int rtc=-1;
    struct tm tm;

    if (( rtc = open ( "/dev/rtc", O_WRONLY )) < 0 ) {
        if (( rtc = open ( "/dev/misc/rtc", O_WRONLY )) < 0 )
            goto on_exit;
    }

    tm = *( utc ? gmtime ( &t ) : localtime ( &t ));
    tm. tm_isdst = 0;

    if ( ioctl ( rtc, RTC_SET_TIME, &tm ) < 0 )
        goto on_exit;

on_exit:
    if (rtc>=0) close ( rtc );
	return 0;
#endif
}

/**
* @fn XORCalc
* @brief 利用连续异或求校验和
* @param in pucBuf 数据缓冲区
* @param in uiLen  数据缓冲区长度
* @return
* @li 异或和
 */
static int XORCalc(unsigned char *pucBuf, unsigned int uiLen)
{
    int i;
    unsigned int uiCheckSUM;

    uiCheckSUM = 0;
    for (i=0; i<uiLen; i+=4) {
        uiCheckSUM ^= (*(int *)(pucBuf + i));
    }

    return uiCheckSUM;
}


/**
* @fn snCheck
* @brief 序列号校验
* @param 无
* @return
* @li SUCC 校验成功
* @li FAIL 校验失败
 */
static int snCheck(void)
{
    unsigned int uiCheckSUM;
    int hFile;
    char cBuf[4096];

    if((hFile = open(SERIALNOFILE,O_RDWR))<0) {
        return -1;
    }
    read(hFile,cBuf,sizeof(cBuf));
    close(hFile);
    c_pTSNManageInfo = (NEWTSNManage *)cBuf;

    uiCheckSUM = XORCalc((unsigned char *)c_pTSNManageInfo+4, 4096-4);

    if (*(unsigned int *)((unsigned char *)c_pTSNManageInfo) == uiCheckSUM) { //校验值正确
        return 0;
    } else {
        return -2;
    }
}


static int snReadUSN(TField *pTFUSN)
{
#if 0
    NEWTSNManage readbuf;
    int hFile;
    char cBuf[4096];
    int ret;

    if((hFile = open(SERIALNOFILE,O_RDWR))<0) {
        return -1;
    }
    ret=read(hFile,cBuf,sizeof(cBuf));
    close(hFile);
    c_pTSNManageInfo = (NEWTSNManage *)cBuf;

    readbuf = *c_pTSNManageInfo;
    pTFUSN->m_ucLen = readbuf.m_flag.UserLen;
    if (pTFUSN->m_ucLen == 0xff) {
        return -2;
    } else if((pTFUSN->m_ucLen)>24) {
        return -3;
    }
    if(snCheck() != 0) {
        return -4;
    }
    memcpy((void *)pTFUSN->m_cData,(void *)readbuf.m_TFUser,pTFUSN->m_ucLen);
    pTFUSN->m_cData[pTFUSN->m_ucLen]=0;
    return 0;
#endif
    int hFile = 0;
    char szBuf[128] = {0};
    if((hFile = NDK_FsOpen(SNFILE, "r"))<0) {
        return SN_EMPTY;
    }
    NDK_FsRead(hFile, szBuf,sizeof(szBuf));
    NDK_FsClose(hFile);
    pTFUSN->m_ucLen = strlen(szBuf);
    if (pTFUSN->m_ucLen == 0) {
        return SN_EMPTY;
    } else if((pTFUSN->m_ucLen)>24) {
        return SN_EMPTY;
    }
    memcpy((void *)pTFUSN->m_cData, szBuf, pTFUSN->m_ucLen);
    pTFUSN->m_cData[pTFUSN->m_ucLen] = 0;

    return 0;
}

/**
* @fn snReadPSN
* @brief 读取本机产品机号
* @param in pTFPSN 产品机号缓冲区
* @return
* @li SUCC 读取成功
* @li FAIL 读取失败
 */
static int snReadPSN(TField *pTFPSN)
{
#if 0
    NEWTSNManage readbuf;
    int hFile;
    char cBuf[4096];

    if((hFile = open(SERIALNOFILE,O_RDWR))<0) {
        return -1;
    }
    read(hFile,cBuf,sizeof(cBuf));
    close(hFile);
    c_pTSNManageInfo = (NEWTSNManage *)cBuf;
    readbuf = *c_pTSNManageInfo;
    pTFPSN->m_ucLen = readbuf.m_flag.Producelen;
    if (pTFPSN->m_ucLen == 0xff) {
        return -2;
    } else if((pTFPSN->m_ucLen)>24) {
        return -3;
    }
    if (snCheck() != 0) {
        return -4;
    }
    memcpy((void *)pTFPSN->m_cData, (void *)readbuf.m_TFProduce, pTFPSN->m_ucLen);
    pTFPSN->m_cData[pTFPSN->m_ucLen]=0;
    return 0;
#endif
    int hFile = 0;
    char szBuf[128] = {0};

    if((hFile = NDK_FsOpen(PNFILE, "r"))<0) {
        return SN_EMPTY;
    }
    NDK_FsRead(hFile, szBuf,sizeof(szBuf));
    NDK_FsClose(hFile);
    pTFPSN->m_ucLen = strlen(szBuf);
    if (pTFPSN->m_ucLen == 0) {
        return SN_EMPTY;
    } else if((pTFPSN->m_ucLen)>24) {
        return SN_EMPTY;
    }
    memcpy((void *)pTFPSN->m_cData, szBuf, pTFPSN->m_ucLen);
    pTFPSN->m_cData[pTFPSN->m_ucLen] = 0;

    return 0;

}

int ndk_cpu_type(void)
{
    char recbuf[100];
    if( ndk_cpu_state < 0) {
        memset(recbuf,0x00,sizeof(recbuf));
        if(ndk_getCpuType(recbuf) < 0) {
            //fprintf(stderr,"\n\nndk_get cpu type err\n\n");
            ndk_cpu_state = 0;
            return -1;
        } else {
            //fprintf(stderr,"recbuf:%s\n",recbuf);
            if(strcmp(recbuf,"bcm589x") == 0) {
                ndk_cpu_state = 1;
                return 1;
            } else {
                ndk_cpu_state = 0;
                return 0;
            }
        }
    } else
        return ndk_cpu_state;
}



/* End of this file */


