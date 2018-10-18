/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: unittest模块
* file name		: 
* Author 			: unittest
* version			: 
* DATE			: 20120809
* directory 		: 
* description		: unittest模块总入口,本模块的公共函数与公共定义也可以放于此处.注意：本文件只是一个样例模板!
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
//#include "自己需要用到的头文件"	//按需定义

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"unittest模块"
//其它宏定义,按需定义

/*------------global variables definition-----------------------*/
//尽量少用全局变量（而使用参数来传值），只在非用不可时才使用。同时，为了不与他人的全局变量发生冲突，若只有一个文件内使用请加static修饰符；若需要跨文件使用时，则命名规则为"文件名_变量名"，如，gen100_flg。

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
//定义本测试模块的测试支撑函数，可供本模块的用例函数使用，参数及返回值类型可自由定义(不做限制)

/****************************************************************
* function name 	 	: unittest
* functional description 	: unittest模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	unittest      20120809	created
*				  	unittest      20120810	modify for xxx
*****************************************************************/
void unittest(PF* list, unsigned char id[])//入口函数名就是主文件名,也是模块名
{
	//以下为局部变量

	//以下为函数体
	cls_show_msg1(1, "%s测试...", TESTMODULE);//屏幕提示当前所在的测试模块


	//模块测试前置－PreHandle（if necessery）
	//if 模块测试前置运行失败
		//报错并退出整个模块测试;

	process(list, id);

	//模块测试后置－PostHandle（if necessery）
	//运行模块测试后置

	//模块测试结束
	return;
}

//更多的编码规范，参见"DOC\测试用例代码编写规范CHECKLIST.doc"
//ppp.c是原国内测试套件的用例库中PPP测试模块的主文件，供参考

