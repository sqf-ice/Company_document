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

#include <stdlib.h>
#include <string.h>

#include "NDK.h"
#include "NDK_debug.h"
#include "NDK_tools.h"

/** @addtogroup ����
* @{
*/

/**
 *@brief    2����󲻳���12λ���޷������ִ��ӷ�
 *@details  2�����ִ������λ��ӣ���ӽ�����ܳ���12λ
 *@param    pszDigStr1      ���ִ�1
 *@param    pszDigStr2      ���ִ�2
 *@param    pnResultLen     ���������pszResult�Ĵ�С
 *@retval   pszResult       ��Ӻ͵����ִ�
 *@retval   pnResultLen     ��Ӻ͵�λ��
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"        �����Ƿ�(pszDigStr1/pszDigStr2/pszResult/pnResultLenΪNULL�����ִ����Ϸ�)
 *@li   \ref NDK_ERR "NDK_ERR"      ����ʧ��(�����ִ���ӳ���12λ)
*/
NEXPORT int NDK_AddDigitStr(const uchar *pszDigStr1, const uchar *pszDigStr2, uchar* pszResult, int *pnResultLen )
{
    int unLen1 = 0, unLen2 = 0, i = 0, nFlag = 0;
    uchar sBuf[12+1];

    if ((pszDigStr1 == NULL) || (pszDigStr2 == NULL) || (pszResult==NULL)||(pnResultLen==NULL)) {
        return NDK_ERR_PARA;
    }
    unLen1 = strlen((const char *)pszDigStr1);
    unLen2 = strlen((const char *)pszDigStr2);
    if ((12 < unLen1) || (12 < unLen2)) {
        return NDK_ERR_PARA;
    }
    /*�ж����ִ��Ƿ�Ϸ�*/
    if ((NDK_IsDigitStr(pszDigStr1) != NDK_OK) || (NDK_IsDigitStr(pszDigStr2) != NDK_OK)) {
        return NDK_ERR_PARA;
    }

    memset(sBuf, '0', 12);
    sBuf[12] = 0;
    memcpy(sBuf + 12 - unLen1, pszDigStr1, unLen1);

    /*ѭ������i: ȡλ��; ����nFlag: ��λ��־*/
    sBuf[11] += pszDigStr2[unLen2 - 1] - '0';
    for (i = unLen2 - 1; i >= 0; i --) {
        nFlag = 0;
        if (sBuf[i + (11 - (unLen2 - 1))] - '0' >= 10) {
            nFlag = 1;
            /*������ 12λ�����ִ���ӳ���12λ������ʧ��*/
            if (i + (11 - (unLen2 - 1)) - 1 < 0) {
                return NDK_ERR;
            }
            sBuf[i + (11 - (unLen2 - 1))] -= 10;
            if ( i == 0 ) {
                sBuf[i + (11 - (unLen2 - 1)) - 1] += nFlag;
                if ( unLen1 > unLen2 ) {
                    if( sBuf[i + (11 - (unLen2 - 1)) - 1] - '0' >= 10 ) {
                        sBuf[i + (11-(unLen2-1)) -1] -= 10;
                        sBuf[i + (11-(unLen2-1)) -1 - 1] += nFlag;
                    } else {
                        nFlag = 0;
                    }
                }
            } else {
                sBuf[i + (11 - (unLen2 - 1)) - 1]
                += (pszDigStr2[i - 1] - '0') + nFlag;
            }
        } else {
            if (i>0) {
                sBuf[i + (11 - (unLen2 - 1)) - 1] += (pszDigStr2[i - 1] - '0');
            }
            nFlag = 0;
        }
    }

    //memset(pszResult, '\0', *pnResultLen);
    if(unLen1 > unLen2) {
        if (*pnResultLen < unLen1 + nFlag) {
            return NDK_ERR_PARA;
        } else
            *pnResultLen = unLen1 + nFlag;
    } else {
        if (*pnResultLen < unLen2 + nFlag) {
            return NDK_ERR_PARA;
        } else
            *pnResultLen = unLen2 + nFlag;
    }
    memcpy(pszResult, sBuf + 12 - *pnResultLen, *pnResultLen);

    return NDK_OK;
}

/**
 *@brief    ��6λ���ִ�pcStrNum����1��Ż�ԭֵ
 *@param    pszStrNum       ��Ҫ�����ӵ����ִ�,��������������Ϊ7
 *@retval   pszStrNum       ���Ӻ�Ľ����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"        �����Ƿ�(pszStrNumΪNULL��pszStrNum���ȴ���6��pszStrNum���ִ����Ϸ�)
*/
NEXPORT int NDK_IncNum (uchar * pszStrNum )
{
    long lNum;
    uchar szBuf[6 + 1];

    if ( (pszStrNum == NULL) || ( strlen((const char *)pszStrNum) > 6 ) ) {
        return NDK_ERR_PARA;
    }
    /*�ж����ִ��Ƿ�Ϸ�*/
    if (NDK_IsDigitStr(pszStrNum) != NDK_OK) {
        return NDK_ERR_PARA;
    }

    memset(szBuf, '\0', sizeof(szBuf));
    memcpy(szBuf, pszStrNum, 6);

    lNum = atol((char *)szBuf) + 1l;
    if (lNum >= 1000000l) {
        lNum = 1l;
    }
    sprintf((char *)pszStrNum, "%06ld", lNum);

    return NDK_OK;
}

/**
 *@brief    �Ѵ�С����Ľ���ַ���תΪ����С����Ľ���ַ���
 *@param    pszSource       ��ת���Ľ���ַ���
 *@param    pnTargetLen     Ŀ�껺�����Ĵ�С
 *@retval   pszTarget       ת������ַ���
 *@retval   pnTargetLen     ת������ַ�������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(pszSource/pszTarget/pnTargetLenΪNULL)
*/
NEXPORT int NDK_FmtAmtStr (const uchar* pszSource, uchar* pszTarget, int* pnTargetLen )
{
    int i = 0, j = 0, unLen = 0, nFlag = 0;

    if ((pszSource == NULL) || (pszTarget == NULL) || (pnTargetLen == NULL)) {
        return NDK_ERR_PARA;
    }

    /*ѭ������i: Դ��λ��; ѭ������j: Ŀ�괮λ��*/
    unLen = strlen((const char *)pszSource);
    if (*pnTargetLen < unLen) {
        return NDK_ERR_PARA;
    }

    for (i = 0, j = 0; i < unLen; i ++, j ++) {
        if ( ((pszSource[i] - '0' < 0) || (pszSource[i] - '0' > 9)) && (pszSource[i] != '.')) {
            return NDK_ERR_PARA;
        }
        if (pszSource[i] == '.') {
            /*С��������λ���طǷ�*/
            if (i==0) {
                return NDK_ERR_PARA;
            }
            j --;
            nFlag ++;
        } else {
            pszTarget[j] = pszSource[i];
        }
    }

    /*����nFlag��Դ����С����ĸ���������1����Դ���Ƿ�*/
    if (nFlag > 1) {
        return NDK_ERR_PARA;
    }
    *pnTargetLen = strlen((const char *)pszTarget);
    return NDK_OK;
}

/**
 *@brief    ��AscII����ַ���ת����ѹ����HEX��ʽ
 *@details  ��ż�����ȵ��ַ������ݶ��뷽ʽ����ȡ��0���Ҳ�F��ʽ
 *@param    pszAsciiBuf     ��ת����ASCII�ַ���
 *@param    nLen            �������ݳ���(ASCII�ַ����ĳ���)
 *@param    ucType          ���뷽ʽ  0�������  1���Ҷ���
 *@retval   psBcdBuf        ת�������HEX����
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(pszAsciiBuf/psBcdBufΪNULL��nLen<=0)
*/
NEXPORT int NDK_AscToHex (const uchar* pszAsciiBuf, int nLen, uchar ucType, uchar* psBcdBuf)
{
    int i = 0;
    uchar cTmp, cTmp1;

    if ((pszAsciiBuf == NULL) || (psBcdBuf==NULL) || (nLen <= 0) ) {
        return NDK_ERR_PARA;
    }

    if ((ucType!=0) && (ucType!=1)) {
        return  NDK_ERR_PARA;
    }

    if (nLen & 0x01 && ucType) { /*�б��Ƿ�Ϊ�����Լ����Ǳ߶���*/
        cTmp1 = 0 ;
    } else {
        cTmp1 = 0x55 ;
    }

    for (i = 0; i < nLen; pszAsciiBuf ++, i ++) {
        if ( *pszAsciiBuf >= 'a' ) {
            cTmp = *pszAsciiBuf - 'a' + 10 ;
        } else if ( *pszAsciiBuf >= 'A' ) {
            cTmp = *pszAsciiBuf - 'A' + 10 ;
        } else if ( *pszAsciiBuf >= '0' ) {
            cTmp = *pszAsciiBuf - '0' ;
        } else {
            cTmp = *pszAsciiBuf;
            cTmp&=0x0f;
        }

        if ( cTmp1 == 0x55 ) {
            cTmp1 = cTmp;
        } else {
            *psBcdBuf ++ = cTmp1 << 4 | cTmp;
            cTmp1 = 0x55;
        }
    }
    if (cTmp1 != 0x55) {
        *psBcdBuf = cTmp1 << 4;
    }

    return NDK_OK;
}

/**
 *@brief    ��HEX������ת����AscII���ַ���
 *@param    psBcdBuf        ��ת����Hex����
 *@param    nLen            ת�����ݳ���(ASCII�ַ����ĳ���)
 *@param    ucType          ���뷽ʽ  1�������  0���Ҷ���
 *@retval   pszAsciiBuf     ת�������AscII������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(psBcdBuf/pszAsciiBufΪNULL��nLen<0��ucType�Ƿ�)
*/
NEXPORT int NDK_HexToAsc (const uchar* psBcdBuf, int nLen, uchar ucType, uchar* pszAsciiBuf)
{
    int i = 0;

    if ((psBcdBuf == NULL) || (pszAsciiBuf == NULL) || (nLen<0)
        || ((ucType!=0) && (ucType!=1))) {
        return NDK_ERR_PARA;
    }

    if ((nLen & 0x01) && ucType) { /*�б��Ƿ�Ϊ�����Լ����Ǳ߶���*/
        /*0��1��*/
        i = 1;
        nLen ++;
    } else {
        i = 0;
    }
    for (; i < nLen; i ++, pszAsciiBuf ++) {
        if (i & 0x01) {
            *pszAsciiBuf = *psBcdBuf ++ & 0x0f;
        } else {
            *pszAsciiBuf = *psBcdBuf >> 4;
        }
        if (*pszAsciiBuf > 9) {
            *pszAsciiBuf += 'A' - 10;
        } else {
            *pszAsciiBuf += '0';
        }

    }
    *pszAsciiBuf = 0;
    return NDK_OK;
}

/**
 *@brief    ����ת��Ϊ4�ֽ��ַ����飨��λ��ǰ��
 *@param    unNum       ��Ҫת����������
 *@retval   psBuf       ת��������ַ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(psBufΪNULL)
*/
NEXPORT int NDK_IntToC4 (uchar* psBuf, uint unNum )
{
    if (psBuf==NULL) {
        return NDK_ERR_PARA;
    }

    *( psBuf ) = unNum >> 24;
    *( psBuf + 1 ) = (unNum >> 16) ;
    *( psBuf + 2 ) = (unNum >> 8) ;
    *( psBuf + 3 ) = unNum %256;
    return NDK_OK;
}

/**
 *@brief    ����ת��Ϊ2�ֽ��ַ����飨��λ��ǰ��
 *@param    unNum       ��Ҫת����������
 *@retval   psBuf       ת��������ַ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(psBufΪNULL)
*/
NEXPORT int NDK_IntToC2(uchar* psBuf, uint unNum )
{
    if (psBuf==NULL) {
        return NDK_ERR_PARA;
    }

    if (unNum <= 65535) {
        *(psBuf + 1) = unNum % 256;
        *psBuf = unNum >>8;
    }
    return NDK_OK;
}

/**
 *@brief    4�ֽ��ַ�����ת��Ϊ���ͣ���λ��ǰ��
 *@param    psBuf       ��Ҫת�����ַ���
 *@retval   unNum       ת�������������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(unNum��psBufΪNULL)
*/
NEXPORT int NDK_C4ToInt(uint* unNum, uchar* psBuf )
{
    if ((unNum==NULL) || (psBuf==NULL)) {
        return NDK_ERR_PARA;
    }

    *unNum = ((*psBuf) << 24) + (*(psBuf+1) << 16) + (*(psBuf+2) << 8) + (*(psBuf + 3));
    return NDK_OK;
}

/**
 *@brief    2�ֽ��ַ�����ת��Ϊ���ͣ���λ��ǰ��
 *@details  psBuf����Ҫ>=2
 *@param    psBuf       ��Ҫת�����ַ���
 *@retval   unNum       ת�������������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(unNum��psBufΪNULL)
*/
NEXPORT int NDK_C2ToInt(uint *unNum, uchar *psBuf)
{
    if ((unNum==NULL) || (psBuf==NULL)) {
        return NDK_ERR_PARA;
    }

    *unNum = ((*psBuf) << 8) + (*(psBuf + 1));
    return NDK_OK;
}

/**
 *@brief    ����(0-99)ת��Ϊһ�ֽ�BCD
 *@param    nNum        ��Ҫת����������(0-99)
 *@retval   ch          ת�������һ��BCD�ַ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(chΪNULL��nNum�Ƿ�)
*/
NEXPORT int NDK_ByteToBcd(int nNum, uchar *ch)
{
    if (ch==NULL) {
        return NDK_ERR_PARA;
    }

    if ((nNum < 0) || (nNum > 99)) {
        return NDK_ERR_PARA;
    }
    *ch = ((nNum / 10) << 4) | (nNum % 10);
    return NDK_OK;
}

/**
 *@brief    һ�ֽ�BCDת��Ϊ����(0-99)
 *@param    ch      ��Ҫת����BCD�ַ�
 *@retval   pnNum   ת�����������ֵ(0-99)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(pnNumΪNULL��ch �Ƿ�)
*/
NEXPORT int NDK_BcdToByte(uchar ch, int *pnNum)
{
    if (pnNum==NULL) {
        return NDK_ERR_PARA;
    }

    if (((ch & 0x0F) > 9) || ((ch >> 4) > 9)) {
        return NDK_ERR_PARA;
    }

    *pnNum = (ch >> 4) * 10 + (ch & 0x0f);
    return NDK_OK;
}

/**
 *@brief    ����(0-9999)��Ҫת����������(0-9999)
 *@param    nNum        ���ִ�1
 *@param    pnBcdLen    ����������Ĵ�С
 *@retval   pnBcdLen    ת�����BCD���ȣ�����ɹ���ֵ���̶�����ֵΪ2
 *@retval   psBcd       ת����������ֽ�BCD
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(psBcd��pnBcdLenΪNULL��nNum �Ƿ�)
*/
NEXPORT int NDK_IntToBcd(uchar *psBcd, int *pnBcdLen, int nNum)
{
    if ((psBcd==NULL) || (pnBcdLen==NULL)) {
        return NDK_ERR_PARA;
    }

    if ((nNum < 0) || (nNum > 9999) || (*pnBcdLen < 2)) {
        return NDK_ERR_PARA;
    }

    NDK_ByteToBcd(nNum / 100, &psBcd[0]);
    NDK_ByteToBcd(nNum % 100, &psBcd[1]);

    *pnBcdLen = 2;
    return NDK_OK;
}

/**
 *@brief    ���ֽ�BCDת��Ϊ����(0-9999)
 *@details  psBcd����Ӧ����2
 *@param    psBcd       ��Ҫת�������ֽ�BCD
 *@retval   nNum        ת���������(0-9999)
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(psBcd��nNumΪNULL��nNum �Ƿ�)
*/
NEXPORT int NDK_BcdToInt(const uchar * psBcd, int *nNum)
{
    int nNum1, nNum2, ret;

    if ((psBcd == NULL) || (nNum==NULL)) {
        return NDK_ERR_PARA;
    }

    ret = NDK_BcdToByte(psBcd[0], &nNum1);
    if (ret!=NDK_OK) {
        return ret;
    }
    ret = NDK_BcdToByte(psBcd[1], &nNum2);
    if (ret!=NDK_OK) {
        return ret;
    }
    *nNum = nNum1 * 100 + nNum2;
    return NDK_OK;
}

/**
 *@brief    ����LRC
 *@details  psbuf����ĳ���>nLen
 *@param    psBuf       ��Ҫ����LRC���ַ���
 *@param    nLen        ��Ҫ����LRC���ַ����ĳ���
 *@retval   ucLRC       ����ó���LRC
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(psBuf��ucLRCΪNULL��nLen<=0)
*/
NEXPORT int NDK_CalcLRC(const uchar *psBuf, int nLen, uchar *ucLRC)
{
    int i;

    if ((psBuf == NULL) || (ucLRC==NULL) || (nLen <= 0) ) {
        return NDK_ERR_PARA;
    }

    *ucLRC = 0x00;
    for (i = 0; i < nLen; i++) {
        *ucLRC ^= psBuf[i];
    }
    return NDK_OK;
}

/**
 *@brief    �ַ���ȥ��ո�
 *@param    pszBuf      ����ַ����Ļ�����
 *@retval   pszBuf      ȥ����ո����ַ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(pszBufΪNULL)
*/
NEXPORT int NDK_LeftTrim(uchar *pszBuf)
{
    int j, k, nLen;

    if (pszBuf==NULL) {
        return NDK_ERR_PARA;
    }

    j = 0;
    nLen = strlen((const char *)pszBuf);
    while ((j < nLen) && (pszBuf[j] == ' ')) {
        j++;
    }
    for (k = j; k <= nLen; k++) {
        pszBuf[k-j] = pszBuf[k];
    }
    return NDK_OK;
}

/**
 *@brief    �ַ���ȥ�ҿո�
 *@param    pszBuf      ����ַ����Ļ�����
 *@retval   pszBuf      ȥ���ҿո����ַ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(pszBufΪNULL)
*/
NEXPORT int NDK_RightTrim(uchar *pszBuf)
{
    int i;

    if (pszBuf==NULL) {
        return NDK_ERR_PARA;
    }

    i = strlen((const char *)pszBuf);
    while ((i > 0) && (pszBuf[i-1] == ' ')) {
        i--;
        pszBuf[i] = '\0';
    }
    return NDK_OK;
}

/**
 *@brief    �ַ���ȥ���ҿո�
 *@param    pszBuf          ����ַ����Ļ�����
 *@retval   pszBuf          ȥ�����ҿո����ַ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(pszBufΪNULL)
*/
NEXPORT int NDK_AllTrim(uchar *pszBuf)
{
    int i, j, k;

    if (pszBuf==NULL) {
        return NDK_ERR_PARA;
    }

    i = strlen((const char *)pszBuf);
    while ((i > 0) && (pszBuf[i-1] == ' ')) {
        i--;
        pszBuf[i] = '\0';
    }

    j = 0;
    while ((j < i) && (pszBuf[j] == ' ')) {
        j++;
    }
    for (k = j; k <= i; k++) {
        pszBuf[k-j] = pszBuf[k];
    }
    return NDK_OK;
}

/**
 *@brief    ��һ�ַ��������ĳһ�ַ�ʹ֮����ΪnLen
 *@details  pszString����ĳ���Ӧ>nlen, �ַ����ĳ���ҪС��nlen
 *@param    pszString       ����ַ����Ļ�����
 *@param    ch              ��Ҫ������ַ�
 *@param    nOption         ��������
                            0    ���ַ���ǰ����ַ�
                            1    ���ַ���������ַ�
                            2    ���ַ���ǰ����ַ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(pszStringΪNULL��pszString���ȷǷ���nOption�Ƿ�)
*/
NEXPORT int NDK_AddSymbolToStr(uchar *pszString, int nLen, uchar ch, int nOption)
{
    int i, j, p;

    if (pszString==NULL) {
        return NDK_ERR_PARA;
    }
    i = strlen((const char *)pszString);
    if (i >= nLen) {
        return NDK_ERR_PARA;
    }

    switch (nOption) {
        case    0:
            p = nLen-i;
            for (j = i; j >= 0; j--) {
                pszString[j+p] = pszString[j];
            }
            for (i = 0; i < p; i++) {
                pszString[i] = ch;
            }
            break;
        case    1:
            for (; i < nLen; i++) {
                pszString[i] = ch;
            }
            pszString[i] = '\0';
            break;
        case    2:
            p = (nLen - i) / 2;
            for (j = i; j >= 0; j--) {
                pszString[j+p] = pszString[j];
            }

            for (i = 0; i < p; i++) {
                pszString[i] = ch;
            }

            i = strlen((const char *)pszString);
            for (; i < nLen; i++) {
                pszString[i] = ch;
            }
            pszString[i] = '\0';
            break;
        default:
            return NDK_ERR_PARA;
    }

    return NDK_OK;
}

/**
 *@brief    ��ȡ�Ӵ�
 *@details  �Ӵ����'\0'������
 *@param    pszSouStr       ��Ҫ���н�ȡ���ַ���
 *@param    nStartPos       Ҫ��ȡ�Ӵ�����ʼλ�� �ַ�����λ����1��ʼ����
 *@param    nNum            Ҫ��ȡ���ַ���
 *@retval   pszObjStr       ���Ŀ�괮�Ļ�����
 *@retval   pnObjStrLen     �Ӵ��ĳ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(pszObjStr/pnObjStrLen/pszSouStrΪNULL)
*/
NEXPORT int NDK_SubStr(const uchar *pszSouStr, int nStartPos, int nNum, uchar *pszObjStr, int *pnObjStrLen)
{
    if ((pszObjStr == NULL) || (pnObjStrLen==NULL) || (pszSouStr == NULL)) {
        return NDK_ERR_PARA;
    }

    if ((*pnObjStrLen < nNum) || (nStartPos < 1) || (nNum <= 0) || (nStartPos > strlen((const char *)pszSouStr))) {
        *pszObjStr = '\0';
        *pnObjStrLen = 0;
        return NDK_ERR_PARA;
    }

    memcpy(pszObjStr, pszSouStr+nStartPos-1, nNum);
    *(pszObjStr+nNum) = '\0';
    *pnObjStrLen = strlen((const char *)pszObjStr);
    return NDK_OK;
}

/**
 *@brief    �жϸ���һ�ַ��ǲ��������ַ�
 *@param    ch      ��Ҫ�жϵ��ַ�
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(ch�Ƿ�)
*/
NEXPORT int NDK_IsDigitChar(uchar ch)
{
    if ((ch >= '0') && (ch <= '9')) {
        return NDK_OK;
    } else {
        return NDK_ERR_PARA;
    }
}

/**
 *@brief    ����һ�ִ��Ƿ�Ϊ�����ִ�
 *@param    pszString       ��Ҫ�жϵ��ַ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(pszStringΪNULL)
*/
NEXPORT int NDK_IsDigitStr(const uchar *pszString)
{
    uchar c;
    int nCount;

    if (pszString==NULL) {
        return NDK_ERR_PARA;
    }

    for (nCount=0; (c = *(pszString+nCount)) != '\0'; nCount++) {
        if (NDK_IsDigitChar(c) != NDK_OK) {
            return NDK_ERR_PARA;
        }
    }

    return NDK_OK;
}

/**
 *@brief    ȡĳ�Ӵ�ת������ֵ
 *@details  ����������ʵ�ֹ����е��ã�����Ϊ����Ĺ����ṩ
 *@param    psString        ��ʼ���� ��ʽΪ YYYYMMDD
 *@param    nStart          ��ʼλ��
 *@param    nNumber         ��������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(psStringΪNULL��nNumber>5)
*/
static int ProSubNum(const uchar *psString, int nStart, int nNumber)
{
    uchar sBuf[6];
    int nNum;

    if ((nNumber > 5) || (psString==NULL)) {
        return NDK_ERR_PARA;
    }

    memcpy(sBuf, psString + nStart, nNumber);
    *(sBuf + nNumber) = '\0';
    sscanf((char *)sBuf, "%d", &nNum);
    return(nNum);
}

/**
 *@brief    �ж�ĳ���Ƿ�����
 *@param    nYear       ���
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR "NDK_ERR"  ����ʧ��
*/
NEXPORT int NDK_IsLeapYear(int nYear)
{
    if ((nYear % 400) == 0) {
        return NDK_OK;
    }
    if ((nYear % 4 == 0) && (nYear % 100 != 0)) {
        return NDK_OK;
    } else {
        return NDK_ERR;
    }
}

/**
 *@brief    �ҳ�ĳ��ĳ�µ��������
 *@param    nYear       ���
 *@param    nMon        �·�
 *@retval   pnDays      ����ݸ��¶�Ӧ���������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(�ꡢ�¡��շǷ�)
*/
NEXPORT int NDK_MonthDays(int nYear, int nMon, int *pnDays)
{
    int monthdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (pnDays==NULL) {
        return NDK_ERR_PARA;
    }

    if ((nMon < 1) || (nMon > 12) || (nYear <= 0)) {
        *pnDays = 0;
        return NDK_ERR_PARA;
    }

    if (nMon == 2) {
        if (NDK_IsLeapYear(nYear) == NDK_OK) {
            *pnDays = monthdays[nMon-1] + 1;
        } else {
            *pnDays = monthdays[nMon-1];
        }
    } else {
        *pnDays = monthdays[nMon-1];
    }
    return NDK_OK;
}


/**
 *@brief    �ж��ṩ���ַ����ǲ��ǺϷ������ڸ�ʽ��
 *@param    pszDate     ���ڸ�ʽ�ַ���  ��ʽΪ YYYYMMDD
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    �����Ƿ�(pszDateΪNULL��pszDate�����Ȳ�����8��pszDate�Ƿ�)
*/
NEXPORT int  NDK_IsValidDate(const uchar *pszDate)
{
    int nYear, nMonth, nDay, nMonthDays;

    if (pszDate == NULL || strlen((char*)pszDate) != 8) {
        return NDK_ERR_PARA;
    }

    if (NDK_IsDigitStr(pszDate) != NDK_OK ) {
        return NDK_ERR_PARA;
    }

    nYear  = ProSubNum(pszDate, 0, 4);
    nMonth = ProSubNum(pszDate, 4, 2);
    nDay   = ProSubNum(pszDate, 6, 2);

    if (nYear < 1970) {
        return NDK_ERR_PARA;
    }

    if (NDK_MonthDays(nYear, nMonth, &nMonthDays) < 0) {
        return NDK_ERR_PARA;
    }

    if (( nDay < 1 ) || (nDay > nMonthDays)) {
        return NDK_ERR_PARA;
    }

    return NDK_OK;
}


/** @} */ // ����ģ�����

/* End of this file */
