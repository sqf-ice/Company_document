/*************************************************************************************************
 * 新大陆支付技术公司 版权所有(c) 2012-20xx
 *
 * PHENIX 平台非接触卡接口
 * 结构定义：       NDK_RFID.c
 * 作    者：       Steven Lin
 * 日    期：       2012-9-25
 * 最后修改人：
 * 最后修改日期：   2012-9-25
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
 *                                       宏定义
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
*                                       类型及变量
*********************************************************************************************************/
static struct arg_t {
    int     nStatus;                        /**< 返回的状态值*/
    uint    unCmdarg_len;                   /**< 参数的字节数*/
    uchar   ucCmdarg[argsize];              /**< ioctl传入或者回送的参数*/
} s_arg;

struct arg_tm {
    int    nStatus;
    uchar  ucAuth_mode;                     /**< KEYA,B */
    ushort   unEestartadr;                      /**< EEPROM中起始地址 */
    uchar  ucKeysector;                     /**< EEPROM中key所在扇区号 */
    uchar  ucLength_key;                        /**< key[argsize]中key长度，也可以是数据 */
    uchar  ucKey[argsize];                  /**< key 或者 数据 */
    uchar  ucLength_snr;                        /**< snr[argsize]中序列号长度 */
    uchar  ucSnr[argsize];                  /**< 序列号 */
    uchar  ucBlock;                         /**< 要验证的块号 */
} m_arg;

int     nErrorCode=FAIL;                    /**< 错误码 */
int     nDevnum_Rfid=FAIL;                  /**< 文件句柄 */
uchar   ucA_snr[16];                            /**< UID数据 */
uchar   ucSnrlen=0;                         /**< UID数据长度 */

/*********************************************************************************************************
*                                       内部函数说明
*********************************************************************************************************/
static int IccOpen_Rfid(void);
static int IccClose_Rfid(void);
static int IccErrPro(int nErrret);
static int ICCM1ErrPRO(int nErrcode);

/*********************************************************************************************************
*                                       函数
*********************************************************************************************************/

/**
 *@brief    获取射频驱动版本号
 *@param    pszVersion: 返回的驱动版本号字符串
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(pszVersion为NULL)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_GETVISION调用失败返回)
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
 *@brief    射频接口器件初始化，也可用于判断器件是否装配或可工作
 *@param
 *@retval   psStatus:   具体定义留待扩充
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_INIT调用失败返回)
 *@li   NDK_ERR_RFID_INITSTA    非接触卡-射频接口器件故障或者未配置
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
 *@brief    开射频输出
 *@param
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR         操作失败
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_RFOPEN调用失败返回)
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
 *@brief    关闭射频输出，可降低功耗并保护射频器件。
 *@param
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR         操作失败
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_RFCLOSE调用失败返回)
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
 *@brief    获取卡片状态(是否已寻卡，用于判断是否可休眠)
 *@param
 *@retval
 *@return
 *@li   NDK_OK          操作成功(上电成功)
 *@li   NDK_ERR_RFID_NOTACTIV   非接触卡-未激活(未上电成功)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_WORKSTATUS调用失败返回)
*/
NEXPORT int  NDK_RfidPiccState(void)
{
    int nRet;

    nRet = IccOpen_Rfid();
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_Rfid, RFID_IOCG_WORKSTATUS, &s_arg);
    if (nRet == SUCC) { //闲
        nRet = NDK_OK;
    } else {            //忙
        nRet =  NDK_ERR_RFID_NOTACTIV;
    }

    IccClose_Rfid();

    return nRet;
}

/**
 *@brief    设备强制休眠
 *@param
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_SUSPEND调用失败返回)
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
 *@brief    设备唤醒
 *@param
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_RESUME调用失败返回)
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
 *@brief    设置超时控制
 *@param　　ucTimeOutCtl 值０是不执行，值不为０是执行
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_TIMEOUTCTL调用失败返回)
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
 *@brief    设置寻卡策略(寻卡操作前设置一次即可，M1卡操作时需要设置成TYPE-A卡模式)
 *@param    ucPicctype =0xcc，表示寻卡时只针对TYPE-A的卡
 *                      0xcb，表示寻卡时只针对TYPE-B的卡
 *                      0xcd，表示寻卡时针对TYPE-A及TYPE-B的卡
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(ucPicctype非法)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_SETPICCTYPE调用失败返回)
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
 *@brief    射频卡寻卡(探测刷卡区域是否有卡,寻卡策略之前需要已设置过一次)
 *@param
 *@retval   psPicctype: 激活的卡类型 0xcc=TYPE-A卡，0xcb=TYPE-B卡
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_PICCDEDECT调用失败返回)
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
        nRet = s_arg.nStatus;           /**< 含有可能的失败代码，如无卡、多卡等*/
        if (s_arg.nStatus==SUCC) {
        } else {
            nRet = IccErrPro(s_arg.nStatus);

        }
    }

    IccClose_Rfid();

    return nRet;
}


/**
 *@brief    射频卡激活(已探测有卡的情况下),相当于powerup , 按改进的流程（原先生产版本）。
 *@param
 *@retval   psPicctype: 激活的卡类型 0xcc=TYPE-A卡，0xcb=TYPE-B卡
 *          pnDatalen:  数据长度
 *          psDatabuf:  数据缓冲区(A卡为UID；B卡databuf[1]~[4]为UID，其它是应用及协议信息)
 *@return
 *@li   NDK_OK          激活成功
 *@li   NDK_ERR_PARA        参数非法(psPicctype/pnDatalen/psDatabuf为NULL)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_PICCACTIVATE调用失败返回)
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
        nRet = s_arg.nStatus;       /**< 含有可能的失败代码，如无卡、多卡等*/
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
 *@brief	射频CPU A卡激活，调用M1卡寻卡、防冲突、选卡接口后调用激活卡片。
 *@param    cid：RATS命令的cid字符，默认卡片传入0，3911平台目前忽略此参数。
 *@retval 	
 *			pnDatalen:	数据长度
 *			psDatabuf:	数据缓冲区(A卡的ats)
 *@return
 *@li	NDK_OK			激活成功
 *@li	NDK_ERR_PARA		参数非法(pnDatalen/psDatabuf为NULL)
 *@li	NDK_ERR_OPEN_DEV	设备文件打开失败(射频设备文件打开失败)
 *@li	NDK_ERR_IOCTL		驱动调用错误(射频驱动接口ioctl_isoRatsA调用失败返回)
 *@li   其他错误码和M1卡接口一致，比如奇偶校验错误、crc错误、超时错误等。	
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
 *@brief    射频卡激活(已探测有卡的情况下),相当于powerup ,按EMV L1 MAINLOOP流程。
 *@param
 *@retval   psPicctype: 激活的卡类型 0xcc=TYPE-A卡，0xcb=TYPE-B卡
 *          pnDatalen:  数据长度
 *          psDatabuf:  数据缓冲区(A卡为UID；B卡databuf[1]~[4]为UID，其它是应用及协议信息)
 *@return
 *@li   NDK_OK          激活成功
 *@li   NDK_ERR_PARA        参数非法(psPicctype/pnDatalen/psDatabuf为NULL)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_PICCACTIVATE_EMV调用失败返回)
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
        nRet = s_arg.nStatus;       /**< 含有可能的失败代码，如无卡、多卡等*/
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
 *@brief    关闭射频使卡失效。读写操作结束后应该执行该操作，相当于powerdown 。
 *@param    ucDelayms:  等0则一直关闭RF;不等0则关闭ucDelayms毫秒后再打开RF。
 *                      关闭6～10ms可使卡失效。如果后续没有连续的读卡操作，应该置0以关闭RF 。
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口ioctl_PiccDeselect调用失败返回)
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
 *@brief    射频卡APDU交互,即读写卡过程(已激活的情况下)
 *@param    nSendlen    发送的命令长度
 *          psSendbuf   发送命令缓冲区
 *@retval   pnRecvlen:  接收数据长度
 *          psRecebuf:  接收数据缓冲区
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(psSendbuf/pnRecvlen/psRecebuf为NULL、nSendlen小于5)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_PICCAPDU调用失败返回)
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
*   M1卡
****************************************************/

/**
 *@brief    M1寻卡(寻卡类型必须已经设置为TYPE-A)
 *@param    ucReqcode:  0=请求REQA, 非0=唤醒WUPA, 一般情况下需要使用WUPA
 *@retval   pnDatalen:  接收数据长度(2字节)
 *          psDatabuf:  接收数据缓冲区
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(pnDatalen/psDatabuf为NULL)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1REQUEST调用失败返回)
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
 *@brief    M1卡防冲突(NDK_M1Request有卡的情况下)
 *@param
 *@retval   pnDatalen:  接收数据长度(UID长度)
 *          psDatabuf:  接收数据缓冲区(UID)
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(pnDatalen/psDatabuf为NULL)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1ANTI调用失败返回)
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
 *@brief    M1卡防冲突(NDK_M1Request有卡的情况下),针对多级级联的UID .
 *@param    ucSelcode:  PICC_ANTICOLL1/PICC_ANTICOLL2/PICC_ANTICOLL3
 *@retval   pnDatalen:  接收数据长度(UID长度)
 *          psDatabuf:  接收数据缓冲区(UID)
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(pnDatalen/psDatabuf为NULL、ucSelcode非法)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1ANTI调用失败返回)
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
 *@brief    M1卡选卡(NDK_M1Anti成功的情况下)
 *@param    nUidlen:    uid长度
 *          psUidbuf:   uid数据缓冲区
 *@retval   psSakbuf:   选卡返回数据(1字节SAK)
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(psUidbuf/psSakbuf为NULL、nUidlen不等于4)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1SELECT调用失败返回)
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
 *@brief    M1卡选卡(NDK_M1Anti成功的情况下)，针对多级级联的UID .
 *@param    ucSelcode   PICC_ANTICOLL1/PICC_ANTICOLL2/PICC_ANTICOLL3
 *          nUidlen:    uid长度
 *          psUidbuf:   uid数据缓冲区
 *@retval   psSakbuf:   选卡返回数据(1字节SAK)
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(psUidbuf/psSakbuf为NULL、nUidlen不等于4、ucSelcode非法)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1SELECT调用失败返回)
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
 *@brief    M1卡认证密钥存储(同一个密钥存储一次即可)
 *@param    ucKeytype:  认证密钥类型 A=0x00 ；B=0x01
 *          ucKeynum:   密钥序列号(0~15，A/B各有16个密钥)
 *          psKeydata   密钥,6字节
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(ucKeytype、ucKeynum非法、psKeydata为NULL)
 *@li   NDK_ERR                   操作失败
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1KEYSTORE调用失败返回)
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
 *@brief    M1卡装载已存储的密钥(同一个密钥加载一次即可)
 *@param    ucKeytype:  认证密钥类型 A=0x00 ；B=0x01
 *          ucKeynum:   密钥序列号(0~15，A/B各有16个密钥)
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(ucKeytype、ucKeynum非法)
 *@li   NDK_ERR                   操作失败
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1KEYLOAD调用失败返回)
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
 *@brief    M1卡用已加载的密钥认证
 *@param    nUidlen:    uid长度
 *          psUidbuf：  uid数据(NDK_M1Anti获取的)
 *          ucKeytype:  认证密钥类型 A=0x00 ；B=0x01
 *          ucBlocknum: 要认证的块号(注意:不是扇区号!)
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(ucKeytype、nUidlen非法、psUidbuf为NULL)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1INTERAUTHEN调用失败返回)
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
 *@brief    M1卡直接外带KEY认证
 *@param    nUidlen:    uid长度
 *          psUidbuf：  uid数据(NDK_M1Anti获取的)
 *          ucKeytype:  认证密钥类型 A=0x00 ；B=0x01
 *          psKeydata:  key(6字节)
 *          ucBlocknum: 要认证的块号(注意:不是扇区号!)
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR                   操作失败
 *@li   NDK_ERR_PARA        参数非法(ucKeytype、nUidlen非法、psKeydata为NULL)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1INTERAUTHEN调用失败返回)
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
 *@brief    M1卡块读取操作
 *@param    ucBlocknum: 要读的块号
 *@retval   nDatalen:   读取的块数据长度
 *          psBlockdata:块数据
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(pnDatalen、psBlockdata为NULL)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1READ调用失败返回)
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
 *@brief    M1卡块写操作
 *@param    ucBlocknum: 要写的块号
 *          nDatalen:   读取的块数据长度
 *          psBlockdata:块数据
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(pnDatalen、psBlockdata为NULL、pnDatalen非法)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1WRITE调用失败返回)
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
 *@brief    M1卡块增量操作
 *@param    ucBlocknum: 执行增量操作的块号。注意：增量操作只针对寄存器，未真正写入块数据区。另外，卡的块数据必须满足增/减量格式。
 *          nDatalen:   增量数据长度(4字节)
 *          psDatabuf:  增量数据
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(psDatabuf为NULL、nDatalen不等于4)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1INCREMENT调用失败返回)
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
 *@brief    M1卡块减量操作
 *@param    ucBlocknum: 执行减量操作的块号。注意：减量操作只针对寄存器，未真正写入块数据区。另外，卡的块数据必须满足增/减量格式。
 *          nDatalen:   减量数据长度(4字节)
 *          psDatabuf:  减量数据
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(psDatabuf为NULL、nDatalen不等于4)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1DECREMENT调用失败返回)
 *@li   NDK_ERR_MI_NOTAGERR 非接触卡-无卡,              0xff
 *@li   NDK_ERR_MI_CRCERR   非接触卡-CRC错,             0xfe
 *@li   NDK_ERR_MI_EMPTY    非接触卡-非空,              0xfd
 *@li   NDK_ERR_MI_AUTHERR  非接触卡-认证错,            0xfc
 *@li   NDK_ERR_MI_PARITYERR    非接触卡-奇偶错,            0xfb
 *@li   NDK_ERR_MI_CODEERR  非接触卡-接收代码错         0xfa
 *@li   NDK_ERR_MI_SERNRERR 非接触卡-防冲突数据校验错   0xf8
 *@li   NDK_ERR_MI_KEYERR   非接触卡-认证KEY错          0xf7
 *@li   NDK_ERR_MI_NOTAUTHERR   非接触卡-未认证             0xf6
 *@li   NDK_ERR_MI_BITCOUNTERR  非接触卡-接收BIT错          0xf5
 *@li   NDK_ERR_MI_BYTECOUNTERR 非接触卡-接收字节错         0xf4
 *@li   NDK_ERR_MI_WriteFifo    非接触卡-FIFO写错误         0xf3
 *@li   NDK_ERR_MI_TRANSERR 非接触卡-传送操作错误       0xf2
 *@li   NDK_ERR_MI_WRITEERR 非接触卡-写操作错误         0xf1
 *@li   NDK_ERR_MI_INCRERR  非接触卡-增量操作错误       0xf0
 *@li   NDK_ERR_MI_DECRERR  非接触卡-减量操作错误       0xef
 *@li   NDK_ERR_MI_OVFLERR  非接触卡-溢出错误           0xed
 *@li   NDK_ERR_MI_FRAMINGERR   非接触卡-帧错               0xeb
 *@li   NDK_ERR_MI_COLLERR  非接触卡-冲突               0xe8
 *@li   NDK_ERR_MI_INTERFACEERR 非接触卡-复位接口读写错     0xe6
 *@li   NDK_ERR_MI_ACCESSTIMEOUT    非接触卡-接收超时           0xe5
 *@li   NDK_ERR_MI_PROTOCOLERR  非接触卡-协议错             0xe4
 *@li   NDK_ERR_MI_QUIT 非接触卡-异常终止           0xe2
 *@li   NDK_ERR_MI_PPSErr   非接触卡-PPS操作错          0xe1
 *@li   NDK_ERR_MI_SpiRequest   非接触卡-申请SPI失败        0xa0
 *@li   NDK_ERR_MI_NY_IMPLEMENTED   非接触卡-无法确认的错误状态 0x9c
 *@li   NDK_ERR_MI_CardTypeErr  非接触卡-卡类型错           0x83
 *@li   NDK_ERR_MI_ParaErrInIoctl   非接触卡-IOCTL参数错        0x82
 *@li   NDK_ERR_MI_Para 非接触卡-内部参数错         0xa9
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
 *@brief    M1卡增/减量操作后的传送操作(寄存器值真正写入卡的块数据区)
 *@param    ucBlocknum: 执行减量操作的块号
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1TRANSFER调用失败返回)
 *@li   NDK_ERR_MI_NOTAGERR 非接触卡-无卡,              0xff
 *@li   NDK_ERR_MI_CRCERR   非接触卡-CRC错,             0xfe
 *@li   NDK_ERR_MI_EMPTY    非接触卡-非空,              0xfd
 *@li   NDK_ERR_MI_AUTHERR  非接触卡-认证错,            0xfc
 *@li   NDK_ERR_MI_PARITYERR    非接触卡-奇偶错,            0xfb
 *@li   NDK_ERR_MI_CODEERR  非接触卡-接收代码错         0xfa
 *@li   NDK_ERR_MI_SERNRERR 非接触卡-防冲突数据校验错   0xf8
 *@li   NDK_ERR_MI_KEYERR   非接触卡-认证KEY错          0xf7
 *@li   NDK_ERR_MI_NOTAUTHERR   非接触卡-未认证             0xf6
 *@li   NDK_ERR_MI_BITCOUNTERR  非接触卡-接收BIT错          0xf5
 *@li   NDK_ERR_MI_BYTECOUNTERR 非接触卡-接收字节错         0xf4
 *@li   NDK_ERR_MI_WriteFifo    非接触卡-FIFO写错误         0xf3
 *@li   NDK_ERR_MI_TRANSERR 非接触卡-传送操作错误       0xf2
 *@li   NDK_ERR_MI_WRITEERR 非接触卡-写操作错误         0xf1
 *@li   NDK_ERR_MI_INCRERR  非接触卡-增量操作错误       0xf0
 *@li   NDK_ERR_MI_DECRERR  非接触卡-减量操作错误       0xef
 *@li   NDK_ERR_MI_OVFLERR  非接触卡-溢出错误           0xed
 *@li   NDK_ERR_MI_FRAMINGERR   非接触卡-帧错               0xeb
 *@li   NDK_ERR_MI_COLLERR  非接触卡-冲突               0xe8
 *@li   NDK_ERR_MI_INTERFACEERR 非接触卡-复位接口读写错     0xe6
 *@li   NDK_ERR_MI_ACCESSTIMEOUT    非接触卡-接收超时           0xe5
 *@li   NDK_ERR_MI_PROTOCOLERR  非接触卡-协议错             0xe4
 *@li   NDK_ERR_MI_QUIT 非接触卡-异常终止           0xe2
 *@li   NDK_ERR_MI_PPSErr   非接触卡-PPS操作错          0xe1
 *@li   NDK_ERR_MI_SpiRequest   非接触卡-申请SPI失败        0xa0
 *@li   NDK_ERR_MI_NY_IMPLEMENTED   非接触卡-无法确认的错误状态 0x9c
 *@li   NDK_ERR_MI_CardTypeErr  非接触卡-卡类型错           0x83
 *@li   NDK_ERR_MI_ParaErrInIoctl   非接触卡-IOCTL参数错        0x82
 *@li   NDK_ERR_MI_Para 非接触卡-内部参数错         0xa9
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
 *@brief    M1卡寄存器值恢复操作(恢复寄存器初始值，使之前的增/减量操作无效)
 *@param    ucBlocknum: 执行减量操作的块号
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口RFID_IOCG_M1RESTORE调用失败返回)
 *@li   NDK_ERR_MI_NOTAGERR 非接触卡-无卡,              0xff
 *@li   NDK_ERR_MI_CRCERR   非接触卡-CRC错,             0xfe
 *@li   NDK_ERR_MI_EMPTY    非接触卡-非空,              0xfd
 *@li   NDK_ERR_MI_AUTHERR  非接触卡-认证错,            0xfc
 *@li   NDK_ERR_MI_PARITYERR    非接触卡-奇偶错,            0xfb
 *@li   NDK_ERR_MI_CODEERR  非接触卡-接收代码错         0xfa
 *@li   NDK_ERR_MI_SERNRERR 非接触卡-防冲突数据校验错   0xf8
 *@li   NDK_ERR_MI_KEYERR   非接触卡-认证KEY错          0xf7
 *@li   NDK_ERR_MI_NOTAUTHERR   非接触卡-未认证             0xf6
 *@li   NDK_ERR_MI_BITCOUNTERR  非接触卡-接收BIT错          0xf5
 *@li   NDK_ERR_MI_BYTECOUNTERR 非接触卡-接收字节错         0xf4
 *@li   NDK_ERR_MI_WriteFifo    非接触卡-FIFO写错误         0xf3
 *@li   NDK_ERR_MI_TRANSERR 非接触卡-传送操作错误       0xf2
 *@li   NDK_ERR_MI_WRITEERR 非接触卡-写操作错误         0xf1
 *@li   NDK_ERR_MI_INCRERR  非接触卡-增量操作错误       0xf0
 *@li   NDK_ERR_MI_DECRERR  非接触卡-减量操作错误       0xef
 *@li   NDK_ERR_MI_OVFLERR  非接触卡-溢出错误           0xed
 *@li   NDK_ERR_MI_FRAMINGERR   非接触卡-帧错               0xeb
 *@li   NDK_ERR_MI_COLLERR  非接触卡-冲突               0xe8
 *@li   NDK_ERR_MI_INTERFACEERR 非接触卡-复位接口读写错     0xe6
 *@li   NDK_ERR_MI_ACCESSTIMEOUT    非接触卡-接收超时           0xe5
 *@li   NDK_ERR_MI_PROTOCOLERR  非接触卡-协议错             0xe4
 *@li   NDK_ERR_MI_QUIT 非接触卡-异常终止           0xe2
 *@li   NDK_ERR_MI_PPSErr   非接触卡-PPS操作错          0xe1
 *@li   NDK_ERR_MI_SpiRequest   非接触卡-申请SPI失败        0xa0
 *@li   NDK_ERR_MI_NY_IMPLEMENTED   非接触卡-无法确认的错误状态 0x9c
 *@li   NDK_ERR_MI_CardTypeErr  非接触卡-卡类型错           0x83
 *@li   NDK_ERR_MI_ParaErrInIoctl   非接触卡-IOCTL参数错        0x82
 *@li   NDK_ERR_MI_Para 非接触卡-内部参数错         0xa9
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
 *@brief    简易快速寻卡(用于测试等操作中加快执行速度)
 *@param    nModecode:   =0正常寻卡；非0快速寻卡
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR     操作失败
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口ioctl_PiccQuickRequest调用失败返回)
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
 *@brief    屏蔽对ISO1443-4协议支持的判断(仅用来支持某些非标卡)
 *@param    nModecode:  非0则执行屏蔽; =0则按标准执行
 *@retval
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口ioctl_PiccQuickRequest调用失败返回)
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
 *@brief    读取屏蔽ISO1443-4协议支持的设置
 *@param
 *@retval   nModecode: 非0则执行屏蔽
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数错误(pnModecode为NULL)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口ioctl_GetIgnoreProtocol调用失败返回)
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
 *@brief    读取射频接口芯片类型(加载配置参数时传入的)
 *@param
 *@retval   rfidtype: 见EM_NDK_RFIDTYPE
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数错误(pnRfidtype为NULL)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口ioctl_Get_rfid_ic_type调用失败返回)
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
 *@brief    felica卡寻卡
 *@param 	
 *@return   psRecebuf返回数据buf，pnRecvlen返回数据长度
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数错误(pnRfidtype为NULL)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口ioctl_Get_rfid_ic_type调用失败返回)
 *@li   NDK_ERR_RFID_NOCARD  场强范围内无卡片
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
 *@brief    felica卡数据读写，传入和返回数据格式为Len+cmd+data
 *@param 	nSendlen 发送数据长度，psSendbuf 发送数据buf
 *@return   psRecebuf返回数据buf，pnRecvlen返回数据长度
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数错误(pnRfidtype为NULL)
 *@li   NDK_ERR_OPEN_DEV    设备文件打开失败(射频设备文件打开失败)
 *@li   NDK_ERR_IOCTL       驱动调用错误(射频驱动接口ioctl_Get_rfid_ic_type调用失败返回)
 *@li   其他  参照Mifara卡的错误码
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



