/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: usb
* file name		: 
* Author 			: 
* version			: V1.0
* DATE			: 20130311
* directory 		: 
* description		: sd/usb总入口,本模块的公共函数与公共定义也可以放于此处
* related document	: 
*
************************************************************************
* log			: 
* Revision 1.0  20061101 14:10 zhangnw
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define MAXWAITTIME 5//30
#define	TESTMODULE	"SD/USB"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: usb入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		zhangnw 		   20090730 	 	created
*****************************************************************/
void sdusb(PF* list, unsigned char id[])
{
	/*private & local definition*/
	int disktype = get_disktype();
	/*process body*/
#if USB_ENABLE||SD_ENABLE
	cls_show_msg1(MAXWAITTIME, "%s测试...注意:应按应用提示信息进行操作!不应按API提示.", TESTMODULE);//时间长些,以保证用户看清楚提示
	if(conf_disktype()!=NDK_OK)
		return;
	NDK_DiskClose(disktype);	
	process(list, id);
	NDK_DiskClose(disktype);
#else
	cls_show_msg("本机具不支持%s测试!", TESTMODULE);
#endif
}

