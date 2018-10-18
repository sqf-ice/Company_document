#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "NDK_TPPrnBDF.h"
#include "NDK_TPPrnlist.h"
#include "NDK.h"
#include "NDK_debug.h"
#include "NDK_TPPrint.h"
#include "NDK_TPPrnDrv.h"
#include "NDK_TPPrnFont.h"


PRN_SECTION_HEADER *prncustom_font;
prn_list_t* prnfont_list = NULL;

int MAXPixel=384; /*最大每行可填充到的像素号*/

static char FontName[300]= {0};

#define LoadFont    1
#define NotLoadFont     0
int SDKFONTMARK=NotLoadFont;    /*用于标识是否有下载spire的字库*/


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

extern unsigned int ndk_BDF_Xpos;
extern unsigned int ndk_BDF_LineSpace;
extern unsigned int ndk_BDF_WordSpace;
extern unsigned int ndk_BDF_Xmode;
extern unsigned int ndk_BDF_Ymode;


unsigned int ndk_BDF_MaxWidth = 0;
unsigned int ndk_BDF_Height = 0;

PRN_SECTION_HEADER * prn_list_getfont(prn_list_t * li, int fontid)
{
    prnlist_node_t * ntmp;
    PRN_SECTION_HEADER * font_custm;

    if (li == NULL) {
        return NULL;
    }
    ntmp = li->node;
    while (ntmp) {
        font_custm = (PRN_SECTION_HEADER * )(ntmp->element);
        if (font_custm->fontid==fontid) {
            break;
        }
        ntmp = ntmp->next;
    }
    return (ntmp == NULL ? NULL : ntmp->element);
}


int prn_loadfont_bdf(uint unFontID,const char *pszPath)
{
    PRN_SECTION_HEADER *font_custm,*last_font;
    PMWCFONT pf;

    pf = bdf_read_font((char *)pszPath);

    prn_list_remove(prnfont_list, 0);
    prn_list_uninit(prnfont_list);
    prnfont_list=NULL;

    if(prnfont_list==NULL) {
        if (prn_list_init(&prnfont_list) < 0) {
            return NDK_ERR;
        }
    }

    if((font_custm = calloc(1,sizeof(PRN_SECTION_HEADER)))==NULL) {
        return NDK_ERR_MACLLOC;
    }

    font_custm->pf = pf;
    font_custm->fontid = unFontID;
    font_custm->next=NULL;
    font_custm->start = pf->firstchar;
    font_custm->end = pf->size+pf->firstchar-1;

    if((last_font=prn_list_getfont(prnfont_list,unFontID))==NULL) {
        prn_list_add(prnfont_list, font_custm, 0);
    } else {
        while(last_font->next!=NULL) {
            last_font =last_font->next;
        }
        last_font->next = font_custm;
    }
    return NDK_OK;

}


void prn_XFontMul(const unsigned char * srcbuf, unsigned char * dstbuf, int x, int y, int mul, const int wmaxdot)
{
    int i, j, k, m;
    int umask=0x80;
    int mask=0xff;
    int linedot;   /*每行点数记录*/

    mask>>=8-mul;
    m=0;

    for (i=0; i<y; i++) {       /*行数*/
        linedot=0;
        for (j=0; j<x; j++) {   /*每行字节数*/
            umask=0x80;
            for(k=0; k<8; k++) {
                if((*(srcbuf+i*x+j) & umask) !=0) {
                    if((8-m%8)>=mul) {
                        *(dstbuf+m/8) |= mask<<(8-mul-m%8);
                    } else {
                        *(dstbuf+m/8) |= mask>>(mul-(8-m%8));
                        *(dstbuf+m/8+1) |= mask<<(8-(mul-(8-m%8)));
                    }
                }
                umask>>=1;
                linedot++;
                m+=mul;
                if(linedot == wmaxdot) {    /*只取有效的点*/
                    j=x;
                    m=(m+7)/8*8;
                    break;
                }
            }
        }
    }
}

void prn_YFontMul(const unsigned char * srcbuf, unsigned char * dstbuf, int x, int y, int mul)
{
    int i, j, m;

    for (i=0; i<y; i++) {       /*行数*/
        for (j=0; j<x; j++) {   /*每行字节数*/
            for (m=0; m<mul; m++) {
                *(dstbuf+i*x*mul+j+m*x)=*(srcbuf+i*x+j);
            }
        }
    }
}


int ndk_PrnLoadBDFFont(const char *pszPath)
{

    int ret;
    //fprintf(stderr,"go into prn load bdf\n");
    //fprintf(stderr,"path:%s\n",pszPath);
    if (access(pszPath, F_OK) != 0)
        return NDK_ERR_PATH;

    if(strcmp(FontName, pszPath)!=0) {

        ret=prn_loadfont_bdf(1, pszPath);
        if(ret != NDK_OK) {
            return ret;
        }
        memset(FontName, 0, sizeof(FontName));
        strcpy(FontName, pszPath);
    }

    SDKFONTMARK = LoadFont;
    return NDK_OK;
}

int ndk_PrnBDFStr(ushort *pusText)
{
    int xpos;
    PRN_SECTION_HEADER *font_custm, *font_default;
    int i=0, j, k;
    int offset;
    int fontbyte=0;
    char tmp[48*48/8]= {0};
    char multmpw[48*48/8]= {0};
    char multmph[48*48/8]= {0};
    //char mulx=0,muly=0;
    //char tmp[48*6];
    int ret, w, h,width;
    int prnw,prnh;
    int xmark;
    int linelen;    /*一行打印的字节数*/
    char endprint_flag;

    if(SDKFONTMARK != LoadFont)
        return NDK_ERR;



    if((font_default=prn_list_getfont(prnfont_list, 1))==NULL) {
        return NDK_ERR;
    }
    prncustom_font = font_default;
    font_custm = prncustom_font;


    xpos=ndk_BDF_Xpos;

    //w=(font_custm->pf->maxwidth+7)/8;         /*取出的字模每行所占字节数*/
    //h=font_custm->pf->height;
    //fontbyte=h*w;                         /*字模所占字节数*/

    xmark=ndk_BDF_Xpos;
    i=0;
    while(pusText[i]) {
        if((pusText[i]<font_custm->pf->firstchar)||(pusText[i]>=font_custm->pf->firstchar+font_custm->pf->size)) {
            i++;
            continue ;
        }
        if(font_custm->pf->bbw==NULL)
            width = font_custm->pf->maxwidth;//16;//font_custm->pf->width[text[i]-font_custm->pf->firstchar];
        else
            width = (font_custm->pf->bbw[pusText[i]-font_custm->pf->firstchar]);
        w=((width+15)/16)*2;        /*取出的字模每行所占字节数*/
        h=font_custm->pf->height;
        fontbyte=h*w;                           /*字模所占字节数*/
        if(((xpos+width*ndk_BDF_Xmode)>384)) {
            /*
             *满一行的数据("\r"，"\n"，或者数据超过一行的容量)进行每行数据的状态属性保存，并malloc下一行的存储空间给后续使用
             */
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh =ndk_CurrLineHigh;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset =ndk_BDF_Xpos;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps +=ndk_BDF_LineSpace;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usmode=7;
            ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnGray = ndk_prngrey;
            //if (ndk_LineState == OpenLine){
            //  ndk_underend =ndk_lineoffset-ndk_columnblank;
            //  ndk_getunderline();
            //  }
            //if ((ndk_AlignType==RIGHTALIGN)||(ndk_AlignType==MIDALIGN))
            //  ndk_aligndel();
            //ndk_understart = ndk_leftblank;/*一行数据满后行属性各变量都恢复初始状态*/
            //ndk_underend = ndk_leftblank;
            //ndk_lineoffset = ndk_leftblank;
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
            }
            xpos=ndk_BDF_Xpos;

        }
        memset(tmp, 0, sizeof(tmp));
        //offset= font_custm->pf->offset[pusText[i]]*2;
        if(font_custm->pf->offset==NULL) {
            offset = 2*(pusText[i]-font_custm->pf->firstchar)*font_custm->pf->height;

        } else {
            offset= font_custm->pf->offset[pusText[i]-font_custm->pf->firstchar]*2;
        }

        for(k=0; k<fontbyte; k++) {
            j=(k/2)*2;
            tmp[k] = *((char *)(font_custm->pf->bits)+offset+j+1-k%2);
            //if(font_custm->pf->maxwidth>8){
            //  j=(k/2)*2;
            //  tmp[k] = *((char *)(font_custm->pf->bits)+offset+j+1-k%2);
            //}else{
            //  j=k*2;
            //  tmp[k] = *((char *)(font_custm->pf->bits)+offset+j+1);
            //}
            //fprintf(stderr,"%x ",tmp[k]);
        }
#if 1
        /*放大处理。目前支持放大倍数为8倍，且放大后字体最大不能超过48*48*/
        if(ndk_BDF_Xmode!= 1) {
            memset(multmpw, 0, sizeof(multmpw));
            prn_XFontMul(tmp, multmpw, w, h, ndk_BDF_Xmode, width);
            //w=((width*ndk_BDF_Xmode+15)/16)*2;
            w=(width*ndk_BDF_Xmode+7)/8;
        }

        if(ndk_BDF_Ymode!=1) {
            memset(multmph, 0, sizeof(multmph));
            if(ndk_BDF_Xmode!= 1)
                prn_YFontMul(multmpw, multmph, w, h, ndk_BDF_Ymode);
            else {
                prn_YFontMul(tmp, multmph, w, h, ndk_BDF_Ymode);
            }

            h*=ndk_BDF_Ymode;
        }
        //w=((width*ndk_BDF_Xmode+15)/16)*2;
        if(ndk_BDF_Ymode!=1) {
            memcpy(tmp, multmph, sizeof(multmph));
        } else if(ndk_BDF_Xmode!=1) {
            memcpy(tmp, multmpw, sizeof(multmpw));
        }
#endif

        ret = xpos%8;
        //prnw=(font_custm->pf->maxwidth*pmsg->SDKWmult+7)/8;
        for(k=0; k<h; k++) {                                        /*高度*/
            for (j=0; j<w; j++) {                                       /*字节数*/
                if (ret == 0) {
                    if ((xpos/8+j)<(384/8)) {
                        ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1-k][xpos/8+j]=tmp[(h-k-1)*w+j];
                    }
                } else {
                    if ((xpos/8+j)<(384/8)) {
                        ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1-k][xpos/8+j] |= tmp[(h-k-1)*w+j]>>ret;
                    }
                    if ((xpos/8+j+1)<(384/8)) {
                        ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1-k][xpos/8+j+1]|= tmp[(h-k-1)*w+j]<<(8-ret);
                    }
                }
            }
        }

        if( h > ndk_CurrLineHigh )
            ndk_CurrLineHigh=h;
        xpos += width*ndk_BDF_Xmode +ndk_BDF_WordSpace;
        i++;
        //w=(font_custm->pf->maxwidth+7)/8;         /*取出的字模每行所占字节数*/
        //h=font_custm->pf->height;
        //fontbyte=h*w;
    }

    //if(xpos != xmark){
    //  printDatasdk(384,  prnh, 0, &SDKPrnBuf[MAXH-prnh][0]);
    /// memset(SDKPrnBuf, 0, sizeof(SDKPrnBuf));
    //  FeedPrinterByPixelsdk(pmsg->SDKV_space);
    //}
    ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh =ndk_CurrLineHigh;
    ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset =ndk_BDF_Xpos;
    ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps +=ndk_BDF_LineSpace;
    ndk_gPrnDotlineData[ndk_DotlineNum]->usmode=7;
    ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnGray = ndk_prngrey;
    //if (ndk_LineState == OpenLine){
    //  ndk_underend =ndk_lineoffset-ndk_columnblank;
    //  ndk_getunderline();
    //  }
    //if ((ndk_AlignType==RIGHTALIGN)||(ndk_AlignType==MIDALIGN))
    //  ndk_aligndel();
    //ndk_understart = ndk_leftblank;/*一行数据满后行属性各变量都恢复初始状态*/
    //ndk_underend = ndk_leftblank;
    //ndk_lineoffset = ndk_leftblank;
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
    }

    //return 1;
    if(ndk_PrnDirSwitch == 0)
        return NDK_OK;
    else
        return ndk_PrnDirectly();



}

int ndk_TP_PrnCheckBDFPara(uint unXpos,uint unXmode,uint unYmode)
{
    int ret;
    PRN_SECTION_HEADER *font_custm, *font_default;
    int w,h;
    if(SDKFONTMARK != LoadFont)
        return NDK_ERR;

    if((font_default=prn_list_getfont(prnfont_list, 1))==NULL) {
        return NDK_ERR;
    }

    w=font_default->pf->maxwidth;       /*取出的字模每行所占字节数*/
    h=font_default->pf->height;
    fprintf(stderr,"check width:%d,height:%d\n",w,h);
    if(((w*unXmode+unXpos) > 384 )|| (h*unYmode > 48))
        return NDK_ERR_PARA;
    else
        return NDK_OK;

}








