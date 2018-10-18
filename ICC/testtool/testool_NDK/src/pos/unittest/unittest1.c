/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: unittest模块
* file name		: 
* Author 			: unittest
* version			: 
* DATE			: 20120809
* directory 		: 
* description		: （本字段必须填写！）描述本用例的测试内容或测试目的，如"测试xxx能否xxx".技巧：将API手册上对接口的功能描述改写成"测试xxx能否xxx"的形式即可.注意：本文件只是一个样例模板!
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
//#include "自己需要用到的头文件"	//按需定义

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"xxx"	//待测API或API对的名字（当是API对，格式："api1,api2"）
//其它宏定义

/*------------global variables definition-----------------------*/
//尽量少用全局变量（而使用参数来传值），只在非用不可时才使用。同时，为了不与他人的全局变量发生冲突，若只有一个文件内使用请加static修饰符；若需要跨文件使用时，则命名规则为"文件名_变量名"，如，gen100_flg。

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
//声明在unittest.c中定义的可供本模块的用例函数使用的本测试模块的测试支撑函数,需要用extern修饰

/*---------------functions definition---------------------------*/
//定义本用例用到的其它私有函数,需要用static修饰.如下：
static int func1(char *str)//参数及返回值类型可自由定义(不做限制)
{
	//以下为局部变量

	//以下为函数体
	return SUCC;
}

/****************************************************************
* function name 	 	: unittest1
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	 	 unittest      20120809	created
*				  	 unittest      20120810	modify for xxx
*****************************************************************/
void unittest1(void)//函数名为"模块名+用例号"的形式,函数名就是主文件名,无参数无返回
{
	//以下为局部变量

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前所在的测试用例,这里的格式不是必须的,可以根据实际,使用其它格式,如使用cls_show_msg1等
	//或者使用以下格式：
	CUT_BEGIN();

//-------------------------------------------需要用例开发人员填写的第1部分（测试用例主体）的起点--------------------------------------------
	//测试前置－PreCondition或PreHandle（if necessery）
	if 测试前置运行失败
		goto PRE_EXIT;

	//各个case及subcase，注释上每个（子）用例的意图
	//case1：对测试意图的注释
	//代码段1（调用被测API）
	if((ret=被测接口())!=预期结果)	//通用的验证方法
	{
		//关于测试结果的上报,必须调用send_result,不允许调用printf,portwrite其它函数
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);//这里的例子不是必须的，出错信息要能打出有利于定位问题的信息，如，循环次数，预期，实际等。
		return;或goto EXIT或其它
	}
	//或者使用以下格式：
	CUT_ASSERT_NOT_EQUAL(被测接口(), 预期结果, return或goto EXIT1);

	//case2：对测试意图的注释
	//代码段2
	……
	
	//测试后置－PostCondition或PostHandle（if necessery）
	if 测试后置运行失败
		goto POST_EXIT;
//-------------------------------------------需要用例开发人员填写的第1部分（测试用例主体）的终点--------------------------------------------

	//测试结束
	send_result("%s测试通过", TESTAPI);//或还可输出其它信息,如：send_result("%s测试通过(耗时%f)", TESTAPI, fTimeElapsed);
	//或者使用以下格式：
	CUT_END();
	return;
//-------------------------------------------需要用例开发人员填写的第2部分（出错处理）的起点--------------------------------------------
PRE_EXIT://if necessary
EXIT1:
	//一些错误处理
	return;
EXIT2:	//if necessary
	//一些错误处理
	return;
POST_EXIT://if necessary
	//一些错误处理
	return;
//-------------------------------------------需要用例开发人员填写的第2部分（出错处理）的终点--------------------------------------------
}

//更多的编码规范，参见"DOC\测试用例代码编写规范CHECKLIST.doc".强烈建议阅读！至少应阅读第6部分（中的"编译时独立性"与"运行时独立性"）,以为用例代码质量要求有一定的了解.
//上述模板中,对注释的最低要求:必须对测试点caseX进行注释
//关于过程中错误的测试结果的上报：
// 1、直接通过if之类的条件判断后，调用send_result进行上报，由于send_result支持可变参（弥补了测试断言对参数的限制），if判断内还可以有其它动作。此方式较为灵活(是通用的方法)；
// 2、使用CUT_ASSERT一族的断言（定义及作用说明在inc\pos\cut_assert.h中）进行测试，此方式可使测试代码量减少，但要注意需要进行出错处理时，要使用相应的测试宏，同时这些宏仅支持单元测试使用，不支持未来的系统测试（如果未来有的话）；
//rfcard4.c、ppp1.c、display18.c是原国内测试套件的用例库中的测试用例代码，供参考

