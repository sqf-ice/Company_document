/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-自适应
 * 作    者：    产品开发部
 * 日    期：    2014-9-10
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/types.h>
#include <fcntl.h>

#include "sqlite3.h"
#include "NDK.h"
#include "modem.h"
#include "mdm_cfg.h"
#include "mdm_debug.h"
#include "mdm_sdlc_sev.h"
#include "mdm_asyn_sev.h"
#include "mdm_db.h"

//#define debug

extern ST_MDM_CFG_LIST g_stMdmCfgList[];

static sqlite3 *mdm_db_handle = NULL;
static ST_MDM_DB_RES s_stMdmDbRes;
static const ST_MDM_DB_RES_LIST s_stMdmDbResList[] = {
    { "Succ",      &s_stMdmDbRes.nSucc      },
    { "AnswerToneErr", &s_stMdmDbRes.nAnswerToneErr },
    { "LowVolErr",     &s_stMdmDbRes.nLowVolErr },
    { "HighVolErr",    &s_stMdmDbRes.nHighVolErr    },
    { "RecSignalErr",  &s_stMdmDbRes.nRecSignalErr  },
    { "DialToneErr",   &s_stMdmDbRes.nDialToneErr   },
    { "Busy",      &s_stMdmDbRes.nBusy      },
    { "OtherErr",      &s_stMdmDbRes.nOtherErr  },
    { NULL,        NULL             }
};
static pthread_rwlock_t s_stDbRWLock = PTHREAD_RWLOCK_INITIALIZER;        //数据读写锁

static int mdm_db_open(void)
{
    int rc;
    char cmd[128];

    if (mdm_db_handle != NULL)
        return MDM_OK;

    if (access(MDM_DB_MAIN_PATH, F_OK) < 0) {
        snprintf(cmd, sizeof(cmd), "mkdir -p %s", MDM_DB_MAIN_PATH);
        if (system(cmd) != 0) {
            mdmprint("main mkdir err\n");
            return MDM_ERR_DB_MKDIR;
        }
    }

    rc = sqlite3_open(MDM_DB_MAIN_FILE, &mdm_db_handle);
    if (rc != SQLITE_OK) {
        mdmprint("Can't open database: %s\n", sqlite3_errmsg(mdm_db_handle));
        sqlite3_close(mdm_db_handle);
        mdm_db_handle = NULL;
        return MDM_ERR_DB_OPEN;
    }
    return MDM_OK;
}

/**
 *@brief                关闭主数据库文件
 *@return
 *@li             0     成功
 *@li            -1     失败
 */
static int mdm_db_close(void)
{
    int rc;

    //char cmd[128];
    if (mdm_db_handle == NULL) {
        mdmprint("db_handle = NULL\n");
        return MDM_OK;
    }

    rc = sqlite3_close(mdm_db_handle);
    if (rc != SQLITE_OK) {
        mdmprint("close database err: %s\n", sqlite3_errmsg(mdm_db_handle));
        mdm_db_handle = NULL;
        return MDM_ERR_DB_CLOSE;
    }
    mdm_db_handle = NULL;
    return MDM_OK;
}

/**
 *@brief                创建数据库链表
 *@return
 *@li             0     成功
 *@li            -1     失败
 */
static int mdm_db_createtable(void)
{
    mdmprint("call %s %d\n", __func__, __LINE__);
    char *errmsg;
    char cmd[1024] = { 0 };
	int rc, nRow, nColumn, i = 0;
	char **dbResult;
    const ST_MDM_CFG_LIST *pstCfglist;
    const ST_MDM_DB_RES_LIST *pstDbReslist;

    if (mdm_db_handle == NULL) {
        mdmprint("db_handle = NULL\n");
        return MDM_ERR_DB_OPEN;
    }

    /*  若不存在该应用统计表，则建立*/
    snprintf(cmd, sizeof(cmd), "create table if not exists %s ("
             "No int primary key not null,"
             "DialNum char,"
             "Time char,"
             "DialType char,"
             , MDM_DB_MAIN_TABLE);

    pstCfglist = g_stMdmCfgList;
    while (pstCfglist->cfgname != NULL) {
        snprintf(cmd + strlen(cmd), sizeof(cmd), "%s int", pstCfglist->cfgname);
        pstCfglist++;
        snprintf(cmd + strlen(cmd), sizeof(cmd), ",");
    }

    pstDbReslist = s_stMdmDbResList;
    while (pstDbReslist->resname != NULL) {
        snprintf(cmd + strlen(cmd), sizeof(cmd), "%s int", pstDbReslist->resname);
        pstDbReslist++;
        if (pstDbReslist->resname != NULL)
            snprintf(cmd + strlen(cmd), sizeof(cmd), ",");
    }
    snprintf(cmd + strlen(cmd), sizeof(cmd), ")");
#ifdef debug
    mdmprint("%s\n\n", cmd);
#endif
    rc = sqlite3_exec(mdm_db_handle, cmd, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        if (errmsg) {
            mdmprint("Create table error: %s\n", errmsg);
            sqlite3_free(errmsg);
        }
        return MDM_ERR_DB_EXEC;
    }
	snprintf(cmd, sizeof(cmd), "select * from %s limit 1", MDM_DB_MAIN_TABLE); //sqlite没有top
	rc = sqlite3_get_table(mdm_db_handle, cmd, &dbResult, &nRow, &nColumn, &errmsg);
#ifdef debug
	mdmprint("%s\n", cmd);
#endif
	if (rc != SQLITE_OK) {
		if (errmsg) {
			mdmprint("read table error: %s\n", errmsg);
			sqlite3_free(errmsg);
		}
		return MDM_ERR_DB_EXEC;
	}

	pstCfglist = g_stMdmCfgList;
	while (pstCfglist->cfgname != NULL) {
		i = 0;
		while (i < nColumn) {
			//mdmprint("%s\n", dbResult[i]);
			if (strcmp(dbResult[i], pstCfglist->cfgname))
				i++;
			else
				break;
		}
		if (i == nColumn) {      //找不到配置项则添加
			mdmprint("Add Column %s\n", pstCfglist->cfgname);
			snprintf(cmd, sizeof(cmd), "alter table %s add column %s int", MDM_DB_MAIN_TABLE, pstCfglist->cfgname);
			rc = sqlite3_exec(mdm_db_handle, cmd, NULL, NULL, &errmsg);
#ifdef debug
			mdmprint("%s\n", cmd);
#endif
			if (rc != SQLITE_OK) {
				if (errmsg) {
					mdmprint("Create table error: %s\n", errmsg);
					sqlite3_free(errmsg);
				}
				return MDM_ERR_DB_EXEC;
			}
		}
		pstCfglist++;
	}

	sqlite3_free_table(dbResult);
    return MDM_OK;
}

static int mdm_db_filesize(void)
{
    int ret = 0;
    struct stat buf, *p = &buf;

    if (stat(MDM_DB_MAIN_FILE, p))
        return ret;
    ret = p->st_size;

    mdmprint("call %s %d size %d\n", __func__, __LINE__, ret);
    return ret;
}

static int mdm_db_getstatics(void)
{
    mdmprint("call %s %d\n", __func__, __LINE__);
    char *errmsg;
    char cmd[256];
    int rc, nRow, nColumn, i = 0;
    char **dbResult;
    int statics = 0;

    if (mdm_db_handle == NULL) {
        mdmprint("db_handle = NULL\n");
        return MDM_ERR_DB_OPEN;
    }
    /*  若不存在该应用统计表，则建立*/
    snprintf(cmd, sizeof(cmd), "select * from %s order by No desc limit 1", MDM_DB_MAIN_TABLE); //sqlite没有top
    rc = sqlite3_get_table(mdm_db_handle, cmd, &dbResult, &nRow, &nColumn, &errmsg);
#ifdef debug
    mdmprint("%s\n", cmd);
#endif
    if (rc != SQLITE_OK) {
        if (errmsg) {
            mdmprint("read table error: %s\n", errmsg);
            sqlite3_free(errmsg);
        }
        return MDM_ERR_DB_EXEC;
    }
    if (nRow > 0) {
        while (i < nColumn) {
            if (memcmp(dbResult[i], "No", 2)) {
                i++;
            } else {
                statics = atoi(dbResult[i + nColumn]);
                break;
            }
        }
        if (i == nColumn) {
            mdmdataprintf("no No");
            sqlite3_free_table(dbResult);
            return statics;
        }
    } else {
        mdmdataprintf("nRow [%d]\n", nRow);
        sqlite3_free_table(dbResult);
        return statics;
    }

    sqlite3_free_table(dbResult);
    mdmprint("StatisticsNo %d\n", statics);
    return statics;
}

static int mdm_db_del(int nStatisticsNo)
{
    mdmprint("call %s %d\n", __func__, __LINE__);
    char *errmsg;
    char cmd[256];
    int rc, deletno = 0;

    if (mdm_db_handle == NULL) {
        mdmprint("db_handle = NULL\n");
        return MDM_ERR_DB_OPEN;
    }
    if (nStatisticsNo <= MDM_DB_DELETE_ITEMS)
        return MDM_OK;
    deletno = nStatisticsNo - MDM_DB_DELETE_ITEMS;
    snprintf(cmd, sizeof(cmd), "delete from %s where No < %d", MDM_DB_MAIN_TABLE, deletno);
    rc = sqlite3_exec(mdm_db_handle, cmd, NULL, NULL, &errmsg);
#ifdef debug
    mdmprint("%s\n", cmd);
#endif
    if (rc != SQLITE_OK) {
        if (errmsg) {
            mdmprint("Store data error: %s\n", errmsg);
            sqlite3_free(errmsg);
        }
        return MDM_ERR_DB_EXEC;
    }

    //delete之后数据库没有变小，要'vacuum'用于清除数据库空间；
    //这条命令需要大概0.3s时间。
    snprintf(cmd, sizeof(cmd), "vacuum");
    rc = sqlite3_exec(mdm_db_handle, cmd, NULL, NULL, &errmsg);
#ifdef debug
    mdmprint("%s\n", cmd);
#endif
    if (rc != SQLITE_OK) {
        if (errmsg) {
            mdmprint("Store data error: %s\n", errmsg);
            sqlite3_free(errmsg);
        }
        return MDM_ERR_DB_EXEC;
    }

    mdmprint("db delete items less %d\n", deletno);
    return MDM_OK;
}

static void mdm_db_dealres(ST_MDM_STATUS stMdmStatus, ST_MDM_DB_RES *stMdmDbRes)
{
    if ((stMdmStatus.nDialType == MODEM_DIAL_TYPE_SDLC)
        && (stMdmStatus.nStatus == MDM_SDLCS_RR)) {
        stMdmDbRes->nSucc = 1;
    } else if ((stMdmStatus.nDialType == MODEM_DIAL_TYPE_ASYN)
               && (stMdmStatus.nStatus == MDM_ASYNS_CONNECT)) {
        stMdmDbRes->nSucc = 1;
    } else if (stMdmStatus.nStatus == MDMSTATUS_MS_NODIALTONE) {
        stMdmDbRes->nSucc = 0;
        stMdmDbRes->nDialToneErr = 1;
        stMdmDbRes->nOtherErr = stMdmStatus.nStatus;
    } else if (stMdmStatus.nStatus == MDMSTATUS_MS_BUSY) {
        stMdmDbRes->nSucc = 0;
        stMdmDbRes->nBusy = 1;
        stMdmDbRes->nOtherErr = stMdmStatus.nStatus;
    } else if (stMdmStatus.nStatus < MDMSTATUS_NORETURN_AFTERPREDIAL) {
        stMdmDbRes->nSucc = 0;
        stMdmDbRes->nOtherErr = stMdmStatus.nStatus;
    }
    switch (stMdmStatus.nErr) {
        case MDM_ERR_ADAPT_HIGHVOL:
            stMdmDbRes->nHighVolErr = 1;
            break;
        case MDM_ERR_ADAPT_ANSWERTONE:
            stMdmDbRes->nAnswerToneErr = 1;
            break;
        case MDM_ERR_ADAPT_LOWVOL:
            stMdmDbRes->nLowVolErr = 1;
            break;
        case MDM_ERR_ADAPT_RECSIGNAL:
            stMdmDbRes->nRecSignalErr = 1;
            break;
        default:
            break;
    }
    return;
}


static int mdm_db_add(int nStatisticsNo, ST_MDM_STATUS stMdmStatus)
{
    mdmprint("call %s %d\n", __func__, __LINE__);
    int rc, ret;
    char *errmsg;
    char cmd[1024] = { 0 };
    const ST_MDM_CFG_LIST *pstCfglist;
    const ST_MDM_DB_RES_LIST *pstDbReslist;
    char stDialTypeName[8] = { 0 };

    if (mdm_db_handle == NULL)
        return MDM_ERR_DB_OPEN;

    if (stMdmStatus.nDialType == MODEM_DIAL_TYPE_SDLC)
        snprintf(stDialTypeName, sizeof(stDialTypeName), "SDLC");
    else if (stMdmStatus.nDialType == MODEM_DIAL_TYPE_ASYN)
        snprintf(stDialTypeName, sizeof(stDialTypeName), "ASYN");

    /*  若不存在该应用统计表，则建立*/
    snprintf(cmd, sizeof(cmd), "insert into %s "
             "(No,DialNum,Time,DialType,"
             , MDM_DB_MAIN_TABLE);
    pstCfglist = g_stMdmCfgList;
    while (pstCfglist->cfgname != NULL) {
        snprintf(cmd + strlen(cmd), sizeof(cmd), "%s", pstCfglist->cfgname);
        pstCfglist++;
        //if(pstCfglist->cfgname != NULL)
        snprintf(cmd + strlen(cmd), sizeof(cmd), ",");
    }
    pstDbReslist = s_stMdmDbResList;
    while (pstDbReslist->resname != NULL) {
        snprintf(cmd + strlen(cmd), sizeof(cmd), "%s", pstDbReslist->resname);
        pstDbReslist++;
        if (pstDbReslist->resname != NULL)
            snprintf(cmd + strlen(cmd), sizeof(cmd), ",");
    }
    snprintf(cmd + strlen(cmd), sizeof(cmd), ") ");
    snprintf(cmd + strlen(cmd), sizeof(cmd), "values (%d,'%s','%s','%s',",
             nStatisticsNo, stMdmStatus.szDialNum, stMdmStatus.szDialTime, stDialTypeName);
    pstCfglist = g_stMdmCfgList;
    while (pstCfglist->cfgname != NULL) {
        snprintf(cmd + strlen(cmd), sizeof(cmd), "%d", *pstCfglist->cfgvalueadr);
        pstCfglist++;
        snprintf(cmd + strlen(cmd), sizeof(cmd), ",");
    }
    memset(&s_stMdmDbRes, 0, sizeof(s_stMdmDbRes));
    mdm_db_dealres(stMdmStatus, &s_stMdmDbRes);
#ifdef debug
    mdmprint("call %s %d\n", __func__, __LINE__);
    pstDbReslist = s_stMdmDbResList;
    while (pstDbReslist->resname != NULL) {
        mdmdataprintf("%s %d\n", pstDbReslist->resname, *pstDbReslist->resvalueadr);
        pstDbReslist++;
    }
#endif
    pstDbReslist = s_stMdmDbResList;
    while (pstDbReslist->resname != NULL) {
        snprintf(cmd + strlen(cmd), sizeof(cmd), "%d", *pstDbReslist->resvalueadr);
        pstDbReslist++;
        if (pstDbReslist->resname != NULL)
            snprintf(cmd + strlen(cmd), sizeof(cmd), ",");
    }
    snprintf(cmd + strlen(cmd), sizeof(cmd), ")");
#ifdef debug
    mdmprint("%s\n", cmd);
#endif
    if ((ret = mdm_db_filesize()) >= MDM_DB_MAX_FILESIZE) {
        mdmprint("call %s %d size %d\n", __func__, __LINE__, ret);
        mdm_db_del(nStatisticsNo);
    }

    rc = sqlite3_exec(mdm_db_handle, cmd, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        if (errmsg) {
            mdmprint("Store data error: %s\n", errmsg);
            sqlite3_free(errmsg);
        }
        return MDM_ERR_DB_EXEC;
    }

    return MDM_OK;
}

static int mdm_db_update(int nStatisticsNo, ST_MDM_STATUS stMdmStatus)
{
    //mdmprint("call %s %d\n", __func__, __LINE__);
    static ST_MDM_DB_RES s_stMdmDbResBak;
    int rc, updateflag = 0;
    char *errmsg;
    char cmd[1024] = { 0 };
    const ST_MDM_DB_RES_LIST *pstDbReslist;

    if (mdm_db_handle == NULL)
        return MDM_ERR_DB_OPEN;
    memcpy(&s_stMdmDbResBak, &s_stMdmDbRes, sizeof(s_stMdmDbRes));
    memset(&s_stMdmDbRes, 0, sizeof(s_stMdmDbRes));
    mdm_db_dealres(stMdmStatus, &s_stMdmDbRes);
    if (memcmp(&s_stMdmDbResBak, &s_stMdmDbRes, sizeof(s_stMdmDbRes))) {
#ifdef debug
        mdmprint("call %s %d\n", __func__, __LINE__);
        pstDbReslist = s_stMdmDbResList;
        while (pstDbReslist->resname != NULL) {
            mdmdataprintf("%s %d\n", pstDbReslist->resname, *pstDbReslist->resvalueadr);
            pstDbReslist++;
        }
#endif
        snprintf(cmd, sizeof(cmd), "update %s set ", MDM_DB_MAIN_TABLE);
        pstDbReslist = s_stMdmDbResList;
        while (pstDbReslist->resname != NULL) {
            if (*pstDbReslist->resvalueadr) {
                if (updateflag)
                    snprintf(cmd + strlen(cmd), sizeof(cmd), ",");
                snprintf(cmd + strlen(cmd), sizeof(cmd), "%s=%d", pstDbReslist->resname, *pstDbReslist->resvalueadr);
                updateflag = 1;
            }
            pstDbReslist++;
        }
        if (!updateflag)
            return MDM_OK;
        snprintf(cmd + strlen(cmd), sizeof(cmd), " where No=%d", nStatisticsNo);
#ifdef debug
        mdmprint("%s\n", cmd);
#endif
        rc = sqlite3_exec(mdm_db_handle, cmd, NULL, NULL, &errmsg);
        if (rc != SQLITE_OK) {
            if (errmsg) {
                mdmprint("Store data error: %s\n", errmsg);
                sqlite3_free(errmsg);
            }
            return MDM_ERR_DB_EXEC;
        }
    }
    return MDM_OK;
}

static int mdm_db_read(ST_MDM_DB_RES *stMdmDbResSta, ST_MDM_STATUS stMdmStatus, int nReadNum)
{
    mdmprint("call %s %d\n", __func__, __LINE__);
    char *errmsg;
    char cmd[256];
    int rc, nRow, nColumn, i, j;
    char **dbResult;
    char stDialTypeName[8] = { 0 };
    ST_MDM_DB_RES stMdmDbResBak;
    const ST_MDM_DB_RES_LIST *pstDbReslist;

    if (mdm_db_handle == NULL) {
        mdmprint("db_handle = NULL\n");
        return MDM_ERR_DB_OPEN;
    }

    if (stMdmStatus.nDialType == MODEM_DIAL_TYPE_SDLC)
        snprintf(stDialTypeName, sizeof(stDialTypeName), "SDLC");
    else if (stMdmStatus.nDialType == MODEM_DIAL_TYPE_ASYN)
        snprintf(stDialTypeName, sizeof(stDialTypeName), "ASYN");

    snprintf(cmd, sizeof(cmd), "select * from %s where DialNum='%s' and DialType='%s' order by No desc limit %d",
             MDM_DB_MAIN_TABLE, stMdmStatus.szDialNum, stDialTypeName, nReadNum);
    rc = sqlite3_get_table(mdm_db_handle, cmd, &dbResult, &nRow, &nColumn, &errmsg);
    if (rc != SQLITE_OK) {
        if (errmsg) {
            mdmprint("read table error: %s\n", errmsg);
            sqlite3_free(errmsg);
        }
        return MDM_ERR_DB_EXEC;
    }
#ifdef debug
    mdmprint("%s\n", cmd);
    mdmprint("rc %d row %d column %d\n", rc, nRow, nColumn);
    if (nRow > 0) {
        for (i = 0; i <= nRow; i++) {
            for (j = 0; j < nColumn; j++)
                mdmdataprintf("%s\t", dbResult[i * nColumn + j]);
            mdmdataprintf("\n");
        }
    }
#endif

    if (nRow > 0) {
        memcpy(&stMdmDbResBak, &s_stMdmDbRes, sizeof(s_stMdmDbRes));
        memset(&s_stMdmDbRes, 0, sizeof(s_stMdmDbRes));
        pstDbReslist = s_stMdmDbResList;
        while (pstDbReslist->resname != NULL) {
            for (i = 0; i < nColumn; i++) {
                if (!memcmp(dbResult[i], pstDbReslist->resname, strlen(dbResult[i])))
                    for (j = 1; j <= nRow; j++)
                        *pstDbReslist->resvalueadr += atoi(dbResult[i + nColumn * j]);
            }
            pstDbReslist++;
        }
        memcpy(stMdmDbResSta, &s_stMdmDbRes, sizeof(s_stMdmDbRes));
#ifdef debug
        const ST_MDM_DB_RES_LIST *pstReslist = NULL;
        pstReslist = s_stMdmDbResList;
        while (pstReslist->resname != NULL) {
            mdmdataprintf("%s %d\n", pstReslist->resname, *pstReslist->resvalueadr);
            pstReslist++;
        }
#endif
        memcpy(&s_stMdmDbRes, &stMdmDbResBak, sizeof(s_stMdmDbRes));
    }

    sqlite3_free_table(dbResult);
    return MDM_OK;
}


int mdm_db_get(ST_MDM_DB_RES *stMdmDbRes)
{
    pthread_rwlock_rdlock(&s_stDbRWLock);
    memcpy(stMdmDbRes, &s_stMdmDbRes, sizeof(s_stMdmDbRes));
    pthread_rwlock_unlock(&s_stDbRWLock);
    return MDM_OK;
}


int mdm_db_result(ST_MDM_DB_RES *stMdmDbResSta, ST_MDM_STATUS stMdmStatus, int nReadNum)
{
    mdmprint("call %s %d\n", __func__, __LINE__);
    pthread_rwlock_wrlock(&s_stDbRWLock);
    mdm_db_open();
    mdm_db_read(stMdmDbResSta, stMdmStatus, nReadNum);
    mdm_db_close();
    pthread_rwlock_unlock(&s_stDbRWLock);
    mdmprint("call %s %d\n", __func__, __LINE__);
    return MDM_OK;
}

int mdm_db_statistics(ST_MDM_STATUS stMdmStatus, int nNewDialFlag)
{
    int ret = 0;
    static char s_cDbStatusFlag = 0; //0，第一次打开数据库，1，已经创建表格获取数据条目数；
    static int s_nStatisticsNo = 0;

    pthread_rwlock_wrlock(&s_stDbRWLock);
    mdm_db_open();
    if (s_cDbStatusFlag == 0) {
        mdm_db_createtable();
        if ((ret = mdm_db_getstatics()) < MDM_OK) {
            mdm_db_close();
            pthread_rwlock_unlock(&s_stDbRWLock);
            return ret;
        } else {
            s_nStatisticsNo = ret;
        }
        s_cDbStatusFlag = 1;
    }
    if (nNewDialFlag == 1) {
        s_nStatisticsNo++;
        if ((ret = mdm_db_add(s_nStatisticsNo, stMdmStatus)) != MDM_OK) {
            mdm_db_close();
            pthread_rwlock_unlock(&s_stDbRWLock);
            return ret;
        }
    } else {
        if ((ret = mdm_db_update(s_nStatisticsNo, stMdmStatus)) != MDM_OK) {
            mdm_db_close();
            pthread_rwlock_unlock(&s_stDbRWLock);
            return ret;
        }
    }
    //mdm_db_close();
    pthread_rwlock_unlock(&s_stDbRWLock);
    return MDM_OK;
}
