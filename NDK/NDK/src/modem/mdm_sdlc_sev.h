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

#ifndef MDM_SDLC_SERV_INCLUDE_H
#define MDM_SDLC_SERV_INCLUDE_H

#define MAXLEN_SDLCDATA 2500    //单帧最大可传输数据大小
#define UpNo(no)  (((no) - 1) % 8)
#define NextNo(no)  (((no) + 1) % 8)
#define AddNo(no)   ((no) = NextNo(no))

typedef enum {
    MDM_SDLCS_CLOSED = 0,   //已经挂断
    MDM_SDLCS_INITED,       //已初始化
    MDM_SDLCS_DIALED,       //已拨号
    MDM_SDLCS_CONNECT,      //已经接收到CONNECT
    MDM_SDLCS_RR,           //在链路连接状态
} EM_MDM_SDLCS;

typedef enum {
    MDM_SDLCC_DONE,         //任务已经执行
    MDM_SDLCC_INIT,         //初始化
    MDM_SDLCC_DIAL,         //开始SDLC同步拨号
    MDM_SDLCC_HUANGUP,      //挂断SDLC同步
} EM_MDM_SDLCC;

typedef enum {
    MDM_SDLC_SEND_NONE = 0, //不发送任何帧
    MDM_SDLC_SEND_UA,       //发送UA帧
    MDM_SDLC_SEND_RR,       //发送RR帧
	MDM_SDLC_SEND_REJ,		//发送REJ帧
    MDM_SDLC_SEND_I,        //发送I帧
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
