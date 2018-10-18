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

#ifndef __NDK_DEBUG_H
#define __NDK_DEBUG_H

#include <syslog.h>

#define NEXPORT		/**<NDK �����ṩ�������ꡢ�ṹ����Ϣ����Ҫ����ͷ�����Ӹö���,�������߽��Զ������ı�*/
/**
 *@brief LOG���ģ�鴮����
*/
#define NDK_LOG_MODULE_PRINT	              "print"		/**<��ӡģ��*/
#define NDK_LOG_MODULE_DISP		"disp"		/**<��ʾģ��*/
#define NDK_LOG_MODULE_SEC		"sec"		/**<��ȫģ��*/
#define NDK_LOG_MODULE_WLM		"wirelessmodem"	/**<����ģ��*/
#define NDK_LOG_MODULE_ETH		"ethernet"              /**<��̫��ģ��*/
#define NDK_LOG_MODULE_SOCKET	"socket"		/**<socketģ��*/
#define NDK_LOG_MODULE_PORT		"port"		/**<����ģ��*/
#define NDK_LOG_MODULE_PPP		"ppp"		/**<PPPģ��*/
#define NDK_LOG_MODULE_WIFI                  "wifi"                     /**<wifiģ��*/
#define NDK_LOG_MODULE_MODEM               "modem"              /**<MODEM*/
#define NDK_LOG_MODULE_APP                    "mulapp"               /**<��Ӧ��*/
#define NDK_LOG_MODULE_SYS                    "sys"               /**<ϵͳ�ӿ�ģ��*/
#define NDK_LOG_MODULE_RFID                    "rfid"               /**<��Ƶģ��*/


/**
 *@brief ������Ϣ���壬�ּ����
 *	NDK_LOG_DEBUG      	debug����
 * 	NDK_LOG_INFO		��Ϣ
 *  NDK_LOG_NOTICE		������Ϣ�����Ƚ���Ҫ
 *	NDK_LOG_WARNING 	����
 *	NDK_LOG_ERR			����
 *	NDK_LOG_CRIT  		�ٽ�
 *	NDK_LOG_ALERT		������������
 *	NDK_LOG_EMERG      	ϵͳ������
*/

//��ͨģ��DEBUG
void NDK_LOG_DEBUG(char *module,const char *format, ...);
void NDK_LOG_INFO(char *module,const char *format, ...);
void NDK_LOG_NOTICE(char *module,const char *format, ...);


//ͨѸģ��DEBUG
void NDK_COMM_LOG_DEBUG(char *module,const char *format, ...);
void NDK_COMM_LOG_INFO(char *module,const char *format, ...);
void NDK_COMM_LOG_NOTICE(char *module,const char *format, ...);

void NDK_LOG_ALERT(char *module,const char *format, ...);
void NDK_LOG_CRIT(char *module,const char *format, ...);
void NDK_LOG_EMERG(char *module,const char *format, ...);
void NDK_LOG_ERR(char *module,const char *format, ...);
void NDK_LOG_WARNING(char *module,const char *format, ...);


#endif
