/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: unittest模块
* file name		: 
* Author 			: unittest
* version			: 
* DATE			: 20120815
* directory 		: 
* description		: 本用例是一个以新测试风格(测试断言)进行测试的可编译通过的样例代码.本例测试operate及乘法器multiply功能是否正确
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
//#include "自己需要用到的头文件"	//按需定义

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"operate,multiply"	//待测API或API对的名字（当是API对，格式："api1,api2"）

#define STEP1()
#define STEP2() SUCC

/*------------global variables definition-----------------------*/
//尽量少用全局变量（而使用参数来传值），只在非用不可时才使用。同时，为了不与他人的全局变量发生冲突，若只有一个文件内使用请加static修饰符；若需要跨文件使用时，则命名规则为"文件名_变量名"，如，gen100_flg。
static int init = FALSE;

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
	//比方说,需要创建了一个文件才能进行multiply运算
	return SUCC;
}

//以下模拟一对被测函数.其中,operate为无参无返回,其工作是否正常需要通过multiply来验证;同时,执行multiply前一般都需要配对调用operate. 故关系密切的它们放一起测试
static void operate(void){init = TRUE;}

static int multiply(int a, int b)
{
	//以下为局部变量
	int cnt = 1, result = a;

	//以下为函数体
	if(init==FALSE)
		return FAIL;
	for(; cnt<b; cnt++)
		result+=a;
	init = FALSE;
	return result;
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
void unittest3(void)//函数名为"模块名+用例号"的形式,函数名就是主文件名,无参数无返回
{
	//以下为局部变量
	int ret = 2012, *ptr = NULL;
	char *str = "hello world!", *str1 = "hello world!", *str2 = "hello World!";

	//以下为函数体
	CUT_BEGIN();

//-------------------------------------------需要用例开发人员填写的第1部分（测试用例主体）的起点--------------------------------------------
	//测试前置－PreCondition或PreHandle（if necessery）
	CUT_ASSERT_EQUAL(PreHandle(), SUCC, goto PRE_EXIT);

	//case1: 不调用operate,直接调用multiply应失败
	CUT_ASSERT_EQUAL(multiply(1, 1), FAIL, goto EXIT1);

	//case2: 正确使用multiply进行运算,应与预期相符.以下分几个子用例进行测试
	operate();
	CUT_ASSERT_EQUAL(multiply(8, 7), 56, goto EXIT1);
	operate();
	CUT_ASSERT_EQUAL(multiply(0, 7), 0, goto EXIT1);
	operate();
	CUT_ASSERT_EQUAL(multiply(99, -3), -297, cls_show_msg("2012 is the end of the world?!");ret = 2013;goto EXIT1);//此处是一个actions动作序列为多个动作的例子,动作序列间用;分隔

	//case3: 验证multiply是否满足"乘法交换律"
	operate();
	CUT_ASSERT_EQUAL(multiply(4, 5), multiply(5, 4), goto EXIT2);

	//caseX: 这里的用例不是为了本用例的验证目的.而是为了显示其它测试断言的用法.更多测试断言及用法说明见cut_assert.h
	//注意:使用测试断言虽然方便,但它们 不支持可变参,当目前提供的测试宏不能满足测试需求时,请参照unittest2.c使用send_result的风格
	operate();
	CUT_ASSERT(multiply(3, 3)>3, return);
	operate();
	CUT_ASSERT_EQUAL(multiply(3, 3), 9, return);
	operate();
	CUT_ASSERT_NOT_EQUAL(multiply(3, 3), 90, return);
	
	CUT_ASSERT_STRING_EQUAL(str, str1, return);
	CUT_ASSERT_STRING_NOT_EQUAL(str, str2, return);
	CUT_ASSERT_NSTRING_EQUAL(str, str2, 5, return);
	CUT_ASSERT_NSTRING_NOT_EQUAL(str, str2, strlen(str), return);
	CUT_ASSERT_PTR_NULL(ptr, return);
	CUT_ASSERT_PTR_EQUAL(ptr, NULL, return);
	ptr = &ret;
	CUT_ASSERT_PTR_NOT_NULL(ptr, return);
	CUT_ASSERT_PTR_NOT_EQUAL(ptr, NULL, return);

	//测试后置－PostCondition或PostHandle（if necessery）
	//清理动作1
	STEP1();
	//清理动作2,预期应执行成功
	CUT_ASSERT_EQUAL(STEP2(), SUCC, goto POST_EXIT);
//-------------------------------------------需要用例开发人员填写的第1部分（测试用例主体）的终点--------------------------------------------

	//测试结束
	CUT_END();
	return;
//-------------------------------------------需要用例开发人员填写的第2部分（出错处理）的起点--------------------------------------------
PRE_EXIT://if necessary
EXIT1:
	//清理PreHandle中创建的文件
	return;
EXIT2:	//if necessary
	//清理PreHandle中创建的文件
	//此外,还有一些额外的清理动作
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

