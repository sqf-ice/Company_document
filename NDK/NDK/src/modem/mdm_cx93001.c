/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-cx93001芯片
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
#include "mdm_cx93001.h"
#include "mdm_sdlc_sev.h"
#include "mdm_asyn_sev.h"
#include "mdm_drv.h"
#include "mdm_debug.h"

extern ST_MDM_CFG g_stMdmCfg;

static const ST_MDM_AT_CMD CON_Patch_F2_00_0E_0B_02[] = {
    { "AT-PV\r",      MDM_AT_RET_PV_F2000E0B02, 2000 },
    { "AT**\r",   MDM_AT_RET_DWN_INIT,      2000 },
    {
        "S32500003000A9008D1746830200E2C114080BE2C1141006224300401532201D3080EB4303C52C\r"
        "S3250000302040A541F0577C283078307D30923097309230A2309230C6309230C6309230C630AF\r"
        "S32500003040923019319230B630703077307730B95D308540B95E308541430367B5605F300D52\r"
        "S325000030600A52414D204C6F6164204572726F7200A000204E30A22260C953F03760C390C975\r"
        "S3250000308030F0301419C933F02A1419C937F024A2206020CA31801C20CA31C3A314B3C3B3BF\r"
        "S325000030A0801120CA31043AC000F004C080D0E0C3C3C380E8E86020CA31043AC31AC9FFD0B5\r"
        "S325000030C0CEC3297C2A3120CA31043A048804880488048804880488048804880438E016D09E\r"
        "S325000030E0D2830D000016F30BF02CF309F0C5F30CF0C192080000164E00166E02166E0116B8\r"
        "S32500003100AD01168D160CAD02168D170C7B0CB494C2081F0C809DA21C6020CA31043A98A32E\r"
        "S325000031209DCACA14FBD002143260683130316931A200F30CD016830C000016F4CDC33DF060\r"
        "S3250000314027A31CA39888D0F960A27760F21B0C80F8830C000016F4CDC33DA31C8D800CA3AC\r"
        "S325000031601C8D810C8888D0F260F308F008F30CD00943007831A20060C37892080000164EEF\r"
        "S3250000318000166E02166E01164E00166E02166E0116A9008D740C8D750CA93E8D720CA91327\r"
        "S325000031A08D730CA9408D740CA9008D750CAD01168D720CAD02168D730CA9808D740CA900D1\r"
        "S325000031C08D750CC2081F0CA20060043304330433043338E9309011C90A900AE9119009C94D\r"
        "S311000031E006B005890A041360DBA220601B\r"
        "S70500003000CA\r"
        "S025000000000123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEFDA\r"
        "S025000000200123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEFBA\r"
        "S025000000400123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF9A\r"
        "S32500004EA1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0B\r"
        "S32500004EC1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEB\r"
        "S32500004EE1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFCB\r"
        "S32500004F01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFAA\r"
        "S32500004F21FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF8A\r"
        "S32500004F41FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF6A\r"
        "S32500004F61FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF4A\r"
        "S32500004F81FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF2A\r"
        "S32500004FA1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0A\r"
        "S32500004FC1FFFFFFFFFFFFFF00000000000000000000000000000000000000000000000000D1\r"
        "S30D00004FE1000000FFFFFFFFFFC7\r"
        "S70500000000FA\r"
        "S32500004FE94444444CD7514C1D524444444444444444444444444444444444444444444CC304\r"
        "S32500005009554C7B554444444CA7554444444444444CE1504444444444444444444444444473\r"
        "S3250000502944444444444444444444444444444444444444444444444444444444444CB1515F\r"
        "S325000050494444444444444CB0544444444444444C82544444444444444444444444444444E7\r"
        "S325000050694444444444444444444444444444444444444444444444444444444444444444A1\r"
        "S3250000508944444444444444444444444444444444444444444444444444444444444CF151BF\r"
        "S325000050A94444444444444444444444444444444444444444444444444444444444444C0D90\r"
        "S311000050C956444444444444F2000E0B02DA\r"
        "S70500000000FA\r"
        "S325000050D50000D7007F00000000000000F2721404034CA551D204D650CED850D0FBCED950EA\r"
        "S325000050F5D0F6A97F8DD950430321D0A54069F48540A54169018541A540855FA5418560A960\r"
        "S325000051152583080367D84300DF554303D2D0A5408559A541855AA541C901D004A540C92C3D\r"
        "S32500005135906EA94083080367D84300DF55A5578540A558854183080333D84300DF55A540B0\r"
        "S325000051558561A5418562A92383080367D84300DF55C204D650A5618540A5628541A541CD3A\r"
        "S32500005175D850D005A540CDD750B025A20CA9F4830803BCC14300DF55B2A655B2D75683127B\r"
        "S32500005195034CB6831300A651B3481046D2803714448312000000A95834034CB6F2D6500445\r"
        "S325000051B5014408227361000600C9D6D012D202D650A9269261000600A9D1926100070032A2\r"
        "S325000051D52844C2019C14C2019B14A9008DDB50B301304AB3A02A4AB3002B4A44AD3A15C9DC\r"
        "S325000051F58DD00AA9008DDA50C220D55044C9DED016F2D5502011A00DA200B20B47A90A4364\r"
        "S3250000521504AB4AD220D55044E2D550010320265244ADDA500AAA7C2E5238526052D15269E9\r"
        "S32500005235539053C204D550C208D550C210D550C201D650C220D650C210D650ADDB50F00695\r"
        "S32500005255A9018DDA5060C201D55060A5B2C9ACD010207E5220A352D201D650A9028DDA501E\r"
        "S3250000527560F2BC1401FA4CBC53A011A200B20A47A9B94304AB4AA011A200B20B47A9B943AB\r"
        "S3250000529504AB4AB2A840B2F94143045D4B60A016A200B20A47A9BA4304AB4AA00AA200B22D\r"
        "S325000052B50B47A9BA4304AB4A8308040F4C4300DF55B2BC40B2F9414304CB4B60A5B2C9AC5E\r"
        "S325000052D5D0658006A9008DDA5060F2D6501017E2AB0040F5A91E8D3046A9008D3146C240BE\r"
        "S325000052F53C14D210D650E2AB0040D9E23C1440D9C210D650B3FF0F46C210D550E29211408F\r"
        "S3250000531504D210D550D2205A1443032A7320BC53201254A9038DDA50A92E8D4046A9008DEC\r"
        "S325000053354146C2203D1460F201030209A5B2C9AAF0F44CBC5343046245E23D14081520C010\r"
        "S3250000535553A91A8D3C46A9008D3D46C2083D144304DC4660E23D142022D2205A1443032A2B\r"
        "S3250000537573E2D5501004D201BF14A904830803741F4300DF55A9008DDA5060E2D6502005AC\r"
        "S32500005395F23D1420D4E20103020E202D53D220D650A9048DDA50809DA5B2C9AAF00820BCEE\r"
        "S325000053B553A9038DDA5060202F5460F2D5500208B28340B2EE418006B28940B2EE418308D5\r"
        "S325000053D500EC53831003DAB6831102A004430370B683100000006043036441A200B140290D\r"
        "S325000053F57FF0074303F340E880F3AD020329E009014303FABB43036441340305B7AD1614D3\r"
        "S32500005415290CDAA202FCFAC900D00160AD16142903D002A9014304064E60F23814200EE206\r"
        "S325000054355A141009AD161429034304064EC201B500C204A400C208A400C210A400D208D5B7\r"
        "S3250000545550C210A900D204D550201055C204D550A91743040046D201A900D204A900ADDB2B\r"
        "S3250000547550F009430125DCA9028DDA5060F2BC140116ADDB50F011ADDE14C918D00A831022\r"
        "S325000054950317658311009E54448310000000F2020308043403176534035E65F2BC14011D85\r"
        "S325000054B5ADDB50F018AD1215C975D011ADCB14C902D00A831203BD61831300D35444831281\r"
        "S325000054D5000000F2BC140128AEDB50F0234303CE7BC917D005201055A91743040046D201F1\r"
        "S325000054F5A900D204A9008314033D7C83150009553403BD612010553403F07CF2BC14012FB1\r"
        "S32500005515ADDB50F02AE2A900100D43046A45ADDB5029E009048013E2D5500409ADDB50294A\r"
        "S32500005535E009028005ADDB5029E08DA414802DF2D5500805E25A14101EE2A900100DA5B26C\r"
        "S32500005555C9AAF013A9C48DA4148011E2D5500407A9C28DA4148005A9C08DA41483080341B3\r"
        "S325000055757C4300DF55608314000000E25A14200A831203EC748313009E55A9008DDA50AD1E\r"
        "S32500005595DB50F004D201D55044831200000034030075F20E14800C8310022B258311022E0B\r"
        "S325000055B525800A8310024325831102462544E2D5504013AD3848C909F00CC942F008C99F1F\r"
        "S325000055D5F004D2208414440101018DDC558CDE558EDD5593887A985A0AA8B90356AAB904FC\r"
        "S325000055F55693C848DAADDC55ACDE55AEDD55C37824DC6695A06F6F03831003E65083110009\r"
        "S32500005615185644902BAD9657F011B29644B2574583080319574300DF5590043403E850F223\r"
        "S325000056355414101883100371518311005856800C205B56C9FFF0050AAA2055563403EB501A\r"
        "S325000056557CD1566C4600DA5AA54048A54148A54248A54348B2B842B25643A000AD7A468563\r"
        "S3250000567540AD7B468541A142C9FFF025AAE642D002E643B140D142D005CA10F78015C8E621\r"
        "S3250000569542D002E643A142D0F6E642D002E64380D5A0FF68854368854268854168854098A0\r"
        "S325000056B57AFA60032B41384500004100004800062B46434C41535300004400FF1857E056B1\r"
        "S325000056D5E956EE56DB56F2BC14010DE26E140104C2086814C201D55060A9008DDA50831041\r"
        "S325000056F5031620831100FE56608310000000E25614040CD2085914D2205A1443032A733461\r"
        "S325000057150316208310032D0E831100235760E254140102800E831003680E8311005257349E\r"
        "S3250000573503340E831003940E83110046573403740E8310000000ADDB503403960E902A8364\r"
        "S3250000575510000000F008C921F004C9C1D01FA8CDDB50D005F2D55001108DDB50A9008DDA6A\r"
        "S3250000577550D201D550D202D55034036A0E3403470EF006D240D5508004C240D5503403FB88\r"
        "S31600005795592D4C494D50000C0000865701000000005B\r"
        "S70500000000FA\r"
        "S32500005C8A44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAD6\r"
        "S32500005CAAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44B6\r"
        "S32500005CCAEAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEAF0\r"
        "S31D00005CEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEA44EAEADC\r"
        "S70500000000FA\r"
        "S30600005C004459\r"
        "S70500005C009E\r"
        "S025000000000123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEFDA\r"
        "S025000000200123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEFBA\r"
        "S025000000400123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF9A\r"
        "S3258003F9780200087C3800627C0021071464AEF315FFF73F7C6F0A32000021077CC0440700E9\r"
        "S3098003F9986D0A320039\r"
        "S705800000007A\r"
        "S3258003F8800021077CC044077C50002F00261E7300AFFF3F004503427C0021077400000F726E\r"
        "S3258003F8A037180F7440000F725EFE0F7480000F7C0021077404000F72634E0F7444000F7228\r"
        "S3118003F8C060FE0F7484000F7C0021877C9F\r"
        "S7058003F880FF\r"
        "S30E00004AAFA9508D971234032D4E17\r"
        "S70500004AAF01\r", MDM_AT_RET_OK, 2000
    },
    { "AT+GCI=26\r",  MDM_AT_RET_OK,        2000 },
    { "AT!4907=78\r", MDM_AT_RET_OK,        1000 },
    { "AT!4912=02\r", MDM_AT_RET_OK,        1000 },
    { NULL,       MDM_AT_RET_NULL,      0    },
};
static const ST_MDM_AT_CMD stCx93001SdlcInitstr[] = {
    { "AT&F;E0;W2\r", MDM_AT_RET_OK,   2000 },
    { NULL,       MDM_AT_RET_NULL, 0    },
};
static const ST_MDM_AT_CMD stCx93001AsynInitstr[] = {
    { "AT&F;E0;W2;+ER=1;\r", MDM_AT_RET_OK,   2000 },
    { NULL,          MDM_AT_RET_NULL, 0    },
};
static ST_MDM_CFG s_stMdmCfgBak;

static int mdm_get_line_vol_cx9300(void)
{
    char respone[256], *q;
    float vol;
    int i;

    if (mdm_port_at_cmd_process("AT-TRV\r", respone, sizeof(respone), 1000) == MDM_AT_RET_OK) {
        if ((q = strstr(respone, ATSTREND)) == NULL)
            return MDM_ERR_GETLINEVOL_FAIL;
        i = q - respone + 2;
        for (; i < strlen(respone); i++)
            if ((respone[i] <= '9') && (respone[i] >= '0'))
                break;
        if (i == strlen(respone))
            return MDM_ERR_GETLINEVOL_FAIL; //线压检测失败
        vol = atof(respone + i);
        if (vol < VOL_OF_ABSENT_LINE_cx93001)
            return MDM_ERR_NOLINE;          //电话线未插好
        else if (vol < VOL_OF_LINE_IN_USE_Cx93001)
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
static int mdm_adapt_init_cx93001(void)
{
    char cmd[256] = { 0 };

    //应答音时间设置
    if (s_stMdmCfgBak.nAnserToneTime != g_stMdmCfg.nAnserToneTime) {
        sprintf(cmd, "AT!4879=%02X\r", g_stMdmCfg.nAnserToneTime);
        mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    }
    //设置载波电平
    if (s_stMdmCfgBak.nDbmLevel != g_stMdmCfg.nDbmLevel) {
        sprintf(cmd, "ATS91=%d\r", g_stMdmCfg.nDbmLevel);
        mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    }
    memcpy(&s_stMdmCfgBak, &g_stMdmCfg, sizeof(g_stMdmCfg));
    return MDM_OK;
}


int mdm_sdlc_init_cx93001(void)
{
    const ST_MDM_AT_CMD *pstMdmATCmd;
    int ret;
    char cmd[256] = { 0 };

    //打补丁
    pstMdmATCmd = CON_Patch_F2_00_0E_0B_02;
    if ((ret = mdm_port_at_cmd_process(pstMdmATCmd->cmd, NULL, 0, pstMdmATCmd->timeout_ms)) != pstMdmATCmd->exp_ret) {
        pstMdmATCmd++;
        while (pstMdmATCmd->cmd != NULL) {
            if ((ret = mdm_port_at_cmd_process(pstMdmATCmd->cmd, NULL, 0, pstMdmATCmd->timeout_ms)) != pstMdmATCmd->exp_ret)
                return MDM_ERR_SDLCS_INIT_FAIL;
            pstMdmATCmd++;
        }
    }
    //同步初始化
    pstMdmATCmd = stCx93001SdlcInitstr;
    while (pstMdmATCmd->cmd != NULL) {
        if ((ret = mdm_port_at_cmd_process(pstMdmATCmd->cmd, NULL, 0, pstMdmATCmd->timeout_ms)) != pstMdmATCmd->exp_ret)
            return MDM_ERR_SDLCS_INIT_FAIL;
        pstMdmATCmd++;
    }
    //模式选择
    if ((g_stMdmCfg.nBps == 1) && (g_stMdmCfg.nCommType == 1))
        mdm_port_at_cmd_process("AT%C0;\\N0;+A8E=,,,0;$F2;+MS=V22;+ES=6,,8;+ESA=0,0,,,1,0;S17=13\r", NULL, 0, 1000);
    else if ((g_stMdmCfg.nBps == 1) && (g_stMdmCfg.nCommType != 1))
        mdm_port_at_cmd_process("AT%C0;\\N0;+A8E=,,,0;$F2;+MS=B212;+ES=6,,8;+ESA=0,0,,,1,0;S17=13\r", NULL, 0, 1000);
    else if (g_stMdmCfg.nBps == 2)
        mdm_port_at_cmd_process("AT%C0;\\N0;+A8E=,,,0;$F0;+MS=V22B;+ES=6,,8;+ESA=0,0,,,1,0;S17=7\r", NULL, 0, 1000);
    else if (g_stMdmCfg.nBps == 3)
        mdm_port_at_cmd_process("AT%C0;\\N0;+A8E=,,,0;$F4;+ES=6,,8;+ESA=0,0,,,1,0;S17=13\r", NULL, 0, 1000);
    //应答音时间设置
    sprintf(cmd, "AT!4879=%02X\r", g_stMdmCfg.nAnserToneTime);
    mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    //阻抗设置设置
    sprintf(cmd, "AT!4883=%02X\r", g_stMdmCfg.nImpedance);
    mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    //设置载波电平
    if (g_stMdmCfg.nDbmLevel != 0x0A) {
        sprintf(cmd, "ATS91=%d\r", g_stMdmCfg.nDbmLevel);
        mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    }
    //设置拨号音检测
    if (!g_stMdmCfg.nDialoneValue) {
        mdm_port_at_cmd_process("ATX0\r", NULL, 0, 1000);
        if (g_stMdmCfg.nDialoneTimeValue != 2) {
            sprintf(cmd, "ATS6=%d\r", g_stMdmCfg.nDialoneTimeValue);
            mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
        }
    }
    //设置
    sprintf(cmd, "ATS11=%dS7=%dS10=%d\r", g_stMdmCfg.nDTMF, g_stMdmCfg.nFrameS7, g_stMdmCfg.nFrameS10);
    mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    memcpy(&s_stMdmCfgBak, &g_stMdmCfg, sizeof(g_stMdmCfg));
    return MDM_OK;
}


int mdm_sdlc_dial_cx93001(const char *dialno)
{
    int ret;

    //检测线压
    if (g_stMdmCfg.nLineVolt)
        if ((ret = mdm_get_line_vol_cx9300()) != MDM_OK)
            return ret;
    if ((ret = mdm_sdlc_serv_init()) != MDM_OK)
        return ret;
    if ((ret = mdm_sdlc_serv_dial(dialno)) != MDM_OK)
        return ret;
    return MDM_OK;
}

int mdm_sdlc_hangup_cx93001(void)
{
    int ret;

    ret = mdm_sdlc_serv_hangup();
    mdm_adapt_init_cx93001();
    return ret;
}

int mdm_asyn_init_cx93001(void)
{
    int ret;
    char cmd[256] = { 0 };
    const ST_MDM_AT_CMD *pstMdmATCmd;

    //打补丁
    pstMdmATCmd = CON_Patch_F2_00_0E_0B_02;
    if ((ret = mdm_port_at_cmd_process(pstMdmATCmd->cmd, NULL, 0, pstMdmATCmd->timeout_ms)) != pstMdmATCmd->exp_ret) {
        pstMdmATCmd++;
        while (pstMdmATCmd->cmd != NULL) {
            if ((ret = mdm_port_at_cmd_process(pstMdmATCmd->cmd, NULL, 0, pstMdmATCmd->timeout_ms)) != pstMdmATCmd->exp_ret)
                return MDM_ERR_ASYN_INIT_FAIL;
            pstMdmATCmd++;
        }
    }
    //异步初始化
    pstMdmATCmd = stCx93001AsynInitstr;
    while (pstMdmATCmd->cmd != NULL) {
        if ((ret = mdm_port_at_cmd_process(pstMdmATCmd->cmd, NULL, 0, pstMdmATCmd->timeout_ms)) != pstMdmATCmd->exp_ret)
            return MDM_ERR_ASYN_INIT_FAIL;
        pstMdmATCmd++;
    }
    //应答音时间设置
    sprintf(cmd, "AT!4879=%02X\r", g_stMdmCfg.nAsynAnserToneTime);
    mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    //阻抗设置设置
    sprintf(cmd, "AT!4883=%02X\r", g_stMdmCfg.nAsynImpedance);
    mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    //设置载波电平
    if (g_stMdmCfg.nAsynDbmLevel != 0x0A) {
        sprintf(cmd, "ATS91=%d\r", g_stMdmCfg.nAsynDbmLevel);
        mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    }
    //设置拨号音检测
    if (!g_stMdmCfg.nAsynDialoneValue) {
        mdm_port_at_cmd_process("ATX0\r", NULL, 0, 1000);
        if (g_stMdmCfg.nAsynDialoneTimeValue != 2) {
            sprintf(cmd, "ATS6=%d\r", g_stMdmCfg.nAsynDialoneTimeValue);
            mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
        }
    }
    //设置
    sprintf(cmd, "ATS11=%dS7=%dS10=%d\r", g_stMdmCfg.nAsynDTMF, g_stMdmCfg.nAsynFrameS7, g_stMdmCfg.nAsynFrameS10);
    mdm_port_at_cmd_process(cmd, NULL, 0, 1000);
    return MDM_OK;
}


int mdm_asyn_dial_cx93001(const char *dialno)
{
    int ret;

    //检测线压
    if (g_stMdmCfg.nLineVolt)
        if ((ret = mdm_get_line_vol_cx9300()) != MDM_OK)
            return ret;
    if ((ret = mdm_asyn_serv_init()) != MDM_OK)
        return ret;
    if ((ret = mdm_asyn_serv_dial(dialno)) != MDM_OK)
        return ret;
    return MDM_OK;
}

int mdm_asyn_hangup_cx93001(void)
{
    int ret;

    ret = mdm_asyn_serv_hangup();
    return ret;
}
