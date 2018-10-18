#if 1
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

#include "NDK.h"
#include "NDK_debug.h"
#include "NDK_Print.h"

static int prn_driver_init(void)
{
    int ret;
    char buf[20];

    memset(buf,0x00,sizeof(buf));
    if(p_ndk_PrnDriver==NULL) {
        if ((ret=NDK_SysGetPosInfo(1,NULL,buf))<0) {
            NDK_LOG_DEBUG(NDK_LOG_MODULE_PRINT, "call NDK_SysGetPosInfo err\r\n");
            return ret;
        }

        fprintf(stderr, "print type = %02x\r\n", buf[SYS_HWTYPE_PRINTER]);
        if(buf[SYS_HWTYPE_PRINTER]==0x0) {
            p_ndk_PrnDriver =(ndk_prn_driver *) &ndk_NO_driver;
        } else if (buf[SYS_HWTYPE_PRINTER]&0x80) { //���
            p_ndk_PrnDriver = (ndk_prn_driver *)&ndk_HIP_driver;
        } else {
            p_ndk_PrnDriver =(ndk_prn_driver *)&ndk_TP_driver;
        }
    }

    return 0;
}

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
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnInit(uint unPrnDirSwitch)
{
    EM_PRN_TYPE tmptype;
    NDK_LOG_INFO (NDK_LOG_MODULE_PRINT,"call %s \n", __FUNCTION__);
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }
    NDK_PrnGetType(&tmptype);
    NDK_LOG_INFO(NDK_LOG_MODULE_PRINT,"%s succ,current print type is %d\n",__func__,tmptype);
    return p_ndk_PrnDriver->p_ndk_prnInit(unPrnDirSwitch);

}
/**
 *@brief        ��ȡ��ӡ��ͨ������ֵ(�ײ���δ֧��)
 *@details  ��ӡ��ͨ������ֵ��ȡ��
 *@param    emChanelNum     ADͨ����(�ο�\ref EM_PRN_CHANELNUM "EM_PRN_CHANELNUM")
 *@retval   punAdValue      AD����ֵ(��ѹ��ֵ��VΪ��λ���¶���0.1CΪ��λ)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����\ref EM_NDK_ERR "EM_NDK_ERR"        ����ʧ��
*/
NEXPORT int NDK_PrnGetAd(EM_PRN_CHANELNUM emChanelNum,uint *punAdValue)
{
    return -1;
}
/**
 *@brief        ��ӡ�ַ���
 *@details  �ú�������ת�����д�ӡ���ַ��������󻺳�������ӡ�����ڵ���Start֮��ʼ������ӡ���ú���Ϊ�����������
 *@param    pszBuf Ϊ��\0Ϊ��β�Ĵ�,�������ݿ�ΪASC�룬���� ����\n��\r(��ʾ�������У����ڿ�����ֱ�ӽ�ֽ)��
            ��pszBuf�����к��ֺ�ASCII�Ļ�ϴ�ʱ,��ĸ�ͺ���ֻ�����һ�������йء�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnStr(const char *pszBuf)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnStr(pszBuf);

}

/**
 *@brief        ��ʼ������ӡ.
 *@details  NDK_PrnStr��NDK_PrnImage�����������ת���ɵ���洢���������й��������øú�����ʼ������ӡ��
                        ����NDK_PrnStart��ӡ������Ҫ�жϷ���ֵ�Ƿ�Ϊ0���������-1��˵�����ӡ����ʧ�ܣ����������ش�ӡ��״ֵ̬�������м�������������
                        NDK_PrnStart��ӡ����֮��������ȴ����ش�ӡ��״̬��ֵ��Ӧ�ÿɸ���NDK_PrnStart���ص�ֵ���жϴ�ӡ��״̬�Ƿ�������
                        (������صķǴ�ӡ��״ֵ̬����NDK_OK��������ϵͳ����ʱ��ҪӦ��ȥȡ��ӡ��״̬���ÿ����ԱȽ�С)
 *@return
 *@li   NDK_OK              ��ӡ������ȡ��ӡ��״̬����
 *@li   EM_NDK_ERR      ����ϵͳ����(��NDK_ERR_OPEN_DEV����NDK_ERR_MACLLOC)
 *@li EM_PRN_STATUS      ��ӡ��״ֵ̬
*/
NEXPORT int NDK_PrnStart(void)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnStart();
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
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnImage(uint unXsize,uint unYsize,uint unXpos,const char *psImgBuf)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnImage(unXsize,unYsize,unXpos,psImgBuf);

}

/**
 *@brief        ���ڻ�ȡ����ӡ������
 *@retval   pemType ���ڷ��ش�ӡ�����͵�ֵ�������£�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnGetType(EM_PRN_TYPE *pemType)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnGetType(pemType);
}


/**
 *@brief        ȡ��ӡ�����İ汾��Ϣ.
 *@retval   pszVer ���ڴ洢���ذ汾�ַ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnGetVersion(char *pszVer)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnGetVersion(pszVer);
}

/**
 *@brief        ���ô�ӡ����
 *@details  ����ASCII��ӡ����ͺ������塣Ӧ�ò�ɲο��ײ��Ӧ�ò�Ľӿ��ļ��е���ض��塣
 *@param        emHZFont ���ú��������ʽ
 *@param    emZMFont����ASCII�����ʽ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/

NEXPORT int NDK_PrnSetFont(EM_PRN_HZ_FONT emHZFont,EM_PRN_ZM_FONT emZMFont)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnSetFont(emHZFont,emZMFont);

}


/**
 *@brief        ��ȡ��ӡ��״ֵ̬.
 *@details      �ڴ�ӡ֮ǰ��ʹ�øú����жϴ�ӡ���Ƿ�ȱֽ��
 *@retval     pemStatus ���ڷ��ش�ӡ��״ֵ̬(������������漸����������Ĺ�ϵ)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnGetStatus(EM_PRN_STATUS *pemStatus)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnGetStatus(pemStatus);
}

/**
 *@brief��  ���ô�ӡģʽ.
 *@param        unMode ��ӡģʽ(Ĭ����ʹ������ģʽ)
 *@param    unSigOrDou ��ӡ��˫��ѡ��(ֻ�������Ч����������)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/

NEXPORT int NDK_PrnSetMode(EM_PRN_MODE emMode,uint unSigOrDou)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnSetMode(emMode,unSigOrDou);

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
NEXPORT int NDK_PrnIntegrate(uint unXsize,uint unYsize,uint unXpos,uint unYpos,const char *psImgBuf,const char *pszTextBuf, uint unMode)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnIntegrate(unXsize,unYsize,unXpos,unYpos,psImgBuf,pszTextBuf,unMode);
}


/**
 *@brief        ���ô�ӡ�Ҷ�
 *@details      ���ô�ӡ�Ҷ�(����ʱ��)���Ա���ڲ�ͬ�Ĵ�ӡֽ���д�ӡЧ��΢��.
 *@param    unGrey �Ҷ�ֵ����Χ0~5��0Ϊ���Ч����5Ϊ��Ũ�Ĵ�ӡЧ������ӡ����Ĭ�ϵĻҶȼ���Ϊ3��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnSetGreyScale(uint unGrey)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    ret=p_ndk_PrnDriver->p_ndk_prnSetGreyScale(unGrey);
    NDK_LOG_INFO(NDK_LOG_MODULE_PRINT,"%s succ,current print grayscale is %d\n",__func__,unGrey);
    return ret;

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
NEXPORT int NDK_PrnSetForm(uint unBorder,uint unColumn, uint unRow)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnSetForm(unBorder,unColumn,unRow);
}

/**
 *@brief    ��������ֽ
 *@details  �ô�ӡ����ֽ������Ϊ���ص�,���øú�������û��������ֽ�����Ǵ��ڽṹ���У��ȵ���start֮��ʹ�ӡ����һ��ִ��
 *@param  unPixel ��ֽ���ص� ֵ��> 0
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnFeedByPixel(uint unPixel)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnFeedByPixel(unPixel);
}

/**
 *@brief    ��ӡ�Ƿ����»��߹���.
 *@param  emStatus 0�����»��ߣ�1�����»���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnSetUnderLine(EM_PRN_UNDERLINE_STATUS emStatus)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnSetUnderLine(emStatus);
}

/**
 *@brief    ���ö��뷽ʽ.(��δʵ��)
 *@param  unType 0:�����; 1���ж���; 2�Ҷ���;3�رն��뷽ʽ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnSetAlignment(uint unType)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnSetAlignment(unType);
}

/**
 *@brief    ��ȡ��ӡ����.
 *@retval pfLen �����Ѿ���ӡ�ĳ���(��ǰ��������ʼ��ӡ�����ڴ�ӡ����Ϊֹ����ӡ����ӡ���ܹ�����)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnGetPrintedLen(uint *punLen)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnGetPrintedLen(punLen);
}

/**
 *@brief        �Զ�������ע�ᡣ
 *@param        pstMsg ST_PrintFontMsg����ָ�룬ʹ���Զ���ע��Ҫ�����Ӧ��Ϣ����д
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnFontRegister(ST_PRN_FONTMSG *pstMsg)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnFontRegister(pstMsg);
}

/**
 *@brief    ����ע�����ѡ���ӡ����.
 *@param  unFontId ע�������id(�����ú�Ḳ��NDK_PrnSetFont���趨������)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnSetUsrFont(uint unFontId)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnSetUsrFont(unFontId);
}

/**
 *@brief    ��øôδ�ӡ�ĵ�������.
 *@retval  punLine ��������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnGetDotLine(uint *punLine)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnGetDotLine(punLine);
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
NEXPORT int NDK_PrnPicture(uint unXpos,const char *pszPath)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    return p_ndk_PrnDriver->p_ndk_prnPicture(unXpos,pszPath);
}

/**
 *@brief    ���ô�ӡҳ��
 *@detail   �Դ�ӡ����ӡҳ���������ã���apiֻ�������Ч
 *@param  len   ҳ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
NEXPORT int NDK_PrnSetPageLen(uint len)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    if(p_ndk_PrnDriver->p_ndk_prnSetPageLen)
        return p_ndk_PrnDriver->p_ndk_prnSetPageLen(len);

    return  NDK_OK;
}

/**
 *@brief    ����BDF����
 *@detail  ʹ�øú�������BDF���嵽�ڴ��У��Ƚϴ�������ķ�һЩʱ�䡣
 *@param  pszPath BDF���ڵ�·��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/

int NDK_PrnLoadBDFFont(const char *pszPath)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    if(p_ndk_PrnDriver->p_ndk_prnLoadBDFFont)
        return p_ndk_PrnDriver->p_ndk_prnLoadBDFFont(pszPath);

    return  NDK_OK;
}

/**
 *@brief    ��ӡBDF�����е�����
 *@param  pusText unsigned short ���͵����ݡ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/

int NDK_PrnBDFStr(ushort *pusText)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    if(p_ndk_PrnDriver->p_ndk_prnBDFStr)
        return p_ndk_PrnDriver->p_ndk_prnBDFStr(pusText);

    return  NDK_OK;

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

int NDK_PrnSetBDF(uint unXpos,uint unLineSpace,uint unWordSpace,uint unXmode,uint unYmode)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    if(p_ndk_PrnDriver->p_ndk_prnSetBDF)
        return p_ndk_PrnDriver->p_ndk_prnSetBDF(unXpos,unLineSpace,unWordSpace,unXmode,unYmode);

    return  NDK_OK;

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
NEXPORT int NDK_PrnFeedPaper(EM_PRN_FEEDPAPER emType)
{
    int ret;

    if ((ret=prn_driver_init())<0) {
        return ret;
    }

    if(p_ndk_PrnDriver->p_ndk_prnFeedPaper)
        return p_ndk_PrnDriver->p_ndk_prnFeedPaper(emType);

    return  NDK_OK;

}

/**
 *@brief    ���������ַ�����ȡ��ϵͳָ����ӡ������Ӧ�ĵ��󻺳�
 *@param    emGetFont �ֿ����ߴ��С,Ŀǰ��֧��16x16����(����16x16,�ַ�8x16)
 *@param    pszBuf �ַ���
 *@retval   psOutbuf   ���ص������ݺ������У�����ͼ��ʾ:\n
-----------------D7~~D0--------------D7~~D0------------------\n
Byte 1: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte2 \n
Byte 3: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte4 \n
Byte 5: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte6 \n
Byte 7: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte8 \n
Byte 9: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte10    \n
Byte11: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte12    \n
Byte13: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte14    \n
Byte15: �� �� �� �� �� �� �� ��  ��  �� �� �� �� �� �� �� Byte16    \n
---------------------------------------------------------------\n
 *@param    unBuf_w psOutbuf �������ؿ�,Ҫ���Ȳ�С��pszBuf�ַ�������*�ַ�������(��8)
 *@param    unBuf_h psOutbuf �������ظ�,Ҫ��С���ַ�����߶�

 *@return
 *@li       NDK_OK    �����ɹ�
 *@li       \ref NDK_ERR_PARA     "NDK_ERR_PARA"        ��������
 *@li       \ref NDK_ERR_NOT_SUPPORT  "NDK_ERR_NOT_SUPPORT"  δ֪���ֿ�ߴ�
*/
NEXPORT int NDK_PrnGetDotData(EM_PRN_GETFONT emGetFont,const char *pszBuf,char *psOutbuf,uint unBuf_w,uint unBuf_h)
{
	char *pstr = pszBuf;
	int offset = 0,i,k=0;
	int fd = -1;
	char dotfile[PATH_MAX];

	if(pszBuf==NULL||psOutbuf==NULL)
	{
		return NDK_ERR_PARA;
	}
	if((emGetFont>=PRN_GETFONT_MAX)||(emGetFont<PRN_GETFONT_16X16))
	{
		return NDK_ERR_NOT_SUPPORT;
	}
    if ((unBuf_w<(8*strlen(pszBuf)))||(unBuf_h<16)) {
        return NDK_ERR_PARA;
    }

	switch(emGetFont)
	{
	case PRN_GETFONT_16X16:
		strcpy(dotfile,"/guiapp/share/fonts/font_h.bin");
		break;

	default:
		return NDK_ERR_NOT_SUPPORT;
		break;
	}

	while(*pstr)
	{
		if(*pstr&0x80)
		{
			offset=((*pstr-0x81)*191+*(pstr+1)-0x40)*32;//GB18030
			if(fd<0)
			{
				fd = open(dotfile, O_RDONLY);
				if (fd < 0) {
					return NDK_ERR;
				}
			}
			lseek(fd, offset, 0);
			for(i=0;i<16;i++)
			{
				read(fd,psOutbuf+k+i*((unBuf_w+7)/8),2);
			}
			k+=2;
			pstr+=2;
		}
		else
		{
			offset = ((*pstr-0x20)*16)+0xbde40;
			if(fd<0)
			{
				fd = open(dotfile, O_RDONLY);
				if (fd < 0) {
					return NDK_ERR;
				}
			}
			lseek(fd, offset, 0);
			for(i=0;i<16;i++)
			{
				read(fd,psOutbuf+k+i*((unBuf_w+7)/8),1);
			}
			k++;
			pstr++;
		}
	}
	if(fd>=0)
	{
		close(fd);
	}
	return NDK_OK;

}


/* End of this file */
#endif
