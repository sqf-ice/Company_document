/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：	产品开发部
* 日    期：	2012-08-17
* 版	本：	V1.00
* 最后修改人：
* 最后修改日期：
*/

#ifndef __NDK_App_H
#define __NDK_App_H

#include <stdlib.h>
#include "NDK.h"
#include "NDK_debug.h"

/** @addtogroup 安全
* @{
*/

//#define DEBUG
#undef SECDEBUG
#ifdef DEBUG
#define SECDEBUG(fmt, args...) fprintf(stderr, "sec_ndk: "fmt, ##args)
#define SECDEBUG_RAW(fmt, args...) fprintf(stderr, fmt, ## args)
#define SECDEBUG_DATA(info, data, len) {int i;\
										fprintf(stderr,"NDK_SECP_%s: len=%d data=", (info), (len));\
										for(i=0; i<(len); i++){\
										fprintf(stderr, "%02x ", *(data+i));\
										}\
										fprintf(stderr, "\n");\
										}
#else
#define SECDEBUG(fmt, args...)
#define SECDEBUG_RAW(fmt, args...)
#define SECDEBUG_DATA(info, data, len)
#endif

/** @} */ // 安全模块结束

#endif

/* End of this file */
