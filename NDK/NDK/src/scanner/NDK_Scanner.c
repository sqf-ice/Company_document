#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include "NDK.h"
#include "NDK_debug.h"
#include "NDK_Scanner.h"

#define ARR_SIZE(a)             (sizeof((a))/sizeof((a[0])))
int scanner_fd;

int ScannerType = SCAN_TYPE_EM3000;



extern int NDK_ScanGetType(int *pnType);
typedef struct {
    int  suspendflag;
    char *cmd;
} st_scan_cmd;
st_scan_cmd *st_cmd_floodlight;
static  st_scan_cmd EM3000_st_floodlight_cmd[]= {
    {1,"nls0200000"},//闪烁
    {1,"nls0200020"},//无照明
    {1,"nls0200030"},//读码常亮
    {1,NULL},
};
static  st_scan_cmd EM1300_st_floodlight_cmd[]= {
    {0,NULL},//闪烁
    {0,NULL},//无照明
    {0,NULL},//读码常亮
    {0,NULL},
};

st_scan_cmd *st_cmd_focuslight;
static  st_scan_cmd EM3000_st_focuslight_cmd[]= {
    {1,"nls0201000"},//闪烁
    {1,"nls0201020"},//无照明
    {1,"nls0201030"},//读码常亮
    {1,NULL},
};
static  st_scan_cmd EM1300_st_focuslight_cmd[]= {
    {0,NULL},//闪烁
    {0,NULL},//无照明
    {0,NULL},//读码常亮
    {0,NULL},
};
st_scan_cmd *st_cmd_factorydefault;
static st_scan_cmd EM3000_st_factorydefault_cmd[] = {
    {1,"nls0001000"},
    {1,NULL},
};
static st_scan_cmd EM1300_st_factorydefault_cmd[] = {
    {1,"nls99900030"},
    {1,NULL},
};

st_scan_cmd *st_cmd_SENSITIVITY;
static st_scan_cmd EM3000_st_SENSITIVITY_cmd[] = {
    {1,"nls0312040"},
    {1,NULL},
};
static st_scan_cmd EM1300_st_SENSITIVITY_cmd[] = {
    {1,"nls99900161"},
    {1,NULL},
};

static int get_scan_type()
{
    char hardwareinfo[16]  = {0};
    int ret = -1;
    uint len;
    ret= NDK_SysGetPosInfo(SYS_HWINFO_GET_HARDWARE_INFO,&len,hardwareinfo);
    fprintf(stderr,"-----------[%s][%d][%02x]\n",__func__,__LINE__,hardwareinfo[3]);
    if(ret!=NDK_OK)
        return -1;
    if(hardwareinfo[3]==0x01)
        ScannerType=SCAN_TYPE_EM1300;
    else if(hardwareinfo[3]==0x02)
        ScannerType=SCAN_TYPE_EM3000;
    else if(hardwareinfo[3]==0x03)
        ScannerType=SCAN_TYPE_SE955;
    else if(hardwareinfo[3]==0x04)
        ScannerType=SCAN_TYPE_UE966;
    else if(hardwareinfo[3]==0x05)
        ScannerType=SCAN_TYPE_EM3095;
    else if(hardwareinfo[3]==0x06)
        ScannerType=SCAN_TYPE_EM1365;
    else
        return -1;
    return 0;

}
static int ScanSendCmd(int cmdlen,char *cmd)
{
    int ret,rlen = 0;
    char ack;
    if(cmd == NULL)
        return -1;

    if(NDK_PortOpen(PORT_NUM_SCAN,"9600,8,N,1")!=NDK_OK)
        return -1;
    NDK_PortClrBuf(PORT_NUM_SCAN);
    ret=NDK_PortWrite(PORT_NUM_SCAN,cmdlen,cmd);
    if(ret!=NDK_OK)
        return -1;
    ret=NDK_PortRead(PORT_NUM_SCAN,1,&ack,1000,&rlen);
    fprintf(stderr,"----[%s][%d][%02x]\n",__func__,__LINE__,ack);
    if(ret!=NDK_OK)
        return -1;
    if(rlen==1&&ack==0x06)
        return 0;
    else
        return -1;
}

int NDK_ScanInit(void)
{
    scanner_fd=open("/dev/scanner",O_RDONLY);
    if(scanner_fd<0)
        return NDK_ERR_OPEN_DEV;

    if(get_scan_type()!=0)
        return NDK_ERR;
    ioctl(scanner_fd,SCAN_IOCS_INIT);
    NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT,0);

    return NDK_OK;
}
int NDK_ScanSet(EM_SCAN_SETTYPE emScanSet,uint unSetValue)
{
    int arrsize = 0;
    char tmpcmd[32] = {0};
    if((ScannerType==SCAN_TYPE_SE955) || (ScannerType==SCAN_TYPE_UE966)||(ScannerType==SCAN_TYPE_EM3095)||(ScannerType==SCAN_TYPE_EM1365)) {
        return NDK_ERR_NOT_SUPPORT;
    }
    switch(emScanSet) {
        case SCAN_SETTYPE_FLOODLIGHT:
            if(ScannerType==SCAN_TYPE_EM3000)
                st_cmd_floodlight= EM3000_st_floodlight_cmd;
            else
                st_cmd_floodlight=EM1300_st_floodlight_cmd;
            arrsize=ARR_SIZE(st_cmd_floodlight);
            if(unSetValue<0||unSetValue>arrsize) {
                fprintf(stderr,"---------[%s][%d][%d]\n",__func__,__LINE__,arrsize);
                return NDK_ERR_PARA;
            }
            if(st_cmd_floodlight[unSetValue].suspendflag&&st_cmd_floodlight[unSetValue].cmd!=NULL) {
                if(ScanSendCmd(strlen(st_cmd_floodlight[unSetValue].cmd),st_cmd_floodlight[unSetValue].cmd)!=0)
                    return NDK_ERR;
            } else
                return NDK_ERR_NOT_SUPPORT;
            break;
        case SCAN_SETTYPE_FOCUSLIGHT:
            if(ScannerType==SCAN_TYPE_EM3000)
                st_cmd_focuslight= EM3000_st_focuslight_cmd;
            else
                st_cmd_focuslight=EM1300_st_focuslight_cmd;
            arrsize=ARR_SIZE(st_cmd_focuslight);
            if(unSetValue<0||unSetValue>arrsize) {
                fprintf(stderr,"---------[%s][%d][%d]\n",__func__,__LINE__,arrsize);
                return NDK_ERR_PARA;
            }
            if(st_cmd_focuslight[unSetValue].suspendflag&&st_cmd_focuslight[unSetValue].cmd!=NULL) {
                if(ScanSendCmd(strlen(st_cmd_focuslight[unSetValue].cmd),st_cmd_focuslight[unSetValue].cmd)!=0)
                    return NDK_ERR;
            } else
                return NDK_ERR_NOT_SUPPORT;
            break;
        case SCAN_SETTYPE_SENSITIVITY:
            if(ScannerType==SCAN_TYPE_EM3000)
                st_cmd_SENSITIVITY=EM3000_st_SENSITIVITY_cmd;
            else
                st_cmd_SENSITIVITY=EM1300_st_SENSITIVITY_cmd;
            if(unSetValue<0||unSetValue>20) {
                fprintf(stderr,"---------[%s][%d][%d]\n",__func__,__LINE__,arrsize);
                return NDK_ERR_PARA;
            }
            if(st_cmd_SENSITIVITY[0].suspendflag&&st_cmd_SENSITIVITY[0].cmd!=NULL) {
                sprintf(tmpcmd,"%s=%d",st_cmd_SENSITIVITY[0].cmd,unSetValue);
                if(ScanSendCmd(strlen(tmpcmd),tmpcmd)!=0)
                    return NDK_ERR;
            } else
                return NDK_ERR_NOT_SUPPORT;
            break;
        case SCAN_SETTYPE_FACTORYDEFAULT:
            if(ScannerType==SCAN_TYPE_EM3000)
                st_cmd_factorydefault=  EM3000_st_factorydefault_cmd;
            else
                st_cmd_factorydefault=  EM1300_st_factorydefault_cmd;
            if(st_cmd_factorydefault[0].suspendflag&&st_cmd_factorydefault[0].cmd!=NULL) {
                if(ScanSendCmd(strlen(st_cmd_factorydefault[0].cmd),st_cmd_factorydefault[0].cmd)!=0)
                    return NDK_ERR;
            } else
                return NDK_ERR_NOT_SUPPORT;
            break;
        default:
            return NDK_ERR_NOT_SUPPORT;
    }
    return NDK_OK;

}

int NDK_ScanDoScan(int nTimeOut,char *pszValue,int *pnLen)
{
    char buf[1024] = {0},*read;
    int ret = NDK_OK,buflen = 0,len = 0,flag = 0,totallen = 0;
    int tmptime;
    time_t oldtime,changetime;
    if(pszValue==NULL||pnLen==NULL||nTimeOut<0)
        return NDK_ERR_PARA;
    if(NDK_PortOpen(PORT_NUM_SCAN,"9600,8,N,1")!=NDK_OK)
        return NDK_ERR;
    if(scanner_fd>0)
        ioctl(scanner_fd,SCAN_IOCS_TRIG);
    read=buf;
    oldtime=time(NULL);
    while(1) {
        changetime=time(NULL)-oldtime;
        if(changetime>nTimeOut) {
            ret=NDK_ERR_TIMEOUT;
            break;
        }
        totallen+=len;
        if(totallen>sizeof(buf)) {
            ret=NDK_ERR_OVERFLOW;
            break;
        }
        read+=len;
        ndk_msdelay(100);
        NDK_PortReadLen(PORT_NUM_SCAN,&buflen);
        if(buflen) {
            tmptime=(buflen*8*1000)/9600;
            flag=1;
            NDK_PortRead(PORT_NUM_SCAN,buflen,read,tmptime,&len);
        }
        if(buflen<=0&&flag==1)
            break;
    }

    if(totallen>0) {
        NDK_SysBeep();
        memcpy(pszValue,buf,totallen);
        *pnLen=totallen;
        ret = NDK_OK;
    }
    if(scanner_fd>0)
        ioctl(scanner_fd,SCAN_IOCS_EXIT);
    NDK_PortClrBuf(PORT_NUM_SCAN);
    return ret;
}
/*
    扫描头识别：
    SCANNER_EM3000识别：发送字符"?"，收到应答为"!"，则表示为EM3000扫描头；
    SCANNER_EM1300识别：发送字符串"$$$$%%%%",收到应答为"@@@@^^^^"
    SCANNER_SE955，需要参考手册，需要先通过RTS/CTS管脚控制时序，时序正确之后，才能接收命令，然后通过串口收发命令
    由于目前只支持以上3种扫描头，若不是前两种，则都按照SE955来识别；
*/
int NDK_ScanGetType(int *pnType)
{
    char tmpbuf[16] = {0};
    char readdata[1024] = {0};
    int ret,rlen = 0;


    if(NDK_PortOpen(PORT_NUM_COM2,"9600,8,N,1")!=NDK_OK)
        return NDK_ERR;
    NDK_PortWrite(PORT_NUM_COM2,8,"$$$$%%%%%%%%");
    memset(tmpbuf,0,sizeof(tmpbuf));
    ret= NDK_PortWrite(PORT_NUM_COM2,8,"$$$$%%%%%%%%");
    if(ret!=NDK_OK) {
        fprintf(stderr,"-----------[%s][%d]\n",__func__,__LINE__);
        return NDK_ERR;
    }
    ret= NDK_PortRead(PORT_NUM_COM2,8,tmpbuf,3000,&rlen);
    if(ret==NDK_OK) {
        if(rlen == 8 && strcmp(tmpbuf,"@@@@^^^^") == 0) {
            * pnType=  SCAN_TYPE_EM1300;
            goto exit_type;
        }
    }

    memset(tmpbuf,0,sizeof(tmpbuf));
    ret= NDK_PortWrite(PORT_NUM_COM2,4,"????");
    if(ret!=NDK_OK) {
        fprintf(stderr,"-----------[%s][%d]\n",__func__,__LINE__);
        return NDK_ERR;
    }
    ret= NDK_PortRead(PORT_NUM_COM2,4,tmpbuf,3000,&rlen);
    if(ret==NDK_OK) {
        if(rlen == 4 && strcmp(tmpbuf,"!!!!") == 0) {
            * pnType = SCAN_TYPE_EM3000;
            goto exit_type;
        }
    }

    memset(tmpbuf,0,sizeof(tmpbuf));
    ret= NDK_PortWrite(PORT_NUM_COM2,6,"\x04\xA3\x04\x00\xFF\x55");
    if(ret!=NDK_OK) {
        fprintf(stderr,"-----------[%s][%d]\n",__func__,__LINE__);
        return NDK_ERR;
    }
    ret= NDK_PortRead(PORT_NUM_COM2,sizeof(readdata),readdata,3000,&rlen);
    if(ret==NDK_OK) {
        if(strstr(readdata+7, "uE966") != NULL)
            * pnType = SCAN_TYPE_UE966;
        else
            * pnType = SCAN_TYPE_SE955;
    }
    ScannerType=  * pnType;
exit_type:
    NDK_PortClrBuf(PORT_NUM_COM2);
    return NDK_OK;
}

