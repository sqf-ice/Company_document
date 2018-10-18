/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-设备驱动
 * 作    者：    产品开发部
 * 日    期：    2013-12-10
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */

#ifndef MDM_COMM_INCLUDE_H
#define MDM_COMM_INCLUDE_H

#include <sys/time.h>

#define MDM_AUX_DEV_NAME        "/dev/ttyS2"
#define MAXLEN_PORTBUF 4096         //modem串口的最大缓存大小
#define ATSTREND "\x0d\x0a"

//modem AT返回值
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

//连续发送AT命令时，使用
typedef struct {
    const char *    cmd;            //命令字符串
    EM_MDM_AT_RET   exp_ret;        //预期返回
    unsigned int    timeout_ms;     //超时时间
} ST_MDM_AT_CMD;

int mdm_port_close(void);
int mdm_port_clr_buf(void);
int mdm_port_buf_len(int *pnReadlen);
int mdm_port_get_line(char *pszData, unsigned int nMaxLen, unsigned int nTimeOutMs);
int mdm_port_put_string(const char *psSendBuf, int SendLen);
int mdm_port_at_cmd_process(const char *pszCmd, char *pszRespone, int nMaxLen, int nTimeOutMs);
int mdm_port_read_line(char *pszData, unsigned int *punDatalen, unsigned int nTimeOutMs);

#endif
