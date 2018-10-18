/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：    产品开发部
* 日    期：    2012-08-17
* 版    本：    V1.00
* 最后修改人：
* 最后修改日期：
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/shm.h>

#include "ppp.h"
#include "../wirelessmodem/wirelessmodem.h"
#include "../public/delay.h"
#include "../public/config.h"
#include "../modem/modem.h"

#define read_lock()     setlock(F_RDLCK)
#define write_lock()    setlock(F_WRLCK)
#define map_unlock()    setlock(F_UNLCK)

/**<内部变量*/
static int lock_fd=-1;
static pppTaskData *p_pppTask_map=(pppTaskData *)0;
static char m_szUsrName[64]="";
static char m_szUsrPasswd[64]="";

/**
*@fn        int setlock(void)
*@brief     锁操作
*@param     type 操作类型
*@return    无
*@section   history     修改历史
            \<author\>  \<time\>    \<desc\>
*/
static int setlock(int type)
{
    struct flock lock;
    char szLineBuf[128];

    if (access(PPP_CFG_PATH, F_OK)<0) {
        mkdir(PPP_CFG_PATH,0755);
    }

    if (lock_fd<0) {
        lock_fd = open(PPP_FILE_LOCK, O_RDWR|O_CREAT, 0666);
        if (lock_fd<0) {
            TRACE_PPP("open ppp_lock err!%s", strerror(errno));
            return -1;
        }
    }

    /*Describe   the   lock   we   want*/
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 1;/*Lock   a   single   byte*/
    lock.l_type   =   type;

    while(1) {
        if ((fcntl(lock_fd, F_SETLKW,&lock))==-1) {
            if (errno == EINTR) {
                continue;
            }
            TRACE_PPP("fcntl err!%s", strerror(errno));
            return -1;
        }

        return 0;
    }
}


/**
*@fn        int init_shm(void)
*@brief     初始化共享内存
*@param     无
*@return    无
*@section   history     修改历史
            \<author\>  \<time\>    \<desc\>
*/
static int init_shm(void)
{
    int shm_id;
    key_t key;
    int fd;

    if (p_pppTask_map!=(pppTaskData *)0) {
        return 0;
    }

    if (access(PPP_SHM_NAME, F_OK)<0) {
        fd = open(PPP_SHM_NAME, O_RDWR|O_CREAT, 0666);
        if (fd<0) {
            TRACE_PPP("open PPP_SHM_NAME err!%s", strerror(errno));
            return -1;
        }
        close(fd);
    }

    if ((key=ftok(PPP_SHM_NAME,0))==-1) {
        TRACE_PPP("ftok error.%s", strerror(errno));
        return -1;
    }

    if ((shm_id=shmget(key, 4096, IPC_CREAT|0666))==-1) {
        TRACE_PPP("shmget err!%s", strerror(errno));
        return -1;
    }

    if ((p_pppTask_map = (pppTaskData *)shmat(shm_id,NULL,0))==(pppTaskData *)0) {
        TRACE_PPP("shmat err!%s", strerror(errno));
        return -1;
    }

    return 0;
}


/**
*@fn        int allow_sq_flush(void)
*@brief     允许信号刷新
*@param     无
*@return    @li     成功
*@section   history     修改历史
            \<author\>  \<time\>    \<desc\>
*/
int ndk_allow_sq_flush(void)
{
    int i;

    if (init_shm()<0) {
        return NDK_ERR_SHM;
    }

    for (i = 10; i > 0; i--) { //等待pppTask初始化共享内存结束
        read_lock();
        if (!strcmp(p_pppTask_map->ver, PPP_TASK_SHM_VER)) {
            map_unlock();
            break;
        }
        map_unlock();
        ndk_msdelay(100);
    }

    if (i==0) {
        return -1;
    }

    write_lock();
    p_pppTask_map->sq_flush_flag = WLM_SQ_FLUSH_ENABLE;
    map_unlock();
    return 0;
}


/**
*@fn        ndk_NET_PPPGetStatus(int *)
*@brief     获取PPP状态
*@param     pemStatus PPP状态 EM_PPP_STATUS
*@return    @li NDK_OK 成功
*@section   history     修改历史
            \<author\>  \<time\>    \<desc\>
*/
int ndk_NET_PPPGetStatus(EM_PPP_STATUS *pemStatus)
{

    if (read_lock()<0) {
        return NDK_ERR_SHM;
    }

    if (init_shm()<0) {
        map_unlock();
        return NDK_ERR_SHM;
    }

    if (strcmp(p_pppTask_map->ver, PPP_TASK_SHM_VER)) {//共享内存还未初始化
        TRACE_PPP("shm no init!");
        *pemStatus = PPP_STATUS_DISCONNECT;
    } else {
        if (p_pppTask_map->cmd==PPP_CMD_NOP) {//命令已执行,返回实际状态
            *pemStatus = p_pppTask_map->status;
        } else if (p_pppTask_map->cmd==PPP_CMD_DAIL) {//拨号命令未执行
            TRACE_PPP("not exec dail.");
            *pemStatus = PPP_STATUS_CONNECTING;
        } else if (p_pppTask_map->cmd==PPP_CMD_HANGUP) {//挂断命令未执行
            TRACE_PPP("not exec hangup.");
            *pemStatus = PPP_STATUS_DISCONNECTING;
        } else {
            map_unlock();
            return NDK_ERR_PARA;
        }
    }

    if (map_unlock()<0) {
        return NDK_ERR_SHM;
    }

    return NDK_OK;
}


int ndk_write_rst_flag(void)
{
    write_lock();

    if (init_shm()<0) {
        map_unlock();
        return NDK_ERR_SHM;
    }

    if (!strcmp(p_pppTask_map->ver, PPP_TASK_SHM_VER)) {
        p_pppTask_map->rst_flag=1;
    }

    map_unlock();
    return 0;
}


/**
*@fn        int stop_sq_flush(void)
*@brief     停止信号刷新
*@param     无
*@return    @li     成功
*@section   history     修改历史
            \<author\>  \<time\>    \<desc\>
*/
int ndk_stop_sq_flush(void)
{
    int i;

    if (init_shm()<0) {
        return NDK_ERR_SHM;
    }

    for (i = 10; i > 0; i--) { //等待pppTask初始化共享内存结束
        read_lock();
        if (!strcmp(p_pppTask_map->ver, PPP_TASK_SHM_VER)) {
            map_unlock();
            break;
        }
        map_unlock();
        ndk_msdelay(100);
    }

    if (i==0) {
        return -1;
    }

    write_lock();
    if (p_pppTask_map->sq_flush_flag==WLM_SQ_FLUSH_DISABLE) { //允许刷新状态，需等待后台刷新任务应答
        map_unlock();
        return 0;
    }
    map_unlock();

    for (i = 10; i > 0; i--) { //等待后台刷新进程应答
        write_lock();
        if (p_pppTask_map->sq_flush_flag!=WLM_SQ_FLUSH_WORKING) {
            p_pppTask_map->sq_flush_flag=WLM_SQ_FLUSH_DISABLE;
            map_unlock();
            return 0;
        }
        map_unlock();
        ndk_msdelay(100);
    }
    return -1;
}


/**
*@fn        NET_PPPSetParam(int ,const void *)
*@brief     设置PPP参数
*@param     nParamType 参数类型
*@param     pPPPParam ppp参数结构
*@return    @li NDK_OK              成功
            @li NDK_ERR_PPP_PARAM       参数错误
            @li NET_OPEN_FILE_ERR   打开配置文件错误
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_NET_PPPSetParam(const void *pInParam, int nValidLen)
{
    TRACE_PPP("set param start!");

    if (write_lock()<0) {   //写锁定
        TRACE_PPP("set param write_lock err!");
        return NDK_ERR_SHM;
    }

    if (init_shm()<0) {
        TRACE_PPP("set param init shm err!");
        map_unlock();
        return NDK_ERR_SHM;
    }

    p_pppTask_map->debug = 0;
    if ((nValidLen==sizeof(strPPPCfg)) || (nValidLen==0)) { //0是为了兼容旧应用
        strPPPCfg *p_cfg=(strPPPCfg *)pInParam;
        p_pppTask_map->devtype = p_cfg->nDevType;
        if (p_cfg->PPPIntervalTimeOut>30) p_pppTask_map->link_resend_time = p_cfg->PPPIntervalTimeOut;
        p_pppTask_map->linkparam = p_cfg->nPPPFlag;
        if (p_cfg->szApn[0]!=0)  strcpy(p_pppTask_map->apn, p_cfg->szApn);
        if (p_cfg->szDailNum[0]!=0) strcpy(p_pppTask_map->dailnum, p_cfg->szDailNum);
    } else if (nValidLen==sizeof(strPPPCfg_apn)) {
        strPPPCfg_apn *p_cfg=(strPPPCfg_apn *)pInParam;
        p_pppTask_map->devtype = p_cfg->nDevType;
        if (p_cfg->PPPIntervalTimeOut>30) p_pppTask_map->link_resend_time = p_cfg->PPPIntervalTimeOut;
        p_pppTask_map->linkparam = p_cfg->nPPPFlag;
        if (p_cfg->szApn[0]!=0)  strcpy(p_pppTask_map->apn, p_cfg->szApn);
        if (p_cfg->szDailNum[0]!=0) strcpy(p_pppTask_map->dailnum, p_cfg->szDailNum);
    } else if (nValidLen==sizeof(ST_PPP_CFG)) {
        ST_PPP_CFG *p_extcfg=(ST_PPP_CFG *)pInParam;
        p_pppTask_map->devtype = p_extcfg->nDevType;
        p_pppTask_map->linkparam = p_extcfg->nPPPFlag;
        if (p_extcfg->PPPIntervalTimeOut>30) p_pppTask_map->link_resend_time = p_extcfg->PPPIntervalTimeOut;
        if (p_extcfg->szApn[0]!=0)  strcpy(p_pppTask_map->apn, p_extcfg->szApn);
        if (p_extcfg->szDailNum[0]!=0) strcpy(p_pppTask_map->dailnum, p_extcfg->szDailNum);
        if (p_extcfg->nMinSQVal>8) p_pppTask_map->min_sq = p_extcfg->nMinSQVal;
        if (p_extcfg->szPin[0]!=0) strcpy(p_pppTask_map->pin, p_extcfg->szPin);
        if (p_extcfg->nPPPHostIP[0]!=0) strcpy(p_pppTask_map->hostip, p_extcfg->nPPPHostIP);
    } else if (nValidLen==sizeof(strExtPPPCfg_apn)) {
        strExtPPPCfg_apn *p_extcfg=(strExtPPPCfg_apn *)pInParam;
        p_pppTask_map->devtype = p_extcfg->nDevType;
        p_pppTask_map->linkparam = p_extcfg->nPPPFlag;
        if (p_extcfg->PPPIntervalTimeOut>30) p_pppTask_map->link_resend_time = p_extcfg->PPPIntervalTimeOut;
        if (p_extcfg->szApn[0]!=0)  strcpy(p_pppTask_map->apn, p_extcfg->szApn);
        if (p_extcfg->szDailNum[0]!=0) strcpy(p_pppTask_map->dailnum, p_extcfg->szDailNum);
        if (p_extcfg->nMinSQVal>8) p_pppTask_map->min_sq = p_extcfg->nMinSQVal;
        if (p_extcfg->szPin[0]!=0) strcpy(p_pppTask_map->pin, p_extcfg->szPin);
        if (p_extcfg->nPPPHostIP[0]!=0) strcpy(p_pppTask_map->hostip, p_extcfg->nPPPHostIP);
    } else {
        TRACE_PPP("set param err!");
        map_unlock();
        return NDK_ERR_PPP_PARAM;
    }

    TRACE_PPP("set param succ.");
    map_unlock();
    return NDK_OK;
}

int ndk_read_rst_flag(void)
{
    int flag=1;

    read_lock();

    if (init_shm()>=0) {
        if (!strcmp(p_pppTask_map->ver, PPP_TASK_SHM_VER)) { //共享内存已初始化
            flag = p_pppTask_map->rst_flag;
        }
    }

    map_unlock();
    return flag;
}

/**
*@fn        NET_PPPOpen(int)
*@brief     打开PPP
*@param     无
*@return    @li NDK_OK      成功
            @li NET_ERROR   错误
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_NET_PPPOpen(int nIsUseChat,int nIsSimBind)
{
    EM_PPP_STATUS status;

    TRACE_PPP("call ppp open.");

    if (ndk_NET_PPPGetStatus(&status)==NDK_OK) {
        if ((status==PPP_STATUS_CONNECTING) || (status==PPP_STATUS_CONNECTED)) {
            TRACE_PPP("ppp opened!");
            return NDK_ERR_PPP_OPEN;
        }
    }

    if (write_lock()<0) {
        return NDK_ERR_SHM;
    }

    if (init_shm()<0) {
        map_unlock();
        return NDK_ERR_SHM;
    }


    if (!strcmp(p_pppTask_map->ver, PPP_TASK_SHM_VER)) {
        unsigned int sq;

        //信号值限制以设置为准，如果无设置则使用缺省值，不采用应用传入的值。ym 2011-07-13
        if (ndk_getconfig("wls","sq_limit", CFG_INT, &sq)<0) {
            sq = WLM_SQ_LIMIT_DEFAULT;
        }

        //避免MDM影响,在无线拨号时，停用modem
        if (p_pppTask_map->devtype == 0)////0表示 无线,1为有线
            ndk_mdmsleep();
        if(p_pppTask_map->devtype == 0) {
			map_unlock();
            enablewirelessmodemport();
			write_lock();
        } else {
			map_unlock();
            enablewiremodemport();
			write_lock();
        }
        p_pppTask_map->min_sq = sq;
        p_pppTask_map->cmd = PPP_CMD_DAIL;
        if (p_pppTask_map->devtype==1) { //有线modem设备，由应用程序自身进行拨号处理。
            p_pppTask_map->ischat = 0;
        } else {
            p_pppTask_map->ischat = nIsUseChat;
        }
        strcpy(p_pppTask_map->name, m_szUsrName);
        strcpy(p_pppTask_map->passwd, m_szUsrPasswd);
        p_pppTask_map->dail_sim_bind = nIsSimBind;
        map_unlock();
        return NDK_OK;
    }

    map_unlock();
    return NDK_ERR_PPP_DEVICE;
}

/**
*@fn        NET_PPPClose()
*@brief     关闭PPP
*@param     无
*@return    @li NDK_OK 成功
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_NET_PPPClose(void)
{
    TRACE_PPP("call ppp close.");
    if (write_lock()<0) {
        TRACE_PPP("ppp close return NDK_ERR_SHM.");
        return NDK_ERR_SHM;
    }

    if (init_shm()<0) {
        map_unlock();
        TRACE_PPP("ppp close return init_shm.");
        return NDK_ERR_SHM;
    }

    if (!strcmp(p_pppTask_map->ver, PPP_TASK_SHM_VER)) {
        p_pppTask_map->cmd = PPP_CMD_HANGUP;
    }

    map_unlock();
    TRACE_PPP("ppp close return succ.");
    return NDK_OK;
}


/**
*@fn        getPPPerrorcode(int)
*@brief     获取错误码
*@param     fd 句柄
*@return    0   成功
            <0  错误
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
int ndk_getPPPerrorcode(int ph)
{
    int ret;

    if (read_lock()<0) {
        return NDK_ERR_SHM;
    }

    if (strcmp(p_pppTask_map->ver, PPP_TASK_SHM_VER)) {//共享内存还未初始化
        ret = 0;
    } else {
        ret = p_pppTask_map->errcode;
    }

    map_unlock();
    return ret;
}


/**
*@fn        netSetLogin(const char *, const char *)
*@brief     设置用户名密码
*@param     luser 用户名
*@param     lpassword 密码
*@return    0   成功
            <0  错误
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
void ndk_netSetLogin(const char *luser, const char *lpassword)
{
    if (luser!=NULL) {
        strncpy(m_szUsrName, luser, sizeof(m_szUsrName)-1);
        m_szUsrName[sizeof(m_szUsrName)-1] = '\0';
    }

    if (lpassword!=NULL) {
        strncpy(m_szUsrPasswd, lpassword, sizeof(m_szUsrPasswd)-1);
        m_szUsrPasswd[sizeof(m_szUsrPasswd)-1] = '\0';
    }
    return ;
}




/**
*@fn        getPPP0Addr(int)
*@brief     获取PPP0的地址信息
*@param     szAddrInfo 地址字符串信息
*@return    网络地址
*@section   history     修改历史
                \<author\>  \<time\>    \<desc\>
*/
unsigned long ndk_getPPP0Addr(char *szAddrInfo)
{
    FILE *fd;
    char szLineBuf[256];
    char cPPP0Flag=0;
    unsigned long ulAddr=0;
    char *pszStart, *pszEnd;

    //使用ifconfig命令获得网络配置
    if ((fd=popen("ifconfig", "r")) != NULL) {
        while (fgets(szLineBuf, sizeof(szLineBuf)-1, fd)!=NULL) {
            if (!cPPP0Flag) {
                if (strstr(szLineBuf, "ppp0")!=NULL) {  //查找PPP0的配置
                    cPPP0Flag = 1;
                }
            } else {
                if ((pszStart=strstr(szLineBuf, szAddrInfo))!=NULL) {
                    pszStart += strlen(szAddrInfo);
                    if ((pszEnd=strchr(pszStart, ' '))!=NULL) {
                        *pszEnd = '\0';
                    }

                    ulAddr = inet_addr(pszStart);
                    break;
                }
            }
        }
    }

    pclose(fd);
    return ulAddr;
}

