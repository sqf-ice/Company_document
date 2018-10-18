/*
* �´�½��˾ ��Ȩ����(c) 2011-2015
*
* ͳһƽ̨NDK API
* ��    �ߣ�    ��Ʒ������
* ��    �ڣ�    2012-08-17
* ��    ����    V1.00
* ����޸��ˣ�
* ����޸����ڣ�
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "NDK.h"
#include "NDK_debug.h"

#if 0

NEXPORT typedef enum {
    NDK_OK=0,               /**<�����ɹ�*/
    NDK_ERR=-1,         /**<����ʧ��*/
    NDK_ERR_PARA=-2,
    NDK_ERR_MACLLOC=-3,
    NDK_ERR_OPEN=-4,
    NDK_ERR_IO=-5,
    NDK_ERR_WRITE =-6,
    NDK_ERR_READ = -7

} EM_NDK_ERR;
#endif

#define FILENAME_MAXLEN 19


/**
 *@brief        ����Ŀ¼.
 *@details
 *@param    pszName Ŀ¼����
 *@return
 *@li    NDK_OK             �����ɹ�����
 *@li   NDK_ERR_PARA        �����Ƿ�(pszNameΪNULL)
 *@li   NDK_ERR     ����ʧ��(����mkdir����ʧ�ܷ���)
*/

NEXPORT int NDK_FsCreateDirectory(const char *pszName)
{
    char buf[PATH_MAX];
    int ret;
    memset(buf,0x00,sizeof(buf));
    if( pszName == NULL)
        return NDK_ERR_PARA;
    ret=mkdir(pszName,0755);
    if(ret==0)
        return NDK_OK;
    else
        return NDK_ERR;
}

/**
 *@brief        ɾ��Ŀ¼.
 *@details
 *@param    pszName Ŀ¼����
 *@return
 *@li    NDK_OK             �����ɹ�����
 *@li    NDK_ERR_PARA       �����Ƿ�(pszNameΪNULL)
 *@li    NDK_ERR        ����ʧ��(����remove()ʧ�ܷ���)
*/

NEXPORT int NDK_FsRemoveDirectory(const char *pszName)
{
#if 0
    char buf[PATH_MAX];
    int ret;

    memset(buf,0x00,sizeof(buf));
    if( pszName == NULL)
        return NDK_ERR_PARA;
    sprintf(buf,"rm %s -r",pszName);
    ret = system(buf);

    if(ret==0)
        return NDK_OK;
    else
        return NDK_ERR;
#endif
    int ret;

    if (pszName == NULL)
        return NDK_ERR_PARA;

    ret = remove(pszName);

    if ( ret == 0 )
        return NDK_OK;
    else
        return NDK_ERR;
}
/**
 *@brief        �ļ�ϵͳ��ʽ��
 *@details  �ù��ܽ����ڴ�ͳpos��gpƽ̨posֱ�ӷ���-1
 *@return
 *@li    NDK_OK             �����ɹ�����
 *@li   NDK_ERR_NOT_SUPPORT     δ֧�ָù���
*/

NEXPORT int NDK_FsFormat(void)
{
    return NDK_ERR_NOT_SUPPORT;
}


/**
 *@brief        �г��ƶ�Ŀ¼�µ������ļ�
 *@details  �����psBuf�ĵ�sizeһ��Ҫ���󣬷�������������pbuf ÿ20���ֽڴ洢һ���ļ���
            ǰ19 Ϊ�ļ����������Զ��ض̡���20�ֽ����Ϊ1���ʾ���ļ�Ϊ�ļ��У�0Ϊ��ͨ�ļ�
 *@param       pPath ָ��Ҫ��ȡ��Ŀ¼
 *@retval        psBuf ���ļ����洢��pbuf�ܷ���
 *@retval        punNum ���ظ��ļ���Ŀ¼���ļ�����
 *@return
 *@li    NDK_OK             �����ɹ�
 *@li    NDK_ERR_PARA       �����Ƿ�(pPath��psBuf��punNumΪNULL)
 *@li    NDK_ERR_PATH       �ļ�·���Ƿ�(����opendir()ʧ�ܷ���)
*/
NEXPORT int NDK_FsDir(const char *pPath,char *psBuf,uint *punNum)
{
    DIR * thedir=NULL;
    struct dirent * ent=NULL;
    int ret = 0;
    char *p;

    if(psBuf == NULL||pPath==NULL||punNum==NULL)
        return NDK_ERR_PARA;

    thedir = opendir(pPath);
    if (!thedir)
        return NDK_ERR_PATH;
    p = (char*)psBuf;
    ent = readdir(thedir);
    while (ent) {
        if(strcmp(ent->d_name,".")==0||strcmp(ent->d_name,"..")==0) {
            ent = readdir(thedir);
            continue;
        } else {
            strncpy(p, ent->d_name, FILENAME_MAXLEN);
            *(p+FILENAME_MAXLEN) = (char)(ent->d_type==DT_DIR? 1:0);
            p += (FILENAME_MAXLEN+1);
            ret++;
            ent = readdir(thedir);

        }
    }
    if (thedir) closedir(thedir);
    //if(punNum !=NULL)
    *punNum = ret;
    return NDK_OK;

}

/* End of this file */

