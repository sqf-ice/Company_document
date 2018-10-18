/*
 * �´�½��˾ ��Ȩ����(c) 2011-2015
 *
 * modemģ��-�豸����
 * ��    �ߣ�    ��Ʒ������
 * ��    �ڣ�    2013-12-10
 * ��    ����    V1.00
 * ����޸��ˣ�
 * ����޸����ڣ�
 */

#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

#include "NDK.h"
#include "NDK_Net.h"
#include "NDK_debug.h"
#include "modem.h"
#include "mdm_drv.h"
#include "mdm_comm.h"
#include "mdm_cv34.h"
#include "mdm_cx93001.h"
#include "mdm_cfg.h"
#include "mdm_sdlc_sev.h"
#include "mdm_asyn_sev.h"
#include "mdm_adapt.h"

extern void comm_methodcall_general(int method_id, int input_data, int *ret);

extern ST_MDM_CFG g_stMdmCfg;
extern int g_AuxPrintFlag;

EM_MODEM_DIAL_TYPE g_ModemCommType = MODEM_DIAL_TYPE_NOT;   /**<0:��ʼ��̬��1��ͬ����2���첽*/

static MODEM_CHIP_TYPE s_ModemChipType = MODEM_TPYE_NODEFINED;
static const MODEM_FUNC *modem_func = NULL;
static const MODEM_FUNC MODEM_FUNC_CV34 = {
    .mdm_sdlc_init      = mdm_sdlc_init_cv34,
	.mdm_sdlc_dial		= mdm_sdlc_dial_cv34,
    .mdm_sdlc_hangup    = mdm_sdlc_hangup_cv34,
    .mdm_asyn_init      = mdm_asyn_init_cv34,
	.mdm_asyn_dial		= mdm_asyn_dial_cv34,
    .mdm_asyn_hangup    = mdm_asyn_hangup_cv34,
};
static const MODEM_FUNC MODEM_FUNC_CX93001 = {
    .mdm_sdlc_init      = mdm_sdlc_init_cx93001,
	.mdm_sdlc_dial		= mdm_sdlc_dial_cx93001,
    .mdm_sdlc_hangup    = mdm_sdlc_hangup_cx93001,
    .mdm_asyn_init      = mdm_asyn_init_cx93001,
	.mdm_asyn_dial		= mdm_asyn_dial_cx93001,
    .mdm_asyn_hangup    = mdm_asyn_hangup_cx93001,
};

static int get_modem_version(char *pversion)
{
    int ret = -1;

    ret = mdm_port_at_cmd_process("AT+GMI\r", pversion, 256, 2000);
    if (ret != MDM_AT_RET_OK)
        return MDM_ERR_ATCMD_RESP;
    return MDM_OK;
}

static int get_modem_type(void)
{
    char MdmChipVer[256];
    int modemchiptype = MODEM_TPYE_NODEFINED;

    if (get_modem_version(MdmChipVer) != 0)
        return MDM_ERR_GET_CHIPTYPE;
    if (strstr(MdmChipVer, "Agere Systems") != NULL)
        modemchiptype = LSI_MODEM;
    else if (strstr(MdmChipVer, "CONEXANT") != NULL)
        modemchiptype = CONAXENT_MODEM;
    else
        return MDM_ERR_CHIPTYPE_INVALIDE;
    return modemchiptype;
}
static int mdm_type_init(void)
{
    int ret;

    if (s_ModemChipType == MODEM_TPYE_NODEFINED) {
        ret = get_modem_type();
        s_ModemChipType = ret;
        switch (s_ModemChipType) {
            case LSI_MODEM:
                modem_func = &MODEM_FUNC_CV34;
                break;
            case CONAXENT_MODEM:
                modem_func = &MODEM_FUNC_CX93001;
                break;
            default:
                s_ModemChipType = MODEM_TPYE_NODEFINED;
                return ret;
        }
    }
    return MDM_OK;
}

/**
 *@brief    ͬ�����ų�ʼ��������
 *@param    nType       ��������������Ӧ��ͬ����·����������ʹ�á�
 *@return   ��
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(nType�����������Ƿ�)
 *@li   NDK_ERR_MODEM_GETVERFAIL    MODEM ��ȡоƬ�汾ʧ��
 *@li   NDK_ERR_MODEM_ASYNINITFAIL  MODEMͬ����ʼ��ʧ��
 *
 */
NEXPORT int NDK_MdmSdlcInit(EM_MDM_PatchType nType)
{
    int ret;

    g_AuxPrintFlag = 0;
    if ((nType > 5) || (nType < 0))
        return NDK_ERR_PARA;
    mdm_cfg_init();
    if ((ret = mdm_type_init()) != MDM_OK)
        return NDK_ERR_MODEM_GETVERFAIL;
    if (g_ModemCommType != MODEM_DIAL_TYPE_SDLC)
        if ((ret = modem_func->mdm_sdlc_init()) != MDM_OK)
            return NDK_ERR_MODEM_SDLCINITFAIL;
    g_ModemCommType = MODEM_DIAL_TYPE_SDLC;
    return NDK_OK;
}

/**
 *@brief    �첽modem��ʼ����
 *@param    unType  ��������������Ӧ��ͬ����·����������ʹ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_MODEM_GETVERFAIL    MODEM ��ȡоƬ�汾ʧ��
 *@li   NDK_ERR_MODEM_ASYNINITFAIL  MODEM�첽��ʼ��ʧ��
 */
NEXPORT int NDK_MdmAsynInit(EM_MDM_PatchType nType)
{
    int ret;

    g_AuxPrintFlag = 0;
    mdm_cfg_init();
    if ((ret = mdm_type_init()) != MDM_OK)
        return NDK_ERR_MODEM_GETVERFAIL;
    if (g_ModemCommType != MODEM_DIAL_TYPE_ASYN)
        if ((ret = modem_func->mdm_asyn_init()) != MDM_OK)
            return NDK_ERR_MODEM_ASYNINITFAIL;
    g_ModemCommType = MODEM_DIAL_TYPE_ASYN;
    return NDK_OK;
}
/**
 *@brief    modem���ź�����
 *@param    pszDailNum  ���ź���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszDailNumΪNULL��pszDailNum���ȴ���25)
 *@li   NDK_ERR_MODEM_NOLINE        δ����
 *@li   NDK_ERR_MODEM_OTHERMACHINE      ���ڲ���
 *@li   NDK_ERR_MODEM_SDLCDIALFAIL  MODEM ͬ������ʧ��
 *@li   NDK_ERR_MODEM_ASYNDIALFAIL  MODEM �첽����ʧ��
 *@li   NDK_ERR_MODEM_INIT_NOT      MODEMδ���г�ʼ��
 */
NEXPORT int NDK_MdmDial(const char *pszDailNum)
{
    char szbuf[MAXLEN_DIALNUM] = { 0 }, tmp[MAXLEN_DIALNUM + 10] = { 0 }, *strend = NULL;
    int ret, fd;
    static EM_MODEM_DIAL_TYPE s_emModemType = MODEM_DIAL_TYPE_NOT;
    static char s_szDilNum[MAXLEN_DIALNUM] = { 0 };

    if ((pszDailNum == NULL) || (strlen(pszDailNum) > MAXLEN_DIALNUM))
        return NDK_ERR_PARA;

    sprintf(szbuf, "%s", pszDailNum);
    strend = strchr(szbuf, '\r');
    if (strend != NULL)
        memset(strend, 0, sizeof(szbuf) - (strend - szbuf));
    if ((s_emModemType != g_ModemCommType) || strcmp(s_szDilNum, szbuf)) {
        fd = open(MDM_LASTDIAL_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd > 0) {
            if (g_ModemCommType == MODEM_DIAL_TYPE_SDLC)
                snprintf(tmp, sizeof(tmp), "SDLC:");
            else if (g_ModemCommType == MODEM_DIAL_TYPE_ASYN)
                snprintf(tmp, sizeof(tmp), "ASYN:");
            snprintf(tmp + strlen(tmp), sizeof(tmp), "%s", szbuf);
            write(fd, &tmp, strlen(tmp));
            close(fd);
            s_emModemType = g_ModemCommType;
            strcpy(s_szDilNum, szbuf);
        }
    }

    if (g_ModemCommType == MODEM_DIAL_TYPE_SDLC) {
        mdm_drv_sdlcdial();
		if ((ret = modem_func->mdm_sdlc_dial(szbuf)) != MDM_OK) {
            mdm_drv_dialfail();
            switch (ret) {
                case MDM_ERR_NOLINE:
                    comm_methodcall_general(FUNC_MODEM_SIGNAL_SET, 0x20, &ret);
                    return NDK_ERR_MODEM_NOLINE;
                case MDM_ERR_OTHERMACHINE:
                    return NDK_ERR_MODEM_OTHERMACHINE;
                default:
                    return NDK_ERR_MODEM_SDLCDIALFAIL;
            }
        }
    } else if (g_ModemCommType == MODEM_DIAL_TYPE_ASYN) {
        mdm_drv_asyndial();
		if ((ret = modem_func->mdm_asyn_dial(szbuf)) != MDM_OK) {
            mdm_drv_dialfail();
            switch (ret) {
                case MDM_ERR_NOLINE:
                    comm_methodcall_general(FUNC_MODEM_SIGNAL_SET, 0x20, &ret);
                    return NDK_ERR_MODEM_NOLINE;
                case MDM_ERR_OTHERMACHINE:
                    return NDK_ERR_MODEM_OTHERMACHINE;
                default:
                    return NDK_ERR_MODEM_ASYNDIALFAIL;
            }
        }
    } else {
        return NDK_ERR_MODEM_INIT_NOT;
    }
    comm_methodcall_general(FUNC_MODEM_SIGNAL_SET, 0x40, &ret);
    return NDK_OK;
}
/**
 *@brief    ���modem״̬��
 *@param    *pcstatus   modem״̬��ʵ�ʷ���ֵ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pcstatusΪNULL)
 *@li   NDK_ERR_MODEM_INIT_NOT      MODEMδ���г�ʼ��
 */
NEXPORT int NDK_MdmCheck(EM_MDMSTATUS *pcstatus)
{
    if (pcstatus == NULL)
        return NDK_ERR_PARA;
    if (g_ModemCommType == MODEM_DIAL_TYPE_SDLC)
        *pcstatus = mdm_sdlc_serv_getstatus();
    else if (g_ModemCommType == MODEM_DIAL_TYPE_ASYN)
        *pcstatus = mdm_asyn_serv_getstatus();
    else
        return NDK_ERR_MODEM_INIT_NOT;
    if (*pcstatus < MDMSTATUS_NORETURN_AFTERPREDIAL)
        mdm_drv_dialfail();
    return NDK_OK;
}
/**
 *@brief    modem���ݷ��͡�
 *@param    pszdata     ���͵�����
 *@param    unDatalen   ���͵����ݳ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszdataΪNULL��unDatalen�Ƿ�)
 *@li   NDK_ERR_MODEM_INIT_NOT      MODEMδ���г�ʼ��
 *@li   NDK_ERR_MODEM_NOPREDIAL     MODEM δ����
 *@li   NDK_ERR_MODEM_SDLCWRITEFAIL     MODEMͬ��дʧ��
 *@li  NDK_ERR_MODEM_ASYNWRITEFAIL     MODEM�첽дʧ��
 *@li   NDK_ERR_TIMEOUT     ��ʱ����
 */
NEXPORT int NDK_MdmWrite(const char *pszdata, uint unDatalen)
{
    int ret;

    if ((pszdata == NULL) || ((int)unDatalen <= 0))
        return NDK_ERR_PARA;

    if (g_ModemCommType == MODEM_DIAL_TYPE_SDLC) {
        if ((ret = mdm_sdlc_serv_write(pszdata, (int)unDatalen)) != MDM_OK) {
            if (ret == MDM_ERR_SDLC_NODIAL)
                return NDK_ERR_MODEM_NOPREDIAL;
            else if (ret == MDM_ERR_SDLC_DISCONNECT)
                return NDK_ERR_MODEM_NOCARRIER;
            else if (ret == MDM_ERR_SDLC_WRITE_BUFERR)
                return NDK_ERR_PARA;
            else
                return NDK_ERR_MODEM_SDLCWRITEFAIL;
        }
    } else if (g_ModemCommType == MODEM_DIAL_TYPE_ASYN) {
        if ((int)unDatalen > MAXLEN_PORTBUF)
            return NDK_ERR_PARA;
        else if ((ret = mdm_port_put_string(pszdata, (int)unDatalen)) != MDM_OK)
            return NDK_ERR_MODEM_ASYNWRITEFAIL;
    } else {
        return NDK_ERR_MODEM_INIT_NOT;
    }
    return NDK_OK;
}
/**
 *@brief    modem���ݽ��ա�
 *@param    *pszdata    ���յ�����
 *@param    *punDatalen     ���յ����ݳ���
 *@param   unSenconds  ��ʱʱ�䣬��SΪ��λ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszdata��punDatalenΪNULL��unSenconds�Ƿ�)
 *@li   NDK_ERR_MODEM_INIT_NOT      MODEMδ���г�ʼ��
 *@li   NDK_ERR_TIMEOUT     ��ʱ����
 */
NEXPORT int NDK_MdmRead(char *pszdata, uint *punDatalen, uint unSenconds)
{
    int ret;

    if ((pszdata == NULL) || (punDatalen == NULL) || ((int)unSenconds < 0))
        return NDK_ERR_PARA;

    if (g_ModemCommType == MODEM_DIAL_TYPE_SDLC) {
        if ((ret = mdm_sdlc_serv_read(pszdata, (int *)punDatalen, (int)unSenconds)) != MDM_OK) {
            if (ret == MDM_ERR_SDLC_NODIAL)
                return NDK_ERR_MODEM_NOPREDIAL;
            else if (ret == MDM_ERR_SDLC_DISCONNECT)
                return NDK_ERR_MODEM_NOCARRIER;
            else
                return NDK_ERR_TIMEOUT;
        }
    } else if (g_ModemCommType == MODEM_DIAL_TYPE_ASYN) {
        if ((*(int *)punDatalen <= 0) || (*(int *)punDatalen > MAXLEN_PORTBUF))
            return NDK_ERR_PARA;
        else if ((ret = mdm_port_read_line(pszdata, punDatalen, unSenconds * 1000)) != MDM_OK)
            return NDK_ERR_TIMEOUT;
    } else {
        return NDK_ERR_MODEM_INIT_NOT;
    }
    return NDK_OK;
}
/**
 *@brief    modem�ҶϺ�����
 *@param
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_MODEM_SDLCHANGUPFAIL        MODEMͬ���Ҷ�ʧ��
 *@li   NDK_ERR_MODEM_ASYNHANGUPFAIL        MODEM�첽�Ҷ�ʧ��
 *@li   NDK_ERR_MODEM_INIT_NOT      MODEMδ���г�ʼ��
 */
NEXPORT int NDK_MdmHangup(void)
{
    int ret;

    comm_methodcall_general(FUNC_MODEM_SIGNAL_SET, 0x00, &ret);
    mdm_drv_hungup();
    g_AuxPrintFlag = 0;
    if (g_ModemCommType == MODEM_DIAL_TYPE_SDLC) {
        if ((ret = modem_func->mdm_sdlc_hangup()) != MDM_OK)
            return NDK_ERR_MODEM_SDLCHANGUPFAIL;
    } else if (g_ModemCommType == MODEM_DIAL_TYPE_ASYN) {
        if ((ret = modem_func->mdm_asyn_hangup()) != MDM_OK)
            return NDK_ERR_MODEM_ASYNHANGUPFAIL;
    } else {
        return NDK_ERR_MODEM_INIT_NOT;
    }
    return NDK_OK;
}
/**
 *@brief    ���modem���ջ�������
 *      ֻҪ��ʼ���󣬶����ܽ��ж�Ӧ��ͬ�����첽���ջ�����ա�
 *@param
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_MODEM_SDLCCLRBUFFAIL        MODEMͬ���建��ʧ��
 *@li   NDK_ERR_MODEM_ASYNCLRBUFFAIL        MODEM�첽�建��ʧ��
 *@li   NDK_ERR_MODEM_INIT_NOT      MODEMδ���г�ʼ��
 */
NEXPORT int NDK_MdmClrbuf(void)
{
    int ret;

    if (g_ModemCommType == MODEM_DIAL_TYPE_SDLC) {
        if ((ret = mdm_sdlc_serv_clrbuff()) != MDM_OK)
            return NDK_ERR_MODEM_SDLCCLRBUFFAIL;
    } else if (g_ModemCommType == MODEM_DIAL_TYPE_ASYN) {
        if ((ret = mdm_port_clr_buf()) != MDM_OK)
            return NDK_ERR_MODEM_ASYNCLRBUFFAIL;
    } else {
        return NDK_ERR_MODEM_INIT_NOT;
    }
    return NDK_OK;
}
/**
 *@brief    ��ȡmodem���ȡ�
 *@param    *punReadlen     ���صĳ���ֵ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(punReadlenΪNULL)
 *@li  NDK_ERR_MODEM_GETBUFFLENFAIL        MODEM��ȡ����ʧ��
 *@li   NDK_ERR_MODEM_INIT_NOT      MODEMδ���г�ʼ��
 */
NEXPORT int NDK_MdmGetreadlen(uint *punReadlen)
{
    int ret;

    if (punReadlen == NULL)
        return NDK_ERR_PARA;
    if (g_ModemCommType == MODEM_DIAL_TYPE_SDLC) {
        *punReadlen = mdm_sdlc_serv_readlen();
    } else if (g_ModemCommType == MODEM_DIAL_TYPE_ASYN) {
        if ((ret = mdm_port_buf_len((int *)punReadlen)) != MDM_OK)
            return NDK_ERR_MODEM_GETBUFFLENFAIL;
    } else {
            return NDK_ERR_MODEM_INIT_NOT;
    }
    return NDK_OK;
}

/**
 *@brief    modem��λ����(�ղ�����ʼ�շ��سɹ�)��
 *@return
 *@li   NDK_OK              �����ɹ���
 */
NEXPORT int NDK_MdmReset(void)
{
#if 0
    int ret;

    mdm_drv_hungup();
    g_AuxPrintFlag = 0;
    if (g_ModemCommType == MODEM_DIAL_TYPE_SDLC)
        if ((ret = mdm_sdlc_serv_init()) != MDM_OK)
            return NDK_ERR_MODEM_RESETFAIL;
    if ((ret = mdm_drv_reset()) != MDM_OK)
        return NDK_ERR_MODEM_RESETFAIL;
#endif
    return NDK_OK;
}
/**
 *@brief    AT������չ����(unX,unY)����
 *@param    pucCmdstr   ��������
 *@param    pszRespData     ���ص���Ӧ����
 *@param    unpLen  ���ص����ݳ���
 *@param    unTimeout   ��ʱʱ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pucCmdstr\pszRespData\unpLenΪNULL��unTimeoutС��0��������ȴ���52)
 *@li   NDK_ERR_MODEM_ATCOMNORESPONSE       MODEM AT�������
 */
NEXPORT int NDK_MdmExCommand(uchar *pucCmdstr, uchar *pszRespData, uint *unpLen, uint unTimeout)
{
    int ret;

    if ((pucCmdstr == NULL) || (pszRespData == NULL) || (unpLen == NULL) || (int)unTimeout < 0)
        return NDK_ERR_PARA;
    if (strlen((char *)pucCmdstr) > MAXLEN_PERATCOMMAND_SYS)
        return NDK_ERR_PARA;
    if (((pucCmdstr[0] != 'a') && (pucCmdstr[0] != 'A'))
        || ((pucCmdstr[1] != 't') && (pucCmdstr[1] != 'T'))
        || (pucCmdstr[strlen((char *)pucCmdstr) - 1] != '\r'))
        return NDK_ERR_PARA;

    ret = mdm_port_at_cmd_process((const char *)pucCmdstr, (char *)pszRespData, 128, unTimeout * 1000);
    if (ret >= MDM_AT_RET_OK) {
        *unpLen = strlen((char *)pszRespData);
        return NDK_OK;
    }
    return NDK_ERR_MODEM_ATCOMNORESPONSE;
}

/**
 *@brief    modem����Ӧ��
 *@param    emModemDialType   ��������
 *@param    pszDialNum     ���ź���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pstDialNumΪNULL��nModemDialType����ͬ�������첽����)
 *@li   NDK_ERR_MODEM_SELFADAPTFAIL      ����Ӧʧ�ܣ�δ�ҵ�����ʹ�õĲ�����
 */
NEXPORT int NDK_MdmAdapt(EM_MODEM_DIAL_TYPE emModemDialType, const char *pszDialNum)
{
    int ret;

    if (pszDialNum == NULL)
        return NDK_ERR_PARA;
    if ((emModemDialType != MODEM_DIAL_TYPE_SDLC) && (emModemDialType != MODEM_DIAL_TYPE_ASYN))
        return NDK_ERR_PARA;

    ret = mdm_adapt_param(emModemDialType, pszDialNum);
    if ((ret == MDM_ERR_ADAPT_CONFIG_FILE) || (ret == MDM_ERR_ADAPT_FAIL))
        return NDK_ERR_MODEM_SELFADAPTFAIL;
    else if (ret != MDM_OK)
        return ret;
    return NDK_OK;
}

int ndk_mdmadaptrate(void)
{
    return mdm_adapt_rate();
}

int ndk_mdmsleep(void)
{
    int ret;

    if ((ret = mdm_drv_sleep()) != MDM_OK)
        return NDK_ERR_MODEM_SLEPPFAIL;
    return NDK_OK;
}

int ndk_mdmreset(void)
{
    int ret;

    mdm_drv_hungup();
    g_AuxPrintFlag = 0;
    if (g_ModemCommType == MODEM_DIAL_TYPE_SDLC)
        if ((ret = mdm_sdlc_serv_init()) != MDM_OK)
            return NDK_ERR_MODEM_RESETFAIL;
    if ((ret = mdm_drv_reset()) != MDM_OK)
        return NDK_ERR_MODEM_RESETFAIL;
    return NDK_OK;
}
