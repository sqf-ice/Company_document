/**
*@file      wlm_atmanage.c
*@brief     AT�������
*@version   1.0.0
*@author    yanm
*@date      2008-04-16
*@section   history     �޸���ʷ
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


#define WLM_DEFAULT_BUF_LEN         1024    ///<ȱʡ�����ݻ��峤��
#define WLM_DEFAULT_TIMEOUT         1500    ///<ȱʡ��ʱʱ��1500ms
//���߶�·���� linsx add 2012-10-23
#define MUX_TMPLE_FILE "/tmp/gsm_mux"

extern int g_AuxNo;
extern int m_AuxInitFlag;

static int m_nPreRecChar=-1;
static WLMATDrv *s_pWlmAtDrv=NULL;          ///<����ģ���Ӧ��at��������
static WLMRESPTABLE *s_pWlmRespTable=NULL;  ///<��������ش���
static int ndk_GetMuxInfo(void);
static int NET_WLMGetLine(unsigned char *pszDataBuf, unsigned int nMaxLen, unsigned int uimilTimeOut);
static int GetCmdList(void);
static int ndk_WLMInitAux(int nAuxno);

extern int ndk_WLMGetType(void);
extern int ndk_WLMReadStatus(int StatusType);


/**
*@fn        NET_WLMSendAtCmd(EM_WLM_CMD ,const char *, const char *)
*@brief     ��AT�������ģ��
*@param     emAtCmdNo �����
*@param     pszAtCmd �������Ϊδ����ʱ�����ִ���Ч
*@param     pszInputParam ����Ĳ���
*@return    @li NDK_OK              �ɹ�
            @li NET_ERROR           ʧ��
*@section   history     �޸���ʷ
            \<author\>  \<time\>    \<desc\>
*/
int ndk_NET_WLMSendAtCmd(EM_WLM_CMD emAtCmdNo, const char *pszAtCmd, const char *pszInputParam)
{
    char szSendCmdBuf[128+2];
    EM_WLM_CMD nCmdNo;              //ʵ��������
    WLMATCMDMAP *pWlmPubAtCmdMap;       //��������ӳ���
    WLMATCMDMAP *pWlmAtCmdMap;          //ģ��ʵ������ӳ���
    WLMCMDRESPTABLE *pWlmCmdResTable;   //ģ����������⴦���
    char *pszTmp;
    if ((pszAtCmd!=NULL)?(strlen(pszAtCmd)>=48):0)          /*�������������48*/
        return NDK_ERR_PARA;

    if ((pszInputParam!=NULL)?(strlen(pszInputParam)>=64):0)    /*���������������64*/
        return NDK_ERR_PARA;
    if(ndk_GetMuxInfo()!=0) {
        NDK_LOG_ERR(NDK_LOG_MODULE_WLM,"get mux state error!");
        return NDK_ERR;
    }
    //��ȡģ�������б�
    if (GetCmdList() != 0) {
        NDK_LOG_ERR(NDK_LOG_MODULE_WLM,"get cmd list error!");
        return NDK_ERR;
    }
    nCmdNo = emAtCmdNo;
    pWlmAtCmdMap = s_pWlmAtDrv->pWlmAtCmdMap;
    pWlmCmdResTable = s_pWlmAtDrv->pWlmRespTable;
    s_pWlmRespTable = NULL;

    /*��ȡ����ţ���emAtCmdNo==WM_UNDEFINE��ѯ���������Ƿ��ڹ����б���*/
    if (WLM_CMD_UNDEFINE==nCmdNo) {
        if (NULL==pszAtCmd)         /*��������Ϊ��ָ��*/
            return NDK_ERR_PARA;

        /*�������������Ƿ��ڹ�������*/
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
        while (pWlmCmdResTable->cmd!=WLM_CMD_UNDEFINE) {//��Ѱ��������Ƿ������⴦��
            if (pWlmCmdResTable->cmd==nCmdNo) {//����ת�������
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
*@brief     ȡAT�����
*@param     pszOutput �������
*@param     nMaxlen ��󳤶�
*@param     nMilTimeOut ��ʱʱ��
*@param     iLineMode ���ģʽ
*@return    @li NDK_OK              �ɹ�
            @li NET_ERROR           ʧ��
*@section   history     �޸���ʷ
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

    //��ȡģ�������б�
    if (s_pWlmAtDrv==NULL) {
        NDK_LOG_CRIT(NDK_LOG_MODULE_WLM,"get cmd list error!");
        return NDK_ERR;
    }

    //�ж�ģ���Ƿ��
    if (ndk_WLMReadStatus(WLM_STATUS)==0) {
        NDK_LOG_CRIT(NDK_LOG_MODULE_WLM,"WLM is closed!");
        return NDK_ERR;
    }

    if ((NULL==pszOutput)||(0==nMaxlen)) {  /*�������Ϊ�ջ��߻��峤��Ϊ0��ʹ���ڲ���ʱ����*/
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
            for (i = 0; i < 2; i++) {//������ת���������������
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

                if (nexitflag) {//��������,���˳�
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
*@brief     ȡ������
*@param     ��
*@return    @li NDK_OK              �ɹ�
            @li NET_ERROR           ʧ��
*@section   history     �޸���ʷ
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
    timeout=(tc=uimilTimeOut/50);       /*��ʱ����ֵ*/

    while ((pszTmp-pszDataBuf) < nMaxLen) {
        if (m_nPreRecChar!=-1) {
            c = m_nPreRecChar&0xFF;
            m_nPreRecChar = -1;
        } else {
            NDK_PortRead(g_AuxNo,1,(char *)&c,0,&readlen);
            if (readlen!=1) {   //ûȡ�����ݻ����
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
        } else if (c=='\x0d') { //���յ��н�����
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

            //�˴��϶���ȡ���ַ�c��ֵ
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
*@brief     ��ȡ�����ͷָ��
*@param     ��
*@return    @li NULL            ʧ��
            @li ����            �����б�ͷָ��
*@section   history     �޸���ʷ
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

    //�ж�ģ���Ƿ��
    if (ndk_WLMReadStatus(WLM_STATUS)==0) {
        TRACE_AT("WLM is closed!");
        return NDK_ERR;
    }
#if 1
	nType = ndk_WLMGetType();
    if((access(MUX_TMPLE_FILE,F_OK)<0)&& (nType != WLM_WCDMA_H350)&& (nType != WLM_EVDO_DE910_USB)&& (nType != WLM_WCDMA_EHS5_USB)) {
        if (!ndk_read_rst_flag()) {//�ո�λ��ʱ�򲻽���DCD����
            //�ж��Ƿ���ͨѶ״̬
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
    if (access(MUX_TMPLE_FILE,F_OK)<0) { //��·���÷ǿ���״̬,ʹ��������
        if(g_AuxNo!=WIRELESS_AUX) { //���ԭ�Ȳ�ΪWIRELESS_AUX����ô���³�ʼ��
            g_AuxNo=WIRELESS_AUX;
            m_AuxInitFlag=0;
        }
    } else {
        if(nAuxno==-1) { //����Ĭ�ϵ�/dev/modem1
            if(g_AuxNo!=WIRELESS_MUX1) {
                g_AuxNo=WIRELESS_MUX1;
                m_AuxInitFlag=0;
            }
        } else { //���ò������õ�AUX
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
        //enablewirelessmodemport();//����SDK��ʼ�����뵱��ʵ��
        memset(szAuxAttr,0,sizeof(szAuxAttr));
        //wlm_methodcall_gsmmux(2,&ret);
        ndk_WLMBpSelect(szAuxAttr);
        //Ϊ��ֹ�����ͬʱ��ʼ��tty�豸ʱ�����ܵ��½��̳�ʼ�����������жϴ��ڳ�ʼ���Ƿ�ɹ� yanm 20101119
        if (NDK_PortOpen(g_AuxNo, szAuxAttr) != NDK_OK) {
            fprintf(stderr, "init wlm aux %d BPS=%s ERR!\r\n", g_AuxNo,szAuxAttr);
            usleep(1000);
            if (NDK_PortOpen(g_AuxNo, szAuxAttr) != NDK_OK) { //��ʼ��������ٳ�ʼ��һ�Σ������Ȼ�����򷵻� yanm 20101119
                fprintf(stderr, "init wlm aux %d BPS=%s ERR!\r\n", g_AuxNo,szAuxAttr);
                return -1;
            }
        }
        m_AuxInitFlag=1;
        return 0;
    }
    return 0;
}

