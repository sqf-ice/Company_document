/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-配置管理
 * 作    者：    产品开发部
 * 日    期：    2014-9-2
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */

#ifndef MDM_ADAPT_INCLUDE_H
#define MDM_ADAPT_INCLUDE_H

#define SDLC_MODIFY_CONFIG_FILE        "/etc/mdm_sdlc_modify.conf"
#define ASYN_MODIFY_CONFIG_FILE        "/etc/mdm_asyn_modify.conf"
#define SYS_RESTORE_CONFIG_FILE        "/mnt/hwinfo/sys_mdm_restore.conf"
#define SYS_VALID_CONFIG_FILE          "/mnt/hwinfo/sys_mdm_valid.conf"

#define MDM_PatchType       5
#define MDM_DIAL_MAX_TIMES  6
#define MDM_SUCC_MAX_TIMES  4
#define MDM_ERR_MAX_TIMES   2
#define MDM_AUTO_MAX_TIMES  10

int mdm_adapt_param(EM_MODEM_DIAL_TYPE emModemDialType, const char *pszDialNum);
int mdm_adapt_auto(ST_MDM_STATUS stMdmStatus, int nNewDialFlag);
int mdm_adapt_rate(void);

#endif
