#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sem.h>
#include <unistd.h>

#include "NDK.h"
#include "wlan.h"
#include "NDK_Net.h"

extern int wifi_methodcall_status(int method_id, int * ret);
extern int wifi_methodcall(int method_id, void * wlm_arg, int wlm_arg_len, void * wlm_out, int * ret);

int if_pid_exist(char *p_pidname)
{
    FILE *fd;
    char szLineBuf[1024], cmd[256];

    if(p_pidname==NULL)
        return -1;

    sprintf(cmd, "ps|grep %s", p_pidname);
    if ((fd=popen(cmd, "r")) != NULL) {
        memset(szLineBuf, 0X00, sizeof(szLineBuf));
        while (fgets(szLineBuf, sizeof(szLineBuf)-1, fd) != NULL) {
            //能找到输入的进程名称，但又不是由grep命令获取到的。
            if (strstr(szLineBuf, p_pidname) && !strstr(szLineBuf, "grep")) {
                pclose(fd);
                return 1;
            }
            memset(szLineBuf, 0X00, sizeof(szLineBuf));
        }
        pclose(fd);
    }

    return 0;
}


int EnableWiFiService()
{
    int ret=0;
    if (!if_pid_exist("wpa_supplicant")) {
        comm_methodcall_general(FUNC_WIFI_ENABLE,0,&ret);
        if(ret!=0) {
            return NDK_ERR_WIFI_DEVICE_UNAVAILABLE;
        }
    }
    return ret;
}

int GetWiFiOpenStatus()
{
    int iStatus=0;
    comm_methodcall_general(FUNC_WIFI_INIT_STATUS_GET,0,&iStatus);
    return iStatus;
}

int if_wlan_ready()
{
    if(GetWiFiOpenStatus()!=0)
        return NDK_ERR_WIFI_DEVICE_UNAVAILABLE;
    if(if_pid_exist("wpa_supplicant")!=1)
        return NDK_ERR_WIFI_DEVICE_UNAVAILABLE;
    return NDK_OK;
}
int GetWiFiScanList(ST_WIFI_APINFO ** ap_scan_info)
{
    int nRet=0;
    int n=0;
    char szTemp[128];
    char szApInfo[1024*50];
    memset(szApInfo,0,sizeof(szApInfo));
    wifi_methodcall_info(FUNC_WIFI_INFO_SCAN_RESULTS,szApInfo,&nRet);
    if(nRet==1){//如果只扫到1个AP，重新扫描一次
        ndk_msdelay(3000);
        wifi_methodcall_info(FUNC_WIFI_INFO_SCAN_RESULTS,szApInfo,&nRet);
    }
    while(nRet==0&&n<15) {
        memset(szApInfo,0,sizeof(szApInfo));
        wifi_methodcall_info(FUNC_WIFI_INFO_SCAN_RESULTS,szApInfo,&nRet);
        n++;
        ndk_msdelay(3000);
    }
    if(nRet>0)
        memcpy(ap_scan_info,szApInfo,nRet*sizeof(ST_WIFI_APINFO));
    return nRet;
}

int GetWiFiMac(char *szWiFiMac)
{
    int nRet;
    char szTemp[128];
    wifi_methodcall_info(FUNC_WIFI_INFO_MAC,szWiFiMac,&nRet);

    return nRet;
}
int GetWiFiStatus(int *piStatus)
{

    comm_methodcall_general(FUNC_WIFI_CONNECT_STATUS_GET, 0,piStatus);
    return 0;
}

int Disable_WiFi(void)
{
    int nRet=-1;
    comm_methodcall_general(FUNC_WIFI_DISABLE,0,&nRet);
    return nRet;
}

int Disconnect_WiFi(void)
{
    int nRet=-1;
    comm_methodcall_general(FUNC_WIFI_DISCONNECT,0,&nRet);
    return nRet;
}

