#ifndef _NDK_PRNFONT_H_
#define _NDK_PRNFONT_H_

/*
  *字节二倍放大数组
  */
#define TPFC_VAR_ZOOM2_LEN			(256*2)
extern const unsigned char g_pucZoom2Buf[TPFC_VAR_ZOOM2_LEN];
/*
  *字节三倍放大数组
  */
#define TPFC_VAR_ZOOM3_LEN			(256*3)
extern const unsigned char g_pucZoom3Buf[TPFC_VAR_ZOOM3_LEN];
/*
  *字节紧凑压缩模式数组
  */
#define TPFC_VAR_CLOSECOMPRESS_LEN	(16)
extern const unsigned char g_pucCloseCompressBuf[TPFC_VAR_CLOSECOMPRESS_LEN];
/*
  *字节宽松压缩模式数组
  */
#define TPFC_VAR_LOOSECOMPRESS_LEN	(16)
extern const unsigned char g_pucLooseCompressBuf[TPFC_VAR_LOOSECOMPRESS_LEN];
/*
  *字体转换缓冲
  */
#define TPFC_VAR_FONT_BUF_LEN	(48*6)
extern unsigned char g_pucFontBuf[TPFC_VAR_FONT_BUF_LEN];


extern void ndk_DotChange(char * ch1, int iXMagnify,int iYMagnify, int x,int y);

#endif

