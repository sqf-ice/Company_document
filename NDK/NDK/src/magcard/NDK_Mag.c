/******************************************************************************************************
* 新大陆公司 版权所有(c) 2006-2008
*
* POS API
* 显示相关          api   --- NDK_Mag.c
* 作    者：        huds
* 日    期：        2012-09-21
* 最后修改人：
* 最后修改日期：
******************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <pthread.h>
#include <sched.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <semaphore.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <fcntl.h>
#include <assert.h>
#include <linux/input.h>
#include <signal.h>

#include "NDK.h"
#include "NDK_debug.h"
#include "NDK_Mag.h"

/*********************************************************************************************************
*                                       宏定义
*********************************************************************************************************/
#define MAX_DATA_PER_TRACK          256                             /* 解码后的磁道数据 */
#define MAX_TRACK_DATA_BYTES        (MAX_DATA_PER_TRACK * 3)
#define DATAHEAD_OFFSET             2
#define DATAHEADLEN                 (DATAHEAD_OFFSET + 3)
/*********************************************************************************************************
*                                       变量
*********************************************************************************************************/
typedef enum {
    cr_NORMAL = 0,
    cr_RAW,
} CARDREAD_MODE;
static int nMagFd = -1;
static pthread_mutex_t mMagMutex = PTHREAD_MUTEX_INITIALIZER;
/*********************************************************************************************************
*                                       函数声明
*********************************************************************************************************/
/**
 *@brief    打开磁卡设备
 *@param    无
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR         操作失败(mag设备节点已打开)
 *@li   NDK_ERR_OPEN_DEV    打开设备文件错误(打开磁卡设备文件错误)
 *@li   NDK_ERR_IOCTL       驱动调用错误(磁卡驱动接口调用失败返回)
*/
NEXPORT int NDK_MagOpen(void)
{
    int nTracks = TK1_2_3;
    if(pthread_mutex_trylock(&mMagMutex) != 0) {        /* mag设备节点已打开 */
        return NDK_ERR;
    }
    if ((nMagFd = open("/dev/mag", O_RDONLY|O_NONBLOCK)) < 0) {
        pthread_mutex_unlock(&mMagMutex);   /* 解锁 */
        return NDK_ERR_OPEN_DEV;
    }

    if(ioctl(nMagFd,MAG_IOCS_START,0) <0 ) {
        close(nMagFd);
        nMagFd = -1;
        pthread_mutex_unlock(&mMagMutex);   /* 解锁 */
        return NDK_ERR_OPEN_DEV;
    }

    fcntl(nMagFd, F_SETFD, FD_CLOEXEC);                 /* 表示不传递给exec创建的新进程 */
    if (ioctl(nMagFd, MAG_IOCG_TRACK, &nTracks) < 0) {
        close(nMagFd);
        nMagFd = -1;
        pthread_mutex_unlock(&mMagMutex);   /* 解锁 */
        return NDK_ERR_IOCTL;
    }
    return NDK_OK;
}

/**
 *@brief:   关闭磁卡设备
 *@param:   无
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR         操作失败(磁卡设备未打开、调用close()失败返回、驱动调用失败)
*/
NEXPORT int NDK_MagClose(void)
{
    if(nMagFd < 0) {
        return NDK_ERR;
    }

    if(ioctl(nMagFd,MAG_IOCS_END,0) <0 ) {
        return NDK_ERR;
    }

    if( close(nMagFd) < 0) {
        return NDK_ERR;
    }
    nMagFd = -1;
    pthread_mutex_unlock(&mMagMutex);   /* 解锁 */
    return NDK_OK;
}

/**
 *@brief    复位磁头
 *@details  复位磁头且清除磁卡缓冲区数据
 *@param    无
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR         操作失败(磁卡未打开)
 *@li   NDK_ERR_IOCTL       驱动调用错误(磁卡驱动接口MAG_IOCS_RESET调用失败返回)
*/
NEXPORT int NDK_MagReset(void)
{
    if(nMagFd < 0) {
        return NDK_ERR;
    }
    if (ioctl(nMagFd, MAG_IOCS_RESET, 0) < 0) {
        return NDK_ERR_IOCTL;
    }
    return NDK_OK;
}

/**
 *@brief    判断是否刷过卡
 *@retval   pcSwiped    1----已刷卡    0-----未刷卡
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA            参数非法(pcSwiped非法)
 *@li   NDK_ERR             操作失败(磁卡设备未打开)
 *@li   NDK_ERR_IOCTL           驱动调用错误(磁卡驱动接口MAG_IOCG_SWIPED调用失败返回)
*/
NEXPORT int NDK_MagSwiped(uchar * pcSwiped)
{
    int nSwiped = 0;

    if(pcSwiped==NULL)
        return NDK_ERR_PARA;
    if(nMagFd < 0)
        return NDK_ERR;
    if (ioctl(nMagFd, MAG_IOCG_SWIPED, &nSwiped) < 0) {
        return NDK_ERR_IOCTL;
    }
    *pcSwiped = ((nSwiped == 0) ? 0 : 1);
    return NDK_OK;
}

/**
 *@brief    读取磁卡缓冲区的1、2、3磁道的数据
 *@details  与MagSwiped函数配合使用。如果不需要某磁道数据,可以将该磁道对应的指针置为NULL,这时将不会输出该磁道的数据
 *@retval   pszTk1  磁道1
 *@retval   pszTk2  磁道2
 *@retval   pszTk3  磁道3
 *@retval   pnErrorCode 磁卡错误代码
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR         操作失败(磁卡设备未打开)
 *@li   NDK_ERR_IOCTL           驱动调用错误(磁卡驱动接口调用失败返回)
 *@li   NDK_ERR_NOSWIPED        无磁卡刷卡记录
*/
NEXPORT int NDK_MagReadNormal(char *pszTk1, char *pszTk2, char *pszTk3, int *pnErrorCode)
{
    int nTracks = 0, i, nSwiped, nMode = cr_NORMAL, nErrorCode;
    char cData[MAX_TRACK_DATA_BYTES];
    char *p, *q;
    int len;
    /*
        if( pnErrorCode == NULL )
            return NDK_ERR_PARA;
        if( (pszTk1 == NULL) && (pszTk2 == NULL) && (pszTk3 == NULL) )
            return NDK_ERR_PARA;
    */
    if(nMagFd < 0) {
        return NDK_ERR;
    }
    if(pszTk1) {
        pszTk1[0]=0;
        nTracks |= TK1;
    }
    if(pszTk2) {
        pszTk2[0]=0;
        nTracks |= TK2;
    }
    if(pszTk3) {
        pszTk3[0]=0;
        nTracks |= TK3;
    }
    if (ioctl(nMagFd, MAG_IOCG_SWIPED, &nSwiped) < 0) {
        return NDK_ERR_IOCTL;
    }
    if( nSwiped == 0) {
        return NDK_ERR_NOSWIPED;
    }
    /*
    if (ioctl(nMagFd, MAG_IOCG_TRACK, &nTracks) < 0) {
        return NDK_CMDUNSUPPORTED;
    }*/
    if (ioctl(nMagFd, MAG_IOCG_CRMODE, &nMode) < 0) {
        return NDK_ERR_IOCTL;
    }
    if ((len=read(nMagFd, cData, MAX_TRACK_DATA_BYTES)) < 0) {
        return NDK_ERR;
    }
    if (ioctl(nMagFd, MAG_IOCG_STATUS, &nErrorCode) < 0) {
        return NDK_ERR_IOCTL;
    }
    if(pnErrorCode) {
        *pnErrorCode = nErrorCode;
    }
    p = q = cData;
    for(i = 0; i < 3; i++) {
        while(1) { //查找磁道数据分割符'{'
            if(p > len+cData) return NDK_ERR_SWIPED_DATA;
            if (*p == '{') {
                p++;
                break;
            }
            p++;
        }

        q=p;
        while(1) {
            if(q > len+cData) return NDK_ERR_SWIPED_DATA;
            if (*q == '}') {
                break;
            }
            q++;
        }

        *q = '\0';
        if((nTracks & TK1) && (i == 0))
            strcpy(pszTk1, p);
        if((nTracks & TK2) && (i == 1))
            strcpy(pszTk2, p);
        if((nTracks & TK3) && (i == 2))
            strcpy(pszTk3, p);
        p = q;
    }
    return NDK_OK;
}

/**
 *@brief    读取磁卡缓冲区的1、2、3磁道的原始数据
 *@details  与MagSwiped函数配合使用,如果不需要某磁道数据,可以将该磁道对应的指针置为NULL,这时将不会输出该磁道的数据
 *@retval   pszTk1  磁道1
 *@retval   punTk1Len   磁道1数据长度
 *@retval   pszTk2  磁道2
 *@retval   punTk2Len   磁道2数据长度
 *@retval   pszTk3  磁道3
 *@retval   punTk3Len   磁道3数据长度
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA            参数非法(pszTk2/pszTk3/pszTk1为NULL、长度为NULL)
 *@li   NDK_ERR             操作失败(磁卡设备未打开)
 *@li   NDK_ERR_IOCTL           驱动调用错误(磁卡驱动接口调用失败返回)
*/
NEXPORT int NDK_MagReadRaw(     uchar *pszTk1, ushort* punTk1Len,
                                uchar *pszTk2, ushort* punTk2Len,
                                uchar *pszTk3, ushort* punTk3Len )
{
    int nTracks = 0, nRet, nSwiped, nMode = cr_RAW;
    ushort unLen, unOffset, unTk1_Len, unTk2_Len, unTk3_Len;
    char cData[MAX_TRACK_DATA_BYTES];
    if( (pszTk2 == NULL) && (pszTk3 == NULL) && (pszTk1 == NULL) )
        return NDK_ERR_PARA;
    if(nMagFd < 0) {
        return NDK_ERR;
    }
    if(pszTk1) {
        if(punTk1Len == NULL) {
            return NDK_ERR_PARA;
        }
        //pszTk1[0] = 0;
        *punTk1Len = 0;
        nTracks |= TK1;
    }
    if(pszTk2) {
        if(punTk2Len == NULL) {
            return NDK_ERR_PARA;
        }
        //pszTk2[0] = 0;
        *punTk2Len = 0;
        nTracks |= TK2;
    }
    if(pszTk3) {
        if(punTk3Len == NULL) {
            return NDK_ERR_PARA;
        }
        //pszTk3[0] = 0;
        *punTk3Len = 0;
        nTracks |= TK3;
    }
    if (ioctl(nMagFd, MAG_IOCG_SWIPED, &nSwiped) < 0) {
        return NDK_ERR_IOCTL;
    }
    if( nSwiped == 0) {
        return NDK_ERR_NOSWIPED;
    }
    if (ioctl(nMagFd, MAG_IOCG_CRMODE, &nMode) < 0) { /* 解码后数据 */
        return NDK_ERR_IOCTL;
    }
    /*
        if (ioctl(nMagFd, MAG_IOCG_TRACK, &nTracks) < 0) {
            return NDK_CMDUNSUPPORTED;
        }
    */
    if((nRet = read(nMagFd, cData, MAX_TRACK_DATA_BYTES)) < 0) {
        return NDK_ERR;
    }
    unLen = (ushort)cData[DATAHEAD_OFFSET+0] +
            (ushort)cData[DATAHEAD_OFFSET+1] +
            (ushort)cData[DATAHEAD_OFFSET+2] +
            DATAHEADLEN;
    if(unLen != nRet) {
        return NDK_ERR;
    }
    unTk1_Len = cData[DATAHEAD_OFFSET+2];
    unTk2_Len = cData[DATAHEAD_OFFSET+0];
    unTk3_Len = cData[DATAHEAD_OFFSET+1];
    if((nTracks & TK1) && (unTk1_Len > 0)) {
        unOffset = unTk2_Len+ unTk3_Len+ DATAHEADLEN;
        memcpy(pszTk1, &cData[unOffset], unTk1_Len);
        *punTk1Len = unTk1_Len;
    }
    if((nTracks & TK2) && (unTk2_Len > 0)) {
        unOffset = DATAHEADLEN;
        memcpy(pszTk2, &cData[unOffset], unTk2_Len);
        *punTk2Len = unTk2_Len;
    }
    if((nTracks & TK3) && (unTk3_Len > 0)) {
        unOffset = unTk2_Len + DATAHEADLEN;
        memcpy(pszTk3, &cData[unOffset], unTk3_Len);
        *punTk3Len = unTk3_Len;
    }
    return NDK_OK;
}

/* end of this file */
