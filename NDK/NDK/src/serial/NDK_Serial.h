/*
* 新大陆公司 版权所有(c) 2006-2008
*
* POS API
* 文件配置相关	--- Config.c	*
* 作    者：    		产品开发部
* 日    期：    		2012-08-17
* 最后修改人：  	lidh
* 最后修改日期：
*/
#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "NDK.h"

/** @addtogroup 串口通讯
* @{
*/

/**
 *@brief  波特率
*/
typedef enum {
    PORT_BPS_300 = 0,	/**<300波特率*/
    PORT_BPS_1200,	/**<1200波特率*/
    PORT_BPS_2400,	/**<2400波特率*/
    PORT_BPS_4800,	/**<4800波特率*/
//  PORT_BPS_7200,	/**<7200波特率*/
    PORT_BPS_9600,	/**<9600波特率*/
    PORT_BPS_19200,	/**<19200波特率*/
    PORT_BPS_38400,	/**<38400波特率*/
    PORT_BPS_57600,	/**<57600波特率*/
 // PORT_BPS_76800,	/**<76800波特率,所有版本BIOS不支持该波特率*/
    PORT_BPS_115200 = 10	/**<115200波特率*/
} EM_PORT_BPS;


/**
 *@brief  串口设置
*/
typedef enum {
   PORT_CFG_DB8 = 0xc0,	/**<8位数据位选择*/
   PORT_CFG_DB7 = 0x40,	/**<7位数据位选择*/
   PORT_CFG_DB6 = 0x80,	/**<6位数据位选择*/
   PORT_CFG_DB5 = 0x00,	/**<5位数据位选择*/
   PORT_CFG_STOP2 = 0x20,	/**<2位停止位*/
   PORT_CFG_STOP15 = 0x10,	/**<1.5位停止位*/
   PORT_CFG_STOP1 = 0x08,	/**<1位停止位*/
   PORT_CFG_NP = 0x04,	/**<无校验*/
   PORT_CFG_EP = 0x02,	/**<偶校验*/
   PORT_CFG_OP = 0x01,	/**<奇校验*/
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


/** @} */ // 串口通讯模块结束

#endif

/* End of this file */
