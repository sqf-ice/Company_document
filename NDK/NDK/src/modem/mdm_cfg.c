/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-配置管理
 * 作    者：    产品开发部
 * 日    期：    2013-12-10
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */
#include <string.h>

#include "libconfig.h"
#include "modem.h"
#include "mdm_cfg.h"
#include "mdm_debug.h"

//#define debug

ST_MDM_CFG g_stMdmCfg, g_stMdmDefCfg = {
    .nDbmLevel      = 10,
    .nAnserToneTime     = 10,
    .nDialoneTimeValue  = 2,
    .nDialoneValue      = 1,
    .nDTMF          = 100,
    .nFrameS7       = 20,
    .nFrameS10      = 20,
    .nBps           = 1,
    .nCountryFlag       = 1,
    .nDataTimeOut       = 18,
    .nLineVolt      = 1,
    .nFrameRst      = 8,
    .nCommType      = 1,
    .nImpedance     = 1,
    .nParamAdaptFlag    = 0,
    .nRecSignaldetFlag  = 0,
	.nCetRRdetFlag		= 0,
	.nCetRejSendFlag	= 0,
    .nAsynDbmLevel      = 10,
    .nAsynAnserToneTime = 10,
    .nAsynDialoneTimeValue  = 2,
    .nAsynDialoneValue  = 1,
    .nAsynDTMF      = 100,
    .nAsynFrameS7       = 50,
    .nAsynFrameS10      = 20,
    .nAsynProtocolFlag  = 0,
    .nAsynImpedance     = 1,
};
const ST_MDM_CFG_LIST g_stMdmCfgList[] = {
    { "baud_freq",         &g_stMdmCfg.nBps          },
    { "answer_tone_time",      &g_stMdmCfg.nAnserToneTime        },
    { "choose_country",    &g_stMdmCfg.nCountryFlag      },
    { "frame_data",        &g_stMdmCfg.nDataTimeOut      },
    { "modem_volt",        &g_stMdmCfg.nDbmLevel         },
    { "modem_voice_time",      &g_stMdmCfg.nDialoneTimeValue     },
    { "modem_voice",       &g_stMdmCfg.nDialoneValue         },
    { "modem_dtmf",        &g_stMdmCfg.nDTMF             },
    { "line_volt",         &g_stMdmCfg.nLineVolt         },
    { "frame_s10",         &g_stMdmCfg.nFrameS10         },
    { "frame_s7",          &g_stMdmCfg.nFrameS7          },
    { "frame_rst",         &g_stMdmCfg.nFrameRst         },
    { "ccitt_bell",        &g_stMdmCfg.nCommType         },
    { "impedance",         &g_stMdmCfg.nImpedance        },
    { "paramadapt",        &g_stMdmCfg.nParamAdaptFlag       },
    { "recsignal_detect",      &g_stMdmCfg.nRecSignaldetFlag     },
	{ "ConnectRR_detect",	   &g_stMdmCfg.nCetRRdetFlag	     },
	{ "RepeatPack_response",	   &g_stMdmCfg.nCetRejSendFlag	     },
    { "asyn_protocol",     &g_stMdmCfg.nAsynProtocolFlag     },
    { "asyn_modem_volt",       &g_stMdmCfg.nAsynDbmLevel         },
    { "asyn_answer_tone_time", &g_stMdmCfg.nAsynAnserToneTime    },
    { "asyn_modem_voice_time", &g_stMdmCfg.nAsynDialoneTimeValue },
    { "asyn_modem_voice",      &g_stMdmCfg.nAsynDialoneValue     },
    { "asyn_modem_dtmf",       &g_stMdmCfg.nAsynDTMF         },
    { "asyn_frame_s7",     &g_stMdmCfg.nAsynFrameS7      },
    { "asyn_frame_s10",    &g_stMdmCfg.nAsynFrameS10         },
    { "asyn_impedance",    &g_stMdmCfg.nAsynImpedance        },
    { NULL,            NULL                  }
};


static int mdm_cfg_set(config_setting_t *cfg_setting, const char *name, int value)
{
    config_setting_t *setting;

    if ((setting = config_setting_get_member(cfg_setting, name)) == NULL)
        if ((setting = config_setting_add(cfg_setting, name, CONFIG_TYPE_INT)) == NULL)
            return MDM_ERR_CFG_READ;
    config_setting_set_int(setting, value);

    return MDM_OK;
}


int mdm_cfg_init(void)
{
    mdmprint("call %s %d\n", __func__, __LINE__);
    static char s_cInitFlag = 0;
    config_t cfg;
    config_setting_t *cfg_setting;
    const ST_MDM_CFG_LIST *pstCfglist;

    if (!s_cInitFlag) {
        memcpy(&g_stMdmCfg, &g_stMdmDefCfg, sizeof(ST_MDM_CFG));

        config_init(&cfg);
        if (config_read_file(&cfg, USER_CONFIG_FILE) == CONFIG_FALSE) { //配置文件不存在，直接用默认值,返回成功
            config_destroy(&cfg);
            return MDM_OK;
        }
        if ((cfg_setting = config_lookup(&cfg, MDM_SETTING_NAME)) == NULL) {
            config_destroy(&cfg);
            return MDM_ERR_CFG_READ;
        }

        pstCfglist = g_stMdmCfgList;
        while (pstCfglist->cfgname != NULL) {
            config_setting_lookup_int(cfg_setting, pstCfglist->cfgname, pstCfglist->cfgvalueadr);
            pstCfglist++;
        }
        config_destroy(&cfg);

        s_cInitFlag = 1;
    }
#ifdef debug
    pstCfglist = g_stMdmCfgList;
    while (pstCfglist->cfgname != NULL) {
        mdmdataprintf("%s %d\n", pstCfglist->cfgname, *pstCfglist->cfgvalueadr);
        pstCfglist++;
    }
#endif
    return MDM_OK;
}

int mdm_cfg_save(void)
{
    mdmprint("call %s %d\n", __func__, __LINE__);
    config_t cfg;
    config_setting_t *cfg_setting;
    const ST_MDM_CFG_LIST *pstCfglist;

    config_init(&cfg);
    if (config_read_file(&cfg, USER_CONFIG_FILE) == CONFIG_FALSE) {
        config_destroy(&cfg);
        return MDM_ERR_CFG_OPEN;
    }
    if ((cfg_setting = config_lookup(&cfg, MDM_SETTING_NAME)) == NULL) {
        config_destroy(&cfg);
        return MDM_ERR_CFG_READ;
    }
    pstCfglist = g_stMdmCfgList;
    while (pstCfglist->cfgname != NULL) {
        mdm_cfg_set(cfg_setting, pstCfglist->cfgname, *pstCfglist->cfgvalueadr);
#ifdef debug
        mdmdataprintf("%s %d\n", pstCfglist->cfgname, *pstCfglist->cfgvalueadr);
#endif
        pstCfglist++;
    }

    config_write_file(&cfg, USER_CONFIG_FILE);
    config_destroy(&cfg);
    return MDM_OK;
}
