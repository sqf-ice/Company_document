/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 算法模块
* file name		: alg7.c
* Author 			: wangzy
* version			:
* DATE				: 20130319
* directory 	:
* description	: 测试NDK_AlgRSAKeyPairVerify接口功能
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
#define 	TESTAPI		"NDK_AlgRSAKeyPairVerify"
//#define     RSA_KEYLEN_NUM  3
//#define     RSA_EXPKIND_NUM  2

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
void alg7(void)
{
	//局部变量初始化
	ST_RSA_PUBLIC_KEY   pubkey={0};
	ST_RSA_PUBLIC_KEY   pubkey2={0};
	ST_RSA_PRIVATE_KEY prikey={0};
	ST_RSA_PRIVATE_KEY prikey2={0};
	int ret=0;
	/*uchar exp[3]="03";
	uchar modulus[129]="9CFE5CED36BE450CE3A5763FA69A358F3DB19BF7C4A28D6380285B98218FDEAC5E508649CDCCD35D4D1DB04FD6CECCEFE3C390D419296B7866A16C0406F601F9";
	uchar priexp[129]="68A99348CF29835DED18F97FC466CE5F7E7667FA831708ED001AE7BAC10A9471D8A77033F9F3FB5F2B9A204025D9A0678935EB6C61586B1DC256E0798F6999C3";
	uchar priexp2[129]="78A99348CF29835DED18F97FC466CE5F7E7667FA831708ED001AE7BAC10A9471D8A77033F9F3FB5F2B9A204025D9A0678935EB6C61586B1DC256E0798F6999C3";*/
	uchar exp_10001[6]="10001";
	uchar modulus_10001[257]="69321A8FA58E2CA50D37AA1A9F667769058CB8240E3E398E0581F84175247ED2B6FBBFAF49706D1842246D875C11B8BBC9B195920F20D2022C2D32D511B923807F0C88966D1F6579C7F574DB5189319509D36838F9D52C3A6CB32AC92ED0D1048CFE7CC8E09BA41AD9E6207EAB4629C0008C4461A49C023173D9C424E0F7BA01";
	uchar priexp_10001[257]="2920DCBFA2D3C662B59B38D775D0E308DD919A0106CC8F4616A32D03B2A015319F3728478927FA8F4C74DF99D7865A3A996DE3F39A1493E7238C7C5E4FB299CD44AD12351D4B8C11DA8A24FF6ED8DB544744047B66FB5ADADC4188FF79CDB208EF00658DBCEB1C41C8FBEFFB6049B0E606E1F558F7BEA4FEE40D828BD47DE98D";
	uchar priexp2_10001[257]="7685AB311046DA1BC94378D5C6E52969046CF97AEE68C300B1024DB515ACA1F3B256321DC6C08400729C21E8991C461F43D1FE266CB23BB53BD2218B1F3C3192FD07FC529B366CF5D6111B56604453661FF9F00C3CA745CD2DD1F46DDB195A5B659F9767A00FA3F60821D26D01CA23924FA66271335E2EF318ADFCFCB8AA82CD";//本数据会测试出中端的一个跑飞BUG
	//uchar priexp2_10001[257]="5A5DA1C11461A1B70803ACA79BC594A5ABCF9EC84DBED0FFC4CCD8F33827469E303BEE7284B2D16F5D10520E38368D307D74EA2104A562EEF09E4BE1309399C7EBF7A46CDC2650073574A562CDC5BAFE24A1C015222246C81365DF43098F7519A4195497D31C0D2689BBE25BC5C87E27B330CA41D83FF35D5AC159FE8C3E9C5D";

	cls_printf("测试%s...", TESTAPI);
	//测试前置:生成秘钥对
	if((ret = NDK_AlgRSAKeyPairGen(RSA_KEY_LEN_2048,RSA_EXP_10001, &pubkey2,&prikey2)) !=NDK_OK)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case1: NDK_AlgRSAKeyPairVerify传入非法参数测试
	if((ret = NDK_AlgRSAKeyPairVerify(NULL,&prikey2)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_AlgRSAKeyPairVerify(&pubkey2,NULL)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	

	
	//人工构造公私钥对
	/*pubkey.bits=512;
	memcpy(pubkey.modulus,modulus,strlen((char *)modulus));
	memcpy(pubkey.exponent,exp,strlen((char *)exp));
	prikey.bits=512;
	memcpy(prikey.exponent,priexp,strlen((char *)priexp));
	memcpy(prikey.modulus,modulus,strlen((char *)modulus));*/
	pubkey.bits=1024;
	memcpy(pubkey.modulus, modulus_10001, strlen((char *)modulus_10001));
	memcpy(pubkey.exponent, exp_10001, strlen((char *)exp_10001));
	prikey.bits=1024;
	memcpy(prikey.exponent, priexp_10001, strlen((char *)priexp_10001));
	memcpy(prikey.modulus, modulus_10001, strlen((char *)modulus_10001));

	//case2:匹配的密钥对应返回NDK_OK
	if((ret = NDK_AlgRSAKeyPairVerify(&pubkey2,&prikey2)) !=NDK_OK)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_AlgRSAKeyPairVerify(&pubkey,&prikey)) !=NDK_OK)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:不匹配的密钥对应返回错
	if((ret = NDK_AlgRSAKeyPairVerify(&pubkey2,&prikey)) !=NDK_ERR)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	if((ret = NDK_AlgRSAKeyPairVerify(&pubkey,&prikey2)) !=NDK_ERR)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case4: 私钥指数不正确应返回错
	memcpy(prikey.exponent,priexp2_10001,strlen((char *)priexp2_10001));
	//memcpy(prikey.exponent,priexp2,strlen((char *)priexp2));
	if((ret = NDK_AlgRSAKeyPairVerify(&pubkey,&prikey)) !=NDK_ERR)
	{	
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 	
	return;
}

