/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec5.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecGetMacDUKPT接口功能
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
#define TESTAPI	"NDK_SecGetMacDUKPT"	

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
*						jiangym			20150125	modified
*						zhengry			20170425	modified						
*****************************************************************/
void sec20(void)
{
	int ret = 0, i=0;
	ST_SEC_KCV_INFO KcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar DataIn[33]={0},MacOut[9]={0},KSN1[11]={0},KSN2[11]={0},KSN3[11]={0},KeyValue[17]={0},PreOut[9]={0};
#if TR31_ENABLE
	tr31_key_t tr31_st;
	unsigned int len = 0;
	uchar mkey[17]={0},block[512]={0};
#endif
#if SEC_RSA_ENABLE
	ST_SEC_RSA_KEY RsaKeyIn;
	uchar *uModeulus1=(uchar *)"4FB2D08918F766B30B2144FE25D09A160FFCF256086D82773FB04D4BD7E4772F2557FC25EE130A0B984467995BE80EE1751F577BD3E5CAFF60712722F33D0479";
#endif
	uchar DukptKeyValue[16]={0xD5,0xD4,0x4F,0xF7,0x20,0x68,0x3D,0x0D,0x70,0xBE,0x25,0x28,0x18,0xE2,0xA1,0x8A};//DUKPY初始明文密钥，密钥派生结果情况参考sec21的用例注释
	uchar KSN[11]={0};//初始KSN
#if OVERSEAS_ENABLE
	int num=2;
	int sec_mac_x99[2]={SEC_MAC_X99, SEC_MAC_RESP_X99};
	int sec_mac_x9191[2]={SEC_MAC_X919, SEC_MAC_RESP_X919};
	int sec_mac_ecb[2]={SEC_MAC_ECB, SEC_MAC_RESP_ECB};
	int sec_mac_9606[2]={SEC_MAC_9606, SEC_MAC_RESP_9606};
#else
	int num=1;
	int sec_mac_x99[1]={SEC_MAC_X99};
	int sec_mac_x9191[1]={SEC_MAC_X919};
	int sec_mac_ecb[1]={SEC_MAC_ECB};
	int sec_mac_9606[1]={SEC_MAC_9606};
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
	case SEC_KEY_TYPE_TDK:		//派生数据加解密KEY 6ef92795e5ed714dcdeb767b330ea9bd    安卓平台产品需要把自己当数据和密钥进行3DES运算  BFA9CA10BF791459 482339A090BF7CC9 为实际密钥
		key[5] ^= 0xff;
		key[13] ^= 0xff;
	case SEC_KEY_TYPE_RESP_TAK: //派生MAC RESPONSE  6EF927951A12714DCDEB767BCCF1A9BD  
		key[4] ^= 0xff;
		key[12] ^= 0xff;
	case SEC_KEY_TYPE_RESP_TDK: //派生数据加解密KEY RESPONSE    6EF9276AE512714DCDEB768433F1A9BD    
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

	//明文安装一组DUKPT, ID=6, psKsnIn为10个字节0x00  /*约定:以下KCV均以不校验方式安装*/
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(KSN, 0x00, 10);//初始KSN
	if ((ret=NDK_SecLoadTIK(6, 0, 16, DukptKeyValue, KSN, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//安装TLK, ID=1  用于密文安装
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
	memcpy(KeyValueIn, "\xDA\x36\xEC\x83\x4A\xDE\x38\xD9\x5A\x51\x68\xF9\x5E\x9A\x5E\x23", 16);
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

	//明文安装TPK, ID=5
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, "\x21\x21\x21\x21\x21\x21\x21\x21\x23\x23\x23\x23\x23\x23\x23\x23", stKeyInfo.nDstKeyLen);
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &KcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1:非法测试
	//case1.1:psDataIn==NULL
	if ((ret=NDK_SecGetMacDukpt(6, NULL, 8, MacOut, KSN1, SEC_MAC_X99))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.2:psMacOut==NULL
	memset(DataIn, 0x12, 32);
	if ((ret=NDK_SecGetMacDukpt(6, DataIn, 8, NULL, KSN1, SEC_MAC_X99))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.3:ucMod 非法
	if ((ret=NDK_SecGetMacDukpt(6, DataIn, 8, MacOut, KSN1, 100))!=NDK_ERR_SECCR_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.4:Datalen<0
	if ((ret=NDK_SecGetMacDukpt(6, DataIn, -1, MacOut, KSN1, SEC_MAC_X99))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.5:ucGroupIdx不存在(10, 101)
	if ((ret=NDK_SecGetMacDukpt(10, DataIn, 8, MacOut, KSN1, SEC_MAC_X99))!=NDK_ERR_SECKM_READ_REC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.6:psKsnOut=NULL
	if ((ret=NDK_SecGetMacDukpt(6, DataIn, 8, MacOut, NULL, SEC_MAC_X99))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	for(i=0;i<num;i++)
	{
		//case2:以SEC_MAC_X99模式,7字节数据输入,DUKPT6
		memset(DataIn, 0x22, sizeof(DataIn));
		memset(MacOut, 0, sizeof(MacOut));
		memset(PreOut, 0, sizeof(PreOut));
		if ((ret=NDK_SecGetMacDukpt(6, DataIn, 7, MacOut, KSN1, sec_mac_x99[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		if(i==0)
			//使用工具计算得:319AD5A162A11685  (派生Mac密钥与22222222222222  3DES生成)
			memcpy(PreOut, "\x31\x9A\xD5\xA1\x62\xA1\x16\x85", 8);
		else
			//海外版本新增SEC_MAC_RESP_X99,以SEC_MAC_RESP_X99模式,派生密钥为6EF927951A12714DCDEB767BCCF1A9BD与22222222 22222200做3DES运算
			memcpy(PreOut, "\x46\x17\x7F\x79\x78\x43\x16\xF9", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR; 
		}

		//case3:以SEC_MAC_X99模式,8字节数据输入,DUKPT2
		memset(MacOut, 0, sizeof(MacOut));
		memset(PreOut, 0, sizeof(PreOut));
		if ((ret=NDK_SecGetMacDukpt(2, DataIn, 8, MacOut, KSN, sec_mac_x99[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		} 
		if(i==0)
			//(ID2派生Mac密钥bae698c2150172d5e3cf99433dd1a90e与2222222222222222  3DES)使用工具计算得:6D8D05AB755327F0   
			memcpy(PreOut, "\x6D\x8D\x05\xAB\x75\x53\x27\xF0", 8);
		else
			//ID2派生MAC RESP密钥bae698c2ea018dd5e3cf9943c2d1560e与22222222 22222222做3DES运算得FA3F038599099ACA
			memcpy(PreOut, "\xFA\x3F\x03\x85\x99\x09\x9A\xCA", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR;
		}

		//case4:以SEC_MAC_X99模式,10字节数据输入,DUKPT3
		memset(MacOut, 0, sizeof(MacOut));
		memset(PreOut, 0, sizeof(PreOut));
		if ((ret=NDK_SecGetMacDukpt(3, DataIn, 10, MacOut, KSN1, sec_mac_x99[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		if(i==0)
			//输入的数据为10字节，分为2个block，每个block为8字节，不足时后面补0，派生密钥与第一个数据block加密，得到的结果与第二个数据block做异或，得到的结果与派生秘钥做加密，得到最后的加密值。(实际用计算工具就可得到结果)
			//(ID3派生Mac密钥:9b53578c811008c6f3bf8ca3bbff9c58)使用工具计算得:3FE28BE4A5E6EA0F
			memcpy(PreOut, "\x3F\xE2\x8B\xE4\xA5\xE6\xEA\x0F", 8);
		else
			//(ID3派生Mac RESP密钥:9b53578c7e10f7c6f3bf8ca344ff6358)使用工具计算得:67ECF89CE8067F2D
			memcpy(PreOut, "\x67\xEC\xF8\x9C\xE8\x06\x7F\x2D", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR;
		}

		/**************************
		SEC_MAC_X919模式:例如输入的数据为18字节,则分为3个block,每个block为8字节,不足时后面补0,18字节的话分为8字节8字节与2字节
		2字节后面补0成8字节,同时密钥也分为8字节的block,这里的密钥可分为2个block,然后第一个数据block与第一个密钥block进行
		des加密,加密后的结果与第二个数据block进行异或,异或后的结果再与第一个密钥block进行des加密,加密后的结果与最后一个
		block进行异或,异或后的结果与整个密钥进行3des加密,加密结果即是macout的输出结果(如果只有两个数据block,那么第一个数据
		block加密后的结果与第二个数据block异或后的结果与整个密钥进行3des加密)以此类推.......
		以上操作都是通过工具中的des、 xor 计算
		***************************/
		//case5:以SEC_MAC_X919/SEC_MAC_RESP_X919模式,13字节数据输入,DUKPT6
		memset(MacOut, 0, sizeof(MacOut));
		memset(PreOut, 0, sizeof(PreOut));
		if ((ret=NDK_SecGetMacDukpt(6, DataIn, 13, MacOut, KSN1, sec_mac_x9191[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		if(i==0)
			//4617F3B494C22554 
			memcpy(PreOut, "\x46\x17\xF3\xB4\x94\xC2\x25\x54", 8);
		else
			//以SEC_MAC_RESP_X919模式,第一个数据block为8个0x22,第二个数据block为5个0x22+3个0x00,前八个字节派生密钥先与block1加密,加密结果与block2做异或,异或的结果与16个字节的派生密钥做加密,得到加密结果
			memcpy(PreOut, "\xC1\x6C\x45\xC4\x4E\xA1\x70\x5E", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR;
		}

		//case6:以SEC_MAC_X919/SEC_MAC_RESP_X919模式,16字节数据输入,DUKPT2
		memset(MacOut, 0, sizeof(MacOut));
		memset(PreOut, 0, sizeof(PreOut));
		if ((ret=NDK_SecGetMacDukpt(2, DataIn, 16, MacOut, KSN1, sec_mac_x9191[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		if(i==0)
			//ID2派生Mac密钥bae698c2150172d5e3cf99433dd1a90e  得F43027C8D1725AE9
			memcpy(PreOut, "\xF4\x30\x27\xC8\xD1\x72\x5A\xE9", 8);
		else
			//ID2派生MAC RESP密钥bae698c2ea018dd5e3cf9943c2d1560e 得84EB6857EEDFB7B4
			//以SEC_MAC_RESP_X919模式,第一个数据block为8个0x22,第二个数据block为8个0x22,前八个字节派生密钥先与block1加密,加密结果与block2做异或,异或的结果与16个字节的派生密钥做加密,得到加密结果
			memcpy(PreOut, "\x84\xEB\x68\x57\xEE\xDF\xB7\xB4", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR;
		}

		//case7:以SEC_MAC_X919/SEC_MAC_RESP_X919模式,18字节数据输入,DUKPT3
		memset(MacOut, 0, sizeof(MacOut));
		memset(PreOut, 0, sizeof(PreOut));
		if ((ret=NDK_SecGetMacDukpt(3, DataIn, 18, MacOut, KSN1, sec_mac_x9191[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		if(i==0)
			//ID3派生Mac密钥:9b53578c811008c6f3bf8ca3bbff9c58  得1AF1B490219AD23E
			memcpy(PreOut, "\x1A\xF1\xB4\x90\x21\x9A\xD2\x3E", 8);
		else
			//ID3派生Mac RESP密钥:9b53578c7e10f7c6f3bf8ca344ff6358 得63DFC061C3169F47
			//以SEC_MAC_RESP_X919模式,第一个数据block为8个0x22,第二个数据block为8个0x22,第三个数据block为2个0x22+6个0x00,前八个字节派生密钥先与block1加密,加密结果与block2做异或,
			//异或的结果与前八个字节派生密钥做加密,得到结果与block3数据做异或,异或结果再与整个16字节派生秘钥做加密,得到加密结果
			memcpy(PreOut, "\x63\xDF\xC0\x61\xC3\x16\x9F\x47", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR;
		}
			
		/**************************
		若没工具，在SEC_MAC_ECB/SEC_MAC_RESP_ECB模式下原理如下:
		step1:将数据分为8个字节一块,相互异或，得到8个字节的异或结果
		step2:将8个字节的异或结果转换为ASCII码得到16字节的异或结果
		step3:将派生密钥与step2得到的前八个字节做3des加密
		step4:将step3得到的加密结果与step2后八个字节做异或,得到8个字节的异或结果
		step5:将step4得到的异或结果与派生密钥做3des加密,得到8个字节的密钥结果
		step6:将step5得到的8个字节密钥结果转换为16字节的ASCII码，取前8个字节为计算结果
		***************************/
		//case8:以SEC_MAC_ECB/SEC_MAC_RESP_ECB模式,19字节数据输入,DUKPT6
		memset(MacOut, 0, sizeof(MacOut));
		memset(PreOut, 0, sizeof(PreOut));
		if ((ret=NDK_SecGetMacDukpt(6, DataIn, 19, MacOut, KSN1, sec_mac_ecb[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		if(i==0)
			//使用工具计算得:3333384146333538
			memcpy(PreOut, "\x33\x33\x38\x41\x46\x33\x35\x38", 8);
		else
			//使用工具计算得:3745434637393945
			memcpy(PreOut, "\x37\x45\x43\x46\x37\x39\x39\x45", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR;
		}

		//case9:以SEC_MAC_ECB/SEC_MAC_RESP_ECB模式,24字节数据输入,DUKPT2
		memset(MacOut, 0, sizeof(MacOut));
		memset(PreOut, 0, sizeof(PreOut));
		if ((ret=NDK_SecGetMacDukpt(2, DataIn, 24, MacOut, KSN1, sec_mac_ecb[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		if(i==0)
			//ID2派生Mac密钥bae698c2150172d5e3cf99433dd1a90e使用工具计算得:3534304237453033
			memcpy(PreOut, "\x35\x34\x30\x42\x37\x45\x30\x33", 8);
		else
			//ID2派生MAC RESP密钥bae698c2ea018dd5e3cf9943c2d1560e使用工具计算得:3239433134303532
			memcpy(PreOut, "\x32\x39\x43\x31\x34\x30\x35\x32", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR;
		}
			
		//case10:以SEC_MAC_ECB/SEC_MAC_RESP_ECB模式,26字节数据输入,DUKPT3
		memset(MacOut, 0, sizeof(MacOut));
		memset(PreOut, 0, sizeof(PreOut));
		if ((ret=NDK_SecGetMacDukpt(3, DataIn, 26, MacOut, KSN1, sec_mac_ecb[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		if(i==0)
			//ID3派生Mac密钥:9b53578c811008c6f3bf8ca3bbff9c58使用工具计算得:3246353833313337
			memcpy(PreOut, "\x32\x46\x35\x38\x33\x31\x33\x37", 8);
		else
			//ID3派生Mac RESP密钥:9b53578c7e10f7c6f3bf8ca344ff6358 使用工具计算得:3546423335463032
			memcpy(PreOut, "\x35\x46\x42\x33\x35\x46\x30\x32", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR;
		}
			
		/**************************		
		若没工具，在SEC_MAC_9606/SEC_MAC_RESP_9606模式下原理如下:
		将数据分为8个字节一块,相互异或，得到8个字节的异或结果,异或结果再与派生密钥做加密
		***************************/
		//case11:以SEC_MAC_9606/SEC_MAC_RESP_9606模式,30字节数据输入,DUKPT6
		memset(MacOut, 0, sizeof(MacOut));
		memset(PreOut, 0, sizeof(PreOut));
		if ((ret=NDK_SecGetMacDukpt(6, DataIn, 30, MacOut, KSN1, sec_mac_9606[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		if(i==0)
			//使用工具计算得:41CC9D655690CA9E
			memcpy(PreOut, "\x41\xCC\x9D\x65\x56\x90\xCA\x9E", 8);
		else
			//使用工具计算得:6C903CA8FE843E82
			memcpy(PreOut, "\x6C\x90\x3C\xA8\xFE\x84\x3E\x82", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR;
		}

		//case12:以SEC_MAC_9606/SEC_MAC_RESP_9606模式,31字节数据输入,DUKPT2
		memset(MacOut, 0, sizeof(MacOut));
		memset(PreOut, 0, sizeof(PreOut));
		if ((ret=NDK_SecGetMacDukpt(2, DataIn, 31, MacOut, KSN1, sec_mac_9606[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		if(i==0)
			//ID2派生Mac密钥bae698c2150172d5e3cf99433dd1a90e使用工具计算得:36F2DE096FD18506
			memcpy(PreOut, "\x36\xF2\xDE\x09\x6F\xD1\x85\x06", 8);
		else
			//ID2派生MAC RESP密钥bae698c2ea018dd5e3cf9943c2d1560e使用工具计算得:6E69B5D79979BB92
			memcpy(PreOut, "\x6E\x69\xB5\xD7\x99\x79\xBB\x92", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR;
		}

		//case13:以SEC_MAC_9606/SEC_MAC_RESP_9606模式,32字节数据输入,DUKPT3
		memset(MacOut, 0, sizeof(MacOut));
		memset(PreOut, 0, sizeof(PreOut));
		if ((ret=NDK_SecGetMacDukpt(3, DataIn, 32, MacOut, KSN1, sec_mac_9606[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
		if(i==0)
			//ID3派生Mac密钥:9b53578c811008c6f3bf8ca3bbff9c58使用工具计算得:A15F00C8AF304190
			memcpy(PreOut, "\xA1\x5F\x00\xC8\xAF\x30\x41\x90", 8);
		else
			//ID3派生Mac RESP密钥:9b53578c7e10f7c6f3bf8ca344ff6358使用工具计算得:8DC1B7534F1AC8BF
			memcpy(PreOut, "\x8D\xC1\xB7\x53\x4F\x1A\xC8\xBF", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR;
		}
		
		//case16:以SEC_MAC_X919/SEC_MAC_RESP_X919模式,13字节数据输入,MAC需使用TAK实际上ID=5上装载的是TPK,违反密钥专用性,应失败
		if ((ret=NDK_SecGetMacDukpt(5, DataIn, 13, MacOut, KSN1, sec_mac_x9191[i]))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		}
	}

	//case14:验证GETMAC后,KSN不变
	//getksn
	if((ret=NDK_SecGetDukptKsn(2, KSN1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//getmac
	if ((ret=NDK_SecGetMacDukpt(3, DataIn, 13, MacOut, KSN2, SEC_MAC_9606))!=NDK_OK)
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
	
#if SEC_RSA_ENABLE
	//case15:以SEC_MAC_X919模式,13字节数据输入,MAC需使用TAK实际上ID=4上装载的是RSA,违反密钥专用性,应失败
	if ((ret=NDK_SecGetMacDukpt(4, DataIn, 13, MacOut, KSN1, SEC_MAC_X919))!=NDK_ERR_SECKM_READ_REC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case17:KSN自加后，相应的派生前的密钥会自动更新，并且KSN要自动更新
	if ((ret=NDK_SecIncreaseDukptKsn(6))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for(i=0;i<num;i++)
	{
		//自加后派生前密钥变成:5635b184dd5d8f415617b6600c6e47cf，由此派生出MAC密钥5635b184dd5d70415617b6600c6eb8cf，由此派生出MAC RESPONSE密钥5635B184225D8F415617B660F36E47CF
		memset(KSN1, 0x00, sizeof(KSN1));
		memset(MacOut, 0, sizeof(MacOut));
		if ((ret=NDK_SecGetMacDukpt(6, DataIn, 8, MacOut, KSN1, sec_mac_x99[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
			GOTOERR;
		} 
		if(i==0)
			//本预期结果使用16字节密钥5635b184dd5d70415617b6600c6eb8cf对8字节数据(2222222222222222)进行加密计算后结果3FCBC4E7C2FE492C
			memcpy(PreOut, "\x3F\xCB\xC4\xE7\xC2\xFE\x49\x2C", 8);
		else
			//本预期结果使用16字节密钥5635B184225D8F415617B660F36E47CF对8字节数据(2222222222222222)进行加密计算后结果D711C99288DA4B5C
			memcpy(PreOut, "\xD7\x11\xC9\x92\x88\xDA\x4B\x5C", 8);
		if(memcmp(MacOut, PreOut, 8))
		{
			send_result("line %d:%s测试失败(i=%d, 0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, i, MacOut[0], MacOut[1], MacOut[2], MacOut[3], MacOut[4], MacOut[5], MacOut[6], MacOut[7]);
			GOTOERR;
		}
		if( KSN1[7]!=0x00 || KSN1[8]!=0x00 || KSN1[9]!=0x02)
		{
			send_result("line %d:%s测试失败(KSN1[7]=0x%2x,KSN1[8]=0x%2x,KSN1[9]=0x%2x)", __LINE__, TESTAPI, KSN1[7],KSN1[8],KSN1[9]);
			goto ERR;
		}
	}
	send_result("%s测试通过", TESTAPI);
	
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;

}

