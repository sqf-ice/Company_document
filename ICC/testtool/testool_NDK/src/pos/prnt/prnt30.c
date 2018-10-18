/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: ´òÓ¡Ä£¿é
* file name		: prnt30.c
* Author 		:  
* version		: 
* DATE			:  
* directory 		: 
* description		: ĞÂNDK_ttf½Å±¾´òÓ¡½Ó¿Ú²âÊÔ
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"ttf_script_print"	
#define MAXWAITTIME 90
#define DESTFILE "/appfs/ums"
#define SOURCEFILE "/mnt/sdcard/picture/ums"  
#define PNGFORMAT ".png"
#define IMAGEPATH "/mnt/sdcard/picture/"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 		:void prnt30(void)
* functional description 	: 
* input parameter	 		:ÎŞ
* output parameter	 	:ÎŞ
* return value		 	:ÎŞ
* history 		 		:  author		date		remarks
*			  	  	  	    sull           20180723         created
*****************************************************************/
//ĞÂÔöµÄttf´òÓ¡½Ó¿ÚÖ»ÅĞ¶ÏÃüÁîÕıÈ·Óë·ñ£¬²»ÅĞ¶Ï´òÓ¡»ú×´Ì¬Öµ,ĞèÒª×Ô¼ºÈ¥µ÷ÓÃstatusº¯ÊıÅĞ¶Ï
static int getStatus(void)
{
	EM_PRN_STATUS emStatus = 0;
	int ret = 0;

	while(1)
	{
		if((ret=NDK_PrnGetStatus(&emStatus))!=NDK_OK )
		{
			send_result("line %d:%s»ñÈ¡´òÓ¡×´Ì¬Ê§°Ü(%d)",__LINE__,TESTAPI,ret);
			return ret;
		}
		switch(emStatus)
		{
			case PRN_STATUS_BUSY:
				continue;
				break;
			case PRN_STATUS_OK:
				return PRN_STATUS_OK;
				break;
			case PRN_STATUS_NOPAPER:
				return PRN_STATUS_NOPAPER;
				break;
			default:
				send_result("line %d:%s´òÓ¡»ú×´Ì¬Òì³£(%d)", __LINE__, TESTAPI, emStatus);
				return emStatus;
		}
	}	
}
void prnt30(void)
{
	//ÒÔÏÂÎª¾Ö²¿±äÁ¿
#include	"dotfont.h"
#include	"hzfont.h"

	EM_PRN_STATUS emStatus = 0;
	int nBusy = FALSE, nKeyin = 0, i = 0;
	time_t nOldtime = 0, nDiff = 0;
	int nRet = 0, nRet1 = 0;
	char *type[] = {"l", "c", "r", "l"};
	//char *position[] = {"¾Ó×ó", "¾ÓÖĞ", "¾ÓÓÒ", "¾Ó×ó"};
	char *position[] = {"å±…å·¦", "å±…ä¸­", "å±…å³", "å±…å·¦"};
	int p = 0;
	#if 0  //ttf_strµÄGB2312¸ñÊ½£¬·½±ãÎÊÌâ¶¨Î»
	char *ttf_str[] = {
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªÏµÍ³Ä¬ÈÏÓ¢ÎÄ×ÖÌå\n!font DroidSans.ttf\n!NLFONT 9 12 3\n*text l DroidSansDroidSans\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªÏµÍ³Ä¬ÈÏÓ¢ÎÄ´Ö×ÖÌå\n!font DroidSans-Bold.ttf\n!NLFONT 9 12 3\n*text l DroidSansDroidSans\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªÏµÍ³Ä¬ÈÏÖĞÓ¢ÎÄ×ÖÌå\n!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ĞÂ´óÂ½Ö§¸¶¼¼ÊõÓĞÏŞ¹«Ë¾newlandpatment\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªDroidaSansMono×ÖÌå\n!font DroidSansMono.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªDancingScript´Ö×ÖÌå\n!font DancingScript-Bold.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªDancingScript³£¹æ×ÖÌå\n!font DancingScript-Regular.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªRoboto³£¹æ£¨Ó¢ÎÄ£©\n!font Roboto-Regular.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªRoboto´ÖÌå£¨Ó¢ÎÄ£©\n!font Roboto-Bold.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªRoboto´ÖĞ±Ìå£¨Ó¢ÎÄ£©\n!font Roboto-BoldItalic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",						
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªRobotoRobotoĞ±Ìå£¨Ó¢ÎÄ£©\n!font Roboto-Italic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªRobotoÏ¸×ÖÌå£¨Ó¢ÎÄ£©\n!font Roboto-Thin.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªRobotoÏ¸Ğ±Ìå£¨Ó¢ÎÄ£©\n!font Roboto-ThinItalic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªRobotoCondensed³£¹æ£¨Ó¢ÎÄ£©\n!font RobotoCondensed-Regular.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªRobotoCondensed´ÖÌå£¨Ó¢ÎÄ£©\n!font RobotoCondensed-Bold.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªRobotoCondensed´ÖĞ±Ìå£¨Ó¢ÎÄ£©\n!font RobotoCondensed-BoldItalic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªRobotoCondensedÏ¸Ìå£¨Ó¢ÎÄ£©\n!font RobotoCondensed-Light.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªRRobotoCondensedÏ¸Ğ±Ìå£¨Ó¢ÎÄ£©\n!font RobotoCondensed-LightItalic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªNotoSerif´ÖĞ±Ìå£¨Ó¢ÎÄ£©\n!font NotoSerif-BoldItalic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªNotoSerif´ÖÌå£¨Ó¢ÎÄ£©\n!font NotoSerif-Bold.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªNotoSerifĞ±Ìå£¨Ó¢ÎÄ£©\n!font NotoSerif-Italic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ÒÔÏÂÉèÖÃÎªNotoSerif³£¹æ£¨Ó¢ÎÄ£©\n!font NotoSerif-Regular.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l »Ö¸´Ä¬ÈÏ×ÖÌå\n*line\n*feedline 1\n",
					};
	#endif
	//TTF´òÓ¡½Å±¾ÃüÁîÖ»Ö§³ÖUTF-8¸ñÊ½£¬¶øsourceingsightÄ¬ÈÏGB2312¸ñÊ½£¬¹Ê´ËÎÄ¼şµÄÃüÁîÔÚsourceinsightÉÏÏÔÊ¾ÎªÂÒÂë£¬¶¨Î»ÎÊÌâÊ±¿É²é¿´¶ÔÓ¦µÄGB2312Âë
	char *ttf_str[] = {
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºç³»ç»Ÿé»˜è®¤è‹±æ–‡å­—ä½“\n!font DroidSans.ttf\n!NLFONT 9 12 3\n*text l DroidSansDroidSans\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºç³»ç»Ÿé»˜è®¤è‹±æ–‡ç²—å­—ä½“\n!font DroidSans-Bold.ttf\n!NLFONT 9 12 3\n*text l DroidSansDroidSans\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºç³»ç»Ÿé»˜è®¤ä¸­è‹±æ–‡å­—ä½“\n!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l æ–°å¤§é™†æ”¯ä»˜æŠ€æœ¯æœ‰é™å…¬å¸newlandpatment\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºDroidaSansMonoå­—ä½“\n!font DroidSansMono.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºDancingScriptç²—å­—ä½“\n!font DancingScript-Bold.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºDancingScriptå¸¸è§„å­—ä½“\n!font DancingScript-Regular.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºRobotoå¸¸è§„ï¼ˆè‹±æ–‡ï¼‰\n!font Roboto-Regular.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºRobotoç²—ä½“ï¼ˆè‹±æ–‡ï¼‰\n!font Roboto-Bold.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºRobotoç²—æ–œä½“ï¼ˆè‹±æ–‡ï¼‰\n!font Roboto-BoldItalic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",						
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºRobotoRobotoæ–œä½“ï¼ˆè‹±æ–‡ï¼‰\n!font Roboto-Italic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºRobotoç»†å­—ä½“ï¼ˆè‹±æ–‡ï¼‰\n!font Roboto-Thin.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºRobotoç»†æ–œä½“ï¼ˆè‹±æ–‡ï¼‰\n!font Roboto-ThinItalic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºRobotoCondensedå¸¸è§„ï¼ˆè‹±æ–‡ï¼‰\n!font RobotoCondensed-Regular.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºRobotoCondensedç²—ä½“ï¼ˆè‹±æ–‡ï¼‰\n!font RobotoCondensed-Bold.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºRobotoCondensedç²—æ–œä½“ï¼ˆè‹±æ–‡ï¼‰\n!font RobotoCondensed-BoldItalic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºRobotoCondensedç»†ä½“ï¼ˆè‹±æ–‡ï¼‰\n!font RobotoCondensed-Light.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºRRobotoCondensedç»†æ–œä½“ï¼ˆè‹±æ–‡ï¼‰\n!font RobotoCondensed-LightItalic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºNotoSerifç²—æ–œä½“ï¼ˆè‹±æ–‡ï¼‰\n!font NotoSerif-BoldItalic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºNotoSerifç²—ä½“ï¼ˆè‹±æ–‡ï¼‰\n!font NotoSerif-Bold.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºNotoSerifæ–œä½“ï¼ˆè‹±æ–‡ï¼‰\n!font NotoSerif-Italic.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l ä»¥ä¸‹è®¾ç½®ä¸ºNotoSerifå¸¸è§„ï¼ˆè‹±æ–‡ï¼‰\n!font NotoSerif-Regular.ttf\n!NLFONT 9 12 3\n*text l newlandpatmentcompany\n*line\n*feedline 1\n",
						"!font DroidSansFallback.ttf\n!NLFONT 9 12 3\n*text l æ¢å¤é»˜è®¤å­—ä½“\n*line\n*feedline 1\n",
					};
	char *pngbuf[] = {"ysz1", "ysz2", "ysz3", "ysz4", "ysz5", "ysz6", "ysz7", "color1", "color2", "color3", "color4", "color5", "color6",
					"IHDR1", "IHDR2", "IHDR3", "IHDR4", "IHDR5", "IHDR6", "IHDR7", "IHDR8", "IHDR9", "IHDR10", "IHDR11", "IHDR12", "IHDR13",
					"IHDR14", "IHDR15", "other1", "other2", "other3", "other4", "other5", "other6", "other7", "other8", "other9", "other10",
					"other11", "other12", "other13", "other14", "other15", "other16"};
	char *whbuf[] = {"284*52", "268*100", "292*52", "309*72", "211*93", "309*72", "302*112", "384*300", "384*300", "384*300", "384*300", "384*300", "384*300", "280*280", "310*136", "310*136", "315*164", 
					 "384*75", "384*241", "195*384", "384*144", "294*292", "300*191", "100*54", "300*200", "384*192", "315*164", "220*183", "315*164", "384*149", "327*87", "315*164", "220*165", "240*240",
					 "250*250", "99*99", "384*49", "328*112", "310*136", "205*86", "361*361", "361*361", "384*259", "384*49"};  //¸÷pngÍ¼Æ¬¶ÔÓ¦µÄ¿íºÍ¸ß
	#if 0 //buf_strµÄGB2312¸ñÊ½£¬·½±ãÎÊÌâ¶¨Î»
	char *buf_str[] = {
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È120¾ÓÓÒ)ÇëÉ¨Ãè´òÓ¡³öµÄÌõĞÎÂëÄÚÈİÊÇ·ñÎª:0123456789ABC\n!barcode 2 120\n*barcode r 0123456789ABC\n*line\n*feedline 1\n",//ĞÂÔöÓÃÀı²âÊÔµÚÒ»´Îµ÷ÓÃ½Å±¾´òÓ¡Ö±½Ó´òÓ¡ÌõÂë,ÖĞ¼ä²»Ó¦¸ÃÓĞ¿Õ°×ĞĞ
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È8¸ß¶È384¾ÓÓÒ)ÇëÉ¨Ãè´òÓ¡³öµÄÌõĞÎÂëÄÚÈİÊÇ·ñÎª:0123456789ABC\n!barcode 8 384\n*barcode r 0123456789ABC\n*line\n*feedline 1\n",//²âÊÔÌõÂë¿í¶È³¬¹ı384,·µ»ØÊ§°Ü
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È384¾ÓÓÒ)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:ABC123456789DEFGH\n!qrcode 384 3\n*qrcode r ABC123456789DEFGH\n*line\n*feedline 1\n",//ÓÃÀı²âÊÔ¶şÎ¬Âë¿í¶È³¬¹ı384,·µ»ØÊ§°Ü
						//ttfÌØÓĞ:Ê¹ÓÃWellnerAdaptiveThresholdãĞÖµËã·¨ yzÎª0
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡PNGÍ¼Æ¬£¬ãĞÖµÊ¹ÓÃWellnerAdaptiveThresholdËã·¨(¾Ó×ó)£¬Ğ§¹ûÇë¶ÔÕÕÊµ¼ÊÍ¼Æ¬£¬²ÊÉ«Í¼Æ¬Ç³ÑÕÉ«½«´òÓ¡³É°×É«£¬ÉîÑÕÉ«½«´òÓ¡³ÉºÚÉ«\n*image l 384*300 path:yz:0;/mnt/sdcard/picture/color1.png\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡PNGÍ¼Æ¬£¬ãĞÖµÊ¹ÓÃWellnerAdaptiveThresholdËã·¨(¾ÓÖĞ)£¬Ğ§¹ûÇë¶ÔÕÕÊµ¼ÊÍ¼Æ¬£¬²ÊÉ«Í¼Æ¬Ç³ÑÕÉ«½«´òÓ¡³É°×É«£¬ÉîÑÕÉ«½«´òÓ¡³ÉºÚÉ«\n*image c 394*300 path:yz:0;/mnt/sdcard/picture/color2.png\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡PNGÍ¼Æ¬£¬ãĞÖµÊ¹ÓÃWellnerAdaptiveThresholdËã·¨(¾ÓÓÒ)£¬Ğ§¹ûÇë¶ÔÕÕÊµ¼ÊÍ¼Æ¬£¬²ÊÉ«Í¼Æ¬Ç³ÑÕÉ«½«´òÓ¡³É°×É«£¬ÉîÑÕÉ«½«´òÓ¡³ÉºÚÉ«\n*image r 384*300 path:yz:0;/mnt/sdcard/picture/color3.png\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡PNGÍ¼Æ¬,ãĞÖµ255´òÓ¡ºÚ¿é(¾ÓÓÒ)\n*image r 384*240 path:yz:255;/mnt/sdcard/picture/png03.png\n*line\n*feedline 1\n",  //ĞÂÔöÓÃÀı²âÊÔpngÍ¼Æ¬¹¦ÄÜÊ¹ÓÃpng02ºÍpng03Ìæ»»Ô­À´µÄlogoÍ¼Æ¬
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡PNGÍ¼Æ¬,ãĞÖµÄ¬ÈÏ(¾ÓÖĞ),×¢Òâ´òÓ¡Ğ§¹û\n*image c 384*49 path:/mnt/sdcard/picture/png08.png\n*line\n*feedline 1\n",//ĞÂÔö´òÓ¡Í¼Æ¬
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Ñ¹ËõPNGÍ¼Æ¬,ãĞÖµ×Ô¶¯£¬(100*200£¬¾Ó×ó)£¬²»±Ø¹Ø×¢´òÓ¡Ğ§¹û\n*image l 100*200 path:/mnt/sdcard/picture/png02.png\n*line\n*feedline 1\n", 
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Ñ¹ËõPNGÍ¼Æ¬,ãĞÖµ×Ô¶¯£¬(100*200£¬¾ÓÖĞ)£¬²»±Ø¹Ø×¢´òÓ¡Ğ§¹û\n*image c 100*200 path:/mnt/sdcard/picture/png03.png\n*line\n*feedline 1\n", 
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Ñ¹ËõPNGÍ¼Æ¬,ãĞÖµ×Ô¶¯£¬(100*200£¬¾ÓÓÒ)£¬²»±Ø¹Ø×¢´òÓ¡Ğ§¹û\n*image r 100*200 path:/mnt/sdcard/picture/png04.png\n*line\n*feedline 1\n", //ĞÂÔö´òÓ¡Ñ¹ËõµÄPNGÍ¼Æ¬
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Ñ¹ËõPNGÍ¼Æ¬,WellnerAdaptiveThresholdËã·¨ãĞÖµ×Ô¶¯,(384*144,¾ÓÓÒ),²»±Ø¹Ø×¢´òÓ¡Ğ§¹û\n*image r 384*144 path:yz:0;/mnt/sdcard/picture/IHDR8.png\n*feedline 1\n",
						//ĞÂÔö¶şÎ¬ÂëÖ§³Ö400³¤¶È
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È100¾ÓÖĞ£¬ÄÚÈİ400)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼\
						¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼\
						¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼\n!qrcode 100 2\n*qrcode c ¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼\
						¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼¸üĞÂ¼ÇÂ¼\n*line\n*feedline 1\n",
						//´òÓ¡ºº×Ö:hzÃüÁî¡¢underlineÃüÁî
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖÑùÊ½Ğ¡×ÖÌåÎ÷·½ÑùÊ½´ó×ÖÌå¾Ó×ó´øÏÂ»®Ïß\n!hz s\n!asc l\n!gray 5\n!yspace 6\n*underline l aBc34¹ú¹ú¹ú¹úaBc34\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖÑùÊ½±ê×¼×ÖÌåÎ÷·½ÑùÊ½±ê×¼×ÖÌå¾ÓÖĞ´øÏÂ»®Ïß\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*underline c aBc1234¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹úaBc1234\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖÑùÊ½´ó×ÖÌåÎ÷·½ÑùÊ½Ğ¡×ÖÌå¾ÓÓÒ´øÏÂ»®Ïß\n!hz l\n!asc s\n!gray 5\n!yspace 6\n*underline r aBc1234¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹úaBc1234\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖÑùÊ½´ó×ÖÌåÎ÷·½ÑùÊ½Ğ¡×ÖÌåÆ«ÒÆ16´øÏÂ»®Ïß\n!hz l\n!asc s\n!gray 5\n!yspace 6\n*underline x:16 aBc1234¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹úaBc1234\n*line\n*feedline 1\n",//ĞÂÔöÆ«ÒÆÁ¿´òÓ¡x:16
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖÑùÊ½Ğ¡×ÖÌå,Ã¿ĞĞ24¸ö,¾Ó×ó\n!hz s\n!gray 5\n!yspace 6\n*text l ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖÑùÊ½Ğ¡×ÖÌå,Ã¿ĞĞ24¸ö,Æ«ÒÆ120ÏñËØ\n!hz s\n!gray 5\n!yspace 6\n*text x:120 ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",//ĞÂÔöÆ«ÒÆÁ¿´òÓ¡x:120
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖÑùÊ½±ê×¼×ÖÌå,Ã¿ĞĞ16¸ö,¾ÓÖĞ\n!hz n\n!gray 5\n!yspace 6\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖÑùÊ½´ó×ÖÌå,Ã¿ĞĞ12¸ö,¾ÓÓÒ\n!hz l\n!gray 5\n!yspace 6\n*text r ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖÑùÊ½Ğ¡×ÖÌå¿í¶È±ê×¼×ÖÌå¸ß¶È,¾Ó×ó\n!hz sn\n!gray 5\n!yspace 6\n*text l ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",		
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖÑùÊ½Ğ¡×ÖÌå¿í¶È´ó×ÖÌå¸ß¶È,¾ÓÖĞ\n!hz sl\n!gray 5\n!yspace 6\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖÑùÊ½±ê×¼×ÖÌå¿í¶È´ó×ÖÌå¸ß¶È,¾ÓÓÒ\n!hz nl\n!gray 5\n!yspace 6\n*text r ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						//´òÓ¡Ó¢ÎÄ:ascÃüÁî
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Î÷·½ÑùÊ½Ğ¡×ÖÌå,Ã¿ĞĞ48¸ö,¾Ó×ó\n!asc s\n!gray 5\n!yspace 6\n*text l sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Î÷·½ÑùÊ½±ê×¼×ÖÌå,Ã¿ĞĞ32¸ö,¾ÓÖĞ\n!asc n\n!gray 5\n!yspace 6\n*text c sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Î÷·½ÑùÊ½´ó×ÖÌå,Ã¿ĞĞ24¸ö,¾ÓÓÒ\n!asc l\n!gray 5\n!yspace 6\n*text r sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Î÷·½ÑùÊ½Ğ¡×ÖÌå¿í¶È±ê×¼×ÖÌå¸ß¶È,¾Ó×ó\n!asc sn\n!gray 5\n!yspace 6\n*text l sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Î÷·½ÑùÊ½Ğ¡×ÖÌå¿í¶È´ó×ÖÌå¸ß¶È,¾ÓÖĞ\n!asc sl\n!gray 5\n!yspace 6\n*text c sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Î÷·½ÑùÊ½±ê×¼×ÖÌå¿í¶È´ó×ÖÌå¸ß¶È,¾ÓÓÒ\n!asc nl\n!gray 5\n!yspace 6\n*text r sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						//»Ò¶È:grayÃüÁî
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×Ö»Ò¶ÈÖµÎª1\n!hz n\n!gray 1\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×Ö»Ò¶ÈÖµÎª3\n!hz n\n!gray 3\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×Ö»Ò¶ÈÖµÎª7\n!hz n\n!gray 7\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×Ö»Ò¶ÈÖµÎª10\n!hz n\n!gray 10\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×Ö»Ò¶ÈÖµÎª5\n!hz n\n!gray 5\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",//»Ò¶ÈÖµ5·ÅºóÃæÊÇÎªÁË»Ö¸´Ä¬ÈÏÖµ5
						//ĞĞ¼ä¾à:yspaceÃüÁî
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖĞĞ¼ä¾àÎª0\n!hz n\n!yspace 0\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖĞĞ¼ä¾àÎª10\n!hz n\n!yspace 10\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖĞĞ¼ä¾àÎª20\n!hz n\n!yspace 20\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖĞĞ¼ä¾àÎª30\n!hz n\n!yspace 30\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖĞĞ¼ä¾àÎª40\n!hz n\n!yspace 40\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖĞĞ¼ä¾àÎª50\n!hz n\n!yspace 50\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖĞĞ¼ä¾àÎª60\n!hz n\n!yspace 60\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖĞĞ¼ä¾àÎª6\n!hz n\n!yspace 6\n*text c ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",//»Ö¸´³ÉÄ¬ÈÏÖµĞĞ¼ä¾à6
						//ÌõĞÎÂë:!barcodeÃüÁîºÍ*barcodeÃüÁî
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È120¾ÓÓÒ)ÇëÉ¨Ãè´òÓ¡³öµÄÌõĞÎÂëÄÚÈİÊÇ·ñÎª:0123456789ABC\n!barcode 2 120\n*barcode r 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È4¸ß¶È128¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄÌõĞÎÂëÄÚÈİÊÇ·ñÎª:123456\n!barcode 4 128\n*barcode c 123456\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È4¸ß¶È128Æ«ÒÆ50)ÇëÉ¨Ãè´òÓ¡³öµÄÌõĞÎÂëÄÚÈİÊÇ·ñÎª:123456\n!barcode 4 128\n*barcode x:50 123456\n*line\n*feedline 1\n",//ĞÂÔöÆ«ÒÆÁ¿´òÓ¡x:50
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È8¸ß¶È320¾Ó×ó)ÇëÉ¨Ãè´òÓ¡³öµÄÌõĞÎÂëÄÚÈİÊÇ·ñÎª:123\n!barcode 8 320\n*barcode l 123\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È1¸ß¶È64¾ÓÖĞ,´¿×Ö·û31¸ö),²»ÒªÇóÄÜ¹»É¨Ãè³öÀ´\n!barcode 1 64\n*barcode c AAAAAAAAAABBBBBBBBBBCCCCCCCCCCD\n*line\n*feedline 1\n",//ĞÂÔö¿ÉÒÔÉ¨ÃèµÄ×Ö·ûÊıµÄ±ß½ç²âÊÔ,¿í¶ÈÎª1:´¿×Ö·û31¸ö,´¿Êı×Ö60,¿í¶ÈÎª2:´¿×Ö·û14¸ö,´¿Êı×Ö28¸ö
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È1¸ß¶È64¾ÓÖĞ,´¿Êı×Ö60¸ö),²»ÒªÇóÄÜ¹»É¨Ãè³öÀ´\n!barcode 1 64\n*barcode c 012345678901234567890123456789012345678901234567890123456789\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,´¿Êı×Ö28¸ö)ÇëÉ¨Ãè´òÓ¡³öµÄÌõĞÎÂëÄÚÈİÊÇ·ñÎª:0123456789012345678901234567\n!barcode 2 64\n*barcode c 0123456789012345678901234567\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,´¿×ÖÄ¸14¸ö)ÇëÉ¨Ãè´òÓ¡³öµÄÌõĞÎÂëÄÚÈİÊÇ·ñÎª:AAAAAAAAAABBBB\n!barcode 2 64\n*barcode c AAAAAAAAAABBBB\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,×ÖÄ¸ÓëÊı×Ö×éºÏ)ÇëÉ¨Ãè´òÓ¡³öµÄÌõĞÎÂëÄÚÈİÊÇ·ñÎª:01AB23BC34C\n!barcode 2 64\n*barcode c 01AB23BC34C\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,×ÖÄ¸ÓëÊı×Ö×éºÏ)ÇëÉ¨Ãè´òÓ¡³öµÄÌõĞÎÂëÄÚÈİÊÇ·ñÎª:A1B2C3D4E5F6G7\n!barcode 2 64\n*barcode c A1B2C3D4E5F6G7\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,×ÖÄ¸ÓëÊı×Ö×éºÏ)ÇëÉ¨Ãè´òÓ¡³öµÄÌõĞÎÂëÄÚÈİÊÇ·ñÎª:ABC123CDE4567\n!barcode 2 64\n*barcode c ABC123CDE4567\n*line\n*feedline 1\n",							
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,´¿Êı×Ö32¸ö(³¬¹ı28,¿í¶È»á×Ô¶¯½µÎª1)),²»ÒªÇóÄÜ¹»É¨Ãè³öÀ´\n!barcode 2 64\n*barcode c 01234567890123456789012345678912\n*line\n*feedline 1\n",//»Ö¸´Ä¬ÈÏ¿í¶È2,¸ß¶È64(³¬¹ı28¸ö,¿í¶È»á×Ô¶¯½µÎª1)
						//¶şÎ¬Âë:!qrcodeÃüÁîºÍ*qrcodeÃüÁî
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È384¾ÓÓÒ)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:ABC123456789DEFGH\n!qrcode 384 3\n*qrcode r ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È10¾Ó×ó)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:ABC123456789DEFGH\n!qrcode 10 0\n*qrcode l ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È50¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:ABC123456789DEFGH\n!qrcode 50 1\n*qrcode c ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È50Æ«ÒÆ50)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:ABC123456789DEFGH\n!qrcode 50 1\n*qrcode x:50 ABC123456789DEFGH\n*line\n*feedline 1\n",//ĞÂÔöÆ«ÒÆÁ¿´òÓ¡x:50
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È200¾ÓÓÒ)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:ABC123456789DEFGH\n!qrcode 200 3\n*qrcode r ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È100¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:ABC123456789DEFGH\n!qrcode 100 2\n*qrcode c ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È300¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:400×Ö½ÚµÄA\n!qrcode 300 1\n*qrcode c AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È300¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:401×Ö½ÚµÄA\n!qrcode 300 1\n*qrcode c AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n*line\n*feedline 1\n",
						//´òÓ¡Í¼Æ¬
						//"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Í¼Æ¬(¾Ó×ó)\n*image l 32*64 #ums\n*line\n*feedline 1\n",
						//"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Í¼Æ¬(¾ÓÖĞ)\n*image c 64*128 #ums\n*line\n*feedline 1\n",
						//"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Í¼Æ¬(¾ÓÓÒ)\n*image r 128*64 #ums\n*line\n*feedline 1\n",						
						//"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Í¼Æ¬(¾ÓÓÒ)\n*image r 128*64 #logo\n*line\n*feedline 1\n",						
						//"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Í¼Æ¬(¾ÓÖĞ)\n*image c 128*64 data:base64:ROIGODIhuA\n*line\n*feedline 1\n",
						//´òÓ¡·Ö¸ô·û
						"!hz n\n!asc n\n!gray 7\n!yspace 6\n*text c ÒÔÏÂ´òÓ¡Á½Ìõ·Ö¸ô·û\n*line\n*line\n*feedline 1\n",						
						//"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡Á½Ìõ·Ö¸ô·û\n*line\n*feedline p 1\n", 
						//NLFONTÃüÁî
						"!NLFONT 9 12 0\n*text l ¿¿×ó´òÓ¡µÄÖĞÎÄÎª×ÔÓĞËÎÌå24x24,Î÷ÎÄÎªGulimche×ÖÌå12x24,ºáÏò×İÏò¾ù·Å´ó2±¶\n*line\n*feedline 1\n",
						"!NLFONT 10 13 1\n*text l ¿¿×ó´òÓ¡µÄÖĞÎÄÎª·ÂËÎ24x24,Î÷ÎÄÎªMSGothic´ÖÌå16*32,ºáÏò·Å´ó2±¶×İÏòÕı³£\n*line\n*feedline 1\n",
						"!NLFONT 11 17 2\n*text l ¿¿×ó´òÓ¡µÄÖĞÎÄÎª¿¬Ìå24*24,Î÷ÎÄÎª¿¬Ìå´ÖÌå12x16,ºáÏòÕı³£×İÏò·Å´ó2±¶\n*line\n*feedline 1\n",
						"!NLFONT 13 19 3\n*text l ¿¿×ó´òÓ¡µÄÖĞÎÄÎªËÎÌå12x12,Î÷ÎÄÎª¿¬Ìå´ÖÌå16x32,ºáÏò×İÏò¾ùÕı³£\n*line\n*feedline 1\n",
						"!NLFONT 9 12 0\n*text c ¾ÓÖĞ´òÓ¡µÄÖĞÎÄÎª×ÔÓĞËÎÌå24x24,Î÷ÎÄÎªGulimche×ÖÌå12x24,ºáÏò×İÏò¾ù·Å´ó2±¶\n*line\n*feedline 1\n",
						"!NLFONT 10 13 1\n*text c ¾ÓÖĞ´òÓ¡µÄÖĞÎÄÎª·ÂËÎ24x24,Î÷ÎÄÎªMSGothic´ÖÌå16*32,ºáÏò2±¶·Å´ó×İÏòÕı³£\n*line\n*feedline 1\n",
						"!NLFONT 11 17 2\n*text c ¾ÓÖĞ´òÓ¡µÄÖĞÎÄÎª¿¬Ìå24*24,Î÷ÎÄÎª¿¬Ìå´ÖÌå12x16,ºáÏòÕı³£×İÏò2±¶·Å´ó\n*line\n*feedline 1\n",
						"!NLFONT 13 19 3\n*text c ¾ÓÖĞ´òÓ¡µÄÖĞÎÄÎªËÎÌå12x12,Î÷ÎÄÎª¿¬Ìå´ÖÌå16x32,ºáÏò×İÏò¾ùÕı³£\n*line\n*feedline 1\n",
						"!NLFONT 9 12 4\n*text r ¿¿ÓÒ´òÓ¡µÄÖĞÎÄÎª×ÔÓĞËÎÌå24x24,Î÷ÎÄÎªGulimche×ÖÌå12x24,ºáÏò3±¶·Å´ó×İÏò3±¶·Å´ó\n*line\n*feedline 1\n",
						"!NLFONT 10 13 5\n*text r ¿¿ÓÒ´òÓ¡µÄÖĞÎÄÎª·ÂËÎ24x24,Î÷ÎÄÎªMSGothic´ÖÌå16*32,ºáÏò3±¶·Å´ó×İÏòÕı³£\n*line\n*feedline 1\n",
						"!NLFONT 11 17 6\n*text r ¿¿ÓÒ´òÓ¡µÄÖĞÎÄÎª¿¬Ìå24*24,Î÷ÎÄÎª¿¬Ìå´ÖÌå12x16,ºáÏòÕı³£×İÏò3±¶·Å´ó\n*line\n*feedline 1\n",
						"!NLFONT 13 19 3\n*text r ¿¿ÓÒ´òÓ¡µÄÖĞÎÄÎªËÎÌå12x12,Î÷ÎÄÎª¿¬Ìå´ÖÌå16x32,ºáÏò×İÏò¾ùÕı³£\n*line\n*feedline 1\n",
						//ÔİÍ£Ê±¼ä:pauseÃüÁî¡¢NLPRNOVERÃüÁî						
						"!NLFONT 9 12\n*text l ÒÔÏÂ´òÓ¡Í¼Æ¬¼°ÎÄ×Ö,½«ÔİÍ£5sºó´òÓ¡\n!NLPRNOVER\n*pause 5\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*image c 128*64 #ums\n*text c ÒøÁªÉÌÎñ-È«Ãñ¸¶½»Ò×Æ¾Ìõ\n!hz l\n!asc l\n*text c (´ó×ÖÌå)¿Í·şµç»°:95534\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text l ÒÔÆ«ÒÆ256´òÓ¡Í¼Æ¬\n!NLPRNOVER\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*image x:256 128*64 #ums\n*line\n*feedline 1\n",//ĞÂÔöÆ«ÒÆx:256´òÓ¡128¿î¶ÈµÄÍ¼Æ¬
						//ÍËÖ½
						"!NLFONT 9 12\n*text l ½øÍËÖ½²âÊÔµÚÒ»ĞĞÎÄ×Ö\n*feedline 1\n!gray 5\n!yspace 6\n*text c µÚ¶şĞĞÎÄ×Ö.´òÓ¡µÄÎÄ×ÖÃ»ÓĞÏà»¥¸²¸Ç\n*line\n*feedline 1\n",//*feedline -1\n
						"!gray 5\n!yspace 6\n*text c ±¾ĞĞÎÄ×ÖÓ¦ÍêÈ«ÔÚÖ½²ÛÍâ\n*feedline 5\n",//!NLPRNOVER\n	
						//ÌõĞÎÂë
	                    			//ttfÌØÓĞ:!BARCODEÃüÁîºÍ*BARCODEÃüÁî£¬Ö§³Ö¶àÖÖ±àÂë£¬Ç°2¸ö²ÎÊı¸ú!barcodeÒ»Ñù£¬µÚÈı¸ö²ÎÊı±íÊ¾ÊÇ·ñÏÔÊ¾ÏÂ·½ÌáÊ¾ĞÅÏ¢£¬µÚËÄ¸ö²ÎÊı±íÊ¾ÌõÂë±àÂëÀàĞÍ
	                    			//0 caodebar; 1 code39; 2 code93; 3 code128; 4 EAN-8/EAN-13/JAN-8/JAN-13; 5 ITF; 6 UPCA; 7 UPCE
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄCodebarÌõĞÎÂëÄÚÈİÊÇ·ñÎª:0123456789£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÓĞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 1 0\n*BARCODE c A0123456789C\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄCode39ÌõĞÎÂëÄÚÈİÊÇ·ñÎª:ABCDEFGHIJKLMNOPQRSTUVWXYZ -.$/+%0123456789£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÓĞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 1 1\n*BARCODE c ABCDEFGHIJKLMNOPQRSTUVWXYZ -.$/+%0123456789\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄCode93ÌõĞÎÂëÄÚÈİÊÇ·ñÎª:CD 45%+67$/£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÓĞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 1 2\n*BARCODE c CD 45%+67$\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄCode128ÌõĞÎÂëÄÚÈİÊÇ·ñÎª:A0123456C%$#@£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÓĞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 1 3\n*BARCODE c A0123456C%$#@\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄEAN-8ÌõĞÎÂëÄÚÈİÊÇ·ñÎª:12345670£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÎŞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 0 4\n*BARCODE c 1234567\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄEAN-13ÌõĞÎÂëÄÚÈİÊÇ·ñÎª:1234567890128£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÎŞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 0 4\n*BARCODE c 123456789012\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄEAN-8ÌõĞÎÂëÄÚÈİÊÇ·ñÎª:00001236£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÓĞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 1 4\n*BARCODE c 123\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄEAN-13ÌõĞÎÂëÄÚÈİÊÇ·ñÎª:0000123456784£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÓĞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 1 4\n*BARCODE c 12345678\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄITFÌõĞÎÂëÄÚÈİÊÇ·ñÎª:01234567890128£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÎŞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 0 5\n*BARCODE c 0123456789012\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄITFÌõĞÎÂëÄÚÈİÊÇ·ñÎª:00123456789050£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÓĞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 1 5\n*BARCODE c 12345678905\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄUPC-AÌõĞÎÂëÄÚÈİÊÇ·ñÎª:123456789012£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÎŞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 0 6\n*BARCODE c 12345678901\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄUPC-AÌõĞÎÂëÄÚÈİÊÇ·ñÎª:000765432108£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÓĞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 1 6\n*BARCODE c 76543210\n*line\n*feedline 1\n",							
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È6¸ß¶È120¾ÓÖĞ)ÇëÉ¨Ãè´òÓ¡³öµÄUPC-EÌõĞÎÂëÄÚÈİÊÇ·ñÎª:01234565£¬²¢ÇÒÌõÂëÏÂ·½¡¾ÎŞ¡¿ÏÔÊ¾ÌõÂëĞÅÏ¢\n!BARCODE 6 120 0 7\n*BARCODE c 0123456\n*line\n*feedline 1\n",
						//¶şÎ¬Âë
						//ttfÌØÓĞ:!QRCODEÃüÁîºÍ*QRCODEÃüÁî£¬Ö§³Ö¶àÖÖ±àÂë£¬Ç°2¸ö²ÎÊı¸ú!qrcodeÒ»Ñù£¬µÚ¶ş¸ö²ÎÊı½ö¶ÔQRCODEÓĞĞ§£¬µÚÈı¸ö²ÎÊı±íÊ¾¶şÎ¬Âë±àÂëÀàĞÍ
						//0 Data Matrix;  1 Matrixcode; 2 PDF417; 3 QRCODE
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È200¾ÓÖĞ,±àÂë·½Ê½Data Matrix)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:Aa12%#@*&¦Á¦¢¦Ã¨¤¨²¨¯\n!QRCODE 200 0 0\n*QRCODE c Aa12%#@*&¦Á¦¢¦Ã¨¤¨²¨¯\n*line\n*feedline 1\n",//ĞÂÔöÒÔ½Ï´ó¸ß¶ÈµÄ¶şÎ¬Âë¿ªÊ¼´ò ÔÙ´òÓ¡¸ß¶ÈĞ¡µÄ
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È200¾ÓÖĞ,±àÂë·½Ê½Maxicode)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:ABC~*&%#!56789DEFGH\n!QRCODE 200 0 1\n*QRCODE c ABC~*&%#!56789DEFGH\n*line\n*feedline 1\n",//ĞÂÔöÒÔ½Ï´ó¸ß¶ÈµÄ¶şÎ¬Âë¿ªÊ¼´ò ÔÙ´òÓ¡¸ß¶ÈĞ¡µÄ
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È200¾ÓÖĞ,±àÂë·½Ê½PDF417)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:PDF²âÊÔÖ§³ÖÖĞÎÄ½Ï²î\n!QRCODE 200 0 2\n*QRCODE c PDF²âÊÔÖ§³ÖÖĞÎÄ½Ï²î\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È200¾ÓÖĞ,±àÂë·½Ê½QR Code)ÇëÉ¨Ãè´òÓ¡³öµÄ¶şÎ¬ÂëÄÚÈİÊÇ·ñÎª:QRCODE²âÊÔÖ§³ÖÖĞÎÄºÃ\n!QRCODE 200 2 3\n*QRCODE c QRCODE²âÊÔÖ§³ÖÖĞÎÄºÃ\n*line\n*feedline 1\n",
						//ttfÌØÓĞ:*TEXTÖ¸Áî£¬ÓÃ·¨Óë*textÖ¸ÁîÒ»Ñù£¬Çø±ğÊÇ*text»á»»ĞĞ£¬*TEXT²»»á»»ĞĞ
						//ttfÌØÓĞ:*UNDERLINEÖ¸Áî£¬ÓÃ·¨Óë*underlineÖ¸ÁîÒ»Ñù£¬Çø±ğÊÇ*underline»á»»ĞĞ£¬*UNDERLINE²»»á»»ĞĞ
						//"!NLFONT 2 1 3\n*TEXT l ±¾ĞĞÎÄ×ÖTest\n!NLFONT 9 12 3\n*text r ÎÄ×Ö¾ÓÓÒ²¢ÇÒ±¾ĞĞ²»»»ĞĞTest\n",
						"!NLFONT 2 1 3\n*TEXT l ±¾ĞĞÎÄ×ÖTest\n!NLFONT 9 12 3\n*text r ²»»»ĞĞTest\n"
						"!NLFONT 9 12 3\n*UNDERLINE l ±¾ĞĞÎÄ×Ö´øÏÂ»®Ïß\n*underline r ²¢ÇÒ²»»»ĞĞTest\n",
						"!NLFONT 9 12 3\n*TEXT l ×ó±ßleft\n!NLFONT 2 1 3\n*UNDERLINE c ÏÂ»®Ïßcenter\n!NLFONT 6 4 3\n*text r ÓÒ±ßright\n",
						//Ó¢ÎÄºáÏò·Å´ó×İÏò·Å´óÖ®Ç°´æÔÚÒ»¸öÓ¢ÎÄ×ÖÄ¸»á·Ö¿ª2±ßµÄÎÊÌâ
						"!NLFONT 9 12 0\n*text r ¿¿ÓÒ´òÓ¡ÎÄ×Ö,ºáÏò·Å´ó¡¢×İÏò·Å´ó,¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n",
						"!NLFONT 9 12 0\n*text r To print text,transverse magnification,vertical magnification\n*line\n",
					  };
	#endif
	char *buf_str[] = {
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦120å±…å³)è¯·æ‰«ææ‰“å°å‡ºçš„æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:0123456789ABC\n!barcode 2 120\n*barcode r 0123456789ABC\n*line\n*feedline 1\n",//ĞÂÔöÓÃÀı²âÊÔµÚÒ»´Îµ÷ÓÃ½Å±¾´òÓ¡Ö±½Ó´òÓ¡ÌõÂë,ÖĞ¼ä²»Ó¦¸ÃÓĞ¿Õ°×ĞĞ
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦8é«˜åº¦384å±…å³)è¯·æ‰«ææ‰“å°å‡ºçš„æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:0123456789ABC\n!barcode 8 384\n*barcode r 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦384å±…å³)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:ABC123456789DEFGH\n!qrcode 384 3\n*qrcode r ABC123456789DEFGH\n*line\n*feedline 1\n",
						//ttfÌØÓĞ:Ê¹ÓÃWellnerAdaptiveThresholdãĞÖµËã·¨ yzÎª0
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°PNGå›¾ç‰‡ï¼Œé˜ˆå€¼ä½¿ç”¨WellnerAdaptiveThresholdç®—æ³•(å±…å·¦)ï¼Œæ•ˆæœè¯·å¯¹ç…§å®é™…å›¾ç‰‡ï¼Œå½©è‰²å›¾ç‰‡æµ…é¢œè‰²å°†æ‰“å°æˆç™½è‰²ï¼Œæ·±é¢œè‰²å°†æ‰“å°æˆé»‘è‰²\n*image l 384*300 path:yz:0;/mnt/sdcard/picture/color1.png\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°PNGå›¾ç‰‡ï¼Œé˜ˆå€¼ä½¿ç”¨WellnerAdaptiveThresholdç®—æ³•(å±…ä¸­)ï¼Œæ•ˆæœè¯·å¯¹ç…§å®é™…å›¾ç‰‡ï¼Œå½©è‰²å›¾ç‰‡æµ…é¢œè‰²å°†æ‰“å°æˆç™½è‰²ï¼Œæ·±é¢œè‰²å°†æ‰“å°æˆé»‘è‰²\n*image c 394*300 path:yz:0;/mnt/sdcard/picture/color2.png\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°PNGå›¾ç‰‡ï¼Œé˜ˆå€¼ä½¿ç”¨WellnerAdaptiveThresholdç®—æ³•(å±…å³)ï¼Œæ•ˆæœè¯·å¯¹ç…§å®é™…å›¾ç‰‡ï¼Œå½©è‰²å›¾ç‰‡æµ…é¢œè‰²å°†æ‰“å°æˆç™½è‰²ï¼Œæ·±é¢œè‰²å°†æ‰“å°æˆé»‘è‰²\n*image r 384*300 path:yz:0;/mnt/sdcard/picture/color3.png\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°PNGå›¾ç‰‡,é˜ˆå€¼255æ‰“å°é»‘å—(å±…å³)\n*image r 384*240 path:yz:255;/mnt/sdcard/picture/png03.png\n*line\n*feedline 1\n",  //æ–°å¢ç”¨ä¾‹æµ‹è¯•pngå›¾ç‰‡åŠŸèƒ½ä½¿ç”¨png02å’Œpng03æ›¿æ¢åŸæ¥çš„logoå›¾ç‰‡
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°PNGå›¾ç‰‡,é˜ˆå€¼é»˜è®¤(å±…ä¸­),æ³¨æ„æ‰“å°æ•ˆæœ\n*image c 384*49 path:/mnt/sdcard/picture/png08.png\n*line\n*feedline 1\n",//æ–°å¢æ‰“å°å›¾ç‰‡
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°å‹ç¼©PNGå›¾ç‰‡,é˜ˆå€¼è‡ªåŠ¨ï¼Œ(100*200ï¼Œå±…å·¦)ï¼Œä¸å¿…å…³æ³¨æ‰“å°æ•ˆæœ\n*image l 100*200 path:/mnt/sdcard/picture/png02.png\n*line\n*feedline 1\n", 
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°å‹ç¼©PNGå›¾ç‰‡,é˜ˆå€¼è‡ªåŠ¨ï¼Œ(100*200ï¼Œå±…ä¸­)ï¼Œä¸å¿…å…³æ³¨æ‰“å°æ•ˆæœ\n*image c 100*200 path:/mnt/sdcard/picture/png03.png\n*line\n*feedline 1\n", 
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°å‹ç¼©PNGå›¾ç‰‡,é˜ˆå€¼è‡ªåŠ¨ï¼Œ(100*200ï¼Œå±…å³)ï¼Œä¸å¿…å…³æ³¨æ‰“å°æ•ˆæœ\n*image r 100*200 path:/mnt/sdcard/picture/png04.png\n*line\n*feedline 1\n", //æ–°å¢æ‰“å°å‹ç¼©çš„PNGå›¾ç‰‡
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°å‹ç¼©PNGå›¾ç‰‡,WellnerAdaptiveThresholdç®—æ³•é˜ˆå€¼è‡ªåŠ¨,(384*144,å±…å³),ä¸å¿…å…³æ³¨æ‰“å°æ•ˆæœ\n*image r 384*144 path:yz:0;/mnt/sdcard/picture/IHDR8.png\n*feedline 1\n",
						//ĞÂÔö¶şÎ¬ÂëÖ§³Ö400³¤¶È
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦100å±…ä¸­ï¼Œå†…å®¹400)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•\
						æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•\
						æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•\n!qrcode 100 2\n*qrcode c æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•\
						æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•æ›´æ–°è®°å½•\n*line\n*feedline 1\n",
						//´òÓ¡ºº×Ö:hzÃüÁî¡¢underlineÃüÁî
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—æ ·å¼å°å­—ä½“è¥¿æ–¹æ ·å¼å¤§å­—ä½“å±…å·¦å¸¦ä¸‹åˆ’çº¿\n!hz s\n!asc l\n!gray 5\n!yspace 6\n*underline l aBc34å›½å›½å›½å›½aBc34\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—æ ·å¼æ ‡å‡†å­—ä½“è¥¿æ–¹æ ·å¼æ ‡å‡†å­—ä½“å±…ä¸­å¸¦ä¸‹åˆ’çº¿\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*underline c aBc1234å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½aBc1234\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—æ ·å¼å¤§å­—ä½“è¥¿æ–¹æ ·å¼å°å­—ä½“å±…å³å¸¦ä¸‹åˆ’çº¿\n!hz l\n!asc s\n!gray 5\n!yspace 6\n*underline r aBc1234å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½aBc1234\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—æ ·å¼å¤§å­—ä½“è¥¿æ–¹æ ·å¼å°å­—ä½“åç§»16å¸¦ä¸‹åˆ’çº¿\n!hz l\n!asc s\n!gray 5\n!yspace 6\n*underline x:16 aBc1234å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½aBc1234\n*line\n*feedline 1\n",//Æ«ÒÆÁ¿´òÓ¡x:16
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—æ ·å¼å°å­—ä½“,æ¯è¡Œ24ä¸ª,å±…å·¦\n!hz s\n!gray 5\n!yspace 6\n*text l å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—æ ·å¼å°å­—ä½“,æ¯è¡Œ24ä¸ª,åç§»120åƒç´ \n!hz s\n!gray 5\n!yspace 6\n*text x:120 å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",//Æ«ÒÆÁ¿´òÓ¡x:120
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—æ ·å¼æ ‡å‡†å­—ä½“,æ¯è¡Œ16ä¸ª,å±…ä¸­\n!hz n\n!gray 5\n!yspace 6\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—æ ·å¼å¤§å­—ä½“,æ¯è¡Œ12ä¸ª,å±…å³\n!hz l\n!gray 5\n!yspace 6\n*text r å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—æ ·å¼å°å­—ä½“å®½åº¦æ ‡å‡†å­—ä½“é«˜åº¦,å±…å·¦\n!hz sn\n!gray 5\n!yspace 6\n*text l å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",		
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—æ ·å¼å°å­—ä½“å®½åº¦å¤§å­—ä½“é«˜åº¦,å±…ä¸­\n!hz sl\n!gray 5\n!yspace 6\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—æ ·å¼æ ‡å‡†å­—ä½“å®½åº¦å¤§å­—ä½“é«˜åº¦,å±…å³\n!hz nl\n!gray 5\n!yspace 6\n*text r å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						//´òÓ¡Ó¢ÎÄ:ascÃüÁî
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°è¥¿æ–¹æ ·å¼å°å­—ä½“,æ¯è¡Œ48ä¸ª,å±…å·¦\n!asc s\n!gray 5\n!yspace 6\n*text l sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°è¥¿æ–¹æ ·å¼æ ‡å‡†å­—ä½“,æ¯è¡Œ32ä¸ª,å±…ä¸­\n!asc n\n!gray 5\n!yspace 6\n*text c sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°è¥¿æ–¹æ ·å¼å¤§å­—ä½“,æ¯è¡Œ24ä¸ª,å±…å³\n!asc l\n!gray 5\n!yspace 6\n*text r sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°è¥¿æ–¹æ ·å¼å°å­—ä½“å®½åº¦æ ‡å‡†å­—ä½“é«˜åº¦,å±…å·¦\n!asc sn\n!gray 5\n!yspace 6\n*text l sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°è¥¿æ–¹æ ·å¼å°å­—ä½“å®½åº¦å¤§å­—ä½“é«˜åº¦,å±…ä¸­\n!asc sl\n!gray 5\n!yspace 6\n*text c sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°è¥¿æ–¹æ ·å¼æ ‡å‡†å­—ä½“å®½åº¦å¤§å­—ä½“é«˜åº¦,å±…å³\n!asc nl\n!gray 5\n!yspace 6\n*text r sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						//»Ò¶È:grayÃüÁî
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—ç°åº¦å€¼ä¸º1\n!hz n\n!gray 1\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—ç°åº¦å€¼ä¸º3\n!hz n\n!gray 3\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—ç°åº¦å€¼ä¸º7\n!hz n\n!gray 7\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—ç°åº¦å€¼ä¸º10\n!hz n\n!gray 10\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—ç°åº¦å€¼ä¸º5\n!hz n\n!gray 5\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",//»Ò¶ÈÖµ5·ÅºóÃæÊÇÎªÁË»Ö¸´Ä¬ÈÏÖµ5
						//ĞĞ¼ä¾à:yspaceÃüÁî
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—è¡Œé—´è·ä¸º0\n!hz n\n!yspace 0\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—è¡Œé—´è·ä¸º10\n!hz n\n!yspace 10\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—è¡Œé—´è·ä¸º20\n!hz n\n!yspace 20\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—è¡Œé—´è·ä¸º30\n!hz n\n!yspace 30\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—è¡Œé—´è·ä¸º40\n!hz n\n!yspace 40\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—è¡Œé—´è·ä¸º50\n!hz n\n!yspace 50\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—è¡Œé—´è·ä¸º60\n!hz n\n!yspace 60\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—è¡Œé—´è·ä¸º6\n!hz n\n!yspace 6\n*text c å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",//»Ö¸´³ÉÄ¬ÈÏÖµĞĞ¼ä¾à6
						//ÌõĞÎÂë:!barcodeÃüÁîºÍ*barcodeÃüÁî
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦120å±…å³)è¯·æ‰«ææ‰“å°å‡ºçš„æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:0123456789ABC\n!barcode 2 120\n*barcode r 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦4é«˜åº¦128å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:123456\n!barcode 4 128\n*barcode c 123456\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦4é«˜åº¦128åç§»50)è¯·æ‰«ææ‰“å°å‡ºçš„æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:123456\n!barcode 4 128\n*barcode x:50 123456\n*line\n*feedline 1\n",//Æ«ÒÆÁ¿´òÓ¡x:50
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦8é«˜åº¦320å±…å·¦)è¯·æ‰«ææ‰“å°å‡ºçš„æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:123\n!barcode 8 320\n*barcode l 123\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦1é«˜åº¦64å±…ä¸­,çº¯å­—ç¬¦31ä¸ª),ä¸è¦æ±‚èƒ½å¤Ÿæ‰«æå‡ºæ¥\n!barcode 1 64\n*barcode c AAAAAAAAAABBBBBBBBBBCCCCCCCCCCD\n*line\n*feedline 1\n",//ĞÂÔö¿ÉÒÔÉ¨ÃèµÄ×Ö·ûÊıµÄ±ß½ç²âÊÔ,¿í¶ÈÎª1:´¿×Ö·û31¸ö,´¿Êı×Ö60,¿í¶ÈÎª2:´¿×Ö·û14¸ö,´¿Êı×Ö28¸ö
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦1é«˜åº¦64å±…ä¸­,çº¯æ•°å­—60ä¸ª),ä¸è¦æ±‚èƒ½å¤Ÿæ‰«æå‡ºæ¥\n!barcode 1 64\n*barcode c 012345678901234567890123456789012345678901234567890123456789\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,çº¯æ•°å­—28ä¸ª)è¯·æ‰«ææ‰“å°å‡ºçš„æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:0123456789012345678901234567\n!barcode 2 64\n*barcode c 0123456789012345678901234567\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,çº¯å­—æ¯14ä¸ª)è¯·æ‰«ææ‰“å°å‡ºçš„æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:AAAAAAAAAABBBB\n!barcode 2 64\n*barcode c AAAAAAAAAABBBB\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,å­—æ¯ä¸æ•°å­—ç»„åˆ)è¯·æ‰«ææ‰“å°å‡ºçš„æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:01AB23BC34C\n!barcode 2 64\n*barcode c 01AB23BC34C\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,å­—æ¯ä¸æ•°å­—ç»„åˆ)è¯·æ‰«ææ‰“å°å‡ºçš„æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:A1B2C3D4E5F6G7\n!barcode 2 64\n*barcode c A1B2C3D4E5F6G7\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,å­—æ¯ä¸æ•°å­—ç»„åˆ)è¯·æ‰«ææ‰“å°å‡ºçš„æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:ABC123CDE4567\n!barcode 2 64\n*barcode c ABC123CDE4567\n*line\n*feedline 1\n",							
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,çº¯æ•°å­—32ä¸ª(è¶…è¿‡28,å®½åº¦ä¼šè‡ªåŠ¨é™ä¸º1)),ä¸è¦æ±‚èƒ½å¤Ÿæ‰«æå‡ºæ¥\n!barcode 2 64\n*barcode c 01234567890123456789012345678912\n*line\n*feedline 1\n",//»Ö¸´Ä¬ÈÏ¿í¶È2,¸ß¶È64(³¬¹ı28¸ö,¿í¶È»á×Ô¶¯½µÎª1)
						//¶şÎ¬Âë:!qrcodeÃüÁîºÍ*qrcodeÃüÁî
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦384å±…å³)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:ABC123456789DEFGH\n!qrcode 384 3\n*qrcode r ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦10å±…å·¦)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:ABC123456789DEFGH\n!qrcode 10 0\n*qrcode l ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦50å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:ABC123456789DEFGH\n!qrcode 50 1\n*qrcode c ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦50åç§»50)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:ABC123456789DEFGH\n!qrcode 50 1\n*qrcode x:50 ABC123456789DEFGH\n*line\n*feedline 1\n",//ĞÂÔöÆ«ÒÆÁ¿´òÓ¡x:50
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦200å±…å³)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:ABC123456789DEFGH\n!qrcode 200 3\n*qrcode r ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦100å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:ABC123456789DEFGH\n!qrcode 100 2\n*qrcode c ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦300å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:400å­—èŠ‚çš„A\n!qrcode 300 1\n*qrcode c AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦300å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:401å­—èŠ‚çš„A\n!qrcode 300 1\n*qrcode c AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n*line\n*feedline 1\n",
						//´òÓ¡Í¼Æ¬, ¿ª·¢Ò²²»ÖªµÀÕâÊÇÊ²Ã´¸ñÊ½µÄÍ¼Æ¬£¬ÇÒÊµ¼ÊÃ»ÓĞÓÃµ½ÕâÑùµÄÍ¼Æ¬£¬²»²â
						//"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°å›¾ç‰‡(å±…å·¦)\n*image l 32*64 #ums\n*line\n*feedline 1\n",
						//"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°å›¾ç‰‡(å±…ä¸­)\n*image c 64*128 #ums\n*line\n*feedline 1\n",
						//"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°å›¾ç‰‡(å±…å³)\n*image r 128*64 #ums\n*line\n*feedline 1\n",						
						//"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°å›¾ç‰‡(å±…å³)\n*image r 128*64 #logo\n*line\n*feedline 1\n",						
						//"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°å›¾ç‰‡(å±…ä¸­)\n*image c 128*64 data:base64:ROIGODIhuA\n*line\n*feedline 1\n",
						//´òÓ¡·Ö¸ô·û
						"!hz n\n!asc n\n!gray 7\n!yspace 6\n*text c ä»¥ä¸‹æ‰“å°ä¸¤æ¡åˆ†éš”ç¬¦\n*line\n*line\n*feedline 1\n",						
						//"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°ä¸¤æ¡åˆ†éš”ç¬¦\n*line\n*feedline p 1\n", 
						//NLFONTÃüÁî
						"!NLFONT 9 12 0\n*text l é å·¦æ‰“å°çš„ä¸­æ–‡ä¸ºè‡ªæœ‰å®‹ä½“24x24,è¥¿æ–‡ä¸ºGulimcheå­—ä½“12x24,æ¨ªå‘çºµå‘å‡æ”¾å¤§2å€\n*line\n*feedline 1\n",
						"!NLFONT 10 13 1\n*text l é å·¦æ‰“å°çš„ä¸­æ–‡ä¸ºä»¿å®‹24x24,è¥¿æ–‡ä¸ºMSGothicç²—ä½“16*32,æ¨ªå‘æ”¾å¤§2å€çºµå‘æ­£å¸¸\n*line\n*feedline 1\n",
						"!NLFONT 11 17 2\n*text l é å·¦æ‰“å°çš„ä¸­æ–‡ä¸ºæ¥·ä½“24*24,è¥¿æ–‡ä¸ºæ¥·ä½“ç²—ä½“12x16,æ¨ªå‘æ­£å¸¸çºµå‘æ”¾å¤§2å€\n*line\n*feedline 1\n",
						"!NLFONT 13 19 3\n*text l é å·¦æ‰“å°çš„ä¸­æ–‡ä¸ºå®‹ä½“12x12,è¥¿æ–‡ä¸ºæ¥·ä½“ç²—ä½“16x32,æ¨ªå‘çºµå‘å‡æ­£å¸¸\n*line\n*feedline 1\n",
						"!NLFONT 9 12 0\n*text c å±…ä¸­æ‰“å°çš„ä¸­æ–‡ä¸ºè‡ªæœ‰å®‹ä½“24x24,è¥¿æ–‡ä¸ºGulimcheå­—ä½“12x24,æ¨ªå‘çºµå‘å‡æ”¾å¤§2å€\n*line\n*feedline 1\n",
						"!NLFONT 10 13 1\n*text c å±…ä¸­æ‰“å°çš„ä¸­æ–‡ä¸ºä»¿å®‹24x24,è¥¿æ–‡ä¸ºMSGothicç²—ä½“16*32,æ¨ªå‘2å€æ”¾å¤§çºµå‘æ­£å¸¸\n*line\n*feedline 1\n",
						"!NLFONT 11 17 2\n*text c å±…ä¸­æ‰“å°çš„ä¸­æ–‡ä¸ºæ¥·ä½“24*24,è¥¿æ–‡ä¸ºæ¥·ä½“ç²—ä½“12x16,æ¨ªå‘æ­£å¸¸çºµå‘2å€æ”¾å¤§\n*line\n*feedline 1\n",
						"!NLFONT 13 19 3\n*text c å±…ä¸­æ‰“å°çš„ä¸­æ–‡ä¸ºå®‹ä½“12x12,è¥¿æ–‡ä¸ºæ¥·ä½“ç²—ä½“16x32,æ¨ªå‘çºµå‘å‡æ­£å¸¸\n*line\n*feedline 1\n",
						"!NLFONT 9 12 4\n*text r é å³æ‰“å°çš„ä¸­æ–‡ä¸ºè‡ªæœ‰å®‹ä½“24x24,è¥¿æ–‡ä¸ºGulimcheå­—ä½“12x24,æ¨ªå‘3å€æ”¾å¤§çºµå‘3å€æ”¾å¤§\n*line\n*feedline 1\n",
						"!NLFONT 10 13 5\n*text r é å³æ‰“å°çš„ä¸­æ–‡ä¸ºä»¿å®‹24x24,è¥¿æ–‡ä¸ºMSGothicç²—ä½“16*32,æ¨ªå‘3å€æ”¾å¤§çºµå‘æ­£å¸¸\n*line\n*feedline 1\n",
						"!NLFONT 11 17 6\n*text r é å³æ‰“å°çš„ä¸­æ–‡ä¸ºæ¥·ä½“24*24,è¥¿æ–‡ä¸ºæ¥·ä½“ç²—ä½“12x16,æ¨ªå‘æ­£å¸¸çºµå‘3å€æ”¾å¤§\n*line\n*feedline 1\n",
						"!NLFONT 13 19 3\n*text r é å³æ‰“å°çš„ä¸­æ–‡ä¸ºå®‹ä½“12x12,è¥¿æ–‡ä¸ºæ¥·ä½“ç²—ä½“16x32,æ¨ªå‘çºµå‘å‡æ­£å¸¸\n*line\n*feedline 1\n",
						//ÔİÍ£Ê±¼ä:pauseÃüÁî¡¢NLPRNOVERÃüÁî					
						"!NLFONT 9 12\n*text l ä»¥ä¸‹æ‰“å°å›¾ç‰‡åŠæ–‡å­—,å°†æš‚åœ5såæ‰“å°\n!NLPRNOVER\n*pause 5\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*image c 128*64 #ums\n*text c é“¶è”å•†åŠ¡-å…¨æ°‘ä»˜äº¤æ˜“å‡­æ¡\n!hz l\n!asc l\n*text c (å¤§å­—ä½“)å®¢æœç”µè¯:95534\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text l ä»¥åç§»256æ‰“å°å›¾ç‰‡\n!NLPRNOVER\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*image x:256 128*64 #ums\n*line\n*feedline 1\n",//æ–°å¢åç§»x:256æ‰“å°128æ¬¾åº¦çš„å›¾ç‰‡
						//ÍËÖ½
						"!NLFONT 9 12\n*text l è¿›é€€çº¸æµ‹è¯•ç¬¬ä¸€è¡Œæ–‡å­—\n*feedline 1\n!gray 5\n!yspace 6\n*text c ç¬¬äºŒè¡Œæ–‡å­—.æ‰“å°çš„æ–‡å­—æ²¡æœ‰ç›¸äº’è¦†ç›–\n*line\n*feedline 1\n",//*feedline -1\n
						"!gray 5\n!yspace 6\n*text c æœ¬è¡Œæ–‡å­—åº”å®Œå…¨åœ¨çº¸æ§½å¤–\n*feedline 5\n",//!NLPRNOVER\n	
						//ÌõĞÎÂë
						//ttfÌØÓĞ:!BARCODEÃüÁîºÍ*BARCODEÃüÁî£¬Ö§³Ö¶àÖÖ±àÂë£¬Ç°2¸ö²ÎÊı¸ú!barcodeÒ»Ñù£¬µÚÈı¸ö²ÎÊı±íÊ¾ÊÇ·ñÏÔÊ¾ÏÂ·½ÌáÊ¾ĞÅÏ¢£¬µÚËÄ¸ö²ÎÊı±íÊ¾ÌõÂë±àÂëÀàĞÍ
	                    			//0 caodebar; 1 code39; 2 code93; 3 code128; 4 EAN-8/EAN-13/JAN-8/JAN-13; 5 ITF; 6 UPCA; 7 UPCE
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„Codebaræ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:0123456789ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æœ‰ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 1 0\n*BARCODE c A0123456789C\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„Code39æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:ABCDEFGHIJKLMNOPQRSTUVWXYZ -.$/+%0123456789ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æœ‰ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 1 1\n*BARCODE c ABCDEFGHIJKLMNOPQRSTUVWXYZ -.$/+%0123456789\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„Code93æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:CD 45%+67$/ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æœ‰ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 1 2\n*BARCODE c CD 45%+67$\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„Code128æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:A0123456C%$#@ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æœ‰ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 1 3\n*BARCODE c A0123456C%$#@\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„EAN-8æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:12345670ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æ— ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 0 4\n*BARCODE c 1234567\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„EAN-13æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:1234567890128ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æ— ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 0 4\n*BARCODE c 123456789012\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„EAN-8æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:00001236ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æœ‰ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 1 4\n*BARCODE c 123\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„EAN-13æ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:0000123456784ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æœ‰ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 1 4\n*BARCODE c 12345678\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„ITFæ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:01234567890128ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æ— ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 0 5\n*BARCODE c 0123456789012\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„ITFæ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:00123456789050ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æœ‰ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 1 5\n*BARCODE c 12345678905\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„UPC-Aæ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:123456789012ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æ— ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 0 6\n*BARCODE c 12345678901\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„UPC-Aæ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:000765432108ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æœ‰ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 1 6\n*BARCODE c 76543210\n*line\n*feedline 1\n",							
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦6é«˜åº¦120å±…ä¸­)è¯·æ‰«ææ‰“å°å‡ºçš„UPC-Eæ¡å½¢ç å†…å®¹æ˜¯å¦ä¸º:01234565ï¼Œå¹¶ä¸”æ¡ç ä¸‹æ–¹ã€æ— ã€‘æ˜¾ç¤ºæ¡ç ä¿¡æ¯\n!BARCODE 6 120 0 7\n*BARCODE c 0123456\n*line\n*feedline 1\n",
						//¶şÎ¬Âë
						//ttfÌØÓĞ:!QRCODEÃüÁîºÍ*QRCODEÃüÁî£¬Ö§³Ö¶àÖÖ±àÂë£¬Ç°2¸ö²ÎÊı¸ú!qrcodeÒ»Ñù£¬µÚ¶ş¸ö²ÎÊı½ö¶ÔQRCODEÓĞĞ§£¬µÚÈı¸ö²ÎÊı±íÊ¾¶şÎ¬Âë±àÂëÀàĞÍ
						//0 Data Matrix;  1 Matrixcode; 2 PDF417; 3 QRCODE
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦200å±…ä¸­,ç¼–ç æ–¹å¼Data Matrix)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:Aa12%#@*&Î±Î’Î³Ã ÃºÇ’\n!QRCODE 200 0 0\n*QRCODE c Aa12%#@*&Î±Î’Î³Ã ÃºÇ’\n*line\n*feedline 1\n",//ĞÂÔöÒÔ½Ï´ó¸ß¶ÈµÄ¶şÎ¬Âë¿ªÊ¼´ò ÔÙ´òÓ¡¸ß¶ÈĞ¡µÄ
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦200å±…ä¸­,ç¼–ç æ–¹å¼Maxicode)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:ABC~*&%#!56789DEFGH\n!QRCODE 200 0 1\n*QRCODE c ABC~*&%#!56789DEFGH\n*line\n*feedline 1\n",//ĞÂÔöÒÔ½Ï´ó¸ß¶ÈµÄ¶şÎ¬Âë¿ªÊ¼´ò ÔÙ´òÓ¡¸ß¶ÈĞ¡µÄ
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦200å±…ä¸­,ç¼–ç æ–¹å¼PDF417)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:PDFæµ‹è¯•æ”¯æŒä¸­æ–‡è¾ƒå·®\n!QRCODE 200 0 2\n*QRCODE c PDFæµ‹è¯•æ”¯æŒä¸­æ–‡è¾ƒå·®\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦200å±…ä¸­,ç¼–ç æ–¹å¼QR Code)è¯·æ‰«ææ‰“å°å‡ºçš„äºŒç»´ç å†…å®¹æ˜¯å¦ä¸º:QRCODEæµ‹è¯•æ”¯æŒä¸­æ–‡å¥½\n!QRCODE 200 2 3\n*QRCODE c QRCODEæµ‹è¯•æ”¯æŒä¸­æ–‡å¥½\n*line\n*feedline 1\n",
						//ttfÌØÓĞ:*TEXTÖ¸Áî£¬ÓÃ·¨Óë*textÖ¸ÁîÒ»Ñù£¬Çø±ğÊÇ*text»á»»ĞĞ£¬*TEXT²»»á»»ĞĞ
						//ttfÌØÓĞ:*UNDERLINEÖ¸Áî£¬ÓÃ·¨Óë*underlineÖ¸ÁîÒ»Ñù£¬Çø±ğÊÇ*underline»á»»ĞĞ£¬*UNDERLINE²»»á»»ĞĞ
						//"!NLFONT 2 1 3\n*TEXT l æœ¬è¡Œæ–‡å­—Test\n!NLFONT 9 12 3\n*text r æ–‡å­—å±…å³å¹¶ä¸”æœ¬è¡Œä¸æ¢è¡ŒTest\n",
						"!NLFONT 2 1 3\n*TEXT l æœ¬è¡Œæ–‡å­—Test\n!NLFONT 9 12 3\n*text r ä¸æ¢è¡ŒTest\n",
						"!NLFONT 9 12 3\n*UNDERLINE l æœ¬è¡Œæ–‡å­—å¸¦ä¸‹åˆ’çº¿\n*underline r å¹¶ä¸”ä¸æ¢è¡ŒTest\n",
						"!NLFONT 9 12 3\n*TEXT l å·¦è¾¹left\n!NLFONT 2 1 3\n*UNDERLINE c ä¸‹åˆ’çº¿center\n!NLFONT 6 4 3\n*text r å³è¾¹right\n",
						//Ó¢ÎÄºáÏò·Å´ó×İÏò·Å´óÖ®Ç°´æÔÚÒ»¸öÓ¢ÎÄ×ÖÄ¸»á·Ö¿ª2±ßµÄÎÊÌâ
						"!NLFONT 9 12 0\n*text r é å³æ‰“å°æ–‡å­—,æ¨ªå‘æ”¾å¤§ã€çºµå‘æ”¾å¤§,å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n",
						"!NLFONT 9 12 0\n*text r To print text,transverse magnification,vertical magnification\n*line\n",
					  };
	#if 0 //err_strµÄGB2312¸ñÊ½£¬·½±ãÎÊÌâ¶¨Î»
	char *err_str[] = {
						"!NLFONT 9 12\n*text c ÒÔÏÂ½øĞĞÒì³£²âÊÔ,ºöÂÔ´òÓ¡Ğ§¹û,²»ÅÜ·É¼´¿É:´òÓ¡ºº×Ö»Ò¶ÈÖµÉèÖÃ0\n!hz n\n!gray 0\n!yspace 10\n*text l ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×Ö»Ò¶ÈÖµÉèÖÃ11\n!hz n\n!gray 11\n!yspace 10\n*text l ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ºº×ÖĞĞ¼ä¾àÉèÖÃ70\n!hz n\n!gray 5\n!yspace 70\n*text l ¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú¹ú\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È0¸ß¶È64¾ÓÖĞ)\n!gray 5\n!yspace 6\n!barcode 0 64\n*barcode c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È9¸ß¶È64¾ÓÖĞ)\n!barcode 9 64\n*barcode c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È4¸ß¶È0¾ÓÖĞ)\n!barcode 4 0\n*barcode c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È4¸ß¶È400¾ÓÖĞ)\n!barcode 4 400\n*barcode c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È128¾ÓÖĞ,ÉèÖÃ¸ß¶Èºó»¹ÓĞ¿Õ¸ñ)\n!barcode 2 128 \n*barcode c 0123456789ABC\n*line\n*feedline 1\n",//ÉèÖÃ¸ß¶ÈÖ®ºó»¹ÓĞ¿Õ¸ñ
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¾À´í¼¶±ğ²ÎÊıÎª4)\n!qrcode 100 4\n*qrcode c ABC123456789DEFGH\n*line\n*feedline 1\n",					
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¾À´í¼¶±ğ²ÎÊıÎª-1)\n!qrcode 100 -1\n*qrcode c ABC123456789DEFGH\n*line\n*feedline 1\n!NLPRNOVER\n",
						//"!NLFONT 9 12\n*text l ÒÔÆ«ÒÆ257´òÓ¡Í¼Æ¬,Ô¤ÆÚÎŞÍ¼Æ¬,º¯Êı·µ»Ø-1\n!NLPRNOVER\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*image x:257 128*64 #ums\n*line\n*feedline 1\n",//ĞÂÔöÆ«ÒÆx:257´òÓ¡128¿î¶ÈµÄÍ¼Æ¬£¬³¬¹ı384
						//ĞÂÔöBARCODEºÍQRCODEÒì³£Çé¿ö
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È0¸ß¶È64¾ÓÖĞ)\n!gray 5\n!yspace 6\n!BARCODE 0 64 3\n*BARCODE c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È9¸ß¶È64¾ÓÖĞ)\n!BARCODE 9 64 3\n*BARCODE c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È4¸ß¶È0¾ÓÖĞ)\n!BARCODE 4 0 3\n*BARCODE c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È4¸ß¶È328¾ÓÖĞ)\n!BARCODE 4 328 3\n*BARCODE c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È128¾ÓÖĞ,ÉèÖÃ±àÂë·½Ê½ºó»¹ÓĞ¿Õ¸ñ)\n!BARCODE 2 128 3 \n*BARCODE c 0123456789ABC\n*line\n*feedline 1\n",//ÉèÖÃ¸ß¶ÈÖ®ºó»¹ÓĞ¿Õ¸ñ
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¾À´í¼¶±ğ²ÎÊıÎª4)\n!QRCODE 100 4 3\n*QRCODE c ABC123456789DEFGH\n*line\n*feedline 1\n",					
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¾À´í¼¶±ğ²ÎÊıÎª-1)\n!QRCODE 100 -1 3\n*QRCODE c ABC123456789DEFGH\n*line\n*feedline 1\n!NLPRNOVER\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,±àÂë·½Ê½CODEBAR,Êı¾İ¸ñÊ½²»·ûºÏ±àÂë¹æ·¶)\n!BARCODE 2 64 0\n*BARCODE c 123456784\n*line\n*feedline 1\n",						
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,±àÂë·½Ê½CODE39,Êı¾İ¸ñÊ½²»·ûºÏ±àÂë¹æ·¶)\n!BARCODE 2 64 1\n*BARCODE c !#*123456784\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,±àÂë·½Ê½CODE93,Êı¾İ¸ñÊ½²»·ûºÏ±àÂë¹æ·¶)\n!BARCODE 2 64 2\n*BARCODE c !#*123456784\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,±àÂë·½Ê½EAN-8/JAN-8,Êı¾İĞ£ÑéÎ»´íÎó)\n!BARCODE 2 64 4\n*BARCODE c 34569837\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,±àÂë·½Ê½EAN-13/JAN-13,Êı¾İĞ£ÑéÎ»´íÎó)\n!BARCODE 2 64 4\n*BARCODE c 0123456789019\n*line\n*feedline 1\n",			
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,±àÂë·½Ê½EAN/JAN,Êı¾İ³¤¶È´íÎó)\n!BARCODE 2 64 4\n*BARCODE c 012345678901234567\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,±àÂë·½Ê½ITF,Êı¾İ³¤¶È´íÎó)\n!BARCODE 2 64 5\n*BARCODE c 3801234567899123456\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,±àÂë·½Ê½UPC-A,Êı¾İ³¤¶È´íÎó)\n!BARCODE 2 64 6\n*BARCODE c 3801234567899123456\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,±àÂë·½Ê½UPC-E,Êı¾İ³¤¶È´íÎó)\n!BARCODE 2 64 7\n*BARCODE c 01234567890\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,±àÂë·½Ê½UPC-E,µÚÒ»Î»Êı¾İÄÚÈİ´íÎó)\n!BARCODE 2 64 7\n*BARCODE c 21234569\n*line\n*feedline 1\n",					
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,±àÂë·½Ê½ÉèÖÃ´íÎó)\n!BARCODE 2 64 8\n*BARCODE c 21234569\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡ÌõĞÎÂë(¿í¶È2¸ß¶È64¾ÓÖĞ,±àÂë·½Ê½ÉèÖÃ´íÎó)\n!BARCODE 2 64 -1\n*BARCODE c 21234569\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È200¾ÓÓÒ,±àÂë·½Ê½ÉèÖÃ´íÎó)\n!QRCODE 200 3 4\n*QRCODE r ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ÒÔÏÂ´òÓ¡¶şÎ¬Âë(¸ß¶È200¾ÓÓÒ,±àÂë·½Ê½ÉèÖÃ´íÎó)\n!QRCODE 200 3 -1\n*QRCODE r ABC123456789DEFGH\n*line\n*feedline 1\n",
						
					  };
	#endif
	char *err_str[] = {
						"!NLFONT 9 12\n*text c ä»¥ä¸‹è¿›è¡Œå¼‚å¸¸æµ‹è¯•,å¿½ç•¥æ‰“å°æ•ˆæœ,ä¸è·‘é£å³å¯:æ‰“å°æ±‰å­—ç°åº¦å€¼è®¾ç½®0\n!hz n\n!gray 0\n!yspace 10\n*text l å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—ç°åº¦å€¼è®¾ç½®11\n!hz n\n!gray 11\n!yspace 10\n*text l å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ±‰å­—è¡Œé—´è·è®¾ç½®70\n!hz n\n!gray 5\n!yspace 70\n*text l å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½å›½\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦0é«˜åº¦64å±…ä¸­)\n!gray 5\n!yspace 6\n!barcode 0 64\n*barcode c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦9é«˜åº¦64å±…ä¸­)\n!barcode 9 64\n*barcode c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦4é«˜åº¦0å±…ä¸­)\n!barcode 4 0\n*barcode c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦4é«˜åº¦400å±…ä¸­)\n!barcode 4 400\n*barcode c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦128å±…ä¸­,è®¾ç½®é«˜åº¦åè¿˜æœ‰ç©ºæ ¼)\n!barcode 2 128 \n*barcode c 0123456789ABC\n*line\n*feedline 1\n",//ÉèÖÃ¸ß¶ÈÖ®ºó»¹ÓĞ¿Õ¸ñ
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (çº é”™çº§åˆ«å‚æ•°ä¸º4)\n!qrcode 100 4\n*qrcode c ABC123456789DEFGH\n*line\n*feedline 1\n",					
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (çº é”™çº§åˆ«å‚æ•°ä¸º-1)\n!qrcode 100 -1\n*qrcode c ABC123456789DEFGH\n*line\n*feedline 1\n!NLPRNOVER\n",
						//"!NLFONT 9 12\n*text l ä»¥åç§»257æ‰“å°å›¾ç‰‡,é¢„æœŸæ— å›¾ç‰‡,å‡½æ•°è¿”å›-1\n!NLPRNOVER\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*image x:257 128*64 #ums\n*line\n*feedline 1\n",//æ–°å¢åç§»x:257æ‰“å°128æ¬¾åº¦çš„å›¾ç‰‡ï¼Œè¶…è¿‡384
						//ĞÂÔöBARCODEºÍQRCODEÒì³£Çé¿ö
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦0é«˜åº¦64å±…ä¸­)\n!gray 5\n!yspace 6\n!BARCODE 0 64 3\n*BARCODE c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦9é«˜åº¦64å±…ä¸­)\n!BARCODE 9 64 3\n*BARCODE c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦4é«˜åº¦0å±…ä¸­)\n!BARCODE 4 0 3\n*BARCODE c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦4é«˜åº¦328å±…ä¸­)\n!BARCODE 4 328 3\n*BARCODE c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦128å±…ä¸­,è®¾ç½®ç¼–ç æ–¹å¼åè¿˜æœ‰ç©ºæ ¼)\n!BARCODE 2 128 3 \n*BARCODE c 0123456789ABC\n*line\n*feedline 1\n",//ÉèÖÃ¸ß¶ÈÖ®ºó»¹ÓĞ¿Õ¸ñ
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (çº é”™çº§åˆ«å‚æ•°ä¸º4)\n!QRCODE 100 4 3\n*QRCODE c ABC123456789DEFGH\n*line\n*feedline 1\n",					
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (çº é”™çº§åˆ«å‚æ•°ä¸º-1)\n!QRCODE 100 -1 3\n*QRCODE c ABC123456789DEFGH\n*line\n*feedline 1\n!NLPRNOVER\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,ç¼–ç æ–¹å¼CODEBAR,æ•°æ®æ ¼å¼ä¸ç¬¦åˆç¼–ç è§„èŒƒ)\n!BARCODE 2 64 0\n*BARCODE c 123456784\n*line\n*feedline 1\n",						
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,ç¼–ç æ–¹å¼CODE39,æ•°æ®æ ¼å¼ä¸ç¬¦åˆç¼–ç è§„èŒƒ)\n!BARCODE 2 64 1\n*BARCODE c !#*123456784\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,ç¼–ç æ–¹å¼CODE93,æ•°æ®æ ¼å¼ä¸ç¬¦åˆç¼–ç è§„èŒƒ)\n!BARCODE 2 64 2\n*BARCODE c !#*123456784\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,ç¼–ç æ–¹å¼EAN-8/JAN-8,æ•°æ®æ ¡éªŒä½é”™è¯¯)\n!BARCODE 2 64 4\n*BARCODE c 34569837\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,ç¼–ç æ–¹å¼EAN-13/JAN-13,æ•°æ®æ ¡éªŒä½é”™è¯¯)\n!BARCODE 2 64 4\n*BARCODE c 0123456789019\n*line\n*feedline 1\n",			
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,ç¼–ç æ–¹å¼EAN/JAN,æ•°æ®é•¿åº¦é”™è¯¯)\n!BARCODE 2 64 4\n*BARCODE c 012345678901234567\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,ç¼–ç æ–¹å¼ITF,æ•°æ®é•¿åº¦é”™è¯¯)\n!BARCODE 2 64 5\n*BARCODE c 3801234567899123456\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,ç¼–ç æ–¹å¼UPC-A,æ•°æ®é•¿åº¦é”™è¯¯)\n!BARCODE 2 64 6\n*BARCODE c 3801234567899123456\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,ç¼–ç æ–¹å¼UPC-E,æ•°æ®é•¿åº¦é”™è¯¯)\n!BARCODE 2 64 7\n*BARCODE c 01234567890\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,ç¼–ç æ–¹å¼UPC-E,ç¬¬ä¸€ä½æ•°æ®å†…å®¹é”™è¯¯)\n!BARCODE 2 64 7\n*BARCODE c 21234569\n*line\n*feedline 1\n",					
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,ç¼–ç æ–¹å¼è®¾ç½®é”™è¯¯)\n!BARCODE 2 64 8\n*BARCODE c 21234569\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°æ¡å½¢ç (å®½åº¦2é«˜åº¦64å±…ä¸­,ç¼–ç æ–¹å¼è®¾ç½®é”™è¯¯)\n!BARCODE 2 64 -1\n*BARCODE c 21234569\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦200å±…å³,ç¼–ç æ–¹å¼è®¾ç½®é”™è¯¯)\n!QRCODE 200 3 4\n*QRCODE r ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c ä»¥ä¸‹æ‰“å°äºŒç»´ç (é«˜åº¦200å±…å³,ç¼–ç æ–¹å¼è®¾ç½®é”™è¯¯)\n!QRCODE 200 3 -1\n*QRCODE r ABC123456789DEFGH\n*line\n*feedline 1\n",
						
					  };
	uchar *rmbbuf=(uchar *)"\xA3\xA4\xA3\xA4";
	int len = 0;
	char tmp[256] = {0};
	char tmp1[200] = {0};
	char tmp2[] = "*line\n*feedline 1\n";

	//ÒÔÏÂÎªº¯ÊıÌå
	cls_printf("²âÊÔ%s...", TESTAPI); 

	//²âÊÔÇ°ÖÃ:3G »úÆ÷Í¼Æ¬Òª´æÔÚK21¶Ë4G »úÆ÷Í¼Æ¬´æÔÚ°²×¿¶Ë/mnt/sdcard/picture/(/data/share/ Â·¾¶ÒÑ²»ÄÜÓÃ)ËùÒÔ°ÑÂ·¾¶¾ù¸Ä³É´Ë 
	//²âÊÔËùÓĞÊÕ¼¯µ½µÄpngÍ¼Æ¬
	if(cls_show_msg("ÊÇ·ñÒÑ½«ËùÓĞpngÍ¼Æ¬ºÍumsÎÄ¼şÏÂÔØµ½pad¶Ë/mnt/sdcard/picture/Â·¾¶,½«DroidSansFallback.ttfÎÄ¼ş·ÅÖÃÔÚSD¿¨Â·¾¶ÏÂ,·ñ[ESC]ÍË³ö,ÊÇ[ÆäËû]¼ÌĞø")==ESC)
		return;
	//°ÑÍ¼Æ¬ÎÄ¼ş¿½±´µ½k21¶Ë(4G»úÆ÷¼´Ê¹ÓĞ´Ë²Ù×÷Ò²²»»áÓ°Ïì)
	if((nRet=NDK_CopyFileToSecMod(SOURCEFILE, DESTFILE))!=NDK_OK)
	{
		send_result("line %d:¿½±´Í¼Æ¬ÎÄ¼şÊ§°Ü(%d)",__LINE__, nRet);
		return;	
	}	
	//ÓÉÓÚumsÊı¾İÂ·¾¶±ØĞëÔÚ/data/share/Â·¾¶,Ê¹ÓÃÏµÍ³ÃüÁî°Ñums¿½µ½/data/share/Â·¾¶ÏÂ
	system("rm -rf  /data/share/ums");
	system("cp /mnt/sdcard/picture/ums  /data/share/ums");
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//³õÊ¼»¯Ê¹¿ªÊ¼ÉèÖÃµÄÅäÖÃÓĞĞ§ 
	{
		send_result("line %d:%s²âÊÔÊ§°Ü",__LINE__,TESTAPI);
		goto prnt_error;	
	}
	//¼ÓÔØ¿â
	JNI_dlload();
	//´òÓ¡ÄÚÈİ
	while(1)
	{
		if((nRet=NDK_PrnGetStatus(&emStatus))!=NDK_OK )
		{
			send_result("line %d:%s²âÊÔÊ§°Ü%d",__LINE__,TESTAPI,nRet);
			goto prnt_error;
		}
		if(emStatus != PRN_STATUS_BUSY)
			show_stopwatch(DISABLE, 0);
		
		switch(emStatus)
		{
		case PRN_STATUS_BUSY:
			if(nBusy==FALSE)
			{
				nOldtime = time(NULL);//´ÓµÚÒ»´ÎÊÕµ½Ã¦×´Ì¬¿ªÊ¼¼ÆÊ±
				nBusy = TRUE;
			}
			if((nDiff=time(NULL)-nOldtime)>MAXWAITTIME)
			{
				send_result("line %d:´òÓ¡»ú´¦ÓÚÃ¦×´Ì¬Ê±¼ä¹ı³¤", __LINE__);
				goto prnt_error;
			}
			show_stopwatch(ENABLE, nDiff);
			continue;
			break;

		case PRN_STATUS_OK:
			nBusy=FALSE;
			//case1:ĞÂÔö!fontÃüÁî£¬ÓÃÓÚ¼ÓÔØ/system/fonts/Ä¿Â¼ÏÂµÄ×ÖÌå
			for(i=0;i<SZ_ARRAY(ttf_str);i++)
			{
				if((nRet = ttf_script_print(ttf_str[i]))!= NDK_OK)					
				{
					send_result("line %d:%s²âÊÔÊ§°Ü(%d,%d)",__LINE__,TESTAPI,nRet,i);
					goto prnt_error;
				}
				else 
				{
					if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
						cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
				}
			}
			
			//case2:¸÷ÖÖÊäÈë²ÎÊı²âÊÔ
			for(i=0;i<SZ_ARRAY(buf_str);i++)
			{
				if((nRet = ttf_script_print(buf_str[i]))!= NDK_OK)					
				{
					send_result("line %d:%s²âÊÔÊ§°Ü(%d,%d)",__LINE__,TESTAPI,nRet,i);
					goto prnt_error;
				}
				else
				{
					if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
						cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
				}
			}
			
			//case3:´òÓ¡¸÷ÖÖ×ÖÌåµÄºº×ÖºÍÈËÃñ±Ò·ûºÅ,×¢Òâ²»ÊÇËùÓĞµÄ×ÖÌå¶¼Ö§³Ö,²»Ö§³ÖµÄ×ÖÌåÔ¤ÆÚ´òÓ¡Ğ§¹ûÍ¬×îºóÒ»´ÎÉúĞ§µÄ×ÖÌåÒ»Ñù,Ö§³Ö×ÖÌå
			for(i = 0;i<SZ_ARRAY(hzfont);i++)
			{
				memset(tmp, 0, sizeof(tmp));
				sprintf(tmp, "!NLFONT %d 12 3", hzfont[i]);
				len = strlen(tmp);
				memcpy(&tmp[len], "\x0a", 1);
				tmp[len+1] = '\0';
				if((nRet = ttf_script_print(tmp))!= NDK_OK)
				{
					send_result("line %d:%s²âÊÔÊ§°Ü(%d,%d)",__LINE__,TESTAPI,nRet,i);
					goto prnt_error;
				}
				else
				{
					if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
						cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
				}
				memset(tmp, 0, sizeof(tmp));
				//sprintf(tmp, "*text c ÒÔ%s×ÖÌå¾ÓÖĞ´òÓ¡Á½ºáRMB·ûºÅ£¤£¤", hzfontstr[i]);
				sprintf(tmp, "*text c ä»¥%så­—ä½“å±…ä¸­æ‰“å°ä¸¤æ¨ªRMBç¬¦å·ï¿¥ï¿¥", hzfontstr[i]);
				len = strlen(tmp);
				memcpy(&tmp[len], "\x0a", 1);
				tmp[len+1] = '\0';
				if((nRet = ttf_script_print(tmp))!= NDK_OK)
				{					
					send_result("line %d:%s²âÊÔÊ§°Ü(%d,%d)",__LINE__,TESTAPI,nRet, i);
					goto prnt_error;
				}
				else
				{
					if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
						cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
				}
				if((nRet = ttf_script_print(tmp2))!= NDK_OK)
				{					
					send_result("line %d:%s²âÊÔÊ§°Ü(%d)",__LINE__,TESTAPI,nRet);
					goto prnt_error;
				}
				else
				{
					if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
						cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
				}
			}
			//case4:´òÓ¡Ê£ÓàµÄÎ÷ÎÄ×ÖÌå£¬ÓÃÓÚ²âÊÔ×Ö¿â(Ôö¼ÓÆäËü¹ú¼Ò»õ±Ò·ûºÅ²âÊÔ)
			for(i = 0;i<SZ_ARRAY(dotfont);i++)
			{
				memset(tmp, 0, sizeof(tmp));
				sprintf(tmp, "!NLFONT 9 %d 3", dotfont[i]);
				len = strlen(tmp);
				memcpy(&tmp[len], "\x0a", 1);
				tmp[len+1] = '\0';
				if((nRet = ttf_script_print(tmp))!= NDK_OK)
				{
					send_result("line %d:%s²âÊÔÊ§°Ü(%d,%d)",__LINE__,TESTAPI,nRet, i);
					goto prnt_error;
				}
				else
				{
					if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
						cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
				}
				memset(tmp, 0, sizeof(tmp));
				//sprintf(tmp, "*text c ÒÔÎ÷ÎÄ%s×ÖÌå¾ÓÖĞ(ÃÀÔª) print dollar symbol:$$", dotfontstr[i]);
				sprintf(tmp, "*text c ä»¥è¥¿æ–‡%så­—ä½“å±…ä¸­(ç¾å…ƒ) print dollar symbol:$$", dotfontstr[i]);
				len = strlen(tmp);
				memcpy(&tmp[len], "\x0a", 1);
				tmp[len+1] = '\0';
				if((nRet = ttf_script_print(tmp))!= NDK_OK)
				{
					send_result("line %d:%s²âÊÔÊ§°Ü(%d,%d)",__LINE__,TESTAPI,nRet, i);
					goto prnt_error;
				}
				else
				{
					if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
						cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
				}
				if((nRet = ttf_script_print(tmp2))!= NDK_OK)
				{
					send_result("line %d:%s²âÊÔÊ§°Ü(%d)",__LINE__,TESTAPI,nRet);
					goto prnt_error;
				}
				else
				{
					if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
						cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
				}
			}
			//case4.1:½øĞĞÄ¿Ç°ÊÕ¼¯µ½µÄÈ«²¿PNGÍ¼Æ¬²âÊÔ(°üÀ¨colorÏµÀà¡¢IHDRÏµÀà¡¢ÒÕÊõ×ÖÏµÀà£¬ÆäËûÏµÀà) 
			for(i=0;i<SZ_ARRAY(pngbuf);i++)
			{
				switch(pngbuf[i][0])
				{
					case 'y':
						p = 0;
						break;
					case 'c':
						p = 1;
						break;
					case 'I':
						p = 2;
						break;
					case 'o':
						p = 3;
						break;
					default:
						break;
				}
				memset(tmp, 0, sizeof(tmp));
				sprintf(tmp, "*image %s %s path:%s%s%s", type[p], whbuf[i], IMAGEPATH, pngbuf[i], PNGFORMAT);
				len = strlen(tmp);
				memcpy(&tmp[len], "\x0a", 1);
				tmp[len+1] = '\0';
				memset(tmp1, 0, sizeof(tmp1));
				//sprintf(tmp1, "*text c ÒÔÏÂ´òÓ¡PNGÍ¼Æ¬,ãĞÖµ×Ô¶¯(%s),Ğ§¹ûÇë¶ÔÕÕÊµ¼ÊÍ¼Æ¬,²ÊÉ«Í¼Æ¬Ç³ÑÕÉ«½«´òÓ¡³É°×É«£¬ÉîÑÕÉ«½«´òÓ¡³ÉºÚÉ«",position[p]);
				sprintf(tmp1, "*text c ä»¥ä¸‹æ‰“å°PNGå›¾ç‰‡,é˜ˆå€¼è‡ªåŠ¨(%s),æ•ˆæœè¯·å¯¹ç…§å®é™…å›¾ç‰‡,å½©è‰²å›¾ç‰‡æµ…é¢œè‰²å°†æ‰“å°æˆç™½è‰²ï¼Œæ·±é¢œè‰²å°†æ‰“å°æˆé»‘è‰²",position[p]);
				len = strlen(tmp1);
				memcpy(&tmp1[len], "\x0a", 1);
				tmp[len+1] = '\0';
				if((nRet = ttf_script_print("!NLFONT 9 12\n")) != NDK_OK)
				{			
					send_result("line %d:%s²âÊÔÊ§°Ü(%d,%d)",__LINE__,TESTAPI,nRet,i);
					goto prnt_error;	
				}
				else
				{
					if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
						cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
				}
				if((nRet = ttf_script_print(tmp1)) != NDK_OK)
				{			
					send_result("line %d:%s²âÊÔÊ§°Ü(%d,%d)",__LINE__,TESTAPI,nRet,i);
					goto prnt_error;	
				}
				else
				{
					if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
						cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
				}
				if((nRet = ttf_script_print(tmp))!= NDK_OK)
				{	
					send_result("line %d:%s²âÊÔÊ§°Ü(%d,%d)",__LINE__,TESTAPI,nRet,i);
					goto prnt_error;
				}
				else
				{
					if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
						cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
				}
				if((nRet = ttf_script_print(tmp2))!= NDK_OK)
				{	
					send_result("line %d:%s²âÊÔÊ§°Ü(%d,%d)",__LINE__,TESTAPI,nRet,i);
					goto prnt_error;
				}
				else
				{
					if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
						cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
				}
			}
			
			//case5:´òÓ¡ÃüÁîÉèÖÃÖĞ²ÎÊıÉèÖÃ´íÎóÊ±Ó¦¸Ã·µ»ØNDK_ERR_PARA
			for(i=0;i<SZ_ARRAY(err_str);i++)
			{
				if((nRet = ttf_script_print(err_str[i]))!= NDK_ERR_PARA)
				{
					cls_show_msg("line %d:%s²âÊÔÊ§°Ü(%d,%d)",__LINE__,TESTAPI,nRet,i);
					goto prnt_error;	
				}
			}
			
			//case6:Òì³£²ÎÊı²âÊÔ
			if((nRet = ttf_script_print(NULL))!=NDK_ERR) 
			{
				send_result("line %d:%s²âÊÔÊ§°Ü(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;
			}

			//case7:Æ«ÒÆx:257´òÓ¡128¿í¶ÈµÄÍ¼Æ¬£¬³¬¹ı384
			//if((nRet = ttf_script_print("!NLFONT 9 12\n*text l ÒÔÆ«ÒÆ257´òÓ¡Í¼Æ¬,Ô¤ÆÚÎŞÍ¼Æ¬,º¯Êı·µ»Ø-1\n!NLPRNOVER\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*image x:257 128*64 #ums\n*line\n*feedline 1\n"))!=NDK_ERR)
			if ((nRet = ttf_script_print("!NLFONT 9 12\n*text l ä»¥åç§»257æ‰“å°å›¾ç‰‡,é¢„æœŸæ— å›¾ç‰‡,å‡½æ•°è¿”å›-1\n!NLPRNOVER\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*image x:257 128*64 #ums\n*line\n*feedline 1\n"))!=NDK_ERR)
			{
				send_result("line %d:%s²âÊÔÊ§°Ü(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;
			}

			// case8:´«ÈëµÄÍ¼Æ¬·ÇPNGÀàĞÍ£¨±ÈÈçbmp»òjpg£©£¬Ó¦·µ»ØÊ§°Ü£¨-1£©
			//if((nRet = ttf_script_print("!NLFONT 9 12\n*text c ´òÓ¡Í¼Æ¬ÀàĞÍ·Ç·¨²âÊÔ£¬Ó¦´ò²»³öÍ¼Æ¬\n*image c 361*361 path:/mnt/sdcard/picture/xdl.jpg\n*line\n*feedline 1\n"))!=NDK_ERR) 
			if((nRet = ttf_script_print("!NLFONT 9 12\n*text c æ‰“å°å›¾ç‰‡ç±»å‹éæ³•æµ‹è¯•ï¼Œåº”æ‰“ä¸å‡ºå›¾ç‰‡\n*image c 361*361 path:/mnt/sdcard/picture/xdl.jpg\n*line\n*feedline 1\n"))!=NDK_ERR) 
			{
				send_result("line %d:%s²âÊÔÊ§°Ü(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;
			}
		
			//case9:!fontÃüÁî:´«Èë²»´æÔÚÎÄ¼ş£¬·µ»Ø-7
			if((nRet = ttf_script_print("!font err.ttf\n"))!=NDK_ERR_PATH) 
			{
				send_result("line %d:%s²âÊÔÊ§°Ü(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;
			}
			
			//case10: Òì³£²âÊÔ!fontÃüÁî £º´«ÈëÆäËûÂ·¾¶ttfÎÄ¼ş
			//if((nRet = ttf_script_print("!font /mnt/sdcard/DroidSansFallback.ttf\n*text l ´òÓ¡ÆäËûÂ·¾¶ÏÂttfÎÄ¼ş£¬±¾ĞĞÎÄ×ÖÒª³É¹¦´ò³ö\n*line\n*feedline 1\n"))!=NDK_OK) 
			if((nRet = ttf_script_print("!font /mnt/sdcard/DroidSansFallback.ttf\n*text l æ‰“å°å…¶ä»–è·¯å¾„ä¸‹ttfæ–‡ä»¶ï¼Œæœ¬è¡Œæ–‡å­—è¦æˆåŠŸæ‰“å‡º\n*line\n*feedline 1\n"))!=NDK_OK) 
			{
				send_result("line %d:%s²âÊÔÊ§°Ü(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;
			}
			else
			{
				if((nRet=getStatus()) == PRN_STATUS_NOPAPER)
					cls_show_msg("´òÓ¡»úÈ±Ö½,×°Ö½ºóÈÎÒâ¼ü¼ÌĞø");
			}

			//²âÊÔ½áÊø
			send_result("%s²âÊÔÍ¨¹ı", TESTAPI);
			goto prnt_error;
			break;
		case PRN_STATUS_NOPAPER:
			cls_printf("´òÓ¡»úÈ±Ö½...×°Ö½ºó,ÈÎÒâ¼ü¼ÌĞø,ESC·ÅÆú");
			lib_kbflush(); //ÇåKB BUF
			while ((nKeyin=lib_kbhit())==0)
			{
				NDK_SysBeep();
				//Õı³£×°Ö½
				NDK_PrnFeedByPixel(10);
				sleep(2);
			}
			if (nKeyin==ESC)
			{
				send_result("line %d:´òÓ¡»úÈ±Ö½,ÓÃ»§·ÅÆú", __LINE__);
				goto prnt_error;
			}
			NDK_PrnStart();
			cls_printf("²âÊÔ%s...", TESTAPI);
			break;
		case PRN_STATUS_OVERHEAT:
			send_result("line %d: ´òÓ¡»ú¹ıÈÈ", __LINE__);
			goto prnt_error;
			break;
		case PRN_STATUS_VOLERR:
			send_result("line %d: ´òÓ¡»úµçÔ´¹ÊÕÏ", __LINE__);
			goto prnt_error;
			break;
		case PRN_STATUS_DESTROYED:
			send_result("line %d: ´òÓ¡»úËğ»µ", __LINE__);
			goto prnt_error;
			break;
		default:
			//Î´Öª×´Ì¬£¬ÅĞÎª´í
			send_result("line %d: UNKNOWN STATE(tpstatus=%d)", __LINE__, emStatus);
			goto prnt_error;
			break;
		}
	
	}
	
prnt_error:
	NDK_PrnInit(g_PrnSwitch);
	return;
}


