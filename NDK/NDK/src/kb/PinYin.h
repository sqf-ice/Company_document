#ifndef __SINOVOICE_PIN_YIN_H__
#define __SINOVOICE_PIN_YIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	jtHWR_FUNC_CODEPAGE_GBK			936		// GBK (default)
#define jtHWR_FUNC_CODEPAGE_UNICODE		1200	// Unicode (UTF-16) little endien
void jtPinYin_SetParam_CodePage(long iParamValue);


/*
 *	����:�����û�������Ƶ�ʵ����������˳��
 *	����:
 *	pInfo[in]		:ƴ��������Ϣ��ָ��
 *	cInput[in]		:�û�����Ĵ���������
 *	szPinYin[in]	:��Ҫ��ѯ��ƴ��
 *	iLen        	:ƴ�����ݵĳ���
 *	����ֵ			��
 */
long jtPinYin_AdjustFrequency(char *                pInfo, 
                              char *                cInput  ,
                              const unsigned short* szPinYin,
				              long                  iLen);

/*
 *	����:��ƴ����Ϣ�����в���ָ���ĺ���
 *	����:
 *	pInfo[in]		:ƴ��������Ϣ��ָ��
 *	len[in]			:ƴ�����ݵĳ���
 *	szPinYin[in]	:��Ҫ��ѯ��ƴ��
 *	szBuffer[out]	:��ƴ������ѯ���ĺ��ֻ�����
 *  buflen[in]      :���ֻ������ĳ���
 *	����ֵ			-2����֤ʧ��
 *					-1������Ĳ�������ָ��Ϊ�ա����Ȳ���򳤶�ָ������
 *					 0������Ĳ�����ȷ��ƴ��
 *					������ʵ�ʻ�õĺ��ֵĸ���
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
