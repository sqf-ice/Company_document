#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include "wlan_ss.h"

int wifi_fd = -1;
/******************************************************************
函数名： int get_wifi_fd(usngined int *keycount)
输入：void
输出：int   --wifi_ss的fd
功能描述：取得wifi_ss的fd
作者：tuzg
日期：2013815
版　本：1.00
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
函数名： int SendWifiConnectCmd(void)
输入：void
输出：int   --ioctl结果
功能描述：向驱动发送Wifi connect命令
作者：tuzg
日期：20130815
版　本：1.00
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
函数名： int SendWifiShutdownCmd(void)
输入：void
输出：int   --ioctl结果
功能描述：向驱动发送Wifi挂断命令
作者：tuzg
日期：20130815
版　本：1.00
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
函数名： int SendWifiSuccCmd(void)
输入：void
输出：int   --ioctl结果
功能描述：向驱动发送Wifi连接成功命令
作者：tuzg
日期：20130815
版　本：1.00
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


