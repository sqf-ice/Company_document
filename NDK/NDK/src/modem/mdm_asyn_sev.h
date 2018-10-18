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

#ifndef MDM_ASYN_SERV_INCLUDE_H
#define MDM_ASYN_SERV_INCLUDE_H

typedef enum {
    MDM_ASYNS_CLOSED = 0,   //�Ѿ��Ҷ�
    MDM_ASYNS_INITED,       //�ѳ�ʼ��
    MDM_ASYNS_DIALED,       //�Ѳ���
    MDM_ASYNS_CONNECT,      //�Ѿ����յ�CONNEC
} EM_MDM_ASYNS;

typedef enum {
    MDM_ASYNC_DONE,         //�����Ѿ�ִ��
    MDM_ASYNC_INIT,         //��ʼ��
    MDM_ASYNC_DIAL,         //��ʼASYNͬ������
    MDM_ASYNC_HUANGUP,      //�Ҷ�ASYNͬ��
} EM_MDM_ASYNC;

int mdm_asyn_serv_init(void);
int mdm_asyn_serv_dial(const char *dialno);
int mdm_asyn_serv_hangup(void);
int mdm_asyn_serv_getstatus(void);
#endif
