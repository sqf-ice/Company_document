/*************************************************************************************************
 * �´�½֧��������˾ ��Ȩ����(c) 2012-20xx
 *
 * PHENIX ƽ̨�ǽӴ����ӿ�
 * �ṹ���壺       NDK_RFID.c
 * ��    �ߣ�       Steven Lin
 * ��    �ڣ�       2012-9-25
 * ����޸��ˣ�
 * ����޸����ڣ�   2012-9-25
 **************************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <termio.h>
#include <fcntl.h>
#include <linux/input.h>
#include "NDK.h"
#include "NDK_debug.h"
#include "../public/config.h"

/*********************************************************************************************************
 *                                       �궨��
*********************************************************************************************************/
#define RFID_IOC_MAGIC              188

#define	ioctl_isoRatsA				_IOWR(RFID_IOC_MAGIC, 46, uchar *)
#define ioctl_PiccDeselect          _IOWR(RFID_IOC_MAGIC, 50, uchar *)
#define ioctl_PiccQuickRequest      _IOWR(RFID_IOC_MAGIC, 61, uchar *)
#define ioctl_SetIgnoreProtocol     _IOWR(RFID_IOC_MAGIC, 62, uchar *)
#define ioctl_GetIgnoreProtocol     _IOWR(RFID_IOC_MAGIC, 63, uchar *)
#define ioctl_Get_rfid_ic_type      _IOWR(RFID_IOC_MAGIC, 70, uchar *)

#define RFID_IOCG_SUSPEND           _IOWR(RFID_IOC_MAGIC, 140, uchar *)
#define RFID_IOCG_RESUME            _IOWR(RFID_IOC_MAGIC, 141, uchar *)
#define RFID_IOCG_WORKSTATUS        _IOWR(RFID_IOC_MAGIC, 142, uchar *)

#define RFID_IOCG_GETVISION         _IOWR(RFID_IOC_MAGIC, 158, uchar *)
#define RFID_IOCG_INIT              _IOWR(RFID_IOC_MAGIC, 159, uchar *)
#define RFID_IOCG_RFOPEN            _IOWR(RFID_IOC_MAGIC, 160, uchar *)
#define RFID_IOCG_RFCLOSE           _IOWR(RFID_IOC_MAGIC, 161, uchar *)
#define RFID_IOCG_PICCSTATUE        _IOWR(RFID_IOC_MAGIC, 162, uchar *)
#define RFID_IOCG_SETPICCTYPE       _IOWR(RFID_IOC_MAGIC, 163, uchar *)
#define RFID_IOCG_PICCDEDECT        _IOWR(RFID_IOC_MAGIC, 164, uchar *)
#define RFID_IOCG_PICCACTIVATE      _IOWR(RFID_IOC_MAGIC, 165, uchar *)
#define RFID_IOCG_PICCAPDU          _IOWR(RFID_IOC_MAGIC, 166, uchar *)
#define RFID_IOCG_M1REQUEST         _IOWR(RFID_IOC_MAGIC, 167, uchar *)
#define RFID_IOCG_M1ANTI            _IOWR(RFID_IOC_MAGIC, 168, uchar *)
#define RFID_IOCG_M1SELECT          _IOWR(RFID_IOC_MAGIC, 169, uchar *)
#define RFID_IOCG_M1KEYSTORE        _IOWR(RFID_IOC_MAGIC, 170, uchar *)
#define RFID_IOCG_M1KEYLOAD         _IOWR(RFID_IOC_MAGIC, 171, uchar *)
#define RFID_IOCG_M1INTERAUTHEN     _IOWR(RFID_IOC_MAGIC, 172, uchar *)
#define RFID_IOCG_M1EXTERAUTHEN     _IOWR(RFID_IOC_MAGIC, 173, uchar *)
#define RFID_IOCG_M1READ            _IOWR(RFID_IOC_MAGIC, 174, uchar *)
#define RFID_IOCG_M1WRITE           _IOWR(RFID_IOC_MAGIC, 175, uchar *)
#define RFID_IOCG_M1INCREMENT       _IOWR(RFID_IOC_MAGIC, 176, uchar *)
#define RFID_IOCG_M1DECREMENT       _IOWR(RFID_IOC_MAGIC, 177, uchar *)
#define RFID_IOCG_M1RESTORE         _IOWR(RFID_IOC_MAGIC, 178, uchar *)
#define RFID_IOCG_M1TRANSFER        _IOWR(RFID_IOC_MAGIC, 179, uchar *)
#define RFID_IOCG_TIMEOUTCTL        _IOWR(RFID_IOC_MAGIC, 180, uchar *)
#define RFID_IOCG_PICCACTIVATE_EMV  _IOWR(RFID_IOC_MAGIC, 181, uchar *)

#define RFID_IOCG_GETRFSTATUS       _IOWR(RFID_IOC_MAGIC, 212, uchar *)
#define RFID_IOCG_TRANSCEIVE        _IOWR(RFID_IOC_MAGIC, 213, uchar *)
#define RFID_IOCG_FELICA_POLL       _IOWR(RFID_IOC_MAGIC, 214, uchar *)
#define RFID_IOCG_FELICA_TRANSCEIVE _IOWR(RFID_IOC_MAGIC, 215, uchar *)



#define  FAIL               (-1)
#define  SUCC               0

#define argsize             512

#define PICCTYPE_A          0xCC
#define PICCTYPE_B          0xCB
#define PICCTYPE_AB         0xCD

#define PICC_ANTICOLL1      0x93
#define PICC_ANTICOLL2      0x95
#define PICC_ANTICOLL3      0x97


/*********************************************************************************************************
*                                       ���ͼ�����
*********************************************************************************************************/
static struct arg_t {
    int     nStatus;                        /**< ���ص�״ֵ̬*/
    uint    unCmdarg_len;                   /**< �������ֽ���*/
    uchar   ucCmdarg[argsize];              /**< ioctl������߻��͵Ĳ���*/
} s_arg;

struct arg_tm {
    int    nStatus;
    uchar  ucAuth_mode;                     /**< KEYA,B */
    ushort   unEestartadr;                      /**< EEPROM����ʼ��ַ */
    uchar  ucKeysector;                     /**< EEPROM��key���������� */
    uchar  ucLength_key;                        /**< key[argsize]��key���ȣ�Ҳ���������� */
    uchar  ucKey[argsize];                  /**< key ���� ���� */
    uchar  ucLength_snr;                        /**< snr[argsize]�����кų��� */
    uchar  ucSnr[argsize];                  /**< ���к� */
    uchar  ucBlock;                         /**< Ҫ��֤�Ŀ�� */
} m_arg;

int     nErrorCode=FAIL;                    /**< ������ */
int     nDevnum_Rfid=FAIL;                  /**< �ļ���� */
uchar   ucA_snr[16];                            /**< UID���� */
uchar   ucSnrlen=0;                         /**< UID���ݳ��� */

/*********************************************************************************************************
*                                       �ڲ�����˵��
*********************************************************************************************************/
static int IccOpen_Rfid(void);
static int IccClose_Rfid(void);
static int IccErrPro(int nErrret);
static int ICCM1ErrPRO(int nErrcode);

/*********************************************************************************************************
*                                       ����
*********************************************************************************************************/

/**
 *@brief    ��ȡ��Ƶ�����汾��
 *@param    pszVersion: ���ص������汾���ַ���
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pszVersionΪNULL)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_GETVISION����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidVersion(uchar *pszVersion)
{
    int nRet;

    if (pszVersion == NULL) {
        return NDK_ERR_PARA;
    }

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len=0;
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_GETVISION, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if(s_arg.unCmdarg_len>15) {
            s_arg.unCmdarg_len = 15;
        }
        memcpy(pszVersion, s_arg.ucCmdarg, s_arg.unCmdarg_len);
        pszVersion[s_arg.unCmdarg_len]=0;
        nRet =  NDK_OK;
    }

    IccClose_Rfid();

    return nRet;
}


/**
 *@brief    ��Ƶ�ӿ�������ʼ����Ҳ�������ж������Ƿ�װ���ɹ���
 *@param
 *@retval   psStatus:   ���嶨����������
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_INIT����ʧ�ܷ���)
 *@li   NDK_ERR_RFID_INITSTA    �ǽӴ���-��Ƶ�ӿ��������ϻ���δ����
*/
NEXPORT int  NDK_RfidInit(uchar *psStatus)
{
    int nRet;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_INIT, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (s_arg.nStatus == SUCC) {
            if(psStatus)
                *psStatus = s_arg.ucCmdarg[0];
            nRet =  NDK_OK;
        } else {
            nRet = NDK_ERR_RFID_INITSTA;
        }
    }

    IccClose_Rfid();

    return nRet;
}

/**
 *@brief    ����Ƶ���
 *@param
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR         ����ʧ��
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_RFOPEN����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidOpenRf(void)
{
    int nRet;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_RFOPEN, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (s_arg.nStatus == SUCC) {
            nRet =  NDK_OK;
        } else {
            nRet = NDK_ERR;
        }
    }

    IccClose_Rfid();

    return nRet;
}

/**
 *@brief    �ر���Ƶ������ɽ��͹��Ĳ�������Ƶ������
 *@param
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR         ����ʧ��
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_RFCLOSE����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidCloseRf(void)
{
    int nRet;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_RFCLOSE, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (s_arg.nStatus == SUCC) {
            nRet =  NDK_OK;
        } else {
            nRet = NDK_ERR;
        }
    }

    IccClose_Rfid();

    return nRet;
}


/**
 *@brief    ��ȡ��Ƭ״̬(�Ƿ���Ѱ���������ж��Ƿ������)
 *@param
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�(�ϵ�ɹ�)
 *@li   NDK_ERR_RFID_NOTACTIV   �ǽӴ���-δ����(δ�ϵ�ɹ�)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_WORKSTATUS����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidPiccState(void)
{
    int nRet;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_WORKSTATUS, &s_arg);
    if (nRet == SUCC) { //��
        nRet = NDK_OK;
    } else {            //æ
        nRet =  NDK_ERR_RFID_NOTACTIV;
    }

    IccClose_Rfid();

    return nRet;
}

/**
 *@brief    �豸ǿ������
 *@param
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_SUSPEND����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidSuspend(void)
{
    int nRet;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_SUSPEND, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    }

    IccClose_Rfid();

    return nRet;
}


/**
 *@brief    �豸����
 *@param
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_RESUME����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidResume(void)
{
    int nRet;


    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_RESUME, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    }

    IccClose_Rfid();

    return nRet;
}

/**
 *@brief    ���ó�ʱ����
 *@param����ucTimeOutCtl ֵ���ǲ�ִ�У�ֵ��Ϊ����ִ��
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_TIMEOUTCTL����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidTimeOutCtl(uchar ucTimeOutCtl)
{
    int nRet;




    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.ucCmdarg[0] = ucTimeOutCtl;
    s_arg.unCmdarg_len = 1;
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_TIMEOUTCTL, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = s_arg.nStatus;
    }

    IccClose_Rfid();

    return nRet;
}



/**
 *@brief    ����Ѱ������(Ѱ������ǰ����һ�μ��ɣ�M1������ʱ��Ҫ���ó�TYPE-A��ģʽ)
 *@param    ucPicctype =0xcc����ʾѰ��ʱֻ���TYPE-A�Ŀ�
 *                      0xcb����ʾѰ��ʱֻ���TYPE-B�Ŀ�
 *                      0xcd����ʾѰ��ʱ���TYPE-A��TYPE-B�Ŀ�
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(ucPicctype�Ƿ�)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_SETPICCTYPE����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidPiccType(uchar ucPicctype)
{
    int nRet;
	static int ingore_ctrl = -1;
	int mode = 0;
    if ((ucPicctype!=PICCTYPE_A) && (ucPicctype!=PICCTYPE_B) && (ucPicctype!=PICCTYPE_AB)) {
        return NDK_ERR_PARA;
    }
	if(ingore_ctrl == -1) ndk_getconfig("smartcard", "rfid_ingore_ctrl", CFG_INT, &ingore_ctrl);
	if(ingore_ctrl == 1) {
		NDK_GetIgnoreProtocol(&mode);
		if(mode == 0) NDK_SetIgnoreProtocol(1);
	}
	
    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.ucCmdarg[0] = ucPicctype;
    s_arg.unCmdarg_len = 1;
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_SETPICCTYPE, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = s_arg.nStatus;
    }

    IccClose_Rfid();

    return nRet;
}


/**
 *@brief    ��Ƶ��Ѱ��(̽��ˢ�������Ƿ��п�,Ѱ������֮ǰ��Ҫ�����ù�һ��)
 *@param
 *@retval   psPicctype: ����Ŀ����� 0xcc=TYPE-A����0xcb=TYPE-B��
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_PICCDEDECT����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidPiccDetect(uchar *psPicctype)
{
    int nRet;


    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_PICCDEDECT, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = s_arg.nStatus;           /**< ���п��ܵ�ʧ�ܴ��룬���޿����࿨��*/
        if (s_arg.nStatus==SUCC) {
        } else {
            nRet = IccErrPro(s_arg.nStatus);

        }
    }

    IccClose_Rfid();

    return nRet;
}


/**
 *@brief    ��Ƶ������(��̽���п��������),�൱��powerup , ���Ľ������̣�ԭ�������汾����
 *@param
 *@retval   psPicctype: ����Ŀ����� 0xcc=TYPE-A����0xcb=TYPE-B��
 *          pnDatalen:  ���ݳ���
 *          psDatabuf:  ���ݻ�����(A��ΪUID��B��databuf[1]~[4]ΪUID��������Ӧ�ü�Э����Ϣ)
 *@return
 *@li   NDK_OK          ����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psPicctype/pnDatalen/psDatabufΪNULL)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_PICCACTIVATE����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidPiccActivate(uchar *psPicctype, int *pnDatalen,  uchar *psDatabuf)
{
    int nRet;

    if ( (psPicctype == NULL) || (pnDatalen == NULL) || (psDatabuf == NULL)) {
        return NDK_ERR_PARA;
    }

    *psPicctype = 0;
    *pnDatalen = 0;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_PICCACTIVATE, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = s_arg.nStatus;       /**< ���п��ܵ�ʧ�ܴ��룬���޿����࿨��*/
        if (s_arg.nStatus==SUCC) {
            *psPicctype = s_arg.ucCmdarg[0];
            *pnDatalen = s_arg.ucCmdarg[1];
            memcpy(psDatabuf, &s_arg.ucCmdarg[2], s_arg.ucCmdarg[1]);
        } else {
            nRet = IccErrPro(s_arg.nStatus);

        }
    }

    IccClose_Rfid();

    return nRet;

}
/**
 *@brief	��ƵCPU A���������M1��Ѱ��������ͻ��ѡ���ӿں���ü��Ƭ��
 *@param    cid��RATS�����cid�ַ���Ĭ�Ͽ�Ƭ����0��3911ƽ̨Ŀǰ���Դ˲�����
 *@retval 	
 *			pnDatalen:	���ݳ���
 *			psDatabuf:	���ݻ�����(A����ats)
 *@return
 *@li	NDK_OK			����ɹ�
 *@li	NDK_ERR_PARA		�����Ƿ�(pnDatalen/psDatabufΪNULL)
 *@li	NDK_ERR_OPEN_DEV	�豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li	NDK_ERR_IOCTL		�������ô���(��Ƶ�����ӿ�ioctl_isoRatsA����ʧ�ܷ���)
 *@li   �����������M1���ӿ�һ�£�������żУ�����crc���󡢳�ʱ����ȡ�	
*/

NEXPORT int  NDK_RfidTypeARats(uchar cid,int *pnDatalen, uchar *psDatabuf)
{
	int nRet = 0;
	if((pnDatalen == NULL) || (psDatabuf == NULL)) return NDK_ERR_PARA;
	*pnDatalen = 0;
	nRet = IccOpen_Rfid();
    if(nRet < 0) return NDK_ERR_OPEN_DEV;
	s_arg.unCmdarg_len = 1;
	s_arg.ucCmdarg[0] = cid;
	nRet = ioctl(nDevnum_Rfid, ioctl_isoRatsA, &s_arg);
	if(nRet != SUCC) nRet = NDK_ERR_IOCTL;
	else {
		nRet = s_arg.nStatus;
		if(nRet == SUCC) {
			*pnDatalen = s_arg.unCmdarg_len;
			memcpy(psDatabuf, &s_arg.ucCmdarg[0], *pnDatalen);
		}
		else nRet = ICCM1ErrPRO(s_arg.nStatus);
	}
	IccClose_Rfid();
	return nRet;
}


/**
 *@brief    ��Ƶ������(��̽���п��������),�൱��powerup ,��EMV L1 MAINLOOP���̡�
 *@param
 *@retval   psPicctype: ����Ŀ����� 0xcc=TYPE-A����0xcb=TYPE-B��
 *          pnDatalen:  ���ݳ���
 *          psDatabuf:  ���ݻ�����(A��ΪUID��B��databuf[1]~[4]ΪUID��������Ӧ�ü�Э����Ϣ)
 *@return
 *@li   NDK_OK          ����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psPicctype/pnDatalen/psDatabufΪNULL)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_PICCACTIVATE_EMV����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidPiccActivate_EMV(uchar *psPicctype, int *pnDatalen,  uchar *psDatabuf)
{
    int nRet;

    if ( (psPicctype == NULL) || (pnDatalen == NULL) || (psDatabuf == NULL)) {
        return NDK_ERR_PARA;
    }

    *psPicctype = 0;
    *pnDatalen = 0;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_PICCACTIVATE_EMV, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = s_arg.nStatus;       /**< ���п��ܵ�ʧ�ܴ��룬���޿����࿨��*/
        if (s_arg.nStatus==SUCC) {
            *psPicctype = s_arg.ucCmdarg[0];
            *pnDatalen = s_arg.ucCmdarg[1];
            memcpy(psDatabuf, &s_arg.ucCmdarg[2], s_arg.ucCmdarg[1]);
        } else {
            nRet = IccErrPro(s_arg.nStatus);

        }
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    �ر���Ƶʹ��ʧЧ����д����������Ӧ��ִ�иò������൱��powerdown ��
 *@param    ucDelayms:  ��0��һֱ�ر�RF;����0��ر�ucDelayms������ٴ�RF��
 *                      �ر�6��10ms��ʹ��ʧЧ���������û�������Ķ���������Ӧ����0�Թر�RF ��
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�ioctl_PiccDeselect����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidPiccDeactivate(uchar ucDelayms)
{
    int nRet;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }
    s_arg.ucCmdarg[0] = 0;
    s_arg.ucCmdarg[1] = ucDelayms;
    s_arg.ucCmdarg[2] = 0;

    s_arg.unCmdarg_len = 3;
    nRet = ioctl(nDevnum_Rfid, ioctl_PiccDeselect, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = s_arg.nStatus;
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    ��Ƶ��APDU����,����д������(�Ѽ���������)
 *@param    nSendlen    ���͵������
 *          psSendbuf   �����������
 *@retval   pnRecvlen:  �������ݳ���
 *          psRecebuf:  �������ݻ�����
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psSendbuf/pnRecvlen/psRecebufΪNULL��nSendlenС��5)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_PICCAPDU����ʧ�ܷ���)
*/
NEXPORT int  NDK_RfidPiccApdu(int nSendlen, uchar *psSendbuf, int *pnRecvlen,  uchar *psRecebuf)
{
    int nRet;

    if ((nSendlen < 5) || (psSendbuf == NULL) || (pnRecvlen == NULL) || (psRecebuf == NULL)) {
        return NDK_ERR_PARA;
    }

    *pnRecvlen=0;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        NDK_LOG_DEBUG(NDK_LOG_MODULE_RFID,"%s fail,error return-%s\n",__func__,"NDK_ERR_OPEN_DEV");
        return NDK_ERR_OPEN_DEV;
    }

    if(nSendlen>argsize)nSendlen=argsize;
    memcpy(s_arg.ucCmdarg, psSendbuf, nSendlen);
    s_arg.unCmdarg_len = nSendlen;
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_PICCAPDU, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = s_arg.nStatus;
        if (s_arg.nStatus==SUCC) {
            *pnRecvlen = s_arg.unCmdarg_len;
            memcpy(psRecebuf, s_arg.ucCmdarg, s_arg.unCmdarg_len);
        }
    }

    IccClose_Rfid();
    //NDK_LOG_DEBUG(NDK_LOG_MODULE_RFID,"%s fail,error return-%d\n",__func__,nRet);
    return nRet;

}

/****************************************************
*   M1��
****************************************************/

/**
 *@brief    M1Ѱ��(Ѱ�����ͱ����Ѿ�����ΪTYPE-A)
 *@param    ucReqcode:  0=����REQA, ��0=����WUPA, һ���������Ҫʹ��WUPA
 *@retval   pnDatalen:  �������ݳ���(2�ֽ�)
 *          psDatabuf:  �������ݻ�����
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pnDatalen/psDatabufΪNULL)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1REQUEST����ʧ�ܷ���)
*/
NEXPORT int  NDK_M1Request(uchar ucReqcode, int *pnDatalen, uchar *psDatabuf)
{
    int nRet;

    if ((pnDatalen == NULL) || (psDatabuf == NULL)) {
        return NDK_ERR_PARA;
    }

    *pnDatalen=0;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len = 1;
    if (ucReqcode == 0) { /**< REQA*/
        s_arg.ucCmdarg[0] = 0x26;
    } else {            /**< WUPA*/
        s_arg.ucCmdarg[0] = 0x52;
    }

    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1REQUEST, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = s_arg.nStatus;
        if (s_arg.nStatus==SUCC) {
            if (s_arg.unCmdarg_len!=2) {
                s_arg.unCmdarg_len = 2;
            }
            *pnDatalen = s_arg.unCmdarg_len;
            memcpy(psDatabuf, s_arg.ucCmdarg, s_arg.unCmdarg_len);
            ucSnrlen = s_arg.unCmdarg_len;
            memcpy(psDatabuf, s_arg.ucCmdarg, s_arg.unCmdarg_len);//??????????????????????
        } else {
            nRet = ICCM1ErrPRO(s_arg.nStatus);
        }
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    M1������ͻ(NDK_M1Request�п��������)
 *@param
 *@retval   pnDatalen:  �������ݳ���(UID����)
 *          psDatabuf:  �������ݻ�����(UID)
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pnDatalen/psDatabufΪNULL)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1ANTI����ʧ�ܷ���)
*/
NEXPORT int  NDK_M1Anti(int *pnDatalen, uchar *psDatabuf)
{
    int nRet;


    if ((pnDatalen == NULL) || (psDatabuf == NULL)) {
        return NDK_ERR_PARA;
    }

    *pnDatalen=0;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len = 2;
    s_arg.ucCmdarg[0] = PICC_ANTICOLL1;
    s_arg.ucCmdarg[1] = 0;
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1ANTI, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = s_arg.nStatus;
        if (s_arg.nStatus==SUCC) {
            if (s_arg.unCmdarg_len!=4) {
                s_arg.unCmdarg_len = 4;
            }
            *pnDatalen = s_arg.unCmdarg_len;
            memcpy(psDatabuf, s_arg.ucCmdarg, s_arg.unCmdarg_len);
            ucSnrlen = s_arg.unCmdarg_len;
            memcpy(psDatabuf, s_arg.ucCmdarg, s_arg.unCmdarg_len);//?????????????????
        } else {
            nRet = ICCM1ErrPRO(s_arg.nStatus);
        }
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    M1������ͻ(NDK_M1Request�п��������),��Զ༶������UID .
 *@param    ucSelcode:  PICC_ANTICOLL1/PICC_ANTICOLL2/PICC_ANTICOLL3
 *@retval   pnDatalen:  �������ݳ���(UID����)
 *          psDatabuf:  �������ݻ�����(UID)
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pnDatalen/psDatabufΪNULL��ucSelcode�Ƿ�)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1ANTI����ʧ�ܷ���)
*/
NEXPORT int  NDK_M1Anti_SEL(uchar ucSelcode, int *pnDatalen, uchar *psDatabuf)
{
    int nRet;


    if ((pnDatalen == NULL) || (psDatabuf == NULL) ||
        ((ucSelcode!=PICC_ANTICOLL1)&&(ucSelcode!=PICC_ANTICOLL2)&&(ucSelcode!=PICC_ANTICOLL3))
       ) {
        return NDK_ERR_PARA;
    }


    *pnDatalen=0;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len = 2;
    s_arg.ucCmdarg[0] = ucSelcode;
    s_arg.ucCmdarg[1] = 0;
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1ANTI, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = s_arg.nStatus;
        if (s_arg.nStatus==SUCC) {
            if (s_arg.unCmdarg_len!=4) {
                s_arg.unCmdarg_len = 4;
            }
            *pnDatalen = s_arg.unCmdarg_len;
            memcpy(psDatabuf, s_arg.ucCmdarg, s_arg.unCmdarg_len);
            ucSnrlen = s_arg.unCmdarg_len;
            memcpy(psDatabuf, s_arg.ucCmdarg, s_arg.unCmdarg_len);//?????????????????
        } else {
            nRet = ICCM1ErrPRO(s_arg.nStatus);
        }
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    M1��ѡ��(NDK_M1Anti�ɹ��������)
 *@param    nUidlen:    uid����
 *          psUidbuf:   uid���ݻ�����
 *@retval   psSakbuf:   ѡ����������(1�ֽ�SAK)
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psUidbuf/psSakbufΪNULL��nUidlen������4)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1SELECT����ʧ�ܷ���)
*/
NEXPORT int  NDK_M1Select(int nUidlen, uchar *psUidbuf, uchar *psSakbuf)
{
    int nRet;

    if ((nUidlen!=4) || (psUidbuf == NULL) || (psSakbuf == NULL)) {
        return NDK_ERR_PARA;
    }

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len = 5;
    s_arg.ucCmdarg[0] = PICC_ANTICOLL1;
    memcpy(&s_arg.ucCmdarg[1], psUidbuf, 4);
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1SELECT, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = s_arg.nStatus;
        if (s_arg.nStatus==SUCC) {
            psSakbuf[0] = s_arg.ucCmdarg[0];
        } else {
            nRet = ICCM1ErrPRO(s_arg.nStatus);
        }
    }

    IccClose_Rfid();

    return nRet;

}

/**
 *@brief    M1��ѡ��(NDK_M1Anti�ɹ��������)����Զ༶������UID .
 *@param    ucSelcode   PICC_ANTICOLL1/PICC_ANTICOLL2/PICC_ANTICOLL3
 *          nUidlen:    uid����
 *          psUidbuf:   uid���ݻ�����
 *@retval   psSakbuf:   ѡ����������(1�ֽ�SAK)
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psUidbuf/psSakbufΪNULL��nUidlen������4��ucSelcode�Ƿ�)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1SELECT����ʧ�ܷ���)
*/
NEXPORT int  NDK_M1Select_SEL(uchar ucSelcode, int nUidlen, uchar *psUidbuf, uchar *psSakbuf)
{
    int nRet;

    if ( (nUidlen!=4) || (psUidbuf == NULL) || (psSakbuf == NULL) ||
         ((ucSelcode!=PICC_ANTICOLL1)&&(ucSelcode!=PICC_ANTICOLL2)&&(ucSelcode!=PICC_ANTICOLL3))
       ) {
        return NDK_ERR_PARA;
    }

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len = 5;
    s_arg.ucCmdarg[0] = ucSelcode;
    memcpy(&s_arg.ucCmdarg[1], psUidbuf, 4);
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1SELECT, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = s_arg.nStatus;
        if (s_arg.nStatus==SUCC) {
            psSakbuf[0] = s_arg.ucCmdarg[0];
        } else {
            nRet = ICCM1ErrPRO(s_arg.nStatus);
        }
    }

    IccClose_Rfid();

    return nRet;

}

/**
 *@brief    M1����֤��Կ�洢(ͬһ����Կ�洢һ�μ���)
 *@param    ucKeytype:  ��֤��Կ���� A=0x00 ��B=0x01
 *          ucKeynum:   ��Կ���к�(0~15��A/B����16����Կ)
 *          psKeydata   ��Կ,6�ֽ�
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(ucKeytype��ucKeynum�Ƿ���psKeydataΪNULL)
 *@li   NDK_ERR                   ����ʧ��
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1KEYSTORE����ʧ�ܷ���)
*/
NEXPORT int  NDK_M1KeyStore(uchar ucKeytype,  uchar ucKeynum, uchar *psKeydata)
{
    int nRet;


    if ( ((ucKeytype!=0)&&(ucKeytype!=1)&&(ucKeytype!=0x60)&&(ucKeytype!=0x61)) || (ucKeynum>15) || (psKeydata == NULL)) {
        return NDK_ERR_PARA;
    }

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    m_arg.ucAuth_mode = ucKeytype;
    m_arg.ucKeysector = ucKeynum;
    memcpy(m_arg.ucKey, psKeydata, 6);
    m_arg.ucLength_key = 6;
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1KEYSTORE, &m_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (m_arg.nStatus == SUCC) {
            nRet = NDK_OK;
        } else {
            nRet = NDK_ERR;
        }
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    M1��װ���Ѵ洢����Կ(ͬһ����Կ����һ�μ���)
 *@param    ucKeytype:  ��֤��Կ���� A=0x00 ��B=0x01
 *          ucKeynum:   ��Կ���к�(0~15��A/B����16����Կ)
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(ucKeytype��ucKeynum�Ƿ�)
 *@li   NDK_ERR                   ����ʧ��
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1KEYLOAD����ʧ�ܷ���)
*/
NEXPORT int  NDK_M1KeyLoad(uchar ucKeytype,  uchar ucKeynum)
{
    int nRet;


    if ( ((ucKeytype!=0) && (ucKeytype!=1)&&(ucKeytype!=0x60) && (ucKeytype!=0x61)) || (ucKeynum>15) ) {
        return NDK_ERR_PARA;
    }

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    m_arg.ucAuth_mode = ucKeytype;
    m_arg.ucKeysector = ucKeynum;
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1KEYLOAD, &m_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (m_arg.nStatus == SUCC) {
            nRet = NDK_OK;
        } else {
            nRet = NDK_ERR;
        }
    }

    IccClose_Rfid();

    return nRet;

}

/**
 *@brief    M1�����Ѽ��ص���Կ��֤
 *@param    nUidlen:    uid����
 *          psUidbuf��  uid����(NDK_M1Anti��ȡ��)
 *          ucKeytype:  ��֤��Կ���� A=0x00 ��B=0x01
 *          ucBlocknum: Ҫ��֤�Ŀ��(ע��:����������!)
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(ucKeytype��nUidlen�Ƿ���psUidbufΪNULL)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1INTERAUTHEN����ʧ�ܷ���)
*/
NEXPORT int  NDK_M1InternalAuthen(int nUidlen, uchar *psUidbuf, uchar ucKeytype, uchar ucBlocknum)
{
    int nRet;

    if ( (nUidlen!=4) || (psUidbuf == NULL) || ((ucKeytype!=0)&&(ucKeytype!=1)&&(ucKeytype!=0x60)&&(ucKeytype!=0x61)) ) {
        return NDK_ERR_PARA;
    }

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    if(nUidlen>argsize)nUidlen=argsize;
    m_arg.ucAuth_mode = ucKeytype;
    m_arg.ucBlock = ucBlocknum;
    m_arg.ucLength_snr = nUidlen;
    memcpy(m_arg.ucSnr, psUidbuf, nUidlen);
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1INTERAUTHEN, &m_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (m_arg.nStatus == SUCC) {
            nRet = NDK_OK;
        } else {
            //nRet = FAIL;
            nRet = ICCM1ErrPRO(m_arg.nStatus);
        }

    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    M1��ֱ�����KEY��֤
 *@param    nUidlen:    uid����
 *          psUidbuf��  uid����(NDK_M1Anti��ȡ��)
 *          ucKeytype:  ��֤��Կ���� A=0x00 ��B=0x01
 *          psKeydata:  key(6�ֽ�)
 *          ucBlocknum: Ҫ��֤�Ŀ��(ע��:����������!)
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR                   ����ʧ��
 *@li   NDK_ERR_PARA        �����Ƿ�(ucKeytype��nUidlen�Ƿ���psKeydataΪNULL)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1INTERAUTHEN����ʧ�ܷ���)
*/
NEXPORT int  NDK_M1ExternalAuthen(int nUidlen, uchar *psUidbuf, uchar ucKeytype, uchar *psKeydata, uchar ucBlocknum)
{
    int nRet;


    if ( (nUidlen!=4) || (psUidbuf == NULL) || ((ucKeytype!=0)&&(ucKeytype!=1)&&(ucKeytype!=0x60)&&(ucKeytype!=0x61)) || (psKeydata == NULL) ) {
        return NDK_ERR_PARA;
    }

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    m_arg.ucAuth_mode = ucKeytype;
    m_arg.ucBlock = ucBlocknum;
    m_arg.ucLength_snr = nUidlen;
    memcpy(m_arg.ucSnr, psUidbuf, nUidlen);
    m_arg.ucLength_key = 6;
    memcpy(m_arg.ucKey, psKeydata, 6);
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1EXTERAUTHEN, &m_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (m_arg.nStatus == SUCC) {
            nRet = NDK_OK;
        } else {
            nRet = NDK_ERR;
        }
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    M1�����ȡ����
 *@param    ucBlocknum: Ҫ���Ŀ��
 *@retval   nDatalen:   ��ȡ�Ŀ����ݳ���
 *          psBlockdata:������
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pnDatalen��psBlockdataΪNULL)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1READ����ʧ�ܷ���)
*/
NEXPORT int  NDK_M1Read(uchar ucBlocknum, int *pnDatalen, uchar *psBlockdata)
{
    int nRet;


    if ( (pnDatalen == NULL) ||  (psBlockdata == NULL) ) {
        return NDK_ERR_PARA;
    }

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len = 1;
    s_arg.ucCmdarg[0] = ucBlocknum;
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1READ, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (s_arg.nStatus == SUCC) {
            *pnDatalen = 16;
            memcpy(psBlockdata, s_arg.ucCmdarg, 16);
            nRet = SUCC;
        } else {
            *pnDatalen = 0;
            // nRet = FAIL;
            nRet = ICCM1ErrPRO(s_arg.nStatus);
        }
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    M1����д����
 *@param    ucBlocknum: Ҫд�Ŀ��
 *          nDatalen:   ��ȡ�Ŀ����ݳ���
 *          psBlockdata:������
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pnDatalen��psBlockdataΪNULL��pnDatalen�Ƿ�)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1WRITE����ʧ�ܷ���)
*/
NEXPORT int  NDK_M1Write(uchar ucBlocknum, int *pnDatalen, uchar *psBlockdata)
{
    int nRet;


    if ( (pnDatalen == NULL) || (*pnDatalen!=16) || (psBlockdata == NULL) ) {
        return NDK_ERR_PARA;
    }

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len = 1+16;
    s_arg.ucCmdarg[0] = ucBlocknum;
    memcpy(&s_arg.ucCmdarg[1], psBlockdata, 16);
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1WRITE, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (s_arg.nStatus == SUCC) {
            nRet = NDK_OK;
        } else {
            //nRet = FAIL;
            nRet = ICCM1ErrPRO(s_arg.nStatus);
        }
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    M1������������
 *@param    ucBlocknum: ִ�����������Ŀ�š�ע�⣺��������ֻ��ԼĴ�����δ����д��������������⣬���Ŀ����ݱ���������/������ʽ��
 *          nDatalen:   �������ݳ���(4�ֽ�)
 *          psDatabuf:  ��������
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psDatabufΪNULL��nDatalen������4)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1INCREMENT����ʧ�ܷ���)
*/
NEXPORT int  NDK_M1Increment(uchar ucBlocknum, int nDatalen, uchar *psDatabuf)
{
    int nRet;


    if ( (nDatalen!=4) || (psDatabuf == NULL) ) {
        return NDK_ERR_PARA;
    }

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len = 1+nDatalen;
    s_arg.ucCmdarg[0] = ucBlocknum;
    memcpy(&s_arg.ucCmdarg[1], psDatabuf, nDatalen);
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1INCREMENT, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (s_arg.nStatus == SUCC) {
            nRet = NDK_OK;
        } else {
            //nRet = FAIL;
            nRet = ICCM1ErrPRO(s_arg.nStatus);
        }
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    M1�����������
 *@param    ucBlocknum: ִ�м��������Ŀ�š�ע�⣺��������ֻ��ԼĴ�����δ����д��������������⣬���Ŀ����ݱ���������/������ʽ��
 *          nDatalen:   �������ݳ���(4�ֽ�)
 *          psDatabuf:  ��������
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psDatabufΪNULL��nDatalen������4)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1DECREMENT����ʧ�ܷ���)
 *@li   NDK_ERR_MI_NOTAGERR �ǽӴ���-�޿�,              0xff
 *@li   NDK_ERR_MI_CRCERR   �ǽӴ���-CRC��,             0xfe
 *@li   NDK_ERR_MI_EMPTY    �ǽӴ���-�ǿ�,              0xfd
 *@li   NDK_ERR_MI_AUTHERR  �ǽӴ���-��֤��,            0xfc
 *@li   NDK_ERR_MI_PARITYERR    �ǽӴ���-��ż��,            0xfb
 *@li   NDK_ERR_MI_CODEERR  �ǽӴ���-���մ����         0xfa
 *@li   NDK_ERR_MI_SERNRERR �ǽӴ���-����ͻ����У���   0xf8
 *@li   NDK_ERR_MI_KEYERR   �ǽӴ���-��֤KEY��          0xf7
 *@li   NDK_ERR_MI_NOTAUTHERR   �ǽӴ���-δ��֤             0xf6
 *@li   NDK_ERR_MI_BITCOUNTERR  �ǽӴ���-����BIT��          0xf5
 *@li   NDK_ERR_MI_BYTECOUNTERR �ǽӴ���-�����ֽڴ�         0xf4
 *@li   NDK_ERR_MI_WriteFifo    �ǽӴ���-FIFOд����         0xf3
 *@li   NDK_ERR_MI_TRANSERR �ǽӴ���-���Ͳ�������       0xf2
 *@li   NDK_ERR_MI_WRITEERR �ǽӴ���-д��������         0xf1
 *@li   NDK_ERR_MI_INCRERR  �ǽӴ���-������������       0xf0
 *@li   NDK_ERR_MI_DECRERR  �ǽӴ���-������������       0xef
 *@li   NDK_ERR_MI_OVFLERR  �ǽӴ���-�������           0xed
 *@li   NDK_ERR_MI_FRAMINGERR   �ǽӴ���-֡��               0xeb
 *@li   NDK_ERR_MI_COLLERR  �ǽӴ���-��ͻ               0xe8
 *@li   NDK_ERR_MI_INTERFACEERR �ǽӴ���-��λ�ӿڶ�д��     0xe6
 *@li   NDK_ERR_MI_ACCESSTIMEOUT    �ǽӴ���-���ճ�ʱ           0xe5
 *@li   NDK_ERR_MI_PROTOCOLERR  �ǽӴ���-Э���             0xe4
 *@li   NDK_ERR_MI_QUIT �ǽӴ���-�쳣��ֹ           0xe2
 *@li   NDK_ERR_MI_PPSErr   �ǽӴ���-PPS������          0xe1
 *@li   NDK_ERR_MI_SpiRequest   �ǽӴ���-����SPIʧ��        0xa0
 *@li   NDK_ERR_MI_NY_IMPLEMENTED   �ǽӴ���-�޷�ȷ�ϵĴ���״̬ 0x9c
 *@li   NDK_ERR_MI_CardTypeErr  �ǽӴ���-�����ʹ�           0x83
 *@li   NDK_ERR_MI_ParaErrInIoctl   �ǽӴ���-IOCTL������        0x82
 *@li   NDK_ERR_MI_Para �ǽӴ���-�ڲ�������         0xa9
*/
NEXPORT int  NDK_M1Decrement(uchar ucBlocknum, int nDatalen, uchar *psDatabuf)
{
    int nRet;


    if ( (nDatalen!=4) || (psDatabuf == NULL) ) {
        return NDK_ERR_PARA;
    }

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len = 1+nDatalen;
    s_arg.ucCmdarg[0] = ucBlocknum;
    memcpy(&s_arg.ucCmdarg[1], psDatabuf, nDatalen);
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1DECREMENT, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (s_arg.nStatus == SUCC) {
            nRet = NDK_OK;
        } else {
            //nRet = FAIL;
            nRet = ICCM1ErrPRO(s_arg.nStatus);
        }
    }

    IccClose_Rfid();

    return nRet;

}

/**
 *@brief    M1����/����������Ĵ��Ͳ���(�Ĵ���ֵ����д�뿨�Ŀ�������)
 *@param    ucBlocknum: ִ�м��������Ŀ��
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1TRANSFER����ʧ�ܷ���)
 *@li   NDK_ERR_MI_NOTAGERR �ǽӴ���-�޿�,              0xff
 *@li   NDK_ERR_MI_CRCERR   �ǽӴ���-CRC��,             0xfe
 *@li   NDK_ERR_MI_EMPTY    �ǽӴ���-�ǿ�,              0xfd
 *@li   NDK_ERR_MI_AUTHERR  �ǽӴ���-��֤��,            0xfc
 *@li   NDK_ERR_MI_PARITYERR    �ǽӴ���-��ż��,            0xfb
 *@li   NDK_ERR_MI_CODEERR  �ǽӴ���-���մ����         0xfa
 *@li   NDK_ERR_MI_SERNRERR �ǽӴ���-����ͻ����У���   0xf8
 *@li   NDK_ERR_MI_KEYERR   �ǽӴ���-��֤KEY��          0xf7
 *@li   NDK_ERR_MI_NOTAUTHERR   �ǽӴ���-δ��֤             0xf6
 *@li   NDK_ERR_MI_BITCOUNTERR  �ǽӴ���-����BIT��          0xf5
 *@li   NDK_ERR_MI_BYTECOUNTERR �ǽӴ���-�����ֽڴ�         0xf4
 *@li   NDK_ERR_MI_WriteFifo    �ǽӴ���-FIFOд����         0xf3
 *@li   NDK_ERR_MI_TRANSERR �ǽӴ���-���Ͳ�������       0xf2
 *@li   NDK_ERR_MI_WRITEERR �ǽӴ���-д��������         0xf1
 *@li   NDK_ERR_MI_INCRERR  �ǽӴ���-������������       0xf0
 *@li   NDK_ERR_MI_DECRERR  �ǽӴ���-������������       0xef
 *@li   NDK_ERR_MI_OVFLERR  �ǽӴ���-�������           0xed
 *@li   NDK_ERR_MI_FRAMINGERR   �ǽӴ���-֡��               0xeb
 *@li   NDK_ERR_MI_COLLERR  �ǽӴ���-��ͻ               0xe8
 *@li   NDK_ERR_MI_INTERFACEERR �ǽӴ���-��λ�ӿڶ�д��     0xe6
 *@li   NDK_ERR_MI_ACCESSTIMEOUT    �ǽӴ���-���ճ�ʱ           0xe5
 *@li   NDK_ERR_MI_PROTOCOLERR  �ǽӴ���-Э���             0xe4
 *@li   NDK_ERR_MI_QUIT �ǽӴ���-�쳣��ֹ           0xe2
 *@li   NDK_ERR_MI_PPSErr   �ǽӴ���-PPS������          0xe1
 *@li   NDK_ERR_MI_SpiRequest   �ǽӴ���-����SPIʧ��        0xa0
 *@li   NDK_ERR_MI_NY_IMPLEMENTED   �ǽӴ���-�޷�ȷ�ϵĴ���״̬ 0x9c
 *@li   NDK_ERR_MI_CardTypeErr  �ǽӴ���-�����ʹ�           0x83
 *@li   NDK_ERR_MI_ParaErrInIoctl   �ǽӴ���-IOCTL������        0x82
 *@li   NDK_ERR_MI_Para �ǽӴ���-�ڲ�������         0xa9
*/
NEXPORT int   NDK_M1Transfer(uchar ucBlocknum)
{
    int nRet;


    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len = 1;
    s_arg.ucCmdarg[0] = ucBlocknum;
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1TRANSFER, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (s_arg.nStatus == SUCC) {
            nRet = NDK_OK;
        } else {
            //nRet = FAIL;
            nRet = ICCM1ErrPRO(s_arg.nStatus);
        }
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    M1���Ĵ���ֵ�ָ�����(�ָ��Ĵ�����ʼֵ��ʹ֮ǰ����/����������Ч)
 *@param    ucBlocknum: ִ�м��������Ŀ��
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�RFID_IOCG_M1RESTORE����ʧ�ܷ���)
 *@li   NDK_ERR_MI_NOTAGERR �ǽӴ���-�޿�,              0xff
 *@li   NDK_ERR_MI_CRCERR   �ǽӴ���-CRC��,             0xfe
 *@li   NDK_ERR_MI_EMPTY    �ǽӴ���-�ǿ�,              0xfd
 *@li   NDK_ERR_MI_AUTHERR  �ǽӴ���-��֤��,            0xfc
 *@li   NDK_ERR_MI_PARITYERR    �ǽӴ���-��ż��,            0xfb
 *@li   NDK_ERR_MI_CODEERR  �ǽӴ���-���մ����         0xfa
 *@li   NDK_ERR_MI_SERNRERR �ǽӴ���-����ͻ����У���   0xf8
 *@li   NDK_ERR_MI_KEYERR   �ǽӴ���-��֤KEY��          0xf7
 *@li   NDK_ERR_MI_NOTAUTHERR   �ǽӴ���-δ��֤             0xf6
 *@li   NDK_ERR_MI_BITCOUNTERR  �ǽӴ���-����BIT��          0xf5
 *@li   NDK_ERR_MI_BYTECOUNTERR �ǽӴ���-�����ֽڴ�         0xf4
 *@li   NDK_ERR_MI_WriteFifo    �ǽӴ���-FIFOд����         0xf3
 *@li   NDK_ERR_MI_TRANSERR �ǽӴ���-���Ͳ�������       0xf2
 *@li   NDK_ERR_MI_WRITEERR �ǽӴ���-д��������         0xf1
 *@li   NDK_ERR_MI_INCRERR  �ǽӴ���-������������       0xf0
 *@li   NDK_ERR_MI_DECRERR  �ǽӴ���-������������       0xef
 *@li   NDK_ERR_MI_OVFLERR  �ǽӴ���-�������           0xed
 *@li   NDK_ERR_MI_FRAMINGERR   �ǽӴ���-֡��               0xeb
 *@li   NDK_ERR_MI_COLLERR  �ǽӴ���-��ͻ               0xe8
 *@li   NDK_ERR_MI_INTERFACEERR �ǽӴ���-��λ�ӿڶ�д��     0xe6
 *@li   NDK_ERR_MI_ACCESSTIMEOUT    �ǽӴ���-���ճ�ʱ           0xe5
 *@li   NDK_ERR_MI_PROTOCOLERR  �ǽӴ���-Э���             0xe4
 *@li   NDK_ERR_MI_QUIT �ǽӴ���-�쳣��ֹ           0xe2
 *@li   NDK_ERR_MI_PPSErr   �ǽӴ���-PPS������          0xe1
 *@li   NDK_ERR_MI_SpiRequest   �ǽӴ���-����SPIʧ��        0xa0
 *@li   NDK_ERR_MI_NY_IMPLEMENTED   �ǽӴ���-�޷�ȷ�ϵĴ���״̬ 0x9c
 *@li   NDK_ERR_MI_CardTypeErr  �ǽӴ���-�����ʹ�           0x83
 *@li   NDK_ERR_MI_ParaErrInIoctl   �ǽӴ���-IOCTL������        0x82
 *@li   NDK_ERR_MI_Para �ǽӴ���-�ڲ�������         0xa9
*/
NEXPORT int   NDK_M1Restore(uchar ucBlocknum)
{
    int nRet;


    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len = 1;
    s_arg.ucCmdarg[0] = ucBlocknum;
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_M1RESTORE, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (s_arg.nStatus == SUCC) {
            nRet = NDK_OK;
        } else {
            //nRet = FAIL;
            nRet = ICCM1ErrPRO(s_arg.nStatus);
        }
    }

    IccClose_Rfid();

    return nRet;

}

/**
 *@brief    ���׿���Ѱ��(���ڲ��ԵȲ����мӿ�ִ���ٶ�)
 *@param    nModecode:   =0����Ѱ������0����Ѱ��
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR     ����ʧ��
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�ioctl_PiccQuickRequest����ʧ�ܷ���)
*/
NEXPORT int   NDK_PiccQuickRequest(int nModecode)
{
    int nRet;


    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    s_arg.unCmdarg_len = 1;
    s_arg.ucCmdarg[0] = nModecode;
    nRet = ioctl(nDevnum_Rfid, ioctl_PiccQuickRequest, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        if (s_arg.nStatus == SUCC) {
            nRet = NDK_OK;
        } else {
            nRet = NDK_ERR;
        }
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    ���ζ�ISO1443-4Э��֧�ֵ��ж�(������֧��ĳЩ�Ǳ꿨)
 *@param    nModecode:  ��0��ִ������; =0�򰴱�׼ִ��
 *@retval
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�ioctl_PiccQuickRequest����ʧ�ܷ���)
*/
NEXPORT int   NDK_SetIgnoreProtocol(int nModecode)
{
    int nRet;


    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, ioctl_SetIgnoreProtocol, &nModecode);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = NDK_OK;
    }
    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    ��ȡ����ISO1443-4Э��֧�ֵ�����
 *@param
 *@retval   nModecode: ��0��ִ������
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        ��������(pnModecodeΪNULL)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�ioctl_GetIgnoreProtocol����ʧ�ܷ���)
*/
NEXPORT int   NDK_GetIgnoreProtocol(int *pnModecode)
{
    int nRet;


    if(pnModecode==NULL)
        return NDK_ERR_PARA;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, ioctl_GetIgnoreProtocol, pnModecode);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = NDK_OK;
    }

    IccClose_Rfid();

    return nRet;

}


/**
 *@brief    ��ȡ��Ƶ�ӿ�оƬ����(�������ò���ʱ�����)
 *@param
 *@retval   rfidtype: ��EM_NDK_RFIDTYPE
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        ��������(pnRfidtypeΪNULL)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�ioctl_Get_rfid_ic_type����ʧ�ܷ���)
*/
NEXPORT int   NDK_GetRfidType(int *pnRfidtype)
{
    int nRet;

    if(pnRfidtype==NULL)
        return NDK_ERR_PARA;
    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, ioctl_Get_rfid_ic_type, pnRfidtype);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    } else {
        nRet = NDK_OK;
    }
    return nRet;

}
/**
 *@brief    felica��Ѱ��
 *@param 	
 *@return   psRecebuf��������buf��pnRecvlen�������ݳ���
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        ��������(pnRfidtypeΪNULL)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�ioctl_Get_rfid_ic_type����ʧ�ܷ���)
 *@li   NDK_ERR_RFID_NOCARD  ��ǿ��Χ���޿�Ƭ
*/
NEXPORT int NDK_RfidFelicaPoll(uchar *psRecebuf,int *pnRecvlen)
{
	int nRet = 0;
	if((psRecebuf == NULL) || (pnRecvlen == NULL)) return NDK_ERR_PARA;
	*pnRecvlen = 0;
    nRet = IccOpen_Rfid();
    if (nRet < 0) return NDK_ERR_OPEN_DEV;
	nRet = ioctl(nDevnum_Rfid, RFID_IOCG_FELICA_POLL, &s_arg);
	if(nRet != SUCC) nRet = NDK_ERR_IOCTL;
	else {
		if(s_arg.nStatus != 0) nRet = NDK_ERR_RFID_NOCARD;
		else {
			*pnRecvlen = s_arg.unCmdarg_len; 
			memcpy(psRecebuf,s_arg.ucCmdarg, *pnRecvlen);
			nRet = NDK_OK;		
		}
	}
    IccClose_Rfid();
    return nRet;		
}
/**
 *@brief    felica�����ݶ�д������ͷ������ݸ�ʽΪLen+cmd+data
 *@param 	nSendlen �������ݳ��ȣ�psSendbuf ��������buf
 *@return   psRecebuf��������buf��pnRecvlen�������ݳ���
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        ��������(pnRfidtypeΪNULL)
 *@li   NDK_ERR_OPEN_DEV    �豸�ļ���ʧ��(��Ƶ�豸�ļ���ʧ��)
 *@li   NDK_ERR_IOCTL       �������ô���(��Ƶ�����ӿ�ioctl_Get_rfid_ic_type����ʧ�ܷ���)
 *@li   ����  ����Mifara���Ĵ�����
*/
NEXPORT int  NDK_RfidFelicaApdu(int nSendlen, uchar *psSendbuf, int *pnRecvlen,  uchar *psRecebuf)
{
    int nRet = 0;

    if ( (nSendlen < 2) || (psSendbuf == NULL) || (pnRecvlen == NULL) || (psRecebuf == NULL)) {
        return NDK_ERR_PARA;
    }
    *pnRecvlen=0;
    nRet = IccOpen_Rfid();
    if (nRet < 0) return NDK_ERR_OPEN_DEV;

    if(nSendlen > argsize) nSendlen = argsize;
    memcpy(s_arg.ucCmdarg, psSendbuf, nSendlen);
    s_arg.unCmdarg_len = nSendlen;
    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_FELICA_TRANSCEIVE, &s_arg);
    if (nRet != SUCC) {
        nRet = NDK_ERR_IOCTL;
    }
	else {
        if (s_arg.nStatus == SUCC) {
            *pnRecvlen = s_arg.unCmdarg_len;
            memcpy(psRecebuf, s_arg.ucCmdarg, s_arg.unCmdarg_len);
        }
		else nRet = ICCM1ErrPRO(s_arg.nStatus);

    }
    IccClose_Rfid();
    return nRet;
}
/*--------------------------------------------------------
--------------------------------------------------------*/
static int IccOpen_Rfid(void)
{
    nDevnum_Rfid = open("/dev/rfid",O_RDWR);
    if (nDevnum_Rfid<0) {
        return NDK_ERR_OPEN_DEV;
    }

    return SUCC;
}

/*--------------------------------------------------------
--------------------------------------------------------*/
static int IccClose_Rfid(void)
{
    //int nRet;

    close(nDevnum_Rfid);
    nDevnum_Rfid = FAIL;
    return SUCC;
}


/*--------------------------------------------------------
--------------------------------------------------------*/
static int IccErrPro(int nErrret)
{

    int nRet=FAIL;

    switch(nErrret) {
        case 0x01:
            nRet=NDK_ERR_RFID_NOPICCTYPE;
            break;
        case 0x02:
            nRet=NDK_ERR_RFID_NOTDETE;
            break;
        case 0x03:
            nRet=NDK_ERR_RFID_AANTI;
            break;
        case 0x04:
            nRet=NDK_ERR_RFID_RATS;
            break;
        case 0x07:
            nRet=NDK_ERR_RFID_BACTIV;
            break;
        case 0x0A:
            nRet=NDK_ERR_RFID_ASEEK;
            break;
        case 0x0B:
            nRet=NDK_ERR_RFID_BSEEK;
            break;
        case 0x0C:
            nRet=NDK_ERR_RFID_ABON;
            break;
        case 0x0D:
            nRet=NDK_ERR_RFID_NOCARD;
            break;
            /*case 0x0E:
                nRet=NDK_ERR_RFID_COLLISION_A;
                break;
            case 0x0F:
                nRet=NDK_ERR_RFID_COLLISION_B;
                break;*/
        case 0x0E:
            nRet=NDK_ERR_RFID_UPED;
            break;
        case 0x0F:
            nRet=NDK_ERR_RFID_PROTOCOL;
            break;
        default:
            break;
    }

    return nRet;
}


/*--------------------------------------------------------
--------------------------------------------------------*/
static int ICCM1ErrPRO(int nErrcode)
{
    int nRet=FAIL;
    switch(nErrcode) {
        case 0xff:
            nRet=NDK_ERR_MI_NOTAGERR;
            break;
        case 0xfe:
            nRet=NDK_ERR_MI_CRCERR;
            break;
        case 0xfd:
            nRet=NDK_ERR_MI_EMPTY;
            break;
        case 0xfc:
            nRet=NDK_ERR_MI_AUTHERR;
            break;
        case 0xfb:
            nRet=NDK_ERR_MI_PARITYERR;
            break;
        case 0xfa:
            nRet=NDK_ERR_MI_CODEERR;
            break;
        case 0xf8:
            nRet=NDK_ERR_MI_SERNRERR;
            break;
        case 0xf7:
            nRet=NDK_ERR_MI_KEYERR;
            break;
        case 0xf6:
            nRet=NDK_ERR_MI_NOTAUTHERR;
            break;
        case 0xf5:
            nRet=NDK_ERR_MI_BITCOUNTERR;
            break;
        case 0xf4:
            nRet=NDK_ERR_MI_BYTECOUNTERR;
            break;
        case 0xf3:
            nRet=NDK_ERR_MI_WriteFifo;
            break;
        case 0xf2:
            nRet=NDK_ERR_MI_TRANSERR;
            break;
        case 0xf1:
            nRet=NDK_ERR_MI_WRITEERR;
            break;
        case 0xf0:
            nRet=NDK_ERR_MI_INCRERR;
            break;
        case 0xef:
            nRet=NDK_ERR_MI_DECRERR;
            break;
        case 0xed:
            nRet=NDK_ERR_MI_OVFLERR;
            break;
        case 0xeb:
            nRet=NDK_ERR_MI_FRAMINGERR;
            break;
        case 0xe8:
            nRet=NDK_ERR_MI_COLLERR;
            break;
        case 0xe6:
            nRet=NDK_ERR_MI_INTERFACEERR;
            break;
        case 0xe5:
            nRet=NDK_ERR_MI_ACCESSTIMEOUT;
            break;
        case 0xe4:
            nRet=NDK_ERR_MI_PROTOCOLERR;
            break;
        case 0xe2:
            nRet=NDK_ERR_MI_QUIT;
            break;
        case 0xe1:
            nRet=NDK_ERR_MI_PPSErr;
            break;
        case 0xe0:
            nRet=NDK_ERR_MI_SpiRequest;
            break;
        case 0x9c:
            nRet=NDK_ERR_MI_NY_IMPLEMENTED;
            break;
        case 0x83:
            nRet=NDK_ERR_MI_CardTypeErr;
            break;
        case 0x82:
            nRet=NDK_ERR_MI_ParaErrInIoctl;
            break;
        case 0xa9:
            nRet=NDK_ERR_MI_Para;
            break;
        default:
            break;
    }

    return nRet;
}
/*--------------------------------------------------------
    END OF FILR
--------------------------------------------------------*/



