/*
* 新大陆公司 版权所有(c) 2006-2008
*
* POS API
* 文件配置相关	--- Config.c	*
* 作    者：    		产品开发部
* 日    期：    		2012-08-17
* 最后修改人：
* 最后修改日期：
*/
#include <linux/ioctl.h>
#ifndef __DISP_H__
#define	__DISP_H__

/**< device name */
#define DEV_NAME		"disp"
/**< major number of device */
#define DEV_MAJOR	10

#define DISP_IOC_MAGIC		DEV_MAJOR
#define DISP_IOC_GDRIVERINFO	_IO(DISP_IOC_MAGIC, 1)
#define DISP_IOC_GINFO		_IO(DISP_IOC_MAGIC, 2)     /**< get information */
#define DISP_IOC_GTYPE		_IO(DISP_IOC_MAGIC, 3)     /**< get information */
#define DISP_IOC_BL			_IO(DISP_IOC_MAGIC, 4)    /* set BL(back light) */
#define DISP_IOC_SPIXEL		_IO(DISP_IOC_MAGIC, 5)      /* set pixel */
#define DISP_IOC_SBLOCK		_IO(DISP_IOC_MAGIC, 6)      /**< set block */
#define DISP_IOC_LOCK	    	_IO(DISP_IOC_MAGIC, 7)       /**< set block */
#define DISP_IOC_SLEEP	    	_IO(DISP_IOC_MAGIC, 8)       /**< lcd sleep */
#define DISP_IOC_WAKE	    	_IO(DISP_IOC_MAGIC, 9)       /**< lcd wakeup */
#define DISP_IOC_RESET	    	_IO(DISP_IOC_MAGIC, 10)       /**< lcd reset */
#define DISP_IOC_SCONTRAST		_IO(DISP_IOC_MAGIC, 11)       /**< set contrast */


#define GPIO_IOC_MAGIC 'G'
#define GPIO_IOCS_BACKLIGHT		_IO(GPIO_IOC_MAGIC, 2)		/*backlight control*/
#endif

