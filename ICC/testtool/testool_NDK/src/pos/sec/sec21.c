/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec7.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecCalcDesDUKPT接口功能
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SecCalcDesDUKPT"	


/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author		date		remarks
*			  	  	   linrq       20130228	created
*						linwl			20150125	modified
*****************************************************************/
void sec21(void)
{
	int ret = 0, i = 0, j=0;
	ST_SEC_KCV_INFO KcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar DataIn[1024+8]={0},KeyValue[17]={0},KSN1[11]={0},KSN2[11]={0},KSN3[11]={0},sIV[9]={0}, DataOut[1024+8]={0}, En_DataOut[1024]={0}, PreOut[1024+8]={0};
#if TR31_ENABLE
	tr31_key_t tr31_st;
	unsigned int len = 0;
	uchar mkey[17]={0},block[512]={0};
#endif
#if SEC_RSA_ENABLE
	ST_SEC_RSA_KEY RsaKeyIn;
	uchar *uModeulus1=(uchar *)"4FB2D08918F766B30B2144FE25D09A160FFCF256086D82773FB04D4BD7E4772F2557FC25EE130A0B984467995BE80EE1751F577BD3E5CAFF60712722F33D0479";
#endif
	uchar DukptKeyValue[16]={0xD5,0xD4,0x4F,0xF7,0x20,0x68,0x3D,0x0D,0x70,0xBE,0x25,0x28,0x18,0xE2,0xA1,0x8A};//DUKPY初始明文密钥
	uchar KSN[11]={0};//初始KSN
#if OVERSEAS_ENABLE
	int num=3;
	int sec_des_mode[3]={0, SEC_DES_MODE_ECB, SEC_DES_MODE_CBC};
	int sec_tdk_key_type[3]={SEC_KEY_TYPE_TDK, SEC_KEY_TYPE_RESP_TDK, SEC_KEY_TYPE_RESP_TDK};
	int sec_tak_key_type[3]={SEC_KEY_TYPE_TAK, SEC_KEY_TYPE_RESP_TAK, SEC_KEY_TYPE_RESP_TAK};
	char en_out_1024[1024]={0x2d,0xc0,0xda,0x58,0xdc,0x03,0xfe,0x24,0x25,0x0d,0x0d,0x38,0x5b,0x7e,0xc5,0x62,0xf7,0x28,0x7a,0xc0,0x95,0xb2,0xba,0xd0,0xa7,0xff,0xfb,0x37,0x46,0x37,0x3c,0xc1,0x6d,0xd4,0x9e,0x8e,0x0c,0x94,0xbb,0x3f,0xe5,0x79,0x03,0x82,0xb0,0x07,0x90,0xb1,0x29,0x70,0x0a,0xfa,0x39,0xfc,0x4e,0xe1,0x0b,0x53,0x4a,0x4e,0x2d,0x72,0x86,0x9b,
							0x09,0x33,0xa3,0x65,0xb7,0x37,0x42,0x9c,0xcf,0x25,0xb9,0x1f,0xa6,0x7e,0xcf,0xcf,0xe6,0xc8,0x5f,0x29,0xee,0x23,0x0f,0x31,0x91,0x95,0x13,0xa4,0x88,0xd5,0xe3,0x1d,0x91,0x81,0x06,0x1f,0x9c,0xcb,0x6c,0x5f,0xe8,0xd7,0x64,0x6c,0x60,0xe0,0x2f,0x8a,0xae,0x61,0x1e,0x77,0x6a,0x43,0x80,0x66,0x42,0xa0,0x02,0x66,0xce,0x17,0x0d,0x32,
							0x35,0x30,0x5a,0xdf,0x4d,0xf5,0xb1,0xdd,0xa9,0x56,0xc3,0x61,0xc4,0x8b,0x4a,0xa3,0x26,0x7f,0xb9,0x61,0x68,0xb1,0x29,0xfc,0x92,0x04,0x68,0x9f,0xff,0xfc,0x68,0x24,0xde,0x1d,0x27,0x13,0x84,0x1c,0x4c,0x8a,0x48,0x5d,0x3f,0xfb,0x61,0xee,0xb2,0xf7,0x9e,0x5c,0x89,0xf9,0x38,0xf6,0x2c,0x82,0xf8,0x3b,0xbc,0x41,0xb5,0x67,0xdc,0x00,
							0x73,0x81,0x46,0x97,0x07,0x5d,0xe4,0x18,0xfb,0xed,0x54,0xac,0x1f,0xaf,0xf1,0x15,0x82,0xc5,0xd4,0x02,0x05,0xcf,0x6d,0xd7,0x22,0x09,0x6c,0x72,0x5b,0x12,0x8f,0x66,0xe4,0xe2,0x10,0xdc,0x22,0xb9,0xd1,0x08,0x81,0x32,0xe6,0x4a,0x4f,0x37,0xdf,0xb7,0x98,0xeb,0x3a,0xfa,0xf6,0x7f,0x30,0x89,0xc6,0x60,0x14,0x51,0x0f,0x59,0x45,0x63,
							0xf6,0x04,0xc8,0x56,0x9d,0x5a,0xa2,0x87,0x7e,0x6f,0x71,0x6a,0x57,0xfe,0x03,0x74,0x04,0x7e,0xb6,0xa0,0x6f,0x98,0x87,0x64,0x99,0x9b,0xd7,0x88,0x38,0x66,0x00,0x83,0xf3,0xfa,0x3f,0x19,0x22,0x6c,0xc7,0xbb,0x0c,0xeb,0xbb,0x41,0xa2,0x0e,0x22,0xe8,0x9c,0xbd,0x3d,0x79,0x13,0x15,0xc7,0x01,0x19,0x91,0x82,0x7d,0x3d,0x85,0x20,0x69,
							0xf4,0x28,0xf6,0xdd,0xab,0x44,0xba,0x53,0x02,0xec,0x5e,0x6a,0x19,0xc5,0x2e,0x53,0xac,0xa0,0xa4,0xa8,0x47,0x2f,0x7a,0x44,0x7f,0x19,0xd5,0x72,0x61,0xd6,0x9b,0xc5,0xcd,0x74,0x7a,0xec,0x9b,0x51,0x97,0x51,0x44,0x08,0x16,0x1c,0x8c,0x73,0xfc,0x31,0x20,0xbe,0x45,0x92,0xb4,0x96,0x4f,0x9c,0xcf,0xfa,0xe5,0x6f,0xa0,0xce,0xca,0x61,
							0xee,0xf4,0x02,0xb1,0x6f,0x70,0xce,0x9e,0x7e,0x44,0x03,0xc6,0xaa,0x33,0x50,0xe4,0xd3,0x3b,0x36,0x52,0xa4,0xc1,0xb0,0x9a,0xf9,0xae,0x70,0x36,0x85,0x09,0xe1,0x3f,0xc2,0x4c,0xdf,0x9a,0xbf,0x85,0xbd,0x04,0x6f,0x3b,0xb3,0xa8,0x86,0x50,0x10,0x43,0xbe,0xd8,0x4f,0x4a,0x14,0x21,0x00,0xf4,0xa5,0x17,0x4c,0xf0,0x61,0xe7,0x32,0xca,
							0x8b,0x3f,0xef,0x8a,0xa0,0x14,0x1c,0x28,0x9e,0xc2,0xfe,0x1f,0x12,0x5e,0xaf,0x77,0xe0,0x89,0xb1,0x21,0x45,0x61,0x36,0x53,0xf2,0xab,0x5f,0xb3,0xe0,0x08,0x95,0x0c,0x24,0xe9,0x1e,0x1f,0xd2,0x95,0xf7,0xd7,0x57,0x7a,0xe3,0x62,0x39,0xbe,0x45,0x0f,0x4d,0x26,0x6a,0xa0,0x8a,0xbe,0xe9,0xca,0x3d,0x2f,0x15,0x22,0x87,0x0a,0x64,0x35,
							0x0d,0x65,0x2c,0xba,0xa2,0xfd,0x06,0xe0,0xff,0xf9,0xad,0xd7,0x0a,0x42,0xd0,0xc5,0x87,0x44,0x80,0x7f,0x98,0x05,0x1d,0x56,0xef,0xf6,0x47,0x27,0xa6,0xca,0xca,0xa2,0xa3,0x54,0x13,0x60,0x5d,0x8c,0x0a,0x74,0x27,0xdc,0x8b,0x61,0x32,0x22,0x15,0xd0,0x46,0xae,0x72,0x13,0x9c,0x20,0xb7,0xe9,0x15,0x00,0x0f,0x0f,0xd0,0x21,0x2a,0xd3,
							0xd7,0x99,0xa6,0xfb,0x91,0x0c,0xb2,0xb5,0x40,0x2b,0x9f,0xfc,0xaa,0x91,0x9b,0x3d,0x09,0x67,0xdd,0x6f,0xd9,0xa6,0xb6,0xd8,0x9d,0x39,0x57,0x4a,0xd7,0xc1,0xcc,0x02,0xae,0xdc,0x1e,0xab,0xab,0xbf,0x43,0xac,0xf4,0x73,0xe5,0xb2,0x45,0xdf,0xe2,0x60,0xa4,0x5a,0xf8,0x8b,0x0e,0xb8,0xc5,0x5c,0xc9,0x90,0x5f,0xe1,0xf5,0x35,0xfa,0x48,
							0x21,0x52,0x1b,0x64,0x05,0x2b,0x3e,0xb2,0xd2,0x1a,0x27,0x97,0x9f,0xa5,0xd0,0x37,0x03,0xd3,0xb4,0xa0,0xa9,0x68,0x0e,0x0e,0xff,0x00,0x4c,0x8e,0x57,0xd8,0x2d,0xa8,0x6c,0x8b,0x4d,0x35,0x25,0x8c,0xb0,0x5b,0x58,0xf5,0xf5,0x24,0xad,0xfe,0x9d,0x49,0x4b,0x74,0x67,0xb0,0x68,0x75,0x88,0xc9,0x6d,0xe9,0x24,0xdd,0x98,0xff,0xaa,0xa7,
							0xda,0x5a,0x5e,0xeb,0x69,0xbd,0x5c,0xb7,0xc7,0x53,0x17,0x18,0x75,0x29,0x14,0xec,0xc4,0x56,0x3f,0x6e,0xed,0xc5,0x56,0x12,0xa5,0xe5,0x2d,0x86,0x7c,0xe8,0xc2,0x81,0x86,0x4d,0xe1,0xe4,0xf5,0xe3,0x7f,0x8e,0xba,0x3d,0xc0,0xe7,0xaa,0xaa,0xed,0xc2,0x7e,0xb0,0x30,0x2b,0x4f,0x57,0x0f,0x28,0x5f,0xdb,0xb2,0x59,0x98,0x03,0x02,0x4e,
							0x2a,0x64,0xcf,0xf9,0x73,0x12,0xea,0x49,0x7d,0x24,0x8a,0xc3,0x52,0x65,0x4e,0x60,0x51,0x26,0xc5,0x5e,0xc9,0x6d,0x6e,0xb9,0xf7,0x8c,0x13,0x0c,0x56,0xa6,0x3c,0x2f,0xf2,0x59,0x9f,0x70,0x93,0x4c,0x07,0xee,0x59,0xf2,0xdb,0xdb,0xa7,0x1f,0x06,0xbe,0x2d,0x8b,0x22,0xc0,0xea,0x6f,0x5d,0xef,0x7d,0xaf,0x6c,0x19,0x60,0x78,0x41,0x2c,
							0xb4,0x0d,0x11,0x22,0xca,0xb7,0x28,0xe3,0x2c,0x88,0xb0,0xa8,0x0f,0x8f,0x36,0xc7,0x46,0x9c,0xac,0x9d,0x69,0x7e,0x42,0xe1,0x37,0x2c,0xbb,0x0a,0x26,0x4d,0x6e,0x1d,0xca,0xc7,0xcd,0x23,0x4c,0xfe,0x7f,0x60,0xb1,0xed,0xd3,0x97,0xc4,0xa2,0xd8,0xa7,0x0e,0xd2,0x7c,0x2c,0xa1,0x3a,0xff,0x5c,0xcc,0x07,0x7e,0x23,0x02,0xa2,0x5e,0xdc,
							0xf7,0x13,0x35,0x9e,0x9e,0x64,0x74,0xc4,0x63,0xce,0x22,0x06,0x14,0xaa,0x73,0x9e,0x22,0x6b,0xce,0x08,0x40,0x4c,0x34,0xb2,0x6d,0x20,0x87,0xac,0x4f,0x2f,0xa0,0x34,0xe8,0x99,0xc8,0xfb,0x42,0x85,0xb0,0xfb,0x29,0xcf,0x74,0x76,0xc8,0xd2,0xfc,0x8d,0x5c,0x1d,0x21,0xe5,0x1b,0x2e,0xd8,0x04,0x4e,0x75,0x50,0x8d,0x93,0x8f,0x04,0x6c,
							0x14,0x53,0x07,0xec,0x06,0x71,0xbf,0xd7,0xbf,0xbc,0xe9,0xce,0x1c,0x80,0xea,0x1f,0x24,0x8f,0x5d,0x74,0x47,0x03,0x17,0xd5,0x72,0xd2,0xa6,0x40,0xb5,0x17,0x96,0x7e,0x92,0x19,0x4c,0x28,0xf5,0x02,0x60,0xa0,0xd2,0xd5,0xd8,0x46,0xcb,0x4a,0xa3,0x6e,0xa4,0x1d,0x4c,0x1d,0xd7,0x19,0x0e,0x18,0xb5,0x5f,0x09,0x0f,0xeb,0x1c,0x7d,0xc2,};
#else
	int num=1;
	int sec_des_mode[1]={0};
	int sec_tdk_key_type[1]={SEC_KEY_TYPE_TDK};
	int sec_tak_key_type[1]={SEC_KEY_TYPE_TAK};
#endif

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI); 

	//注释本用例使用默认KSN为10字节00，最后3个字节为KSN计数为000000
	//默认生产DUKPT的初始密钥为D5D44FF720683D0D70BE252818E2A18A
	//安装好后的派生前密钥为6ef92795e512714dcdeb767b33f1a9bd  对应KSN为000001
	//通过以下规则派生出各种DUKPT密钥
	/*
	case SEC_KEY_TYPE_TPK:		//派生PIN KEY   	6ef92795e51271b2cdeb767b33f1a942
		key[7] ^= 0xff;
		key[15] ^= 0xff;
		break;
	case SEC_KEY_TYPE_TAK:		//派生MAC KEY 		6ef92795e5128e4dcdeb767b33f156bd
		key[6] ^= 0xff;
		key[14] ^= 0xff;
		break;
	case SEC_KEY_TYPE_TDK:		//派生数据加解密KEY 6ef92795e5ed714dcdeb767b330ea9bd    所有产品陆续导入:需要把自己当数据和密钥进行3DES运算  BFA9CA10BF791459 482339A090BF7CC9 为实际密钥
		key[5] ^= 0xff;
		key[13] ^= 0xff;
	case SEC_KEY_TYPE_RESP_TAK: //派生MAC RESPONSE  6EF927951A12714DCDEB767BCCF1A9BD    
		key[4] ^= 0xff;
		key[12] ^= 0xff;
	case SEC_KEY_TYPE_RESP_TDK: //派生数据加解密KEY RESPONSE    6EF9276AE512714DCDEB768433F1A9BD  海外平台产品需要把自己当数据和密钥进行3DES运算 B3EBB63165DC301B B0A878B8686A67C6 为实际密钥
		key[3] ^= 0xff;
		key[11] ^= 0xff;		
		*/
	//KSN自加后生成的派生前密钥为 5635b184dd5d8f415617b6600c6e47cf
	/*由于海外版本要求安装的密钥不相同id2 id3相应的修改了DUKPT的初始密钥
		id=2的初始DUKPT密钥为:4B82DBD972BBBF20BCBA8632CF9AB57C
		安装好后的派生前密钥为bae698c215018dd5e3cf99433dd1560e  对应KSN为000001
		id=3的初始DUKPT密钥为:25AB8A24A244883CB2CA76D7617BC60B
		安装好后的派生前密钥为9b53578c8110f7c6f3bf8ca3bbff6358  对应KSN为000001
		派生方法如上,这里不具体列出
	*/
	//测试前置:擦除所有密钥
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//明文安装一组DUKPT, ID=6, KSN为10个字节0x00 /*约定:以下KCV均以不校验方式安装*/
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(KSN, 0x00, 10);//初始KSN
	if ((ret=NDK_SecLoadTIK(6, 0, 16, DukptKeyValue, KSN, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//安装TLK, ID=1,本密钥用于密文安装第ID=2的DUKPT密钥
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x11, stKeyInfo.nDstKeyLen);
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	 //由于海外版本禁止安装相同的DES密钥 (包括DES密钥,DUKPT 初始密钥TIK) 国兵解释,故这里使用不同的密钥安装使用于所有平台产品 20180116 modify
	//密文安装一组DUKPT, ID=2, KSN为10个字节0x00 \xDA\x36\xEC\x83\x4A\xDE\x38\xD9\x5A\x51\x68\xF9\x5E\x9A\x5E\x23 是如下明文 经ID1密钥加密后的密文(分成前后两个8字节计算)
	//此处密文是明文为:4B82DBD972BBBF20BCBA8632CF9AB57C与16位0x11加密后的结果:1A5DA388BAB978A8AA577F104AF0FC9B
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memcpy(KeyValue, "\x1A\x5D\xA3\x88\xBA\xB9\x78\xA8\xAA\x57\x7F\x10\x4A\xF0\xFC\x9B", 16);
	if ((ret=NDK_SecLoadTIK(2, 1, 16, KeyValue, KSN, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0  //这里安装后的id2与id6是一样的密钥 
	//密文安装一组DUKPT, ID=2, KSN为10个字节0x00 \xDA\x36\xEC\x83\x4A\xDE\x38\xD9\x5A\x51\x68\xF9\x5E\x9A\x5E\x23 是DukptKeyValue 经ID1密钥加密后的密文(分成前后两个8字节计算)
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memcpy(KeyValue, "\xDA\x36\xEC\x83\x4A\xDE\x38\xD9\x5A\x51\x68\xF9\x5E\x9A\x5E\x23", 16);
	if ((ret=NDK_SecLoadTIK(2, 1, 16, KeyValue, KSN, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if TR31_ENABLE	//支持TR31格式密钥安装的设备才进行TR31测试
	//TR-31密文安装一组DUKPT, ID=3, KSN为10个字节0x00,计算出的结果  block 安装到ID=3中
	memset(KSN, 0x00, 10);
	memset(mkey, 0x11, 16);
	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = 3;
	memcpy(tr31_st.usage, KT_BDK_TYPE, 4);	/*DUKPT*/
	 //由于海外版本禁止安装相同的密钥故这里改成使用不同的明文安装20180116 modify by jym
	//安装的明文修改为25AB8A24A244883CB2CA76D7617BC60B
	memcpy(DukptKeyValue, "\x25\xAB\x8A\x24\xA2\x44\x88\x3C\xB2\xCA\x76\xD7\x61\x7B\xC6\x0B", 16);
	tr31_st.key = DukptKeyValue;//将DUKPT明文密钥按照TR31格式进行计算后得到block
	tr31_st.keylen = 16;
	
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//将DUKPT明文密钥按照TR31格式进行计算后得到block，然后进行安装
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadTIK(3, 1, len, block, KSN, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else
	//明文25AB8A24A244883CB2CA76D7617BC60B与16位0x11des加密结果(分成前后8字节)3CC6B71F7117702C3850C279A2CF834D
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(KSN, 0x00, 10);
	memcpy(KeyValue, "\x3C\xC6\xB7\x1F\x71\x17\x70\x2C\x38\x50\xC2\x79\xA2\xCF\x83\x4D", 16);
	if ((ret=NDK_SecLoadTIK(3, 1, 16, KeyValue, KSN, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if SEC_RSA_ENABLE
	//明文安装RSA, ID=4
	RsaKeyIn.usBits=RSA_KEY_LEN_512;
	strcpy((char *)RsaKeyIn.sExponent, "03");
	memcpy(RsaKeyIn.sModulus, uModeulus1, RsaKeyIn.usBits*2/8);
	if((ret=NDK_SecLoadRsaKey(4, &RsaKeyIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//明文安装TDK, ID=5
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x21, stKeyInfo.nDstKeyLen);
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &KcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//约定:若无特殊说明sIV取全零(或传NULL)
	memset(sIV, 0x00, sizeof(sIV));//初始向量参数用于扩展，以后可能实现支持cbc加密模式等，目前无效

	//case1:非法测试
	//case1.1:psDataIn==NULL，应该返回NDK_ERR_PARA
	memset(DataIn, 0x33, sizeof(DataIn));
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TDK, sIV, 8, NULL, DataOut, KSN1, SEC_DES_ENCRYPT/*|SEC_DES_KEYLEN_8|SEC_DES_ECBMODE*/))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.2:psDataOut==NULL，应该返回NDK_ERR_PARA
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TDK, sIV, 8, DataIn, NULL, KSN1, SEC_DES_ENCRYPT/*|SEC_DES_KEYLEN_8|SEC_DES_ECBMODE*/))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case1.3:ucMod 非法// 开发解释ucMod通过判断比特位,故不存在非法值
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TDK, sIV, 8, DataIn, DataOut, KSN1, 100))!=NDK_OK/*NDK_ERR_PARA*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.4:uckeyIdx不存在(10,101)
	if((ret=NDK_SecCalcDesDukpt(10,SEC_KEY_TYPE_TDK, sIV, 8, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECKM_READ_REC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR
	}
	if((ret=NDK_SecCalcDesDukpt(101,SEC_KEY_TYPE_TDK, sIV, 8, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECKM_READ_REC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.5:nDataInLen非8整数倍(nDataInLen=5)
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TDK, sIV, 5, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		//GOTOERR;
	}

	//case1.6:psDataOut=NULL
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TDK, sIV, 8, DataIn, NULL, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.7:psKsnOut=NULL
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TDK, sIV, 8, DataIn, DataOut, NULL, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if ANDIROD_ENABLE
	//case1.8:输入数据psDataIn大于1024时返回错误	//20160531根据N900 3G版本 V1.1.16新增用例
	//memset(DataIn, 5, sizeof(DataIn));
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TDK, NULL, 1024+8, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	/**************************
	SEC_DES_MODE_ECB模式工作原理:
	明文数据以8字节为一块,密钥与明文块0做des加密得到密文块0,密钥再与下个明文块x分别做des加密得到相应的密文块x后,将密文块组合起来即得到相应的密文
	SEC_DES_MODE_CBC模式工作原理:
	明文数据以8字节为一块,第一次IV与明文块0异或后的结果再与密钥做des加密得到密文块0,得到的这个密文再与下个明文块x异或后的结果与密钥做des加密得到相应的密文块x,将密文块组合起来即得到相应的密文
	**************************/
	for(j=0; j<num; j++)
	{
		//case2.1:密钥类型为SEC_KEY_TYPE_TDK,以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8|SEC_DES_ECBMODE模式, 8字节数据输入,DUKPT6,psIV=NULL
		//case2.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8|SEC_DES_ECBMODE模式, 8字节数据输入,DUKPT6,psIV=NULL
		//case2.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8|SEC_DES_MODE_CBC模式, 8字节数据输入,DUKPT6,psIV=NULL
		/***这里有异常DUKPT密钥都是16长度的，但实际传入SEC_DES_KEYLEN_8会使用8字节进单DES运行，其他地方类似问题，国兵解释不修改**/
		memset(DataOut, 0, sizeof(DataOut));
		memset(PreOut, 0, sizeof(PreOut));
		if((ret=NDK_SecCalcDesDukpt(6, sec_tdk_key_type[j] , NULL, 8, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8|sec_des_mode[j]))!=NDK_OK)  
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if(j==0)
		{
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM) //ME 31s导入新安全库故做调整 20180511 modify //ME30S虽然是海外产品但不支持SEC_KEY_TYPE_RESP_TDK和SEC_KEY_TYPE_RESP_TAK
			//使用8字节密钥6ef92795e5ed714d(该密钥由16字节DUKPT密钥的前8字节密钥组成)对8字节数据3333333333333333进行加密后拼接得到结果
			memcpy(PreOut,"\xE4\x87\xC1\x16\xA5\x48\xB9\x2C", 8);
#else
			//使用8字节密钥BFA9CA10BF791459(该密钥由16字节DUKPT密钥的前8字节密钥组成)对8字节数据3333333333333333进行des加密后拼接得到结果0C547B0E8F24275F
			memcpy(PreOut,"\x0C\x54\x7B\x0E\x8F\x24\x27\x5F", 8);
#endif
		}
		else if(j==1 || j==2)
			//当IV为NULL时,明文数据为8字节时,SEC_DES_MODE_ECB与SEC_DES_MODE_CBC模式下，加密结果一样
			//以SEC_DES_MODE_ECB/SEC_DES_MODE_CBC模式,使用8字节密钥B3EBB63165DC301B(该密钥由16字节派生数据加解密KEY的前8字节密钥组成)对8字节数据3333333333333333进行加密后拼接得到结果
			memcpy(PreOut,"\x7C\xA5\x3C\x79\x1E\xD2\xF4\x6F", 8);
		if(memcmp(DataOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(j=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, j, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7]);
			GOTOERR;
		}

		//case3.1:密钥类型为SEC_KEY_TYPE_TDK,以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|SEC_DES_ECBMODE模式, 16字节数据输入,DUKPT6,psIV=8个0x00
		//case3.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|SEC_DES_ECBMODE模式, 16字节数据输入,DUKPT6,psIV=8个0x00
		//case3.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|SEC_DES_MODE_CBC模式, 16字节数据输入,DUKPT6,psIV=8个0x00
		memset(sIV, 0x00, sizeof(sIV));
		memset(DataOut, 0, sizeof(DataOut));
		memset(PreOut, 0, sizeof(PreOut));
		if((ret=NDK_SecCalcDesDukpt(6,sec_tdk_key_type[j], sIV, 16, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|sec_des_mode[j]))!=NDK_OK)  //linrq NDK_OK
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if(j==0)
		{
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM)
			//本预期结果使用16字节密钥6ef92795e5ed714dcdeb767b330ea9bd对16字节数据分两个8字节(3333333333333333)进行加密计算后拼接成16字节结果
			memcpy(PreOut,"\xC6\xDB\x29\xAB\xCB\x5D\x0C\x32\xC6\xDB\x29\xAB\xCB\x5D\x0C\x32", 16);
#else
			//本预期结果使用16字节密钥BFA9CA10BF791459482339A090BF7CC9对16字节数据分两个8字节(3333333333333333)进行加密计算后拼接成16字节结果60DEF45C79ED5B06
			memcpy(PreOut,"\x60\xDE\xF4\x5C\x79\xED\x5B\x06\x60\xDE\xF4\x5C\x79\xED\x5B\x06", 16);
#endif
		}
		else if(j==1)
			//以SEC_DES_MODE_ECB模式,使用16字节派生数据加解密KEY:B3EBB63165DC301B B0A878B8686A67C6对16字节数据分两个8字节(3333333333333333)进行3des加密计算后拼接成16字节结果
			memcpy(PreOut,"\x85\x95\x78\xE2\xBD\x3C\xA6\x6A\x85\x95\x78\xE2\xBD\x3C\xA6\x6A", 16);
		else if(j==2)
			//以SEC_DES_MODE_CBC模式,使用16字节派生数据加解密KEY:B3EBB63165DC301B B0A878B8686A67C6对16字节数据(0x33)前八字节做3des加密得到859578E2BD3CA66A，加密结果与后八字节数据做异或得到B6A64BD18E0F9559，再与密钥做3des计算，得到加密结果
			memcpy(PreOut,"\x85\x95\x78\xE2\xBD\x3C\xA6\x6A\x99\x67\xBD\x7C\x10\x63\x51\xEF", 16);
		if(memcmp(DataOut, PreOut, 16))
		{
			send_result("line %d:%s测试失败(j=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, j, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7], DataOut[8], DataOut[9], DataOut[10], DataOut[11], DataOut[12], DataOut[13], DataOut[14], DataOut[15]);
			GOTOERR;
		}

		//case4.1:密钥类型为SEC_KEY_TYPE_TDK,以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24|SEC_DES_ECBMODE模式, 24字节数据输入,DUKPT6,psIV=非0值
		//case4.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24|SEC_DES_ECBMODE模式, 24字节数据输入,DUKPT6,psIV=非0值
		//case4.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24|SEC_DES_MODE_CBC模式, 24字节数据输入,DUKPT6,psIV=非0值
		memset(sIV, 0x12, sizeof(sIV));
		memset(DataOut, 0, sizeof(DataOut));
		memset(PreOut, 0, sizeof(PreOut));
		if((ret=NDK_SecCalcDesDukpt(6,sec_tdk_key_type[j], sIV, 24, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24|sec_des_mode[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if(j==0)
		{
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM)
			//使用24字节密钥6ef92795e5ed714dcdeb767b330ea9bd6ef92795e5ed714d(该密钥由16字节DUKPT密钥+前8字节密钥组成)对3组8字节数据3333333333333333进行加密后拼接得到结果
			memcpy(PreOut,"\xC6\xDB\x29\xAB\xCB\x5D\x0C\x32\xC6\xDB\x29\xAB\xCB\x5D\x0C\x32\xC6\xDB\x29\xAB\xCB\x5D\x0C\x32", 24);
#else
			//使用24字节密钥BFA9CA10BF791459482339A090BF7CC9BFA9CA10BF791459(该密钥由16字节DUKPT密钥+前8字节密钥组成)对3组8字节数据3333333333333333进行加密后拼接得到结果
			memcpy(PreOut,"\x60\xDE\xF4\x5C\x79\xED\x5B\x06\x60\xDE\xF4\x5C\x79\xED\x5B\x06\x60\xDE\xF4\x5C\x79\xED\x5B\x06", 24);
#endif
		}
		else if(j==1)
			//以SEC_DES_MODE_ECB模式,使用24字派生数据加解密KEY:B3EBB63165DC301B B0A878B8686A67C6(该密钥由16字节DUKPT密钥+前8字节密钥组成)对3组8字节数据3333333333333333进行3des加密计算后拼接成结果
			memcpy(PreOut,"\x85\x95\x78\xE2\xBD\x3C\xA6\x6A\x85\x95\x78\xE2\xBD\x3C\xA6\x6A\x85\x95\x78\xE2\xBD\x3C\xA6\x6A", 24);
		else if(j==2)
			//以SEC_DES_MODE_CBC模式,使用24字派生数据加解密KEY:B3EBB63165DC301B B0A878B8686A67C6(该密钥由16字节DUKPT密钥+前8字节密钥组成)对24字节数据(0x33)前八字节与IV做异或后进行3DES加密F0A8F0600FAF7F89，加密结果与中间8字节数据异或后再加密B0B7D4B2F14830C0，加密结果再与最后8字节数据异或后再加密3D7AC8138C7D83D4
			memcpy(PreOut,"\xF0\xA8\xF0\x60\x0F\xAF\x7F\x89\xB0\xB7\xD4\xB2\xF1\x48\x30\xC0\x3D\x7A\xC8\x13\x8C\x7D\x83\xD4", 24);
		if(memcmp(DataOut, PreOut, 24))
		{
			send_result("line %d:%s测试失败(j=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, j, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7], DataOut[8], DataOut[9], DataOut[10], DataOut[11], DataOut[12], DataOut[13], DataOut[14], DataOut[15], DataOut[16], DataOut[17], DataOut[18], DataOut[19], DataOut[20], DataOut[21], DataOut[22], DataOut[23]);
			GOTOERR; 
		}

		//case18.1:密钥类型为SEC_KEY_TYPE_TDK,以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|SEC_DES_ECBMODE模式, 1024字节数据输入,DUKPT6,psIV=非0值,为测试高端AZ平台大数据加密而增加的1024字节数据加密测试20170220
		//case18.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|SEC_DES_ECBMODE模式, 1024字节数据输入,DUKPT6,psIV=非0值
		//case18.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|SEC_DES_MODE_CBC模式, 1024字节数据输入,DUKPT6,psIV=非0值
		memset(sIV, 0x14, sizeof(sIV));
		memset(DataOut, 0, sizeof(DataOut));
		memset(En_DataOut, 0, sizeof(En_DataOut));
		if((ret=NDK_SecCalcDesDukpt(6,sec_tdk_key_type[j], sIV, 1024, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|sec_des_mode[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		for(i=0;i<128;i++)
		{
			if(j==0)					
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM)
				//本预期结果使用16字节密钥6ef92795e5ed714dcdeb767b330ea9bd对1024字节数据分128个8字节(3333333333333333)进行加密计算后拼接成1024字节结果
				memcpy(En_DataOut+i*8, "\xC6\xDB\x29\xAB\xCB\x5D\x0C\x32", 8);
#else
				//本预期结果使用16字节密钥BFA9CA10BF791459482339A090BF7CC9对1024字节数据分128个8字节(3333333333333333)进行加密计算后拼接成1024字节结果
				memcpy(En_DataOut+i*8, "\x60\xDE\xF4\x5C\x79\xED\x5B\x06", 8);
#endif
			else if(j==1)
				//以SEC_DES_MODE_ECB模式,使用16字派生数据加解密KEY:B3EBB63165DC301B B0A878B8686A67C6对1024字节数据(128组8字节数据3333333333333333)进行3des加密计算后拼接成结果
				memcpy(En_DataOut+i*8, "\x85\x95\x78\xE2\xBD\x3C\xA6\x6A", 8);
		}
#if OVERSEAS_ENABLE
		if(j==2)
			//以SEC_DES_MODE_CBC模式,使用16字派生数据加解密KEY:6EF9276AE512714DCDEB768433F1A9BD对1024字节数据(128组8字节数据3333333333333333)进行加密计算后拼接成结果
			memcpy(En_DataOut, en_out_1024, 1024);
#endif
		if(memcmp(DataOut,En_DataOut, 1024))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			//GOTOERR;
		}

		//case5.1:密钥类型为SEC_KEY_TYPE_TDK,以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_8|SEC_DES_ECBMODE模式, 8字节数据输入,DUKPT2,psIV=NULL
		//case5.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_8|SEC_DES_ECBMODE模式, 8字节数据输入,DUKPT2,psIV=NULL
		//case5.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_8|SEC_DES_MODE_CBC模式, 8字节数据输入,DUKPT2,psIV=NULL
		memset(DataOut, 0, sizeof(DataOut));
		memset(PreOut, 0, sizeof(PreOut));
		if((ret=NDK_SecCalcDesDukpt(2,sec_tdk_key_type[j], NULL, 8, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_8|sec_des_mode[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if(j==0)
		{
			//id2 派生TDK:bae698c215fe8dd5e3cf99433d2e560e 派生respTDK:bae6983d15018dd5e3cf99bc3dd1560e
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM)
			//本预期结果使用8字节密钥bae698c215fe8dd5对8字节数据3333333333333333在加解密工具\PosDebug工具解密得出1B732966E2E8CC15
			memcpy(PreOut,"\x1B\x73\x29\x66\xE2\xE8\xCC\x15", 8);
#else	
			//id2的tdk自已与自己加密得:7C878556EE105C36 0E4784D9303A432E  
			//本预期结果使用8字节密钥7C878556EE105C36对8字节数据3333333333333333在加解密工具\PosDebug工具解密得出A26910A96DE82334
			memcpy(PreOut,"\xA2\x69\x10\xA9\x6D\xE8\x23\x34", 8);
#endif
		}
		else if(j==1 || j==2)
			//respTDK自己与自己加密后得:CE69D74207D3FCBC EE5A3F0E73FF9EC2
			//以SEC_DES_MODE_ECB/SEC_DES_MODE_CBC模式,使用8字节密钥CE69D74207D3FCBC(该密钥由16字节派生数据加解密KEY的前8字节密钥组成)对8字节数据3333333333333333进行解密后拼接得到结果48B84BE0F1D66133
			memcpy(PreOut,"\x48\xB8\x4B\xE0\xF1\xD6\x61\x33", 8);
		if(memcmp(DataOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(j=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, j, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7]);
			GOTOERR;
		}
	
		//case6.1:密钥类型为SEC_KEY_TYPE_TDK,以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_16|SEC_DES_ECBMODE模式, 16字节数据输入,DUKPT2,psIV=8个0x00
		//case6.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_16|SEC_DES_ECBMODE模式, 16字节数据输入,DUKPT2,psIV=8个0x00
		//case6.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_16|SEC_DES_MODE_CBC模式, 16字节数据输入,DUKPT2,psIV=8个0x00
		memset(sIV, 0x00, sizeof(sIV));
		memset(DataOut, 0, sizeof(DataOut));
		memset(PreOut, 0, sizeof(PreOut));		
		if((ret=NDK_SecCalcDesDukpt(2,sec_tdk_key_type[j], sIV, 16, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_16|sec_des_mode[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if(j==0)
		{
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM)
			//本预期结果使用16字节密钥bae698c215fe8dd5e3cf99433d2e560e对16字节数据分两个8字节(3333333333333333)进行解密计算后拼接成16字节结果5E4774EB6BA03BD35E4774EB6BA03BD3
			memcpy(PreOut,"\x5E\x47\x74\xEB\x6B\xA0\x3B\xD3\x5E\x47\x74\xEB\x6B\xA0\x3B\xD3", 16);
#else
			//本预期结果使用16字节密钥7C878556EE105C36 0E4784D9303A432E对16字节数据分两个8字节(3333333333333333)进行解密计算后拼接成16字节结果A35E54145BBD3063A35E54145BBD3063
			memcpy(PreOut,"\xA3\x5E\x54\x14\x5B\xBD\x30\x63\xA3\x5E\x54\x14\x5B\xBD\x30\x63", 16);
#endif
		}
		else if(j==1)
			//以SEC_DES_MODE_ECB模式,使用16字节派生数据加解密KEY:CE69D74207D3FCBC EE5A3F0E73FF9EC2对16字节数据分两个8字节(3333333333333333)进行3des解密计算后拼接成16字节结果
			memcpy(PreOut,"\x94\x4F\x5C\xD5\x22\xE5\xD1\x33\x94\x4F\x5C\xD5\x22\xE5\xD1\x33", 16);
		else if(j==2)
			//以SEC_DES_MODE_CBC模式,使用16字节派生数据加解密KEY:CE69D74207D3FCBC EE5A3F0E73FF9EC2对16字节数据(0x33)前八字节做3des解密得到944F5CD522E5D133，解密结果与后八字节数据做异或得到A77C6FE611D6E200
			memcpy(PreOut,"\x94\x4F\x5C\xD5\x22\xE5\xD1\x33\xA7\x7C\x6F\xE6\x11\xD6\xE2\x00", 16);
		if(memcmp(DataOut, PreOut, 16))
		{
			send_result("line %d:%s测试失败(j=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, j, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7], DataOut[8], DataOut[9], DataOut[10], DataOut[11], DataOut[12], DataOut[13], DataOut[14], DataOut[15]);
			GOTOERR;
		}
		
		//case7.1:密钥类型为SEC_KEY_TYPE_TDK,以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_ECBMODE模式, 16字节数据输入,DUKPT2,psIV=8个0x00	
		//case7.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_ECBMODE模式, 16字节数据输入,DUKPT2,psIV=8个0x00
		//case7.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_MODE_CBC模式, 16字节数据输入,DUKPT2,psIV=8个0x00
		memset(sIV, 0x00, sizeof(sIV));
		memset(DataOut, 0, sizeof(DataOut));
		memset(PreOut, 0, sizeof(PreOut));
		if((ret=NDK_SecCalcDesDukpt(2,sec_tdk_key_type[j], sIV, 16, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|sec_des_mode[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if(j==0)
		{
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM)
			//本预期结果使用16字节密钥bae698c215fe8dd5e3cf99433d2e560e对16字节数据分两个8字节(3333333333333333)进行解密计算后拼接成16字节结果5E4774EB6BA03BD35E4774EB6BA03BD3
			memcpy(PreOut,"\x5E\x47\x74\xEB\x6B\xA0\x3B\xD3\x5E\x47\x74\xEB\x6B\xA0\x3B\xD3", 16);
#else
			//本预期结果使用16字节密钥7C878556EE105C36 0E4784D9303A432E对16字节数据分两个8字节(3333333333333333)进行解密计算后拼接成16字节结果A35E54145BBD3063A35E54145BBD3063
			memcpy(PreOut,"\xA3\x5E\x54\x14\x5B\xBD\x30\x63\xA3\x5E\x54\x14\x5B\xBD\x30\x63", 16);
#endif
		}
		else if(j==1)
			//以SEC_DES_MODE_ECB模式,使用16字节派生数据加解密KEY:CE69D74207D3FCBC EE5A3F0E73FF9EC2对16字节数据分两个8字节(3333333333333333)进行3des解密计算后拼接成16字节结果
			memcpy(PreOut,"\x94\x4F\x5C\xD5\x22\xE5\xD1\x33\x94\x4F\x5C\xD5\x22\xE5\xD1\x33", 16);
		else if(j==2)
			//以SEC_DES_MODE_CBC模式,使用16字节派生数据加解密KEY:CE69D74207D3FCBC EE5A3F0E73FF9EC2对16字节数据(0x33)前八字节做3des解密得到944F5CD522E5D133，解密结果与后八字节数据做异或得到A77C6FE611D6E200
			memcpy(PreOut,"\x94\x4F\x5C\xD5\x22\xE5\xD1\x33\xA7\x7C\x6F\xE6\x11\xD6\xE2\x00", 16);
		if(memcmp(DataOut, PreOut, 16))
		{
			send_result("line %d:%s测试失败(j=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, j, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7], DataOut[8], DataOut[9], DataOut[10], DataOut[11], DataOut[12], DataOut[13], DataOut[14], DataOut[15]);
			GOTOERR;
		}
	
		//case8.1:密钥类型为SEC_KEY_TYPE_TDK,以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_24|SEC_DES_CBCMODE模式, 24字节数据输入,DUKPT2,psIV=非0值
		//case8.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_24|SEC_DES_ECBMODE模式, 24字节数据输入,DUKPT2,psIV=非0值
		//case8.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_24|SEC_DES_MODE_CBC模式, 24字节数据输入,DUKPT2,psIV=非0值
		memset(sIV, 0x12, sizeof(sIV));
		memset(DataOut, 0, sizeof(DataOut));
		memset(PreOut, 0, sizeof(PreOut));
		if((ret=NDK_SecCalcDesDukpt(2,sec_tdk_key_type[j], sIV, 24, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_24|sec_des_mode[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if(j==0)
		{
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM)
			//使用24字节密钥bae698c215fe8dd5e3cf99433d2e560ebae698c215fe8dd5(该密钥由16字节DUKPT密钥+前8字节密钥组成)对3组8字节数据3333333333333333进行解密后拼接得到结果
			memcpy(PreOut,"\x5E\x47\x74\xEB\x6B\xA0\x3B\xD3\x5E\x47\x74\xEB\x6B\xA0\x3B\xD3\x5E\x47\x74\xEB\x6B\xA0\x3B\xD3", 24);
#else
			//使用24字节密钥7C878556EE105C360E4784D9303A432E7C878556EE105C36(该密钥由16字节DUKPT密钥+前8字节密钥组成)对3组8字节数据3333333333333333进行解密后拼接得到结果
			memcpy(PreOut,"\xA3\x5E\x54\x14\x5B\xBD\x30\x63\xA3\x5E\x54\x14\x5B\xBD\x30\x63\xA3\x5E\x54\x14\x5B\xBD\x30\x63", 24);
#endif
		}
		else if(j==1)
			//以SEC_DES_MODE_ECB模式,使用24字派生数据加解密KEY:CE69D74207D3FCBC EE5A3F0E73FF9EC2(该密钥由16字节DUKPT密钥+前8字节密钥组成)对3组8字节数据3333333333333333进行3des解密计算后拼接成结果
			memcpy(PreOut,"\x94\x4F\x5C\xD5\x22\xE5\xD1\x33\x94\x4F\x5C\xD5\x22\xE5\xD1\x33\x94\x4F\x5C\xD5\x22\xE5\xD1\x33", 24);
		else if(j==2)
			//以SEC_DES_MODE_CBC模式,使用24字派生数据加解密KEY:CE69D74207D3FCBC EE5A3F0E73FF9EC2(该密钥由16字节DUKPT密钥+前8字节密钥组成)对第一个密文块(24字节数据(0x33)前八字节)做3des解密，解密结果与IV做异或后865D4EC730F7C321，密钥对第二块密文块(中间八个字节0x33)做解密与第一个密文块(前八个字节0x33)异或A77C6FE611D6E200，密钥对第三块密文块(后八个字节0x33)做解密与第二个密文块(中间八个字节0x33)异或A77C6FE611D6E200，
			memcpy(PreOut,"\x86\x5D\x4E\xC7\x30\xF7\xC3\x21\xA7\x7C\x6F\xE6\x11\xD6\xE2\x00\xA7\x7C\x6F\xE6\x11\xD6\xE2\x00", 24);
		if(memcmp(DataOut, PreOut, 24))
		{
			send_result("line %d:%s测试失败(j=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, j, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7], DataOut[8], DataOut[9], DataOut[10], DataOut[11], DataOut[12], DataOut[13], DataOut[14], DataOut[15], DataOut[16], DataOut[17], DataOut[18], DataOut[19], DataOut[20], DataOut[21], DataOut[22], DataOut[23]);
			GOTOERR;
		}
	
		//case19.1:密钥类型为SEC_KEY_TYPE_TDK,以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_16|SEC_DES_CBCMODE模式, 1024字节数据输入,DUKPT2,psIV=8个0x00,为测试高端AZ平台大数据解密密而增加的1024字节数据解密测试20170220
		//case19.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_16|SEC_DES_ECBMODE模式, 1024字节数据输入,DUKPT6,psIV=8个0x00
		//case19.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_16|SEC_DES_MODE_CBC模式, 1024字节数据输入,DUKPT6,psIV=8个0x00
		memset(sIV, 0x00, sizeof(sIV));
		memset(DataOut, 0, sizeof(DataOut));
		memset(En_DataOut, 0, sizeof(En_DataOut));
		if((ret=NDK_SecCalcDesDukpt(2,sec_tdk_key_type[j], sIV, 1024, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_16|sec_des_mode[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		for(i=0;i<128;i++)
			if(j==0)
			{
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM)
				//本预期结果使用16字节密钥bae698c215fe8dd5e3cf99433d2e560e对1024字节数据分128个8字节(3333333333333333)进行解密计算后拼接成1024字节结果5E4774EB6BA03BD3 128组
				memcpy(En_DataOut+i*8, "\x5E\x47\x74\xEB\x6B\xA0\x3B\xD3", 8);
#else
				//本预期结果使用16字节密钥7C878556EE105C36 0E4784D9303A432E对1024字节数据分128个8字节(3333333333333333)进行解密计算后拼接成1024字节结果
				memcpy(En_DataOut+i*8, "\xA3\x5E\x54\x14\x5B\xBD\x30\x63", 8);
#endif
			}				
			else if(j==1)
				//以SEC_DES_MODE_ECB模式,使用16字派生数据加解密KEY:CE69D74207D3FCBC EE5A3F0E73FF9EC2对1024字节数据(128组8字节数据3333333333333333)进行3des解密计算后拼接成结果
				memcpy(En_DataOut+i*8, "\x94\x4F\x5C\xD5\x22\xE5\xD1\x33", 8);
		if(j==2)
		{	
			//以SEC_DES_MODE_CBC模式,使用16字派生数据加解密KEY:CE69D74207D3FCBC EE5A3F0E73FF9EC2对1024字节数据分128个8字节(3333333333333333)进行解密，密钥与第一个密文块(前八字节)做3des解密为944F5CD522E5D133，密钥对第二块密文块(下一个八个字节0x33)做解密与第一个密文块(前八个字节0x33)异或A77C6FE611D6E200，密钥对第三块密文块(下一个八个字节0x33)做解密与第二个密文块(中间八个字节0x33)异或A77C6FE611D6E200,如此循环
			memcpy(En_DataOut, "\x94\x4F\x5C\xD5\x22\xE5\xD1\x33", 8);
			for(i=1;i<128;i++)
				memcpy(En_DataOut+i*8, "\xA7\x7C\x6F\xE6\x11\xD6\xE2\x00", 8);
		}
		if(memcmp(DataOut, En_DataOut, 1024))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
			
		//case9:密钥类型为SEC_KEY_TYPE_TDK,以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8|SEC_DES_ECBMODE模式, 8字节数据输入,DUKPT3,psIV=NULL
		//case9.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8|SEC_DES_ECBMODE模式, 8字节数据输入,DUKPT3,psIV=NULL
		//case9.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8|SEC_DES_MODE_CBC模式, 8字节数据输入,DUKPT3,psIV=NULL
		memset(DataOut, 0, sizeof(DataOut));
		memset(PreOut, 0, sizeof(PreOut));
		if((ret=NDK_SecCalcDesDukpt(3,sec_tdk_key_type[j], sIV, 8, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8|sec_des_mode[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if(j==0)
		{
		//id3的派生tdk:9b53578c81eff7c6f3bf8ca3bb006358  派生resp TDK:9b5357738110f7c6f3bf8c5cbbff6358 使用8字节密钥进行加密
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM)
			memcpy(PreOut,"\xC8\x90\x4F\xFC\x51\x31\xA7\x12", 8);
#else
		//tdk自己与自己加密后得:5554FBAAC1171CCBC4443DAED3F2AC2C
			memcpy(PreOut,"\x4B\xB0\x49\xD8\xF1\x13\x18\x26", 8);
#endif
		}
		//resp tdk自己与自己加密后得:33B498402A9C1DF5796200121F73183F
		else if(j==1||j==2)
			memcpy(PreOut,"\x28\x1E\x6F\x28\xE9\x27\xC7\xD7", 8);
		if(memcmp(DataOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(j=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, j, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7]);
			GOTOERR;
		}
	
		//case10.1:密钥类型为SEC_KEY_TYPE_TDK,以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|SEC_DES_ECBMODE模式,16字节数据输入,DUKPT3,psIV=8个0x00
		//case10.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|SEC_DES_ECBMODE模式, 16字节数据输入,DUKPT3,psIV=8个0x00
		//case10.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|SEC_DES_MODE_CBC模式, 16字节数据输入,DUKPT3,psIV=8个0x00
		memset(sIV, 0x00, sizeof(sIV));
		memset(DataOut, 0, sizeof(DataOut));
		memset(PreOut, 0, sizeof(PreOut));
		if((ret=NDK_SecCalcDesDukpt(3,sec_tdk_key_type[j], sIV, 16, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|sec_des_mode[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if(j==0)
		{
			//id3的派生tdk:9b53578c81eff7c6f3bf8ca3bb006358  派生resp TDK:9b5357738110f7c6f3bf8c5cbbff6358 
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM)
			memcpy(PreOut,"\xD7\xD8\x41\x7C\xCB\xD9\xB2\x9A\xD7\xD8\x41\x7C\xCB\xD9\xB2\x9A", 16);
#else
			//5554FBAAC1171CCBC4443DAED3F2AC2C
			memcpy(PreOut,"\x2C\x32\xD6\xE7\xB8\x98\x49\x64\x2C\x32\xD6\xE7\xB8\x98\x49\x64", 16);
#endif
		}
		//respTDK:33B498402A9C1DF5796200121F73183F 与3333333333333333加密得60792F7B9851A7C1 然后拼接
		else if(j==1)
			memcpy(PreOut,"\x60\x79\x2F\x7B\x98\x51\xA7\xC1\x60\x79\x2F\x7B\x98\x51\xA7\xC1", 16);
		else if(j==2)
		//33B498402A9C1DF5796200121F73183F 与3333333333333333加密得60792F7B9851A7C1 然后与后八字节异或的结果再与密钥加密得0C97B5747942DA68
			memcpy(PreOut,"\x60\x79\x2F\x7B\x98\x51\xA7\xC1\x0C\x97\xB5\x74\x79\x42\xDA\x68", 16);
		if(memcmp(DataOut, PreOut, 16))
		{
			send_result("line %d:%s测试失败(j=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, j, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7], DataOut[8], DataOut[9], DataOut[10], DataOut[11], DataOut[12], DataOut[13], DataOut[14], DataOut[15]);
			GOTOERR;
		}
		
		//case11.1:密钥类型为SEC_KEY_TYPE_TDK,以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24|SEC_DES_ECBMODE模式,24字节数据输入,DUKPT3,psIV=非0值
		//case11.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24|SEC_DES_ECBMODE模式, 24字节数据输入,DUKPT3,psIV=非0值
		//case11.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TDK, 以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24|SEC_DES_MODE_CBC模式, 24字节数据输入,DUKPT3,psIV=非0值
		memset(sIV, 0x12, sizeof(sIV));
		memset(DataOut, 0, sizeof(DataOut));
		memset(PreOut, 0, sizeof(PreOut));
		if((ret=NDK_SecCalcDesDukpt(3,sec_tdk_key_type[j], sIV, 24, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24|sec_des_mode[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if(j==0)
		{
			//id3的派生tdk:9b53578c81eff7c6f3bf8ca3bb0063589b53578c81eff7c6  与3333333333333333加密得D7D8417CCBD9B29A 然后拼接
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM)
			memcpy(PreOut,"\xD7\xD8\x41\x7C\xCB\xD9\xB2\x9A\xD7\xD8\x41\x7C\xCB\xD9\xB2\x9A\xD7\xD8\x41\x7C\xCB\xD9\xB2\x9A", 24);
#else
			//5554FBAAC1171CCBC4443DAED3F2AC2C5554FBAAC1171CCB 与3333333333333333加密得2C32D6E7B8984964 然后拼接
			memcpy(PreOut,"\x2C\x32\xD6\xE7\xB8\x98\x49\x64\x2C\x32\xD6\xE7\xB8\x98\x49\x64\x2C\x32\xD6\xE7\xB8\x98\x49\x64", 24);
#endif
		}
		else if(j==1)
			//33B498402A9C1DF5796200121F73183F33B498402A9C1DF5 与3333333333333333加密得60792F7B9851A7C1然后拼接
			memcpy(PreOut,"\x60\x79\x2F\x7B\x98\x51\xA7\xC1\x60\x79\x2F\x7B\x98\x51\xA7\xC1\x60\x79\x2F\x7B\x98\x51\xA7\xC1", 24);
		else if(j==2)
			//3333333333333333与IV进行异或后的结果与33B498402A9C1DF5796200121F73183F33B498402A9C1DF5 做3des加密得405CF4A0072A4F3A,加密结果与明文块2做异或后与密钥进行加密得42F9E19ABF90E842，密文与第三个明文异或后结果再与密钥加密得18B57A5D1E96514C 如此循环
			memcpy(PreOut,"\x40\x5C\xF4\xA0\x07\x2A\x4F\x3A\x42\xF9\xE1\x9A\xBF\x90\xE8\x42\x18\xB5\x7A\x5D\x1E\x96\x51\x4C ", 24);
		if(memcmp(DataOut, PreOut, 24))
		{
			send_result("line %d:%s测试失败(j=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, j, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7], DataOut[8], DataOut[9], DataOut[10], DataOut[11], DataOut[12], DataOut[13], DataOut[14], DataOut[15], DataOut[16], DataOut[17], DataOut[18], DataOut[19], DataOut[20], DataOut[21], DataOut[22], DataOut[23]);
			GOTOERR;
		}

		//case12.1:密钥类型为SEC_KEY_TYPE_TAK,以SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_ECBMODE模式,ucKeyIdx=DUKPT1,ucKeyType=TAK,应成功(后门功能),psIV=NULL
		//case12.2:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TAK, 以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_ECBMODE模式,ucKeyIdx=DUKPT1,ucKeyType=TAK,应成功(后门功能),psIV=NULL
		//case12.3:(海外产品)密钥类型为SEC_KEY_TYPE_RESP_TAK, 以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_MODE_CBC模式,ucKeyIdx=DUKPT1,ucKeyType=TAK,应成功(后门功能),psIV=NULL
		memset(DataOut, 0, sizeof(DataOut));
		memset(PreOut, 0, sizeof(PreOut));
		if((ret=NDK_SecCalcDesDukpt(6,sec_tak_key_type[j], NULL, 24, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|sec_des_mode[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if(j==0)
			//使用安装的16字节DUKPT密钥派生成MAC密钥6ef92795e5128e4dcdeb767b33f156bd密钥对3组8字节数据3333333333333333进行解密后拼接得到结果15066CFCCBAE36FF
			memcpy(PreOut,"\x15\x06\x6C\xFC\xCB\xAE\x36\xFF\x15\x06\x6C\xFC\xCB\xAE\x36\xFF\x15\x06\x6C\xFC\xCB\xAE\x36\xFF", 24);
		else if(j==1)
			//以SEC_DES_MODE_ECB模式,使用16字节DUKPT密钥派生成MAC RESPONSE密钥:6EF927951A12714DCDEB767BCCF1A9BD对24字节数据分三个8字节(3333333333333333)进行3des解密计算后拼接成24字节结果
			memcpy(PreOut,"\x70\xEA\x01\x51\x66\x2A\x82\x83\x70\xEA\x01\x51\x66\x2A\x82\x83\x70\xEA\x01\x51\x66\x2A\x82\x83", 24);
		else if(j==2)
			//以SEC_DES_MODE_CBC模式,使用16字节DUKPT密钥派生成MAC RESPONSE密钥:6EF927951A12714DCDEB767BCCF1A9BD对24字节数据(0x33)第一个密文块(前八字节)做3des解密为70EA0151662A8283，密钥对第二块密文块(下一个八个字节0x33)做解密与第一个密文块(前八个字节0x33)异或43D932625519B1B0，密钥对第三块密文块(下一个八个字节0x33)做解密与第二个密文块(中间八个字节0x33)异或43D932625519B1B0
			memcpy(PreOut,"\x70\xEA\x01\x51\x66\x2A\x82\x83\x43\xD9\x32\x62\x55\x19\xB1\xB0\x43\xD9\x32\x62\x55\x19\xB1\xB0", 24);			
		if(memcmp(DataOut, PreOut, 24))
		{
			send_result("line %d:%s测试失败(j=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, j, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7], DataOut[8], DataOut[9], DataOut[10], DataOut[11], DataOut[12], DataOut[13], DataOut[14], DataOut[15], DataOut[16], DataOut[17], DataOut[18], DataOut[19], DataOut[20], DataOut[21], DataOut[22], DataOut[23]);
			GOTOERR;
		}

		//case15:以SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_CBCMODE模式,ucKeyIdx=TDK's ID,ucKeyType=DUKPT的TDK,却进行DUKPT数据加解密，应失败,psIV=NULL
		if((ret=NDK_SecCalcDesDukpt(5,sec_tdk_key_type[j], NULL, 24, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|sec_des_mode[j]))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
	}	

#if !K21_ENABLE
	//case13:以SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_CBCMODE模式,ucKeyIdx=DUKPT1,ucKeyType=TPK,却进行数据加解密应失败,psIV=NULL
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TPK, NULL, 24, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else
	//case13:低端平台产品允许通过DES运算接口来进行PIN密钥扩展DES运算 ucKeyIdx=DUKPT1,ucKeyType=TPK进行DES运算，应该成功 
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TPK, NULL, 24, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//使用安装的16字节DUKPT密钥派生成PIN密钥6ef92795e51271b2cdeb767b33f1a942密钥对3组8字节数据3333333333333333进行解密后拼接得到结果\x5E\xEE\xE5\x7C\xE7\x26\x2C\x1B
	if(memcmp(DataOut,"\x5E\xEE\xE5\x7C\xE7\x26\x2C\x1B\x5E\xEE\xE5\x7C\xE7\x26\x2C\x1B\x5E\xEE\xE5\x7C\xE7\x26\x2C\x1B", 24))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif

#if SEC_RSA_ENABLE
	//case14:以SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_CBCMODE模式,ucKeyIdx=RSA's ID,ucKeyType=TDK,应失败,psIV=NULL
	if((ret=NDK_SecCalcDesDukpt(4,SEC_KEY_TYPE_TDK, NULL, 24, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECKM_READ_REC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case16:验证GETDES后,KSN不变
	//getksn
	if((ret=NDK_SecGetDukptKsn(2, KSN1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//getdes
	if((ret=NDK_SecCalcDesDukpt(3,SEC_KEY_TYPE_TDK, NULL, 16, DataIn, DataOut, KSN2, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//getksn	
	if((ret=NDK_SecGetDukptKsn(2, KSN3))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(KSN1, KSN2, 10)||memcmp(KSN1, KSN3, 10))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case17:KSN自加后，相应的派生前的密钥会自动更新，并且KSN要自动更新
	if ((ret=NDK_SecIncreaseDukptKsn(6))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for(j=0;j<num;j++)
	{
		//自加后派生前密钥变成:5635b184dd5d8f415617b6600c6e47cf，由此派生出DATA加解密密钥5635b184dda28f415617b6600c9147cf,安卓派生密钥为B7942062C24DAC1D857F90F85A2F1348,派生数据加解密KEY RESPONSE为9EAAAFD51C2D67AD 7AE163998A911BC9(未加密之前为5635B17BDD5D8F415617B69F0C6E47CF)
		memset(KSN1, 0x00, sizeof(KSN1));
		memset(DataOut, 0, sizeof(DataOut));
		memset(PreOut, 0, sizeof(PreOut));
		if((ret=NDK_SecCalcDesDukpt(6,sec_tdk_key_type[j], NULL, 16, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|sec_des_mode[j]))!=NDK_OK)  //linrq NDK_OK
		{
			send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, j);
			GOTOERR;
		}
		if(j==0)
		{
#if !(ANDIROD_ENABLE||OVERSEAS_ENABLE||CPU5810X_ENABLE||defined ME30GX||defined ME32GX||defined ME50NX||defined ME68||defined ME32||defined ME32THM||defined ME31S||defined ME30S||defined ME30THM)
			//本预期结果使用16字节密钥5635b184dda28f415617b6600c9147cf对16字节数据分两个8字节(3333333333333333)进行加密计算后拼接成16字节结果
			memcpy(PreOut,"\xFD\x16\x62\x03\x49\x2A\xD9\x08\xFD\x16\x62\x03\x49\x2A\xD9\x08", 16);
#else
			//本预期结果使用16字节密钥B7942062C24DAC1D857F90F85A2F1348对16字节数据分两个8字节(3333333333333333)进行加密计算后拼接成16字节结果
			memcpy(PreOut,"\x10\xBF\x5F\x57\xE9\xE8\x14\x5F\x10\xBF\x5F\x57\xE9\xE8\x14\x5F", 16);	
#endif
		}
		else if(j==1)
			//以SEC_DES_MODE_ECB模式,本预期结果使用16字节密钥9EAAAFD51C2D67AD 7AE163998A911BC9对16字节数据分两个8字节(3333333333333333)进行加密计算后拼接成16字节结果
			memcpy(PreOut,"\x36\x9B\x3E\x59\x81\x71\xC2\x63\x36\x9B\x3E\x59\x81\x71\xC2\x63", 16);
		else if(j==2)
			//以SEC_DES_MODE_CBC模式,使用16字节派生数据加解密KEY:9EAAAFD51C2D67AD 7AE163998A911BC9对16字节数据(0x33)前八字节做3des加密得到369B3E598171C263，加密结果与后八字节数据做异或得到05A80D6AB242F150，再与密钥做3des计算EC7EEFDB95B60CAB
			memcpy(PreOut,"\x36\x9B\x3E\x59\x81\x71\xC2\x63\x49\xC2\x5B\x7F\x2A\xEF\x6C\x82", 16);
		if(memcmp(DataOut, PreOut, 16))
		{
			send_result("line %d:%s测试失败(j=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, j, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7], DataOut[8], DataOut[9], DataOut[10], DataOut[11], DataOut[12], DataOut[13], DataOut[14], DataOut[15]);
			GOTOERR;
		}
		if( KSN1[7]!=0x00 || KSN1[8]!=0x00 || KSN1[9]!=0x02)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			goto ERR;
		}
	}
	send_result("%s测试通过", TESTAPI);
	
ERR:  //测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}

