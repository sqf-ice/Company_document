/*
* �´�½��˾ ��Ȩ����(c) 2011-2015
*
* ͳһƽ̨NDK API
* ��    �ߣ�	��Ʒ������
* ��    �ڣ�	2012-08-17
* ��	����	V1.00
* ����޸��ˣ�csl
* ����޸����ڣ�
*/
#ifndef _PPP_H_H_
#define _PPP_H_H_

#include "NDK.h"
#include "NDK_debug.h"
#include "NDK_PPP.h"


#ifdef NDK_DEBUG
#define NDK_DEBUG_PPP
#endif

#ifdef NDK_DEBUG_PPP
#define TRACE_PPP(fmt, args...) NDK_LOG_DEBUG(NDK_LOG_MODULE_PPP,fmt, ##args)
#else
#define TRACE_PPP(fmt, args...)
#endif


#define PPP_TASK_SHM_VER 	"V1.0.0"

#define PPP_CFG_PATH			"/tmp/ppp"
#define PPP_FILE_LOCK			"/tmp/ppp/ppp_lock"
#define PPP_SHM_NAME 	 		"/tmp/ppp/pppshm"


typedef enum PPP_CMD {
    PPP_CMD_NOP=0,
    PPP_CMD_DAIL,
    PPP_CMD_HANGUP
} PPP_CMD;

typedef struct netPPPCfg {
    int nDevType;			/**<���ͣ�����������MODEM��������MODEM��0��ʾ���ߣ�1��ʾ����*/
    unsigned int nPPPFlag;	/**<�Ƿ�֧��ά�ֳ����ӱ�ʶ��������Ҫ�������Ӧ�ĺ궨��*/
    char szApn[32];			/**<APN����*/
    char szDailNum[32];		/**<���ź���*/
    int (*ModemDial)(void);/**<���ź���*/
    unsigned int PPPIntervalTimeOut; /*ά�ֳ����ӵ����ݰ����͵�ʱ����*/
} strPPPCfg;

typedef struct netPPPCfg_apn {
    int nDevType;			/**<���ͣ�����������MODEM��������MODEM��0��ʾ���ߣ�1��ʾ����*/
    unsigned int nPPPFlag;	/**<�Ƿ�֧��ά�ֳ����ӱ�ʶ��������Ҫ�������Ӧ�ĺ궨��*/
    char szApn[64];			/**<APN����*/
    char szDailNum[32];		/**<���ź���*/
    int (*ModemDial)(void);/**<���ź���*/
    unsigned int PPPIntervalTimeOut; /**<ά�ֳ����ӵ����ݰ����͵�ʱ����*/
} strPPPCfg_apn;

typedef struct ext_netPPPCfg_apn {
    int nDevType;			/**<���ͣ�����������MODEM��������MODEM��0��ʾ���ߣ�1��ʾ����*/
    unsigned int nPPPFlag;	/**<�Ƿ�֧��ά�ֳ����ӱ�ʶ��������Ҫ�������Ӧ�ĺ궨��*/
    char szApn[64];			/**<APN����*/
    char szDailNum[32];		/**<���ź���*/
    int (*ModemDial)(void);/**<���ź���*/
    unsigned int PPPIntervalTimeOut; /*ά�ֳ����ӵ����ݰ����͵�ʱ����*/
    unsigned char nMinSQVal;	/**<��ʼ��ʱ���������С���ź�ֵ*/
    char szPin[31];				/**<SIM��PIN��*/
    char nPPPHostIP[20];		/**<ά�ֳ�������ҪPIN������IP*/
} strExtPPPCfg_apn;

/**
 *@brief  ����ȡ�ź�ǿ��״̬
*/
typedef enum{
	WLM_SQ_FLUSH_ENABLE=0,		/**<����ˢ��״̬*/
	WLM_SQ_FLUSH_WORKING,		/**<����ˢ��*/
	WLM_SQ_FLUSH_DISABLE,		/**<��ֹˢ��״̬*/
}EM_WLM_SQ_FLUSH;

/**
 *@brief  PPP�������ݽṹ
*/
typedef struct {
    char ver[8];					/**<�����汾*/
    char cmd;						/**<��������*/
    char status;					/**<����״̬*/
    char devtype;					/**<�豸����*/
    char debug;						/**<���Ա�־*/
    int errcode;					/**<�������*/
    unsigned int linkparam;			/**<PPP���Ӳ���*/
    char ischat;					/**<�Ƿ�ʹ��chat����*/
    char name[47];					/**<�û���*/
    char passwd[32];				/**<����*/
    char apn[64];					/**<apn*/
    char dailnum[32];				/**<���ź���*/
    unsigned char min_sq;			/**<����ʱ�������С�ź�ֵ*/
    char pin[31];					/**<��PIN��*/
    char hostip[20];				/**<ά�ֳ�������ҪPIN������IP*/
    unsigned int link_resend_time;	/**<�����ӷ��ͼ��ʱ��*/
    unsigned int link_keep_cnt;		/**<�������Ѽ����Ŀ���ʱ��*/
    pid_t ppid;						/**<���̺�*/
    int rst_flag;					/**<��λ��־*/
    char sq_flush_flag;				/**<�����ź�ˢ�±�־*/
	unsigned int pppTimeCnt;		/**<pppʱ�����*/
	unsigned int auth_type;			/**<CHAP PAP EAP*/
	unsigned int sq;							/**<��ǰ�ź�ֵ��99--���źš�0~31�ź�ֵ*/
	unsigned int dail_sim_bind;      /**<dail�����Ƿ��sim�����ţ�0δ�󶨲��ţ�1�󶨲���*/
} pppTaskData;


int ndk_stop_sq_flush(void);
int ndk_allow_sq_flush(void);
int ndk_write_rst_flag(void);
int ndk_NET_PPPGetStatus(EM_PPP_STATUS *piStatus);
int ndk_NET_PPPSetParam(const void *pInParam, int nValidLen);
int ndk_read_rst_flag(void);
int ndk_NET_PPPOpen(int nIsUseChat,int nIsSimBind);
int ndk_NET_PPPClose(void);
int ndk_getPPPerrorcode(int ph);
void ndk_netSetLogin(const char *luser, const char *lpassword);
unsigned long ndk_getPPP0Addr(char *szAddrInfo);


#endif
