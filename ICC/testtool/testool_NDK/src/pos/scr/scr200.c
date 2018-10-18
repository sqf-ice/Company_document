/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 显示模块 
* file name			: 
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 显示模块内随机组合测试
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI	"scr模块内随机组合"
#define BIGPNG  "big.png"
#define SHUTDOWNPIC "shutdown.jpg"
#define FILENAME3 "arial.ttf"

#define     HZPATHCP    "hz24x24"
#define     YWPATHCP    "yw12x24"
#define 	BDFFILE		 "10x20.bdf"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
	ScrGotoxy=0 ,			 
	ScrGetxy,
	ScrSetAttr,	
	ScrClrs,
	ScrClrLine,
	ScrDrawBitmap,
	ScrLine,
	ScrRectangle,
	ScrPutPixel,
	ScrGetPixel,
	ScrGetLcdSize,
	ScrBackLight,
	ScrSetContrast,
	ScrGetFontSize,
	ScrDispString,
	ScrPrintf,
#if !K21_ENABLE
	ScrGetColorDepth,
	ScrSetFontType,
	ScrGetFontType,
	ScrPush,
	ScrPop,
	ScrSetbgPic,
	ScrClrbgPic,
	ScrSetViewPort,
	ScrGetViewPort,
	ScrAutoUpdate,
	ScrFullRefresh,
	ScrRefresh,
	ScrSetSpace,
	ScrSetFontColor,
	ScrDispPic,
	ScrFontAdd,
	ScrStatusbar,
	ScrLoadBDF,
	ScrDispBDFText,
	ScrTSCalibrate,
	ScrImgDecode,
	ScrImgDestroy,
	ScrDispImg,
#endif
#if OVERSEAS_ENABLE&& (!defined SP630PG)
	ScrInitTrueTypeFont,
	ScrDestroyTrueTypeFont,
#if !defined ME51
	SrcSetTrueTypeFontSizeInPixel,
#endif
	SrcGetTrueTypeFontSize,
	ScrSetTrueTypeFontEncodingFormat,
	ScrDispTrueTypeFontText,
#endif
	MaxNum,//新增枚举值时在此值前面加,要保证此值为最后一个值
}EM_SCR;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: 
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20170914   	created
*****************************************************************/
static int scr_randtest(int funnum)
{
	int ret = -1;
	uint PosX = 0, PosY = 0,getx = 0, gety = 0, ViewWidth = 0, ViewHeight = 0,  punX =0, punY = 0;
	uint x0 = 0, y0 = 0, width = 0, height = 0;
	color_t punColor = BLACK;
	EM_TEXT_ATTRIBUTE OldAttr = 0;
	const char pszS1[] = "NEWLAND\r\nPOS";	
#if !K21_ENABLE
	char *imgbuf1 = NULL;
	ushort text[] = {0x0023, 0x0033, 0x0043, 0x0066,0x0000};//对应BDF字符#3Cf
	EM_DISPFONT fonttype = 0;
	uint puncd = 0;
	int pnOldauto = 0;
#endif
#if OVERSEAS_ENABLE
	char uni_arial_str[]={0x33, 0x06, 0x44, 0x06, 0x27, 0x06, 0x45, 0x06, 0x0C, 0x06, 0x20, 0x00, 0x86, 0x06,
		                  0xCC, 0x06, 0x46, 0x06};  //波斯语的:你好，中国
	char asc_str[]="Innovation in China 0123456789!";
	int getwidth =0, getheight = 0;
#endif		     

	NDK_ScrGetViewPort(&punX,&punY, &ViewWidth, &ViewHeight);
	switch(funnum)
	{	
		case ScrGotoxy:  
			PosX = rand()%(ViewWidth-(GUI_GetFontWidth()/2));
			PosY = rand()%(ViewHeight-GUI_GetFontHeight());
			NDK_ScrGotoxy(PosX, PosY);
			break;
		case ScrGetxy:  
			if((ret=NDK_ScrGetxy(&getx, &gety))!=NDK_OK)
				return ret;
			break;
		case ScrSetAttr:
			if((ret=NDK_ScrSetAttr(TEXT_ATTRIBUTE_NORMAL,&OldAttr))!=NDK_OK)
				return ret;
			break;
		case ScrClrs:  
			if((ret=NDK_ScrClrs())!=NDK_OK)
				return ret;
			break;
		case ScrClrLine:  
			NDK_ScrClrLine(0,rand()%10);
			break;
		case ScrDrawBitmap:  
			//低端产品屏幕不同 不判断返回值
			NDK_ScrDrawBitmap(0,0,64,128,NL_BMP);
			break;	
		case ScrLine:
			NDK_ScrLine(0,0,ViewWidth-1,ViewHeight-1,BLACK);
			break;
		case ScrRectangle:	
			x0 = rand()%(ViewWidth-1);
			y0 = rand()%(ViewHeight-1);
			width = rand()%(ViewWidth-x0);
			height = rand()%(ViewHeight-y0);		
			NDK_ScrRectangle(x0, y0, width, height, rand()%2, BLACK);
			break;
		case ScrPutPixel:	
			x0 = rand()%ViewWidth;
			y0= rand()%ViewHeight;
			NDK_ScrPutPixel(x0, y0, BLACK);
			break;
		case ScrGetPixel:
			x0 = rand()%ViewWidth;
			y0= rand()%ViewHeight;
			NDK_ScrGetPixel(x0, y0, &punColor);
		case ScrGetLcdSize:
			if((ret=NDK_ScrGetLcdSize(&width,&height))!=NDK_OK)
				return ret;
			break;
		case ScrBackLight:	
           	 	if((ret=NDK_ScrBackLight(rand()%2))!=NDK_OK)
				return ret;
			break;
		case ScrSetContrast:
			//if((ret=NDK_ScrSetContrast(rand()%64))!=NDK_OK)  
				//return ret;
			NDK_ScrSetContrast(rand()%64);
			break;
		case ScrGetFontSize:
			if((ret=NDK_ScrGetFontSize(&width,&height))!=NDK_OK)
				return ret;
			break;
		case ScrDispString:
			NDK_ScrDispString(x0,y0,pszS1,0);
			break;
		case ScrPrintf:
			NDK_ScrPrintf("国国国国国国国国");
			break;
#if !K21_ENABLE
		case ScrGetColorDepth:
			if((ret=NDK_ScrGetColorDepth(&puncd))!=NDK_OK)
				return ret;
			break;
		case ScrSetFontType:
			if((ret=NDK_ScrSetFontType(rand()%3))!=NDK_OK)//恢复默认DISPFONT_CUSTOM
				return ret;
			break;
		case ScrGetFontType:
			if((ret=NDK_ScrGetFontType(&fonttype))!=NDK_OK) 
				return ret;
			break;
		case ScrPush:
			if((ret=NDK_ScrPush())!=NDK_OK)
				return ret;
			break;
		case ScrPop:
			NDK_ScrPop();
			break;
		case ScrSetbgPic:
			if((ret=NDK_ScrSetbgPic(BIGPNG))!=NDK_OK)
				return ret;
			break;
		case ScrClrbgPic:	
			if((ret=NDK_ScrClrbgPic())!=NDK_OK)
				return ret;
			break;
		case ScrSetViewPort:
			NDK_ScrSetViewPort( 15+punX,15+punY,ViewWidth-30,ViewHeight-30) ;
			break;
		case ScrGetViewPort:
			if((ret=NDK_ScrGetViewPort(&x0,&y0,&width,&height))!=NDK_OK)
				return ret;
			break;
		case ScrAutoUpdate:
			if((ret=NDK_ScrAutoUpdate(rand()%2,&pnOldauto))!=NDK_OK) //默认值
				return ret;
			break;
		case ScrFullRefresh:	
			if ((ret=NDK_ScrFullRefresh())!=NDK_OK)
				return ret;
			break;
		case ScrRefresh:
			if ((ret=NDK_ScrRefresh())!=NDK_OK)
				return ret;
			break;
		case ScrSetSpace:
			if((ret=NDK_ScrSetSpace(GUI_GetFontWidth(),GUI_GetFontHeight()))!=NDK_OK)
				return ret;
			break;
		case ScrSetFontColor:
			if((ret=NDK_ScrSetFontColor(BLACK,rand()%3))!=NDK_OK)
				return ret;
			break;
		case ScrDispPic:
			if((ret=NDK_ScrDispPic(0,0,ViewWidth,ViewHeight,SHUTDOWNPIC))!=NDK_OK)
				return ret;
			break;
		case ScrFontAdd:
			if((ret=NDK_ScrFontAdd(HZPATHCP,YWPATHCP))!=NDK_OK)
				return ret;
			break;
		case ScrStatusbar:
			if((ret=NDK_ScrStatusbar(rand()%2==0?(STATUSBAR_DISP_ALL|STATUSBAR_POSITION_TOP):(STATUSBAR_DISP_CLOSE|STATUSBAR_POSITION_BOTTOM)))!=NDK_OK) //退出关闭
				return ret; 
			break;
		case ScrLoadBDF:
			if((ret=NDK_ScrLoadBDF(0,BDFFILE,&width,&height))!=NDK_OK)
				return ret;
			break;
		case ScrDispBDFText:
			NDK_ScrDispBDFText(0,0,0,text);
			break;
		case ScrTSCalibrate:
			NDK_ScrTSCalibrate();
			break;
		case ScrImgDecode:
			if((ret=NDK_ScrImgDecode(SHUTDOWNPIC,&imgbuf1))!=NDK_OK)
				return ret;
			break;
		case ScrImgDestroy:
			NDK_ScrImgDestroy(imgbuf1);
			break;
		case ScrDispImg:
			NDK_ScrDispImg(0,0,GUI_GetLCDWidth(),GUI_GetLCDHeight(),imgbuf1);
			break;
#endif
#if OVERSEAS_ENABLE &&(!defined SP630PG)
		case ScrInitTrueTypeFont:
			if((ret=NDK_ScrInitTrueTypeFont(FILENAME3, 0))!=NDK_OK)
				return ret;
			break;
		case ScrDestroyTrueTypeFont:
			NDK_ScrDestroyTrueTypeFont();
			break;
#if !defined ME51
		case SrcSetTrueTypeFontSizeInPixel:
			NDK_ScrSetTrueTypeFontSizeInPixel(12,12);
			break;
#endif
		case SrcGetTrueTypeFontSize:
			NDK_SrcGetTrueTypeFontSize(asc_str, strlen(asc_str), &getwidth, &getheight);
			break;
		case ScrSetTrueTypeFontEncodingFormat:
			NDK_ScrSetTrueTypeFontEncodingFormat(DISP_TTF_INPUT_UNICODE);
			break;
		case ScrDispTrueTypeFontText:
			NDK_ScrDispTrueTypeFontText(0, 0, uni_arial_str, sizeof(uni_arial_str));
			break;
#endif
	}	
	return SUCC;
}

void scr200(void)
{
	/*private & local definition*/
	int  i = 0, bak = 0, num[50] ={0}, j = 0; 
	int ret = -1,cnt =50, len = 0, pnOldauto = 0, pnOldauto_set = 0; ;//cnt 待定
	uint punX = 0, punY = 0, punWidth = 0, punHeight = 0;
#if K21_ENABLE
	char *scrstr[] = {"NDK_ScrGotoxy","NDK_ScrGetxy","NDK_ScrSetAttr","NDK_ScrClrs","NDK_ScrClrLine","NDK_ScrDrawBitmap","NDK_ScrLine","NDK_ScrRectangle","NDK_ScrPutPixel","NDK_ScrGetPixel","NDK_ScrGetLcdSize","NDK_ScrBackLight","NDK_ScrSetContrast","NDK_ScrGetFontSize","NDK_ScrDispString","NDK_ScrPrintf"};
#elif OVERSEAS_ENABLE&& !defined SP630PG&& ! defined ME51
	char *scrstr[] = {"NDK_ScrGotoxy","NDK_ScrGetxy","NDK_ScrSetAttr","NDK_ScrClrs","NDK_ScrClrLine","NDK_ScrDrawBitmap","NDK_ScrLine","NDK_ScrRectangle","NDK_ScrPutPixel","NDK_ScrGetPixel","NDK_ScrGetLcdSize","NDK_ScrBackLight","NDK_ScrSetContrast","NDK_ScrGetFontSize","NDK_ScrDispString","NDK_ScrPrintf",
				"NDK_ScrGetColorDepth","NDK_ScrSetFontType","NDK_ScrGetFontType","NDK_ScrPush","NDK_ScrPop","NDK_ScrSetbgPic","NDK_ScrClrbgPic","NDK_ScrSetViewPort","NDK_ScrGetViewPort","NDK_ScrAutoUpdate","NDK_ScrFullRefresh","NDK_ScrRefresh","NDK_ScrSetSpace","NDK_ScrSetFontColor","NDK_ScrDispPic",
				"NDK_ScrFontAdd","NDK_ScrStatusbar","NDK_ScrLoadBDF","NDK_ScrDispBDFText","NDK_ScrTSCalibrate","NDK_ScrImgDecode","NDK_ScrImgDestroy","NDK_ScrDispImg", "NDK_ScrInitTrueTypeFont", "NDK_ScrDestroyTrueTypeFont", "NDK_SrcSetTrueTypeFontSizeInPixel", "NDK_SrcGetTrueTypeFontSize",
				"NDK_ScrSetTrueTypeFontEncodingFormat", "NDK_ScrDispTrueTypeFontText"};
#elif defined ME51
char *scrstr[] = {"NDK_ScrGotoxy","NDK_ScrGetxy","NDK_ScrSetAttr","NDK_ScrClrs","NDK_ScrClrLine","NDK_ScrDrawBitmap","NDK_ScrLine","NDK_ScrRectangle","NDK_ScrPutPixel","NDK_ScrGetPixel","NDK_ScrGetLcdSize","NDK_ScrBackLight","NDK_ScrSetContrast","NDK_ScrGetFontSize","NDK_ScrDispString","NDK_ScrPrintf",
			"NDK_ScrGetColorDepth","NDK_ScrSetFontType","NDK_ScrGetFontType","NDK_ScrPush","NDK_ScrPop","NDK_ScrSetbgPic","NDK_ScrClrbgPic","NDK_ScrSetViewPort","NDK_ScrGetViewPort","NDK_ScrAutoUpdate","NDK_ScrFullRefresh","NDK_ScrRefresh","NDK_ScrSetSpace","NDK_ScrSetFontColor","NDK_ScrDispPic",
			"NDK_ScrFontAdd","NDK_ScrStatusbar","NDK_ScrLoadBDF","NDK_ScrDispBDFText","NDK_ScrTSCalibrate","NDK_ScrImgDecode","NDK_ScrImgDestroy","NDK_ScrDispImg", "NDK_ScrInitTrueTypeFont", "NDK_ScrDestroyTrueTypeFont", "NDK_SrcGetTrueTypeFontSize",
			"NDK_ScrSetTrueTypeFontEncodingFormat", "NDK_ScrDispTrueTypeFontText"};
#elif defined SP630PG
char *scrstr[] = {"NDK_ScrGotoxy","NDK_ScrGetxy","NDK_ScrSetAttr","NDK_ScrClrs","NDK_ScrClrLine","NDK_ScrDrawBitmap","NDK_ScrLine","NDK_ScrRectangle","NDK_ScrPutPixel","NDK_ScrGetPixel","NDK_ScrGetLcdSize","NDK_ScrBackLight","NDK_ScrSetContrast","NDK_ScrGetFontSize","NDK_ScrDispString","NDK_ScrPrintf",
			"NDK_ScrGetColorDepth","NDK_ScrSetFontType","NDK_ScrGetFontType","NDK_ScrPush","NDK_ScrPop","NDK_ScrSetbgPic","NDK_ScrClrbgPic","NDK_ScrSetViewPort","NDK_ScrGetViewPort","NDK_ScrAutoUpdate","NDK_ScrFullRefresh","NDK_ScrRefresh","NDK_ScrSetSpace","NDK_ScrSetFontColor","NDK_ScrDispPic",
			"NDK_ScrFontAdd","NDK_ScrStatusbar","NDK_ScrLoadBDF","NDK_ScrDispBDFText","NDK_ScrTSCalibrate","NDK_ScrImgDecode","NDK_ScrImgDestroy","NDK_ScrDispImg"};
#else
	char *scrstr[] = {"NDK_ScrGotoxy","NDK_ScrGetxy","NDK_ScrSetAttr","NDK_ScrClrs","NDK_ScrClrLine","NDK_ScrDrawBitmap","NDK_ScrLine","NDK_ScrRectangle","NDK_ScrPutPixel","NDK_ScrGetPixel","NDK_ScrGetLcdSize","NDK_ScrBackLight","NDK_ScrSetContrast","NDK_ScrGetFontSize","NDK_ScrDispString","NDK_ScrPrintf",
				"NDK_ScrGetColorDepth","NDK_ScrSetFontType","NDK_ScrGetFontType","NDK_ScrPush","NDK_ScrPop","NDK_ScrSetbgPic","NDK_ScrClrbgPic","NDK_ScrSetViewPort","NDK_ScrGetViewPort","NDK_ScrAutoUpdate","NDK_ScrFullRefresh","NDK_ScrRefresh","NDK_ScrSetSpace","NDK_ScrSetFontColor","NDK_ScrDispPic",
				"NDK_ScrFontAdd","NDK_ScrStatusbar","NDK_ScrLoadBDF","NDK_ScrDispBDFText","NDK_ScrTSCalibrate","NDK_ScrImgDecode","NDK_ScrImgDestroy","NDK_ScrDispImg"};
#endif		

	/*process body*///确保下载图片 字库等
	//测试前置:
#if STABAR_ENABLE
	NDK_ScrStatusbar(STATUSBAR_DISP_CLOSE);//状态栏开启会影响最后的退出界面故入口直接关闭 
#endif
	if(cls_show_msg1(30,"请确认已经在测试程序路径下载了%s,%s,%s,%s,%s,%s,按任意键继续,ESC键退出",BIGPNG,SHUTDOWNPIC,HZPATHCP,YWPATHCP,BDFFILE,FILENAME3)==ESC)
		return;	
	NDK_ScrGetViewPort(&punX,&punY,&punWidth,&punHeight); //获取屏幕的大小
	NDK_ScrAutoUpdate(0,&pnOldauto_set);
		
	len = (MaxNum>20)?MaxNum:20;
	while(cnt)
	{	
		cnt--;
		bak++;
		
		//产生随机数
		cls_printf("第%d次随机顺序:",bak);
		srand((unsigned)time(NULL));
		for(i=0;i<len;i++)
		{	
			num[i] = rand()%MaxNum;
			NDK_ScrPrintf("%d ", num[i]);
			NDK_ScrRefresh();
		}
		
		//进行随机测试
		for(i=0;i<len;i++)
		{
			if((ret = scr_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%s)", __LINE__, TESTAPI, bak,ret,scrstr[num[i]]);
				for(j=0;j<=i;j++)
					send_result("%s ",scrstr[num[j]]);
				goto ERR;
			}
		}	
	}
	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_ScrSetViewPort(punX,punY,punWidth,punHeight);
	NDK_ScrAutoUpdate(pnOldauto_set,&pnOldauto);
#if !(defined ME30||defined ME30S||defined ME30THM||defined ME30MH||defined ME30GX||defined ME32THM||defined ME31||defined SP10)
	NDK_ScrSetContrast(32);//默认值为32
#else
	NDK_ScrSetContrast(19);
#endif
#if OVERSEAS_ENABLE &&(!defined SP630PG)
	NDK_ScrSetFontColor(WHITE,FONTCOLOR_REVERSE);
	NDK_ScrSetFontColor(BLACK,FONTCOLOR_BG_REVERSE);
	NDK_ScrSetTrueTypeFontEncodingFormat(DISP_TTF_INPUT_ASCII);
	NDK_ScrDestroyTrueTypeFont();
#endif
	NDK_ScrSetFontType(DISPFONT_CUSTOM);
#if !K21_ENABLE
	NDK_ScrSetSpace(0,0);
#endif
	NDK_ScrBackLight(BACKLIGHT_ON); //打开液晶背光
	return;
}

