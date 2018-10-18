#ifndef _DEBUG_H_
#define _DEBUG_H_

/******************************************************************************
** Copyright (c) 2008福建新大陆电脑公司金融产品开发中心
** 版本号  	：V0.1
** 生成日期	：2008-09-16
** 创建人  	：lipm
** 文件功能	：调试函数头文件
**
** 文件内容	：调试函数宏定义
** 主要函数及其功能：
				     DOWNLOAD_ASSERT		提示错误发生的位置
				     DOWNLOAD_DUMP			打印调试信息到日志文件

** 与其它文件的关系：
**           		 为下载程序各模块提供调试函数宏定义.

** 修改人  	：无
** 日　期  	：
** 描　述  	：无
******************************************************************************/
#include "NDK.h"
#include "Dump.h"


//维护下载日志
#define WriteLog(fp, x...) debug_print(fp, __FILE__, __LINE__, __FUNCTION__, ##x);

//调试开关：正式版本中，请将此宏定义注释
//#define DEBUG


#ifdef DEBUG

/* DOWNLOAD_ASSERT 宏的功能:
    在调试环境下，如果程序运行发生错误，则在pos屏幕上提示错误发生的位置*/
#define DOWNLOAD_ASSERT(x) if(!(x)) \
	{ \
		int nKey;\
		NDK_ScrClrs();\
		NDK_ScrPrintf("%s(%d):\n%s", __FILE__, __LINE__, __FUNCTION__); \
		NDK_KbGetCode(0,&nKey); \
	} \

/* DOWNLOAD_DUMP 宏的功能:
    在调试环境下，将调试信息打印到文件fp_log中*/
#define DOWNLOAD_DUMP(fp, x...) debug_print(fp, __FILE__, __LINE__, __FUNCTION__, ##x);

#else

#define DOWNLOAD_ASSERT(x)
#define DOWNLOAD_DUMP(fp, x...)

#endif
#endif
