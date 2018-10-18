/************************************************************************
*
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			: 算法模块
* file name		    : alg12.c
* Author 			: zhengry	
* version			:
* DATE				: 20150908
* directory 		:
* description		: 测试NDK_AlgSM2Sign函数使用SM2非对称公私钥对中的私钥对摘要（32字节）计算数字签名（64字节）的功能（国密算法）
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
#define 	TESTAPI		"NDK_AlgSM2Sign"

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
*			  	  	      zhengry           20150908  	    created
*****************************************************************/
void alg12(void)
{
	int ret=0;
	uchar prikey[32]={0}, pubkey[64]={0}, summary[32]={0}, sign_data[65]={0}, message[]="newland";
	uchar ID[]={"test"};
	/*uchar pre_prikey[32]={0x56, 0x4e, 0x11, 0xcc, 0x71, 0x6d, 0x20, 0x20,
						  	0xec, 0x45, 0x6d, 0x0a, 0xa3, 0xb0, 0x64, 0x14,
						 	0xaf, 0x49, 0xd8, 0xde, 0x7f, 0x68, 0x5f, 0x8d,
						  	0x23, 0x45, 0x6b, 0x11, 0xb7, 0x52, 0xbf, 0x7c};
	uchar pre_summary[32]={0x06, 0xfe, 0x86, 0x15, 0x30, 0x37, 0x76, 0x22,
						   0xf1, 0xbc, 0x27, 0x6d, 0x9f, 0x41, 0x17, 0x28,
						   0x42, 0x4a, 0x83, 0xad, 0x1c, 0x14, 0x0d, 0x6f,
						   0x39, 0x17, 0x6f, 0x0c, 0xfa, 0xbb, 0xfe, 0x44};
	uchar pre_signdata[64]={0xc8, 0x88, 0xbe, 0x7b, 0x8d, 0x46, 0x87, 0x3e,
							0x71, 0x47, 0xd1, 0x11, 0x03, 0x78, 0x65, 0x03,
							0x03, 0xe6, 0x83, 0x0c, 0xe1, 0xed, 0xcc, 0x22,
							0xe1, 0x88, 0x12, 0xb9, 0x0f, 0xe1, 0x46, 0xff, 
							0x0a, 0x16, 0x00, 0x5c, 0x5b, 0x59, 0x7c, 0x95,
							0x86, 0xec, 0xb1, 0x52, 0xc6, 0x62, 0xb3, 0x2d,
							0x24, 0xde, 0x34, 0xdc, 0x21, 0xec, 0xe7, 0xc3,
							0xeb, 0x1c, 0xa2, 0x5f, 0xdc, 0xa3, 0x1e, 0x9f};*/
	//process body
	cls_printf("测试%s...", TESTAPI);

	//测试前置
	memset(prikey, 0, sizeof(prikey));
	memset(pubkey, 0, sizeof(pubkey));
	memset(summary, 0, sizeof(summary));
	memset(sign_data, 0, sizeof(sign_data));
	if((ret=NDK_AlgSM2KeyPairGen(pubkey, prikey))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2GenE(strlen((char *)ID), ID, strlen((char *)message), message, pubkey, summary))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case1:参数异常 
	if((ret=NDK_AlgSM2Sign(NULL, summary, sign_data))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2Sign(prikey, NULL, sign_data))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2Sign(prikey, summary, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:签名后的数据为64字节
	if((ret=NDK_AlgSM2Sign(prikey, summary, sign_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(sign_data[64]!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, sign_data[64]);
		RETURN;
	}

	//输入私钥是个地址，只会取相应长度的数据
	//case2:异常测试:输入公钥，签名应该失败
	/*if((ret=NDK_AlgSM2Sign(pubkey, summary, sign_data))!=NDK_ERR)//返回0
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		//RETURN;
	}

	//摘要没有正确错误的概念	
	//case3:异常测试:输入错误的摘要，签名应该失败
	if((ret=NDK_AlgSM2Sign(prikey, pre_summary, sign_data))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case4:正常测试:预设公钥和生成的摘要，得到的签名数据和预设的应该一致
	memset(sign_data, 0, sizeof(sign_data));
	if((ret=NDK_AlgSM2Sign(pre_prikey, pre_summary, sign_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(sign_data, pre_signdata, sizeof(pre_signdata)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}*/
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 	
	return;
}
