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

#ifndef __NDK_DEBUG_H
#define __NDK_DEBUG_H

#include <syslog.h>

#define NEXPORT		/**<NDK 对外提供函数、宏、结构等信息，需要在行头部增加该定义,解析工具将自动生成文本*/
/**
 *@brief LOG输出模块串定义
*/
#define NDK_LOG_MODULE_PRINT	              "print"		/**<打印模块*/
#define NDK_LOG_MODULE_DISP		"disp"		/**<显示模块*/
#define NDK_LOG_MODULE_SEC		"sec"		/**<安全模块*/
#define NDK_LOG_MODULE_WLM		"wirelessmodem"	/**<无线模块*/
#define NDK_LOG_MODULE_ETH		"ethernet"              /**<以太网模块*/
#define NDK_LOG_MODULE_SOCKET	"socket"		/**<socket模块*/
#define NDK_LOG_MODULE_PORT		"port"		/**<串口模块*/
#define NDK_LOG_MODULE_PPP		"ppp"		/**<PPP模块*/
#define NDK_LOG_MODULE_WIFI                  "wifi"                     /**<wifi模块*/
#define NDK_LOG_MODULE_MODEM               "modem"              /**<MODEM*/
#define NDK_LOG_MODULE_APP                    "mulapp"               /**<多应用*/
#define NDK_LOG_MODULE_SYS                    "sys"               /**<系统接口模块*/
#define NDK_LOG_MODULE_RFID                    "rfid"               /**<射频模块*/


/**
 *@brief 调试信息定义，分级输出
 *	NDK_LOG_DEBUG      	debug调试
 * 	NDK_LOG_INFO		信息
 *  NDK_LOG_NOTICE		正常信息，但比较重要
 *	NDK_LOG_WARNING 	警告
 *	NDK_LOG_ERR			错误
 *	NDK_LOG_CRIT  		临界
 *	NDK_LOG_ALERT		必需立即介入
 *	NDK_LOG_EMERG      	系统部可用
*/

//普通模块DEBUG
void NDK_LOG_DEBUG(char *module,const char *format, ...);
void NDK_LOG_INFO(char *module,const char *format, ...);
void NDK_LOG_NOTICE(char *module,const char *format, ...);


//通迅模块DEBUG
void NDK_COMM_LOG_DEBUG(char *module,const char *format, ...);
void NDK_COMM_LOG_INFO(char *module,const char *format, ...);
void NDK_COMM_LOG_NOTICE(char *module,const char *format, ...);

void NDK_LOG_ALERT(char *module,const char *format, ...);
void NDK_LOG_CRIT(char *module,const char *format, ...);
void NDK_LOG_EMERG(char *module,const char *format, ...);
void NDK_LOG_ERR(char *module,const char *format, ...);
void NDK_LOG_WARNING(char *module,const char *format, ...);


#endif
