#ifndef _NDK_PRNFONT_H_
#define _NDK_PRNFONT_H_

/*
  *�ֽڶ����Ŵ�����
  */
#define TPFC_VAR_ZOOM2_LEN			(256*2)
extern const unsigned char g_pucZoom2Buf[TPFC_VAR_ZOOM2_LEN];
/*
  *�ֽ������Ŵ�����
  */
#define TPFC_VAR_ZOOM3_LEN			(256*3)
extern const unsigned char g_pucZoom3Buf[TPFC_VAR_ZOOM3_LEN];
/*
  *�ֽڽ���ѹ��ģʽ����
  */
#define TPFC_VAR_CLOSECOMPRESS_LEN	(16)
extern const unsigned char g_pucCloseCompressBuf[TPFC_VAR_CLOSECOMPRESS_LEN];
/*
  *�ֽڿ���ѹ��ģʽ����
  */
#define TPFC_VAR_LOOSECOMPRESS_LEN	(16)
extern const unsigned char g_pucLooseCompressBuf[TPFC_VAR_LOOSECOMPRESS_LEN];
/*
  *����ת������
  */
#define TPFC_VAR_FONT_BUF_LEN	(48*6)
extern unsigned char g_pucFontBuf[TPFC_VAR_FONT_BUF_LEN];


extern void ndk_DotChange(char * ch1, int iXMagnify,int iYMagnify, int x,int y);

#endif

