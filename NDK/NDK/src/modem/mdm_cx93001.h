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

#ifndef MDM_CX93001_INCLUDE_H
#define MDM_CX93001_INCLUDE_H


#define VOL_OF_ABSENT_LINE_cx93001      1.8             //С�ڸ�ֵ��ʾ���绰��δ���
#define VOL_OF_LINE_IN_USE_Cx93001      12.0            //С�ڸ�ֵ��ʾ����⵽������ʹ����ѹ����


int mdm_sdlc_init_cx93001(void);
int mdm_sdlc_dial_cx93001(const char *dialno);
int mdm_sdlc_hangup_cx93001(void);
int mdm_asyn_init_cx93001(void);
int mdm_asyn_dial_cx93001(const char *dialno);
int mdm_asyn_hangup_cx93001(void);

#endif
