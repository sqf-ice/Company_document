/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec6.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecGetPinDukpt,NDK_SecGetPinResultDukpt接口功能
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
#define TESTAPI	"NDK_SecGetPinDukpt,NDK_SecGetPinResultDukpt"

#define PINTIMEOUT_MAX	(200*1000)
#define PINTIMEOUT_MIN	(5*1000)
//#define KT_BDK_TYPE             "B1TE"

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
*****************************************************************/
void sec19(void)
{
	int ret = 0, nStatus = 0, i = 0;
	uchar KeyValueIn[17]={0}, szPinOut[8+1]={0}, KSNOUT[11]={0}, KSN1[11]={0},KSN2[11]={0},KSN3[11]={0};
	uchar PinDesOut[9]={0}, PinDesOut1[9]={0};
	ST_SEC_KCV_INFO KcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
#if TR31_ENABLE
	tr31_key_t tr31_st;
	unsigned int len = 0;
	uchar mkey[17]={0},block[512]={0};
#endif
#if SEC_RSA_ENABLE
	ST_SEC_RSA_KEY RsaKeyIn;
	uchar *uModeulus1=(uchar *)"4FB2D08918F766B30B2144FE25D09A160FFCF256086D82773FB04D4BD7E4772F2557FC25EE130A0B984467995BE80EE1751F577BD3E5CAFF60712722F33D0479";
#endif
	uchar *szPan=(uchar *)"6225885916163157";
	uchar szPan1[8] = {0x00,0x00,0x58,0x85,0x91,0x61,0x63,0x15};
	uchar DukptKeyValue[16]={0xD5,0xD4,0x4F,0xF7,0x20,0x68,0x3D,0x0D,0x70,0xBE,0x25,0x28,0x18,0xE2,0xA1,0x8A};//DUKPY初始明文密钥	
	uchar DukptPinKeyValue[16]={0x6E,0xF9,0x27,0x95,0xE5,0x12,0x71,0xB2,0xCD,0xEB,0x76,0x7B,0x33,0xF1,0xA9,0x42};//DUKPY派生出的pin密钥
	uchar DukptPinKeyValue2[16]={0xba,0xe6,0x98,0xc2,0x15,0x01,0x8d,0x2a,0xe3,0xcf,0x99,0x43,0x3d,0xd1,0x56,0xf1};//id为2的DUKPT密钥派生出的pin密钥
	uchar KSN[11]={0};//初始KSN

	cls_show_msg1(2, "测试%s...", TESTAPI); 

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

	//安装TLK, ID=1 本密钥用于密文安装第ID=2的DUKPT密钥
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
		RETURN;
	}
	
	 //由于海外版本禁止安装相同的DES密钥 (包括DES密钥,DUKPT 初始密钥TIK) 国兵解释,故这里使用不同的密钥安装使用于所有平台产品 20180116 modify
	//密文安装一组DUKPT, ID=2, KSN为10个字节0x00 \xDA\x36\xEC\x83\x4A\xDE\x38\xD9\x5A\x51\x68\xF9\x5E\x9A\x5E\x23 是如下明文 经ID1密钥加密后的密文(分成前后两个8字节计算)
	//此处密文是明文为:4B82DBD972BBBF20BCBA8632CF9AB57C与16位0x11加密后的结果:1A5DA388BAB978A8AA577F104AF0FC9B
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memcpy(KeyValueIn, "\x1A\x5D\xA3\x88\xBA\xB9\x78\xA8\xAA\x57\x7F\x10\x4A\xF0\xFC\x9B", 16);
	if ((ret=NDK_SecLoadTIK(2, 1, 16, KeyValueIn, KSN, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0  //这里安装后的id2与id6是一样的密钥 
	//密文安装一组DUKPT, ID=2, KSN为10个字节0x00 \xDA\x36\xEC\x83\x4A\xDE\x38\xD9\x5A\x51\x68\xF9\x5E\x9A\x5E\x23 是DukptKeyValue 经ID1密钥加密后的密文(分成前后两个8字节计算)
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memcpy(KeyValueIn, "\xDA\x36\xEC\x83\x4A\xDE\x38\xD9\x5A\x51\x68\xF9\x5E\x9A\x5E\x23", 16);
	if ((ret=NDK_SecLoadTIK(2, 1, 16, KeyValueIn, KSN, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if TR31_ENABLE	//支持TR31格式密钥安装的设备才进行TR31测试
	//TR-31密文安装一组DUKPT, ID=3, KSN为10个字节0x00,计算出的结果  block 安装到ID=3中  
	memset(mkey, 0x11, 16);
	memset(KSN, 0x00, 10);
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
	//密文安装一组DUKPT, ID=3, KSN为10个字节 是明文 经ID1密钥加密后的密文(分成前后两个8字节计算)
	//明文25AB8A24A244883CB2CA76D7617BC60B与16位0x11des加密结果(分成前后8字节)3CC6B71F7117702C3850C279A2CF834D
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(KSN, 0x00, 10);
	memcpy(KeyValueIn, "\x3C\xC6\xB7\x1F\x71\x17\x70\x2C\x38\x50\xC2\x79\xA2\xCF\x83\x4D", 16);
	if ((ret=NDK_SecLoadTIK(3, 1, 16, KeyValueIn, KSN, &KcvInfoIn))!=NDK_OK)
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
	memset(stKeyInfo.sDstKeyValue, 0x21, stKeyInfo.nDstKeyLen);
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &KcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
if(auto_flag==0 || auto_flag==1)
{
	//case3:不初始化触屏获取屏幕按键值，则返回初始化失败
#if ANDIROD_ENABLE && !defined IM81AZ 
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"4", szPan, KSN, NULL, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_ERR_SECVP_NOT_ACTIVE)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinResultDukpt(szPinOut, KSNOUT, &nStatus))!=NDK_ERR_SECVP_NOT_ACTIVE)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case1:非法测试
	//case1.1:pszExpPinLenIn==NULL
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinDukpt(2, NULL, szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.2:pszDataIn==NULL //pszDataIn某些情况下可以为空.放到下面用例测试.

	//case1.3:ucMod 非法
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinDukpt(2, (uchar *)"1,2", szPan, KSN,szPinOut, -1, PINTIMEOUT_MIN))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.4:nTimeOutMs>MAX或<MIN
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinDukpt(2, (uchar *)"4", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX+1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinDukpt(2, (uchar *)"4", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.5:ucGroupIdx不存在(10,101),和NDK_SecGetPin一样返回NDK_ERR_SECCR_GET_KEY
	//不应有提示!zhangnw20130913 cls_printf("尽快输入1234并确认...");
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinDukpt(10, (uchar *)"4", szPan,KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.6:psKsnOut=NULL 应该要失败
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinDukpt(2, (uchar *)"4", szPan, NULL, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.7:测试NDK_SecGetPinResultDukpt 传入参数为NULL 应该返回失败
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"4", szPan, KSN, NULL, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinResultDukpt(NULL, KSNOUT, &nStatus))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinResultDukpt(szPinOut, NULL, &nStatus))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinResultDukpt(szPinOut, KSNOUT, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:以SEC_PIN_ISO9564_0,pszExpPinLenIn=0,pszDataIn=NULL, ID=6,应失败，SEC_PIN_ISO9564_0要求使用主帐号加密的同时主帐号为NULL，应该返回NDK_ERR_PARA
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"0", NULL, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
}
if(auto_flag==0 || auto_flag==2)
{
	//case3:以SEC_PIN_ISO9564_0,pszExpPinLenIn=0,pszDataIn!=NULL, ID=6,应成功
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("请尽快按[确认]...");
#else
	cls_printf("请尽快按[确认]...");
#endif
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"0", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if(memcmp(szPinOut, "\x00\x00\x00\x00\x00\x00\x00\x00", 8))	/*与开发确认后,若pszExpPinLenIn="0"则返回全0*/
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例通过!");

	//case19:验证输入长度限制出现重复"0,0,0"预期按确认返回NDK_OK，应用发现该情况下按确认会返回失败-1121的BUG，因此导入该用例
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("请尽快按[确认]...");
#else
	cls_printf("请尽快按[确认]...");
#endif
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"0,0,0", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if(memcmp(szPinOut, "\x00\x00\x00\x00\x00\x00\x00\x00", 8))	/*与开发确认后,若pszExpPinLenIn="0"则返回全0*/
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例通过!");
	
	//case4:以SEC_PIN_ISO9564_1,pszExpPinLenIn=4,pszDataIn=NULL, ID=2,应成功
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入123并确认,若无反应继续输4并确认...");
#else
	cls_printf("尽快输入123并确认,若无反应继续输4并确认...");
#endif
	if((ret=NDK_SecGetPinDukpt(2, (uchar *)"4,4"/*"4"*/, NULL, KSN, szPinOut, SEC_PIN_ISO9564_1, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//id为2初始密钥改变,派生出的pin密钥也改变为:bae698c215018d2ae3cf99433dd156f1
	//输出的密文通过des解密后可以得到明文,明文的前几位数据是固定的可以判断
	if((ret=NDK_AlgTDes(szPinOut, PinDesOut, DukptPinKeyValue2, sizeof(DukptPinKeyValue2), ALG_TDS_MODE_DEC)) !=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//明文为141234xxxxxxxxxx (16位)第一位表示计算模式为SEC_PIN_ISO9564_1,第二位表示密码有4位,后面是密码1234剩余的是随机数
	if(memcmp(PinDesOut, "\x14\x12\x34", 3))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

#if 1 /*根据最新修改,输入的密码不应超过设定的值，故屏蔽此用例*/
	//case12:以SEC_PIN_ISO9564_2,pszExpPinLenIn=6,pszDataIn=NULL, ID=3,应成功
	memset(szPinOut, 0, sizeof(szPinOut));
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("请尝试输入1237890后,如不能输入再退格改为123456并确认...");
#else
	cls_printf("请尝试输入1237890后,如不能输入再退格改为123456并确认...");
#endif
	if((ret=NDK_SecGetPinDukpt(3, (uchar *)"6", NULL, KSN, szPinOut, SEC_PIN_ISO9564_2, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	//修改为使用id3初始密钥派生出PIN密钥9b53578c8110f739f3bf8ca3bbff63a7对SEC_PIN_ISO9564_2格式PINBLOCK 26123456FFFFFFFF进行3DES加密得出3983B8E7E8A856AA
	/*if(memcmp(szPinOut, "\xC0\x86\x6C\x84\x43\x86\x9A\xED", 8))*/
	if(memcmp(szPinOut, "\x39\x83\xB8\xE7\xE8\xA8\x56\xAA", 8))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif

	//case5:以SEC_PIN_ISO9564_3,pszExpPinLenIn=12,pszDataIn!=NULL, ID=6,应成功	
	memset(szPinOut, 0, sizeof(szPinOut));
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入123456789012并确认...");
#else
	cls_printf("尽快输入123456789012并确认...");
#endif
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"12", szPan, KSN, szPinOut, SEC_PIN_ISO9564_3, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//输出的密文通过解密得出明文,明文再与卡号异或后可以得出固定的前几位数
	if((ret=NDK_AlgTDes(szPinOut, PinDesOut1, DukptPinKeyValue, sizeof(DukptPinKeyValue), ALG_TDS_MODE_DEC)) !=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//明文为PinDesOut1 然后与卡号右边12位(去掉最后一位,前面补0)  即:0000588591616315异或得出结果再比较	
	memset(PinDesOut, 0, sizeof(PinDesOut));
	for (i = 0; i < 8; i++) 
	{
		PinDesOut[i] = PinDesOut1[i] ^ szPan1[i];
	}
	//异或后的结果:第一位表示计算模式为SEC_PIN_ISO9564_3,第二位表示密码有12位(c),后面是密码123456789012剩余的是随机数
	if(memcmp(PinDesOut, "\x3c\x12\x34\x56\x78\x90\x12", 7))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例通过!");

	//case6:以SEC_PIN_ISO9564_0,pszExpPinLenIn=13,pszDataIn!=NULL, ID=6,应失败
	//cls_printf("尽快输入1234567890123并确认...");
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"13", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7:以SEC_PIN_ISO9564_0,pszExpPinLenIn=12,pszDataIn!=NULL, ID=6
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入123456并按2次取消...");
#else
	cls_printf("尽快输入123456并按2次取消...");
#endif	
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"12", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_ERR_SECVP_GET_ESC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case8:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4,5,12",pszDataIn!=NULL, ID=6
	memset(szPinOut, 0, sizeof(szPinOut));
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入12345并确认...");
#else
	cls_printf("尽快输入12345并确认...");
#endif	
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"4,5,12", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//该结果为使用16字节PIN密钥 6ef92795e51271b2cdeb767b33f1a942对帐号和密钥进行PIN加密计算得出 69C1207610BB425F
	if(memcmp(szPinOut, "\x69\xC1\x20\x76\x10\xBB\x42\x5F", 8))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case9:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL, ID=6,NDK_SecGetPinResultDukpt()函数获取 PINBLOCK 
	memset(szPinOut, 0, sizeof(szPinOut));
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入4321(将不回显输入按键)并确认...");
#else
	cls_printf("尽快输入4321(将不回显输入按键)并确认...");
#endif
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"4", szPan, KSN, NULL, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0
	if((ret=NDK_SecGetPinResult(szPinOut, &nStatus))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else
	do {
		ret= NDK_SecGetPinResultDukpt(szPinOut, KSNOUT, &nStatus);
		if (ret != 0) {
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if (nStatus == SEC_VPP_KEY_ENTER) {		/*确认键按下，可返回数据*/
			break;
		} else if (nStatus == SEC_VPP_KEY_ESC) {	/*取消键按下，返回错误*/
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			goto ERR;
			break;
		} else if (nStatus == SEC_VPP_KEY_PIN || nStatus == SEC_VPP_KEY_BACKSPACE || nStatus == SEC_VPP_KEY_CLEAR ||  nStatus == SEC_VPP_KEY_NULL) {   
			//由于本函数在存在pinblock时，界面由状态栏处理，这里对其他键不处理
			//继续循环等待
			//由于取PIN函数需要非阻塞，即使没值也要返回应用程序处理（判断是否中断退出），因此增加SEC_VPP_NULL 返回值
		} else {
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nStatus);
			goto ERR;
			break;
		}
		NDK_SysMsDelay(10);//usleep(10*1000);
	} while(1);
#endif
	//该结果为使用16字节PIN密钥 6ef92795e51271b2cdeb767b33f1a942对帐号和密钥进行PIN加密计算得出 F0828F1547B362AD
	if(memcmp(szPinOut, "\xF0\x82\x8F\x15\x47\xB3\x62\xAD", 8))
	{		
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case10:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL,nTimeOutMs=0, ID=6
	//cls_printf("尽快输入4321并确认...");    //nTimeOutMs=0 返回参数失败
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"4", szPan, KSN, NULL, SEC_PIN_ISO9564_0, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case13:以SEC_PIN_ISO9564_0,pszExpPinLenIn=4,pszDataIn!=NULL, ID=6,应成功
	memset(szPinOut, 0, sizeof(szPinOut));
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入4321后,再按1次取消,再输入1234并确认...");
#else
	cls_printf("尽快输入4321后,再按1次取消,再输入1234并确认...");
#endif
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"4", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//该结果为使用16字节PIN密钥 6ef92795e51271b2cdeb767b33f1a942对帐号和密钥进行PIN加密计算得出 756000DE335639DF
	if(memcmp(szPinOut, "\x75\x60\x00\xDE\x33\x56\x39\xDF", 8))
	{	
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case14:验证GETPIN后,KSN不变
	//getksn
	if((ret=NDK_SecGetDukptKsn(2, KSN1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//getpin
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入1234并确认...");
#else
	cls_printf("尽快输入1234并确认...");
#endif
	if((ret=NDK_SecGetPinDukpt(2, (uchar *)"4", szPan, KSN2, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
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
	//case15:ID=RSA's ID,应失败
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入1并确认...");
#else
	cls_printf("尽快输入1并确认...");
#endif
	if((ret=NDK_SecGetPinDukpt(4, (uchar *)"1", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_SECCR_GET_KEY/*NDK_ERR_SECVP_GENERALERROR*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case16:ID=TPK's ID,应失败
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入1并确认...");
#else
	cls_printf("尽快输入1并确认...");
#endif
	if((ret=NDK_SecGetPinDukpt(5, (uchar *)"1", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_SECCR_GET_KEY/*NDK_ERR_SECVP_GENERALERROR*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case17: 在不为预期输入位数时候不让确认
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入12345并确认,若无反应继续输6并按确认");
#else
	cls_printf("尽快输入12345并确认,若无反应继续输6并按确认");
#endif
	if((ret=NDK_SecGetPinDukpt(2, (uchar *)"6,4", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case18: 在有预期输入位数限定后应不能超过最大值
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("请尝试输入123456789,应无法输入9,再按确认");
#else
	cls_printf("请尝试输入123456789,应无法输入9,再按确认");
#endif
	if((ret=NDK_SecGetPinDukpt(2, (uchar *)"8,6,4", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case19:KSN自加后，相应的派生前的密钥会自动更新，并且KSN要自动更新
	if ((ret=NDK_SecIncreaseDukptKsn(6))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//自加后派生前密钥变成:5635b184dd5d8f415617b6600c6e47cf，由此派生出pin加解密密钥5635b184dd5d8fbe5617b6600c6e4730
	memset(szPinOut, 0, sizeof(szPinOut));
	memset(KSN1, 0x00, sizeof(KSN1));
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入123456并确认...");
#else
	cls_printf("尽快输入123456并确认...");
#endif
	if((ret=NDK_SecGetPinDukpt(6, (uchar *)"6", szPan, KSN1, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//本预期结果使用16字节密钥5635b184dd5d8fbe5617b6600c6e4730 与卡号 密码计算得出 E2EB3D877C330126
	if(memcmp(szPinOut, "\xE2\xEB\x3D\x87\x7C\x33\x01\x26", 8))
	{	
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if( KSN1[7]!=0x00 || KSN1[8]!=0x00 || KSN1[9]!=0x02)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
}	
else
	send_result("line %d:%s部分子用例需手动测试", __LINE__, TESTAPI);
	send_result("%s测试通过", TESTAPI);
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}

