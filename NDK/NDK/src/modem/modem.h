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
#ifndef MODEM_INCLUDE_H
#define MODEM_INCLUDE_H

#include <pthread.h>
#define MAXLEN_DIALNUM  80
#define MAXLEN_PERATCOMMAND_SYS 52
#define MDM_LASTDIAL_FILE	"/tmp/ModemLastDial"

typedef enum {
    MDM_OK = 0,
    MDM_ERR_PARA = -1,
    MDM_ERR_DEV_OPEN = -2,
    MDM_ERR_DEV_IOCTL = -3,
    MDM_ERR_SDLC_PTHREAD_CREAT = -4,
    MDM_ERR_CFG_OPEN = -5,
    MDM_ERR_CFG_READ = -6,
    MDM_ERR_CFG_INVALID = -7,

    MDM_ERR_AUX_SET = -8,
    MDM_ERR_AUX_GET = -9,
    MDM_ERR_AUX_CLR_BUF = -10,
    MDM_ERR_AUX_WRITE = -11,
    MDM_ERR_AUX_WRITE_LEN = -12,
    MDM_ERR_AUX_WRITE_TIMEOUT = -13,
    MDM_ERR_AUX_READ = -14,
    MDM_ERR_AUX_READ_TIMEOUT = -15,
    MDM_ERR_AUX_READ_LEN = -16,
    MDM_ERR_AUX_BUF_LEN = -17,

    MDM_ERR_SDLC_PACK_DATALEN = -18,
    MDM_ERR_SDLC_PACK_PARA = -19,
    MDM_ERR_SDLC_UNPACK_PARA = -20,
    MDM_ERR_SDLC_UNPACK_DATALEN = -21, //解包数据溢出
    MDM_ERR_SDLC_WAITSNRM_TIMEOUT = -22,
    MDM_ERR_SDLC_WRITE_BUFERR = -23,
    MDM_ERR_SDLC_READ_BUFERR = -24,
    MDM_ERR_SDLC_STATUS = -25,
    MDM_ERR_SDLC_NODIAL = -26,
    MDM_ERR_SDLC_DISCONNECT = -27,
    MDM_ERR_SDLC_WRITE_TIMEOUT = -28,
    MDM_ERR_SDLC_READ_TIMEOUT = -29,
    MDM_ERR_SDLC_HUNGUP_TIMEOUT = -30,
    MDM_ERR_SDLC_HUNGUP_FAIL = -31,
    MDM_ERR_SDLC_DAIL_FAIL = -32,
    MDM_ERR_SDLC_DAIL_TIMEOUT = -33,
    MDM_ERR_SDLCS_INIT_FAIL = -34,
    MDM_ERR_SDLCSERV_INIT_FAIL = -35,
    MDM_ERR_SDLCSERV_INIT_TIMEOUT = -36,
    MDM_ERR_ASYN_READ_TIMEOUT = -37,
    MDM_ERR_ASYN_HUNGUP_FAIL = -38,
    MDM_ERR_ASYN_DAIL_FAIL = -39,
    MDM_ERR_ASYN_INIT_FAIL = -40,
    MDM_ERR_ASYN_GET_STATUS = -41,
    MDM_ERR_ASYN_STATUS_INVALID = -42,
    MDM_ERR_NOLINE = -43,
    MDM_ERR_OTHERMACHINE = -44,
    MDM_ERR_GET_CHIPTYPE = -45,
    MDM_ERR_CHIPTYPE_INVALIDE = -46,
    MDM_ERR_ATCMD_RESP = -47,
    MDM_ERR_GETLINEVOL_FAIL = -48,
    MDM_ERR_ASYN_STATUS = -50,
    MDM_ERR_ASYN_PTHREAD_CREAT = -51,
    MDM_ERR_ASYNSERV_INIT_FAIL = -52,
    MDM_ERR_ASYNSERV_INIT_TIMEOUT = -53,
    MDM_ERR_ASYN_DAIL_TIMEOUT = -54,
    MDM_ERR_ASYN_HUNGUP_TIMEOUT = -55,
    MDM_ERR_ADAPT_FAIL = -56,
    MDM_ERR_ADAPT_DIALFAIL = -57,
    MDM_ERR_ADAPT_CONFIG_FILE = -58,
    MDM_ERR_ADAPT_HIGHVOL = -59,
    MDM_ERR_ADAPT_ANSWERTONE = -60,
    MDM_ERR_ADAPT_LOWVOL = -70,
    MDM_ERR_ADAPT_RECSIGNAL = -80,
    MDM_ERR_DB_SIZE = -81,
    MDM_ERR_SDLC_RECSIGNAL = -82,
    MDM_ERR_SDLC_WAITRR_TIMEOUT = -83,
    MDM_ERR_DB_OPEN = -84,
    MDM_ERR_DB_MKDIR = -85,
    MDM_ERR_DB_CLOSE = -86,
    MDM_ERR_DB_EXEC = -87,
	MDM_ERR_DIAL_RECSNRMMORE = -88,
} EM_MDM_ERR;


/**
 *@brief modem模块定义
 */
typedef enum {
    MODEM_TPYE_NODEFINED = 0,       /**<模块未定义*/
    CONAXENT_MODEM,                 /**<conexant模块*/
    LSI_MODEM,                      /**<CV34 ver 8 版本*/
} MODEM_CHIP_TYPE;

typedef struct {
    pthread_rwlock_t    rwLock;                         //服务线程锁
    int         nDialType;                      //同步或者异步拨号
    int         nStatus;                        //服务状态
    int         nCtrl;                          //服务控制变量
    char            szDialNum[MAXLEN_DIALNUM + 5];  //拨号号码
    char            szDialTime[32];
    int         nErr;
} ST_MDM_STATUS;


typedef struct {
    int (*mdm_sdlc_init)(void);
	int (*mdm_sdlc_dial)(const char *);
    int (*mdm_sdlc_hangup)(void);
    int (*mdm_asyn_init)(void);
	int (*mdm_asyn_dial)(const char *);
    int (*mdm_asyn_hangup)(void);
} MODEM_FUNC;

int ndk_mdmadaptrate(void);
int ndk_mdmsleep(void);
int ndk_mdmreset(void);

#endif
