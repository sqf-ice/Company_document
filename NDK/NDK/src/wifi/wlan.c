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

extern int EnableWiFiService();
extern int GetWiFiOpenStatus();
extern int wifi_methodcall_disable(int * ret);
extern int GetWiFiScanList(ST_WIFI_APINFO ** ap_scan_info);
extern int wifi_methodcall_connect(int method_id, void * wlm_arg, int wlm_arg_len, int * ret);
extern int GetWiFiMac(char * szWiFiMac);
extern int GetWiFiStatus(int * piStatus);
extern int wlan_disable(void);
extern int ndk_Get_DNS_IP(char * DNSBuf);

static ST_WIFI_APINFO currentAPInfo[50];
static int current_ap_cnt = -1;

int NDK_WiFiInit(void)
{
    int ret = -1;

    if((ret=EnableWiFiService())<0) {
        return ret;
    }

    if((ret=if_wlan_ready())!=NDK_OK) {
        PDEBUG("device unavailable\n");
        return ret;
    }
    return NDK_OK;
}

int NDK_WiFiGetNetList(char **ppszESSIDlist, int *pnNumList)
{
    int ret, i;
    //ST_WIFI_APINFO currentAPInfo[50];

    if ((ppszESSIDlist==NULL) || (pnNumList==NULL)) {
        PDEBUG("%s:%d pstParam invalid\n", __func__, __LINE__);
        return NDK_ERR_WIFI_INVDATA;
    }
    if((ret=if_wlan_ready())!=NDK_OK) {
        PDEBUG("device unavailable\n");
        return ret;
    }
    memset(currentAPInfo,0,sizeof(currentAPInfo));
    ret=GetWiFiScanList(&currentAPInfo);
    if (ret > 0) {
        PDEBUG("%s:%d find %d AP\n", __func__, __LINE__, ret);
        *pnNumList = ret;
        current_ap_cnt = ret;
        for (i=0; i<ret; i++) {
            sprintf(*(ppszESSIDlist+i), "%s", currentAPInfo[i].sEssid);
            PDEBUG("%s:%d AP[%d] essid=%s\n", __func__, __LINE__, i, *(ppszESSIDlist+i));
        }
    } else {
        if (0 == ret) {
            PDEBUG("%s:%d no AP is scaned\n", __func__, __LINE__);
            *pnNumList = 0;
            return NDK_ERR_WIFI_DEVICE_NOTOPEN;
        } else {
            PDEBUG("%s:%d is failed=%d\n", __func__, __LINE__, ret);
            return NDK_ERR;
        }
    }

    return NDK_OK;
}

int NDK_WiFiSignalCover(const char *pszNetName, int *pnSignal)
{
    int ret, i;

    if ((pszNetName==NULL) || (pnSignal==NULL)) {
        PDEBUG("%s:%d pstParam invalid\n", __func__, __LINE__);
        return NDK_ERR_WIFI_INVDATA;
    }
    if((ret=if_wlan_ready())!=NDK_OK) {
        PDEBUG("device unavailable\n");
        return ret;
    }
    //ST_WIFI_APINFO currentAPInfo[50];
    //ret=GetWiFiScanList(&currentAPInfo);
    //PDEBUG("GetWiFiScanList ret =%d\n",ret);

    if (current_ap_cnt  > 0) {
        PDEBUG("%s:%d find %d AP\n", __func__, __LINE__, current_ap_cnt);
        for (i=0; i<current_ap_cnt; i++) {
            if (0 == strcmp(currentAPInfo[i].sEssid, pszNetName)) {
                *pnSignal = atoi(currentAPInfo[i].sSignal);
                PDEBUG("%s:%d find AP[%d] essid=%s pnSignal=%d\n", __func__, __LINE__, i, pszNetName, *pnSignal);
                break;
            }
        }
        if (i == current_ap_cnt) {
            PDEBUG("%s:%d not find AP essid=%s\n", __func__, __LINE__, pszNetName);
            return NDK_ERR_WIFI_DEVICE_NOTOPEN;
        }
    } else {
        if (0 == current_ap_cnt) {
            PDEBUG("%s:%d no AP is scaned\n", __func__, __LINE__);
            return NDK_ERR_WIFI_DEVICE_NOTOPEN;
        } else {
            PDEBUG("%s:%d is failed=%d\n", __func__, __LINE__, current_ap_cnt);
            return NDK_ERR;
        }
    }

    return NDK_OK;
}

int NDK_WiFiGetSec(const char *pszESSIDName, EM_WIFI_NET_SEC *pstSec)
{
    int ret, i;
    //ST_WIFI_APINFO currentAPInfo[50];

    if ((pszESSIDName==NULL) || (pstSec==NULL)) {
        PDEBUG("%s:%d pstParam invalid\n", __func__, __LINE__);
        return NDK_ERR_WIFI_INVDATA;
    }
    if((ret=if_wlan_ready())!=NDK_OK) {
        PDEBUG("device unavailable\n");
        return ret;
    }
    //ret=GetWiFiScanList(&currentAPInfo);

    if (current_ap_cnt > 0) {
        PDEBUG("%s:%d find %d AP\n", __func__, __LINE__, current_ap_cnt);
        for (i=0; i<current_ap_cnt; i++) {
            if (0 == strcmp(currentAPInfo[i].sEssid, pszESSIDName)) {
                if (strstr(currentAPInfo[i].sKeyModeStr, "WPA2") != NULL ) {
                    PDEBUG("%s:%d find AP[%d] essid=%s secmode is WIFI_NET_SEC_WPA2\n", __func__, __LINE__, i, pszESSIDName);
                    *pstSec = WIFI_NET_SEC_WPA2;
                    break;
                } else {
                    if (strstr(currentAPInfo[i].sKeyModeStr, "WPA") != NULL ) {
                        PDEBUG("%s:%d find AP[%d] essid=%s secmode is WIFI_NET_SEC_WPA\n", __func__, __LINE__, i, pszESSIDName);
                        *pstSec = WIFI_NET_SEC_WPA;
                        break;
                    } else {
                        if (strstr(currentAPInfo[i].sKeyModeStr, "WEP") != NULL ) {
                            PDEBUG("%s:%d find AP[%d] essid=%s secmode is WEP\n", __func__, __LINE__, i, pszESSIDName);
                            *pstSec = WIFI_NET_SEC_WEP_OPEN;
                            break;
                        } else {
                            PDEBUG("%s:%d find AP[%d] essid=%s secmode is WIFI_NET_SEC_NONE\n", __func__, __LINE__, i, pszESSIDName);
                            *pstSec = WIFI_NET_SEC_NONE;
                            break;
                        }
                    }
                }
            }
        }
        if (i == current_ap_cnt) {
            PDEBUG("%s:%d not find AP essid=%s\n", __func__, __LINE__, pszESSIDName);
            return NDK_ERR_WIFI_DEVICE_NOTOPEN;
        }
    } else {
        if (0 == current_ap_cnt) {
            PDEBUG("%s:%d no AP is scaned\n", __func__, __LINE__);
            return NDK_ERR_WIFI_DEVICE_NOTOPEN;
        } else {
            PDEBUG("%s:%d is failed=%d\n", __func__, __LINE__, current_ap_cnt);
            return NDK_ERR;
        }
    }

    return NDK_OK;

}

int NDK_WiFiConnect(const char *pszESSIDName, const ST_WIFI_PARAM *pstParam)
{
    int ret;
    SendWifiConnectCmd();
    if (pszESSIDName == NULL || (pstParam==NULL) || (pstParam->emSecMode<0) || (pstParam->emSecMode>4)) {
        PDEBUG("%s:%d pstParam invalid\n", __func__, __LINE__);
        return NDK_ERR_WIFI_INVDATA;
    }
    if((ret=if_wlan_ready())!=NDK_OK) {
        PDEBUG("device unavailable\n");
        return ret;
    }
    T_WiFiConnectParam tempConnParam;
    memset(&tempConnParam,0,sizeof(tempConnParam));
    strcpy(tempConnParam.asEssid,pszESSIDName);
    tempConnParam.if_DHCP=pstParam->ucIfDHCP;
    tempConnParam.encrypt_type=pstParam->emSecMode;
    tempConnParam.key_type=pstParam->emKeyType;
    if(pstParam->emSecMode!=0) { //考虑对于不加密模式
        if(pstParam->pszKey==NULL)
            return NDK_ERR_WIFI_INVDATA;
        strcpy(tempConnParam.asKey,pstParam->pszKey);
    }
    if(!pstParam->ucIfDHCP) {
        if(pstParam->psEthIp==NULL||pstParam->psEthNetmask==NULL||pstParam->psEthGateway==NULL||pstParam->psEthDnsPrimary==NULL||pstParam->psEthDnsSecondary==NULL)
            return NDK_ERR_WIFI_INVDATA;
        strcpy(tempConnParam.wifi_connect_ip.wifi_ip,pstParam->psEthIp);
        strcpy(tempConnParam.wifi_connect_ip.wifi_netmask,pstParam->psEthNetmask);
        strcpy(tempConnParam.wifi_connect_ip.wifi_gateway,pstParam->psEthGateway);
        strcpy(tempConnParam.wifi_connect_ip.wifi_dns_primary,pstParam->psEthDnsPrimary);
        strcpy(tempConnParam.wifi_connect_ip.wifi_dns_secondary,pstParam->psEthDnsSecondary);
    }
    wifi_methodcall_connect(0, &tempConnParam, sizeof(tempConnParam),&ret);
    return NDK_OK;
}

int NDK_WiFiConnectState(EM_WPA_CONSTATE *pemState)
{
    int ret=-1;
    if (pemState == NULL) {
        PDEBUG("%s:%d pstParam invalid\n", __func__, __LINE__);
        return NDK_ERR_WIFI_INVDATA;
    }
    if((ret=if_wlan_ready())!=NDK_OK) {
        PDEBUG("device unavailable\n");
        return ret;
    }
    GetWiFiStatus(&ret);
    if(ret<0) {
        PDEBUG("[%s] error ret=%d\n",__func__,ret);
        return NDK_ERR;
    }
    PDEBUG("[%s] ret=%d\n",__func__,ret);
    if(ret==0) {
        *pemState=-1;//未连接成功
        //return NDK_ERR_WIFI_PROCESS_INBADSTATE;
    } else if(ret==1) {
        *pemState=WIFI_WPA_CONSTATE_LINKING;
    } else if(ret==2) {
        *pemState=WIFI_WPA_CONSTATE_CONTEXT;
        SendWifiSuccCmd();
    }

    return NDK_OK;
}

int NDK_WiFiIsConnected(void)
{
    EM_WPA_CONSTATE con_state;

    if (NDK_WiFiConnectState(&con_state)!=NDK_OK) {
        return NDK_ERR_WIFI_DEVICE_UNAVAILABLE;
    }

    if (con_state!=WIFI_WPA_CONSTATE_CONTEXT) {
        PDEBUG("%s:%d is not connected with AP yet\n", __func__, __LINE__);
        return NDK_ERR_WIFI_NON_CONNECTED;
    }
    return NDK_OK;
}

int NDK_WiFiDisconnect(void)
{
    int ret;

    if((ret=if_wlan_ready())!=NDK_OK) {
        PDEBUG("device unavailable\n");
        return ret;
    }

    return Disconnect_WiFi();   
}


int NDK_WiFiShutdown(void)
{
    SendWifiShutdownCmd();
    return Disable_WiFi();
}

int NDK_WiFiGetNetInfo(ST_WIFI_APINFO *pstList, unsigned int nMaxNum, int *pnNumList)
{

    int ret;

    if ((pstList==NULL) || (pnNumList==NULL)) {
        PDEBUG("%s:%d pstParam invalid\n", __func__, __LINE__);
        return NDK_ERR_WIFI_INVDATA;
    }

    if ((nMaxNum==0) || (nMaxNum>AP_NUM_MAX)) {
        PDEBUG("%s:%d pstParam invalid\n", __func__, __LINE__);
        return NDK_ERR_WIFI_INVDATA;
    }

    if((ret=if_wlan_ready())!=NDK_OK) {
        PDEBUG("device unavailable\n");
        return ret;
    }
    //ST_WIFI_APINFO currentAPInfo[50];
    memset(currentAPInfo,0,sizeof(currentAPInfo));
    ret=GetWiFiScanList(&currentAPInfo);
    if (ret > 0) {
        PDEBUG("%s:%d find %d AP\n", __func__, __LINE__, ret);
        current_ap_cnt=ret;
        if (ret > nMaxNum)
            ret = nMaxNum;
      	*pnNumList = ret;
        memcpy(pstList,currentAPInfo,ret*sizeof(ST_WIFI_APINFO));
    } else {
        if (0 == ret) {
            PDEBUG("%s:%d no AP is scaned\n", __func__, __LINE__);
            *pnNumList = 0;
            return NDK_ERR_WIFI_DEVICE_NOTOPEN;
        } else {
            PDEBUG("%s:%d is failed=%d\n", __func__, __LINE__, ret);
            return NDK_ERR;
        }
    }

    return NDK_OK;
}

int NDK_WiFiGetMac(char *pszMac)
{
    int ret;
    if((ret=if_wlan_ready())!=NDK_OK) {
        PDEBUG("device unavailable\n");
        return ret;
    }
    if (NULL == pszMac)
        return NDK_ERR_WIFI_INVDATA;

    ret=GetWiFiMac(pszMac);
    if(ret<0) {
        PDEBUG("GetWiFiMac error\n");
        return NDK_ERR;
    }

    return NDK_OK;
}

int NDK_WiFiSleep(void)
{
    return -1;
}

int NDK_WiFiWakeUp(void)
{
    return -1;
}

