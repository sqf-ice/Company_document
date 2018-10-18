
#ifndef NDK_TOOLS_H
#define NDK_TOOLS_H

/** @addtogroup 工具
* @{
*/

int NDK_AddDigitStr(const uchar *pszDigStr1, const uchar *pszDigStr2, uchar* pszResult, int *pnResultLen );
int NDK_IncNum (uchar * pszStrNum );
int NDK_FmtAmtStr (const uchar* pszSource, uchar* pszTarget, int* pnTargetLen );
int NDK_AscToHex (const uchar* pszAsciiBuf, int nLen, uchar ucType, uchar* psBcdBuf);
int NDK_HexToAsc (const uchar* psBcdBuf, int nLen, uchar ucType, uchar* pszAsciiBuf);
int NDK_IntToC4 (uchar* psBuf, uint unNum );
int NDK_IntToC2(uchar* psBuf, uint unNum );
int NDK_C4ToInt(uint* unNum, uchar* psBuf );
int NDK_C2ToInt(uint *unNum, uchar *psBuf);
int NDK_ByteToBcd(int nNum, uchar *ch);
int NDK_BcdToByte(uchar ch, int *pnNum);
int NDK_IntToBcd(uchar *psBcd, int *pnBcdLen, int nNum);
int NDK_BcdToInt(const uchar * psBcd, int *nNum);
int NDK_CalcLRC(const uchar *psBuf, int nLen, uchar *ucLRC);
int NDK_LeftTrim(uchar *pszBuf);
int NDK_RightTrim(uchar *pszBuf);
int NDK_AllTrim(uchar *pszBuf);
int NDK_AddSymbolToStr(uchar *pszString, int nLen, uchar ch, int nOption);
int NDK_SubStr(const uchar *pszSouStr, int nStartPos, int nNum, uchar *pszObjStr, int *pnObjStrLen);
int NDK_IsDigitChar(uchar ch);
int NDK_IsDigitStr(const uchar *pszString);
int NDK_IsLeapYear(int nYear);
int NDK_MonthDays(int nYear, int nMon, int *pnDays);
int NDK_IsValidDate(const uchar *pszDate);

/** @} */ // 工具模块结束

#endif

/* End of this file */
