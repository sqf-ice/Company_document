/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-cv34芯片
 * 作    者：    产品开发部
 * 日    期：    2013-12-10
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "modem.h"
#include "mdm_comm.h"
#include "mdm_cfg.h"
#include "mdm_cv34.h"
#include "mdm_sdlc_sev.h"
#include "mdm_asyn_sev.h"
#include "mdm_drv.h"
#include "mdm_debug.h"

extern ST_MDM_CFG g_stMdmCfg;

static const ST_MDM_AT_CMD stCv34SdlcInitstr[] = {
    { "ATZ\r",     MDM_AT_RET_OK,   1000 },
    { "ATE0\r",    MDM_AT_RET_OK,   1000 },
    { "AT+GCI=26\r",   MDM_AT_RET_OK,   1500 },
    { "AT%T21,29,F\r", MDM_AT_RET_OK,   1000 },     //ats7最小设置成15,否则最小值是35
    { "AT%T21,2B,1\r", MDM_AT_RET_OK,   1000 },     //ATS8最小设置成1,否则最小值是2
    { NULL,        MDM_AT_RET_NULL, 0    },
};
static const ST_MDM_AT_CMD stCv34AsynInitstr[] = {
    { "ATZ\r",      MDM_AT_RET_OK,   1000 },
    { "ATE0\r",     MDM_AT_RET_OK,   1000 },
    { "AT+GCI=26\r",    MDM_AT_RET_OK,   1500 },
    { "AT%T21,2A,64\r", MDM_AT_RET_OK,   1000 },    //ats7最大设置成100，否则最小值是65
    { "AT%T21,2B,1\r",  MDM_AT_RET_OK,   1000 },    //ATS8最小设置成1,否则最小值是2
    //如果使用AT+GCI=26这个命令,那么AT%T21,2B,1必须放在AT%T214,25cc,7000\AT%T215,1,0 之前,
    //否则AT%T214,25cc,7000\AT%T215,1,0命令将失效,如果使用AT%T19,0,19就没问题。
    { "AT%T21,36,2\r",  MDM_AT_RET_OK,   1000 },    //设置拨号音检测时间为200ms.默认是1.9s.
    { "AT%T21,8,64\r",  MDM_AT_RET_OK,   1000 },    //设置拨号音检测前等待时间为1s.默认是0s
    { NULL,         MDM_AT_RET_NULL, 0    },
};
static ST_MDM_CFG s_stMdmCfgBak;

static int mdm_get_line_vol_cv34(void)
{
    char respone[256], *q;
    int vol, i;

    if (mdm_port_at_cmd_process("ATLS1\r", respone, sizeof(respone), 1000) == MDM_AT_RET_OK) {
        if ((q = strstr(respone, ATSTREND)) == NULL)
            return MDM_ERR_GETLINEVOL_FAIL;
        i = q - respone + 2;
        for (; i < strlen(respone); i++)
            if ((respone[i] <= '9') && (respone[i] >= '0'))
                break;
        if (i == strlen(respone))
            return MDM_ERR_GETLINEVOL_FAIL; //线压检测失败
        vol = strtol(respone + i, NULL, 16);
        if (vol <= VOL_OF_ABSENT_LINE_CV34)
            return MDM_ERR_NOLINE;          //电话线未插好
        else if (vol <= VOL_OF_LINE_IN_USE_CV34)
            return MDM_ERR_OTHERMACHINE;    //线压低，并机
    } else {
        return MDM_ERR_ATCMD_RESP;              //线压检测失败
    }
    return MDM_OK;
}

/********************************************************************************
* 用于自适应功能，因为modem初始化后不会重复初始化；
* 所以如果自适应有修改参数，就放在挂断的时候进行设置。
********************************************************************************/
static int mdm_adapt_init_cv34(void)
{
    char cmd[256] = { 0 };

    //应答音时间设置
    if (s_stMdmCfgBak.nAnserToneTime != g_stMdmCfg.nAnserToneTime) {
        sprintf(cmd, "AT%%T21,17,%02X\r", g_stMdmCfg.nAnserToneTime);
        mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    }
    //设置载波电平
    if (s_stMdmCfgBak.nDbmLevel != g_stMdmCfg.nDbmLevel) {
        sprintf(cmd, "AT%%T21,2F,%02X\r", g_stMdmCfg.nDbmLevel);
        mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    }
    memcpy(&s_stMdmCfgBak, &g_stMdmCfg, sizeof(g_stMdmCfg));
    return MDM_OK;
}

static int mdm_sdlc_hangupinit_cv34(void)
{
    mdm_port_at_cmd_process("AT+ES=6,,8;+ESA=0,,,,1,0,,\r", NULL, 0, 1000);
    if (g_stMdmCfg.nBps == 0x01) {
        if (g_stMdmCfg.nCountryFlag == 1) {
            mdm_port_at_cmd_process("AT&&R2801,0\r", NULL, 0, 1000);
            mdm_port_at_cmd_process("AT\\F1\r", NULL, 0, 1000);
            mdm_port_at_cmd_process("AT%T245,0\r", NULL, 0, 1000);
        } else if (g_stMdmCfg.nCountryFlag == 2) {
            mdm_port_at_cmd_process("AT\\F1\r", NULL, 0, 1000);
        } else if (g_stMdmCfg.nCountryFlag == 3) {
            mdm_port_at_cmd_process("AT%T246,1\r", NULL, 0, 1000);
            mdm_port_at_cmd_process("AT%T132,1,100\r", NULL, 0, 1000);
            mdm_port_at_cmd_process("AT\\F1\r", NULL, 0, 1000);
            mdm_port_at_cmd_process("AT%T245,0\r", NULL, 0, 1000);
        }
    } else if (g_stMdmCfg.nBps == 0x03) {
        mdm_port_at_cmd_process("AT\\F2\r", NULL, 0, 1000);
    }
    return MDM_OK;
}

int mdm_sdlc_init_cv34(void)
{
    const ST_MDM_AT_CMD *pstMdmATCmd;
    char cmd[256] = { 0 };

    //补丁包
    pstMdmATCmd = stCv34SdlcInitstr;
    while (pstMdmATCmd->cmd != NULL) {
        if (mdm_port_at_cmd_process(pstMdmATCmd->cmd, NULL, 0, pstMdmATCmd->timeout_ms) != pstMdmATCmd->exp_ret)
            return MDM_ERR_ATCMD_RESP;
        pstMdmATCmd++;
    }
    //国家选择
    if (g_stMdmCfg.nBps == 1) {
        if (g_stMdmCfg.nCountryFlag == 1) {
            mdm_port_at_cmd_process("AT%T21,36,2\r", NULL, 0, 1000);
            mdm_port_at_cmd_process("AT%T21,8,64\r", NULL, 0, 1000);
            mdm_port_at_cmd_process("AT&&R2801,0\r", NULL, 0, 1000);
        } else if (g_stMdmCfg.nCountryFlag == 2) {
            mdm_port_at_cmd_process("AT%T21,36,2\r", NULL, 0, 1000);
            mdm_port_at_cmd_process("AT%T21,8,64\r", NULL, 0, 1000);
        } else if (g_stMdmCfg.nCountryFlag == 3) {
            mdm_port_at_cmd_process("AT%T21,36,2\r", NULL, 0, 1000);
            mdm_port_at_cmd_process("AT%T21,8,24\r", NULL, 0, 1000);
            mdm_port_at_cmd_process("AT%T246,1\r", NULL, 0, 1000);
            mdm_port_at_cmd_process("AT%T132,1,100\r", NULL, 0, 1000);
        } else {
            return MDM_ERR_CFG_INVALID;
        }
    } else {
        mdm_port_at_cmd_process("AT%T21,36,2\r", NULL, 0, 1000);
        mdm_port_at_cmd_process("AT%T21,8,64\r", NULL, 0, 1000);
    }
    //应答音时间设置
    if (g_stMdmCfg.nAnserToneTime != 0x0A) {
        sprintf(cmd, "AT%%T21,17,%02X\r", g_stMdmCfg.nAnserToneTime);
        mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    }
    //设置载波电平
    if (g_stMdmCfg.nDbmLevel != 0x0A) {
        sprintf(cmd, "AT%%T21,2F,%02X\r", g_stMdmCfg.nDbmLevel);
        mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    }
    //解决设置成中国后拨号等待的问题
    mdm_port_at_cmd_process("AT%T214,25CC,7000\r", NULL, 0, 1000);
    mdm_port_at_cmd_process("AT%T215,1,0\r", NULL, 0, 1000);
    //设置DTMF持续时间、DTMF间隔时间、载波等待时间、载波丢失超时时间
    sprintf(cmd, "ATS11=%dS9=%dS7=%dS10=%dS8=1S6=2S35=0\r", g_stMdmCfg.nDTMF, g_stMdmCfg.nDTMF, g_stMdmCfg.nFrameS7, g_stMdmCfg.nFrameS10);
    mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    //设置盲拨等待时间
    if (!g_stMdmCfg.nDialoneValue) {
        mdm_port_at_cmd_process("ATX0\r", NULL, 0, 1000);
        if (g_stMdmCfg.nDialoneTimeValue != 2) {
            sprintf(cmd, "ATS6=%d\r", g_stMdmCfg.nDialoneTimeValue);
            mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
        }
    }
    //模式选择
    if (g_stMdmCfg.nBps == 1) {
        if (g_stMdmCfg.nCommType == 1)
            mdm_port_at_cmd_process("AT+MS=V22\r", NULL, 0, 1000);
        else if (g_stMdmCfg.nCommType != 1)
            mdm_port_at_cmd_process("AT+MS=Bell212A\r", NULL, 0, 1000);
        mdm_port_at_cmd_process("AT\\F1\r", NULL, 0, 1000);
    } else if (g_stMdmCfg.nBps == 2) {
        mdm_port_at_cmd_process("AT+MS=V22B\r", NULL, 0, 1000);
    } else if (g_stMdmCfg.nBps == 3) {
        mdm_port_at_cmd_process("AT&K0\r", NULL, 0, 1000);
        mdm_port_at_cmd_process("AT+MS=V29\r", NULL, 0, 1000);
        mdm_port_at_cmd_process("AT\\F2\r", NULL, 0, 1000);
    }
    mdm_port_at_cmd_process("AT+ES=6,,8;+ESA=0,,,,1,0,,\r", NULL, 0, 1000);
    //国家选择
    if ((g_stMdmCfg.nBps == 1) || (g_stMdmCfg.nCountryFlag == 3))
        mdm_port_at_cmd_process("AT%T245,0\r", NULL, 0, 1000);

    memcpy(&s_stMdmCfgBak, &g_stMdmCfg, sizeof(g_stMdmCfg));
    return MDM_OK;
}

int mdm_sdlc_dial_cv34(const char *dialno)
{
    int ret;

    //检测线压
    if (g_stMdmCfg.nLineVolt)
        if ((ret = mdm_get_line_vol_cv34()) != MDM_OK)
            return ret;
    if ((ret = mdm_sdlc_serv_init()) != MDM_OK)
        return ret;
    if ((ret = mdm_sdlc_serv_dial(dialno)) != MDM_OK)
        return ret;
    return MDM_OK;
}

int mdm_sdlc_hangup_cv34(void)
{
    int ret;

    ret = mdm_sdlc_serv_hangup();
    mdm_adapt_init_cv34();
    mdm_sdlc_hangupinit_cv34();
    return ret;
}

int mdm_asyn_init_cv34(void)
{
    char cmd[256] = { 0 };
    const ST_MDM_AT_CMD *pstMdmATCmd;

    pstMdmATCmd = stCv34AsynInitstr;
    while (pstMdmATCmd->cmd != NULL) {
        if (mdm_port_at_cmd_process(pstMdmATCmd->cmd, NULL, 0, pstMdmATCmd->timeout_ms) != pstMdmATCmd->exp_ret)
            return MDM_ERR_ASYN_INIT_FAIL;
        pstMdmATCmd++;
    }
    //应答音时间设置
    if (g_stMdmCfg.nAsynAnserToneTime != 0x0A) {
        sprintf(cmd, "AT%%T21,17,%02X\r", g_stMdmCfg.nAsynAnserToneTime);
        mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    }
    //设置载波电平
    if (g_stMdmCfg.nAsynDbmLevel != 0x0A) {
        sprintf(cmd, "AT%%T21,2F,%02X\r", g_stMdmCfg.nAsynDbmLevel);
        mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    }
    //解决设置成中国后拨号等待的问题
    mdm_port_at_cmd_process("AT%T214,25CC,7000\r", NULL, 0, 1000);
    mdm_port_at_cmd_process("AT%T215,1,0\r", NULL, 0, 1000);
    //设置DTMF持续时间、、载波等待时间、载波丢失超时时间
    sprintf(cmd, "ATS11=%dS9=%dS7=%dS10=%dS8=1S6=2\r", g_stMdmCfg.nAsynDTMF, g_stMdmCfg.nAsynDTMF, g_stMdmCfg.nAsynFrameS7, g_stMdmCfg.nAsynFrameS10);
    mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    //设置盲拨等待时间
    if (!g_stMdmCfg.nAsynDialoneValue) {
        mdm_port_at_cmd_process("ATX0\r", NULL, 0, 1000);
        if (g_stMdmCfg.nAsynDialoneTimeValue != 2) {
            sprintf(cmd, "ATS6=%d\r", g_stMdmCfg.nAsynDialoneTimeValue);
            mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
        }
    }
    mdm_port_at_cmd_process("AT+ER=1\r", NULL, 0, 1000);
    return MDM_OK;
}

int mdm_asyn_dial_cv34(const char *dialno)
{
    int ret;

    //检测线压
    if (g_stMdmCfg.nLineVolt)
        if ((ret = mdm_get_line_vol_cv34()) != MDM_OK)
            return ret;
    if ((ret = mdm_asyn_serv_init()) != MDM_OK)
        return ret;
    if ((ret = mdm_asyn_serv_dial(dialno)) != MDM_OK)
        return ret;
    return MDM_OK;
}

int mdm_asyn_hangup_cv34(void)
{
    int ret;

    ret = mdm_asyn_serv_hangup();
    return ret;
}
