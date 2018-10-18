/*
* �´�½��˾ ��Ȩ����(c) 2006-2008
*
* POS API
* �ļ��������	--- Config.c	*
* ��    �ߣ�    		��Ʒ������
* ��    �ڣ�    		2012-08-17
* ����޸��ˣ�  	lidh
* ����޸����ڣ�
*/
#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "NDK.h"

/** @addtogroup ����ͨѶ
* @{
*/

/**
 *@brief  ������
*/
typedef enum {
    PORT_BPS_300 = 0,	/**<300������*/
    PORT_BPS_1200,	/**<1200������*/
    PORT_BPS_2400,	/**<2400������*/
    PORT_BPS_4800,	/**<4800������*/
//  PORT_BPS_7200,	/**<7200������*/
    PORT_BPS_9600,	/**<9600������*/
    PORT_BPS_19200,	/**<19200������*/
    PORT_BPS_38400,	/**<38400������*/
    PORT_BPS_57600,	/**<57600������*/
 // PORT_BPS_76800,	/**<76800������,���а汾BIOS��֧�ָò�����*/
    PORT_BPS_115200 = 10	/**<115200������*/
} EM_PORT_BPS;


/**
 *@brief  ��������
*/
typedef enum {
   PORT_CFG_DB8 = 0xc0,	/**<8λ����λѡ��*/
   PORT_CFG_DB7 = 0x40,	/**<7λ����λѡ��*/
   PORT_CFG_DB6 = 0x80,	/**<6λ����λѡ��*/
   PORT_CFG_DB5 = 0x00,	/**<5λ����λѡ��*/
   PORT_CFG_STOP2 = 0x20,	/**<2λֹͣλ*/
   PORT_CFG_STOP15 = 0x10,	/**<1.5λֹͣλ*/
   PORT_CFG_STOP1 = 0x08,	/**<1λֹͣλ*/
   PORT_CFG_NP = 0x04,	/**<��У��*/
   PORT_CFG_EP = 0x02,	/**<żУ��*/
   PORT_CFG_OP = 0x01,	/**<��У��*/
} EM_PORT_CFG;

#define POWER_IOC_MAGIC 'P'
#define POWER_SET_USB_STATUS	_IOW(POWER_IOC_MAGIC, 17, int)
#define GPIO_IOC_MAGIC 'G'
#define GPIO_IOCS_USB_SLAVE		_IO(GPIO_IOC_MAGIC, 1)

int NDK_PortOpen(EM_PORT_NUM emPort, const char *pszAttr);
int NDK_PortClose(EM_PORT_NUM emPort);
int NDK_PortRead(EM_PORT_NUM emPort, uint unLen, char *pszOutbuf,int nTimeoutMs, int *pnReadlen);
int NDK_PortWrite(EM_PORT_NUM emPort, uint unLen,const char *pszInbuf);
int NDK_PortTxSendOver(EM_PORT_NUM emPort);
int NDK_PortClrBuf(EM_PORT_NUM emPort);
int NDK_PortReadLen(EM_PORT_NUM emPort,int *pnReadlen);


/** @} */ // ����ͨѶģ�����

#endif

/* End of this file */
