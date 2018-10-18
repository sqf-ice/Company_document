/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-配置管理
 * 作    者：    产品开发部
 * 日    期：    2014-9-10
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */

#ifndef MDM_DB_INCLUDE_H
#define MDM_DB_INCLUDE_H

//数据库相关文件
#define MDM_DB_MAIN_PATH        "/appfs/apps/data/mdm_statistics/"      /*数据库的主文件目录*/
#define MDM_DB_MAIN_FILE        MDM_DB_MAIN_PATH "mdm_statistics.db"    /*数据库的主文件*/
#define MDM_DB_MAIN_TABLE       "MDMStatistics"                         /*硬件统计数据库文件内的表名称*/
#define MDM_DB_MAX_FILESIZE     256 * 1024                              /*256K, (1条记录3072字节；100条记录13312字节)*/
#define MDM_DB_DELETE_ITEMS     10                                      /*数据库删除多余记录后剩下的记录数量*/

typedef struct {
    int nSucc;
    int nAnswerToneErr;
    int nLowVolErr;
    int nHighVolErr;
    int nRecSignalErr;
    int nDialToneErr;
    int nBusy;
    int nOtherErr;
} ST_MDM_DB_RES;

typedef struct {
    const char *    resname;        //字符串
    int *       resvalueadr;    //值的地址
} ST_MDM_DB_RES_LIST;

int mdm_db_get(ST_MDM_DB_RES *stMdmDbRes);
int mdm_db_statistics(ST_MDM_STATUS stMdmSdlcs, int nNewDialFlag);
int mdm_db_result(ST_MDM_DB_RES *stMdmDbResSta, ST_MDM_STATUS stMdmStatus, int nReadNum);

#endif
