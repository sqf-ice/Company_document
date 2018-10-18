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

#ifndef MDM_DRV_INCLUDE_H
#define MDM_DRV_INCLUDE_H

#define TICKS_PERSECOND 100

#define MDM_DEV             "/dev/modem"
#define MDM_IOC_MAGIC       'M'

#define MDM_IOCG_VER        _IO(MDM_IOC_MAGIC, 0)
#define MDM_IOCS_RESET      _IO(MDM_IOC_MAGIC, 1)
#define MDM_IOCS_SLEEP      _IO(MDM_IOC_MAGIC, 2)
#define MDM_IOCS_GETDCD     _IO(MDM_IOC_MAGIC, 3)
#define MDM_IOCS_SETDTR     _IO(MDM_IOC_MAGIC, 4)
/*****************************************************************
 *                                     ���ͳ�Ʒ���IOCS
 ******************************************************************/
#define MDM_IOCS_SDLCDAIL       _IO(MDM_IOC_MAGIC, 6)
#define MDM_IOCS_ASYNDAIL       _IO(MDM_IOC_MAGIC, 7)
#define MDM_IOCS_FAIL       _IO(MDM_IOC_MAGIC, 8)
#define MDM_IOCS_HUNGUP     _IO(MDM_IOC_MAGIC, 9)


#define TICK_DEV_MAGIC    'T'
#define TICK_IOCG_JIFFIES _IO(TICK_DEV_MAGIC, 0)

void mdm_msdelay(uint msTime);
long int mdm_get_time(void);
int mdm_get_datetime(char *pstDatetime);
int mdm_drv_exist(void);
int mdm_drv_reset(void);
int mdm_drv_sleep(void);
int mdm_drv_sdlcdial(void);
int mdm_drv_asyndial(void);
int mdm_drv_hungup(void);
int mdm_drv_dialfail(void);
int mdm_drv_dtrhungup(void);

#endif
