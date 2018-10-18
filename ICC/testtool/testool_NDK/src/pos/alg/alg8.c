/************************************************************************
*
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			: 算法模块
* file name		    : alg8.c
* Author 			: zhengry	
* version			:
* DATE				: 20150907
* directory 		:
* description		: 测试NDK_AlgSM2KeyPairGen生成SM2非对称公私钥对的功能（国密算法）
* related document	: NDK.chm
*
************************************************************************
* log			:
* Revision
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#define     MES_LEN    1024-96

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_AlgSM2KeyPairGen"

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
*			  	  	      zhengry           20150907  	    created
*****************************************************************/
void alg8(void)
{
	uchar pubkey[64]={0}, prikey[32]={0}, pubkey1[65]={0}, prikey1[33]={0};
	int ret=0;
		
	//process body
	cls_printf("测试%s...", TESTAPI);

	//测试前置
	memset(pubkey, 0, sizeof(pubkey));
	memset(prikey, 0, sizeof(prikey));
	memset(pubkey1, 0, sizeof(pubkey1));
	memset(prikey1, 0, sizeof(prikey1));

	//该接口参数传NULL代表不传出密钥值
	//case1:正常测试
	if((ret=NDK_AlgSM2KeyPairGen(NULL, prikey))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2KeyPairGen(pubkey, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2KeyPairGen(NULL, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case2:生成的公钥应该是64字节，生成的私钥应该是32字节
	if((ret=NDK_AlgSM2KeyPairGen(pubkey1, prikey1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(pubkey1[64]!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, pubkey1[64]);
		RETURN;
	}
	if(prikey1[32]!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, prikey1[32]);
		RETURN;
	}
	
	//case3:正常测试
	if((ret=NDK_AlgSM2KeyPairGen(pubkey, prikey))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case4:第二次生成的密钥对应该与第一次生成的密钥对不一样
	if(!memcmp(prikey, prikey1, sizeof(prikey)))
	{
		send_result("line %d:%s生成密钥对失败", __LINE__, TESTAPI);
		RETURN;
	}
	if(!memcmp(pubkey, pubkey1, sizeof(pubkey)))
	{
		send_result("line %d:%s生成密钥对失败", __LINE__, TESTAPI);
		RETURN;
	}
		
	//测试结束
	send_result("%s测试通过", TESTAPI); 	
	return;

}


