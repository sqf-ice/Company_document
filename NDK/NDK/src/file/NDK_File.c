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
#include <fcntl.h>
#include <sys/vfs.h>


#include "NDK.h"
#include "NDK_debug.h"

#define USERBINFS_CHARDEV           "/dev/mtd5"
#define USERBINFS_BLKDEV            "/dev/mtdblock5"
#define USERBINFS_ROOT              "/appfs"
#define USERDATFS_CHARDEV           USERBINFS_CHARDEV
#define USERDATFS_BLKDEV            USERBINFS_BLKDEV
#define USERDATFS_ROOT              USERBINFS_ROOT
#define USER_BIN_PATH       USERBINFS_ROOT"/apps/"
#define USER_CONFIG_PATH    USERBINFS_ROOT"/etc/"
#define USER_DATA_PATH      USERDATFS_ROOT"/data/"
#define USER_LIB_PATH       USERBINFS_ROOT"/lib/"
#define TMP_PATH                    "/tmp/"


int ndk_fs_open[1024];
int ndk_fs_open_num = 0;
int ndk_fs_flag = 0;



/**
 *@brief        打开文件.
 *@details
 *@param    pszName 文件名
 *@param    pszMode 打开模式 "r"(以只读方式打开，如果不存在则失败) or "w"(以写的方式打开，如果文件不存在则创建)。
 *@return
 *@li    fd             操作成功返回文件描述符
 *@li   NDK_ERR_PARA        参数错误(文件名为NULL、模式为不正确)
 *@li   NDK_ERR_OPEN_DEV        文件打开失败
*/
NEXPORT int NDK_FsOpen(const char *pszName,const char *pszMode)
{
    int fd;
    //fprintf(stderr,"close : ndk_fs_open_num:%d\n",ndk_fs_open_num);
    //for(i=0;i<ndk_fs_open_num;i++)
    //  fprintf(stderr,"%d ",ndk_fs_open[i]);O_RSYNC

    if (pszName == NULL || pszMode==NULL) {
        return NDK_ERR_PARA;
    }
    if ( pszMode[0] == 'w' )
        fd = open(pszName, O_RDWR|O_CREAT|O_SYNC, 0666);
    else if ( pszMode[0] == 'r' )
        fd = open(pszName, O_RDONLY);
    else
        return NDK_ERR_PARA;

    if ( fd < 0 )
        return NDK_ERR_OPEN_DEV;
    else {
        if(ndk_fs_open_num == 1024)
            return NDK_ERR_OPEN_DEV;
        ndk_fs_open[ndk_fs_open_num] = fd;
        ndk_fs_open_num++;
        return fd;
    }
}

int ndk_fs_opened(int handle)
{
    int i;
    if(ndk_fs_open_num == 0)
        return 0;
    for (i=0; i<ndk_fs_open_num; i++) {
        if (ndk_fs_open[i] == handle) {
            ndk_fs_flag = i;
            return 1;
        }
    }
    ndk_fs_flag = 0;
    return 0;
}

/**
 *@brief        关闭文件.
 *@details
 *@param    nHandle 文件句柄
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR     操作失败(所关闭的文件不是调用NDK_FsOpen打开的、调用close()关闭失败)
*/
NEXPORT int NDK_FsClose(int nHandle)
{
    int ret,i;
    //fprintf(stderr,"close : ndk_fs_open_num:%d\n",ndk_fs_open_num);
    //for(i=0;i<ndk_fs_open_num;i++)
    //  fprintf(stderr,"%d ",ndk_fs_open[i]);
    if (!ndk_fs_opened(nHandle)) {
        return NDK_ERR;
    }
    ret=close(nHandle);
    if ( ret < 0 )
        return NDK_ERR;
    else {
        for(i=ndk_fs_flag; i <( ndk_fs_open_num-1); i++)
            ndk_fs_open[i] = ndk_fs_open[i+1];
        ndk_fs_open_num--;
        return NDK_OK;
    }
}

/**
 *@brief        从打开的nHandle文件当前指针读unLength个字符到缓冲区psBuffer.
 *@details
 *@param    nHandle 文件句柄
 *@param    unLength，需要读取的字符的长度
 *@retval    psBuffer，需要读入的缓冲区注意要足够读入length字节
 *@return
 *@li   实际读到数据长度            操作成功
 *@li   NDK_ERR_PARA                参数错误(psBuffer为NULL)
 *@li   NDK_ERR_READ        读文件失败(所读的文件不是调用NDK_FsOpen打开的、调用read()关闭失败)
*/
NEXPORT int NDK_FsRead(int nHandle, char *psBuffer, uint unLength )
{
    int ret;
    if (psBuffer == NULL)
        return NDK_ERR_PARA;
    if (!ndk_fs_opened(nHandle)) {
        return NDK_ERR_READ;
    }
    ret = read(nHandle, psBuffer, unLength);
    if ( ret < 0 ) {
        return NDK_ERR_READ;
    }

    return ret;
}

/**
 *@brief        向打开的nHandle文件写入unLength个字节.
 *@details
 *@param    nHandle 文件句柄
 *@param    psBuffer，需要写入文件内容的缓冲区
 *@param    unLength，需要写入的长度
 *@return
 *@li   实际写数据长度          操作成功
 *@li   NDK_ERR_PARA                参数错误(psBuffer为NULL)
 *@li   NDK_ERR_WRITE       写文件失败(所写的文件不是调用NDK_FsOpen打开的、调用write()关闭失败)
*/
NEXPORT int NDK_FsWrite(int nHandle, const char *psBuffer, uint unLength )
{
    int ret;

    if (psBuffer == NULL) {
        return NDK_ERR_PARA;
    }
    if (!ndk_fs_opened(nHandle)) {
        return NDK_ERR_WRITE;
    }
    ret = write(nHandle, psBuffer, unLength);
    if ( ret < 0)
        return NDK_ERR_WRITE;
    return ret;
}
/**
 *@brief        移动文件指针到从unPosition起距ulDistance的位置
 *@details
 *@param    nHandle 文件句柄
 *@param    ulDistance，根据参数unPosition来移动读写位置的位移数。
 *@param    unPosition，需要读取的字符的长度
                        SEEK_SET 参数offset即为新的读写位置。
                        SEEK_CUR 以目前的读写位置往后增加offset个位移量。
                        SEEK_END 将读写位置指向文件尾后再增加offset个位移量。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR     操作失败(所移动的文件不是调用NDK_FsOpen打开的、调用lseek()关闭失败)
*/
NEXPORT int NDK_FsSeek(int nHandle, ulong ulDistance, uint unPosition )
{
    long ret;

    if (!ndk_fs_opened(nHandle)) {
        return NDK_ERR;
    }
    ret = lseek(nHandle, ulDistance, unPosition);
    if ( ret < 0 )
        return NDK_ERR;
    else
        return NDK_OK;
}

/**
 *@brief        删除指定文件
 *@details
 *@param    pszName 要删除的文件名
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误(pszName为NULL)
 *@li   NDK_ERR             操作成功(调用remove()失败返回)
*/
NEXPORT int NDK_FsDel(const char *pszName)
{
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
 *@brief        文件长度
 *@details
 *@param    pszName 文件名
 *@retval   punSize 文件大小返回值
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误(pszName、punSize为NULL)
 *@li   NDK_ERR             操作成功(打开文件失败、调用fstat()失败返回)
*/
NEXPORT int NDK_FsFileSize(const char *pszName,uint *punSize)
{
    int ret;
    int fd;
    struct stat buf, *p=&buf;

    if(pszName == NULL||punSize==NULL)
        return NDK_ERR_PARA;
    fd = open(pszName, O_RDONLY);
    if ( fd < 0 )
        return NDK_ERR;
    ret=fstat( fd, p );
    if( ret < 0) {
        close(fd);
        return NDK_ERR;
    }
    *punSize = p->st_size;
    close(fd);
    return NDK_OK;

}
/**
 *@brief        文件重命名
 *@details
 *@param    pszsSrcname 原文件名
 *@param    pszDstname 目标文件名
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误(pszsSrcname、pszDstname为NULL)
 *@li   NDK_ERR         操作失败(调用rename()失败返回)
*/
NEXPORT int NDK_FsRename(const char *pszsSrcname, const char *pszDstname )
{
    int ret;
    if ((pszsSrcname == NULL) || (pszDstname == NULL)) {
        return NDK_ERR_PARA;
    }

    ret = rename(pszsSrcname, pszDstname);
    if (ret < 0)
        return NDK_ERR;
    else
        return NDK_OK;
}
/**
 *@brief        测试文件是否存在
 *@details
 *@param    pszName 文件名
 *@return
 *@li   NDK_OK              操作成功(文件存在)
 *@li   NDK_ERR_PARA        参数错误(pszName为NULL)
 *@li   NDK_ERR     操作失败(调用access()失败返回)
*/

NEXPORT int NDK_FsExist(const char *pszName)
{
    if (pszName==NULL)
        return NDK_ERR_PARA;

    if (access(pszName, F_OK) == 0)
        return NDK_OK;
    else
        return NDK_ERR;
}
/**
 *@brief        文件截短
 *@details   NDK_FsTruncate()会将参数pszPath 指定的文件大小改为参数unLen 指定的大小。如果原来的文件大小比参数unLen大，则超过的部分会被删去。
           如果原来文件的大小比unLen小的话，不足的部分将补上0xff
 *@param    pszPath 文件路径
 *@param    unLen 所要截短长度
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误(pszPath为NULL)
 *@li   NDK_ERR_PATH        文件路径非法
 *@li   NDK_ERR     操作失败(计算文件大小失败、调用lseek()失败返回、调用truncate()失败返回)
 *@li   NDK_ERR_WRITE       写文件失败(调用write()失败返回)
*/
NEXPORT int NDK_FsTruncate(const char *pszPath ,uint unLen )
{
    int ret,i;
    uint size;
    int fd;
    char c=0xff;

    if(pszPath == NULL)
        return NDK_ERR_PARA;
    ret=NDK_FsFileSize(pszPath,&size);
    if(ret!=0)
        return NDK_ERR;
    if(unLen > size) {
        fd = open(pszPath, O_RDWR);
        if ( fd < 0 )
            return NDK_ERR_PATH;
        ret = lseek(fd, 0, SEEK_END);
        if ( ret < 0 ) {
            close(fd);
            return NDK_ERR;
        }
        for(i = 0; i < (unLen-size); i++) {
            ret = write(fd, (char *)&c, 1);
            if(ret < 0) {
                close(fd);
                return NDK_ERR_WRITE;
            }
        }
        close(fd);
        return NDK_OK;
    }
    ret = truncate(pszPath,unLen);
    if(ret < 0 )
        return NDK_ERR;
    else
        return NDK_OK;
}
/**
 *@brief        读取文件流位置
 *@details   用来取得文件流目前的读写位置
 *@param    nHandle 文件句柄
 *@retval    pulRet 文件流位置
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误(pulRet为NULL)
 *@li   NDK_ERR             操作失败(操作的文件不是调用NDK_FsOpen打开的文件、调用lseek()失败返回)
*/
NEXPORT int NDK_FsTell(int nHandle,ulong *pulRet)
{
    long ret;

    if(pulRet==NULL) {
        return NDK_ERR_PARA;
    }
    if (!ndk_fs_opened(nHandle)) {
        return NDK_ERR;
    }
    ret = lseek(nHandle, 0, SEEK_CUR);
    if ( ret < 0 )
        return NDK_ERR;
    *pulRet = ret;

    return NDK_OK;
}

/**
 *@brief        取文件系统磁盘空间的使用情况
 *@details
 *@param    unWhich :0--已经使用的磁盘空间1--剩余的磁盘空间
 *@retval    pulSpace 返回磁盘空间使用量或者剩余量
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误(pulSpace为NULL)
 *@li   NDK_ERR             操作失败(调用statfs()失败返回)
*/
NEXPORT int NDK_FsGetDiskSpace(uint unWhich,ulong *pulSpace)
{
    struct statfs buf, *p=&buf;
    int ret;

    if(pulSpace==NULL)
        return NDK_ERR_PARA;
    //ret=statfs( USERDATFS_ROOT, p );
    ret=statfs( USER_BIN_PATH, p );
    if ( ret<0 ) {
        return NDK_ERR;
    }
    if (unWhich == 1) {
        /* when userfs_root is not mounted, f_bavail will be a invalid huge value */
        if (p->f_bavail > 0x80000) p->f_bavail = 0;
        *pulSpace=p->f_bsize*p->f_bavail;
    } else
        *pulSpace = p->f_bsize * (p->f_blocks - p->f_bavail);

    return NDK_OK;


}



