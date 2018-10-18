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

#include "NDK_HIPdefine.h"
#include "NDK_HIPPrnDrv.h"
#include "NDK_HIPgl.h"
#include "NDK.h"

static int PrnMode;     //d1����ѹ����d2����ѹ��
static int RowSpace;
//static int ErrorCode;

static char CurrPrintFont=0x22,CurrHzPrintFont=0x22,ZMDotAscX=8,ZMDotAscY=1;    //����ѡ��
static const char * CurrPrintZMDotAsc=__byAsc8x8DotBuf; //��ӡ��ĸ������
static int lineoffset = 0;      //���ڱ��浱ǰ�е�ƫ�Ƶ�ַ
static char prlinebuf[4][PR_MAXLINEWIDE];   //���ڱ��浱ǰ�еĵ���
static int CurrLineHigh = 0; //һ�����ݵĸ߶�
static int uiPrLineCount = 0;
static int PRLINEWIDE = 180;;//һ�����ݵ���󳤶�
static int g_nBorder = 0;//������߽�
static int g_nColumn = 0;//���ô�ӡ�����ּ��
static int g_nColumn_s = 0;//���ô�ӡ�����ּ��ԭʼֵ
static int PageLen = 792;
static int PageToMargin;
static int StartPos = -1;
static int LineLen;
static int LineFeedNum;
static int UnderLineFlag = 1;
static int Prn_errflag =0;

extern EM_PRN_HZ_FONT currentPrnHZFont;
extern EM_PRN_ZM_FONT currentPrnZMFont;
extern int ndk_Is_test_version;
extern EM_PRN_MODE currentMode;

void hip_PrnDlOneLine(void);

extern int NDK_HIP_PrnSetFont(EM_PRN_HZ_FONT emHZFont,EM_PRN_ZM_FONT emZMFont);
void ndk_HIP_PrnWarning()
{
    EM_PRN_HZ_FONT tmpPrnHZFont;
    EM_PRN_ZM_FONT tmpPrnZMFont;
    EM_PRN_MODE tmpMode;

    tmpPrnHZFont=currentPrnHZFont;
    tmpPrnZMFont=currentPrnZMFont;
    tmpMode=currentMode;
    
    NDK_HIP_PrnSetMode(PRN_MODE_NORMAL,0);
    NDK_HIP_PrnSetFont(PRN_HZ_FONT_24x24,PRN_ZM_FONT_24x32);
    NDK_HIP_PrnStr("ע�⣺����Ϊ���԰汾������ӡ��ҳ��Ч\r\r");
    NDK_HIP_PrnSetMode(tmpMode,0);
    NDK_HIP_PrnSetFont(tmpPrnHZFont,tmpPrnZMFont);
}

int PrintSwitch;//0�رգ�1��
int hip_setprintswitch(uint unPrnDirSwitch)
{
    PrintSwitch = unPrnDirSwitch;

    Prn_errflag = 0;

    return 0;
}

int hip_SetUnderLine(int emStatus)
{
    UnderLineFlag = emStatus;
    return 0;
}

/**
 * @brief ���ô�ӡģʽ���м��
 */
int hip_setprintmode(int mode, int row_space)
{
    if ( (mode <0) || (mode > 7) || (row_space > 7)) {
        //ErrorCode = PARAERR;
        return FAIL;
    }

    PrnMode = mode;
    if(row_space >= 0)
        RowSpace = row_space;

    if(PrnMode & 4) {
        PRLINEWIDE = 360 - g_nBorder;
        g_nColumn = g_nColumn_s;
    } else {
        PRLINEWIDE = 180 - ( g_nBorder >> 1 );
        g_nColumn = g_nColumn_s >> 1;
    }

    return 0;
}

/*---------------��ӡͼ��-----------------------
    xsize ---ͼ�εĿ��
    ysize ---ͼ�εĸ߶�
    xpos --- ͼ�ε����Ͻǵ���λ��
    ImagBufͼ���������,Ϊ��������
        ��һ���ֽڵ�һ�е�ǰ8���㣬D7Ϊ��һ����
        .......
       �Դ��������еĵ�������
    ���أ� SUCC 0 ---�����ɹ�       FAIL -1 ---ʧ��
------------------------------------------------*/
int hip_printimage(int xsize,int ysize,int xpos,char *ImgBuf)
{
    int prn_fd;
    int i,j,m,n,xsize1;
    int ycount;
    char cTemBuf[2][360];
    unsigned char mask,mask1;
    unsigned char buf[366];
    int s_g_nBorder;
    int ret = 0;

    if(access("/etc/pubkey_test",F_OK)==0) {
        if(ndk_Is_test_version==1) {
            ndk_Is_test_version=0;
            ndk_HIP_PrnWarning();
        }
    }
    s_g_nBorder = g_nBorder;
    g_nBorder = 0;

    if(!( PrnMode & 4 )) {      //����ѹ��
        PRLINEWIDE = 180;
        if(((xpos>>1)+xsize>PRLINEWIDE)||(xpos<0))
            return -1;
    } else {
        PRLINEWIDE = 360;
        if((xpos+xsize>PRLINEWIDE)||(xpos<0))
            return -1;
    }

    if(ysize<=0)
        return -1;

    //ErrorCode=0;

    if( lineoffset )
        hip_PrnDlOneLine();

    prn_fd = open(PRN_DEV_NAME, O_RDWR);
    if (prn_fd < 0) {
        printf("open dev file fail\n");
        return -1;
    }

    //if(!( PrnMode & 4 ))      //����ѹ��
    //  xpos <<=1;

    //xpos += g_nBorder;

    xsize1=(xsize+7)/8;
    ycount=0;

    do {
        mask1=0x01;
        memset(cTemBuf,0,sizeof(cTemBuf));
        for (n=0; n<16; n++) {
            for (i=0; i<xsize1; i++) {
                mask=0x80;
                for (m=0; m<8; m++) {
                    if (((mask>>m)&ImgBuf[ycount*xsize1+i])!=0) {
                        if(!( PrnMode & 2 ))            //����ѹ��
                            cTemBuf[n/8][i*8+m]|=mask1;
                        else
                            cTemBuf[n&1][i*8+m]|=mask1;
                    }
                }
            }

            if(!( PrnMode & 2 )) {          //����ѹ��
                if(n==7)
                    mask1=1;
                else
                    mask1<<=1;
            } else {
                if( n & 1 )
                    mask1<<=1;
            }

            ycount++;
            if (ycount>=ysize)
                break;
        }

        for (n=0; n<2; n++) {
            buf[0] = 0;

            if( n & 1 ) {
                if(!( PrnMode & 2 ))            //����ѹ��
                    buf[1] = 16;
                else
                    buf[1] = 1;
            } else {
                if( ycount <= 16 ) {
                    if( LineFeedNum < 0 ) {
                        LineFeedNum = -LineFeedNum;
                        buf[0] = (LineFeedNum >> 8) | 0x80;
                        buf[1] = LineFeedNum & 0xff;
                    } else {
                        buf[0] = (LineFeedNum >> 8);
                        buf[1] = LineFeedNum & 0xff;
                    }
                    LineFeedNum = 0;
                } else {
                    if(!( PrnMode & 2 ))        //����ѹ��
                        buf[1] = 16;
                    else
                        buf[1] = 15;
                }
            }

            buf[2] = xpos >> 8;
            buf[3] = xpos & 0xff;
            if(!( PrnMode & 4 )) {              //����ѹ��
                buf[4] = (xsize<<1) >> 8;
                buf[5] = (xsize<<1) & 0xff;
            } else {
                buf[4] = xsize >> 8;
                buf[5] = xsize & 0xff;
            }

            for(j=0; j<xsize; j++) {
                unsigned char temp=0;

                if(!( PrnMode & 4 )) {      //����ѹ��
                    buf[6+xpos+2*j]=cTemBuf[n&1][j];
                    buf[6+xpos+2*j+1]=0;
                } else {
                    temp = (~temp) & cTemBuf[n&1][j];
                    buf[6+xpos+j]=temp;
                    j++;
                    temp = (~temp) & cTemBuf[n&1][j];
                    buf[6+xpos+j]=temp;
                }
            }
            if( buf[0] & 0x80 )
                uiPrLineCount -= (( buf[0] & 0x7f ) << 8) + buf[1];
            else
                uiPrLineCount += (( buf[0] & 0x7f ) << 8) + buf[1];

            ret = ioctl(prn_fd, PRN_DEAL_DATA, buf);
            if(ret)  Prn_errflag = ret ;

            if (ycount>=ysize) {
                if( PrnMode & 2 ) {         //����ѹ��
                    if(!( n & 1 ))
                        continue;
                } else {
                    if(!( n & 1 )) {
                        if( ((ycount & 0xf) > 8) || (!(ycount & 0xf)) )
                            continue;
                    }
                }

                buf[0] = 0;
                if( ycount & 7 )
                    buf[1] = (ycount & 7) << 1;
                else
                    buf[1] = 16;
                if( PrnMode & 2 )           //����ѹ��
                    buf[1]--;

                buf[2] = 0;
                buf[3] = 0;
                buf[4] = 0;
                buf[5] = 0;

                uiPrLineCount += buf[1];
                ret = ioctl(prn_fd, PRN_DEAL_DATA, buf);
                if(ret)  Prn_errflag = ret ;

                if( PrintSwitch )
                    ioctl(prn_fd, PRN_PRINT_DONE, 0);

                close(prn_fd);

                g_nBorder = s_g_nBorder;
                if(PrnMode & 4)
                    PRLINEWIDE = 360 - g_nBorder;
                else
                    PRLINEWIDE = 180 - ( g_nBorder >> 1 );

                if (Prn_errflag)
                    return Prn_errflag;

                return 0;
            }
        }
    } while(1);

    if (Prn_errflag)
        return Prn_errflag;

    return 0;
}

/**
 * @brief ��������ֽ
 * @param nStepCounts
 *        ��ֽ�����ͷ��򣨾���ֵ�����������Ϊ792����������������ֽ��
 *        ʵ������������ֽ,line>0:��ֽ,<0:��ֽ,=0:������1��=2�� ��ڲ���792ȡģ
 */
int hip_FeedPrinterStepByStep(int nStepCounts)
{
    int prn_fd;
    unsigned char buf[366];
    int temp;
    int ret =0;

    if (nStepCounts > 792)
        nStepCounts = 792;
    if (nStepCounts < -792)
        nStepCounts = -792;

    prn_fd = open(PRN_DEV_NAME, O_RDWR);
    if (prn_fd < 0) {
        printf("open dev file fail\n");
        return -1;
    }

    if(nStepCounts<0)                   //��ֽ��ʱ���ȶ���4����Ȼ���ٽ�ֽ4��
        temp=-nStepCounts + 4;
    else
        temp=nStepCounts;

    buf[0] = temp >> 8;
    if(nStepCounts<0)
        buf[0] |=0x80;
    buf[1] = temp & 0xff;
    buf[2] = 0;
    buf[3] = 0;
    buf[4] = 0;
    buf[5] = 0;

    ret = ioctl(prn_fd, PRN_DEAL_DATA, buf);
    if(ret)  Prn_errflag = ret ;

    if(nStepCounts<0) {
        buf[0] = 0;
        buf[1] = 4;
        buf[2] = 0;
        buf[3] = 0;
        buf[4] = 0;
        buf[5] = 0;
        ret = ioctl(prn_fd, PRN_DEAL_DATA, buf);
        if(ret)  Prn_errflag = ret ;
    }

    if( PrintSwitch )
        ioctl(prn_fd, PRN_PRINT_DONE, 0);

    close(prn_fd);

    uiPrLineCount += nStepCounts;

    return 0;
}
#if 1
//#define FONT_DEV  "/dev/mtd6"
#define FONT_DEV    "/guiapp/share/fonts/font_h.bin"

static int GetNFFont(unsigned char *p, int offset, int size)
{
    int fd;

    fd = open(FONT_DEV, O_RDONLY);
    if (fd < 0) {
        printf("open mtd6 err\n");
        return -1;
    }
    //printf("offset=%08x size=%d\n",offset,size);
    lseek(fd, offset, 0);
    read(fd, p, size);
    close(fd);
    return 0;
}

/*******************************************************************************
* FUNCTION:
*       y  �����ϷŴ���
*
* DESCRIPTION:
*       y����λΪ�ֽ�
*
* INPUTS:
*        iXMagnify,iYMagnify �ķŴ���,x,yΪch1�еĻ������е����С
*
* OUTPUTS:
*        pstr[][]ȫ�ֱ������ش�ӡ����
*
* RETURNS:
*        None
*
* COMMENTS:
*
*******************************************************************************/
static unsigned char pstr[6*48];  //����Ŵ�ѹ�����壬ע�����ڻ���pstr��С������
//�Ŵ��ĵ����С���ܳ���pstr�Ĵ�С
int hip_yDotchange(unsigned char * ch1, int iYMagnify,
                   int x,int y)
{
    int i,j,k,iMagnify;
    unsigned long int ulmask,uloldmask;
    unsigned char mask;//,ucmask,ucTemp;//tmp;
    //char cMagnifyflag=0;
    union {
        unsigned char ch[4];
        unsigned long int in;
    } pch4;

    mask=0xff;
    if (!(iYMagnify%2)) { //���ֻ���Ŵ���ѹ��
        iMagnify=iYMagnify/2;
    } else iMagnify=iYMagnify;
    /*----------------����Ŵ�n��--------------------------*/
    if (iMagnify==1) { //���û�зŴ�������ֱ�ӿ���
        for (i=0; i<y; i++) {
            memcpy(pstr+48*i,ch1+i*x,x);
        }
    } else if((iMagnify*y)<=6&&iMagnify>1) {
        //cMagnifyflag=1;
        mask=0xff;
        uloldmask=(int)(mask>>(8-iMagnify));
        for (k=0; k<y; k++) {
            for (j=0; j<x; j++) {
                mask=0x01;
                pch4.in=0;
                ulmask=uloldmask;
                for(i=0; i<8; i++) {
                    if (*(ch1+j+k*x)&mask) {
                        pch4.in|=ulmask;
                    }
                    mask<<=1;
                    ulmask<<=iMagnify;
                }//��λ�ں�
                for (i=0; i<iMagnify; i++) {
                    pstr[i*48+j+k*48*iMagnify]=pch4.ch[i];
                }
            }
        }
    } else
        return -1;
    return SUCC;
}

/*******************************************************************************
* FUNCTION:
*       x  �����ϷŴ���
*
* DESCRIPTION:
*       x����λΪ��
*
* INPUTS:
*        iXMagnify,iYMagnify �ķŴ���,iX,iY,��ǰȫ�ֱ����еĻ�������С
*
* OUTPUTS:
*        pstr[][]ȫ�ֱ������ش�ӡ����
*
* RETURNS:
*        None
*
* COMMENTS:
*
*******************************************************************************/
void hip_xDotchange(int iXMagnify,int iX,int iY)
{
    int i,j,n,iMagnify;
    char cMagnifyflag=0;
    if (!(iXMagnify%2)) {
        iMagnify=iXMagnify/2;
    } else iMagnify=iXMagnify;
    /*----------------����Ŵ�n��--------------------------*/
    if (iMagnify>1&&iMagnify*iX<=48) {
        unsigned char ucTembuf[6*48];
        cMagnifyflag=1;
        for (n=0; n<iY; n++) {
            for (j=0; j<iMagnify; j++) {
                for (i=0; i<iX; i++) {
                    ucTembuf[j+i*iMagnify+n*48]=pstr[i+n*48];
                }
            }
        }
        memcpy(pstr,ucTembuf,6*48);
    }
}
/*******************************************************************************
* FUNCTION:
*       x y ������ѹ������
*
* DESCRIPTION:
*       x����λΪ�㣬y�����ϵ�λΪ�ֽ�
*
* INPUTS:
*        iXMagnify,iYMagnify �ķŴ���,iX,iY,��ǰȫ�ֱ����еĻ�������С
*
* OUTPUTS:
*        pstr[][]ȫ�ֱ������ش�ӡ����
*
* RETURNS:
*        None
*
* COMMENTS:
*
*******************************************************************************/
void hip_XYCompress(int iXMagnify,int iYMagnify,int iX,int iY)
{
    unsigned char mask,ucmask,ucTemp;//tmp;
    int i,j,k,m,n,iMagnify;
    char cXMagnifyflag=0;   //Ĭ�ϲ���ѹ��
    char cYMagnifyflag=0;   //Ĭ�ϲ���ѹ��
    unsigned char byBuf48x48[48][48], byBuf24x24[24][24];

    //x y ����Ҫѹ��
    //�Ż��㷨������4����ϲ���һ����
    if((iYMagnify%2)&&(iXMagnify%2)) {

        memset(byBuf48x48,0,sizeof(byBuf48x48));
        memset(byBuf24x24,0,sizeof(byBuf24x24));

        //λ���ֽ�ת��

        for(i=0; i<iX; i++) {
            mask = 0x01;
            for(j=0; j<iY*8; j++) {
                byBuf48x48[i][j] = ((pstr[i+(j/8)*(48)]&mask) != 0);//iy*8
                mask <<= 1;
                if(mask == 0x00) {
                    mask = 0x01;
                }
            }
        }

        // 48X48->24X24
        for (i=0; i<(iX/2); i++) {
            for(j=0; j<((iY*8/2)); j++) {
                int cnt;

                cnt =  byBuf48x48[i*2][j*2]   + byBuf48x48[i*2][j*2+1]
                       +byBuf48x48[i*2+1][j*2] + byBuf48x48[i*2+1][j*2+1];
                byBuf24x24[i][j] =  (cnt+2)/4;
            }
        }
        // ���ֽڷ�ʽ���
        memset(pstr,0,sizeof(pstr));//

        for (i=0; i<(iX/2); i++) {
            mask=0x01;
            for(j=0; j<(iY*8/2); j++) {
                if(byBuf24x24[i][j])
                    pstr[i+(j/8)*(48)] |= mask;
                mask<<=1;
                if(mask==0)
                    mask=0x01;
            }
        }

        return;
    }

    if ((iYMagnify%2)) {     //���ֻ���Ŵ���ѹ��(ż��)
        iMagnify=iYMagnify; //�õ���ʵ�Ŵ���
    } else {
        iMagnify=iYMagnify/2;  // ����ԭ���ķŴ���

    }
    if(iMagnify>1)
        cYMagnifyflag=1;
    if (iYMagnify%2) {


        //�������Ҫѹ��һ��
        for (j=0; j<iX; j++) {
            k=0;
            // y<2*3
            for (m=0; m<iY*iMagnify; m++) {
                mask=0x08;      //0000 1000
                ucmask=0xc0;    //1100 0000
                ucTemp=0;
                //
                for(i=0; i<4; i++) {
                    //cMagnifyflag�����Ƿ��Ѿ��Ŵ���
                    if (cYMagnifyflag?((*(pstr+j+48*m)&ucmask)==ucmask):(*(pstr+j+48*m)&ucmask)) {
                        // ������ϲ���һ����
                        ucTemp|=mask;
                    }
                    mask>>=1;
                    ucmask>>=2;
                }

                // ������ȷ�����ݵ�Ŀ��ص�
                *(pstr+j+48*m)=0;
                if ((int)(m%2)) {
                    *(pstr+j+k)=*(pstr+j+k)|(ucTemp<<4);
                    k+=48;
                } else {
                    *(pstr+j+k)|=ucTemp;
                }
            }
        }
        return;
    }

    if ((iXMagnify%2)) {     //���ֻ���Ŵ���ѹ��(ż��)
        iMagnify=iXMagnify; //�õ���ʵ�Ŵ���
    } else {
        iMagnify=iXMagnify/2;  // ����ԭ���ķŴ���

    }
    if(iMagnify>1)
        cXMagnifyflag=1;
    // x �����ϱ���ѹ��
    if(iXMagnify%2) {


        // iY=4
        for (n=0; n<iY; n++) {
            //  i<16*3/2=24
            //��8������д���(����һ�У���48��6������48��)
            for (i=0; i<iX/2; i++) { //48iX*iXMagnify/2
                if (i>0) {
                    pstr[n*48+i]=0;
                }
                //ѹ���㷨���ұ���λ   ||| |||
                //
                pstr[n*48+i]|=pstr[n*48+i*2];
                cXMagnifyflag?(pstr[n*48+i]&=pstr[n*48+i*2+1]):(pstr[n*48+i]|=pstr[n*48+i*2+1]);

            }
        }
    }

}
// int iXMagnify,int iYMagnify����Ŵ���������Ŵ���
// x,y��ǰ�ĵĵ����С16��2����48��2
// ע�⣬���ڵ��ó�������ƣ�һ����������������ٺ������
void hip_DotChange(unsigned char * ch1, int iXMagnify,int iYMagnify,
                   int x,int y)
{
    int iTempX,iTempY;
    //����Ŵ� iYMagnify��
    hip_yDotchange(ch1,iYMagnify,x,y);

    if (iYMagnify%2) {              //������Ҫ��С
        if(iYMagnify>1)
            iTempY=iYMagnify*y;     //���Ŵ󵽵ı���
        else
            iTempY=y;
    } else
        iTempY=iYMagnify*y/2;

    //����Ŵ� iXMagnify��
    hip_xDotchange(iXMagnify,x,iTempY);
    if (iXMagnify%2) {              //������Ҫ��С
        if(iXMagnify>1)
            iTempX=(iXMagnify)*x;   //���Ŵ󵽵ı���
        else
            iTempX=x;
    } else
        iTempX=iXMagnify*x/2;

    //x y ������ͬʱѹ��1��
    hip_XYCompress(iXMagnify,iYMagnify,iTempX,iTempY);
}

/**
 * @brief ��ȡ������ӡ������
 */
int hip_getprintertype()
{
    return PRINTTYPE_HIP;
}

#if 0
/************************************************************************/
/* ��ת��������                                                         */
/************************************************************************/
void hip_RotateDot(unsigned char* src,unsigned char* dest, int *h, int *w, int rotatetype)
{
    char byBuf48x48s[48][48];
    char byBuf48x48d[48][48];
    unsigned char mask;
    char i,j;
    int temp;

    memset(byBuf48x48s, 0, sizeof(byBuf48x48s));
    memset(byBuf48x48d, 0, sizeof(byBuf48x48d));

    //λ���ֽ�ת��
    for(i=0; i<*w; i++) {
        mask = 0x01;
        for(j=0; j<*h; j++) {

            byBuf48x48s[i][j] = ((src[i+(j/8)*(48)]&mask) != 0)?1:0;
            mask <<= 1;
            if(mask == 0x00) {
                mask = 0x01;
            }

        }
    }
    switch(rotatetype) {
        case ROTATE_270:
            //��ʱ����ת270
            for(i=0; i<48; i++) {
                for(j=0; j<48; j++) {
                    byBuf48x48d[i][j]=byBuf48x48s[47-j][i];
                }
            }
            //�Ƶ���ȷ��λ��
            memset(byBuf48x48s, 0, sizeof(byBuf48x48s));
            for(i=0; i<*w; i++) {
                for(j=0; j<*h; j++) {
                    byBuf48x48s[j][i]=byBuf48x48d[j][48-(*w)+i];
                }
            }
            temp = *h;
            *h = *w;
            *w = temp;
            break;
        case ROTATE_180:
            //��ת180
            for(i=0; i<48; i++) {
                for(j=0; j<48; j++) {
                    byBuf48x48d[i][j]=byBuf48x48s[47-i][47-j];
                }
            }
            //�Ƶ���ȷ��λ��
            memset(byBuf48x48s, 0, sizeof(byBuf48x48s));
            for(i=0; i<*w+1; i++) {
                for(j=0; j<*h; j++) {
                    byBuf48x48s[i][j]=byBuf48x48d[48-(*w)+i][48-(*h)+j];
                }
            }
            break;

        case ROTATE_90:
            //��ʱ����ת90
            for(i=0; i<48; i++) {
                for(j=0; j<48; j++) {
                    byBuf48x48d[i][j]=byBuf48x48s[j][47-i];
                }
            }
            //�Ƶ���ȷ��λ��
            memset(byBuf48x48s, 0, sizeof(byBuf48x48s));
            for(i=0; i<*w; i++) {
                for(j=0; j<*h; j++) {
                    byBuf48x48s[j][i]=byBuf48x48d[48-(*h)+j][i];
                }
            }
            temp = *h;
            *h = *w;
            *w = temp;
            break;

        case ROTATE_0:
            //��ת0
            break;
    }
    //�����ֽ�˳��
    *h=8*((*h+7)/8);
    *w=8*((*w+7)/8);
    for (i=0; i<(*w); i++) {
        mask=0x01;
        for(j=0; j<(*h); j++) {
            if(byBuf48x48s[i][j])
                /*              dest[k] |= mask;
                            k++;
                */              dest[i+(j/8)*(48)] |= mask;
            mask<<=1;
            if(mask==0)
                mask=0x01;
        }
    }

}
/************************************************************************/
/* ת���ɵ�����ʽ                                                       */
/************************************************************************/
void hip_Convertopic(char * inbuf,int xsize,int ysize,char *outbuf)
{
    int i,j,k,lin =0;
    unsigned char mask;
    unsigned char temp ;

    mask = 0x01;
    for(j =0 ; j<ysize; j++) {
        for(i =0 ; i< (xsize/8); i++) {
            temp =0 ;
            for(k=0; k<8; k++) {
                if(mask&inbuf[i*8+k+lin*48])
                    temp|=0x01<<(7-k);
            }
//          outbuf[j*xsize/8+i] =temp;
            outbuf[j*6+i] =temp;  //48/8=6
//          outbuf[m] =temp;
//          m++;
        }
        mask<<=1;
        if(mask==0x00) {
            mask =0x01;
            lin++;
        }
    }
    return ;
}

/************************************************************************
���ܣ�ȡ�ú��ֺ���ĸ������Ϣ������ȡ������Ϣ
���ݺ��ֻ���ĸ��Ϣ���ظú��ֻ���ĸ�ĵ���
 ���룺
    outtype�����ص�������ͣ��� TYPE_PIC������ͼ�ε�����ʽ
                                TYPE_DOT      ���������ʽ
����codebuf:    ���ֻ���ĸ�ı���; ��"��","A",ע���������ֻ��Ϊ1�����ֻ�һ����ĸ
font:       ��������      �� HZ16x16
rotatetype:     ��ת����
                ROTATE_0 ����ת
                ROTATE_90 ��ת90��
                ROTATE_180 ��ת180��
                ROTATE_270 ��ת270��
�������outbuf:     ������Ϣ
����    FAIL:       ʧ�ܣ���������
    SUCC:       �ɹ�
************************************************************************/
int hip_GetCharacterDotMatrix(int outtype, unsigned char *codebuf,int font, int rotatetype,TYPEDOTMATRIX *outbuf)
{
    //����
    int cX,cY;
    int oldfont;
    int h = 48, w = 48;
    char tmpoutbuf[6*48];
    char picbuf[6*48];
    char *pbuf;
    extern int g_printfont;

    if((outtype !=TYPE_PIC)&&(outtype !=TYPE_DOT) )
        return FAIL;

    //�Ƿ�����
    if(rotatetype<ROTATE_0||rotatetype>ROTATE_270)
        return FAIL;

    oldfont = g_printfont;  //���ݴ�ӡ����
    hip_setprintfont(font);
    //���ֵ�����Ϣ
    if(codebuf[0]&0x80) {
        unsigned char offset[32];
//      unsigned char hz16x16[32];
//      offset = hz16x16;
//      offset = (unsigned char *)(HZDOTBASE+((codebuf[0]-(unsigned char)0x81)*191+codebuf[1]-(unsigned char)0x40)*32);
        GetNFFont(offset, ((codebuf[0]-(unsigned char)0x81)*191+codebuf[1]-(unsigned char)0x40)*32, 32);

        memset(pstr,0,sizeof(pstr));
        //�������
        {
            cX=0;
            if ((cX=(CurrHzPrintFont&0xf0)>>4)==5) {
                hip_DotChange(offset,3,2,16,2);
                hip_DotChange(pstr,1,2,48,2);
                cX=12;
                cY=2;
            } else {
                cY=CurrHzPrintFont&0x0f;
                hip_DotChange(offset,cX,cY,16,2);
                cX=cX*8;
            }
        }
        w=cX;
        h=cY*8;
    }
    //��ĸ������Ϣ
    else {
        char cX,cY;
        unsigned char * ucZMdot;
        cX=(CurrPrintFont&0xf0)>>4;
        cY=CurrPrintFont&0x0f;

        //5*16=0x22
        ucZMdot=(unsigned char *)(CurrPrintZMDotAsc+((codebuf[0]-0x20)*ZMDotAscX*ZMDotAscY));
        hip_DotChange(ucZMdot,cX,cY,ZMDotAscX,ZMDotAscY);
        cX=(ZMDotAscX*(cX>>1));
        cY=((cY>>1)*ZMDotAscY);

        w = cX;//8*((cX+7)/8);
        h = cY*8;
    }

    //��ת����
    memset(tmpoutbuf,0,sizeof(tmpoutbuf));
    memset(picbuf,0,sizeof(picbuf));
    hip_RotateDot(pstr,(unsigned char*)tmpoutbuf, &h, &w, rotatetype);
    //�����ݸ��Ƶ�Ŀ�껺������
    pbuf=outbuf->buf;
    if(outtype == TYPE_PIC) {
        hip_Convertopic(tmpoutbuf,w,h,picbuf);
        memcpy(pbuf, picbuf, sizeof(picbuf));
    } else {
        memcpy(pbuf, tmpoutbuf, sizeof(tmpoutbuf));
    }
    outbuf->xsize=w;
    outbuf->ysize=h;
    hip_setprintfont(oldfont);  //��ԭ��ӡ����
    return SUCC;
}
#endif

int hip_sendHZtolinebuf(char * HZDot)
{
    char cX,cY,i,j;
    unsigned char ucHZdot[32];
    //ucHZdot=(unsigned char *)(HZDOTBASE+((HZDot[0]-(unsigned char)0x81)*191+HZDot[1]-(unsigned char)0x40)*32);//GB18030

    GetNFFont(ucHZdot, ((HZDot[0]-(unsigned char)0x81)*191+HZDot[1]-(unsigned char)0x40)*32, 32);
//  for(i=0;i<32;i++)
//      printf("%02x ",ucHZdot[i]);
    //memset(ucHZdot,0x55,32);
    if ((cX=(CurrHzPrintFont&0xf0)>>4)==5) {
        if(lineoffset>(PRLINEWIDE-12))  {
            lineoffset=PRLINEWIDE;
            return 1;
        }
        hip_DotChange(ucHZdot,3,2,16,2);
        hip_DotChange(pstr,1,2,48,2);
        cX=12;
        cY=2;
    } else {
        cY=CurrHzPrintFont&0x0f;
        if(lineoffset>(PRLINEWIDE-cX*8))    {
            lineoffset=PRLINEWIDE;
            return 1;
        }
        hip_DotChange(ucHZdot,cX,cY,16,2);
        cX=cX*8;
    }

    if(lineoffset>(PRLINEWIDE-cX)) {
        lineoffset=PRLINEWIDE;
        return 1;
    }

    for (i=0; i<cY; i++) {
        memcpy(&prlinebuf[4-cY][lineoffset]
               +i*PR_MAXLINEWIDE,(pstr+i*48),cX);
    }

    if( !UnderLineFlag ) {
        for(j=0; j<cX; j++)
            prlinebuf[3][lineoffset+j] |= 0x80;
    }

    lineoffset+=cX;
    lineoffset += g_nColumn;
    if(CurrLineHigh<cY)
        CurrLineHigh=cY;
    return SUCC;
}

int hip_sendZMtolinebuf(char ZMDot)
{
    char cX,cY,i,j;
    unsigned char * ucZMdot;
    cX=(CurrPrintFont&0xf0)>>4;
    cY=CurrPrintFont&0x0f;
    if(lineoffset>(PRLINEWIDE-ZMDotAscX*(cX>>1)))   {
        lineoffset=PRLINEWIDE;
        return 1;
    }
    ucZMdot=(unsigned char *)(CurrPrintZMDotAsc+((ZMDot-0x20)*ZMDotAscX*ZMDotAscY));
    hip_DotChange(ucZMdot,cX,cY,ZMDotAscX,ZMDotAscY);
    cX=(ZMDotAscX*(cX>>1));
    cY=((cY>>1)*ZMDotAscY);

    if(lineoffset>(PRLINEWIDE-cX)) {
        lineoffset=PRLINEWIDE;
        return 1;
    }

    for (i=0; i<cY; i++) {
        memcpy(&prlinebuf[4-cY][lineoffset]
               +i*PR_MAXLINEWIDE,(pstr+i*48),cX);
    }

    if( !UnderLineFlag ) {
        for(j=0; j<cX; j++)
            prlinebuf[3][lineoffset+j] |= 0x80;
    }

    lineoffset+=cX;
    lineoffset += g_nColumn;
    if(CurrLineHigh<cY)
        CurrLineHigh=cY;
    return SUCC;
}

/*------------------------------------------------------------------------------------
����ASCII��ӡ����
    Ŀǰֻ�ṩDOT8x16,DOT8x8,DOT6x8,DOT5x7
��8λ ΪASC��
��8λ Ϊ����
CurrHzPrintFont��CurrPrintFont�ֱ�����ֺ���ĸ�ķŴ�������8λΪ����Ŵ�����2
��8λΪ����Ŵ�����2.
---------------------------------------------------------------------------------*/
int g_printfont; //���ڱ��ݵ���ת��֮ǰ������
int hip_setprintfont(int font)
{
    //ErrorCode=0;
    if(font<0) { //��֤��������Ч��
        //ErrorCode=PARAERR;//linyl 2012.5.13 ���������ֵerrorcode
        return FAIL;
    }

    g_printfont = font;
    //ErrorCode=0;
    switch((font >> 8)&0xff) {
        case HZ24x24:
            CurrHzPrintFont=0x33;
            break;
        case HZ16x32:
            CurrHzPrintFont=0x24;
            break;
        case HZ32x32:
            CurrHzPrintFont=0x44;
            break;
        case HZ32x16:
            CurrHzPrintFont=0x42;
            break;
        case HZ24x32:
            CurrHzPrintFont=0x34;
            break;
        case HZ16x16:
            CurrHzPrintFont=0x22;
            break;
        case HZ12x16:
            CurrHzPrintFont=0x52;
            break;
        case HZ16x8:
            CurrHzPrintFont=0x12;// ʵ�ʶ�ӦHZ8x16
            break;
        case 0:
            break;
    }

    switch(font & 0xff) {
        case DOT8x16:
            CurrPrintFont=0x22;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
            break;
        case DOT16x16:
            CurrPrintFont=0x42;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
            break;
        case DOT16x32:
            CurrPrintFont=0x44;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
            break;
        case DOT24x32:
            CurrPrintFont=0x64;
            CurrPrintZMDotAsc=__byAsc16x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=2;
            break;
        case DOT6x8:
        case DOT8x8:
            CurrPrintZMDotAsc=__byAsc8x8DotBuf;
            ZMDotAscX=8;
            ZMDotAscY=1;
            CurrPrintFont=0x22;
            break;
        case DOT5x7:
            CurrPrintZMDotAsc=Ascii5x7Dot;
            ZMDotAscX=5;
            ZMDotAscY=1;
            CurrPrintFont=0x22;
            break;
        case DOT5x16:
            CurrPrintZMDotAsc=__byAsc5x16DotBuf;
            ZMDotAscX=5;
            ZMDotAscY=2;
            CurrPrintFont=0x22;
            break;
        case DOT10x16:
            CurrPrintZMDotAsc=__byAsc10x16DotBuf;
            ZMDotAscX=10;
            ZMDotAscY=2;
            CurrPrintFont=0x22;
            break;
        case DOT10x8:
            CurrPrintZMDotAsc=__byAsc10x8DotBuf;
            ZMDotAscX=10;
            ZMDotAscY=1;
            CurrPrintFont=0x22;
            break;
        case 0:
            break;
            /*  case DOT8x32:
                    CurrPrintFont=0x24;
                    CurrPrintZMDotAsc=__byAsc16x8DotBuf;
                    ZMDotAscX=8;
                    ZMDotAscY=2;
                    break;*/

    }
    return SUCC;
}

int hip_getprinterstatus()
{
    int prn_fd;
    int prstat;

    prn_fd = open(PRN_DEV_NAME, O_RDWR);
    if (prn_fd < 0) {
        printf("open dev file fail\n");
        return -1;
    }

    ioctl(prn_fd, PRN_READ_STATE, &prstat);

    close(prn_fd);

    switch(prstat) {
        case PR_ERR_INIT://��Դ
            return PRNOPOWER;
            break;
        case PR_ERR_PAPER://ȱֽ
            return PRNOPAPER;
            break;
        case PR_ERR_BUSY:
            return PRNOTREADY;//��ӡ��δ����
            break;
        case PR_ERR_OK:
            break;
        case PR_ERR_DONE:
            return PRINTOVER;
            break;
        case PR_ERR_HPInvalid://���ʧ��
            return PRHPInvalid;
            break;
        default:
            return PRUNKOWNSTATUS;
            break;
    }
    return PROK;
}

int hip_getprinterversion(char *buf)
{
    int prn_fd;

    prn_fd = open(PRN_DEV_NAME, O_RDWR);
    if (prn_fd < 0) {
        printf("open dev file fail\n");
        return -1;
    }

    ioctl(prn_fd, PRN_GET_VERSION, buf);

    close(prn_fd);

    return 0;
}

int hip_setprintpagelen(int pagelen)
{
    //ErrorCode=0;

    if((pagelen< 0)||(pagelen)>2370) {
        //ErrorCode=PARAERR;
        return FAIL;
    }

    PageLen = pagelen;

    return SUCC;
}

int hip_clrprintbuf(void)
{
    int prn_fd;

    prn_fd = open(PRN_DEV_NAME, O_RDWR);
    if (prn_fd < 0) {
        printf("open dev file fail\n");
        return -1;
    }

    ioctl(prn_fd, PRN_CLEAR_BUF, 0);

    close(prn_fd);

    StartPos = -1;
    LineLen = 0;
    lineoffset = 0;
    CurrLineHigh = 0;
    memset(prlinebuf,0,sizeof(prlinebuf));
    LineFeedNum = 0;

    return 0;
}

void hip_PrnPrintDone(void)
{
    int prn_fd;

    prn_fd = open(PRN_DEV_NAME, O_RDWR);
    if (prn_fd < 0) {
        printf("open dev file fail\n");
        return;
    }

    ioctl(prn_fd, PRN_PRINT_DONE, 0);

    close(prn_fd);
}

void hip_PrnStart(void)
{
    if( PrintSwitch )
        return;

    if( lineoffset )
        hip_print("\n");
}

void hip_PrnDlOneLine(void)
{
    int i,j;
    int prn_fd;
    int StartPos1;
    int ret =0;

    unsigned char buf[366];

    if( (StartPos == -1) || !LineLen ) { //����
        if( PrnMode & 2 )                   //����ѹ��
            LineFeedNum +=8;
        else
            LineFeedNum +=16;
    } else {
        prn_fd = open(PRN_DEV_NAME, O_RDWR);
        if (prn_fd < 0) {
            printf("open dev file fail\n");
            return;
        }

        if( PrnMode & 2 ) {                 //����ѹ����������������
            for(j=0; j<LineLen; j++) {
                if(CurrLineHigh & 1) {      //�����У���������Ųһ��
                    prlinebuf[0][StartPos+j] = prlinebuf[1][StartPos+j];
                    prlinebuf[1][StartPos+j] = prlinebuf[2][StartPos+j];
                    prlinebuf[2][StartPos+j] = prlinebuf[3][StartPos+j];
                    prlinebuf[3][StartPos+j] = 0;
                }
                for(i=(CurrLineHigh+1)/2; i>0; i--) {
                    int k;
                    unsigned char temp1,temp2;

                    temp1 = prlinebuf[4-i*2][StartPos+j];
                    temp2 = prlinebuf[5-i*2][StartPos+j];
                    prlinebuf[4-i*2][StartPos+j] = 0;
                    prlinebuf[5-i*2][StartPos+j] = 0;
                    for(k=0; k<4; k++) {
                        if( temp1 & (1<<(k<<1)) )
                            prlinebuf[4-i*2][StartPos+j] |= 1 << k;
                        if( temp1 & (1<<((k<<1)+1)) )
                            prlinebuf[5-i*2][StartPos+j] |= 1 << k;
                        if( temp2 & (1<<(k<<1)) )
                            prlinebuf[4-i*2][StartPos+j] |= 1 << (k+4);
                        if( temp2 & (1<<((k<<1)+1)) )
                            prlinebuf[5-i*2][StartPos+j] |= 1 << (k+4);
                    }
                }
            }
        }

        if( PrnMode & 4 ) {                         //����ѹ��
            StartPos1 = StartPos + g_nBorder;
            buf[2] = (StartPos1) >> 8;
            buf[3] = (StartPos1) & 0xff;
            buf[4] = (LineLen) >> 8;
            buf[5] = (LineLen) & 0xff;
        } else {
            StartPos1 = (StartPos << 1) + g_nBorder;
            buf[2] = (StartPos1) >> 8;
            buf[3] = (StartPos1) & 0xff;
            buf[4] = (LineLen<<1) >> 8;
            buf[5] = (LineLen<<1) & 0xff;
        }

        for(i=CurrLineHigh; i>0; i--) {
            unsigned char last=0;

            if( PrnMode & 2 ) {                     //����ѹ��
                if( i & 1 )
                    i++;
            }

            buf[0] = LineFeedNum >> 8;
            buf[1] = LineFeedNum & 0xff;

            for(j=0; j<LineLen; j++) {
                if( PrnMode & 4 ) {                 //����ѹ��
                    last = (~last) & prlinebuf[4-i][StartPos+j];
                    buf[6+StartPos1+j] = last;
                } else {
                    buf[6+StartPos1+j*2] = prlinebuf[4-i][StartPos+j];
                    buf[6+StartPos1+j*2+1] = 0;
                }
            }
            uiPrLineCount += LineFeedNum;
            ret = ioctl(prn_fd, PRN_DEAL_DATA, buf);
            if(ret)  Prn_errflag = ret ;

            if( PrnMode & 2 )                       //����ѹ��
                LineFeedNum = 1;
            else
                LineFeedNum = 16;

            if( PrnMode & 2 ) {                     //����ѹ��
                i--;

                last = 0;

                buf[0] = LineFeedNum >> 8;
                buf[1] = LineFeedNum & 0xff;

                for(j=0; j<LineLen; j++) {
                    if( PrnMode & 4 ) {             //����ѹ��
                        last = (~last) & prlinebuf[4-i][StartPos+j];
                        buf[6+StartPos1+j] = last;
                    } else {
                        buf[6+StartPos1+j*2] = prlinebuf[4-i][StartPos+j];
                        buf[6+StartPos1+j*2+1] = 0;
                    }
                }
                uiPrLineCount += LineFeedNum;
                ret = ioctl(prn_fd, PRN_DEAL_DATA, buf);
                if(ret)  Prn_errflag = ret ;

                if( ( i == 1 ) && ( CurrLineHigh & 1 ) )
                    LineFeedNum = 7;
                else
                    LineFeedNum = 15;
            }
        }

        if( PrintSwitch )
            ioctl(prn_fd, PRN_PRINT_DONE, 0);

        close(prn_fd);
    }

    StartPos = -1;
    LineLen = 0;
    lineoffset = 0;
    CurrLineHigh = 0;
    memset(prlinebuf,0,sizeof(prlinebuf));
    LineFeedNum += RowSpace;
}

void hip_PrnDlBufEnd(void)
{
    int prn_fd;
    int ret=0;

    unsigned char buf[366];

    if( !LineFeedNum || lineoffset )
        return;

    prn_fd = open(PRN_DEV_NAME, O_RDWR);
    if (prn_fd < 0) {
        printf("open dev file fail\n");
        return;
    }

    buf[0] = LineFeedNum >> 8;
    buf[1] = LineFeedNum & 0xff;
    buf[2] = 0;
    buf[3] = 0;
    buf[4] = 0;
    buf[5] = 0;

    uiPrLineCount += LineFeedNum;
    ret = ioctl(prn_fd, PRN_DEAL_DATA, buf);
    if(ret)  Prn_errflag = ret ;

    if( PrintSwitch )
        ioctl(prn_fd, PRN_PRINT_DONE, 0);

    close(prn_fd);

    LineFeedNum = 0;
}

void hip_PrnDlPage(void)
{
    int prn_fd;

    if( lineoffset )
        hip_PrnDlOneLine();

    if( uiPrLineCount % PageLen )
        LineFeedNum = (PageLen - (uiPrLineCount % PageLen)) % PageLen;
    else if( LineFeedNum )
        LineFeedNum = PageLen;

    hip_PrnDlBufEnd();

    prn_fd = open(PRN_DEV_NAME, O_RDWR);
    if (prn_fd < 0) {
        printf("open dev file fail\n");
        return;
    }

    if( PrintSwitch )
        ioctl(prn_fd, PRN_PRINT_DONE, 0);
    close(prn_fd);
}

int hip_print(char * pbuf)
{
    unsigned int buflen,offset;

    //ErrorCode=0;

    //�ж����ݳ���
    buflen=strlen(pbuf);
    if(buflen>PERPRINTMAXLEN)
        return -6;
    if(buflen==0)
        return 0;
    if(access("/etc/pubkey_test",F_OK)==0) {
        if(ndk_Is_test_version==1) {
            ndk_Is_test_version=0;
            ndk_HIP_PrnWarning();
        }
    }
    offset=0;
    while(1) {
        if (pbuf[offset]=='\r'||pbuf[offset]=='\n') {//�õ�һ������
            hip_PrnDlOneLine();
            offset++;
            continue;
        }
        if(lineoffset>=PRLINEWIDE)  {//�õ�һ������
            hip_PrnDlOneLine();
            continue;
        }
        if(offset>=buflen) {
            hip_PrnDlBufEnd();
            if (Prn_errflag)
                return Prn_errflag;
            return SUCC;
        }

        if (pbuf[offset]=='\f') {
            hip_PrnDlPage();
            offset++;
            continue;
        }

        if(pbuf[offset]&0x80) { //���ִ���
            if(StartPos == -1)
                StartPos = lineoffset;
            if(hip_sendHZtolinebuf(pbuf+offset)==1)// ����1��ʾһ������
                continue;
            LineLen = lineoffset - StartPos - g_nColumn;
            offset+=2;
        } else { //��ĸ����
            if (((unsigned char)pbuf[offset])<=0x7f&&((unsigned char)pbuf[offset])>=0x20) {//�Ƿ��ڵ����ķ�Χ��

                if(StartPos == -1) {
                    if((unsigned char)pbuf[offset]!=0x20)
                        StartPos = lineoffset;
                }
                if(hip_sendZMtolinebuf(pbuf[offset])==1)//����1��ʾһ������
                    continue;
                if((unsigned char)pbuf[offset]!=0x20)
                    LineLen = lineoffset - StartPos - g_nColumn;
            }
            offset++;
        }
    }
    if (Prn_errflag)
        return Prn_errflag;
    return 0;
}

int hip_PrSetPageTopMargin(unsigned int n)
{
    PageToMargin = n;

    return 0;
}

/*
������߽�
���ô�ӡƾ������߾࣬������Ϊ��λ
����:���ô�ӡƾ������߾࣬������Ϊ��λ��
����:nBorder ��߽����ظ��� ֵ��0��336 (10����)
����:

*/
void hip_setPrintLeftBorder(int nBorder)
{
    //if( nBorder>248 || nBorder<0 )
    //{
    //ErrorCode = PARAERR;
    //  return ;
    //}
    if( nBorder>288 || nBorder<0 ) {
        //ErrorCode = PARAERR;
        return ;
    }

    //ErrorCode=0;

    g_nBorder = nBorder;

    if(PrnMode & 4) {
        PRLINEWIDE = 360 - g_nBorder;
    } else {
        PRLINEWIDE = 180 - ( g_nBorder >> 1 );
    }

}

/*

���ô�ӡ�ּ�ࡢ�м��
���ô�ӡ�ּ�ࡢ�м�ࡣ�ڶԴ�ӡ����Ч���ú�һֱ��Ч��ֱ���´�����

����:���ô�ӡ�ּ�ࡢ�м�ࡣ�ڶԴ�ӡ����Ч���ú�һֱ��Ч��ֱ���´����á�
����:
nColumn �ּ��          Int ֵ��0-255(10����)
nRow    �м��          Int ֵ��0��255 (10����)
����:

*/
void hip_setPrintRange(int nColumn, int nRow)
{
    //if( nColumn>216 || nColumn<0 || nRow>216 || nRow<0)
    //{
    //ErrorCode = PARAERR;
    //  return ;
    //}
    if( nColumn>255 || nColumn<0 || nRow>255 || nRow<0) {
        //ErrorCode = PARAERR;
        return ;
    }

    //ErrorCode=0;

    g_nColumn_s = nColumn;
    if(PrnMode & 4)
        g_nColumn = g_nColumn_s;
    else
        g_nColumn = g_nColumn_s >> 1;

    RowSpace = nRow;
}

/*
�ô�ӡ����ֽ������Ϊ���ص�

����:�ô�ӡ����ֽ������Ϊ���ص�
����:
nPixel  ���ظ���  Int   ֵ��
����: FAIL ����ʧ��
         SUCC �ɹ�
*/
int hip_FeedPrinterByPixel (int nPixel)
{
    if( nPixel < -(792) || nPixel >(792) ) {
        //ErrorCode=PARAERR;
        return -1;
    }

    //ErrorCode=0;

    //if( PrnMode & 2 )         //����ѹ��
    hip_FeedPrinterStepByStep(nPixel);
    //else
    //hip_FeedPrinterStepByStep(nPixel<<1);

    return 0;
}

/******************************************************************
��������int hip_GetTotalMotorSteps(void)
���룺
�����int   --  ��������ܹ�������
����������
******************************************************************/
int hip_GetTotalMotorSteps(void)
{
    return uiPrLineCount;
}

int hip_resetprint(void)
{
    return 0;
}

int hip_printintegrate(int xsize,int ysize,int xpos,int ypos,char *ImgBuf,char *TextBuf, int mode)
{
    return 0;
}

#endif

#if 0
char PrnLog1[64][128];
int main(int argc ,char * argv[])
{
    int c;

    printf("print test\n");
    while(1) {
        c = getchar();
        //  if(argc<2)
        //      return;
        //  c = argv[1][0];
        if(c == '9') {
            memset(PrnLog1,0x55,sizeof(PrnLog1));
            hip_printimage(128,64,16,(char *)PrnLog1);
        }
        if(c == '1') {
            hip_setprintmode(0, 2);
            hip_printimage(128,64,16,(char *)NL_BMP);
            //  hip_printimage(128,60,16,(char *)NL_BMP);
            //  hip_printimage(128,56,16,(char *)NL_BMP);
            //  hip_printimage(128,52,16,(char *)NL_BMP);
            //  hip_printimage(128,48,16,(char *)NL_BMP);
            //  hip_FeedPrinterStepByStep(128);
        }
        if(c == '4') {
            hip_setprintmode(4, 2);
            hip_printimage(128,64,16,(char *)NL_BMP);
        }
        if(c == '5') {
            hip_setprintmode(2, 2);
            hip_printimage(128,64,16,(char *)NL_BMP);
        }
        if(c == '6') {
            hip_setprintmode(6, 2);
            hip_printimage(128,64,16,(char *)NL_BMP);
        }
        if(c == '7') {
            hip_printimage(128,64,16,(char *)CCB_BMP);
        }
        if(c == '8') {
            hip_setPrintLeftBorder(0);
            hip_setprintmode(0, 2);
            hip_print("1234567890\n");
            hip_setPrintLeftBorder(16);
            hip_setprintmode(2, 2);
            hip_print("1234567890\n");
            hip_print(" 1234567890\n");
            hip_setprintmode(4, 2);
            hip_print(" 1234567890\n");
            hip_setprintmode(6, 2);
            hip_print(" 1234567890\n");
            hip_setprintmode(0, 2);
            hip_print(" 1234����:�ô�ӡ����ֽ������Ϊ���ص�\n\n");
            hip_setprintmode(2, 2);
            hip_print(" 1234����:�ô�ӡ����ֽ������Ϊ���ص�\n");
            hip_setPrintLeftBorder(0);
            hip_setprintmode(4, 2);
            hip_print("1234����:�ô�ӡ����ֽ������Ϊ���ص�\n");
            hip_setprintmode(6, 2);
            hip_print("1234����:�ô�ӡ����ֽ������Ϊ���ص�\n");
        }

        if(c == '2') {
            hip_FeedPrinterStepByStep(64);
        }
        if(c == '3') {
            hip_FeedPrinterStepByStep(-64);
        }
        if(c == 'q')
            return 0;
    }
    return 0;
}
#endif
