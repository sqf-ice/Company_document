/*
 * �´�½��˾ ��Ȩ����(c) 2011-2015
 *
 * modemģ��-�豸����
 * ��    �ߣ�    ��Ʒ������
 * ��    �ڣ�    2013-12-10
 * ��    ����    V1.00
 * ����޸��ˣ�
 * ����޸����ڣ�
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
