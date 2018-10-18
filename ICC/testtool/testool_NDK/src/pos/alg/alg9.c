/************************************************************************
*
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			: 算法模块
* file name		    : alg9.c
* Author 			: zhengry	
* version			:
* DATE				: 20150907
* directory 		:
* description		: 测试NDK_AlgSM2Encrypt函数使用SM2非对称公私钥对中的公钥进行加密的功能（国密算法）
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
#define 	TESTAPI		"NDK_AlgSM2Encrypt"

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
void alg9(void)
{
	int ret=0;
	ushort len=0, cryp_len=0;
	uchar pubkey[64]={0}, prikey[32]={0}, cryp[MES_LEN+96]={0}, message[MES_LEN]={0};
	char sBuf[128]={0};
	unsigned int unLen=0;
	uchar err_pubkey[64]={0x82, 0xba, 0x45, 0xaf, 0x9c, 0xa8, 0xd8, 0xc8,
						  0x1f, 0x5a, 0x3a, 0xbd, 0x41, 0x77, 0x90, 0x3d,
						  0x3a, 0x22, 0xf0, 0xd7, 0x6e, 0xa0, 0xe1, 0x1f,
						  0xcc, 0x4c, 0x6a, 0xf4, 0x53, 0x6a, 0xac, 0xf4,
						  0xa6, 0x7a, 0x1c, 0x2e, 0xfb, 0xc3, 0x00, 0x17,
						  0xcd, 0xd8, 0x46, 0x22, 0x3e, 0xbc, 0xed, 0x89,
						  0x5d, 0xdc, 0x14, 0xeb, 0xf5, 0xdc, 0x7b, 0x17,
						  0xdf, 0xc5, 0xce, 0x18, 0x77, 0xc0, 0x7c, 0x00};//将正确的公钥修改了最后一个字节所得到一个不合法的公钥
	uchar real_message[]="newland";
	uchar cha_message[]="`~!01@新$#[^%]*(大)'-_=+|&{}:陆;?/><.,";//支持中文和特殊字符
	uchar max_message[]="dafdkjkkemfkweknfjndkanfknwejojfaeinfknfkdsnkfnjkdbngiuhqoptreopikpkdnnkwnjnfjnbkfasdfaesfaeafsfadfaedafdkjkkemfkweknfjndkanfknwejojfaeinfknfkdsnkfnjkdbngiuhqoptreopikpkdnnkwnjnfjnbkfasdfaesfaeafsfadfaedafdkjkkemfkweknfjndkanfknwejojfaeinfknfkdsnkfnjkdbngiuhqoptreopikpkdnnkwnjnfjnbkfasdfaesfaeafsfadfaedafdkjkkemfkweknfjndkanfknwejojfaeinfknfkdsnkfnjkdbngiuhqoptreopikpkdnnkwnjnfjnbkfasdfaesfaeafsfadfaedafdkjkkemfkweknfjndkanfknwejojfaeinfknfkdsnkfnjkdbngiuhqoptreopikpkdnnkwnjnfjnbkfasdfaesfaeafsfadfaedafdkjkkemfkweknfjndkanfknwejojfaeinfknfkdsnkfnjkdbngiuhqoptreopikpkdnnkwnjnfjnbkfasdfaesfaeafsfadfaedafdkjkkemfkweknfjndkanfknwejojfaeinfknfkdsnkfnjkdbngiuhqoptreopikpkdnnkwnjnfjnbkfasdfaesfaeafsfadfaedafdkjkkemfkweknfjndkanfknwejojfaeinfknfkdsnkfnjkdbngiuhqoptreopikpkdnnkwnjnfjnbkfasdfaesfaeafsfadfaedafdkjkkemfkweknfjndkanfknwejojfaeinfknfkdsnkfnjkdbngiuhqoptreopikpkdnnkwnjnfjnbkfasdfaesfaeafsfadfaedafdkjkkemfkweknfjn";
	/*uchar pre_pubkey[64]={0x82, 0xba, 0x45, 0xaf, 0x9c, 0xa8, 0xd8, 0xc8,
						  0x1f, 0x5a, 0x3a, 0xbd, 0x41, 0x77, 0x90, 0x3d,
						  0x3a, 0x22, 0xf0, 0xd7, 0x6e, 0xa0, 0xe1, 0x1f,
						  0xcc, 0x4c, 0x6a, 0xf4, 0x53, 0x6a, 0xac, 0xf4,
						  0xa6, 0x7a, 0x1c, 0x2e, 0xfb, 0xc3, 0x00, 0x17,
						  0xcd, 0xd8, 0x46, 0x22, 0x3e, 0xbc, 0xed, 0x89,
						  0x5d, 0xdc, 0x14, 0xeb, 0xf5, 0xdc, 0x7b, 0x17,
						  0xdf, 0xc5, 0xce, 0x18, 0x77, 0xc0, 0x7c, 0x99};
	uchar pre_message[16]={0xfb, 0x0c, 0x7f, 0xf7, 0xad, 0xfb, 0xee, 0xe5,
						   0x79, 0x70, 0xbe, 0x3a, 0x5c, 0xc5, 0x7e, 0x82};
	uchar pre_cryp[112]={0xe2, 0x93, 0x70, 0x1e, 0x4e, 0x9f, 0x4f, 0x5a,
						 0xd7, 0x08, 0xc6, 0xe9, 0xed, 0x62, 0x06, 0x0c,
						 0xd7, 0xb3, 0x7e, 0x33, 0x8a, 0x7b, 0xd7, 0xf3,
						 0x80, 0x7d, 0x14, 0x5d, 0xe0, 0x81, 0x06, 0x73,
						 0x77, 0xef, 0x54, 0x2c, 0xca, 0x29, 0x00, 0xa7,
						 0xdd, 0x92, 0x3f, 0x69, 0xfc, 0xa5, 0x14, 0x7c,
						 0x82, 0x34, 0xec, 0xf2, 0x66, 0x59, 0x90, 0xcc,
						 0xcd, 0xf3, 0x4c, 0x26, 0x9e, 0x41, 0xc3, 0x35,
						 0x63, 0x90, 0x99, 0x82, 0x7c, 0xa9, 0xf0, 0xf8,
						 0xa7, 0x52, 0x88, 0x3a, 0x93, 0xaa, 0xa3, 0x5e,
						 0x31, 0x62, 0x15, 0x7b, 0xd8, 0xb7, 0x62, 0x36,
						 0x8a, 0xc4, 0x1c, 0xa6, 0xe3, 0x1b, 0xea, 0x4f,
						 0x0e, 0xa3, 0x26, 0x17, 0x01, 0x66, 0x61, 0xab,
						 0x5d, 0x69, 0xcb, 0xfd, 0xbd, 0x4a, 0x16, 0xb7};
	uchar real_cryp[MES_LEN+96]={0};*/
	
	//process body
	cls_printf("测试%s...", TESTAPI);

	//测试前置
	memset(pubkey, 0, sizeof(pubkey));
	memset(prikey, 0, sizeof(prikey));
	memset(message, 0, sizeof(message));
	if((ret=NDK_AlgSM2KeyPairGen(pubkey, prikey))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	
	//case1:参数异常测试
	if((ret=NDK_AlgSM2Encrypt(NULL, real_message, strlen((char*)real_message), cryp, &cryp_len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2Encrypt(pubkey, NULL, strlen((char*)real_message), cryp, &cryp_len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2Encrypt(pubkey, real_message, 0, cryp, &cryp_len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2Encrypt(pubkey, real_message, strlen((char*)real_message), NULL, &cryp_len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2Encrypt(pubkey, real_message, strlen((char*)real_message), cryp, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2Encrypt(pubkey, real_message, MES_LEN+1, cryp, &cryp_len))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	
	//case2:异常测试: 在软算法的情况下，若公钥值不合法,应该返回失败；在硬算法的情况下，由于算法流程是硬件芯片内完成，无法判断
	memset(sBuf,0xff,sizeof(sBuf)-1);
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_HARDWARE_INFO, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(sBuf[15]==0xff)
	{
		if((ret=NDK_AlgSM2Encrypt(err_pubkey, real_message, strlen((char*)real_message), cryp, &cryp_len))!=NDK_ERR)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}	
	}
	
	//case3:正常测试:生成SM2密钥对，对明文进行加密和解密
	if((ret=NDK_AlgSM2Encrypt(pubkey, real_message, strlen((char*)real_message), cryp, &cryp_len))!=NDK_OK||cryp_len!=strlen((char*)real_message)+96)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, cryp_len);
		RETURN;
	}
	if((ret=NDK_AlgSM2Decrypt(prikey, cryp, strlen((char*)real_message)+96, message, &len))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(message, real_message, len))
	{
		send_result("line %d:%sSM2加解密失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case4:正常测试:明文的大小为1024-96字节，加密应该成功
	memset(cryp, 0, sizeof(cryp));
	memset(message, 0, sizeof(message));
	if((ret=NDK_AlgSM2Encrypt(pubkey, max_message, strlen((char*)max_message), cryp, &cryp_len))!=NDK_OK||cryp_len!=strlen((char*)max_message)+96)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, cryp_len);
		RETURN;
	}
	if((ret=NDK_AlgSM2Decrypt(prikey, cryp, strlen((char*)max_message)+96, message, &len))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(message, max_message, len))
	{
		send_result("line %d:%sSM2加解密失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case5:正常测试:明文为中文和特殊字符，加密应该成功
	memset(cryp, 0, sizeof(cryp));	
	memset(message, 0, sizeof(message));
	if((ret=NDK_AlgSM2Encrypt(pubkey, cha_message, strlen((char*)cha_message), cryp, &cryp_len))!=NDK_OK||cryp_len!=strlen((char*)cha_message)+96)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, cryp_len);
		RETURN;
	}
	if((ret=NDK_AlgSM2Decrypt(prikey, cryp, strlen((char*)cha_message)+96, message, &len))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(message, cha_message, len))
	{
		send_result("line %d:%sSM2加解密失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case6:正常测试:预设公钥来加密，加密的密文和预期密文应该一致
	/*memset(cryp, 0, sizeof(cryp));
	if((ret=NDK_AlgSM2Encrypt(pre_pubkey, pre_message, sizeof(pre_message), cryp, &cryp_len))!=NDK_OK||cryp_len!=strlen(pre_message)+96)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, cryp_len);
		RETURN;
	}
	if(memcmp(cryp, pre_cryp, cryp_len))
	{
		send_result("line %d:%sSM2加密失败", __LINE__, TESTAPI);
		RETURN;
	}*/
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 	
	return;

}


