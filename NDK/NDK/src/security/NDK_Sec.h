/*
* �´�½��˾ ��Ȩ����(c) 2011-2015
*
* ͳһƽ̨NDK API
* ��    �ߣ�	��Ʒ������
* ��    �ڣ�	2012-08-17
* ��	����	V1.00
* ����޸��ˣ�
* ����޸����ڣ�
*/

#ifndef __NDK_App_H
#define __NDK_App_H

#include <stdlib.h>
#include "NDK.h"
#include "NDK_debug.h"

/** @addtogroup ��ȫ
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

/** @} */ // ��ȫģ�����

#endif

/* End of this file */
