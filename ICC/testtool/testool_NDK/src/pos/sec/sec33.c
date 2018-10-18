/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec33.c
* Author 		: linwl
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试Ndk_getVKeybPin能否启用虚拟键盘,从键盘读入按键进行PIN计算,并将计算PINBLOCK的结果送回(DUKPT密钥体系)
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
#define TESTAPI	"Ndk_getVKeybPin"

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
*			  	  	 		linwl       20160720	created
*****************************************************************/
void sec33(void)
{
	int ret = 0, i = 0;
	uchar KeyValueIn[17]={0}, KSN1[11]={0}, KSN2[11]={0}, KSN3[11]={0};//, KSNOUT[11]={0}
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
	char *szPan="6225885916163157";
	char szPan1[8] = {0x00,0x00,0x58,0x85,0x91,0x61,0x63,0x15}, szPinOutStr[8+1]={0}, szPinOut[16+1]={0};
	uchar DukptKeyValue[16]={0xD5,0xD4,0x4F,0xF7,0x20,0x68,0x3D,0x0D,0x70,0xBE,0x25,0x28,0x18,0xE2,0xA1,0x8A};//DUKPY初始明文密钥	
	uchar DukptPinKeyValue[16]={0x6E,0xF9,0x27,0x95,0xE5,0x12,0x71,0xB2,0xCD,0xEB,0x76,0x7B,0x33,0xF1,0xA9,0x42};//DUKPY派生出的pin密钥
	char KSN[11]={0};//初始KSN

	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}

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
	if ((ret=NDK_SecLoadTIK(6, 0, 16, DukptKeyValue, (uchar *)KSN, &KcvInfoIn))!=NDK_OK)
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
	
	//密文安装一组DUKPT, ID=2, KSN为10个字节0x00 \xDA\x36\xEC\x83\x4A\xDE\x38\xD9\x5A\x51\x68\xF9\x5E\x9A\x5E\x23 是DukptKeyValue 经ID1密钥加密后的密文(分成前后两个8字节计算)
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memcpy(KeyValueIn, "\xDA\x36\xEC\x83\x4A\xDE\x38\xD9\x5A\x51\x68\xF9\x5E\x9A\x5E\x23", 16);
	if ((ret=NDK_SecLoadTIK(2, 1, 16, KeyValueIn, (uchar *)KSN, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if TR31_ENABLE	//支持TR31格式密钥安装的设备才进行TR31测试
	//TR-31密文安装一组DUKPT, ID=3, KSN为10个字节0x00,计算出的结果  block 安装到ID=3中  
	memset(mkey, 0x11, 16);
	memset(KSN, 0x00, 10);
	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = 3;
	memcpy(tr31_st.usage, KT_BDK_TYPE, 4);	/*DUKPT*/
	tr31_st.key = DukptKeyValue;//将DUKPT明文密钥按照TR31格式进行计算后得到block
	tr31_st.keylen = 16;
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//将DUKPT明文密钥按照TR31格式进行计算后得到block，然后进行安装
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadTIK(3, 1, len, block, (uchar *)KSN, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else
	//密文安装一组DUKPT, ID=3, KSN为10个字节0x00 \xDA\x36\xEC\x83\x4A\xDE\x38\xD9\x5A\x51\x68\xF9\x5E\x9A\x5E\x23 是DukptKeyValue 经ID1密钥加密后的密文(分成前后两个8字节计算)
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(KSN, 0x00, 10);
	memcpy(KeyValueIn, "\xDA\x36\xEC\x83\x4A\xDE\x38\xD9\x5A\x51\x68\xF9\x5E\x9A\x5E\x23", 16);
	if ((ret=NDK_SecLoadTIK(3, 1, 16, KeyValueIn, (uchar *)KSN, &KcvInfoIn))!=NDK_OK)
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

	//case1:非法测试
	//case1.1:pszExpPinLenIn==NULL
	if((ret=Ndk_getVKeybPin(NULL, 2, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, szPan, KSN, szPinOut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=Ndk_getVKeybPin("0", 2, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, NULL, KSN, szPinOut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=Ndk_getVKeybPin("0", 2, SEC_PIN_ISO9564_1, PINTIMEOUT_MIN, NULL, KSN, szPinOut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=Ndk_getVKeybPin("4", 2, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, KSN, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case1.2:uckeyIdx不存在
	if((ret=Ndk_getVKeybPin("4", 10, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, szPan, KSN, szPinOut))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case1.3:ucMod 非法
	if((ret=Ndk_getVKeybPin("4", 2, -1, PINTIMEOUT_MIN, szPan, KSN, szPinOut))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.4:nTimeOutMs>MAX或nTimeOutMs<MIN
	if((ret=Ndk_getVKeybPin("4", 2, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX+1, szPan, KSN, szPinOut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=Ndk_getVKeybPin("4", 2, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN-1, szPan, KSN, szPinOut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.5:以SEC_PIN_ISO9564_0,pszExpPinLenIn=13,pszDataIn!=NULL,应失败
	//cls_printf("尽快输入1234567890123并确认...");
	if((ret=Ndk_getVKeybPin("13", 3, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, KSN, szPinOut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.7:测试NDK_SecGetPinResultDukpt 传入参数为NULL 应该返回失败
	if((ret=Ndk_getVKeybPin("4", 6, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, NULL, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:以SEC_PIN_ISO9564_0,pszExpPinLenIn=0,pszDataIn!=NULL, ID=6,应成功
	cls_printf("\n请尽快按[确认]...");
	if((ret=Ndk_getVKeybPin("0", 6, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, KSN, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if(memcmp(szPinOut, "0000000000000000", 16))	/*与开发确认后,若pszExpPinLenIn="0"则返回全0*/
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例通过!");

	//case19:验证输入长度限制出现重复"0,0,0"预期按确认返回NDK_OK，应用发现该情况下按确认会返回失败-1121的BUG，因此导入该用例
	cls_printf("\n请尽快按[确认]...");
	if((ret=Ndk_getVKeybPin("0,0,0", 6, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, KSN, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if(memcmp(szPinOut, "0000000000000000", 16))	/*与开发确认后,若pszExpPinLenIn="0"则返回全0*/
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例通过!");
	
	//case4:以SEC_PIN_ISO9564_1,pszExpPinLenIn=4,pszDataIn=NULL, ID=2,应成功
	cls_printf("\n尽快输入123并确认,若无反应继续输4并确认...");
	if((ret=Ndk_getVKeybPin("4,4", 2, SEC_PIN_ISO9564_1, PINTIMEOUT_MAX, NULL, KSN, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_AscToHex((uchar *)szPinOut,strlen((char *)szPinOut),0,(uchar *)szPinOutStr);//需增加操作将字符串szPinOut转化成HEX后进行计算
	//输出的密文通过des解密后可以得到明文,明文的前几位数据是固定的可以判断
	if((ret=NDK_AlgTDes((uchar *)szPinOutStr, PinDesOut, DukptPinKeyValue, sizeof(DukptPinKeyValue), ALG_TDS_MODE_DEC)) !=NDK_OK)
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

	//case12:以SEC_PIN_ISO9564_2,pszExpPinLenIn=6,pszDataIn=NULL, ID=3,应成功
	memset(szPinOut, 0, sizeof(szPinOut));
	cls_printf("\n请尝试输入1237890后,如不能输入再退格改为123456并确认...");
	if((ret=Ndk_getVKeybPin("6", 3, SEC_PIN_ISO9564_2, PINTIMEOUT_MAX, NULL, KSN, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	//该结果为使用16字节PIN密钥 6ef92795e51271b2cdeb767b33f1a942 对SEC_PIN_ISO9564_2格式PINBLOCK 26123456FFFFFFFF进行3DES加密得出 C0866C8443869AED 
	if(memcmp(szPinOut, "C0866C8443869AED", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case5:以SEC_PIN_ISO9564_3,pszExpPinLenIn=12,pszDataIn!=NULL, ID=6,应成功	
	memset(szPinOut, 0, sizeof(szPinOut));
	cls_printf("\n尽快输入123456789012并确认...");
	if((ret=Ndk_getVKeybPin("12", 6, SEC_PIN_ISO9564_3, PINTIMEOUT_MAX, szPan, KSN, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_AscToHex((uchar *)szPinOut,strlen((char *)szPinOut),0,(uchar *)szPinOutStr);
	//输出的密文通过解密得出明文,明文再与卡号异或后可以得出固定的前几位数
	if((ret=NDK_AlgTDes((uchar *)szPinOutStr, PinDesOut1, DukptPinKeyValue, sizeof(DukptPinKeyValue), ALG_TDS_MODE_DEC)) !=NDK_OK)
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
	if((ret=Ndk_getVKeybPin("13", 6, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, KSN, szPinOut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7:以SEC_PIN_ISO9564_0,pszExpPinLenIn=12,pszDataIn!=NULL, ID=6
	cls_printf("\n尽快输入123456并按2次取消...");
	if((ret=Ndk_getVKeybPin("12", 6, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, KSN, szPinOut))!=NDK_ERR_QUIT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case8:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4,5,12",pszDataIn!=NULL, ID=6
	memset(szPinOut, 0, sizeof(szPinOut));
	cls_printf("\n尽快输入12345并确认...");
	if((ret=Ndk_getVKeybPin("4,5,12", 6, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, KSN, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//该结果为使用16字节PIN密钥 6ef92795e51271b2cdeb767b33f1a942对帐号和密钥进行PIN加密计算得出 69C1207610BB425F
	if(memcmp(szPinOut, "69C1207610BB425F", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case9:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL, ID=6,NDK_SecGetPinResultDukpt()函数获取 PINBLOCK 
	memset(szPinOut, 0, sizeof(szPinOut));
	cls_printf("\n尽快输入4321(将不回显输入按键)并确认...");
	if((ret=Ndk_getVKeybPin("4", 6, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, KSN, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//该结果为使用16字节PIN密钥 6ef92795e51271b2cdeb767b33f1a942对帐号和密钥进行PIN加密计算得出 F0828F1547B362AD
	if(memcmp(szPinOut, "F0828F1547B362AD", 16))
	{		
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case10:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL,nTimeOutMs=0, ID=6
	//cls_printf("尽快输入4321并确认...");    //nTimeOutMs=0 返回参数失败
	if((ret=Ndk_getVKeybPin("4", 6, SEC_PIN_ISO9564_0, 0, szPan, KSN, NULL))!=NDK_ERR_SECVP_TIMED_OUT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case13:以SEC_PIN_ISO9564_0,pszExpPinLenIn=4,pszDataIn!=NULL, ID=6,应成功
	memset(szPinOut, 0, sizeof(szPinOut));
	cls_printf("\n尽快输入4321后,再按1次取消,再输入1234并确认...");
	if((ret=Ndk_getVKeybPin("4", 6, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, KSN, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//该结果为使用16字节PIN密钥 6ef92795e51271b2cdeb767b33f1a942对帐号和密钥进行PIN加密计算得出 756000DE335639DF
	if(memcmp(szPinOut, "756000DE335639DF", 16))
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
	cls_printf("\n尽快输入1234并确认...");
	if((ret=Ndk_getVKeybPin("4", 2, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, (char *)KSN2, szPinOut))!=NDK_OK)
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
	cls_printf("\n尽快输入1并确认...");
	if((ret=Ndk_getVKeybPin("1", 4, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, szPan, KSN, szPinOut))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case16:ID=TPK's ID,应失败
	cls_printf("\n尽快输入1并确认...");
	if((ret=Ndk_getVKeybPin("1", 5, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, szPan, KSN, szPinOut))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case17: 在不为预期输入位数时候不让确认
	cls_printf("\n尽快输入12345并确认,若无反应继续输6并按确认");
	if((ret=Ndk_getVKeybPin("6,4", 2, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, szPan, KSN, szPinOut))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case18: 在有预期输入位数限定后应不能超过最大值
	cls_printf("\n请尝试输入123456789,应无法输入9,再按确认");
	if((ret=Ndk_getVKeybPin("8,6,4", 2, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, KSN, szPinOut))!=NDK_OK)
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
	cls_printf("\n尽快输入123456并确认...");
	if((ret=Ndk_getVKeybPin("6", 6, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, (char *)KSN1, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//本预期结果使用16字节密钥5635b184dd5d8fbe5617b6600c6e4730 与卡号 密码计算得出 E2EB3D877C330126
	if(memcmp(szPinOut, "E2EB3D877C330126", 16))
	{	
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if( KSN1[7]!=0x00 || KSN1[8]!=0x00 || KSN1[9]!=0x02)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	
	send_result("%s测试通过", TESTAPI);
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}

