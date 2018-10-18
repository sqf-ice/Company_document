#ifndef __NDK_PRNDRV_H__
#define	__NDK_PRNDRV_H__
/*Æ«ÒÆµØÖ·*/
#define __byAsc8x16DotBuf		0xbde40
#define __byAsc16x8DotBuf		0xbde40
#define __byAsc5x7DotBuf		0xbc040
#define __byAsc8x8DotBuf		0xbc340
#define __byAsc10x8DotBuf	       0xbc640
#define __byAsc5x16DotBuf		0xbcc40
#define __byAsc10x16DotBuf	       0xbd240

extern int ndk_get_prn_fd(void);
extern void ndk_aligndel(void);
extern int ndk_getunderline(void);
extern int ndk_SetFontMsg( int font);
extern int ndk_sendHZtolinebuf(const char * HZDot);
extern int ndk_sendZMtolinebuf(char ZMDot);	
extern int ndk_sendUSRHZtolinebuf(const char* Pdot);
extern int ndk_sendUSRZMtolinebuf(const char* Pdot) ;
extern void ndk_resetnewprintparam(void);
extern int ndk_ClosePrinter(void);
extern int ndk_printData(int xsize,int ysize,int xpos,char *ImgBuf);
extern int ndk_FeedPrinterByPixel (int nPixel);
extern int ndk_GetTotalMotorSteps(void);







#endif
