/*
* �´�½��˾ ��Ȩ����(c) 2006-2008
*
* POS API
* �ļ��������	--- Config.c	*
* ��    �ߣ�    		��Ʒ������
* ��    �ڣ�    		2012-08-17
* ����޸��ˣ�  
* ����޸����ڣ�
*/
#ifndef __CONFIG_H__
#define	__CONFIG_H__

#include "parsecfg.h"

int ndk_getconfig(const char * optname, const char * confname, cfgValueType type, void * confvalue);
int ndk_setconfig(const char *optname, const char * confname, cfgValueType type, const void * confvalue);

#endif

