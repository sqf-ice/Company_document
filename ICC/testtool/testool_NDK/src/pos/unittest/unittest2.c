/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: unittest模块
* file name		: 
* Author 			: unittest
* version			: 
* DATE			: 20120815
* directory 		: 
* description		: 本用例是一个以旧测试风格(通用方法)进行测试的可编译通过的样例代码.本例测试add加法器功能是否正确
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
//#include "自己需要用到的头文件"	//按需定义

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"add"	//待测API或API对的名字（当是API对，格式："api1,api2"）

#define StartStopwatch()
#define ReadStopwatch()	(0.001)

/*------------global variables definition-----------------------*/
//尽量少用全局变量（而使用参数来传值），只在非用不可时才使用。同时，为了不与他人的全局变量发生冲突，若只有一个文件内使用请加static修饰符；若需要跨文件使用时，则命名规则为"文件名_变量名"，如，gen100_flg。

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
//声明在unittest.c中定义的可供本模块的用例函数使用的本测试模块的测试支撑函数,需要用extern修饰

/*---------------functions definition---------------------------*/
//以下模拟本用例的测试前置
static int PreHandle(void)
{
	//以下为局部变量

	//以下为函数体
	return SUCC;
}

//以下模拟一个被测函数
static int add(int a, int b)
{
	//以下为局部变量

	//以下为函数体
	return (a+b);
}

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	unittest      20120809	created
*				  	unittest      20120810	modify for xxx
*****************************************************************/
void unittest2(void)//函数名为"模块名+用例号"的形式,函数名就是主文件名,无参数无返回
{
	//以下为局部变量
	int ret = 0, ret1 = 0;
	float fTimeElapsed = 0.0;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前所在的测试用例,这里的格式不是必须的,可以根据实际,使用其它格式,如使用cls_show_msg1等

//-------------------------------------------需要用例开发人员填写的第1部分（测试用例主体）的起点--------------------------------------------
	//测试前置－PreCondition或PreHandle（if necessery）
	if(PreHandle()!=SUCC)
		goto PRE_EXIT;

	//case1: 选取2个正整数进行运算,应与预期相符
	//case1.1: 第1个加数>第2个加数
	StartStopwatch();//运算计时开始
	if((ret=add(16, 2))!=18)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	fTimeElapsed = ReadStopwatch();//模拟运算计算结果
	//case1.2: 第1个加数<第2个加数
	if((ret=add(2, 16))!=18)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//case2: 选取1个正整数和1个负整数进行运算,应与预期相符
	if((ret=add(-2, 16))!=14)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//case3: 测试add是否支持"加法交换律"((a+b)=?=(b+a))
	if((ret=add(7, -16))!=(ret1=add(-16, 7)))
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, ret1);
		return;
	}
	
	//测试后置－PostCondition或PostHandle（if necessery）
	//本用例无后置动作
	
//-------------------------------------------需要用例开发人员填写的第1部分（测试用例主体）的终点--------------------------------------------

	//测试结束
	send_result("%s测试通过(耗时%f)", TESTAPI, fTimeElapsed);//或还可输出其它信息,如：send_result("%s测试通过(耗时%f)", TESTAPI, fTimeElapsed);
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

