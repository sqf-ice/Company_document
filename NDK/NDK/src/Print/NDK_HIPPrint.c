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
#include "NDK_TPPrint.h"
#include <unistd.h>

extern int hip_setprintfont(int font);
extern int hip_setprintswitch(uint unPrnDirSwitch);
extern int hip_setprintmode(int mode, int row_space);
extern void hip_setPrintRange(int nColumn, int nRow);
extern void hip_setPrintLeftBorder(int nBorder);
extern int hip_print(char * pbuf);
extern void hip_PrnPrintDone(void);
extern int hip_printimage(int xsize,int ysize,int xpos,char *ImgBuf);
extern int hip_getprinterversion(char *buf);
extern int hip_getprinterstatus();
extern int hip_FeedPrinterByPixel (int nPixel);
extern int hip_GetTotalMotorSteps(void);
extern int hip_clrprintbuf(void);
extern void hip_PrnStart(void);

void ndk_hip_resetnewprintparam(void);
image_t * ndk_image_decolor(image_t * img);
int NDK_HIP_PrnGetStatus(EM_PRN_STATUS *pemStatus);

extern int NDK_FsExist(const char *pszName);
extern image_t * image_decode(char * filepath);
extern print_buf* ndk_image2printbuf(image_t * img);
extern void image_destroy(image_t * pimage);
extern int ndk_Is_test_version;
extern EM_PRN_HZ_FONT currentPrnHZFont;
extern EM_PRN_ZM_FONT currentPrnZMFont;
extern EM_PRN_MODE currentMode;

int LastTotalMotorSteps;


/** @addtogroup 打印
* @{
*/

/**
 *@brief          打印机初始化
 *@details      包含清缓冲区,重置打印参数(包括字体、边距和模式等)。
 *@param      unPrnDirSwitch  是否开启边送边打还是调用NDK_PrnStart开始打印开关。
              0--关闭边送边打功能(默认)
                  在该模式下，所有的NDK_PrnStr,NDK_PrnImage都完成点阵转换工作，将点阵数据存到数据缓冲区，
                  在调用NDK_PrnStart之后才开始所有和打印相关的工作，包括走纸和打印。
              1--开启边送边打功能
                  在该模式下，只要满一行数据，就会送驱动打印，调用NDK_PrnStart，无任何效果，直接返回。
                  调用NDK_PrnFeedByPixel，将立即走纸返回。而在关闭模式下该操作会在NDK_PrnStart之后进行。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
*/
int NDK_HIP_PrnInit(uint unPrnDirSwitch)
{
    if((unPrnDirSwitch != 0) && (unPrnDirSwitch != 1)) {
        NDK_LOG_CRIT(NDK_LOG_MODULE_PRINT,"call %s fail in line %d \n",__FUNCTION__,__LINE__);
        return NDK_ERR_PARA;
    }

    hip_setprintswitch(unPrnDirSwitch);

    ndk_hip_resetnewprintparam();

    LastTotalMotorSteps = hip_GetTotalMotorSteps();

    return NDK_OK;
}
/**
 *@brief        打印字符串
 *@details      该函数负责转换所有打印的字符串到点阵缓冲区，打印工作在调用Start之后开始送数打印。该函数为纯软件操作。
 *@param        pszBuf 为以\0为结尾的串,串的内容可为ASC码，汉字 换行\n或\r(表示结束本行，对于空行则直接进纸)。
                当pszBuf里面有汉字和ASCII的混合串时,字母和汉字只和最近一次设置有关。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
*/
int NDK_HIP_PrnStr(const char *pszBuf)
{
    return hip_print((char *) pszBuf);
}
/**
 *@brief        开始启动打印.
 *@details  NDK_PrnStr和NDK_PrnImage都是完成数据转换成点阵存储到缓冲区中工作，调用该函数开始送数打印。
                        调用NDK_PrnStart打印结束后要判断返回值是否为0，如果返回-1则说明向打印送数失败，则立即返回，不进行继续送数操作。
                        返回值正常时，还要调用取状态来判断打印机是否正常
 *@return
 *@li   NDK_OK                操作成功
 *@li   NDK_ERRCODE         操作失败(打印过程未出错，在释放或者申请空间的时候出错)
 *@li EM_PRN_STATUS   如果是打印过程出错，返回出错时打印机的状态
*/
int NDK_HIP_PrnStart(void)
{
    EM_PRN_STATUS status;

    hip_PrnStart();
    if(access("/etc/pubkey_test",F_OK)==0)
        ndk_Is_test_version=1;

    hip_PrnPrintDone();
    while(1) {
        status = 0;
        NDK_HIP_PrnGetStatus(&status);
        if( status != PRN_STATUS_BUSY)
            break;
    }

    LastTotalMotorSteps = hip_GetTotalMotorSteps();

    return status;
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
 *@li   NDK_ERR_PARA        参数错误
*/
int NDK_HIP_PrnImage(uint unXsize,uint unYsize,uint unXpos,const char *psImgBuf)
{
    if(psImgBuf == NULL)
        return NDK_ERR_PARA;
    if( hip_printimage(unXsize,unYsize,unXpos,(char *)psImgBuf) < 0 )
        return NDK_ERR_PARA;

    return NDK_OK;
}
/**
 *@brief        用于获取本打印机类型
 *@retval   pemType 用于返回打印机类型的值定义如下：
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
*/
int NDK_HIP_PrnGetType(EM_PRN_TYPE *pemType)
{
    if(pemType == NULL)
        return NDK_ERR_PARA;
    *pemType = PRN_TYPE_HIP;

    return NDK_OK;
}
/**
 *@brief        取打印驱动的版本信息.
 *@retval   pszVer 用于存储返回版本字符串
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
*/
int NDK_HIP_PrnGetVersion(char *pszVer)
{
    if(pszVer == NULL)
        return NDK_ERR_PARA;
    if( hip_getprinterversion(pszVer) == 0 )
        return NDK_OK;

    return -1;
}
/**
 *@brief        设置打印字体
 *@details  设置ASCII打印字体和汉字字体。应用层可参看底层和应用层的接口文件中的相关定义。
 *@param        emHZFont 设置汉字字体格式
 *@param    emZMFont设置ASCII字体格式
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
*/
int NDK_HIP_PrnSetFont(EM_PRN_HZ_FONT emHZFont,EM_PRN_ZM_FONT emZMFont)
{
    if ( (emZMFont < 0 ) || ( emZMFont > ASCFONTNUM) || (emHZFont < 0 )||( emHZFont > HZFONTNUM ) )
        return NDK_ERR_PARA;

    switch(emHZFont) {

        case PRN_HZ_FONT_24x24A:
        case PRN_HZ_FONT_24x24B:
        case PRN_HZ_FONT_24x24C:
        case PRN_HZ_FONT_24x24USER:
        case PRN_HZ_FONT_48x24A:
        case PRN_HZ_FONT_48x24B:
        case PRN_HZ_FONT_48x24C:
        case PRN_HZ_FONT_24x48A:
        case PRN_HZ_FONT_24x48B:
        case PRN_HZ_FONT_24x48C:
        case PRN_HZ_FONT_48x48A:
        case PRN_HZ_FONT_48x48B:
        case PRN_HZ_FONT_48x48C:
            emHZFont = PRN_HZ_FONT_24x24;
            break;

        case PRN_HZ_FONT_12x12A:
            emHZFont = PRN_HZ_FONT_12x16;
            break;
        default:
            break;
    }

    switch(emZMFont) {
        case PRN_ZM_FONT_12x16A:
        case PRN_ZM_FONT_12x16B:
        case PRN_ZM_FONT_12x16C:
            emZMFont = PRN_ZM_FONT_10x16;
            break;

        case PRN_ZM_FONT_12x24A:
        case PRN_ZM_FONT_12x24B:
        case PRN_ZM_FONT_12x24C:
            emZMFont = PRN_ZM_FONT_16x32;
            break;

        case PRN_ZM_FONT_16x32A:
        case PRN_ZM_FONT_16x32B:
        case PRN_ZM_FONT_16x32C:
            emZMFont = PRN_ZM_FONT_16x32;
            break;

        case PRN_ZM_FONT_24x24A:
        case PRN_ZM_FONT_24x24B:
        case PRN_ZM_FONT_24x24C:
            emZMFont = PRN_ZM_FONT_24x32;
            break;

        case PRN_ZM_FONT_32x32A:
        case PRN_ZM_FONT_32x32B:
        case PRN_ZM_FONT_32x32C:
            emZMFont = PRN_ZM_FONT_24x32;
            break;

        case PRN_ZM_FONT_12x12:
            emZMFont = PRN_ZM_FONT_16x16;
            break;
        default:
            break;
    }

    hip_setprintfont((emHZFont<<8)|emZMFont);
    currentPrnHZFont=emHZFont;
    currentPrnZMFont=emZMFont;
    return NDK_OK;
}
/**
 *@brief          获取打印机状态值.
 *@details      在打印之前可使用该函数判断打印机是否缺纸。
 *@retval       pemStatus 用于返回打印机状态值(错误码存在两个或者多个值相或的关系)
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
int NDK_HIP_PrnGetStatus(EM_PRN_STATUS *pemStatus)
{
    *pemStatus = hip_getprinterstatus();
    return NDK_OK;
}
/**
 *@brief：  设置打印模式.
 *@param        unMode 打印模式(默认是使用正常模式)
 *@param    unSigOrDou 打印单双向选择0--单向 1--双向(只对针打有效，热敏忽略)
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
*/
int NDK_HIP_PrnSetMode(EM_PRN_MODE emMode,uint unSigOrDou)
{
    if( (unSigOrDou != 0 ) && (unSigOrDou != 1 ))
        return NDK_ERR_PARA;

    if ( (emMode <0) || (emMode > 3)) {
        return NDK_ERR_PARA;
    }

    hip_setprintmode(emMode<<1, -1);
    currentMode=emMode;
    return NDK_OK;
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
int NDK_HIP_PrnIntegrate(uint unXsize,uint unYsize,uint unXpos,uint unYpos,const char *psImgBuf,const char *pszTextBuf, uint unMode)
{
    return -1;
}
/**
 *@brief        设置打印灰度
 *@details      设置打印灰度(加热时间)，以便对于不同的打印纸进行打印效果微调.
 *@param    unGrey 灰度值，范围0~5；0为最淡的效果，5为最浓的打印效果。打印驱动默认的灰度级别为3。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
int NDK_HIP_PrnSetGreyScale(uint unGrey)
{
    return NDK_OK;
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
int NDK_HIP_PrnSetForm(uint unBorder,uint unColumn, uint unRow)
{
    if ( (unBorder <0) ||(unBorder> 288) ||(unColumn <0) ||(unColumn> 255)||(unRow <0) ||(unRow> 255)) {
        return NDK_ERR_PARA;
    }

    hip_setPrintRange(unColumn, unRow);
    hip_setPrintLeftBorder(unBorder);

    return NDK_OK;
}
/**
 *@brief      按像素走纸
 *@details  让打印机走纸，参数为像素点,调用该函数，并没有马上走纸，而是存在缓冲区中，等调用start之后和打印动作一起执行
 *@param    unPixel 走纸像素点 值域> 0
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
int NDK_HIP_PrnFeedByPixel(uint unPixel)
{
    if( hip_FeedPrinterByPixel(unPixel) == 0 )
        return NDK_OK;

    return NDK_ERR_PARA;
}

/**
 *@brief    打印是否开启下划线功能.
 *@param  emStatus 0：开下划线；1：关下划线
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
int NDK_HIP_PrnSetUnderLine(EM_PRN_UNDERLINE_STATUS emStatus)
{
    if( (emStatus != 0 ) && (emStatus != 1 ))
        return NDK_ERR_PARA;
    hip_SetUnderLine(emStatus);
    return NDK_OK;
}

/**
 *@brief    设置对齐方式.(暂未实现)
 *@param  unType 0:左对齐; 1居中对齐; 2右对齐;3关闭对齐方式
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
int NDK_HIP_PrnSetAlignment(uint unType)
{
    return NDK_OK;
}
/**
 *@brief    获取打印长度.
 *@retval punLen 返回已经打印的长度(当前开机，开始打印到现在打印结束为止所打印机打印的总共长度)
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
int NDK_HIP_PrnGetPrintedLen(uint *punLen)
{
    int ret;
    unsigned int motorsteps = 0;
    unsigned int printlen_m,printlen_mm;

    if(punLen == NULL)
        return NDK_ERR_PARA;
    ret = hip_GetTotalMotorSteps();

    if(ret < 0)
        return NDK_ERR;
    else
        motorsteps = (unsigned int)ret;

    //printlen_m = motorsteps/(16*1000);//单位为米
    //printlen_mm = (motorsteps - printlen_m*16*1000)/(16*100);
    //*punLen=motorsteps/16000;
    printlen_mm = motorsteps/(24.0)*4.235;
    *punLen = printlen_mm;
    return NDK_OK;
}
/**
 *@brief        自定义字体注册。
 *@param        pstMsg ST_PRN_FONTMSG类型指针，使用自定义注册要完成相应信息的填写
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
int NDK_HIP_PrnFontRegister(ST_PRN_FONTMSG *pstMsg)
{
    return NDK_OK;
}
/**
 *@brief    根据注册号来选择打印字体.
 *@param  unFontId 注册字体的id(该设置后会覆盖NDK_PrnSetFont中设定的字体)
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
int NDK_HIP_PrnSetUsrFont(uint unFontId)
{
    return NDK_OK;
}
/**
 *@brief    获得该次打印的点阵行数.
 *@retval  punLine 返回行数
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
extern int PrintSwitch;//0关闭，1打开
int NDK_HIP_PrnGetDotLine(uint *punLine)
{
    if(punLine == NULL)
        return NDK_ERR_PARA;
    if(PrintSwitch) {
        *punLine = 0;
        return NDK_ERR_PARA;
    }
    *punLine = hip_GetTotalMotorSteps() - LastTotalMotorSteps;
    return NDK_OK;
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
int NDK_HIP_PrnPicture(uint unXpos,const char *pszPath)
{
    int ret;
    image_t * img;
    print_buf *printbuf;

    if(pszPath == NULL)
        return NDK_ERR_PARA;
    if(NDK_FsExist(pszPath) != 0)
        return NDK_ERR_PATH;
    img = image_decode((char *)pszPath);
    if(img==NULL)
        return NDK_ERR_DECODE_IMAGE;
    img = ndk_image_decolor(img);
    printbuf = ndk_image2printbuf(img);
    //fprintf(stderr,"width:%d\n",printbuf->width);
    //fprintf(stderr,"printbuf->height:%d\n",printbuf->height);
    image_destroy(img);
    ret=NDK_HIP_PrnImage(printbuf->width,printbuf->height,unXpos,printbuf->image_buf);
    if(ret != NDK_OK)
        return ret;
    return NDK_OK;
}

int NDK_HIP_PrnSetPageLen(uint len)
{
    if(hip_setprintpagelen(len) == 0)
        return NDK_OK;

    return NDK_ERR_PARA;
}

/**
 *@brief    加载BDF字体
 *@detail  使用该函数加载BDF字体到内存中，比较大的字体会耗费一些时间。
 *@param  pszPath BDF所在的路径
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/

int NDK_HIP_PrnLoadBDFFont(const char *pszPath)
{
    return NDK_ERR_NOT_SUPPORT;
}


/**
 *@brief    打印BDF字体中的内容
 *@param  pusText unsigned short 类型的数据。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/

int NDK_HIP_PrnBDFStr(ushort *pusText)
{
    return NDK_ERR_NOT_SUPPORT;
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

int NDK_HIP_PrnSetBDF(uint unXpos,uint unLineSpace,uint unWordSpace,uint unXmode,uint unYmode)
{
    return NDK_ERR_NOT_SUPPORT;
}



void ndk_hip_resetnewprintparam(void)
{
    NDK_HIP_PrnSetFont(PRN_HZ_FONT_16x16,PRN_ZM_FONT_8x16);
    NDK_HIP_PrnSetMode(3,0);
	currentPrnHZFont = PRN_HZ_FONT_16x16;
	currentPrnZMFont = PRN_ZM_FONT_8x16;
	currentMode = 3;
    NDK_HIP_PrnSetForm(0,0,0);
    hip_clrprintbuf();
#if 0
    ndk_SetFontMsg((PRN_HZ_FONT_16x16 << 8)|PRN_ZM_FONT_8x16);  /*设置默认字体*/
    NDK_PrnSetMode(3,0);             /*模式恢复为正常模式*/
    ndk_columnblank= 0;                 /*字间距*/
    ndk_rowspace= 0;                    /*行间距*/
    ndk_leftblank= 0;                       /*左边距， 以点为单位*/
    ndk_LineState = UnableLine;             /*下划线启用标识，默认为关*/
    ndk_understart = 0;                 /*下划线从第几个点开始*/
    ndk_underend = 0;                   /*下划线从第几个点结束*/
    ndk_AlignType = UNABLEALIGN;
    ndk_lineoffset= ndk_leftblank;     /*每一行的起始位置为左偏移保持一致*/
#endif
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
NEXPORT int NDK_HIP_PrnFeedPaper(EM_PRN_FEEDPAPER emType)
{
    return NDK_ERR_NOT_SUPPORT;
}


/** @} */ // 打印模块结束

/* End of this file */
