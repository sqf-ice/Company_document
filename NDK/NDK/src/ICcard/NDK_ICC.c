/*************************************************************************************************
 * 新大陆支付技术公司 版权所有(c) 2012-2015
 *
 * PHENIX 平台IC卡接口
 * 结构定义  --- NDK_IC.c
 * 作    者：
 * 日    期：    2012-９-25
 * 最后修改人：  linwx
 * 最后修改日期：2012-９-25
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
#include  <termio.h>
#include <fcntl.h>
#include <linux/input.h>
#include "NDK_ICC.h"
#include "../public/delay.h"
#include "../public/config.h"

//#define IC_DEBUG
#undef IC_DEBUG
#ifdef IC_DEBUG
#define ic_printf(fmt, arg...)  printf(fmt, ##arg)
#else
#define ic_printf(fmt, arg...)
#endif



/*********************************************************************************************************
*                                       宏定义
*********************************************************************************************************/
#define   ic0cpucard            0x00
#define   ic0at24c01            0x01
#define   ic0at24c02            0x02
#define   ic0at24c04            0x03
#define   ic0at24c08            0x04
#define   ic0at24c16            0x05
#define   ic0at24c32            0x06
#define   ic0at24c64            0x07
#define   ic0at45d041_3v        0x08
#define   ic0sle44x2            0x09
#define   ic0at88sc102          0x0a
#define   ic0at88sc1604         0x0b
#define   ic0at88sc1608         0x0c
#define   ic0sle44x8            0x0d
#define   ic0at88sc153          0x0e
#define   ic0at24c128           0x0f
#define   ic0at24c256           0x10

#define   firstcardnum          ICTYPE_M_1
#define   lastcardnum           ICTYPE_M_1_256




#define Magic   'S'
#define N_POWERDOWN         _IOWR(Magic, 1, int)
//#define N_POWERON         _IOWR(Magic, 2, int)
//#define N_GetState        _IOWR(Magic, 3, int)
#define N_GetState_c        _IOWR(Magic, 4, int)
//#define N_SC_start        _IOWR(Magic, 66, int)
//#define N_GetPowerSta     _IOWR(Magic, 67, int)
#define ioctl_SC_APDU       _IOWR(Magic, 68, int)
//#define ioctl_SC_Read     _IOWR(Magic, 69, int)
#define ioctl_SetICType     _IOWR(Magic, 70, int)
#define ioctl_GetICType     _IOWR(Magic, 71, int)
#define ioctl_SC_PowerOn    _IOWR(Magic, 72, int)
#define ioctl_ClearICInit   _IOWR(Magic, 73, int)
#define ioctl_ResetSC       _IOWR(Magic, 74, int)

#define ioctl_mem_init_card         _IOWR(Magic, 80, int)
#define ioctl_mem_card_powerup      _IOWR(Magic, 81, int)
#define ioctl_mem_card_powerdown    _IOWR(Magic, 82, int)
#define ioctl_mem_card_getsta       _IOWR(Magic, 83, int)
#define ioctl_mem_card_rw           _IOWR(Magic, 84, int)

#define ioctl_getvision             _IOWR(Magic, 108, int)


#define  FAIL  (-1)
#define  SUCC  0

#define  _IC1   0               //第一个IC卡
#define  _IC2   1               //第二个IC卡
#define  _SAM1  2               //1SAM
#define  _SAM2  3               //2SAM
#define  _SAM3  4               //3SAM



#define MaxBufLen_CAPDU     261
#define MaxBufLen_RAPDU     512

#define rw_memorycard_maxlength  238+10   // 读写Memory数据的最大长度





/*********************************************************************************************************
*                                       变量
*********************************************************************************************************/

////////////////////////////////////////////////////
/*static*/ struct  ioctl_CMDarg {
    uchar   ucDevnum;
    uchar   ucBuf_capdu[MaxBufLen_CAPDU];
    uint    nCapdu_len;
    uchar   ucBuf_rapdu[MaxBufLen_RAPDU];
    uint    nRapdu_len;
    uchar   ucCmdflag;
    int             nRetflag;
} CMDarg;

int nDevnum_sc0=FAIL;

/*********************************************************************************************************
*                                       函数声明
*********************************************************************************************************/

static int IccOpen_sc(EM_ICTYPE emIctype);
static int IccClose_sc(EM_ICTYPE emIctype);



//memory card
static int IccInit_mem_card(void);
static int IccMem_card_powerup(EM_ICTYPE emIctype, uchar *psAtrbuf,int *pnAtrlen);
static int IccMem_card_powerdown(EM_ICTYPE emIctype);
static int IccMem_card_check(void);
static int IccMem_card_pwr_chk(void);
static int IccRw_mem_card_check(void);
static int IccMem_card_rw(EM_ICTYPE emIctype, int nSendlen, uchar *psSendbuf, int *pnRcvlen, uchar *psRecebuf);

/**
 *@brief    设置卡类型
 *@param    emIctype　卡类型
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA            参数非法(emIctype非法)
 *@li   NDK_ERR_OPEN_DEV        打开设备文件错误(打开IC卡设备文件失败)
 *@li   NDK_ERR_IOCTL   驱动调用错误(IC驱动接口ioctl_SetICType调用失败返回)
*/
NEXPORT int  NDK_IccSetType(EM_ICTYPE emIctype)
{
    int nRet;

    switch (emIctype) {
        case ICTYPE_IC:
            CMDarg.ucBuf_capdu[0] = _IC1;
            CMDarg.ucBuf_capdu[1] = 0xFF;
            break;

        case ICTYPE_SAM1:
            CMDarg.ucBuf_capdu[0] = _SAM1;
            CMDarg.ucBuf_capdu[1] = 0xFF;

            break;
        case ICTYPE_SAM2:
            CMDarg.ucBuf_capdu[0] = _SAM2;
            CMDarg.ucBuf_capdu[1] = 0xFF;

            break;
        case ICTYPE_SAM3:
            CMDarg.ucBuf_capdu[0] = _SAM3;
            CMDarg.ucBuf_capdu[1] = 0xFF;
            break;
        case ICTYPE_M_1:
            CMDarg.ucCmdflag = _IC1;
            CMDarg.ucBuf_capdu[0] = ic0at24c32;
            CMDarg.ucBuf_capdu[1] = 0xFF;
            break;

        case ICTYPE_M_2:
            CMDarg.ucCmdflag = _IC1;
            CMDarg.ucBuf_capdu[0] = ic0sle44x2;
            CMDarg.ucBuf_capdu[1] = 0xFF;

            break;
        case ICTYPE_M_3:
            CMDarg.ucCmdflag = _IC1;
            CMDarg.ucBuf_capdu[0] = ic0sle44x8;
            CMDarg.ucBuf_capdu[1] = 0xFF;

            break;
        case ICTYPE_M_4:
            CMDarg.ucCmdflag = _IC1;
            CMDarg.ucBuf_capdu[0] = ic0at88sc102;
            CMDarg.ucBuf_capdu[1] = 0xFF;
            break;
        case ICTYPE_M_5:
            CMDarg.ucCmdflag = _IC1;
            CMDarg.ucBuf_capdu[0] = ic0at88sc1604;
            CMDarg.ucBuf_capdu[1] = 0xFF;

            break;
        case ICTYPE_M_6:
            CMDarg.ucCmdflag = _IC1;
            CMDarg.ucBuf_capdu[0] = ic0at88sc1608;
            CMDarg.ucBuf_capdu[1] = 0xFF;
            break;
		case ICTYPE_ISO7816:
			CMDarg.ucBuf_capdu[0] = 0x78;
            CMDarg.ucBuf_capdu[1] = 0x16;
			break;
		case ICTYPE_M_7:
            CMDarg.ucCmdflag = _IC1;
            CMDarg.ucBuf_capdu[0] = ic0at88sc153;
            CMDarg.ucBuf_capdu[1] = 0xFF;
            break;
		case ICTYPE_M_1_1:
            CMDarg.ucCmdflag = _IC1;
            CMDarg.ucBuf_capdu[0] = ic0at24c01;
            CMDarg.ucBuf_capdu[1] = 0xFE;
            break;
		case ICTYPE_M_1_2:
			CMDarg.ucCmdflag = _IC1;
            CMDarg.ucBuf_capdu[0] = ic0at24c02;
            CMDarg.ucBuf_capdu[1] = 0xFE;
			break;
		case ICTYPE_M_1_4:
            CMDarg.ucCmdflag = _IC1;
            CMDarg.ucBuf_capdu[0] = ic0at24c04;
            CMDarg.ucBuf_capdu[1] = 0xFE;
            break;
		case ICTYPE_M_1_8:
	        CMDarg.ucCmdflag = _IC1;
	        CMDarg.ucBuf_capdu[0] = ic0at24c08;
	        CMDarg.ucBuf_capdu[1] = 0xFE;
        	break;
		case ICTYPE_M_1_16:
			CMDarg.ucCmdflag = _IC1;
	        CMDarg.ucBuf_capdu[0] = ic0at24c16;
	        CMDarg.ucBuf_capdu[1] = 0xFE;
			break;
		case ICTYPE_M_1_32:
			CMDarg.ucCmdflag = _IC1;
	        CMDarg.ucBuf_capdu[0] = ic0at24c32;
	        CMDarg.ucBuf_capdu[1] = 0xFE;
			break;
		case ICTYPE_M_1_64:
	        CMDarg.ucCmdflag = _IC1;
	        CMDarg.ucBuf_capdu[0] = ic0at24c64;
	        CMDarg.ucBuf_capdu[1] = 0xFE;
	        break;
		case ICTYPE_M_1_128:
			CMDarg.ucCmdflag = _IC1;
	        CMDarg.ucBuf_capdu[0] = ic0at24c128;
	        CMDarg.ucBuf_capdu[1] = 0xFE;
			break;
		case ICTYPE_M_1_256:
	        CMDarg.ucCmdflag = _IC1;
	        CMDarg.ucBuf_capdu[0] = ic0at24c256;
	        CMDarg.ucBuf_capdu[1] = 0xFE;
	        break;
        default:
            nRet = NDK_ERR_PARA;
            return nRet;
    }

    nRet = IccOpen_sc(ICTYPE_IC);
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }


    ioctl(nDevnum_sc0, ioctl_SetICType, &CMDarg);

    IccClose_sc(ICTYPE_IC);

    return NDK_OK;
}

/**
 *@brief    获取卡类型
 *@param    pemIctype　卡类型
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA            参数非法(pemIctype非法)
 *@li   NDK_ERR_OPEN_DEV        打开设备文件错误(打开IC卡设备文件失败)
 *@li   NDK_ERR_IOCTL   驱动调用错误
*/
NEXPORT int  NDK_IccGetType(EM_ICTYPE *pemIctype)
{

    int nRet;

    if(pemIctype==NULL)
        return NDK_ERR_PARA;


    nRet =IccOpen_sc(ICTYPE_IC);
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    ioctl(nDevnum_sc0, ioctl_GetICType, &CMDarg);

    IccClose_sc(ICTYPE_IC);
	if(CMDarg.ucBuf_capdu[1]==0xFF){
	    switch (CMDarg.ucBuf_capdu[0]) {
	        case _IC1:
	            *pemIctype=ICTYPE_IC;
	            break;
	        case _SAM1:
	            *pemIctype=ICTYPE_SAM1;
	            break;
	        case _SAM2:
	            *pemIctype=ICTYPE_SAM2;
	            break;
	        case _SAM3:
	            *pemIctype=ICTYPE_SAM3;
	            break;
	        case ic0at24c32:
	            *pemIctype=ICTYPE_M_1;
	            break;
	        case ic0sle44x2:
	            *pemIctype=ICTYPE_M_2;
	            break;
	        case ic0sle44x8:
	            *pemIctype=ICTYPE_M_3;
	            break;
	        case ic0at88sc102:
	            *pemIctype=ICTYPE_M_4;
	            break;
	        case ic0at88sc1604:
	            *pemIctype=ICTYPE_M_5;
	            break;
	        case ic0at88sc1608:
	            *pemIctype=ICTYPE_M_6;
	            break;
			case ic0at88sc153:
	            *pemIctype=ICTYPE_M_7;
	            break;
	        default:
	            nRet = NDK_ERR_IOCTL;
	    }
	}
	else{
		  switch (CMDarg.ucBuf_capdu[0]) {
	        case ic0at24c01:
	            *pemIctype=ICTYPE_M_1_1;
	            break;
	        case ic0at24c02:
	            *pemIctype=ICTYPE_M_1_2;
	            break;
	        case ic0at24c04:
	            *pemIctype=ICTYPE_M_1_4;
	            break;
	        case ic0at24c08:
	            *pemIctype=ICTYPE_M_1_8;
	            break;
			case ic0at24c16:
	            *pemIctype=ICTYPE_M_1_16;
	            break;
	        case ic0at24c32:
	            *pemIctype=ICTYPE_M_1_32;
	            break;
	        case ic0at24c64:
	            *pemIctype=ICTYPE_M_1_64;
	            break;
	        case ic0at24c128:
	            *pemIctype=ICTYPE_M_1_128;
	            break;
	        case ic0at24c256:
	            *pemIctype=ICTYPE_M_1_256;
	            break;
			case 0x78:
				 *pemIctype=ICTYPE_ISO7816;
				 break;
	        default:
	            nRet = NDK_ERR_IOCTL;
	    }
	}
    return NDK_OK;
}



/**
 *@brief    获取驱动程序版本号
 *@retval   pszVersion   　　驱动程序版本号,要求缓冲大小不低于16字节
 *　
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA            参数非法(pszVersion为NULL)
 *@li   NDK_ERR_OPEN_DEV        打开设备文件错误(打开IC卡设备文件失败)
 *@li   NDK_ERR_IOCTL           驱动调用错误(IC驱动接口ioctl_getvision调用失败返回)
*/
NEXPORT int  NDK_IccGetVersion(char *pszVersion)
{
    int nRet;

    if ((pszVersion==NULL)) {
        return NDK_ERR_PARA;
    }

    nRet = IccOpen_sc(ICTYPE_IC);
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_sc0, ioctl_getvision, &CMDarg);
    IccClose_sc(ICTYPE_IC);
    if (nRet == SUCC) {
        memset(pszVersion,'\0', 8);
        memcpy(pszVersion, CMDarg.ucBuf_rapdu, CMDarg.nRapdu_len);
    } else {
        pszVersion[0]='\0';
        return NDK_ERR_IOCTL;
    }

    CMDarg.nRapdu_len = 0;
    return NDK_OK;
}



/**
 *@brief    获取卡片状态
 *@retval   pnSta   bit0：如果IC卡1已插卡，为“1”，否则，为“0”
 *                  bit1：如果IC卡1已上电，为“1”，否则，为“0”
 *                  bit2：保留，返回值“0”
 *                  bit3：如果SAM卡4已上电，为“1”，否则，为“0”
 *                  bit4：如果SAM卡1已上电，为“1”，否则，为“0”
 *                  bit5：如果SAM卡2已上电，为“1”，否则，为“0”
 *                  bit6：如果SAM卡3已上电，为“1”，否则，为“0”
 *                  bit7：保留，返回值“0”
 *　
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA            参数错误(pnSta为NULL)
 *@li   NDK_ERR_OPEN_DEV        打开设备文件错误(打开IC卡设备文件失败)
*/
NEXPORT int  NDK_IccDetect(int *pnSta)
{
    int nRet;
    if(pnSta==NULL)
        return NDK_ERR_PARA;

    nRet = IccOpen_sc(ICTYPE_IC);
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    ioctl(nDevnum_sc0, N_GetState_c, &CMDarg);

    IccClose_sc(ICTYPE_IC);
    *pnSta = CMDarg.nRetflag & 0xff0000ff;
    return NDK_OK;
}


/**
 *@brief    上电
 *@param    emIctype　卡类型     ICC_IC 接触式IC卡,MEM卡
 *                    ICC_SAM1　ＳＡＭ１卡，
 *                    ICC_SAM2　ＳＡＭ２卡，
 *                    ICC_SAM3　ＳＡＭ３卡，
 *                    ICC_SAM4　ＳＡＭ４卡
  *                   ICTYPE_M_1     at24c01 at24c02 at24c04 at24c08 at24c16 at24c32 at24c64
 *                    ICTYPE_M_2     sle44x2
 *                    ICTYPE_M_3     sle44x8
 *                    ICTYPE_M_4     at88sc102
 *                    ICTYPE_M_5     at88sc1604
 *                                 ICTYPE_M_6     at88sc1608
 *@retval 　　psAtrbuf  表示ATR数据
 *@retval     pnAtrlen  表示ATR数据的长度
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA            参数非法(psAtrbuf/nAtrlen为NULL、emIctype非法)
 *@li   NDK_ERR             操作失败(MEMORY卡检测失败)
 *@li   NDK_ERR_OPEN_DEV        设备文件打开失败(IC卡设备文件打开失败)
 *@li   NDK_ERR_ICC_CARDNOREADY_ERR 卡未准备好
 *@li   NDK_ERR_IOCTL   驱动调用错误
*/
NEXPORT int  NDK_IccPowerUp (EM_ICTYPE emIctype, uchar *psAtrbuf,int *nAtrlen)
{
    int nRet;
    unsigned int i;
    EM_ICTYPE eCardtype;
	static int enanble_7816 = -1;
    ic_printf("\NDK_IccPowerUp\n");
    if((psAtrbuf==NULL)||(nAtrlen==NULL)) {
        ic_printf("\pow_up NDK_ERR_PARA1\n");
        return NDK_ERR_PARA;
    }
    if ((emIctype>lastcardnum)||(emIctype<0)) {
        ic_printf("\pow_up NDK_ERR_PARA2\n");
        return NDK_ERR_PARA;
    }

//--------------------------------------------------
#if 0
    nRet = NDK_IccGetType(&eCardtype);
    if (nRet != NDK_OK) {
        printf("\NDK_IccGetType\n");
        return nRet;

    }

    if ((emIctype < firstcardnum))
        goto iccc2;

#endif



    if((emIctype >= firstcardnum) &&(emIctype <= lastcardnum)&&(emIctype!=ICTYPE_ISO7816)) {
        ic_printf("\n pow_up IccMem_card_check\n");
        nRet = IccMem_card_check();
        if (nRet != NDK_OK) {
            return NDK_ERR;
        }
        IccMem_card_powerdown(emIctype);
        ndk_udelay(500);
        nRet = IccMem_card_powerup(emIctype, psAtrbuf,nAtrlen);
        if(nRet<0) {
            return NDK_ERR_OPEN_DEV;
        }
        return NDK_OK;
    } else {
        ic_printf("\niccc2\n");
		if(enanble_7816 == -1) ndk_getconfig("smartcard", "ic_7816_ctrl", CFG_INT, &enanble_7816);
		if(enanble_7816 == 1) {
			NDK_IccGetType(&eCardtype);
			if(eCardtype != ICTYPE_ISO7816) NDK_IccSetType(ICTYPE_ISO7816);
		}
        nRet = IccOpen_sc(emIctype);
        if (nRet<0) {
            return NDK_ERR_OPEN_DEV;
        }
        switch (emIctype) {
            case ICTYPE_IC:

                // 等待插卡 8秒
                nRet =0;
                i = 300; //20ms * 300 = 6s ioctl need time 2s
                while (i > 0) {
                    ioctl(nDevnum_sc0, N_GetState_c, &CMDarg);
                    if (CMDarg.nRetflag & 0x01) {
                        nRet = 1;
                        break;
                    }
                    ndk_udelay(20*1000); //20ms
                    --i;
                }
                if (nRet == 0 ) {
                    IccClose_sc(emIctype);
                    return NDK_ERR_ICC_CARDNOREADY_ERR;
                } else {
                    CMDarg.ucCmdflag = _IC1;
                }
                break;

            case ICTYPE_SAM1:
                CMDarg.ucCmdflag = _SAM1;
                break;
            case ICTYPE_SAM2:
                CMDarg.ucCmdflag = _SAM2;
                break;
            case ICTYPE_SAM3:
                CMDarg.ucCmdflag = _SAM3;
                break;
            default:
                nRet = NDK_ERR_PARA;
                break;
        }
        memset(CMDarg.ucBuf_capdu, 0, 30);
        memset(CMDarg.ucBuf_rapdu, 0, 30);
        CMDarg.nCapdu_len = 0;
        CMDarg.nRapdu_len = 0;
        CMDarg.nRetflag = 0;
        nRet = ioctl(nDevnum_sc0, ioctl_SC_PowerOn, &CMDarg);
        if (nRet < 0) {
            nRet = CMDarg.nRetflag;
            IccClose_sc(emIctype);
			if(nRet==0xffffffa0)
				return -100;
            return NDK_ERR_IOCTL;
        }
        memcpy(psAtrbuf, CMDarg.ucBuf_rapdu, CMDarg.nRapdu_len);
        *nAtrlen = CMDarg.nRapdu_len;
        nRet = NDK_OK;
    }

    IccClose_sc(emIctype);
    return nRet;
}


/**
 *@brief    下电
 *@param    emIctype　 ICC_IC 接触式IC卡,MEM卡　
 *                    ICC_SAM1　ＳＡＭ１卡，
 *                    ICC_SAM2　ＳＡＭ２卡，
 *                    ICC_SAM3　ＳＡＭ３卡，
 *                    ICC_SAM4　ＳＡＭ４卡
  *                   ICTYPE_M_1     at24c01 at24c02 at24c04 at24c08 at24c16 at24c32 at24c64
 *                    ICTYPE_M_2     sle44x2
 *                    ICTYPE_M_3     sle44x8
 *                    ICTYPE_M_4     at88sc102
 *                    ICTYPE_M_5     at88sc1604
 *                                 ICTYPE_M_6     at88sc1608
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA            参数非法(emIctype非法)
 *@li   NDK_ERR_OPEN_DEV        打开设备文件错误(打开IC卡设备文件失败)
 *@li   NDK_ERR_IOCTL   驱动调用错误
*/
NEXPORT int  NDK_IccPowerDown(EM_ICTYPE emIctype)
{
    int nRet = 0;
    //EM_ICTYPE eCardtype;
    if ((emIctype>lastcardnum)||(emIctype<0)) {
        ic_printf("\NDK_ERR_PARA2\n");
        return NDK_ERR_PARA;
    }
//---------------------------------------------------------------
//memory 卡
#if 0
    nRet = NDK_IccGetType(&eCardtype);
    if (nRet != SUCC)
        return nRet;


    if ((emIctype < firstcardnum))
        goto iccd2;
#endif
    if((emIctype >= firstcardnum) && (emIctype <= lastcardnum)&&(emIctype!=ICTYPE_ISO7816)) {
        nRet = IccMem_card_powerdown(emIctype);
        if(nRet<0) {
            return NDK_ERR_OPEN_DEV;
        }
        return NDK_OK;
    } else {
        nRet = IccOpen_sc(emIctype);
        if (nRet<0) {
            return NDK_ERR_OPEN_DEV;
        }

        switch (emIctype) {
            case ICTYPE_IC:
                CMDarg.ucCmdflag = _IC1;
                break;
            case ICTYPE_SAM1:
                CMDarg.ucCmdflag = _SAM1;
                break;
            case ICTYPE_SAM2:
                CMDarg.ucCmdflag = _SAM2;
                break;
            case ICTYPE_SAM3:
                CMDarg.ucCmdflag = _SAM3;
                break;
            default:
                nRet = NDK_ERR_PARA;
                break;
        }
        nRet = ioctl(nDevnum_sc0,  N_POWERDOWN, &CMDarg);
        if (nRet < 0) {
            nRet = CMDarg.nRetflag;
            return NDK_ERR_IOCTL;
        }
        nRet = NDK_OK;
        memset(CMDarg.ucBuf_capdu, 0, 30);
        memset(CMDarg.ucBuf_rapdu, 0, 30);
        CMDarg.nCapdu_len = 0;
        CMDarg.nRapdu_len = 0;
        CMDarg.nRetflag = 0;
    }
    IccClose_sc(emIctype);
    ndk_udelay(1000*200);
    return nRet;
}


/**
 *@brief    IC卡操作
 *@param    emIctype　 ICC_IC 接触式IC卡，MEM卡　　
 *                    ICC_SAM1　ＳＡＭ１卡，
 *                    ICC_SAM2　ＳＡＭ２卡，
 *                    ICC_SAM3　ＳＡＭ３卡，
 *                    ICC_SAM4　ＳＡＭ４卡
  *                   ICTYPE_M_1     at24c01 at24c02 at24c04 at24c08 at24c16 at24c32 at24c64
 *                    ICTYPE_M_2     sle44x2
 *                    ICTYPE_M_3     sle44x8
 *                    ICTYPE_M_4     at88sc102
 *                    ICTYPE_M_5     at88sc1604
 *                                 ICTYPE_M_6     at88sc1608
 *@param 　 nSendlen  　 发送数据的长度
 *@param    psSendbuf　　发送的数据
 *@retval   pnRecvlen    接收数据长度
 *@retval   psRecebuf    接收的数据
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA            参数非法(psSendbuf/pnRecvlen/psRecebuf为NULL、nSendlen小于0、emIctype非法)
 *@li   NDK_ERR_OPEN_DEV        打开设备文件错误(打开IC卡设备文件失败)
 *@li   NDK_ERR_IOCTL   驱动调用错误
*/
NEXPORT int  NDK_Iccrw(EM_ICTYPE emIctype, int nSendlen,uchar *psSendbuf, int *pnRecvlen,uchar *psRecebuf)
{
    int   nRet = 0;
    // EM_ICTYPE eCardtype;

    if((nSendlen<0)||(psSendbuf==NULL)||(pnRecvlen==NULL)||(psRecebuf==NULL)) {
        return NDK_ERR_PARA;
    }
    if ((emIctype>lastcardnum)||(emIctype<0)) {
        ic_printf("\icc_rw NDK_ERR_PARA2\n");
        return NDK_ERR_PARA;
    }

//--------------------------------------------------------------
//memory 卡
#if 0
    nRet = NDK_IccGetType(&eCardtype);
    if (nRet != NDK_OK)
        return nRet;

    if ((emIctype < firstcardnum))
        goto iccn2;
#endif
    if((emIctype >= firstcardnum) && (emIctype <= lastcardnum)&&(emIctype!=ICTYPE_ISO7816)) {
        nRet = IccMem_card_rw(emIctype, nSendlen, psSendbuf, pnRecvlen, psRecebuf);
        if(nRet<0) {
            //return NDK_ERR_OPEN_DEV;
            ic_printf("\icc_rw mem err %d\n",nRet);
            return NDK_ERR_PARA;
        }
        return NDK_OK;
    } else {
        nRet = IccOpen_sc(emIctype);
        if (nRet<0) {
            return NDK_ERR_OPEN_DEV;
        }

        switch (emIctype) {
            case ICTYPE_IC:
                CMDarg.ucCmdflag = _IC1;
                break;
            case ICTYPE_SAM1:
                CMDarg.ucCmdflag = _SAM1;
                break;
            case ICTYPE_SAM2:
                CMDarg.ucCmdflag = _SAM2;
                break;
            case ICTYPE_SAM3:
                CMDarg.ucCmdflag = _SAM3;
                break;
            default:
                *psRecebuf = 0;
                nRet = NDK_ERR_PARA;
                break;
        }
        memcpy(CMDarg.ucBuf_capdu, psSendbuf, nSendlen);
        CMDarg.nCapdu_len = nSendlen;
        CMDarg.nRapdu_len = 0;
        nRet = ioctl(nDevnum_sc0, ioctl_SC_APDU, &CMDarg);
        if (nRet == SUCC) {
            memcpy(psRecebuf, CMDarg.ucBuf_rapdu, CMDarg.nRapdu_len);
            *pnRecvlen = CMDarg.nRapdu_len;
        } else {
            *pnRecvlen =  0;
            nRet = CMDarg.nRetflag ;
            IccClose_sc(emIctype);
			if(nRet==0xffffffa0)
				return -100;
            return NDK_ERR_IOCTL;
        }
    }
    IccClose_sc(emIctype);
    return nRet;
}


/*--------------------------------------------------------
--------------------------------------------------------*/
static int IccOpen_sc(EM_ICTYPE emIctype)
{
    if (nDevnum_sc0 >= 0) {
        return SUCC;
    }
    switch (emIctype) {
        case ICTYPE_IC:
        case ICTYPE_SAM1:
        case ICTYPE_SAM2:
        case ICTYPE_SAM3:
        case ICTYPE_M_1:
		case ICTYPE_M_1_1:
		case ICTYPE_M_1_2:
		case ICTYPE_M_1_4:
		case ICTYPE_M_1_8:
		case ICTYPE_M_1_16:
		case ICTYPE_M_1_32:
		case ICTYPE_M_1_64:
        case ICTYPE_M_2:
        case ICTYPE_M_3:
        case ICTYPE_M_4:
        case ICTYPE_M_5:
        case ICTYPE_M_6:
		case ICTYPE_M_7:
            nDevnum_sc0 = open("/dev/sc0",O_RDWR);
            break;
        default:
            nDevnum_sc0 = FAIL;
    }
    if (nDevnum_sc0<0) {
        return NDK_ERR_OPEN_DEV;
    }

    return SUCC;
}

/*--------------------------------------------------------
--------------------------------------------------------*/
static int IccClose_sc(EM_ICTYPE emIctype)
{
    int nRet;

    switch (emIctype) {
        case ICTYPE_IC:
        case ICTYPE_SAM1:
        case ICTYPE_SAM2:
        case ICTYPE_SAM3:
        case ICTYPE_M_1:
		case ICTYPE_M_1_1:
		case ICTYPE_M_1_2:
		case ICTYPE_M_1_4:
		case ICTYPE_M_1_8:
		case ICTYPE_M_1_16:
		case ICTYPE_M_1_32:
		case ICTYPE_M_1_64:
        case ICTYPE_M_2:
        case ICTYPE_M_3:
        case ICTYPE_M_4:
        case ICTYPE_M_5:
        case ICTYPE_M_6:
		case ICTYPE_M_7:
            nRet = close(nDevnum_sc0);
            break;
        default:
            nRet = NDK_ERR;
    }
    nDevnum_sc0 = FAIL;
    nRet=SUCC;
    return nRet;

}

/*--------------------------------------------------------
初始化memorycard
    返回值 :直接利用ioctl函数返回
            SUCC 成功
            FAIL 失败
--------------------------------------------------------*/
static int IccInit_mem_card(void)
{

    int nRet;

    nRet = IccOpen_sc(ICTYPE_IC);
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_sc0, ioctl_mem_init_card, NULL);

    IccClose_sc(ICTYPE_IC);

    return(nRet);
}

/*--------------------------------------------------------
给memory卡上电
    参数: memerycardno  要操作的卡型号
          atrbuf        用与接收复位应答的数据，最少保证10个字节。
                        返回格式：数据长度(1Byte)+ATR
    返回值 : SUCC 成功
             FAIL 失败
--------------------------------------------------------*/
static int IccMem_card_powerup(EM_ICTYPE emIctype,uchar *psAtrbuf,int *pnAtrlen)
{

    int nRet;

    nRet = IccOpen_sc(ICTYPE_IC);
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }
    if ((emIctype >= firstcardnum) && (emIctype <= lastcardnum)) {
        switch (emIctype) {
			case ICTYPE_M_1:
			case ICTYPE_M_1_32:
				CMDarg.ucCmdflag = ic0at24c32;
				break;
			case ICTYPE_M_1_1:
				CMDarg.ucCmdflag = ic0at24c01;
				break;
			case ICTYPE_M_1_2:
				CMDarg.ucCmdflag = ic0at24c02;
				break;
			case ICTYPE_M_1_4:
				CMDarg.ucCmdflag = ic0at24c04;
				break;
			case ICTYPE_M_1_8:
				CMDarg.ucCmdflag = ic0at24c08;
				break;
			case ICTYPE_M_1_16:
				CMDarg.ucCmdflag = ic0at24c16;
				break;
			case ICTYPE_M_1_64:
				CMDarg.ucCmdflag = ic0at24c64;
				break;
			case ICTYPE_M_1_128:
				CMDarg.ucCmdflag = ic0at24c128;
				break;
			case ICTYPE_M_1_256:
				CMDarg.ucCmdflag = ic0at24c256;
				break;

            case ICTYPE_M_2:
                CMDarg.ucCmdflag =ic0sle44x2;
                break;
            case ICTYPE_M_3:
                CMDarg.ucCmdflag = ic0sle44x8;
                break;
            case ICTYPE_M_4:
                CMDarg.ucCmdflag = ic0at88sc102;
                break;
            case ICTYPE_M_5:
                CMDarg.ucCmdflag = ic0at88sc1604;
                break;
            case ICTYPE_M_6:
                CMDarg.ucCmdflag = ic0at88sc1608;
                break;
			case ICTYPE_M_7:
                CMDarg.ucCmdflag = ic0at88sc153;
                break;
            default:
                nRet = NDK_ERR_PARA;
                return nRet;
        }
        nRet = ioctl(nDevnum_sc0,  ioctl_mem_card_powerup, &CMDarg);
        if (nRet == SUCC) {
            *pnAtrlen = CMDarg.nRapdu_len;
            memcpy(psAtrbuf, CMDarg.ucBuf_rapdu, CMDarg.nRapdu_len);
            nRet = NDK_OK;
        } else {
            nRet = NDK_ERR_ICC_COPYERR;
        }
    } else {
        nRet = NDK_ERR_PARA;
    }

    IccClose_sc(ICTYPE_IC);

    return(nRet);
}

/*--------------------------------------------------------
给memory卡下电
    参数: memerycardno  要操作的卡型号
    返回值 : SUCC 成功
             FAIL 失败
--------------------------------------------------------*/
static int IccMem_card_powerdown(EM_ICTYPE emIctype)
{
    int nRet;

    nRet = IccOpen_sc(ICTYPE_IC);
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    if ((emIctype >= firstcardnum) && (emIctype <= lastcardnum)) {
        switch (emIctype) {
			case ICTYPE_M_1:
			case ICTYPE_M_1_32:
				CMDarg.ucCmdflag = ic0at24c32;
				break;
			case ICTYPE_M_1_1:
				CMDarg.ucCmdflag = ic0at24c01;
				break;
			case ICTYPE_M_1_2:
				CMDarg.ucCmdflag = ic0at24c02;
				break;
			case ICTYPE_M_1_4:
				CMDarg.ucCmdflag = ic0at24c04;
				break;
			case ICTYPE_M_1_8:
				CMDarg.ucCmdflag = ic0at24c08;
				break;
			case ICTYPE_M_1_16:
				CMDarg.ucCmdflag = ic0at24c16;
				break;
			case ICTYPE_M_1_64:
				CMDarg.ucCmdflag = ic0at24c64;
				break;
			case ICTYPE_M_1_128:
				CMDarg.ucCmdflag = ic0at24c128;
				break;
			case ICTYPE_M_1_256:
				CMDarg.ucCmdflag = ic0at24c256;
				break;
            case ICTYPE_M_2:
                CMDarg.ucCmdflag =ic0sle44x2;
                break;
            case ICTYPE_M_3:
                CMDarg.ucCmdflag = ic0sle44x8;
                break;
            case ICTYPE_M_4:
                CMDarg.ucCmdflag = ic0at88sc102;
                break;
            case ICTYPE_M_5:
                CMDarg.ucCmdflag = ic0at88sc1604;
                break;
            case ICTYPE_M_6:
                CMDarg.ucCmdflag = ic0at88sc1608;
                break;
            case ICTYPE_M_7:
                CMDarg.ucCmdflag = ic0at88sc153;
                break;
            default:
                nRet = NDK_ERR_PARA;
                return nRet;
        }
        nRet = ioctl(nDevnum_sc0,  ioctl_mem_card_powerdown, &CMDarg);
        if (nRet != SUCC) {
            nRet = NDK_ERR_ICC_WRITE_ERR;
        } else
            nRet=NDK_OK;
    } else {
        nRet = NDK_ERR_PARA;
    }

    IccClose_sc(ICTYPE_IC);

    return(nRet);
}

/*--------------------------------------------------------
检查memory是否在卡座内
--------------------------------------------------------*/
static int IccMem_card_check(void)
{
    int nRet;

    nRet = IccOpen_sc(ICTYPE_IC);
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_sc0, ioctl_mem_card_getsta, &CMDarg);

    IccClose_sc(ICTYPE_IC);

    if (CMDarg.nRetflag & 0x01)
        return NDK_OK;
    else
        return NDK_ERR_ICC_CARDPULL_ERR;
}
/*--------------------------------------------------------
检查memory是否上电
--------------------------------------------------------*/
static int IccMem_card_pwr_chk(void)
{
    int nRet;

    nRet = IccOpen_sc(ICTYPE_IC);
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_sc0, ioctl_mem_card_getsta, &CMDarg);

    IccClose_sc(ICTYPE_IC);

    if (CMDarg.nRetflag & 0x02)
        return SUCC;
    else
        return NDK_ERR_ICC_POWERON_ERR;
}
/*--------------------------------------------------------
检查memory是否上电
--------------------------------------------------------*/
static int IccRw_mem_card_check(void)
{
    int nRet;

    nRet = IccOpen_sc(ICTYPE_IC);
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    nRet = ioctl(nDevnum_sc0, ioctl_mem_card_getsta, &CMDarg);

    IccClose_sc(ICTYPE_IC);

    if ((CMDarg.nRetflag & 0x03) == 0x03)
        return SUCC;
    else
        return NDK_ERR_ICC_POWERON_ERR;

}


/*--------------------------------------------------------
memory卡APDU, 读写指定的卡型号
  参 数 : memerycardno:卡型号
          sendlen  : 缓冲区内的数据长度
          *sendbuf : 格式: cla ins P1 P2 le/lc data
          *rcvlen  : 接收到的数据的长度指针
          *recebuf : 接收数据返回缓冲区
  返回值 :
          SUCC:成功, FAIL:失败
--------------------------------------------------------*/
static int IccMem_card_rw(EM_ICTYPE emIctype, int nSendlen, uchar *psSendbuf, int *pnRcvlen, uchar *psRecebuf)
{

    int nRet;

    nRet = IccOpen_sc(ICTYPE_IC);
    if (nRet<0) {
        return NDK_ERR_OPEN_DEV;
    }

    *pnRcvlen = 0;

    nRet = ioctl(nDevnum_sc0, ioctl_mem_card_getsta, &CMDarg);
    /*if ((CMDarg.nRetflag & 0x01) == 0) {
        IccClose_sc(ICTYPE_IC);
        return NDK_ERR_ICC_CARDNOREADY_ERR;
    }
    if ((CMDarg.nRetflag & 0x02) == 0) {
        IccClose_sc(ICTYPE_IC);
        return NDK_ERR_ICC_POWERON_ERR;
    }*/

    if ( (emIctype < firstcardnum) || (emIctype > lastcardnum) )
        goto ec001;
    if ( (psSendbuf == NULL) || (pnRcvlen == NULL) || (psRecebuf == NULL) ||
         (nSendlen < 5) || (nSendlen >= rw_memorycard_maxlength) )
        goto ec001;
    if ( ( (psSendbuf[1] == 0xB0) || (psSendbuf[1] == 0x0E) ) && (nSendlen != 5) )
        goto ec001;

    if ( ( (psSendbuf[1] == 0xD0) || (psSendbuf[1] == 0x20) || (psSendbuf[1] == 0x82) ) && (psSendbuf[4] != nSendlen - 5) ) {
    ec001:
        *pnRcvlen = 0;
        IccClose_sc(ICTYPE_IC);
        return NDK_ERR_ICC_COM_ERR;
    }

    switch (emIctype) {
        case ICTYPE_M_1:
		case ICTYPE_M_1_32:
            CMDarg.ucCmdflag = ic0at24c32;
            break;
		case ICTYPE_M_1_1:
            CMDarg.ucCmdflag = ic0at24c01;
            break;
		case ICTYPE_M_1_2:
            CMDarg.ucCmdflag = ic0at24c02;
            break;
		case ICTYPE_M_1_4:
            CMDarg.ucCmdflag = ic0at24c04;
            break;
		case ICTYPE_M_1_8:
            CMDarg.ucCmdflag = ic0at24c08;
            break;
		case ICTYPE_M_1_16:
            CMDarg.ucCmdflag = ic0at24c16;
            break;
		case ICTYPE_M_1_64:
            CMDarg.ucCmdflag = ic0at24c64;
            break;
		case ICTYPE_M_1_128:
            CMDarg.ucCmdflag = ic0at24c128;
            break;
		case ICTYPE_M_1_256:
            CMDarg.ucCmdflag = ic0at24c256;
            break;
        case ICTYPE_M_2:
            CMDarg.ucCmdflag =ic0sle44x2;
            break;
        case ICTYPE_M_3:
            CMDarg.ucCmdflag = ic0sle44x8;
            break;
        case ICTYPE_M_4:
            CMDarg.ucCmdflag = ic0at88sc102;
            break;
        case ICTYPE_M_5:
            CMDarg.ucCmdflag = ic0at88sc1604;
            break;
        case ICTYPE_M_6:
            CMDarg.ucCmdflag = ic0at88sc1608;
            break;
		case ICTYPE_M_7:
            CMDarg.ucCmdflag = ic0at88sc153;
            break;
        default:
            nRet = NDK_ERR_PARA;
            return nRet;
    }
    ic_printf("mem_rw ictype = %d cmdflag = %d\n",emIctype,CMDarg.ucCmdflag);
    CMDarg.nCapdu_len = nSendlen;
    memcpy(CMDarg.ucBuf_capdu, psSendbuf, nSendlen);
    nRet = ioctl(nDevnum_sc0, ioctl_mem_card_rw, &CMDarg);
    IccClose_sc(ICTYPE_IC);
    ic_printf("mem_rw ret %d\n",nRet);
    if (CMDarg.nRapdu_len > 0) {
        *pnRcvlen = CMDarg.nRapdu_len;
        memcpy(psRecebuf, CMDarg.ucBuf_rapdu, CMDarg.nRapdu_len);
        if(nRet < 0)/*add by guoc*/
            return NDK_ERR_PARA;
    } else {
        *pnRcvlen = 0;

        return NDK_ERR_ICC_COPYERR;
    }


    return NDK_OK;

}








