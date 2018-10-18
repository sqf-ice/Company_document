/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：	产品开发部
* 日    期：	2012-08-17
* 版	本：	V1.00
* 最后修改人：csl
* 最后修改日期：
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
    int nDevType;			/**<类型，区别是无线MODEM还是有线MODEM，0表示无线，1表示有线*/
    unsigned int nPPPFlag;	/**<是否支持维持长连接标识，根据需要传入相对应的宏定义*/
    char szApn[32];			/**<APN设置*/
    char szDailNum[32];		/**<拨号号码*/
    int (*ModemDial)(void);/**<拨号函数*/
    unsigned int PPPIntervalTimeOut; /*维持长链接的数据包发送的时间间隔*/
} strPPPCfg;

typedef struct netPPPCfg_apn {
    int nDevType;			/**<类型，区别是无线MODEM还是有线MODEM，0表示无线，1表示有线*/
    unsigned int nPPPFlag;	/**<是否支持维持长连接标识，根据需要传入相对应的宏定义*/
    char szApn[64];			/**<APN设置*/
    char szDailNum[32];		/**<拨号号码*/
    int (*ModemDial)(void);/**<拨号函数*/
    unsigned int PPPIntervalTimeOut; /**<维持长链接的数据包发送的时间间隔*/
} strPPPCfg_apn;

typedef struct ext_netPPPCfg_apn {
    int nDevType;			/**<类型，区别是无线MODEM还是有线MODEM，0表示无线，1表示有线*/
    unsigned int nPPPFlag;	/**<是否支持维持长连接标识，根据需要传入相对应的宏定义*/
    char szApn[64];			/**<APN设置*/
    char szDailNum[32];		/**<拨号号码*/
    int (*ModemDial)(void);/**<拨号函数*/
    unsigned int PPPIntervalTimeOut; /*维持长链接的数据包发送的时间间隔*/
    unsigned char nMinSQVal;	/**<初始化时，允许的最小的信号值*/
    char szPin[31];				/**<SIM卡PIN码*/
    char nPPPHostIP[20];		/**<维持长链接需要PIN的主机IP*/
} strExtPPPCfg_apn;

/**
 *@brief  无线取信号强度状态
*/
typedef enum{
	WLM_SQ_FLUSH_ENABLE=0,		/**<允许刷新状态*/
	WLM_SQ_FLUSH_WORKING,		/**<正在刷新*/
	WLM_SQ_FLUSH_DISABLE,		/**<禁止刷新状态*/
}EM_WLM_SQ_FLUSH;

/**
 *@brief  PPP任务数据结构
*/
typedef struct {
    char ver[8];					/**<参数版本*/
    char cmd;						/**<控制命令*/
    char status;					/**<拨号状态*/
    char devtype;					/**<设备类型*/
    char debug;						/**<调试标志*/
    int errcode;					/**<错误代码*/
    unsigned int linkparam;			/**<PPP链接参数*/
    char ischat;					/**<是否使用chat拨号*/
    char name[47];					/**<用户名*/
    char passwd[32];				/**<密码*/
    char apn[64];					/**<apn*/
    char dailnum[32];				/**<拨号号码*/
    unsigned char min_sq;			/**<拨号时允许的最小信号值*/
    char pin[31];					/**<卡PIN码*/
    char hostip[20];				/**<维持长链接需要PIN的主机IP*/
    unsigned int link_resend_time;	/**<长链接发送间隔时间*/
    unsigned int link_keep_cnt;		/**<长链接已计数的空闲时间*/
    pid_t ppid;						/**<进程号*/
    int rst_flag;					/**<复位标志*/
    char sq_flush_flag;				/**<无线信号刷新标志*/
	unsigned int pppTimeCnt;		/**<ppp时间计数*/
	unsigned int auth_type;			/**<CHAP PAP EAP*/
	unsigned int sq;							/**<当前信号值，99--无信号。0~31信号值*/
	unsigned int dail_sim_bind;      /**<dail功能是否绑定sim卡拨号，0未绑定拨号，1绑定拨号*/
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
