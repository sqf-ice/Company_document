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

#define NDK_APP_PATH_MAX		256			/**<Ӧ�ó����·������*/
#define NDK_APP_MAX_CHILD		1

#define NDK_APP_FILE_PATH		"/appfs/apps/"
#define NDK_APP_PUBKEY_FILE_DBG		"/etc/pubkey_dbg"

/** @} */ // ��Ӧ��ģ�����

#endif
