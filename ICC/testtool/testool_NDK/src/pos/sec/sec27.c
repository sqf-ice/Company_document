/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec27.c
* Author 		: zhengry	
* version		: 
* DATE			: 20160107
* directory 		: 
* description		: 测试NDK_SecCalcDes计算国密算法sm4加解密接口功能
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
*			  	  	      zhengry   20160114	created
*****************************************************************/
void sec27()
{
	ST_SEC_KEY_INFO stKeyInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;
	int ret=0;
	uchar udesout[17]={0}, udesin[17]={0};
#if	TR31_ENABLE    
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
	cls_show_msg1(2, "测试%s...", TESTAPI); 
	
	memset(udesin, 0x20, 16);

	//测试前置:擦除所有密钥
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//明文安装TMK,ID=2
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, 0x11, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); 
		RETURN;
	}
	
	//明文安装TDK1,ID=3
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); 
		RETURN;
	}
	
	//TMK安装TDK2,ID=4
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, 0x15, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); 
		RETURN;
	}
	
#if TR31_ENABLE	//支持TR31格式密钥安装的设备才进行TR31测试
	//TMK以TR31格式打包安装TDK3,ID=5
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
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
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x17, 8);
	memset(stKeyInfo.sDstKeyValue+8, 0x27, 8);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//明文安装TAK,ID=6
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=6;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x19, stKeyInfo.nDstKeyLen/2);//修改密钥前8字节和后8字节不一致 
	memset(stKeyInfo.sDstKeyValue+stKeyInfo.nDstKeyLen/2, 0x29, stKeyInfo.nDstKeyLen/2);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//明文安装TPK,ID=7
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=7;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x1b, stKeyInfo.nDstKeyLen);
	
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
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

	//case1.1:异常测试:以SEC_SM4_ENCRYPT加密/SEC_SM4_DECRYPT解密,数据长度不是16整数倍 应该失败
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 8, udesout, SEC_SM4_ENCRYPT))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 15, udesout, SEC_SM4_DECRYPT))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.3:异常测试:uckeyIdx不存在(id=10)
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 10, udesin, 16, udesout, SEC_SM4_ENCRYPT))!=NDK_ERR_SECCR_GET_KEY)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:正常测试:ucKeyType或上算法类型，应该也可以成功
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK|SEC_KEY_SM4, 3, udesin, 16, udesout, SEC_SM4_ENCRYPT))!=NDK_OK)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期结果可以由16字节的0x20经16字节的0x13密钥在sm4算法下加密得到(工具为中国银联基础加解密工具软件)
	if(memcmp(udesout, "\x75\x6C\x98\xAE\x73\x17\xB8\xCF\x7D\xA2\x02\xD7\x62\x6F\x17\xF9", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case3:正常测试:以SEC_SM4_ENCRYPT 加密 密钥ID=3
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 16, udesout, SEC_SM4_ENCRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(udesout, "\x75\x6C\x98\xAE\x73\x17\xB8\xCF\x7D\xA2\x02\xD7\x62\x6F\x17\xF9", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case4:正常测试:以SEC_SM4_DECRYPT 解密 密钥ID=4
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 4, udesin, 16, udesout, SEC_SM4_DECRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期结果是由16字节的密文0X20经ID=4的密钥值(由16个0X11字节对密文16个0X15解密得到)在sm4算法下解密得到(工具为中国银联基础加解密工具软件)
	if(memcmp(udesout, "\x24\xED\xA0\x62\xC1\xBC\x74\x88\xE3\x08\x3D\x79\x8B\xBF\xF1\x26", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case5:正常测试:以SEC_SM4_ENCRYPT 加密 密钥ID=5
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 5, udesin, 16, udesout, SEC_SM4_ENCRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期结果是由16字节的0X20经8个字节的0x17+8个字节的0x27的密钥(由TR31格式打包)在sm4算法下加密得到(工具为中国银联基础加解密工具软件)
	if(memcmp(udesout, "\x99\x5C\xB1\x8C\xC2\x96\xC2\xC1\x09\x98\x47\xF5\xAE\x07\x0F\x83", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case6:以SEC_SM4_DECRYPT模式,ucKeyIdx=TAK's ID,ucKeyType=TAK,应成功(后门功能)
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TAK, 6, udesin, 16, udesout, SEC_SM4_DECRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期结果可以由16字节的0x20经8个字节的0x19+8个字节的0x29密钥在sm4算法下解密得到(工具为中国银联基础加解密工具软件)
	if(memcmp(udesout, "\x02\x01\x16\xF1\xB6\xB3\xCD\x68\x21\xFC\xDF\x4B\x7A\x0A\x63\x20", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
#if !K21_ENABLE
	//case7:以SEC_SM4_DECRYPT模式,ucKeyIdx=TPK's ID,ucKeyType=TPK,应失败(适合中端)
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 7, udesin, 16, udesout, SEC_SM4_DECRYPT))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else
	//case8:(未导入还未测试)低端平台产品允许通过sm4运算接口来进行PIN密钥扩展sm4运算 ucKeyIdx=TPK's ID,ucKeyType=TPK进行sm4运算，应该成功  
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 7, udesin, 16, udesout, SEC_SM4_DECRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//预期结果可以由16字节的0x20经16字节的0x1b密钥在sm4算法下解密得到(工具为中国银联基础加解密工具软件)
	if(memcmp(udesout, "\x75\x4A\x94\xE0\x0F\xA7\x9B\x39\xBD\xB8\xA9\xEC\x30\x67\x01\xD3", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif

#if SEC_RSA_ENABLE
	//case9:以SEC_SM4_DECRYPT模式,ucKeyIdx=RSA's ID,ucKeyType=TDK,应失败
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 8, udesin, 16, udesout, SEC_SM4_DECRYPT))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if DUKPT_ENABLE	
	//case10:以SEC_SM4_DECRYPT模式,ucKeyIdx=DUKPT's ID,ucKeyType=TDK,应失败
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 9, udesin, 16, udesout, SEC_SM4_DECRYPT))!=NDK_ERR_SECCR_GET_KEY)
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
