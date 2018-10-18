/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec28.c
* Author 		: zhengry	
* version		: 
* DATE			: 20160107
* directory 		: 
* description		: 测试NDK_SecGetMac国密MAC计算接口功能
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
* history 		 		:  author		date	remarks
*			  	  	      zhengry   20160115	created
*****************************************************************/
void sec28(void)
{
	int ret=0;
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar szDataIn[257]={0},szMac[17]={0};
#if DUKPT_ENABLE	//支持DUKPT密钥密钥体系的才进行DUKPT密钥安装
	uchar sKeyValueIn[17]={0},sKsnIn[11]={0};
#endif
#if	TR31_ENABLE    
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
	cls_show_msg1(2, "测试%s...", TESTAPI); 

	//测试前置:擦除所有密钥 
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	memset(szDataIn, 0x20, sizeof(szDataIn));

	//明文安装TMK,id=2,明文为8个0x11+8个0x13
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, "\x11\x11\x11\x11\x11\x11\x11\x11\x13\x13\x13\x13\x13\x13\x13\x13", stKeyInfo.nDstKeyLen);
	
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
		
	//以SEC_KCV_NONE明文方式安装TAK1,id=3，明文为16个0x15
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, "\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15", stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//以SEC_KCV_ZERO方式(TMK发散)安装TAK2,id=4
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, "\x16\x26\xE4\x01\xBF\xF1\x1B\x1B\x64\xF7\x4D\x21\x39\xEF\x27\xFA", stKeyInfo.nDstKeyLen);/*对应的key明文16个字节的0x17*/

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, "\x66\x42\xC0\xC2", stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
#if TR31_ENABLE	//支持TR31格式密钥安装的设备才进行TR31测试
	//以SEC_KCV_ZERO方式(TMK以TR31方式打包发散)安装TAK3,id=5
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memcpy(mkey, "\x11\x11\x11\x11\x11\x11\x11\x11\x13\x13\x13\x13\x13\x13\x13\x13", 16);//对应于TMK 
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
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x1b, 8);
	memset(stKeyInfo.sDstKeyValue+8, 0x1d, 8);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//以SEC_KCV_NONE明文方式安装TPK,id=6,明文为16个0x1f
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=6;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, "\x1f\x1f\x1f\x1f\x1f\x1f\x1f\x1f\x1f\x1f\x1f\x1f\x1f\x1f\x1f\x1f", stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
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

	//case1:正常测试:以SEC_MAC_SM4模式,7字节数据输入,TAK1
	if ((ret=NDK_SecGetMac(3, szDataIn, 7, szMac, SEC_MAC_SM4))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szMac, "\x6F\x41\x61\x91\x36\xAF\x4B\xC4\xDE\xBD\x30\x7E\x6B\x85\x17\x3C", 16))
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szMac[0],szMac[1],szMac[2],szMac[3],szMac[4],szMac[5],szMac[6],szMac[7],szMac[8],szMac[9],szMac[10],szMac[11],szMac[12],szMac[13],szMac[14],szMac[15]);
		GOTOERR;
	}
	
	//case2:正常测试:以SEC_MAC_SM4模式,16字节数据输入,TAK2
	if ((ret=NDK_SecGetMac(4, szDataIn, 16, szMac, SEC_MAC_SM4))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szMac, "\x33\xBA\x97\x84\x7A\xCA\x62\xF6\xCC\xF4\x9E\x78\xA1\x2C\x53\xF6", 16))
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szMac[0],szMac[1],szMac[2],szMac[3],szMac[4],szMac[5],szMac[6],szMac[7],szMac[8],szMac[9],szMac[10],szMac[11],szMac[12],szMac[13],szMac[14],szMac[15]);
		GOTOERR;
	}
	
	//case3:正常测试:以SEC_MAC_SM4模式,256字节数据输入,TAK3
	if ((ret=NDK_SecGetMac(5, szDataIn, 256, szMac, SEC_MAC_SM4))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szMac, "\xfa\xcd\x5f\x4e\xba\x8a\x2d\x05\x3a\x96\x10\xed\x11\xa9\x22\xed", 16))
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szMac[0],szMac[1],szMac[2],szMac[3],szMac[4],szMac[5],szMac[6],szMac[7],szMac[8],szMac[9],szMac[10],szMac[11],szMac[12],szMac[13],szMac[14],szMac[15]);
		GOTOERR;
	}

	//case4:异常测试:以SEC_MAC_SM4模式,16字节数据输入,TPK
	if ((ret=NDK_SecGetMac(6, szDataIn, 16, szMac, SEC_MAC_SM4))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);  
		GOTOERR;
	}
	
#if SEC_RSA_ENABLE
	//case5:异常测试:以SEC_MAC_SM4模式,16字节数据输入,RSA
	if ((ret=NDK_SecGetMac(7, szDataIn, 16, szMac, SEC_MAC_SM4))!=NDK_ERR_SECCR_GET_KEY)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if DUKPT_ENABLE	//支持DUKPT密钥密钥体系的才进行DUKPT密钥测试
	//case6:异常测试:以SEC_MAC_SM4模式,16字节数据输入,DUKPT
	if ((ret=NDK_SecGetMac(8, szDataIn, 16, szMac, SEC_MAC_SM4))!=NDK_ERR_SECCR_GET_KEY)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
#endif

	send_result("%s测试通过", TESTAPI);
	
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}

