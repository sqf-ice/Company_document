#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <time.h>

#define FALSE 0
#define TRUE (!FALSE)

#define FAIL (-1)
#define SUCC 0
#define FSFAIL	(-1)
#define QUIT (-2)
#define FUNCQUIT (-4)  //功能键退出
#define FSSUCC		0

#define FRONT 0 //插入队列头（优先级最高）
#define TAIL  1 //插入队列尾（优先级最低）


#define MIN(a, b)	((a)>(b)?(b):(a))

#define nlmain main(int argc, char *argv[]) \
	{ int ret; config_init(); ret=_nlmain(); config_exit(); return ret;} int _nlmain

typedef short SOCKET;

extern int ErrorCode;
extern unsigned int g_language; /* 1 english, 0 chinese */

//功能键
#define  F1			0x01
#define  F2			0x02
#define  F3			0x03
#define  F4			0x04
#define  F5			0x05
#define  F6			0x06
#define	 F7			0x07
#define	 F8			0x08
#define	 F9			0x09
#define  LEFT		0x13
#define  RIGHT		0x14
#define  UP         0x11
#define  DOWN       0x12
#define  MENU       F3
//专用键
#define  BASP		0x0a    //backspace
#define	 BACKSPACE	BASP
#define  CR			0x0D	//确认键
#define  ENTER		CR
#define  ESC		0x1B
#define	 ZMK		0x1C	//字母键
#define  DOT		0x2E	//.
//数字键
#define ZERO		0x30
#define ONE			0x31
#define TWO			0x32
#define THREE		0x33
#define FOUR		0x34
#define FIVE		0x35
#define SIX			0x36
#define SEVEN		0x37
#define EIGHT		0x38
#define NINE		0x39
//新增ATM键
#define	 ATM1		((unsigned char)0xa1)
#define	 ATM2		((unsigned char)0xa2)
#define	 ATM3		((unsigned char)0xa3)
#define	 ATM4		((unsigned char)0xa4)
#define  ATM5		((unsigned char)0xa5)
#define	 ATM6		((unsigned char)0xa6)
#define  ATM7		((unsigned char)0xa7)
#define  ATM8		((unsigned char)0xa8)
//新增进纸退纸键
#define	 JZK		0xb1		//进纸键
#define	 TZK		0xb2		//退纸键

/********************** from posdef.h *********************/

/*         define keyboard value                   */
#define		KeyEsc          'c'		/* ESC 键 */
#define		KeyEnter        'e'		/* 回车键 */
#define		KeyLeft         F1		/* 左键'l'   *///20010827ZW
#define		KeyRight        F5		/* 右键'r'   *///20010827
#define		KeyBackspace    'b'		/* 退格键 */
#define		Key0            '0'
#define		Key1            '1'
#define		Key2            '2'
#define		Key3            '3'
#define		Key4            '4'
#define		Key5            '5'
#define		Key6            '6'
#define		Key7            '7'
#define		Key8            '8'
#define		Key9            '9'
#define		KeyS            '*'
#define		KeyR            '.'		/* '#' */
/**************** end of from posdef.h *******************/

/* key value */
#define BASP_VALUE		BASP
#define ENTER_VALUE		ENTER
#define ESC_VALUE		ESC
#define ZMK_VALUE		ZMK
#define F1_VALUE		F1
#define F2_VALUE		F2
#define F3_VALUE		F3
#define F4_VALUE		F4
#define F5_VALUE		F5
#define F6_VALUE		F6
#define F7_VALUE		F7
#define F8_VALUE		F8
#define F9_VALUE		F9
#define ATM1_VALUE		ATM1
#define ATM2_VALUE		ATM2
#define ATM3_VALUE		ATM3
#define ATM4_VALUE		ATM4
#define ATM5_VALUE		ATM5
#define ATM6_VALUE		ATM6
#define ATM7_VALUE		ATM7
#define ATM8_VALUE		ATM8
#define FORWARD_VALUE		JZK
#define BACKWARD_VALUE		TZK

#define ISCHAR(kb)	(kb.mode & 1)
#define ALTCHAR(kb)	(kb.mode & 0x80)

struct event {
    unsigned char type;		/* EVENT_* */
    union {
        struct {
            unsigned char code;	/* key code for KBD event */
            unsigned char value;/* char value */
            unsigned char mode;	/*  bit0: 1, printable key; 0, other key
                                 *  bit7: 1, should replace the former char
                                 */
        } kb;
    } data;
    void * priv;
};


//为了保持与其他机具兼容IME_BIHUA
#define  ENGLISH IME_ENGLISH	//英文
#define  PINYIN	IME_NUMPY// 全拼
#define  QUWEI	IME_QUWEI	// 区位
#define  NUMPY	IME_NUMPY	//数拼
#define  BIHUA  IME_BIHUA
#define IME_LAST_ONE -1		//和前次一样

#define EVENT_KBD	1
#define EVENT_TIMER     2
#define EVENT_MAGCARD	3

/* from bioscfg.h in NL8200 */

#define LCD_MAX_COL 240 /* Available values of column coordinate: 0～239 */
#define LCD_MAX_ROW 320 /* Available values of row coordinate: 0～319 */
#define MAXCHARPERLINE 30

#define MAXDISPLINE		36	//NL-GP730界面允许8*8字符显示的最大行数,19,20行系统保留作为状态栏显示
#define NORMAL_TEXT		0
#define REVERSE_TEXT	1

#define FONT_DOT_24 1
#define FONT_DOT_16 0


enum COLORS {
    WHITE,
    BLACK
};

enum fill_patterns {/* Fill patterns for rectangle and circle */
    NO_FILL,        /* no fill */
    SOLID_FILL      /* fills area in solid fill color */
};

//点阵旋转方向
enum ROTATE {
    ROTATE_0,ROTATE_90,ROTATE_180,ROTATE_270
};

#define TYPE_PIC  0 //图型点阵输出
#define TYPE_DOT  1//字库点阵输出

//点阵旋转缓冲输出
typedef struct {
    char* buf;
    int xsize;
    int ysize;
} TYPEDOTMATRIX;

#define AUX1 0
#define AUX2 1
#define AUX3 2

#define AUX_WIRELESS 	AUX3		/*无线接口*/
//#define AUX_PINPAD 		AUX2			/*密码键盘接口*/

#define RECVBUF	1
#define SENDBUF	0

#define		BPS300		0
#define		BPS1200		1
#define		BPS2400		2
#define		BPS4800		3
#define		BPS7200		4
#define		BPS9600		5
#define		BPS19200		6
#define		BPS38400		7
#define		BPS57600		8
//#define		BPS76800		9//所有版本BIOS不支持该波特率
#define		BPS115200		10

#define	DB8		0xc0		//8位数据位选择
#define	DB7		0x40		//7位数据位选择
#define	DB6		0x80		//6位数据位选择
#define	DB5		0x00		//5位数据位选择
#define	STOP2		0x20		//2位停止位
#define	STOP15		0x10		//1.5位停止位
#define	STOP1		0x08		//1位停止位
#define	NP		0x04		//	无校验
#define	EP		0x02		//	偶校验
#define	OP		0x01		//	奇校验

#define PARAERR	(0x1FF)

#define SOH     1
#define STX     2
#define ETX     3
#define EOT     4
#define ENQ     5
#define ACK     6
#define DLE     0x10
#define NAK     0x15
#define ETB     0x17

#define NORMAL_DENSITY	(0<<0)		//普通
#define HIGH_DENSITY	(1<<0)		//高密

#define  IC1    0				/*第一个IC卡*/
#define  IC2    1				/*第二个IC卡*/
#define  SIM1  0x80				/*第一个SIM卡*/
#define  SIM2  0x81				/*第二个SIM卡*/
#define  SIM3  0x82				/*第三个SIM卡*/
#define  SIM4  0x83				/*第四个SIM卡*/
#define  SAM1  0x80				/*第一个SIM卡*/
#define  SAM2  0x81				/*第二个SIM卡*/
#define  SAM3  0x82				/*第三个SIM卡*/
#define  SAM4  0x83				/*第四个SIM卡*/
#define  ESAM  0x83				/*第四个SIM卡*/
#define  MIFARE		0xa1		/*射频卡*/

#define  IC_0	0
#define  IC_1	0x01		//IC卡选择
#define  TK2    0x02		/*只读二道*/
#define  TK3    0x04		/*只读三道*/
#define  TK2_3    	(TK2 | TK3)   	/* 0x06  同时读二三道*/
#define	 TK1		0x01	//只读一磁道
#define	 TK1_2_3	( TK1 | TK2 | TK3 )
#define	 TK1_2		( TK1 | TK2 )
#define  TK1_3		( TK1 | TK3 )
#define  MAGCARD	 0x01		/*刷磁卡*/
#define	 STRIPE		0x01		/*刷磁卡*/
#define  INSERTIC  	0x02		/*插入IC卡*/
#define  KEYIN	0x04			/*键入卡号*/
#define	 JUSTRETURN 	0    	/*标志函数超过等待时间后返回*/
#define  STANDBY 		1		/*标志函数超过等待时间后休眠*/
#define  TIMEOUT 		(-3)	/*超时返回*/

#define	EM3K_TRIG	(1<<2)	//扫描头TRIG管脚,用于控制启动扫描
#define	EM3K_WKUP	(1<<5)	//扫描头唤醒管脚


/*****************************************************************************
 * 下面是打印模块相关的信息 *******************************************************
 *****************************************************************************/
/********************
 * 打印字体信息        *
 ********************/
//for printnew.c
#define DOT8x16  1
#define DOT16x16 2
#define DOT16x32 3
#define DOT24x32 4
#define DOT6x8   5
#define DOT8x8   6
#define DOT5x7      7
#define DOT5x16     8
#define DOT10x16    9
#define DOT10x8     10
/*以下为新增点阵字体--lingb. 20100809*/
#define DOT12x16A	11	/*MSGothic粗体12x16点阵*/
#define DOT12x24A	12	/*Gulimche字体12x24点阵*/
#define DOT16x32A	13	/*MSGothic粗体16x32点阵*/
#define DOT12x16B	14	/*MSGothic粗体12x16点阵*/
#define DOT12x24B	15	/*MSGothic粗体12x24点阵*/
#define DOT16x32B	16	/*MSGothic粗体16x32点阵*/
#define DOT12x16C	17	/*楷体粗体12x16点阵*/
#define DOT12x24C	18	/*楷体粗体12x24点阵*/
#define DOT16x32C	19	/*楷体粗体16x32点阵*/

#define ASCFONTNUM  19   /**! !Attention: 添加新字体时，需要修改该参数 */

#define HZ24x24  1
#define HZ16x32  2
#define HZ32x32  3
#define HZ32x16  4
#define HZ24x32  5
#define HZ16x16  6
#define HZ12x16  7
#define HZ16x8   8
/*以下新增三种24x24汉字字库--lingb. 20100809*/
#define HZ24x24A  9		/*自有宋体24x24点阵*/
#define HZ24x24B  10		/*仿宋24x24点阵*/
#define HZ24x24C  11		/*楷体24x24点阵*/
#define HZ24x24USER  12		/*用户自定义24x24点阵*/


#define HZFONTNUM  12    /**! !Attention：添加新字体时，需要修改该参数 */


/**********************
 * 打印设备错误信息       *
 **********************/
#define PRNOPAPER       (1<<1)  //< 打印机缺纸，提示请装纸。
#define PRISHOT         (1<<2)  //< 打印机过热
#define PRNOTREADY      (1<<3)  //< 打印机忙，一般打印机正在打印时会返回该值
#define PRINTOVER       (1<<8)  //< 打印结束，如果在上一次打印完毕后还未调用

#define TP_ERR_VOLT_LOW		(1<<4)	//低电压保护
#define TP_ERR_VOLT_WARN	(1<<5)	//低电压提示
#define TP_ERR_VOLT_TOP		(1<<6)	//高电压保护
#define PRNOPOWER			(TP_ERR_VOLT_LOW|TP_ERR_VOLT_WARN|TP_ERR_VOLT_TOP)
// print或printimage这两个函数之一始终返回该值
#define PRUNKOWNSTATUS  (-16) //< 打印驱动返回未定义错误，亦属严重出错
#define PRHPInvalid     (1<<9) //< 打印机失步，必须重新打印(可能是打印机中有纸屑)
#define PRQUIT          (-2)
#define PROK            0     //< 打印机状态良好，无需烦心。

/**********************
 * 兼容原有代码
 *********************/
#define TPNOPAPER       PRNOPAPER
#define TPISHOT         PRISHOT
#define TPNOTREADY      PRNOTREADY
#define TPPRINTOVER     PRINTOVER
#define TPNOPOWER       PRNOPOWER
#define TPUNKOWNSTATUS  PRUNKOWNSTATUS
#define TPHPInvalid     PRHPInvalid
#define TPQUIT          PRQUIT
#define TPOK            PROK

#define IPNOPAPER       PRNOPAPER
#define IPISHOT         PRISHOT
#define IPNOTREADY      PRNOTREADY
#define IPPRINTOVER     PRINTOVER
#define IPNOPOWER       PRNOPOWER
#define IPUNKOWNSTATUS  PRUNKOWNSTATUS
#define IPHPInvalid     PRHPInvalid
#define IPQUIT          PRQUIT
#define IPOK            PROK


/**********************
 * 打印机类型信息         *
 **********************/
enum PRINTTYPE_DEF {
    PRINTTYPE_TP=0,             //< 热敏打印机
    PRINTTYPE_HIP,              //< 穿孔针打
    PRINTTYPE_FIP,              //< 摩擦针打
    PRINTTYPE_END               //< 无
};


/***********************
 * 打印模式信息            *
 ***********************/
#define IPMODE_NORMAL_DOUBLE    0       //< 纵向正常双向
#define IPMODE_NORMAL_SINGLE    1       //< 纵向正常单向
#define IPMODE_COMPRESS_DOUBLE  2       //< 纵向压缩双向
#define IPMODE_COMPRESS_SINGLE  3       //< 纵向压缩单向
#define IPMODE_NORMAL_DOUBLE_ROW    4   //< 纵向正常双向－横向压缩
#define IPMODE_NORMAL_SINGLE_ROW    5   //< 纵向正常单向－横向压缩
#define IPMODE_COMPRESS_DOUBLE_ROW  6   //< 纵向压缩双向－横向压缩
#define IPMODE_COMPRESS_SINGLE_ROW  7   //< 纵向压缩单向－横向压缩
#define IPMODE_MAX_MODE     8           //< ?


/***********************
 * 文字图像混合排版模式*
 ***********************/
typedef enum PRN_TYPESETTING {
    TPSET_AUTO = 0,		//<自动适应，文字环绕图像，保证不重合打印
    TPSET_TEXTUP,		//<文字在上，若出现重合，文件将直接覆写在图像上
    TPSET_PICUP,		//<图像在上，若出现重合，图像将直接覆写在文件上
    TPSET_MIX			//<文字图像嵌套，若重合，文字和图像将嵌套打印
} PRN_TYPESETTING;

/*****************************************************************************
 * 打印模块信息结束 *************************************************************
 *****************************************************************************/

//esimkey.c
#define ENCRYPT  0
#define DISCRYPT 1
#define MAINKEYTYPE 0x35
#define WORKKEYTYPE 0x33

#define NORMAL 1
#define PASSWD 0
#define	   ENCRYPT		0		/*加密*/
#define    DECRYPT		1		/*解密*/

#define CONNECT_AFTERPREDIAL	2	//已连接
#define OK_AFTERPREDIAL			1	//拨号成功
#define NORETURN_AFTERPREDIAL	0	//连接中或拨号无返回
#define NOPREDIAL				(-11)//并未拨号,或SDLC链接已终止,期待再次拨号
#define MS_NODIALTONE	(-2)		//没有检测到拨号音
#define MS_NOCARRIER	(-3)  		//没有接收到载波或载波丢失
#define MS_BUSY			(-4)		//检测到忙音或电话线没插
#define MS_ERROR		(-5)		//命令行出错
#define LINE_VOL_TOO_LOW	(-8)		//存在并机，（有分机在使用电话线路）需要继续检测线压VB202M
//#define getch() getkeycode(0)

//ppp的状态值
typedef enum {
    PHASE_DEAD = 0,			//ppp关闭
    PHASE_INITIALIZE,		//PPP正在初始化
    PHASE_ESTABLISH,		//PPP连接稳定但是尚未获得IP地址
    PHASE_AUTHENTICATE,		//PPP正在进行用户名及密码的认证
    PHASE_CALLBACK,			//PPP回复呼叫
    PHASE_NETWORK,			//PPP进入INTERNET网并获得IP地址可以进行TCPIP连接
    PHASE_TERMINATE			//ppp中断中
} LinkPhase;

// 存放于ErrorCode的系统级错误码	//用于磁卡刷卡成功判断
enum SYS_ERRORCODE {
    MAGCARD_TK1_LRC_ERR=0xC01,										// 一道LRC校验失败
    MAGCARD_TK2_LRC_ERR=0xC02,										// 二道LRC校验失败
    MAGCARD_TK3_LRC_ERR=0xC08,										// 三道LRC校验失败
    MAGCARD_TK12_LRC_ERR=MAGCARD_TK1_LRC_ERR|MAGCARD_TK2_LRC_ERR,	// 一、二道LRC校验失败
    MAGCARD_TK23_LRC_ERR=MAGCARD_TK2_LRC_ERR|MAGCARD_TK3_LRC_ERR,	// 二、三道LRC校验失败
    MAGCARD_TK13_LRC_ERR=MAGCARD_TK1_LRC_ERR|MAGCARD_TK3_LRC_ERR,	// 一、三道LRC校验失败
    MAGCARD_TK123_LRC_ERR
    =MAGCARD_TK1_LRC_ERR|MAGCARD_TK2_LRC_ERR|MAGCARD_TK3_LRC_ERR	// 一、二、三道LRC校验失败
};

enum PPP_DEVICE_TYPE {
    NET_DEV_NULL=0,
    PPP_DEV_WLM,	//无线设备,如GPRS CDMA ,他们操作串口1
    PPP_DEV_WM	,		//有线MODEM
    NET_DEV_ETH,		//以太网
    NET_DEV_END   //用于判断设置的值的范围
};

//无线模块类型
enum WLMTYPE {
    WLM_NONE=0x00,
    WLM_CDMA_XXXX=0x80,
    WLM_GPRS_MC39I=0x02,
    WLM_GPRS_SM300=0x03,
    WLM_GPRS_SM700=0x05,
    WLM_CDMA_DTGS800=0x81,
    WLM_CDMA_DTM228C=0x82,
    WLM_CDMA_MC8331=0x83,
    WLM_CDMA_EM200=0x84,
	WLM_GPRS_M72=0x06
};

/* clock */
#define USE_HW_CLOCK	1	/* use hardware rtc */

enum CLOCK_STYLE {
    RTC_TIME_ONLY,
    RTC_DATE_TIME_ONE_LINE,
    RTC_DATE_TIME_TWO_LINES,
};

struct postime {
    char		yearh;      /*  年份的高字节 BCD表示*/
    char		yearl;		/*  年份的低字节 BCD表示*/
    char		month;		/*  月份	 BCD表示1--12*/
    char		day;		/*  日		 BCD表示1--31*/
    char		week;		/*	0--6对应星期日--星期六 */
    char		hour;		/*  小时	 BCD表示0--23*/
    char		minute;		/*	分	 BCD表示0--59*/
    char		second;		/*	秒	 BCD表示0--59*/
};

#define ESIMtableAddress		0
/* end of bioscfg.h in NL8200*/

/*****************************************************************************
* PPP TCP错误代码定义
*****************************************************************************/
#define PPPERR_PARAM 	-1		/* Invalid parameter. */
#define PPPERR_OPEN 	-2		/* Unable to open PPP session. */
#define PPPERR_DEVICE 	-3		/* Invalid I/O device for PPP. */
#define PPPERR_ALLOC 	-4		/* Unable to allocate resources. */
#define PPPERR_USER 	-5		/* User interrupt. */
#define PPPERR_CONNECT 	-6		/* Connection lost. */
#define PPPERR_AUTHFAIL -7		/* Failed authentication challenge. */
#define PPPERR_PROTOCOL -8		/* Failed to meet protocol. */
#define PPPERR_TASKCREATE -9	/* Failed to create pppMain task, added by huly */

#define TCPERR_EOF 		-1		/* End of data. */
#define TCPERR_ALLOC 	-2		/* Unable to allocate a control block. */
#define TCPERR_PARAM 	-3		/* Invalid parameters. */
#define TCPERR_INVADDR 	-4		/* Invalid address. */
#define TCPERR_CONFIG 	-5		/* Invalid configuration. */
#define TCPERR_CONNECT 	-6		/* No connection. */
#define TCPERR_RESET 	-7		/* Connection reset received. */
#define TCPERR_TIMEOUT 	-8		/* Timeout on transmission. */
#define TCPERR_NETWORK 	-9		/* Network error - unreachable? */
#define TCPERR_PREC 	-10		/* IP Precedence error. */
#define TCPERR_PROTOCOL -11		/* Protocol error. */
#define TCPERR_PPPCLOSE -12		/* PPP已经关闭，导致TCP错误 */

#ifdef USE_MULTIAPP_V1
/*************************** multi app **************************************/
enum USERPRG_INFO_METHOD {
    UPIM_DESP,
    UPIM_SERINO
};

//用户程序信息
typedef struct tagUsrPrgInfo {
    char cSeriNo;				// 用户程序的编号
    char cDespBuf[17];
    char cReserved[2];				//预留
    unsigned int nChksum;
    unsigned int nStartAddress;
    int  len;
    time_t itime;
    char cVerBuf[16];
    int Reserved[6];			//预留
} USRPRGINFO;
#endif

#define MAX_APP_NUM		9

/* user path */
#define USERBINFS_CHARDEV			"/dev/mtd5"
#define USERBINFS_BLKDEV			"/dev/mtdblock5"
#define USERBINFS_ROOT  			"/appfs"
#define USERDATFS_CHARDEV			USERBINFS_CHARDEV
#define USERDATFS_BLKDEV			USERBINFS_BLKDEV
#define USERDATFS_ROOT  			USERBINFS_ROOT
#define USER_BIN_PATH		USERBINFS_ROOT"/apps/"
#define USER_CONFIG_PATH	USERBINFS_ROOT"/etc/"
#define USER_DATA_PATH		USERDATFS_ROOT"/data/"
#define USER_LIB_PATH		USERBINFS_ROOT"/lib/"
#define TMP_PATH					"/tmp/"

#ifdef	CONFIG_NLGP730_L1
#define TMP_UPDATE_PATH					TMP_PATH"update/"
#endif
#define USER_PROGRAM_ENTRY		USER_BIN_PATH"userapp"
#define MIYAO_PROGRAM_ENTRY		USER_BIN_PATH"miyaoapp"
#define MULTI_USERAPP_FORMAT	"userapp%d"

#define MULTIAPP_CONFIG			USER_CONFIG_PATH"multiapp_v2.ini"

#ifndef PATH_MAX
#define PATH_MAX	4096
#endif

#define MAX_APPNAME_LEN	 	32
#define MAX_VERSION_LEN		16
#define MAX_TIME_LEN	 	32

#define USERPRG_EXIST 		-2
#define USERPRG_NOT_EXIST 	-3

/**
 * 新增下载类型定义，用于配合PCI送检方案中的下载认证密钥、应用固件加解密密钥使用。
 * 同时保留国内现有的传统下载方案，即使用DFT_NORMAL_XXX类型
 */
enum _DOWNLOAD_FILE_TYPE {
	DFT_NORMAL_APP = 0,		/*国内普通应用下载类型*/
	DFT_NORMAL_FIRMWAR,		/*国内普通固件下载类型*/
	DFT_PCI_APP = 0x10,			/*PCI版本的应用下载类型*/
	DFT_PCI_FIRMWARE,		/*PCI版本的固件下载类型*/
};

/*多应用定义*/
typedef struct tagV2USRPRGINFO {
    char 			cAppName[MAX_APPNAME_LEN + 1];
    char			cVerBuf[MAX_VERSION_LEN + 1];											/*用户程序版本信息					*/
    int				cSeriNo;													/*编号,V2版保留,但意义不同	*/
//	char			sHeaderVer[4];										/*文件头版本								*/
//	unsigned int	uiFileLen;										/* 带重定位信息文件长度			*/
//	time_t			tCreateTime;										/* 文件创建时间							*/
    char			tBuildTime[MAX_TIME_LEN + 1];											/* 文件编译时间							*/
    int				bIsMaster;
//	unsigned long    uiInfoBase;					/*文件信息基地址		*/
    unsigned long	sReverse[3];				/*保留域*/
} V2USRPRGINFO;																/*应用信息结构，开放给应用层*/

/************************************************************************/
/* V2版远程升级命令结构	                                                */
/************************************************************************/
typedef struct tagLocalCMDNODE {
    char			sAppName[17];					/*应用程序名称	16										*/
    char			sFileName[13];				/*应用程序文件名12										*/
    char			cType;								/*应用类型														*/
    unsigned char   ucCmd;					/*命令																*/
    unsigned int	uiReverse[2];			/*保留																*/
} UPDATECMD;												/*远程下载中应用层传入的数据结构			*/

/*更新的处理命令*/
enum {
    CMD_UPDATE=0,									/*更新																	*/
    CMD_DELETE=1,									/*删除																	*/
    CMD_INSERT=3,									/*添加																	*/
    CMD_RESERVE=2									/*保留																	*/
};															/*应用程序处理类型											*/


/************************************************************************/
/* 应用程序类型					                                                */
/************************************************************************/
enum {
    APP_SLAVE=0XFF,								/*子应用																*/
    APP_MASTER=0XFA,							/*主控																	*/
    APP_LIB=0XF6									/*应用库																*/
};

struct pos_info {
    char cpu_ver[16];
    unsigned int fclk, hclk, sdclk;			//单位：MHZ
    int  sdram_size, nor_size, nand_size;	//单位：sdram_size(KB) nor_size, nand_size(MB)
//	unsigned char hwconfig;
};

enum SLEEP_LEVEL {NO_SLEEP, MID_SLEEP, DEEP_SLEEP};

#define WLM_SQ_LIMIT_DEFAULT	8			//信号值缺省判断
#define WLM_SQ_APP_LIMIT		15			//应用判断的限制值

#define NET_EN_CFG
//208版为了支持取单个硬件信息进行了调整，初始值由1升到2
#define NO_MCPU_VER		0x02	//主CPU软件版本
#define NO_SCPU_VER		0x03	//辅CPU版本
#define NO_MODEM_VER	0x04	//MODEM硬件版本
#define NO_FRAM_VER		0x05	//FRAM器件类型
#define NO_PRINTER_VER	0x06	//打印驱动类型
#define NO_CPLD_VER		0x07	//CPLD版本
#ifdef NET_EN_CFG
#define NO_NET_VER		0x08	//网络设备类型及版本
#endif
#define NO_LCD_VER		0x09	//液晶类型
#define NO_POS_TYPE		0x0A	//POS的型号，如8200、8300
#define NO_POS_SN		0x0B	//POS的产品序列号
#define NO_POS_OWNER	0x0C	//POS的产权信息

#define NO_DISK_SPACE      0x0D    //电子盘剩余空间
#define NO_USER_SPACE      0X0E    //应用程序使用空间
#define NO_SRAM_SPACE      0x0f    //内存剩余空间
#define NO_KEY_COUNT       0x10    //按键次数
#define NO_CARD_COUNT      0x11    //累计刷卡次数统计
#define NO_MODEMTIME_TOTAL 0X12   //累计有线modem通讯在线时间
#define NO_PPPTIME_TOTAL   0X13   //累计PPP通讯在线时间
#define NO_TCPTIME_TOTAL   0x14   //累计TCP通讯在线时间
#define NO_PRINT_LEN       0X15    //累计打印公里数
#define NO_SYSTEM_ERROR    0X16    //异常信息
#define NO_POWERON_TIME    0x17    //开机及运行时间
#define NO_MAGCARD_TYPE    0x18    //获取磁头的版本类型
#define NO_SRAM_SIZE    0X20      //SRAM大小
#define NO_HDR_VER_END	0xff
#define NO_SIMPLEHDR_START	NO_MCPU_VER
#define NO_SIMPLEHDR_END	NO_SRAM_SIZE


#endif
