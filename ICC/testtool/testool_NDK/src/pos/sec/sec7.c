/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec7.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecCalcDes接口功能
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
void sec7(void)
{
	int ret = 0;
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar udesout[31]={0}, udesin[31]={0};
#if DUKPT_ENABLE	//支持DUKPT密钥密钥体系的才进行DUKPT密钥安装
	uchar sKeyValueIn[17]={0},sKsnIn[11]={0};
#endif
#if	TR31_ENABLE && !(OVERSEAS_ENABLE||defined ME30S||defined N910P)
	tr31_key_t tr31_st;
	unsigned int len = 0;
	ST_EXTEND_KEYBLOCK* p;
	uchar block[512]={0},mkey[17]={0},skey[32]={0};
#endif
#if SEC_RSA_ENABLE
	ST_SEC_RSA_KEY RsaKey;
	uchar *Pmod=(uchar *)"66C2BCD4AC436815C193A8D29A0755C80393D7AFC7D78B12FE371F91874D449271207DFA6A054D185B03E543137C59577769940830FEEA040053F72583B85951";
#endif
#if ANDIROD_ENABLE
	uchar udeserrout[1025]={0}, udeerrsin[1025]={0};
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

	memset(udesin, 0x20, sizeof(udesin));

	//安装TMK, ID=5
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x11, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

#if !(OVERSEAS_ENABLE||defined ME30S||defined N910P)//ME30S是海外产品
	//安装TDK1(密钥8字节),ID=1
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=8;
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//安装TDK2(密钥16字节),ID=2,密文安装
	stKeyInfo.ucScrKeyIdx=5;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, "\x14\x5F\x5C\x6E\x3D\x91\x44\x57\x38\xBE\xDB\x24\xA6\xD3\x80\x18", stKeyInfo.nDstKeyLen); /*对应的key明文为8个字节的0x15+8个字节的0x17，修改前8后8密钥不一致*/
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if TR31_ENABLE&&!(OVERSEAS_ENABLE||defined ME30S||defined N910P)	//支持TR31格式密钥安装的设备才进行TR31测试
	//安装TDK3(密钥24字节),ID=3,TR-31格式安装
	stKeyInfo.ucScrKeyIdx=5;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, 0x11, 16);/*由TMK发散*/
//	memset(skey, 0x17, 24);
	memset(skey, 0x17, 8);
	memset(skey+8, 0x19, 8);
	memset(skey+16, 0x1B, 8);
	
	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = skey;
	tr31_st.keylen = 24;
	
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
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=24;
	memset(stKeyInfo.sDstKeyValue, 0x17, 8);
	memset(stKeyInfo.sDstKeyValue+8, 0x19, 8);
	memset(stKeyInfo.sDstKeyValue+16, 0x1B, 8);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//安装TAK, ID=4
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x19, stKeyInfo.nDstKeyLen/2);//修改密钥前8字节和后8字节不一致 linwl20130903
	memset(stKeyInfo.sDstKeyValue+stKeyInfo.nDstKeyLen/2, 0x1B, stKeyInfo.nDstKeyLen/2);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//安装TPK, ID=8
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK;
	stKeyInfo.ucDstKeyIdx=8;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x1b, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if SEC_RSA_ENABLE//!K21_ENABLE
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
	
#if DUKPT_ENABLE	//支持DUKPT密钥密钥体系的才进行DUKPT密钥安装
	//明文安装DUKPT,ID=7	
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(sKeyValueIn, 0x1f, 16);
	memset(sKsnIn, 0x12, 10);
	if ((ret=NDK_SecLoadTIK(7, 0, 16, sKeyValueIn, sKsnIn, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case1:非法测试
	//case1.1:psDataIn==NULL
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, NULL, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.2:psDataOut==NULL
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, 8, NULL, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_PARA)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case1.3:ucMod 非法
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, 8, udesout, 100))!=NDK_ERR_SECCR_PARAM)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.4:Datalen<=0
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, -1, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.5:uckeyIdx不存在(10,101)
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 10, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECCR_GET_KEY)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0 //101已合法
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 101, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if 0 //合法的后门功能,见case12
	//case1.6:ucKeyType=TAK
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TAK, 4, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case1.7:nDataInLen非8整数倍(nDataInLen=5),20130625经过确认非8整数倍应该返回参数错误，底层需进行修改linwl
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 5, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECP_PARAM/*NDK_OK*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if ANDIROD_ENABLE
	//case1.8:输入数据psDataIn大于1024时返回错误	//20160531根据N900 3G版本 V1.1.16新增用例
	memset(udeerrsin, 5, sizeof(udeerrsin));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udeerrsin, sizeof(udeerrsin), udeserrout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if !(OVERSEAS_ENABLE||defined ME30S||defined N910P)//ME30S是海外产品
	//case2:以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8|SEC_DES_ECBMODE模式, 8字节数据输入,TDK1
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 8))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case3:以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|SEC_DES_ECBMODE模式, 16字节数据输入,TDK1
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, 16, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case4:以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24|SEC_DES_ECBMODE模式, 24字节数据输入,TDK1
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, 24, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9\x38\x77\x48\x93\xF5\x5F\xF4\xC9\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 24))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif

	//case5:以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_8|SEC_DES_CBCMODE模式, 8字节数据输入,TDK2	
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 8, udesout, SEC_DES_DECRYPT|SEC_DES_KEYLEN_8))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !OVERSEAS_ENABLE
	if(memcmp(udesout, "\x28\xC6\xF7\xAC\x64\x87\x4D\xC1", 8))
#else  //海外版本最后一个参数无效均使用输入的id2 密钥长度16字节进行加解密20160706 modify
	if(memcmp(udesout, "\x1E\x91\xC7\xC6\x2A\xBC\xCC\xA7", 8))
#endif
	{
		send_result("line %d:%s测试失败(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)", __LINE__, TESTAPI,udesout[0],udesout[1],udesout[2],udesout[3],udesout[4],udesout[5],udesout[6],udesout[7]);
		GOTOERR;
	}

	//case6:以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_16|SEC_DES_CBCMODE模式, 16字节数据输入,TDK2
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 16, udesout, SEC_DES_DECRYPT|SEC_DES_KEYLEN_16))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(memcmp(udesout, "\x1E\x91\xC7\xC6\x2A\xBC\xCC\xA7\x1E\x91\xC7\xC6\x2A\xBC\xCC\xA7", 16))//密钥修改成前8字节和后8字节不一样，相应的修改预期结果linwl20130903
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case7:以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_CBCMODE模式, 16字节数据输入,TDK2	
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 16, udesout, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(udesout, "\x1E\x91\xC7\xC6\x2A\xBC\xCC\xA7\x1E\x91\xC7\xC6\x2A\xBC\xCC\xA7", 16))//密钥修改成前8字节和后8字节不一样，相应的修改预期结果linwl20130903
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case8:以 SEC_DES_DECRYPT|SEC_DES_KEYLEN_24|SEC_DES_CBCMODE模式, 24字节数据输入,TDK2
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 24, udesout, SEC_DES_DECRYPT|SEC_DES_KEYLEN_24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(memcmp(udesout, "\x1E\x91\xC7\xC6\x2A\xBC\xCC\xA7\x1E\x91\xC7\xC6\x2A\xBC\xCC\xA7\x1E\x91\xC7\xC6\x2A\xBC\xCC\xA7", 24))//密钥修改成前8字节和后8字节不一样，相应的修改预期结果linwl20130903
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case9:以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8|SEC_DES_ECBMODE模式, 8字节数据输入,TDK3
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !OVERSEAS_ENABLE
	if(memcmp(udesout, "\x38\x12\x10\xE0\x4F\xFA\xFA\x80", 8))
#else //海外版本最后一个参数无效均使用输入的id3 密钥长度24字节进行加解密20160706 modify
	if(memcmp(udesout, "\x51\xBA\xAF\xE8\x08\xFA\x27\x37", 8))
#endif
	{
	send_result("line %d:%s测试失败(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)", __LINE__, TESTAPI,udesout[0],udesout[1],udesout[2],udesout[3],udesout[4],udesout[5],udesout[6],udesout[7]);
		GOTOERR;
	}

	//case10:以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16|SEC_DES_ECBMODE模式,16字节数据输入,TDK3
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 16, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !OVERSEAS_ENABLE
	if(memcmp(udesout, "\xE8\x78\x69\x90\xBB\x51\x92\xD2\xE8\x78\x69\x90\xBB\x51\x92\xD2", 16))//密钥修改成前8字节和后8字节不一样，相应的修改预期结果linwl20130903
#else //海外版本最后一个参数无效均使用输入的id3 密钥长度24字节进行加解密20160706 modify
	if(memcmp(udesout, "\x51\xBA\xAF\xE8\x08\xFA\x27\x37\x51\xBA\xAF\xE8\x08\xFA\x27\x37", 16))
#endif
	{
	send_result("line %d:%s测试失败(%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x)", __LINE__, TESTAPI,udesout[0],udesout[1],udesout[2],udesout[3],udesout[4],udesout[5],udesout[6],udesout[7]);
		GOTOERR;
	}

	//case11:以 SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24|SEC_DES_ECBMODE模式,24字节数据输入,TDK3
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 24, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(udesout, "\x51\xBA\xAF\xE8\x08\xFA\x27\x37\x51\xBA\xAF\xE8\x08\xFA\x27\x37\x51\xBA\xAF\xE8\x08\xFA\x27\x37", 24))//密钥修改成前8字节和后8字节不一样，相应的修改预期结果linwl20130903
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case12:以SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_CBCMODE模式,ucKeyIdx=TAK's ID,ucKeyType=TAK,应成功(后门功能)
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TAK, 4, udesin, 16, udesout, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(udesout, "\x3F\xFA\x7C\x9D\xEB\xD8\xD5\x1C\x3F\xFA\x7C\x9D\xEB\xD8\xD5\x1C", 16))//密钥修改成前8字节和后8字节不一样，相应的修改预期结果linwl20130903
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
#if !K21_ENABLE
	//case13:以SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_CBCMODE模式,ucKeyIdx=TPK's ID,ucKeyType=TPK,应失败(适合中端)
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 8, udesin, 16, udesout, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#else
	//case13:低端平台产品允许通过DES运算接口来进行PIN密钥扩展DES运算 ucKeyIdx=TPK's ID,ucKeyType=TPK进行DES运算，应该成功 
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 8, udesin, 16, udesout, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(udesout, "\x4D\x58\xB8\xA6\xCD\x9D\xEE\x86\x4D\x58\xB8\xA6\xCD\x9D\xEE\x86", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif

#if SEC_RSA_ENABLE
	//case14:以SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_CBCMODE模式,ucKeyIdx=RSA's ID,ucKeyType=TDK,应失败
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 6, udesin, 16, udesout, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

#if DUKPT_ENABLE	//支持DUKPT密钥密钥体系的才进行DUKPT密钥测试
	//case15:以SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT|SEC_DES_CBCMODE模式,ucKeyIdx=DUKPT's ID,ucKeyType=TDK,应失败
	 if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 7, udesin, 16, udesout, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT))!=NDK_ERR_SECCR_GET_KEY)
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

