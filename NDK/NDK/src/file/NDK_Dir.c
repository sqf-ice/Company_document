/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：    产品开发部
* 日    期：    2012-08-17
* 版    本：    V1.00
* 最后修改人：
* 最后修改日期：
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
    NDK_OK=0,               /**<操作成功*/
    NDK_ERR=-1,         /**<操作失败*/
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
 *@brief        创建目录.
 *@details
 *@param    pszName 目录名称
 *@return
 *@li    NDK_OK             操作成功返回
 *@li   NDK_ERR_PARA        参数非法(pszName为NULL)
 *@li   NDK_ERR     操作失败(调用mkdir命令失败返回)
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
 *@brief        删除目录.
 *@details
 *@param    pszName 目录名称
 *@return
 *@li    NDK_OK             操作成功返回
 *@li    NDK_ERR_PARA       参数非法(pszName为NULL)
 *@li    NDK_ERR        操作失败(调用remove()失败返回)
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
 *@brief        文件系统格式化
 *@details  该功能仅限于传统pos上gp平台pos直接返回-1
 *@return
 *@li    NDK_OK             操作成功返回
 *@li   NDK_ERR_NOT_SUPPORT     未支持该功能
*/

NEXPORT int NDK_FsFormat(void)
{
    return NDK_ERR_NOT_SUPPORT;
}


/**
 *@brief        列出制定目录下的所有文件
 *@details  传入的psBuf的的size一定要够大，否则会出现溢出情况pbuf 每20个字节存储一个文件名
            前19 为文件名，超过自动截短。第20字节如果为1则表示该文件为文件夹，0为普通文件
 *@param       pPath 指定要读取的目录
 *@retval        psBuf 将文件名存储到pbuf总返回
 *@retval        punNum 返回该文件夹目录的文件总数
 *@return
 *@li    NDK_OK             操作成功
 *@li    NDK_ERR_PARA       参数非法(pPath、psBuf、punNum为NULL)
 *@li    NDK_ERR_PATH       文件路径非法(调用opendir()失败返回)
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

