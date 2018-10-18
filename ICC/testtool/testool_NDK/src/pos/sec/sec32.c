/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec32.c
* Author 		: linwl
* version		: 
* DATE			: 20160720
* directory 		: 
* description		: 测试Ndk_getVKeybPin能否启用虚拟键盘,从键盘读入按键进行PIN计算,并将计算PINBLOCK的结果送回(MK/SK密钥体系)
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
*			  	  	   		linwl       20160720	created
*****************************************************************/
void sec32(void)
{
	int ret = 0, i = 0;
	tr31_key_t tr31_st;
	unsigned int len = 0;
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	ST_EXTEND_KEYBLOCK *p;
	uchar sKeyValueIn[17]={0}, sKsnIn[11]={0}, mkey[17]={0}, skey[17]={0}, block[512]={0};
	char szPinOutStr[8+1]={0}, szPinOut[16+1]={0}, szExpPinLenIn[100+1]={0};
	char *szPan="6225885916163157";	
	char szPan1[8] = {0x00,0x00,0x58,0x85,0x91,0x61,0x63,0x15};
#if SEC_RSA_ENABLE
	ST_SEC_RSA_KEY RsaKey;
	char *Pmod=(uchar *)"66C2BCD4AC436815C193A8D29A0755C80393D7AFC7D78B12FE371F91874D449271207DFA6A054D185B03E543137C59577769940830FEEA040053F72583B85951";
#endif
	uchar PinKeyValue[8]={0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15};
	uchar PinKeyValue1[24]={0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19};
	uchar PinDesOut[9]={0},PinDesOut1[9]={0};
	//char KSN[11]={0};//初始KSN
	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...请确保安卓系统已经安装好虚拟键盘APK", TESTAPI); 

	//测试前置:擦除所有密钥
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//安装TMK, ID=5
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=16;
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(stKeyInfo.sDstKeyValue, 0x11, stKeyInfo.nDstKeyLen);
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//安装TAK(16bytes), ID=1
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if !OVERSEAS_ENABLE
	//安装TPK1(8bytes), ID=2,密文安装
	stKeyInfo.ucScrKeyIdx=5;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=8;
	memcpy(stKeyInfo.sDstKeyValue, "\x14\x5F\x5C\x6E\x3D\x91\x44\x57", stKeyInfo.nDstKeyLen);/*对应的key明文为8个字节的0x15*/
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//安装TPK2(16bytes), ID=3,TR-31格式安装
	stKeyInfo.ucScrKeyIdx=5;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, 0x11, 16);/*由TMK发散*/
	//memset(skey, 0x17, 16); /*TPK2, 密钥明文16个字节0x17*/
	memset(skey, 0x17, 8);	/*TAK2密钥明文:8个字节的0x17+8个字节0x19*/
	memset(skey+8, 0x19, 8);	
	
	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_PIN_TYPE, 4);	/*TPK*/
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

	//安装TPK3(24bytes), ID=4
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=24;
	memset(stKeyInfo.sDstKeyValue, 0x19, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if SEC_RSA_ENABLE
	//明文安装RSA, ID=6
	RsaKey.usBits=RSA_KEY_LEN_512;
	strcpy((char *)RsaKey.sExponent, "03");
	memcpy(RsaKey.sModulus, Pmod, RsaKey.usBits*2/8);
	if ((ret=NDK_SecLoadRsaKey(6, &RsaKey))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	
	//明文安装DUKPT,ID=7	
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(sKeyValueIn, 0x1d, 16);
	memset(sKsnIn, 0x12, 10);
	if ((ret=NDK_SecLoadTIK(7, 0, 16, sKeyValueIn, sKsnIn, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1:非法测试
	//case1.1:pinlen, account, KSN, pinblock这4个参数为NULL的情况应该返回参数错误-4022
	strcpy((char *)szExpPinLenIn, "4,5,6,7,8,9,10,11,12");
	if((ret=Ndk_getVKeybPin(NULL, 2, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, szPan, NULL, szPinOut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=Ndk_getVKeybPin("0", 3, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, NULL, NULL, szPinOut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=Ndk_getVKeybPin("0", 3, SEC_PIN_ISO9564_1, PINTIMEOUT_MIN, NULL, NULL, szPinOut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=Ndk_getVKeybPin("13", 2, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, szPan, NULL, szPinOut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=Ndk_getVKeybPin("4", 3, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, NULL, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case1.2:uckeyIdx不存在
	if((ret=Ndk_getVKeybPin(szExpPinLenIn, 10, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, szPan, NULL, szPinOut))!=NDK_ERR_SECCR_GET_KEY)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.3:ucMod 非法
	if((ret=Ndk_getVKeybPin(szExpPinLenIn, 2, -1, PINTIMEOUT_MIN, szPan, NULL, szPinOut))!=NDK_ERR_SECP_PARAM)   
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.4:nTimeOutMs>MAX或nTimeOutMs<MIN
	if((ret=Ndk_getVKeybPin(szExpPinLenIn, 2, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX+1, szPan, NULL, szPinOut))!=NDK_ERR_PARA)	 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=Ndk_getVKeybPin(szExpPinLenIn, 2, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN-1, szPan, NULL, szPinOut))!=NDK_ERR_PARA)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.5:以SEC_PIN_ISO9564_0,pszExpPinLenIn=13,pszDataIn!=NULL,应失败
	//cls_printf("尽快输入1234567890123并确认...");
	if((ret=Ndk_getVKeybPin("13", 3, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, NULL, szPinOut))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:以SEC_PIN_ISO9564_0,pszExpPinLenIn=0,pszDataIn!=NULL,应成功
	cls_printf("\n请不要进行任何操作等待安卓系统自动进入休眠");
	if((ret=Ndk_getVKeybPin("0", 3, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, NULL, szPinOut))!=NDK_ERR_QUIT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(10, "子用例通过!请尽快唤醒POS");//尽快唤醒POS，否则在休眠状态测试虚拟键盘会返回-11

	//case3:验证输入长度限制出现重复"0,0,0"预期按确认返回NDK_OK，应用发现该情况下按确认会返回失败-1121的BUG，因此导入该用例
	cls_printf("\n请尽快按[确认]...");
	if((ret=Ndk_getVKeybPin("0,0,0", 3, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, NULL, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(memcmp(szPinOut, "0000000000000000", 16))	/*与开发确认后,若pszExpPinLenIn="0"则返回全0*/
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, szPinOut);
		GOTOERR;
	}
	
	//case4:以SEC_PIN_ISO9564_1,pszExpPinLenIn=4,pszDataIn=NULL,应成功(SEC_PIN_ISO9564_1 模式下使用随机数，故不做比较)
	cls_printf("\n尽快输入123并确认,若无反应继续输4并确认...");
	if((ret=Ndk_getVKeybPin("4,4", 2, SEC_PIN_ISO9564_1, PINTIMEOUT_MAX, szPan, NULL, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_AscToHex((uchar *)szPinOut,strlen((char *)szPinOut),0,(uchar *)szPinOutStr);//需增加操作将字符串szPinOut转化成HEX后进行计算
	//输出的密文通过des解密后可以得到明文,明文的前几位数据是固定的可以判断 added jiangym 20150125
	if((ret=NDK_AlgTDes((uchar *)szPinOutStr, PinDesOut, PinKeyValue, sizeof(PinKeyValue), ALG_TDS_MODE_DEC)) !=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//明文为141234xxxxxxxxxx (16位)第一位表示计算模式为SEC_PIN_ISO9564_1,第二位表示密码有4位,后面是密码1234剩余的是随机数
	if(memcmp(PinDesOut, "\x14\x12\x34", 3))
	{
		send_result("line %d:%s测试失败0x%2x,0x%2x,0x%2x", __LINE__, TESTAPI, PinDesOut[0], PinDesOut[1], PinDesOut[2]);
		GOTOERR;
	}

	//case5:以SEC_PIN_ISO9564_2,pszExpPinLenIn=6,pszDataIn=NULL,应成功
	cls_printf("\n请尝试输入1237890后,如不能输入再退格改为123456并确认...");
	if((ret=Ndk_getVKeybPin("6", 3, SEC_PIN_ISO9564_2, PINTIMEOUT_MAX, szPan, NULL, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//如果使用8字节0x17+8字节0x19对26123456FFFFFFFF计算结果为:\xe2\x9c\x72\xc1\x6\x43\x53\x48
	if(memcmp(szPinOut,"E29C72C106435348",16))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, szPinOut);
		GOTOERR;
	}
	
	//case6:以SEC_PIN_ISO9564_3,pszExpPinLenIn=12,pszDataIn!=NULL,应成功(SEC_PIN_ISO9564_3 模式下使用随机数，故不做比较)
	cls_printf("\n尽快输入123456789012并确认...");
	if((ret=Ndk_getVKeybPin("12", 4, SEC_PIN_ISO9564_3, PINTIMEOUT_MAX, szPan, NULL, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_AscToHex((uchar *)szPinOut,strlen((char *)szPinOut),0,(uchar *)szPinOutStr);
	//输出的密文通过解密得出明文,明文再与卡号异或后可以得出固定的前几位数  added jiangym 20150125
	if((ret=NDK_AlgTDes((uchar *)szPinOutStr, PinDesOut1, PinKeyValue1, sizeof(PinKeyValue1), ALG_TDS_MODE_DEC)) !=NDK_OK)
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
	//异或后的结果:第一位表示计算模式为SEC_PIN_ISO9564_3,第二位表示密码有10位(a),后面是密码123456789012剩余的是随机数
	if(memcmp(PinDesOut, "\x3c\x12\x34\x56\x78\x90\x12", 7))
	{		
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, PinDesOut[0],PinDesOut[1],PinDesOut[2],PinDesOut[3],PinDesOut[4],PinDesOut[5],PinDesOut[6],PinDesOut[7]);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例通过!");

	//case7:以SEC_PIN_ISO9564_0,pszExpPinLenIn=12,pszDataIn!=NULL
	cls_printf("\n尽快输入123456并按2次取消...");
	if((ret=Ndk_getVKeybPin("12", 3, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, NULL, szPinOut))!=NDK_ERR_QUIT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//按键保护
	cls_show_msg1(2, "子用例通过!不要再按[取消]");

	//case8:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4,5,12",pszDataIn!=NULL
	cls_printf("\n尽快输入12345并确认...");
	if((ret=Ndk_getVKeybPin("4,5,12", 3, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, NULL, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szPinOut, "E9F2EDDD59857FD0", 16))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, szPinOut);
		GOTOERR;
	}

	//case9:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL,nTimeOutMs=MIN
	cls_printf("\n请不要按键,耐心等待%ds...", PINTIMEOUT_MIN/1000);
	if((ret=Ndk_getVKeybPin("4", 3, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, szPan, NULL, szPinOut))!=NDK_ERR_SECVP_TIMED_OUT)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case10:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL,ucKeyIdx=TAK's ID
	if((ret=Ndk_getVKeybPin("4", 1, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, szPan, NULL, szPinOut))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if SEC_RSA_ENABLE
	//case11:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL,ucKeyIdx=RSA's ID
	if((ret=Ndk_getVKeybPin("4", 6, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, szPan, NULL, szPinOut))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case12:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL,ucKeyIdx=DUKPT's ID
	if((ret=Ndk_getVKeybPin("4", 7, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN, szPan, NULL, szPinOut))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case13:以SEC_PIN_ISO9564_0,pszExpPinLenIn=4,pszDataIn!=NULL,应成功
	cls_printf("\n尽快输入4321后,再按1次取消,再输入1234并确认...");
	if((ret=Ndk_getVKeybPin("4", 3, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, NULL, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szPinOut, "2E4000EFF86DCA6A", 16))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, szPinOut);
		GOTOERR;
	}

	//case14: 在不为预期输入位数时候不让确认
	cls_printf("\n尽快输入12345并确认,若无反应继续输6并按确认");
	if((ret=Ndk_getVKeybPin("6,4", 3, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, NULL, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szPinOut, "98ECAB1BC863DF00",16))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, szPinOut);
		GOTOERR;
	}

	//case15: 在有预期输入位数限定后应不能超过最大值
	cls_printf("\n请尝试输入123456789,应无法输入9,再按确认");
	if((ret=Ndk_getVKeybPin("8,6,4", 3, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX, szPan, NULL, szPinOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szPinOut, "6B76244371639AF9",16))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, szPinOut);
		goto ERR;
	}
	
	send_result("%s测试通过", TESTAPI);
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}

