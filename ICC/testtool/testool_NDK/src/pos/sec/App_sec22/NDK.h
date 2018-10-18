/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：	产品开发部
* 日    期：	2012-08-17
* 版	本：	V1.00
* 最后修改人：
* 最后修改日期：
*/
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#ifndef __NDKAPI__H
#define __NDKAPI__H

/** @addtogroup 错误码定义
* @{
*/
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned short ushort;
/**
 *@brief 函数错误码返回
*/
typedef enum {
	NDK_OK,					/**<操作成功*/
	NDK_ERR=-1,				/**<操作失败*/
	NDK_ERR_INIT_CONFIG = -2,	 /**<初始化配置失败*/
	NDK_ERR_CREAT_WIDGET = -3,	/**<创建界面错误*/
	NDK_ERR_OPEN_DEV = -4,/**<打开设备文件错误*/
	NDK_ERR_IOCTL = -5,	/**<驱动调用错误*/
	NDK_ERR_PARA = -6,	/**<参数非法*/
	NDK_ERR_PATH = -7,	/**<文件路径非法*/
	NDK_ERR_DECODE_IMAGE = -8,	/**<图像解码失败*/
	NDK_ERR_MACLLOC=-9,			/*内存空间不足*/
	NDK_ERR_TIMEOUT = -10,		/**<超时错误*/
	NDK_ERR_QUIT = -11,		/**<按取消退出*/
	NDK_ERR_WRITE = -12, /**<写文件失败*/
	NDK_ERR_READ = -13, 	/**<读文件失败*/
	NDK_ERR_OVERFLOW = -15,		/**<缓冲溢出*/
	NDK_ERR_SHM = -16,	/**<共享内存出错*/
	NDK_ERR_NOSWIPED = -50,	/**<无磁卡刷卡记录*/
	NDK_ERR_NO_SIMCARD = -201,	/**<无SIM卡*/
	NDK_ERR_PIN = -202, /**<SIM卡密码错误*/
	NDK_ERR_PIN_LOCKED = -203,	/**<SIM卡被锁定*/
	NDK_ERR_PIN_UNDEFINE = -204,	/**<SIM卡未定义错误*/
	NDK_ERR_EMPTY = -205,			/**<返回空串*/
	NDK_ERR_PPP_PARAM = -301,	/**<PPP参数出错*/
	NDK_ERR_PPP_DEVICE = -302,/**<PPP无效设备*/
	NDK_ERR_PPP_OPEN = -303, /**<PPP已打开*/
	NDK_ERR_TCP_ALLOC = -304,	/**<无法分配*/
	NDK_ERR_TCP_PARAM = -305,	/**<无效参数*/
	NDK_ERR_TCP_TIMEOUT = -306,	/**<传输超时*/
	NDK_ERR_TCP_INVADDR = -307,	/**<无效地址*/
	NDK_ERR_TCP_CONNECT = -308,	/**<没有连接*/
	NDK_ERR_TCP_PROTOCOL = -309,/**<协议错误*/
	NDK_ERR_TCP_NETWORK = -310,	/**<网络错误*/
	NDK_ERR_TCP_SEND = -311,	/**<发送错误*/
	NDK_ERR_TCP_RECV = -312,	/**<接收错误*/
	NDK_ERR_GET_NETADDR = -401,/**<获取本地地址或子网掩码失败*/
	NDK_ERR_GET_GATEWAY = -402,	/**<获取网关地址失败*/
	NDK_ERR_ADDR_ILLEGAL =-403,/**<获取地址格式错误*/
	NDK_ERR_MODEM_COUNTRYFAIL=-501,/**<MODEM打补丁失败*/
	NDK_ERR_MODEM_SDLCINITFAIL=-502,/**<MODEM同步初始化失败*/
	NDK_ERR_MODEM_ASYNINITFAIL=-503,/**<MODEM异步初始化失败*/
	NDK_ERR_MODEM_SDLCDIALFAIL=-504,/**<MODEM同步拨号失败*/
	NDK_ERR_MODEM_INIT_NOT=-505,/**<MODEM未进行初始化*/
	NDK_ERR_MODEM_SDLCWRITEFAIL=-506,/**<MODEM同步写失败*/
	NDK_ERR_MODEM_ASYNWRITEFAIL=-507,/**<MODEM异步写失败*/
	NDK_ERR_MODEM_SDLCREADFAIL=-508,/**<MODEM同步读失败*/
	NDK_ERR_MODEM_ASYNREADFAIL=-509,/**<MODEM异步读失败*/
	NDK_ERR_MODEM_SDLCHANGUPFAIL=-510,/**<MODEM同步挂断失败*/
	NDK_ERR_MODEM_ASYNHANGUPFAIL=-511,/**<MODEM异步挂断失败*/
	NDK_ERR_MODEM_SDLCCLRBUFFAIL=-512,/**<MODEM同步清缓冲失败*/
	NDK_ERR_MODEM_ASYNCLRBUFFAIL=-513,/**<MODEM异步清缓冲失败*/

	NDK_ERR_ICC_WRITE_ERR =			-601,	/**< 写器件83c26出错*/
	NDK_ERR_ICC_COPYERR=			-602,	/**<内核数据拷贝出错*/
	NDK_ERR_ICC_POWERON_ERR=		-603,	/**<上电出错*/
	NDK_ERR_ICC_COM_ERR=			-604,	/**<命令出错*/
	NDK_ERR_ICC_CARDPULL_ERR=		-605,	/**<卡拔出了*/
	NDK_ERR_ICC_CARDNOREADY_ERR=	-606,	/**<卡未准备好*/

	NDK_ERR_APP_BASE=(-800),							/**<应用接口错误基数*/
	NDK_ERR_APP_CFG_OPEN=(NDK_ERR_APP_BASE-1),			/**<打开配置文件错误*/
	NDK_ERR_APP_CFG_GET=(NDK_ERR_APP_BASE-2),			/**<获取配置文件项错误*/
	NDK_ERR_APP_CFG_SET=(NDK_ERR_APP_BASE-3),			/**<写配置文件项错误*/
	NDK_ERR_APP_NOT_EXIST=(NDK_ERR_APP_BASE-4),			/**<应用项不存在*/
	NDK_ERR_APP_CFG_INIT=(NDK_ERR_APP_BASE-5),			/**<打开配置文件错误*/
	NDK_ERR_APP_CFG_ALLOC=(NDK_ERR_APP_BASE-6),			/**<分配新节点错误*/
	NDK_ERR_APP_CFG_DUMP=(NDK_ERR_APP_BASE-7),			/**<打开配置文件错误*/
	NDK_ERR_APP_CFG_SECTION=(NDK_ERR_APP_BASE-8),		/**<*/
	NDK_ERR_APP_MAX_CHILD=(NDK_ERR_APP_BASE-9),			/**<子应用运行数超过最大运行数目*/
	NDK_ERR_APP_CREAT_CHILD=(NDK_ERR_APP_BASE-10),		/**<创建子进程错误*/
	NDK_ERR_APP_WAIT_CHILD=(NDK_ERR_APP_BASE-11),		/**<等待子进程结束错误*/
	NDK_ERR_APP_FILE_READ=(NDK_ERR_APP_BASE-12),		/**<读文件错误*/
	NDK_ERR_APP_FILE_WRITE=(NDK_ERR_APP_BASE-13),		/**<写文件错误*/
	NDK_ERR_APP_FILE_STAT=(NDK_ERR_APP_BASE-14),		/**<获取文件信息错误*/
	NDK_ERR_APP_FILE_OPEN=(NDK_ERR_APP_BASE-15),		/**<文件打开错误*/
	NDK_ERR_APP_NLD_HEAD_LEN=(NDK_ERR_APP_BASE-16),		/**<NLD文件获取头信息长度错误*/
	NDK_ERR_APP_PUBKEY_EXPIRED=(NDK_ERR_APP_BASE-17),	/**<公钥有效期*/
	NDK_ERR_APP_MMAP=(NDK_ERR_APP_BASE-18),				/**<内存映射错误*/
	NDK_ERR_APP_MALLOC=(NDK_ERR_APP_BASE-19),			/**<动态内存分配错误*/
	NDK_ERR_APP_SIGN_DECRYPT=(NDK_ERR_APP_BASE-20),		/**<签名数据解签错误*/
	NDK_ERR_APP_SIGN_CHECK=(NDK_ERR_APP_BASE-21),		/**<签名数据校验错误*/
	NDK_ERR_APP_MUNMAP=(NDK_ERR_APP_BASE-22),			/**<内存映射释放错误*/
	NDK_ERR_APP_TAR=(NDK_ERR_APP_BASE-23),				/**<tar命令执行失败*/

    NDK_ERR_SECP_BASE = (-1000),
    NDK_ERR_SECP_TIMEOUT = (NDK_ERR_SECP_BASE - 1),             /**<获取键值超时*/
    NDK_ERR_SECP_PARAM = (NDK_ERR_SECP_BASE - 2),               /**<输入参数非法*/
    NDK_ERR_SECP_DBUS = (NDK_ERR_SECP_BASE - 3),                /**<DBUS通讯错误*/
    NDK_ERR_SECP_MALLOC = (NDK_ERR_SECP_BASE - 4),              /**<动态内存分配错误*/
    NDK_ERR_SECP_OPEN_SEC = (NDK_ERR_SECP_BASE - 5),            /**<打开安全设备错误*/
    NDK_ERR_SECP_SEC_DRV = (NDK_ERR_SECP_BASE - 6),             /**<安全设备操作错误*/
    NDK_ERR_SECP_GET_RNG = (NDK_ERR_SECP_BASE - 7),             /**<获取随机数*/
    NDK_ERR_SECP_GET_KEY = (NDK_ERR_SECP_BASE - 8),             /**<获取密钥值*/
    NDK_ERR_SECP_KCV_CHK = (NDK_ERR_SECP_BASE - 9),             /**<KCV校验错误*/
    NDK_ERR_SECP_GET_CALLER = (NDK_ERR_SECP_BASE - 10),         /**<获取调用者信息错误*/
    NDK_ERR_SECP_OVERRUM = (NDK_ERR_SECP_BASE - 11),            /**<运行次数出错*/

    NDK_ERR_SECVP_BASE = (-1100),                           /**<未知错误*/
    NDK_ERR_SECVP_TIMEOUT = (NDK_ERR_SECVP_BASE - 1),       /**<获取键值超时*/
    NDK_ERR_SECVP_PARAM = (NDK_ERR_SECVP_BASE - 2),         /**<输入参数非法*/
    NDK_ERR_SECVP_DBUS = (NDK_ERR_SECVP_BASE - 3),          /**<DBUS通讯错误*/
    NDK_ERR_SECVP_OPEN_EVENT0 =	(NDK_ERR_SECVP_BASE - 4),   /**<打开event0设备出错*/
    NDK_ERR_SECVP_SCAN_VAL = (NDK_ERR_SECVP_BASE - 5),      /**<扫描值超出定义*/
    NDK_ERR_SECVP_OPEN_RNG = (NDK_ERR_SECVP_BASE - 6),      /**<打开随机数设备错误*/
    NDK_ERR_SECVP_GET_RNG = (NDK_ERR_SECVP_BASE - 7),       /**<获取随机数出错*/

    NDK_ERR_SECCR_BASE = (-1200),                           /**<未知错误*/
    NDK_ERR_SECCR_TIMEOUT = (NDK_ERR_SECCR_BASE - 1),       /**<获取键值超时*/
    NDK_ERR_SECCR_PARAM = (NDK_ERR_SECCR_BASE - 2),         /**<输入参数非法*/
    NDK_ERR_SECCR_DBUS = (NDK_ERR_SECCR_BASE - 3),          /**<DBUS通讯错误*/
    NDK_ERR_SECCR_MALLOC = (NDK_ERR_SECCR_BASE - 4),        /**<动态内存分配错误*/
    NDK_ERR_SECCR_OPEN_RNG = (NDK_ERR_SECCR_BASE - 5),      /**<打开随机数设备错误*/
    NDK_ERR_SECCR_DRV = (NDK_ERR_SECCR_BASE - 6),           /**<驱动加密错误*/
    NDK_ERR_SECCR_KEY_TYPE = (NDK_ERR_SECCR_BASE - 7),      /**<密钥类型错误*/
    NDK_ERR_SECCR_KEY_LEN = (NDK_ERR_SECCR_BASE - 8),       /**<密钥长度错误*/
    NDK_ERR_SECCR_GET_KEY = (NDK_ERR_SECCR_BASE - 9),       /**<获取密钥错误*/

    NDK_ERR_SECKM_BASE = (-1300),
    NDK_ERR_SECKM_TIMEOUT = (NDK_ERR_SECKM_BASE - 1),           /**<获取键值超时*/
    NDK_ERR_SECKM_PARAM = (NDK_ERR_SECKM_BASE - 2),             /**<输入参数非法*/
    NDK_ERR_SECKM_DBUS = (NDK_ERR_SECKM_BASE - 3),              /**<DBUS通讯错误*/
    NDK_ERR_SECKM_MALLOC = (NDK_ERR_SECKM_BASE - 4),            /**<动态内存分配错误*/
    NDK_ERR_SECKM_OPEN_DB = (NDK_ERR_SECKM_BASE - 5),           /**<数据库打开错误*/
    NDK_ERR_SECKM_DEL_DB = (NDK_ERR_SECKM_BASE - 6),            /**<删除数据库错误*/
    NDK_ERR_SECKM_DEL_REC = (NDK_ERR_SECKM_BASE - 7),           /**<删除记录错误*/
    NDK_ERR_SECKM_INSTALL_REC = (NDK_ERR_SECKM_BASE - 8),       /**<安装密钥记录错误*/
    NDK_ERR_SECKM_READ_REC = (NDK_ERR_SECKM_BASE - 9),          /**<读密钥记录错误*/
    NDK_ERR_SECKM_OPT_NOALLOW = (NDK_ERR_SECKM_BASE - 10),      /**<操作不允许*/
    NDK_ERR_SECKM_KEY_MAC = (NDK_ERR_SECKM_BASE - 11),          /**<密钥MAC校验错误*/
    NDK_ERR_SECKM_KEY_TYPE = (NDK_ERR_SECKM_BASE - 12),         /**<密钥类型错误*/
    NDK_ERR_SECKM_KEY_ARCH = (NDK_ERR_SECKM_BASE - 13),         /**<密钥体系错误*/
    NDK_ERR_SECKM_KEY_LEN  = (NDK_ERR_SECKM_BASE - 14),         /**<密钥长度错误*/

	NDK_ERR_RFID_INITSTA=			-2005,  /**<非接触卡-射频接口器件故障或者未配置*/
	NDK_ERR_RFID_NOCARD=			-2008,  /**<非接触卡-无卡  0x0D*/
	NDK_ERR_RFID_MULTICARD=			-2009,  /**<非接触卡-多卡状态*/
	NDK_ERR_RFID_SEEKING=			-2010,  /**<非接触卡-寻卡/激活过程中失败*/
	NDK_ERR_RFID_PROTOCOL=			-2011,  /**<非接触卡-不支持ISO1444-4协议，如M1卡  F*/

	NDK_ERR_RFID_NOPICCTYPE=		-2012,  /**<非接触卡-未设置卡 0x01*/
	NDK_ERR_RFID_NOTDETE=			-2013,  /**<非接触卡-未寻卡   0x02*/
	NDK_ERR_RFID_AANTI=				-2014,  /**<非接触卡-A卡冲突(多张卡存在)  0x03*/
	NDK_ERR_RFID_RATS=				-2015,  /**<非接触卡-A卡RATS过程出错   0x04*/
	NDK_ERR_RFID_BACTIV=			-2016,  /**<非接触卡-B卡激活失败   0x07*/
	NDK_ERR_RFID_ASEEK=				-2017,  /**<非接触卡-A卡寻卡失败(可能多张卡存在)   0x0A*/
	NDK_ERR_RFID_BSEEK=				-2018,  /**<非接触卡-B卡寻卡失败(可能多张卡存在)   0x0B*/
	NDK_ERR_RFID_ABON=				-2019,  /**<非接触卡-A、B卡同时存在   0x0C*/
	NDK_ERR_RFID_UPED=				-2020,  /**<非接触卡-已经激活(上电)   0x0E*/
	NDK_ERR_RFID_NOTACTIV=			-2021,  /**<非接触卡-未激活*/

	NDK_ERR_MI_NOTAGERR=			-2030,  /**<非接触卡-无卡,				0xff*/
	NDK_ERR_MI_CRCERR=				-2031,  /**<非接触卡-CRC错,				0xfe*/
	NDK_ERR_MI_EMPTY=				-2032,  /**<非接触卡-非空,				0xfd*/
	NDK_ERR_MI_AUTHERR=				-2033,  /**<非接触卡-认证错,			0xfc*/
	NDK_ERR_MI_PARITYERR=			-2034,  /**<非接触卡-奇偶错,			0xfb*/
	NDK_ERR_MI_CODEERR=				-2035,  /**<非接触卡-接收代码错			0xfa*/
	NDK_ERR_MI_SERNRERR=            -2036,  /**<非接触卡-防冲突数据校验错	0xf8*/
	NDK_ERR_MI_KEYERR=              -2037,  /**<非接触卡-认证KEY错			0xf7*/
	NDK_ERR_MI_NOTAUTHERR=          -2038,  /**<非接触卡-未认证				0xf6*/
	NDK_ERR_MI_BITCOUNTERR=         -2039,  /**<非接触卡-接收BIT错			0xf5*/
	NDK_ERR_MI_BYTECOUNTERR=        -2040,  /**<非接触卡-接收字节错			0xf4*/
	NDK_ERR_MI_WriteFifo=           -2041,  /**<非接触卡-FIFO写错误			0xf3*/
	NDK_ERR_MI_TRANSERR=            -2042,  /**<非接触卡-传送操作错误		0xf2*/
	NDK_ERR_MI_WRITEERR=            -2043,  /**<非接触卡-写操作错误			0xf1*/
	NDK_ERR_MI_INCRERR=				-2044,  /**<非接触卡-增量操作错误		0xf0*/
	NDK_ERR_MI_DECRERR=             -2045,  /**<非接触卡-减量操作错误		0xef*/
	NDK_ERR_MI_OVFLERR=             -2046,  /**<非接触卡-溢出错误			0xed*/
	NDK_ERR_MI_FRAMINGERR=          -2047,  /**<非接触卡-帧错				0xeb*/
	NDK_ERR_MI_COLLERR=             -2048,  /**<非接触卡-冲突				0xe8*/
	NDK_ERR_MI_INTERFACEERR=        -2049,  /**<非接触卡-复位接口读写错		0xe6*/
	NDK_ERR_MI_ACCESSTIMEOUT=       -2050,  /**<非接触卡-接收超时			0xe5*/
	NDK_ERR_MI_PROTOCOLERR=			-2051,  /**<非接触卡-协议错				0xe4*/
	NDK_ERR_MI_QUIT=                -2052,  /**<非接触卡-异常终止			0xe2*/
	NDK_ERR_MI_PPSErr=				-2053,  /**<非接触卡-PPS操作错			0xe1*/
	NDK_ERR_MI_SpiRequest=			-2054,  /**<非接触卡-申请SPI失败		0xa0*/
	NDK_ERR_MI_NY_IMPLEMENTED=		-2055,  /**<非接触卡-无法确认的错误状态	0x9c*/
	NDK_ERR_MI_CardTypeErr=			-2056,  /**<非接触卡-卡类型错			0x83*/
	NDK_ERR_MI_ParaErrInIoctl=		-2057,  /**<非接触卡-IOCTL参数错		0x82*/
	NDK_ERR_MI_Para=				-2059, /**<非接触卡-内部参数错			0xa9*/
}EM_NDK_ERR;

/** @} */ // 函数错误码结束

/** @addtogroup 显示
* @{
*/


typedef unsigned int color_t;/**<RGB色彩数值,0(黑色) - 0xFFFF(白色)*/

/**
 *@brief 显示模式定义
*/
typedef enum {
	TEXT_ATTRIBUTE_NORMAL = 1<<0,			/**<字体正常显示*/
	TEXT_ATTRIBUTE_REVERSE = 1<<1,		/**<字体反向显示*/
	TEXT_ATTRIBUTE_UNDERLINE = 1<<2,		/**<字体加下划线*/
}EM_TEXT_ATTRIBUTE;

/**
 *@brief 背光控制
*/
typedef enum {
	BACKLIGHT_OFF ,			/**<点亮液晶背光*/
	BACKLIGHT_ON,			/**<关闭液晶*/
	BACKLIGHT_LOCKON,		/**<禁止液晶背光自动关闭*/
}EM_BACKLIGHT;

/**
 *@brief 系统字体选择定义
*/
typedef enum {
	DISPFONT_CUSTOM,			/**<系统默认的显示字体*/
	DISPFONT_EXTRA,			/**<额外尺寸的显示字体*/
	DISPFONT_USER,				/**<用户自定义字体*/
}EM_DISPFONT;

/**
 *@brief 系统字体颜色设置定义
*/
typedef enum {
	FONTCOLOR_NORMAL,				/**<字体正常显示时字体颜色*/
	FONTCOLOR_REVERSE,				/**<字体反向显示时字体颜色*/
	FONTCOLOR_BG_REVERSE,				/**<字体反向显示时背景颜色*/
}EM_FONTCOLOR;

/**
 *@brief 矩形填充模式定义
*/
typedef	enum  {
    RECT_PATTERNS_NO_FILL,        /**<非填充，只绘制边框*/
    RECT_PATTERNS_SOLID_FILL      /**<填充模式*/
}EM_RECT_PATTERNS;

/**
 *@brief	用户程序界面初始化。
 *@details  建议在程序最初调用，该函数成功调用后，显示模块的API才能正常使用。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrInitGui(void);/**	*@example NDK_disp_example.c**/


/**
 *@brief	获取显示模块版本。
 *@retval	pszVer	返回模块版本,输入的pnVer应该不小于16字节。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrGetVer(char *pszVer);

/**
 *@brief	设置显示模式，并获取之前的显示模式。
 *@param	emNewattr 	要设置的新显示模式。
 *@retval	pemOldattr 	输出之前的显示模式，peOldattr为NULL时不返回。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrSetAttr(EM_TEXT_ATTRIBUTE emNewattr, EM_TEXT_ATTRIBUTE *pemOldattr);

/**
 *@brief	保存当前屏幕
 *@details	包括显示内容、光标位置及显示模式，该保存结果可调用\ref NDK_ScrPop "NDK_ScrPop"快速恢复显示。
			NDK_ScrPush与\ref NDK_ScrPop "NDK_ScrPop"成对使用，不能嵌套。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrPush(void);

/**
 *@brief	快速恢复利用\ref NDK_ScrPush "NDK_ScrPush"保存的显示状态，包括显示内容、光标位置及文本显示属性。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrPop(void);

/**
 *@brief	设置背景图片。图片文件支持格式请查看各机型的图片格式限制。
 *@param	pszfilepath 	图片文件路径+文件名
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrSetbgPic(char *pszfilepath);

/**
 *@brief	取消背景图片。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrClrbgPic(void);

/**
 *@brief	清屏，把光标移到像素坐标(0,0)，同时将屏幕显示模式设置为TEXT_ATTRIBUTE_NORMAL。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrClrs(void);

/**
 *@brief	清除若干行(单位：像素)，把光标移到(0,unStartY)处。
		  	输入参数超出视口边界时，以视口边界为准。
 *@param	unStartY		开始行号（纵坐标，单位：像素），从0开始计数
 *@param	unEndY			结束行号（纵坐标，单位：像素），从0开始计数
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrClrLine(uint unStartY,uint unEndY);


/**
 *@brief	液晶显示光标位置移到像素坐标(unX,unY)处。
			如果输入参数非法，则光标保留位置不变,返回错误信息。
 *@param	unX 	横坐标（单位：像素）
 *@param	unY 	纵坐标（单位：像素）
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrGotoxy(uint unX, uint unY);


/**
 *@brief	获取当前像素光标位置的横坐标和纵坐标。
 *@retval	punX 返回光标位置的横坐标（单位：像素）。
 *@retval	punY 返回光标位置的纵坐标（单位：像素）。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrGetxy(uint *punX, uint *punY);


/**
 *@brief	设置显示区域尺寸。
 *@details  未设置情况下显示区域为实际屏幕尺寸，通过该接口设置显示区域后，其它API的显示操作只在该区域内有效。\n
			如设置(10,10,100,100)为应用显示区域，则应用程序中使用的像素坐标(0,0)实际上
			是屏幕像素坐标(10,10),清屏操作也只清除像素坐标(10,10,100,100)范围内的显示数据。
 *@param	unX		应用程序显示区域起始坐标的横坐标（单位：像素）。
 *@param	unY		应用程序显示区域起始坐标的横坐标（单位：像素）。
 *@param	unWidth	应用程序显示区域宽度（单位：像素）。
 *@param	unHeight 	应用程序显示区域高度（单位：像素）。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrSetViewPort(uint unX,uint unY,uint unWidth, uint unHeight);

/**
 *@brief	获取当前显示区域尺寸。
 *@retval	punX		显示区域起始坐标的横坐标（单位：像素）。
 *@retval	punY		显示区域起始坐标的纵坐标（单位：像素）。
 *@retval	punWidth	显示区域高度（单位：像素）。
 *@retval	punHeight	显示区域高度（单位：像素）。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrGetViewPort(uint *punX,uint *punY,uint *punWidth,uint *punHeight);


/**
 *@brief	在显示区域显示Bitmap图片。
 *@details  bitmap格式：1byte对应8个像素点,0表示白点，1表示黑点，显示数据横向排列，如下图所示:\n
-----------------D7~~D0--------------D7~~D0------------------\n
Byte 1: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte2	\n
Byte 3: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte4	\n
Byte 5: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte6	\n
Byte 7: ○ ○ ● ● ● ● ● ●  ●  ● ● ● ● ● ○ ○ Byte8	\n
Byte 9: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte10	\n
Byte11: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte12	\n
Byte13: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte14	\n
Byte15: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte16	\n
---------------------------------------------------------------\n
	如果显示图片范围超出屏幕范围或通过\ref NDK_ScrSetViewPort "NDK_ScrSetViewPort()"设置的用户使用区域时，则该函数操作无效，返回调用失败。
 *@param	unX 		图片在显示区域的左上角横坐标（单位：像素）
 *@param	unY 		图片在显示区域的左上角纵坐标（单位：像素）
 *@param	unWidth 	图片宽度（单位：像素）
 *@param	unHeight 	图片高度（单位：像素）
 *@param	psBuf 		Bitmap图片数据
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrDrawBitmap(uint unX,uint unY,uint unWidth, uint unHeight, const char *psBuf);


/**
 *@brief	从显示区域上像素坐标(unStartX,unStartY)到(unEndX,unEndY)画直线，unColor表示画线的RGB色彩值。
 *@param	unStartX 	直线的起点横坐标（单位：像素）
 *@param	unStartY 	直线的起点纵坐标（单位：像素）
 *@param	unEndX 		直线的终点横坐标（单位：像素）
 *@param	unEndY 		直线的终点纵坐标（单位：像素）
 *@param	unColor 		颜色数值 <0-0xFFFF>
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrLine(uint unStartX, uint unStartY, uint unEndX, uint unEndY, color_t unColor);


/**
 *@brief	在显示区域画一个矩形。
 *@details	如果矩形边界超出屏幕范围或通过\ref NDK_ScrSetViewPort "NDK_ScrSetViewPort()"设置的用户使用区域时，则该函数操作无效，返回调用失败。
 *@param	unX 		矩形的起点横坐标（单位：像素）
 *@param	unY 		矩形的起点纵坐标（单位：像素）
 *@param	unWidth 		矩形的宽（单位：像素）
 *@param	unHeight 		矩形的高（单位：像素）
 *@param	emFill_pattern 	0为填充模式，1为填充模式
 *@param	unColor	颜色数值 <0-0xFFFF>
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrRectangle(uint unX, uint unY, uint unWidth, uint unHeight, EM_RECT_PATTERNS emFill_pattern, color_t unColor);


/**
 *@brief	在显示区域显示指定的图片，图片文件支持格式请查看各机型的图片格式限制。
 *@details	如果显示图片范围超出屏幕范围或通过\ref NDK_ScrSetViewPort "NDK_ScrSetViewPort()"设置的用户使用区域时，则该函数操作无效，返回调用失败。
 *@param	unX 		图片显示的左上角横坐标（单位：像素）
 *@param	unY 		图片显示的左上角纵坐标（单位：像素）
 *@param	unWidth 	图片的宽（单位：像素）
 *@param	unHeight 	图片显示的高（单位：像素）
 *@param	pszPic 	图片文件所在的路径。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrDispPic(uint unX,uint unY,uint unWidth, uint unHeight, const char *pszPic);

/**
 *@brief	取显示区域上指定像素坐标点的颜色数值。
 *@param	unX 		横坐标（单位：像素）
 *@param	unY 		纵坐标（单位：像素）
 *@retval	punColor	返回的颜色值。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrGetPixel(uint unX, uint unY, color_t *punColor);


/**
 *@brief	在显示区域上指定像素坐标画点。
 *@param	unX 		横坐标（单位：像素）
 *@param	unY		纵坐标（单位：像素）
 *@param	unColor 	颜色数值 <0-0xFFFF>
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrPutPixel(uint unX, uint unY, color_t unColor);


/**
 *@brief	将显存中用户设置的显示区域内的数据刷新到液晶屏上显示。
 *@details	系统缺省为自动刷新。为避免闪屏，可以通过\ref NDK_ScrAutoUpdate "NDK_ScrAutoUpdate()"设置为非自动刷新，在NDK_ScrRefresh
			调用后系统才将显存中的数据刷新到液晶屏上。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrRefresh(void);

/**
 *@brief	将显存中全部数据刷新到LCD全屏上显示。
 *@details	该接口与\ref NDK_ScrRefresh "NDK_ScrRefresh()"区别在于不受限于显示区域的大小，当通过NDK_ScrSetViewPort设置的显示区域为全屏时
			，则\ref NDK_ScrRefresh "NDK_ScrRefresh()"与该接口效果一样
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrFullRefresh(void);

/**
 *@brief	设置是否自动刷新。
 *@param	nNewauto
					非0:自动刷新
					0:不自动刷新，只有调用\ref NDK_ScrRefresh "NDK_ScrRefresh()"才显示显存中的数据。
 *@retval	pnOldauto	返回设置之前的自动刷新状态。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrAutoUpdate(int nNewauto, int *pnOldauto);


/**
 *@brief	获取液晶屏尺寸。
 *@retval		punWidth	返回LCD宽度（单位：像素）。
 *@retval		punHeight	返回LCD高度（单位：像素）。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrGetLcdSize(uint *punWidth,uint *punHeight);

/**
 *@brief	获取液晶屏色深。
 *@details	可用于判断液晶屏是单色屏或彩屏
 *@retval		puncd	返回液晶屏色深：1----黑白两色,
										16----16位色，彩屏
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrGetColorDepth(uint *puncd);

/**
 *@brief	开关背光操作。
 *@param	emBL	BACKLIGHT_OFF – 关闭液晶背光
 					BACKLIGHT_ON –打开液晶背光
 					BACKLIGHT_LOCKON –液晶背光常亮
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrBackLight(EM_BACKLIGHT emBL);

/**
 *@brief		设置屏幕对比度。
 *@param		unContrast	对比度级别为0~63，0最淡，63最深。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrSetContrast(uint unContrast);


/**
 *@brief	设置使用的显示字体。
 *@details  系统默认支持2种尺寸点阵大小的字体可供切换,不同机型设定的系统默认字体尺寸不一定相同\n\n
			彩屏支持:汉字16x16 ASCII:8x16 (DISPFONT_CUSTOM)\n
					 汉字24x24 ASCII:12x24 (DISPFONT_EXTRA)\n
			黑白屏支持:汉字12x12 ASCII:6x12 (DISPFONT_CUSTOM)\n
						汉字16x16 ASCII:8x16 (DISPFONT_EXTRA)\n\n
			设置显示字体对后续显示字体起作用，对之前已刷新显示的内容无效
 *@param	emType	选择字体
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrSetFontType(EM_DISPFONT emType);


/**
 *@brief	获取当前系统使用的汉字显示字体宽和高。
 *@details  系统使用的ASCII字符字体点阵宽度固定为汉字的一半
 *@retval	punWidth	返回当前系统当前显示字体的汉字点阵宽。
 *@retval	punHeight	返回当前系统当前显示字体的汉字点阵高。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrGetFontSize(uint *punWidth,uint *punHeight);


/**
 *@brief	设置行间距和字间距。
 *@details  wspace大于当前使用的英文字体像素宽度的2倍时，字间距设为英文字体像素宽度的2倍
			hspace大于当前使用的英文字体像素高度时，行间距设为英文字体像素高度
 *@param	unWspace	字间距（单位：像素）
 *@param	unHpace	行间距（单位：像素）
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrSetSpace(uint unWspace,uint unHpace);


/**
 *@brief	设置字体颜色，包含正显、反显、反显背景色。
 *@param	unColor	颜色数值
 *@param	emType	选择设置对像

 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrSetFontColor(color_t unColor, EM_FONTCOLOR emType);



/**
 *@brief	显示字符串。
 *@param	unX	显示字符串位置的横坐标
 *@param	unY	显示字符串位置的纵坐标
 *@param	pszS	要显示的字符串指针
 *@param	unMode	设置显示ASCII字符时尺寸
					1：使用小号英文字体，不适用于包含汉字的字符串
						黑白屏：8x8尺寸英文字体
						彩屏：8x16尺寸英文字体
					0：系统当前使用的显示字体中ASCII点阵尺寸
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrDispString(uint unX,uint unY,const char *pszS,uint unMode);

/**
 *@brief	屏幕显示格式化输出，使用方法同printf
 *@param	format	参数输出的格式
  *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ScrPrintf(const char *format, ...);
/** @} */ // 显示模块结束

/** @addtogroup 键盘
* @{
*/

/**
 *@brief 输入控制
*/
typedef enum {
	INPUT_CONTRL_NOLIMIT, 		/**<可输入任意ASCII码字符，输满后直接返回*/
	INPUT_CONTRL_LIMIT,		/**<只允许输入数字与小数点，输满后直接返回*/
	INPUT_CONTRL_NOLIMIT_ERETURN,		/**<可输入任意ASCII码字符，输满后等待确认键*/
	INPUT_CONTRL_LIMIT_ERETURN,			/**<只允许输入数字与小数点，输满后等待确认键返回*/
}EM_INPUT_CONTRL;

/**
 *@brief 输入字符显示控制
*/
typedef enum {
	INPUTDISP_NORMAL, 		/**<输入内容正常显示字符*/
	INPUTDISP_PASSWD,			/**<输入内容显示为'*' */
	INPUTDISP_OTHER,			/**<支持带预设值的接收输入缓冲*/
}EM_INPUTDISP;

/**
 *@brief 输入法类型
*/
typedef enum {
    IME_NUMPY,	/**<数拼*/
    IME_ENGLISH, 	/**<英文*/
	IME_NUM,	/**<数字*/
	IME_BIHUA,	/**<笔画*/
    IME_QUWEI,	/**< 区位*/
    IME_MAXNUM=IME_QUWEI,	/**<仅用于错误判断*/
}EM_IME;

/**
 *@brief	清除键盘缓冲区。
 *@return
 *@li		NDK_OK			   操作成功
 *@li		其它EM_NDK_ERR    操作失败
*/
int NDK_KbFlush(void);

/**
 *@brief	开关长按键或组合键功能。
 *@param	nSelect 	0   关闭 1   开启
 *@param	nMode 		0   长按键  1   组合键
 *@param	pnState 	获取长按键或组合键原来的状态，0--关闭 1---开启。
 *@return
 *@li		NDK_OK				  操作成功
 *@li		其它EM_NDK_ERR    操作失败
*/
int NDK_KbSwitch(int nSelect, int nMode,int *pnState);

/**
 *@brief 	获取长按键或组合键的开关状态。
 *@param	nMode 	0   长按键  1   组合键
 *@param	pnState 	获取长按键或组合键状态，0--关闭 1---开启。
 *@return
 *@li		NDK_OK 				   操作成功
 *@li		其它EM_NDK_ERR	   操作失败
*/
int NDK_KbGetSwitch(int nMode,int *pnState);

/**
 *@brief 	超时时间内读取键盘按键值
 *@details	在规定的时间里读按键，读键过程如下:按下一个键，等待放开，返回键码。
 *@param	unTime	小于等于0 :无超时，一直等待读按键
							其他:为等待时间(以秒为单位)
 *@param	pnCode	获取输入键码，若在规定的时间内没有按键按下，pnCode的值为0
 *@li       NDK_OK 				   操作成功
 *@li   	其它EM_NDK_ERR	   操作失败
*/
int NDK_KbGetCode(uint unTime, int *pnCode);

/**
 *@brief	获取缓冲区中的首个键盘键值，立即返回
 *@details	检查按键缓冲区是否有按键，若有读键，返回键码,若没有键按下立即返回0。
   			一般该API是在一个程序循环体使用，并且使用之前应该\ref NDK_KbFlush "NDK_KbFlush"把缓冲区清除。
 			与\ref NDK_KbGetCode "NDK_KbGetCode"区别于本函数不进行等待，而是立即返回。
 *@param	pnCode	获取输入键码，无按键按下时pnCode的值为0
 *@return
 *@li        	NDK_OK 				   操作成功
 *@li   		其它EM_NDK_ERR	   操作失败
*/
int NDK_KbHit(int *pnCode);

/**
*@brief		输入字符串
*@details	从键盘读入一个以换行符为终结符的字符串，将其存入缓冲区pszBuf中。
			ESC键返回操作失败,回车读键完成返回,其他功能键无效。
*@param		pszBuf	接收字符串数据
*@param		unMin	最小输入串长
*@param		unMaxlen	最大输入串长
*@param		punLen	获取实际输入串的长度(>0)
*@param		emMode	显示类型，
					取值INPUTDISP_NORMAL时显示字符，
					取值INPUTDISP_PASSWD时显示'*'。
					取值为INPUTDISP_OTHER时，pcBuf若有内容（以\0为结尾的字符串）相当于已经从键盘上输入的数据,并且用明码显示出来。
*@param		unWaittime	等待输入的时间，若是0一直等待，其他为等待的秒数。若超时没有按下回车键，自动返回，返回TimeOut。
*@param		emControl	INPUT_CONTRL_NOLIMIT：任意ASCII码字符，输满后直接返回
						INPUT_CONTRL_LIMIT：只读数字与小数点，输满后直接返回
						INPUT_CONTRL_NOLIMIT_ERETURN：任意ASCII码字符，输满后等待确认键返回
						INPUT_CONTRL_LIMIT_ERETURN，只读数字与小数点，输满后等待确认键返回
 *@return
 *@li        	NDK_OK 			   操作成功
 *@li   		其它EM_NDK_ERR	   操作失败

*/
int NDK_KbGetInput(char *pszBuf,uint unMin,uint unMaxlen,uint *punLen,EM_INPUTDISP emMode,uint unWaittime, EM_INPUT_CONTRL emControl);

/**
 *@brief	汉字输入法
 *@details 	通过按“字母键”选择输入法。
			输入步骤：
			a. 在拼音输入栏直接输入拼音如“xin”输入“946”，并选择。按“退格”清除输入，按方向键进行左右移动拼音选择。
			b. 按确认”进入备选汉字栏，选择需要的汉字，方向键进行左移与右移。如果按“退格”键可退回‘a’步骤。
    					按“确认”键选中需要的汉字。
			c. 在输入法状态下，在选字的时候，可以通过方向键左右切换。
	 		标点符号输入：
				在增加开启输入法的情况下，按数字键“0”出现标点符号，按方向键进行左右移动选择符号，按确认键返选定符号；

 *@param	pszS 		接收数据字符串，pcS若有内容（以\0为结尾的字符串）相当于已经从键盘上输入的数据。
 *@param	unMaxlen 	接收数据字符串的最大长度。
 *@param	emMethod		输入法选择,若是emMethod取EM_IME之外的其他值，则该函数默认激活数拼输入法。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_KbHZInput(char *pszS,uint unMaxlen,EM_IME emMethod);

/** @} */ // 键盘模块结束


/** @addtogroup 打印
* @{
*/

/**
 *@brief 字体定义值
*/
typedef enum {
	PRN_HZ_FONT_24x24 = 1,
	PRN_HZ_FONT_16x32 ,
	PRN_HZ_FONT_32x32 ,
	PRN_HZ_FONT_32x16 ,
	PRN_HZ_FONT_24x32 ,
	PRN_HZ_FONT_16x16 ,
	PRN_HZ_FONT_12x16 ,
	PRN_HZ_FONT_16x8 ,
	PRN_HZ_FONT_24x24A ,			/**<自有宋体24x24点阵*/
	PRN_HZ_FONT_24x24B ,			/**<仿宋24x24点阵*/
	PRN_HZ_FONT_24x24C ,			/**<楷体24x24点阵*/
	PRN_HZ_FONT_24x24USER ,
	PRN_HZ_FONT_12x12A ,			/**<宋体12x12点阵*/
	PRN_HZ_FONT_48x24A ,
	PRN_HZ_FONT_48x24B ,
	PRN_HZ_FONT_48x24C ,
	PRN_HZ_FONT_24x48A ,
	PRN_HZ_FONT_24x48B ,
	PRN_HZ_FONT_24x48C ,
	PRN_HZ_FONT_48x48A ,
	PRN_HZ_FONT_48x48B ,
	PRN_HZ_FONT_48x48C
}EM_PRN_HZ_FONT;

typedef enum {
	PRN_ZM_FONT_8x16 = 1,
	PRN_ZM_FONT_16x16 ,
	PRN_ZM_FONT_16x32 ,
	PRN_ZM_FONT_24x32 ,
	PRN_ZM_FONT_6x8 ,
	PRN_ZM_FONT_8x8 ,
	PRN_ZM_FONT_5x7 ,
	PRN_ZM_FONT_5x16 ,
	PRN_ZM_FONT_10x16 ,
	PRN_ZM_FONT_10x8 ,
	PRN_ZM_FONT_12x16A ,       /**<MSGothic粗体12x16点阵*/
	PRN_ZM_FONT_12x24A ,				/**<Gulimche字体12x24点阵*/
	PRN_ZM_FONT_16x32A ,				/**<MSGothic粗体16x32点阵*/
	PRN_ZM_FONT_12x16B ,				/**<MSGothic粗体12x16点阵*/
	PRN_ZM_FONT_12x24B ,				/**<MSGothic粗体12x24点阵*/
	PRN_ZM_FONT_16x32B ,				/**<MSGothic粗体16x32点阵*/
	PRN_ZM_FONT_12x16C ,				/**<楷体粗体12x16点阵*/
	PRN_ZM_FONT_12x24C ,				/**<楷体粗体12x24点阵*/
	PRN_ZM_FONT_16x32C ,				/**<楷体粗体16x32点阵*/
	PRN_ZM_FONT_24x24A ,
	PRN_ZM_FONT_32x32A ,
	PRN_ZM_FONT_24x24B ,
	PRN_ZM_FONT_32x32B ,
	PRN_ZM_FONT_24x24C ,
	PRN_ZM_FONT_32x32C ,
	PRN_ZM_FONT_12x12
}EM_PRN_ZM_FONT;

/**
 *@brief  打印机状态以及错误定义，取打印机状态返回值存在两个或多个或上的关系
*/

typedef enum{
	PRN_STATUS_OK = 0,			/**<打印机正常*/
	PRN_STATUS_BUSY = 8,			/**<打印机正在打印*/
	PRN_STATUS_NOPAPER = 2,       /**<打印机缺纸*/
	PRN_STATUS_OVERHEAT = 4,      /**<打印机过热*/
	PRN_STATUS_VOLERR = 112       /**<打印机电压异常*/
}EM_PRN_STATUS;

/**
 *@brief 打印机类型枚举类型值
*/

typedef enum {
    PRN_TYPE_TP = 0,             /**<热敏打印机*/
    PRN_TYPE_HIP,              /**<穿孔针打*/
    PRN_TYPE_FIP,              /**<摩擦针打*/
    PRN_TYPE_END               /**<无*/
}EM_PRN_TYPE;

/**
 *@brief  打印机打印模式
*/
typedef enum{
	PRN_MODE_ALL_DOUBLE = 0,			/**<横向放大、纵向放大*/
	PRN_MODE_WIDTH_DOUBLE ,			/**<横向放大、纵向正常*/
	PRN_MODE_HEIGHT_DOUBOE,       /**<横向正常、纵向放大*/
	PRN_MODE_NORMAL               /**<横向正常、纵向正常*/
}EM_PRN_MODE;


/**
 *@brief 下划线功能开关的枚举类型值
*/
typedef enum{
	PRN_UNDERLINE_STATUS_OPEN = 0,			/**<下划线功能开*/
	PRN_UNDERLINE_STATUS_CLOSE			   /**<下划线功能关*/
}EM_PRN_UNDERLINE_STATUS;

/**
 *@brief 字库注册信息
*/
typedef struct
{
	int nOffset;						/**<需要获取的字在字库中的依移量*/
	int nFontByte;					   /**<需要获取的字在字库中占字节数*/
}ST_PRN_RECMSG;


/**
 *@brief 下划线功能开关的枚举类型值
*/
typedef struct
{
	char *pszName;	    			/**<字库名或字库所在路径*/
	int  nNum;				    	/**<注册号,支持0~7注册号*/
	int  nDirection;   				/**<0:横向字体； 1:纵向字体*/
	int  w;							/**<字体显示的宽度*/
	int  h;							/**<字体显示的高度*/
	int  (*func)(char *, ST_PRN_RECMSG *);	/**<获取依移地址和占用字节数的函数*/
	int  nIsHZ;                     /**<注册的字库是汉字字库还是ASCII字库，1:HZ  0:ZM*/
}ST_PRN_FONTMSG;


/**
 *@brief 		  打印机初始化
 *@details  	包含清缓冲区,重置打印参数(包括字体、边距和模式等)。
 *@param      unPrnDirSwitch  是否开启边送边打还是调用NDK_PrnStart开始打印开关。
              0--关闭边送边打功能(默认)
	              在该模式下，所有的NDK_PrnStr,NDK_PrnImage都完成点阵转换工作，将点阵数据存到数据缓冲区，
	              在调用NDK_PrnStart之后才开始所有和打印相关的工作，包括走纸和打印。
              1--开启边送边打功能
	              在该模式下，只要满一行数据，就会送驱动打印，调用NDK_PrnStart，无任何效果，直接返回。
	              调用NDK_PrnFeedByPixel，将立即走纸返回。而在关闭模式下该操作会在NDK_PrnStart之后进行。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnInit(uint unPrnDirSwitch);
/**
 *@brief 		打印字符串
 *@details 		该函数负责转换所有打印的字符串到点阵缓冲区，打印工作在调用Start之后开始送数打印。该函数为纯软件操作。
 *@param		pszBuf 为以\0为结尾的串,串的内容可为ASC码，汉字 换行\n或\r(表示结束本行，对于空行则直接进纸)。
 				当pszBuf里面有汉字和ASCII的混合串时,字母和汉字只和最近一次设置有关。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnStr(const char *pszBuf);
/**
 *@brief 		开始启动打印.
 *@details 	NDK_PrnStr和NDK_PrnImage都是完成数据转换成点阵存储到缓冲区中工作，调用该函数开始送数打印。
			 			调用NDK_PrnStart打印结束后要判断返回值是否为0，如果返回-1则说明向打印送数失败，则立即返回打印机状态值，不进行继续送数操作。
			 			NDK_PrnStart打印结束之后会阻塞等待返回打印机状态的值。应用可根据NDK_PrnStart返回的值来判断打印机状态是否正常。
			 			(如果返回的非打印机状态值或者NDK_OK，即其他系统错误时需要应用去取打印机状态，该可能性比较小)
 *@return
 *@li	NDK_OK				打印结束且取打印机状态正常
 *@li	EM_NDK_ERR		其他系统错误(如NDK_ERR_OPEN_DEV或者NDK_ERR_MACLLOC)
*/
int NDK_PrnStart(void);
/**
 *@brief 		打印图形(该函数也是转换打印点阵到点阵缓冲区，调用NDK_PrnStart开始打印)
 *@details  	热敏打最大宽度384个点。如果xsize和xpos相加之和大于上述宽度限制会返回失败，如果是横向放大模式的话不能超过384/2。
 *@param 		unXsize 图形的宽度（像素）
 *@param 		unYsize 图形的高度（像素）
 *@param 		unXpos  图形的左上角的列位置，且必须满足xpos+xsize<=ndk_PR_MAXLINEWIDE（正常模式为384，横向放大时为384/2）
 *@param 		psImgBuf 图象点阵数据,为横向排列，第一个字节第一行的前8个点，D7为第一个点
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnImage(uint unXsize,uint unYsize,uint unXpos,const char *psImgBuf);
/**
 *@brief 		用于获取本打印机类型
 *@retval   pemType 用于返回打印机类型的值定义如下：
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnGetType(EM_PRN_TYPE *pemType);
/**
 *@brief 		取打印驱动的版本信息.
 *@retval   pszVer 用于存储返回版本字符串
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnGetVersion(char *pszVer);
/**
 *@brief 		设置打印字体
 *@details  设置ASCII打印字体和汉字字体。应用层可参看底层和应用层的接口文件中的相关定义。
 *@param 	emHZFont	设置汉字字体格式
 *@param    emZMFont	设置ASCII字体格式
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnSetFont(EM_PRN_HZ_FONT emHZFont,EM_PRN_ZM_FONT emZMFont);
/**
 *@brief		获取打印机状态值.
 *@details		在打印之前可使用该函数判断打印机是否缺纸。
 *@retval	    pemStatus 用于返回打印机状态值(错误码存在两个或者多个值相或的关系)
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnGetStatus(EM_PRN_STATUS *pemStatus);
/**
 *@brief： 	设置打印模式.
 *@param 	emMode 打印模式(默认是使用正常模式)
 *@param     unSigOrDou 打印单双向选择0--单向 1--双向(只对针打有效，热敏忽略)
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnSetMode(EM_PRN_MODE emMode,uint unSigOrDou);
/**
 *@brief  	文字与图形的排版打印.(暂未实现)
 *@param    unXsize 图像宽度(像素)，不能大于384，必须是8的整数倍
 *@param    unYsize 图像高度(像素)，不能大于384，必须是8的整数倍
 *@param    unXpos  图像的横向偏移位置，(xpos+xsize)不能大于384
 *@param    unYpos  图像的纵向偏移位置
 *@param    psImgBuf 图像点阵缓冲，必须为横向排列的点阵数据
 *@param    pszTextBuf] 文字字符串首地址，字符串长度不能超过1K
 *@param    unMode 打印的模式
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnIntegrate(uint unXsize,uint unYsize,uint unXpos,uint unYpos,const char *psImgBuf,const char *pszTextBuf, uint unMode);
/**
 *@brief		设置打印灰度
 *@details		设置打印灰度(加热时间)，以便对于不同的打印纸进行打印效果微调.
 *@param    unGrey 灰度值，范围0~5；0为最淡的效果，5为最浓的打印效果。打印驱动默认的灰度级别为3。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnSetGreyScale(uint unGrey);
/**
 *@brief  	设置打印左边界、字间距、行间距。在对打印机有效设置后将一直有效，直至下次
 *@param  unBorder 左边距 值域为：0-288(默认为0)
 *@param    unColumn 字间距 值域为：0-255(默认为0)
 *@param    unRow 行间距 值域为：0-255(默认为0)
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnSetForm(uint unBorder,uint unColumn, uint unRow);
/**
 *@brief 	  按像素走纸
 *@details	让打印机走纸，参数为像素点,调用该函数，并没有马上走纸，而是存在缓冲区中，等调用start之后和打印动作一起执行
 *@param    unPixel 走纸像素点 值域> 0
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnFeedByPixel(uint unPixel);

/**
 *@brief	打印是否开启下划线功能.
 *@param  emStatus 0：开下划线；1：关下划线
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnSetUnderLine(EM_PRN_UNDERLINE_STATUS emStatus);

/**
 *@brief	设置对齐方式.(暂未实现)
 *@param  unType 0:左对齐; 1居中对齐; 2右对齐;3关闭对齐方式
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnSetAlignment(uint unType);
/**
 *@brief	获取打印长度.
 *@retval punLen 返回已经打印的长度(当前开机，开始打印到现在打印结束为止所打印机打印的总共长度)
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnGetPrintedLen(uint *punLen);
/**
 *@brief		自定义字体注册。
 *@param		pstMsg ST_PRN_FONTMSG类型指针，使用自定义注册要完成相应信息的填写
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnFontRegister(ST_PRN_FONTMSG *pstMsg);
/**
 *@brief	根据注册号来选择打印字体.
 *@param  unFontId 注册字体的id(该设置后会覆盖NDK_PrnSetFont中设定的字体)
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnSetUsrFont(uint unFontId);
/**
 *@brief	获得该次打印的点阵行数.
 *@retval  punLine 返回行数
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnGetDotLine(uint *punLine);
/**
 *@brief	打印bmp，png等格式的图片
 *@details  该函数将存储在pos上的图片进行解码后存储到点阵缓冲区，  不过图片解码会暂用一定的时间，必要的时候需要有一定的等待时间
 *@param  pszPath 图片所在的路径
 *@param  unXpos  图形的左上角的列位置，且必须满足xpos+xsize(解码后图片的宽度值)<=ndk_PR_MAXLINEWIDE（正常模式为384，横向放大时为384/2）
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PrnPicture(uint unXpos,const char *pszPath);

/** @} */ // 打印模块结束


/** @addtogroup 文件系统
* @{
*/

/**
 *@brief 		打开文件.
 *@details
 *@param    pszName 文件名
 *@param    pszMode 打开模式 "r"(以只读方式打开，如果不存在则失败) or "w"(以写的方式打开，如果文件不存在则创建)。
 *@return
 *@li	 fd				操作成功返回文件描述符
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FsOpen(const char *pszName,const char *pszMode);


/**
 *@brief 		关闭文件.
 *@details
 *@param    nHandle 文件句柄
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FsClose(int nHandle);

/**
 *@brief 		从打开的nHandle文件当前指针读unLength个字符到缓冲区psBuffer.
 *@details
 *@param    nHandle 文件句柄
 *@param    unLength，需要读取的字符的长度
 *@retval    psBuffer，需要读入的缓冲区注意要足够读入length字节
 *@return
 *@li	length				实际读取到文件的长度
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FsRead(int nHandle, char *psBuffer, uint unLength );

/**
 *@brief 		向打开的nHandle文件写入unLength个字节.
 *@details
 *@param    nHandle 文件句柄
 *@param    psBuffer，需要写入文件内容的缓冲区
 *@param    unLength，需要写入的长度
 *@return
 *@li	length				返回实际写入到文件的长度
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FsWrite(int nHandle, const char *psBuffer, uint unLength );

/**
 *@brief 		移动文件指针到从unPosition起距ulDistance的位置
 *@details
 *@param    nHandle 文件句柄
 *@param    ulDistance，根据参数unPosition来移动读写位置的位移数。
 *@param    unPosition，需要读取的字符的长度
 						SEEK_SET 参数offset即为新的读写位置。
						SEEK_CUR 以目前的读写位置往后增加offset个位移量。
						SEEK_END 将读写位置指向文件尾后再增加offset个位移量。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FsSeek(int nHandle, ulong ulDistance, uint unPosition );

/**
 *@brief 		删除指定文件
 *@details
 *@param    pszName 要删除的文件名
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FsDel(const char *pszName);

/**
 *@brief 		文件长度
 *@details
 *@param    pszName 文件名
 *@retval   punSize 文件大小返回值
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FsFileSize(const char *pszName,uint *punSize);

/**
 *@brief 		文件重命名
 *@details
 *@param    pszsSrcname 原文件名
 *@param    pszDstname 目标文件名
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FsRename(const char *pszsSrcname, const char *pszDstname );

/**
 *@brief 		测试文件是否存在
 *@details
 *@param    pszName 文件名
 *@return
 *@li	NDK_OK				操作成功(文件存在)
 *@li	其它EM_NDK_ERR		操作失败
*/

int NDK_FsExist(const char *pszName);

/**
 *@brief 		文件截短
 *@details   NDK_FsTruncate()会将参数pszPath 指定的文件大小改为参数unLen 指定的大小。如果原来的文件大小比参数unLen大，则超过的部分会被删去。
 		   			如果原来文件的大小比unLen小的话，不足的部分将补上0xff
 *@param    pszPath 文件路径
 *@param    unLen 所要截短长度
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FsTruncate(const char *pszPath ,uint unLen );

/**
 *@brief 	  	读取文件流位置
 *@details   用来取得文件流目前的读写位置
 *@param    nHandle 文件句柄
 *@retval    pulRet 文件流位置
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FsTell(int nHandle,ulong *pulRet);

/**
 *@brief 	  	取文件系统磁盘空间的使用情况
 *@details
 *@param    	unWhich :0--已经使用的磁盘空间1--剩余的磁盘空间
 *@retval     pulSpace 返回磁盘空间使用量或者剩余量
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FsGetDiskSpace(uint unWhich,ulong *pulSpace);
/**
 *@brief 		创建目录.
 *@details
 *@param    pszName 目录名称
 *@return
 *@li	 NDK_OK				操作成功返回
 *@li	其它EM_NDK_ERR		操作失败
*/

int NDK_FsCreateDirectory(const char *pszName);


/**
 *@brief 		删除目录.
 *@details
 *@param    pszName 目录名称
 *@return
 *@li	 NDK_OK				操作成功返回
 *@li	其它EM_NDK_ERR		操作失败
*/

int NDK_FsRemoveDirectory(const char *pszName);
/**
 *@brief 		文件系统格式化
 *@details	该功能仅限于传统pos上gp平台pos直接返回-1
 *@return
 *@li	 NDK_OK				操作成功返回
 *@li	其它EM_NDK_ERR		操作失败
*/

int NDK_FsFormat(void);


/**
 *@brief 		列出制定目录下的所有文件
 *@details	传入的psBuf的的size一定要够大，否则会出现溢出情况pbuf 每20个字节存储一个文件名
 						前19 为文件名，超过自动截短。第20字节如果为1则表示该文件为文件夹，0为普通文件
 *@param       pPath 指定要读取的目录
 *@retval        psBuf 将文件名存储到pbuf总返回
 *@retval        punNum 返回该文件夹目录的文件总数
 *@return
 *@li	 NDK_OK				操作成功返回
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FsDir(const char *pPath,char *psBuf,uint *punNum);

/** @} */ // 文件系统模块结束


/** @addtogroup 应用管理
* @{
*/

#define NDK_APP_NAME_MAX_LEN		32	/**< 应用名称最大长度*/
#define NDK_APP_VER_MAX_LEN			16	/**< 版本最大长度*/
#define NDK_APP_BUILD_TIME_LEN		32	/**< 创建时间最大长度*/

/**
 *@brief 应用信息,该结构在NLD结构中使用，涉及补齐问题。结构定义不允许修改，否则会和下载工具打包的信息部一致
*/
typedef struct APPINFO{
    uchar 	szAppName[NDK_APP_NAME_MAX_LEN + 1];		/**<用户程序名称*/
    uchar	szVerBuf[NDK_APP_VER_MAX_LEN + 1];			/**<用户程序版本信息*/
    int		nSeriNo;									/**<编号,V2版保留,但意义不同	*/
    uchar	szBuildTime[NDK_APP_BUILD_TIME_LEN + 1];	/**<文件编译时间*/
    int		nIsMaster;									/**<主控标志 */
    uint	sunReverse[3];
}ST_APPINFO;

typedef int (*CallbackMock)(int, void *,int,void **,int *);

/**
 *@brief	设置回调函数指针
 *@param	NDK_EventMain	函数地址
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AppSetEventCallBack(CallbackMock NDK_EventMain);

/**
 *@brief	运行应用程序
 *@param	pszAppName	应用名称。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AppRun(const uchar *pszAppName);

/**
 *@brief	执行事件操作
 *@param	pszFilename		应用名称
 *@param	psInEventMsg	输入事件消息
 *@param	nInlen			输入事件长度
 *@param	nMaxOutLen		允许的输出事件的长度
 *@retval	psOutEventMsg	获取到的应用信息
 *@retval	pnOutLen		实际输出的数据长度
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AppDoEvent(const uchar *pszFilename,int nModuleID,const void *psInEventMsg, int nInlen, void *psOutEventMsg, int nMaxOutLen, int *pnOutLen);

/**
 *@brief	获取安装完应用程序是否需要重启的标志位
 *@param	*nRebootFlag		输出参数1表示安装完应用程序需要重启才能生效，0不需要重启
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它NDK_ERRCODE		操作失败
*/
int NDK_AppGetReboot(int *nRebootFlag);
/**
 *@brief	装载应用
 *@param	pszFilename		应用名称
 *@param	nRebootFlag		重启标志，除传统POS终端外，其他终端该参数无意义
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AppLoad(const uchar *pszFilename, int nRebootFlag);

/**
 *@brief	获取应用信息
 *@param	pszAppName	应用名称, 可以输入为NULL。
 *@param	nPos		偏移，该偏移位置为在系统中信息表偏移，和下载顺序相关。
 *@param	nSizeofInfo	输出应用信息的最大长度。
 *@retval	pstAppInfo	获取到的应用信息
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AppGetInfo(const uchar *pszAppName, int nPos, ST_APPINFO *pstAppInfo, int nSizeofInfo);

/**
 *@brief	删除应用程序
 *@param	pszAppName	应用名称
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AppDel(const uchar *pszAppName);


/** @} */ // 多应用模块结束

/** @addtogroup 算法
* @{
*/
#define MAX_RSA_MODULUS_LEN		512				/**< 最大模长度 */
#define MAX_RSA_PRIME_LEN		256				/**< 最大模素数长度 */

/**
 *@brief 对称密钥算法
*/
typedef enum{
	ALG_TDS_MODE_ENC = 0,		/**< DES加密 */
	ALG_TDS_MODE_DEC = 1,		/**< DES解密 */
}EM_ALG_TDS_MODE;

/**
 *@brief RSA算法密钥长度
*/
typedef enum{
	RSA_KEY_LEN_512  = 512,
	RSA_KEY_LEN_1024 = 1024,
	RSA_KEY_LEN_2048 = 2048,
}EM_RSA_KEY_LEN;

/**
 *@brief RSA算法指数
*/
typedef enum{
	RSA_EXP_3 = 0x03,
	RSA_EXP_10001 = 0x10001,
}EM_RSA_EXP;

/**
 *@brief RSA公钥
*/
typedef struct {
    ushort bits;       							/**< 模位数 */
    uchar modulus[MAX_RSA_MODULUS_LEN+1];      	/**< 模 */
	uchar publicExponent[MAX_RSA_MODULUS_LEN+1]; 	/**< 公钥指数 */
    uchar exponent[MAX_RSA_MODULUS_LEN+1];     	/**< 私钥指数 */
    uchar prime[2][MAX_RSA_PRIME_LEN+1];      	/**< pq素数 */
    uchar primeExponent[2][MAX_RSA_PRIME_LEN+1]; 	/**< 素数与指数除法值 */
    uchar coefficient[MAX_RSA_PRIME_LEN+1];  	 	/**< 素数与素数除法值 */
}ST_RSA_PRIVATE_KEY;

/**
 *@brief RSA私钥
*/
typedef struct {
    ushort bits;                    			/**< 模位数 */
    uchar modulus[MAX_RSA_MODULUS_LEN+1];  		/**< 模 */
    uchar exponent[MAX_RSA_MODULUS_LEN+1];		/**< 指数 */
}ST_RSA_PUBLIC_KEY;

/**
 *@brief	计算des
 *@param	psDataIn	加密数据缓冲
 *@param	psKey		密钥缓冲,长度8,16,24
 *@param    nKeyLen     密钥长度，值只能为8,16,24
 *@param	nMode		加密模式 参见\ref EM_ALG_TDS_MODE "EM_ALG_TDS_MODE"
 *@retval	psDataOut	输出数据
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AlgTDes(uchar *psDataIn, uchar *psDataOut, uchar *psKey, int nKeyLen, int nMode);

/**
 *@brief	计算sha1
 *@param	psDataIn	输入数据
 *@param	nInlen		数据长度
 *@retval	psDataOut	输出数据（sha1计算结果长度为20字节）
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AlgSHA1(uchar *psDataIn, int nInlen, uchar *psDataOut);

/**
 *@brief	计算sha256
 *@param	psDataIn	输入数据
 *@param	nInlen		数据长度
 *@retval	psDataOut	输出数据（sha256计算结果长度为  字节）
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AlgSHA256(uchar *psDataIn, int nInlen, uchar *psDataOut);

/**
 *@brief	计算sha512
 *@param	psDataIn	输入数据
 *@param	unInlen		加密模式
 *@retval	psDataOut	输出数据（sha512计算结果长度为 字节）
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AlgSHA512(uchar *psDataIn, int nInlen, uchar *psDataOut);

/**
 *@brief	RSA密钥对生成
 *@param	nProtoKeyBit		密钥位数，当前支持512、1024和2048位 参考\ref EM_RSA_KEY_LEN "EM_RSA_KEY_LEN"
 *@param	nPubEType			指数类型，参考\ref EM_RSA_EXP "EM_RSA_EXP"
 *@retval	pstPublicKeyOut		公钥
 *@retval	pstPrivateKeyOut	私钥
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AlgRSAKeyPairGen( int nProtoKeyBit, int nPubEType, ST_RSA_PUBLIC_KEY *pstPublicKeyOut, ST_RSA_PRIVATE_KEY *pstPrivateKeyOut);

/**
 *@brief	RSA密钥对加解密
 *@details	该函数进行RSA加密或解密运算,加密或解密通过选用不同的密钥实现。如(Modul,Exp)选用私有密钥,则进行加密;如选用公开密钥,则进行解密。
 			psDataIn的第一个字节必须小于psModule的第一个字节。 该函数可实现长度不超过2048 bits 的RSA运算。
 			输入的数据开辟的缓冲须是模长度+1。
 *@param	psModule		模缓冲,字符串的形式存入,如"31323334"
 *@param	nModuleLen	模的长度 只有三种选择512/8,1024/8,2048/8
 *@param	psExp			存放RSA运算的指数缓冲区指针。就是e.按高位在前,低位在后的顺序存储,如"10001"
 *@param	psDataIn		数据缓冲,缓冲区的大小须比模的长度大1
 *@retval	psDataOut		输出数据,输出的数据长度等于模的长度。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AlgRSARecover(uchar *psModule, int nModuleLen, uchar *psExp, uchar *psDataIn, uchar *psDataOut);

/**
 *@brief	RSA密钥对校验
 *@param	pstPublicKey		公钥
 *@param	pstPrivateKey		私钥
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AlgRSAKeyPairVerify(ST_RSA_PUBLIC_KEY *pstPublicKey, ST_RSA_PRIVATE_KEY *pstPrivateKey);


/** @} */ // 算法模块结束


/** @addtogroup 安全
* @{
*/

#define MAX_RSA_MODULUS_LEN		512		/**<RSA最大模长度*/

/**
 *@brief 终端密钥类型
*/
typedef enum{
	SEC_KEY_TYPE_TLK=0,	/**<终端装载密钥*/
	SEC_KEY_TYPE_TMK,	/**<终端MAC密钥*/
	SEC_KEY_TYPE_TPK,	/**<终端PIN密钥*/
	SEC_KEY_TYPE_TAK,	/**<终端MAC密钥*/
	SEC_KEY_TYPE_TDK,	/**<终端数据加解密密钥*/
}EM_SEC_KEY_TYPE;

/**
 *@brief 密钥校验模式
*/
typedef enum{
	SEC_KCV_NONE=0,		/**<无验证*/
	SEC_KCV_ZERO,		/**<对8个字节的0x00计算DES/TDES加密,得到的密文的前4个字节即为KCV*/
	SEC_KCV_VAL,		/**<首先对密钥明文进行奇校验,再对“\x12\x34x56\x78\x90\x12\x34\x56”进行DES/TDES加密运算,得到密文的前4个字节即为KCV*/
	SEC_KCV_DATA,		/**<传入一串数据KcvData,使用源密钥对[aucDstKeyValue(密文) + KcvData]进行指定模式的MAC运算,得到8个字节的MAC即为KCV */
}EM_SEC_KCV;

/**
 *@brief MAC算法
*/
typedef enum{
	SEC_MAC_X99=0,      /**< X99算法：数据分为8字节block，不足补0，每个block加密后与下一个block异或后按密钥长度加密*/
	SEC_MAC_X919,       /**< X99算法：数据分为8字节block，不足补0，每个block加密后与下一个block异或后按密钥DES加密，
                            最后帧如果密钥长度为16字节则按3DES，如果为8字节按DES*/
	SEC_MAC_ECB,        /**< 全部数据异或后，将异或后数据做DES后进行变换，参考银联规范中关于ECB算法说明*/
	SEC_MAC_9606,       /**< 全部数据异或后，最后将异或数据做des运算*/
}EM_SEC_MAC;
/**
 * 用于实现PIN输入过程的超时控制的变量
 */
typedef enum {
	SEC_PIN_ISO9564_0=3,    /**<使用主账号加密，密码不足位数补'F'*/
	SEC_PIN_ISO9564_1=4,    /**<不使用主账号加密，密码不足位数补随机数*/
	SEC_PIN_ISO9564_2=5,    /**<不使用主账号加密，密码不足位数补'F'*/
	SEC_PIN_ISO9564_3=6     /**<使用主账号加密，密码不足位数补随机数*/
}EM_SEC_PIN;

/**
 *@brief DES计算类型，对于不同位可以进行或运算，
        例如：SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8|SEC_DES_ECBMODE
            表示，用8字节密钥长度，使用ECB模式进行加密运算
*/
typedef enum{
	SEC_DES_ENCRYPT=0,                  /**<DES加密*/
	SEC_DES_DECRYPT=1,                  /**<DES解密*/
	SEC_DES_KEYLEN_DEFAULT=(0<<1),      /**<使用安装长度的密钥进行加密*/
    SEC_DES_KEYLEN_8=(1<<1),            /**<使用8字节密钥进行加密*/
    SEC_DES_KEYLEN_16=(2<<1),           /**<使用16字节密钥进行加密*/
    SEC_DES_KEYLEN_32=(3<<1),           /**<使用32字节密钥进行加密*/
	SEC_DES_ECBMODE=(1<<3),             /**<使用ECB模式进行加密*/
	SEC_DES_CBCMODE=(1<<4),             /**<使用CBC模式进行加密*/
}EM_SEC_DES;

/**
 *@brief 密钥信息
*/
typedef struct{
    uchar 	ucScrKeyType; 		/**< 发散该密钥的源密钥的密钥类型，参考 KEY_TYPE, 不得低于ucDstKeyType所在的密钥级别*/
    uchar 	ucDstKeyType; 		/**< 目的密钥的密钥类型，参考KEY_TYPE */
    uchar 	ucScrKeyIdx;		/**< 发散该密钥的源密钥索引,索引一般从1开始,如果该变量为0,则表示这个密钥的写入是明文形式 */
    uchar 	ucDstKeyIdx;		/**< 目的密钥索引 */
    int 	nDstKeyLen;			/**< 目的密钥长度,8,16,24 */
    uchar 	sDstKeyValue[24];	/**< 写入密钥的内容 */
}ST_SEC_KEY_INFO;

/**
 *@brief 校验信息
*/
typedef struct{
    int 	nCheckMode; 		/**< 校验模式 */
    int 	nLen;				/**< 校验数据区长度 */
    uchar 	sCheckBuf[128];		/**< 校验数据缓冲区 */
}ST_SEC_KCV_INFO;

/**
 *@brief 脱机密文PIN密钥
*/
typedef struct
{
	uint	unModlen;					/**< 加密公钥模数长  */
	uchar	sMod[MAX_RSA_MODULUS_LEN];  /**< 加密公钥模数,高字节在前,低字节在后,不足位前补0 */
	uchar	sExp[4];       				/**< 加密公钥指数,高字节在前,低字节在后,不足位前补0 */
	uchar	ucIccRandomLen;   			/**< 从卡片取得的随机数长  */
	uchar	sIccRandom[8];   			/**< 从卡片取得的随机数  */
}ST_SEC_RSA_PINKEY;

/**
 *@brief RSA密钥信息
*/
typedef struct {
    ushort usBits;                    		/**< RSA密钥位数 */
    uchar sModulus[MAX_RSA_MODULUS_LEN];  	/**< 模 */
    uchar sExponent[MAX_RSA_MODULUS_LEN]; 	/**< 指数 */
}ST_SEC_RSA_KEY;

/**
 *@brief	读取安全接口版本
 *@retval	pszVerInfoOut	版本信息（小于16字节）
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecGetVer(uchar * pszVerInfoOut);

/**
 *@brief	获取随机数
 *@param	nRandLen		需要获取的长度
 *@retval	pvRandom		随机数缓冲
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它\ref EM_NDK_ERR "EM_NDK_ERR"		操作失败
*/
int NDK_SecGetRandom(int nRandLen , void *pvRandom);

/**
 *@brief	设置安全配置
 *@details	1、用户一旦通过此函数设置了安全配置信息，则后续操作根据此设置的配置信息进行控制。
 			如果没有调用此函数设置，则后续操作会按照默认的最低安全配置进行。
 			2、通常安全配置信息只允许升高，不允许降低（配置信息参数表中任意一位由1降至0都被认为安全性降低）。
 *@param	unCfgInfo		配置信息
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecSetCfg(uint unCfgInfo);

/**
 *@brief	读取安全配置
 *@retval	punCfgInfo		配置信息
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecGetCfg(uint *punCfgInfo);

/**
 *@brief	读取密钥kcv值
 *@details	获取密钥的KCV值,以供对话双方进行密钥验证,用指定的密钥及算法对一段数据进行加密,并返回部分数据密文。
 *@param	ucKeyType		密钥类型
 *@param	ucKeyIdx		密钥序号
 *@param	pstKcvInfoOut	KCV加密模式
 *@retval	pstKcvInfoOut	KCV值
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecGetKcv(uchar ucKeyType, uchar ucKeyIdx, ST_SEC_KCV_INFO *pstKcvInfoOut);

/**
 *@brief	擦除所有密钥
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecKeyErase(void);

/**
 *@brief	写入一个密钥,包括TLK,TMK和TWK的写入、发散,并可以选择使用KCV验证密钥正确性。
 *@details
 	PED采用三层密钥体系,自上到下的顺序依次为：
	TLK－Terminal Key Loading Key
    	收单行或POS运营商的私有密钥,由收单行或者POS运营商在安全环境下直接写入。
    	该密钥每个PED终端只有一个,其索引号自1至1

	TMK－Terminal Master Key＝Acquirer Master Key
		终端主密钥,或者称为收单行主密钥。该类密钥可有100个,索引号自1至100
		TMK可以在安全环境下直接写入,直接写入TMK,并通过TMK分散TWK的方式与MK/SK的密钥体系一致。
	TWK－Transaction working key = Transaction Pin Key + Transaction MAC Key + Terminal DES Enc Key + Terminal DES DEC/ENC Key
		终端工作密钥,进行PIN密文、MAC等运算的密钥。该类密钥可有100个,索引号自1至100。
		TPK:用于应用输入PIN后,计算PIN Block。
		TAK:用于应用报文通讯中,计算MAC。
		TEK:用于对应用中敏感数据进行DES/TDES加密传输或存储。
		TDK:用于对应用中敏感数据进行DES/TDES加解密运用
	TWK可以在安全环境下直接写入,直接写入TWK与Fixed Key密钥体系一致。每个密钥有其索引号,长度,用途和标签。
	其中密钥的标签是在写入密钥前通过API设定的,以授权该密钥的使用权限并保证密钥不会被滥用。

	DUKPT密钥机制：
	DUKPT【Derived Unique Key Per Transaction】密钥体系是一次交易一密钥的密钥体系,其每笔交易的工作密钥【PIN、MAC】是不同的。
	它引入了KSN【Key Serial Number】的概念,KSN是能实现一次一密的关键因子。 每个KSN对应的密钥，根据密钥用途，产生出不同的密钥。
 	该类密钥可有10组。在写入TIK的时候,需要选择组的索引号,在使用DUKPT密钥时选择对应的组索引。
 *@param	pstKeyInfoIn		密钥信息
 *@param	pstKcvInfoIn		密钥校验信息
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecLoadKey(ST_SEC_KEY_INFO * pstKeyInfoIn, ST_SEC_KCV_INFO * pstKcvInfoIn);

/**
 *@brief	设置两次计算PINBlock或者计算MAC之间最小间隔时间
 *@details 	PINBLOCK间隔时间的计算方式：
 			默认为120秒那只能调用4次,即TPKIntervalTimeMs默认值为30秒,调用该函数重新设置后,限制为4*TPKIntervalTimeMs时间内只能调用4次。
 			比如传入的TPKIntervalTimeMs为20000(ms),则80秒内只能调用4次
 *@param	unTPKIntervalTimeMs	PIN密钥计算间隔时间，0-采用默认值，0xFFFFFFFF，不改变
 *@param	unTAKIntervalTimeMs	MAC密钥计算间隔时间，0-采用默认值，0xFFFFFFFF，不改变
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecSetIntervaltime(uint unTPKIntervalTimeMs, uint unTAKIntervalTimeMs);

/**
 *@brief	设置功能键功能
 *@details 	对密码输入过程中，功能键用途进行定义
 *@param	ucType	功能用途类型定义
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecSetFunctionKey(uchar ucType);

/**
 *@brief	计算MAC
 *@param	ucKeyIdx		密钥序号
 *@param	psDataIn		输入数据
 *@param	nDataInLen		输入数据长度
 *@param	ucMod			MAC计算模式 参考/ref EM_SEC_MAC "EM_SEC_MAC"
 *@retval	psMacOut		MAC值，长度8字节
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecGetMac(uchar ucKeyIdx, uchar *psDataIn, int nDataInLen, uchar *psMacOut, uchar ucMod);

/**
 *@brief	获取PIN Block
 *@param	ucKeyIdx		密钥序号
 *@param	pszExpPinLenIn	密码长度，可使用,进行分割，例如：0,4,6
 *@param	pszDataIn		按ISO9564要求的输入PIN BLOCK
 *@param	ucMode			计算模式 参考/ref EM_SEC_PIN "EM_SEC_PIN"
 *@param	nTimeOutMs		超时时间（不允许大于120秒）单位:ms
 *@retval	psPinBlockOut	    PIN Block输出
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecGetPin(uchar ucKeyIdx, uchar *pszExpPinLenIn,const uchar * pszDataIn, uchar *psPinBlockOut, uchar ucMode, uint nTimeOutMs);

/**
 *@brief	计算DES
 *@details 	使用指定密钥进行des计算，注意：1~100序号进行加解密
 *@param	ucKeyType		DES密钥类型
 *@param	ucKeyIdx		DES密钥序号
 *@param	psDataIn		数据信息
 *@param	nDataInLen		数据长度
 *@param	ucMode			加密模式 参考/ref EM_SEC_DES "EM_SEC_DES"
 *@retval	psDataOut		数据输出信息
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecCalcDes(uchar ucKeyType, uchar ucKeyIdx, uchar * psDataIn, int nDataInLen, uchar *psDataOut, uchar ucMode);

/**
 *@brief	校验脱机明文PIN
 *@details 	获取明文PIN,然后按照应用提供的卡片命令与卡片通道号,将明文PIN BLOCK直接发送给卡片(PIN BLOCK格式在用法部分描述)。
 *@param	ucIccSlot		IC卡号
 *@param	pszExpPinLenIn	密码长度，可使用,进行分割，例如：0,4,6
 *@param	ucMode			IC卡计算模式(只支持EMV)
 *@param	unTimeoutMs		超时时间
 *@retval	psIccRespOut	卡片应答码
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecVerifyPlainPin(uchar ucIccSlot, uchar *pszExpPinLenIn, uchar *psIccRespOut, uchar ucMode,  uint unTimeoutMs);

/**
 *@brief	校验脱机明文PIN
 *@details 	先获取明文PIN,再用应用提供的RsaPinKey对明文PIN按照EMV规范进行加密,然后用应用提供的卡片命令与卡片通道号,将密文PIN直接发送给卡片
 *@param	ucIccSlot		IC卡号
 *@param	pszExpPinLenIn	密码长度，可使用,进行分割，例如：0,4,6
 *@param	pstRsaPinKeyIn	RSA密钥数据
 *@param	ucMode			IC卡计算模式(只支持EMV)
 *@param	unTimeoutMs		超时时间
 *@retval	psIccRespOut	卡片应答码
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecVerifyCipherPin(uchar ucIccSlot, uchar *pszExpPinLenIn, ST_SEC_RSA_KEY *pstRsaPinKeyIn, uchar *psIccRespOut, uchar ucMode, uint unTimeoutMs);

/**
 *@brief	安装DUKPT密钥
 *@param	ucGroupIdx		密钥组ID
 *@param	ucSrcKeyIdx		原密钥ID（用来加密初始密钥值的密钥ID）
 *@param	ucKeyLen		密钥长度
 *@param	psKeyValueIn	初始密钥值
 *@param	psKsnIn		    KSN值
 *@param	pstKcvInfoIn	Kcv信息
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecLoadTIK(uchar ucGroupIdx, uchar ucSrcKeyIdx, uchar ucKeyLen, uchar * psKeyValueIn, uchar * psKsnIn, ST_SEC_KEY_INFO * pstKcvInfoIn);

/**
 *@brief	获取DUKPT值
 *@param	ucGroupIdx		DUKPT密钥组ID
 *@retval	psKsnOut		当前KSN号
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecGetDukptKsn(uchar ucGroupIdx, uchar * psKsnOut);

/**
 *@brief	KSN号增加
 *@param	ucGroupIdx		DUKPT密钥组ID
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecIncreaseDukptKsn(uchar ucGroupIdx);

/**
 *@brief	获取DUKPT密钥的PIN Block
 *@param	ucGroupIdx		密钥序号
 *@param	pszExpPinLenIn	密码长度，可使用,进行分割，例如：0,4,6
 *@param	psDataIn		按ISO9564要求的输入PIN BLOCK
 *@param	ucMode			计算模式
 *@param	unTimeoutMs		超时时间
 *@retval	psKsnOut		当前KSN号
 *@retval	psPinBlockOut	PIN Block输出
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecGetPinDukpt(uchar ucGroupIdx, uchar *pszExpPinLenIn, uchar * psDataIn, uchar* psKsnOut, uchar *psPinBlockOut, uchar ucMode, uint unTimeoutMs);

/**
 *@brief	计算DUKPT密钥MAC
 *@param	ucGroupIdx		密钥组号
 *@param	psDataIn		输入数据
 *@param	nDataInLen		输入数据长度
 *@param	ucMode			MAC计算模式
 *@retval	psMacOut		MAC值，长度8字节
 *@retval	psKsnOut		当前KSN号
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecGetMacDukpt(uchar ucGroupIdx, uchar *psDataIn, int nDataInLen, uchar *psMacOut, uchar *psKsnOut, uchar ucMode);

/**
 *@brief	计算DES
 *@details 	使用指定密钥进行des计算
 *@param	ucGroupIdx		DUKPT密钥组号
 *@param	ucKeyVarType	密钥类型
 *@param	psIV			初始向量
 *@param	psDataIn		数据信息
 *@param	usDataInLen		数据长度
 *@param	ucMode			加密模式
 *@retval	psDataOut		数据输出信息
 *@retval	psKsnOut		当前KSN号
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecCalcDesDukpt(uchar ucGroupIdx, uchar ucKeyVarType, uchar *psIV, ushort usDataInLen, uchar *psDataIn,uchar *psDataOut,uchar *psKsnOut ,uchar ucMode);

/**
 *@brief	安装RSA密钥
 *@param	ucRSAKeyIndex	密钥序号
 *@param 	pstRsakeyIn		RSA密钥信息
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecLoadRsaKey(uchar ucRSAKeyIndex, ST_SEC_KEY_INFO *pstRsakeyIn);

/**
 *@brief	安装RSA密钥
 *@param	ucRSAKeyIndex	密钥序号
 *@param 	psDataIn		待加密数据，不能超过密钥模长
 *@param	nDataInLen		数据长度
 *@retval	psDataOut		输出数据,和模等长
 *@retval	psKeyInfoOut	密钥信息
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SecRecover(uchar ucRSAKeyIndex, const uchar *psDataIn, int nDataInLen, uchar *psDataOut, uchar *psKeyInfoOut);

/** @} */ // 安全模块结束



/** @addtogroup 串口通讯
* @{
*/


/**
 *@brief  串口选择
*/
typedef enum {
    PORT_NUM_COM1 = 0,	/**<串口1*/
    PORT_NUM_COM2 = 1,	/**<串口2*/
    PORT_NUM_WIRELESS = 2,	/**<无线模块*/
    PORT_NUM_MUX1 = 3,	/**<多路复用1*/
    PORT_NUM_MUX2 = 4,	/**<多路复用2*/
    PORT_NUM_MUX3 = 5,	/**<多路复用3*/
    PORT_NUM_MODEM = 6,	/**<有线模块*/
    PORT_NUM_WIFI = 7,	/**<Wifi模块*/
    PORT_NUM_USB = 8	/**<USB模块*/
} EM_PORT_NUM;




/**
 *@brief	初始化串口，对串口波特率，数据位、奇偶位、停止位等进行设置。建议每次使用串口之前先调用该初始化函数
 *@param	emPort	指定的串口
 *@param	pszAttr	通讯率和格式串,例"115200,8,N,1",如果只写波特率则缺省为"8,N,1"
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PortOpen(EM_PORT_NUM emPort, const char *pszAttr);

/**
 *@brief	关闭串口
 *@param	emPort	指定的串口
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_PortClose(EM_PORT_NUM emPort);

/**
 *@brief	在设定超时时间里从指定的串口，读取指定长度的数据，存放于pszOutbuf
 *@param	emPort	指定的串口
 *@param	unLen	表示要读的数据长度，>0
 *@param	nTimeoutMs	等待时间，单位为毫秒
 *@retval	pszOutbuf	接收数据缓冲区的头指针
 *@retval	pnReadlen	返回读的实际长度
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_PortRead(EM_PORT_NUM emPort, uint unLen, char *pszOutbuf,int nTimeoutMs, int *pnReadlen);

/**
 *@brief	给指定的串口送指定长度的数据，存放于pszInbuf
 *@param	emPort	指定的串口
 *@param	unLen	表示要写的数据长度
 *@param	pszInbuf	数据发送的缓冲区
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_PortWrite(EM_PORT_NUM emPort, uint unLen,const char *pszInbuf);

/**
 *@brief	判断指定串口发送缓冲区是否为空
 *@param	emPort	指定的串口
 *@return
 *@li	NDK_OK	缓冲区无数据
 *@li	NDK_ERR	缓冲区有数据
*/
int NDK_PortTxSendOver(EM_PORT_NUM emPort);

/**
 *@brief	清除指定串口的接收缓冲区
 *@param	emPort	指定的串口
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PortClrBuf(EM_PORT_NUM emPort);

/**
 *@brief	取缓冲区里有多少字节要被读取
 *@param	emPort	指定的串口
 *@retval	pnReadlen	返回缓冲区被读取的长度
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PortReadLen(EM_PORT_NUM emPort,int *pnReadlen);

/** @} */ // 串口通讯模块结束

/** @addtogroup modem模块
* @{
*/

/**
*@brief  modem连接状态定义
*/
typedef enum MDM_STATUS
{
	MDMSTATUS_NORETURN_AFTERPREDIAL=0,
	MDMSTATUS_OK_AFTERPREDIAL=1,
	MDMSTATUS_CONNECT_AFTERPREDIAL=2,
	MDMSTATUS_MS_NODIALTONE = -2,
	MDMSTATUS_MS_NOCARRIER =	-3,
	MDMSTATUS_MS_BUSY = -4,
	MDMSTATUS_MS_ERROR = -5,
	MDMSTATUS_LINE_VOL_TOO_LOW =-8,
	MDMSTATUS_NOPREDIAL = -11,
}EM_MDMSTATUS;
/**
*@brief  modem初始化函数的补丁参数类型定义
*/
typedef enum MDM_Patchtype
{
	MDM_PatchType5=5,//默认的补丁包方式
	MDM_PatchType4=4,
	MDM_PatchType3=3,//快速连接补丁包
	MDM_PatchType2=2,
	MDM_PatchType1=1,
}EM_MDM_PatchType;

/**
 *@brief	同步拨号初始化函数。
 *@param	nType		补丁包参数，对应不同的线路环境而设置使用。
 *@return	无
  *@li	NDK_OK				操作成功
 *@li	其它NDK_ERR_MODEM_COUNTRYFAIL	打补丁失败
                 NDK_ERR_MODEM_SDLCINITFAIL    初始化失败
*/
int NDK_MdmSdlcInit(EM_MDM_PatchType nType);

/**
 *@brief	异步modem初始化。
 *@param	nType 	补丁包参数，对应不同的线路环境而设置使用
 *@return
 *@li	NDK_OK				操作成功
 *@li	NDK_ERR_MODEM_ASYNINITFAIL	初始化失败
*/
int NDK_MdmAsynInit(EM_MDM_PatchType nType);

/**
 *@brief	modem拨号函数。
 *@param	pszDailNum 	拨号号码
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它NDK_ERR_MODEM_SDLCDIALFAIL		同步拨号失败
                 NDK_ERR_MODEM_INIT_NOT         拨号前未做初始化
*/
int NDK_MdmDial(const char * pszDailNum);

/**
 *@brief	检测modem状态。
 *@param	*pemStatus 	modem状态的实际返回值
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_MdmCheck(EM_MDMSTATUS  *pemStatus);

/**
 *@brief	modem数据发送。
 *@param	pszData 	发送的数据
 *@param	unDatalen 	发送的数据长度
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它NDK_ERR_MODEM_SDLCWRITEFAIL		同步发送失败
				 NDK_ERR_MODEM_ASYNWRITEFAIL      异步发送失败
				 NDK_ERR_MODEM_INIT_NOT                 未初始化
*/
int NDK_MdmWrite(const char *pszData,uint unDatalen);

/**
 *@brief	modem数据接收。
 *@param	*pszData 	接收的数据
 *@param	*punDatalen 	接收的数据长度
  *@param	unSenconds	超时时间，以S为单位
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它	NDK_ERR_MODEM_SDLCREADFAIL	同步接收操作失败
					NDK_ERR_MODEM_ASYNREADFAIL   异步接收失败
					NDK_ERR_MODEM_INIT_NOT                 未初始化
*/
int NDK_MdmRead(char  *pszData,uint *punDatalen,uint unSenconds);

/**
 *@brief	modem挂断函数。
 *@param	无
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_MdmHangup(void);

/**
 *@brief	清除modem缓冲区。
 *@param	无
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_MdmClrbuf(void);

/**
 *@brief	读取modem长度。
 *@param	*punReadlen 	返回的长度值
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_MdmGetreadlen(uint *punReadlen);

/**
 *@brief	modem复位函数。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_MdmReset(void);

/**
 *@brief	AT命令交互函数。
 *@param	pucCmdstr 	输入的命令串
 *@param	pszRespData 	返回的响应数据
 *@param	punLen 	返回的数据长度
 *@param	unTimeout 	超时时间
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_MdmExCommand(uchar *pucCmdstr,uchar *pszRespData,uint *punLen,uint unTimeout);

/** @} */ // modem模块结束

/** @addtogroup 无线模块
* @{
*/

/**
 *@brief  无线模块常用命令定义
*/
typedef enum {
    WLM_CMD_E0=0,					/**<关闭回显*/
    WLM_CMD_CSQ=1,				/**<取信号值*/
    WLM_CMD_CREG=2,				/**<网络注册状态*/
    WLM_CMD_CPIN=3,				/**<查询卡PIN*/
    WLM_CMD_CPIN0=4,			/**<设置PIN码*/
    WLM_CMD_CGATT0=5,			/**<注销GPRS网络*/
    WLM_CMD_CGATT1=6,			/**<注册GPRS网络*/
    WLM_CMD_DIAL=7,				/**<拨号*/
    WLM_CMD_D2=8,					/**<启用硬件挂断功能*/
    WLM_CMD_COPS=9,				/**<注册网络*/
    WLM_CMD_CGMR=10,			/**<取模块版本*/
    WLM_CMD_CGSN,         /**<<获取GSN号(imei或meid)*/
    WLM_CMD_END,          /**<命令结束，如果新增命令都放这之前位置*/
    WLM_CMD_UNDEFINE=1024,		/**<列表中未定义*/
    WLM_CMD_PUBLIC=255
} EM_WLM_CMD;

/**
 *@brief  AT指令数据包
*/
typedef struct NDK_WL_ATCMD{
		EM_WLM_CMD AtCmdNo;	/**<命令号*/
		char *pcAtCmd;			/**<命令字串*/
		char *pcAddParam;		/**<附加参数*/
}ST_ATCMD_PACK;

/**
 *@brief  无线模块返回状态定义
*/
typedef enum{
    WLM_STATUS_UNTYPED=4,	/**<未定义返回*/
    WLM_STATUS_NO_CARRIER=3,/**<无载波*/
    WLM_STATUS_RING=2,		/**<RING串*/
    WLM_STATUS_CONNECT=1,	/**<连接串*/
    WLM_STATUS_OK=0,				/**<成功*/
    WLM_STATUS_ERROR=-1,			/**<错误*/
    WLM_STATUS_RET_ERROR=-114,	/**<返回出错*/
}EM_WLM_STATUS;


/**
 *@brief	无线MODEM的硬件复位
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_WlModemReset(void);

/**
 *@brief	关闭无线MODEM模块
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_WlModemClose(void);

/**
 *@brief	无线MODEM初始化，切换串口到无线并判断模块AT指令能否正常响应，检测SIM卡
 *@param	nTimeout	超时时间，单位MS
 *@param	pszPINPassWord	PIN码
 *@retval	pemStatus	执行成功返回无线状态，失败返回 NDK_FAIL	失败
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_WlInit(int nTimeout,const char *pszPINPassWord,EM_WLM_STATUS *pemStatus);


/**
 *@brief	获取无线MODEM信号强度
 *@retval	pnSq	取到的信号强度，取到的值	0-31 为成功，99	为未知,-1 为失败
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_WlModemGetSQ(int *pnSq);


/**
 *@brief	开启或关闭系统获取无线模块信号强度
 *@param	nx	显示无线信号起始横坐标
 *@param	ny	显示无线信号起始纵坐标
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_WlShowSignalQuality(int nx, int ny);


/**
 *@brief	向无线模块发送和接收AT指令。当pATCmdPack为空NULL时表明直接等待接收,否则发送AT指令;如果pcOutput为空NULL则表明不接收AT指令
 *@param	pstATCmdPack	AT指令数据包
 *@param	unMaxlen	缓冲最大长度（pszOutput缓冲区长度）（=0时使用缺省长度1024）
 *@param	unTimeout	命令超时时间，单位：MS
 *@retval	pszOutput	输出串
 *@retval	pemStatus	执行成功返回无线状态，失败返回 NDK_FAIL	失败
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_WlSendATCmd(const ST_ATCMD_PACK *pstATCmdPack,char *pszOutput,uint unMaxlen,uint unTimeout,EM_WLM_STATUS *pemStatus);

/**
 *@brief	关闭射频
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_WlCloseRF(void);

/**
 *@brief	选择SIM卡
 *@param	ucSimNo	卡号
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_WlSelSIM(uchar ucSimNo);


/** @} */ // 无线模块结束


/** @addtogroup Socket通讯
* @{
*/


/**
 *@brief	打开TCP通讯通道
 *@retval	pnFd	返回TCP通道句柄
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_TcpOpen(int *pnFd);

/**
 *@brief	关闭TCP通讯通道
 *@param	unFd	要关闭的TCP通道句柄
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_TcpClose(uint unFd);

/**
 *@brief	等待TCP关闭成功，一旦关闭及时退出，调用NDK_TcpClose()后，必须继续调用该函数确保TCP链路完全关闭
 *@param	unFd	TCP通道句柄
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_TcpWait(uint unFd);

/**
 *@brief	绑定本端的IP地址和端口号
 *@param	unFd		TCP通道句柄
 *@param	pszMyIp	源地址
 *@param	usMyPort	源端口
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_TcpBind(uint unFd, const char *pszMyIp, ushort usMyPort);

/**
 *@brief	连接服务器
 *@param	unFd	TCP通道句柄
 *@param	pszRemoteIp	远程地址
 *@param	usRemotePort	远程端口
 *@param	unTimeout	远程连接超时时间，单位为秒
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_TcpConnect(uint unFd, const char *pszRemoteIp, ushort usRemotePort, uint unTimeout);

/**
 *@brief	监听申请的连接
 *@param	unFd	TCP通道句柄
 *@param	nBacklog	等待连接队列的最大长度
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_TcpListen(uint unFd, int nBacklog);

/**
 *@brief	接收连接请求
 *@param	unFd	TCP通道句柄
 *@param	pszPeerIp	接收连接实体的地址
 *@param	usPeerPort	接收连接实体的端口
 *@retval	punNewFd	返回TCP通道句柄
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_TcpAccept(uint unFd, const char *pszPeerIp, ushort usPeerPort, uint *punNewFd);

/**
 *@brief	发送数据
 *@param	unFd	TCP通道句柄
 *@param	pInbuf	发送缓冲区的地址
 *@param	unLen	发送数据的长度
 *@param	unTimeout	超时时间，单位为秒
 *@retval	punWriteLen	接收实际发送长度,如果为NULL则不接收
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_TcpWrite(uint unFd, const void *pInbuf, uint unLen, uint unTimeout, uint *punWriteLen);

/**
 *@brief	接收数据
 *@param	unFd	TCP通道句柄
 *@param	unLen	接收数据的长度
 *@param	unTimeout	超时时间，单位为秒
 *@retval	pOutbuf	接收缓冲区的地址
 *@retval	punReadlen	返回实际发送长度
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_TcpRead(uint unFd, void *pOutbuf, uint unLen, uint unTimeout, uint *punReadlen);

/**
 *@brief	打开UDP通讯通道
 *@retval	pnFd	返回UDP通道句柄
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_UdpOpen(int *pnFd);


/** @} */ // Socket通讯模块结束


/** @addtogroup PPP通讯
* @{
*/

/**
 *@brief  PPP连接状态定义
*/
typedef enum {
    PPP_STATUS_DISCONNECT=0,		/**<PPP未连接*/
    PPP_STATUS_CONNECTING=2,		/**<PPP正在连接*/
    PPP_STATUS_CONNECTED=5,			/**<PPP已连接*/
    PPP_STATUS_DISCONNECTING=6		/**<正在挂断*/
} EM_PPP_STATUS;

/**
 *@brief  PPP配置参数
*/
typedef struct {
    int nDevType;			/**<类型，区别是无线MODEM还是有线MODEM，0表示无线，1表示有线*/
    unsigned int nPPPFlag;	/**<是否支持维持长连接标识，根据需要传入相对应的宏定义*/
    char szApn[64];			/**<APN设置*/
    char szDailNum[32];		/**<拨号号码*/
    int (*ModemDial)(void);/**<拨号函数*/
    unsigned int PPPIntervalTimeOut; /**<维持长链接的数据包发送的时间间隔,<30S将不会进行长链接的维持*/
    unsigned char nMinSQVal;	/**<初始化时，允许的最小的信号值*/
    char szPin[31];				/**<SIM卡PIN码*/
    char nPPPHostIP[20];		/**<维持长链接需要PIN的主机IP*/
} ST_PPP_CFG;

/**
 *@brief	配置PPP参数
 *@param	pstPPPCfg	ppp参数结构(设备类型设置缺省时为无线)
 *@param	nuValidLen	参数有效长度
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_PppSetCfg(ST_PPP_CFG *pstPPPCfg, uint nuValidLen);

/**
 *@brief	PPP拨号
 *@param	pszUsername	用户名
 *@param	pszPassword	密码
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_PppDial(const char *pszUsername,const char *pszPassword);

/**
 *@brief	PPP挂断
 *@param	nHangupType	挂断类型 0 非阻塞挂断 1 阻塞挂断
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PppHangup(int nHangupType);

/**
 *@brief	获取PPP状态
 *@retval	pemStatus	返回PPP状态,为NULL则不执行该操作
 *@retval	pnErrCode	返回PPP连接错误,为NULL则不执行该操作
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PppCheck(EM_PPP_STATUS *pemStatus, int *pnErrCode);

/**
 *@brief	获取本地地址和主机地址
 *@retval	punLocalAddr	返回本地地址,为NULL则不接收
 *@retval	punHostAddr	返回主机地址,为NULL则不接收
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PppGetAddr(uint *punLocalAddr, uint *punHostAddr);

/**
 *@brief	将一个点分十进制的IP转换成一个长整数型数
 *@param	pszIp	IP地址字符串
 *@retval	pulIpAddr	返回转换后的长整形数
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PppAddrChange(register const char *pszIp, ulong *pulIpAddr);

/** @} */ // PPP通讯模块结束

/** @addtogroup 以太网通讯
* @{
*/


/**
 *@brief	设置网络地址,参数都为NULL返回参数错误(设置后如果原已动态分配的地址,也会被修改)
 *@param	pszIp	本地IP 地址字符串的首指针,为NULL则不设IP地址.仅支持IPV4协议
 *@param	pszMask	本地子网掩码字符串的首指针,为NULL则不设Mask地址.仅支持IPV4协议
 *@param	pszGateway	本地网关地址字符串的首指针,为NULL则不设Gateway地址.仅支持IPV4协议
 *@param 	pszDns	本地DNS服务器IP地址,为NULL则不设DNS地址,最多可设置3个DNS,之间以分号符';'隔开
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_EthSetAddress(const char *pszIp, const char *pszMask, const char *pszGateway, const char *pszDns);


/**
 *@brief	获取网络MAC地址
 *@retval	pszMacAddr	返回MAC地址
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_EthGetMacAddr(char *pszMacAddr);


/**
 *@brief	获取网络地址,参数都为NULL返回参数错误
 *@retval	pszIp	返回IP地址,为NULL则不取IP地址
 *@retval	pszGateway	返回网关地址,为NULL则不取Gateway地址
 *@retval	pszMask	返回子网掩码,为NULL则不取Mask地址
 *@retval	pszDns	DNS服务器IP地址,为NULL则不取DNS地址,一次性取出所以DNS,之间以分号符';'隔开
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_EthGetNetAddr(char *pszIp, char *pszMask, char *pszGateway, char *pszDns);


/**
 *@brief	设置以太网工作模式
 *@param	nMode	工作模式
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_EthSetWorkMode(int nMode);


/**
 *@brief	使用dhcp获取网络地址
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_NetDHCP(void);


/**
 *@brief	网络PING功能
 *@param	pszIp	本地IP地址字符串的首指针,不可为空指针.仅支持IPV4协议
 *@param	nTimeoutMs	超时时间,单位为毫秒
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_NetPing(char *pszIp, uint nTimeoutMs);


/** @} */ // 以太网通讯模块结束


/** @addtogroup 磁卡
* @{
*/

/**
 *@brief	打开磁卡设备
 *@param	无
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int NDK_MagOpen(void);

/**
 *@brief	关闭磁卡设备
 *@param	无
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int NDK_MagClose(void);


/**
 *@brief	复位磁头
 *@details	 复位磁头且清除磁卡缓冲区数据
 *@param	无
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int NDK_MagReset(void);


/**
 *@brief	判断是否刷过卡
 *@retval	pcSwiped	1----已刷卡    0-----未刷卡
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int NDK_MagSwiped(uchar * pcSwiped);


/**
 *@brief	读取磁卡缓冲区的1、2、3磁道的数据
 *@details	与MagSwiped函数配合使用。如果不需要某磁道数据,可以将该磁道对应的指针置为NULL,这时将不会输出该磁道的数据
 *@retval	pszTk1	磁道1
 *@retval	pszTk2	磁道2
 *@retval	pszTk3	磁道3
 *@retval	pnErrorCode	磁卡错误代码
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int NDK_MagReadNormal(char *pszTk1, char *pszTk2, char *pszTk3, int *pnErrorCode);


/**
 *@brief	读取磁卡缓冲区的1、2、3磁道的原始数据
 *@details	与MagSwiped函数配合使用,如果不需要某磁道数据,可以将该磁道对应的指针置为NULL,这时将不会输出该磁道的数据
 *@retval	pszTk1	磁道1
 *@retval	punTk1Len	磁道1数据长度
 *@retval	pszTk2	磁道2
 *@retval	punTk2Len	磁道2数据长度
 *@retval	pszTk3	磁道3
 *@retval	punTk3Len	磁道3数据长度

 *@return:
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int NDK_MagReadRaw(uchar *pszTk1, ushort* punTk1Len, uchar *pszTk2, ushort* punTk2Len,uchar *pszTk3, ushort* punTk3Len );

/** @} */ // 磁卡模块结束

/** @addtogroup IC卡
* @{
*/

typedef enum{
  ICTYPE_IC,  /**<接触式IC卡*/
  ICTYPE_SAM1, /**<SAM1卡*/
  ICTYPE_SAM2, /**<SAM2卡*/
  ICTYPE_SAM3, /**<SAM3卡*/
  ICTYPE_SAM4, /**<SAM4卡*/
  ICTYPE_M_1, /**<at24c01 at24c02 at24c04 at24c08 at24c16 at24c32 at24c64 */
  ICTYPE_M_2, /**<sle44x2*/
  ICTYPE_M_3, /**<sle44x8*/
  ICTYPE_M_4, /**<at88sc102*/
  ICTYPE_M_5, /**<at88sc1604*/
  ICTYPE_M_6, /**<at88sc1608*/
}EM_ICTYPE;

/**
 *@brief	获取驱动程序版本号
 *@retval 	pszVersion   　　驱动程序版本号,要求缓冲大小不低于16字节
 *　
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int  NDK_IccGetVersion(char *pszVersion);

/**
 *@brief	设置卡类型
 *@param	emIctype　  卡类型　
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int NDK_IccSetType(EM_ICTYPE emIctype);

/**
 *@brief	获取卡类型
 *@retval 	pemIctype 卡类型　
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int NDK_IccGetType(EM_ICTYPE *pemIctype);

/**
 *@brief	获取卡片状态
 *@retval 	pnSta   bit0：如果IC卡1已插卡，为“1”，否则，为“0”
 *					bit1：如果IC卡1已上电，为“1”，否则，为“0”
 *					bit2：如果SAM卡1已上电，为“1”，否则，为“0”
 *					bit3：如果SAM卡2已上电，为“1”，否则，为“0”
 *					bit4：如果SAM卡3已上电，为“1”，否则，为“0”
 *					bit5：如果SAM卡4已上电，为“1”，否则，为“0”
 *					bit6：保留，返回值“0”
 *					bit7：保留，返回值“0”
 *　
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int NDK_IccDetect(int *pnSta);


/**
 *@brief	上电
 *@param	emIctype　 ICC_IC 接触式IC卡，　
 *                    ICC_SAM1　ＳＡＭ１卡，
 *					  ICC_SAM2　ＳＡＭ２卡，
 *					  ICC_SAM3　ＳＡＭ３卡，
 *					  ICC_SAM4　ＳＡＭ４卡，
 *@retval 　　psAtrbuf  表示ATR数据
 *@retval     pnAtrlen  表示ATR数据的长度
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int NDK_IccPowerUp (EM_ICTYPE emIctype, uchar *psAtrbuf,int *pnAtrlen);

/**
 *@brief	下电
 *@param	emIctype　 ICC_IC 接触式IC卡，　
 *                    ICC_SAM1　ＳＡＭ１卡，
 *					  ICC_SAM2　ＳＡＭ２卡，
 *					  ICC_SAM3　ＳＡＭ３卡，
 *					  ICC_SAM4　ＳＡＭ４卡，
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int NDK_IccPowerDown(EM_ICTYPE emIctype);

/**
 *@brief	IC卡操作
 *@param	emIctype　 ICC_IC 接触式IC卡，　
 *                    ICC_SAM1　ＳＡＭ１卡，
 *					  ICC_SAM2　ＳＡＭ２卡，
 *					  ICC_SAM3　ＳＡＭ３卡，
 *					  ICC_SAM4　ＳＡＭ４卡，
 *@param 　 nSendlen  　 发送数据的长度
 *@param	psSendbuf　　发送的数据
 *@retval 	pnRecvlen    接收数据长度
 *@retval 	psRecebuf    接收的数据
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR			操作失败
*/
int NDK_Iccrw(EM_ICTYPE emIctype, int nSendlen,  uchar *psSendbuf, int *pnRecvlen,  uchar *psRecebuf);

/** @} */ // IC卡模块结束

/** @addtogroup 射频卡
* @{
*/
typedef enum {
	RFID_Autoscan=		0x00,
	RFID_RC531=			0x01,
	RFID_PN512=			0x02,
	RFID_No=			0xFF,
}EM_RFID;

/**
 *@brief	获取射频驱动版本号
 *@param	pszVersion	返回的驱动版本号字符串
 *@return
 *@li	NDK_OK			操作成功
 *@li	EM_NDK_ERR		操作失败
*/
int  NDK_RfidVersion(uchar *pszVersion);


/**
 *@brief	射频接口器件初始化，可用于判断器件是否装配或可工作
 *@retval 	psStatus  扩充备用
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_RfidInit(uchar *psStatus);


/**
 *@brief	开射频输出
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_RfidOpenRf(void);


/**
 *@brief	关闭射频输出，可降低功耗并保护射频器件。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_RfidCloseRf(void);


/**
 *@brief	获取卡片状态(是否已寻卡，用于判断是否可休眠)
 *@return
 *@li	NDK_OK		已寻卡成功(忙状态)
 *@li	NDK_ERR_RFID_NOTACTIV		未寻卡成功(非忙状态)
*/
int NDK_RfidPiccState(void);


/**
 *@brief	设备强制休眠
 *@return
 *@li	NDK_OK		执行成功
 *@li	NDK_ERR		执行失败
*/
int NDK_RfidSuspend(void);


/**
 *@brief	设备唤醒
 *@return
 *@li	NDK_OK		执行成功
 *@li	NDK_ERR		执行失败
*/
int NDK_RfidResume(void);


/**
 *@brief	设置寻卡策略(寻卡操作前设置一次即可，M1卡操作时需要设置成TYPE-A卡模式)
 *@param	ucPicctype = 0xcc，表示寻卡时只针对TYPE-A的卡.
 *			   0xcb，表示寻卡时只针对TYPE-B的卡.
 *			   0xcd，表示寻卡时针对TYPE-A及TYPE-B的卡.
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_RfidPiccType(uchar ucPicctype);


/**
 *@brief	射频卡寻卡(探测刷卡区域是否有卡)
 *@retval 	psPicctype	激活的卡类型 0xcc=TYPE-A卡，0xcb=TYPE-B卡
 *@return
 *@li	NDK_OK				寻卡成功，卡类型在picctype
 *@li	其它EM_NDK_ERR		寻卡失败
*/
int NDK_RfidPiccDetect(uchar *psPicctype);


/**
 *@brief	射频卡激活(有卡的情况下),相当于powerup 。
 *@retval 	psPicctype	激活的卡类型 0xcc=TYPE-A卡，0xcb=TYPE-B卡
 *@retval	pnDatalen	 数据长度
 *@retval	psDatabuf	数据缓冲区(A卡为UID；B卡databuf[1]~[4]为UID，其它是应用及协议信息)
 *@return
 *@li	NDK_OK				激活成功
 *@li	其它EM_NDK_ERR		激活失败
*/
int NDK_RfidPiccActivate(uchar *psPicctype, int *pnDatalen,  uchar *psDatabuf);


/**
 *@brief	关闭射频使卡失效。读写操作结束后应该执行该操作，相当于powerdown 。
 *@param	ucDelayms	等0则一直关闭RF;不等0则关闭ucDelayms毫秒后再打开RF。
 *                      关闭6～10ms可使卡失效。如果后续没有连续的读卡操作，应该置0以关闭RF 。
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_RfidPiccDeactivate(uchar ucDelayms);


/**
 *@brief	射频卡APDU交互,即读写卡过程(已激活的情况下))
 *@param	nSendlen		发送的命令长度
 *@param	psSendbuf		发送命令缓冲区
 *@retval 	pnRecvlen	接收数据长度
 *@retval	psRecebuf	接收数据缓冲区
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_RfidPiccApdu(int nSendlen, uchar *psSendbuf, int *pnRecvlen,  uchar *psRecebuf);


/**
 *@brief	M1寻卡(寻卡类型必须已经设置为TYPE-A)
 *@param	ucReqcode		0=请求REQA, 非0=唤醒WUPA, 一般情况下需要使用WUPA
 *@retval 	pnDatalen	 接收数据长度(2字节)
 *@retval	psDatabuf		接收数据缓冲区
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1Request(uchar ucReqcode, int *pnDatalen, uchar *psDatabuf);


/**
 *@brief	M1卡防冲突(NDK_RfidPiccDetect探测有卡的情况下)
 *@retval 	pnDatalen	接收数据长度(UID长度)
 *@retval	psDatabuf	接收数据缓冲区(UID)
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1Anti(int *pnDatalen, uchar *psDatabuf);


/**
*@brief		M1卡选卡(NDK_M1Anti成功的情况下)
*@param		nUidlen			uid长度
*@param		pnUidbuf	     uid数据缓冲区
*@retval 	psSakbuf	    选卡返回数据(1字节SAK)
*@return
*@li	NDK_OK				操作成功
*@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1Select(int nUidlen, uchar *pnUidbuf, uchar *psSakbuf);


/**
 *@brief	M1卡认证密钥存储(同一个密钥存储一次即可)
 *@param	ucKeytype		认证密钥类型 A=0x00 ；B=0x01
 *@param	ucKeynum	 密钥序列号(0~15)
 *@param	psKeydata		密钥,6字节
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1KeyStore(uchar ucKeytype,  uchar ucKeynum, uchar *psKeydata);


/**
 *@brief	M1卡装载已存储的密钥(同一个密钥加载一次即可)
 *@param	ucKeytype	认证密钥类型 A=0x00 ；B=0x01
 *@param    ucKeynum	 密钥序列号(0~15，A/B各有16个密钥)
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1KeyLoad(uchar ucKeytype,  uchar ucKeynum);


/**
 *@brief	M1卡用已加载的密钥认证
 *@param	nUidlen	uid长度
 *@param	psUidbuf	uid数据(NDK_M1Anti获取的)
 *@param	ucKeytype	认证密钥类型 A=0x00 ；B=0x01
 *@param    ucBlocknum	要认证的块号(注意:不是扇区号!)
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1InternalAuthen(int nUidlen, uchar *psUidbuf, uchar ucKeytype, uchar ucBlocknum);


/**
 *@brief	M1卡直接外带KEY认证
 *@param	nUidlen	uid长度
 *@param	psUidbuf	uid数据(NDK_M1Anti获取的)
 *@param	ucKeytype	认证密钥类型 A=0x00 ；B=0x01
 *@param	psKeydata	key(6字节)
 *@param    ucBlocknum	要认证的块号(注意:不是扇区号!)

 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1ExternalAuthen(int nUidlen, uchar *psUidbuf, uchar ucKeytype, uchar *psKeydata, uchar ucBlocknum);


/**
 *@brief	M1卡'块'读取操作
 *@param	ucBlocknum	要读的块号
 *@retval	pnDatalen	读取的块数据长度
 *@retval	psBlockdata	块数据
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1Read(uchar ucBlocknum, int *pnDatalen, uchar *psBlockdata);


/**
 *@brief	M1卡'块'写操作
 *@param	ucBlocknum	要写的块号
 *@param	pnDatalen	读取的块数据长度
 *@param	psBlockdata	块数据
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1Write(uchar ucBlocknum, int *pnDatalen, uchar *psBlockdata);


/**
 *@brief	M1卡'块'增量操作
 *@param	ucBlocknum	执行增量操作的块号。注意：增量操作只针对寄存器，未真正写入块数据区。另外，卡的块数据必须满足增/减量格式。
 *@param	nDatalen	增量数据长度(4字节)
 *@param	psDatabuf	增量数据
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1Increment(uchar ucBlocknum, int nDatalen, uchar *psDatabuf);


/**
 *@brief	M1卡'块'减量操作
 *@param	ucBlocknum	执行减量操作的块号。注意：减量操作只针对寄存器，未真正写入块数据区。另外，卡的块数据必须满足增/减量格式。
 *@param	nDanalen	增量数据长度(4字节)
 *@param	psDatabuf	增量数据
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1Decrement(uchar ucBlocknum, int nDanalen, uchar *psDatabuf);


/**
 *@brief	M1卡增/减量操作后的传送操作(寄存器值真正写入卡的块数据区)
 *@param	ucBlocknum	执行减量操作的块号
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1Transfer(uchar ucBlocknum);


/**
 *@brief	M1卡寄存器值恢复操作(恢复寄存器初始值，使之前的增/减量操作无效)
 *@param	ucBlocknum:   执行减量操作的块号
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_M1Restore(uchar ucBlocknum);


/**
*@brief	简易快速寻卡(用于测试等操作中加快速度)
*@param	nModecode	  =0正常寻卡；非0快速寻卡
*@return
*@li	NDK_OK				操作成功
*@li	其它EM_NDK_ERR		操作失败
*/
int NDK_PiccQuickRequest(int nModecode);


/**
 *@brief	屏蔽对ISO1443-4协议支持的判断
 *@param	nModecode	非0则执行屏蔽
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SetIgnoreProtocol(int nModecode);


/**
*@brief	读取屏蔽ISO1443-4协议支持的设置
*@retval	pnModecode	非0则执行屏蔽
*@return
*@li	NDK_OK				操作成功
*@li	其它EM_NDK_ERR		操作失败
*/
int NDK_GetIgnoreProtocol(int *pnModecode);


/**
 *@brief	读取射频接口芯片类型
 *@retval	pnRfidtype	见/ref EM_RFID "EM_RFID"
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int  NDK_GetRfidType(int *pnRfidtype);

/** @} */ // 射频卡模块结束

/** @addtogroup 系统接口
* @{
*/

/**
 *@brief  取POS版本和硬件信息的索引号
 *@details SYS_HWINFO_GET_POS_TYPE 返回的机型字符串格式为"NLGP-XXX"，如"NLGP-730"、"NLGP-8510"\n
		   SYS_HWINFO_GET_HARDWARE_INFO 返回硬件信息定义如下：
				 获取POS硬件信息每一个字节代表不同的模块\n
				 同一字节内不同的值代表同一模块的不同芯片类型\n
				 至少支持63个外部设备，应用传入参数数组不小于64 \n\n
				 参考\ref EM_SYS_HWTYPE "EM_SYS_HWTYPE"
				 
   pinfo[0]:无线模块\n
    0xFF     :没有无线模块\n
    0x01     :SIM300	\n
    0x02    :M72	\n	
    0x03    :DTGS800	\n
    0x04    :SM700	\n
    0x05    :MC39I	\n
    0x06    :DTM228C	\n
    0x07    :MC8331	\n
    0x08    :EM200	\n\n
   pinfo[1]:射频模块	\n
    0xFF     :没有射频模块	\n
    0x01     :RC531	\n
    0x02    :PN512	\n\n
   pinfo[2]:磁卡模块	\n
    0xFF     :没有磁卡模块	\n
    0x01     :giga	\n
    0x02     :mesh	\n\n
   pinfo[3]:扫描头模块	\n
    0xFF     :没有磁卡模块	\n
    0x01     :EM1300	\n
    0x02     :EM3000	\n
    0x03     :SE955	\n\n
   pinfo[4]:是否支持外接密码键盘	\n
    0xFF     :不支持	\n
    0x01     :支持	\n\n
   pinfo[5]:串口个数	\n
    0xFF     :无	\n
    0x01     :1个	\n
    0x02     :2个	\n\n
   pinfo[6]:是否支持USB	\n
    0xFF     :否	\n
    0x01     :是	\n\n
   pinfo[7]:MODEM模块	\n
    0xFF     :否	\n
    0x01     :是	\n\n
   pinfo[8]:wifi模块	\n
    0xFF     :无wifi模块	\n
    0x01     :是"USI WM-G-MR-09"模块	\n\n
   pinfo[9]:是否支持以太网	\n
    0xFF     :否	\n
    0x01     :是	\n\n
   pinfo[10]:打印模块	\n
    0xFF     :无打印模块	\n
    0x01~0x7F     :热敏	\n
    0x82~0x7E     :针打	\n\n
   pinfo[11]:是否支持触屏	\n
    0xFF     :否	\n
    0x01     :是	\n\n
   pinfo[12]:是否有射频LED灯	\n
    0xFF     :否	\n
    0x01     :是	\n
*/
typedef enum {
	SYS_HWINFO_GET_POS_TYPE=0,      		/**<取pos机器类型   			*/
	SYS_HWINFO_GET_HARDWARE_INFO,       /**<获取POS机上所支持硬件类型，详细返回值如上所诉*/
	SYS_HWINFO_GET_BIOS_VER,        		/**<取bios版本信息 			 */
	SYS_HWINFO_GET_POS_USN,        		/**<取机器序列号    		*/
	SYS_HWINFO_GET_POS_PSN,        		/**<取机器机器号    		*/
	SYS_HWINFO_GET_BOARD_VER,       		/**<取主板号        			*/
	SYS_HWINFO_GET_CREDITCARD_COUNT,		/**<取pos刷卡总数					*/
	SYS_HWINFO_GET_PRN_LEN,				/**<取pos打印总长度    		*/
	SYS_HWINFO_GET_POS_RUNTIME,          /**<取pos机开机运行时间  */
	SYS_HWINFO_GET_KEY_COUNT,            /**<取pos机按键次数  */
	SYS_HWINFO_GET_CPU_TYPE,
} EM_SYS_HWINFO;

/**
 *@brief  读取系统配置信息的索引号
*/
typedef enum {
	SYS_CONFIG_SLEEP_ENABLE,	    /**<休眠使能 0:禁止 1:启用 */
	SYS_CONFIG_SLEEP_TIME,      	/**<进入休眠时间前待机时间*/
	SYS_CONFIG_SLEEP_MODE,      	/**<休眠模式 1:浅休眠 2:深休眠*/
	SYS_CONFIG_LANGUAGE,			/**<获取系统语言 0:中文 1:english */
	SYS_CONFIG_APP_AUTORUN,      	/**<开机自动运行主控程序 0:禁用 1:启用*/	
} EM_SYS_CONFIG;

/**
 *@brief  硬件设备信息类型
*/
typedef enum {
	SYS_HWTYPE_WIRELESS_MODEM=0,	/**<无线modem */
	SYS_HWTYPE_RFID,				/**< 射频卡*/
	SYS_HWTYPE_MAG_CARD,			/**< 磁卡*/
	SYS_HWTYPE_SCANNER,				/**< 扫描头*/
	SYS_HWTYPE_PINPAD,				/**< 密码键盘*/
	SYS_HWTYPE_AUX,					/**< 串口*/
	SYS_HWTYPE_USB,					/**< USB*/
	SYS_HWTYPE_MODEM,				/**< modem*/
	SYS_HWTYPE_WIFI,				/**< wifi*/
	SYS_HWTYPE_ETHERNET,			/**< 以太网*/
	SYS_HWTYPE_PRINTER,				/**< 打印机*/
	SYS_HWTYPE_TOUCHSCREEN,			/**< 触摸屏*/
	SYS_HWTYPE_RFIDLED,				/**< 射频LED灯*/
	SYS_HWTYPE_MAX					/**< 最大值，只允许放在最后*/
} EM_SYS_HWTYPE;

/**
 *@brief	获取NDK库版本号
 *@retval   pszVer	版本号字符串,缓冲大小不低于8字节
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_Getlibver(char *pszVer);

/**
 *@brief 		系统初始化(暂未使用)
 *@details
 *@return
 *@li	 fd				操作成功
 *@li	其它EM_NDK_ERR		操作失败

*/
int NDK_SysInit(void);
/**
 *@brief 		POS重启
 *@details
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysReboot(void);
/**
 *@brief 		POS关机
 *@details
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysShutDown(void);
/**
 *@brief 		Beep只响一声，如果要连续响多声，可以在中间加延时
 *@details
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysBeep(void);
/**
 *@brief 		设置beep的音量
 *@details
 *@param    unVolumn    所要设置的音量的参数，参数范围为0~5，不设置底层默认为5
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysSetBeepVol(uint unVolumn);
/**
 *@brief 		取beep的音量
 *@details
 *@retval    punVolumn    所要设置的音量的参数
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysGetBeepVol(uint *punVolumn);
/**
 *@brief 		按一定的频率响一定的时间
 *@details
 *@param    unFrequency声音的频率，单位:Hz    ，范围为0 < unFrequency <=4000
 *@param    unSeconds声音持续的时间，单位:ms   ，范围为unSeconds > 0
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysTimeBeep(uint unFrequency,uint unSeconds);
/**
 *@brief 		设置是否允许自动进入休眠
 *@param    unFlag  0:不允许自动进入休眠，1:允许自动进入休眠，其他值参数不合法
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysSetSuspend(uint unFlag);
/**
 *@brief 		设置是否立即进入休眠
 *@details  设置是否自动进入休眠开关对此函数无影响。只要调用机器立即进入休眠
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysGoSuspend(void);
/**
 *@brief 		取电源电量
 *@retval   punVol  如果有插电源则为0，否则返回电池电量
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysGetPowerVol(uint *punVol);
/**
 *@brief 		单位延时( 单位时间为0.1s)
 *@details
 *@param    unDelayTime 延时时间，范围unDelayTime > 0
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysDelay(uint unDelayTime);
/**
 *@brief 		单位延时 (单位时间为0.1s)
 *@details
 *@param    unDelayTime 延时时间，范围unDelayTime > 0
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysMsDelay(uint unDelayTime);
/**
 *@brief 		取POS当前时间
 *@details
 *@param     pstTime  传入tm结构体类型的指针，返回当前pos时间
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysGetPosTime(struct tm *pstTime);

/**
 *@brief 		设置POS当前时间
 *@details
 *@param     stTime  传入tm结构体类型的变量，设置pos时间为变量time的时间
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysSetPosTime(struct tm stTime);

/**
 *@brief 		  读取指定字库的内容(接口暂定，未实现)
 *@details    根据偏移取指定字库的n个字节的内容
 *@param      pPath 字库所在的路径
 *@param      unOffset 字库偏移量
 *@param      unLen 要取字库字节数
 *@retval     psBuf 用于存储取出来的字库内容信息
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysReadFont(const char * pPath,uint unOffset,char *psBuf,uint unLen);
/**
 *@brief 		设置pos射频4掌灯的亮灭状态
 *@details
 *@param      nLed    nLed 的低4位表示4掌灯。由右到左分别是1、2、3、4。例如(nLed&(1 << 0)) !=0  的话表示操作第一掌灯
 *@param      nStatus  nStatus 的低8位表示4掌灯的状态。每两位为表示一掌灯的情况，由右到左分别是1、2、3、4。
 		     例如(nStatus&3)为第一掌灯要控制的状态。((nStatus>>2)&3)为第二掌灯。
 		     0表示状态不变，为1表示状态为亮，2为灭，3为闪。
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_LedStatus(int nLed,int nStatus);
/**
 *@brief 		启动跑表，开始计时
 *@details  由NDK_SysStartWatch()和NDK_SysStopWatch()配合使用。精度在1毫秒以内
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysStartWatch(void);
/**
 *@brief 		停止跑表并保存计数值
 *@details
 *@retval   punTime 跑表结束时的计数值
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysStopWatch(uint *punTime);
/**
 *@brief 	读取pos硬件信息接口
 *@details	如果传入的取硬件信息的索引emFlag不在范围内，则返回参数错误，如果没取到版本信息返回NDK_ERR
 			传入的参数的数组大小可暂定为100字节,最小不少于16字节。api只返回前100个字节的信息
 *@param    emFlag 所要读取设备信息的索引号
 *@retval   punLen 返回传回的buf信息的长度
 *@retval   psBuf用于存储返回的信息
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SysGetPosInfo(EM_SYS_HWINFO emFlag,uint *punLen,char *psBuf);

/**
 *@brief    读取系统配置信息
 *@param    emConfig 所要读取配置信息的索引号
 *@retval   pnvalue 返回的配置值
 *@return
 *@li	 NDK_OK				操作成功
 *@li	其它NDK_ERRCODE		操作失败
*/
int NDK_SysGetConfigInfo(EM_SYS_CONFIG emConfig,int *pnvalue);

/** @} */ // 系统接口结束

/** @addtogroup 工具
* @{
*/

/**
 *@brief	2个最大不超过12位的无符号数字串加法
 *@details	2个数字串逐次逐位相加，相加结果不能超过12位
 *@param	pszDigStr1		数字串1
 *@param	pszDigStr2		数字串2
 *@param	pnResultLen		结果缓冲区pszResult的大小
 *@retval	pszResult		相加和的数字串
 *@retval	pnResultLen		相加和的位数
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AddDigitStr(const uchar *pszDigStr1, const uchar *pszDigStr2, uchar* pszResult, int *pnResultLen );

/**
 *@brief	将6位数字串pcStrNum增加1后放回原值
 *@param	pszStrNum		需要被增加的数字串,缓冲区长度至少为7
 *@retval	pszStrNum		增加后的结果串
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_IncNum (uchar * pszStrNum );

/**
 *@brief	把带小数点的金额字符串转为不带小数点的金额字符串
 *@param	pszSource		待转换的金额字符串
 *@param	pnTargetLen		目标缓冲区的大小
 *@retval	pszTarget		转换后的字符串
 *@retval	pnTargetLen		转换后的字符串长度
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_FmtAmtStr (const uchar* pszSource, uchar* pszTarget, int* pnTargetLen );

/**
 *@brief	将AscII码的字符串转换成压缩的HEX格式
 *@details	非偶数长度的字符串根据对齐方式，采取左补0，右补F方式
 *@param	pszAsciiBuf		被转换的ASCII字符串
 *@param	nLen			输入数据长度(ASCII字符串的长度)
 *@param	ucType			对齐方式  0－左对齐  1－右对齐
 *@retval	psBcdBuf		转换输出的HEX数据
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AscToHex (const uchar* pszAsciiBuf, int nLen, uchar ucType, uchar* psBcdBuf);

/**
 *@brief	将HEX码数据转换成AscII码字符串
 *@param	psBcdBuf		被转换的Hex数据
 *@param	nLen			转换数据长度(ASCII字符串的长度)
 *@param	ucType			对齐方式  1－左对齐  0－右对齐
 *@retval	pszAsciiBuf		转换输出的AscII码数据
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_HexToAsc (const uchar* psBcdBuf, int nLen, uchar ucType, uchar* pszAsciiBuf);

/**
 *@brief	整型转换为4字节字符数组（高位在前）
 *@param	unNum		需要转换的整型数
 *@retval	psBuf		转换输出的字符串
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_IntToC4 (uchar* psBuf, uint unNum );

/**
 *@brief	整型转换为2字节字符数组（高位在前）
 *@param	unNum		需要转换的整型数
 *@retval	psBuf		转换输出的字符串
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_IntToC2(uchar* psBuf, uint unNum );

/**
 *@brief	4字节字符数组转换为整型（高位在前）
 *@param	psBuf		需要转换的字符串
 *@retval	unNum		转换输出的整型数
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_C4ToInt(uint* unNum, uchar* psBuf );

/**
 *@brief	2字节字符数组转换为整型（高位在前）
 *@details	psBuf长度要>=2
 *@param	psBuf		需要转换的字符串
 *@retval	unNum		转换输出的整型数
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_C2ToInt(uint *unNum, uchar *psBuf);

/**
 *@brief	整数(0-99)转换为一字节BCD
 *@param	nNum		需要转换的整型数(0-99)
 *@retval	ch			转换输出的一个BCD字符
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_ByteToBcd(int nNum, uchar *ch);

/**
 *@brief	一字节BCD转换为整数(0-99)
 *@param	ch		需要转换的BCD字符
 *@retval	pnNum	转换输出的整数值(0-99)
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_BcdToByte(uchar ch, int *pnNum);

/**
 *@brief	整数(0-9999)需要转换的整型数(0-9999)
 *@param	nNum		数字串1
 *@param	pnBcdLen	输出缓冲区的大小
 *@retval	pnBcdLen	转换后的BCD长度，如果成功此值，固定返回值为2
 *@retval	psBcd		转换输出的两字节BCD
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_IntToBcd(uchar *psBcd, int *pnBcdLen, int nNum);

/**
 *@brief	二字节BCD转换为整数(0-9999)
 *@details	psBcd长度应等于2
 *@param	psBcd		需要转换的两字节BCD
 *@retval	nNum		转换后的整数(0-9999)
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_BcdToInt(const uchar * psBcd, int *nNum);

/**
 *@brief	计算LRC
 *@details	psbuf缓冲的长度>nLen
 *@param	psBuf		需要计算LRC的字符串
 *@param	nLen		需要计算LRC的字符串的长度
 *@retval	ucLRC		计算得出的LRC
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_CalcLRC(const uchar *psBuf, int nLen, uchar *ucLRC);

/**
 *@brief	字符串去左空格
 *@param	pszBuf		存放字符串的缓冲区
 *@retval	pszBuf		去掉左空格后的字符串
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_LeftTrim(uchar *pszBuf);

/**
 *@brief	字符串去右空格
 *@param	pszBuf		存放字符串的缓冲区
 *@retval	pszBuf		去掉右空格后的字符串
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_RightTrim(uchar *pszBuf);

/**
 *@brief	字符串去左右空格
 *@param	pszBuf			存放字符串的缓冲区
 *@retval	pszBuf			去掉左右空格后的字符串
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AllTrim(uchar *pszBuf);

/**
 *@brief	往一字符串里加入某一字符使之长度为nLen
 *@details	pszString缓冲的长度应>nlen, 字符串的长度要小于nlen
 *@param	pszString		存放字符串的缓冲区
 *@param    nLen			字符串长度
 *@param	ch				所要加入的字符
 *@param	nOption			操作类型
                          	0    往字符串前面加字符
                          	1    往字符串后面加字符
                          	2    往字符串前后加字符
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_AddSymbolToStr(uchar *pszString, int nLen, uchar ch, int nOption);

/**
 *@brief	截取子串
 *@details	子串后带'\0'结束符
 *@param	pszSouStr		需要进行截取的字符串
 *@param	nStartPos		要截取子串的起始位置 字符串的位置由1开始计数
 *@param	nNum			要截取的字符数
 *@retval	pszObjStr		存放目标串的缓冲区
 *@retval	pnObjStrLen		子串的长度
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_SubStr(const uchar *pszSouStr, int nStartPos, int nNum, uchar *pszObjStr, int *pnObjStrLen);

/**
 *@brief	判断给定一字符是不是数字字符
 *@param	ch		需要判断的字符
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_IsDigitChar(uchar ch);

/**
 *@brief	测试一字串是否为纯数字串
 *@param	pszString		需要判断的字符串
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_IsDigitStr(const uchar *pszString);

/**
 *@brief	判断某年是否闰年
 *@param	nYear		年份
 *@return
 *@li	NDK_OK				操作成功(润年)
 *@li	其它EM_NDK_ERR		操作失败(非闰年)
*/
int NDK_IsLeapYear(int nYear);

/**
 *@brief	找出某年某月的最大天数
 *@param	nYear		年份
 *@param	nMon		月份
 *@retval	pnDays		该年份该月对应的最大天数
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_MonthDays(int nYear, int nMon, int *pnDays);

/**
 *@brief	判断提供的字符串是不是合法的日期格式串
 *@param	pszDate		日期格式字符串  格式为 YYYYMMDD
 *@return
 *@li	NDK_OK				操作成功
 *@li	其它EM_NDK_ERR		操作失败
*/
int NDK_IsValidDate(const uchar *pszDate);

/** @} */ // 工具模块结束

typedef struct{
	unsigned short  timeout;				//超时时间5~200秒
	unsigned short  size;					//这个结构的大小，后期扩展使用
	int  indexKey;					//密钥序号
	int  type;		//类型
	char * pPAN;					//主账号
	unsigned char *  pPINBlock;			//pinblock
	void *  pAD;					//附加数据，例如dukpt的ksn号
	int  sizeAD;					//附加数据长度
//	int  keySimulated ;
	int  nPINmismatch ;
	unsigned int  Exponent; 		//RSA公钥指数
	unsigned char  SWBytes [2] ;			//校验命令,例如“9000”
	int  PINBlockFormat; 	//pin block格式
	unsigned char *  pSCRC;				//校验代码
}sec_vpp_data;

int SDK_SecVPPEvent(int handle, int flag);
int SDK_SecVPPInit(int handle, sec_vpp_data stSecVppData);
#endif
/* End of this file */

