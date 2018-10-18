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
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include <sys/mman.h>
#include <signal.h>

#include "NDK_TPPrint.h"
#include "NDK_TPPrnDrv.h"
#include "NDK_TPPrnFont.h"
#include "NDK.h"
#include "NDK_debug.h"
#include "../public/config.h"


extern unsigned char ndk_prnmode;    /**<打印模式0~7，默认为7*/
extern PrnDotlineData *ndk_gPrnDotlineData[200]; /**< 存储行点阵信息的指针数组，使用时使用动态分配，最大允许200行*/
extern unsigned int ndk_DotlineNum; /**<当前已经存储的行数以及即将要存储到数组的位置*/
extern int ndk_HZ_Xchange; /**<用于字库不存在设置的基本字体时需要通过放大的倍数*/
extern int ndk_HZ_Ychange;
extern int ndk_ZM_Xchange;
extern int ndk_ZM_Ychange;
extern int ndk_usr_hz_select;    /**<是否通过NDK_PrnSetUsrFont设置了用户自定义的汉字字体*/
extern int ndk_usr_zm_select;    /**<是否通过NDK_PrnSetUsrFont设置了用户自定义的ASCII字体*/
extern int ndk_HZUserFontId;    /**<通过NDK_PrnSetUsrFont设置了用户自定义的汉字字体id*/
extern int ndk_ZMUserFontId;    /**<通过NDK_PrnSetUsrFont设置了用户自定义的ASSIC字体id*/
extern ST_PRN_FONTMSG ndk_UsrMsg[MaxFontNum];
extern unsigned int ndk_CurrLineHigh;                                   /*一行数据的高度*/
extern unsigned int ndk_lineoffset;
extern unsigned int ndk_prn_mode;
extern unsigned int ndk_PR_MAXLINEWIDE;
extern unsigned int ndk_hz12mark;
extern unsigned int ndk_asc12mark;
extern unsigned int ndk_understart;                     /*下划线从第几个点开始*/
extern unsigned int ndk_underend;                           /*下划线从第几个点结束*/
extern unsigned int ndk_columnblank;                        /*字间距*/
extern unsigned int ndk_rowspace;                           /*行间距*/
extern unsigned int ndk_leftblank;                          /*左边距， 以点为单位*/
extern unsigned int ndk_LineState ;
extern unsigned int ndk_AlignType ;             /*对齐模式。默认取消对齐模式*/
extern unsigned int ndk_prngrey;
extern unsigned int ndk_PrnDirSwitch;       /*打印边送边打的开关*/
extern unsigned int ndk_PrnIsSetMode;
extern unsigned int ndk_Old_DotlineNum;

unsigned int ndk_BDF_Xpos = 0;
unsigned int ndk_BDF_LineSpace = 0;
unsigned int ndk_BDF_WordSpace = 0;
unsigned int ndk_BDF_Xmode = 1;
unsigned int ndk_BDF_Ymode = 1;
unsigned int ndk_Is_prnInit = 0;
int ndk_Is_test_version = 1;

EM_PRN_HZ_FONT currentPrnHZFont = PRN_HZ_FONT_24x24;
EM_PRN_ZM_FONT currentPrnZMFont = PRN_ZM_FONT_8x16;
EM_PRN_MODE currentMode = PRN_MODE_NORMAL;

#if 0
typedef enum {
    NDK_OK=0,               /**<操作成功*/
    NDK_ERR=-1,         /**<操作失败*/
    NDK_ERR_PARA=-2,
    NDK_ERR_MACLLOC=-3,
    NDK_ERR_OPEN_DEV=-4,
    NDK_ERR_IOCTL=-5,

} EM_NDK_ERR;
#endif
extern int NDK_FsOpen(const char *pszName,const char *pszMode);
extern int NDK_FsClose(int nHandle);
extern int NDK_FsRead(int nHandle, char *psBuffer, uint unLength );
extern int NDK_FsSeek(int nHandle, ulong ulDistance, uint unPosition );
extern int NDK_FsExist(const char *pszName);


extern image_t * ndk_image_decolor(image_t * img);
extern print_buf* ndk_image2printbuf(image_t * img);
extern int ndk_IOSetGreyScale(uint unGrey);
extern int ndk_IOSetMode(uint unMode);
extern int ndk_PrnDirectly(void);
extern image_t * image_decode(char * filepath);
extern void image_destroy(image_t * pimage);
extern int ndk_PrnLoadBDFFont(const char *pszPath);
extern int ndk_PrnBDFStr(ushort *pusText);

extern int ndk_TP_PrnCheckBDFPara(uint unXpos,uint unXmode,uint unYmode);


extern void ndk_TP_PrnWarning();




int NDK_TP_PrnSetMode(EM_PRN_MODE emMode,uint unSigOrDou);
int NDK_TP_PrnSetGreyScale(uint unGrey);
int NDK_TP_PrnGetStatus(EM_PRN_STATUS *pemStatus);


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
 *@li   NDK_ERR_PARA        参数错误
 *@li   NDK_ERR_OPEN_DEV        打印设备打开失败
 *@li   NDK_ERR_IOCTL       驱动调试错误(清打印缓冲区失败、打印重新设置失败)
 *@li   NDK_ERR_MACLLOC     内存空间不足
*/
int NDK_TP_PrnInit(uint unPrnDirSwitch)
{
    int    fd;
    int    ret;
    int i;
    //fprintf(stderr,"%s__%d\n",__FUNCTION__,unPrnDirSwitch);

    if((unPrnDirSwitch != 0)&& (unPrnDirSwitch != 1)) {
        NDK_LOG_CRIT(NDK_LOG_MODULE_PRINT,"parameter error \n");
        return NDK_ERR_PARA;
    }
    fd = ndk_get_prn_fd();
    if (fd < 0) {
        NDK_LOG_CRIT(NDK_LOG_MODULE_PRINT,"get print fd fail \n");
        return NDK_ERR_OPEN_DEV;
    }

    ret = ioctl(fd, PRN_IOCT_CLRBUF);
    if (ret < 0) {
        NDK_LOG_CRIT(NDK_LOG_MODULE_PRINT,"clear buf fail \n");
        return NDK_ERR_IOCTL;
    }

    ret = ioctl(fd, PRN_IOCT_RESET);
    if (ret < 0) {
        NDK_LOG_CRIT(NDK_LOG_MODULE_PRINT,"reset fail \n");
        return NDK_ERR_IOCTL;
    }

    ndk_resetnewprintparam();
    if(ndk_DotlineNum!=0)
        for(i=0; i<=ndk_DotlineNum; i++) {
            free(ndk_gPrnDotlineData[i]);
            ndk_gPrnDotlineData[i]=NULL;
        }
    if(ndk_gPrnDotlineData[0] != NULL) { /*防止由边送边打状态切换回等待打印时，指针未释放情况*/
        free(ndk_gPrnDotlineData[0]);
        ndk_gPrnDotlineData[0] = NULL;
    }
    ndk_gPrnDotlineData[0]=(PrnDotlineData *)malloc(sizeof(PrnDotlineData));
    if(ndk_gPrnDotlineData[0]==NULL) {
        NDK_LOG_ERR(NDK_LOG_MODULE_PRINT,"malloc error\n");
        return NDK_ERR_MACLLOC;
    } else {
        memset(ndk_gPrnDotlineData[0]->ucData,0x00,sizeof(ndk_gPrnDotlineData[0]->ucData));
        ndk_gPrnDotlineData[0]->usPrnLineHigh = 0;
        ndk_gPrnDotlineData[0]->usPrnLineOffset = 0;
        ndk_gPrnDotlineData[0]->usPrnSteps = 0;

    }
    ndk_DotlineNum = 0;
    ndk_PrnDirSwitch = unPrnDirSwitch;
    ndk_Is_prnInit = 1;
    if (ndk_getconfig("prn","prt_heatdelay",CFG_INT,&ndk_prngrey)<0) {
        ndk_prngrey = 3;
    }

    return NDK_OK;

}
/**
 *@brief        打印字符串
 *@details  该函数负责转换所有打印的字符串到点阵缓冲区，打印工作在调用Start之后开始送数打印。该函数为纯软件操作。
 *@param    pszBuf 为以\0为结尾的串,串的内容可为ASC码，汉字 换行\n或\r(表示结束本行，对于空行则直接进纸)。
            当pszBuf里面有汉字和ASCII的混合串时,字母和汉字只和最近一次设置有关。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        操作失败
 *@li   NDK_ERR_INIT_CONFIG     初始化配置失败(打印未初始化)
 *@li   NDK_ERR_MACLLOC     内存空间不足
 *@li       EM_PRN_STATUS      打印机状态值
*/
int NDK_TP_PrnStr(const char *pszBuf)
{
    unsigned int buflen,offset;
    char endprint_flag;

    if(ndk_DotlineNum == 200)
        return NDK_ERR_PARA;
    if(pszBuf == NULL)
        return NDK_ERR_PARA;
    if(ndk_Is_prnInit != 1)
        return NDK_ERR_INIT_CONFIG;
    buflen = strlen(pszBuf);
    if(access("/etc/pubkey_test",F_OK)==0) {
        if(ndk_Is_test_version==1) {
            ndk_Is_test_version=0;
            ndk_TP_PrnWarning();
        }
    }
    if (buflen == 0)/*避免零数据存储，防止与'\r'冲突*/
        return NDK_OK;
    offset = 0;
    endprint_flag = 0;   /**<字符串结束标志*/

    do {
        do {
            if(*pszBuf == '\f') {   /**<现有打印结束由调用NDK_PrnStart控制，无需打印结束标志，直接跳过该字符*/
                pszBuf++;
                continue;
            }

            //判断行缓冲是否满
            if (*pszBuf == '\r' || *pszBuf == '\n') {   /*得到一行数据*/
                if (ndk_CurrLineHigh < 1) {
                    ndk_CurrLineHigh = 1;
                }
                pszBuf++;
                offset++;
                break;
            }
            if (ndk_lineoffset >= ndk_PR_MAXLINEWIDE) { /*得到一行数据*/
                break;
            }
            //按行提取数据,识别'\0'
            if (*pszBuf == '\0') {
                if(*(pszBuf-1)=='\n'||*(pszBuf-1)=='\r')
                    endprint_flag=1;//说明pbuf是以换行符结束的。ndk_DotlineNum在处理换行符时已经++了。直接返回
                else
                    endprint_flag=2;//说明pbuf不是以换行符结束的。后面继续调用NDK_PrnStr时可以再后面添加数据,需要存储该半行字体，等后面的数据或换行符
                break;
            }

            if (*pszBuf & 0x80) {   /*汉字处理*/
                if(ndk_usr_hz_select==0) { /*是否使用自定义的汉字字体0--pos自带汉字 1--使用用户传入的字体*/
                    //fprintf(stderr,"HZ--\n");
                    if (ndk_sendHZtolinebuf(pszBuf) == 1)   /*返回1表示一行已满*/
                        continue;

                    offset += 2;
                    pszBuf+=2;
                } else {
                    //fprintf(stderr,"HZ_USR\n");
                    if (ndk_sendUSRHZtolinebuf(pszBuf) == 1)
                        continue;
                    offset += 2;
                    pszBuf+= 2;
                }
            } else {    /*字母处理*/
                if(ndk_usr_zm_select==0) { /*是否使用自定义的字母字体，与自定义汉字一样*/
                    //fprintf(stderr,"ZM\n");
                    if ((*pszBuf) <= 0x7f &&(*pszBuf) >= 0x20) {    /*是否在点阵库的范围内*/
                        if (ndk_sendZMtolinebuf(*pszBuf) == 1)  /*返回1表示一行已满*/
                            continue;
                    }
                    offset++;
                    pszBuf++;
                } else {
                    //fprintf(stderr,"ZM_USR\n");
                    if ((*pszBuf) <= 0x7f &&(*pszBuf) >= 0x20) {
                        if (ndk_sendUSRZMtolinebuf(pszBuf) == 1)
                            continue;
                    }
                    offset++;
                    pszBuf++;

                }

            }
        } while (1);/*得到一行数据*/
        if(endprint_flag) {
            if(endprint_flag==2) { /*防止最后一行结束没有\n结束，调用start开始打印遗漏最后一行*/
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh =ndk_CurrLineHigh;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset =ndk_lineoffset;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps +=ndk_rowspace;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usmode=ndk_prnmode;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnGray = ndk_prngrey;
                if (ndk_LineState == OpenLine) {
                    ndk_underend =ndk_lineoffset-ndk_columnblank;
                    ndk_getunderline();/*如果打开下划线功能则接收到一行的数据存储之后对点阵进行处理*/
                    ndk_understart= ndk_lineoffset;/* 下划线开始位置从该行存储偏移位置开始*/
                }
                if ((ndk_AlignType==RIGHTALIGN)||(ndk_AlignType==MIDALIGN))
                    ndk_aligndel();
                break;
            }
            //return NDK_OK;
            break;
        }

        if(ndk_CurrLineHigh==1) { /*单独调用NDK_PrnStr("\n"),时直接把步进数加到上一行的步进数上面，避免再开一个结构体存储节省空间*/
            if(ndk_DotlineNum== 0) { /*处理打印第一行就调用NDK_PrnStr("\n")的特殊情况*/
                ndk_gPrnDotlineData[0]->usPrnSteps += ndk_rowspace+8;
                ndk_gPrnDotlineData[0]->usPrnLineHigh = 1;
                ndk_gPrnDotlineData[0]->usmode=ndk_prnmode;
                ndk_gPrnDotlineData[0]->usPrnGray = ndk_prngrey;
                ndk_gPrnDotlineData[ndk_DotlineNum+1]=(PrnDotlineData *)malloc(sizeof(PrnDotlineData));
                if(ndk_gPrnDotlineData[ndk_DotlineNum+1]==NULL) {
                    NDK_LOG_ERR(NDK_LOG_MODULE_PRINT,"malloc error\n");
                    return NDK_ERR_MACLLOC;
                } else {
                    ndk_DotlineNum++;
                    memset(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData,0x00,sizeof(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData));
                    ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh = 0;
                    ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset = 0;
                    ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps = 0;
                    ndk_gPrnDotlineData[ndk_DotlineNum]->usmode= 7;
                    ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnGray = 3;
                }
            } else
                ndk_gPrnDotlineData[ndk_DotlineNum-1]->usPrnSteps += ndk_rowspace+8;
            ndk_CurrLineHigh =  0;

        } else {
            /*
             *满一行的数据("\r"，"\n"，或者数据超过一行的容量)进行每行数据的状态属性保存，并malloc下一行的存储空间给后续使用
             */
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh =ndk_CurrLineHigh;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset =ndk_lineoffset;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps +=ndk_rowspace;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usmode=ndk_prnmode;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnGray = ndk_prngrey;
            if (ndk_LineState == OpenLine) {
                ndk_underend =ndk_lineoffset-ndk_columnblank;
                ndk_getunderline();
            }
            if ((ndk_AlignType==RIGHTALIGN)||(ndk_AlignType==MIDALIGN))
                ndk_aligndel();
            ndk_understart = ndk_leftblank;/*一行数据满后行属性各变量都恢复初始状态*/
            ndk_underend = ndk_leftblank;
            ndk_lineoffset = ndk_leftblank;
            ndk_CurrLineHigh = 0;
            ndk_gPrnDotlineData[ndk_DotlineNum+1]=(PrnDotlineData *)malloc(sizeof(PrnDotlineData));
            if(ndk_gPrnDotlineData[ndk_DotlineNum+1]==NULL) {
                NDK_LOG_ERR(NDK_LOG_MODULE_PRINT,"malloc error\n");
                return NDK_ERR_MACLLOC;

            } else {
                ndk_DotlineNum++;
                memset(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData,0x00,sizeof(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData));
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh = 0;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset = 0;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps = 0;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usmode= 7;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnGray = 3;
                if(ndk_DotlineNum == 200)
                    return NDK_ERR_PARA;
            }
        }

    } while(1);
    if(ndk_PrnDirSwitch == 0)
        return NDK_OK;
    else
        return ndk_PrnDirectly();
}

/**
 *@brief        开始启动打印.
 *@details  NDK_PrnStr和NDK_PrnImage都是完成数据转换成点阵存储到缓冲区中工作，调用该函数开始送数打印。
                        调用NDK_PrnStart打印结束后要判断返回值是否为0，如果返回-1则说明向打印送数失败，则立即返回打印机状态值，不进行继续送数操作。
                        NDK_PrnStart打印结束之后会阻塞等待返回打印机状态的值。应用可根据NDK_PrnStart返回的值来判断打印机状态是否正常。
                        (如果返回的非打印机状态值或者NDK_OK，即其他系统错误时需要应用去取打印机状态，该可能性比较小)
 *@return
 *@li   NDK_OK              打印结束且取打印机状态正常
 *@li   NDK_ERR_INIT_CONFIG     初始化配置失败(打印未初始化)
 *@li   NDK_ERR_MACLLOC     内存空间不足
 *@li   NDK_ERR_OPEN_DEV        打印设备打开失败
 *@li       EM_PRN_STATUS      打印机状态值
*/
int NDK_TP_PrnStart(void)
{
    int i;
    int ret;
    EM_PRN_STATUS status;
    int get_val;


    if(ndk_Is_prnInit != 1)
        return NDK_ERR_INIT_CONFIG;

    /*
      *下列为真正的打印送数操作，打印每行之前要设置每行的属性变量，如模式和打印灰度等
     */
    // fprintf(stderr,"ndk_PrnDirSwitch:%d\n",ndk_PrnDirSwitch);
    //fprintf(stderr,"prn_star line num:%d\n",ndk_DotlineNum);
    if(ndk_PrnDirSwitch == 0) {
        for(i = 0; i<=ndk_DotlineNum; i++) {
            //fprintf(stderr,"%d:%d\n",i,ndk_gPrnDotlineData[i]->usPrnLineHigh);
            //
            if (ndk_gPrnDotlineData[i]->usPrnLineHigh > 1) {
                if(ndk_gPrnDotlineData[i]->usPrnGray >=0)
                    ndk_IOSetGreyScale(ndk_gPrnDotlineData[i]->usPrnGray );
                ndk_IOSetMode(ndk_gPrnDotlineData[i]->usmode);
                ret=ndk_printData(384,  ndk_gPrnDotlineData[i]->usPrnLineHigh, 0, (char *)&ndk_gPrnDotlineData[i]->ucData[MAXHIGHT-ndk_gPrnDotlineData[i]->usPrnLineHigh]);
                if(ret < 0) {
                    get_val=NDK_TP_PrnGetStatus(&status);
                    if(get_val != 0)
                        return get_val;
                    NDK_LOG_DEBUG(NDK_LOG_MODULE_PRINT,"ndk_printData:ndk_DotlineNum[%d]of[%d] error,status:%d\n",ndk_DotlineNum,i,status);
                    return status;
                }
                ndk_FeedPrinterByPixel(ndk_gPrnDotlineData[i]->usPrnSteps);

            } else {
                ndk_FeedPrinterByPixel(ndk_gPrnDotlineData[i]->usPrnSteps);
            }

        }
        //ndk_ClosePrinter();
        for(i=0; i<=ndk_DotlineNum; i++) {
            free(ndk_gPrnDotlineData[i]);
            ndk_gPrnDotlineData[i]=NULL;
        }
        ndk_gPrnDotlineData[0]=(PrnDotlineData *)malloc(sizeof(PrnDotlineData));
        if(ndk_gPrnDotlineData[0]==NULL) {
            NDK_LOG_ERR(NDK_LOG_MODULE_PRINT,"malloc error\n");
            return NDK_ERR_MACLLOC;
        } else {
            ndk_DotlineNum=0;
            memset(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData,0x00,sizeof(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData));
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh = 0;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset = 0;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps = 0;
        }
        ndk_understart = ndk_leftblank;
        ndk_underend = ndk_leftblank;
        ndk_lineoffset = ndk_leftblank;
        ndk_CurrLineHigh = 0;
        //return NDK_OK;
    } else {
        //fprintf(stderr,"high2222:%d\n",ndk_gPrnDotlineData[0]->usPrnLineHigh);
        if(ndk_gPrnDotlineData[0]->usPrnLineHigh != 0) {
            if(ndk_gPrnDotlineData[0]->usPrnGray >= 0)
                ndk_IOSetGreyScale(ndk_gPrnDotlineData[0]->usPrnGray );
            ndk_IOSetMode(ndk_gPrnDotlineData[0]->usmode);
            ret=ndk_printData(384,  ndk_gPrnDotlineData[0]->usPrnLineHigh, 0, (char *)&ndk_gPrnDotlineData[0]->ucData[MAXHIGHT-ndk_gPrnDotlineData[0]->usPrnLineHigh]);
            if(ret < 0) {
                get_val=NDK_TP_PrnGetStatus(&status);
                if(get_val != 0)
                    return get_val;
                NDK_LOG_DEBUG(NDK_LOG_MODULE_PRINT,"ndk_printData:ndk_DotlineNum[%d] error,status:%d\n",ndk_DotlineNum,status);
                return status;
            }
            ndk_FeedPrinterByPixel(ndk_gPrnDotlineData[0]->usPrnSteps);
        }
        //ndk_ClosePrinter();
        ndk_DotlineNum=0;
        memset(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData,0x00,sizeof(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData));
        ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh = 0;
        ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset = 0;
        ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps = 0;
        ndk_understart = ndk_leftblank;
        ndk_underend = ndk_leftblank;
        ndk_lineoffset = ndk_leftblank;
        ndk_CurrLineHigh = 0;
        //return NDK_OK;
    }
    if(access("/etc/pubkey_test",F_OK)==0)
        ndk_Is_test_version=1;
    while(1) {   /**< 最后返回打印结束时的打印状态值*/
        get_val=NDK_TP_PrnGetStatus(&status);
        if(get_val != 0)
            return get_val;
        if(status == PRN_ERR_BUSY)
            continue;
        else
            return status;
    }

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
 *@li   NDK_ERR_PARA                参数错误
 *@li   NDK_ERR_MACLLOC     内存空间不足
 *@li   NDK_ERR_INIT_CONFIG     初始化配置失败(打印未初始化)
 *@li   \ref EM_PRN_STATUS   "EM_PRN_STATUS"   打印机状态值
*/
int NDK_TP_PrnImage(uint unXsize,uint unYsize,uint unXpos,const char *psImgBuf)
{

    int cnt,i,j,k,offset,ucX,ucY;

    if(ndk_Is_prnInit != 1)
        return NDK_ERR_INIT_CONFIG;
    if(ndk_DotlineNum == 200)
        return NDK_ERR_PARA;

    if ( (unXsize <= 0) || (unYsize <=0) || (unXpos <0) || ((unXpos + unXsize) > ndk_PR_MAXLINEWIDE)||psImgBuf==NULL) {
        return NDK_ERR_PARA;
    }
    if(access("/etc/pubkey_test",F_OK)==0) {
        if(ndk_Is_test_version==1) {
            ndk_Is_test_version=0;
            ndk_TP_PrnWarning();
        }
    }
    if(ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh != 0) {
        ndk_gPrnDotlineData[ndk_DotlineNum+1]=(PrnDotlineData *)malloc(sizeof(PrnDotlineData));
        if(ndk_gPrnDotlineData[ndk_DotlineNum+1]==NULL) {
            NDK_LOG_ERR(NDK_LOG_MODULE_PRINT,"malloc error\n");
            return NDK_ERR_MACLLOC;
        } else {
            ndk_DotlineNum++;
            memset(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData,0x00,sizeof(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData));
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh = 0;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset = 0;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps = 0;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usmode= 7;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnGray = 3;
        }
    }

    ucX = (unXsize+7)/8;
    cnt = (unYsize+47)/48;

    if(ndk_PrnIsSetMode == 1)  //设置左偏移以及字间距时如果已经设置模式为横向放大则左偏移和字间距都减半(为保持放大后字间距和左偏移不随模式而变大)
        offset=unXpos/2;
    else
        offset=unXpos;
    for(i = 1 ; i<= cnt; i++) {
        if(i == cnt)
            ucY = unYsize-48 *(cnt-1);
        else
            ucY = 48;

        if(offset%8) {
            for(k = 0 ; k < ucY ; k++)
                for( j = 0 ; j < ucX ; j++) {
                    ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-ucY+k][offset/8+j] |= psImgBuf[(i-1)*MAXHIGHT*ucX+k*ucX+j]>>(offset&0x07);
                    if ((offset/8+j+1)!=(ndk_PR_MAXLINEWIDE/8))
                        ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-ucY+k][offset/8+j+1] |= psImgBuf[(i-1)*MAXHIGHT*ucX+k*ucX+j]<<(8-(offset&0x07));
                }
        } else {
            for(k = 0 ; k < ucY ; k++)
                for( j = 0 ; j < ucX ; j++) {
                    ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-ucY+k][offset/8+j] =psImgBuf[(i-1)*MAXHIGHT*ucX+k*ucX+j];
                }
        }
        ndk_CurrLineHigh=ucY;
        ndk_lineoffset=offset;
        ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh =ndk_CurrLineHigh;
        ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset =ndk_lineoffset;
        ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps =0;
        ndk_gPrnDotlineData[ndk_DotlineNum]->usmode=ndk_prnmode;
        ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnGray = ndk_prngrey;
        ndk_lineoffset = ndk_leftblank;
        ndk_CurrLineHigh = 0;
        ndk_gPrnDotlineData[ndk_DotlineNum+1]=(PrnDotlineData *)malloc(sizeof(PrnDotlineData));
        if(ndk_gPrnDotlineData[ndk_DotlineNum+1]==NULL) {
            //fprintf(stderr,"malloc error\n");
            NDK_LOG_ERR(NDK_LOG_MODULE_PRINT,"malloc error\n");
            return NDK_ERR_MACLLOC;

        } else {
            ndk_DotlineNum++;
            memset(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData,0x00,sizeof(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData));
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh = 0;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset = 0;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps = 0;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usmode= 7;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnGray = 3;
        }

    }
    if(ndk_PrnDirSwitch == 0)
        return NDK_OK;
    else
        return ndk_PrnDirectly();

}

/**
 *@brief        用于获取本打印机类型
 *@retval   pemType 用于返回打印机类型的值定义如下：
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
int NDK_TP_PrnGetType(EM_PRN_TYPE *pemType)
{
    *pemType=PRN_TYPE_TP;
    return NDK_OK;
}


/**
 *@brief        取打印驱动的版本信息.
 *@retval   pszVer 用于存储返回版本字符串
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
 *@li   NDK_ERR_OPEN_DEV        打印设备打开失败
 *@li   NDK_ERR_IOCTL       驱动调用失败(获取打印版本失败)
*/
int NDK_TP_PrnGetVersion(char *pszVer)
{
    int fd;
    int ret;

    if(pszVer == NULL)
        return NDK_ERR_PARA;
    fd = ndk_get_prn_fd();
    if (fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }

    ret = ioctl(fd, PRN_IOCG_VER, pszVer);
    if (ret < 0) {
        return NDK_ERR_IOCTL;
    }
    return NDK_OK;

}

/**
 *@brief        设置打印字体
 *@details  设置ASCII打印字体和汉字字体。应用层可参看底层和应用层的接口文件中的相关定义。
 *@param        emHZFont 设置汉字字体格式，0保持当前字体不变。
 *@param    emZMFont设置ASCII字体格式，0保持当前字体不变。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
*/

int NDK_TP_PrnSetFont(EM_PRN_HZ_FONT emHZFont,EM_PRN_ZM_FONT emZMFont)
{
    //int      fd;
    //int      ret;
    //prnparam r_prnparam, *p=&r_prnparam;

    if ( (emZMFont < 0 ) || ( emZMFont > ASCFONTNUM) || (emHZFont < 0 )||( emHZFont > HZFONTNUM ) )
        return NDK_ERR_PARA;
    ndk_SetFontMsg((emHZFont << 8)|emZMFont);
    currentPrnHZFont=emHZFont;
    currentPrnZMFont=emZMFont;
    ndk_usr_hz_select=0;
    ndk_usr_zm_select=0;
#if 0
    fd = ndk_get_prn_fd();
    if (fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }

    ret = ioctl(fd, PRN_IOCG_PARAM, p);
    if (ret < 0) {
        return NDK_ERR_IOCTL;
    }

    p->ASCfont = unFont;

    /* TODO:
    * 如果用户要设置汉字，需要底层驱动的支持.
    */

    ret = ioctl(fd, PRN_IOCS_PARAM, p);

    if (ret < 0) {
        return NDK_ERR_IOCTL;
    }
#endif
    return NDK_OK;

}


/**
 *@brief        获取打印机状态值.
 *@details      在打印之前可使用该函数判断打印机是否缺纸。
 *@retval     pemStatus 用于返回打印机状态值(错误码存在下面几种情况的相或的关系)
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
 *@li   NDK_ERR_OPEN_DEV        打印设备打开失败
 *@li   NDK_ERR_IOCTL       驱动调用失败(获取打印状态失败)
*/
int NDK_TP_PrnGetStatus(EM_PRN_STATUS *pemStatus)
{
    int fd;
    int ret;
    int status, *p=&status;

    if(pemStatus == NULL) {
        NDK_LOG_ERR(NDK_LOG_MODULE_PRINT,"abnormal state,parameter error \n");
        return NDK_ERR_PARA;
    }
    fd = ndk_get_prn_fd();
    if (fd < 0) {
        NDK_LOG_ERR(NDK_LOG_MODULE_PRINT,"abnormal state,get print fd fail \n");
        return NDK_ERR_OPEN_DEV;
    }

    ret = ioctl(fd, PRN_IOCG_STATUS, p);
    if (ret < 0) {
        NDK_LOG_ERR(NDK_LOG_MODULE_PRINT,"abnormal state,get print status fail \n");
        return NDK_ERR_IOCTL;
    }

    ret = 0;

    if ((status  == PRN_ERR_OK) || (status == PRN_ERR_DONE))
        ret = PRN_STATUS_OK;

    if (status == PRN_ERR_BUSY)
        ret = PRN_STATUS_BUSY;

    if (status & TP_ERR_VOLT)
        ret = PRN_STATUS_VOLERR;

    if (status & PRN_ERR_HEAT)
        ret = PRN_STATUS_OVERHEAT;

    if (status & PRN_ERR_PAPER)
        ret = PRN_STATUS_NOPAPER;

    *pemStatus = ret;
    return NDK_OK;

}

/**
 *@brief：  设置打印模式.
 *@param        unMode 打印模式(默认是使用正常模式)
 *@param    unSigOrDou 打印单双向选择(只对针打有效，热敏忽略)
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
*/

int NDK_TP_PrnSetMode(EM_PRN_MODE emMode,uint unSigOrDou)
{
    if ( (emMode <0) || (emMode > 3)) {
        return NDK_ERR_PARA;
    }
    if( (unSigOrDou != 0 ) && (unSigOrDou != 1 ))
        return NDK_ERR_PARA;

    if((ndk_PrnIsSetMode == 1 ) && (emMode >= 2)) { //由原先的横向放大模式跳回原先的正常模式，左偏移以及字间距恢复回原先两倍
        ndk_leftblank = ndk_leftblank*2;
        ndk_columnblank = ndk_columnblank*2;
        ndk_PrnIsSetMode = 0;
    }

    if((ndk_PrnIsSetMode == 0 ) && (emMode < 2)) { //由横向正常模式跳到横向放大模式，左偏移以及字间距减半(为保持放大后字间距和左偏移不随模式而变大)
        ndk_leftblank = ndk_leftblank/2;
        ndk_columnblank = ndk_columnblank/2;
        ndk_PrnIsSetMode = 1;
    }

    ndk_prnmode=emMode*2+1;            //api还是保持原先的mode为0到7的状态
    if(emMode < 2)
        ndk_PR_MAXLINEWIDE=384/2;
    else
        ndk_PR_MAXLINEWIDE=384;
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
 *@li   NDK_ERR_NOT_SUPPORT       不支持该功能
*/
int NDK_TP_PrnIntegrate(uint unXsize,uint unYsize,uint unXpos,uint unYpos,const char *psImgBuf,const char *pszTextBuf, uint unMode)
{
    return NDK_ERR_NOT_SUPPORT;
}


/**
 *@brief        设置打印灰度
 *@details      设置打印灰度(加热时间)，以便对于不同的打印纸进行打印效果微调.
 *@param    unGrey 灰度值，范围0~5；0为最淡的效果，5为最浓的打印效果。打印驱动默认的灰度级别为3。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
*/
int NDK_TP_PrnSetGreyScale(uint unGrey)
{

    if ( (unGrey <0) ||(unGrey > 5) ) {
        return NDK_ERR_PARA;
    }
    ndk_prngrey=unGrey;

    return NDK_OK;

}

/**
 *@brief    设置打印左边界、字间距、行间距。在对打印机有效设置后将一直有效，直至下次
 *@param  unBorder 左边距 值域为：0-288(默认为0)
 *@param    unColumn 字间距 值域为：0-255(默认为0)
 *@param    unRow 行间距 值域为：0-255(默认为0)
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
 *@li   NDK_ERR_OPEN_DEV        打印设备打开失败
 *@li   NDK_ERR_IOCTL       驱动调用失败(左间距、字间距、行间距设置失败、图形对齐方式设置失败)
*/
int NDK_TP_PrnSetForm(uint unBorder,uint unColumn, uint unRow)
{
    int fd;
    int ret;
    BSP_printerRange p;

    if ( (unBorder <0) ||(unBorder> 288) ||(unColumn <0) ||(unColumn> 255)||(unRow <0) ||(unRow> 255) ) {
        return NDK_ERR_PARA;
    }

    p.nBoard = unBorder;
    p.nRaw = unRow;
    p.nColumn=unColumn;

    fd = ndk_get_prn_fd();
    if (fd < 0) {
        return NDK_ERR_OPEN_DEV;
    }

    ret = ioctl(fd,PRN_IOCS_PRNRANGE,&p);
    if (ret < 0) {
        return NDK_ERR_IOCTL;
    }

    if(ndk_lineoffset == ndk_leftblank)
        if(ndk_PrnIsSetMode == 1)
            ndk_lineoffset= unBorder/2;
        else
            ndk_lineoffset= unBorder;
    ndk_rowspace = unRow;
    ndk_leftblank = unBorder;
    ndk_columnblank = unColumn;


    if(ndk_PrnIsSetMode == 1) { //设置左偏移以及字间距时如果已经设置模式为横向放大则左偏移和字间距都减半(为保持放大后字间距和左偏移不随模式而变大)
        ndk_leftblank = ndk_leftblank/2;
        ndk_columnblank = ndk_columnblank/2;
    }

    if (ndk_LineState== OpenLine)
        ndk_understart = ndk_lineoffset;

    ndk_AlignType = UNABLEALIGN;
    ret = ioctl(fd, PRN_IOCS_ALIGNTYPE, &ndk_AlignType);
    if (ret < 0) {
        return NDK_ERR_IOCTL;
    }
    NDK_LOG_INFO(NDK_LOG_MODULE_PRINT,"%s succ,current print form is(unBorder:%d,unColumn:%d,unRow:%d)\n",unBorder,unColumn,unRow);
    return NDK_OK;
}

/**
 *@brief    按像素走纸
 *@details  让打印机走纸，参数为像素点,调用该函数，并没有马上走纸，而是存在结构体中，等调用start之后和打印动作一起执行
 *@param  unPixel 走纸像素点 值域> 0
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
 *@li   NDK_ERR_INIT_CONFIG     初始化配置失败(打印未初始化配置)
 *@li   NDK_ERR_MACLLOC     内存空间不足
*/
int NDK_TP_PrnFeedByPixel(uint unPixel)
{
    if(ndk_Is_prnInit != 1)
        return NDK_ERR_INIT_CONFIG;
    if(( unPixel < 0) || (unPixel > 1024))
        return NDK_ERR_PARA;
    if(ndk_PrnDirSwitch == 0) {
        if (ndk_DotlineNum==0) {
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps+=unPixel;
            ndk_gPrnDotlineData[ndk_DotlineNum+1]=(PrnDotlineData *)malloc(sizeof(PrnDotlineData));
            if(ndk_gPrnDotlineData[ndk_DotlineNum+1]==NULL) {
                //fprintf(stderr,"malloc error\n");
                NDK_LOG_ERR(NDK_LOG_MODULE_PRINT,"malloc error\n");
                return NDK_ERR_MACLLOC;
            } else {
                ndk_DotlineNum++;
                memset(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData,0x00,sizeof(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData));
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh = 0;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset = 0;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps = 0;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usmode= 7;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnGray = 3;
            }
        } else
            ndk_gPrnDotlineData[ndk_DotlineNum-1]->usPrnSteps+=unPixel;
    } else
        ndk_FeedPrinterByPixel(unPixel);

    return NDK_OK;


}

/**
 *@brief    打印是否开启下划线功能.
 *@param  emStatus 0：开下划线；1：关下划线
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
*/
int NDK_TP_PrnSetUnderLine(EM_PRN_UNDERLINE_STATUS emStatus)
{
    if (emStatus > CloseLine)
        return NDK_ERR_PARA;

    if (emStatus == OpenLine) {         /*启用下划线功能*/
        ndk_understart = ndk_lineoffset;
        ndk_LineState = emStatus;
    }

    if ((emStatus == CloseLine)&&(ndk_LineState == OpenLine)) {
        ndk_underend = ndk_lineoffset-ndk_columnblank;
        ndk_LineState = emStatus;
    }

    return NDK_OK;

}

/**
 *@brief    设置对齐方式.(暂未实现)
 *@param  unType 0:左对齐; 1居中对齐; 2右对齐;3关闭对齐方式
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_NOT_SUPPORT       不支持该功能
*/
int NDK_TP_PrnSetAlignment(uint unType)
{
    return NDK_ERR_NOT_SUPPORT;
}

/**
 *@brief    获取打印长度.
 *@retval pfLen 返回已经打印的长度(当前开机，开始打印到现在打印结束为止所打印机打印的总共长度)
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
 *@li   NDK_ERR     操作失败(获取马达的总共步进计数失败)
*/
int NDK_TP_PrnGetPrintedLen(uint *punLen)
{
    int ret;
    unsigned int motorsteps = 0;
    unsigned int printlen_m,printlen_mm;

    if(punLen == NULL)
        return NDK_ERR_PARA;
    ret = ndk_GetTotalMotorSteps();
    if(ret < 0)
        return NDK_ERR;
    else
        motorsteps = (unsigned int)ret;

    //printlen_m = motorsteps/(16*1000);//单位为米
    //printlen_mm = (motorsteps - printlen_m*16*1000)/(16*100);
    //*punLen=motorsteps/16000;
    printlen_mm = motorsteps/16;
    *punLen = printlen_mm;
    return NDK_OK;

}

/**
 *@brief        自定义字体注册。
 *@param        pstMsg ST_PrintFontMsg类型指针，使用自定义注册要完成相应信息的填写
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
*/
int NDK_TP_PrnFontRegister(ST_PRN_FONTMSG *pstMsg)
{
    int fp;
    if(pstMsg == NULL)
        return NDK_ERR_PARA;
    if (((pstMsg->nNum) >= MaxFontNum)||((pstMsg->nNum) < 0))
        return NDK_ERR_PARA;
    if((pstMsg->pszName == NULL) || (pstMsg->func == NULL) ||((pstMsg->nDirection != 0)&&(pstMsg->nDirection != 1))||((pstMsg->nIsHZ!= 0)&&(pstMsg->nIsHZ != 1)))
        return NDK_ERR_PARA;

    if ((fp = NDK_FsOpen(pstMsg->pszName, "r")) < 0)    /*查看设置的字库是否存在*/
        return NDK_ERR_PARA;

    NDK_FsClose(fp);
    memcpy(&ndk_UsrMsg[pstMsg->nNum], pstMsg, sizeof(ndk_UsrMsg[pstMsg->nNum]));

    NDK_LOG_INFO(NDK_LOG_MODULE_PRINT,"%s succ,current print font name is %s\n",pstMsg->pszName);
    return NDK_OK;
}

/**
 *@brief    根据注册号来选择打印字体.
 *@param  unFontId 注册字体的id(该设置后会覆盖NDK_PrnSetFont中设定的字体)
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
*/
int NDK_TP_PrnSetUsrFont(uint unFontId)
{
    int fp;

    if ((unFontId>= MaxFontNum)||(unFontId < 0))
        return NDK_ERR_PARA;

    if ((fp = NDK_FsOpen(ndk_UsrMsg[unFontId].pszName, "r")) < 0)    /*查看设置的字库是否存在*/
        return NDK_ERR_PATH;

    NDK_FsClose(fp);
    if(ndk_UsrMsg[unFontId].nIsHZ==1) {
        ndk_usr_hz_select=1;
        ndk_HZUserFontId=unFontId;
    } else {
        ndk_usr_zm_select=1;
        ndk_ZMUserFontId=unFontId;
    }
    return NDK_OK;

}

/**
 *@brief    获得该次打印的点阵行数.
 *@retval  punLine 返回行数
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA                参数错误
 *@li   NDK_ERR_INIT_CONFIG     初始化配置失败(打印未初始化配置)
*/
int NDK_TP_PrnGetDotLine(uint *punLine)
{
    int cnt;
    int num = 0;
    if(ndk_Is_prnInit != 1)
        return NDK_ERR_INIT_CONFIG;
    if(punLine == NULL)
        return NDK_ERR_PARA;
    for(cnt=0; cnt<ndk_DotlineNum; cnt++)
        num+=ndk_gPrnDotlineData[cnt]->usPrnLineHigh;

    *punLine=num;
    return NDK_OK;

}
/**
 *@brief    打印bmp，png等格式的图片
 *@detail  该函数将存储在pos上的图片进行解码后存储到点阵缓冲区，  不过图片解码会暂用一定的时间，必要的时候需要有一定的等待时间
 *@param  pszPath 图片所在的路径
 *@param  unXpos  图形的左上角的列位置，且必须满足xpos+xsize(解码后图片的宽度值)<=ndk_PR_MAXLINEWIDE（正常模式为384，横向放大时为384/2）
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA                参数错误
 *@li   NDK_ERR_INIT_CONFIG     初始化配置失败(打印未初始化配置)
 *@li   NDK_ERR_DECODE_IMAGE                图像解码失败
 *@li   NDK_ERR_MACLLOC                 内存空间不足
*/
int NDK_TP_PrnPicture(uint unXpos,const char *pszPath)
{
    int ret;
    image_t * img;
    print_buf *printbuf;

    if(ndk_Is_prnInit != 1)
        return NDK_ERR_INIT_CONFIG;
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
    ret=NDK_TP_PrnImage(printbuf->width,printbuf->height,unXpos,printbuf->image_buf);
    if(ret != NDK_OK)
        return ret;
    return NDK_OK;


}

int NDK_TP_PrnSetPageLen(uint len)
{
    return NDK_ERR_NOT_SUPPORT;
}
/**
 *@brief    加载BDF字体
 *@detail  使用该函数加载BDF字体到内存中，比较大的字体会耗费一些时间。
 *@param  pszPath BDF所在的路径
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数错误
 *@li   NDK_ERR_PARA        参数错误
*/

int NDK_TP_PrnLoadBDFFont(const char *pszPath)
{
    int ret;
    if(pszPath == NULL)
        return NDK_ERR_PARA;
    ret = ndk_PrnLoadBDFFont(pszPath);
    if(ret == NDK_OK) {
        ndk_BDF_Xpos = 0;
        ndk_BDF_LineSpace = 0;
        ndk_BDF_WordSpace = 0;
        ndk_BDF_Xmode = 1;
        ndk_BDF_Ymode = 1;
    }
    return ret;
}

/**
 *@brief    打印BDF字体中的内容
 *@param  pusText unsigned short 类型的数据。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/

int NDK_TP_PrnBDFStr(ushort *pusText)
{
    if(ndk_DotlineNum == 200)
        return NDK_ERR_PARA;
    if(ndk_Is_prnInit != 1)
        return NDK_ERR_INIT_CONFIG;
    if(pusText == NULL)
        return NDK_ERR_PARA;
    return ndk_PrnBDFStr(pusText);
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
 *@li   NDK_ERR_PARA        参数错误
 *@li   NDK_ERR       操作失败(初始化链表头节点，使用calloc保证数据清0失败)
*/

int NDK_TP_PrnSetBDF(uint unXpos,uint unLineSpace,uint unWordSpace,uint unXmode,uint unYmode)
{

    int ret;
    if((unXpos < 0)||(unXpos > 288)||(unLineSpace <0 )||(unLineSpace > 255 )||(unWordSpace <0 )||(unWordSpace > 255 ))
        return NDK_ERR_PARA;

    ret = ndk_TP_PrnCheckBDFPara(unXpos,unXmode,unYmode);
    if(ret != 0)
        return ret;

    ndk_BDF_Xpos = unXpos;
    ndk_BDF_LineSpace = unLineSpace;
    ndk_BDF_WordSpace = unWordSpace;
    ndk_BDF_Xmode = unXmode;
    ndk_BDF_Ymode = unYmode;
    return NDK_OK;
}

/**
 *@brief  操作热敏打印机在打印单页前或打印完成后的进退纸操作
 *@param        emType
PRN_FEEDPAPER_BEFORE  ：       单页打印前退纸
PRN_FEEDPAPER_AFTER   ：        单页打印完成后进纸

 *@return
 *@li           NDK_OK                          操作成功
 *@li           NDK_ERR_PARA                          参数错误
 *@li           NDK_ERR_INIT_CONFIG     初始化配置失败(打印未初始化配置)
 *@li           NDK_ERR_OPEN_DEV     打印设备打开失败
 *@li           NDK_ERR_IOCTL     驱动调用失败(打印完走纸到撕纸处驱动调用失败、退纸驱动调用失败)
*/
NEXPORT int NDK_TP_PrnFeedPaper(EM_PRN_FEEDPAPER emType)
{
    int ret,fd = -1;
    if(ndk_Is_prnInit != 1)
        return NDK_ERR_INIT_CONFIG;
    fd = ndk_get_prn_fd();
    if (fd < 0) {
        NDK_LOG_DEBUG(NDK_LOG_MODULE_PRINT,"%s:%d ndk_get_prn_fd failed! \n",__func__, __LINE__);
        return NDK_ERR_OPEN_DEV;
    }
    if(emType == PRN_FEEDPAPER_AFTER) {
        ret = ioctl(fd, PRN_IOCS_FEEDENDPRINT);
        if (ret < 0) {
            NDK_LOG_DEBUG(NDK_LOG_MODULE_PRINT,"%s:%d PRN_IOCS_FEEDENDPRINT failed! \n",__func__, __LINE__);
            return NDK_ERR_IOCTL;
        }
    } else if(emType == PRN_FEEDPAPER_BEFORE) {
        ret = ioctl(fd, PRN_IOCS_FEEDBACK);
        if (ret < 0) {
            NDK_LOG_DEBUG(NDK_LOG_MODULE_PRINT,"%s:%d PRN_IOCS_FEEDBACK failed! \n",__func__, __LINE__);
            return NDK_ERR_IOCTL;
        }
    } else {
        return NDK_ERR_PARA;
    }
    return NDK_OK;
}


/* End of this file */




