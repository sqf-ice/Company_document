/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt24.c
* Author 		:  
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试 NDK_Script_Print脚本打印功能
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
#define TESTAPI	"NDK_Script_Print"	
#define MAXWAITTIME 90
#define DESTFILE "/appfs/ums"
#define SOURCEFILE "/mnt/sdcard/picture/ums"  //"/data/share/ums"  原先的路径无权限 20171101 modify
#define PNGFORMAT ".png"
#define IMAGEPATH "/mnt/sdcard/picture/"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 		:void prnt24(void)
* functional description 	: 
* input parameter	 		:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author		date		remarks
*			  	  	  	jiangym		20160606 created
*****************************************************************/
void prnt24(void)
{
	//以下为局部变量
#include	"dotfont.h"
#include	"hzfont.h"

	EM_PRN_STATUS emStatus = 0;
	int nBusy = FALSE, nKeyin = 0, i = 0;
	time_t nOldtime = 0, nDiff = 0;
	int nRet = 0, nRet1 = 0;
	char *type[] = {"l", "c", "r", "l"};
	char *position[] = {"居左", "居中", "居右", "居左"};
	int p = 0;
	char *pngbuf[] = {"ysz1", "ysz2", "ysz3", "ysz4", "ysz5", "ysz6", "ysz7", "color1", "color2", "color3", "color4", "color5", "color6",
					"IHDR1", "IHDR2", "IHDR3", "IHDR4", "IHDR5", "IHDR6", "IHDR7", "IHDR8", "IHDR9", "IHDR10", "IHDR11", "IHDR12", "IHDR13",
					"IHDR14", "IHDR15", "other1", "other2", "other3", "other4", "other5", "other6", "other7", "other8", "other9", "other10",
					"other11", "other12", "other13", "other14", "other15", "other16"};
	char *whbuf[] = {"284*52", "268*100", "292*52", "309*72", "211*93", "309*72", "302*112", "384*300", "384*300", "384*300", "384*300", "384*300", "384*300", "280*280", "310*136", "310*136", "315*164", 
					 "384*75", "384*241", "195*384", "384*144", "294*292", "300*191", "100*54", "300*200", "384*192", "315*164", "220*183", "315*164", "384*149", "327*87", "315*164", "220*165", "240*240",
					 "250*250", "99*99", "384*49", "328*112", "310*136", "205*86", "361*361", "361*361", "384*259", "384*49"};  //各png图片对应的宽和高
	char *buf_str[] = {
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度2高度120居右)请扫描打印出的条形码内容是否为:0123456789ABC\n!barcode 2 120\n*barcode r 0123456789ABC\n*line\n*feedline 1\n",//新增用例测试第一次调用脚本打印直接打印条码,中间不应该有空白行
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度8高度400居右)请扫描打印出的条形码内容是否为:0123456789ABC\n!barcode 8 400\n*barcode r 0123456789ABC\n*line\n*feedline 1\n",//新增用例测试条码宽度超过384,预期能够打印和正常扫描,或者返回失败
						"!NLFONT 9 12\n*text c 以下打印二维码(高度400居右)请扫描打印出的二维码内容是否为:ABC123456789DEFGH\n!qrcode 400 3\n*qrcode r ABC123456789DEFGH\n*line\n*feedline 1\n",//新增用例测试二维码宽度超过384,预期能够打印和正确扫描
						//"!NLFONT 9 12\n*text c 以下打印PNG图片,阈值默认(居左),注意打印效果\n*image l 240*240 path:/mnt/sdcard/picture/png03.png\n*line\n*feedline 1\n", //原先的路径无权限,故修改到/mnt/sdcard/ 20171101 modify 整合到所有的png图片打印中 20180307
						//"!NLFONT 9 12\n*text c 以下打印PNG图片,阈值默认(居中),注意打印效果\n*image c 220*165 path:/mnt/sdcard/picture/png02.png\n*line\n*feedline 1\n", 				
						//"!NLFONT 9 12\n*text c 以下打印PNG图片,阈值默认(居右),注意打印效果\n*image r 220*165 path:/mnt/sdcard/picture/png04.png\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印PNG图片,阈值255打印黑块(居右)\n*image r 384*240 path:yz:255;/mnt/sdcard/picture/png03.png\n*line\n*feedline 1\n",  //新增用例测试png图片功能使用png02和png03替换原来的logo图片
						"!NLFONT 9 12\n*text c 以下打印PNG图片,阈值默认(居中),注意打印效果\n*image c 384*49 path:/mnt/sdcard/picture/png08.png\n*line\n*feedline 1\n",//新增打印图片
						"!NLFONT 9 12\n*text c 以下打印压缩PNG图片,阈值自动，(100*200，居左)，不必关注打印效果\n*image l 100*200 path:/mnt/sdcard/picture/png02.png\n*line\n*feedline 1\n", 
						"!NLFONT 9 12\n*text c 以下打印压缩PNG图片,阈值自动，(100*200，居中)，不必关注打印效果\n*image c 100*200 path:/mnt/sdcard/picture/png03.png\n*line\n*feedline 1\n", 
						"!NLFONT 9 12\n*text c 以下打印压缩PNG图片,阈值自动，(100*200，居右)，不必关注打印效果\n*image r 100*200 path:/mnt/sdcard/picture/png04.png\n*line\n*feedline 1\n", //新增打印压缩的PNG图片
						"!NLFONT 9 12\n*text c 以下打印汉字样式小字体西方样式大字体居左带下划线\n!hz s\n!asc l\n!gray 5\n!yspace 6\n*underline l aBc34国国国国aBc34\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字样式标准字体西方样式标准字体居中带下划线\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*underline c aBc1234国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国aBc1234\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字样式大字体西方样式小字体居右带下划线\n!hz l\n!asc s\n!gray 5\n!yspace 6\n*underline r aBc1234国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国aBc1234\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字样式大字体西方样式小字体偏移16带下划线\n!hz l\n!asc s\n!gray 5\n!yspace 6\n*underline x:16 aBc1234国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国aBc1234\n*line\n*feedline 1\n",//新增偏移量打印x:16
						"!NLFONT 9 12\n*text c 以下打印汉字样式小字体,每行24个,居左\n!hz s\n!gray 5\n!yspace 6\n*text l 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字样式小字体,每行24个,偏移120像素\n!hz s\n!gray 5\n!yspace 6\n*text x:120 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",//新增偏移量打印x:120
						"!NLFONT 9 12\n*text c 以下打印汉字样式标准字体,每行16个,居中\n!hz n\n!gray 5\n!yspace 6\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字样式大字体,每行12个,居右\n!hz l\n!gray 5\n!yspace 6\n*text r 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字样式小字体宽度标准字体高度,居左\n!hz sn\n!gray 5\n!yspace 6\n*text l 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",		
						"!NLFONT 9 12\n*text c 以下打印汉字样式小字体宽度大字体高度,居中\n!hz sl\n!gray 5\n!yspace 6\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字样式标准字体宽度大字体高度,居右\n!hz nl\n!gray 5\n!yspace 6\n*text r 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印西方样式小字体,每行48个,居左\n!asc s\n!gray 5\n!yspace 6\n*text l sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印西方样式标准字体,每行32个,居中\n!asc n\n!gray 5\n!yspace 6\n*text c sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印西方样式大字体,每行24个,居右\n!asc l\n!gray 5\n!yspace 6\n*text r sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印西方样式小字体宽度标准字体高度,居左\n!asc sn\n!gray 5\n!yspace 6\n*text l sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印西方样式小字体宽度大字体高度,居中\n!asc sl\n!gray 5\n!yspace 6\n*text c sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印西方样式标准字体宽度大字体高度,居右\n!asc nl\n!gray 5\n!yspace 6\n*text r sssssssssssssssssssssssssssssssssssssssssssssssssssssss\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字灰度值为1\n!hz n\n!gray 1\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字灰度值为3\n!hz n\n!gray 3\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字灰度值为7\n!hz n\n!gray 7\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字灰度值为10\n!hz n\n!gray 10\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字灰度值为5\n!hz n\n!gray 5\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",//灰度值5放后面是为了恢复默认值5
						"!NLFONT 9 12\n*text c 以下打印汉字行间距为0\n!hz n\n!yspace 0\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字行间距为10\n!hz n\n!yspace 10\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字行间距为20\n!hz n\n!yspace 20\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字行间距为30\n!hz n\n!yspace 30\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字行间距为40\n!hz n\n!yspace 40\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字行间距为50\n!hz n\n!yspace 50\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字行间距为60\n!hz n\n!yspace 60\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字行间距为6\n!hz n\n!yspace 6\n*text c 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",//恢复成默认值行间距6
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度2高度16居右)请扫描打印出的条形码内容是否为:0123456789ABC\n!barcode 2 16\n*barcode r 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度4高度128居中)请扫描打印出的条形码内容是否为:123456\n!barcode 4 128\n*barcode c 123456\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度4高度128偏移50)请扫描打印出的条形码内容是否为:123456\n!barcode 4 128\n*barcode x:50 123456\n*line\n*feedline 1\n",//新增偏移量打印x:50
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度8高度320居左)请扫描打印出的条形码内容是否为:123\n!barcode 8 320\n*barcode l 123\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度1高度64居中,纯字符31个),不要求能够扫描出来\n!barcode 1 64\n*barcode c AAAAAAAAAABBBBBBBBBBCCCCCCCCCCD\n*line\n*feedline 1\n",//新增可以扫描的字符数的边界测试,宽度为1:纯字符31个,纯数字60,宽度为2:纯字符14个,纯数字28个
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度1高度64居中,纯数字60个),不要求能够扫描出来\n!barcode 1 64\n*barcode c 012345678901234567890123456789012345678901234567890123456789\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度2高度64居中,纯数字28个)请扫描打印出的条形码内容是否为:0123456789012345678901234567\n!barcode 2 64\n*barcode c 0123456789012345678901234567\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度2高度64居中,纯字母14个)请扫描打印出的条形码内容是否为:AAAAAAAAAABBBB\n!barcode 2 64\n*barcode c AAAAAAAAAABBBB\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度2高度64居中,字母与数字组合)请扫描打印出的条形码内容是否为:01AB23BC34C\n!barcode 2 64\n*barcode c 01AB23BC34C\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度2高度64居中,字母与数字组合)请扫描打印出的条形码内容是否为:A1B2C3D4E5F6G7\n!barcode 2 64\n*barcode c A1B2C3D4E5F6G7\n*line\n*feedline 1\n",	 						
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度2高度64居中,字母与数字组合)请扫描打印出的条形码内容是否为:ABC123CDE4567\n!barcode 2 64\n*barcode c ABC123CDE4567\n*line\n*feedline 1\n",							
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度2高度64居中,纯数字32个(超过28,宽度会自动降为1)),不要求能够扫描出来\n!barcode 2 64\n*barcode c 01234567890123456789012345678912\n*line\n*feedline 1\n",//恢复默认宽度2,高度64(超过28个,宽度会自动降为1)
						"!NLFONT 9 12\n*text c 以下打印二维码(高度400居右)请扫描打印出的二维码内容是否为:ABC123456789DEFGH\n!qrcode 400 3\n*qrcode r ABC123456789DEFGH\n*line\n*feedline 1\n",//新增以较大高度的二维码开始打 再打印高度小的
						"!NLFONT 9 12\n*text c 以下打印二维码(高度10居左)请扫描打印出的二维码内容是否为:ABC123456789DEFGH\n!qrcode 10 0\n*qrcode l ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印二维码(高度50居中)请扫描打印出的二维码内容是否为:ABC123456789DEFGH\n!qrcode 50 1\n*qrcode c ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印二维码(高度50偏移50)请扫描打印出的二维码内容是否为:ABC123456789DEFGH\n!qrcode 50 1\n*qrcode x:50 ABC123456789DEFGH\n*line\n*feedline 1\n",//新增偏移量打印x:50
						"!NLFONT 9 12\n*text c 以下打印二维码(高度200居右)请扫描打印出的二维码内容是否为:ABC123456789DEFGH\n!qrcode 200 3\n*qrcode r ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印二维码(高度100居中)请扫描打印出的二维码内容是否为:ABC123456789DEFGH\n!qrcode 100 2\n*qrcode c ABC123456789DEFGH\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印二维码(高度300居中)请扫描打印出的二维码内容是否为:400字节的A\n!qrcode 300 1\n*qrcode c AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印二维码(高度300居中)请扫描打印出的二维码内容是否为:401字节的A\n!qrcode 300 1\n*qrcode c AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印图片(居左)\n*image l 32*64 #ums\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印图片(居中)\n*image c 64*128 #ums\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印图片(居右)\n*image r 128*64 #ums\n*line\n*feedline 1\n",
						"!hz n\n!asc n!gray 7\n!yspace 6\n*text c 以下打印两条分隔符\n*line\n*line\n*feedline 1\n",
						
						"!NLFONT 9 12 0\n*text l 靠左打印的中文为自有宋体24x24,西文为Gulimche字体12x24,横向纵向均放大\n*line\n*feedline 1\n",
						"!NLFONT 10 13 1\n*text l 靠左打印的中文为仿宋24x24,西文为MSGothic粗体16*32,横向放大纵向正常\n*line\n*feedline 1\n",
						"!NLFONT 11 17 2\n*text l 靠左打印的中文为楷体24*24,西文为楷体粗体12x16,横向正常纵向放大\n*line\n*feedline 1\n",
						"!NLFONT 13 19 3\n*text l 靠左打印的中文为宋体12x12,西文为楷体粗体16x32,横向纵向均正常\n*line\n*feedline 1\n",
						//以下为20160920新增用例
						"!NLFONT 9 12 0\n*text c 居中打印的中文为自有宋体24x24,西文为Gulimche字体12x24,横向纵向均放大\n*line\n*feedline 1\n",
						"!NLFONT 10 13 1\n*text c 居中打印的中文为仿宋24x24,西文为MSGothic粗体16*32,横向放大纵向正常\n*line\n*feedline 1\n",
						"!NLFONT 11 17 2\n*text c 居中打印的中文为楷体24*24,西文为楷体粗体12x16,横向正常纵向放大\n*line\n*feedline 1\n",
						"!NLFONT 13 19 3\n*text c 居中打印的中文为宋体12x12,西文为楷体粗体16x32,横向纵向均正常\n*line\n*feedline 1\n",
						"!NLFONT 9 12 0\n*text r 靠右打印的中文为自有宋体24x24,西文为Gulimche字体12x24,横向纵向均放大\n*line\n*feedline 1\n",
						"!NLFONT 10 13 1\n*text r 靠右打印的中文为仿宋24x24,西文为MSGothic粗体16*32,横向放大纵向正常\n*line\n*feedline 1\n",
						"!NLFONT 11 17 2\n*text r 靠右打印的中文为楷体24*24,西文为楷体粗体12x16,横向正常纵向放大\n*line\n*feedline 1\n",
						"!NLFONT 13 19 3\n*text r 靠右打印的中文为宋体12x12,西文为楷体粗体16x32,横向纵向均正常\n*line\n*feedline 1\n",
						"!NLFONT 0 0 3\n*text r 靠右打印的中文为宋体12x12,西文为楷体粗体16x32,横向纵向均正常\n*line\n*feedline 1\n",
						
						"!NLFONT 9 12\n*text l 以下打印图片及文字,将暂停5s后打印\n!NLPRNOVER\n*pause 5\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*image c 128*64 #ums\n*text c 银联商务-全民付交易凭条\n!hz l\n!asc l\n*text c (大字体)客服电话:95534\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text l 以偏移256打印图片\n!NLPRNOVER\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*image x:256 128*64 #ums\n*line\n*feedline 1\n",//新增偏移x:256打印128款度的图片
						"!NLFONT 9 12\n*text l 进退纸测试第一行文字\n*feedline 1\n!gray 5\n!yspace 6\n*text c 第二行文字.打印的文字没有相互覆盖\n*line\n*feedline 1\n",//*feedline -1\n
						"!gray 5\n!yspace 6\n*text c 本行文字应完全在纸槽外\n*feedline 5\n",//!NLPRNOVER\n	
						// 新增打印默认字体 20180419  
						//字体样式为0为默认值，若设为0默认，则这次打印的字体为上一次打印的字体，若为第一次打印，则0为默认字体，汉字默认字体为24*24，英文默认字体为12*12
						//!hz s\n!+!NLFONT 0会导致汉字打印异常，且默认出错后，再使用默认仍会是错的，除非打一次正常的非默认的
						"!hz s\n!asc s\n!NLFONT 0 0 3\n*text c 以下进行汉字与英文默认字体测试,居中打印汉字大小为小字体,英文大小为小字体,字体样式为汉字默认(24*24)英文默认(12*12)打印模式为横向正常纵向正常,English\n*line\n",
						"!hz s\n!NLFONT 0 1 3\n*text c 以下进行汉字默认字体测试,居中打印汉字大小为小字体,字体样式为汉字默认(24*24)英文(8*16)打印模式为横向正常纵向正常,English\n*line\n",
						"!asc s\n!NLFONT 1 0 3\n*text c 以下进行英文默认字体测试,居中打印英文大小为小字体,字体样式为汉字(24*24)英文默认(12*12)打印模式为横向正常纵向正常,English\n*line\n",
						"!hz s\n!asc s\n!NLFONT 1 1 3\n*text c 以下进行字体测试,居中打印汉字大小为小字体,字体样式为汉字(24*24)英文(8*16)打印模式为横向正常纵向正常,English\n*line\n",
						"!hz s\n!asc s\n!NLFONT 0 0 3\n*text c 以下进行汉字与英文默认字体测试,居中打印英文大小为小字体,字体样式为汉字默认(24*24)英文默认(12*12)打印模式为横向正常纵向正常,English\n*line\n",
						"!hz s\n!asc s\n!NLFONT 1 1 3\n*text c 以下进行字体测试,居中打印汉字大小为小字体,字体样式为汉字（24*24）英文（8*16）打印模式为横向正常纵向正常,English\n*line\n",
					  };
	char *err_str[] = {
						"!NLFONT 9 12\n*text c 以下进行异常测试,忽略打印效果,不跑飞即可:打印汉字灰度值设置0\n!hz n\n!gray 0\n!yspace 10\n*text l 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字灰度值设置11\n!hz n\n!gray 11\n!yspace 10\n*text l 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印汉字行间距设置70\n!hz n\n!gray 5\n!yspace 70\n*text l 国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国国\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度0高度64居中)\n!gray 5\n!yspace 6\n!barcode 0 64\n*barcode c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度9高度64居中)\n!barcode 9 64\n*barcode c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度4高度0居中)\n!barcode 4 0\n*barcode c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度4高度328居中)\n!barcode 4 328\n*barcode c 0123456789ABC\n*line\n*feedline 1\n",
						"!NLFONT 9 12\n*text c 以下打印条形码(宽度2高度128居中,设置高度后还有空格)\n!barcode 2 128 \n*barcode c 0123456789ABC\n*line\n*feedline 1\n",//设置高度之后还有空格
						"!NLFONT 9 12\n*text c 以下打印二维码(纠错级别参数为4)\n!qrcode 100 4\n*qrcode c ABC123456789DEFGH\n*line\n*feedline 1\n",					
						"!NLFONT 9 12\n*text c 以下打印二维码(纠错级别参数为-1)\n!qrcode 100 -1\n*qrcode c ABC123456789DEFGH\n*line\n*feedline 1\n!NLPRNOVER\n",
						"!NLFONT 9 12\n*text l 以偏移257打印图片,预期无图片,函数返回-1\n!NLPRNOVER\n!hz n\n!asc n\n!gray 5\n!yspace 6\n*image x:257 128*64 #ums\n*line\n*feedline 1\n",//新增偏移x:257打印128款度的图片，超过384
					  };
	uchar *rmbbuf=(uchar *)"\xA3\xA4\xA3\xA4";
	int len = 0;
	char tmp[256] = {0};
	char tmp1[200] = {0};
	char tmp2[] = "*line\n*feedline 1\n";

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:3G 机器图片要存在K21端4G 机器图片存在安卓端/mnt/sdcard/picture/(/data/share/ 路径已不能用20171101 modify)所以把路径均改成此 
	//新增测试所有收集到的png图片 20180307 sull add
	if(cls_show_msg("是否已将所有png图片和ums文件下载到pad端/mnt/sdcard/picture/路径,否[ESC]退出,是[其他]继续")==ESC)
		return;
	//把图片文件拷贝到k21端(4G机器即使有此操作也不会影响)
	if((nRet=NDK_CopyFileToSecMod(SOURCEFILE, DESTFILE))!=NDK_OK)
	{
		send_result("line %d:拷贝图片文件失败(%d)",__LINE__, nRet);
		return;	
	}	
	//由于ums数据路径必须在/data/share/路径,使用系统命令把ums拷到/data/share/路径下20171101 modify
	system("rm -rf  /data/share/ums");
	system("cp /mnt/sdcard/picture/ums  /data/share/ums");
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//初始化使开始设置的配置有效 
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;	
	}
	
	while(1)
	{
		if((nRet=NDK_PrnGetStatus(&emStatus))!=NDK_OK )
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI,nRet);
			goto prnt_error;
		}
		if(emStatus != PRN_STATUS_BUSY)
			show_stopwatch(DISABLE, 0);
		
		switch(emStatus)
		{
		case PRN_STATUS_BUSY:
			if(nBusy==FALSE)
			{
				nOldtime = time(NULL);//从第一次收到忙状态开始计时
				nBusy = TRUE;
			}
			if((nDiff=time(NULL)-nOldtime)>MAXWAITTIME)
			{
				send_result("line %d:打印机处于忙状态时间过长", __LINE__);
				goto prnt_error;
			}
			show_stopwatch(ENABLE, nDiff);
			continue;
			break;

		case PRN_STATUS_OK:
			nBusy=FALSE;
			//case1:各种输入参数测试
			for(i=0;i<SZ_ARRAY(buf_str);i++)
			{
				if((nRet = NDK_Script_Print(buf_str[i],strlen(buf_str[i])))!= NDK_OK)
				{
					if(nRet == PRN_STATUS_NOPAPER)
						cls_show_msg("打印机缺纸,装纸后任意键继续");
					else
					{
						send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
						goto prnt_error;
					}
				}
			}
			//case4:新增打印各种字体的汉字和人民币符号,注意不是所有的字体都支持,不支持的字体预期打印效果同最后一次生效的字体一样,支持字体
			for(i = 0;i<SZ_ARRAY(hzfont);i++)
			{
				memset(tmp, 0, sizeof(tmp));
				sprintf(tmp,"!NLFONT %d 12 3\n*text c 以%s字体居中打印两横RMB符号%s\n*line\n*feedline 1\n", hzfont[i], hzfontstr[i], rmbbuf);
				if((nRet = NDK_Script_Print(tmp,strlen(tmp)))!= NDK_OK)
				{
					if(nRet == PRN_STATUS_NOPAPER)
						cls_show_msg("打印机缺纸,装纸后任意键继续");
					else
					{
						send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
						goto prnt_error;
					}
				}
			}
			//case4.1:进行目前收集到的全部PNG图片测试(包括color系类、IHDR系类、艺术字系类，其他系类) sull 20180319 add
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
				sprintf(tmp1, "!NLFONT 9 12\n*text c 以下打印PNG图片,阈值自动(%s),效果请对照实际图片,彩色图片浅颜色将打印成白色，深颜色将打印成黑色\n",position[p]);
				if((nRet = NDK_Script_Print(tmp1,strlen(tmp1))) != NDK_OK)
				{			
					if(nRet == PRN_STATUS_NOPAPER)
						cls_show_msg("打印机缺纸,装纸后任意键继续");
					else
					{
						send_result("line %d:%s测试失败(%d,%s)",__LINE__,TESTAPI,nRet,tmp);
						goto prnt_error;
					}	
				}
				if((nRet = NDK_Script_Print(tmp,strlen(tmp)))!= NDK_OK)
				{	
					if(nRet == PRN_STATUS_NOPAPER)
						cls_show_msg("打印机缺纸,装纸后任意键继续");
					else
					{
						send_result("line %d:%s测试失败(%d,%s)",__LINE__,TESTAPI,nRet,tmp);
						goto prnt_error;
					}	
				}
				if((nRet = NDK_Script_Print(tmp2,strlen(tmp2)))!= NDK_OK)
				{	
					if(nRet == PRN_STATUS_NOPAPER)
						cls_show_msg("打印机缺纸,装纸后任意键继续");
					else
					{
						send_result("line %d:%s测试失败(%d,%s)",__LINE__,TESTAPI,nRet,tmp);
						goto prnt_error;
					}	
				}
			}
			
			//case2:打印命令设置中参数设置错误时应该不会出现跑飞情况
			for(i=0;i<SZ_ARRAY(err_str);i++)
			{
				if((nRet = NDK_Script_Print(err_str[i],strlen(err_str[i])))!= NDK_OK)
				{
					if(i==10)//对于预期失败的异常测试进行处理
					{
						if(nRet != NDK_ERR)
						{
							send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
							goto prnt_error;
						}
					}
					else
					{
						if(nRet == PRN_STATUS_NOPAPER)
							cls_show_msg("打印机缺纸,装纸后任意键继续");
						else
						{
							send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
							goto prnt_error;
						}
					}
				}
			}
			if (cls_show_msg("以上均正常打印并无跑飞,是[确认],否[其它]")!=ENTER)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto prnt_error;
			}
			
			//case3:异常参数测试
			if((nRet = NDK_Script_Print(NULL,10))!=NDK_ERR_PARA) 
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
				goto prnt_error;
			}
			if((nRet1 = NDK_Script_Print(buf_str[0],0))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet1);
				goto prnt_error;
			}
			if((nRet1 = NDK_Script_Print(buf_str[0],-1))!=NDK_ERR_PARA)
			{
				send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet1);
				goto prnt_error;
			}	
			
			send_result("%s测试通过", TESTAPI);
			goto prnt_error;
			break;
		case PRN_STATUS_NOPAPER:
			cls_printf("打印机缺纸...装纸后,任意键继续,ESC放弃");
			lib_kbflush(); //清KB BUF
			while ((nKeyin=lib_kbhit())==0)
			{
				NDK_SysBeep();
				//正常装纸
				NDK_PrnFeedByPixel(10);
				sleep(2);
			}
			if (nKeyin==ESC)
			{
				send_result("line %d:打印机缺纸,用户放弃", __LINE__);
				goto prnt_error;
			}
			NDK_PrnStart();
			cls_printf("测试%s...", TESTAPI);
			break;
		case PRN_STATUS_OVERHEAT:
			send_result("line %d: 打印机过热", __LINE__);
			goto prnt_error;
			break;
		case PRN_STATUS_VOLERR:
			send_result("line %d: 打印机电源故障", __LINE__);
			goto prnt_error;
			break;
		case PRN_STATUS_DESTROYED:
			send_result("line %d: 打印机损坏", __LINE__);
			goto prnt_error;
			break;
		default:
			//未知状态，判为错
			send_result("line %d: UNKNOWN STATE(tpstatus=%d)", __LINE__, emStatus);
			goto prnt_error;
			break;
		}
	
	}
	
prnt_error:
	NDK_PrnInit(g_PrnSwitch);
	return;
}

