#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include "wlan_ss.h"

int wifi_fd = -1;
/******************************************************************
�������� int get_wifi_fd(usngined int *keycount)
���룺void
�����int   --wifi_ss��fd
����������ȡ��wifi_ss��fd
���ߣ�tuzg
���ڣ�2013815
�桡����1.00
******************************************************************/

int get_wifi_fd(void)
{
    if(wifi_fd == -1) {
        wifi_fd = open(WLAN_DEV_NAME,O_RDONLY);
        if(wifi_fd < 0) {
            return -1;
        }
    }
    return wifi_fd ;
}

/******************************************************************
�������� int SendWifiConnectCmd(void)
���룺void
�����int   --ioctl���
��������������������Wifi connect����
���ߣ�tuzg
���ڣ�20130815
�桡����1.00
******************************************************************/

int SendWifiConnectCmd(void)
{
    int fd = -1;
    int ret = 0;
    fd = get_wifi_fd();
    if(fd < 0) {
        return -1;
    }
    ret = ioctl(fd,WIFI_SS_IOCS_DAIL_CMD);
    if(ret < 0) {
        return -1;
    }
    return 0;

}
/******************************************************************
�������� int SendWifiShutdownCmd(void)
���룺void
�����int   --ioctl���
��������������������Wifi�Ҷ�����
���ߣ�tuzg
���ڣ�20130815
�桡����1.00
******************************************************************/

int SendWifiShutdownCmd(void)
{
    int fd = -1;
    int ret = 0;
    fd = get_wifi_fd();
    if(fd < 0) {
        return -1;
    }
    ret = ioctl(fd,WIFI_SS_IOCS_HANGUP_CMD);
    if(ret < 0) {
        return -1;
    }
    return 0;

}
/******************************************************************
�������� int SendWifiSuccCmd(void)
���룺void
�����int   --ioctl���
��������������������Wifi���ӳɹ�����
���ߣ�tuzg
���ڣ�20130815
�桡����1.00
******************************************************************/

int SendWifiSuccCmd(void)
{
    int fd = -1;
    int ret = 0;
    fd = get_wifi_fd();
    if(fd < 0) {
        return -1;
    }
    //fprintf(stderr,"<<<<<<<<<<<<<<<<[%s][%d]\n",__func__,__LINE__);
    ret = ioctl(fd,WIFI_SS_IOCS_SUCC_CMD);
    if(ret < 0) {
        return -1;
    }
    //fprintf(stderr,"<<<<<<<<<<<<<<<<[%s][%d]\n",__func__,__LINE__);
    return 0;

}


