/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name		: sec37.c
* Author 		:  
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试NDK_SecGetMac AES MAC计算接口功能
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
#define TESTAPI	"NDK_SecGetMac"	

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
* history 		 		:  author		date	   remarks
*			  	  	      jiangym		20180321 created
*****************************************************************/
void sec37(void)
{
	int ret=0;
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	ST_EXTEND_KEYBLOCK stExtendKey;
	uchar szDataIn[257]={0},szMac[17]={0};
	char keyval[33] = {0};
#if DUKPT_ENABLE	//支持DUKPT密钥密钥体系的才进行DUKPT密钥安装
	uchar sKeyValueIn[17]={0},sKsnIn[11]={0};
#endif
#if	0//TR31_ENABLE    
	tr31_key_t tr31_st;
	unsigned int len = 0;
	ST_EXTEND_KEYBLOCK* p;
	uchar block[512]={0},mkey[17]={0}, skey[17]={0};
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

	//测试前置:擦除所有密钥 
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	memset(szDataIn, 0x20, sizeof(szDataIn));

	//明文安装TMK,id=2,明文为16个0x13
	memset(&stKeyInfo, 0, sizeof(stKeyInfo));
	memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
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
		
	//以SEC_KCV_NONE明文方式安装TAK1,id=3，明文为16个0x15
	memset(&stKeyInfo, 0, sizeof(stKeyInfo));
	memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	memset(keyval, 0x15, stExtendKey.len);
	stExtendKey.pblock = keyval;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//以SEC_KCV_ZERO方式(TMK发散)安装TAK2,id=4
	memset(&stKeyInfo, 0, sizeof(stKeyInfo));
	memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	memcpy(keyval, "\x43\xAD\x57\xDC\xAF\xF9\x8A\xC3\xC5\x49\x62\xF4\x30\xAD\xBB\xC1 ", stExtendKey.len);
	stExtendKey.pblock = keyval;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, "\x8E\x68\x02\x75", stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//AES 暂时不支持 TR31格式安装
#if  0//TR31_ENABLE	//支持TR31格式密钥安装的设备才进行TR31测试
	//以SEC_KCV_ZERO方式(TMK以TR31方式打包发散)安装TAK3,id=5
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, 0x13, 16);//对应于TMK 
	memset(skey, 0x1b, 8);	//TAK2密钥明文:8个字节的0x1b+8个字节0x1d,修改密钥前8字节和后8字节不一致 
	memset(skey+8, 0x1d, 8);	
	
	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAC_TYPE, 4);	/*TAK*/
	tr31_st.key = skey;
	tr31_st.keylen = 16;
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	p=(ST_EXTEND_KEYBLOCK *)(stKeyInfo.sDstKeyValue);
	p->format=SEC_KEYBLOCK_FMT_TR31;
	p->len=len;
	p->pblock=(char *)block;
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);//ret=-1312
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
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	memset(keyval, 0x1b, stExtendKey.len/2);
	memset(keyval+stExtendKey.len/2, 0x1d, stExtendKey.len/2);
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

	//以SEC_KCV_NONE明文方式安装TPK,id=6,明文为16个0x1f
	memset(&stKeyInfo, 0, sizeof(stKeyInfo));
	memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=6;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK|SEC_KEY_AES; 
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	memset(keyval, 0x1f, stExtendKey.len);
	stExtendKey.pblock = keyval;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
#if SEC_RSA_ENABLE
	//明文安装RSA, ID=7
	RsaKey.usBits=RSA_KEY_LEN_512;
	strcpy((char *)RsaKey.sExponent, "03");
	memcpy(RsaKey.sModulus, Pmod, RsaKey.usBits*2/8);
	if ((ret=NDK_SecLoadRsaKey(7, &RsaKey))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if DUKPT_ENABLE	//支持DUKPT密钥密钥体系的才进行DUKPT密钥安装
	//明文安装DUKPT,ID=8
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(sKeyValueIn, 0x1d, 16);
	memset(sKsnIn, 0x12, 10);
	if ((ret=NDK_SecLoadTIK(8, 0, 16, sKeyValueIn, sKsnIn, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if defined N900AZ||defined N910AZ ||defined IM81AZ
	if(g_seckeyowner)
#endif
	{
		//新增用例:以SEC_KCV_NONE明文方式安装TAK4,id=9,明文为32个0x31
		memset(&stKeyInfo, 0, sizeof(stKeyInfo));
		memset(&stKcvInfoIn, 0, sizeof(stKcvInfoIn));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=9;
		stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 32;
		memset(keyval, 0x31, stExtendKey.len);
		stExtendKey.pblock = keyval;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		stKcvInfoIn.nLen=0;
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
	}

	//case1:正常测试:以SEC_MAC_AES模式,7字节数据输入,TAK1
	if ((ret=NDK_SecGetMac(3, szDataIn, 7, szMac, SEC_MAC_AES))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szMac, "\x08\x8A\x23\x17\x93\x51\x2F\xB1\xD1\x6D\xE3\x90\x87\x03\x8D\xCC ", 16))
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szMac[0],szMac[1],szMac[2],szMac[3],szMac[4],szMac[5],szMac[6],szMac[7],szMac[8],szMac[9],szMac[10],szMac[11],szMac[12],szMac[13],szMac[14],szMac[15]);
		GOTOERR;
	}
	
	//case2:正常测试:以SEC_MAC_AES模式,16字节数据输入,TAK2
	if ((ret=NDK_SecGetMac(4, szDataIn, 16, szMac, SEC_MAC_AES))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szMac, "\xBC\x75\xD3\xA8\x0D\x71\xE3\x72\x20\xB5\x53\xF5\x65\x9D\x65\x0D  ", 16))
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szMac[0],szMac[1],szMac[2],szMac[3],szMac[4],szMac[5],szMac[6],szMac[7],szMac[8],szMac[9],szMac[10],szMac[11],szMac[12],szMac[13],szMac[14],szMac[15]);
		GOTOERR;
	}
	
	//case3:正常测试:以SEC_MAC_AES模式,42字节数据输入,TAK3
	if ((ret=NDK_SecGetMac(5, szDataIn, 42, szMac, SEC_MAC_AES))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szMac, "\xA1\xFC\x91\xF6\x8E\x48\x11\x59\x93\x92\x68\x45\xEA\xBE\x1C\x16", 16))
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szMac[0],szMac[1],szMac[2],szMac[3],szMac[4],szMac[5],szMac[6],szMac[7],szMac[8],szMac[9],szMac[10],szMac[11],szMac[12],szMac[13],szMac[14],szMac[15]);
		GOTOERR;
	}

	//case4:异常测试:以SEC_MAC_AES模式,16字节数据输入,TPK
	if ((ret=NDK_SecGetMac(6, szDataIn, 16, szMac, SEC_MAC_AES))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);  
		GOTOERR;
	}
	
#if SEC_RSA_ENABLE
	//case5:异常测试:以SEC_MAC_AES模式,16字节数据输入,RSA
	if ((ret=NDK_SecGetMac(7, szDataIn, 16, szMac, SEC_MAC_AES))!=NDK_ERR_SECCR_GET_KEY)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if DUKPT_ENABLE	//支持DUKPT密钥密钥体系的才进行DUKPT密钥测试
	//case6:异常测试:以SEC_MAC_AES模式,16字节数据输入,DUKPT
	if ((ret=NDK_SecGetMac(8, szDataIn, 16, szMac, SEC_MAC_AES))!=NDK_ERR_SECCR_GET_KEY)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
#endif

#if defined N900AZ||defined N910AZ ||defined IM81AZ
	if(g_seckeyowner)
#endif
	{
		//case7:正常测试:以SEC_MAC_AES模式,42字节数据输入,TAK4
		if ((ret=NDK_SecGetMac(9, szDataIn, 42, szMac, SEC_MAC_AES))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(memcmp(szMac, "\xF8\xCE\x3B\x6C\x87\xA6\xAD\xA1\x1E\xCC\x08\x60\xFC\x98\xB8\x4F", 16))
		{
			send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szMac[0],szMac[1],szMac[2],szMac[3],szMac[4],szMac[5],szMac[6],szMac[7],szMac[8],szMac[9],szMac[10],szMac[11],szMac[12],szMac[13],szMac[14],szMac[15]);
			GOTOERR;
		}
	}
	send_result("%s测试通过", TESTAPI);
	
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}

