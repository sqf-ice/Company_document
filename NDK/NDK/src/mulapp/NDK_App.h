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

#ifndef NDK_APP_H
#define NDK_APP_H

#include "NDK.h"
#include "NDK_debug.h"
/*
 * Debug Switch
 */
//#define DEBUG
#ifdef DEBUG
#define PDEBUG(fmt, args...) fprintf(stderr,  "Mulapp: "fmt, ##args)
#else
#define PDEBUG(fmt, args...) 
#endif

#define NDK_APP_PATH_MAX		256			/**<应用程序最长路径名称*/
#define NDK_APP_MAX_CHILD		1

#define NDK_APP_FILE_PATH		"/appfs/apps/"
#define NDK_APP_PUBKEY_FILE_DBG		"/etc/pubkey_dbg"

/** @} */ // 多应用模块结束

#endif
