#ifndef __SINOVOICE_PIN_YIN_H__
#define __SINOVOICE_PIN_YIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	jtHWR_FUNC_CODEPAGE_GBK			936		// GBK (default)
#define jtHWR_FUNC_CODEPAGE_UNICODE		1200	// Unicode (UTF-16) little endien
void jtPinYin_SetParam_CodePage(long iParamValue);


/*
 *	功能:根据用户的输入频率调整汉字输出顺序
 *	参数:
 *	pInfo[in]		:拼音数据信息的指针
 *	cInput[in]		:用户输入的待调整汉字
 *	szPinYin[in]	:所要查询的拼音
 *	iLen        	:拼音数据的长度
 *	返回值			无
 */
long jtPinYin_AdjustFrequency(char *                pInfo, 
                              char *                cInput  ,
                              const unsigned short* szPinYin,
				              long                  iLen);

/*
 *	功能:在拼音信息数据中查找指定的汉字
 *	参数:
 *	pInfo[in]		:拼音数据信息的指针
 *	len[in]			:拼音数据的长度
 *	szPinYin[in]	:所要查询的拼音
 *	szBuffer[out]	:由拼音所查询到的汉字缓冲区
 *  buflen[in]      :汉字缓冲区的长度
 *	返回值			-2：验证失败
 *					-1：传入的参数错误，指针为空、长度不足或长度指定错误
 *					 0：传入的不是正确的拼音
 *					其它：实际获得的汉字的个数
 */
 long jtPinYin_SearchWord(
	char * pInfo,
	long len,
	const unsigned short* szPinYin,
	unsigned short *szBuffer,
	long buflen);

#ifdef __cplusplus
}
#endif


#endif //__SINOVOICE_PIN_YIN_H__
