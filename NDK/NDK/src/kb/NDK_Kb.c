/*
* �´�½��˾ ��Ȩ����(c) 2011-2015
*
* ͳһƽ̨NDK API
* ��    �ߣ�    ��Ʒ������
* ��    �ڣ�    2012-08-17
* ��    ����    V1.00
* ����޸��ˣ�
* ����޸����ڣ�
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
** ���ӳ������ַ�����
***********************************************/
const char ChTbl[12][12]= {
    {'0',' ','<','>','(',')','&','{','}','[',']',0},    /*�ո��ᵽ����0����*/
    {'1','q','z','Q','Z','=','#','*',0},        /*�Ⱥ�= �滻ԭ���Ŀո�' 'λ��*/
    {'2','a','b','c','A','B','C',0},
    {'3','d','e','f','D','E','F',0},
    {'4','g','h','i','G','H','I',0},
    {'5','j','k','l','J','K','L',0},
    {'6','m','n','o','M','N','O',0},
    {'7','p','r','s','P','R','S',0},
    {'8','t','u','v','T','U','V',0},
    {'9','w','x','y','W','X','Y',0},
    {'.','*',',','@','\\','/','+','-','_','|',0},
    {'~','%','^',';','$','!',':','?','\'','\"',0}   /*ɾ�������õ�'`'������*�ż�*/
};

const int FKC[]= {F1,F2,F3,F4,F5,F6,F7,F8,F9,ATM1,ATM2,ATM3,ATM4,ATM5,ATM6,ATM7,ATM8,ESC,BASP,CR,ZMK,0};
static int IsFuncKey(int kc)
{
    int i;

    if (kc==0) return 0;    //kc Ϊ0,û�а�������,����Ϊ0
    for (i=0; FKC[i]!=0; i++) {
        if (kc==FKC[i]) return 1; //���ܼ�
    }
    return 0;   //���ǹ��ܼ�

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
    if (kc==0) return 0;    //û�м�����

//....................................................
    //�м����������´���
label_JustKeyCode:
    if (IsFuncKey(kc)) {
        if (kc == CR && ch0) kc = ch0;
        if (kc == ZMK) {    /*��ĸ����ӳ��Ϊ'#'����--added by lingb 20090811*/
            kc = '#';
            NDK_ScrGetxy(&x,&y);
            ndk_DispASCxy(x,y,kc);
            ndk_delay(10);
        }
        return kc;
    }

//..........................................................
    //�м����£������ǹ��ܼ����������ּ�

    if (oldkc==kc)count ++;
    else {
        oldkc = kc;
        count = 0;
    }

    ch0=kc_cnt2ASC(kc,count);

    NDK_ScrGetxy(&x,&y);
    ndk_DispASCxy(x,y,ch0);

    NDK_KbGetCode(1, &kc);      //debug  for 10,������1���ڶ�һ������
    if (kc!=0) {    //�м�����
        goto label_JustKeyCode;
    } else {    // ����500msû��������

        return ch0; // ����500msû��������������ASC�룬����
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
        ch=GetASC();        //GetASCΪʵ���������̵��ӳ���
        if (ch!=0) return ch;   //ch==0,�޼�����
    } while ( (time(0)-time1) < timeout );

    return 0;   //��ʱ������0
}


/**
 *@brief    ������̻�������
 *@return
 *@li       NDK_OK                �����ɹ�
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
 *@brief    ���س���������ϼ����ܡ�
 *@param    nSelect     0   �ر� 1   ����
 *@param    nMode       0   ������  1   ��ϼ�
 *@param    pnState     ��ȡ����������ϼ�ԭ����״̬��0--�ر� 1---������
 *@return
 *@li       NDK_OK                �����ɹ�
 *@li       ����EM_NDK_ERR    ����ʧ��
*/
NEXPORT int NDK_KbSwitch(int nSelect, int nMode,int *pnState)
{
    return NDK_ERR;
}


/**
 *@brief        ��ȡ����������ϼ��Ŀ���״̬��
 *@param    nMode   0   ������  1   ��ϼ�
 *@param    pnState     ��ȡ����������ϼ�״̬��0--�ر� 1---������
 *@return
 *@li       NDK_OK                 �����ɹ�
 *@li       ����EM_NDK_ERR     ����ʧ��
*/
NEXPORT int NDK_KbGetSwitch(int nMode,int *pnState)
{
    return NDK_ERR;
}


/**
 *@brief    ��ʱʱ���ڶ�ȡ���̰���ֵ
 *@details  �ڹ涨��ʱ�����������������������:����һ�������ȴ��ſ������ؼ��롣
 *@param    unTime  С�ڵ���0 :�޳�ʱ��һֱ�ȴ�������
                            ����:Ϊ�ȴ�ʱ��(����Ϊ��λ)
 *@param    pnCode  ��ȡ������룬���ڹ涨��ʱ����û�а������£�pnCode��ֵΪ0
 *@li           NDK_OK                 �����ɹ�
 *@li           NDK_ERR                 ����ʧ��
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
			/*�����źš���Դ״̬ˢ����Ҫʹ��SIGALRM�ź�(widget/notifier.c)���˴����ܱ����ź��жϣ������Ҫ��ʱ����*/
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
 *@brief    ��ȡ�������е��׸����̼�ֵ����������
 *@details  ��鰴���������Ƿ��а��������ж��������ؼ���,��û�м�������������0��
            һ���API����һ������ѭ����ʹ�ã�����ʹ��֮ǰӦ��NDK_KbFlash�ѻ����������
            ��NDK_KbGetCode�����ڱ����������еȴ��������������ء�
 *@param    pnCode  ��ȡ������룬�ް�������ʱpnCode��ֵΪ0
 *@return
 *@li           NDK_OK                 �����ɹ�
 *@li           NDK_ERR_PARA       �����Ƿ�
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
*@brief     �����ַ���
*@details   �Ӽ��̶���һ���Ի��з�Ϊ�ս�����ַ�����������뻺����pszBuf�С�
            ESC�����ز���ʧ��,�س�������ɷ���,��ĸ������Ӣ���ַ�����ģʽ����������ң�������������ģʽ��
*@param     pszBuf  �����ַ�������
*@param     unMinlen    ��С���봮��
*@param     unMaxlen    ������봮��
*@param     punLen  ��ȡʵ�����봮�ĳ���(>0)
*@param     emMode  ��ʾ���ͣ�
                    ȡֵINPUTDISP_NORMALʱ��ʾ�ַ���
                    ȡֵINPUTDISP_PASSWDʱ��ʾ'*'��
                    ȡֵΪINPUTDISP_OTHERʱ��pszBuf�������ݣ���\0Ϊ��β���ַ������൱���Ѿ��Ӽ��������������,������������ʾ������
*@param     unWaittime  �ȴ������ʱ�䣬����0һֱ�ȴ�������Ϊ�ȴ�������������ʱû�а��»س������Զ����أ�����TimeOut��
*@param     emControl   INPUT_CONTRL_NOLIMIT������ASCII���ַ���������ֱ�ӷ���
                        INPUT_CONTRL_LIMIT��ֻ��������С���㣬������ֱ�ӷ���
                        INPUT_CONTRL_NOLIMIT_ERETURN������ASCII���ַ���������ȴ�ȷ�ϼ�����
                        INPUT_CONTRL_LIMIT_ERETURN��ֻ��������С���㣬������ȴ�ȷ�ϼ�����
 *@return
 *@li           NDK_OK             �����ɹ�
 *@li           NDK_ERR_PARA       �����Ƿ�
 *@li                     NDK_ERR                      ����ʧ��
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
    ndk_DispASCxy(CursorPosX,CursorPosY,'_'); //��ʾ���
    do {
        if ((!fAscInput)||(count>=unMaxlen)) { //zw 20040603 add ">=" for getreturnline wait for CR ,don't enter char
            if(NDK_KbGetCode(unWaittime, &ch))
                return NDK_ERR_PARA;
        } else {
            ch=getkey(unWaittime);
        }
        switch (ch) {
            case BASP:  //�˸�
                if (count==0) {
                    ndk_DispASCxy(CursorPosX,CursorPosY,'_'); //��ʾ�¹��  //zw 040209 add
                } else {
                    count--;
                    dispverstep = 0;
                    NDK_ScrAutoUpdate(0, NULL);
                    //����CursorPosX,CursorPosY
                    ndk_DispASCxy(CursorPosX,CursorPosY,' '); //����ɹ��
                    CursorPosX -= sys.font->afontw;
                    if (((signed int)CursorPosX)<0) {
                        if(linecount>0)
                            linecount--;
                        CursorPosX=linetail[linecount]-sys.font->afontw;
                        CursorPosY -= sys.font->fonth;  //ֻҪcount >0,�Ͳ������
                    }
                    NDK_ScrAutoUpdate(1, NULL);
                    ndk_DispASCxy(CursorPosX,CursorPosY,'_'); //��ʾ�¹��
                }

                break;

            case CR:    // �س���������ɣ��ѻس�תΪ'\0'
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
                ndk_DispASCxy(CursorPosX,CursorPosY,'_'); //��ʾ��� //zw 040202 add
                break;

            case 0: //����ʱ
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
                    ndk_DispASCxy(CursorPosX,CursorPosY,'_'); //��ʾ��� //zw 040202 add
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
                                dispverstep = 1;//������Ļ��Χ������Ч
                            } else if(linecount<sizeof(linetail)-1) {
                                linecount++;
                            }
                            CursorPosX=0;
                        }
                        ndk_DispASCxy(CursorPosX,CursorPosY,'_'); //��ʾ���
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
 *@brief    �������뷨
 *@details  ͨ��������ĸ����ѡ�����뷨��
            ���벽�裺
            a. ��ƴ��������ֱ������ƴ���硰xin�����롰946������ѡ�񡣰����˸�������룬����������������ƶ�ƴ��ѡ��
            b. ��ȷ�ϡ����뱸ѡ��������ѡ����Ҫ�ĺ��֣�������������������ơ���������˸񡱼����˻ء�a�����衣
                        ����ȷ�ϡ���ѡ����Ҫ�ĺ��֡�
            c. �����뷨״̬�£���ѡ�ֵ�ʱ�򣬿���ͨ������������л���
            ���������룺
                �����ӿ������뷨������£������ּ���0�����ֱ����ţ�����������������ƶ�ѡ����ţ���ȷ�ϼ���ѡ�����ţ�

 *@param    pszS        ���������ַ�����pcS�������ݣ���\0Ϊ��β���ַ������൱���Ѿ��Ӽ�������������ݡ�
 *@param    unMaxlen    ���������ַ�������󳤶ȡ�
 *@param    emMethod        ���뷨ѡ��,����emMethodȡEM_IME֮�������ֵ����ú���Ĭ�ϼ�����ƴ���뷨��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�
 *@li       NDK_ERR_MACLLOC             �ڴ�ռ䲻��
 *@li       NDK_ERR_NOT_SUPPORT             ��֧�ָù���
*/
NEXPORT int NDK_KbHZInput(char *pszS,uint unMaxlen,EM_IME emMethod)
{
    char s[8];
    int pnType, oldauto, width = 0;
    int i = 0,j=0,k = 0,count = 1;/*i:��ǰ�ַ���λ��,j:���������,k�û���Ļ���ż���*/
    int *num_per_line = NULL;/*ÿ�λ���ʱ�����ж����ַ�(Ӣ���ַ���1��,�����ַ���2��)*/
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
        if(tmp[11]!=0xff)//�ж��Ƿ���ڴ����豸
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
      *��ԭ��pszS�е�������ʾ����������¼�������
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
                    /**< ���ڴ治�㣬��������*/
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
            /**< NDK_ScrPrintf������Ƿ��Զ�������,��,��Ҫ��¼�µ�һ��*/
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
             *���д���
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
                    num_per_line[j] = i;/**< �������������ĩβ������*/
                } else {
                    num_per_line[j] = i-pnType;/**< �������������һ��*/
                }
                /**< �������뷨״̬������*/
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

            if(hz) {//((*(p-2) & 0x80) && (*(p-1) >=0x40)) {//GBK18030���ָ�ʽ
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
             *���д���
             */
            if (CursorPosX == 0) {
                if ((j > (k-1)) && (k != 0)) {
                    /**< ����δ��*/
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
                    /**< �����ѿ�*/
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
             *δ����ʱɾ��һ���ַ�
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
 *@brief    ��ȡ���������Ĵ���״̬
 *@details  ��API���Ի�ȡ���ص�Ĵ���״̬(\ref EM_PADSTATE "EM_PADSTATE"),��APIҲ���Ի�ȡ����ֵ��\n
                                ��ʹ�ø�API��ʱ��Ҫע��:����ڵ��ø�API֮���ٵ���\ref NDK_KbHit "NDK_KbHit()"����\ref NDK_KbGetCode "NDK_KbGetCode()"
                                ��Ӱ�촥������״̬�ķ��ء�\nͨ����API��ȡ����ֵʱ����״ֵ̬����\ref   PADSTATE_KEY "PADSTATE_KEY" ʱ��
                                ��ʾ��ʱ�а��������£�����ȡ�İ���ֵ��pstPaddata->unKeycode�С�
 *@param    pstPaddata  ������������Ϣ(�ο�\ref ST_PADDATA "ST_PADDATA")
 *@param    unTimeOut   ��ʱʱ��(0��ʾ����������0��ʾ��������ʱʱ��ֵΪunTimeOut��λΪ:����)
 *@return
 *@li           NDK_OK                 �����ɹ�
 *@li       \ref NDK_ERR_PARA     "NDK_ERR_PARA"       �����Ƿ�(pstPaddataΪNULL)
 *@li       \ref NDK_ERR_TIMEOUT  "NDK_ERR_TIMEOUT"   ��ʱ����
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

