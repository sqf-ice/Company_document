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


/** @addtogroup ��ӡ
* @{
*/

/**
 *@brief          ��ӡ����ʼ��
 *@details      �����建����,���ô�ӡ����(�������塢�߾��ģʽ��)��
 *@param      unPrnDirSwitch  �Ƿ������ͱߴ��ǵ���NDK_PrnStart��ʼ��ӡ���ء�
              0--�رձ��ͱߴ���(Ĭ��)
                  �ڸ�ģʽ�£����е�NDK_PrnStr,NDK_PrnImage����ɵ���ת�����������������ݴ浽���ݻ�������
                  �ڵ���NDK_PrnStart֮��ſ�ʼ���кʹ�ӡ��صĹ�����������ֽ�ʹ�ӡ��
              1--�������ͱߴ���
                  �ڸ�ģʽ�£�ֻҪ��һ�����ݣ��ͻ���������ӡ������NDK_PrnStart�����κ�Ч����ֱ�ӷ��ء�
                  ����NDK_PrnFeedByPixel����������ֽ���ء����ڹر�ģʽ�¸ò�������NDK_PrnStart֮����С�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
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
 *@brief        ��ӡ�ַ���
 *@details      �ú�������ת�����д�ӡ���ַ��������󻺳�������ӡ�����ڵ���Start֮��ʼ������ӡ���ú���Ϊ�����������
 *@param        pszBuf Ϊ��\0Ϊ��β�Ĵ�,�������ݿ�ΪASC�룬���� ����\n��\r(��ʾ�������У����ڿ�����ֱ�ӽ�ֽ)��
                ��pszBuf�����к��ֺ�ASCII�Ļ�ϴ�ʱ,��ĸ�ͺ���ֻ�����һ�������йء�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
*/
int NDK_HIP_PrnStr(const char *pszBuf)
{
    return hip_print((char *) pszBuf);
}
/**
 *@brief        ��ʼ������ӡ.
 *@details  NDK_PrnStr��NDK_PrnImage�����������ת���ɵ���洢���������й��������øú�����ʼ������ӡ��
                        ����NDK_PrnStart��ӡ������Ҫ�жϷ���ֵ�Ƿ�Ϊ0���������-1��˵�����ӡ����ʧ�ܣ����������أ������м�������������
                        ����ֵ����ʱ����Ҫ����ȡ״̬���жϴ�ӡ���Ƿ�����
 *@return
 *@li   NDK_OK                �����ɹ�
 *@li   NDK_ERRCODE         ����ʧ��(��ӡ����δ�������ͷŻ�������ռ��ʱ�����)
 *@li EM_PRN_STATUS   ����Ǵ�ӡ���̳������س���ʱ��ӡ����״̬
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
 *@brief        ��ӡͼ��(�ú���Ҳ��ת����ӡ���󵽵��󻺳���������NDK_PrnStart��ʼ��ӡ)
 *@details      �����������384���㡣���xsize��xpos���֮�ʹ�������������ƻ᷵��ʧ�ܣ�����Ǻ���Ŵ�ģʽ�Ļ����ܳ���384/2��
 *@param        unXsize ͼ�εĿ�ȣ����أ�
 *@param        unYsize ͼ�εĸ߶ȣ����أ�
 *@param        unXpos  ͼ�ε����Ͻǵ���λ�ã��ұ�������xpos+xsize<=ndk_PR_MAXLINEWIDE������ģʽΪ384������Ŵ�ʱΪ384/2��
 *@param        psImgBuf ͼ���������,Ϊ�������У���һ���ֽڵ�һ�е�ǰ8���㣬D7Ϊ��һ����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
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
 *@brief        ���ڻ�ȡ����ӡ������
 *@retval   pemType ���ڷ��ش�ӡ�����͵�ֵ�������£�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
*/
int NDK_HIP_PrnGetType(EM_PRN_TYPE *pemType)
{
    if(pemType == NULL)
        return NDK_ERR_PARA;
    *pemType = PRN_TYPE_HIP;

    return NDK_OK;
}
/**
 *@brief        ȡ��ӡ�����İ汾��Ϣ.
 *@retval   pszVer ���ڴ洢���ذ汾�ַ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
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
 *@brief        ���ô�ӡ����
 *@details  ����ASCII��ӡ����ͺ������塣Ӧ�ò�ɲο��ײ��Ӧ�ò�Ľӿ��ļ��е���ض��塣
 *@param        emHZFont ���ú��������ʽ
 *@param    emZMFont����ASCII�����ʽ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
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
 *@brief          ��ȡ��ӡ��״ֵ̬.
 *@details      �ڴ�ӡ֮ǰ��ʹ�øú����жϴ�ӡ���Ƿ�ȱֽ��
 *@retval       pemStatus ���ڷ��ش�ӡ��״ֵ̬(����������������߶��ֵ���Ĺ�ϵ)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_HIP_PrnGetStatus(EM_PRN_STATUS *pemStatus)
{
    *pemStatus = hip_getprinterstatus();
    return NDK_OK;
}
/**
 *@brief��  ���ô�ӡģʽ.
 *@param        unMode ��ӡģʽ(Ĭ����ʹ������ģʽ)
 *@param    unSigOrDou ��ӡ��˫��ѡ��0--���� 1--˫��(ֻ�������Ч����������)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        ��������
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
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_HIP_PrnIntegrate(uint unXsize,uint unYsize,uint unXpos,uint unYpos,const char *psImgBuf,const char *pszTextBuf, uint unMode)
{
    return -1;
}
/**
 *@brief        ���ô�ӡ�Ҷ�
 *@details      ���ô�ӡ�Ҷ�(����ʱ��)���Ա���ڲ�ͬ�Ĵ�ӡֽ���д�ӡЧ��΢��.
 *@param    unGrey �Ҷ�ֵ����Χ0~5��0Ϊ���Ч����5Ϊ��Ũ�Ĵ�ӡЧ������ӡ����Ĭ�ϵĻҶȼ���Ϊ3��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_HIP_PrnSetGreyScale(uint unGrey)
{
    return NDK_OK;
}
/**
 *@brief    ���ô�ӡ��߽硢�ּ�ࡢ�м�ࡣ�ڶԴ�ӡ����Ч���ú�һֱ��Ч��ֱ���´�
 *@param  unBorder ��߾� ֵ��Ϊ��0-288(Ĭ��Ϊ0)
 *@param    unColumn �ּ�� ֵ��Ϊ��0-255(Ĭ��Ϊ0)
 *@param    unRow �м�� ֵ��Ϊ��0-255(Ĭ��Ϊ0)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
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
 *@brief      ��������ֽ
 *@details  �ô�ӡ����ֽ������Ϊ���ص�,���øú�������û��������ֽ�����Ǵ��ڻ������У��ȵ���start֮��ʹ�ӡ����һ��ִ��
 *@param    unPixel ��ֽ���ص� ֵ��> 0
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_HIP_PrnFeedByPixel(uint unPixel)
{
    if( hip_FeedPrinterByPixel(unPixel) == 0 )
        return NDK_OK;

    return NDK_ERR_PARA;
}

/**
 *@brief    ��ӡ�Ƿ����»��߹���.
 *@param  emStatus 0�����»��ߣ�1�����»���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_HIP_PrnSetUnderLine(EM_PRN_UNDERLINE_STATUS emStatus)
{
    if( (emStatus != 0 ) && (emStatus != 1 ))
        return NDK_ERR_PARA;
    hip_SetUnderLine(emStatus);
    return NDK_OK;
}

/**
 *@brief    ���ö��뷽ʽ.(��δʵ��)
 *@param  unType 0:�����; 1���ж���; 2�Ҷ���;3�رն��뷽ʽ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_HIP_PrnSetAlignment(uint unType)
{
    return NDK_OK;
}
/**
 *@brief    ��ȡ��ӡ����.
 *@retval punLen �����Ѿ���ӡ�ĳ���(��ǰ��������ʼ��ӡ�����ڴ�ӡ����Ϊֹ����ӡ����ӡ���ܹ�����)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
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

    //printlen_m = motorsteps/(16*1000);//��λΪ��
    //printlen_mm = (motorsteps - printlen_m*16*1000)/(16*100);
    //*punLen=motorsteps/16000;
    printlen_mm = motorsteps/(24.0)*4.235;
    *punLen = printlen_mm;
    return NDK_OK;
}
/**
 *@brief        �Զ�������ע�ᡣ
 *@param        pstMsg ST_PRN_FONTMSG����ָ�룬ʹ���Զ���ע��Ҫ�����Ӧ��Ϣ����д
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_HIP_PrnFontRegister(ST_PRN_FONTMSG *pstMsg)
{
    return NDK_OK;
}
/**
 *@brief    ����ע�����ѡ���ӡ����.
 *@param  unFontId ע�������id(�����ú�Ḳ��NDK_PrnSetFont���趨������)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_HIP_PrnSetUsrFont(uint unFontId)
{
    return NDK_OK;
}
/**
 *@brief    ��øôδ�ӡ�ĵ�������.
 *@retval  punLine ��������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
extern int PrintSwitch;//0�رգ�1��
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
 *@brief    ��ӡbmp��png�ȸ�ʽ��ͼƬ
 *@detail  �ú������洢��pos�ϵ�ͼƬ���н����洢�����󻺳�����  ����ͼƬ���������һ����ʱ�䣬��Ҫ��ʱ����Ҫ��һ���ĵȴ�ʱ��
 *@param  pszPath ͼƬ���ڵ�·��
 *@param  unXpos  ͼ�ε����Ͻǵ���λ�ã��ұ�������xpos+xsize(�����ͼƬ�Ŀ��ֵ)<=ndk_PR_MAXLINEWIDE������ģʽΪ384������Ŵ�ʱΪ384/2��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
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
 *@brief    ����BDF����
 *@detail  ʹ�øú�������BDF���嵽�ڴ��У��Ƚϴ�������ķ�һЩʱ�䡣
 *@param  pszPath BDF���ڵ�·��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/

int NDK_HIP_PrnLoadBDFFont(const char *pszPath)
{
    return NDK_ERR_NOT_SUPPORT;
}


/**
 *@brief    ��ӡBDF�����е�����
 *@param  pusText unsigned short ���͵����ݡ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/

int NDK_HIP_PrnBDFStr(ushort *pusText)
{
    return NDK_ERR_NOT_SUPPORT;
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
 *@li   ����EM_NDK_ERR      ����ʧ��
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
    ndk_SetFontMsg((PRN_HZ_FONT_16x16 << 8)|PRN_ZM_FONT_8x16);  /*����Ĭ������*/
    NDK_PrnSetMode(3,0);             /*ģʽ�ָ�Ϊ����ģʽ*/
    ndk_columnblank= 0;                 /*�ּ��*/
    ndk_rowspace= 0;                    /*�м��*/
    ndk_leftblank= 0;                       /*��߾࣬ �Ե�Ϊ��λ*/
    ndk_LineState = UnableLine;             /*�»������ñ�ʶ��Ĭ��Ϊ��*/
    ndk_understart = 0;                 /*�»��ߴӵڼ����㿪ʼ*/
    ndk_underend = 0;                   /*�»��ߴӵڼ��������*/
    ndk_AlignType = UNABLEALIGN;
    ndk_lineoffset= ndk_leftblank;     /*ÿһ�е���ʼλ��Ϊ��ƫ�Ʊ���һ��*/
#endif
}


/**
 *@brief  ����������ӡ���ڴ�ӡ��ҳǰ���ӡ��ɺ�Ľ���ֽ����
 *@param        emType
PRN_FEEDPAPER_BEFORE  ��       ��ҳ��ӡǰ��ֽ
PRN_FEEDPAPER_AFTER   ��        ��ҳ��ӡ��ɺ��ֽ

 *@return
 *@li NDK_OK �����ɹ�
 *@li ����EM_NDK_ERR ����ʧ��
*/
NEXPORT int NDK_HIP_PrnFeedPaper(EM_PRN_FEEDPAPER emType)
{
    return NDK_ERR_NOT_SUPPORT;
}


/** @} */ // ��ӡģ�����

/* End of this file */
