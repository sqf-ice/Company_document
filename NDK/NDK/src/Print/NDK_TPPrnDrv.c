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

#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>



#include "NDK_TPPrint.h"
#include "NDK_TPPrnDrv.h"
#include "NDK_TPPrnFont.h"
#include "NDK.h"
#include "NDK_debug.h"


extern int NDK_FsOpen(const char *pszName,const char *pszMode);
extern int NDK_FsClose(int nHandle);
extern int NDK_FsRead(int nHandle, char *psBuffer, uint unLength );
extern int NDK_FsSeek(int nHandle, ulong ulDistance, uint unPosition );
extern int NDK_TP_PrnGetStatus(EM_PRN_STATUS *pemStatus);


extern int NDK_TP_PrnSetMode(EM_PRN_MODE emMode,uint unSigOrDou);



extern unsigned int ndk_BDF_Xpos;
extern unsigned int ndk_BDF_LineSpace;
extern unsigned int ndk_BDF_WordSpace;
extern unsigned int ndk_BDF_Xmode;
extern unsigned int ndk_BDF_Ymode;



unsigned char ndk_prnmode=7;    /**<打印模式0~7，默认为7*/
PrnDotlineData *ndk_gPrnDotlineData[201]= {NULL}; /**< 存储行点阵信息的指针数组，使用时使用动态分配，最大允许200行*/
unsigned int ndk_DotlineNum = 0; /**<当前已经存储的行数以及即将要存储到数组的位置*/
unsigned int ndk_Old_DotlineNum = 24; /**<记录上一行的高度，用于一直送\n时走纸步数*/
int ndk_HZ_Xchange=1; /**<用于字库不存在设置的基本字体时需要通过放大的倍数*/
int ndk_HZ_Ychange=1;
int ndk_ZM_Xchange=1;
int ndk_ZM_Ychange=1;
int ndk_usr_hz_select=0;    /**<是否通过NDK_PrnSetUsrFont设置了用户自定义的汉字字体*/
int ndk_usr_zm_select=0;    /**<是否通过NDK_PrnSetUsrFont设置了用户自定义的ASCII字体*/
int ndk_HZUserFontId =-1;    /**<通过NDK_PrnSetUsrFont设置了用户自定义的汉字字体id*/
int ndk_ZMUserFontId =-1;    /**<通过NDK_PrnSetUsrFont设置了用户自定义的ASSIC字体id*/
ST_PRN_FONTMSG ndk_UsrMsg[MaxFontNum];
unsigned int ndk_CurrLineHigh=0;                                    /*一行数据的高度*/
unsigned int ndk_lineoffset=0;
unsigned int ndk_prn_mode=3;
unsigned int ndk_PR_MAXLINEWIDE=384;
unsigned int ndk_hz12mark=0;
unsigned int ndk_asc12mark=0;
unsigned int ndk_understart = 0;                        /*下划线从第几个点开始*/
unsigned int ndk_underend = 0;                          /*下划线从第几个点结束*/
unsigned int ndk_columnblank= 0;                        /*字间距*/
unsigned int ndk_rowspace   = 0;                            /*行间距*/
unsigned int ndk_leftblank  = 0;                            /*左边距， 以点为单位*/
unsigned int ndk_LineState = UnableLine;
unsigned int ndk_AlignType = UNABLEALIGN;           /*对齐模式。默认取消对齐模式*/
unsigned int ndk_prngrey=-1;
unsigned int ndk_PrnDirSwitch=0;

unsigned int ndk_PrnIsSetMode = 0;                  /*是否已经设置了模式，由于要做到左偏移以及字间距不受模式的影响，所以只能在横向放大的情况下左偏移和字间距为原先的一半*/
static int prn_fd = -1;



//#define FONT_DEV  "/dev/mtd6"
#define FONT_DEV    "/guiapp/share/fonts/font_h.bin"

#define FONT_LEN    (0xbde40+0x600)
#define FONT_FILE_FOLDER    "/appfs/lib/font/"          /*打印字库在文件系统中的存储文件夹路径*/

static char CurrPrintFont=0x22;
static char CurrHzPrintFont=0x22;
static char ZMDotAscX=8;
static char ZMDotAscY=1;                                /*字体选择*/
static int CurrPrintZMDotAsc;


/*12x12的汉字字体点阵缓冲*/
static char g_puc12x12HZDotBuf[(16/8)*12];

/*24x24的汉字字体点阵缓冲*/
static char g_puc24x24HZDotBuf[24*24/8];

/*英文字体点阵缓冲,按照目前最大的点阵定义*/
static char g_pucAllZMDotBuf[32*32/8];
/*
 * 用于保存当前ASCII字体类型-楷体,MSGothic,Gulimche
 * NewCurrPrintFont/NewZMDotAscX/NewZMDotAscY
 *      -- 代表新英文字库字体的转换参数
 * */
static char NewCurrPrintFont=0x22;
static char NewZMDotAscX=8;
static char NewZMDotAscY= 1;

static stFontContrl *pCurZMFontSet = NULL;  /*打印默认使用自带宋体打印24X24汉字*/
static int FontZMType = -1;                 /* -1--表示使用旧的字母点阵， 0--使用新的英文字体*/

static unsigned char pstr[6*48];


#define PARAERR (0x1FF)


static int sendasc12tolinebuf(const char* Pdot);   /*12*12英文字库*/
static int send12tolinebuf(const char* Pdot);   /*12*12字库*/

extern int NDK_TP_PrnStr(const char *pszBuf);
extern EM_PRN_HZ_FONT currentPrnHZFont;
extern EM_PRN_ZM_FONT currentPrnZMFont;
extern EM_PRN_MODE currentMode;




/****************************************************************************
* 以下为新增英文字库相关数据
****************************************************************************/
static int func12en(const char *s)
{
    char i,j;
    short k;
    int offset=0;

    k = *s-0x21+0xA3A1;
    i =(char)((k & 0xff00)>>8)-0xa0;
    j =(char)(k & 0x00ff)-0xa0;

    offset = (94*(i-1) +j-1)*24;
    return((94*(i-1) +j-1)*24);
}
/*英文字体点阵偏移计算公式*/
static int fontzmoffset(const char *s)
{
    return ((*s-0x20)*((NewZMDotAscX+7)>>3)*NewZMDotAscY*8);
}

/****************************************************************************
* 以下为新增24x24点阵的汉字字库相关数据
****************************************************************************/
/*自带宋体点阵偏移计算公式*/
static int font24offsetGBK(const char *s)
{
    return ((s[0]-0x81)*192 + (s[1]-0x40))*sizeof(g_puc24x24HZDotBuf);
}

/*点阵工具生成的24点阵偏移计算公式*/
static int font24offsetGB2312(const char *s)
{
    /*GB2312字库的偏移公式*/
    return  ((s[0]-0xa1)*94 + (s[1]-0xa1))*sizeof(g_puc24x24HZDotBuf);
}

/*点阵工具生成的12点阵偏移计算公式*/
static int font12offsetGBK(const char *s)
{
    return (((s[0]-0x81)*190+s[1]-0x41)*24);
}


/*24x24的宋体字库结构*/
static stFontContrl ft_hz24a = {
    .fonttype = PRN_HZ_FONT_24x24A,
    .fontname = FONT_FILE_FOLDER"SongTiGBK_24.dzk",
    .ft_getdotoffset = font24offsetGBK
};

/*24x24的仿宋字库结构*/
static stFontContrl ft_hz24b = {
    .fonttype = PRN_HZ_FONT_24x24B,
    .fontname = FONT_FILE_FOLDER"FangSongGB2312_24.dzk",
    .ft_getdotoffset = font24offsetGB2312
};

/*24x24的楷体字库结构*/
static stFontContrl ft_hz24c = {
    .fonttype = PRN_HZ_FONT_24x24C,
    .fontname = FONT_FILE_FOLDER"KaiTiGB2312_24.dzk",
    .ft_getdotoffset = font24offsetGB2312
};

/*24x24的用户自定义字库结构*/
static stFontContrl ft_hz24user = {
    .fonttype = PRN_HZ_FONT_24x24USER,
    .fontname = FONT_FILE_FOLDER"CustomHanzi_24.dzk",
    .ft_getdotoffset = font24offsetGB2312
};

/*12x12的宋体字库结构*/
static stFontContrl ft_hz12a = {
    .fonttype = PRN_HZ_FONT_12x12A,
    .fontname = FONT_FILE_FOLDER"SongTiGBK_12.dzk",
    .ft_getdotoffset = font12offsetGBK
};

/*24x24的汉字字库数组，数组下标索引需要与字体宏定义值的大小顺序排列一致*/
static stFontContrl *gsHZFontArray[] = {
    &ft_hz24a,      /* HZ24x24A */
    &ft_hz24b,      /* HZ24x24B */
    &ft_hz24c,      /* HZ24x24C */
    &ft_hz24user,   /* HZ24x24USER */
    &ft_hz12a,      /*HZ12x12A */
    NULL
};
static stFontContrl *pCurHZFontSet = &ft_hz24a; /*打印默认使用自带宋体打印24X24汉字*/

static stFontContrl ft_dot12x16a = {    /*MSGothic粗体12x16点阵*/
    .fonttype = PRN_ZM_FONT_12x16A,
    .fontname = FONT_FILE_FOLDER"ASCII_A_12x16.bin",
    .ft_getdotoffset = fontzmoffset
};
static stFontContrl ft_dot12x24a = {    /*Gulimche字体12x24点阵*/
    .fonttype = PRN_ZM_FONT_12x24A,
    .fontname = FONT_FILE_FOLDER"ASCII_A_12x24.bin",
    .ft_getdotoffset = fontzmoffset
};
static stFontContrl ft_dot16x32a = {    /*MSGothic粗体16x32点阵*/
    .fonttype = PRN_ZM_FONT_16x32A,
    .fontname = FONT_FILE_FOLDER"ASCII_A_16x32.bin",
    .ft_getdotoffset = fontzmoffset
};
static stFontContrl ft_dot12x16b = {    /*MSGothic粗体12x16点阵*/
    .fonttype = PRN_ZM_FONT_12x16B,
    .fontname = FONT_FILE_FOLDER"ASCII_B_12x16.bin",
    .ft_getdotoffset = fontzmoffset
};
static stFontContrl ft_dot12x24b = {    /*MSGothic粗体12x24点阵*/
    .fonttype = PRN_ZM_FONT_12x24B,
    .fontname = FONT_FILE_FOLDER"ASCII_B_12x24.bin",
    .ft_getdotoffset = fontzmoffset
};
static stFontContrl ft_dot16x32b = {    /*MSGothic粗体16x32点阵*/
    .fonttype = PRN_ZM_FONT_16x32B,
    .fontname = FONT_FILE_FOLDER"ASCII_B_16x32.bin",
    .ft_getdotoffset = fontzmoffset
};
static stFontContrl ft_dot12x16c = {    /*楷体粗体12x16点阵*/
    .fonttype = PRN_ZM_FONT_12x16C,
    .fontname = FONT_FILE_FOLDER"ASCII_C_12x16.bin",
    .ft_getdotoffset = fontzmoffset
};
static stFontContrl ft_dot12x24c = {    /*楷体粗体12x24点阵*/
    .fonttype = PRN_ZM_FONT_12x24C,
    .fontname = FONT_FILE_FOLDER"ASCII_C_12x24.bin",
    .ft_getdotoffset = fontzmoffset
};
static stFontContrl ft_dot16x32c = {    /*楷体粗体16x32点阵*/
    .fonttype = PRN_ZM_FONT_16x32C,
    .fontname = FONT_FILE_FOLDER"ASCII_C_16x32.bin",
    .ft_getdotoffset = fontzmoffset
};

static stFontContrl ft_dot12x12= {
    .fonttype = PRN_ZM_FONT_12x12,
    .fontname = FONT_FILE_FOLDER"hzk12",
    .ft_getdotoffset = func12en
};
extern int NDK_TP_PrnSetFont(EM_PRN_HZ_FONT emHZFont,EM_PRN_ZM_FONT emZMFont);
void ndk_TP_PrnWarning()
{
    EM_PRN_HZ_FONT tmpPrnHZFont;
    EM_PRN_ZM_FONT tmpPrnZMFont;
    EM_PRN_MODE tmpMode;

    tmpPrnHZFont=currentPrnHZFont;
    tmpPrnZMFont=currentPrnZMFont;
    tmpMode=currentMode;
    
    NDK_TP_PrnSetMode(PRN_MODE_NORMAL,0);
    NDK_TP_PrnSetFont(PRN_HZ_FONT_24x24,PRN_ZM_FONT_24x32);
    NDK_TP_PrnStr("注意：本机为测试版本，本打印单页无效\r\r");
    NDK_TP_PrnSetMode(tmpMode,0);
    NDK_TP_PrnSetFont(tmpPrnHZFont,tmpPrnZMFont);   
}
static int GetNFFont(char *p, int offset, int size)  /*获取存取在NandFlash中的字库字模*/
{
    int fd;

    fd = open(FONT_DEV, O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    lseek(fd, offset, 0);
    read(fd, p, size);
    close(fd);
    return 0;
}

void ndk_aligndel(void)
{
#if 0
    int tmp=0;
    int m,k,umask;
    int i,j;
    int hight;

    if (ndk_AlignType == MIDALIGN)      /*居中对齐*/
        tmp = (ndk_PR_MAXLINEWIDE-ndk_lineoffset+ndk_columnblank)/2;
    if (ndk_AlignType == RIGHTALIGN)        /*右对齐*/
        tmp = (ndk_PR_MAXLINEWIDE-ndk_lineoffset+ndk_columnblank);

    memset(prnbuf, 0, sizeof(prnbuf));

    hight = ndk_CurrLineHigh;

    if (tmp%8 == 0) {
        if ((ndk_lineoffset-ndk_columnblank)%8 != 0)
            m=(ndk_lineoffset-ndk_columnblank)/8+1;
        else
            m=(ndk_lineoffset-ndk_columnblank)/8;

        for (i=(MAXHIGHT-hight); i<MAXHIGHT; i++) {
            for (j=0; j<m; j++) {
                prnbuf[i][tmp/8+j]=ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[i][j];
            }
        }
    } else {
        k = tmp%8;

        if ((ndk_lineoffset-ndk_columnblank)%8 != 0)
            m=(ndk_lineoffset-ndk_columnblank)/8+1;
        else
            m=(ndk_lineoffset-ndk_columnblank)/8;

        for (i=(MAXHIGHT-hight); i<MAXHIGHT; i++) {
            for (j=0; j<m; j++) {
                umask=0;
                umask = (ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[i][j]&0xff)>>k;
                prnbuf[i][tmp/8+j] |=umask;
                umask = (ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[i][j]&0xff)<<(8-k);
                prnbuf[i][tmp/8+j+1] |=umask;
            }
        }
    }
    memcpy(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData, prnbuf, sizeof(prnbuf));
#endif
}

/***********************************************************************************************************************/
//函数功能：划下划线
//函数说明：
/***********************************************************************************************************************/
int ndk_getunderline(void)
{
    int i;

    if ((ndk_LineState == OpenLine)&&(ndk_understart < ndk_underend)) {     /*一行数据结束时，下划线功能仍存在*/
        if (ndk_understart%8 == 0) {    /*下划线从整字节处开始*/
            for (i=ndk_understart/8; i<(ndk_underend/8); i++)
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1][i] =0xff;

            if (ndk_underend%8!=0)
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1][ndk_underend/8] |=0xff<<(8-ndk_underend%8);
        } else {
            ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1][ndk_understart/8]|= (0xff>>ndk_understart%8);

            for (i=(ndk_understart/8+1); i<(ndk_underend/8); i++)
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1][i] =0xff;

            if (ndk_underend%8!=0)
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1][ndk_underend/8] |=0xff<<(8-ndk_underend%8);
        }
    } else if ((ndk_LineState == CloseLine)&&(ndk_understart < ndk_underend)) {
        if (ndk_understart%8==0) {                  /*下划线从整字节处开始*/
            for (i=ndk_understart/8; i<(ndk_underend/8); i++)
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1][i] =0xff;

            if (ndk_underend%8!=0)
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1][ndk_underend/8] |=0xff<<(8-ndk_underend%8);
        } else    {                                 /*下划线不从整字节开始*/
            ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1][ndk_understart/8]|= (0xff>> (ndk_understart%8) );

            for (i=(ndk_understart/8+1); i<(ndk_underend/8); i++)
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1][i] =0xff;

            if (ndk_underend%8!=0)
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-1][ndk_underend/8] |=0xff<<(8-ndk_underend%8);
        }
    }
    return 0;
}

/*设置打印字体*/
static int g_printfont; /*用于备份点阵转换之前的字体*/
int ndk_SetFontMsg( int font)
{
    if((((unsigned int)font >> 8) & 0xff) != 0) {
        ndk_hz12mark=0;
        ndk_HZ_Xchange=1;
        ndk_HZ_Ychange=1;

    }
    if((font & 0xff) != 0) {
        g_printfont = font;
        FontZMType = -1;
        CurrPrintZMDotAsc=__byAsc8x8DotBuf; //打印字母的字体
        ndk_asc12mark=0;
        ndk_ZM_Xchange=1;
        ndk_ZM_Ychange=1;
    }

    switch (((unsigned int)font >> 8) & 0xff) {
        case PRN_HZ_FONT_24x24USER:
        case PRN_HZ_FONT_24x24A:
        case PRN_HZ_FONT_24x24B:
        case PRN_HZ_FONT_24x24C:
            pCurHZFontSet = gsHZFontArray[(((unsigned int)font >> 8)&0xff) - PRN_HZ_FONT_24x24A];   /*切换到指定的汉字字体--20100811 by lingb*/
            CurrHzPrintFont=0x33;
            break;
        case PRN_HZ_FONT_24x24:
            pCurHZFontSet = gsHZFontArray[0];   /*切换到默认宋体-20100811*/
            CurrHzPrintFont=0x33;
            break;
        case PRN_HZ_FONT_16x32:
            CurrHzPrintFont=0x24;
            break;
        case PRN_HZ_FONT_32x32:
            CurrHzPrintFont=0x44;
            break;
        case PRN_HZ_FONT_32x16:
            CurrHzPrintFont=0x42;
            break;
        case PRN_HZ_FONT_24x32:
            CurrHzPrintFont=0x34;
            break;
        case PRN_HZ_FONT_16x16:
            CurrHzPrintFont=0x22;
            break;
        case PRN_HZ_FONT_12x16:
            CurrHzPrintFont=0x52;
            break;
        case PRN_HZ_FONT_16x8:
            CurrHzPrintFont=0x12;   /* 实际对应HZ8x16*/
            break;
        case PRN_HZ_FONT_12x12A:
            pCurHZFontSet = gsHZFontArray[4];
            CurrHzPrintFont=0x22;
            ndk_hz12mark=1;
            break;
#if 1
        case PRN_HZ_FONT_24x48A:
        case PRN_HZ_FONT_24x48B:
        case PRN_HZ_FONT_24x48C:
            pCurHZFontSet = gsHZFontArray[(((unsigned int)font >> 8)&0xff) - PRN_HZ_FONT_24x48A];   /*切换到指定的汉字字体--20100811 by lingb*/
            CurrHzPrintFont=0x33;
            ndk_HZ_Ychange=2;
            break;
        case PRN_HZ_FONT_48x24A:
        case PRN_HZ_FONT_48x24B:
        case PRN_HZ_FONT_48x24C:
            pCurHZFontSet = gsHZFontArray[(((unsigned int)font >> 8)&0xff) - PRN_HZ_FONT_48x24A];   /*切换到指定的汉字字体--20100811 by lingb*/
            CurrHzPrintFont=0x33;
            ndk_HZ_Xchange=2;
            break;
        case PRN_HZ_FONT_48x48A:
        case PRN_HZ_FONT_48x48B:
        case PRN_HZ_FONT_48x48C:
            pCurHZFontSet = gsHZFontArray[(((unsigned int)font >> 8)&0xff) - PRN_HZ_FONT_48x48A];   /*切换到指定的汉字字体--20100811 by lingb*/
            CurrHzPrintFont=0x33;
            ndk_HZ_Xchange=2;
            ndk_HZ_Ychange=2;
            break;
        case 0:
            break;
#endif
        default:
            CurrHzPrintFont=0x22;
            break;
    }

    switch (font & 0xff) {
        case PRN_ZM_FONT_8x16:
            CurrPrintFont=0x22;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
            break;
        case PRN_ZM_FONT_8x32:
            CurrPrintFont=0x24;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
            break;
        case PRN_ZM_FONT_16x16:
            CurrPrintFont=0x42;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
            break;
        case PRN_ZM_FONT_16x32:
            CurrPrintFont=0x44;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
            break;
        case PRN_ZM_FONT_24x32:
            CurrPrintFont=0x64;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
            break;
        case PRN_ZM_FONT_6x8:
        case PRN_ZM_FONT_8x8:
            CurrPrintZMDotAsc=__byAsc8x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=1;
            CurrPrintFont=0x22;
            break;
        case PRN_ZM_FONT_5x7:
            CurrPrintZMDotAsc=__byAsc5x7DotBuf;
            ZMDotAscX=5;
            ZMDotAscY=1;
            CurrPrintFont=0x22;
            break;
        case PRN_ZM_FONT_5x16:
            CurrPrintZMDotAsc=__byAsc5x16DotBuf;
            ZMDotAscX=5;
            ZMDotAscY=2;
            CurrPrintFont=0x22;
            break;
        case PRN_ZM_FONT_10x16:
            CurrPrintZMDotAsc=__byAsc10x16DotBuf;
            ZMDotAscX=10;
            ZMDotAscY=2;
            CurrPrintFont=0x22;
            break;
        case PRN_ZM_FONT_10x8:
            CurrPrintZMDotAsc=__byAsc10x8DotBuf;
            ZMDotAscX=10;
            ZMDotAscY=1;
            CurrPrintFont=0x22;
            break;
            /*新增A、B、C分别代表字体"Gulimche","MSGothic","楷体" - 2010-8-14 by lingb*/
        case PRN_ZM_FONT_12x16A: {
            pCurZMFontSet = &ft_dot12x16a;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=12;
            NewZMDotAscY=2;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x22;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        break;
        case PRN_ZM_FONT_12x24A: {
            pCurZMFontSet = &ft_dot12x24a;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=12;
            NewZMDotAscY=3;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x42;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        break;
        case PRN_ZM_FONT_16x32A: {
            pCurZMFontSet = &ft_dot16x32a;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=16;
            NewZMDotAscY=4;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x44;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        break;
        case PRN_ZM_FONT_12x16B: {
            pCurZMFontSet = &ft_dot12x16b;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=12;
            NewZMDotAscY=2;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x22;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        break;
        case PRN_ZM_FONT_12x24B: {
            pCurZMFontSet = &ft_dot12x24b;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=12;
            NewZMDotAscY=3;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x42;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        break;
        case PRN_ZM_FONT_16x32B: {
            pCurZMFontSet = &ft_dot16x32b;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=16;
            NewZMDotAscY=4;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x44;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        break;
        case PRN_ZM_FONT_12x16C: {
            pCurZMFontSet = &ft_dot12x16c;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=12;
            NewZMDotAscY=2;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x22;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        break;
        case PRN_ZM_FONT_12x24C: {
            pCurZMFontSet = &ft_dot12x24c;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=12;
            NewZMDotAscY=3;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x42;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        break;
        case PRN_ZM_FONT_16x32C: {
            pCurZMFontSet = &ft_dot16x32c;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=16;
            NewZMDotAscY=4;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x44;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        break;

        case PRN_ZM_FONT_12x12:
            pCurZMFontSet = &ft_dot12x12;
            ndk_asc12mark=1;
            break;
        case PRN_ZM_FONT_24x24A: {
            pCurZMFontSet = &ft_dot12x24a;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=12;
            NewZMDotAscY=3;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x42;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        ndk_ZM_Xchange=2;
        break;
        case PRN_ZM_FONT_32x32A: {
            pCurZMFontSet = &ft_dot16x32a;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=16;
            NewZMDotAscY=4;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x44;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        ndk_ZM_Xchange=2;
        break;
        case PRN_ZM_FONT_24x24B: {
            pCurZMFontSet = &ft_dot12x24b;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=12;
            NewZMDotAscY=3;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x42;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        ndk_ZM_Xchange=2;
        break;
        case PRN_ZM_FONT_32x32B: {
            pCurZMFontSet = &ft_dot16x32b;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=16;
            NewZMDotAscY=4;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x44;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        ndk_ZM_Xchange=2;
        break;
        case PRN_ZM_FONT_24x24C: {
            pCurZMFontSet = &ft_dot12x24c;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=12;
            NewZMDotAscY=3;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x42;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        ndk_ZM_Xchange=2;
        break;
        case PRN_ZM_FONT_32x32C: {
            pCurZMFontSet = &ft_dot16x32c;
            NewCurrPrintFont=0x22;
            NewZMDotAscX=16;
            NewZMDotAscY=4;
            FontZMType = 0;
        }
        {
            CurrPrintFont=0x44;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
        }
        ndk_ZM_Xchange=2;
        break;
        case 0:
            break;
        default:
            CurrPrintZMDotAsc=__byAsc8x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=1;
            CurrPrintFont=0x22;
            break;
    }
    return 0;
}

/***********************************************************************************************
* 函数原型：
* 功能描述：取汉字字体点阵
* 输入参数：无
* 输出参数：无
* 返回值:   1 -- 取点阵成功
            0 -- 取点阵失败
***********************************************************************************************/
static int gethz24dot(const char *HZDot)
{
    int fp;
    int offset;
    if ((fp = NDK_FsOpen(pCurHZFontSet->fontname, "r")) < 0)
        return 0;
    offset = pCurHZFontSet->ft_getdotoffset(HZDot);
    NDK_FsSeek(fp, offset, SEEK_SET);
    NDK_FsRead(fp, (char *)g_puc24x24HZDotBuf, sizeof(g_puc24x24HZDotBuf));
    NDK_FsClose(fp);

    return 1;
}
/***********************************************************************************************
* 函数原型：
* 功能描述：取英文字符点阵
* 输入参数：无
* 输出参数：无
* 返回值:   1 -- 取点阵成功
            0 -- 取点阵失败
***********************************************************************************************/
static int getascdot(const char *HZDot)
{
    int fp;
    int offset;
    if ((fp = NDK_FsOpen(pCurZMFontSet->fontname, "r")) < 0)
        return 0;
    offset = pCurZMFontSet->ft_getdotoffset(HZDot);
    NDK_FsSeek(fp, offset, SEEK_SET);
    NDK_FsRead(fp, (char *)g_pucAllZMDotBuf, sizeof(g_pucAllZMDotBuf));
    NDK_FsClose(fp);
    return 1;
}

static int UniteLineBuf(unsigned char ucX, unsigned char ucY)
{
    int i, j;
    //int Ystart;   /*当前文字点阵缓冲的纵向区间起始点行*/
    //int Yend;     /*当前文字点阵缓冲的纵向区间终点点行*/
    //int BmpYstart;
    //int BmpYend;
    //fprintf(stderr,"unit_ndk_DotlineNum:%d\n",ndk_DotlineNum);
    //fprintf(stderr,"unit_ndk_lineoffset:%d\n",ndk_lineoffset);
    //fprintf(stderr,"cx:%d\n",ucX);
    //fprintf(stderr,"cy:%d\n",ucY);
#if 0
    if (pIntegratePrn && pIntegratePrn->TypeSetting == TPSET_AUTO) {    /*只有当文字环绕的时候才需要特别判断字符边界*/
        Ystart = TotalDotLine;
        i = (ndk_CurrLineHigh>ucY)?ndk_CurrLineHigh:ucY;                        /*按本行目前最高字体算*/
        Yend = TotalDotLine + i*8;
        BmpYstart = pIntegratePrn->ImageYPos ;
        BmpYend= (pIntegratePrn->ImageYPos + pIntegratePrn->sPrnImage.height);
        if ( BmpYstart >= Yend ||BmpYend <= Ystart) {                   /*纵向没有重合的区间则可以拷贝点阵*/
            ;
        } else if ((ndk_lineoffset + ucX) <= pIntegratePrn->sPrnImage.leftmargin) { /*图像左边空间足够存放当前字符点阵*/
            ;
        } else {
            if (ndk_lineoffset < (pIntegratePrn->sPrnImage.leftmargin + pIntegratePrn->sPrnImage.width)) {/*如果字符超过图像左边界则移动到图像右边*/
                ndk_lineoffset = (pIntegratePrn->sPrnImage.leftmargin + pIntegratePrn->sPrnImage.width);
            }
            if (ndk_lineoffset + ucX > ndk_PR_MAXLINEWIDE) {    /*若图像右边不够存放当前字符，则需要换行*/
                ndk_lineoffset = ndk_PR_MAXLINEWIDE;
                if (ndk_CurrLineHigh == 0) {    /*图像的位置使得当前行无法存下任何一个字符的时候*/
                    //打印图像高度为字符高度
                    ndk_CurrLineHigh = ucY;
                }
                return 1;
            }
        }
    }
#endif
    /*从内存中将转换结果拷贝到行缓冲的相应位置*/
    if (ndk_lineoffset%8) { /*行缓冲还没有整字节结束*/
        for (i = 0; i < (ucX+7)/8-1; i++) {
            for (j = 0; j < ucY*8; j++) {
                //prlinebuf[MAXHIGHT-ucY*8+j][ndk_lineoffset/8+i] |= ((*(g_pucFontBuf+j*6+i))>>(ndk_lineoffset&0x07));/*补足行缓冲该字节余下的位*/
                //prlinebuf[MAXHIGHT-ucY*8+j][ndk_lineoffset/8+i+1] |= ((*(g_pucFontBuf+j*6+i))<<(8-(ndk_lineoffset&0x07)));/*字体缓冲该字节剩下的位放入行缓冲下一字节*/
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-ucY*8+j][ndk_lineoffset/8+i] |= ((*(g_pucFontBuf+j*6+i))>>(ndk_lineoffset&0x07));
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-ucY*8+j][ndk_lineoffset/8+i+1] |= ((*(g_pucFontBuf+j*6+i))<<(8-(ndk_lineoffset&0x07)));
            }
        }
        if ((ndk_lineoffset/8+i+1) > ndk_PR_MAXLINEWIDE/8) { /*最后一个字节可能溢出行缓冲，因为字模有补零*/
            for (j = 0; j < ucY*8; j++) {
                //prlinebuf[MAXHIGHT-ucY*8+j][ndk_lineoffset/8+i] |= ((*(g_pucFontBuf+j*6+i))>>(ndk_lineoffset&0x07));/*补足行缓冲该字节余下的位*/
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-ucY*8+j][ndk_lineoffset/8+i] |= ((*(g_pucFontBuf+j*6+i))>>(ndk_lineoffset&0x07));
            }
        } else {
            for (j = 0; j < ucY*8; j++) {
                //prlinebuf[MAXHIGHT-ucY*8+j][ndk_lineoffset/8+i] |= ((*(g_pucFontBuf+j*6+i))>>(ndk_lineoffset&0x07));/*补足行缓冲该字节余下的位*/
                //prlinebuf[MAXHIGHT-ucY*8+j][ndk_lineoffset/8+i+1] |= ((*(g_pucFontBuf+j*6+i))<<(8-(ndk_lineoffset&0x07)));/*字体缓冲该字节剩下的位放入行缓冲下一字节*/
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-ucY*8+j][ndk_lineoffset/8+i] |= ((*(g_pucFontBuf+j*6+i))>>(ndk_lineoffset&0x07));
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-ucY*8+j][ndk_lineoffset/8+i+1] |= ((*(g_pucFontBuf+j*6+i))<<(8-(ndk_lineoffset&0x07)));
            }
        }
    } else { /*行缓冲已经整字节结束*/
        for (i=0; i<ucY*8; i++) {
            //memcpy(prlinebuf[MAXHIGHT-ucY*8+i]+ndk_lineoffset/8,(g_pucFontBuf+i*6),(ucX+7)/8);
            //fprintf(stderr,"cpy:%d\n",i);
            memcpy(ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-ucY*8+i]+ndk_lineoffset/8,(g_pucFontBuf+i*6),(ucX+7)/8);

        }
    }
    ndk_lineoffset+=(ucX+ndk_columnblank);

    ucY=ucY*8;

    if (ndk_CurrLineHigh<ucY)
        ndk_CurrLineHigh=ucY;
    //fprintf(stderr,"unit over\n");

    return 0;
}

int ndk_sendHZtolinebuf(const char * HZDot)
{
    char cX, cY, i, j;
    char buf[32];
    int offset;
    if(ndk_hz12mark==1) {
        return send12tolinebuf(HZDot);
    } else {
        offset=((HZDot[0]-0x81)*191+HZDot[1]-0x40)*32;//GB18030
        GetNFFont(buf, offset, sizeof(buf));
        //fprintf(stderr,"get font\n");
        if ((cX=(CurrHzPrintFont&0xf0)>>4)==5) { /*12*16*/
            if (ndk_lineoffset>(ndk_PR_MAXLINEWIDE-12)) {
                ndk_lineoffset=ndk_PR_MAXLINEWIDE;
                return 1;
            }

            ndk_DotChange(buf,3,2,16,2);
            /*中间结果按照dotchange参数格式要求拷贝*/
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 16; j++) {
                    pstr[3*j+i] = *(g_pucFontBuf+6*j+i);
                }
            }

            ndk_DotChange((char *)pstr,1,2,24,2);
            cX=12;
            cY=2;
        } else if ((CurrHzPrintFont == 0x33) && gethz24dot(HZDot) ) {   /*若是24x24的汉字点阵并且取点阵成功，则使用24点阵字库来打印*/

            cY=(CurrHzPrintFont&0x0f)*ndk_HZ_Ychange;
            if (ndk_lineoffset>(ndk_PR_MAXLINEWIDE-cX*8*ndk_HZ_Xchange)) {
                ndk_lineoffset=ndk_PR_MAXLINEWIDE;
                return 1;
            }
            ndk_DotChange(g_puc24x24HZDotBuf,2*ndk_HZ_Xchange,2*ndk_HZ_Ychange,24,3);
            cX=cX*8*ndk_HZ_Xchange;
        } else {
            cY=(CurrHzPrintFont&0x0f);
            if (ndk_lineoffset>(ndk_PR_MAXLINEWIDE-cX*8)) {
                ndk_lineoffset=ndk_PR_MAXLINEWIDE;
                return 1;
            }
            ndk_DotChange(buf,cX,cY,16,2);
            cX=cX*8;
        }
        return UniteLineBuf(cX, cY);
    }
}
int ndk_sendZMtolinebuf(char ZMDot)
{
    char cX,cY;
    char * ucZMdot;
    int offset;
    char buf[32*32/8]= {0};

    if (ndk_asc12mark==1) {
        return sendasc12tolinebuf(&ZMDot);
    } else {
        if(FontZMType == 0) {   /*新字体从字库文件获取点阵*/
            if(getascdot(&ZMDot)) {
                cX=(NewCurrPrintFont&0xf0)>>4;
                cY=NewCurrPrintFont&0x0f;
                if (ndk_lineoffset>(ndk_PR_MAXLINEWIDE-NewZMDotAscX*(cX>>1)*    ndk_ZM_Xchange)) {
                    ndk_lineoffset=ndk_PR_MAXLINEWIDE;
                    return 1;
                }
                ucZMdot = g_pucAllZMDotBuf;

                ndk_DotChange(ucZMdot,cX*ndk_ZM_Xchange,cY*ndk_ZM_Ychange,((NewZMDotAscX+7)>>3)<<3,NewZMDotAscY);
                cX=(NewZMDotAscX*(cX>>1))*ndk_ZM_Xchange;/*放大后的宽度*/
                cY=((cY>>1)*NewZMDotAscY)*ndk_ZM_Ychange;/*放大后的高度*/
                return UniteLineBuf(cX, cY);
            } else {
                offset=CurrPrintZMDotAsc+((ZMDot-0x20)*((ZMDotAscX+7)>>3)*ZMDotAscY*8);         /*字模宽度不足字节的位有补零*/
            }
        } else
            offset=CurrPrintZMDotAsc+((ZMDot-0x20)*((ZMDotAscX+7)>>3)*ZMDotAscY*8);             /*字模宽度不足字节的位有补零*/

        GetNFFont(buf, offset, ((ZMDotAscX+7)>>3)*ZMDotAscY*8);

        cX=(CurrPrintFont&0xf0)>>4;
        cY=CurrPrintFont&0x0f;

        if (ndk_lineoffset>(ndk_PR_MAXLINEWIDE-ZMDotAscX*(cX>>1))) {
            ndk_lineoffset=ndk_PR_MAXLINEWIDE;
            return 1;
        }

        ndk_DotChange(buf,cX,cY,((ZMDotAscX+7)>>3)<<3,ZMDotAscY);   //unlinrq
        cX=(ZMDotAscX*(cX>>1));/*放大后的宽度*/
        cY=((cY>>1)*ZMDotAscY);/*放大后的高度*/
        return UniteLineBuf(cX, cY);
    }
}

int ndk_sendUSRHZtolinebuf(const char* Pdot)   /*用户自定义字库*/
{
    int fp;
    int w,h,m,n;
    int tmp,i,j,k,umask;
    ST_PRN_RECMSG DisMsg;

    if (ndk_lineoffset>(ndk_PR_MAXLINEWIDE-ndk_UsrMsg[ndk_HZUserFontId].w)) {
        ndk_lineoffset = ndk_PR_MAXLINEWIDE;
        return 1;
    }

    if ((fp = NDK_FsOpen(ndk_UsrMsg[ndk_HZUserFontId].pszName, "r")) < 0)    /*查看设置的字库是否存在*/
        return 1;

    ndk_UsrMsg[ndk_HZUserFontId].func((char *)Pdot, &DisMsg);

    char fontbuf[DisMsg.nFontByte];
    memset(fontbuf,0,sizeof(fontbuf));
    NDK_FsSeek(fp, DisMsg.nOffset, SEEK_SET);
    NDK_FsRead(fp, fontbuf, DisMsg.nFontByte );
    NDK_FsClose(fp);

    if (ndk_UsrMsg[ndk_HZUserFontId].nDirection == 1) {                     /*纵向字库处理*/
        char fontbuf1[DisMsg.nFontByte];

        memset(fontbuf1,0,sizeof(fontbuf1));
        n=0;
        if (ndk_UsrMsg[ndk_HZUserFontId].w%8!=0)
            tmp = ndk_UsrMsg[ndk_HZUserFontId].w/8+1;
        else
            tmp = ndk_UsrMsg[ndk_HZUserFontId].w/8;

        for (i=0; i<tmp; i++) {
            for(k=0; k<8; k++) {
                umask = 0x80;
                umask >>=k;
                m=0;
                for(j=0; j<ndk_UsrMsg[ndk_HZUserFontId].h; j++) {
                    fontbuf1[n]|=((fontbuf[i+(ndk_UsrMsg[ndk_HZUserFontId].w/8)*j] & umask)<<k)>>m;

                    m++;
                    if((m%8 == 0)&&(m!=0)) {
                        n++;
                        m=0;
                    }
                }
            }
        }
        memcpy(fontbuf, fontbuf1, sizeof(fontbuf1));
    }

    w=DisMsg.nFontByte/ndk_UsrMsg[ndk_HZUserFontId].h;
    h=ndk_UsrMsg[ndk_HZUserFontId].h;

    if (ndk_UsrMsg[ndk_HZUserFontId].w%8==0) {
        m=ndk_UsrMsg[ndk_HZUserFontId].w/8;
    } else {
        m=ndk_UsrMsg[ndk_HZUserFontId].w/8+1;
    }

    if (ndk_CurrLineHigh<h)
        ndk_CurrLineHigh=h;

    tmp = ndk_lineoffset%8;

    for(i=0; i<h; i++) {
        for (j=0; j<m; j++) {
            if (tmp == 0) {
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-h+i][ndk_lineoffset/8+j]=fontbuf[i*w+j];
            } else {
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-h+i][ndk_lineoffset/8+j] |= fontbuf[i*w+j]>>tmp;

                if ((ndk_lineoffset/8+j)!=(ndk_PR_MAXLINEWIDE/8))
                    ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-h+i][ndk_lineoffset/8+j+1] |= fontbuf[i*w+j]<<(8-tmp);
            }
        }
    }

    ndk_lineoffset +=(ndk_UsrMsg[ndk_HZUserFontId].w+ndk_columnblank);
    return 0;
}


int ndk_sendUSRZMtolinebuf(const char* Pdot)   /*用户自定义字库*/
{
    int fp;
    int w,h,m,n;
    int tmp,i,j,k,umask;
    ST_PRN_RECMSG DisMsg;

    if (ndk_lineoffset>(ndk_PR_MAXLINEWIDE-ndk_UsrMsg[ndk_ZMUserFontId].w)) {
        ndk_lineoffset = ndk_PR_MAXLINEWIDE;
        return 1;
    }

    if ((fp = NDK_FsOpen(ndk_UsrMsg[ndk_ZMUserFontId].pszName, "r")) < 0)    /*查看设置的字库是否存在*/
        return 1;

    ndk_UsrMsg[ndk_ZMUserFontId].func((char *)Pdot, &DisMsg);

    char fontbuf[DisMsg.nFontByte];
    memset(fontbuf,0,sizeof(fontbuf));
    NDK_FsSeek(fp, DisMsg.nOffset, SEEK_SET);
    NDK_FsRead(fp, fontbuf, DisMsg.nFontByte );
    NDK_FsClose(fp);

    if (ndk_UsrMsg[ndk_ZMUserFontId].nDirection == 1) {                     /*纵向字库处理*/
        char fontbuf1[DisMsg.nFontByte];

        memset(fontbuf1,0,sizeof(fontbuf1));
        n=0;
        if (ndk_UsrMsg[ndk_ZMUserFontId].w%8!=0)
            tmp = ndk_UsrMsg[ndk_ZMUserFontId].w/8+1;
        else
            tmp = ndk_UsrMsg[ndk_ZMUserFontId].w/8;

        for (i=0; i<tmp; i++) {
            for(k=0; k<8; k++) {
                umask = 0x80;
                umask >>=k;
                m=0;
                for(j=0; j<ndk_UsrMsg[ndk_ZMUserFontId].h; j++) {
                    fontbuf1[n]|=((fontbuf[i+(ndk_UsrMsg[ndk_ZMUserFontId].w/8)*j] & umask)<<k)>>m;

                    m++;
                    if((m%8 == 0)&&(m!=0)) {
                        n++;
                        m=0;
                    }
                }
            }
        }
        memcpy(fontbuf, fontbuf1, sizeof(fontbuf1));
    }

    w=DisMsg.nFontByte/ndk_UsrMsg[ndk_ZMUserFontId].h;
    h=ndk_UsrMsg[ndk_ZMUserFontId].h;

    if (ndk_UsrMsg[ndk_ZMUserFontId].w%8==0) {
        m=ndk_UsrMsg[ndk_ZMUserFontId].w/8;
    } else {
        m=ndk_UsrMsg[ndk_ZMUserFontId].w/8+1;
    }

    if (ndk_CurrLineHigh<h)
        ndk_CurrLineHigh=h;

    tmp = ndk_lineoffset%8;

    for(i=0; i<h; i++) {
        for (j=0; j<m; j++) {
            if (tmp == 0) {
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-h+i][ndk_lineoffset/8+j]=fontbuf[i*w+j];
            } else {
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-h+i][ndk_lineoffset/8+j] |= fontbuf[i*w+j]>>tmp;

                if ((ndk_lineoffset/8+j)!=(ndk_PR_MAXLINEWIDE/8))
                    ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-h+i][ndk_lineoffset/8+j+1] |= fontbuf[i*w+j]<<(8-tmp);
            }
        }
    }

    ndk_lineoffset +=(ndk_UsrMsg[ndk_ZMUserFontId].w+ndk_columnblank);
    return 0;
}



static int send12tolinebuf(const char* Pdot)   /*12*12字库*/
{
    int fp;
    int w,h;
    int tmp,i,j;
    int offset;

    if (ndk_lineoffset>(ndk_PR_MAXLINEWIDE-12)) {
        ndk_lineoffset = ndk_PR_MAXLINEWIDE;
        return 1;
    }

    if ((fp = NDK_FsOpen(pCurHZFontSet->fontname, "r")) < 0) {
        return -1;
    }

    offset = pCurHZFontSet->ft_getdotoffset(Pdot);
    NDK_FsSeek(fp, offset, SEEK_SET);
    NDK_FsRead(fp, g_puc12x12HZDotBuf, sizeof(g_puc12x12HZDotBuf));
    NDK_FsClose(fp);

    //   FontChange(g_puc12x12HZDotBuf, 12, 12);

    tmp = ndk_lineoffset%8;
    w=2;
    h=12;

    for(i=0; i<h; i++) {
        for (j=0; j<w; j++) {
            //fprintf(stderr,"i:%d\n",i);
            //fprintf(stderr,"j:%d\n",j);
            //fprintf(stderr,"tmp:%d\n",tmp);
            //fprintf(stderr,"vlu:%d\n",ndk_lineoffset/8+j);
            //fprintf(stderr,"vlu-2:%d\n",ndk_PR_MAXLINEWIDE/8);
            if (tmp == 0) {
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-h+i][ndk_lineoffset/8+j]=g_puc12x12HZDotBuf[i*w+j];
            } else {
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-h+i][ndk_lineoffset/8+j] |= g_puc12x12HZDotBuf[i*w+j]>>tmp;
                if ((ndk_lineoffset/8+j+1)!=(ndk_PR_MAXLINEWIDE/8))
                    ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-h+i][ndk_lineoffset/8+j+1] |= g_puc12x12HZDotBuf[i*w+j]<<(8-tmp);
            }
        }

    }

    ndk_lineoffset +=(12+ndk_columnblank);
    if (ndk_CurrLineHigh<12)
        ndk_CurrLineHigh=12;

    return 0;
}

static int sendasc12tolinebuf(const char* Pdot)   /*12*12英文字库*/
{
    int fp;
    int w,h;
    int tmp,i,j;
    int offset;

    if (ndk_lineoffset>(ndk_PR_MAXLINEWIDE-12)) {
        ndk_lineoffset = ndk_PR_MAXLINEWIDE;
        return 1;
    }

    if ((fp = NDK_FsOpen(pCurZMFontSet->fontname, "r")) < 0) {
        return -1;
    }
    offset = pCurZMFontSet->ft_getdotoffset(Pdot);
    NDK_FsSeek(fp, offset, SEEK_SET);
    NDK_FsRead(fp, g_puc12x12HZDotBuf, sizeof(g_puc12x12HZDotBuf));
    NDK_FsClose(fp);

    tmp = ndk_lineoffset%8;

    w=2;

    h=12;

    for(i=0; i<h; i++) {
        for (j=0; j<w; j++) {
            if (tmp == 0) {
                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-h+i][ndk_lineoffset/8+j]=g_puc12x12HZDotBuf[i*w+j];
            } else {

                ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-h+i][ndk_lineoffset/8+j] |= g_puc12x12HZDotBuf[i*w+j]>>tmp;
                if ((ndk_lineoffset/8+j)!=(ndk_PR_MAXLINEWIDE/8))
                    ndk_gPrnDotlineData[ndk_DotlineNum]->ucData[MAXHIGHT-h+i][ndk_lineoffset/8+j+1] |= g_puc12x12HZDotBuf[i*w+j]<<(8-tmp);

            }
        }
    }

    ndk_lineoffset +=(12+ndk_columnblank);
    if (ndk_CurrLineHigh<12)
        ndk_CurrLineHigh=12;

    return 0;
}

int ndk_get_prn_fd(void)
{
    if (prn_fd == -1) {
        prn_fd = open(PRN_DEV_NAME, O_RDWR);
        if (prn_fd < 0)
            return -1;
    }
    return prn_fd;
}

void ndk_resetnewprintparam(void)
{
    ndk_SetFontMsg((PRN_HZ_FONT_16x16 << 8)|PRN_ZM_FONT_8x16);  /*设置默认字体*/
	currentPrnHZFont = PRN_HZ_FONT_16x16;
	currentPrnZMFont = PRN_ZM_FONT_8x16;
    NDK_TP_PrnSetMode(3,0);             /*模式恢复为正常模式*/
	currentMode = 3;
    ndk_columnblank= 0;                 /*字间距*/
    ndk_rowspace= 0;                    /*行间距*/
    ndk_leftblank= 0;                       /*左边距， 以点为单位*/
    ndk_LineState = UnableLine;             /*下划线启用标识，默认为关*/
    ndk_understart = 0;                 /*下划线从第几个点开始*/
    ndk_underend = 0;                   /*下划线从第几个点结束*/
    ndk_AlignType = UNABLEALIGN;
    ndk_lineoffset= ndk_leftblank;     /*每一行的起始位置为左偏移保持一致*/
    ndk_BDF_Xpos = 0;
    ndk_BDF_LineSpace = 0;
    ndk_BDF_WordSpace = 0;
    ndk_BDF_Xmode = 1;
    ndk_BDF_Ymode = 1;
}


int ndk_ClosePrinter(void)
{
    int ret;
    int fd;
    fd = ndk_get_prn_fd();
    if (fd < 0) {
        return -1;
    }

    ioctl(fd, PRN_IOCS_STOP, &ret);
    return ret;
}


int ndk_printData(int xsize,int ysize,int xpos,char *ImgBuf)
{
    int fd;
    prnimage r_prnimage, *p=&r_prnimage;
    prnparam r_prnparam, *param=&r_prnparam;
    int ret;

    //fprintf(stderr,"xsize:%d\n",xsize);
    //for(i =0;i<384;i++)
    //fprintf(stderr,"%02x ",ImgBuf[i]);
    if ( (xsize <= 0) || (ysize <=0) || (xpos <0) || ((xpos + xsize) > 384)||ImgBuf==NULL) {

        return -1;
    }
    fd = ndk_get_prn_fd();
    if (fd < 0) {
        return -1;
    }
    //ret = ioctl(fd, PRN_IOCG_PARAM, param);
    //if (ret < 0) {
    //  return -1;
    //}

    p->leftmargin  = xpos;
    p->width       = xsize;
    p->height      = ysize;
    p->buffer      = ImgBuf;

    ret = ioctl(fd, PRN_IOCT_DATA_NDK, p);
    if (ret < 0) {
        return NDK_ERR_IOCTL;
    }
    return 0;
}

int ndk_FeedPrinterByPixel (int nPixel)
{
    int ret = -1;
    int fd;

    if( nPixel < 0  ) { //linyl 2012.4.28 加入参数判断
        return ret;
    }
    if( nPixel == 0  ) { //linyl 2012.5.28 加入参数判断
        return 0;
    }

    fd = ndk_get_prn_fd();
    if (fd < 0) {
        return -1;
    }
    nPixel=nPixel<<1;
    ioctl(fd, PRN_IOCS_FEEDTP, nPixel);
    return 0;

}

int ndk_GetTotalMotorSteps(void)
{
    int fd;
    int ret;
    unsigned int motorsteps, *p=&motorsteps;

    fd = ndk_get_prn_fd();
    if (fd < 0) {
        return -1;
    }
    ret = ioctl(fd, PRN_IOCG_MOTORSTEPCOUNT, p);
    if (ret < 0) {
        return -1;
    }
    return motorsteps;
}

int ndk_IOSetMode(uint unMode)
{
    int fd;
    int ret;
    prnparam r_prnparam, *p=&r_prnparam;

    if ( (unMode <0) || (unMode > 7)) {
        return -2;
    }

    fd = ndk_get_prn_fd();
    if (fd < 0) {
        return -4;
    }


    ret = ioctl(fd, PRN_IOCG_PARAM, p);
    if (ret < 0) {
        return -5;
    }

    p->line_attr = unMode;    /*对驱动来讲，进行无放大模式。放大处理由API层进行*/
    ret = ioctl(fd, PRN_IOCS_PARAM, p);
    if (ret < 0) {
        return -5;
    }
    return 0;

}
int ndk_IOSetGreyScale(uint unGrey)
{

    int      fd;
    int      ret;

    if ( (unGrey <0) ||(unGrey > 5) ) {
        return -1;
    }
    fd = ndk_get_prn_fd();
    if (fd < 0) {
        return -2;
    }

    ret = ioctl(fd, PRN_IOCS_GREYSCALE, unGrey);
    if (ret < 0) {
        return -5;
    }
    return 0;

}
/**
 *   打印0到ndk_DotlineNum(不包括ndk_DotlineNum)中已有的点阵数据。并把ndk_DotlineNum偏移的数据复制到开始0的位置
 *   也就是每次调用ndk_PrnDirectly之后，有且仅有ndk_gPrnDotlineData[0]中有可能有数据。其他都会被打印出来
 */
int ndk_PrnDirectly(void)
{
    int i;
    int ret;
    EM_PRN_STATUS status;
    //fprintf(stderr,"ndk_DotlineNum:%d\n",ndk_DotlineNum);
    if(ndk_DotlineNum != 0) {
        for(i = 0; i<ndk_DotlineNum; i++) {
            ndk_IOSetGreyScale(ndk_gPrnDotlineData[i]->usPrnGray );
            ndk_IOSetMode(ndk_gPrnDotlineData[i]->usmode);
            ret=ndk_printData(384,  ndk_gPrnDotlineData[i]->usPrnLineHigh, 0, (char *)&ndk_gPrnDotlineData[i]->ucData[MAXHIGHT-ndk_gPrnDotlineData[i]->usPrnLineHigh]);
            if(ret < 0) {
                NDK_TP_PrnGetStatus(&status);
                NDK_LOG_DEBUG(NDK_LOG_MODULE_PRINT,"ndk_printData:ndk_DotlineNum[%d]of[%d] error,status:%d\n",ndk_DotlineNum,i,status);
                return status;
            }
            ndk_FeedPrinterByPixel(ndk_gPrnDotlineData[i]->usPrnSteps);


        }
        for(i=0; i<ndk_DotlineNum; i++) {
            free(ndk_gPrnDotlineData[i]);
            ndk_gPrnDotlineData[i]=NULL;
        }

        ndk_gPrnDotlineData[0] = ndk_gPrnDotlineData[ndk_DotlineNum];
        ndk_gPrnDotlineData[ndk_DotlineNum] = NULL;

        ndk_DotlineNum=0;
    }

    return NDK_OK;
}

