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
 *@brief        ���ļ�.
 *@details
 *@param    pszName �ļ���
 *@param    pszMode ��ģʽ "r"(��ֻ����ʽ�򿪣������������ʧ��) or "w"(��д�ķ�ʽ�򿪣�����ļ��������򴴽�)��
 *@return
 *@li    fd             �����ɹ������ļ�������
 *@li   NDK_ERR_PARA        ��������(�ļ���ΪNULL��ģʽΪ����ȷ)
 *@li   NDK_ERR_OPEN_DEV        �ļ���ʧ��
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
 *@brief        �ر��ļ�.
 *@details
 *@param    nHandle �ļ����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR     ����ʧ��(���رյ��ļ����ǵ���NDK_FsOpen�򿪵ġ�����close()�ر�ʧ��)
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
 *@brief        �Ӵ򿪵�nHandle�ļ���ǰָ���unLength���ַ���������psBuffer.
 *@details
 *@param    nHandle �ļ����
 *@param    unLength����Ҫ��ȡ���ַ��ĳ���
 *@retval    psBuffer����Ҫ����Ļ�����ע��Ҫ�㹻����length�ֽ�
 *@return
 *@li   ʵ�ʶ������ݳ���            �����ɹ�
 *@li   NDK_ERR_PARA                ��������(psBufferΪNULL)
 *@li   NDK_ERR_READ        ���ļ�ʧ��(�������ļ����ǵ���NDK_FsOpen�򿪵ġ�����read()�ر�ʧ��)
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
 *@brief        ��򿪵�nHandle�ļ�д��unLength���ֽ�.
 *@details
 *@param    nHandle �ļ����
 *@param    psBuffer����Ҫд���ļ����ݵĻ�����
 *@param    unLength����Ҫд��ĳ���
 *@return
 *@li   ʵ��д���ݳ���          �����ɹ�
 *@li   NDK_ERR_PARA                ��������(psBufferΪNULL)
 *@li   NDK_ERR_WRITE       д�ļ�ʧ��(��д���ļ����ǵ���NDK_FsOpen�򿪵ġ�����write()�ر�ʧ��)
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
 *@brief        �ƶ��ļ�ָ�뵽��unPosition���ulDistance��λ��
 *@details
 *@param    nHandle �ļ����
 *@param    ulDistance�����ݲ���unPosition���ƶ���дλ�õ�λ������
 *@param    unPosition����Ҫ��ȡ���ַ��ĳ���
                        SEEK_SET ����offset��Ϊ�µĶ�дλ�á�
                        SEEK_CUR ��Ŀǰ�Ķ�дλ����������offset��λ������
                        SEEK_END ����дλ��ָ���ļ�β��������offset��λ������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR     ����ʧ��(���ƶ����ļ����ǵ���NDK_FsOpen�򿪵ġ�����lseek()�ر�ʧ��)
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
 *@brief        ɾ��ָ���ļ�
 *@details
 *@param    pszName Ҫɾ�����ļ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������(pszNameΪNULL)
 *@li   NDK_ERR             �����ɹ�(����remove()ʧ�ܷ���)
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
 *@brief        �ļ�����
 *@details
 *@param    pszName �ļ���
 *@retval   punSize �ļ���С����ֵ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������(pszName��punSizeΪNULL)
 *@li   NDK_ERR             �����ɹ�(���ļ�ʧ�ܡ�����fstat()ʧ�ܷ���)
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
 *@brief        �ļ�������
 *@details
 *@param    pszsSrcname ԭ�ļ���
 *@param    pszDstname Ŀ���ļ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������(pszsSrcname��pszDstnameΪNULL)
 *@li   NDK_ERR         ����ʧ��(����rename()ʧ�ܷ���)
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
 *@brief        �����ļ��Ƿ����
 *@details
 *@param    pszName �ļ���
 *@return
 *@li   NDK_OK              �����ɹ�(�ļ�����)
 *@li   NDK_ERR_PARA        ��������(pszNameΪNULL)
 *@li   NDK_ERR     ����ʧ��(����access()ʧ�ܷ���)
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
 *@brief        �ļ��ض�
 *@details   NDK_FsTruncate()�Ὣ����pszPath ָ�����ļ���С��Ϊ����unLen ָ���Ĵ�С�����ԭ�����ļ���С�Ȳ���unLen���򳬹��Ĳ��ֻᱻɾȥ��
           ���ԭ���ļ��Ĵ�С��unLenС�Ļ�������Ĳ��ֽ�����0xff
 *@param    pszPath �ļ�·��
 *@param    unLen ��Ҫ�ض̳���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������(pszPathΪNULL)
 *@li   NDK_ERR_PATH        �ļ�·���Ƿ�
 *@li   NDK_ERR     ����ʧ��(�����ļ���Сʧ�ܡ�����lseek()ʧ�ܷ��ء�����truncate()ʧ�ܷ���)
 *@li   NDK_ERR_WRITE       д�ļ�ʧ��(����write()ʧ�ܷ���)
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
 *@brief        ��ȡ�ļ���λ��
 *@details   ����ȡ���ļ���Ŀǰ�Ķ�дλ��
 *@param    nHandle �ļ����
 *@retval    pulRet �ļ���λ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������(pulRetΪNULL)
 *@li   NDK_ERR             ����ʧ��(�������ļ����ǵ���NDK_FsOpen�򿪵��ļ�������lseek()ʧ�ܷ���)
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
 *@brief        ȡ�ļ�ϵͳ���̿ռ��ʹ�����
 *@details
 *@param    unWhich :0--�Ѿ�ʹ�õĴ��̿ռ�1--ʣ��Ĵ��̿ռ�
 *@retval    pulSpace ���ش��̿ռ�ʹ��������ʣ����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������(pulSpaceΪNULL)
 *@li   NDK_ERR             ����ʧ��(����statfs()ʧ�ܷ���)
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



