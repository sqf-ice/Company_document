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

#include <sys/select.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>

#include "NDK.h"
#include "NDK_debug.h"
#include "disp.h"
#include "kb.h"
#include "gui.h"
#include "../public/delay.h"

extern uint CursorPosX, CursorPosY;
extern ndk_statusbar_t gndk_statusbar;
extern int NDK_ScrAutoUpdate(int nNewauto, int *pnOldauto);
extern int NDK_ScrClrs(void);
extern int NDK_ScrPush(void);
extern int ndk_ScrPush(void);
extern int ndk_ScrPop(void);
extern int NDK_ScrPrintf(const char *format, ...);
extern int NDK_ScrGotoxy(uint unX, uint unY);
extern int NDK_ScrPop(void);
extern int NDK_ScrDispString(uint unX,uint unY,const char *pszS,uint unMode);
extern int NDK_ScrSetSpace(uint wspace,uint hspace);
extern int NDK_ScrRectangle(uint unX, uint unY, uint unWidth, uint unHeight, EM_RECT_PATTERNS nFill_pattern, color_t unColor);
extern int NDK_SysBeep(void);
extern int get_font_type(void);
extern int NDK_ScrSetFontType(EM_DISPFONT emType);
extern void ndk_DispASCxy(uint unCursorX,uint unCursorY,char ch);
extern char * ndk_disp_save_data(int x,int y,int h);
extern void ndk_disp_restore_data(int x,int y,int h,char *data);
extern int ndk_OneHZInput(char *pcS,int *pnType);
int NDK_KbGetCode(uint unTime, int *pnCode);
int NDK_ScrGetxy(uint *punX, uint *punY);
int NDK_KbHit(int *pnCode);
extern int ndk_getHardWareInfo(char *pinfo);
extern int ndk_KbHandwriteInput(char *pszS,uint unMaxlen);
extern int get_ime_y(void);

pthread_mutex_t ndk_getkey_mutex = PTHREAD_MUTEX_INITIALIZER;



#define MAXLINE 39

/***********************************************
** 本子程序定义字符键码
***********************************************/
const char ChTbl[12][12]= {
    {'0',' ','<','>','(',')','&','{','}','[',']',0},    /*空格提到数字0键上*/
    {'1','q','z','Q','Z','=','#','*',0},        /*等号= 替换原来的空格' '位置*/
    {'2','a','b','c','A','B','C',0},
    {'3','d','e','f','D','E','F',0},
    {'4','g','h','i','G','H','I',0},
    {'5','j','k','l','J','K','L',0},
    {'6','m','n','o','M','N','O',0},
    {'7','p','r','s','P','R','S',0},
    {'8','t','u','v','T','U','V',0},
    {'9','w','x','y','W','X','Y',0},
    {'.','*',',','@','\\','/','+','-','_','|',0},
    {'~','%','^',';','$','!',':','?','\'','\"',0}   /*删除不常用的'`'，补上*号键*/
};

const int FKC[]= {F1,F2,F3,F4,F5,F6,F7,F8,F9,ATM1,ATM2,ATM3,ATM4,ATM5,ATM6,ATM7,ATM8,ESC,BASP,CR,ZMK,0};
static int IsFuncKey(int kc)
{
    int i;

    if (kc==0) return 0;    //kc 为0,没有按键按下,不会为0
    for (i=0; FKC[i]!=0; i++) {
        if (kc==FKC[i]) return 1; //功能键
    }
    return 0;   //不是功能键

}


static int kc_cnt2ASC(int kc,int cnt)
{
    int nRet;

    if ((kc=='.')&&((cnt%(2*strlen(ChTbl[10])))>=strlen(ChTbl[10])))
        kc='*';

    if (kc=='.') kc = 0x3a;
    else if (kc=='*') kc = 0x3b;
    kc-=0x30;

    if (kc>=0&&kc<=11) {
        cnt=cnt % strlen(ChTbl[kc]);

        nRet = ChTbl[kc][cnt];
    } else nRet = 0;

    return nRet;
}


static int GetASC()
{
    int kc,oldkc=0;//sc,
    int count=0;
    int ch0=0;
    uint x,y;

    NDK_KbGetCode(1, &kc);
    if (kc==0) return 0;    //没有键按下

//....................................................
    //有键按下做以下处理
label_JustKeyCode:
    if (IsFuncKey(kc)) {
        if (kc == CR && ch0) kc = ch0;
        if (kc == ZMK) {    /*字母键盘映射为'#'符号--added by lingb 20090811*/
            kc = '#';
            NDK_ScrGetxy(&x,&y);
            ndk_DispASCxy(x,y,kc);
            ndk_delay(10);
        }
        return kc;
    }

//..........................................................
    //有键按下，但不是功能键，而是数字键

    if (oldkc==kc)count ++;
    else {
        oldkc = kc;
        count = 0;
    }

    ch0=kc_cnt2ASC(kc,count);

    NDK_ScrGetxy(&x,&y);
    ndk_DispASCxy(x,y,ch0);

    NDK_KbGetCode(1, &kc);      //debug  for 10,必需在1秒内读一个按键
    if (kc!=0) {    //有键按下
        goto label_JustKeyCode;
    } else {    // 超过500ms没读到按键

        return ch0; // 超过500ms没读到按键，计算ASC码，返回
    }

}


static int getkey(unsigned int timeout)
{
    int ch;
    long time1;
    long t;

    if (timeout<0) {
        return 0;
    }

    if (timeout==0) {
        do {
            ch=GetASC();
            if (ch!=0)return ch;
        } while (1);
    }
    time1=time(&t);
    do {
        ch=GetASC();        //GetASC为实现上述流程的子程序
        if (ch!=0) return ch;   //ch==0,无键按下
    } while ( (time(0)-time1) < timeout );

    return 0;   //超时，返回0
}


/**
 *@brief    清除键盘缓冲区。
 *@return
 *@li       NDK_OK                操作成功
*/
NEXPORT int NDK_KbFlush(void)
{
    int code;
    do {
        NDK_KbHit(&code);
    } while(code);

    return NDK_OK;
}

/**
 *@brief    开关长按键或组合键功能。
 *@param    nSelect     0   关闭 1   开启
 *@param    nMode       0   长按键  1   组合键
 *@param    pnState     获取长按键或组合键原来的状态，0--关闭 1---开启。
 *@return
 *@li       NDK_OK                操作成功
 *@li       其它EM_NDK_ERR    操作失败
*/
NEXPORT int NDK_KbSwitch(int nSelect, int nMode,int *pnState)
{
    return NDK_ERR;
}


/**
 *@brief        获取长按键或组合键的开关状态。
 *@param    nMode   0   长按键  1   组合键
 *@param    pnState     获取长按键或组合键状态，0--关闭 1---开启。
 *@return
 *@li       NDK_OK                 操作成功
 *@li       其它EM_NDK_ERR     操作失败
*/
NEXPORT int NDK_KbGetSwitch(int nMode,int *pnState)
{
    return NDK_ERR;
}


/**
 *@brief    超时时间内读取键盘按键值
 *@details  在规定的时间里读按键，读键过程如下:按下一个键，等待放开，返回键码。
 *@param    unTime  小于等于0 :无超时，一直等待读按键
                            其他:为等待时间(以秒为单位)
 *@param    pnCode  获取输入键码，若在规定的时间内没有按键按下，pnCode的值为0
 *@li           NDK_OK                 操作成功
 *@li           NDK_ERR                 操作失败
*/
NEXPORT int NDK_KbGetCode(uint unTime, int *pnCode)
{
    int ret;
    int nfds;
    fd_set readfds;
    struct timeval tv;
    socket_header_t sdata;
    video_input_data_keybd_t data;
    video_input_data_mouse_t mdata;
    sigset_t newmask, oldmask;


    NDK_KbFlush();
    if (unTime != 0) {
        tv.tv_sec=unTime;
        tv.tv_usec=0;
        if((long)unTime<0) {
            fprintf(stderr,"%s,%d\n",__FUNCTION__,__LINE__) ;
            tv.tv_sec = 0xffff;
        }
    }

again:
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sys.socket_fd, &readfds);
        data.ascii = 0;
        nfds=select(sys.socket_fd+1, &readfds, NULL, NULL, (unTime == 0)?NULL:&tv);
        if (nfds < 0) {
            if (errno == EINTR) {
                goto again;
            } else {
                return NDK_ERR;
            }
        }
        if (nfds == 0) {
            break;
        }
        if (nfds > 0) {
			/*无线信号、电源状态刷新需要使用SIGALRM信号(widget/notifier.c)，此处可能被该信号中断，因此需要暂时屏蔽*/
			sigemptyset(&newmask);
			sigaddset(&newmask, SIGALRM);
			sigprocmask(SIG_BLOCK, &newmask, &oldmask);
			pthread_mutex_lock(&ndk_getkey_mutex);
            if ((ret = socket_unix_recv(sys.socket_fd, &sdata, sizeof (sdata))) > 0) {
                if(sdata==SOCKET_DATA_KEYPAD)
				{
                    ret = socket_unix_recv(sys.socket_fd, &data, sizeof (video_input_data_keybd_t));
					pthread_mutex_unlock(&ndk_getkey_mutex);
					sigprocmask(SIG_UNBLOCK, &newmask, NULL);
				}
                else if(sdata==SOCKET_DATA_MOUSE) 
				{
                    socket_recv(sys.socket_fd, &mdata, sizeof (video_input_data_mouse_t));
					pthread_mutex_unlock(&ndk_getkey_mutex);
				    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                    continue;
                } 
				else
				{
					pthread_mutex_unlock(&ndk_getkey_mutex);
                    sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                    continue;
				}
            }
            if (ret < 0) {
				pthread_mutex_unlock(&ndk_getkey_mutex);
                sigprocmask(SIG_UNBLOCK, &newmask, NULL);
                return NDK_ERR;
            }
			sigprocmask(SIG_UNBLOCK, &newmask, NULL);
            break;
        }
    }
    if(pnCode!=NULL) {
        *pnCode = data.ascii;
    }
    return NDK_OK;
}


/**
 *@brief    获取缓冲区中的首个键盘键值，立即返回
 *@details  检查按键缓冲区是否有按键，若有读键，返回键码,若没有键按下立即返回0。
            一般该API是在一个程序循环体使用，并且使用之前应该NDK_KbFlash把缓冲区清除。
            与NDK_KbGetCode区别于本函数不进行等待，而是立即返回。
 *@param    pnCode  获取输入键码，无按键按下时pnCode的值为0
 *@return
 *@li           NDK_OK                 操作成功
 *@li           NDK_ERR_PARA       参数非法
*/
NEXPORT int NDK_KbHit(int *pnCode)
{
    socket_header_t sdata;
    video_input_data_keybd_t data;
    video_input_data_mouse_t mdata;
    int val;

    if(pnCode==NULL) {
        return NDK_ERR_PARA;
    }
    data.ascii = 0;
    val = fcntl(sys.socket_fd, F_GETFL, 0);
    fcntl(sys.socket_fd, F_SETFL, val|O_NONBLOCK);

	pthread_mutex_lock(&ndk_getkey_mutex);
    while(1) {
        if (socket_unix_recv(sys.socket_fd, &sdata, sizeof (sdata)) > 0) {
            if(sdata ==SOCKET_DATA_KEYPAD) {
                socket_unix_recv(sys.socket_fd, &data, sizeof (video_input_data_keybd_t));
                break;
            } else if(sdata==SOCKET_DATA_MOUSE) {
                socket_unix_recv(sys.socket_fd, &mdata, sizeof (video_input_data_mouse_t));
                continue;
            } else
                continue;
        }
        break;
    }
	pthread_mutex_unlock(&ndk_getkey_mutex);
    fcntl(sys.socket_fd, F_SETFL, val);
    *pnCode = data.ascii;
    return NDK_OK;
}


/**
*@brief     输入字符串
*@details   从键盘读入一个以换行符为终结符的字符串，将其存入缓冲区pszBuf中。
            ESC键返回操作失败,回车读键完成返回,字母键进入英文字符输入模式、方向键（右）返回数字输入模式。
*@param     pszBuf  接收字符串数据
*@param     unMinlen    最小输入串长
*@param     unMaxlen    最大输入串长
*@param     punLen  获取实际输入串的长度(>0)
*@param     emMode  显示类型，
                    取值INPUTDISP_NORMAL时显示字符，
                    取值INPUTDISP_PASSWD时显示'*'。
                    取值为INPUTDISP_OTHER时，pszBuf若有内容（以\0为结尾的字符串）相当于已经从键盘上输入的数据,并且用明码显示出来。
*@param     unWaittime  等待输入的时间，若是0一直等待，其他为等待的秒数。若超时没有按下回车键，自动返回，返回TimeOut。
*@param     emControl   INPUT_CONTRL_NOLIMIT：任意ASCII码字符，输满后直接返回
                        INPUT_CONTRL_LIMIT：只读数字与小数点，输满后直接返回
                        INPUT_CONTRL_NOLIMIT_ERETURN：任意ASCII码字符，输满后等待确认键返回
                        INPUT_CONTRL_LIMIT_ERETURN，只读数字与小数点，输满后等待确认键返回
 *@return
 *@li           NDK_OK             操作成功
 *@li           NDK_ERR_PARA       参数非法
 *@li                     NDK_ERR                      操作失败
*/
NEXPORT int NDK_KbGetInput(char *pszBuf,uint unMinlen,uint unMaxlen,uint *punLen,EM_INPUTDISP emMode,uint unWaittime, EM_INPUT_CONTRL emControl)
{
    int ch, au1;
    int count,i;
    int _bool;
    int fAscInput = 0;
    int linecount = 0;
    int dispverstep = 0;
    int linetail[32];
    char tmp_wspace = sys.wspace;
    char tmp_hspace = sys.hspace;


    if (pszBuf == NULL) {
        return NDK_ERR_PARA;
    }
    if(unMinlen>unMaxlen)
        return NDK_ERR_PARA;

    if(emControl>INPUT_CONTRL_LIMIT_ERETURN || emControl<INPUT_CONTRL_NOLIMIT || emMode<INPUTDISP_NORMAL || emMode > INPUTDISP_OTHER)
        return NDK_ERR_PARA;

    NDK_ScrSetSpace(0,0);
    NDK_ScrAutoUpdate(0, &au1);
    if (emMode==INPUTDISP_OTHER) {
        count=strlen(pszBuf);
        if (count>unMaxlen)
            count = unMaxlen;
        pszBuf[count]=0;
        NDK_ScrDispString(CursorPosX,CursorPosY,pszBuf,0);
        linecount=count/(sys.view.w/sys.font->afontw);
        for(i=0;i<linecount;i++)
            linetail[i]=(sys.view.w/sys.font->afontw)*sys.font->afontw;
    } else
        count=0;
    NDK_ScrAutoUpdate(1, NULL);
    ndk_DispASCxy(CursorPosX,CursorPosY,'_'); //显示光标
    do {
        if ((!fAscInput)||(count>=unMaxlen)) { //zw 20040603 add ">=" for getreturnline wait for CR ,don't enter char
            if(NDK_KbGetCode(unWaittime, &ch))
                return NDK_ERR_PARA;
        } else {
            ch=getkey(unWaittime);
        }
        switch (ch) {
            case BASP:  //退格
                if (count==0) {
                    ndk_DispASCxy(CursorPosX,CursorPosY,'_'); //显示新光标  //zw 040209 add
                } else {
                    count--;
                    dispverstep = 0;
                    NDK_ScrAutoUpdate(0, NULL);
                    //处理CursorPosX,CursorPosY
                    ndk_DispASCxy(CursorPosX,CursorPosY,' '); //清除旧光标
                    CursorPosX -= sys.font->afontw;
                    if (((signed int)CursorPosX)<0) {
                        if(linecount>0)
                            linecount--;
                        CursorPosX=linetail[linecount]-sys.font->afontw;
                        CursorPosY -= sys.font->fonth;  //只要count >0,就不会益出
                    }
                    NDK_ScrAutoUpdate(1, NULL);
                    ndk_DispASCxy(CursorPosX,CursorPosY,'_'); //显示新光标
                }

                break;

            case CR:    // 回车，输入完成，把回车转为'\0'
                if (count >= unMinlen) {
                    pszBuf[count]='\0';
                    NDK_SysBeep();
                    NDK_ScrSetSpace(tmp_wspace,tmp_hspace);
                    NDK_ScrAutoUpdate(au1, NULL);
                    if(punLen)
                        *punLen = count;
                    return NDK_OK;
                } else {
                    break;
                }

            case ESC:
                NDK_ScrSetSpace(tmp_wspace,tmp_hspace);
                NDK_ScrAutoUpdate(au1, NULL);
                return NDK_ERR;

            case RIGHT:
            case ZMK:
                if ((emControl&1) == 0) {
                    fAscInput ^= 1;
                }
                ndk_DispASCxy(CursorPosX,CursorPosY,'_'); //显示光标 //zw 040202 add
                break;

            case 0: //处理超时
                pszBuf[count]='\0';
                NDK_ScrSetSpace(tmp_wspace,tmp_hspace);
                NDK_ScrAutoUpdate(au1, NULL);
                if(punLen)
                    *punLen = count;
                return NDK_ERR_TIMEOUT;

            default:
                if (emControl&1)
                    _bool=isdigit(ch) || (ch=='.');
                else
                    _bool=isprint(ch);
                if (_bool==0 ||(dispverstep == 1)) {
                    ndk_DispASCxy(CursorPosX,CursorPosY,'_'); //显示光标 //zw 040202 add
                    break;
                } else {    //true
                    if (count<unMaxlen) {
                        pszBuf[count]=ch;
                        count++;
                        if (emMode==INPUTDISP_PASSWD)
                            ndk_DispASCxy(CursorPosX,CursorPosY,'*');
                        else
                            ndk_DispASCxy(CursorPosX,CursorPosY,ch);

                        CursorPosX += sys.font->afontw;
                        if (CursorPosX>(sys.view.w-sys.font->afontw)) {

                            CursorPosY += sys.font->fonth;
                            linetail[linecount] = CursorPosX;
                            if (CursorPosY+sys.font->fonth > sys.view.h) {
                                dispverstep = 1;//超出屏幕范围输入无效
                            } else if(linecount<sizeof(linetail)-1) {
                                linecount++;
                            }
                            CursorPosX=0;
                        }
                        ndk_DispASCxy(CursorPosX,CursorPosY,'_'); //显示光标
                    }
                }
                break;
        } //switch(ch)
    } while ( (count<unMaxlen) || (emControl&2));

    NDK_SysBeep();
    pszBuf[count]=0;
    NDK_ScrAutoUpdate(au1, NULL);
    if(punLen)
        *punLen = count;
    return NDK_OK;
}


/**
 *@brief    汉字输入法
 *@details  通过按“字母键”选择输入法。
            输入步骤：
            a. 在拼音输入栏直接输入拼音如“xin”输入“946”，并选择。按“退格”清除输入，按方向键进行左右移动拼音选择。
            b. 按确认”进入备选汉字栏，选择需要的汉字，方向键进行左移与右移。如果按“退格”键可退回‘a’步骤。
                        按“确认”键选中需要的汉字。
            c. 在输入法状态下，在选字的时候，可以通过方向键左右切换。
            标点符号输入：
                在增加开启输入法的情况下，按数字键“0”出现标点符号，按方向键进行左右移动选择符号，按确认键返选定符号；

 *@param    pszS        接收数据字符串，pcS若有内容（以\0为结尾的字符串）相当于已经从键盘上输入的数据。
 *@param    unMaxlen    接收数据字符串的最大长度。
 *@param    emMethod        输入法选择,若是emMethod取EM_IME之外的其他值，则该函数默认激活数拼输入法。
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法
 *@li       NDK_ERR_MACLLOC             内存空间不足
 *@li       NDK_ERR_NOT_SUPPORT             不支持该功能
*/
NEXPORT int NDK_KbHZInput(char *pszS,uint unMaxlen,EM_IME emMethod)
{
    char s[8];
    int pnType, oldauto, width = 0;
    int i = 0,j=0,k = 0,count = 1;/*i:当前字符串位置,j:储存的行数,k用户屏幕最多放几行*/
    int *num_per_line = NULL;/*每次换行时，共有多少字符(英文字符按1算,汉字字符按2算)*/
    char *p = NULL;
    char *pcS_bak = NULL;
    int flat_out_screen = 0;
    char tmp_wspace = sys.wspace;
    char tmp_hspace = sys.hspace;
    int userfonttype = get_font_type();
    int ret;
    int x,y;
    int ystart,yend,xstart,mode;
    EM_TEXT_ATTRIBUTE  pemOldattr;
    extern volatile int CurrMethod;
    extern int gimehandwrite;

    p = pszS;
    if (pszS == NULL)
        return NDK_ERR_PARA;

    if(strlen(pszS)>unMaxlen) {
        return NDK_ERR_PARA;
    }

    if(emMethod==IME_GBK){
        return NDK_ERR_PARA;
    }
    CurrMethod = IME_NUMPY;
    if ( (num_per_line = (int*)calloc(MAXLINE, sizeof(int))) == NULL)
        return NDK_ERR_MACLLOC;

    if ((pcS_bak = (char*)calloc(strlen(pszS)+1, sizeof(char))) == NULL) {
        free(num_per_line);
        return NDK_ERR_MACLLOC;
    }
    if (emMethod <= IME_MAXNUM && emMethod >= IME_NUMPY)
        CurrMethod=emMethod;


    if(gimehandwrite==-1) {
        char tmp[20];
        ndk_getHardWareInfo(tmp);
        if(tmp[11]!=0xff)//判断是否存在触屏设备
            gimehandwrite = 1;
        else
            gimehandwrite = 0;
    }

    if((gimehandwrite==0)&&(emMethod==IME_HANDWRITE))
        return NDK_ERR_NOT_SUPPORT;
    NDK_ScrSetSpace(0,0);
    if(sys.video->sur->width<=240)
        NDK_ScrSetFontType(DISPFONT_CUSTOM);

    if(((CursorPosY==0)&&(CursorPosX ==0))||
       ((CursorPosY + sys.font->fonth)>get_ime_y())) {
        mode = 1;
    } else
        mode = 0;

    if(mode) {
        ndk_ScrPush();
        NDK_ScrSetAttr(TEXT_ATTRIBUTE_NORMAL, &pemOldattr);
        NDK_ScrClrs();
        NDK_ScrSetAttr(pemOldattr, NULL);
    }
    ystart = CursorPosY;
    xstart = CursorPosX;
    NDK_ScrAutoUpdate(0,&oldauto);
    k = (get_ime_y() - ystart)/(sys.font->fonth);
    /*
      *把原来pszS中的内容显示出来，并记录相关数据
      */

    if (strlen(pszS)) {
        strcpy(pcS_bak,pszS);
    handwrite:
        y = ystart;
        x = xstart;
        while (i<strlen(pszS)) {
            if (*p & 0x80) {
                width = sys.font->cfontw;
                pnType = 2;
            } else {
                width = sys.font->afontw;
                pnType = 1;
            }
            if (x + width > sys.view.w) {
                j++;
                num_per_line[j] = i;
                if (y + sys.font->fonth > get_ime_y()) {
                    flat_out_screen = 1;
                }
                if (j >= (MAXLINE-1)*count) {
                    count++;
                    /**< 若内存不足，重新申请*/
                    if ( (num_per_line = (int *)realloc(num_per_line, count*MAXLINE*sizeof(int))) == NULL) {
                        free(num_per_line);
                        free(pcS_bak);
                        if(mode)
                            ndk_ScrPop();
                        NDK_ScrAutoUpdate(oldauto,NULL);
                        NDK_ScrSetSpace(tmp_wspace,tmp_hspace);
                        NDK_ScrSetFontType(userfonttype);
                        return NDK_ERR_MACLLOC;
                    }
                }
                x = 0;
                y += sys.font->fonth;
            } else {
                i += pnType;
                x += width;
                if (pnType == 1)
                    p++;
                else
                    p += 2;
            }
        }
        if (1 == flat_out_screen) {
            if(mode==0) {
                CursorPosX = 0;
            }
            NDK_ScrPrintf("%s",pszS+num_per_line[j-k+1]);
            ndk_DispASCxy(CursorPosX,CursorPosY,'_');
            /**< NDK_ScrPrintf输出后是否自动换行了,是,则要记录新的一行*/
            if (CursorPosX == 0) {
                j++;
                num_per_line[j] = i;
            }
        } else {
            NDK_ScrPrintf("%s",pszS);
            ndk_DispASCxy(CursorPosX,CursorPosY,'_');
            if (CursorPosX == 0) {
                j++;
                num_per_line[j] = i;
            }
        }
    }

    NDK_ScrAutoUpdate(1,NULL);
    s[0]=0;
    num_per_line[0] = 0;

    while ( (s[0]!=ESC) && (s[0]!=ENTER) ) {
        ndk_OneHZInput(s,&pnType);
        if ( (pnType==1 && isprint(s[0])) || (pnType==2) ) {
            uint tmp_x = CursorPosX;
            if ((i + pnType) > unMaxlen)
                continue;
            if (pnType==1) {
                width = sys.font->afontw;
                NDK_ScrPrintf("%c",s[0]);
                if(CursorPosX + width <= sys.view.w)
                    ndk_DispASCxy(CursorPosX,CursorPosY,'_');
                *p++ = s[0];
            } else if (pnType==2) {
                width = sys.font->cfontw;
                NDK_ScrPrintf(s);
                if(CursorPosX + width <= sys.view.w)
                    ndk_DispASCxy(CursorPosX,CursorPosY,'_');
                *p++ = s[0];
                *p++ = s[1];
            }
            *p = '\0';
            i += pnType;
            /*
             *换行处理
             */
            if (CursorPosX < tmp_x) {
                if(((sys.view.w-tmp_x>=sys.font->afontw)&&(sys.view.w-tmp_x<sys.font->cfontw))&&pnType==2)
                {
                    NDK_ScrGotoxy(tmp_x,CursorPosY-sys.font->fonth);
                    NDK_ScrPrintf(" ");
                    if(sys.view.w-tmp_x==sys.font->afontw)
                        NDK_ScrGotoxy(sys.font->cfontw,CursorPosY);
                    else
                        NDK_ScrGotoxy(sys.font->cfontw,CursorPosY+sys.font->fonth);
                }
                j ++;
                if (CursorPosX == 0) {
                    num_per_line[j] = i;/**< 输出的内容在行末尾，换行*/
                } else {
                    num_per_line[j] = i-pnType;/**< 输出的内容在下一行*/
                }
                /**< 超出输入法状态栏处理*/
                if ((CursorPosY+sys.font->fonth > get_ime_y())) {
                    uint tmp_y = CursorPosY-sys.font->fonth;
                    if(mode)
                        NDK_ScrClrs();
                    else {
                        NDK_ScrClrLine(ystart,get_ime_y()-1);
                        CursorPosX = 0;
                    }
                    NDK_ScrPrintf("%s",pszS+num_per_line[j-k+1]);
                    ndk_DispASCxy(CursorPosX,CursorPosY,'_');
                    CursorPosY = tmp_y;
                }
                if (j >= (MAXLINE-1)*count) {
                    count++;
                    if ( (num_per_line = (int *)realloc(num_per_line, count*MAXLINE*sizeof(int))) == NULL) {
                        free(num_per_line);
                        free(pcS_bak);
                        if(mode)
                            ndk_ScrPop();
                        NDK_ScrAutoUpdate(oldauto,NULL);
                        NDK_ScrSetSpace(tmp_wspace,tmp_hspace);
                        NDK_ScrSetFontType(userfonttype);
                        return NDK_ERR_MACLLOC;
                    }
                }
            }
        } else if(pnType == 3) {
            int oldlen = strlen(pszS);
            int newlen ;
            char space[64];
            ret = ndk_KbHandwriteInput(pszS,unMaxlen);
            switch(ret>>8) {
                case 1:
                    newlen = strlen(pszS);
                    if(mode)
                        NDK_ScrClrs();
                    else {
                        if(newlen<oldlen) {
                            memset(space,0x20,oldlen);
                            NDK_ScrDispString(xstart,ystart,space,0);
                        }

                        NDK_ScrClrLine(ystart+sys.font->fonth,get_ime_y()-1);
                        CursorPosX = xstart;
                        CursorPosY = ystart;
                    }
                    p = pszS;
                    i = 0;
                    CurrMethod=(CurrMethod==IME_MAXNUM)?0:(CurrMethod+1);
                    goto handwrite;
                    break;
                case 2:
                    s[0] = ESC;
                    break;
                case 0:
                    s[0] = ENTER;
                    continue;
                    break;
                default:
                    break;
            }
        }

        if ((BASP == s[0]) && (i != 0)) {
            char *q = pszS;
            char hz = 0;
            while(q<p) {
                if(*q&0x80) {
                    hz = 1;
                    q+=2;
                } else {
                    hz =0;
                    q+=1;
                }
            }

            if(hz) {//((*(p-2) & 0x80) && (*(p-1) >=0x40)) {//GBK18030汉字格式
                width = sys.font->cfontw;
                p -= 2;
                *p = '\0';
                i -= 2;
            } else {
                width = sys.font->afontw;
                p --;
                *p = '\0';
                i --;
            }
            /*
             *换行处理
             */
            if (CursorPosX == 0) {
                if ((j > (k-1)) && (k != 0)) {
                    /**< 滚屏未空*/
                    if(mode)
                        NDK_ScrClrs();
                    else {
                        NDK_ScrClrLine(ystart,CursorPosY);
                        if(j==k)
                            CursorPosX = xstart;
                        else
                            CursorPosX = 0;
                    }
                    NDK_ScrPrintf("%s",pszS+num_per_line[j-k]);
                    int tmp_y = CursorPosY;
                    int tmp_x = ((num_per_line[j]-num_per_line[j-1])*sys.font->afontw-width);

                    if(j==k)
                        tmp_x = ((num_per_line[j]-num_per_line[j-1])*sys.font->afontw-width+xstart);
                    if (width == sys.font->afontw)
                    {
                        ndk_DispASCxy(tmp_x,tmp_y,'_');
                    }
                    else
                    {
                         ndk_DispASCxy(tmp_x,tmp_y,'_');
                    }
                    j--;
                } else {
                    int tmp_y = CursorPosY;
                    int tmp_x;
                    if((tmp_y-ystart>=sys.font->fonth)&&(tmp_y-ystart<sys.font->fonth*2))
                        tmp_x=(num_per_line[j]-num_per_line[j-1])*sys.font->afontw-width+xstart;
                    else
                        tmp_x=(num_per_line[j]-num_per_line[j-1])*sys.font->afontw-width;
                    /**< 滚屏已空*/
                    if (width == sys.font->afontw)
                    {
                        ndk_DispASCxy(0,tmp_y,' ');
                        ndk_DispASCxy(tmp_x,tmp_y-sys.font->fonth,'_');
                    }
                    else
                    {
                        ndk_DispASCxy(0,tmp_y,' ');
                        ndk_DispASCxy(tmp_x,tmp_y-sys.font->fonth,'_');
                        ndk_DispASCxy(tmp_x+width/2,tmp_y-sys.font->fonth,' ');
                    }
                    NDK_ScrGotoxy(tmp_x,tmp_y-sys.font->fonth);
                    j--;
                }
            }
            /*
             *未换行时删除一个字符
             */
            else {
                if (width == sys.font->afontw)
                {
                    ndk_DispASCxy(CursorPosX,CursorPosY,' ');
                    if(CursorPosX-width>=sys.font->afontw)
                        ndk_DispASCxy((CursorPosX-width),CursorPosY,'_');
                    else
                        ndk_DispASCxy((CursorPosX-width),CursorPosY,' ');
                }
                else
                {
                    ndk_DispASCxy(CursorPosX,CursorPosY,' ');
                    ndk_DispASCxy((CursorPosX-width/2),CursorPosY,' ');
                    if(CursorPosX-width/2>=sys.font->afontw)
                        ndk_DispASCxy((CursorPosX-width/2),CursorPosY,'_');
                    else
                        ndk_DispASCxy((CursorPosX-width/2),CursorPosY,' ');
                }
            }
        }
        if (ESC == s[0]) {
            strcpy(pszS,pcS_bak);
        }
    }

    free(num_per_line);
    free(pcS_bak);
    if(mode)
        ndk_ScrPop();
    NDK_ScrSetSpace(tmp_wspace,tmp_hspace);
    NDK_ScrSetFontType(userfonttype);
    NDK_ScrAutoUpdate(oldauto,NULL);
    if (ESC == s[0]) {
        return NDK_ERR_QUIT;
    }

    return NDK_OK;

}

/**
 *@brief    获取像素坐标点的触屏状态
 *@details  该API可以获取像素点的触屏状态(\ref EM_PADSTATE "EM_PADSTATE"),该API也可以获取按键值。\n
                                在使用该API的时候要注意:如果在调用该API之后再调用\ref NDK_KbHit "NDK_KbHit()"或者\ref NDK_KbGetCode "NDK_KbGetCode()"
                                会影响触屏坐标状态的返回。\n通过该API获取按键值时，当状态值返回\ref   PADSTATE_KEY "PADSTATE_KEY" 时就
                                表示此时有按键被按下，所获取的按键值在pstPaddata->unKeycode中。
 *@param    pstPaddata  触屏点坐标信息(参考\ref ST_PADDATA "ST_PADDATA")
 *@param    unTimeOut   超时时间(0表示阻塞，大于0表示非阻塞超时时间值为unTimeOut单位为:毫秒)
 *@return
 *@li           NDK_OK                 操作成功
 *@li       \ref NDK_ERR_PARA     "NDK_ERR_PARA"       参数非法(pstPaddata为NULL)
 *@li       \ref NDK_ERR_TIMEOUT  "NDK_ERR_TIMEOUT"   超时错误
*/
int NDK_KbGetPad(ST_PADDATA *pstPaddata,uint unTimeOut)
{
    int ret;
    struct timeval starttv,endtv;
    int ms;
    int blockflag = 0;

    if(pstPaddata==NULL)
        return NDK_ERR_PARA;
    if(unTimeOut==0)
        blockflag=1;
    else if(unTimeOut>0)
        blockflag=0;
    else
        return NDK_ERR_PARA;
    gettimeofday(&starttv,NULL);
    do {
        ret=get_input_event(&(pstPaddata->unPadX),&(pstPaddata->unPadY),&(pstPaddata->unKeycode),blockflag);
        if(ret>0) {
            switch(ret) {
                case 2:
                    if(gndk_statusbar.show&&(gndk_statusbar.rect.y==0)) {
                        pstPaddata->unPadY-=gndk_statusbar.rect.h;
                    }
                    pstPaddata->unKeycode=0;
                    pstPaddata->emPadState=PADSTATE_DOWN;
                    break;
                case 3:
                    if(gndk_statusbar.show&&(gndk_statusbar.rect.y==0)) {
                        pstPaddata->unPadY-=gndk_statusbar.rect.h;
                    }
                    pstPaddata->unKeycode=0;
                    pstPaddata->emPadState=PADSTATE_UP;
                    break;
                default:
                    pstPaddata->emPadState=PADSTATE_KEY;
                    break;
            }
            return NDK_OK;
        }
        gettimeofday(&endtv,NULL);
        ms=(endtv.tv_sec * 1000 + endtv.tv_usec/1000)-(starttv.tv_sec * 1000 + starttv.tv_usec/1000);
    } while(ms<unTimeOut);

    return NDK_ERR_TIMEOUT;

}

/* end of this file */

