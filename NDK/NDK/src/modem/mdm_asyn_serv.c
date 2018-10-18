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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mount.h>

#include "NDK.h"
#include "modem.h"
#include "mdm_comm.h"
#include "mdm_asyn_sev.h"
#include "mdm_cfg.h"
#include "mdm_drv.h"
#include "mdm_debug.h"
#include "mdm_db.h"

extern int g_AuxPrintFlag;
extern ST_MDM_AT_RET_MAP g_stMdmAtRetMap[];
extern ST_MDM_CFG g_stMdmCfg;

static ST_MDM_STATUS s_stMdmAsyns = {
    .rwLock     = PTHREAD_RWLOCK_INITIALIZER,
    .nDialType  = MODEM_DIAL_TYPE_ASYN,
    .nStatus    = MDM_ASYNS_CLOSED,
    .nCtrl      = MDM_ASYNC_INIT,
    .nErr       = MDM_OK,
};
static pthread_t s_nMdmAsynPThreadID = 0;
static char s_stAsynMdmPortBuff[MAXLEN_PORTBUF];        //串口收发缓冲区
static int s_nAsynMdmPortBufLenf;

static int mdm_asyn_hangup()
{
    int ret = -1;

    if ((ret = mdm_drv_dtrhungup()) != MDM_OK)
        return ret;
    switch (s_stMdmAsyns.nStatus) {
        case MDMSTATUS_MS_NODIALTONE:
        case MDMSTATUS_MS_NOCARRIER:
        case MDMSTATUS_MS_BUSY:
        case MDMSTATUS_MS_NOANSWER:
            break;

        default:
            mdm_port_at_cmd_process(NULL, NULL, 0, 3000); //lsi:016300:1.77s  #2 2.58s;conexant:016300: 0.95s #2:0.75s
            break;
    }
    return MDM_OK;
}

static void mdm_get_asyn_status(void)
{
    char *p, *q, *s;
    char resbuf[1024] = { 0 };
    int offset;
    ST_MDM_AT_RET_MAP *pstAtRet;

    pstAtRet = g_stMdmAtRetMap;
    if (s_nAsynMdmPortBufLenf == 0)
        return;
    while (pstAtRet->respone != NULL) {
        if ((p = strstr(s_stAsynMdmPortBuff, pstAtRet->respone)) != NULL) {
            if ((q = strstr(p, ATSTREND)) == NULL)
                return;
            pthread_rwlock_wrlock(&s_stMdmAsyns.rwLock);
            switch (pstAtRet->ret) {
                case MDM_AT_RET_CONNECT:
                    mdmprint("CONNECTTED\n\r");
                    g_AuxPrintFlag = 1;
                    if (g_stMdmCfg.nAsynProtocolFlag == 0) {
                        s_stMdmAsyns.nStatus = MDM_ASYNS_CONNECT;
                    } else if (g_stMdmCfg.nAsynProtocolFlag == 1) {
                        if ((s = strstr(s_stAsynMdmPortBuff, "+ER:")) != NULL)
                            memcpy(resbuf, s, p - s);
                        if (strstr(resbuf, "NONE") != NULL) {
                            mdm_asyn_hangup();
                            mdmprint("NO PROTOCOL \n\r");
                            s_stMdmAsyns.nStatus = MDMSTATUS_MS_NOERRPROTOCOL;
                        } else {
                            s_stMdmAsyns.nStatus = MDM_ASYNS_CONNECT;
                        }
                    }
                    break;

                case MDM_AT_RET_NO_DIALTONE:
                    s_stMdmAsyns.nStatus = MDMSTATUS_MS_NODIALTONE;
                    break;

                case MDM_AT_RET_NO_CARRIER:
                    s_stMdmAsyns.nStatus = MDMSTATUS_MS_NOCARRIER;
                    break;

                case MDM_AT_RET_BUSY:
                    s_stMdmAsyns.nStatus = MDMSTATUS_MS_BUSY;
                    break;

                case MDM_AT_RET_NO_ANSWER:
                    s_stMdmAsyns.nStatus = MDMSTATUS_MS_NOANSWER;
                    break;

                case MDM_AT_RET_RING:
                    s_stMdmAsyns.nStatus = MDMSTATUS_MS_RING;
                    break;

                case MDM_AT_RET_ERROR:
                    s_stMdmAsyns.nStatus = MDMSTATUS_MS_ERROR;
                    break;

                default:
                    break;
            }
            pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
            /**需要对读取到的数据删除**/
            offset = q + strlen(ATSTREND) - s_stAsynMdmPortBuff;
            memmove(s_stAsynMdmPortBuff, s_stAsynMdmPortBuff + offset, s_nAsynMdmPortBufLenf - offset);
            memset(s_stAsynMdmPortBuff + s_nAsynMdmPortBufLenf - offset, 0, offset);
            s_nAsynMdmPortBufLenf -= offset;
            break;
        }
        pstAtRet++;
    }
}
static int mdm_asyn_serv(void)
{
    int ret;

    /************读取串口数据****************/
    if ((ret = mdm_port_get_line(s_stAsynMdmPortBuff + s_nAsynMdmPortBufLenf, MAXLEN_PORTBUF - s_nAsynMdmPortBufLenf, 0)) > 0)
        s_nAsynMdmPortBufLenf += ret;

    /************分析串口数据****************/
    switch (s_stMdmAsyns.nStatus) {
        case MDM_ASYNS_DIALED:
            mdm_get_asyn_status();
            break;

        case MDM_ASYNS_CONNECT:
            break;

        default:
            return MDM_ERR_ASYN_STATUS;
    }
    return MDM_OK;
}
static void *asyn_pthread(void *args)
{
    int cmd, ret;
    static int s_nNewDialFlag = 0;
    char ATCMD[MAXLEN_DIALNUM + 5] = { 0 };

    mdmprint("ASYN PTHREAD START\n\r");
    while (1) {
        pthread_rwlock_rdlock(&s_stMdmAsyns.rwLock);
        cmd = s_stMdmAsyns.nCtrl;
        pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);

        switch (cmd) {
            case MDM_ASYNC_INIT:
                mdmprint("ASYN PTHREAD INITING\n\r");
                pthread_rwlock_wrlock(&s_stMdmAsyns.rwLock);
                memset(s_stAsynMdmPortBuff, 0, sizeof(s_stAsynMdmPortBuff)); //串口收发缓冲区
                s_nAsynMdmPortBufLenf = 0;
                s_stMdmAsyns.nStatus = MDM_ASYNS_INITED;
                s_stMdmAsyns.nCtrl = MDM_ASYNC_DONE;
                s_stMdmAsyns.nErr = MDM_OK;
                pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
                mdmprint("ASYN PTHREAD INITED\n\r");
                break;

            case MDM_ASYNC_DIAL:
                mdmprint("ASYN PTHREAD DIALING\n\r");
                pthread_rwlock_rdlock(&s_stMdmAsyns.rwLock);
                sprintf(ATCMD, "ATDT%s\r", s_stMdmAsyns.szDialNum);
                mdm_port_at_cmd_process(ATCMD, NULL, 0, 0);
                pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
                pthread_rwlock_wrlock(&s_stMdmAsyns.rwLock);
                s_stMdmAsyns.nStatus = MDM_ASYNS_DIALED;
                s_stMdmAsyns.nCtrl = MDM_ASYNC_DONE;
                s_nNewDialFlag = 1;
                mdm_get_datetime(s_stMdmAsyns.szDialTime);
                pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
                mdmprint("ASYN PTHREAD DIALED\n\r");
                break;
            case MDM_ASYNC_HUANGUP:
                mdmprint("ASYN PTHREAD HUNGUPING\n\r");
                mdm_asyn_hangup();
                pthread_rwlock_wrlock(&s_stMdmAsyns.rwLock);
                s_stMdmAsyns.nStatus = MDM_ASYNS_CLOSED;
                s_stMdmAsyns.nCtrl = MDM_ASYNC_DONE;
                pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
                mdmprint("ASYN PTHREAD HUNGUPED\n\r");
                break;

            default:
                break;
        }

        if ((s_stMdmAsyns.nStatus >= MDM_ASYNS_DIALED) && (s_stMdmAsyns.nStatus < MDM_ASYNS_CONNECT)) {
            ret = mdm_asyn_serv();
            pthread_rwlock_wrlock(&s_stMdmAsyns.rwLock);
            if (s_stMdmAsyns.nErr == MDM_OK)
                s_stMdmAsyns.nErr = ret;
            pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
            if ((s_stMdmAsyns.nErr != MDM_OK) || (s_stMdmAsyns.nStatus == MDM_ASYNS_CONNECT)
                || (s_stMdmAsyns.nStatus < MDMSTATUS_NORETURN_AFTERPREDIAL)) {
                mdm_db_statistics(s_stMdmAsyns, s_nNewDialFlag);
                s_nNewDialFlag = 0;
            }
        }
        usleep(10 * 1000);
    }
}

//初始化异步服务线程
static int mdm_asyn_pthread_init(void)
{
    if (!s_nMdmAsynPThreadID) {
        s_stMdmAsyns.nStatus = MDM_ASYNS_CLOSED;
        s_stMdmAsyns.nCtrl = MDM_ASYNC_INIT;
        s_stMdmAsyns.nErr = MDM_OK;
        pthread_rwlock_init(&s_stMdmAsyns.rwLock, NULL);
        if (pthread_create(&s_nMdmAsynPThreadID, NULL, asyn_pthread, NULL) != 0) {
            s_nMdmAsynPThreadID = 0;
            return MDM_ERR_ASYN_PTHREAD_CREAT;
        }
    }
    return MDM_OK;
}

int mdm_asyn_serv_init(void)
{
    int oldstatus, nowstatus;
    int timeout = 10; //初始化超时定位10S
    long int timeticks;

    pthread_rwlock_rdlock(&s_stMdmAsyns.rwLock);
    oldstatus = s_stMdmAsyns.nStatus;
    pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
    if (oldstatus == MDM_ASYNS_INITED)
        return MDM_OK;
    mdm_asyn_pthread_init();
    pthread_rwlock_wrlock(&s_stMdmAsyns.rwLock);
    s_stMdmAsyns.nCtrl = MDM_ASYNC_INIT;
    pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
    timeticks = mdm_get_time();

    /******************判断是否初始化完成**********************/
    while (1) {
        pthread_rwlock_rdlock(&s_stMdmAsyns.rwLock);
        nowstatus = s_stMdmAsyns.nStatus;
        pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
        if (nowstatus != oldstatus) {
            if (nowstatus == MDM_ASYNS_INITED)
                break;
            else
                return MDM_ERR_ASYNSERV_INIT_FAIL;
        }
        if (mdm_get_time() - timeticks > timeout * TICKS_PERSECOND)
            return MDM_ERR_ASYNSERV_INIT_TIMEOUT;
        usleep(10 * 1000);
    }
    return MDM_OK;
}
int mdm_asyn_serv_dial(const char *dialno)
{
    int oldstatus, nowstatus;
    int timeout = 10; //拨号超时定位10S
    long int timeticks;

    pthread_rwlock_rdlock(&s_stMdmAsyns.rwLock);
    oldstatus = s_stMdmAsyns.nStatus;
    pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
    if (oldstatus == MDM_ASYNS_DIALED)
        return MDM_OK;
    pthread_rwlock_wrlock(&s_stMdmAsyns.rwLock);
    memset(s_stMdmAsyns.szDialNum, 0, sizeof(s_stMdmAsyns.szDialNum));
    memcpy(s_stMdmAsyns.szDialNum, dialno, strlen(dialno));
    s_stMdmAsyns.nCtrl = MDM_ASYNC_DIAL;
    pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
    timeticks = mdm_get_time();
    /******************判断是否拨号完成**********************/
    while (1) {
        pthread_rwlock_rdlock(&s_stMdmAsyns.rwLock);
        nowstatus = s_stMdmAsyns.nStatus;
        pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
        if (nowstatus != oldstatus) {
            if (nowstatus == MDM_ASYNS_DIALED)
                break;
            else
                return MDM_ERR_ASYN_DAIL_FAIL;
        }
        if (mdm_get_time() - timeticks > timeout * TICKS_PERSECOND)
            return MDM_ERR_ASYN_DAIL_TIMEOUT;
        usleep(10 * 1000);
    }
    return MDM_OK;
}

int mdm_asyn_serv_hangup(void)
{
    int oldstatus, nowstatus;
    int timeout = 10; //挂断超时定位10S
    long int timeticks;

    pthread_rwlock_rdlock(&s_stMdmAsyns.rwLock);
    oldstatus = s_stMdmAsyns.nStatus;
    pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
    mdmprint("call %s %d nStatus %d\n\r", __func__, __LINE__, oldstatus);
    if (oldstatus == MDM_ASYNS_CLOSED)
        return MDM_OK;
    pthread_rwlock_wrlock(&s_stMdmAsyns.rwLock);
    s_stMdmAsyns.nCtrl = MDM_ASYNC_HUANGUP;
    pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
    timeticks = mdm_get_time();

    /******************判断是否挂断完成**********************/
    while (1) {
        pthread_rwlock_rdlock(&s_stMdmAsyns.rwLock);
        nowstatus = s_stMdmAsyns.nStatus;
        pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
        if (nowstatus != oldstatus) {
            if (nowstatus == MDM_ASYNS_CLOSED)
                break;
            else
                return MDM_ERR_ASYN_HUNGUP_FAIL;
        }
        if (mdm_get_time() - timeticks > timeout * TICKS_PERSECOND)
            return MDM_ERR_ASYN_HUNGUP_TIMEOUT;
        usleep(10 * 1000);
    }
    return MDM_OK;
}

int mdm_asyn_serv_getstatus(void)
{
    int status;

    pthread_rwlock_rdlock(&s_stMdmAsyns.rwLock);
    status = s_stMdmAsyns.nStatus;
    pthread_rwlock_unlock(&s_stMdmAsyns.rwLock);
    /* 与旧的接口返回值兼容*/
    if ((status == MDM_ASYNS_CLOSED) || (status == MDM_ASYNS_INITED))
        status = MDMSTATUS_NOPREDIAL;
    else if (status == MDM_ASYNS_CONNECT)
        status = MDMSTATUS_CONNECT_AFTERPREDIAL;
    else if (status > MDM_ASYNS_INITED)
        status = MDMSTATUS_NORETURN_AFTERPREDIAL;
    return status;
}
