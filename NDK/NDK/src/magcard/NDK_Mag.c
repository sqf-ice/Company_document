/******************************************************************************************************
* �´�½��˾ ��Ȩ����(c) 2006-2008
*
* POS API
* ��ʾ���          api   --- NDK_Mag.c
* ��    �ߣ�        huds
* ��    �ڣ�        2012-09-21
* ����޸��ˣ�
* ����޸����ڣ�
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
*                                       �궨��
*********************************************************************************************************/
#define MAX_DATA_PER_TRACK          256                             /* �����Ĵŵ����� */
#define MAX_TRACK_DATA_BYTES        (MAX_DATA_PER_TRACK * 3)
#define DATAHEAD_OFFSET             2
#define DATAHEADLEN                 (DATAHEAD_OFFSET + 3)
/*********************************************************************************************************
*                                       ����
*********************************************************************************************************/
typedef enum {
    cr_NORMAL = 0,
    cr_RAW,
} CARDREAD_MODE;
static int nMagFd = -1;
static pthread_mutex_t mMagMutex = PTHREAD_MUTEX_INITIALIZER;
/*********************************************************************************************************
*                                       ��������
*********************************************************************************************************/
/**
 *@brief    �򿪴ſ��豸
 *@param    ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR         ����ʧ��(mag�豸�ڵ��Ѵ�)
 *@li   NDK_ERR_OPEN_DEV    ���豸�ļ�����(�򿪴ſ��豸�ļ�����)
 *@li   NDK_ERR_IOCTL       �������ô���(�ſ������ӿڵ���ʧ�ܷ���)
*/
NEXPORT int NDK_MagOpen(void)
{
    int nTracks = TK1_2_3;
    if(pthread_mutex_trylock(&mMagMutex) != 0) {        /* mag�豸�ڵ��Ѵ� */
        return NDK_ERR;
    }
    if ((nMagFd = open("/dev/mag", O_RDONLY|O_NONBLOCK)) < 0) {
        pthread_mutex_unlock(&mMagMutex);   /* ���� */
        return NDK_ERR_OPEN_DEV;
    }

    if(ioctl(nMagFd,MAG_IOCS_START,0) <0 ) {
        close(nMagFd);
        nMagFd = -1;
        pthread_mutex_unlock(&mMagMutex);   /* ���� */
        return NDK_ERR_OPEN_DEV;
    }

    fcntl(nMagFd, F_SETFD, FD_CLOEXEC);                 /* ��ʾ�����ݸ�exec�������½��� */
    if (ioctl(nMagFd, MAG_IOCG_TRACK, &nTracks) < 0) {
        close(nMagFd);
        nMagFd = -1;
        pthread_mutex_unlock(&mMagMutex);   /* ���� */
        return NDK_ERR_IOCTL;
    }
    return NDK_OK;
}

/**
 *@brief:   �رմſ��豸
 *@param:   ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR         ����ʧ��(�ſ��豸δ�򿪡�����close()ʧ�ܷ��ء���������ʧ��)
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
    pthread_mutex_unlock(&mMagMutex);   /* ���� */
    return NDK_OK;
}

/**
 *@brief    ��λ��ͷ
 *@details  ��λ��ͷ������ſ�����������
 *@param    ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR         ����ʧ��(�ſ�δ��)
 *@li   NDK_ERR_IOCTL       �������ô���(�ſ������ӿ�MAG_IOCS_RESET����ʧ�ܷ���)
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
 *@brief    �ж��Ƿ�ˢ����
 *@retval   pcSwiped    1----��ˢ��    0-----δˢ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA            �����Ƿ�(pcSwiped�Ƿ�)
 *@li   NDK_ERR             ����ʧ��(�ſ��豸δ��)
 *@li   NDK_ERR_IOCTL           �������ô���(�ſ������ӿ�MAG_IOCG_SWIPED����ʧ�ܷ���)
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
 *@brief    ��ȡ�ſ���������1��2��3�ŵ�������
 *@details  ��MagSwiped�������ʹ�á��������Ҫĳ�ŵ�����,���Խ��ôŵ���Ӧ��ָ����ΪNULL,��ʱ����������ôŵ�������
 *@retval   pszTk1  �ŵ�1
 *@retval   pszTk2  �ŵ�2
 *@retval   pszTk3  �ŵ�3
 *@retval   pnErrorCode �ſ��������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR         ����ʧ��(�ſ��豸δ��)
 *@li   NDK_ERR_IOCTL           �������ô���(�ſ������ӿڵ���ʧ�ܷ���)
 *@li   NDK_ERR_NOSWIPED        �޴ſ�ˢ����¼
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
        while(1) { //���Ҵŵ����ݷָ��'{'
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
 *@brief    ��ȡ�ſ���������1��2��3�ŵ���ԭʼ����
 *@details  ��MagSwiped�������ʹ��,�������Ҫĳ�ŵ�����,���Խ��ôŵ���Ӧ��ָ����ΪNULL,��ʱ����������ôŵ�������
 *@retval   pszTk1  �ŵ�1
 *@retval   punTk1Len   �ŵ�1���ݳ���
 *@retval   pszTk2  �ŵ�2
 *@retval   punTk2Len   �ŵ�2���ݳ���
 *@retval   pszTk3  �ŵ�3
 *@retval   punTk3Len   �ŵ�3���ݳ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA            �����Ƿ�(pszTk2/pszTk3/pszTk1ΪNULL������ΪNULL)
 *@li   NDK_ERR             ����ʧ��(�ſ��豸δ��)
 *@li   NDK_ERR_IOCTL           �������ô���(�ſ������ӿڵ���ʧ�ܷ���)
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
    if (ioctl(nMagFd, MAG_IOCG_CRMODE, &nMode) < 0) { /* ��������� */
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
