/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-设备驱动
 * 作    者：    产品开发部
 * 日    期：    2013-12-10
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */

#ifndef MDM_DEBUG_INCLUDE_H
#define MDM_DEBUG_INCLUDE_H

extern void TRACE(const char *format, ...);

//#define MODEM_DEBUG_DATA
#ifdef MODEM_DEBUG_DATA
#define mdmprint(format, args ...) TRACE(format, ## args)
#define mdmdataprintf(format, args ...)  fprintf(stderr, format, ## args)
#else
#define mdmprint(format, args ...)
#define mdmdataprintf(format, args ...)
#endif


#endif
