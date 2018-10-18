/*
 * FileName:       
 * Author:         zhangnw  
 * Description:   单元测试断言类
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version>  <desc>
 *	zhangnw	   20120814			created
 */

#ifndef _CUT_ASSERT_H_
#define _CUT_ASSERT_H_

/*-----------------------includes-------------------------------*/
#include "lib.h"

/*---------------constants/macro definition---------------------*/
//Begin a unit-test case
#define CUT_BEGIN() cls_printf("测试%s...", TESTAPI)

//End a unit-test case
#define CUT_END() send_result("%s测试通过", TESTAPI)

//Assert that expression is TRUE (non-zero). Actions (return or goto label or other) take place unless ASSERTION is TRUE.
#define CUT_ASSERT(expression, actions) \
do\
{\
	if(!(expression))\
	{\
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);actions;\
	}\
}while(0)

#define CUT_ASSERT_OP(actual, OP, expected, actions) \
do\
{\
	int actval = 0, expval = 0;\
	if(!((actval=(int)(actual))OP(expval=(int)(expected))))\
	{\
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, actval, expval);actions;\
	}\
}while(0)

//注意:CUT_ASSERT_EQUAL及CUT_ASSERT_NOT_EQUAL只适用于int型数据,对其它数据的测试只能直接写if判断
//Assert that actual == expected. Actions (return or goto label or other) take place unless ASSERTION is TRUE.
#define CUT_ASSERT_EQUAL(actual, expected, actions) CUT_ASSERT_OP(actual, ==, expected, actions)

//Assert that actual != expected. Actions (return or goto label or other) take place unless ASSERTION is TRUE.
#define CUT_ASSERT_NOT_EQUAL(actual, expected, actions) CUT_ASSERT_OP(actual, !=, expected, actions)

#define CUT_ASSERT_PTR_OP(actual, OP, expected, actions) \
do\
{\
	if(!(((void *)(actual))OP((void *)(expected))))\
	{\
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);actions;\
	}\
}while(0)

//Assert that pointers actual == expected. Actions (return or goto label or other) take place unless ASSERTION is TRUE.
#define CUT_ASSERT_PTR_EQUAL(actual, expected, actions) CUT_ASSERT_PTR_OP((actual), ==, (expected), actions)	//CUT_ASSERT((actual)==(expected), actions)

//Assert that pointers actual != expected. Actions (return or goto label or other) take place unless ASSERTION is TRUE.
#define CUT_ASSERT_PTR_NOT_EQUAL(actual, expected, actions) CUT_ASSERT_PTR_OP((actual), !=, (expected), actions)	//CUT_ASSERT((actual)!=(expected), actions)

//Assert that pointer value == NULL. Actions (return or goto label or other) take place unless ASSERTION is TRUE.
#define CUT_ASSERT_PTR_NULL(value, actions) CUT_ASSERT_PTR_OP((value), ==, NULL, actions)	//CUT_ASSERT((value)==NULL, actions)

//Assert that pointer value != NULL. Actions (return or goto label or other) take place unless ASSERTION is TRUE.
#define CUT_ASSERT_PTR_NOT_NULL(value, actions) CUT_ASSERT_PTR_OP((value), !=, NULL, actions)	//CUT_ASSERT((value)!=NULL, actions)

//Assert that strings actual and expected are equivalent. Actions (return or goto label or other) take place unless ASSERTION is TRUE.
#define CUT_ASSERT_STRING_EQUAL(actual, expected, actions) CUT_ASSERT(!strcmp(actual, expected), actions)

//Assert that strings actual and expected differ. Actions (return or goto label or other) take place unless ASSERTION is TRUE.
#define CUT_ASSERT_STRING_NOT_EQUAL(actual, expected, actions) CUT_ASSERT(strcmp(actual, expected), actions)

//Assert that 1st count chars of actual and expected are the same. Actions (return or goto label or other) take place unless ASSERTION is TRUE.
#define CUT_ASSERT_NSTRING_EQUAL(actual, expected, count, actions) CUT_ASSERT(!memcmp(actual, expected, count), actions)

//Assert that 1st count chars of actual and expected differ. Actions (return or goto label or other) take place unless ASSERTION is TRUE.
#define CUT_ASSERT_NSTRING_NOT_EQUAL(actual, expected, count, actions) CUT_ASSERT(memcmp(actual, expected, count), actions)

#endif

