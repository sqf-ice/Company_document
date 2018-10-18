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

#include "NDK_TPPrint.h"
#include "NDK_TPPrnDrv.h"
#include "NDK_TPPrnFont.h"
#include "NDK.h"
#include "NDK_debug.h"

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
int NDK_NO_PrnInit(uint unPrnDirSwitch)
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
int NDK_NO_PrnStr(const char *pszBuf)
{
    return -1;
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
int NDK_NO_PrnStart(void)
{
    return -1;
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
int NDK_NO_PrnImage(uint unXsize,uint unYsize,uint unXpos,const char *psImgBuf)
{
    return -1;
}

/**
 *@brief        ���ڻ�ȡ����ӡ������
 *@retval   pemType ���ڷ��ش�ӡ�����͵�ֵ�������£�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_NO_PrnGetType(EM_PRN_TYPE *pemType)
{
    return -1;
}


/**
 *@brief        ȡ��ӡ�����İ汾��Ϣ.
 *@retval   pszVer ���ڴ洢���ذ汾�ַ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_NO_PrnGetVersion(char *pszVer)
{
    return -1;
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

int NDK_NO_PrnSetFont(EM_PRN_HZ_FONT emHZFont,EM_PRN_ZM_FONT emZMFont)
{
    return -1;
}


/**
 *@brief        ��ȡ��ӡ��״ֵ̬.
 *@details      �ڴ�ӡ֮ǰ��ʹ�øú����жϴ�ӡ���Ƿ�ȱֽ��
 *@retval     pemStatus ���ڷ��ش�ӡ��״ֵ̬(������������漸����������Ĺ�ϵ)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_NO_PrnGetStatus(EM_PRN_STATUS *pemStatus)
{
    return -1;
}

/**
 *@brief��  ���ô�ӡģʽ.
 *@param        unMode ��ӡģʽ(Ĭ����ʹ������ģʽ)
 *@param    unSigOrDou ��ӡ��˫��ѡ��(ֻ�������Ч����������)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/

int NDK_NO_PrnSetMode(EM_PRN_MODE emMode,uint unSigOrDou)
{
    return -1;
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
int NDK_NO_PrnIntegrate(uint unXsize,uint unYsize,uint unXpos,uint unYpos,const char *psImgBuf,const char *pszTextBuf, uint unMode)
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
int NDK_NO_PrnSetGreyScale(uint unGrey)
{
    return -1;

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
int NDK_NO_PrnSetForm(uint unBorder,uint unColumn, uint unRow)
{
    return -1;
}

/**
 *@brief    ��������ֽ
 *@details  �ô�ӡ����ֽ������Ϊ���ص�,���øú�������û��������ֽ�����Ǵ��ڽṹ���У��ȵ���start֮��ʹ�ӡ����һ��ִ��
 *@param  unPixel ��ֽ���ص� ֵ��> 0
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_NO_PrnFeedByPixel(uint unPixel)
{
    return -1;
}

/**
 *@brief    ��ӡ�Ƿ����»��߹���.
 *@param  emStatus 0�����»��ߣ�1�����»���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_NO_PrnSetUnderLine(EM_PRN_UNDERLINE_STATUS emStatus)
{
    return -1;
}

/**
 *@brief    ���ö��뷽ʽ.(��δʵ��)
 *@param  unType 0:�����; 1���ж���; 2�Ҷ���;3�رն��뷽ʽ
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_NO_PrnSetAlignment(uint unType)
{
    return -1;
}

/**
 *@brief    ��ȡ��ӡ����.
 *@retval pfLen �����Ѿ���ӡ�ĳ���(��ǰ��������ʼ��ӡ�����ڴ�ӡ����Ϊֹ����ӡ����ӡ���ܹ�����)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_NO_PrnGetPrintedLen(uint *punLen)
{
    return -1;
}

/**
 *@brief        �Զ�������ע�ᡣ
 *@param        pstMsg ST_PrintFontMsg����ָ�룬ʹ���Զ���ע��Ҫ�����Ӧ��Ϣ����д
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_NO_PrnFontRegister(ST_PRN_FONTMSG *pstMsg)
{
    return -1;
}

/**
 *@brief    ����ע�����ѡ���ӡ����.
 *@param  unFontId ע�������id(�����ú�Ḳ��NDK_PrnSetFont���趨������)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_NO_PrnSetUsrFont(uint unFontId)
{
    return -1;
}

/**
 *@brief    ��øôδ�ӡ�ĵ�������.
 *@retval  punLine ��������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/
int NDK_NO_PrnGetDotLine(uint *punLine)
{
    return -1;
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
int NDK_NO_PrnPicture(uint unXpos,const char *pszPath)
{
    return -1;
}

int NDK_NO_PrnSetPageLen(uint len)
{
    return -1;
}

/**
 *@brief    ����BDF����
 *@detail  ʹ�øú�������BDF���嵽�ڴ��У��Ƚϴ�������ķ�һЩʱ�䡣
 *@param  pszPath BDF���ڵ�·��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/

int NDK_NO_PrnLoadBDFFont(const char *pszPath)
{
    return -1;
}

/**
 *@brief    ��ӡBDF�����е�����
 *@param  pusText unsigned short ���͵����ݡ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   ����EM_NDK_ERR      ����ʧ��
*/

int NDK_NO_PrnBDFStr(ushort *pusText)
{
    return -1;
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

int NDK_NO_PrnSetBDF(uint unXpos,uint unLineSpace,uint unWordSpace,uint unXmode,uint unYmode)
{
    return -1;
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
NEXPORT int NDK_NO_PrnFeedPaper(EM_PRN_FEEDPAPER emType)
{
    return -1;
}

/* End of this file */

