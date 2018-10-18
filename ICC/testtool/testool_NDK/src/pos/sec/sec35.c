/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name		: sec35.c
* Author 		: sull
* version		: 
* DATE			: 20180321
* directory 		: 
* description		: 测试NDK_SecLoadKey AES密钥装载接口功能
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
#define TESTAPI	"NDK_SecLoadKey"	

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
*			  	  	      sull   20180321	created
*****************************************************************/
void sec35(void)
{
	int ret=0;
	ST_SEC_KEY_INFO stKeyInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_EXTEND_KEYBLOCK stExtendKey;
#if	0//TR31_ENABLE
	tr31_key_t tr31_st;
	unsigned int len = 0;
	ST_EXTEND_KEYBLOCK* p;
	uchar block[512]={0},mkey[17]={0};
#endif
	KEY_KCV SecLoad[ ]={
		{(uchar *)"\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11\x11",	           						NULL},				// 0  case1.1
		{(uchar *)"\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12\x12",		   						NULL},				// 1  case1.2  明文安装TLK(算法为DES),明文为16个字节的0x12
		{(uchar *)"\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13",		   						NULL},				// 2  case1.3  (异常)源密钥(密钥类型TLK,密钥算法AES,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,密钥算法DES) 
		{(uchar *)"\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14\x14",		   						NULL},	                        // 3  case1.4: (异常)源密钥(密钥类型TLK,密钥算法AES,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,密钥算法SM4)  
		{(uchar *)"\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15",		   						NULL},                          // 4  case1.5: (异常)源密钥(密钥类型TLK,密钥算法DES,索引号为1)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TMK,密钥算法DES)
		{(uchar *)"\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16",		   						NULL},				// 5  case1.6  明文安装TLK(算法为DES),明文为16个字节的0x16  (TLK与TMK可以一样的密钥)可以适用于case1.13 源密钥(密钥类型TLK,密钥算法DES,索引号为1)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TMK,密钥算法DES,索引号为2)case1.14 源密钥(密钥类型TLK,密钥算法SM4,索引号为1)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TMK,密钥算法SM4,索引号为2)
		{(uchar *)"\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17",		   						NULL},				// 6  case1.7: (异常)源密钥(密钥类型TLK,密钥算法SM4,索引号为1)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TMK,密钥算法SM4)
		{(uchar *)"\x21\x21\x21\x21\x21\x21\x21\x21\x21\x21\x21\x21\x21\x21\x21\x21",		   						(uchar *)"\x9F\xC1\x89\x09"},				// 7  case1.8  明文安装TLK(算法为SM4),明文为16个字节的0x21  (不同体系密钥值可以相同故适用于case1.11 源密钥(密钥类型TLK,密钥算法AES,索引号为1)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TMK,索引号为2)  
		{(uchar *)"\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23",  (uchar *)"\xFE\x2B\xFD\x6A"},				// 8  case1.9  (异常)源密钥(密钥类型TLK,密钥算法DES,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,密钥算法AES)  安装失败了可以适用于case1.12 以SEC_KCV_ZERO方式明文安装目的密钥(密钥类型TMK,索引号为2)(覆盖安装)
		{(uchar *)"\x25\x25\x25\x25\x25\x25\x25\x25\x25\x25\x25\x25\x25\x25\x25\x25",		   						NULL},				// 9  case1.10 (异常)源密钥(密钥类型TLK,密钥算法SM4,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,密钥算法AES) 安装失败可适用于 case1.15 (异常)源密钥(密钥类型TMK,密钥算法AES,索引号为4)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TAK,索引号为3)
		{(uchar *)"\x27\x27\x27\x27\x27\x27\x27\x27\x27\x27\x27\x27\x27\x27\x27\x27",		  						 NULL},				//10  case1.16 (异常)源密钥(密钥type=100,密钥算法AES,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TAK,索引号为3)
		{(uchar *)"\x29\x29\x29\x29\x29\x29\x29\x29\x29\x29\x29\x29\x29\x29\x29\x29",		   						(uchar *)"\xD6\xB9\x4A\xAC"},	                        //11  case1.17 源密钥(密钥类型TMK,密钥算法AES,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TAK,索引号为3)
		{(uchar *)"\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31\x31",		   						NULL},				//12  case1.18 (异常)源密钥(密钥类型TAK,密钥算法AES,索引号为3)以SEC_KCV_NONE方式安装目的密钥(密钥类型TPK,索引号为4)
		{(uchar *)"\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33\x33",								  NULL},				//13  case1.19 (异常)源密钥(密钥类型TAK,密钥算法AES,索引号为3)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,索引号为4)
		{(uchar *)"\x35x35x35x35x35x35x35x35x35x35x35x35x35x35x35x35x35x35x35x35",                                                          (uchar *)"\x35\xA5\x70\xE2"},         //14  case1.20 源密钥(密钥类型TMK,密钥算法AES,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TPK,索引号为4) 
		{(uchar *)"\x37\x37\x37\x37\x37\x37\x37\x37",															   NULL},				//15 case1.21:异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TDK,密钥算法AES,索引号为5,长度为8),应返回失败,AES密钥长度只能为16
		{(uchar *)"\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39", NULL},				//16  case1.22:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TDK,索引号为5,长度为24),应返回成功
		{(uchar *)"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41",								   NULL},				//17 case1.23:正常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TDK,索引号为5,长度为16)
		{(uchar *)"\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43",                                                           NULL},  				//18 case1.24异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥type=100,索引号为6),由于type参数错误，应返回失败
		{(uchar *)"\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45",                                                           (uchar *)"\x19\xD5\xD5\x54"},  				//19  case1.25:异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TDK,索引号为7)应成功,再安装目的密钥(密钥类型TAK,索引号为8),应返回失败
		{(uchar *)"\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51\x51",								  NULL},				//20  case2.1  (异常)以SEC_KCV_NONE方式明文安装目的密钥(密钥类型TAK,索引号为4,长度为sizeof(ST_EXTEND_KEYBLOCK))
		{(uchar *)"\x52\x52\x52\x52\x52\x52\x52\x52\x52\x52\x52\x52\x52\x52\x52\x52",								  (uchar *)"\x40\xD2\x45\x55"},				//21  case2.2  (异常)以SEC_KCV_ZERO方式明文安装目的密钥(密钥类型TAK,索引号为4,长度为16)
		{(uchar *)"\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55",								  NULL},    			//22  case2.3  以SEC_KCV_ZERO方式明文安装目的密钥(密钥类型TPK,索引号为3,长度为16)
		{(uchar *)"\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71\x71",								 NULL},   			//23  case2.4  (异常)以SEC_KCV_NONE方式明文安装目的密钥(密钥type=100,索引号为3,长度为16)
		{(uchar *)"\x73\x73\x73\x73\x73\x73\x73\x73\x73\x73\x73\x73\x73\x73\x73\x73",								 NULL},    			//24  case2.5  (异常)以SEC_KCV_NONE方式明文安装目的密钥(密钥类型TDK,索引号为9,长度为16)
		{(uchar *)"\x75",								 													NULL},   			//25  case2.6  (异常)仅key的奇偶校验位不同,应视为同一个key,以SEC_KCV_NONE方式明文安装目的密钥(密钥类型TDK,索引号为9,长度为16)
		{(uchar *)"\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16",		 NULL},   			//26  case1.121 新增安装32位tmk密钥 用于case1.26安装密钥由tmk发散
		{(uchar *)"\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x16\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15\x15",		 NULL}, 			//27  case1.26 新增测试安装32位密钥 
	};
	char KeyValue[32] = {0};
	uchar ch = 0x00;

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

	//测试前置
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//case1:MK/SK体系 	***安装AES密钥需按照扩展方式进行安装方式调用***
	//case1.1:异常测试:以SEC_KCV_NONE方式明文安装目的密钥(密钥类型TLK,密钥算法aes,索引号为2),由于TLK索引ID只能为1,应返回失败
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[0].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;	
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case1.2(新增) 异常测试:使用DES的安装方式进行安装会返回NDK_ERR_SECP_PARAM
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[1].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case1.2:正常测试:以SEC_KCV_NONE、密钥算法AES方式明文安装TLK(密钥类型TLK,索引号为1)
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[1].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//不允许使用AES算法密钥安装DES算法密钥
	//case1.3:异常测试:源密钥(密钥类型TLK,密钥算法AES,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,密钥算法DES),应该返回失败
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_DES;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[2].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if SEC_SM4_ENABLE   //有支持国密的才验证
	//不允许使用AES算法密钥安装SM4算法密钥
	//case1.4:异常测试:源密钥(密钥类型TLK,密钥算法AES,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,密钥算法SM4),应该返回失败
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[3].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif	
	
	//DES算法密钥体系与AES算法密钥体系相互独立，各自拥有独立的存储空间。
	//case1.5:异常测试:源密钥(密钥类型TLK,密钥算法DES,索引号为1)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TMK,密钥算法DES,索引号为2),由于DES未安装TLK，应返回失败
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_DES;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_DES;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[4].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	
#if !defined N910P //N910P不具备此功能
	if(flag_sdk3 && !g_seckeyowner)//sdk3.0且在共享密钥区时清安全和清密钥后默认安装TLK，sdk2.0不安装  20180724  modify
	{
		if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	else
	{
		if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}		

#else
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case1.6:正常测试:以SEC_KCV_NONE、密钥算法DES方式明文安装TLK(密钥类型TLK,索引号为1),安装DES的TLK应该成功
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_DES;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[5].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if SEC_SM4_ENABLE   //有支持国密的才验证
	//SM4算法密钥体系与AES算法密钥体系相互独立，各自拥有独立的存储空间。
	//case1.7:异常测试:源密钥(密钥类型TLK,密钥算法SM4,索引号为1)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TMK,密钥算法SM4,索引号为2),由于SM4未安装TLK，应返回失败
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[6].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.8:正常测试:以SEC_KCV_NONE、密钥算法SM4方式明文安装TLK(密钥类型TLK,索引号为1),安装SM4的TLK应该成功
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[7].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//不允许使用DES算法密钥安装AES算法密钥
	//case1.9:异常测试:源密钥(密钥类型TLK,密钥算法DES,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,密钥算法AES),应该返回失败
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_DES;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[8].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if SEC_SM4_ENABLE   //有支持国密的才验证
	//不允许使用SM4算法密钥安装AES算法密钥
	//case1.10:异常测试:源密钥(密钥类型TLK,密钥算法SM4,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,密钥算法AES),应该返回失败
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[9].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case1.26:TMK索引不在范围内
	//范围已调整为0-255,故此测试点无效,暂不实现

	//case1.11:正常测试:源密钥(密钥类型TLK,密钥算法AES,索引号为1)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TMK,索引号为2)
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[7].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[7].pkcv, stKcvInfoIn.nLen);
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//允许以不同密钥值重复安装到ID、类型、用途都不变的密钥存储空间上（允许不同值覆盖安装）
	//case1.12:正常测试:以SEC_KCV_ZERO方式明文安装目的密钥(密钥类型TMK,索引号为2)(覆盖安装)
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 24;
	stExtendKey.pblock = (char*)SecLoad[8].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[8].pkcv, stKcvInfoIn.nLen);
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.121 安装32位TMK密钥(由于老产品设置*后是存放在k21端,k21不能安装32位密钥,而安卓端可以安装,此处安装是为了后面安装32位的tak)
#if defined N900AZ||defined N910AZ ||defined IM81AZ
	if(g_seckeyowner)
#endif
	{
		memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
		memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=9;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 32;
		stExtendKey.pblock = (char*)SecLoad[26].pkey;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}	

	//case1.13:源密钥(密钥类型TLK,密钥算法DES,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,密钥算法DES,索引号为2)应该成功
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_DES;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_DES;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[5].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.14:异常测试:源密钥(密钥类型TMK,密钥算法AES,索引号为4)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TAK,索引号为3),由于ID=4没任何密钥，应返回失败
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=4;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[9].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.15:异常测试:源密钥(密钥type=100,密钥算法AES,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TAK,索引号为3),由于type参数错误，应返回失败
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=100;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[10].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.16:正常测试:源密钥(密钥类型TMK,密钥算法AES,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TAK,索引号为3)
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[11].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[11].pkcv, stKcvInfoIn.nLen);
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//在一个存在层次结构的密钥体系（主要指MKSK）中，低级的密钥不应用于安装同级的或比它高级的密钥
	//case1.17:异常测试:源密钥(密钥类型TAK,密钥算法AES,索引号为3)以SEC_KCV_NONE方式安装目的密钥(密钥类型TPK,索引号为4),应返回失败
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=3;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[12].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.18:异常测试:源密钥(密钥类型TAK,密钥算法AES,索引号为3)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,索引号为4),应返回失败
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=3;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[13].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.19:正常测试:源密钥(密钥类型TMK,密钥算法AES,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TPK,索引号为4)
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[14].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[14].pkcv, stKcvInfoIn.nLen);
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//允许以同一个密钥值重复安装到ID、类型、用途都不变的密钥存储空间上（允许同值重复安装）
	//case1.20:正常测试:源密钥(密钥类型TMK,密钥算法AES,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TPK,索引号为4)
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[14].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	
	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[14].pkcv, stKcvInfoIn.nLen);
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case1.21:异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TDK,密钥算法AES,索引号为5,长度为8),应返回失败,AES密钥长度只能为16
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 8;
	stExtendKey.pblock = (char*)SecLoad[15].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.22:正常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TDK,索引号为5,长度为24),应返回成功
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 24;
	stExtendKey.pblock = (char*)SecLoad[16].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.26:新增正常测试:源密钥(密钥类型TMK,索引号为9)以SEC_KCV_NONE方式安装目的密钥(密钥类型TDK,索引号为13,长度为32),应返回成功
#if defined N900AZ||defined N910AZ ||defined IM81AZ
	if(g_seckeyowner)
#endif
	{
		memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
		memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=9;
		stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
		stKeyInfo.ucDstKeyIdx=13;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 32;
		stExtendKey.pblock = (char*)SecLoad[27].pkey;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
		
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	
	//case1.23:正常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TDK,索引号为6,长度为16)
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=6;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock =(char*)SecLoad[17].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//AES暂时不支持TR 31格式安装

	//case1.24:异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥type=100,索引号为7),由于type参数错误，应返回失败
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=7;
	stKeyInfo.ucDstKeyType = 100;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[18].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//不允许以同一个密钥值重复安装到不同ID或类型或用途的密钥存储空间上（尤其对于同一密钥体系的，必须满足“各类密钥的值不能相同"):
	//case1.25:异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TDK,索引号为7)应成功,再安装目的密钥(密钥类型TAK,索引号为8),应返回失败
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
	stKeyInfo.ucDstKeyIdx=7;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[19].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[19].pkcv, stKcvInfoIn.nLen);
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	stKeyInfo.ucDstKeyIdx=8;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stExtendKey.pblock = (char*)SecLoad[19].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
#if ANDIROD_ENABLE&!defined N910P  //跟开发确认安卓平台允许
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)	/*还未执行*/
#else
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_INSTALL_REC)	/*应失败*/
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:FIXED体系:是一级密钥，没有tmk 直接安装 pin/mac/data key
	//case2.1:正常测试:以SEC_KCV_NONE方式明文安装目的密钥(密钥类型TAK,索引号为4,长度为sizeof(ST_EXTEND_KEYBLOCK))
#if defined N900AZ||defined N910AZ ||defined IM81AZ
	if(g_seckeyowner)
#endif
	{
		memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
		memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=4;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 32;
		stExtendKey.pblock = (char*)SecLoad[20].pkey;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	//case2.2:正常测试:以SEC_KCV_ZERO方式明文安装目的密钥(密钥类型TAK,索引号为5,长度为16)
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[21].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[21].pkcv, stKcvInfoIn.nLen);
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.3:正常测试:以SEC_KCV_NONE方式明文安装目的密钥(密钥类型TPK,索引号为3,长度为24)
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 24;
	stExtendKey.pblock = (char*)SecLoad[22].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
		
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	stKcvInfoIn.nLen=0;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.4:异常测试:以SEC_KCV_NONE方式明文安装目的密钥(密钥type=100,索引号为3,长度为16),由于type参数错误，应返回失败
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType = 100;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[23].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); 
		GOTOERR;
	}

	//case2.5:异常测试:以SEC_KCV_NONE方式明文安装目的密钥(密钥类型TDK,索引号为9,长度为16),同一组密钥安装到不同的ID上,ID9应该成功,ID10应该失败
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=9;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	stExtendKey.pblock = (char*)SecLoad[24].pkey;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	stKeyInfo.ucDstKeyIdx=10;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_AES;
#if ANDIROD_ENABLE&!defined N910P//defined IM81||defined N900||
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)	/*还未执行*/
#else
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_INSTALL_REC)	/*应失败*/
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.6:异常测试:仅key的奇偶校验位不同,应视为同一个key,以SEC_KCV_NONE方式明文安装目的密钥(密钥类型TDK,索引号为11,长度为16),再安装目的密钥(密钥类型TAK,索引号为12),应返回安装密钥记录错误
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
	memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=11;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
	stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
	stExtendKey.len = 16;
	memset(KeyValue,*SecLoad[25].pkey,stExtendKey.len);
	stExtendKey.pblock = KeyValue;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	ch = (*(SecLoad[25].pkey)&0xfe)|(*(SecLoad[25].pkey)&0x01?0x00:0x01);
	memset(KeyValue, ch, stExtendKey.len);
	stExtendKey.pblock = KeyValue;
	memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
	stKeyInfo.ucDstKeyIdx=12;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_AES;
#if ANDIROD_ENABLE
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)/*还未实际执行*/
#else
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_INSTALL_REC )/*应失败*/
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//DUKPT 在NDK_SecLoadTIK用例中测试
	send_result("%s测试通过", TESTAPI);
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}

