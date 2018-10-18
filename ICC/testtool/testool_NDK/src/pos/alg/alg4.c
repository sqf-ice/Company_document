/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 算法模块
* file name		: alg4.c
* Author 			: wangzy
* version			:
* DATE				: 20130319
* directory 	:
* description	: 测试NDK_AlgSHA256接口功能
* related document	: NDK.chm
*
************************************************************************
* log			:
* Revision
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_AlgSHA256"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	:
* functional description 	:
* input parameter	 	:
* output parameter	 	:
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	     wangzy         20130319  	   created
*****************************************************************/
void alg4(void)
{
	//局部变量初始化，包括：输入数据、数据长度、输出数据
	uchar datain[1024]={0},dataout[32]={0};
	uchar outcrc[32]={0xb4,0xa3,0x8f,0x5f,0x5a,0x21,0xb6,0xba,0x9f,0x20,0x08,0x87,0x8b,0xea,0xa3,0xa7,0x08,0x86,0x6c,0x66,0xcb,0xad,0xa7,0x1b,0xa2,0x8e,0xc3,0x59,0xb5,0xa5,0x3c,0x3c};
	int ret=0;
	
	cls_printf("测试%s...", TESTAPI);
	
	//case1: 非法参数测试
	//case1.1传入非法输入地址：NULL
	if((ret = NDK_AlgSHA256(NULL, sizeof(datain), dataout)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case1.2:传入非法数据长度：-1
	if((ret = NDK_AlgSHA256(datain, -1, dataout)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case1.3:传入非法输出地址：NULL
	if((ret = NDK_AlgSHA256(datain, sizeof(datain), NULL)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常测试，任意位数据加密后数据长度为32字节 ?BUG:程序返回1 
	memset(datain,'1',sizeof(datain));
	if((ret = NDK_AlgSHA256(datain, sizeof(datain), dataout)) !=NDK_OK)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(outcrc,dataout,sizeof(outcrc)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 	 
	return;
}


