/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 算法模块
* file name		: alg2.c
* Author 			: wangzy
* version			:
* DATE				: 20130319
* directory 	:
* description	: 测试NDK_AlgSHA1接口功能
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
#define 	TESTAPI		"NDK_AlgSHA1"

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
void alg2(void)
{
	//局部变量初始化，包括：输入数据、数据长度、输出数据
	uchar datain[1024]={0},dataout[20]={0};
	uchar outcrc[21]={0xf9,0x7e,0xb9,0x00,0x5c,0xd3,0x5d,0x29,0x32,0xe7,0x1a,0x7c,0xc07,0x54,0x7b,0xc3,0x2f,0x68,0x93,0x23};
	int ret=0;	

	cls_printf("测试%s...", TESTAPI);
	
	//case1: 非法参数测试
	//case1.1传入非法输入地址：NULL
	if((ret = NDK_AlgSHA1(NULL, sizeof(datain), dataout)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case1.2:传入非法数据长度：-1
	if((ret = NDK_AlgSHA1(datain, -1, dataout)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case1.3:传入非法输出地址：NULL
	if((ret = NDK_AlgSHA1(datain, sizeof(datain), NULL)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	memset(datain,'1',sizeof(datain));
	//case2:正常测试，任意位数据加密后数据长度为20字节BUG:程序返回1
	if((ret = NDK_AlgSHA1(datain, sizeof(datain), dataout)) !=NDK_OK)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(outcrc,dataout,sizeof(outcrc)-1))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 	 
	return;
}

