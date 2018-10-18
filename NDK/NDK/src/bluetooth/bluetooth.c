#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sem.h>
#include <unistd.h>
#include <termios.h>
#include <sys/poll.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <libconfig.h>

#include "NDK.h"
#include "libbluetooth.h"
#include "parsecfg.h"


extern int bt_aux_fd;
static char bt_dev_info=0xFF;
static int bt_get_devinfo_flag=0;

static char ndk_get_bt_devinfo(config_t *cfg_file,char * devname)
{
    extern int ndk_getconfig_customize(config_t * cfg_file,const char * optname, const char * confname, cfgValueType type, void * confvalue);
    char *str;
    char c;
    int val;

    if(ndk_getconfig_customize(cfg_file,devname, "type",CFG_STRING,&str)<0)
        return 0xff;
    if(strcmp(str,"")==0)
        return 0xff;
    val = atoi(str);
    if(val == 0)
        return 0xff;
    else {
        c=val&0xff;
        return c;
    }
}

int ndk_is_bt_exist(void)
{
    int ret=-1; 
    config_t bt_devconf;  
    if(bt_get_devinfo_flag==0){
        ndk_initconfig_customize("/mnt/hwinfo/devmgr.conf",&bt_devconf);
        bt_dev_info=ndk_get_bt_devinfo(&bt_devconf,"bt");
        ndk_destoryconfig_customize(&bt_devconf);
        bt_get_devinfo_flag=1;
    }
    if(bt_dev_info==0xff)
        return 0;
    else
        return 1;
}

int NDK_BTReset(void)  
{
    int ret=-1;
    
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;
    if(BT_Disconnect()!=0)//如果处于连接,先断开连接
        return NDK_ERR;
    return bsa_set_config(1);
}

int NDK_BTSetLocalName(const char *pszName)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;
    
    if(NULL==pszName||15<strlen(pszName))
        return NDK_ERR_PARA;    

    return BT_SetName(pszName);
}

int NDK_BTGetLocalName(char *pszName)
{
    int ret=-1;
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;
    
    if(pszName==NULL)
        return NDK_ERR_PARA;

    ret=BT_GetName(pszName);
    return ret;
}

int NDK_BTSetPIN(const char *pszPinCode)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(NULL==pszPinCode||6<strlen(pszPinCode))
        return NDK_ERR_PARA;
        
    return BT_SetPin(pszPinCode);
}

int NDK_BTGetPIN(char *pszPinCode)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(NULL==pszPinCode)
        return NDK_ERR_PARA;
        
    BT_GetPin(pszPinCode);
    fprintf(stderr,"%s      %d      %s\n",__func__,__LINE__,pszPinCode);
    return NDK_OK;
}


int NDK_BTGetLocalMAC(char *pszMac)
{
    int ret=-1;

    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(pszMac==NULL)
        return NDK_ERR_PARA;

    ret=BT_GetMac(pszMac);
    return ret;
}

int NDK_BTStatus(int *pnStatus)
{    
    int status=-1;

    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(NULL==pnStatus)
        return NDK_ERR_PARA;

    status=BT_GetStatus();
    if(status==0)
        *pnStatus=1;//表示未连接
    else
        *pnStatus=0;
    return NDK_OK;
}

int NDK_BTEnterCommand(void)  //ME30接口，直接返回0
{
		return 0;
}
int NDK_BTExitCommand(void)  //ME30接口，直接返回0
{
		return 0;
}

/**
*@brief设置本机蓝牙模块MAC地址(此接口用于生产时设置产品的MAC地址。)
*@param	 pszMac	设定的蓝牙MAC地址，MAC地址长度必需为6个字节。如参数字节数偏大或偏小会导致错误的MAC地址被设置。
 *@return
 *@li	NDK_OK				操作成功
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		参数非法(pszMac为NULL)
 *@li	\ref NDK_ERR "NDK_ERR"	操作失败
*@li	       \ref NDK_ERR_OPEN_DEV  "NDK_ERR_OPEN_DEV"		设备未打开
*/
int NDK_BTSetLocalMAC(const char *pszMac)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(pszMac==NULL)
        return NDK_ERR_PARA;

    return BT_SetMac(pszMac); 
}
/**
 *@brief	断开当前连接
 *@return
 *@li	NDK_OK				操作成功
 *@li	\ref NDK_ERR "NDK_ERR"	操作失败
 *@li       \ref NDK_ERR_OPEN_DEV  "NDK_ERR_OPEN_DEV"		设备未打开
*/
int NDK_BTDisconnect(void)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    return BT_Disconnect();
}
/**
 *@brief	设置蓝牙配对模式
 *@param emMode 对应的四种配对模式，详见EM_PAIRING_MODE
 *@return
 *@li	NDK_OK				操作成功
 *@li	\ref NDK_ERR "NDK_ERR"	操作失败
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		传入非 EM_PAIRING_MODE的参数
 *@li       \ref NDK_ERR_OPEN_DEV  "NDK_ERR_OPEN_DEV"		设备未打开
*/
int NDK_BTSetPairingMode(EM_PAIRING_MODE emMode)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if (emMode != PAIRING_MODE_JUSTWORK && emMode != PAIRING_MODE_PINCODE
			&& emMode != PAIRING_MODE_SSP && emMode != PAIRING_MODE_PASSKEY)
		return NDK_ERR_PARA;

    return bsa_set_security(emMode);
}
/**
 *@brief	获取蓝牙配对状态(只对配对模式SSP PIN和PassKey有效)
 *@param pszKey SSP模式:pszKey返回手机上显示的配对码;PassKey模式：pszKey[0]返回'\0'，表明收到手机配对请求
 *@param pnStatus  1:收到手机配对请求；2: 配对成功；3配对失败
 *@return
 *@li	NDK_OK				操作成功
 *@li	\ref NDK_ERR "NDK_ERR"	操作失败
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		pszKey 或者 pnStatus 为NULL
 *@li       \ref NDK_ERR_OPEN_DEV  "NDK_ERR_OPEN_DEV"		设备未打开
*/
int NDK_BTGetPairingStatus(char * pszKey, int *pnStatus)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if (pszKey == NULL || pnStatus == NULL) 
        return NDK_ERR_PARA;

    return BT_GetPairingStatus(pszKey,pnStatus);
}
/**
 *@brief	蓝牙配对确认
 *@param pszKey     SSP模式:设置为NDK_BTPairGetStatus（）获取到的key;PassKey模式:键盘输入的key
 *@return
 *@li	NDK_OK				操作成功
 *@li	\ref NDK_ERR "NDK_ERR"	操作失败
 *@li	\ref NDK_ERR_PARA "NDK_ERR_PARA" 		pszKey为NULL
 *@li       \ref NDK_ERR_OPEN_DEV  "NDK_ERR_OPEN_DEV"		设备未打开
*/
int NDK_BTConfirmPairing(const char * pszKey, uint unConfirm)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if (pszKey == NULL) return NDK_ERR_PARA;

    return BT_PairingConfirm(pszKey,unConfirm);
}

int NDK_BTMasterScan(EM_BT_DEVICE_TYPE emDevType)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(emDevType!=BT_DEVICE_TYPE_BREDR&&emDevType!=BT_DEVICE_TYPE_BLE&&emDevType!=BT_DEVICE_TYPE_DUMO)
        return NDK_ERR_PARA;
        
    return BT_Scan(emDevType);
}

int NDK_BTMasterGetScanResults(const char *pszBtName,ST_BT_DEV *pstScanResults,unsigned int unMaxnum,int *pnNum)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(pstScanResults==NULL||pnNum==NULL)
        return NDK_ERR_PARA;
    
    return BT_GetScanResults(pszBtName,pstScanResults,unMaxnum,pnNum);
}

int NDK_BTMasterStopScan(void)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;
        
    return BT_StopScan();
}

int NDK_BTMasterBond(const char *pszMac)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(pszMac==NULL)
        return NDK_ERR_PARA;
    
    return BT_Bond(pszMac);
}

int NDK_BTMasterGetBondStatus(EM_PAIRING_MODE *pnMode,char *pszKey,int *pnStatus)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(pnMode==NULL||pszKey==NULL||pnStatus==NULL)
        return NDK_ERR_PARA;

    return BT_GetBondStatus(pnMode, pszKey,pnStatus);
}

int NDK_BTMasterBondConfirm(const char *pszKey,unsigned int unConfirm)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(pszKey==NULL)
        return NDK_ERR_PARA;

    if(unConfirm!=0&&unConfirm!=1)
        return NDK_ERR_PARA;

    return BT_BondConfirm(pszKey,  unConfirm);
}

int NDK_BTMasterConnect(const char *pszMac,int nDevType)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;    

    if(pszMac==NULL)
        return NDK_ERR_PARA;

    if(nDevType!=0&&nDevType!=1&&nDevType!=2)
        return NDK_ERR_PARA;

    return BT_Connect(pszMac,nDevType);
}

int NDK_BTMasterGetConStatus(const char *pszMac,int *pnStatus)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(pszMac==NULL||pnStatus==NULL)
        return NDK_ERR_PARA;

    return BT_ConStatus(pszMac ,pnStatus);
}

int NDK_BTMasterWrite(const char *pszMac,unsigned int unLen, const char *pszInbuf)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(pszMac==NULL||pszInbuf==NULL)
        return NDK_ERR_PARA;

    if(unLen<1)
        return NDK_ERR_PARA;
    return bt_write_master(pszMac,unLen,pszInbuf);
}

int NDK_BTMasterRead(const char *pszMac, unsigned int unLen, char *pszOutbuf, int nTimeoutMs, int *pnReadlen)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(pszMac==NULL||pszOutbuf==NULL||pnReadlen==NULL)
        return NDK_ERR_PARA;

    if(unLen>4096||unLen<1||nTimeoutMs<1)
        return NDK_ERR_PARA;

    return bt_read_master(pszMac,unLen,pszOutbuf,nTimeoutMs,pnReadlen);
}


int NDK_BTMasterReadLen(const char *pszMac,int *pnReadlen)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;

    if(pszMac==NULL||pnReadlen==NULL)
        return NDK_ERR_PARA;

    return bt_master_read_circ_buf_length(pszMac, pnReadlen);
}


int NDK_BTMasterClrBuf(const char *pszMac)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;   

    if(pszMac==NULL)
        return NDK_ERR_PARA;

    return bt_master_init_circ_buf(pszMac);
}

int NDK_BTMasterDisconnect(const char *pszMac)
{
    if(bt_aux_fd!=1)
        return NDK_ERR_OPEN_DEV;  

    if(pszMac==NULL)
        return NDK_ERR_PARA;

    return BT_Disconnect_master(pszMac);
}

