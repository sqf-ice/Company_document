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

#ifndef MDM_ASYN_SERV_INCLUDE_H
#define MDM_ASYN_SERV_INCLUDE_H

typedef enum {
    MDM_ASYNS_CLOSED = 0,   //已经挂断
    MDM_ASYNS_INITED,       //已初始化
    MDM_ASYNS_DIALED,       //已拨号
    MDM_ASYNS_CONNECT,      //已经接收到CONNEC
} EM_MDM_ASYNS;

typedef enum {
    MDM_ASYNC_DONE,         //任务已经执行
    MDM_ASYNC_INIT,         //初始化
    MDM_ASYNC_DIAL,         //开始ASYN同步拨号
    MDM_ASYNC_HUANGUP,      //挂断ASYN同步
} EM_MDM_ASYNC;

int mdm_asyn_serv_init(void);
int mdm_asyn_serv_dial(const char *dialno);
int mdm_asyn_serv_hangup(void);
int mdm_asyn_serv_getstatus(void);
#endif
