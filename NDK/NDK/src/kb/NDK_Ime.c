#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <libgen.h>
#include <pthread.h>
#include "PinYin.h"
#include "NDK.h"
#include "NDK_debug.h"
#include "kb.h"
#include "gui.h"

extern int kbd_fd;

extern void ndk_disprevstr(const char * str);
extern char * ndk_disp_save_data(int x,int y,int h);
extern void ndk_disp_restore_data(int x,int y,int h,char *data);
extern int NDK_SysBeep(void);
extern int NDK_AscToHex (const uchar* pszAsciiBuf, int nLen, uchar ucType, uchar* psBcdBuf);
extern void ndk_draw_image(int x, int y, int w, int h, image_t * img, int xoff, int yoff);
extern ndk_statusbar_t gndk_statusbar;

//定义热键
#define IME_RECYCLE_KEY     ZMK_VALUE
#define IME_NEXT_SCR        0X02//0x14
#define IME_PREV_SCR        0X01//0x13

#define EKEY_LEFT       0x13
#define EKEY_RIGHT    0x14

#define IME_ZERO ZERO
#define IME_ONE ONE
#define IME_TWO TWO
#define IME_THREE THREE
#define IME_FOUR FOUR
#define IME_FIVE FIVE
#define IME_SIX SIX
#define IME_SEVEN SEVEN
#define IME_EIGHT EIGHT
#define IME_NINE NINE

static uint CursorPosX;//,CursorPosY;
extern int NDK_ScrGetxy(uint *punX, uint *punY);
extern int NDK_ScrGotoxy(uint unX, uint unY);
extern int NDK_ScrAutoUpdate(int nNewauto, int *pnOldauto);
extern int NDK_ScrPrintf(const char *format, ...);
extern int NDK_ScrPrintf(const char *format, ...);
extern int NDK_KbGetCode(uint unTime, int *pnCode);
extern int NDK_ScrSetSpace(uint wspace,uint hspace);
extern int NDK_ScrRefresh(void);
extern int NDK_ScrDispString(uint unX,uint unY,const char *pszS,uint unMode);
extern int NDK_ScrPop(void);
static int getkeyvalue(int);
static int getkeyascii(int);
static int Method_Bihua(char *s);
static int NumPYInput(char *s);
static int Method_Bihua(char *s);
static void BHFindCode(void);
static int Method_symbol(char *s);
//#define MAXCHARPERLINE  16

#define NUMPY_NEXT_WORD 0x12
#define NUMPY_PREV_WORD 0x11

#define PUNCTUATION_STR (", .?!'\":;/()-+=_\\|~$&@[]{}#%^*<>`")
#define PUNCTUATION_LEN 33
#define INPUTMETHOD_PATH    "/guiapp/bin/handwrite/"
#define METHOD_BIHUA    "bihua"
#define METHOD_PINYIN   "pinyin"
static const char* BIHUA_BASE;
static const char *BIHUA_1_OFFSET;
static const char *BIHUA_2_OFFSET;
static const char *BIHUA_3_OFFSET;
static const char *BIHUA_4_OFFSET;
static const char *BIHUA_5_OFFSET;
static const char  *BHOffset;
//static const char *HZMBSTARTADDRESS;
static char * save_screen_select;
//数拼输入法存放外码列表
static volatile char gextCodeMultBuf[6][8];
volatile int CurrMethod = IME_NUMPY;  //当前输入法
int gimehandwrite = -1;

static int ime_y;
static int len = 0;
static char* pdata = NULL;

static int kbd_mode =0;

extern  pthread_mutex_t ndk_getkey_mutex;

#define FAIL (-1)
#define SUCC 0

int get_ime_y(void)
{
    int ret;

    ret = sys.view.h-1-sys.secinfo_h-4*sys.font->afontw;
    return ret;
}

/*-------------------------------------
Function:显示输入法状态行信息
Input:   s---输入外码串
-------------------------------------*/
static void DispStateLine(char *s)
{
    char tmps[8];
    int i;
    char tmpsp[26]="_				";
    uint tmpx, tmpy;
    rect_t rt;

    NDK_ScrGetxy(&tmpx,&tmpy);
    switch (CurrMethod) {
        case IME_ENGLISH:
            strcpy(tmps,"英文");
            break;
        case IME_NUMPY:
            strcpy(tmps,"数拼");
            break;
        case IME_QUWEI:
            strcpy(tmps,"区位");
            break;
        case IME_NUM:
            strcpy(tmps,"数字");
            break;
        case IME_BIHUA:
            strcpy(tmps,"笔划");
            break;
        case IME_GBK:
            strcpy(tmps,"内码");
            break;
        case IME_HANDWRITE:
            strcpy(tmps,"手写");
            strcpy(tmpsp,"加载中...");
            break;
    }
    if (s[0]) {
        for (i=0; i<25; i++) {
            if (s[i])
                tmpsp[i]=s[i];
            else {
                tmpsp[i]='_';
                break;
            }
        }
        tmpsp[25]=0;
    }
    NDK_ScrGotoxy(0, ime_y + sys.font->fonth);
    NDK_ScrAutoUpdate(0,NULL);
    rt.x = 0;
    rt.y = ime_y+sys.font->fonth+sys.view.y;
    rt.w = sys.video->sur->width;
    rt.h = (sys.font->fonth+sys.hspace);

	if(sys.backimg==NULL) {
		sys.video->sur->ops->fill_rect(sys.video->sur, &rt, sys.bg);
    } 
	else
	{
		sys.video->sur->ops->fill_rect(sys.video->sur, &rt, sys.bg);
		ndk_draw_image(rt.x-sys.view.x,rt.y-sys.view.y,rt.w,rt.h,sys.backimg,rt.x,rt.y-sys.view.y);
	}
    NDK_ScrGotoxy(0, ime_y + sys.font->fonth);
    NDK_ScrAutoUpdate(1,NULL);
    NDK_ScrPrintf("%s:%s", tmps, tmpsp);
    NDK_ScrGotoxy(tmpx, tmpy);
}


/*-----------------------------------------
Function:在屏幕上显示重码
Input:   s---存放重码的字符串
-----------------------------------------*/
static void DispMultCodeLine(char *s,int flag)
{
    int i,range;
    char  tmps[PUNCTUATION_LEN+1];
    uint tmpx, tmpy;
    rect_t rt;

    NDK_ScrGetxy(&tmpx,&tmpy);
    // 将s复制到tmps，不足部分用空格补齐
    if(flag==0)
    {
        range=(sys.video->sur->width/(sys.font->afontw)/3)*3;
        if(range>30)
            range=30;
    }
    else
    {
        range=sys.video->sur->width/(sys.font->afontw);
        if(range>PUNCTUATION_LEN)
            range=PUNCTUATION_LEN;
    }
    for (i=0; i<range; i++) {
        if (s[i])
            tmps[i]=s[i];
        else {
            break;
        }
    }
    while (i<range) {
        tmps[i++]=' ';
    }
    tmps[i]=0;
    
    NDK_ScrGotoxy(0, ime_y);
    NDK_ScrAutoUpdate(0,NULL);
    rt.x = 0;
    rt.y = ime_y+sys.view.y;
    rt.w = sys.video->sur->width;
    rt.h = (sys.font->fonth+sys.hspace);
	if(sys.backimg==NULL) {
		sys.video->sur->ops->fill_rect(sys.video->sur, &rt, sys.bg);
    } 
	else
	{
	   sys.video->sur->ops->fill_rect(sys.video->sur, &rt, sys.bg);
       ndk_draw_image(rt.x-sys.view.x,rt.y-sys.view.y,rt.w,rt.h,sys.backimg,rt.x,rt.y-sys.view.y);
	}
    NDK_ScrGotoxy(0, ime_y);
    NDK_ScrPrintf("%s", tmps);
    NDK_ScrRefresh();
    NDK_ScrAutoUpdate(1,NULL);
    NDK_ScrGotoxy(tmpx, tmpy);
}


//////////////////////////////////////////////////////////////////////////
//以下是通用模块


#define         py_table                0x0//(0x03f970) //拼音在码表的开始位置

//#define     MAX_NUM_PER_LINE    (MAXCHARPERLINE/3)*2  //黑白每行最大汉字数 //彩屏每行最大汉字数
//#define     MAX_ASCII_PER_LINE  (MAXCHARPERLINE)  //每行最大字符数

#define     MAX_MULTICODE_NUM   480  // 新全拼的最大重码达 469 个。
#define     Word    unsigned int      //原为short
#define     Byte    unsigned char
#define     UINT    unsigned int


//外码缓存及其当前外码个数
static char        ExtCodeBuf[20];
static unsigned char       ExtCodeNum;

//汉字重码缓存及当前重码个数
static char        MultCodeBuf[MAX_MULTICODE_NUM*2+4];  // 新全拼的最大重码达 469 个。
static int     MultCodeNum;  // modify from "char" to "int" by Lingo  2000.05.24

//码表指针
static int     MultCodePtr;  //offset from MultCodeBuf of displayed chinese words

//屏幕显示重码汉字的缓存及当前显示的汉字个数
static char DispMultBuf[34];  //屏幕显示的多码缓冲区，格式如下"1好2号3浩"
static int  NumOfDispMultBuf;  //显示缓冲区中个数

static int  FirstDspPos=0;  //第一个显示的汉字在MultCodBuf的位置


//交换高低字节
static void ExchLowHigh(char * MultCodeBuf,int Num)
{
    char PCh[MAX_MULTICODE_NUM*2+4] ;
    int i;

    for (i=0; i<Num; i++) {
        PCh[i*2+1]=MultCodeBuf[i*2];
        PCh[i*2]=MultCodeBuf[i*2+1];
    }
    memcpy(MultCodeBuf,PCh,Num*2);
}


/*-------------------------------------
Function:判断MultCodeBuf中是否已经有Hz。
Return:  1---SUCC
         0---FAIL
-------------------------------------*/
static int AlreadyInMCB(char *Hz)
{
    int i;
    for (i=0; i<MultCodeNum; i++)
        if (strncmp(Hz,MultCodeBuf+i*2,2)==0)
            return 1;
    return 0;
}

#define Py_List2  0x6de6
#define Py_List3  0
static unsigned char Py_List1[]= {
    0x00,0x00, 0x0f,0x00, 0x52,0x00, 0xf6,0x00, 0x57,0x01, 0x66,0x01, 0x8b,0x01, 0xdd,0x01, 0x33,0x02, 0x33,0x02,
    0x73,0x02, 0xc5,0x02, 0x32,0x03, 0x80,0x03, 0xed,0x03, 0xf2,0x03, 0x39,0x04, 0x80,0x04, 0xbb,0x04, 0x5e,0x05,
    0xaf,0x05, 0xaf,0x05, 0xaf,0x05, 0xd4,0x05, 0x14,0x06, 0x58,0x06, 0x05,0x07
};


/*----------------------------------------------
Function:将CodeBuf中外码对应的所有汉字内码放入
         MultCodeBuf中。
Input:   ExtCodeBuf[]---全局变量，存放外码
         CodePtr---全局变量，指向码表
Output:  MultCodeBuf[]---所有对应的汉字内码
----------------------------------------------*/
static void PyFindCode()
{
    char buffer[2048];
	unsigned char   Hz[2];

    FILE* fp = NULL;
    int j,num,ptr=0;
	if(pdata==NULL){
    	fp = fopen( "/guiapp/bin/handwrite/li.dat", "rb" );   //载入库文件
    	if ( NULL == fp )
    	{
    		return;
    	}
    	fseek( fp, 0, SEEK_END );
    	len = ftell(fp);
    	fseek( fp, 0, SEEK_SET );
    	pdata = (char*)malloc( len );//分配内存
    	if(pdata==NULL){
    		fclose(fp);
    		return;
    		}
    	fread( pdata, 1, len, fp );
    	fclose( fp );
	}
	jtPinYin_SetParam_CodePage( jtHWR_FUNC_CODEPAGE_GBK );	
    num=jtPinYin_SearchWord( pdata, len, (unsigned short*)ExtCodeBuf, (unsigned short*)buffer, 960 );
	MultCodeNum=num;
    if (MultCodeNum>=MAX_MULTICODE_NUM)
	   MultCodeNum=MAX_MULTICODE_NUM; 
	for (j=0;j<num;j++) {
        memcpy( Hz, buffer+ptr, 2 );
        ptr=ptr+2;
        //if (AlreadyInMCB((char *)&Hz)) continue;
        memcpy(MultCodeBuf+MultCodePtr,Hz,2);
        MultCodePtr=MultCodePtr+2;
    }
}

/*---------------------------------------------------------------
Function: 从MultCodeBuf的FstDspPos位置显示MAX_NUM_PER_LINE个汉字
Input:    FstDspPos---指向MultCodeBuf中要显示的第一个字符
---------------------------------------------------------------*/
static void DispMultCode()
{
    int i,range;
    int tmpDspPos;
    if ((FirstDspPos>=MultCodeNum)||(MultCodeNum==0)) {
        FirstDspPos=MultCodeNum;
        return ;  //超过范围
    }
    range=(sys.video->sur->width/(sys.font->afontw)/3);
    if(range>10)
        range=10;
    if (FirstDspPos<0)
        FirstDspPos=0;
    tmpDspPos=FirstDspPos;
    memset(DispMultBuf,0,sizeof(DispMultBuf));
    for (i=0; i<range; i++)  {
		if(i==9)
			DispMultBuf[i*3]='0';
		else
        	DispMultBuf[i*3]='1'+i;
        DispMultBuf[i*3+1]=MultCodeBuf[tmpDspPos*2];
        DispMultBuf[i*3+2]=MultCodeBuf[tmpDspPos*2+1];
        tmpDspPos++;
        if (tmpDspPos>=MultCodeNum) {
            i++;
            break;
        }
    }
    NumOfDispMultBuf=i;
    DispMultCodeLine(DispMultBuf,0);
}


/**
*   @fn    DispState(si)
*   @brief
*   @param 无
*   @return 无
*
*/
static void DispState(char *buff)
{
    int i,j,range;

    if ((FirstDspPos>=MultCodeNum)||(MultCodeNum==0)) {
        FirstDspPos=MultCodeNum;
        return ;  //超过范围
    }
    range=sys.video->sur->width/sys.font->afontw;
    if(range>PUNCTUATION_LEN)
        range=PUNCTUATION_LEN;
    if (FirstDspPos<0)
        FirstDspPos=0;
    memset(DispMultBuf, 0, sizeof(DispMultBuf));
    for (i = 0,j=0; (FirstDspPos+i < MultCodeNum); i++,j++) { //(i < num) && //实现换行功能
        memcpy(DispMultBuf+i , buff + FirstDspPos+i, 1);

        if ((i+1)>=range) {
            i++;
            break;
        }
    }

    NumOfDispMultBuf = i;
    DispMultCodeLine(DispMultBuf,1);
}

//清除重码缓冲区
static void ClearMultCodeBuf()
{
    MultCodeNum=0;
    MultCodePtr=0;
    memset(MultCodeBuf,0,sizeof(MultCodeBuf));
}


//清除显示重码缓冲区
static void ClearDispMultBuf()
{
    memset(DispMultBuf,0,sizeof(DispMultBuf));
    NumOfDispMultBuf=0;
    FirstDspPos=0;
}


//清除外码缓冲区
static void ClearExtCodeBuf()
{
    ExtCodeNum=0;
    memset(ExtCodeBuf,0,sizeof(ExtCodeBuf));
}

/*------------------------------------------------------
Function: 利用ExtCodeBuf中的外码取汉字到MultCodeBuf,
          然后显示N个汉字到屏幕
------------------------------------------------------*/
static void InputMethod(int lMethod)
{
    char    tmpCodeBuf[sizeof(ExtCodeBuf)];
    char    tmpCodeNum;


    ClearMultCodeBuf();
    ClearDispMultBuf();
    memcpy(tmpCodeBuf,ExtCodeBuf,sizeof(ExtCodeBuf));
    tmpCodeNum=ExtCodeNum;

    for (; ExtCodeNum > 0; ExtCodeNum--) {
        switch (lMethod) {
            case PINYIN:
                PyFindCode();       //拼音输入法
                break;
            case BIHUA:
                BHFindCode();       //笔划输入法
                break;
            default:
                break;
        }
        if (MultCodeNum>0) {
            DispMultCode();
            break;
        }
    }
    if (!ExtCodeNum)    {
        ClearDispMultBuf();
        ClearMultCodeBuf();
    }
    if (MultCodeNum==0) {
        DispMultCodeLine("",1);
    }
    memcpy(ExtCodeBuf,tmpCodeBuf,sizeof(ExtCodeBuf));
    tmpCodeNum=ExtCodeNum=tmpCodeNum;
}

static int kbd_setmode(int mode);

static void onmethodchange()
{
    switch (CurrMethod) {
        case IME_ENGLISH:
        case IME_GBK:
        case IME_HANDWRITE:
            NDK_ScrGotoxy(5*sys.font->afontw,ime_y + sys.font->fonth);
            kbd_setmode(1);
            break;
        case IME_QUWEI:
        case IME_NUMPY:
        case IME_BIHUA:
            NDK_ScrGotoxy(5*sys.font->afontw,ime_y + sys.font->fonth);
            kbd_setmode(0);
            break;
    }

}

static void init_ime(void)
{
    int fd;
    struct stat finfo;
    if(BIHUA_BASE==NULL) {
        if ((fd = open(INPUTMETHOD_PATH METHOD_BIHUA, O_RDONLY)) >= 0) {
            fstat(fd, &finfo);
            BIHUA_BASE = mmap(NULL, finfo.st_size, PROT_READ, MAP_SHARED, fd, 0);
            close(fd);

            if (BIHUA_BASE != MAP_FAILED) {
                BIHUA_1_OFFSET = BIHUA_BASE;
                BIHUA_2_OFFSET = BIHUA_1_OFFSET + strlen(BIHUA_1_OFFSET) + 1;
                BIHUA_3_OFFSET = BIHUA_2_OFFSET + strlen(BIHUA_2_OFFSET) + 1;
                BIHUA_4_OFFSET = BIHUA_3_OFFSET + strlen(BIHUA_3_OFFSET) + 1;
                BIHUA_5_OFFSET = BIHUA_4_OFFSET + strlen(BIHUA_4_OFFSET) + 1;
                BHOffset = BIHUA_BASE;
            } else
                BIHUA_BASE = NULL;
        }

    }
    /*if(HZMBSTARTADDRESS==NULL) {
        if ((fd = open(INPUTMETHOD_PATH METHOD_PINYIN, O_RDONLY)) >= 0) {
            fstat(fd, &finfo);
            HZMBSTARTADDRESS = mmap(NULL, finfo.st_size, PROT_READ, MAP_SHARED, fd, 0);
            close(fd);
            if (HZMBSTARTADDRESS== MAP_FAILED)
                HZMBSTARTADDRESS = NULL;
        }
    }*/
}



/*------------------------------------------------------------
Function: 输入一个汉字
Output:   s---返回的字符在这里，以\0为结束
Return:   实际所输入的字节个数，一个汉字2个字节。
      1---输入ASCII
      2---输入一个汉字
------------------------------------------------------------*/
static int GetOneHZ(char *s)
{
    int   ch;
    int   tmpi;
    int range;
    
    ime_y = get_ime_y();
    ClearExtCodeBuf();
    ClearDispMultBuf();
    ClearMultCodeBuf();
    init_ime();
    onmethodchange();
    DispStateLine("");
    DispMultCodeLine("",1);
    range=(sys.video->sur->width/(sys.font->afontw)/3);
    if(range>10)
        range=10;
    while (1) {
        if (CurrMethod == IME_NUM) {
            NDK_KbGetCode(0, &ch);
        } else if(CurrMethod ==IME_HANDWRITE) {
//          NDK_KbGetCode(0, &ch);
//          if (ch>='0'&&ch<='9')
            return 3;
        } else if (CurrMethod==IME_QUWEI) {
            ch=getkeyvalue(0);
        } else if (CurrMethod == IME_NUMPY) {
            ch = NumPYInput(s);
            if (ch == -1)
                return -1;
            else if (ch == 0)
                return 2;
            else if (ch == 1)
                return 1;
        } else if(CurrMethod ==IME_BIHUA) {
            ch = Method_Bihua(s);
            if ((ch == -1)||(ch == 2)||(ch == 1))
                return ch;
        } else if(CurrMethod==IME_GBK) {
            ch=getkeyascii(0);
        } else {
            ch=getkeyvalue(0);
        }
        if (ch==IME_RECYCLE_KEY) {
            CurrMethod=(CurrMethod==IME_MAXNUM)?0:(CurrMethod+1);
            if(CurrMethod==IME_GBK)
                CurrMethod++;
            if((gimehandwrite==0)&&(CurrMethod == IME_HANDWRITE)) { //无触屏机型不支持手写输入
                CurrMethod=(CurrMethod==IME_MAXNUM)?0:(CurrMethod+1);
            }
            ClearExtCodeBuf();
            ClearDispMultBuf();
            ClearMultCodeBuf();
            DispStateLine("");
            DispMultCodeLine("",1);

            onmethodchange();

            continue;
        }

        else if(ch==(KBD_LONGPRESSED|IME_RECYCLE_KEY)) {

            s[0] = '#';
            s[1]=0;
            return 1;
        }

        else if(ch ==(KBD_LONGPRESSED|DOT_VALUE)) {
            s[0] = '*';
            s[1]=0;
            return 1;
        }

        else if(ch==IME_ZERO && (CurrMethod != IME_NUM)&&(CurrMethod!=IME_QUWEI)&&(CurrMethod!=IME_GBK)) {
            ch = Method_symbol(s);
            if(ch==0)
                continue;
            if((ch==1)&&(s[0]==IME_RECYCLE_KEY)) {
                CurrMethod=(CurrMethod==IME_MAXNUM)?0:(CurrMethod+1);
                if(CurrMethod==IME_GBK)
                    CurrMethod++;
                if((gimehandwrite==0)&&(CurrMethod == IME_HANDWRITE)) { //无触屏机型不支持手写输入
                    CurrMethod=(CurrMethod==IME_MAXNUM)?0:(CurrMethod+1);
                }
                ClearExtCodeBuf();
                ClearDispMultBuf();
                ClearMultCodeBuf();
                DispStateLine("");
                DispMultCodeLine("",1);

                onmethodchange();

                continue;
            } else
                return 1;
        } else if(CurrMethod==IME_GBK) {
            s[0]=ch;
            s[1]=ch>>8;
            s[2]=0;

            if(s[1]==0) {
                return 1;
            } else {
                if((s[0]&0x80)==0) {
                    DispStateLine("");
                    DispMultCodeLine("",1);
                    continue;
                }
                return 2;
            }
        }
        //英文输入法
        if ((CurrMethod == IME_NUM)||(CurrMethod==IME_ENGLISH)) {
            s[0]=ch;
            s[1]=0;
            return 1;
        }

        //区位码输入法
        if (CurrMethod==IME_QUWEI) {
            if (ch==ESC_VALUE) {
                s[0]=ESC_VALUE;
                s[1]=0;
                return 1;
            }

            if (ch==BASP_VALUE) {
                if (!ExtCodeNum)    {
                    s[0]=ch;
                    s[1]=0;
                    return 1;
                }
                if (ExtCodeNum>0) {
                    CursorPosX--;
                    ExtCodeNum--;
                    ExtCodeBuf[ExtCodeNum]=0;
                    DispStateLine(ExtCodeBuf);
                }
            }
            if (ch==ENTER_VALUE) {
                if (ExtCodeNum>0) {
                    ClearExtCodeBuf();
                    DispStateLine("");
                    NDK_ScrGotoxy(7*sys.font->afontw,ime_y + sys.font->fonth);
                } else {
                    s[0]=ch;
                    s[1]=0;
                    return 1;
                }
            }


            /*-------------------------------------
            区位码范围：0101～0194，
                        0201～0294，
                        ……
                        8601～8694，
                        8701～8793。
            -------------------------------------*/
            if (ch>='0'&&ch<='9') {
                //第一位不能是‘9’
                if (ExtCodeNum==0&&ch=='9')
                    continue;
                //后两位不能超过‘94’
                if (ExtCodeNum==3&&ExtCodeBuf[2]=='9') {
                    if (ch>'4')
                        continue;
                }
                //头两位和后两位都不能是‘00’
                if ((ExtCodeNum==1&&ExtCodeBuf[0]=='0')||(ExtCodeNum==3&&ExtCodeBuf[2]=='0')) {
                    if (ch=='0')
                        continue;
                }
                if (ExtCodeNum==1&&ExtCodeBuf[0]=='8') {
                    if (ch>'7')
                        continue;
                }

                ExtCodeBuf[ExtCodeNum++]=ch;
                ExtCodeBuf[ExtCodeNum]=0;
                DispStateLine(ExtCodeBuf);
                CursorPosX++;
                if (ExtCodeNum==4) {
                    s[0]=(ExtCodeBuf[0]-'0')*10+(ExtCodeBuf[1]-'0')+160;
                    s[1]=(ExtCodeBuf[2]-'0')*10+(ExtCodeBuf[3]-'0')+160;
                    s[2]=0;
                    return 2;
                }
            }
            continue;
        }  //区位码

        //拼音输入法
        switch (ch) {
            case EKEY_LEFT:
            case IME_PREV_SCR:
                if (!FirstDspPos) //第一组汉字
                    break;
                FirstDspPos-=range;
                DispMultCode();  //在输入法状态行上方显示更新后的重码汉字

                break;
            case EKEY_RIGHT:
            case IME_NEXT_SCR:
                if ((FirstDspPos+NumOfDispMultBuf+1)>MultCodeNum) //最后一组汉字
                    ;
                else
                    FirstDspPos+=range;
                DispMultCode();  //在输入法状态行上方显示更新后的重码汉字
                break;
            case BASP_VALUE:
                if (ExtCodeNum==0) { //没有输入外码
                    s[0]=ch;
                    s[1]=0;
                    return 1;
                } else { //还有外码
                    uint x,y;
                    NDK_ScrGetxy(&x, &y);
                    NDK_ScrGotoxy(x-sys.font->afontw,y);
                    CursorPosX--;
                    ExtCodeNum--;
                    ExtCodeBuf[ExtCodeNum]=0;
                    InputMethod(CurrMethod);
                    DispStateLine(ExtCodeBuf);   //在状态行显示输入字母
                }

                break;
            case '\r':
                if (ExtCodeNum>0) {
                    ClearExtCodeBuf();
                    InputMethod(CurrMethod);
                    DispStateLine("");
                    NDK_ScrGotoxy(5*sys.font->afontw,ime_y + sys.font->fonth);
                } else {
                    s[0]=ch;
                    s[1]=0;
                    return 1;
                }
                break;
            default:
                if ((ch>='a')&&(ch<='z') ) { //---小写
                    if (ExtCodeNum<sizeof(ExtCodeBuf)-1) {
                        uint x,y;
                        NDK_ScrGetxy(&x, &y);
                        NDK_ScrGotoxy(x+sys.font->afontw,y);
                        ExtCodeBuf[ExtCodeNum++]=ch;
                        CursorPosX++;
                    }
                    InputMethod(CurrMethod);
                    DispStateLine(ExtCodeBuf);//在状态行显示输入字母
                } else  if ((ch>='A')&&(ch<='Z') ) { //---大写字母
                    s[0]=ch;
                    s[1]=0;
                    return 1;
                } else if ((ch>='0')&&(ch<='9') ) { //---数字
                    //选择一个汉字
                    tmpi=ch-'0';    //选择从1开始
                    if ( (tmpi<=NumOfDispMultBuf)&&(tmpi>=1)) {
                        s[0]=DispMultBuf[(tmpi-1)*3+1];
                        s[1]=DispMultBuf[(tmpi-1)*3+2];
                        s[2]=0;

                        //清除状态行默认汉字,及多码缓冲区
                        ClearMultCodeBuf();
                        ClearDispMultBuf();
                        DispMultCodeLine("",1);
                        FirstDspPos=0;
                        return 2;
                    }
                    DispStateLine(ExtCodeBuf);

                } else {  //other
                    s[0]=ch;
                    s[1]=0;
                    return 1;
                }

                break;

        }  //switch

    }  //while
}


/*-------------------------------------------------------------
Function:把输入汉字放在s中
Input:   s---存放汉字或字符(以'\0'结束)
         method---输入法选择,
                  (0-English,1-PinYin,2-QuWei,3-BiHua,other-和前次一样)
enum INPUT_METHOD
{
    ENGLISH_S                               = 0,  // 英文
    ENGLISH_B,      // 英文大写
    QUWEI,          // 区位输入法
    PINYIN,         // 拼音输入法
    WBZX,           // 五笔字型输入法
    BIHUA,      //笔划输入法
    MAX_IME_NO      // 结束
};
Return:  1---输入ASCII
     2---输入一个汉字
-------------------------------------------------------------*/
int ndk_OneHZInput(char *pcS,int *pnType)
{
    uint x,y;
    if (pcS == NULL)
        return -1;

    NDK_ScrGetxy(&x, &y);

    //CodePtr=(unsigned char *)HZMBSTARTADDRESS;//+py_table;

    *pnType=GetOneHZ(pcS);
    NDK_ScrGotoxy(x, y);

    return 0;
}

/////////////////////////////////////////////////////////////
// 数拼输入法 直接输入数字在转换成拼音编码在调用拼音输入法
//
/////////////////////////////////////////////////////////////


//码表如下
//第一个字节为1~9 第二个字节为0\2\3\4\6\7\8
static const int secondintserial[10]= {
    //  0   1   2   3   4   5   6   7   8   9
    0 , 7 , 1 , 2 , 3 , 7 , 4 , 5 , 6 , 7
};
#define NINT    512
static const int secondint[9][8]= {

    {   0   ,   2   ,   7   ,   11  ,   47  ,   NINT,   51  ,   NINT},
    {   62  ,   65  ,   75  ,   82  ,   113 ,   NINT,   121 ,   NINT},
    {   128 ,   131 ,   139 ,   147 ,   155+1   ,   162+2   ,   163+2   ,   NINT},
    {   170+2   ,   172+2   ,   182+2   ,   NINT,   190+2   ,   NINT,   198+2   ,   NINT},
    {   214+2   ,   217+2   ,   227+2   ,   234+2   ,   255+2   ,   NINT,   263+2   ,   NINT},
    {   283+2   ,   286+2   ,   296+2   ,   304+2   ,   322+2   ,   NINT,   328+2   ,   NINT},
    {   336+3   ,   339+3   ,   353+3   ,   363+3   ,   393+3   ,   NINT,   403+3   ,   NINT},
    {   416+3   ,   417+3   ,   422+3   ,   425+3   ,   432+3   ,   NINT,   436+3   ,   NINT},
    {   442+3   ,   445+3   ,   453+3   ,   458+3   ,   473+3   ,   NINT,   478+3   ,   489+3   }   //489
};

struct numtopinyin {
    unsigned int pynum;
    char * pystr;
};


static const struct numtopinyin secondtable[]= {
    { 1     , "q"       },      // 0
    { 1     , "z"       },      // 1
    //[1][0]->(0)
    { 12        , "za"  },      // 2
    { 124   , "zai" },      // 3
    { 126   , "zan" },      // 4
    { 126   , "zao" },      // 5
    { 1264  , "zang"    },      // 6
    //[1][2]->(2)
    { 13        , "ze"  },      // 7
    { 134   , "zei" },      // 8
    { 136   , "zen" },      // 9
    { 1364  , "zeng"    },      // 10
    //[1][3]->(7)
    { 14        , "qi"      },      //11
    { 14        , "zi"      },      //12
    { 14        , "zh"  },          //13
    { 142   , "qia" },      //14
    { 142   , "zha" },      //15
    { 143   , "qie" },      //16
    { 143   , "zhe" },      //17
    { 144   , "zhi" },      //18
    { 146   , "qin" },      //19
    { 146     , "qio"       },      //20
    { 146     , "zho"   },      //21
    { 148   , "qiu" },      //22
    { 148   , "zhu" },      //23
    { 1424  , "zhai"    },      //24
    { 1426  , "qian"    },      //25
    { 1426  , "qiao"    },      //26
    { 1426  , "zhan"    },      //27
    { 1426  , "zhao"    },      //28
    { 1434  , "zhei"    },      //29
    { 1436  , "zhen"    },      //30
    { 1464  , "qing"    },      //31
    { 1466    , "qion"  },      //32
    { 1466    , "zhon"  },      //33
    { 1468  , "zhou"    },      //34
    { 1482  , "zhua"    },      //35
    { 1484  , "zhui"    },      //36
    { 1486  , "zhun"    },      //37
    { 1486  , "zhuo"    },      //38
    { 14264 , "qiang"   },      //39
    { 14264 , "zhang"   },      //40
    { 14364 , "zheng"   },      //41
    { 14664 , "qiong"   },      //42
    { 14664 , "zhong"   },      //43
    { 14824 , "zhuai"   },      //44
    { 14826 , "zhuan"   },      //45
    { 148264, "zhuang"},        //46
    //[1][4]->(11)
    { 16       , "zo"       },           //47
    { 166     , "zon"   },          //48
    { 168   , "zou" },      //49
    { 1664  , "zong"    },      //50
    //[1][6]->(47)
    { 18          , "qu"    },      //51
    { 18          , "zu"    },      //52
    { 183   , "que" },      //53
    { 184   , "zui" },      //54
    { 186   , "qun" },      //55
    { 186   , "zun" },      //56
    { 186   , "zuo" },      //57
    { 182     , "qua"   },      //58
    { 182     , "zua"   },          //59
    { 1826  , "quan"    },      //60
    { 1826  , "zuan"    },      //61
    //[1][8]->(51)
    { 2       , "a"     },      //62
    { 2        , "b"        },      //63
    { 2        , "c"        },          //64
    //[2][0]->(62)
    { 22    , "ba"  },      //65
    { 22          , "ca"    },      //66
    { 224   , "bai" },      //67
    { 224   , "cai" },      //68
    { 226   , "ban" },      //69
    { 226   , "bao" },      //70
    { 226   , "can" },      //71
    { 226   , "cao" },      //72
    { 2264  , "bang"    },      //73
    { 2264  , "cang"    },      //74
    //[2][2]->(65)
    { 23          , "ce"    },      //75
    { 23       , "be"       },        //76
    { 234   , "bei" },      //77
    { 236   , "ben" },      //78
    { 236   , "cen" },      //79
    { 2364  , "beng"    },      //80
    { 2364  , "ceng"    },      //81
    //[2][3]->(75)
    { 24          , "ai"        },      //82
    { 24          , "bi"            },      //83
    { 24          , "ci"            },      //84
    { 24       , "ch"       },          //85
    { 242   , "cha"     },      //86
    { 242     , "bia"       },      //87
    { 243   , "bie" },      //88
    { 243   , "che" },      //89
    { 244   , "chi" },      //90
    { 246   , "bin" },      //91
    { 246     , "cho"   },      //92
    { 248   , "chu" },      //93
    { 2424  , "chai"    },      //94
    { 2426  , "bian"    },      //95
    { 2426  , "biao"    },      //96
    { 2426  , "chan"    },      //97
    { 2426  , "chao"    },      //98
    { 2436  , "chen"    },      //99
    { 2464  , "bing"    },      //100
    { 2466    , "chon"  },      //101
    { 2468  , "chou"    },      //102
    { 2482    , "chua"  },          //103
    { 2484  , "chui"    },      //104
    { 2486  , "chun"    },      //105
    { 2486  , "chuo"    },      //106
    { 24264 , "chang"   },      //107
    { 24364 , "cheng"   },      //108
    { 24664 , "chong"   },      //109
    { 24824 , "chuai"   },
    { 24826 , "chuan"   },      //110
    { 248264, "chuang"},        //111
    //[2][4]->(82)
    { 26          , "an"    },      //112
    { 26          , "ao"    },      //113
    { 26          , "bo"    },      //114
    { 26       , "co"   },          //115
    { 264   , "ang" },      //116
    { 266     , "con"   },          //117
    { 268   , "cou" },      //118
    { 2664  , "cong"    },      //119
    //[2][6]->(112)
    { 28        , "bu"  },      //120
    { 28        , "cu"  },      //121
    { 282   , "cua"     },          //122
    { 284   , "cui" },      //123
    { 286   , "cun" },      //124
    { 286   , "cuo" },      //125
    { 2826  , "cuan"},      //126
    //[2][8]->(120)
    { 3     , "d"       },          //127
    { 3     , "e"       },      //128
    { 3         , "f"       },          //129
    //[3][0]->(127)
    { 32        , "fa"  },      //130
    { 32        , "da"  },      //131
    { 324   , "dai" },      //132
    { 326   , "fan" },      //133
    { 326   , "dan" },      //134
    { 326   , "dao" },      //135
    { 3264  , "fang"    },      //136
    { 3264  , "dang"},      //137
    //[3][2]->(130)
    { 33        , "de"  },      //138
    { 33        , "fe"  },      //139
    { 334   , "fei" },      //140
    { 334   , "dei" },      //141
    { 336   , "fen" },      //142
    { 336   , "den" },      //143
    { 3364  , "feng"    },      //144
    { 3364  , "deng"    },      //145
    //[3][3]->(138)
    { 34        , "di"      },      //146
    { 34        , "ei"      },      //146   //------------new   146+1
    { 342   , "dia" },          //147
    { 343   , "die" },      //148
    { 346       , "din" },          //149
    { 348   , "diu" },      //150
    { 3426  , "dian"    },      //151
    { 3426  , "diao"    },      //152
    { 3464  , "ding"    },      //153
    //[3][4]->(146)
    { 36        , "en"  },      //154
    { 36        , "fo"  },      //155
    { 36        , "do"  },          //156
    { 364       , "eng" },          //156   //------------new 156+2
    { 366       , "don" },          //157
    { 368   , "fou" },      //158
    { 368   , "dou" },      //159
    { 3664  , "dong"},      //160
    //[3][6]->(154)
    { 37        , "er"  },      //161
    //[3][7]->(161)
    { 38        , "fu"  },      //162
    { 38        , "du"  },      //163
    { 382       , "dua"     },          //164
    { 384   , "dui" },      //165
    { 386   , "dun" },      //166
    { 386   , "duo" },      //167
    { 3826  , "duan"    },      //168
    //[3][8]->(162)
    { 4         , "g"       },      //169
    { 4         , "h"       },      //170
    //[4][0]->(169)
    { 42        , "ga"  },      //171
    { 42        , "ha"  },      //172
    { 424   , "gai" },      //173
    { 424   , "hai" },      //174
    { 426   , "gan" },      //175
    { 426   , "gao" },      //176
    { 426   , "han" },      //177
    { 426   , "hao" },      //178
    { 4264  , "gang"    },      //179
    { 4264  , "hang"    },      //180
    //[4][2]->(171)
    { 43        , "ge"  },      //181
    { 43        , "he"  },      //182
    { 434   , "gei" },      //183
    { 434   , "hei" },      //184
    { 436   , "gen" },      //185
    { 436   , "hen" },      //186
    { 4364  , "geng"    },      //187
    { 4364  , "heng"    },      //188
    //[4][3]->(181)
    { 46        , "go"      },      //189
    { 46        , "ho"      },      //190
    { 466       , "gon"     },      //191
    { 466       , "hon"     },      //192
    { 468   , "gou" },      //193
    { 468   , "hou" },      //194
    { 4664  , "gong"    },      //195
    { 4664  , "hong"    },      //196
    //[4][6]->(189)
    { 48        , "gu"  },      //197
    { 48        , "hu"  },      //198
    { 482   , "gua" },      //199
    { 482   , "hua" },      //200
    { 484   , "gui" },      //201
    { 484   , "hui" },      //202
    { 486   , "gun" },      //203
    { 486   , "guo" },      //204
    { 486   , "hun" },      //205
    { 486   , "huo" },      //206
    { 4824  , "guai"    },      //207
    { 4824  , "huai"    },      //208
    { 4826  , "guan"    },      //209
    { 4826  , "huan"    },      //210
    { 48264 , "guang"   },      //211
    { 48264 , "huang"   },      //212
    //[4][8]->(197)
    { 5         , "j"       },      //213
    { 5         , "k"       },      //214
    { 5         , "l"       },      //215
    //[5][0]->(213)
    { 52        , "la"      },      //216
    { 52        , "ka"  },      //217
    { 524   , "lai" },      //218
    { 524   , "kai" },      //219
    { 526   , "lan" },      //220
//  { 526   , "lan" },      //221
    { 526   , "lao" },      //222
    { 526   , "kan" },      //223
    { 526   , "kao" },      //224
    { 5264  , "lang"    },      //225
    { 5264  , "kang"    },      //226
    //[5][2]->(216)
    { 53        , "le"      },      //227
    { 53        , "ke"  },      //228
    { 534   , "lei" },      //229
    { 536       , "len"     },      //230
    { 536   , "ken" },      //231
    { 5364  , "leng"    },      //232
    { 5364  , "keng"    },      //233
    //[5][3]->(227)
    { 54        , "li"      },      //234
    { 54        , "ji"      },      //235
    { 542   , "lia" },      //236
    { 542   , "jia" },      //237
    { 543   , "lie" },      //238
    { 543   , "jie" },      //239
    { 546   , "lin" },      //240
    { 546   , "jin" },      //241
    { 546   , "jio" },
    { 548   , "liu" },      //242
    { 548   , "jiu" },      //243
    { 5426  , "lian"    },      //244
    { 5426  , "liao"    },      //245
    { 5426  , "jian"    },      //246
    { 5426  , "jiao"    },      //247
    { 5464  , "ling"    },      //248
    { 5464  , "jing"    },      //249
    { 5466  , "jion"    },
    { 54264 , "liang"   },      //250
    { 54264 , "jiang"   },      //251
    { 54664 , "jiong"   },
    //[5][4]->(234)
    { 56        , "lo"      },          //252
    { 56        , "ko"      },          //253
    { 566       , "lon"     },          //254
    { 566       , "kon"     },          //255
    { 568   , "lou" },      //256
    { 568   , "kou" },      //257
    { 5664  , "long"    },      //258
    { 5664  , "kong"    },      //259
    //[5][6]->(252)
    { 58        , "lu"      },      //260
    { 58        , "lv"      },      //261
    { 58        , "ku"  },      //262
    { 58        , "ju"  },      //263
    { 582   , "kua" },      //264
    { 582       , "lua"     },      //265
    { 582       , "jua"     },      //266
    { 583   , "lve" },      //267
    { 583   , "jue" },      //268
    { 584   , "kui" },      //269
    { 586   , "lun" },      //270
    { 586   , "luo" },      //271
    { 586   , "kun" },      //272
    { 586   , "kuo" },      //273
    { 586   , "jun" },      //274
    { 5824  , "kuai"    },      //275
    { 5826  , "luan"    },      //276
    { 5826  , "kuan"    },      //277
    { 5826  , "juan"    },      //278
    { 58264 , "kuang"   },      //279
    //[5][8]->(260)

    { 6     , "m"   },      //280
    { 6         , "n"       },          //281
    { 6         , "o"       },          //282
    //[6][0]->(280)
    { 62        , "ma"  },      //283
    { 62        , "na"  },      //284
    { 624   , "mai" },      //285
    { 624   , "nai" },      //286
    { 626   , "man" },      //287
    { 626   , "mao" },      //288
    { 626   , "nan" },      //289
    { 626   , "nao" },      //290
    { 6264  , "mang"    },      //291
    { 6264  , "nang"    },      //292
    //[6][2]->(283)
    { 63        , "me"  },      //293
    { 63        , "ne"  },      //294
    { 634   , "mei" },      //295
    { 634   , "nei" },      //296
    { 636   , "men" },      //297
    { 636   , "nen" },      //298
    { 6364  , "meng"    },      //299
    { 6364  , "neng"    },      //300
    //[6][3]->(293)
    { 64        , "mi"  },      //301
    { 64        , "ni"      },      //302
    { 64        , "ng"  },      //303
    { 642       , "mia"     },          //304
    { 642       , "nia"     },      //305
    { 643   , "mie" },      //306
    { 643   , "nie" },      //307
    { 646   , "min" },      //308
    { 646   , "nin" },      //309
    { 648   , "niu" },      //310
    { 648   , "miu" },      //311
    { 6426  , "mian"    },      //312
    { 6426  , "miao"    },      //313
    { 6426  , "nian"    },      //314
    { 6426  , "niao"    },      //315
    { 6464  , "ming"    },      //316
    { 6464  , "ning"    },      //317
    { 64264 , "niang"   },      //318
    //[6][4]->(301)
    { 66        , "mo"  },      //319
    { 66        , "no"      },          //320
    { 666       , "non"     },          //321
    { 668   , "mou" },      //322
    { 668   , "nou" },
    { 6664  , "nong"    },      //323
    //[6][6]->(319)
    { 68        , "ou"  },      //324
    { 68        , "mu"  },      //325
    { 68        , "nu"  },      //326
    { 68        , "nv"  },      //327
    { 682       , "nua"     },          //328
    { 683   , "nve" },      //329
    { 686   , "nun" },      //329   //--------------- new 329+3
    { 686   , "nuo" },      //330
    { 6826  , "nuan"    },      //331
    //[6][8]->(324)
    { 7         , "p"   },          //332
    { 7         , "r"       },          //333
    { 7         , "s"       },          //334
    //[7][0]->(332)
    { 72        , "pa"  },      //335
    { 72        , "sa"  },      //336
    { 72        , "ra"      },          //337
    { 724   , "pai" },      //338
    { 724   , "sai" },      //339
    { 726   , "pan" },      //340
    { 726   , "pao" },      //341
    { 726   , "ran" },      //342
    { 726   , "rao" },      //343
    { 726   , "san" },      //344
    { 726   , "sao" },      //345
    { 7264  , "pang"    },      //346
    { 7264  , "rang"    },      //347
    { 7264  , "sang"    },      //348
    //[7][2]->(335)
    { 73        , "re"  },      //349
    { 73        , "se"  },      //350
    { 73        , "pe"      },      //351
    { 734   , "pei" },      //352
    { 736   , "pen" },      //353
    { 736   , "ren" },      //354
    { 736   , "sen" },      //355
    { 7364  , "peng"    },      //356
    { 7364  , "reng"    },      //357
    { 7364  , "seng"    },      //358
    //[7][3]->(349)
    { 74        , "pi"      },      //359
    { 74        , "ri"      },      //360
    { 74        , "si"      },      //361
    { 74        , "sh"      },          //362
    { 742   , "sha" },      //363
    { 742       , "pia"     },      //364
    { 743   , "pie" },      //365
    { 743   , "she" },      //366
    { 744   , "shi" },      //367
    { 746   , "pin" },      //368
    { 746       , "sho"     },      //369
    { 748   , "shu" },      //370
    { 7424  , "shai"    },      //371
    { 7426  , "pian"    },      //372
    { 7426  , "piao"    },      //373
    { 7426  , "shan"    },      //374
    { 7426  , "shao"    },      //375
    { 7434  , "shei"    },      //376
    { 7436  , "shen"    },      //377
    { 7464  , "ping"    },      //378
    { 7468  , "shou"    },      //379
    { 7482  , "shua"    },      //380
    { 7484  , "shui"    },      //381
    { 7486  , "shun"    },      //382
    { 7486  , "shuo"    },      //383
    { 74264 , "shang"   },      //384
    { 74364 , "sheng"   },      //385
    { 74824 , "shuai"   },      //386
    { 74826  , "shuan"  },          //387
    { 748264, "shuang"},        //388
    //[7][4]->(359)
    { 76    , "po"  },      //389
    { 76        , "ro"      },      //390
    { 76        , "so"  },      //391
    { 766       , "ron"     },      //392
    { 766       , "son"     },          //393
    { 768   , "pou" },      //394
    { 768   , "rou" },      //395
    { 768   , "sou" },      //396
    { 7664  , "rong"    },      //397
    { 7664  , "song"    },      //398
    //[7][6]->(389)
    { 78        , "pu"  },      //399
    { 78        , "ru"  },      //400
    { 78        , "su"  },      //401
    { 782       , "rua"     },          //402
    { 782       , "sua"     },      //403
    { 784   , "rui" },      //404
    { 784   , "sui" },      //405
    { 786   , "run" },      //406
    { 786   , "ruo" },      //407
    { 786   , "sun" },      //408
    { 786   , "suo" },      //409
    { 7826  , "ruan"    },      //410
    { 7826  , "suan"    },      //411
    //[7][8]->(399)
    { 8         , "t"       },          //412
    //[8[[0]->(412)
    { 82        , "ta"  },      //413
    { 824   , "tai" },      //414
    { 826   , "tan" },      //415
    { 826   , "tao" },      //416
    { 8264  , "tang"    },      //417
    //[8][2]->(413)
    { 83        , "te"  },      //418
    { 836       , "ten"     },      //419
    { 8364  , "teng"    },      //420
    //[8][3]->(418)
    { 84        , "ti"      },      //421
    { 842       , "tia"     },      //422
    { 843   , "tie" },      //423
    { 846       , "tin"     },      //424
    { 8426  , "tian"    },      //425
    { 8426  , "tiao"    },      //426
    { 8464  , "ting"    },      //427
    //[8][4]->(421)
    { 86        , "to"      },          //428
    { 866       , "ton"     },          //429
    { 868   , "tou" },      //430
    { 8664  , "tong"    },      //431
    //[8][6]->(428)
    { 88        , "tu"  },      //432
    { 882       , "tua"     },      //433
    { 884   , "tui" },      //434
    { 886   , "tun" },      //435
    { 886   , "tuo" },      //436
    { 8826  , "tuan"    },      //437
    //[8][8]->(432)
    { 9         , "w"       },      //438
    { 9         , "x"       },      //439
    { 9         , "y"       },          //440
    //[9][0]->(438)
    { 92        , "wa"  },      //441
    { 92        , "ya"  },      //442
    { 924   , "wai" },      //443
    { 926   , "yan" },      //444
    { 926   , "yao" },      //445
    { 926   , "wan" },      //446
    { 9264  , "yang"    },      //447
    { 9264  , "wang"    },      //448
    //[9][2]->(441)
    { 93        , "ye"  },      //449
    { 93    , "we"      },      //450
    { 934   , "wei" },      //451
    { 936   , "wen" },      //452
    { 9364  , "weng"},      //453
    //[9][3]->(449)
    { 94        , "yi"  },      //454
    { 94        , "xi"  },      //455
    { 942   , "xia" },      //456
    { 943   , "xie" },      //457
    { 946   , "yin" },      //458
    { 946   , "xin" },      //459
    { 946       , "xio"     },      //460
    { 948   , "xiu" },      //461
    { 9426  , "xian"    },      //462
    { 9426  , "xiao"    },      //463
    { 9464  , "ying"    },      //464
    { 9464  , "xing"    },      //465
    { 9466      , "xion"    },          //466
    { 94264 , "xiang"   },      //467
    { 94664 , "xiong"   },      //468
    //[9][4]->(454)
    { 96        , "yo"  },      //469
    { 96        , "wo"  },      //470
    { 966       , "yon"     },      //471
    { 968   , "you" },      //472
    { 9664  , "yong"    },      //473
    //[9][6]->(469)
    { 98        , "yu"  },      //474
    { 98        , "wu"  },      //475
    { 98        , "xu"  },      //476
    { 982       , "yua"     },          //477
    { 982       , "xua"     },          //478
    { 983   , "yue" },      //479
    { 983   , "xue" },      //480
    { 986   , "yun" },      //481
    { 986   , "xun" },      //482
    { 9826  , "yuan"    },      //483
    { 9826  , "xuan"    },      //484
    //[9][8]->(474)
    { 0     , ""    }       //485
};

//清除状态栏备选拼音组合
static void ClearExtCodeMult(void)
{
    memset((char *)&gextCodeMultBuf,0,sizeof(gextCodeMultBuf));
}


/*------------------------------------------------------
Function: 在汉字被选行反显指定位置的汉字
    即刷新制定位置的备选字符
------------------------------------------------------*/
static void ReverseCode(int RevNum,int len)
{
    char tmp[4];

    memset(tmp, 0, sizeof(tmp));
    memcpy(tmp, DispMultBuf+RevNum*len, len);
    NDK_ScrGotoxy(RevNum*len*sys.font->afontw, ime_y);
    ndk_disprevstr(tmp);
}

//输入数字字符串输出拼音码
static int numtopycode(const char *str)
{
    long int tmppynum1,tmppynum2;
    int i,count;
    int stopi;
    long int tmpint;

    if ((str == NULL) || (strlen(str) == 0))
        return FAIL;

    tmpint = atol(str);

    if ((tmpint < 0) || (tmpint > 999999))
        return FAIL;

    tmppynum1 = str[0] - '1';
    if (strlen(str) > 1)
        tmppynum2 = str[1] - '0';
    else
        tmppynum2 = 0;

    //查找
    {
        //判断有效性
        if ((tmppynum1<0)||(tmppynum1>9)||(tmppynum2>9)||(tmppynum2<0)) {
            return FAIL;
        }
        //方案1二级扫描
        if (secondint[tmppynum1][secondintserial[tmppynum2]]==NINT) {
            return FAIL;
        }
        //找出搜索的上限
        stopi=NINT;
        for (i=1; i<(9*8-(tmppynum1*8+secondintserial[tmppynum2])); i++) {
            if (((int *)(&secondint[tmppynum1][secondintserial[tmppynum2]]))[i]!=NINT) {
                stopi=((int *)(&secondint[tmppynum1][secondintserial[tmppynum2]]))[i];
                break;
            }
        }
        if (stopi==NINT)
            return FAIL;

        count=0;
        for (i=secondint[tmppynum1][secondintserial[tmppynum2]]; i<stopi; i++) {
            //方案1二级扫描
            if ((secondtable[i].pynum)==tmpint) {
                strcpy((char *)(gextCodeMultBuf[count]), secondtable[i].pystr );
                count++;
            }
        }

        if (count>0)
            return count;

        return FAIL;
    }
}
//在状态栏显示拼音组合
static void DispExtCodeMult(int RevNum)
{
    int i, len, count=0;
    int dspgroup    = 0;
    char dspstr[2][16]; //备选拼音组合不会大于两行
    int num = 0;
    char tmpbuf[10];

    while (1) {
        memset( dspstr , 0 , sizeof(dspstr));
        memset( tmpbuf, 0, sizeof(tmpbuf));
        for (i=0; i<6; i++) {
            if ((len=strlen((char *)(gextCodeMultBuf[i])))!=0) {
                if ((count+len) > 11) {
                    dspgroup++;
                    count=len;
                    num = i;
                } else {
                    if (((count+len) < 11) && (count > 0)) {
                        strcat(dspstr[dspgroup], " ");
                        count++;
                    }
                    count += len;
                }

                if (i == RevNum)
                    sprintf(tmpbuf, "\x03%s\x02",gextCodeMultBuf[i]);
                else
                    sprintf(tmpbuf, "%s",gextCodeMultBuf[i]);
                strcat(dspstr[dspgroup], tmpbuf);

            } else {
                if(num==0)
                    num=i;
                break;
            }
        }
        i=0;
        if(RevNum>=num)
            i=1;
        NDK_ScrDispString(5*(sys.font->afontw+sys.wspace),(ime_y + sys.font->fonth),"               ",0);//清除状态行的内容
        NDK_ScrDispString(5*(sys.font->afontw+sys.wspace),(ime_y + sys.font->fonth),dspstr[i],0);
        return;
    }
}

/*------------------------------------------------------------
Function: 数拼输入法
Output:   s---返回的字符在这里，以\0为结束
Return:   SUCC :输入汉字成功
        -1:     取消键退出
        1  :    特殊键值返回(ENTER, BASP)
        IME_RECYCLE_KEY:字母切换键
------------------------------------------------------------*/
static int NumPYInput(char *s)
{
    int NumPYflag=0;                  //数拼标志0.状态行拼音输入模式，1.选字模式
    int StateLinenum=0;
    int pycount=0;
    int pylen=0;                                //输入拼音长度计数即数字键个数
    char tmpbuf[10];
    int ch,range,punval;
    int codenum=0;
    static int CurrtDspPos;
    static char buffer[PUNCTUATION_LEN];

    punval=sys.video->sur->width/sys.font->afontw;
    if(punval>PUNCTUATION_LEN)
        punval=PUNCTUATION_LEN;
    range=sys.video->sur->width/sys.font->afontw/3;
    if(range>10)
        range=10;
    while (1) {
        if (pylen== 0)
            ClearExtCodeMult();
        ch = getkeyvalue(0);
    NUMPY_START:
        if (!NumPYflag) {            //处理状态行拼音输入模式
            switch (ch) {
                case IME_RECYCLE_KEY:      //切换输入法
//              if (pylen <= 0)     //由于8300该键与NUMPY_PREV_WORD键相同
                    return IME_RECYCLE_KEY;
                    break;
                case IME_RECYCLE_KEY|KBD_LONGPRESSED:
                    s[0] = '#';
                    return 1;
                    break;
                case DOT_VALUE|KBD_LONGPRESSED:
                    s[0] = '*';
                    return 1;
                    break;
                case EKEY_LEFT:
                case IME_PREV_SCR:   //左移
                    StateLinenum = (StateLinenum+pycount-1)%pycount;
                    break;
                case EKEY_RIGHT:
                case IME_NEXT_SCR:  //右移
                    StateLinenum = (StateLinenum+1)%pycount;
                    break;
                case ESC_VALUE:             //取消退出
                    s[0] = ESC_VALUE;
                    return -1;
                case ENTER_VALUE:           //选中输入的拼音组合,切换到选字模式
                    if (pylen > 0) {
						if(MultCodeNum>0)
						{
							NumPYflag = 1;
							ReverseCode(0,3);
							continue;
						}
                    } else {
                        s[0] = ENTER_VALUE;
                        return 1;
                    }
					break;
                default:
                    if ((ch > 0x30) && (ch <= 0x39)) {
                        tmpbuf[pylen]=ch;
                        tmpbuf[++pylen]=0;
                    } else if (ch == BASP_VALUE) {
                        if (pylen > 0)
                            tmpbuf[--pylen] = 0;
                        else {
                            s[0] = BASP_VALUE;
                            return 1;
                        }
                    } else if((ch == IME_ZERO)&&(pylen == 0)) {
                        memcpy(buffer,PUNCTUATION_STR,PUNCTUATION_LEN);
                        MultCodeNum = PUNCTUATION_LEN;
                        if(save_screen_select) {
                            free(save_screen_select);
                            save_screen_select =NULL;
                        }
                        save_screen_select = ndk_disp_save_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace));
                        FirstDspPos = 0;
                        DispState(buffer);
                        CurrtDspPos = 0;
                        ReverseCode(CurrtDspPos,1);
                        NumPYflag = 2;
                        continue;
                    }
                    ClearExtCodeMult();
                    pycount = numtopycode(tmpbuf);
                    if (pycount == FAIL) {  //处理无法找到组合
                        if (pylen > 0) {
                            tmpbuf[--pylen] = 0;
                            NDK_SysBeep();
                            pycount = numtopycode(tmpbuf);
                            break;
                        }
                    }
                    StateLinenum=0;
                    break;
            }

            ExtCodeNum=pylen;
            DispExtCodeMult(StateLinenum);
            strcpy(ExtCodeBuf, (char *)(gextCodeMultBuf[StateLinenum]));
            InputMethod(IME_NUMPY);
            DispMultCode();  //在输入法状态行上方显示更新后的重码汉字
        } else if(NumPYflag==1) {
            switch (ch) {
                case IME_RECYCLE_KEY:      //切换输入法
//              if (pylen <= 0)     //由于8300该键与NUMPY_PREV_WORD键相同
                    return IME_RECYCLE_KEY;
                    break;
                case DOT_VALUE|KBD_LONGPRESSED:
                    s[0] = '*';
                    return 1;
                    break;
                case IME_RECYCLE_KEY|KBD_LONGPRESSED:
                    s[0] = '#';
                    return 1;
                    break;
                case EKEY_LEFT:
                case IME_PREV_SCR:
                    if (codenum > 0) {  //超过第一个字，相当于向前换页
                        codenum--;
                        break;
                    }
                case NUMPY_PREV_WORD:
                    if (!FirstDspPos) //第一组汉字
                        NDK_SysBeep();
                    else
                        FirstDspPos-=range;
                    break;
                case EKEY_RIGHT:
                case IME_NEXT_SCR: 
                    if (codenum < strlen (DispMultBuf) /3 -1) { //超过第一个字，相当于向后换页
                        codenum++;
                        break;
                    }
                case NUMPY_NEXT_WORD:
                    if ((FirstDspPos+NumOfDispMultBuf+1) > MultCodeNum) //最后一组汉字
                        NDK_SysBeep();
                    else
                        FirstDspPos += range;
                    break;
                case BASP_VALUE:
                    NumPYflag=0;
                    codenum=0;
                    goto NUMPY_START;
                case ESC_VALUE: //取消键时切换到状态行方式
                    DispMultCode();
                    NumPYflag=0;
                    codenum=0;
                    continue;
                case ENTER_VALUE:   //选中
                    s[0]=DispMultBuf[codenum*3+1];
                    s[1]=DispMultBuf[codenum*3+2];
                    s[2]=0;
                    return SUCC;
                default:
                    if (((ch>='1')&&(ch <= NumOfDispMultBuf+'0'))||(ch=='0'&&NumOfDispMultBuf==10)) { //---数字
                        //选择一个汉字
                        int tmpi;

						if(ch=='0')
							tmpi=10;
						else
                        	tmpi=ch-'0';
                        s[0]=DispMultBuf[(tmpi-1)*3+1];
                        s[1]=DispMultBuf[(tmpi-1)*3+2];
                        s[2]=0;

                        //清除状态行默认汉字,及多码缓冲区
                        ClearMultCodeBuf();
                        ClearDispMultBuf();
                        DispMultCodeLine("",1);
                        FirstDspPos=0;
                        return SUCC;
                    } else
                        NDK_SysBeep();

                    continue;
            }
            DispMultCode();
            if (codenum >= strlen (DispMultBuf) /3 -1)  //处理最后一页越界
                codenum = strlen (DispMultBuf) /3 -1;
            ReverseCode(codenum,3);
        } else { //字符输出
            switch(ch) {
                case IME_RECYCLE_KEY:      //切换输入法
//              if (pylen <= 0)     //由于8300该键与NUMPY_PREV_WORD键相同
                    return IME_RECYCLE_KEY;
                    break;
                case EKEY_LEFT:
                case IME_PREV_SCR:
                    if (CurrtDspPos > 0) { //第一组汉字
                        CurrtDspPos--;
                    } else if (FirstDspPos > 0) { //第一组汉字
                        FirstDspPos -= punval;
                        //  CurrtDspPos = MAX_ASCII_PER_LINE - 1;
                        CurrtDspPos = 0;
                    }
                    DispState(buffer);
                    ReverseCode(CurrtDspPos,1);
                    break;
                case EKEY_RIGHT:
                case IME_NEXT_SCR: 
                    if (CurrtDspPos < (NumOfDispMultBuf - 1)) {
                        CurrtDspPos++;
                    } else if ((FirstDspPos + punval) < MultCodeNum) { //最后一组汉字
                        FirstDspPos += punval;
                        //  CurrtDspPos = 0;
                        CurrtDspPos = ((MultCodeNum -FirstDspPos)/punval)?(punval-1):((MultCodeNum -FirstDspPos)%punval-1);
                    }
                    DispState(buffer);
                    ReverseCode(CurrtDspPos,1);
                    break;
                case NUMPY_PREV_WORD:
                    if (FirstDspPos > 0) { //第一组汉字
                        FirstDspPos -= punval;
                        DispState(buffer);
                        CurrtDspPos = 0;
                        ReverseCode(CurrtDspPos, 1);
                    }
                    break;
                case NUMPY_NEXT_WORD:
                    if ((FirstDspPos + punval) < MultCodeNum) { //最后一组汉字
                        FirstDspPos += punval;
                        DispState(buffer);
                        CurrtDspPos = 0;
                        ReverseCode(CurrtDspPos, 1);
                    }
                    break;
                case BASP:
                    NumPYflag = 0;
                    if (ExtCodeNum > 0) {
                        DispMultCode();
                    } else {
                        //恢复屏幕
                        ndk_disp_restore_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace),save_screen_select);
                        if(save_screen_select) {
                            free(save_screen_select);
                            save_screen_select=NULL;
                        }
                    }
                    break;
                case ENTER:
                    NumPYflag = 0;
                    s[0] = buffer[FirstDspPos + CurrtDspPos];
                    s[1]=0;
                    DispStateLine("");
                    ndk_disp_restore_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace),save_screen_select);
                    if(save_screen_select) {
                        free(save_screen_select);
                        save_screen_select=NULL;
                    }
                    return 1;
                    break;
                default:
                    continue;
            }
        }
    }
}


/******************** event start *****************************************/
static unsigned char lastkey =0;
static unsigned char keyvalueindex =0;
static struct timeval lasttime;

struct button2char {
    char * charmap;
    int num;
};
static struct button2char kbchartab[] = {
    {".,@\\/+-_|~%^;$|:?'\"`", 20}, /* . */
    {" ", 1},
    {"0=<>()&{}[]", 11},    /* 0 */
    {"qzQZ #*", 7},        /* 1 */
    {"abcABC", 6},        /* 2 */
    {"defDEF", 6},        /* 3 */
    {"ghiGHI", 6},        /* 4 */
    {"jklJKL", 6},        /* 5 */
    {"mnoMNO", 6},        /* 6 */
    {"prsPRS", 6},        /* 7 */
    {"tuvTUV", 6},        /* 8 */
    {"wxyWXY", 6},        /* 9 */
};

static struct button2char kbasciitab[] = {
    {0, 0}, /* . */
    {0, 0},
    {"0", 1},    /* 0 */
    {"1", 1},        /* 1 */
    {"2abc", 4},        /* 2 */
    {"3def", 4},        /* 3 */
    {"4", 1},        /* 4 */
    {"5", 1},        /* 5 */
    {"6", 1},        /* 6 */
    {"7", 1},        /* 7 */
    {"8", 1},        /* 8 */
    {"9", 1},        /* 9 */
};

static const unsigned char code2valuetab[] = {
    0, F1_VALUE, F2_VALUE, F3_VALUE, F4_VALUE, F5_VALUE, F6_VALUE, F7_VALUE, F8_VALUE, F9_VALUE,    /* 0~9   */
    BASP_VALUE,  0,  0, 13,  0,  0,  0,  17,  18,  19,  /* 10~19 */
    20,  0,  0,  0,  0,  0,  0,ESC_VALUE,ZMK_VALUE,  0,        /* 20~29 */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,        /* 30~39 */
    0,  0,  0,  0,  0,  0,'.',  0,'0','1',        /* 40~49 */
    '2','3','4','5','6','7','8','9',  0,  0,        /* 50~59 */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,        /* 60~69 */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,        /* 70~79 */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,        /* 80~89 */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,        /* 90~99 */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,        /*100~109 */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,        /*110~119 */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,        /*120~129 */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,        /*130~139 */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,        /*140~149 */
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,        /*150~159 */
    0,ATM1_VALUE,ATM2_VALUE,ATM3_VALUE,ATM4_VALUE,ATM5_VALUE,ATM6_VALUE,ATM7_VALUE,ATM8_VALUE,  0,        /*160~169 */
    0,  0,  0,  0,  0,  0,  0, FORWARD_VALUE,BACKWARD_VALUE,  0,        /*170~179 */
};

static int waitevent(struct event * e,char type)
{
    int ret;
    socket_header_t sdata;
    video_input_data_keybd_t data;
    video_input_data_mouse_t mdata;
    fd_set readset;
    struct timeval evttimeout;
    sigset_t newmask, oldmask;
    struct button2char *keyvalid;
    if(type==0) {
        keyvalid = kbchartab;
    } else {
        keyvalid = kbasciitab;
    }

    /*无线信号、电源状态刷新需要使用SIGALRM信号(widget/notifier.c)，此处可能被该信号中断，因此需要暂时屏蔽*/
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    while (1) {
        FD_ZERO(&readset);
        FD_SET(sys.socket_fd, &readset);

        evttimeout.tv_sec = 0;
        evttimeout.tv_usec = 300000;

        ret = select(sys.socket_fd + 1, &readset, NULL, NULL, &evttimeout);
        if (ret < 0)
            continue;
        else if (ret == 0) { /* expire:0 , */
            e->type = EVENT_TIMER;
            sigprocmask(SIG_UNBLOCK, &newmask, NULL);
            return 0;
        }
        if (FD_ISSET(sys.socket_fd, &readset)) {
            int count;
			pthread_mutex_lock(&ndk_getkey_mutex);
            if ((count = socket_unix_recv(sys.socket_fd, &sdata, sizeof (sdata))) > 0) {
                if(sdata ==SOCKET_DATA_KEYPAD)
				{
                    count = socket_unix_recv(sys.socket_fd, &data, sizeof (video_input_data_keybd_t));
					pthread_mutex_unlock(&ndk_getkey_mutex);
				}
                else if(sdata==SOCKET_DATA_MOUSE) {
                    socket_unix_recv(sys.socket_fd, &mdata, sizeof (video_input_data_mouse_t));
					pthread_mutex_unlock(&ndk_getkey_mutex);
                    continue;
                } else
				{
					pthread_mutex_unlock(&ndk_getkey_mutex);
                    continue;
				}
            }
            if (count <= 0) {
                sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                return -1;
            }
            e->data.kb.code = data.ascii;
            e->data.kb.value = code2valuetab[e->data.kb.code];
            e->data.kb.mode = 0;
            if ((e->data.kb.code >= DOT) && (e->data.kb.code <= NINE)) {
                e->data.kb.mode = 1;    /* printable key */
            }
            if (kbd_mode && e->data.kb.mode) { /* char mode */
                if (lastkey != e->data.kb.code) { /* press another button */
                    lastkey = e->data.kb.code;
                    keyvalueindex = 0;
                    if(keyvalid[e->data.kb.code - DOT].charmap)
                        e->data.kb.value = keyvalid[e->data.kb.code - DOT].charmap[0];
                    else
                        e->data.kb.value = 0;
                    gettimeofday(&lasttime,NULL);
                } else { /* the same button */
                    struct timeval now;
                    long usec_past;
                    if(keyvalid[e->data.kb.code - DOT].charmap) {
                        gettimeofday(&now,NULL);
                        usec_past = MIN(now.tv_sec - lasttime.tv_sec, 2)*1000000 + now.tv_usec - lasttime.tv_usec;
                        /* 同一按键两次按键之间的间距 */
                        if ((usec_past < 1000000)&&(keyvalid[e->data.kb.code - DOT].num!=0)) {
                            keyvalueindex = (keyvalueindex+1)%keyvalid[e->data.kb.code - DOT].num;
                            e->data.kb.value = keyvalid[e->data.kb.code - DOT].charmap[keyvalueindex];
                            e->data.kb.mode |= 0x80;
                        } else {
                            keyvalueindex = 0;
                            e->data.kb.value = keyvalid[e->data.kb.code - DOT].charmap[0];
                        }
                        lasttime = now;
                    } else
                        e->data.kb.value = 0;
                }
            }
            if((data.ascii&0XFF00)==KBD_LONGPRESSED) {
                e->data.kb.value = e->data.kb.value = data.ascii;
                e->type = EVENT_KBD_LONGPRESSED;
            } else if((data.ascii&0XFF00)==KBD_COMBINATION) {
                e->data.kb.value = e->data.kb.value = data.ascii;
                e->type = EVENT_KBD_COMBINATION;
            } else
                e->type = EVENT_KBD;
            sigprocmask(SIG_UNBLOCK, &newmask, NULL);
            return 0;
        }
    }
    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
    return 0;
}


/*
 *
 */
static int getkeyvalue(int timeout)
{
    struct event e;
    time_t expire=0, now;
//    struct timeval tv;
//    long usec_past;
    int key=-1,oldkey = -1;
    uint x,y;
    char d[2];

    NDK_ScrGetxy(&x, &y);
    if (timeout) expire = time(NULL) + timeout;
    timerclear(&lasttime);
    while (1) {
        if (waitevent(&e,0) < 0)  continue;  /* no event occur */
        if(e.type == EVENT_KBD_LONGPRESSED) {
            key = KBD_LONGPRESSED|e.data.kb.value;
            return key;
        }
        if(e.type == EVENT_KBD_COMBINATION) {
            key = KBD_COMBINATION|e.data.kb.value;
            return key;
        }
        if (e.type == EVENT_KBD) {
            key = e.data.kb.value;
            if(key==IME_ZERO) {
                return key;
            }
            if(kbd_mode&&(key==ENTER)) {
                return (oldkey==-1)?key:oldkey;
            }
            if (kbd_mode && ISCHAR(e.data.kb)) {
                NDK_ScrAutoUpdate(1,NULL);
                d[0] = key;
                d[1] = '\0';
                oldkey = key;
                NDK_ScrDispString(x,y,d,0);
                NDK_ScrGotoxy(x,y);
                continue;
            }
            break;
        } else if (e.type == EVENT_TIMER) {
//            if (timerisset(&lasttime)) {
//                gettimeofday(&tv,NULL);
//                usec_past = MIN(tv.tv_sec - lasttime.tv_sec, 2)*1000000 + tv.tv_usec - lasttime.tv_usec;
//                /* 字符按键变化超时 */
//                if (usec_past >= 1000000) {
//                  break;
//                }
//            }
            if (expire) {
                now = time(NULL);
                if (now >= expire)  break;
            }
        }
    }
    return key;

}

/*
 *
 */
static int getkeyascii(int timeout)
{
    struct event e;
    time_t expire=0, now;
//    struct timeval tv;
//    long usec_past;
    int key=-1,oldkey = -1;
    uint x,y,i=0;
    char d[5]= {0};
    unsigned short value;


    NDK_ScrGetxy(&x, &y);
    if (timeout) expire = time(NULL) + timeout;
    timerclear(&lasttime);
    while (1) {
        if (waitevent(&e,1) < 0)  continue;  /* no event occur */
        if(e.type == EVENT_KBD_LONGPRESSED) {
            key = KBD_LONGPRESSED|e.data.kb.value;
            return key;
        }
        if(e.type == EVENT_KBD_COMBINATION) {
            key = KBD_COMBINATION|e.data.kb.value;
            return key;
        }
        if (e.type == EVENT_KBD) {
            key = e.data.kb.value;
            if(kbd_mode&&(key==ENTER)) {
                lastkey=0;
                if((oldkey==-1)||(oldkey==ENTER)||(i==3)) {
                    if(i==3) {
                        i++;
                    }
                    if(i) {
                        NDK_AscToHex((const uchar*)d,i,1,(unsigned char *)(&value));
                        return value;
                    } else {
                        return key;
                    }


                }
                i++;
                d[i] = '_';
                d[i+1] = 0;
                NDK_ScrDispString(x,y,d,0);
                NDK_ScrGotoxy(x,y);
                oldkey = key;
                continue;
            }

            if (kbd_mode && ISCHAR(e.data.kb)) {
                NDK_ScrAutoUpdate(1,NULL);
                d[i] = key;
                d[i+1] = 0;
                oldkey = key;
                NDK_ScrDispString(x,y,d,0);
                NDK_ScrGotoxy(x,y);
                continue;
            }
            if(key==BACKSPACE) {
                if(i) {
                    if(lastkey) {
                        NDK_ScrDispString(x,y,"    ",0);
                        NDK_ScrGotoxy(x,y);
                        d[i] =0;
                        lastkey = 0;

                    } else {
                        i--;
                        d[i] = '_';
                        d[i+1] = ' ';
                    }
                    NDK_ScrDispString(x,y,d,0);
                    NDK_ScrGotoxy(x,y);
                    continue;
                }
            }
            break;
        } else if (e.type == EVENT_TIMER) {
//            if (timerisset(&lasttime)) {
//                gettimeofday(&tv,NULL);
//                usec_past = MIN(tv.tv_sec - lasttime.tv_sec, 2)*1000000 + tv.tv_usec - lasttime.tv_usec;
//                /* 字符按键变化超时 */
//                if (usec_past >= 1000000) {
//                  break;
//                }
//            }
            if (expire) {
                now = time(NULL);
                if (now >= expire)  break;
            }
        }
    }
    return key;

}

static int Method_Bihua(char *s)
{
    static int state = 0;
    static char buffer[PUNCTUATION_LEN];
    static int CurrtDspPos;
    int in,val2,punval;

    val2=(sys.video->sur->width/(sys.font->afontw)/3);
    if(val2>10)
        val2=10;
    punval=sys.video->sur->width/sys.font->afontw;
    if(punval>PUNCTUATION_LEN)
        punval=PUNCTUATION_LEN;
    while(1) {
        NDK_KbGetCode(0 , &in);
        if ((in == IME_RECYCLE_KEY)||(in == ESC)) {//((in == ESC)&&(ExtCodeNum==0))
            state = 0;
            BHOffset = BIHUA_BASE;
            if(save_screen_select) {
                free(save_screen_select);
                save_screen_select =NULL;
            }
            if(in==ESC) {
                s[0] = ESC;
                return 1;
            } else
                return in;
        }
        if(in==(IME_RECYCLE_KEY|KBD_LONGPRESSED)) {
            s[0] = '#';
            return 1;
        }

        if(in==(DOT_VALUE|KBD_LONGPRESSED)) {
            s[0] = '*';
            return 1;
        }
    rest:
        switch (state) {
            case 0: {
                if (in == BASP) {
                    if (ExtCodeNum > 0) { //还有外码
                        ExtCodeBuf[(int)--ExtCodeNum] = 0;
                        DispStateLine(ExtCodeBuf);   //在状态行显示输入字母

                        if (ExtCodeNum > 0) {
                            BHOffset = BIHUA_BASE;
                            InputMethod(IME_BIHUA);
                        } else {
                            BHOffset = BIHUA_BASE;
                            ndk_disp_restore_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace),save_screen_select);
                        }
                    } else {
                        s[0] = BASP_VALUE;
                        return 1;
                    }
                } else if((in == IME_ZERO)&&(ExtCodeNum == 0)) {
                    memcpy(buffer,PUNCTUATION_STR,PUNCTUATION_LEN);
                    MultCodeNum = PUNCTUATION_LEN;
                    if(save_screen_select) {
                        free(save_screen_select);
                        save_screen_select =NULL;
                    }
                    save_screen_select = ndk_disp_save_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace));
                    //DispMultCodeLine(""); //恢复屏幕
                    FirstDspPos = 0;
                    DispState(buffer);
                    CurrtDspPos = 0;
                    ReverseCode(CurrtDspPos,1);
                    state = 1;
                    continue;
                } else if ((in >= IME_ONE)&&(in <= IME_FIVE)) {
                    int ch0 ;

                    if (ExtCodeNum < sizeof(ExtCodeBuf) - 1) {
                        if (ExtCodeNum == 0) {
                            //保存屏幕
                            if(save_screen_select) {
                                free(save_screen_select);
                                save_screen_select =NULL;
                            }
                            save_screen_select = ndk_disp_save_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace));
                        }
                        ch0=in-IME_ZERO+'0';
                        ExtCodeBuf[(int)ExtCodeNum++] = ch0;
                        DispStateLine(ExtCodeBuf);   //在状态行显示输入字母
                        InputMethod(IME_BIHUA);
                    }
                } else if (in == ENTER) {
                    if(NumOfDispMultBuf > 0) {
                        CurrtDspPos = 0;
                        ReverseCode(CurrtDspPos,3);
                        state = 2;
                    } else {
                        s[0] = ENTER;
                        return 1;
                    }
                }


                break;
            }
            case 1: {
                if ((in == IME_PREV_SCR)||( in == EKEY_LEFT)) {
                    if (CurrtDspPos > 0) { //第一组汉字
                        CurrtDspPos--;
                    } else if (FirstDspPos > 0) { //第一组汉字
                        FirstDspPos -= punval;
                        //  CurrtDspPos = MAX_ASCII_PER_LINE - 1;
                        CurrtDspPos = 0;
                    }
                    DispState(buffer);
                    ReverseCode(CurrtDspPos,1);
                } else if( (in == IME_NEXT_SCR) ||( in== EKEY_RIGHT)) {
                    if (CurrtDspPos < (NumOfDispMultBuf - 1)) {
                        CurrtDspPos++;
                    } else if ((FirstDspPos + punval) < MultCodeNum) { //最后一组汉字
                        FirstDspPos += punval;
                        //CurrtDspPos = 0;
                        CurrtDspPos = ((MultCodeNum -FirstDspPos)/punval)?(punval-1):((MultCodeNum -FirstDspPos)%punval-1);
                    }
                    DispState(buffer);
                    ReverseCode(CurrtDspPos,1);
                } else if (in == NUMPY_PREV_WORD) {
                    if (FirstDspPos > 0) { //第一组汉字
                        FirstDspPos -= punval;
                        DispState(buffer);
                        CurrtDspPos = 0;
                        ReverseCode(CurrtDspPos, 1);
                    }
                } else if (in == NUMPY_NEXT_WORD) {
                    if ((FirstDspPos + punval) < MultCodeNum) { //最后一组汉字
                        FirstDspPos += punval;
                        DispState(buffer);
                        CurrtDspPos = 0;
                        ReverseCode(CurrtDspPos, 1);
                    }
                } else if (in == BASP) {
                    state = 0;
                    if (ExtCodeNum > 0) {
                        DispMultCode();
                    } else {
                        //恢复屏幕
                        ndk_disp_restore_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace),save_screen_select);
                        if(save_screen_select) {
                            free(save_screen_select);
                            save_screen_select=NULL;
                            //                setblk(&multblock);
                        }
                    }
                } else if (in == ENTER) {
                    state = 0;
                    s[0] = buffer[FirstDspPos + CurrtDspPos];
//                  if (s[0]  == 0x0f)
//                      s[0]  = 0x20;        /*标点符号输入0x0f表示空格*/
                    s[1]=0;
                    DispStateLine("");
                    //DispMultCode();
                    //恢复屏幕
                    ndk_disp_restore_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace),save_screen_select);
                    if(save_screen_select) {
                        free(save_screen_select);
                        save_screen_select=NULL;
                    }
                    return 1;
                } else if ((in == IME_ONE) || (in == IME_TWO) || (in == IME_THREE) || (in == IME_FOUR) || (in == IME_FIVE)) {
                    state = 0;
                    DispStateLine("");
                    //DispMultCode();
                    goto rest;
                } else {
                    continue;
                }

                break;
            }
            case 2: {
                if ((in == IME_PREV_SCR)||( in== EKEY_LEFT)) {
                    if (CurrtDspPos > 0) {
                        CurrtDspPos--;
                    } else if (FirstDspPos > 0) {//超过第一个字，相当于向前换页
                        FirstDspPos -= val2;
                        CurrtDspPos = 0 ;
                        //CurrtDspPos = NumOfDispMultBuf - 1;
                    }
                    DispMultCode();
                    ReverseCode(CurrtDspPos,3);
                } else if ( (in == IME_NEXT_SCR) ||(    in== EKEY_RIGHT)) {
                    if (CurrtDspPos < NumOfDispMultBuf - 1) {
                        CurrtDspPos++;
                    } else if ((FirstDspPos + NumOfDispMultBuf) < MultCodeNum) {//超过第一个字，相当于向后换页
                        FirstDspPos += val2;
                        CurrtDspPos = ((MultCodeNum -FirstDspPos)/val2)?(val2-1):((MultCodeNum -FirstDspPos)%val2-1);
                        //CurrtDspPos = 0;
                    }
                    DispMultCode();
                    ReverseCode(CurrtDspPos,3);
                } else if (in == NUMPY_PREV_WORD) {
                    if (FirstDspPos) { //第一组汉字
                        FirstDspPos -= val2;
                        CurrtDspPos = 0;
//                      ReverseMultCode(CurrtDspPos, REVERSE_TEXT);
                    }
                    DispMultCode();
                    ReverseCode(CurrtDspPos,3);
                    continue;
                } else if (in == NUMPY_NEXT_WORD) {
                    if ((FirstDspPos + NumOfDispMultBuf) < MultCodeNum) {
                        FirstDspPos += val2;
                        CurrtDspPos = 0;
//                      ReverseMultCode(CurrtDspPos, REVERSE_TEXT);
                    }
                    DispMultCode();
                    ReverseCode(CurrtDspPos,3);
                    continue;
                } else if (in == ENTER) {
                    s[0] = DispMultBuf[CurrtDspPos * 3 + 1];
                    s[1] = DispMultBuf[CurrtDspPos * 3 + 2];
                    s[2] = 0;

                    state = 0;
                    BHOffset = BIHUA_BASE;
                    ClearExtCodeBuf();
                    DispStateLine("");
                    ClearMultCodeBuf();
                    ClearDispMultBuf();

                    //恢复屏幕
                    ndk_disp_restore_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace),save_screen_select);
                    if(save_screen_select) {
                        free(save_screen_select);
                        save_screen_select = NULL;
                    }

                    return 2;
                } else if (in == BASP) {
                    state = 0;
                    if (ExtCodeNum > 0) {
                        DispMultCode();
                    } else {
                        //恢复屏幕
                        ndk_disp_restore_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace),save_screen_select);
                        if(save_screen_select) {
                            free(save_screen_select);
                            save_screen_select = NULL;
                            //                setblk(&multblock);
                        }
                    }
                } else if ((in>=IME_ZERO)&&(in<=IME_NINE)) {
                    int tmpi, ch0 = '0';
//                  char s[3];

                    ch0=in-IME_ZERO+'0';

                    //选择一个汉字
					if(in==IME_ZERO)
						tmpi=10;
					else
                    	tmpi = ch0 - '0';    //选择从1开始
                    if ((tmpi >= 1) && (tmpi <= NumOfDispMultBuf)) {
                        s[0] = DispMultBuf[(tmpi - 1) * 3 + 1];
                        s[1] = DispMultBuf[(tmpi - 1) * 3 + 2];
                        s[2] = 0;

//                      *out = s[1] << 8 | s[0];
                        state = 0;
                        BHOffset = BIHUA_BASE;
                        ClearExtCodeBuf();
                        DispStateLine("");
                        ClearMultCodeBuf();
                        ClearDispMultBuf();

                        //恢复屏幕
                        ndk_disp_restore_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace),save_screen_select);
                        //               setblk(&multblock);
                        if(save_screen_select) {
                            free(save_screen_select);
                            save_screen_select=NULL;
                        }
                        return 2;
                    }
                }

                break;
            }
        }
    }
}

/**
*   @fn    BHFindCode()
*   @brief  查找笔画码表
*   @param 无
*   @return 无
*
*/

static const char  *BHOffset;

static void BHFindCode()
{
    const char *pucOffset;
    char        tmp[sizeof(ExtCodeBuf) + 1];

    if ((ExtCodeNum == 0) || (BIHUA_BASE == NULL))
        return;

    memset(tmp, 0, sizeof(tmp));
    tmp[0] = '|';

    memcpy(tmp + 1, ExtCodeBuf, ExtCodeNum);

    if (BHOffset == BIHUA_BASE) {
        switch (tmp[1]) {
            case '1':
                BHOffset = BIHUA_1_OFFSET;
                break;
            case '2':
                BHOffset = BIHUA_2_OFFSET;
                break;
            case '3':
                BHOffset = BIHUA_3_OFFSET;
                break;
            case '4':
                BHOffset = BIHUA_4_OFFSET;
                break;
            case '5':
                BHOffset = BIHUA_5_OFFSET;
                break;
            default:
                BHOffset = BIHUA_BASE;
                break;
        }
    }
    if (!(BHOffset = strstr(BHOffset, tmp))) {
        BHOffset = BIHUA_BASE;
        goto out;
    }


    pucOffset = BHOffset;
    for (MultCodeNum = 0; MultCodeNum < 100; MultCodeNum++) {
        if ((pucOffset = strstr(pucOffset, tmp)) != NULL) {
            pucOffset = strstr(pucOffset, " ");
            memcpy(MultCodeBuf + 2 * MultCodeNum, pucOffset + 1, 2);
        } else
            break;

    }

out:
    return;
}

static int Method_symbol(char *s)
{
    static int CurrtDspPos;
    static char buffer[PUNCTUATION_LEN];
    int ch,val;

	val=sys.video->sur->width/sys.font->afontw;
	if(val>PUNCTUATION_LEN)
	    val=PUNCTUATION_LEN;
    memcpy(buffer,PUNCTUATION_STR,PUNCTUATION_LEN);
    MultCodeNum = PUNCTUATION_LEN;
    if(save_screen_select) {
        free(save_screen_select);
        save_screen_select =NULL;
    }
    save_screen_select = ndk_disp_save_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace));
    FirstDspPos = 0;
    DispState(buffer);
    CurrtDspPos = 0;
    ReverseCode(CurrtDspPos,1);

    while(1) {
        NDK_KbGetCode(0, &ch);
        switch(ch) {
            case IME_RECYCLE_KEY:      //切换输入法
                s[0] = IME_RECYCLE_KEY;
                s[1] = 0;
                return 1;
                break;
            case EKEY_LEFT:
            case IME_PREV_SCR:
                if (CurrtDspPos > 0) { //第一组汉字
                    CurrtDspPos--;
                } else if (FirstDspPos > 0) { //第一组汉字
                    FirstDspPos -= val;
                    //  CurrtDspPos = MAX_ASCII_PER_LINE - 1;
                    CurrtDspPos = 0;
                }
                DispState(buffer);
                ReverseCode(CurrtDspPos,1);
                break;
            case EKEY_RIGHT:
            case IME_NEXT_SCR:
                if (CurrtDspPos < (NumOfDispMultBuf - 1)) {
                    CurrtDspPos++;
                } else if ((FirstDspPos + val) < MultCodeNum) { //最后一组汉字
                    FirstDspPos += val;
                    //CurrtDspPos = 0;
                    CurrtDspPos = ((MultCodeNum -FirstDspPos)/val)?(val-1):((MultCodeNum -FirstDspPos)%val-1);
                }
                DispState(buffer);
                ReverseCode(CurrtDspPos,1);
                break;
            case NUMPY_PREV_WORD:
                if (FirstDspPos > 0) { //第一组汉字
                    FirstDspPos -= val;
                    DispState(buffer);
                    CurrtDspPos = 0;
                    ReverseCode(CurrtDspPos, 1);
                }
                break;
            case NUMPY_NEXT_WORD:
                if ((FirstDspPos + val) < MultCodeNum) { //最后一组汉字
                    FirstDspPos += val;
                    DispState(buffer);
                    CurrtDspPos = 0;
                    ReverseCode(CurrtDspPos, 1);
                }
                break;
            case BASP:
                if (ExtCodeNum > 0) {
                    DispMultCode();
                } else {
                    //恢复屏幕
                    ndk_disp_restore_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace),save_screen_select);
                    if(save_screen_select) {
                        free(save_screen_select);
                        save_screen_select=NULL;
                    }
                }
                return 0;
                break;
            case ENTER:
                s[0] = buffer[FirstDspPos + CurrtDspPos];
                s[1]=0;
                DispStateLine("");
                //恢复屏幕
                ndk_disp_restore_data(0,(ime_y+sys.view.y),(sys.font->fonth+sys.hspace),save_screen_select);
                if(save_screen_select) {
                    free(save_screen_select);
                    save_screen_select=NULL;
                }
                return 1;
                break;
            default:
                continue;
        }
    }
}

static int kbd_setmode(int mode)
{
    int tmp = kbd_mode;
    kbd_mode = mode;
    return tmp;
}

#define INPUTPROGRAME "/guiapp/bin/handwrite/main"

/**
 *@brief    手写输入法

 *@param    pszS        接收数据字符串，pcS若有内容（以\0为结尾的字符串）相当于已经输入的数据。
 *@param    unMaxlen    接收数据字符串的最大长度。
 *@return
 *@li   NDK_OK              操作成功
 *@li   其它EM_NDK_ERR      操作失败
*/
int ndk_KbHandwriteInput(char *pszS,uint unMaxlen)
{
    int hwpipe[2];
    pid_t pid;
    char *argv[6];
    char _path[PATH_MAX];
    char buf[5];
    char maxlen[5];
    int count;
    int ret;
    extern void ndk_getguifocus(void);

    if (pszS == NULL)
        return NDK_ERR_PARA;

    if (access(INPUTPROGRAME, X_OK) < 0) {
        return NDK_ERR_NOT_SUPPORT;
    }
    pipe(hwpipe);
    if ((pid = vfork()) == 0) {

        strcpy(_path, INPUTPROGRAME);
        chdir(dirname(_path));
        argv[0] = INPUTPROGRAME;
        argv[1] = pszS;
        sprintf(buf,"%04x",hwpipe[1]);
        argv[2] = buf;
        argv[3] = "0001";
        sprintf(maxlen,"%04x",unMaxlen-1);
        argv[4] = maxlen;
        argv[5] = (char *)0;
        setuid(0);
        if(execv(INPUTPROGRAME, argv)==-1)
            fprintf(stderr,"Failed : exec errno %s \n",strerror(errno));

    } else {
        pid = waitpid(pid, &ret, 0);
        count = read(hwpipe[0],pszS,unMaxlen);
        if(count>=0)
            pszS[count] = 0;
        ndk_getguifocus();// by zhengk  增加子进程退出后，父进程获取gui焦点 [12/11/2012]

    }
    return ret;
}

/******************** event end *******************************************/


