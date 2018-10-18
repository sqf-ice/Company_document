/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec6.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecGetPin,NDK_SecGetPinResult接口功能
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
#define TESTAPI	"NDK_SecGetPin,NDK_SecGetPinResult"

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
*			  	  	   linrq       20130228	created
*****************************************************************/
void sec6(void)
{
	int ret = 0, nStatus = 0, i = 0;
#if TR31_ENABLE
	tr31_key_t tr31_st;
	unsigned int len = 0;
	ST_EXTEND_KEYBLOCK *p;
	uchar mkey[17]={0},skey[17]={0},block[512]={0};
#endif	
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar sKeyValueIn[17]={0}, sKsnIn[11]={0},szExpPinLenIn[100+1]={0},szPinOut[8+1]={0};
	uchar *szPan=(uchar *)"6225885916163157";	
	uchar szPan1[8] = {0x00,0x00,0x58,0x85,0x91,0x61,0x63,0x15};
#if SEC_RSA_ENABLE
	ST_SEC_RSA_KEY RsaKey;
	uchar *Pmod=(uchar *)"66C2BCD4AC436815C193A8D29A0755C80393D7AFC7D78B12FE371F91874D449271207DFA6A054D185B03E543137C59577769940830FEEA040053F72583B85951";
#endif
	uchar PinKeyValue[8]={0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15};
	uchar PinKeyValue1[24]={0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19,0x19};
	uchar PinDesOut[9]={0},PinDesOut1[9]={0};
	cls_show_msg1(2, "测试%s...", TESTAPI); 

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

#if !(OVERSEAS_ENABLE||defined ME30S||defined N910P)//ME30S是海外产品
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

#if TR31_ENABLE	//支持TR31格式密钥安装的设备才进行TR31测试
	//安装TPK2(16bytes), ID=3,TR-31格式安装
	stKeyInfo.ucScrKeyIdx=5;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, 0x11, 16);/*由TMK发散*/
	//memset(skey, 0x17, 16); /*TPK3, 密钥明文16个字节0x17*/
	memset(skey, 0x17, 8);	/*TPK3密钥明文:8个字节的0x17+8个字节0x19*/
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
#else
	//不支持TR31格式的产品改用明文安装方式安装密钥
	stKeyInfo.ucScrKeyIdx=0;/*表示明文安装*/
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, "\x17\x17\x17\x17\x17\x17\x17\x17\x19\x19\x19\x19\x19\x19\x19\x19", stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

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
if((auto_flag==0)||(auto_flag==1))
{
	//case3:不初始化触屏获取屏幕按键值，则返回初始化失败
#if ANDIROD_ENABLE && !defined IM81AZ
	if((ret=NDK_SecGetPin(2, (uchar *)"0", szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_SECVP_NOT_ACTIVE)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinResult(szPinOut, &nStatus))!=NDK_ERR_SECVP_NOT_ACTIVE)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case1:非法测试
	//case1.1:pszExpPinLenIn==NULL
	strcpy((char *)szExpPinLenIn, "4,5,6,7,8,9,10,11,12");

	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPin(2, NULL, szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_PARA)
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
	if((ret=NDK_SecGetPin(2, szExpPinLenIn, szPan, szPinOut, -1, PINTIMEOUT_MIN))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.4:nTimeOutMs>MAX或nTimeOutMs<MIN
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPin(2, szExpPinLenIn, szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX+1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPin(2, szExpPinLenIn, szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.5:uckeyIdx不存在
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPin(10, szExpPinLenIn, szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_SECCR_GET_KEY)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.6:测试NDK_SecGetPinResult 传入的参数为NULL 应该返回参数失败
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPin(3/*2*/, (uchar *)"4", szPan, NULL, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinResult(NULL, &nStatus))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPinResult(szPinOut, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:以SEC_PIN_ISO9564_0,pszExpPinLenIn=0,pszDataIn=NULL,应失败
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPin(3/*2*/, (uchar *)"0", NULL, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_PARA)		
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
}
if((auto_flag==0)||(auto_flag==2))
{
	//case3:以SEC_PIN_ISO9564_0,pszExpPinLenIn=0,pszDataIn!=NULL,应成功
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !(K21_ENABLE||defined SP50||defined SP50G)//低端,SP50没有自动休眠功能
	cls_printf("请等待60秒后按[确认],等待过程中不应该进入休眠");
#else
#if ANDIROD_ENABLE
	NDK_ScrPrintf("请尽快按[确认]...");
#else
	cls_printf("请尽快按[确认]...");
#endif
#endif
	if((ret=NDK_SecGetPin(3/*2*/, (uchar *)"0", szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !(K21_ENABLE||defined SP50||defined SP50G)//低端,SP50没有自动休眠功能
	if(cls_show_msg("看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif	
	if(memcmp(szPinOut, "\x00\x00\x00\x00\x00\x00\x00\x00", 8)) /*与开发确认后,若pszExpPinLenIn="0"则返回全0*/
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szPinOut[0],szPinOut[1],szPinOut[2],szPinOut[3],szPinOut[4],szPinOut[5],szPinOut[6],szPinOut[7]);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例通过!");

	//case17:验证输入长度限制出现重复"0,0,0"预期按确认返回NDK_OK，应用发现该情况下按确认会返回失败-1121的BUG，因此导入该用例
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
	if((ret=NDK_SecGetPin(3/*2*/, (uchar *)"0,0,0", szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(memcmp(szPinOut, "\x00\x00\x00\x00\x00\x00\x00\x00", 8))	/*与开发确认后,若pszExpPinLenIn="0"则返回全0*/
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szPinOut[0],szPinOut[1],szPinOut[2],szPinOut[3],szPinOut[4],szPinOut[5],szPinOut[6],szPinOut[7]);
		GOTOERR;
	}

#if !(OVERSEAS_ENABLE||defined ME30S||defined N910P)//ME30S是海外产品
	//case4:以SEC_PIN_ISO9564_1,pszExpPinLenIn=4,pszDataIn=NULL,应成功(SEC_PIN_ISO9564_1 模式下使用随机数，故不做比较)
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
	if((ret=NDK_SecGetPin(2, (uchar *)"4,4", NULL, szPinOut, SEC_PIN_ISO9564_1, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	//输出的密文通过des解密后可以得到明文,明文的前几位数据是固定的可以判断 added jiangym 20150125
	if((ret=NDK_AlgTDes(szPinOut, PinDesOut, PinKeyValue, sizeof(PinKeyValue), ALG_TDS_MODE_DEC)) !=NDK_OK)
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
#endif

#if 1  /*底层更新:输入密码的位数应不超过设定值,故屏蔽此用例chensj 20130726*/
	//case12:以SEC_PIN_ISO9564_2,pszExpPinLenIn=6,pszDataIn=NULL,应成功
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
	if((ret=NDK_SecGetPin(3, (uchar *)"6", NULL, szPinOut, SEC_PIN_ISO9564_2, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//如果使用8字节0x17+8字节0x19对26123456FFFFFFFF计算结果为:\xe2\x9c\x72\xc1\x6\x43\x53\x48
	if(memcmp(szPinOut,"\xe2\x9c\x72\xc1\x6\x43\x53\x48",8))
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szPinOut[0],szPinOut[1],szPinOut[2],szPinOut[3],szPinOut[4],szPinOut[5],szPinOut[6],szPinOut[7]);
		GOTOERR;
	}
#endif
	
	//case5:以SEC_PIN_ISO9564_3,pszExpPinLenIn=12,pszDataIn!=NULL,应成功(SEC_PIN_ISO9564_3 模式下使用随机数，故不做比较)
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
	if((ret=NDK_SecGetPin(4, (uchar *)"12", szPan, szPinOut, SEC_PIN_ISO9564_3, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	//输出的密文通过解密得出明文,明文再与卡号异或后可以得出固定的前几位数  added jiangym 20150125
	if((ret=NDK_AlgTDes(szPinOut, PinDesOut1, PinKeyValue1, sizeof(PinKeyValue1), ALG_TDS_MODE_DEC)) !=NDK_OK)
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
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例通过!");

	//case6:以SEC_PIN_ISO9564_0,pszExpPinLenIn=13,pszDataIn!=NULL,应失败
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入1234567890123并确认...");
#else
	cls_printf("尽快输入1234567890123并确认...");
#endif		
	if((ret=NDK_SecGetPin(3/*2*/, (uchar *)"13", szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7:以SEC_PIN_ISO9564_0,pszExpPinLenIn=12,pszDataIn!=NULL
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
	if((ret=NDK_SecGetPin(3/*2*/, (uchar *)"12", szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_ERR_SECVP_GET_ESC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//按键保护
	cls_show_msg1(2, "子用例通过!不要再按[取消]");

	//case8:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4,5,12",pszDataIn!=NULL
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
	if((ret=NDK_SecGetPin(3/*2*/, (uchar *)"4,5,12", szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szPinOut, "\xE9\xF2\xED\xDD\x59\x85\x7F\xD0", 8))		//密钥3的计算结果
	//if(memcmp(szPinOut, "\x4F\xCB\xA0\x15\xC8\xDA\x83\x9D", 8))	//密钥2的计算结果
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szPinOut[0],szPinOut[1],szPinOut[2],szPinOut[3],szPinOut[4],szPinOut[5],szPinOut[6],szPinOut[7]);
		GOTOERR;
	}
	
	//case9:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL,NDK_SecGetPinResult()函数获取 PINBLOCK 
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入1234(将不会显输入按键)并确认...");//根据高端组反馈说pinblock中有0x00后续都会变成00的bug,修改测试数据为1234  20171115
#else
	cls_printf("尽快输入1234(将不会显输入按键)并确认...");
#endif		
	if((ret=NDK_SecGetPin(3/*2*/, (uchar *)"4", szPan, NULL, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !(K21_ENABLE||defined SP50||defined SP50G)//K21平台不支持忙状态判断 lijq, SP50产品不支持休眠20170418linying added
	//新增用例验证处于PIN输入状态下设备处于忙状态，将不会自动休眠,且NDK_SysGoSuspend增加PIN忙状态返回： NDK_ERR_PIN_BUSY = -3107	
#if ANDIROD_ENABLE
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_PIN_BUSY)
#else
	if((ret=NDK_SysGoSuspend())!=NDK_ERR_PIN_BUSY)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif	
#if 0   /*根据开发人员建议,修改NDK_SecGetPinResult使用*/
	if((ret=NDK_SecGetPinResult(szPinOut, &nStatus))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else	
	do {
		ret=NDK_SecGetPinResult(szPinOut, &nStatus);
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
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			goto ERR;
			break;
		}
		NDK_SysMsDelay(10);//usleep(10*1000);
	} while(1);
#endif
	if(memcmp(szPinOut, "\x2e\x40\x00\xef\xf8\x6d\xca\x6a", 8))//\x35\x27\x9a\xf7\x14\xda\xce\x0d
	//if(memcmp(szPinOut, "\x1E\x03\x49\xA2\x7B\x5D\x55\x16", 8))
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szPinOut[0],szPinOut[1],szPinOut[2],szPinOut[3],szPinOut[4],szPinOut[5],szPinOut[6],szPinOut[7]);
		GOTOERR;
	}
#if !(defined SP50||defined SP50G)//SP50产品不支持休眠20170418linying added
	cls_show_msg("按任意键,[确认键]除外,应该可以马上进入休眠,休眠后请唤醒");//此处按键响应方案导入按下生效后，如果按确认键继续进入休眠后唤醒又按确认键通过，两个确认键被识别成长按键，导致在552行出错，实际键码为400d，该情况经过郑凯确认
#if ANDIROD_ENABLE
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	usleep(1000000);
	if((ret=NDK_SysWakeUp()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else
	if((ret=NDK_SysGoSuspend())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	sleep(1);
#endif
	if(cls_show_msg("看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif
}
else
	send_result("line %d:%s部分子用例需手动测试", __LINE__, TESTAPI);
if((auto_flag==0)||(auto_flag==1))
{
	//case10:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL,nTimeOutMs=MIN
	cls_printf("请不要按键,耐心等待%ds...", PINTIMEOUT_MIN/1000);
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPin(3/*2*/, (uchar *)"4", szPan, szPinOut/*NULL*/, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_SECVP_TIMED_OUT/*NDK_ERR_SECP_TIMEOUT*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case11:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL,ucKeyIdx=TAK's ID
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPin(1, (uchar *)"4", szPan, szPinOut/*NULL*/, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if SEC_RSA_ENABLE
	//case12:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL,ucKeyIdx=RSA's ID
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPin(6, (uchar *)"4", szPan, szPinOut/*NULL*/, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case13:以SEC_PIN_ISO9564_0,pszExpPinLenIn="4",pszDataIn!=NULL,psPinBlockOut=NULL,ucKeyIdx=DUKPT's ID
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecGetPin(7, (uchar *)"4", szPan, szPinOut/*NULL*/, SEC_PIN_ISO9564_0, PINTIMEOUT_MIN))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
}
if((auto_flag==0)||(auto_flag==2))
{
	//case14:以SEC_PIN_ISO9564_0,pszExpPinLenIn=4,pszDataIn!=NULL,应成功
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
	if((ret=NDK_SecGetPin(3/*2*/, (uchar *)"4", szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szPinOut, "\x2e\x40\x00\xef\xf8\x6d\xca\x6a", 8))
	//if(memcmp(szPinOut, "\xE5\x5E\xB5\xED\xC5\xF6\x45\x8F", 8))
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szPinOut[0],szPinOut[1],szPinOut[2],szPinOut[3],szPinOut[4],szPinOut[5],szPinOut[6],szPinOut[7]);
		GOTOERR;
	}

	//case15: 在不为预期输入位数时候不让确认
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
	if((ret=NDK_SecGetPin(3/*2*/, (uchar *)"6,4", szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szPinOut, "\x98\xec\xab\x1b\xc8\x63\xdf\x00",8))
	//if(memcmp(szPinOut, "\x8E\x99\x8A\x1C\x02\x6F\x05\x21",8))
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szPinOut[0],szPinOut[1],szPinOut[2],szPinOut[3],szPinOut[4],szPinOut[5],szPinOut[6],szPinOut[7]);
		GOTOERR;
	}

	//case16: 在有预期输入位数限定后应不能超过最大值
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
	
	if((ret=NDK_SecGetPin(3/*2*/, (uchar *)"8,6,4", szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szPinOut, "\x6b\x76\x24\x43\x71\x63\x9a\xf9",8))
	//if(memcmp(szPinOut, "\xA6\x26\xC1\x9A\xDF\x99\x7D\x69",8))
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szPinOut[0],szPinOut[1],szPinOut[2],szPinOut[3],szPinOut[4],szPinOut[5],szPinOut[6],szPinOut[7]);
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

