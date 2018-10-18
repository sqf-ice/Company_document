/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-通讯
 * 作    者：    产品开发部
 * 日    期：    2013-12-10
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <termios.h>
#include <linux/input.h>
#include <signal.h>

#include "devmgr.h"
#include "modem.h"
#include "mdm_comm.h"
#include "mdm_drv.h"
#include "mdm_debug.h"

extern void enablewiremodemport(void);

int g_AuxPrintFlag = 0;
ST_MDM_AT_RET_MAP g_stMdmAtRetMap[] = {
    { MDM_AT_RET_CONNECT,       "CONNECT"        },
    { MDM_AT_RET_RING,      "RING"       },
    { MDM_AT_RET_NO_CARRIER,    "NO CARRIER"     },
    { MDM_AT_RET_ERROR,     "ERROR"      },
    { MDM_AT_RET_NO_DIALTONE,   "NO DIALTONE"    },
    { MDM_AT_RET_BUSY,      "BUSY"       },
    { MDM_AT_RET_NO_ANSWER,     "NO ANSWER"      },
    { MDM_AT_RET_DWN_INIT,      "Download initiated" },
    { MDM_AT_RET_PV_0000000000, "0000000000"     },
    { MDM_AT_RET_PV_F2000E0B02, "F2000E0B02"     },
    { MDM_AT_RET_OK,        "OK"         },
    { MDM_AT_RET_NULL,      NULL         }
};
static int s_nModemAuxFd = -1;

static int mdm_port_open(void)
{
    char szAuxTmp[64];
    char szModemAux[64];
    char *pszTmp;
    int nPortNo;
    struct termios myCfg, *cfg = &myCfg;

    enablewiremodemport();
    if (s_nModemAuxFd >= 0)
        return MDM_OK;
    //mdm_drv_reset(); enablewiremodemport 中已经复位modem。

    //获取modem串口设备名称
    strcpy(szModemAux, MDM_AUX_DEV_NAME);
    if (get_dev_info("mdm", "port", szAuxTmp) == 0) {
        if ((pszTmp = strstr(szAuxTmp, "ttyS")) != NULL) {
            nPortNo = atoi(pszTmp + strlen("ttyS"));
            if ((nPortNo >= 0) && (nPortNo <= 3))
                sprintf(szModemAux, "/dev/ttyS%d", nPortNo);
        }
    }
    s_nModemAuxFd = open(szModemAux, O_RDWR | O_NONBLOCK);
    if (s_nModemAuxFd < 0)
        return MDM_ERR_DEV_OPEN;
    if (tcgetattr(s_nModemAuxFd, cfg) < 0)
        return MDM_ERR_AUX_GET;

    cfmakeraw(cfg); /**<设置终端属性*/
    cfsetispeed(cfg, B115200);
    cfsetospeed(cfg, B115200);

    cfg->c_iflag &= ~IXON;
    cfg->c_iflag &= ~IGNBRK;
    cfg->c_cflag &= ~CSIZE;
    cfg->c_cflag |= CRTSCTS;
    cfg->c_cflag |= CS8;            /**<8个数据位*/
    cfg->c_cflag &= ~CSTOPB;        /**<1个停止位*/
    cfg->c_cflag &= ~PARENB;        /**<无检验*/

    if (tcsetattr(s_nModemAuxFd, TCSANOW, cfg) < 0)
        return MDM_ERR_AUX_SET;

    if (ioctl(s_nModemAuxFd, TCFLSH, 0) < 0)
        return MDM_ERR_AUX_CLR_BUF;

    return MDM_OK;
}

int mdm_port_close(void)
{
    if (s_nModemAuxFd > 0) {
        close(s_nModemAuxFd);
        s_nModemAuxFd = -1;
    }
    return MDM_OK;
}

int mdm_port_clr_buf(void)
{
    int ret;

    if ((ret = mdm_port_open()) != MDM_OK)
        return ret;
    if (ioctl(s_nModemAuxFd, TCFLSH, 0) < 0)
        return MDM_ERR_AUX_CLR_BUF;
    return MDM_OK;
}

int mdm_port_buf_len(int *pnReadlen)
{
    struct timeval tv;
    int ret, nread = 0, nfds = -1;
    fd_set readfds;
    sigset_t newmask, oldmask;

    if (NULL == pnReadlen)
        return MDM_ERR_PARA;
    if ((ret = mdm_port_open()) != MDM_OK)
        return ret;

    tv.tv_sec = 0;
    tv.tv_usec = 10 * 1000;

    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    FD_ZERO(&readfds);
    FD_SET(s_nModemAuxFd, &readfds);
    nfds = select(s_nModemAuxFd + 1, &readfds, NULL, NULL, &tv);
    if (nfds < 0) {
        sigprocmask(SIG_UNBLOCK, &newmask, NULL);
        return MDM_ERR_AUX_BUF_LEN;
    } else if (nfds == 0) {
        *pnReadlen = 0;
    } else if (nfds > 0) {
        if (FD_ISSET(s_nModemAuxFd, &readfds)) {
            if (ioctl(s_nModemAuxFd, FIONREAD, &nread) < 0) {
                sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                return MDM_ERR_AUX_BUF_LEN;
            }
        } else {
            sigprocmask(SIG_UNBLOCK, &newmask, NULL);
            return MDM_ERR_AUX_BUF_LEN;
        }
    }
    *pnReadlen = nread;
    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
    return MDM_OK;
}

int mdm_port_put_string(const char *psSendBuf, int SendLen)
{
    fd_set writefds;
    struct timeval tv;
    int i, ret, sendedlen = 0;
    sigset_t newmask, oldmask;

    if (SendLen <= 0)
        return MDM_ERR_PARA;
    if ((ret = mdm_port_open()) != MDM_OK)
        return ret;

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    while (1) {
        FD_ZERO(&writefds);
        FD_SET(s_nModemAuxFd, &writefds);

        ret = select(s_nModemAuxFd + 1, NULL, &writefds, NULL, &tv);
        if (ret < 0) {
            if (EINTR == errno)
                continue;
            return MDM_ERR_AUX_WRITE;
        } else if (ret == 0) {
            return MDM_ERR_AUX_WRITE_TIMEOUT;
        } else if (ret > 0) {
            if (FD_ISSET(s_nModemAuxFd, &writefds)) {
                sigemptyset(&newmask);
                sigaddset(&newmask, SIGALRM);
                sigprocmask(SIG_BLOCK, &newmask, &oldmask);
                if ((ret = write(s_nModemAuxFd, psSendBuf + sendedlen, SendLen - sendedlen)) != SendLen - sendedlen) {
                    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                    if (ret > 0) {
                        sendedlen += ret;
                        continue;
                    }
                    return MDM_ERR_AUX_WRITE_LEN;
                }
                sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                break;
            } else {
                return MDM_ERR_AUX_WRITE;
            }
        }
    }
    mdmprint("--> mdmwrt[%d]:", SendLen);
    for (i = 0; i < SendLen; i++) {
        if (g_AuxPrintFlag)
            mdmdataprintf("%02X ", psSendBuf[i]);
        else
            mdmdataprintf("%c", psSendBuf[i]);
    }
    mdmdataprintf("\r\n");
    return MDM_OK;
}

static int mdm_port_get_char(char *c, unsigned int *timeout)
{
    int ret;
    struct timeval tv;
    fd_set readfds;
    sigset_t newmask, oldmask;

    assert(c != NULL);

    tv.tv_sec = *timeout / 1000;
    tv.tv_usec = *timeout % 1000 * 950;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(s_nModemAuxFd, &readfds);

        ret = select(s_nModemAuxFd + 1, &readfds, NULL, NULL, &tv);
        *timeout = tv.tv_sec * 1000;
        *timeout += tv.tv_usec / 950;
        if (ret < 0) {
            if (errno == EINTR)
                continue;
            return MDM_ERR_AUX_READ;
        } else if (ret == 0) {
            return MDM_ERR_AUX_READ_TIMEOUT;
        } else if (ret > 0) {
            if (FD_ISSET(s_nModemAuxFd, &readfds)) {
                sigemptyset(&newmask);
                sigaddset(&newmask, SIGALRM);
                sigprocmask(SIG_BLOCK, &newmask, &oldmask);
                if (read(s_nModemAuxFd, c, 1) != 1) {
                    mdmprint("call %s %d \n\r", __func__, __LINE__);
                    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                    return MDM_ERR_AUX_READ_LEN;
                }
                sigprocmask(SIG_UNBLOCK, &newmask, NULL);
            } else {
                return MDM_ERR_AUX_READ;
            }
            break;
        }
    }
    return MDM_OK;
}

//用于读取命令数据，会在psData后加'\0'
int mdm_port_get_line(char *pszData, unsigned int nMaxLen, unsigned int nTimeOutMs)
{
    char *pszTmp = pszData;
    char c;
    unsigned int readlen = 0;
    int i, ret;

    assert(pszData);

    if ((ret = mdm_port_open()) != MDM_OK)
        return ret;

    while ((readlen = (pszTmp - pszData)) < nMaxLen) {
        if ((ret = mdm_port_get_char(&c, &nTimeOutMs)) != MDM_OK) {
            if (!nTimeOutMs) {
                if (readlen == 0)
                    return ret;
                else
                    break;
            } else {
                continue;
            }
        }
        *pszTmp++ = c;
    }
    if (readlen < nMaxLen)
        *pszTmp = '\0';
    mdmprint("<-- mdmred[%d]:", readlen);
    for (i = 0; i < readlen; i++) {
        if (g_AuxPrintFlag)
            mdmdataprintf("%02X ", pszData[i]);
        else
            mdmdataprintf("%c", pszData[i]);
    }
    mdmdataprintf("\r\n");
    return readlen;
}

//用于读取异步数据，不会在psData后加'\0'
int mdm_port_read_line(char *pszData, unsigned int *punDatalen, unsigned int nTimeOutMs)
{
#if 0
    int i, ret;
    unsigned int readlen = 0, remainlen = *punDatalen, maxlen = *punDatalen;
    struct timeval tv;
    fd_set readfds;
    *punDatalen = 0;
    sigset_t newmask, oldmask;

    assert(pszData != NULL);
    tv.tv_sec = nTimeOutMs / 1000;
    tv.tv_usec = nTimeOutMs % 1000 * 1000;
    if ((ret = mdm_port_open()) != MDM_OK)
        return ret;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(s_nModemAuxFd, &readfds);
        ret = select(s_nModemAuxFd + 1, &readfds, NULL, NULL, &tv);
        if (ret < 0) {
            if (errno == EINTR)
                continue;
            sigprocmask(SIG_UNBLOCK, &newmask, NULL);
            return MDM_ERR_AUX_READ;
        } else if (ret == 0) {
            if (readlen == 0) {
                sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                return MDM_ERR_AUX_READ_TIMEOUT;
            } else {
                break;
            }
        } else if (ret > 0) {
            if (FD_ISSET(s_nModemAuxFd, &readfds)) {
                ret = read(s_nModemAuxFd, pszData + readlen, remainlen);
                if (ret <= 0) {
                    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                    return MDM_ERR_AUX_READ_LEN;
                } else {
                    readlen += ret;
                    *punDatalen = readlen;
                    if (readlen == maxlen) {
                        break;
                    } else {
                        remainlen -= (maxlen - readlen);
                        continue;
                    }
                }
            } else {
                sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                return MDM_ERR_AUX_READ;
            }
            break;
        }
    }
    mdmprint("mdmred[%d]:", readlen);
    for (i = 0; i < readlen; i++) {
        if (g_AuxPrintFlag)
            mdmdataprintf("%02x ", pszData[i]);
        else
            mdmdataprintf("%c", pszData[i]);
    }
    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
    return MDM_OK;
#else
    char *pszTmp = pszData;
    char c;
    unsigned int readlen = 0, nMaxLen = *punDatalen;;
    int i, ret;

    assert(pszData);

    if ((ret = mdm_port_open()) != MDM_OK)
        return ret;

    while ((readlen = (pszTmp - pszData)) < nMaxLen) {
        if ((ret = mdm_port_get_char(&c, &nTimeOutMs)) != MDM_OK) {
            if (!nTimeOutMs) {
                if (readlen == 0)
                    return ret;
                else
                    break;
            } else {
                continue;
            }
        }
        *pszTmp++ = c;
    }

    mdmprint("<-- mdmred[%d]:", readlen);
    for (i = 0; i < readlen; i++) {
        if (g_AuxPrintFlag)
            mdmdataprintf("%02X ", pszData[i]);
        else
            mdmdataprintf("%c", pszData[i]);
    }
    mdmdataprintf("\r\n");
    *punDatalen = readlen;
    return MDM_OK;
#endif
}


/*
** nTimeOutMs 以1ms为一个单位，但是必须是10ms的倍数
*/
int mdm_port_at_cmd_process(const char *pszCmd, char *pszRespone, int nMaxLen, int nTimeOutMs)
{
    char *pszTmp;
    char *pszStart;
    int ret, nLimitLen;
    ST_MDM_AT_RET_MAP *pstAtRet;
    char szTmp[2048];
    long int timeticks;

    if ((ret = mdm_port_open()) != MDM_OK)
        return ret;
    if (pszCmd != NULL) {
        if ((ret = mdm_port_clr_buf()) != MDM_OK)
            return ret;
        if ((ret = mdm_port_put_string(pszCmd, strlen(pszCmd))) != MDM_OK)
            return ret;
    }
    if (nTimeOutMs <= 0)
        return MDM_AT_RET_NULL;
    if (pszRespone == NULL) {
        pszTmp = szTmp;
        nLimitLen = sizeof(szTmp);
    } else {
        pszTmp = pszRespone;
        nLimitLen = nMaxLen;
    }
    *pszTmp = '\0';
    pszStart = pszTmp;
    timeticks = mdm_get_time();
    while (pszTmp < (pszStart + nLimitLen)) {
        while (1) {
            ret = mdm_port_get_line(pszTmp, nLimitLen - (pszTmp - pszStart), 10);
            if (ret > 0) {
                break;
            } else if (mdm_get_time() - timeticks > nTimeOutMs * TICKS_PERSECOND / 1000) {
                mdmprint("call %s %d timeout nTimeOutMs %d ret %d\n\r", __func__, __LINE__, nTimeOutMs, ret);
                return ret;
            }
        }
        pstAtRet = g_stMdmAtRetMap;
        while (pstAtRet->respone != NULL) {
            if (strstr(pszStart, pstAtRet->respone) != NULL)
                return pstAtRet->ret;
            pstAtRet++;
        }
        pszTmp += ret;
    }
    return MDM_ERR_ATCMD_RESP;
}
