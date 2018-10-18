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

#ifndef MDM_SDLC_SERV_INCLUDE_H
#define MDM_SDLC_SERV_INCLUDE_H

#define MAXLEN_SDLCDATA 2500    //��֡���ɴ������ݴ�С
#define UpNo(no)  (((no) - 1) % 8)
#define NextNo(no)  (((no) + 1) % 8)
#define AddNo(no)   ((no) = NextNo(no))

typedef enum {
    MDM_SDLCS_CLOSED = 0,   //�Ѿ��Ҷ�
    MDM_SDLCS_INITED,       //�ѳ�ʼ��
    MDM_SDLCS_DIALED,       //�Ѳ���
    MDM_SDLCS_CONNECT,      //�Ѿ����յ�CONNECT
    MDM_SDLCS_RR,           //����·����״̬
} EM_MDM_SDLCS;

typedef enum {
    MDM_SDLCC_DONE,         //�����Ѿ�ִ��
    MDM_SDLCC_INIT,         //��ʼ��
    MDM_SDLCC_DIAL,         //��ʼSDLCͬ������
    MDM_SDLCC_HUANGUP,      //�Ҷ�SDLCͬ��
} EM_MDM_SDLCC;

typedef enum {
    MDM_SDLC_SEND_NONE = 0, //�������κ�֡
    MDM_SDLC_SEND_UA,       //����UA֡
    MDM_SDLC_SEND_RR,       //����RR֡
	MDM_SDLC_SEND_REJ,		//����REJ֡
    MDM_SDLC_SEND_I,        //����I֡
} EM_MDM_SDLC_SENDTYPE;

int mdm_sdlc_serv_init(void);
int mdm_sdlc_serv_dial(const char *dialno);
int mdm_sdlc_serv_write(const char *pszdata, int ndatalen);
int mdm_sdlc_serv_read(char *pszdata, int *ndatalen, int ntimeout);
int mdm_sdlc_serv_hangup(void);
int mdm_sdlc_serv_getstatus(void);
int mdm_sdlc_serv_readlen(void);
int mdm_sdlc_serv_clrbuff(void);
#endif
