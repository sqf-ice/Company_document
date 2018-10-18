/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-自适应
 * 作    者：    产品开发部
 * 日    期：    2014-9-2
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include "libconfig.h"

#include "NDK.h"
#include "modem.h"
#include "mdm_cfg.h"
#include "mdm_debug.h"
#include "mdm_adapt.h"
#include "mdm_drv.h"
#include "mdm_db.h"

//#define debug

extern ST_MDM_CFG g_stMdmCfg, g_stMdmDefCfg;
extern ST_MDM_CFG_LIST g_stMdmCfgList[];

static int s_nOneGroupDials = 0;
static int s_nGroups = 0;
static int s_nParamGroupNum = 0;

/**********************
*   测试Modem性能。
**********************/
static int mdm_dial_test(EM_MODEM_DIAL_TYPE modemtype, const char *dialno, EM_MDMSTATUS *MdmStatus)
{
    int ret, timeout, key = 0;

	ndk_mdmreset();
    if (modemtype == MODEM_DIAL_TYPE_SDLC) {
        ret = NDK_MdmSdlcInit(MDM_PatchType);
        timeout = 30 * 10;
    } else if (modemtype == MODEM_DIAL_TYPE_ASYN) {
        ret = NDK_MdmAsynInit(MDM_PatchType);
        timeout = 90 * 10;
    } else {
        mdmprint("call %s %d dialtye err %d\n", __func__, __LINE__, modemtype);
        return MDM_ERR_PARA;
    }
    if (ret != NDK_OK) {
        mdmprint("call %s %d ret %d\n", __func__, __LINE__, ret);
        return ret;
    }

    ret = NDK_MdmDial(dialno);
    if (ret != NDK_OK) {
        mdmprint("call %s %d ret %d\n", __func__, __LINE__, ret);
        return ret;
    }

    do {
        ret = NDK_MdmCheck(MdmStatus);
        if (ret != NDK_OK) {
            mdmprint("call %s %d ret %d\n", __func__, __LINE__, ret);
            NDK_MdmHangup();
            return ret;
        }
        if (*MdmStatus < MDMSTATUS_NORETURN_AFTERPREDIAL) {
            NDK_MdmHangup();
            return MDM_OK;
        }
        NDK_KbHit(&key);
        if (key == K_ESC) {			
            NDK_MdmHangup();
            return NDK_ERR_MODEM_SELFADAPTCANCEL;
        }
        mdm_msdelay(100);
        if (!timeout--) {
            NDK_MdmHangup();
            return MDM_OK;
        }
    } while (*MdmStatus < MDMSTATUS_CONNECT_AFTERPREDIAL);

    ret = NDK_MdmHangup();
    if (ret != NDK_OK) {
        mdmprint("call %s %d ret %d\n", __func__, __LINE__, ret);
        return ret;
    }
    return MDM_OK;
}

static int mdm_dial_test_times(EM_MODEM_DIAL_TYPE modemtype, const char *dialno)
{
    int ret, err, succ;
    EM_MDMSTATUS status;

    s_nOneGroupDials = 0;
    err = 0;
    succ = 0;

    while (s_nOneGroupDials < MDM_DIAL_MAX_TIMES) {
        ret = mdm_dial_test(modemtype, dialno, &status);
        if (ret != NDK_OK)
            return ret;
        if (status == MDMSTATUS_CONNECT_AFTERPREDIAL) {
            succ++;
        } else {
            err++;
            succ = 0;
        }
        if (err >= MDM_ERR_MAX_TIMES)
            return MDM_ERR_ADAPT_DIALFAIL;
        if (succ >= MDM_SUCC_MAX_TIMES)
            break;
        s_nOneGroupDials++;
    }
    s_nOneGroupDials = 0;
    return MDM_OK;
}

static int mdm_param_length(const char *pstReadFileName)
{
    mdmprint("call %s %d\n", __func__, __LINE__);
    int length = 0;
    config_t cfg_sys;
    config_setting_t *root;

    config_init(&cfg_sys);
    if (config_read_file(&cfg_sys, pstReadFileName) == CONFIG_FALSE) {
        config_destroy(&cfg_sys);
        mdmprint("call %s %d\n", __func__, __LINE__);
        return MDM_ERR_CFG_OPEN;
    }
    if ((root = config_root_setting(&cfg_sys)) == NULL) {
        mdmprint("Can't find mdm\n");
        config_destroy(&cfg_sys);
        return MDM_ERR_CFG_READ;
    }
    length = config_setting_length(root);
    config_destroy(&cfg_sys);
    mdmprint("mdm setting length %d\n", length);
    return length;
}

static int mdm_param_save(char *pstSaveFileName)
{
    mdmprint("call %s %d %s\n", __func__, __LINE__, pstSaveFileName);
    int fd = -1;
    config_t cfg_save;
    config_setting_t *root_save, *mdmcfg_save, *setting;
    const ST_MDM_CFG_LIST *pstCfglist;

    fd = creat(pstSaveFileName, 0644);
    if (fd == -1) {
        if (errno != EEXIST) {
            mdmprint("call %s %d\n", __func__, __LINE__);
            return MDM_ERR_CFG_OPEN;
        } else {
            mdmprint("call %s %d file exist\n", __func__, __LINE__);
        }
    } else {
        close(fd);
    }
    config_init(&cfg_save);
    if ((root_save = config_root_setting(&cfg_save)) == NULL) {
        config_destroy(&cfg_save);
        mdmprint("call %s %d\n", __func__, __LINE__);
        return MDM_ERR_CFG_READ;
    }
    if ((mdmcfg_save = config_setting_add(root_save, MDM_SETTING_NAME, CONFIG_TYPE_GROUP)) == NULL) {
        config_destroy(&cfg_save);
        mdmprint("call %s %d\n", __func__, __LINE__);
        return MDM_ERR_CFG_READ;
    }
    pstCfglist = g_stMdmCfgList;
    while (pstCfglist->cfgname != NULL) {
        setting = config_setting_add(mdmcfg_save, pstCfglist->cfgname, CONFIG_TYPE_INT);
        config_setting_set_int(setting, *pstCfglist->cfgvalueadr);
#ifdef debug
        mdmdataprintf("%s:%d\n", config_setting_name(setting), config_setting_get_int(setting));
#endif
        pstCfglist++;
    }
    if (config_write_file(&cfg_save, pstSaveFileName) != CONFIG_TRUE) {
        mdmprint("Error while writing file.\n");
        config_destroy(&cfg_save);
        return MDM_ERR_CFG_INVALID;
    }
    config_destroy(&cfg_save);
    return MDM_OK;
}


static int mdm_param_read(const char *pstReadFileName, int index)
{
    mdmprint("call %s %d read: %s\n", __func__, __LINE__, pstReadFileName);
    int i, value, length = 0;
    char *setting_name = NULL;
    config_t modify;
    config_setting_t *root, *modify_setting, *setting;
    const ST_MDM_CFG_LIST *pstCfglist;

    config_init(&modify);
    if (config_read_file(&modify, pstReadFileName) == CONFIG_FALSE) {
        config_destroy(&modify);
        mdmprint("call %s %d\n", __func__, __LINE__);
        return MDM_ERR_CFG_OPEN;
    }
    if (index >= 0) { //读取自适应参数文件
        if ((root = config_root_setting(&modify)) == NULL) {
            mdmprint("Can't find mdm\n");
            config_destroy(&modify);
            return MDM_ERR_CFG_READ;
        }
        if ((modify_setting = config_setting_get_elem(root, index)) == NULL) {
            mdmprint("Can't find mdm\n");
            config_destroy(&modify);
            return MDM_ERR_CFG_READ;
        }
    } else { //读取参数保存文件
        if ((modify_setting = config_lookup(&modify, MDM_SETTING_NAME)) == NULL) {
            config_destroy(&modify);
            return MDM_ERR_CFG_READ;
        }
    }
    length = config_setting_length(modify_setting);
    for (i = 0; i < length; i++) {
        if ((setting = config_setting_get_elem(modify_setting, i)) == NULL) {
            mdmprint("Can't find mdm\n");
            config_destroy(&modify);
            return MDM_ERR_CFG_READ;
        }
        setting_name = config_setting_name(setting);
        value = config_setting_get_int(setting);
#ifdef debug
        mdmdataprintf("%s:%d\n", setting_name, value);
#endif
        pstCfglist = g_stMdmCfgList;
        while (pstCfglist->cfgname != NULL) {
            if (!memcmp(setting_name, pstCfglist->cfgname, strlen(pstCfglist->cfgname))) {
                *pstCfglist->cfgvalueadr = value;
                break;
            }
            pstCfglist++;
        }
    }

    config_destroy(&modify);
    return MDM_OK;
}

static int mdm_param_setdef(EM_MODEM_DIAL_TYPE emModemDialType, char *pszExceptCfg)
{
    int i = 0, pDefCfgOff = 0;
    int nchangeflag = 0;

    while (g_stMdmCfgList[i].cfgname != NULL) {
        if ((pszExceptCfg == NULL) ||
            (memcmp(g_stMdmCfgList[i].cfgname, pszExceptCfg, strlen(pszExceptCfg)))) {
            if (emModemDialType == MODEM_DIAL_TYPE_SDLC) {
                if (strstr(g_stMdmCfgList[i].cfgname, ASYN_CFG_NAMEHEAD) == NULL) {
                    pDefCfgOff = ((char *)g_stMdmCfgList[i].cfgvalueadr - (char *)&g_stMdmCfg);
                    if (*g_stMdmCfgList[i].cfgvalueadr != *((char *)&g_stMdmDefCfg + pDefCfgOff)) {
                        *g_stMdmCfgList[i].cfgvalueadr = *((char *)&g_stMdmDefCfg + pDefCfgOff);
                        nchangeflag = 1;
                    }
                }
            } else if (emModemDialType == MODEM_DIAL_TYPE_ASYN) {
                if (strstr(g_stMdmCfgList[i].cfgname, ASYN_CFG_NAMEHEAD) != NULL) {
                    pDefCfgOff = ((char *)g_stMdmCfgList[i].cfgvalueadr - (char *)&g_stMdmCfg);
                    if (*g_stMdmCfgList[i].cfgvalueadr != *((char *)&g_stMdmDefCfg + pDefCfgOff)) {
                        *g_stMdmCfgList[i].cfgvalueadr = *((char *)&g_stMdmDefCfg + pDefCfgOff);
                        nchangeflag = 1;
                    }
                }
            }
        }
        i++;
    }
    mdmprint("call %s %d flag %d\n", __func__, __LINE__, nchangeflag);
    return nchangeflag;
}

int mdm_adapt_auto(ST_MDM_STATUS stMdmStatus, int nNewDialFlag)
{
    //mdmprint("call %s %d\n", __func__, __LINE__);
    static ST_MDM_DB_RES s_stResStatisBak;
    static ST_MDM_CFG s_stMdmCfgBak;
    ST_MDM_DB_RES stResStatis, stResNow;
    int nFirstAutoFlag = 0, nReadDbFlag = 1;

    //1.保存现有配置到/mnt/hwinfo/sys_mdm_restore.conf
    if (access(SYS_RESTORE_CONFIG_FILE, F_OK) != 0) {
        mdmprint("file %s not exist\n", SYS_RESTORE_CONFIG_FILE);
        nFirstAutoFlag = 1;
        mdm_param_save(SYS_RESTORE_CONFIG_FILE);
    }

    if (nNewDialFlag == 1)
        memset(&s_stResStatisBak, 0, sizeof(s_stResStatisBak));

    mdm_db_get(&stResNow);
    if ((stResNow.nLowVolErr == s_stResStatisBak.nLowVolErr) &&
        (stResNow.nHighVolErr == s_stResStatisBak.nHighVolErr) &&
        (stResNow.nAnswerToneErr == s_stResStatisBak.nAnswerToneErr) &&
        (stResNow.nRecSignalErr == s_stResStatisBak.nRecSignalErr) &&
        (stResNow.nSucc == s_stResStatisBak.nSucc))
        return MDM_OK;
    memcpy(&s_stResStatisBak, &stResNow, sizeof(stResNow));

    if ((stResNow.nSucc) && (memcmp(&s_stMdmCfgBak, &g_stMdmCfg, sizeof(g_stMdmCfg))))
        mdm_param_save(SYS_VALID_CONFIG_FILE);
    memcpy(&s_stMdmCfgBak, &g_stMdmCfg, sizeof(g_stMdmCfg));
    if (stResNow.nLowVolErr || stResNow.nHighVolErr
        || stResNow.nAnswerToneErr || stResNow.nRecSignalErr) {
        if (nFirstAutoFlag) {
            //第一次使用自适应时，恢复成默认参数
            //如果原来参数不是默认配置，则不进行错误判断
            if (mdm_param_setdef(stMdmStatus.nDialType, "paramadapt"))
                nReadDbFlag = 0;
        }
        if (nReadDbFlag) {
            mdmprint("call %s %d\n", __func__, __LINE__);
            mdm_db_result(&stResStatis, stMdmStatus, MDM_AUTO_MAX_TIMES);
            if (stMdmStatus.nDialType == MODEM_DIAL_TYPE_SDLC) {
                //处理同步自适应
                if ((stResStatis.nLowVolErr >= 2) && (stResNow.nLowVolErr)) {
                    if (g_stMdmCfg.nDbmLevel > 2)
                        g_stMdmCfg.nDbmLevel -= 2;
                    else
                        g_stMdmCfg.nDbmLevel = 0;
                }
                if ((stResStatis.nRecSignalErr) && (stResNow.nRecSignalErr))
                    g_stMdmCfg.nRecSignaldetFlag = 1;
                if ((stResStatis.nHighVolErr >= 2) && (stResNow.nHighVolErr))
                    if (g_stMdmCfg.nDbmLevel < 15)
                        g_stMdmCfg.nDbmLevel += 1;
                if ((stResStatis.nAnswerToneErr >= 2) && (stResNow.nAnswerToneErr))
                    if (g_stMdmCfg.nAnserToneTime < 150)
                        g_stMdmCfg.nAnserToneTime += 50;
            } else if (stMdmStatus.nDialType == MODEM_DIAL_TYPE_ASYN) {
                //处理异步自适应
            }
        }
        if (memcmp(&s_stMdmCfgBak, &g_stMdmCfg, sizeof(g_stMdmCfg)))
            mdm_cfg_save();
    }

    //mdmprint("call %s %d\n", __func__, __LINE__);
    return MDM_OK;
}


/**
 *@brief        modem指定参数修复功能
 *@param    nModemDialType     拨号类型(1.同步 2.异步)
 *@param    pszDialNum   拨号号码
 *@return
 *
 */
int mdm_adapt_param(EM_MODEM_DIAL_TYPE emModemDialType, const char *pszDialNum)
{
    int i, ret;
    char *param_file_name = NULL;
	
	s_nOneGroupDials = 0;
	s_nGroups = 0;

    if (pszDialNum == NULL) {
        mdmprint("call %s %d\n", __func__, __LINE__);
        return MDM_ERR_PARA;
    }
    if ((emModemDialType != MODEM_DIAL_TYPE_SDLC) && (emModemDialType != MODEM_DIAL_TYPE_ASYN)) {
        mdmprint("call %s %d\n", __func__, __LINE__);
        return MDM_ERR_PARA;
    }
    mdmprint("call %s %d dialtype %d num %s\n", __func__, __LINE__, emModemDialType, pszDialNum);
    if (emModemDialType == MODEM_DIAL_TYPE_SDLC)
        param_file_name = SDLC_MODIFY_CONFIG_FILE;
    else if (emModemDialType == MODEM_DIAL_TYPE_ASYN)
        param_file_name = ASYN_MODIFY_CONFIG_FILE;
    s_nParamGroupNum = mdm_param_length(param_file_name);

    mdm_cfg_init();
    mdm_param_save(SYS_RESTORE_CONFIG_FILE);

    mdm_param_setdef(emModemDialType, NULL);
    mdm_cfg_save();
    ret = mdm_dial_test_times(emModemDialType, pszDialNum);
    s_nGroups++;
    if (ret == MDM_OK) {
        mdm_param_save(SYS_VALID_CONFIG_FILE);
        return MDM_OK;
    } else if (ret != MDM_ERR_ADAPT_DIALFAIL) {
        mdm_param_read(SYS_RESTORE_CONFIG_FILE, -1);
        mdm_cfg_save();
        return ret;
    }
    mdmprint("call %s %d\n", __func__, __LINE__);

    if (s_nParamGroupNum <= 0) {
        mdm_param_read(SYS_RESTORE_CONFIG_FILE, -1);
        mdm_cfg_save();
        return MDM_ERR_ADAPT_CONFIG_FILE;
    }
    for (i = 0; i < s_nParamGroupNum; i++) {
        mdm_param_read(param_file_name, i);
        mdm_cfg_save();
        ret = mdm_dial_test_times(emModemDialType, pszDialNum);
        s_nGroups++;
        if (ret == MDM_OK) {
            mdm_param_save(SYS_VALID_CONFIG_FILE);
            return MDM_OK;
        } else if (ret != MDM_ERR_ADAPT_DIALFAIL) {
            mdm_param_read(SYS_RESTORE_CONFIG_FILE, -1);
            mdm_cfg_save();
            return ret;
        }
    }

    mdm_param_read(SYS_RESTORE_CONFIG_FILE, -1);
    g_stMdmCfg.nParamAdaptFlag = 1;
    mdm_cfg_save();
    return MDM_ERR_ADAPT_FAIL;
}

int mdm_adapt_rate(void)
{
    int ret;

    if ((s_nOneGroupDials == 0) && (s_nGroups == 0))
        ret = 0;
    else
        ret = (s_nGroups * MDM_DIAL_MAX_TIMES + s_nOneGroupDials) * 100
              / ((s_nParamGroupNum + 1) * MDM_DIAL_MAX_TIMES);
    return ret;
}
