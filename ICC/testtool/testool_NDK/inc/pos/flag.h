/*
 * FileName:       
 * Author:         zhangnw  Version: 1.6.3  Date: 2006-8-2
 * Description:   POS产品公共头文件（各模块的公共文件的内容也可放于此处）
 *			  本文件可放TCR产品共同的特性,若只是某产品特有的，还可定义在INC\TCR下它自己的目录中
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version>  <desc>
 *	 				201706054         从lib.h中分离出来
 */

#ifndef _FLAG_H_
#define _FLAG_H_

/*-----------------------includes-------------------------------*/
//#include "mode.h"

/*---------------constants/macro definition---------------------*/

//平台标志位，用于减少测试代码中的条件编译代码量
#define    ANDIROD_ENABLE (defined N900AZ||defined N910AZ||defined IM81AZ||defined N700||defined N850||defined X5||defined N920||defined N510||defined N550||defined N910P) //安卓平台
#define	K21_ENABLE	(defined ME31||defined SP10||defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME32||defined ME65||defined ME30GX||defined ME32GX||defined ME32THM||defined ME11||defined ME15||defined ME15C||defined ME15CHX||defined ME15B||defined ME20||defined ME50NX||defined ME68||defined ME66||CPU5810X_ENABLE||ANDIROD_ENABLE)//低端平台产品包括K21和GD32等||defined IM81||defined N900统一为IM81AZ /N900AZ
#define	CPU5892_ENABLE 	(defined SP60||defined SP50||defined SP80||defined ME31S||defined ME31SLS||defined SP33||defined SP600||defined GP720||defined ME50S||defined ME50NS)//国内版本NDK产品
#define	CPU5830X_ENABLE  (defined SP930||defined SP930P||defined SP630)//CPU 5830X平台产品
#define	CPUMDM_ENABLE (defined ME31SMDM||defined SP930MDM) //CPU MDM9x07平台产品
#define    CPU5810X_ENABLE (defined ME50||defined ME50N||defined SP610||defined ME62||defined ME50C||defined ME50H)//CPU5810X平台产品属于低端产品
#define	GP_ENABLE	(defined GP730||defined NL8510GP)//国内GP平台产品
#define 	OVERSEAS_ENABLE	(defined SP60G||defined SP50G||defined SP80G||defined SP33G||defined SP600G||defined SP930G||defined SP930PG||defined SP630PG||defined SP830PG||defined ME51||CPUMDM_ENABLE)//国际版本NDK产品

//无键盘标志，没有键盘的设备需要在这里设置标志
#define KB_DISABLE	(defined NL829STD||defined ME11||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B||defined ME65||defined ME66)//||defined IM81AZ	//||defined N900||defined N900AZ||defined N910AZ

//无液晶标志，没有液晶的设备需要在这里设置标志
#define SCR_DISABLE	(defined NL829STD|defined SP10||defined ME11||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B||defined ME66)//||defined IM81|||defined N900||defined N900AZ||defined N910AZ||defined IM81AZ

//硬件不支持1磁道的标志
#define TK1_DISABLE	(defined ME11||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B||defined ME30THM||defined ME32THM)

//电池标志:无法取出电池的设备需要在这里设置标志
#define BATTERY_GETOUT_ENABLE !(defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME32||defined ME65||defined ME30GX||defined ME32GX||defined ME32THM||defined ME11||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B||defined ME66||CPU5810X_ENABLE||defined ME50NX||defined ME68||defined ME50S||defined ME50NS||defined ME31S||defined ME31SLS||defined SP33||defined SP600||defined SP33G||defined SP600G||defined ME51)

//安全模块国密算法支持标志
#define SEC_SM4_ENABLE	(CPU5892_ENABLE||CPU5830X_ENABLE||defined ME30||(ANDIROD_ENABLE&&!defined N910P))//||defined ME30THM

//安全模块AES算法支持标志
#define SEC_AES_ENABLE	 ANDIROD_ENABLE

//ALG模块国密算法支持标志
#define ALG_SM_ENABLE	(CPU5892_ENABLE||CPU5830X_ENABLE||defined ME30||CPU5810X_ENABLE||defined ME50NX||defined ME68||ANDIROD_ENABLE)//||defined ME30THM

//新wifi函数支持标志
#define NEW_WIFI_ENABLE (defined ME50||defined ME50N||defined ME50C||defined ME50H||defined ME62||defined ME50NX||defined ME68)

//TR31密钥安装格式支持的标志
#define TR31_ENABLE !(defined ME15C||defined ME15CHX||defined ME15B)	//ME15C不支持TR31格式密钥

//CBC模式安装格式支持的标志
#define CBC_LOAD_ENABLE (ANDIROD_ENABLE&&!defined N910P)

//DUKPT密钥体系支持的标志
#define DUKPT_ENABLE !(defined ME15C||defined ME15CHX||defined ME15B)	//ME15C不支持DUKPT格式密钥体系

//ALG模块RSA算法函数支持的标志,对应RSA软算法
#define ALG_RSA_ENABLE !(CPU5810X_ENABLE||defined ME50NX||defined ME68||defined ME15||defined ME15C||defined ME15CHX||defined ME15B||defined ME30S||defined ME30MH||defined ME30GX||defined ME32||defined ME65||defined ME32GX)	//ME15C不支持RSA算法CPU5810X_ENABLE||

//SEC模块RSA密钥函数支持的标志,对应RSA硬算法
#define SEC_RSA_ENABLE (CPU5892_ENABLE||CPU5830X_ENABLE)	//仅国内5892平台支持RSA密钥安装

//SHA算法支持的标志
#define SHA_ENABLE !(defined ME15C||defined ME15CHX||defined ME15B)	//ME15C不支持SHA算法

//PIN算法支持的标志
#define PINKEY_ENABLE !(defined ME15C||defined ME15CHX||defined ME15B)	//ME15C不支持PIN算法

//蓝牙仅支持JUSTWORK模式的标志
#define JUSTWORK_FLAG (defined ME15C||defined ME15CHX||defined ME15B||defined ME20)	//ME15C ME20 仅支持JUSTWORK模式||defined ME30MH

//低端蓝牙如YC1021芯片需要手动开启广播标志位
#define SET_BTDISCOVER_ENABLE (defined ME30MH ||defined ME30GX||defined ME32||defined ME32GX||defined ME32THM||defined ME65||defined ME30THM)

#define	ENABLE		1
#define	DISABLE		0

//海外产品固件7.0版本以上开关
#if 0
#define OVERSEAS_OS70_ENABLE ENABLE
#else
#define OVERSEAS_OS70_ENABLE DISABLE
#endif

//4.2以上蓝牙版本开关
#if 0
#define BT42_ENABLE  ENABLE
#else
#define BT42_ENABLE  DISABLE
#endif
//PCI认证版本蓝牙标志 
#if 0
#define BTPCI_ENABLE  ENABLE
#else
#define BTPCI_ENABLE  DISABLE
#endif
//蓝牙支持PINCODE模式使能
#define PINCODE_ENABLE  !(BT42_ENABLE&& BTPCI_ENABLE)//4.2以上蓝牙且是PCI版本无此配对模式
//蓝牙支持JUSTWORK AND SC模式使能   新增的配对模式 (国内版本)
#define JUSTWORK_SC_ENABLE  (BT42_ENABLE && !BTPCI_ENABLE)//4.2以上蓝牙且是BLE协议新增此配对模式
//蓝牙支持just work模式使能(有蓝牙的都有支持此模式 只有PCI认证版本无此模式)
#define JUSTWORK_ENABLE  !BTPCI_ENABLE // PCI认证版本不支持

//摄像头支持标志
#define CAMERA_ENABLE 0//defined SP600

//扫描头非阻塞支持标志
#define SCANASYCN_ENABLE (defined ME50||defined ME50NX||defined ME68||defined ME66||defined ME50N||defined ME50C||defined ME50H)//||defined ME50N//||defined SP600

//扫描头支持标志(阻塞)
#define SCAN_ENABLE (CPU5892_ENABLE||CPU5830X_ENABLE||defined SP600G||defined ME50||defined ME51||defined ME50N||defined ME50C||defined ME50H)

//软解码扫描头支持标志
#define SCAN_SOFT_ENABLE defined SP630||defined SP630PG

//双SIM卡硬件支持标志
#define DOUBLESIM_ENABLE (defined SP600||defined ME51)

//海外版本蓝牙MFI测试支持标志
#if 0
	#define	BTMFI_ENABLE	ENABLE
#else
	#define	BTMFI_ENABLE	DISABLE
#endif

//低端WiFi是否支持优先级功能标志位(内置协议栈的Wifi模块不支持,自主协议栈支持)
#if 0
	#define  WIFI_PRIORITY_ENABLE 	ENABLE   //正基模块
#else
	#define  WIFI_PRIORITY_ENABLE	DISABLE  //乐鑫 九九物联模块
#endif

//格式化输出不支持 开关
#define SPRINTF_UNSPORT ANDIROD_ENABLE

//Statusbar
#if !(defined GP730||defined NL8510GP||K21_ENABLE || defined SP600G || defined SP600)
	#define	STABAR_ENABLE	ENABLE
#else
	#define	STABAR_ENABLE	DISABLE
#endif

//状态栏模拟LED灯支持标志
#if CPU5892_ENABLE|| CPU5830X_ENABLE||OVERSEAS_ENABLE
	#define  	STABAR_LED_ENABLE 	ENABLE
#else
	#define 	STABAR_LED_ENABLE 	DISABLE
#endif

//mag
#if !(defined N510||defined N550)
	#define	MAG_ENABLE	 ENABLE
#else
	#define	MAG_ENABLE	 DISABLE
#endif

#if 0
#define READCARD3_SUPPORT
#endif

//IC/SAM
#if !(defined ME50N||defined ME20||defined ME62||defined ME50NX||defined ME68||defined ME50C||defined ME50H||defined N510||defined N550)//defined ME50支持IC卡510不支持IC卡支持sam1卡
	#define	IC1_ENABLE	ENABLE
#else
	#define	IC1_ENABLE	DISABLE
#endif

#define IC2_ENABLE DISABLE

#if !(defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME32||defined ME65||defined ME30GX||defined ME32GX||defined ME32THM||defined ME11||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B||CPU5810X_ENABLE||defined ME50NX||defined ME68||defined ME66||defined ME50S||defined ME50NS||defined SP600G||defined ME51||defined N550)  //20140218 chensj ,跟开发确认ME30不支持小卡
	#define SAM1_ENABLE ENABLE
#else
	#define SAM1_ENABLE DISABLE
#endif

#if defined IM81||defined IM81AZ||defined N850||defined N700||defined SP60||defined SP50||defined SP80||defined SP600||defined SP33G||defined SP600G||defined ME51||defined SP60G||defined SP50G||defined SP80G||defined NL8510GP||defined SP830PG
	#define SAM2_ENABLE ENABLE
#else
	#define SAM2_ENABLE DISABLE
#endif

#if defined GP730||defined GP720
	#define SAM3_ENABLE ENABLE
#else
	#define SAM3_ENABLE DISABLE
#endif

#if 0
	#define SAM4_ENABLE ENABLE
#else
	#define SAM4_ENABLE DISABLE
#endif

#if !(defined ME31||defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME32||defined ME65||defined ME30GX||defined ME32GX||defined ME32THM||defined ME11||defined SP10||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B||CPU5810X_ENABLE||defined ME50NX||defined ME68||defined ME66)
	#define	SAM_POWERON_MEANWHILE	ENABLE//未来都应该支持IC/SAM之间同时上电,软硬件上可以分开独立控制
#else
	#define	SAM_POWERON_MEANWHILE	DISABLE
#endif

#define ICSAM_PPS_ENABLE	DISABLE

//IC/SAM个数
#define	ICNUM		(IC1_ENABLE+IC2_ENABLE)
#define	SAMNUM	(SAM1_ENABLE+SAM2_ENABLE+SAM3_ENABLE+SAM4_ENABLE)
#define	ICSAMNUM		(ICNUM+SAMNUM)

//RFID
#if !(defined ME15C||defined ME15CHX)
	#define	RFID_ENABLE	ENABLE
#else
	#define	RFID_ENABLE	DISABLE
#endif
//15693协议是否支持
#if 0// ANDIROD_ENABLE
	#define  ISO15693_ENABLE   ENABLE
#else
	#define  ISO15693_ENABLE   DISABLE
#endif
//M0卡是否支持
#if  0
	#define  M0_ENABLE   ENABLE
#else
	#define  M0_ENABLE   DISABLE
#endif

#if !defined RFCARD_TYPE_A
#define RFCARD_TYPE_A	0xcc
#define RFID_TYPE_A RFCARD_TYPE_A //新宏名
#endif
#if !defined RFCARD_TYPE_B
#define RFCARD_TYPE_B	0xcb
#define RFID_TYPE_B RFCARD_TYPE_B
#endif
#if !defined RFCARD_TYPE_AB
#define RFCARD_TYPE_AB 0xcd
#define RFID_TYPE_AB RFCARD_TYPE_AB
#endif

//取smart卡状态字(SW),供IC/SAM/RFID等使用.如,0x9000表示命令执行 成功
#define	SMART_2BYTESTOUSHORT(addr0)	(((*(uchar *)(addr0)<<8)&0x0000ff00)|((*((uchar *)(addr0)+1))&0x000000ff))	//注意一些编译器的uchar与char是一样的,这在移位时会引起问题,故无需转成ushort型,直接&0x0000ffff即可
#define	SMART_GETSW					SMART_2BYTESTOUSHORT

//NFC
#if 0
	#define	NFC_ENABLE	ENABLE//理论上有射频卡的设备需要支持NFC功能，实际测试中请测试人员根据实际情况调整该开关
#else
	#define	NFC_ENABLE	DISABLE
#endif

//port
#define INVALID_COM 0xff  //zhangnw20061207 add 暂定0xff不会与已有资源冲突

#if defined GP720||defined NL8510GP||defined SP80||defined SP50||defined SP80G||defined SP50G||defined SP830PG||defined N850||defined X5
	#define COMNUM 2

#elif defined ME31||defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME32||defined ME65||defined ME30GX||defined ME32GX||defined ME32THM||defined ME11||defined ME15||(ANDIROD_ENABLE&&!defined N850&&!defined X5)||CPU5810X_ENABLE||defined ME50NX||defined ME68||defined ME66||defined ME50S||defined ME50NS||CPU5830X_ENABLE||defined SP930G||defined SP930PG||defined SP630PG||defined ME15CHX||defined ME15B//(||defined ME11||defined ME15 测试USB串口,需要使comnum=1)//IM81AZ虽然有串口但安卓端无法使用,所以也认为没有串口
	#define COMNUM 0
#elif 0
	#define COMNUM 3
#else //默认一个串口,如GP730
	#define COMNUM 1
#endif

//串口，USB从模式的缓冲区开大到16K，低端平台保持2047
#if !K21_ENABLE
#define BUFSIZE_SERIAL (1024*16)//(1024*4)
#elif (defined ME30MH ||defined ME30GX||defined ME32||defined ME32GX||defined ME65)
#define BUFSIZE_SERIAL  (1024)
#elif CPU5810X_ENABLE
#define BUFSIZE_SERIAL  (1024*4)
#else
#define BUFSIZE_SERIAL (1024*2-1)
#endif

//蓝牙，USB主模式 缓冲区中端大小4K，低端大小2047
#if !K21_ENABLE
#define BUFSIZE_BT (1024*4)
#elif defined ME15C||defined ME15CHX||defined ME15B||defined ME20||defined ME30MH||defined ME32||defined ME65||defined ME30GX||defined ME32GX||defined ME32THM||defined ME30THM//ME15C暂时只支持到1023的蓝牙数据包长度ME20支持1024
#define BUFSIZE_BT  (1024-1)
#else
#define BUFSIZE_BT  (1024*2-1)
#endif

#define BUFSIZE_USBHOST BUFSIZE_BT
//#define	MAXSNDLEN	BUFSIZE_SERIAL

//sec

//ALG

//disp
#if 0	//以下宏已过时,原先主要提供给disp模块及main中的setuserscreen使用,现NDK有相应的接口获取这些信息,通过接口获取即可
#if defined DEMO||defined SP730
	#define	LINE_NUMS	(144-1)			/* 点阵方式下POS的Y轴方向最大值 */
	#define	COLU_NUMS	(128-1)			/* 点阵方式下POS的X轴方向最大值 */
#else
	#define	LINE_NUMS	(64-1)			/* 点阵方式下POS的Y轴方向最大值 */
	#define	COLU_NUMS	(128-1)			/* 点阵方式下POS的X轴方向最大值 */
#endif
#endif
#define	BLACK 0
#define	WHITE 0xFFFF
#define	RED	  0xFACB
#define	GREEN 0x3384

#if 0	
#define	GUI_SUPPORT
#endif

//appmgr

/*USB串口主模式*/
#if !K21_ENABLE
	#define	USBHOST_ENABLE	ENABLE //K21产品平台不支持
#else
	#define	USBHOST_ENABLE	DISABLE
#endif

/*U盘*/
#if !K21_ENABLE
	#define	USB_ENABLE	ENABLE //K21产品平台不支持
#else
	#define	USB_ENABLE	DISABLE
#endif

/*SD*/
#if !(K21_ENABLE||OVERSEAS_ENABLE)
	#define	SD_ENABLE	ENABLE //K21产品平台不支持,国际版本也不支持SD卡
#else
	#define	SD_ENABLE	DISABLE
#endif

/*TOUCH*/
#if !((K21_ENABLE&&!defined ME50&&!defined SP610)||defined NL8510GP) //K21产品平台不支持 8510GP也不支持 ME50支持触屏
	#define	TOUCH_ENABLE	ENABLE
#else
	#define	TOUCH_ENABLE	DISABLE
#endif

//ETH
#if !K21_ENABLE ||defined ME50C//K21产品平台不支持
	#define	ETH_ENABLE	ENABLE
#else
	#define	ETH_ENABLE	DISABLE
#endif

//BT
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE) ||defined ME51||defined SP630PG//等后续确定哪些产品需要支持蓝牙后再调整该宏开关 
	#define	BT_ENABLE	ENABLE
#else
	#define	BT_ENABLE	DISABLE
#endif

//BT 主模式
#if !(K21_ENABLE||OVERSEAS_ENABLE) ||defined ME51||defined SP630PG//defined SP60//等后续确定哪些产品需要支持蓝牙后再调整该宏开关
	#define	BT_MASTER_ENABLE	ENABLE
#else
	#define	BT_MASTER_ENABLE	DISABLE
#endif

//DHCP
#if !K21_ENABLE
	#define	DHCP_ENABLE	ENABLE //K21平台不支持
#else
	#define	DHCP_ENABLE	DISABLE
#endif

//WIFI/WALN
#if !K21_ENABLE||(CPU5810X_ENABLE&&!defined SP610)||defined ME50NX||defined ME68
	#define	WLAN_ENABLE	ENABLE //K21平台不支持
#else
	#define	WLAN_ENABLE	DISABLE
#endif

//WIFI AP
#if !(K21_ENABLE||OVERSEAS_ENABLE)//理论上有WIFI的设备也要支持WIFI AP功能,实际测试中请测试人员根据测试情况调整该开关
	#define	WIFIAP_ENABLE	ENABLE
#else
	#define	WIFIAP_ENABLE	DISABLE	
#endif

#define WLAN_SIGNAL_MIN	(50)
#define WLAN_APCNT_MIN	(1)
#define WLAN_APCNT_MAX	(100)

//dns
//从下列表的维护请保持DNS1与DNS3是无效的,DNS2是可用的
#define	DNS1	"220.181.111.148"	//该地址是baidu的IP,也是无效的DNS
#if 0	
#define	DNS2	"211.138.151.161"	//电信,移动使用
#else
//#define	DNS2	"218.85.157.99"	//联通使用
#define	DNS2	"8.8.8.8"	//使用google的DNS
#endif
//#define	DNS2	"192.168.30.1"	//"61.54.28.13"
#define	DNS3	"192.168.30.2"	//该地址是无效的

//prnt
#if !(defined NL829STD||defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME32||defined ME65||defined ME30GX||defined ME32GX||defined ME32THM||defined ME11||defined SP10||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B||defined ME50||defined ME50N||defined ME50C||defined ME50H||defined ME62||defined ME50NX||defined ME68||defined ME66||defined ME50S||defined ME50NS||defined ME51)
	#define	PRN_ENABLE		ENABLE
	
	#define	MAXPRNBUFSIZE		(1024*2)
	//这里给的最大行宽为正常模式下的,放大模式下的值为它的一半
	#define	MAXPRLINEWIDE_TP		384
#if defined GP710||defined GP720
	#define	MAXPRLINEWIDE_HIP		360	//横向放大180,横向正常可达360
#else
	#define	MAXPRLINEWIDE_HIP		280	//144
#endif
	#define	MAXPRLINEWIDE_FIP		424

#if defined GP_ENABLE||CPU5892_ENABLE||CPU5830X_ENABLE||OVERSEAS_ENABLE||ANDIROD_ENABLE||defined ME31//||defined IM81||defined N900
	#define PRNGREY_ENABLE ENABLE

	#define	GREY_MINVALID		(0)
#if 0	//defined E90||defined E90G
	#define	GREY_MAXVALID		(7)
#else
	#define	GREY_MAXVALID		(5)
#endif
	#define	DEFAULT_GREY_VLE	(3)	//应用层默认灰度,与底层不一定一致
#else
	#define PRNGREY_ENABLE DISABLE
#endif

#define PRNUNDERLINE_EN	ENABLE

#else
	#define	PRN_ENABLE		DISABLE
#endif

/***********************
 * 文字图像混合排版模式*
 ***********************/
typedef enum PRN_TYPESETTING
{
	TPSET_AUTO = 0,		//<自动适应，文字环绕图像，保证不重合打印
	TPSET_TEXTUP,		//<文字在上，若出现重合，文件将直接覆写在图像上
	TPSET_PICUP,		//<图像在上，若出现重合，图像将直接覆写在文件上
	TPSET_MIX			//<文字图像嵌套，若重合，文字和图像将嵌套打印
}PRN_TYPESETTING;

//sys
#define	SLEEP_ALLOW_INTERRUPT	//默认系统的sleep(不是下面的sleep宏)都可被中断
#if defined ME15CHX||defined ME30MH||defined ME15B||defined ME32||defined ME66//不支持NDK_SysDelay
#define	sleep(a)		NDK_SysMsDelay(a*1000)
#else
#define	sleep(a)		NDK_SysDelay(a*10)
#endif
#if defined GP720||defined NL8510GP||defined SP80||defined SP50||defined SP80G||defined SP50G||defined SP830PG
	#define	SLEEP_ENABLE	DISABLE
#else
	#define	SLEEP_ENABLE	ENABLE
#endif


/*MODEM*/
//从MODEM硬件上来说,一般是同时支持同步和异步的.但软件上有时未对其中一种做支持(比如,功能未开发,接口未开放等)
//sdlc
#if !(K21_ENABLE||CPU5830X_ENABLE||defined SP930G||defined SP930PG||defined SP630PG) //K21产品平台不支持
	#define	SDLC_ENABLE	ENABLE
#else
	#define	SDLC_ENABLE	DISABLE
#endif
#define	SLEEP_SDLC_HANGUP	(5)
#define	SDLCPCKTHEADER		"\x60\x80\x00\x80\x00"
#define	SDLCPCKTHEADERLEN		(5)
#define	SDLCPCKTMAXLEN		(1024)//(350)   //大网控可以支持比较大的数据收发，为尽量接近应用数据包长度将350改成1024  20140504 linwl

//asyn
#if !(K21_ENABLE||CPU5830X_ENABLE||defined SP930G||defined SP930PG||defined SP630PG) //K21产品平台不支持
	#define	ASYN_ENABLE	ENABLE
#else
	#define	ASYN_ENABLE	DISABLE
#endif
#define	ASYNPCKTHEADERLEN	PCKT0203HEADERLEN
#define	ASYNRESPMAXLEN	(256)
#define	ASYNPCKTMAXLEN	(4*1024)	//PACKMAXLEN	//这个值并非ASYN通讯所能支持的最大包长,只是定个上限而已

// 无线
#if !K21_ENABLE||CPU5810X_ENABLE||defined ME50NX||defined ME68
	#define	WLM_ENABLE	ENABLE //K21产品平台不支持(除5810x产品)
#else
	#define	WLM_ENABLE	DISABLE
#endif
#define SQ_2G_MIN		(0)
#define SQ_2G_MAX		(31)
#define SQ_3G_MIN		(100)
#define SQ_3G_MAX		(199)

//ppp
#if !K21_ENABLE||CPU5810X_ENABLE||defined ME50NX||defined ME68
	#define	PPP_ENABLE	ENABLE //K21产品平台不支持(除5810x产品)
#else
	#define	PPP_ENABLE	DISABLE
#endif

//TD
#if 0
	#define	TD_ENABLE	ENABLE
#else
	#define	TD_ENABLE	DISABLE
#endif

//sckt
#if !K21_ENABLE||CPU5810X_ENABLE||defined ME50NX||defined ME68
	#define	NDKSOCK_ENABLE	ENABLE //K21产品平台不支持(除5810x产品)
#else
	#define	NDKSOCK_ENABLE	DISABLE
#endif

//ssl 
#if CPU5892_ENABLE||OVERSEAS_ENABLE
	#define	NDKSSL_ENABLE	0   //ENABLE   20140822 ssl 在5.0版本中暂时去掉
#else
	#define	NDKSSL_ENABLE	0	//DISABLE
#endif

#endif
