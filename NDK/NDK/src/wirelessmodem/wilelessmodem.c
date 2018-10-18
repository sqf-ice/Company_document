/**
*@file      wlm_atmanage.c
*@brief     AT命令管理
*@version   1.0.0
*@author    yanm
*@date      2008-04-16
*@section   history     修改历史
            \<author\>\<time\>\<version\>\<desc\>
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>

#include "wirelessmodem.h"
#include "wlm_cmdlist.h"
#include "../serial/NDK_Serial.h"
#include "../public/delay.h"
#include "NDK.h"
#include "NDK_debug.h"
#include "../ppp/ppp.h"
#include "../public/config.h"



#define TRACE_AT(fmt, args...)          NDK_COMM_LOG_INFO(NDK_LOG_MODULE_WLM,fmt, ##args)
#define TRACE_HEX_AT(fmt, args...)  NDK_COMM_LOG_INFO(NDK_LOG_MODULE_WLM,fmt, ##args)


#define WLM_DEFAULT_BUF_LEN         1024    ///<缺省的数据缓冲长度
#define WLM_DEFAULT_TIMEOUT         1500    ///<缺省超时时间1500ms
//无线多路复用 linsx add 2012-10-23
#define MUX_TMPLE_FILE "/tmp/gsm_mux"

extern int g_AuxNo;
extern int m_AuxInitFlag;

static int m_nPreRecChar=-1;
static WLMATDrv *s_pWlmAtDrv=NULL;          ///<无线模块对应的at命令驱动
static WLMRESPTABLE *s_pWlmRespTable=NULL;  ///<特殊命令返回处理
static int ndk_GetMuxInfo(void);
static int NET_WLMGetLine(unsigned char *pszDataBuf, unsigned int nMaxLen, unsigned int uimilTimeOut);
static int GetCmdList(void);
static int ndk_WLMInitAux(int nAuxno);

extern int ndk_WLMGetType(void);
extern int ndk_WLMReadStatus(int StatusType);


/**
*@fn        NET_WLMSendAtCmd(EM_WLM_CMD ,const char *, const char *)
*@brief     送AT命令到无线模块
*@param     emAtCmdNo 命令号
*@param     pszAtCmd 当命令号为未定义时，该字串生效
*@param     pszInputParam 命令的参数
*@return    @li NDK_OK              成功
            @li NET_ERROR           失败
*@section   history     修改历史
            \<author\>  \<time\>    \<desc\>
*/
int ndk_NET_WLMSendAtCmd(EM_WLM_CMD emAtCmdNo, const char *pszAtCmd, const char *pszInputParam)
{
    char szSendCmdBuf[128+2];
    EM_WLM_CMD nCmdNo;              //实际命令编号
    WLMATCMDMAP *pWlmPubAtCmdMap;       //公用命令映射表
    WLMATCMDMAP *pWlmAtCmdMap;          //模块实际命令映射表
    WLMCMDRESPTABLE *pWlmCmdResTable;   //模块命令返回特殊处理表
    char *pszTmp;
    if ((pszAtCmd!=NULL)?(strlen(pszAtCmd)>=48):0)          /*输入命令串不超过48*/
        return NDK_ERR_PARA;

    if ((pszInputParam!=NULL)?(strlen(pszInputParam)>=64):0)    /*输入参数串不超过64*/
        return NDK_ERR_PARA;
    if(ndk_GetMuxInfo()!=0) {
        NDK_LOG_ERR(NDK_LOG_MODULE_WLM,"get mux state error!");
        return NDK_ERR;
    }
    //获取模块命令列表
    if (GetCmdList() != 0) {
        NDK_LOG_ERR(NDK_LOG_MODULE_WLM,"get cmd list error!");
        return NDK_ERR;
    }
    nCmdNo = emAtCmdNo;
    pWlmAtCmdMap = s_pWlmAtDrv->pWlmAtCmdMap;
    pWlmCmdResTable = s_pWlmAtDrv->pWlmRespTable;
    s_pWlmRespTable = NULL;

    /*获取命令号，当emAtCmdNo==WM_UNDEFINE查询输入数据是否在公共列表中*/
    if (WLM_CMD_UNDEFINE==nCmdNo) {
        if (NULL==pszAtCmd)         /*输入的命令串为空指针*/
            return NDK_ERR_PARA;

        /*查找输入命令是否在公共串内*/
        pWlmPubAtCmdMap = (WLMATCMDMAP *)&Public_cmd_List;
        while (NULL!=pWlmPubAtCmdMap->pszAtCmdStr) {
            if (!strcasecmp(pszAtCmd, pWlmPubAtCmdMap->pszAtCmdStr)) {
                nCmdNo = pWlmPubAtCmdMap->WLMAtCmdNo;
                break;
            }
            pWlmPubAtCmdMap++;
        }
    } else if ((nCmdNo<0) || (nCmdNo>=WLM_CMD_END)) {
        return NDK_ERR_PARA;
    }

    if (WLM_CMD_UNDEFINE==nCmdNo) {
        pszTmp = (char *)pszAtCmd;
    } else {
        pszTmp = (pWlmAtCmdMap+nCmdNo)->pszAtCmdStr;
        while (pWlmCmdResTable->cmd!=WLM_CMD_UNDEFINE) {//搜寻该命令返回是否有特殊处理
            if (pWlmCmdResTable->cmd==nCmdNo) {//存在转义命令表
                s_pWlmRespTable = pWlmCmdResTable->pRespTable;
            }
            pWlmCmdResTable++;
        }
    }

    if (NULL!=pszInputParam) {
        sprintf(szSendCmdBuf, "AT%s%s\r",pszTmp, pszInputParam);
    } else {
        sprintf(szSendCmdBuf, "AT%s\r",  pszTmp);
    }
    ndk_WLMInitAux(-1);
    m_nPreRecChar = -1;
    NDK_PortClrBuf(g_AuxNo);//modify 2012-9-4
    if(szSendCmdBuf!=NULL)
        NDK_COMM_LOG_INFO(NDK_LOG_MODULE_WLM,"%s succ,send at cmd=%s\n",__func__,szSendCmdBuf);
    //TRACE_HEX_AT("send %s",szSendCmdBuf);
    return NDK_PortWrite(g_AuxNo,strlen(szSendCmdBuf),szSendCmdBuf);//modify 2012-9-4
}

/**
*@fn        NET_WLMGetAtRet(char * ,unsigned int , unsigned int, int)
*@brief     取AT命令返回
*@param     pszOutput 输出数据
*@param     nMaxlen 最大长度
*@param     nMilTimeOut 超时时间
*@param     iLineMode 输出模式
*@return    @li NDK_OK              成功
            @li NET_ERROR           失败
*@section   history     修改历史
            \<author\>  \<time\>    \<desc\>
*/
int ndk_NET_WLMGetAtRet(char *pszOutput, unsigned int nMaxlen, unsigned int nMilTimeOut, int iLineMode)
{
    char *pszTmp;
    WLMRESPTABLE *pWLMRespChange;
    char szTmpBuf[WLM_DEFAULT_BUF_LEN];
    int nRet;
    unsigned int nLimitLen;
    unsigned int len=0;
    unsigned int nLineLen;
    int i;
    int nexitflag;

    //获取模块命令列表
    if (s_pWlmAtDrv==NULL) {
        NDK_LOG_CRIT(NDK_LOG_MODULE_WLM,"get cmd list error!");
        return NDK_ERR;
    }

    //判断模块是否打开
    if (ndk_WLMReadStatus(WLM_STATUS)==0) {
        NDK_LOG_CRIT(NDK_LOG_MODULE_WLM,"WLM is closed!");
        return NDK_ERR;
    }

    if ((NULL==pszOutput)||(0==nMaxlen)) {  /*输出缓冲为空或者缓冲长度为0则使用内部临时缓冲*/
        pszTmp=szTmpBuf;
        nLimitLen=sizeof(szTmpBuf)-1;
    } else {
        pszTmp=pszOutput;
        nLimitLen=nMaxlen-1;
    }

    while (len<nLimitLen) {
        nRet=NET_WLMGetLine((unsigned char *)pszTmp, nLimitLen-len, nMilTimeOut);
        if (nRet<0) {
            return nRet;
        }

        nRet = WLM_STATUS_UNTYPED;
        if (*pszTmp=='\x0d') {
            nRet=NDK_ERR_EMPTY;
        } else {
            pWLMRespChange = s_pWlmRespTable;
            nexitflag =0;
            for (i = 0; i < 2; i++) {//先搜索转义表再搜索公共表
                if (pWLMRespChange!=NULL) {
                    while (pWLMRespChange->pszWlmRespone!=NULL) {
                        if (strstr(pszTmp, pWLMRespChange->pszWlmRespone)) {
                            nRet=pWLMRespChange->WLMRespone;    //???????????????????
                            if (NULL!=pWLMRespChange->pszPubilcRespone) {
                                strcpy(pszTmp, pWLMRespChange->pszPubilcRespone);
                            }
                            nexitflag =1;
                            break;
                        }
                        pWLMRespChange++;
                    }
                }

                if (nexitflag) {//已搜索到,可退出
                    break;
                }

                pWLMRespChange = (WLMRESPTABLE *)&Public_ret_List;
            }
        }

        nLineLen = strlen(pszTmp);
        len += nLineLen;
        pszTmp += nLineLen;

        if (iLineMode || ((nRet!=WLM_STATUS_UNTYPED) && (nRet!=NDK_ERR_EMPTY))) {
            TRACE_AT("ret=%d", nRet);
            TRACE_HEX_AT("buf %s",pszTmp-len);
            if (nRet < 0)
                return NDK_ERR;
            return nRet;
        }
    }

    TRACE_AT("ret=NDK_ERR_OVERFLOW");
    return NDK_ERR_OVERFLOW;
}


/**
*@fn        NET_WLMGetLine(unsigned char *, unsigned int, unsigned int)
*@brief     取行数据
*@param     无
*@return    @li NDK_OK              成功
            @li NET_ERROR           失败
*@section   history     修改历史
            \<author\>  \<time\>    \<desc\>
*/
static int NET_WLMGetLine(unsigned char *pszDataBuf, unsigned int nMaxLen, unsigned int uimilTimeOut)
{
    unsigned char *pszTmp=pszDataBuf;
    unsigned char c;
    unsigned int timeout, tc;
    int readlen = 0;

    if (NULL == pszDataBuf) {
        return NDK_ERR_PARA;
    }

    //ndk_WLMInitAux(-1);
    timeout=(tc=uimilTimeOut/50);       /*超时计数值*/

    while ((pszTmp-pszDataBuf) < nMaxLen) {
        if (m_nPreRecChar!=-1) {
            c = m_nPreRecChar&0xFF;
            m_nPreRecChar = -1;
        } else {
            NDK_PortRead(g_AuxNo,1,(char *)&c,0,&readlen);
            if (readlen!=1) {   //没取到数据或错误
                if (!timeout) {
                    return NDK_ERR_TIMEOUT;
                } else {
                    --timeout;
                    ndk_msdelay(50);
                    continue;
                }
            }
        }

        timeout = tc;

        if ((c>=0x20) || (c=='\t')) {
            *pszTmp++ = c;
        } else if (c=='\x0d') { //接收到行结束附
            *pszTmp++ = c;
            c = 0;
            NDK_PortRead(g_AuxNo,1,(char *)&c,0,&readlen);
            if (readlen!=1) {
                ndk_msdelay(50);
                NDK_PortRead(g_AuxNo,1,(char *)&c,0,&readlen);
                if (readlen!=1) {
                    *pszTmp = '\0';
                    return NDK_OK;
                }
            }

            //此处肯定已取到字符c的值
            if (c=='\x0a') {
                if ((pszTmp-pszDataBuf)<nMaxLen) *pszTmp++ = c;
            } else {
                m_nPreRecChar = c;
            }

            *pszTmp = '\0';
            return NDK_OK;
        }
    }

    *pszTmp = '\0';
    return NDK_ERR_OVERFLOW;
}

/**
*@fn        GetCmdList(void)
*@brief     获取命令表头指针
*@param     无
*@return    @li NULL            失败
            @li 其他            命令列表头指针
*@section   history     修改历史
            \<author\>  \<time\>    \<desc\>
*/
static int GetCmdList(void)
{
    int nType;
    int i;
//   int read_rst_flag(void);
    extern int ndk_InitAux(void);

    if (s_pWlmAtDrv==NULL) {
        nType = ndk_WLMGetType();
        for (i = 0; wlm_at_drv[i].nWLMType != -1; i++) {
            if (wlm_at_drv[i].nWLMType==nType) {
                s_pWlmAtDrv = (WLMATDrv *)&wlm_at_drv[i];
                TRACE_AT("type=%d", nType);
                break;
            }
        }
        if (wlm_at_drv[i].nWLMType==-1) {
            TRACE_AT("no define type=%x", nType);
            return NDK_ERR;
        }
    }

    //判断模块是否打开
    if (ndk_WLMReadStatus(WLM_STATUS)==0) {
        TRACE_AT("WLM is closed!");
        return NDK_ERR;
    }
#if 1
	nType = ndk_WLMGetType();
    if((access(MUX_TMPLE_FILE,F_OK)<0)&& (nType != WLM_WCDMA_H350)&& (nType != WLM_EVDO_DE910_USB)&& (nType != WLM_WCDMA_EHS5_USB)) {
        if (!ndk_read_rst_flag()) {//刚复位的时候不进行DCD拨号
            //判断是否在通讯状态
            for (i = 300; i > 0; i--) {
                if (ndk_WLMReadStatus(WLM_DCDSTATUS)!=0) {
                    TRACE_AT("GetCmdList:cmd status!");
                    break;
                }

                ndk_msdelay(100);
            }

            if (i==0) {
                TRACE_AT("GetCmdList:data comm!");
                return NDK_ERR;
            }
        }
    }
#endif
    //ndk_InitAux();
    return NDK_OK;
}

int ndk_WLMAuxSelect(int nAuxno)
{
    if (access(MUX_TMPLE_FILE,F_OK)<0) { //多路复用非开启状态,使用物理串口
        if(g_AuxNo!=WIRELESS_AUX) { //如果原先不为WIRELESS_AUX，那么重新初始化
            g_AuxNo=WIRELESS_AUX;
            m_AuxInitFlag=0;
        }
    } else {
        if(nAuxno==-1) { //采用默认的/dev/modem1
            if(g_AuxNo!=WIRELESS_MUX1) {
                g_AuxNo=WIRELESS_MUX1;
                m_AuxInitFlag=0;
            }
        } else { //采用参数配置的AUX
            if(g_AuxNo!=nAuxno) {
                g_AuxNo=nAuxno;
                m_AuxInitFlag=0;
            }
        }
    }
}

int ndk_WLMBpSelect(char * szAuxBps)
{
    int wls_bps = 0;
    ndk_getconfig("wls", "bps", CFG_INT, &wls_bps);
    if(wls_bps==WIRELESS_BPS230400) {
        strcpy(szAuxBps,"230400");
    } else {
        strcpy(szAuxBps,"115200");
    }
}

int ndk_WLMInitAux(int nAuxno)
{
    char szAuxAttr[16];
    int ret;
    ndk_WLMAuxSelect(nAuxno);
//    if((g_AuxNo==WIRELESS_MUX1)||(g_AuxNo==WIRELESS_MUX2)) {
//        ndk_InitMuxPtsFile();
//    }
    if ((!m_AuxInitFlag)) {
        //enablewirelessmodemport();//放在SDK初始化代码当中实现
        memset(szAuxAttr,0,sizeof(szAuxAttr));
        //wlm_methodcall_gsmmux(2,&ret);
        ndk_WLMBpSelect(szAuxAttr);
        //为防止多进程同时初始化tty设备时，可能导致进程初始化错误，增加判断串口初始化是否成功 yanm 20101119
        if (NDK_PortOpen(g_AuxNo, szAuxAttr) != NDK_OK) {
            fprintf(stderr, "init wlm aux %d BPS=%s ERR!\r\n", g_AuxNo,szAuxAttr);
            usleep(1000);
            if (NDK_PortOpen(g_AuxNo, szAuxAttr) != NDK_OK) { //初始化错误后再初始化一次，如果仍然错误，则返回 yanm 20101119
                fprintf(stderr, "init wlm aux %d BPS=%s ERR!\r\n", g_AuxNo,szAuxAttr);
                return -1;
            }
        }
        m_AuxInitFlag=1;
        return 0;
    }
    return 0;
}

