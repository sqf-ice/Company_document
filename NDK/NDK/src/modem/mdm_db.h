/*
 * �´�½��˾ ��Ȩ����(c) 2011-2015
 *
 * modemģ��-���ù���
 * ��    �ߣ�    ��Ʒ������
 * ��    �ڣ�    2014-9-10
 * ��    ����    V1.00
 * ����޸��ˣ�
 * ����޸����ڣ�
 */

#ifndef MDM_DB_INCLUDE_H
#define MDM_DB_INCLUDE_H

//���ݿ�����ļ�
#define MDM_DB_MAIN_PATH        "/appfs/apps/data/mdm_statistics/"      /*���ݿ�����ļ�Ŀ¼*/
#define MDM_DB_MAIN_FILE        MDM_DB_MAIN_PATH "mdm_statistics.db"    /*���ݿ�����ļ�*/
#define MDM_DB_MAIN_TABLE       "MDMStatistics"                         /*Ӳ��ͳ�����ݿ��ļ��ڵı�����*/
#define MDM_DB_MAX_FILESIZE     256 * 1024                              /*256K, (1����¼3072�ֽڣ�100����¼13312�ֽ�)*/
#define MDM_DB_DELETE_ITEMS     10                                      /*���ݿ�ɾ�������¼��ʣ�µļ�¼����*/

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
    const char *    resname;        //�ַ���
    int *       resvalueadr;    //ֵ�ĵ�ַ
} ST_MDM_DB_RES_LIST;

int mdm_db_get(ST_MDM_DB_RES *stMdmDbRes);
int mdm_db_statistics(ST_MDM_STATUS stMdmSdlcs, int nNewDialFlag);
int mdm_db_result(ST_MDM_DB_RES *stMdmDbResSta, ST_MDM_STATUS stMdmStatus, int nReadNum);

#endif
