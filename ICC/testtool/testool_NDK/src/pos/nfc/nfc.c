/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: nfc模块
* file name		: 
* Author 			: huangjianb
* version			: 
* DATE			: 20141020
* directory 		: 
* description		: nfc模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"nfc模块"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: nfc
* functional description 	:nfc模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date			remarks
*			  	  			huangjianb 	20130107	created
*				  	
*****************************************************************/
void nfc(PF *list, unsigned char id[])//入口函数名就是主文件名,也是模块名
{
	/*private & local definition*/
	
	/*process body*/
	cls_show_msg1(1, "%s测试...", TESTMODULE);
	
	process(list, id);
	
	//模块测试结束
	return;
}
