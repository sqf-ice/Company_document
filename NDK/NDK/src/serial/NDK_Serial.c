/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：
* 日    期：    2012-08-27
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
#include <assert.h>
#include <time.h>
#include <termios.h>
#include <linux/input.h>
#include <signal.h>
#include <dbus/dbus.h>
#include <stdbool.h>
#include <dbus/dbus-glib.h>
#include <glib.h>

#include "NDK_Serial.h"
#include "NDK.h"
#include "NDK_debug.h"
#include "serial_gui.h"
#include "../public/delay.h"
#include "devmgr.h"
#include "../modem/mdm_drv.h"
#include "parsecfg.h"
#include "../ppp/ppp.h"
#include "../sys/NDK_Sys.h"
#include "NDK_Net.h"
#include "../wirelessmodem/wirelessmodem.h"
#include "../bluetooth/libbluetooth.h"

#define AUX1_DEV    "/dev/ttyS0"
#define AUX2_DEV    "/dev/ttyS1"
#define AUX3_DEV    "/dev/ttyS2"
#define AUX4_DEV    "/dev/modem1"
#define AUX5_DEV    "/dev/modem0"
#define AUX6_DEV    "/dev/modem2"
#define USB_AUX_DEV "/dev/ttygs"
#define USB_SERIAL_DEV  "/dev/ttyUSB0"
#define USB_SERIAL_DEV2 "/dev/ttyUSB2"
#define USB_ACM_DEV3   "/dev/ttyACM3"



#define  ESC        0x1B    /**<取消键*/
#define DEV_CLASS_USB           "usb"
#define DEV_CLASS_SERIAL            "g_serial"
#define DEV_CLASS_STORAGE            "usb-storage"
#define DEV_CLASS_HCD            "za9l1_hcd"
#define DEV_CLASS_UDC            "za9l1_udc"
#define GS_MAJOR            127
#define GS_IOC_MAGIC        GS_MAJOR//add
#define GS_GETSTATUS        _IOW(GS_IOC_MAGIC, 0, int)//add
#define IOCTL_SERIAL_CLR_RXBUF _IOW(GS_IOC_MAGIC, 1, int)//add

#define CPLD_SWITCH_FITST (1 << 1) // bit1 用于控制是否是第一次调用cpld的切换操作，如果是则commserv需要同步初始化完成。
#define CPLD_WHICH_MASK   (1 << 0) //bit0 用于控制切换是无线还是有线modem   0-----有线 1-----无线


#define CPLD_SW_MODEM 0   //切换有线MODEM
#define CPLD_SW_WLMODEM 1 //切换无线MODEM


/**<内部变量*/
static int aux1_fd = -1;
static int aux2_fd = -1;
static int wlm_aux_fd = -1;
static int modem_aux_fd = -1;
static int mux_aux_fd = -1;
static int usb_aux_fd = -1;
static int scan_aux_fd = -1;
static int usb_host_fd = -1;
static int g_bt_nBps=0;
static int g_bt_nBps_flag=-1;

int bt_aux_fd=-1;

int gport_select = -1;             /*TTYS2外设选择*/
//static int config_usb_select = 0;
static int config_usb_host_type = 0;
static int config_usb_charge = -1;

char *pCom1Name = NULL;
char *pCom2Name = NULL;
char *pWlsName = NULL;
char *pScanName = NULL;
char *pMdmName = NULL;
char szCom1Name[32];
char szCom2Name[32];
char szWlsName[32];
char szScanName[32] = {0};
char szMdmName[32];

extern int BT_init(void);
extern int BT_Shutdown(void);
extern int ndk_cpu_type(void);
extern int ndk_ifunloaddrv(char *driver);
extern void dbus_g_connection_close( DBusGConnection *  connection );

void enablewirelessmodemport(void);
void enablewiremodemport(void);
//void dbus_switch_uart(int switch_uart,int *ret);

/**<内部函数声明*/
static int getAuxfd(EM_PORT_NUM emPort);
static int open_usb_aux(void);
static int close_usb_aux(void);
static int SetPort(char *pszBuf);
static int usb_slave_status(void);//0 表示未连接，1表示线连接着
static int close_usb_aux_host(void);

/**
 *@brief    串口复位函数，目前只适用于ME31S的usb串口
 *@param    emPort  指定的串口
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_PortReset(EM_PORT_NUM emPort)
{
	int fd;
	int ops = 0;
	
	if(emPort != PORT_NUM_USB){
		return NDK_ERR_PARA;
	}
	else{
		if (usb_aux_fd >= 0) {
        	close(usb_aux_fd);
    	}
    	usb_aux_fd = -1;
        unload_driver_by_class(DEV_CLASS_USB);

	    fd = open("/dev/power", O_RDWR);
	    if (fd < 0)
	        return NDK_ERR_OPEN_DEV;
	    if (ioctl(fd, POWER_SET_USB_STATUS, &ops) != 0) {
	        close(fd);
	        return NDK_ERR_IOCTL;
	    }
	    close(fd);
	}
	return NDK_OK;

}


/**
 *@brief    初始化串口，对串口波特率，数据位、奇偶位、停止位等进行设置。建议每次使用串口之前先调用该初始化函数。(USB是不需要波特率，但调用函数时还是要传一个，否则会报参数错误)\n
        支持的波特率分别为{300,1200,2400,4800,9600,19200,38400,57600,115200,230400}\n
        支持的数据位分别为{8,7,6,5}\n
        校验方式选择分别为{N(n):无校验;O(o):奇校验;E(e):偶校验}\n
        支持的停止位分别为{1,2}
 *@param    emPort  指定的串口
 *@param    pszAttr 通讯率和格式串,例"115200,8,N,1",如果只写波特率则缺省为"8,N,1"。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_PortOpen(EM_PORT_NUM emPort, const char *pszAttr)
{
    int aux_fd = -1;
    struct termios myCfg, *cfg=&myCfg;
    int ret = -1;
    int nBps = 0;
    int tmp_ret,timeout = 10;
    char *pstr1,pstr2,pstr3;
    char *pFlag, *pAux;
    int i;
    int usbstatus;
    if (NULL == pszAttr&&PORT_NUM_BT!=emPort) { //考虑蓝牙可以传递NULL参数
        return NDK_ERR_PARA;
    }

    switch ( emPort ) {
        case PORT_NUM_COM1: {
            if ( aux1_fd>= 0) {
                close(aux1_fd);
            }

            if (pCom1Name == NULL) {
                if ((get_dev_info("aux", "port", szCom1Name)==0)&&(SetPort(szCom1Name) == NDK_OK)) {
                    pCom1Name = szCom1Name;
                    aux1_fd = open( szCom1Name, O_RDWR |O_NONBLOCK);
                    aux_fd = aux1_fd;
                } else {
                    aux1_fd = open( AUX1_DEV, O_RDWR |O_NONBLOCK);
                    aux_fd = aux1_fd;
                }
            } else {
                aux1_fd = open( pCom1Name, O_RDWR |O_NONBLOCK);
                aux_fd = aux1_fd;
            }
            break;
        }
        case PORT_NUM_COM2: {
            if ( aux2_fd>= 0) {
                close(aux2_fd);
            }

            if (pCom2Name == NULL) {
                if ((get_dev_info("aux", "port", szCom2Name)==0)&&((pAux = strstr(szCom2Name, "ttyS")) != NULL)) {
                    sprintf(szCom2Name, "%s", pAux+1);
                    if (SetPort(szCom2Name) == NDK_OK) {
                        pCom2Name = szCom2Name;
                        aux2_fd = open( szCom2Name, O_RDWR |O_NONBLOCK);
                        aux_fd = aux2_fd;
                    } else {
                        aux2_fd = open( AUX2_DEV, O_RDWR |O_NONBLOCK);
                        aux_fd = aux2_fd;
                    }
                } else {
                    aux2_fd = open( AUX2_DEV, O_RDWR |O_NONBLOCK);
                    aux_fd = aux2_fd;
                }
            } else {
                aux2_fd = open( pCom2Name, O_RDWR |O_NONBLOCK);
                aux_fd = aux2_fd;
            }
            break;
        }
        case PORT_NUM_WIRELESS: {
            if ( wlm_aux_fd>= 0) {
                close(wlm_aux_fd);
                wlm_aux_fd=-1;
            }

            if (pWlsName == NULL) {
                tmp_ret = ndk_WLMGetType();
                if( tmp_ret == WLM_WCDMA_H350) {
                    enablewirelessmodemport();
                    wlm_aux_fd = open( USB_ACM_DEV3, O_RDWR |O_NONBLOCK);
                    aux_fd = wlm_aux_fd;
                } else if( tmp_ret == WLM_WCDMA_EHS5_USB) {
                    enablewirelessmodemport();
                    wlm_aux_fd = open( USB_ACM_DEV3, O_RDWR |O_NONBLOCK);
                    aux_fd = wlm_aux_fd;
				}else if(tmp_ret == WLM_EVDO_DE910_USB){
					enablewirelessmodemport();
                    wlm_aux_fd = open( USB_SERIAL_DEV2, O_RDWR |O_NONBLOCK);
	                aux_fd = wlm_aux_fd;
                } else {
                    if((get_dev_info("wlsmdm", "port", szWlsName)==0)&&(SetPort(szWlsName) == NDK_OK)) {
                        enablewirelessmodemport();
                        pWlsName = szWlsName;
                        wlm_aux_fd = open( szWlsName, O_RDWR |O_NONBLOCK);
                        aux_fd = wlm_aux_fd;
                    } else {
                        enablewirelessmodemport();
                        wlm_aux_fd = open( AUX3_DEV, O_RDWR |O_NONBLOCK);
                        aux_fd = wlm_aux_fd;
                    }
                }
            } else {
                enablewirelessmodemport();
                wlm_aux_fd = open( szWlsName, O_RDWR |O_NONBLOCK);
                aux_fd = wlm_aux_fd;
            }
            break;
        }
        case PORT_NUM_MODEM: {
            if ( modem_aux_fd>= 0) {
                close(modem_aux_fd);
            }
            enablewiremodemport();
            if (pMdmName == NULL) {

                if ((get_dev_info("mdm", "port", szMdmName)==0)&&(SetPort(szMdmName) == NDK_OK)) {
                    pMdmName = szMdmName;
                    modem_aux_fd = open( szMdmName, O_RDWR |O_NONBLOCK);
                    aux_fd = modem_aux_fd;
                } else {
                    modem_aux_fd = open( AUX3_DEV, O_RDWR |O_NONBLOCK);
                    aux_fd = modem_aux_fd;
                }
            } else {
                modem_aux_fd = open( pMdmName, O_RDWR |O_NONBLOCK);
                aux_fd = modem_aux_fd;
            }
            break;
        }
        case PORT_NUM_MUX1: {
            if ( mux_aux_fd>= 0) {
                close(mux_aux_fd);
            }
            enablewirelessmodemport();
            mux_aux_fd = open( AUX4_DEV, O_RDWR |O_NONBLOCK);
            aux_fd = mux_aux_fd;
            break;
        }
        case PORT_NUM_MUX2: {
            if ( mux_aux_fd>= 0) {
                close(mux_aux_fd);
            }
            enablewirelessmodemport();
            mux_aux_fd = open( AUX5_DEV, O_RDWR |O_NONBLOCK);
            aux_fd = mux_aux_fd;
            break;
        }
        case PORT_NUM_MUX3: {
            if ( mux_aux_fd>= 0) {
                close(mux_aux_fd);
            }
            enablewirelessmodemport();
            mux_aux_fd = open( AUX6_DEV, O_RDWR |O_NONBLOCK);
            aux_fd = mux_aux_fd;
            break;
        }
        case PORT_NUM_WIFI: {
            break;
        }
        case PORT_NUM_USB: {
            if ( usb_aux_fd>= 0)
                close(usb_aux_fd);
            usb_aux_fd = -1;
            ret = open_usb_aux();
            if ( ret < 0)
                return NDK_ERR_IOCTL;
            if(ret == 1) {
                for (i=0; i<6; i++) {
                    ndk_msdelay(500);
                    if ((usb_aux_fd = open(USB_AUX_DEV, O_RDWR | O_NONBLOCK)) >= 0)
                        break;

                }
            } else {
                usb_aux_fd = open(USB_AUX_DEV, O_RDWR | O_NONBLOCK);
            }
            aux_fd = usb_aux_fd;
            if(aux_fd >= 0) {
                if(ioctl(usb_aux_fd,IOCTL_SERIAL_CLR_RXBUF) < 0) {
                    fprintf(stderr,"ioctl_serial_clr_rxbuf err\n");
                    return NDK_ERR;
                }
                usbstatus = usb_slave_status();
                if(usbstatus == 0) {
                    close_usb_aux();
                    fprintf(stderr,"ndk_usb_serial_open err with usb line not connect but connect before\n");
                    return NDK_ERR_USB_LINE_UNCONNECT;
                }
                return NDK_OK;
            } else {
                close_usb_aux();
                fprintf(stderr,"ndk_usb_serial_open err with usb line not connect all\n");
                return NDK_ERR_USB_LINE_UNCONNECT;
            }
            break;
        }
        case PORT_NUM_SCAN: {
            if ( scan_aux_fd>= 0) {
                close(scan_aux_fd);
            }
            if (pScanName == NULL) {
                if((get_dev_info("scanner", "port", szScanName)==0)&&((pAux = strstr(szScanName, "ttyS")) != NULL)) {
                    //sprintf(szScanName, "%s", pAux+1);
                    if (SetPort(szScanName) == NDK_OK) {
                        pScanName = szScanName;
                        scan_aux_fd= open( szScanName, O_RDWR |O_NONBLOCK);
                        if(scan_aux_fd<=0) {
                            perror("open scan dev error:");
                            break;
                        }
                        aux_fd = scan_aux_fd;
                    } else {
                        scan_aux_fd = open( AUX2_DEV, O_RDWR |O_NONBLOCK);
                        aux_fd = scan_aux_fd;
                    }
                } else {
                    scan_aux_fd = open( AUX2_DEV, O_RDWR |O_NONBLOCK);
                    aux_fd = scan_aux_fd;
                }
            } else {
                scan_aux_fd = open( pScanName, O_RDWR |O_NONBLOCK);
                aux_fd = scan_aux_fd;
            }
            break;
        }
        case PORT_NUM_USB_HOST: {
            tmp_ret = strtol(pszAttr,&pstr1,10);
            if((pstr1 != NULL)&& (strlen(pstr1) > 7)) {
                timeout = atoi(pstr1+7);
                fprintf(stderr,"PORT_NUM_USB_HOST-timeout:%d,strlen :%d\n",timeout,strlen(pstr1));
                if(timeout <= 0)
                    timeout = 10;
            }
            if (ndk_load_usb_host() < 0)
                return NDK_ERR;

            if ( usb_host_fd>= 0)
                close(usb_host_fd);
            usb_host_fd = -1;
            for (i=0; i<timeout; i++) {
                ndk_msdelay(1000);

                if ((usb_host_fd = open(USB_SERIAL_DEV, O_RDWR | O_NONBLOCK)) >= 0)
                    break;

            }
            aux_fd = usb_host_fd;
            break;
        }
        case PORT_NUM_BT: {
            if(ndk_is_bt_exist()!=1){
                return NDK_ERR;
            }
            if(bt_aux_fd==1) {
                //close(bt_aux_fd);
                return NDK_OK;
            }
            if(NULL==pszAttr)
                g_bt_nBps_flag=0;
            else {
                g_bt_nBps = strtol(pszAttr,&pFlag,10);
                if(g_bt_nBps!=2400&&g_bt_nBps!=4800&&g_bt_nBps!=7200&&g_bt_nBps!=9600&&g_bt_nBps!=19200&&g_bt_nBps!=38400&&g_bt_nBps!=57600&&g_bt_nBps!=115200&&g_bt_nBps!=230400)
                    return NDK_ERR_PARA;
            }
            if(BT_init()==0){
                bt_aux_fd=1;
                return NDK_OK;
           }else
                return NDK_ERR;
                
        }
        default: {
            return NDK_ERR_PARA;
        }
    }
    if (aux_fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }


    ret = tcgetattr(aux_fd,cfg);/**<获取与aux_fd相关的参数,保存在cfg中*/
    if (ret < 0) {
        if (PORT_NUM_USB == emPort) {
            close_usb_aux();
        }
        return NDK_ERR;
    }

    cfmakeraw(cfg); /**<设置终端属性*/


    nBps = strtol(pszAttr,&pFlag,10);

    switch (nBps) {
        case 300    : {
            cfsetispeed(cfg,B300)   ;
            cfsetospeed(cfg,B300)   ;
            break;
        }
        case 1200   : {
            cfsetispeed(cfg,B1200)  ;
            cfsetospeed(cfg,B1200)  ;
            break;
        }
        case 2400   : {
            cfsetispeed(cfg,B2400)  ;
            cfsetospeed(cfg,B2400)  ;
            break;
        }
        case 4800   : {
            cfsetispeed(cfg,B4800)  ;
            cfsetospeed(cfg,B4800)  ;
            break;
        }
        case 9600   : {
            cfsetispeed(cfg,B9600)  ;
            cfsetospeed(cfg,B9600)  ;
            break;
        }
        case 19200  : {
            cfsetispeed(cfg,B19200) ;
            cfsetospeed(cfg,B19200) ;
            break;
        }
        case 38400  : {
            cfsetispeed(cfg,B38400) ;
            cfsetospeed(cfg,B38400) ;
            break;
        }
        case 57600  : {
            cfsetispeed(cfg,B57600) ;
            cfsetospeed(cfg,B57600) ;
            break;
        }
        case 115200 : {
            cfsetispeed(cfg,B115200);
            cfsetospeed(cfg,B115200);
            break;
        }
        case 230400 : {
            cfsetispeed(cfg,B230400);
            cfsetospeed(cfg,B230400);
            break;
        }
        default        : {
            if (PORT_NUM_USB == emPort) {
                close_usb_aux();
            }
            return NDK_ERR_PARA;
        }
    }

    cfg->c_iflag &= ~IXON;
    cfg->c_iflag &= ~IGNBRK;
    cfg->c_cflag &= ~CSIZE;
    if (emPort == PORT_NUM_MODEM)
        cfg->c_cflag |=  CRTSCTS ;//有线modem开启硬件流控

    if (0 == strlen(pFlag)) { /**<缺省*/
        cfg->c_cflag |= CS8;    /**<8个数据位*/
        cfg->c_cflag&=~CSTOPB;  /**<1个停止位*/
        cfg->c_cflag &= ~PARENB;/**<无检验*/
    } else if ((',' == pFlag[0]) && (',' == pFlag[2]) && (',' == pFlag[4])) {
        if ('8' == pFlag[1]) {
            cfg->c_cflag |= CS8;    /**<8个数据位*/
        } else if ('7' == pFlag[1]) {
            cfg->c_cflag |= CS7;    /**<7个数据位*/
        } else if ('6' == pFlag[1]) {
            cfg->c_cflag |= CS6;    /**<6个数据位*/
        } else if ('5' == pFlag[1]) {
            cfg->c_cflag |= CS5;    /**<5个数据位*/
        } else {
            if (PORT_NUM_USB == emPort) {
                close_usb_aux();
            }
            return NDK_ERR_PARA;
        }

        if ( ('n' == pFlag[3]) || ('N' == pFlag[3]) ) {
            cfg->c_cflag &= ~PARENB;/**<无检验*/
        } else if ( ('o' == pFlag[3]) || ('O' == pFlag[3]) ) {
            cfg->c_cflag |= PARENB; /**<奇检验*/
            cfg->c_cflag |= PARODD;
        } else if ( ('e' == pFlag[3]) || ('E' == pFlag[3]) ) {
            cfg->c_cflag |= PARENB; /**<偶检验*/
            cfg->c_cflag &= ~PARODD;
        } else {
            if (PORT_NUM_USB == emPort) {
                close_usb_aux();
            }
            return NDK_ERR_PARA;
        }

        if ('1' == pFlag[5]) {
            cfg->c_cflag&=~CSTOPB;  /**<1个停止位*/
        } else if ('2' == pFlag[5]) {
            cfg->c_cflag|=CSTOPB;   /**<2个停止位*/
        } else {
            if (PORT_NUM_USB == emPort) {
                close_usb_aux();
            }
            return NDK_ERR_PARA;
        }
    } else {
        if (PORT_NUM_USB == emPort) {
            close_usb_aux();
        }
        return NDK_ERR_PARA;
    }

    ret = tcsetattr(aux_fd,TCSANOW,cfg);
    if (ret < 0) {
        if (PORT_NUM_USB == emPort) {
            close_usb_aux();
        }
        return NDK_ERR;
    }

    ret = NDK_PortClrBuf(emPort);/**<清接收缓冲区*/
    if (ret < 0) {
        if (PORT_NUM_USB == emPort) {
            close_usb_aux();
        }
        return ret;
    }
    NDK_LOG_INFO(NDK_LOG_MODULE_PORT,"%s succ,current port set is (port num:%d,Baudrate:%s)\n",__func__,emPort,pszAttr);
    return NDK_OK;
}


/**
*@brief 关闭串口
*@param emPort  指定的串口
*@return
*@li    NDK_OK              操作成功
*@li    其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_PortClose(EM_PORT_NUM emPort)
{
    switch ( emPort ) {
        case PORT_NUM_COM1: {
            if (aux1_fd >= 0) {
                close(aux1_fd);
                aux1_fd = -1;
            } else
                return NDK_ERR;
            break;
        }
        case PORT_NUM_COM2: {
            if (aux2_fd >= 0) {
                close(aux2_fd);
                aux2_fd = -1;
            } else
                return NDK_ERR;
            break;
        }
        case PORT_NUM_SCAN: {
            if (scan_aux_fd>= 0) {
                close(scan_aux_fd);
                scan_aux_fd = -1;
            } else
                return NDK_ERR;
            break;
        }
        case PORT_NUM_WIRELESS: {
            if (wlm_aux_fd >= 0) {
                close(wlm_aux_fd);
                wlm_aux_fd = -1;
            } else
                return NDK_ERR;
            break;
        }
        case PORT_NUM_MODEM: {
            if (modem_aux_fd >= 0) {
                close(modem_aux_fd);
                modem_aux_fd = -1;
            } else
                return NDK_ERR;
            break;
        }
        case PORT_NUM_MUX1:
        case PORT_NUM_MUX2:
        case PORT_NUM_MUX3: {
            if (mux_aux_fd >= 0) {
                close(mux_aux_fd);
                mux_aux_fd = -1;
            } else
                return NDK_ERR;
            break;
        }
        case PORT_NUM_WIFI: {
            break;
        }
        case PORT_NUM_USB: {
            close_usb_aux();
            break;
        }
        case PORT_NUM_USB_HOST: {
            close_usb_aux_host();
            break;
        }
        case PORT_NUM_BT: {
            if (bt_aux_fd == -1) {
                return NDK_OK;
            }
            if(BT_Shutdown()==0){
                bt_aux_fd=-1;
                return NDK_OK;
            }else
                return NDK_ERR;
            break;
        }
        default: {
            return NDK_ERR_PARA;
        }
    }
    NDK_LOG_INFO(NDK_LOG_MODULE_PORT,"call %s,port num is closed\n",__func__,emPort);
    return NDK_OK;
}

int ndk_portread(EM_PORT_NUM emPort, uint unLen, char *pszOutbuf,int nTimeoutMs, int *pnReadlen)
{
    int aux_fd = -1;
    int nfds = -1;
    int ret = -1;
    struct timeval tv;
    struct termios myCfg;
    fd_set readfds;
    int ret_nCount = 0;
    int read_nCount = 0;
    int usbstatus,retval = NDK_OK;
    sigset_t newmask, oldmask;

    if ((NULL == pszOutbuf) || (NULL == pnReadlen)) {
        return NDK_ERR_PARA;
    }
    *pnReadlen = 0;

    if ((emPort < PORT_NUM_COM1) || (emPort > PORT_NUM_BT) || (unLen < 1)  || (nTimeoutMs < 0)) {
        return NDK_ERR_PARA;
    }

    

    aux_fd = getAuxfd(emPort);

    if (aux_fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }

    if(emPort==PORT_NUM_BT) {
        return bt_read_server(unLen,pszOutbuf,nTimeoutMs,pnReadlen);
    }

    tv.tv_sec=nTimeoutMs/1000;
    tv.tv_usec=(nTimeoutMs%1000)*1000;
    //tv.tv_sec=10;
    //tv.tv_usec=0;

    ret_nCount=0;
    read_nCount=unLen;



    /*   sp60 bcm5892 cpu 走不同分支       */
    if(emPort == PORT_NUM_USB) {
        usbstatus = usb_slave_status();
        if(usbstatus == 0) {
            //fprintf(stderr,"ndk_usb_serial_write err with usb line not connect\n");
            return NDK_ERR_USB_LINE_UNCONNECT;
        }
        while (1) {
            FD_ZERO(&readfds);
            FD_SET(aux_fd, &readfds);

            nfds=select(aux_fd+1, &readfds, NULL, NULL, &tv);
            if (nfds < 0) {
                if (EINTR == errno)
                    continue;
                retval = NDK_ERR;
                goto exit;
            }
            if (nfds > 0) {
                if (FD_ISSET(aux_fd, &readfds)) {
                    /*无线信号、电源状态刷新需要使用SIGALRM信号(widget/notifier.c)，此处可能被该信号中断，因此需要暂时屏蔽*/
                    sigemptyset(&newmask);
                    sigaddset(&newmask, SIGALRM);
                    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
                    ret = read( aux_fd, pszOutbuf+ret_nCount, read_nCount );
                    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                    if (ret < 0) {
                        retval = NDK_ERR_READ;
                        goto exit;
                    } else if (0 == ret) {
                        *pnReadlen = ret_nCount;
                        break;
                    } else if (ret < read_nCount) {
                        read_nCount=read_nCount-ret;
                        ret_nCount=ret_nCount+ret;
                        continue;
                    }
                    ret_nCount = ret_nCount+ret;
                    *pnReadlen = ret_nCount;
                    break;
                }
            } else {
                if (0 == ret_nCount) {
                    retval = NDK_ERR_TIMEOUT;
                    goto exit;
                } else if (ret_nCount < unLen) {
                    *pnReadlen = ret_nCount;
                }
                break;
            }
        }
    exit:
        if(emPort == PORT_NUM_USB) {
            usbstatus = usb_slave_status();
            if(usbstatus == 0) {
                fprintf(stderr,"ndk_usb_serial_read err with usb line not connect\n");
                return NDK_ERR_USB_LINE_UNCONNECT;
            } else
                return retval;
        } else
            return retval;
    } else {
        while (1) {
            FD_ZERO(&readfds);
            FD_SET(aux_fd, &readfds);
            nfds=select(aux_fd+1, &readfds, NULL, NULL, &tv);
            if (nfds < 0) {
                if (EINTR == errno) {
                    continue;
                }
                return NDK_ERR;
            }
            if (nfds > 0) {
                if (FD_ISSET(aux_fd, &readfds)) {
                    /*无线信号、电源状态刷新需要使用SIGALRM信号(widget/notifier.c)，此处可能被该信号中断，因此需要暂时屏蔽*/
                    sigemptyset(&newmask);
                    sigaddset(&newmask, SIGALRM);
                    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
                    ret = read( aux_fd, pszOutbuf+ret_nCount, read_nCount );
                    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                    if (ret < 0) {
                        return NDK_ERR_READ;
                    } else if (0 == ret) {
                        *pnReadlen = ret_nCount;
                        break;
                    } else if (ret < read_nCount) {
                        read_nCount=read_nCount-ret;
                        ret_nCount=ret_nCount+ret;
                        continue;
                    }
                    ret_nCount = ret_nCount+ret;
                    *pnReadlen = ret_nCount;
                    break;
                }
            } else {
                if (0 == ret_nCount) {
                    return NDK_ERR_TIMEOUT;
                } else if (ret_nCount < unLen) {
                    *pnReadlen = ret_nCount;
                }
                break;
            }
        }
        //NDK_LOG_INFO(NDK_LOG_MODULE_PORT,"%s succ,port read len=%d\n",__func__,*pnReadlen);
        return NDK_OK;
    }
}

/**
*@brief 在设定超时时间里从指定的串口，读取指定长度的数据，存放于pszOutbuf
*@param emPort  指定的串口
*@param unLen   表示要读的数据长度,>0(PORT_NUM_COM1、PORT_NUM_COM2、PORT_NUM_USB这三种不超过16K，其它不超过4K)
*@param nTimeoutMs  等待时间，单位为毫秒
*@retval    pszOutbuf   接收数据缓冲区的头指针
*@retval    pnReadlen   返回读的实际长度
*@return
*@li    NDK_OK              操作成功
*@li    其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_PortRead(EM_PORT_NUM emPort, uint unLen, char *pszOutbuf,int nTimeoutMs, int *pnReadlen)
{
    if(emPort==PORT_NUM_COM1||emPort==PORT_NUM_COM2||emPort==PORT_NUM_USB) {
        if(unLen>16*1024)
            return NDK_ERR_PARA;
    } else {
        if(unLen>4*1024)
            return NDK_ERR_PARA;
    }
    return ndk_portread(emPort,unLen,pszOutbuf,nTimeoutMs,pnReadlen);
}


/**
*@brief     给指定的串口送指定长度的数据，存放于pszInbuf
*@param emPort  指定的串口
*@param unLen   表示要写的数据长度
*@param pszInbuf    数据发送的缓冲区
*@return
*@li    NDK_OK              操作成功
*@li    其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_PortWrite(EM_PORT_NUM emPort, uint unLen,const char *pszInbuf)
{
    int aux_fd = -1;
    int nfds = -1;
    int ret = -1;
    fd_set writefds;
    sigset_t newmask, oldmask;
    int ret_nCount = 0;
    int write_nCount = 0;
    int usbstatus,retval = NDK_OK;
    struct timeval tvp;
    struct termios myCfg;
    if (NULL == pszInbuf) {
        return NDK_ERR_PARA;
    }

    if ((emPort < PORT_NUM_COM1) || (emPort > PORT_NUM_BT) || (unLen < 1)) { /**<去掉4K限制*/
        return NDK_ERR_PARA;
    }

    
    aux_fd = getAuxfd(emPort);

    if (aux_fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }

    if(emPort==PORT_NUM_BT) {

        return bt_write(unLen,pszInbuf);
    }

    write_nCount = unLen;

    /*   sp60 bcm5892 cpu 走不同分支       */
    if(emPort == PORT_NUM_USB) {
        usbstatus = usb_slave_status();
        if(usbstatus == 0) {
            //fprintf(stderr,"ndk_usb_serial_write err with usb line not connect\n");
            return NDK_ERR_USB_LINE_UNCONNECT;
        }
        tvp.tv_sec = 5;
        tvp.tv_usec = 0;
        while(1) {
            FD_ZERO(&writefds);
            FD_SET(aux_fd, &writefds);

            nfds=select(aux_fd + 1,  NULL, &writefds, NULL, &tvp);  /**<改为阻塞方式*/
            if (nfds < 0) {
                if (EINTR == errno)
                    continue;
            }
            if(nfds == 0) {
                if(emPort == PORT_NUM_USB) {
                    usbstatus = usb_slave_status();
                    if(usbstatus == 1) {
                        close(usb_aux_fd);
                        usb_aux_fd = -1;
                        unload_driver_by_class(DEV_CLASS_USB);
                    }
                }
                retval = NDK_ERR;
                goto exit;
            }

            if (nfds>0) {
                if (FD_ISSET(aux_fd, &writefds)) {
                    /**
                    *无线信号、电源状态刷新需要使用SIGALRM信号(widget/notifier.c)，此处可能被该信号中断，因此需要暂时屏蔽
                    */
                    sigemptyset(&newmask);
                    sigaddset(&newmask, SIGALRM);
                    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
                    ret = write(aux_fd, pszInbuf+ret_nCount, write_nCount);
                    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                    if (ret < 0) {
                        retval = NDK_ERR_WRITE;
                        goto exit;
                    } else if (ret == 0) {
                        retval = NDK_ERR_WRITE;
                        goto exit;
                    } else if (ret < write_nCount) {
                        write_nCount=write_nCount-ret;
                        ret_nCount=ret_nCount+ret;
                        continue;
                    }
                    ret_nCount = ret_nCount+ret;
                    retval  = NDK_OK;
                    goto exit;
                } else {
                    retval = NDK_ERR;
                    goto exit;
                }
            }
            retval  = NDK_OK;
            goto exit;
        }
    exit:
        if(emPort == PORT_NUM_USB) {
            usbstatus = usb_slave_status();
            if(usbstatus == 0) {
                fprintf(stderr,"ndk_usb_serial_write err with usb line not connect\n");
                return NDK_ERR_USB_LINE_UNCONNECT;
            } else
                return retval;
        } else
            return retval;

    } else {
        if(emPort == PORT_NUM_USB_HOST) {
            tvp.tv_sec = 5;
            tvp.tv_usec = 0;
        }
        while(1) {
            FD_ZERO(&writefds);
            FD_SET(aux_fd, &writefds);
            if(emPort == PORT_NUM_USB_HOST) {
                nfds=select(aux_fd + 1,  NULL, &writefds, NULL, &tvp);
            } else
                nfds=select(aux_fd + 1,  NULL, &writefds, NULL, NULL);  /**<改为阻塞方式*/
            if (nfds < 0) {
                if (EINTR == errno) {
                    continue;
                }
            }
            if(nfds == 0) {
                return NDK_ERR;
            }

            if (nfds>0) {
                if (FD_ISSET(aux_fd, &writefds)) {
                    if ((write_nCount&63) || (write_nCount > 4096)) {
                        /**
                        *无线信号、电源状态刷新需要使用SIGALRM信号(widget/notifier.c)，此处可能被该信号中断，因此需要暂时屏蔽
                        */
                        sigemptyset(&newmask);
                        sigaddset(&newmask, SIGALRM);
                        sigprocmask(SIG_BLOCK, &newmask, &oldmask);
                        ret = write(aux_fd, pszInbuf+ret_nCount, write_nCount);
                        sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                        if (ret < 0) {
                            return NDK_ERR_WRITE;
                        } else if (ret == 0) {
                            return NDK_ERR_WRITE;
                        } else if (ret < write_nCount) {
                            write_nCount=write_nCount-ret;
                            ret_nCount=ret_nCount+ret;
                            continue;
                        }

                        ret_nCount = ret_nCount+ret;
                        return NDK_OK;
                    } else {
                        /**
                        *无线信号、电源状态刷新需要使用SIGALRM信号(widget/notifier.c)，此处可能被该信号中断，因此需要暂时屏蔽
                        */
                        sigemptyset(&newmask);
                        sigaddset(&newmask, SIGALRM);
                        sigprocmask(SIG_BLOCK, &newmask, &oldmask);
                        if (write(aux_fd, pszInbuf+ret_nCount, 1) != 1) {
                            sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                            return NDK_ERR_WRITE;
                        }
                        sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                        write_nCount -= 1;
                        ret_nCount += 1;
                        if(write_nCount) {
                            continue;
                        }
                    }
                } else {
                    return NDK_ERR;
                }
            }
            NDK_LOG_INFO(NDK_LOG_MODULE_PORT,"%s succ,port write len=%d\n",__func__,unLen);
            return NDK_OK;
        }
    }
}


/**
*@brief 判断指定串口发送缓冲区是否为空
*@param emPort  指定的串口
*@return
*@li    NDK_OK  缓冲区无数据
*@li    NDK_ERR 缓冲区有数据
*/
NEXPORT int NDK_PortTxSendOver(EM_PORT_NUM emPort)
{
    return NDK_OK;
}


/**
*@brief 清除指定串口的接收缓冲区
*@param emPort  指定的串口
*@return
*@li    NDK_OK              操作成功
*@li    其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_PortClrBuf(EM_PORT_NUM emPort)
{
    int aux_fd = -1;
    int ret = -1;
    int len = 0;
    char recbuf[101];
    struct termios myCfg;


    if ((emPort < PORT_NUM_COM1) || (emPort > PORT_NUM_BT)) {
        return NDK_ERR_PARA;
    }
    /*   sp60 bcm5892 cpu 走不同分支       */
    if(emPort == PORT_NUM_USB) {

        if(usb_aux_fd <= 0)
            return NDK_ERR_OPEN_DEV;
        if(ioctl(usb_aux_fd,IOCTL_SERIAL_CLR_RXBUF) < 0)
            return NDK_ERR;
        else
            return NDK_OK;
    } else {
        

        aux_fd = getAuxfd(emPort);
        if (aux_fd < 0) {
            return NDK_ERR_OPEN_DEV;
        }

        if(emPort == PORT_NUM_BT) {
            bt_init_circ_buf();
            return NDK_OK;
        }

        /**
        *去掉清发送缓冲，只保留清接收缓冲
        */
        ret = ioctl(aux_fd, TCFLSH, 0);
        if (ret < 0) {
            return NDK_ERR;
        }
        NDK_LOG_INFO(NDK_LOG_MODULE_PORT,"call %s,port num is %d\n",__func__,emPort);
        return NDK_OK;
    }
}


/**
*@brief 取缓冲区里有多少字节要被读取
*@param emPort  指定的串口
*@retval    pnReadlen   返回缓冲区被读取的长度
*@return
*@li    NDK_OK              操作成功
*@li    其它EM_NDK_ERRCODE      操作失败
*/
NEXPORT int NDK_PortReadLen(EM_PORT_NUM emPort,int *pnReadlen)
{
    int aux_fd = -1;
    struct timeval tv;
    int nread = 0;
    fd_set readfds;
    struct termios myCfg;
    int nfds = -1;
    sigset_t newmask, oldmask;

    if (NULL == pnReadlen) {
        return NDK_ERR_PARA;
    }

    if ((emPort < PORT_NUM_COM1) || (emPort > PORT_NUM_BT)) {
        return NDK_ERR_PARA;
    }

    

    aux_fd = getAuxfd(emPort);
    if (aux_fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }

    if(emPort == PORT_NUM_BT) {
        bt_read_circ_buf_length(pnReadlen);
        return NDK_OK;
    }

    tv.tv_sec = 0;
    tv.tv_usec = 10 * 1000;

    /**
    *无线信号、电源状态刷新需要使用SIGALRM信号(widget/notifier.c)，此处可能被该信号中断，因此需要暂时屏蔽
    */
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    FD_ZERO(&readfds);
    FD_SET(aux_fd, &readfds);

    nfds = select(aux_fd + 1, &readfds, NULL, NULL, &tv);
    if (nfds < 0) {
        sigprocmask(SIG_UNBLOCK, &newmask, NULL);
        return NDK_ERR;
    }
    if (0 == nfds) {
        sigprocmask(SIG_UNBLOCK, &newmask, NULL);
        *pnReadlen = 0;
        return NDK_OK;
    }
    if (nfds > 0) {
        if (FD_ISSET(aux_fd, &readfds)) {
            ioctl(aux_fd, FIONREAD, &nread);    /**<得到缓冲区里有多少字节要被读取，然后将字节数放入nread里面*/
        }
    }

    *pnReadlen = nread;
    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
    return NDK_OK;
}

/************************************************************************/
/**
*@brief :根据配置文件信息设置串口ttyS
*@param :[pszBuf]   参数信息
*@return:
*@li        NDK_OK = 0 --- 成功
*@li        其它 EM_NDK_ERRCODE 操作失败
*/
static int SetPort(char *pszBuf)
{
    char *pPort;

    if ((pPort = strstr(pszBuf, "ttyS")) != NULL) {
        switch(pPort[4]) {
            case '0':
                memset(pszBuf, 0, sizeof(pszBuf));
                strcpy(pszBuf, "/dev/ttyS0");
                break;
            case '1':
                memset(pszBuf, 0, sizeof(pszBuf));
                strcpy(pszBuf, "/dev/ttyS1");
                break;
            case '2':
                memset(pszBuf, 0, sizeof(pszBuf));
                strcpy(pszBuf, "/dev/ttyS2");
                break;
            case '3':
                memset(pszBuf, 0, sizeof(pszBuf));
                strcpy(pszBuf, "/dev/ttyS3");
                break;
            default:
                return NDK_ERR;
                break;
        }
        return NDK_OK;
    } else
        return NDK_ERR;
}

/**
*@brief :取串口文件标示符
*@param :[emPort]   指定的串口
*@return:
*@li        NDK_OK = 0 --- 成功
*@li        其它 EM_NDK_ERRCODE 操作失败
*/
static int getAuxfd(EM_PORT_NUM emPort)
{
    int aux_fd=-1;
    switch (emPort) {
        case PORT_NUM_COM1:
            aux_fd = aux1_fd;
            break;
        case PORT_NUM_COM2:
            aux_fd = aux2_fd;
            break;
        case PORT_NUM_WIRELESS:
            aux_fd = wlm_aux_fd;
            if(aux_fd>=0)
                enablewirelessmodemport();
            break;
        case PORT_NUM_MODEM:
            aux_fd = modem_aux_fd;
            if(aux_fd>=0)
                enablewiremodemport();
            break;
        case PORT_NUM_MUX1:
        case PORT_NUM_MUX2:
        case PORT_NUM_MUX3:
            aux_fd = mux_aux_fd;
            if(aux_fd>=0)
                enablewirelessmodemport();
            break;
        case PORT_NUM_WIFI:
            break;
        case PORT_NUM_USB:
            aux_fd = usb_aux_fd;
            break;
        case PORT_NUM_SCAN:
            aux_fd = scan_aux_fd;
            break;
        case PORT_NUM_USB_HOST:
            aux_fd = usb_host_fd;
            break;
        case PORT_NUM_BT:
            aux_fd =bt_aux_fd;
            break;
        default:
            break;
    }
    return aux_fd;
}



int switch_uart(int port)
{
    int ret = -1;
    int count = 0;
    comm_methodcall_general(FUNC_CPLD_SWITCH_UART,port,&ret);
    while(ret) {
        count ++;
        if(count >= 6)
            break;
        comm_methodcall_general(FUNC_CPLD_SWITCH_UART,port,&ret);
        ndk_msdelay(1000);
    }
    return ret;
}

void enablewirelessmodemport(void)
{
    int type;

    type = 0;
    if(gport_select!=CPLD_SW_WLMODEM) {
        if(gport_select == -1) {
            type |= CPLD_SWITCH_FITST;
        }
        type |=CPLD_SW_WLMODEM;
        if(mdm_drv_exist() == 1) {
            mdm_drv_sleep();
        }
        switch_uart(type);
        gport_select = CPLD_SW_WLMODEM;
    }
}

void enablewiremodemport(void)
{
    int type;

    type = 0;
    if(gport_select!=CPLD_SW_MODEM) {
        if(gport_select == -1)
            type |= CPLD_SWITCH_FITST;
        type |=CPLD_SW_MODEM;
        if(mdm_drv_exist() == 1) {
            mdm_drv_reset();
        }

        switch_uart(type);
        gport_select = CPLD_SW_MODEM;
    }
}

static int open_usb_aux(void)
{
    int ret1,ret2,ret;
    int fd = -1;
    int ops = 1;
    int load_flag=-1;

    /*   sp60 bcm5892 cpu 走不同分支       */
    if(ndk_cpu_type() == 1) {

        if((system("lsmod | grep usbserial")) == 0) {
            if (ndk_getconfig("usb","port",CFG_INT,&config_usb_host_type)<0)
                config_usb_host_type = 0;
            unload_driver_by_name("usbserial");
            if(config_usb_host_type == 0) {
                unload_driver_by_name("bcm589x_otg");
                unload_driver_by_name("dwc_common_port_lib");
            } else
                unload_driver_by_name("bcm589x_usb_host");
            unload_driver_by_name("usbcore");
        }
        if((system("mount | grep /mnt/usbsd"))==0) {
            fprintf(stderr,"ndk-waring:open usb aux with usb host not unload\n");
            system("sudo umount /mnt/usbsd");
            unload_driver_by_name("usb_storage");
            if(config_usb_host_type == 0) {
                unload_driver_by_name("bcm589x_otg");
                unload_driver_by_name("dwc_common_port_lib");
            } else
                unload_driver_by_name("bcm589x_usb_host");
            unload_driver_by_name("usbcore");
            unload_driver_by_name("sd_mod");
            unload_driver_by_name("scsi_mod");
        }
        //unload_driver_by_class(DEV_CLASS_USB);
        if (ndk_ifunloaddrv("bcm589x_usbcdc") != 0) {
            load_flag = 1;
            if(load_driver_by_class(DEV_CLASS_USB)<0)
                return NDK_ERR;
        }

    } else {
        if((system("mount | grep /mnt/usbsd"))==0) {
            system("sudo umount /mnt/usbsd");
            ret1 = unload_driver_by_class(DEV_CLASS_HCD);
            ret2 = unload_driver_by_class(DEV_CLASS_STORAGE);
        }

        if (ndk_ifunloaddrv("za9l-usbcdc.ko") != 0) {
            fd = open("/dev/gpio", O_RDWR);
            if (fd < 0)
                return NDK_ERR;
            if (ioctl(fd, GPIO_IOCS_USB_SLAVE, &ops) != 0) {
                close(fd);
                return NDK_ERR;
            }
            close(fd);
            ret = load_driver_by_class(DEV_CLASS_USB);
            load_flag = 1;
            if (ret != 0) {
                fprintf(stderr,"load err\n");
                return NDK_ERR;

            }
        }
    }
    fd = open("/dev/power", O_RDWR);
    if (fd < 0)
        return NDK_ERR;
    if (ioctl(fd, POWER_SET_USB_STATUS, &ops) != 0) {
        close(fd);
        return NDK_ERR;
    }
    close(fd);
    if(load_flag == 1)
        return 1;
    else
        return 0;
}

static int close_usb_aux(void)
{
    int fd = -1;
    int ops = 0;
#if 0
    /*   sp60 bcm5892 cpu 走不同分支       */
    if(ndk_cpu_type() == 1) {
        if (usb_aux_fd >= 0) {
            close(usb_aux_fd);
        }
        usb_aux_fd = -1;
        //unload_driver_by_class(DEV_CLASS_USB);
    } else {
        if (usb_aux_fd >= 0) {
            close(usb_aux_fd);
        }
        usb_aux_fd = -1;
        unload_driver_by_class(DEV_CLASS_USB);
        ndk_msdelay(10);
    }
#endif
    if (usb_aux_fd >= 0) {
        close(usb_aux_fd);
    }
    usb_aux_fd = -1;

    if(config_usb_charge == -1) {
        if (ndk_getconfig("usb","charge",CFG_INT,&config_usb_charge) < 0) {
            config_usb_charge = 0;
        }
    }
    if(config_usb_charge == 1) {
        unload_driver_by_class(DEV_CLASS_USB);
    }

    fd = open("/dev/power", O_RDWR);
    if (fd < 0)
        return NDK_ERR;
    if (ioctl(fd, POWER_SET_USB_STATUS, &ops) != 0) {
        close(fd);
        return NDK_ERR;
    }
    close(fd);

    return NDK_OK;
}


int usb_slave_status(void)//0 表示未连接，1表示线连接着
{
    int ret = 0;
    int param = 0;

    if(usb_aux_fd > 0)
        ret = ioctl(usb_aux_fd, GS_GETSTATUS, &param);
    else {
        fprintf(stderr,"usb get line connect status err with usb not open\n");
        return -1;
    }
    if (ret < 0) {
        return -1;
    }
    return param;
}

int ndk_load_usb_host()
{
    int ret,fd;
    int tmp_config_usbhosttype = 0;  // 5892 usb u盘大小口的选择，1 大口USB1 ,0 小口USB0
    ret = NDK_OK;
    int ops = 1;

    if(ndk_cpu_type() == 1) {
        if (ndk_getconfig("usb","port",CFG_INT,&tmp_config_usbhosttype)<0)
            tmp_config_usbhosttype = 0;
        if(ndk_ifunloaddrv("bcm589x-usbcdc")==0) {
            if(unload_driver_by_name("bcm589x-usbcdc")!=0)
                fprintf(stderr,"NDK-err---------[%s][%d]\n",__func__,__LINE__);
        }
        if((system("mount | grep /mnt/usbsd"))==0) {
            fprintf(stderr,"ndk-waring:open NDK_SysModuleInit with usb host not unload\n");
            system("sudo umount /mnt/usbsd");
            unload_driver_by_name("usb_storage");
            if(tmp_config_usbhosttype == 0) {
                unload_driver_by_name("bcm589x_otg");
                unload_driver_by_name("dwc_common_port_lib");
            } else
                unload_driver_by_name("bcm589x_usb_host");
            unload_driver_by_name("usbcore");
            unload_driver_by_name("sd_mod");
            unload_driver_by_name("scsi_mod");
        }
        if(ndk_ifunloaddrv("usbcore")!=0)
            if(load_driver_by_name("usbcore","")!=0)
                return NDK_ERR_USDDISK_DRIVELOADFAIL;
        if(tmp_config_usbhosttype == 0) {
            if(ndk_ifunloaddrv("dwc_common_port_lib")!=0)
                if(load_driver_by_name("dwc_common_port_lib","")!=0)
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
            if(ndk_ifunloaddrv("bcm589x_otg")!=0)
                if(load_driver_by_name("bcm589x_otg","")!=0)
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
        } else {
            if(ndk_ifunloaddrv("bcm589x-usb-host")!=0)
                if(load_driver_by_name("bcm589x-usb-host","")!=0)
                    return NDK_ERR_USDDISK_DRIVELOADFAIL;
        }
        if(ndk_ifunloaddrv("usbserial")!=0)
            if(load_driver_by_name("usbserial","vendor=0x0730 product=0xdcba")!=0)
                return NDK_ERR_USDDISK_DRIVELOADFAIL;
    } else
        ret = NDK_ERR;

    fd = open("/dev/power", O_RDWR);
    if (fd < 0)
        return NDK_ERR;
    if (ioctl(fd, POWER_SET_USB_STATUS, &ops) != 0) {
        close(fd);
        return NDK_ERR;
    }
    close(fd);
    return ret;
}

static int close_usb_aux_host(void)
{
    int fd;
    int ops = 0;

    if (usb_host_fd >= 0) {
        close(usb_host_fd);
        usb_host_fd = -1;
    }
    if((system("lsmod | grep usbserial")) == 0) {
        if (ndk_getconfig("usb","port",CFG_INT,&config_usb_host_type)<0)
            config_usb_host_type = 0;
        unload_driver_by_name("usbserial");
        if(config_usb_host_type == 0) {
            unload_driver_by_name("bcm589x_otg");
            unload_driver_by_name("dwc_common_port_lib");
        } else
            unload_driver_by_name("bcm589x_usb_host");
        unload_driver_by_name("usbcore");
    }

    fd = open("/dev/power", O_RDWR);
    if (fd < 0)
        return NDK_ERR;
    if (ioctl(fd, POWER_SET_USB_STATUS, &ops) != 0) {
        close(fd);
        return NDK_ERR;
    }
    close(fd);
    return NDK_OK;
}
#if 0
void dbus_switch_uart(int switch_uart,int *ret)
{
    GMainContext* main_context = NULL;
    DBusGConnection *bus;
    DBusGProxy *remote_object=NULL;
    GError *error = NULL;
    gint result = 0;
    main_context = g_main_context_new();
    g_type_init ();
    dbus_g_thread_init();
    bus = dbus_g_bus_get_private(DBUS_BUS_SESSION,main_context,&error);
    remote_object = dbus_g_proxy_new_for_name (bus,
                    "nl.phoenix.commserv",
                    "/CommServObject",
                    "nl.commserv.interface");
    dbus_g_proxy_call (remote_object, "cpld_switch_uart", &error, G_TYPE_INT, switch_uart, G_TYPE_INVALID, G_TYPE_INT, &result, G_TYPE_INVALID);
    *ret = result;
    g_object_unref (G_OBJECT (remote_object));
    dbus_g_connection_close(bus);
    dbus_g_connection_unref(bus);
    g_main_context_unref(main_context);
}
#endif
