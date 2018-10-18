/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name		: sec36.c
* Author 		:  	
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试NDK_SecCalcDes计算AES算法加解密接口功能
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
#define TESTAPI	"NDK_SecCalcDes"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef struct key_kcv{
	uchar *pkey;
	uchar *pkcv;
} KEY_KCV;
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author		date	remarks
*			  	  	      jiangym      20180321	created
*****************************************************************/
void sec36()
{
	ST_SEC_KEY_INFO stKeyInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_EXTEND_KEYBLOCK stExtendKey;
	int ret=0;
	uchar udesout[17]={0}, udesin[17]={0};
	char keyval[33] = {0};
#if	0//TR31_ENABLE    
	tr31_key_t tr31_st;
	unsigned int len = 0;
	ST_EXTEND_KEYBLOCK* p;
	uchar block[512]={0},mkey[17]={0},skey[17]={0};
#endif
#if DUKPT_ENABLE	//支持DUKPT密钥密钥体系的才进行DUKPT密钥安装
	uchar sKeyValueIn[17]={0},sKsnIn[11]={0};
#endif
#if SEC_RSA_ENABLE
	ST_SEC_RSA_KEY RsaKey;
	uchar *Pmod=(uchar *)"66C2BCD4AC436815C193A8D29A0755C80393D7AFC7D78B12FE371F91874D449271207DFA6A054D185B03E543137C59577769940830FEEA040053F72583B85951";
#endif	

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	if(!flag_sdk3)
	{
		send_result("line %d:sdk2.0版本不支持%s,无需测试", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI); 
	
	memset(udesin, 0x20, 16);

	//测试前置:擦除所有密钥
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//明文安装TMK,ID=2
	memset(&stKeyInfo, 0, sizeof(stKeyInfo));
	memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	memset(keyval, 0x11, stExtendKey.len);
	stExtendKey.pblock = keyval;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); 
		RETURN;
	}
	
	//明文安装TDK1,ID=3   密钥长度为24位
	memset(&stKeyInfo, 0, sizeof(stKeyInfo));
	memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK|SEC_KEY_AES; 
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 24;
	memset(keyval, 0x13, stExtendKey.len);
	stExtendKey.pblock = keyval;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); 
		RETURN;
	}
	
	//TMK安装TDK2,ID=4,密文安装
	memset(&stKeyInfo, 0, sizeof(stKeyInfo));
	memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	memcpy(keyval, "\x48\x5F\x0E\x65\x96\x7D\x5B\x95\xA8\x8D\x06\xA3\x7C\xD0\xAA\x14 ", stExtendKey.len);
	stExtendKey.pblock = keyval;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); 
		RETURN;
	}

	//AES 暂时不支持TR31格式安装
#if 0//TR31_ENABLE	//支持TR31格式密钥安装的设备才进行TR31测试
	//TMK以TR31格式打包安装TDK3,ID=5
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);	
	memset(mkey, 0x11, 16);/*由TMK发散*/
	memset(skey, 0x17, 8);
	memset(skey+8, 0x27, 8);

	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = skey;
	tr31_st.keylen = 16;
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	p=(ST_EXTEND_KEYBLOCK*)(stKeyInfo.sDstKeyValue);
	p->format=SEC_KEYBLOCK_FMT_TR31;
	p->len=len;
	p->pblock=(char *)block;
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)     
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else
	//不支持TR31格式的产品改用明文安装方式安装密钥
	memset(&stKeyInfo, 0, sizeof(stKeyInfo));
	memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	memset(keyval, 0x17, stExtendKey.len/2);
	memset(keyval+stExtendKey.len/2, 0x27, stExtendKey.len/2);
	stExtendKey.pblock = keyval;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//明文安装TAK,ID=6
	memset(&stKeyInfo, 0, sizeof(stKeyInfo));
	memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=6;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	memset(keyval, 0x19, stExtendKey.len/2);
	memset(keyval+8, 0x29, stExtendKey.len/2);
	stExtendKey.pblock = keyval;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//明文安装TPK,ID=7
	memset(&stKeyInfo, 0, sizeof(stKeyInfo));
	memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=7;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	memset(keyval, 0x1b, stExtendKey.len);
	stExtendKey.pblock =keyval;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if SEC_RSA_ENABLE
	//明文安装RSA, ID=8
	RsaKey.usBits=RSA_KEY_LEN_512;
	strcpy((char *)RsaKey.sExponent, "03");
	memcpy(RsaKey.sModulus, Pmod, RsaKey.usBits*2/8);
	if ((ret=NDK_SecLoadRsaKey(8, &RsaKey))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if DUKPT_ENABLE	//支持DUKPT密钥密钥体系的才进行DUKPT密钥安装
	//明文安装DUKPT,ID=9	
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(sKeyValueIn, 0x1f, 16);
	memset(sKsnIn, 0x12, 10);
	if ((ret=NDK_SecLoadTIK(9, 0, 16, sKeyValueIn, sKsnIn, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if defined N900AZ||defined N910AZ ||defined IM81AZ
	if(g_seckeyowner)
#endif
	{
		//新增32位测试,明文安装TAK,ID=10 20180417 sull add
		memset(&stKeyInfo, 0, sizeof(stKeyInfo));
		memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|SEC_KEY_AES;
		stKeyInfo.ucDstKeyIdx=10;
		//stKeyInfo.nDstKeyLen=16;
		//memset(stKeyInfo.sDstKeyValue, 0x19, stKeyInfo.nDstKeyLen/2);//修改密钥前8字节和后8字节不一致 
		//memset(stKeyInfo.sDstKeyValue+stKeyInfo.nDstKeyLen/2, 0x29, stKeyInfo.nDstKeyLen/2);

		//stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 32;
		memset(keyval, 0x31, stExtendKey.len);
		stExtendKey.pblock = keyval;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		stKcvInfoIn.nLen=0;
		if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}

	//case1.1:异常测试:以SEC_AES_ENCRYPT加密/SEC_AES_DECRYPT解密,数据长度不是16整数倍 应该失败
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 8, udesout, SEC_AES_ENCRYPT))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 15, udesout, SEC_AES_DECRYPT))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.3:异常测试:uckeyIdx不存在(id=10)
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 10, udesin, 16, udesout, SEC_AES_ENCRYPT))!=NDK_ERR_SECCR_GET_KEY)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:正常测试:ucKeyType或上算法类型，应该也可以成功
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK|SEC_KEY_AES, 3, udesin, 16, udesout, SEC_AES_ENCRYPT))!=NDK_OK)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期结果可以由16字节的0x20经24字节的0x13密钥在AES算法下加密得到(工具为AES 加解密软件)
	if(memcmp(udesout, "\x9E\x3A\xA3\xB9\x8B\xE0\xEB\x37\x06\xE3\x26\x7A\xD1\xA6\x01\xD4", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case3:正常测试:以SEC_AES_ENCRYPT 加密 密钥ID=3(密钥为24位)
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 16, udesout, SEC_AES_ENCRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(udesout, "\x9E\x3A\xA3\xB9\x8B\xE0\xEB\x37\x06\xE3\x26\x7A\xD1\xA6\x01\xD4", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case4:正常测试:以SEC_AES_DECRYPT 解密 密钥ID=4
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 4, udesin, 16, udesout, SEC_AES_DECRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期结果是由16字节的密文0X20经ID=4的密钥值(16个0X15)在AES算法下解密得到
	if(memcmp(udesout, "\x80\xFD\x39\x1A\xD9\x34\x9D\x37\xEE\xBD\x0F\x67\xBB\xC3\x40\x26 ", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case5:正常测试:以SEC_AES_ENCRYPT 加密 密钥ID=5
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 5, udesin, 16, udesout, SEC_AES_ENCRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期结果是由16字节的0X20经8个字节的0x17+8个字节的0x27的密钥(由TR31格式打包)在AES算法下加密得到
	if(memcmp(udesout, "\xF3\x56\x83\x05\x43\x4F\x20\x6A\x22\xB7\xA9\x55\x5B\xE5\x69\x31 ", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case6:以SEC_AES_DECRYPT模式,ucKeyIdx=TAK's ID,ucKeyType=TAK,应成功(后门功能)
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TAK, 6, udesin, 16, udesout, SEC_AES_DECRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期结果可以由16字节的0x20经8个字节的0x19+8个字节的0x29密钥在AES算法下解密得到
	if(memcmp(udesout, "\x64\x83\xC3\x22\x53\x13\x5A\x5B\x88\x82\xBC\x08\xF9\x55\x59\x89 ", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
#if !K21_ENABLE
	//case7:以SEC_AES_DECRYPT模式,ucKeyIdx=TPK's ID,ucKeyType=TPK,应失败(适合中端)
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 7, udesin, 16, udesout, SEC_AES_DECRYPT))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else
	//case8:低端平台产品允许通过运算接口来进行PIN密钥扩展应该成功  
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 7, udesin, 16, udesout, SEC_AES_DECRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期结果可以由16字节的0x20经16字节的0x1b密钥在AES算法下解密得到
	if(memcmp(udesout, "\x0D\xCB\x1C\xA7\xCB\x91\xF1\x22\xD4\x2B\xE4\x80\x03\x13\x2B\x42 ", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif

#if SEC_RSA_ENABLE
	//case9:以SEC_AES_DECRYPT模式,ucKeyIdx=RSA's ID,ucKeyType=TDK,应失败
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 8, udesin, 16, udesout, SEC_AES_DECRYPT))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if DUKPT_ENABLE	
	//case10:以SEC_AES_DECRYPT模式,ucKeyIdx=DUKPT's ID,ucKeyType=TDK,应失败
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 9, udesin, 16, udesout, SEC_AES_DECRYPT))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
#endif

#if defined N900AZ||defined N910AZ ||defined IM81AZ
	if(g_seckeyowner)
#endif
	{
		//case11:正常测试:SEC_AES_ENCRYPT 加密 密钥ID=10
		if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TAK, 10, udesin, 16, udesout, SEC_AES_ENCRYPT))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		//预期结果是由16字节的明文0X20经ID=10的密钥值(32个0X31)在AES算法下加密得到
		if(memcmp(udesout, "\x71\xBA\xF2\x19\xAA\x78\x6E\x6B\x6E\x09\xE1\x6C\xD9\x00\x24\xC2", 16))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}
	send_result("%s测试通过", TESTAPI);
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}

