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

#ifndef MDM_COMM_INCLUDE_H
#define MDM_COMM_INCLUDE_H

#include <sys/time.h>

#define MDM_AUX_DEV_NAME        "/dev/ttyS2"
#define MAXLEN_PORTBUF 4096         //modem���ڵ���󻺴��С
#define ATSTREND "\x0d\x0a"

//modem AT����ֵ
typedef enum {
    MDM_AT_RET_OK = 0,
    MDM_AT_RET_CONNECT,
    MDM_AT_RET_RING,
    MDM_AT_RET_NO_CARRIER,
    MDM_AT_RET_ERROR,
    MDM_AT_RET_NO_DIALTONE,
    MDM_AT_RET_BUSY,
    MDM_AT_RET_NO_ANSWER,
    MDM_AT_RET_CIDM,
    MDM_AT_RET_FLASH,
    MDM_AT_RET_STAS,
    MDM_AT_RET_X,
    MDM_AT_RET_UN_OBTAINABLE_NUMBER,
    MDM_AT_RET_F,
    MDM_AT_RET_DWN_INIT,
    MDM_AT_RET_PV_0000000000,
    MDM_AT_RET_PV_F2000E0B02,
    MDM_AT_RET_NULL,
} EM_MDM_AT_RET;

typedef struct {
    EM_MDM_AT_RET   ret;
    const char *    respone;
} ST_MDM_AT_RET_MAP;

//��������AT����ʱ��ʹ��
typedef struct {
    const char *    cmd;            //�����ַ���
    EM_MDM_AT_RET   exp_ret;        //Ԥ�ڷ���
    unsigned int    timeout_ms;     //��ʱʱ��
} ST_MDM_AT_CMD;

int mdm_port_close(void);
int mdm_port_clr_buf(void);
int mdm_port_buf_len(int *pnReadlen);
int mdm_port_get_line(char *pszData, unsigned int nMaxLen, unsigned int nTimeOutMs);
int mdm_port_put_string(const char *psSendBuf, int SendLen);
int mdm_port_at_cmd_process(const char *pszCmd, char *pszRespone, int nMaxLen, int nTimeOutMs);
int mdm_port_read_line(char *pszData, unsigned int *punDatalen, unsigned int nTimeOutMs);

#endif
