/*
 * �´�½��˾ ��Ȩ����(c) 2011-2015
 *
 * modemģ��-�豸����
 * ��    �ߣ�    ��Ʒ������
 * ��    �ڣ�    2013-12-10
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
#include <assert.h>
#include <time.h>
#include <termios.h>
#include <linux/input.h>
#include <signal.h>

#include "NDK.h"
#include "devmgr.h"
#include "modem.h"
#include "mdm_drv.h"
#include "mdm_debug.h"
#include "mdm_comm.h"

extern EM_MODEM_DIAL_TYPE g_ModemCommType;    /**<0:��ʼ��̬��1��ͬ����2���첽*/

void mdm_msdelay(uint msTime)
{
    sigset_t newmask;

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &newmask, NULL);
    usleep(msTime * 1000);
    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
    return;
}

long int mdm_get_time(void)
{
    int fd;
    long int jiffies = 0;

    fd = open("/dev/tick", O_RDONLY);
    if (fd > 0) {
        ioctl(fd, TICK_IOCG_JIFFIES, &jiffies); /*ȡ������jiffies��λֵΪ10ms*/
        close(fd);
    }
    return jiffies;
}

int mdm_get_datetime(char *pstDatetime)
{
    struct timeval tv;
    struct tm dateTime;

    if (pstDatetime == NULL)
        return MDM_ERR_PARA;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &dateTime);
    sprintf(pstDatetime, "%04d-%02d-%02d %02d:%02d:%02d.%03d", (dateTime.tm_year) + 1900, dateTime.tm_mon, dateTime.tm_mday
            , dateTime.tm_hour, dateTime.tm_min, dateTime.tm_sec, (int)tv.tv_usec / 1000);
    return MDM_OK;
}

int mdm_drv_exist(void)
{
    char pinfo[64];
    static int mdm_exist = -1;

    if (mdm_exist < 0) {
        if (get_dev_info("mdm", "exist", pinfo) < 0)
            return -1;
        if (strcmp(pinfo, "true") == 0)
            mdm_exist = 1;
        else if (strcmp(pinfo, "false") == 0)
            mdm_exist = 0;
    }
    return mdm_exist;
}

/**
 *@brief    ����MODEM��Ӳ����λ
 *@return
 *@li   MDM_OK              ��λ�ɹ�
 *@li   MDM_ERR_DEV_OPEN    ���豸�ļ�ʧ��
 *@li   MDM_ERR_DEV_IOCTL   ��������ʧ��
 */
int mdm_drv_reset(void)
{
    mdmprint("call %s\n", __func__);
    int fd;

    g_ModemCommType = MODEM_DIAL_TYPE_NOT;
    if ((fd = open(MDM_DEV, O_RDWR)) < 0)
        return MDM_ERR_DEV_OPEN;
    if (ioctl(fd, MDM_IOCS_RESET, NULL) < 0) {
        close(fd);
        return MDM_ERR_DEV_IOCTL;
    }
    close(fd);
    mdm_msdelay(500);
    mdm_port_close();
    return MDM_OK;
}

/**
 *@brief    ����MODEM������
 *@return
 *@li   MDM_OK              ��λ�ɹ�
 *@li   MDM_ERR_DEV_OPEN    ���豸�ļ�ʧ��
 *@li   MDM_ERR_DEV_IOCTL   ��������ʧ��
 */
int mdm_drv_sleep(void)
{
    mdmprint("call %s\n", __func__);
    int fd;

    g_ModemCommType = MODEM_DIAL_TYPE_NOT;
    if ((fd = open(MDM_DEV, O_RDWR)) < 0)
        return MDM_ERR_DEV_OPEN;
    if (ioctl(fd, MDM_IOCS_SLEEP, NULL) < 0) {
        close(fd);
        return MDM_ERR_DEV_IOCTL;
    }
    close(fd);
    mdm_port_close();
    return MDM_OK;
}

/**
 *@brief    ����MODEM��ȡ�汾
 *@retval   ver �汾�ַ���
 *@return
 *@li   MDM_OK              ��λ�ɹ�
 *@li   MDM_ERR_DEV_OPEN    ���豸�ļ�ʧ��
 *@li   MDM_ERR_DEV_IOCTL   ��������ʧ��
 */
int mdm_dev_get_version(char *ver)
{
    int fd;

    if ((fd = open(MDM_DEV, O_RDWR)) < 0)
        return MDM_ERR_DEV_OPEN;
    if (ioctl(fd, MDM_IOCG_VER, ver) < 0) {
        close(fd);
        return MDM_ERR_DEV_IOCTL;
    }
    close(fd);
    return MDM_OK;
}

/*modemͳ�Ʒ���*/
/**
 *@brief    ����MODEM��ʼͬ������
 *@return
 *@li   MDM_OK             �ɹ�
 *@li   MDM_ERR_DEV_OPEN    ���豸�ļ�ʧ��
 *@li   MDM_ERR_DEV_IOCTL   ��������ʧ��
 */
int mdm_drv_sdlcdial(void)
{
    int fd;

    if ((fd = open(MDM_DEV, O_RDWR)) < 0)
        return MDM_ERR_DEV_OPEN;
    if (ioctl(fd, MDM_IOCS_SDLCDAIL, NULL) < 0) {
        close(fd);
        return MDM_ERR_DEV_IOCTL;
    }
    close(fd);
    return MDM_OK;
}

/**
 *@brief    ����MODEM��ʼ�첽����
 *@return
 *@li   MDM_OK              �ɹ�
 *@li   MDM_ERR_DEV_OPEN    ���豸�ļ�ʧ��
 *@li   MDM_ERR_DEV_IOCTL   ��������ʧ��
 */
int mdm_drv_asyndial(void)
{
    int fd;

    if ((fd = open(MDM_DEV, O_RDWR)) < 0)
        return MDM_ERR_DEV_OPEN;
    if (ioctl(fd, MDM_IOCS_ASYNDAIL, NULL) < 0) {
        close(fd);
        return MDM_ERR_DEV_IOCTL;
    }
    close(fd);
    return MDM_OK;
}

/**
 *@brief    ����MODEM�Ҷϲ���
 *@return
 *@li   MDM_OK              �ɹ�
 *@li   MDM_ERR_DEV_OPEN    ���豸�ļ�ʧ��
 *@li   MDM_ERR_DEV_IOCTL   ��������ʧ��
 */
int mdm_drv_hungup(void)
{
    int fd;

    if ((fd = open(MDM_DEV, O_RDWR)) < 0)
        return MDM_ERR_DEV_OPEN;
    if (ioctl(fd, MDM_IOCS_HUNGUP, NULL) < 0) {
        close(fd);
        return MDM_ERR_DEV_IOCTL;
    }
    close(fd);
    return MDM_OK;
}

/**
 *@brief    ����MODEM����ʧ��
 *@return
 *@li   MDM_OK              �ɹ�
 *@li   MDM_ERR_DEV_OPEN    ���豸�ļ�ʧ��
 *@li   MDM_ERR_DEV_IOCTL   ��������ʧ��
 */
int mdm_drv_dialfail(void)
{
    int fd;

    if ((fd = open(MDM_DEV, O_RDWR)) < 0)
        return MDM_ERR_DEV_OPEN;
    if (ioctl(fd, MDM_IOCS_FAIL, NULL) < 0) {
        close(fd);
        return MDM_ERR_DEV_IOCTL;
    }
    close(fd);
    return MDM_OK;
}


/**
 *@brief    ����MODEM,DTR�Ҷϲ���
 *@return
 *@li   MDM_OK              �ɹ�
 *@li   MDM_ERR_DEV_OPEN    ���豸�ļ�ʧ��
 *@li   MDM_ERR_DEV_IOCTL   ��������ʧ��
 */
int mdm_drv_dtrhungup()
{
    mdmprint("call %s\n", __func__);
    int fd;
    int status;

    if ((fd = open(MDM_DEV, O_RDWR)) < 0)
        return MDM_ERR_DEV_OPEN;
    status = 1;
    if (ioctl(fd, MDM_IOCS_SETDTR, &status) < 0) {
        close(fd);
        return MDM_ERR_DEV_IOCTL;
    }
    mdm_msdelay(100);
    status = 0;
    if (ioctl(fd, MDM_IOCS_SETDTR, &status) < 0) {
        close(fd);
        return MDM_ERR_DEV_IOCTL;
    }
    close(fd);
    return MDM_OK;
}
