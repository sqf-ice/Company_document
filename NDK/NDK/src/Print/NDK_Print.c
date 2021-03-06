#if 1
/*
* 新大陆公司 版权所有(c) 2011-2015
*
* 统一平台NDK API
* 作    者：    产品开发部
* 日    期：    2012-08-17
* 版    本：    V1.00
* 最后修改人：
* 最后修改日期：
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NDK.h"
#include "NDK_debug.h"
#include "NDK_Print.h"

static int prn_driver_init(void)
{
    int ret;
    char buf[20];

    memset(buf,0x00,sizeof(buf));
    if(p_ndk_PrnDriver==NULL) {
        if ((ret=NDK_SysGetPosInfo(1,NULL,buf))<0) {
            NDK_LOG_DEBUG(NDK_LOG_MODULE_PRINT, "call NDK_SysGetPosInfo err\r\n");
            return ret;
        }

        fprintf(stderr, "print type = %02x\r\n", buf[SYS_HWTYPE_PRINTER]);
        if(buf[SYS_HWTYPE_PRINTER]==0x0) {
            p_ndk_PrnDriver =(ndk_prn_driver *) &ndk_NO_driver;
        } else if (buf[SYS_HWTYPE_PRINTER]&0x80) { //针打
            p_ndk_PrnDriver = (ndk_prn_driver *)&ndk_HIP_driver;
        } else {
            p_ndk_PrnDriver =(ndk_prn_driver *)&ndk_TP_driver;
        }
    }

    return 0;
}

/**
 *@brief        打印机初始化
 *@details      包含清缓冲区,重置打印参数(包括字体、边距和模式等)。
 *@param        unPrnDirSwitch  是否开启边送边打还是调用NDK_PrnStart开始打印开关。
                        0--关闭边送边打功能(默认)
                        在该模式下，所有的NDK_PrnStr,NDK_PrnImage都完成点阵转换工作，将点阵数据存到数据缓冲区，
                        在调用NDK_PrnStart之后才开始所有和打印相关的工作，包括走纸和打印。
                        1--开启边送边打功能
                        在该模式下，只要满一行数据，就会送驱动打印，调用NDK_PrnStart，无任何效果，直接返回。
                        调用NDK_PrnFeedByPixel，将立即走纸返回。而在关闭模式下该操作会在NDK_PrnStart之后进行。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnInit(uint unPrnDirSwitch)
{
    EM_PRN_TYPE tmptype;
    NDK_LOG_INFO (NDK_LOG_MODULE_PRINT,"call %s \n", __FUNCTION__);
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }
    NDK_PrnGetType(&tmptype);
    NDK_LOG_INFO(NDK_LOG_MODULE_PRINT,"%s succ,current print type is %d\n",__func__,tmptype);
    return p_ndk_PrnDriver->p_ndk_prnInit(unPrnDirSwitch);

}
/**
 *@brief        获取打印机通道采样值(底层暂未支持)
 *@details  打印机通道采样值获取。
 *@param    emChanelNum     AD通道号(参考\ref EM_PRN_CHANELNUM "EM_PRN_CHANELNUM")
 *@retval   punAdValue      AD采样值(电压的值以V为单位，温度以0.1C为单位)
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它\ref EM_NDK_ERR "EM_NDK_ERR"        操作失败
*/
NEXPORT int NDK_PrnGetAd(EM_PRN_CHANELNUM emChanelNum,uint *punAdValue)
{
    return -1;
}
/**
 *@brief        打印字符串
 *@details  该函数负责转换所有打印的字符串到点阵缓冲区，打印工作在调用Start之后开始送数打印。该函数为纯软件操作。
 *@param    pszBuf 为以\0为结尾的串,串的内容可为ASC码，汉字 换行\n或\r(表示结束本行，对于空行则直接进纸)。
            当pszBuf里面有汉字和ASCII的混合串时,字母和汉字只和最近一次设置有关。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnStr(const char *pszBuf)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnStr(pszBuf);

}

/**
 *@brief        开始启动打印.
 *@details  NDK_PrnStr和NDK_PrnImage都是完成数据转换成点阵存储到缓冲区中工作，调用该函数开始送数打印。
                        调用NDK_PrnStart打印结束后要判断返回值是否为0，如果返回-1则说明向打印送数失败，则立即返回打印机状态值，不进行继续送数操作。
                        NDK_PrnStart打印结束之后会阻塞等待返回打印机状态的值。应用可根据NDK_PrnStart返回的值来判断打印机状态是否正常。
                        (如果返回的非打印机状态值或者NDK_OK，即其他系统错误时需要应用去取打印机状态，该可能性比较小)
 *@return
 *@li   NDK_OK              打印结束且取打印机状态正常
 *@li   EM_NDK_ERR      其他系统错误(如NDK_ERR_OPEN_DEV或者NDK_ERR_MACLLOC)
 *@li EM_PRN_STATUS      打印机状态值
*/
NEXPORT int NDK_PrnStart(void)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnStart();
}

/**
 *@brief        打印图形(该函数也是转换打印点阵到点阵缓冲区，调用NDK_PrnStart开始打印)
 *@details      热敏打最大宽度384个点。如果xsize和xpos相加之和大于上述宽度限制会返回失败，如果是横向放大模式的话不能超过384/2。
 *@param        unXsize 图形的宽度（像素）
 *@param        unYsize 图形的高度（像素）
 *@param        unXpos  图形的左上角的列位置，且必须满足xpos+xsize<=ndk_PR_MAXLINEWIDE（正常模式为384，横向放大时为384/2）
 *@param        psImgBuf 图象点阵数据,为横向排列，第一个字节第一行的前8个点，D7为第一个点
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnImage(uint unXsize,uint unYsize,uint unXpos,const char *psImgBuf)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnImage(unXsize,unYsize,unXpos,psImgBuf);

}

/**
 *@brief        用于获取本打印机类型
 *@retval   pemType 用于返回打印机类型的值定义如下：
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnGetType(EM_PRN_TYPE *pemType)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnGetType(pemType);
}


/**
 *@brief        取打印驱动的版本信息.
 *@retval   pszVer 用于存储返回版本字符串
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnGetVersion(char *pszVer)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnGetVersion(pszVer);
}

/**
 *@brief        设置打印字体
 *@details  设置ASCII打印字体和汉字字体。应用层可参看底层和应用层的接口文件中的相关定义。
 *@param        emHZFont 设置汉字字体格式
 *@param    emZMFont设置ASCII字体格式
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/

NEXPORT int NDK_PrnSetFont(EM_PRN_HZ_FONT emHZFont,EM_PRN_ZM_FONT emZMFont)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnSetFont(emHZFont,emZMFont);

}


/**
 *@brief        获取打印机状态值.
 *@details      在打印之前可使用该函数判断打印机是否缺纸。
 *@retval     pemStatus 用于返回打印机状态值(错误码存在下面几种情况的相或的关系)
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnGetStatus(EM_PRN_STATUS *pemStatus)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnGetStatus(pemStatus);
}

/**
 *@brief：  设置打印模式.
 *@param        unMode 打印模式(默认是使用正常模式)
 *@param    unSigOrDou 打印单双向选择(只对针打有效，热敏忽略)
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/

NEXPORT int NDK_PrnSetMode(EM_PRN_MODE emMode,uint unSigOrDou)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnSetMode(emMode,unSigOrDou);

}

/**
 *@brief    文字与图形的排版打印.(暂未实现)
 *@param    unXsize 图像宽度(像素)，不能大于384，必须是8的整数倍
 *@param    unYsize 图像高度(像素)，不能大于384，必须是8的整数倍
 *@param    unXpos  图像的横向偏移位置，(xpos+xsize)不能大于384
 *@param    unYpos  图像的纵向偏移位置
 *@param    psImgBuf 图像点阵缓冲，必须为横向排列的点阵数据
 *@param    pszTextBuf] 文字字符串首地址，字符串长度不能超过1K
 *@param    unMode 打印的模式
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnIntegrate(uint unXsize,uint unYsize,uint unXpos,uint unYpos,const char *psImgBuf,const char *pszTextBuf, uint unMode)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnIntegrate(unXsize,unYsize,unXpos,unYpos,psImgBuf,pszTextBuf,unMode);
}


/**
 *@brief        设置打印灰度
 *@details      设置打印灰度(加热时间)，以便对于不同的打印纸进行打印效果微调.
 *@param    unGrey 灰度值，范围0~5；0为最淡的效果，5为最浓的打印效果。打印驱动默认的灰度级别为3。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnSetGreyScale(uint unGrey)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    ret=p_ndk_PrnDriver->p_ndk_prnSetGreyScale(unGrey);
    NDK_LOG_INFO(NDK_LOG_MODULE_PRINT,"%s succ,current print grayscale is %d\n",__func__,unGrey);
    return ret;

}

/**
 *@brief    设置打印左边界、字间距、行间距。在对打印机有效设置后将一直有效，直至下次
 *@param  unBorder 左边距 值域为：0-288(默认为0)
 *@param    unColumn 字间距 值域为：0-255(默认为0)
 *@param    unRow 行间距 值域为：0-255(默认为0)
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnSetForm(uint unBorder,uint unColumn, uint unRow)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnSetForm(unBorder,unColumn,unRow);
}

/**
 *@brief    按像素走纸
 *@details  让打印机走纸，参数为像素点,调用该函数，并没有马上走纸，而是存在结构体中，等调用start之后和打印动作一起执行
 *@param  unPixel 走纸像素点 值域> 0
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnFeedByPixel(uint unPixel)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnFeedByPixel(unPixel);
}

/**
 *@brief    打印是否开启下划线功能.
 *@param  emStatus 0：开下划线；1：关下划线
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnSetUnderLine(EM_PRN_UNDERLINE_STATUS emStatus)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnSetUnderLine(emStatus);
}

/**
 *@brief    设置对齐方式.(暂未实现)
 *@param  unType 0:左对齐; 1居中对齐; 2右对齐;3关闭对齐方式
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnSetAlignment(uint unType)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnSetAlignment(unType);
}

/**
 *@brief    获取打印长度.
 *@retval pfLen 返回已经打印的长度(当前开机，开始打印到现在打印结束为止所打印机打印的总共长度)
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnGetPrintedLen(uint *punLen)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnGetPrintedLen(punLen);
}

/**
 *@brief        自定义字体注册。
 *@param        pstMsg ST_PrintFontMsg类型指针，使用自定义注册要完成相应信息的填写
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnFontRegister(ST_PRN_FONTMSG *pstMsg)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnFontRegister(pstMsg);
}

/**
 *@brief    根据注册号来选择打印字体.
 *@param  unFontId 注册字体的id(该设置后会覆盖NDK_PrnSetFont中设定的字体)
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnSetUsrFont(uint unFontId)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnSetUsrFont(unFontId);
}

/**
 *@brief    获得该次打印的点阵行数.
 *@retval  punLine 返回行数
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnGetDotLine(uint *punLine)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnGetDotLine(punLine);
}

/**
 *@brief    打印bmp，png等格式的图片
 *@detail  该函数将存储在pos上的图片进行解码后存储到点阵缓冲区，  不过图片解码会暂用一定的时间，必要的时候需要有一定的等待时间
 *@param  pszPath 图片所在的路径
 *@param  unXpos  图形的左上角的列位置，且必须满足xpos+xsize(解码后图片的宽度值)<=ndk_PR_MAXLINEWIDE（正常模式为384，横向放大时为384/2）
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnPicture(uint unXpos,const char *pszPath)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnPicture(unXpos,pszPath);
}

/**
 *@brief    设置打印页长
 *@detail   对打印机打印页长进行设置，该api只对针打生效
 *@param  len   页长
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
NEXPORT int NDK_PrnSetPageLen(uint len)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    if(p_ndk_PrnDriver->p_ndk_prnSetPageLen)
        return p_ndk_PrnDriver->p_ndk_prnSetPageLen(len);

    return  NDK_OK;
}

/**
 *@brief    加载BDF字体
 *@detail  使用该函数加载BDF字体到内存中，比较大的字体会耗费一些时间。
 *@param  pszPath BDF所在的路径
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/

int NDK_PrnLoadBDFFont(const char *pszPath)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    if(p_ndk_PrnDriver->p_ndk_prnLoadBDFFont)
        return p_ndk_PrnDriver->p_ndk_prnLoadBDFFont(pszPath);

    return  NDK_OK;
}

/**
 *@brief    打印BDF字体中的内容
 *@param  pusText unsigned short 类型的数据。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/

int NDK_PrnBDFStr(ushort *pusText)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    if(p_ndk_PrnDriver->p_ndk_prnBDFStr)
        return p_ndk_PrnDriver->p_ndk_prnBDFStr(pusText);

    return  NDK_OK;

}
/**
 *@brief    设置BDF字体属性
 *@param  unXpos  左偏移 值域为：0-288(默认为0)
 *@param  unLineSpace  行间距 值域为：0-255(默认为0)
 *@param  unWordSpace  字间距 值域为：0-255(默认为0)
 *@param  unXmode  横向放大倍数(注意，字体的MaxWidth*unXmode必须不能超过384，否则失败)
 *@param  unYmode  纵向放大倍数(注意，字体的MaxHeight*unYmode必须不能超过48，否则失败)
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/

int NDK_PrnSetBDF(uint unXpos,uint unLineSpace,uint unWordSpace,uint unXmode,uint unYmode)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    if(p_ndk_PrnDriver->p_ndk_prnSetBDF)
        return p_ndk_PrnDriver->p_ndk_prnSetBDF(unXpos,unLineSpace,unWordSpace,unXmode,unYmode);

    return  NDK_OK;

}

/**
 *@brief  操作热敏打印机在打印单页前或打印完成后的进退纸操作
 *@param        emType
PRN_FEEDPAPER_BEFORE  ：       单页打印前退纸
PRN_FEEDPAPER_AFTER   ：        单页打印完成后进纸

 *@return
 *@li NDK_OK 操作成功
 *@li 其它EM_NDK_ERR 操作失败
*/
NEXPORT int NDK_PrnFeedPaper(EM_PRN_FEEDPAPER emType)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    if(p_ndk_PrnDriver->p_ndk_prnFeedPaper)
        return p_ndk_PrnDriver->p_ndk_prnFeedPaper(emType);

    return  NDK_OK;

}

/**
 *@brief    根据输入字符串获取出系统指定打印字体相应的点阵缓冲
 *@param    emGetFont 字库点阵尺寸大小,目前仅支持16x16字阵(汉字16x16,字符8x16)
 *@param    pszBuf 字符串
 *@retval   psOutbuf   返回点阵数据横向排列，如下图所示:\n
-----------------D7~~D0--------------D7~~D0------------------\n
Byte 1: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte2 \n
Byte 3: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte4 \n
Byte 5: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte6 \n
Byte 7: ○ ○ ● ● ● ● ● ●  ●  ● ● ● ● ● ○ ○ Byte8 \n
Byte 9: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte10    \n
Byte11: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte12    \n
Byte13: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte14    \n
Byte15: ○ ○ ○ ○ ○ ○ ○ ○  ○  ○ ○ ○ ○ ○ ○ ○ Byte16    \n
---------------------------------------------------------------\n
 *@param    unBuf_w psOutbuf 缓存像素宽,要求宽度不小于pszBuf字符串长度*字符点阵宽度(如8)
 *@param    unBuf_h psOutbuf 缓存像素高,要求不小于字符点阵高度

 *@return
 *@li       NDK_OK    操作成功
 *@li       \ref NDK_ERR_PARA     "NDK_ERR_PARA"        参数错误
 *@li       \ref NDK_ERR_NOT_SUPPORT  "NDK_ERR_NOT_SUPPORT"  未知的字库尺寸
*/
NEXPORT int NDK_PrnGetDotData(EM_PRN_GETFONT emGetFont,const char *pszBuf,char *psOutbuf,uint unBuf_w,uint unBuf_h)
{
	char *pstr = pszBuf;
	int offset = 0,i,k=0;
	int fd = -1;
	char dotfile[PATH_MAX];

	if(pszBuf==NULL||psOutbuf==NULL)
	{
		return NDK_ERR_PARA;
	}
	if((emGetFont>=PRN_GETFONT_MAX)||(emGetFont<PRN_GETFONT_16X16))
	{
		return NDK_ERR_NOT_SUPPORT;
	}
    if ((unBuf_w<(8*strlen(pszBuf)))||(unBuf_h<16)) {
        return NDK_ERR_PARA;
    }

	switch(emGetFont)
	{
	case PRN_GETFONT_16X16:
		strcpy(dotfile,"/guiapp/share/fonts/font_h.bin");
		break;

	default:
		return NDK_ERR_NOT_SUPPORT;
		break;
	}

	while(*pstr)
	{
		if(*pstr&0x80)
		{
			offset=((*pstr-0x81)*191+*(pstr+1)-0x40)*32;//GB18030
			if(fd<0)
			{
				fd = open(dotfile, O_RDONLY);
				if (fd < 0) {
					return NDK_ERR;
				}
			}
			lseek(fd, offset, 0);
			for(i=0;i<16;i++)
			{
				read(fd,psOutbuf+k+i*((unBuf_w+7)/8),2);
			}
			k+=2;
			pstr+=2;
		}
		else
		{
			offset = ((*pstr-0x20)*16)+0xbde40;
			if(fd<0)
			{
				fd = open(dotfile, O_RDONLY);
				if (fd < 0) {
					return NDK_ERR;
				}
			}
			lseek(fd, offset, 0);
			for(i=0;i<16;i++)
			{
				read(fd,psOutbuf+k+i*((unBuf_w+7)/8),1);
			}
			k++;
			pstr++;
		}
	}
	if(fd>=0)
	{
		close(fd);
	}
	return NDK_OK;

}


/* End of this file */
#endif
