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
#ifndef __CONFIG_H__
#define	__CONFIG_H__

#include "parsecfg.h"

int ndk_getconfig(const char * optname, const char * confname, cfgValueType type, void * confvalue);
int ndk_setconfig(const char *optname, const char * confname, cfgValueType type, const void * confvalue);

#endif

