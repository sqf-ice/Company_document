
#ifndef _NDK_PRINT_H_
#define _NDK_PRINT_H_
#include <stdio.h>
#include <stdlib.h>

#include "NDK_TPPrint.h"
#include "NDK_TPPrnDrv.h"
#include "NDK_TPPrnFont.h"
#include "../sys/NDK_Sys.h"

extern int NDK_TP_PrnInit(uint unPrnDirSwitch);
extern int NDK_TP_PrnStr(const char *pszBuf);
extern int NDK_TP_PrnStart(void);
extern int NDK_TP_PrnImage(uint unXsize,uint unYsize,uint unXpos,const char *psImgBuf);
extern int NDK_TP_PrnGetType(EM_PRN_TYPE *pemType);
extern int NDK_TP_PrnGetVersion(char *pszVer);
extern int NDK_TP_PrnSetFont(EM_PRN_HZ_FONT emHZFont,EM_PRN_ZM_FONT emZMFont);
extern int NDK_TP_PrnGetStatus(EM_PRN_STATUS *pemStatus);
extern int NDK_TP_PrnSetMode(EM_PRN_MODE emMode,uint unSigOrDou);
extern int NDK_TP_PrnIntegrate(uint unXsize,uint unYsize,uint unXpos,uint unYpos,const char *psImgBuf,const char *pszTextBuf, uint unMode);
extern int NDK_TP_PrnSetGreyScale(uint unGrey);
extern int NDK_TP_PrnSetForm(uint unBorder,uint unColumn, uint unRow);
extern int NDK_TP_PrnFeedByPixel(uint unPixel);
extern int NDK_TP_PrnSetUnderLine(EM_PRN_UNDERLINE_STATUS emStatus);
extern int NDK_TP_PrnSetAlignment(uint unType);
extern int NDK_TP_PrnGetPrintedLen(uint *punLen);
extern int NDK_TP_PrnFontRegister(ST_PRN_FONTMSG *pstMsg);
extern int NDK_TP_PrnSetUsrFont(uint unFontId);
extern int NDK_TP_PrnGetDotLine(uint *punLine);
extern int NDK_TP_PrnPicture(uint unXpos,const char *pszPath);
extern int NDK_TP_PrnSetPageLen(uint len);
extern int NDK_TP_PrnLoadBDFFont(const char *pszPath);
extern int NDK_TP_PrnBDFStr(ushort *pusText);
extern int NDK_TP_PrnSetBDF(uint unXpos,uint unLineSpace,uint unWordSpace,uint unXmode,uint unYmode);
extern int NDK_TP_PrnFeedPaper(EM_PRN_FEEDPAPER emType);


extern int NDK_HIP_PrnInit(uint unPrnDirSwitch);
extern int NDK_HIP_PrnStr(const char *pszBuf);
extern int NDK_HIP_PrnStart(void);
extern int NDK_HIP_PrnImage(uint unXsize,uint unYsize,uint unXpos,const char *psImgBuf);
extern int NDK_HIP_PrnGetType(EM_PRN_TYPE *pemType);
extern int NDK_HIP_PrnGetVersion(char *pszVer);
extern int NDK_HIP_PrnSetFont(EM_PRN_HZ_FONT emHZFont,EM_PRN_ZM_FONT emZMFont);
extern int NDK_HIP_PrnGetStatus(EM_PRN_STATUS *pemStatus);
extern int NDK_HIP_PrnSetMode(EM_PRN_MODE emMode,uint unSigOrDou);
extern int NDK_HIP_PrnIntegrate(uint unXsize,uint unYsize,uint unXpos,uint unYpos,const char *psImgBuf,const char *pszTextBuf, uint unMode);
extern int NDK_HIP_PrnSetGreyScale(uint unGrey);
extern int NDK_HIP_PrnSetForm(uint unBorder,uint unColumn, uint unRow);
extern int NDK_HIP_PrnFeedByPixel(uint unPixel);
extern int NDK_HIP_PrnSetUnderLine(EM_PRN_UNDERLINE_STATUS emStatus);
extern int NDK_HIP_PrnSetAlignment(uint unType);
extern int NDK_HIP_PrnGetPrintedLen(uint *punLen);
extern int NDK_HIP_PrnFontRegister(ST_PRN_FONTMSG *pstMsg);
extern int NDK_HIP_PrnSetUsrFont(uint unFontId);
extern int NDK_HIP_PrnGetDotLine(uint *punLine);
extern int NDK_HIP_PrnPicture(uint unXpos,const char *pszPath);
extern int NDK_HIP_PrnSetPageLen(uint len);
extern int NDK_HIP_PrnLoadBDFFont(const char *pszPath);
extern int NDK_HIP_PrnBDFStr(ushort *pusText);
extern int NDK_HIP_PrnSetBDF(uint unXpos,uint unLineSpace,uint unWordSpace,uint unXmode,uint unYmode);
extern int NDK_HIP_PrnFeedPaper(EM_PRN_FEEDPAPER emType);


extern int NDK_NO_PrnInit(uint unPrnDirSwitch);
extern int NDK_NO_PrnStr(const char *pszBuf);
extern int NDK_NO_PrnStart(void);
extern int NDK_NO_PrnImage(uint unXsize,uint unYsize,uint unXpos,const char *psImgBuf);
extern int NDK_NO_PrnGetType(EM_PRN_TYPE *pemType);
extern int NDK_NO_PrnGetVersion(char *pszVer);
extern int NDK_NO_PrnSetFont(EM_PRN_HZ_FONT emHZFont,EM_PRN_ZM_FONT emZMFont);
extern int NDK_NO_PrnGetStatus(EM_PRN_STATUS *pemStatus);
extern int NDK_NO_PrnSetMode(EM_PRN_MODE emMode,uint unSigOrDou);
extern int NDK_NO_PrnIntegrate(uint unXsize,uint unYsize,uint unXpos,uint unYpos,const char *psImgBuf,const char *pszTextBuf, uint unMode);
extern int NDK_NO_PrnSetGreyScale(uint unGrey);
extern int NDK_NO_PrnSetForm(uint unBorder,uint unColumn, uint unRow);
extern int NDK_NO_PrnFeedByPixel(uint unPixel);
extern int NDK_NO_PrnSetUnderLine(EM_PRN_UNDERLINE_STATUS emStatus);
extern int NDK_NO_PrnSetAlignment(uint unType);
extern int NDK_NO_PrnGetPrintedLen(uint *punLen);
extern int NDK_NO_PrnFontRegister(ST_PRN_FONTMSG *pstMsg);
extern int NDK_NO_PrnSetUsrFont(uint unFontId);
extern int NDK_NO_PrnGetDotLine(uint *punLine);
extern int NDK_NO_PrnPicture(uint unXpos,const char *pszPath);
extern int NDK_NO_PrnSetPageLen(uint len);
extern int NDK_NO_PrnLoadBDFFont(const char *pszPath);
extern int NDK_NO_PrnBDFStr(ushort *pusText);
extern int NDK_NO_PrnSetBDF(uint unXpos,uint unLineSpace,uint unWordSpace,uint unXmode,uint unYmode);
extern int NDK_NO_PrnFeedPaper(EM_PRN_FEEDPAPER emType);


typedef struct {
	int		(* p_ndk_prnInit)(uint);
	int		(* p_ndk_prnStr)(const char *);
	int	    (* p_ndk_prnStart)(void);
	int	    (* p_ndk_prnImage)(uint ,uint ,uint ,const char *);
	int	    (* p_ndk_prnGetType)(EM_PRN_TYPE *);
	int	    (* p_ndk_prnGetVersion)(char *);
	int	    (* p_ndk_prnSetFont)(EM_PRN_HZ_FONT ,EM_PRN_ZM_FONT);
	int     (* p_ndk_prnGetStatus)(EM_PRN_STATUS *);
	int     (* p_ndk_prnSetMode)(EM_PRN_MODE ,uint);
	int     (* p_ndk_prnIntegrate)(uint ,uint ,uint ,uint ,const char *,const char *, uint );
	int     (* p_ndk_prnSetGreyScale)(uint );
	int     (* p_ndk_prnSetForm)(uint ,uint , uint );
	int     (* p_ndk_prnFeedByPixel)(uint );
	int     (* p_ndk_prnSetUnderLine)(EM_PRN_UNDERLINE_STATUS );
	int     (* p_ndk_prnSetAlignment)(uint );
	int     (* p_ndk_prnGetPrintedLen)(uint *);
	int     (* p_ndk_prnFontRegister)(ST_PRN_FONTMSG *);
	int     (* p_ndk_prnSetUsrFont)(uint );
	int     (* p_ndk_prnGetDotLine)(uint *);
	int     (* p_ndk_prnPicture)(uint ,const char *);
	int     (* p_ndk_prnSetPageLen)(uint);
	int     (* p_ndk_prnLoadBDFFont)(const char *);
	int     (* p_ndk_prnBDFStr)(ushort *);
	int     (* p_ndk_prnSetBDF)(uint ,uint ,uint ,uint ,uint);
	int		(* p_ndk_prnFeedPaper)(EM_PRN_FEEDPAPER emType);

}ndk_prn_driver ;


const ndk_prn_driver ndk_TP_driver = {

	.p_ndk_prnInit		    	= NDK_TP_PrnInit,
	.p_ndk_prnStr           	= NDK_TP_PrnStr,
	.p_ndk_prnStart             = NDK_TP_PrnStart,
	.p_ndk_prnImage       		= NDK_TP_PrnImage,
	.p_ndk_prnGetType  			= NDK_TP_PrnGetType,
	.p_ndk_prnGetVersion        = NDK_TP_PrnGetVersion,
	.p_ndk_prnSetFont           = NDK_TP_PrnSetFont,
	.p_ndk_prnGetStatus         = NDK_TP_PrnGetStatus,
	.p_ndk_prnSetMode		    = NDK_TP_PrnSetMode,
	.p_ndk_prnIntegrate         = NDK_TP_PrnIntegrate,
	.p_ndk_prnSetGreyScale      = NDK_TP_PrnSetGreyScale,
	.p_ndk_prnSetForm       	= NDK_TP_PrnSetForm,
	.p_ndk_prnFeedByPixel  		= NDK_TP_PrnFeedByPixel,
	.p_ndk_prnSetUnderLine     	= NDK_TP_PrnSetUnderLine,
	.p_ndk_prnSetAlignment     	= NDK_TP_PrnSetAlignment,
	.p_ndk_prnGetPrintedLen     = NDK_TP_PrnGetPrintedLen,
	.p_ndk_prnFontRegister     	= NDK_TP_PrnFontRegister,
	.p_ndk_prnSetUsrFont        = NDK_TP_PrnSetUsrFont,
	.p_ndk_prnGetDotLine        = NDK_TP_PrnGetDotLine,
	.p_ndk_prnPicture           = NDK_TP_PrnPicture,
	.p_ndk_prnSetPageLen        = NDK_TP_PrnSetPageLen,
	.p_ndk_prnLoadBDFFont       = NDK_TP_PrnLoadBDFFont,
	.p_ndk_prnBDFStr            = NDK_TP_PrnBDFStr,
	.p_ndk_prnSetBDF            = NDK_TP_PrnSetBDF,
	.p_ndk_prnFeedPaper        	= NDK_TP_PrnFeedPaper
};

const ndk_prn_driver ndk_HIP_driver = {

	.p_ndk_prnInit		    	= NDK_HIP_PrnInit,
	.p_ndk_prnStr           	= NDK_HIP_PrnStr,
	.p_ndk_prnStart             = NDK_HIP_PrnStart,
	.p_ndk_prnImage       		= NDK_HIP_PrnImage,
	.p_ndk_prnGetType  			= NDK_HIP_PrnGetType,
	.p_ndk_prnGetVersion        = NDK_HIP_PrnGetVersion,
	.p_ndk_prnSetFont           = NDK_HIP_PrnSetFont,
	.p_ndk_prnGetStatus         = NDK_HIP_PrnGetStatus,
	.p_ndk_prnSetMode		    = NDK_HIP_PrnSetMode,
	.p_ndk_prnIntegrate         = NDK_HIP_PrnIntegrate,
	.p_ndk_prnSetGreyScale      = NDK_HIP_PrnSetGreyScale,
	.p_ndk_prnSetForm       	= NDK_HIP_PrnSetForm,
	.p_ndk_prnFeedByPixel  		= NDK_HIP_PrnFeedByPixel,
	.p_ndk_prnSetUnderLine     	= NDK_HIP_PrnSetUnderLine,
	.p_ndk_prnSetAlignment     	= NDK_HIP_PrnSetAlignment,
	.p_ndk_prnGetPrintedLen     = NDK_HIP_PrnGetPrintedLen,
	.p_ndk_prnFontRegister     	= NDK_HIP_PrnFontRegister,
	.p_ndk_prnSetUsrFont        = NDK_HIP_PrnSetUsrFont,
	.p_ndk_prnGetDotLine        = NDK_HIP_PrnGetDotLine,
	.p_ndk_prnPicture           = NDK_HIP_PrnPicture,
	.p_ndk_prnSetPageLen        = NDK_HIP_PrnSetPageLen,
	.p_ndk_prnLoadBDFFont       = NDK_HIP_PrnLoadBDFFont,
	.p_ndk_prnBDFStr            = NDK_HIP_PrnBDFStr,
	.p_ndk_prnSetBDF            = NDK_HIP_PrnSetBDF,
	.p_ndk_prnFeedPaper        	= NDK_HIP_PrnFeedPaper
};

const ndk_prn_driver ndk_NO_driver = {

	.p_ndk_prnInit		    	= NDK_NO_PrnInit,
	.p_ndk_prnStr           	= NDK_NO_PrnStr,
	.p_ndk_prnStart             = NDK_NO_PrnStart,
	.p_ndk_prnImage       		= NDK_NO_PrnImage,
	.p_ndk_prnGetType  			= NDK_NO_PrnGetType,
	.p_ndk_prnGetVersion        = NDK_NO_PrnGetVersion,
	.p_ndk_prnSetFont           = NDK_NO_PrnSetFont,
	.p_ndk_prnGetStatus         = NDK_NO_PrnGetStatus,
	.p_ndk_prnSetMode		    = NDK_NO_PrnSetMode,
	.p_ndk_prnIntegrate         = NDK_NO_PrnIntegrate,
	.p_ndk_prnSetGreyScale      = NDK_NO_PrnSetGreyScale,
	.p_ndk_prnSetForm       	= NDK_NO_PrnSetForm,
	.p_ndk_prnFeedByPixel  		= NDK_NO_PrnFeedByPixel,
	.p_ndk_prnSetUnderLine     	= NDK_NO_PrnSetUnderLine,
	.p_ndk_prnSetAlignment     	= NDK_NO_PrnSetAlignment,
	.p_ndk_prnGetPrintedLen     = NDK_NO_PrnGetPrintedLen,
	.p_ndk_prnFontRegister     	= NDK_NO_PrnFontRegister,
	.p_ndk_prnSetUsrFont        = NDK_NO_PrnSetUsrFont,
	.p_ndk_prnGetDotLine        = NDK_NO_PrnGetDotLine,
	.p_ndk_prnPicture           = NDK_NO_PrnPicture,
	.p_ndk_prnSetPageLen        = NDK_NO_PrnSetPageLen,
	.p_ndk_prnLoadBDFFont       = NDK_NO_PrnLoadBDFFont,
	.p_ndk_prnBDFStr            = NDK_NO_PrnBDFStr,
	.p_ndk_prnSetBDF            = NDK_NO_PrnSetBDF,
	.p_ndk_prnFeedPaper         = NDK_NO_PrnFeedPaper
};

ndk_prn_driver *p_ndk_PrnDriver = NULL;

#endif
