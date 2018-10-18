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


extern unsigned char ndk_prnmode;    /**<��ӡģʽ0~7��Ĭ��Ϊ7*/
extern PrnDotlineData *ndk_gPrnDotlineData[200]; /**< �洢�е�����Ϣ��ָ�����飬ʹ��ʱʹ�ö�̬���䣬�������200��*/
extern unsigned int ndk_DotlineNum; /**<��ǰ�Ѿ��洢�������Լ�����Ҫ�洢�������λ��*/
extern int ndk_HZ_Xchange; /**<�����ֿⲻ�������õĻ�������ʱ��Ҫͨ���Ŵ�ı���*/
extern int ndk_HZ_Ychange;
extern int ndk_ZM_Xchange;
extern int ndk_ZM_Ychange;
extern int ndk_usr_hz_select;    /**<�Ƿ�ͨ��NDK_PrnSetUsrFont�������û��Զ���ĺ�������*/
extern int ndk_usr_zm_select;    /**<�Ƿ�ͨ��NDK_PrnSetUsrFont�������û��Զ����ASCII����*/
extern int ndk_HZUserFontId;    /**<ͨ��NDK_PrnSetUsrFont�������û��Զ���ĺ�������id*/
extern int ndk_ZMUserFontId;    /**<ͨ��NDK_PrnSetUsrFont�������û��Զ����ASSIC����id*/
extern ST_PRN_FONTMSG ndk_UsrMsg[MaxFontNum];
extern unsigned int ndk_CurrLineHigh;                                   /*һ�����ݵĸ߶�*/
extern unsigned int ndk_lineoffset;
extern unsigned int ndk_prn_mode;
extern unsigned int ndk_PR_MAXLINEWIDE;
extern unsigned int ndk_hz12mark;
extern unsigned int ndk_asc12mark;
extern unsigned int ndk_understart;                     /*�»��ߴӵڼ����㿪ʼ*/
extern unsigned int ndk_underend;                           /*�»��ߴӵڼ��������*/
extern unsigned int ndk_columnblank;                        /*�ּ��*/
extern unsigned int ndk_rowspace;                           /*�м��*/
extern unsigned int ndk_leftblank;                          /*��߾࣬ �Ե�Ϊ��λ*/
extern unsigned int ndk_LineState ;
extern unsigned int ndk_AlignType ;             /*����ģʽ��Ĭ��ȡ������ģʽ*/
extern unsigned int ndk_prngrey;
extern unsigned int ndk_PrnDirSwitch;       /*��ӡ���ͱߴ�Ŀ���*/
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
    NDK_OK=0,               /**<�����ɹ�*/
    NDK_ERR=-1,         /**<����ʧ��*/
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
 *@brief        ��ӡ����ʼ��
 *@details      �����建����,���ô�ӡ����(�������塢�߾��ģʽ��)��
 *@param        unPrnDirSwitch  �Ƿ������ͱߴ��ǵ���NDK_PrnStart��ʼ��ӡ���ء�
                        0--�رձ��ͱߴ���(Ĭ��)
                        �ڸ�ģʽ�£����е�NDK_PrnStr,NDK_PrnImage����ɵ���ת�����������������ݴ浽���ݻ�������
                        �ڵ���NDK_PrnStart֮��ſ�ʼ���кʹ�ӡ��صĹ�����������ֽ�ʹ�ӡ��
                        1--�������ͱߴ���
                        �ڸ�ģʽ�£�ֻҪ��һ�����ݣ��ͻ���������ӡ������NDK_PrnStart�����κ�Ч����ֱ�ӷ��ء�
                        ����NDK_PrnFeedByPixel����������ֽ���ء����ڹر�ģʽ�¸ò�������NDK_PrnStart֮����С�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
 *@li   NDK_ERR_OPEN_DEV        ��ӡ�豸��ʧ��
 *@li   NDK_ERR_IOCTL       �������Դ���(���ӡ������ʧ�ܡ���ӡ��������ʧ��)
 *@li   NDK_ERR_MACLLOC     �ڴ�ռ䲻��
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
    if(ndk_gPrnDotlineData[0] != NULL) { /*��ֹ�ɱ��ͱߴ�״̬�л��صȴ���ӡʱ��ָ��δ�ͷ����*/
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
 *@brief        ��ӡ�ַ���
 *@details  �ú�������ת�����д�ӡ���ַ��������󻺳�������ӡ�����ڵ���Start֮��ʼ������ӡ���ú���Ϊ�����������
 *@param    pszBuf Ϊ��\0Ϊ��β�Ĵ�,�������ݿ�ΪASC�룬���� ����\n��\r(��ʾ�������У����ڿ�����ֱ�ӽ�ֽ)��
            ��pszBuf�����к��ֺ�ASCII�Ļ�ϴ�ʱ,��ĸ�ͺ���ֻ�����һ�������йء�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ����ʧ��
 *@li   NDK_ERR_INIT_CONFIG     ��ʼ������ʧ��(��ӡδ��ʼ��)
 *@li   NDK_ERR_MACLLOC     �ڴ�ռ䲻��
 *@li       EM_PRN_STATUS      ��ӡ��״ֵ̬
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
    if (buflen == 0)/*���������ݴ洢����ֹ��'\r'��ͻ*/
        return NDK_OK;
    offset = 0;
    endprint_flag = 0;   /**<�ַ���������־*/

    do {
        do {
            if(*pszBuf == '\f') {   /**<���д�ӡ�����ɵ���NDK_PrnStart���ƣ������ӡ������־��ֱ���������ַ�*/
                pszBuf++;
                continue;
            }

            //�ж��л����Ƿ���
            if (*pszBuf == '\r' || *pszBuf == '\n') {   /*�õ�һ������*/
                if (ndk_CurrLineHigh < 1) {
                    ndk_CurrLineHigh = 1;
                }
                pszBuf++;
                offset++;
                break;
            }
            if (ndk_lineoffset >= ndk_PR_MAXLINEWIDE) { /*�õ�һ������*/
                break;
            }
            //������ȡ����,ʶ��'\0'
            if (*pszBuf == '\0') {
                if(*(pszBuf-1)=='\n'||*(pszBuf-1)=='\r')
                    endprint_flag=1;//˵��pbuf���Ի��з������ġ�ndk_DotlineNum�ڴ����з�ʱ�Ѿ�++�ˡ�ֱ�ӷ���
                else
                    endprint_flag=2;//˵��pbuf�����Ի��з������ġ������������NDK_PrnStrʱ�����ٺ����������,��Ҫ�洢�ð������壬�Ⱥ�������ݻ��з�
                break;
            }

            if (*pszBuf & 0x80) {   /*���ִ���*/
                if(ndk_usr_hz_select==0) { /*�Ƿ�ʹ���Զ���ĺ�������0--pos�Դ����� 1--ʹ���û����������*/
                    //fprintf(stderr,"HZ--\n");
                    if (ndk_sendHZtolinebuf(pszBuf) == 1)   /*����1��ʾһ������*/
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
            } else {    /*��ĸ����*/
                if(ndk_usr_zm_select==0) { /*�Ƿ�ʹ���Զ������ĸ���壬���Զ��庺��һ��*/
                    //fprintf(stderr,"ZM\n");
                    if ((*pszBuf) <= 0x7f &&(*pszBuf) >= 0x20) {    /*�Ƿ��ڵ����ķ�Χ��*/
                        if (ndk_sendZMtolinebuf(*pszBuf) == 1)  /*����1��ʾһ������*/
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
        } while (1);/*�õ�һ������*/
        if(endprint_flag) {
            if(endprint_flag==2) { /*��ֹ���һ�н���û��\n����������start��ʼ��ӡ��©���һ��*/
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineHigh =ndk_CurrLineHigh;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnLineOffset =ndk_lineoffset;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnSteps +=ndk_rowspace;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usmode=ndk_prnmode;
                ndk_gPrnDotlineData[ndk_DotlineNum]->usPrnGray = ndk_prngrey;
                if (ndk_LineState == OpenLine) {
                    ndk_underend =ndk_lineoffset-ndk_columnblank;
                    ndk_getunderline();/*������»��߹�������յ�һ�е����ݴ洢֮��Ե�����д���*/
                    ndk_understart= ndk_lineoffset;/* �»��߿�ʼλ�ôӸ��д洢ƫ��λ�ÿ�ʼ*/
                }
                if ((ndk_AlignType==RIGHTALIGN)||(ndk_AlignType==MIDALIGN))
                    ndk_aligndel();
                break;
            }
            //return NDK_OK;
            break;
        }

        if(ndk_CurrLineHigh==1) { /*��������NDK_PrnStr("\n"),ʱֱ�ӰѲ������ӵ���һ�еĲ��������棬�����ٿ�һ���ṹ��洢��ʡ�ռ�*/
            if(ndk_DotlineNum== 0) { /*�����ӡ��һ�о͵���NDK_PrnStr("\n")���������*/
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
             *��һ�е�����("\r"��"\n"���������ݳ���һ�е�����)����ÿ�����ݵ�״̬���Ա��棬��malloc��һ�еĴ洢�ռ������ʹ��
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
            ndk_understart = ndk_leftblank;/*һ���������������Ը��������ָ���ʼ״̬*/
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
 *@brief        ��ʼ������ӡ.
 *@details  NDK_PrnStr��NDK_PrnImage�����������ת���ɵ���洢���������й��������øú�����ʼ������ӡ��
                        ����NDK_PrnStart��ӡ������Ҫ�жϷ���ֵ�Ƿ�Ϊ0���������-1��˵�����ӡ����ʧ�ܣ����������ش�ӡ��״ֵ̬�������м�������������
                        NDK_PrnStart��ӡ����֮��������ȴ����ش�ӡ��״̬��ֵ��Ӧ�ÿɸ���NDK_PrnStart���ص�ֵ���жϴ�ӡ��״̬�Ƿ�������
                        (������صķǴ�ӡ��״ֵ̬����NDK_OK��������ϵͳ����ʱ��ҪӦ��ȥȡ��ӡ��״̬���ÿ����ԱȽ�С)
 *@return
 *@li   NDK_OK              ��ӡ������ȡ��ӡ��״̬����
 *@li   NDK_ERR_INIT_CONFIG     ��ʼ������ʧ��(��ӡδ��ʼ��)
 *@li   NDK_ERR_MACLLOC     �ڴ�ռ䲻��
 *@li   NDK_ERR_OPEN_DEV        ��ӡ�豸��ʧ��
 *@li       EM_PRN_STATUS      ��ӡ��״ֵ̬
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
      *����Ϊ�����Ĵ�ӡ������������ӡÿ��֮ǰҪ����ÿ�е����Ա�������ģʽ�ʹ�ӡ�Ҷȵ�
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
    while(1) {   /**< ��󷵻ش�ӡ����ʱ�Ĵ�ӡ״ֵ̬*/
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
 *@brief        ��ӡͼ��(�ú���Ҳ��ת����ӡ���󵽵��󻺳���������NDK_PrnStart��ʼ��ӡ)
 *@details      �����������384���㡣���xsize��xpos���֮�ʹ�������������ƻ᷵��ʧ�ܣ�����Ǻ���Ŵ�ģʽ�Ļ����ܳ���384/2��
 *@param        unXsize ͼ�εĿ�ȣ����أ�
 *@param        unYsize ͼ�εĸ߶ȣ����أ�
 *@param        unXpos  ͼ�ε����Ͻǵ���λ�ã��ұ�������xpos+xsize<=ndk_PR_MAXLINEWIDE������ģʽΪ384������Ŵ�ʱΪ384/2��
 *@param        psImgBuf ͼ���������,Ϊ�������У���һ���ֽڵ�һ�е�ǰ8���㣬D7Ϊ��һ����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA                ��������
 *@li   NDK_ERR_MACLLOC     �ڴ�ռ䲻��
 *@li   NDK_ERR_INIT_CONFIG     ��ʼ������ʧ��(��ӡδ��ʼ��)
 *@li   \ref EM_PRN_STATUS   "EM_PRN_STATUS"   ��ӡ��״ֵ̬
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

    if(ndk_PrnIsSetMode == 1)  //������ƫ���Լ��ּ��ʱ����Ѿ�����ģʽΪ����Ŵ�����ƫ�ƺ��ּ�඼����(Ϊ���ַŴ���ּ�����ƫ�Ʋ���ģʽ�����)
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
 *@brief        ���ڻ�ȡ����ӡ������
 *@retval   pemType ���ڷ��ش�ӡ�����͵�ֵ�������£�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_TP_PrnGetType(EM_PRN_TYPE *pemType)
{
    *pemType=PRN_TYPE_TP;
    return NDK_OK;
}


/**
 *@brief        ȡ��ӡ�����İ汾��Ϣ.
 *@retval   pszVer ���ڴ洢���ذ汾�ַ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
 *@li   NDK_ERR_OPEN_DEV        ��ӡ�豸��ʧ��
 *@li   NDK_ERR_IOCTL       ��������ʧ��(��ȡ��ӡ�汾ʧ��)
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
 *@brief        ���ô�ӡ����
 *@details  ����ASCII��ӡ����ͺ������塣Ӧ�ò�ɲο��ײ��Ӧ�ò�Ľӿ��ļ��е���ض��塣
 *@param        emHZFont ���ú��������ʽ��0���ֵ�ǰ���岻�䡣
 *@param    emZMFont����ASCII�����ʽ��0���ֵ�ǰ���岻�䡣
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
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
    * ����û�Ҫ���ú��֣���Ҫ�ײ�������֧��.
    */

    ret = ioctl(fd, PRN_IOCS_PARAM, p);

    if (ret < 0) {
        return NDK_ERR_IOCTL;
    }
#endif
    return NDK_OK;

}


/**
 *@brief        ��ȡ��ӡ��״ֵ̬.
 *@details      �ڴ�ӡ֮ǰ��ʹ�øú����жϴ�ӡ���Ƿ�ȱֽ��
 *@retval     pemStatus ���ڷ��ش�ӡ��״ֵ̬(������������漸����������Ĺ�ϵ)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
 *@li   NDK_ERR_OPEN_DEV        ��ӡ�豸��ʧ��
 *@li   NDK_ERR_IOCTL       ��������ʧ��(��ȡ��ӡ״̬ʧ��)
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
 *@brief��  ���ô�ӡģʽ.
 *@param        unMode ��ӡģʽ(Ĭ����ʹ������ģʽ)
 *@param    unSigOrDou ��ӡ��˫��ѡ��(ֻ�������Ч����������)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
*/

int NDK_TP_PrnSetMode(EM_PRN_MODE emMode,uint unSigOrDou)
{
    if ( (emMode <0) || (emMode > 3)) {
        return NDK_ERR_PARA;
    }
    if( (unSigOrDou != 0 ) && (unSigOrDou != 1 ))
        return NDK_ERR_PARA;

    if((ndk_PrnIsSetMode == 1 ) && (emMode >= 2)) { //��ԭ�ȵĺ���Ŵ�ģʽ����ԭ�ȵ�����ģʽ����ƫ���Լ��ּ��ָ���ԭ������
        ndk_leftblank = ndk_leftblank*2;
        ndk_columnblank = ndk_columnblank*2;
        ndk_PrnIsSetMode = 0;
    }

    if((ndk_PrnIsSetMode == 0 ) && (emMode < 2)) { //�ɺ�������ģʽ��������Ŵ�ģʽ����ƫ���Լ��ּ�����(Ϊ���ַŴ���ּ�����ƫ�Ʋ���ģʽ�����)
        ndk_leftblank = ndk_leftblank/2;
        ndk_columnblank = ndk_columnblank/2;
        ndk_PrnIsSetMode = 1;
    }

    ndk_prnmode=emMode*2+1;            //api���Ǳ���ԭ�ȵ�modeΪ0��7��״̬
    if(emMode < 2)
        ndk_PR_MAXLINEWIDE=384/2;
    else
        ndk_PR_MAXLINEWIDE=384;
    currentMode=emMode;
    return NDK_OK;

}

/**
 *@brief    ������ͼ�ε��Ű��ӡ.(��δʵ��)
 *@param    unXsize ͼ����(����)�����ܴ���384��������8��������
 *@param    unYsize ͼ��߶�(����)�����ܴ���384��������8��������
 *@param    unXpos  ͼ��ĺ���ƫ��λ�ã�(xpos+xsize)���ܴ���384
 *@param    unYpos  ͼ�������ƫ��λ��
 *@param    psImgBuf ͼ����󻺳壬����Ϊ�������еĵ�������
 *@param    pszTextBuf] �����ַ����׵�ַ���ַ������Ȳ��ܳ���1K
 *@param    unMode ��ӡ��ģʽ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_NOT_SUPPORT       ��֧�ָù���
*/
int NDK_TP_PrnIntegrate(uint unXsize,uint unYsize,uint unXpos,uint unYpos,const char *psImgBuf,const char *pszTextBuf, uint unMode)
{
    return NDK_ERR_NOT_SUPPORT;
}


/**
 *@brief        ���ô�ӡ�Ҷ�
 *@details      ���ô�ӡ�Ҷ�(����ʱ��)���Ա���ڲ�ͬ�Ĵ�ӡֽ���д�ӡЧ��΢��.
 *@param    unGrey �Ҷ�ֵ����Χ0~5��0Ϊ���Ч����5Ϊ��Ũ�Ĵ�ӡЧ������ӡ����Ĭ�ϵĻҶȼ���Ϊ3��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
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
 *@brief    ���ô�ӡ��߽硢�ּ�ࡢ�м�ࡣ�ڶԴ�ӡ����Ч���ú�һֱ��Ч��ֱ���´�
 *@param  unBorder ��߾� ֵ��Ϊ��0-288(Ĭ��Ϊ0)
 *@param    unColumn �ּ�� ֵ��Ϊ��0-255(Ĭ��Ϊ0)
 *@param    unRow �м�� ֵ��Ϊ��0-255(Ĭ��Ϊ0)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
 *@li   NDK_ERR_OPEN_DEV        ��ӡ�豸��ʧ��
 *@li   NDK_ERR_IOCTL       ��������ʧ��(���ࡢ�ּ�ࡢ�м������ʧ�ܡ�ͼ�ζ��뷽ʽ����ʧ��)
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


    if(ndk_PrnIsSetMode == 1) { //������ƫ���Լ��ּ��ʱ����Ѿ�����ģʽΪ����Ŵ�����ƫ�ƺ��ּ�඼����(Ϊ���ַŴ���ּ�����ƫ�Ʋ���ģʽ�����)
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
 *@brief    ��������ֽ
 *@details  �ô�ӡ����ֽ������Ϊ���ص�,���øú�������û��������ֽ�����Ǵ��ڽṹ���У��ȵ���start֮��ʹ�ӡ����һ��ִ��
 *@param  unPixel ��ֽ���ص� ֵ��> 0
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
 *@li   NDK_ERR_INIT_CONFIG     ��ʼ������ʧ��(��ӡδ��ʼ������)
 *@li   NDK_ERR_MACLLOC     �ڴ�ռ䲻��
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
 *@brief    ��ӡ�Ƿ����»��߹���.
 *@param  emStatus 0�����»��ߣ�1�����»���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
*/
int NDK_TP_PrnSetUnderLine(EM_PRN_UNDERLINE_STATUS emStatus)
{
    if (emStatus > CloseLine)
        return NDK_ERR_PARA;

    if (emStatus == OpenLine) {         /*�����»��߹���*/
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
 *@brief    ���ö��뷽ʽ.(��δʵ��)
 *@param  unType 0:�����; 1���ж���; 2�Ҷ���;3�رն��뷽ʽ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_NOT_SUPPORT       ��֧�ָù���
*/
int NDK_TP_PrnSetAlignment(uint unType)
{
    return NDK_ERR_NOT_SUPPORT;
}

/**
 *@brief    ��ȡ��ӡ����.
 *@retval pfLen �����Ѿ���ӡ�ĳ���(��ǰ��������ʼ��ӡ�����ڴ�ӡ����Ϊֹ����ӡ����ӡ���ܹ�����)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
 *@li   NDK_ERR     ����ʧ��(��ȡ�����ܹ���������ʧ��)
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

    //printlen_m = motorsteps/(16*1000);//��λΪ��
    //printlen_mm = (motorsteps - printlen_m*16*1000)/(16*100);
    //*punLen=motorsteps/16000;
    printlen_mm = motorsteps/16;
    *punLen = printlen_mm;
    return NDK_OK;

}

/**
 *@brief        �Զ�������ע�ᡣ
 *@param        pstMsg ST_PrintFontMsg����ָ�룬ʹ���Զ���ע��Ҫ�����Ӧ��Ϣ����д
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
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

    if ((fp = NDK_FsOpen(pstMsg->pszName, "r")) < 0)    /*�鿴���õ��ֿ��Ƿ����*/
        return NDK_ERR_PARA;

    NDK_FsClose(fp);
    memcpy(&ndk_UsrMsg[pstMsg->nNum], pstMsg, sizeof(ndk_UsrMsg[pstMsg->nNum]));

    NDK_LOG_INFO(NDK_LOG_MODULE_PRINT,"%s succ,current print font name is %s\n",pstMsg->pszName);
    return NDK_OK;
}

/**
 *@brief    ����ע�����ѡ���ӡ����.
 *@param  unFontId ע�������id(�����ú�Ḳ��NDK_PrnSetFont���趨������)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
*/
int NDK_TP_PrnSetUsrFont(uint unFontId)
{
    int fp;

    if ((unFontId>= MaxFontNum)||(unFontId < 0))
        return NDK_ERR_PARA;

    if ((fp = NDK_FsOpen(ndk_UsrMsg[unFontId].pszName, "r")) < 0)    /*�鿴���õ��ֿ��Ƿ����*/
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
 *@brief    ��øôδ�ӡ�ĵ�������.
 *@retval  punLine ��������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA                ��������
 *@li   NDK_ERR_INIT_CONFIG     ��ʼ������ʧ��(��ӡδ��ʼ������)
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
 *@brief    ��ӡbmp��png�ȸ�ʽ��ͼƬ
 *@detail  �ú������洢��pos�ϵ�ͼƬ���н����洢�����󻺳�����  ����ͼƬ���������һ����ʱ�䣬��Ҫ��ʱ����Ҫ��һ���ĵȴ�ʱ��
 *@param  pszPath ͼƬ���ڵ�·��
 *@param  unXpos  ͼ�ε����Ͻǵ���λ�ã��ұ�������xpos+xsize(�����ͼƬ�Ŀ��ֵ)<=ndk_PR_MAXLINEWIDE������ģʽΪ384������Ŵ�ʱΪ384/2��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA                ��������
 *@li   NDK_ERR_INIT_CONFIG     ��ʼ������ʧ��(��ӡδ��ʼ������)
 *@li   NDK_ERR_DECODE_IMAGE                ͼ�����ʧ��
 *@li   NDK_ERR_MACLLOC                 �ڴ�ռ䲻��
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
 *@brief    ����BDF����
 *@detail  ʹ�øú�������BDF���嵽�ڴ��У��Ƚϴ�������ķ�һЩʱ�䡣
 *@param  pszPath BDF���ڵ�·��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
 *@li   NDK_ERR_PARA        ��������
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
 *@brief    ��ӡBDF�����е�����
 *@param  pusText unsigned short ���͵����ݡ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
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
 *@brief    ����BDF��������
 *@param  unXpos  ��ƫ�� ֵ��Ϊ��0-288(Ĭ��Ϊ0)
 *@param  unLineSpace  �м�� ֵ��Ϊ��0-255(Ĭ��Ϊ0)
 *@param  unWordSpace  �ּ�� ֵ��Ϊ��0-255(Ĭ��Ϊ0)
 *@param  unXmode  ����Ŵ���(ע�⣬�����MaxWidth*unXmode���벻�ܳ���384������ʧ��)
 *@param  unYmode  ����Ŵ���(ע�⣬�����MaxHeight*unYmode���벻�ܳ���48������ʧ��)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
 *@li   NDK_ERR       ����ʧ��(��ʼ������ͷ�ڵ㣬ʹ��calloc��֤������0ʧ��)
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
 *@brief  ����������ӡ���ڴ�ӡ��ҳǰ���ӡ��ɺ�Ľ���ֽ����
 *@param        emType
PRN_FEEDPAPER_BEFORE  ��       ��ҳ��ӡǰ��ֽ
PRN_FEEDPAPER_AFTER   ��        ��ҳ��ӡ��ɺ��ֽ

 *@return
 *@li           NDK_OK                          �����ɹ�
 *@li           NDK_ERR_PARA                          ��������
 *@li           NDK_ERR_INIT_CONFIG     ��ʼ������ʧ��(��ӡδ��ʼ������)
 *@li           NDK_ERR_OPEN_DEV     ��ӡ�豸��ʧ��
 *@li           NDK_ERR_IOCTL     ��������ʧ��(��ӡ����ֽ��˺ֽ����������ʧ�ܡ���ֽ��������ʧ��)
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




