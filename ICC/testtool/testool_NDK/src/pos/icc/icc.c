/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: icc模块
* file name		: 
* Author 			: chenfm
* version			: 
* DATE			: 20130107
* directory 		: 
* description		: icc模块总入口,本模块的公共函数与公共定义也可以放于此处
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"icc模块"
#define MAXWAITTIME 30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
//不关心下电成败
void icpowerdown_all(void)
{
	/*private & local definition*/
#include "icsam.h"
	int i=0, maxid=ICSAMNUM+1/*SZ_ARRAY(CardNo)*/;

	/*process body*/
	for(i=0;i<maxid;i++)
		NDK_IccPowerDown(CardNo[i]);
}

/****************************************************************
* function name 	 	: icc
* functional description 	:icc模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	chenfm      20130107	created
*				  	
*****************************************************************/
void icc(PF *list, unsigned char id[])//入口函数名就是主文件名,也是模块名
{
	//以下为局部变量

	//以下为函数体
	if(cls_show_msg1(MAXWAITTIME, "%s测试...请确保机具上已有ic/sam所有卡...ESC退出本模块测试", TESTMODULE)==ESC)
		return;
	
	icpowerdown_all();
	process(list, id);
	icpowerdown_all();
	return;
}

