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

#include <stdlib.h>
#include <string.h>

#include "NDK.h"
#include "NDK_debug.h"
#include "NDK_tools.h"

/** @addtogroup 工具
* @{
*/

/**
 *@brief    2个最大不超过12位的无符号数字串加法
 *@details  2个数字串逐次逐位相加，相加结果不能超过12位
 *@param    pszDigStr1      数字串1
 *@param    pszDigStr2      数字串2
 *@param    pnResultLen     结果缓冲区pszResult的大小
 *@retval   pszResult       相加和的数字串
 *@retval   pnResultLen     相加和的位数
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"        参数非法(pszDigStr1/pszDigStr2/pszResult/pnResultLen为NULL、数字串不合法)
 *@li   \ref NDK_ERR "NDK_ERR"      操作失败(两数字串相加超过12位)
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
    /*判断数字串是否合法*/
    if ((NDK_IsDigitStr(pszDigStr1) != NDK_OK) || (NDK_IsDigitStr(pszDigStr2) != NDK_OK)) {
        return NDK_ERR_PARA;
    }

    memset(sBuf, '0', 12);
    sBuf[12] = 0;
    memcpy(sBuf + 12 - unLen1, pszDigStr1, unLen1);

    /*循环变量i: 取位置; 变量nFlag: 进位标志*/
    sBuf[11] += pszDigStr2[unLen2 - 1] - '0';
    for (i = unLen2 - 1; i >= 0; i --) {
        nFlag = 0;
        if (sBuf[i + (11 - (unLen2 - 1))] - '0' >= 10) {
            nFlag = 1;
            /*若两个 12位的数字串相加超过12位，返回失败*/
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
 *@brief    将6位数字串pcStrNum增加1后放回原值
 *@param    pszStrNum       需要被增加的数字串,缓冲区长度至少为7
 *@retval   pszStrNum       增加后的结果串
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"        参数非法(pszStrNum为NULL、pszStrNum长度大于6，pszStrNum数字串不合法)
*/
NEXPORT int NDK_IncNum (uchar * pszStrNum )
{
    long lNum;
    uchar szBuf[6 + 1];

    if ( (pszStrNum == NULL) || ( strlen((const char *)pszStrNum) > 6 ) ) {
        return NDK_ERR_PARA;
    }
    /*判断数字串是否合法*/
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
 *@brief    把带小数点的金额字符串转为不带小数点的金额字符串
 *@param    pszSource       待转换的金额字符串
 *@param    pnTargetLen     目标缓冲区的大小
 *@retval   pszTarget       转换后的字符串
 *@retval   pnTargetLen     转换后的字符串长度
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(pszSource/pszTarget/pnTargetLen为NULL)
*/
NEXPORT int NDK_FmtAmtStr (const uchar* pszSource, uchar* pszTarget, int* pnTargetLen )
{
    int i = 0, j = 0, unLen = 0, nFlag = 0;

    if ((pszSource == NULL) || (pszTarget == NULL) || (pnTargetLen == NULL)) {
        return NDK_ERR_PARA;
    }

    /*循环变量i: 源串位移; 循环变量j: 目标串位移*/
    unLen = strlen((const char *)pszSource);
    if (*pnTargetLen < unLen) {
        return NDK_ERR_PARA;
    }

    for (i = 0, j = 0; i < unLen; i ++, j ++) {
        if ( ((pszSource[i] - '0' < 0) || (pszSource[i] - '0' > 9)) && (pszSource[i] != '.')) {
            return NDK_ERR_PARA;
        }
        if (pszSource[i] == '.') {
            /*小数点在首位返回非法*/
            if (i==0) {
                return NDK_ERR_PARA;
            }
            j --;
            nFlag ++;
        } else {
            pszTarget[j] = pszSource[i];
        }
    }

    /*变量nFlag：源串中小数点的个数，超过1个则源串非法*/
    if (nFlag > 1) {
        return NDK_ERR_PARA;
    }
    *pnTargetLen = strlen((const char *)pszTarget);
    return NDK_OK;
}

/**
 *@brief    将AscII码的字符串转换成压缩的HEX格式
 *@details  非偶数长度的字符串根据对齐方式，采取左补0，右补F方式
 *@param    pszAsciiBuf     被转换的ASCII字符串
 *@param    nLen            输入数据长度(ASCII字符串的长度)
 *@param    ucType          对齐方式  0－左对齐  1－右对齐
 *@retval   psBcdBuf        转换输出的HEX数据
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(pszAsciiBuf/psBcdBuf为NULL、nLen<=0)
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

    if (nLen & 0x01 && ucType) { /*判别是否为奇数以及往那边对齐*/
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
 *@brief    将HEX码数据转换成AscII码字符串
 *@param    psBcdBuf        被转换的Hex数据
 *@param    nLen            转换数据长度(ASCII字符串的长度)
 *@param    ucType          对齐方式  1－左对齐  0－右对齐
 *@retval   pszAsciiBuf     转换输出的AscII码数据
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(psBcdBuf/pszAsciiBuf为NULL、nLen<0、ucType非法)
*/
NEXPORT int NDK_HexToAsc (const uchar* psBcdBuf, int nLen, uchar ucType, uchar* pszAsciiBuf)
{
    int i = 0;

    if ((psBcdBuf == NULL) || (pszAsciiBuf == NULL) || (nLen<0)
        || ((ucType!=0) && (ucType!=1))) {
        return NDK_ERR_PARA;
    }

    if ((nLen & 0x01) && ucType) { /*判别是否为奇数以及往那边对齐*/
        /*0左，1右*/
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
 *@brief    整型转换为4字节字符数组（高位在前）
 *@param    unNum       需要转换的整型数
 *@retval   psBuf       转换输出的字符串
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(psBuf为NULL)
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
 *@brief    整型转换为2字节字符数组（高位在前）
 *@param    unNum       需要转换的整型数
 *@retval   psBuf       转换输出的字符串
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(psBuf为NULL)
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
 *@brief    4字节字符数组转换为整型（高位在前）
 *@param    psBuf       需要转换的字符串
 *@retval   unNum       转换输出的整型数
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(unNum、psBuf为NULL)
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
 *@brief    2字节字符数组转换为整型（高位在前）
 *@details  psBuf长度要>=2
 *@param    psBuf       需要转换的字符串
 *@retval   unNum       转换输出的整型数
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(unNum、psBuf为NULL)
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
 *@brief    整数(0-99)转换为一字节BCD
 *@param    nNum        需要转换的整型数(0-99)
 *@retval   ch          转换输出的一个BCD字符
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(ch为NULL、nNum非法)
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
 *@brief    一字节BCD转换为整数(0-99)
 *@param    ch      需要转换的BCD字符
 *@retval   pnNum   转换输出的整数值(0-99)
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(pnNum为NULL、ch 非法)
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
 *@brief    整数(0-9999)需要转换的整型数(0-9999)
 *@param    nNum        数字串1
 *@param    pnBcdLen    输出缓冲区的大小
 *@retval   pnBcdLen    转换后的BCD长度，如果成功此值，固定返回值为2
 *@retval   psBcd       转换输出的两字节BCD
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(psBcd、pnBcdLen为NULL、nNum 非法)
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
 *@brief    二字节BCD转换为整数(0-9999)
 *@details  psBcd长度应等于2
 *@param    psBcd       需要转换的两字节BCD
 *@retval   nNum        转换后的整数(0-9999)
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(psBcd、nNum为NULL、nNum 非法)
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
 *@brief    计算LRC
 *@details  psbuf缓冲的长度>nLen
 *@param    psBuf       需要计算LRC的字符串
 *@param    nLen        需要计算LRC的字符串的长度
 *@retval   ucLRC       计算得出的LRC
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(psBuf、ucLRC为NULL、nLen<=0)
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
 *@brief    字符串去左空格
 *@param    pszBuf      存放字符串的缓冲区
 *@retval   pszBuf      去掉左空格后的字符串
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(pszBuf为NULL)
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
 *@brief    字符串去右空格
 *@param    pszBuf      存放字符串的缓冲区
 *@retval   pszBuf      去掉右空格后的字符串
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(pszBuf为NULL)
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
 *@brief    字符串去左右空格
 *@param    pszBuf          存放字符串的缓冲区
 *@retval   pszBuf          去掉左右空格后的字符串
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(pszBuf为NULL)
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
 *@brief    往一字符串里加入某一字符使之长度为nLen
 *@details  pszString缓冲的长度应>nlen, 字符串的长度要小于nlen
 *@param    pszString       存放字符串的缓冲区
 *@param    ch              所要加入的字符
 *@param    nOption         操作类型
                            0    往字符串前面加字符
                            1    往字符串后面加字符
                            2    往字符串前后加字符
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(pszString为NULL、pszString长度非法、nOption非法)
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
 *@brief    截取子串
 *@details  子串后带'\0'结束符
 *@param    pszSouStr       需要进行截取的字符串
 *@param    nStartPos       要截取子串的起始位置 字符串的位置由1开始计数
 *@param    nNum            要截取的字符数
 *@retval   pszObjStr       存放目标串的缓冲区
 *@retval   pnObjStrLen     子串的长度
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(pszObjStr/pnObjStrLen/pszSouStr为NULL)
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
 *@brief    判断给定一字符是不是数字字符
 *@param    ch      需要判断的字符
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(ch非法)
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
 *@brief    测试一字串是否为纯数字串
 *@param    pszString       需要判断的字符串
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(pszString为NULL)
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
 *@brief    取某子串转化成数值
 *@details  仅供公共库实现过程中调用，不作为对外的功能提供
 *@param    psString        起始日期 格式为 YYYYMMDD
 *@param    nStart          起始位置
 *@param    nNumber         数串长度
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(psString为NULL、nNumber>5)
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
 *@brief    判断某年是否闰年
 *@param    nYear       年份
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR "NDK_ERR"  操作失败
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
 *@brief    找出某年某月的最大天数
 *@param    nYear       年份
 *@param    nMon        月份
 *@retval   pnDays      该年份该月对应的最大天数
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(年、月、日非法)
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
 *@brief    判断提供的字符串是不是合法的日期格式串
 *@param    pszDate     日期格式字符串  格式为 YYYYMMDD
 *@return
 *@li   NDK_OK              操作成功
 *@li   \ref NDK_ERR_PARA "NDK_ERR_PARA"    参数非法(pszDate为NULL、pszDate串长度不等于8、pszDate非法)
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


/** @} */ // 工具模块结束

/* End of this file */
