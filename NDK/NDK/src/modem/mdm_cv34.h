/*
 * �´�½��˾ ��Ȩ����(c) 2011-2015
 *
 * modemģ��-cx93001оƬ
 * ��    �ߣ�    ��Ʒ������
 * ��    �ڣ�    2013-12-10
 * ��    ����    V1.00
 * ����޸��ˣ�
 * ����޸����ڣ�
 */

#ifndef MDM_CV34_INCLUDE_H
#define MDM_CV34_INCLUDE_H

#define VOL_OF_ABSENT_LINE_CV34     2                   //С�ڸ�ֵ��ʾ���绰��δ���
#define VOL_OF_LINE_IN_USE_CV34     12                  //С�ڸ�ֵ��ʾ����⵽������ʹ����ѹ����

int mdm_sdlc_init_cv34(void);
int mdm_sdlc_dial_cv34(const char *dialno);
int mdm_sdlc_hangup_cv34(void);
int mdm_asyn_init_cv34(void);
int mdm_asyn_dial_cv34(const char *dialno);
int mdm_asyn_hangup_cv34(void);

#endif
