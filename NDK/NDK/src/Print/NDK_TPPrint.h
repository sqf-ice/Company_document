#ifndef _NDK_TPPRINT_H_
#define _NDK_TPPRINT_H_

#include <linux/ioctl.h>
#include "NDK.h"
#include "NDK_debug.h"

#define PRN_DEV_NAME	"/dev/prn"  /*    打印设备名  */
#define prnMajor		10          /*打印主设备号*/
#define PRN_IOC_MAGIC	prnMajor

#define MAXHIGHT 		48
#define MAXLINEDOT		384
#define MaxFontNum 		8
#define OpenLine  	0     /*开下划线*/
#define CloseLine		1	/*关下划线*/
#define UnableLine 	2	/*关闭下划线功能*/

#define LEFTALIGN	 0	/*左对齐*/
#define MIDALIGN	 1	/*居中对齐*/
#define RIGHTALIGN	 2	/*右对齐*/
#define UNABLEALIGN	 3	/*关闭对齐模式*/

#define	PRN_ERR_OK 		 0		/* 0 错误代码,定义为打印机正常*/
#define	PRN_ERR_INIT		(1<<7)  // 1 错误代码,定义为打印机上电复位失败
#define	PRN_ERR_PAPER	    (1<<1)  // 2 错误代码,定义为打印机缺纸
#define	PRN_ERR_HEAT		(1<<2)  // 4 错误代码,定义为打印头温度过高
#define	PRN_ERR_BUSY		(1<<3)  // 8 返回码,定义为打印机忙(正在打印操作).
#define	PRN_ERR_DONE		(1<<8)  // 返回码,定义为上次打印任务成功完成
#define	PRN_ERR_HPInvalid	(1<<9)  // 0x20 错误代码,定义为打印过程中HP位置异常.卡纸或者异物插入打印机头部都将导致该错误
#define TP_ERR_VOLT_LOW	(1<<4)	//低电压保护
#define TP_ERR_VOLT_WARN	(1<<5)	//低电压提示
#define TP_ERR_VOLT_TOP	(1<<6)	//高电压保护
#define TP_ERR_VOLT			(TP_ERR_VOLT_LOW|TP_ERR_VOLT_WARN|TP_ERR_VOLT_TOP)


/*****************************************************************************
 * 下面是打印模块相关的信息 *******************************************************
 *****************************************************************************/
/********************
 * 打印字体信息        *
 ********************/

#define HZFONTNUM  		22    /**! !Attention：添加新字体时，需要修改该参数 */
#define ASCFONTNUM  	27   /**! !Attention: 添加新字体时，需要修改该参数 */

#define PRN_IOCT_RESET				_IO(PRN_IOC_MAGIC, 1)		// param is 	null
#define PRN_IOCG_INFO				_IO(PRN_IOC_MAGIC, 2)		// prninfo,
#define PRN_IOCG_STATUS			_IO(PRN_IOC_MAGIC, 3)		// int

#define PRN_IOCG_MODE				_IO(PRN_IOC_MAGIC, 4)       	// int
#define PRN_IOCS_MODE				_IO(PRN_IOC_MAGIC, 5)		// int

#define PRN_IOCT_FEED_PAPER		_IO(PRN_IOC_MAGIC, 6)		// int
#define PRN_IOCS_REPORT			_IO(PRN_IOC_MAGIC, 7)		// prnreport,
#define PRN_IOCT_IMAGE				_IO(PRN_IOC_MAGIC, 8)		// prnimage

#define PRN_IOCG_PARAM				_IO(PRN_IOC_MAGIC, 9)		// get prnparam
#define PRN_IOCS_PARAM				_IO(PRN_IOC_MAGIC, 10)		// set prnparam

#define PRN_IOCG_ERRNO				_IO(PRN_IOC_MAGIC, 11)		// int

#define PRN_IOCS_B_ADDON			_IO(PRN_IOC_MAGIC, 13)		// int
#define PRN_IOCG_B_ADDON			_IO(PRN_IOC_MAGIC, 14)		// int

#define PRN_IOCS_MISSSTEPDETECT 	_IO(PRN_IOC_MAGIC, 15)      	// int
#define PRN_IOCG_MISSSTEPDETECT 	_IO(PRN_IOC_MAGIC, 16)      	// int
#define PRN_IOCS_SPEED				_IO(PRN_IOC_MAGIC, 17)
#define PRN_IOCT_CLRBUF         		_IO(PRN_IOC_MAGIC, 20)      	// param is 	null

#define PRN_IOCG_PRINTER_TYPE   	_IO(PRN_IOC_MAGIC, 30)      	// int

#define PRN_IOCT_TEST				_IO(PRN_IOC_MAGIC, 101)	    	// null
#define PRN_IOCT_INTEGRATE			_IO(PRN_IOC_MAGIC, 102)
#define PRN_IOCS_GREYSCALE			_IO(PRN_IOC_MAGIC, 103)
#define PRN_IOCG_ISPRINTERBUSY		_IO(PRN_IOC_MAGIC, 104)
#define PRN_IOCS_HEATMODE			_IO(PRN_IOC_MAGIC, 105)		//设置加热模式:加热优先或速度优先
#define PRN_IOCS_MOTORSTEPMODE	_IO(PRN_IOC_MAGIC, 106)		//设置马达步进模式: 8相驱动或4相驱动
#define PRN_IOCG_MOTORSTEPCOUNT	_IO(PRN_IOC_MAGIC, 107)		//获取马达的总共步进计数，用于计数走纸的总长度

#define PRN_IOCG_VER				_IO(PRN_IOC_MAGIC, 108)
#define PRN_IOCS_STOP				_IO(PRN_IOC_MAGIC, 109)
#define PRN_IOCS_PRNRANGE			_IO(PRN_IOC_MAGIC, 110)	  /*左边界,字间距,行间距设定*/
#define PRN_IOCS_ALIGNTYPE			_IO(PRN_IOC_MAGIC, 111)	 /*图形对齐方式*/
#define PRN_IOCS_FEEDTP			_IO(PRN_IOC_MAGIC, 112)
#define PRN_IOCT_DATA				_IO(PRN_IOC_MAGIC, 113)		// prndata
#define PRN_IOCT_DATA_NDK  		_IO(PRN_IOC_MAGIC, 117)		/*ndk送数据接口*/
#define PRN_IOCS_FEEDENDPRINT		_IO(PRN_IOC_MAGIC, 121)		/*打印完走纸到撕纸处*/
#define PRN_IOCS_FEEDBACK		      _IO(PRN_IOC_MAGIC, 122)		/*退纸*/


/***********************
 * 文字图像混合排版模式*
 ***********************/
typedef enum PRN_TYPESETTING {
    TPSET_AUTO = 0,		//<自动适应，文字环绕图像，保证不重合打印
    TPSET_TEXTUP,		//<文字在上，若出现重合，文件将直接覆写在图像上
    TPSET_PICUP,		//<图像在上，若出现重合，图像将直接覆写在文件上
    TPSET_MIX			//<文字图像嵌套，若重合，文字和图像将嵌套打印
} PRN_TYPESETTING;


/**
 * @brief 要打印的图像信息
 */
typedef struct {
	int  leftmargin;			/* 图形的左上角的列位置（以点为单位）*/
	int  width;				/* 图形的宽度（以点为单位）*/
	int  height;				/* 图形的高度（以点为单位）*/
	void *buffer;				/* 图象点阵数据,为横向排列*/
} prnimage;

/**
 * @brief 要打印的文字加图像整合的信息结构
 */
typedef struct PrinterIntegrate {
	char *pstrbuf;					/*文字字符串指针*/
	int nstrlen;						/*文字长度*/
	int ImageYPos;					/*图像开始打印的纵坐标*/
	prnimage sPrnImage;				/*与文字一同打印的位图信息*/
	PRN_TYPESETTING TypeSetting;	/*排版模式*/
} PrinterIntegrate;

/**
 * @brief 打印机参数
 */
typedef struct {
	int charset;			/* 字符集*/
	int line_attr;			/*行属性*/
	int ASCfont;			/* ASC字体*/
	int tabwidth;			/* tab值*/
	int pagelenth;		/* 页长*/
	int linespace;		/* 行间距*/
	int wordspace;		/* 字间距  （TODO:未测试）*/
	int BMcounter;		/* 黑标计数（TODO:未实现）*/
	int heat_time;		/*加热时间*/
} prnparam;


/**
 * @brief  打印行缓冲区数据结构，存储1点行的打印内容
*/
typedef	struct {
	unsigned char ucData[MAXHIGHT][MAXLINEDOT/8];	//打印数据队列元素的数据
	unsigned int  usPrnSteps;	//当前行打印结束之后额外的步进数，用于实现行间距
	unsigned int  usPrnLineHigh;
	unsigned int  usPrnLineOffset;
	unsigned int  usmode;
	unsigned int  usPrnGray;

}PrnDotlineData;

/**
 * @brief  打印图片信息的数据结构
*/

typedef struct {
	unsigned int width;  			/**< 图像宽度（像素） */
	unsigned int height; 			/**< 图像高度（像素）*/
	unsigned int bytes_per_pixel; /**< 每个像素多少字节 */
	void * image_buf;		/**< 图像色彩数据 */
	void * image_alpha;		/**< 图像alpha通道 */
	int    user;			/**< 图像被共享的次数 */
}image_t;
/**
 * @brief  打印图片信息的数据结构
*/
typedef struct {
	unsigned int width;  			/**< 图像宽度（像素） */
	unsigned int height; 			/**< 图像高度（像素）*/
	void * image_buf;		/**< 图像色彩数据 */
}print_buf;

/**
 * @brief 新增字体数据结构
*/
typedef	struct stFontContrl{
	int fonttype;			/*对应的字体类型，例如 HZ24x24A */
	char *fontname;		/*该字体在文件系统中的字库文件完整路径*/
	int fthandle;			/*字库文件句柄 */
	int (* ft_getdotoffset)(const char *);	/*取字体点阵偏移的函数*/
}stFontContrl;

/**
 * @brief 边距设置结构体
*/

typedef struct BSP_printerRange{
	int nBoard;		/*左边界*/
	int nColumn;		/*字间距*/
	int nRaw;		/*行间距*/
}BSP_printerRange;

#endif
