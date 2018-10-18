/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 算法模块
* file name		: alg3.c
* Author 			: wangzy
* version			:
* DATE				: 20130319
* directory 	:
* description	: 测试NDK_AlgSHA512接口功能
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
#define 	TESTAPI		"NDK_AlgSHA512"

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
void alg3(void)
{
	//局部变量初始化，包括：输入数据、数据长度、输出数据
	uchar datain[1024]={0},dataout[64]={0};
	uchar outcrc[64]={0x1f,0x95,0x45,0x3d,0xda,0xca,0x9e,0xd5,0xbb,0x07,0x5a,0x69,0x76,0xbf,0x69,0xee,0xe0,0x2a,0x91,0x86,0xac,0x9b,0xf6,0x34,0x59,0x65,0xaf,0x47,0xfd,0x61,0x34,0xa4,0x27,0xc7,0xf5,0xe7,0xb5,0xbb,0xfe,0xae,0xae,0x3c,0x84,0x3b,0x07,0xb5,0x46,0x46,0xf6,0x6f,0xd6,0x73,0x40,0x04,0xeb,0xc4,0xff,0x9e,0x06,0xba,0xb3,0xe1,0x88,0x5b};
	int ret=0;

	cls_printf("测试%s...", TESTAPI);
	
	//case1: 非法参数测试
	//case1.1传入非法输入地址：NULL
	if((ret = NDK_AlgSHA512(NULL, sizeof(datain), dataout)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case1.2:传入非法数据长度：-1
	if((ret = NDK_AlgSHA512(datain, -1, dataout)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case1.3:传入非法输出地址：NULL
	if((ret = NDK_AlgSHA512(datain, sizeof(datain), NULL)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常测试，任意位数据加密后数据长度为64字节 ?BUG:程序返回1 
	memset(datain,'1',sizeof(datain));
	if((ret = NDK_AlgSHA512(datain, sizeof(datain), dataout)) !=NDK_OK)
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

