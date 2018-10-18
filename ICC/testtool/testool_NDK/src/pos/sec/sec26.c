/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec26.c
* Author 		: zhengry	
* version		: 
* DATE			: 20160107
* directory 		: 
* description		: 测试NDK_SecLoadKey国密部分密钥装载接口功能
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
*			  	  	      zhengry   20160108	created
*****************************************************************/
void sec26(void)
{
	int ret=0;
	ST_SEC_KEY_INFO stKeyInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;
#if	TR31_ENABLE
	tr31_key_t tr31_st;
	unsigned int len = 0;
	ST_EXTEND_KEYBLOCK* p;
	uchar block[512]={0},mkey[17]={0};
#endif
	KEY_KCV SecLoad[ ]={
		{(uchar *)"\x11",															   NULL},						// 0  case1.1  
		{(uchar *)"\x13",															   NULL},						// 1  case1.5  明文安装TLK(算法为DES),明文为16个字节的0x13 
		{(uchar *)"\x15",															   NULL},						// 2  case1.3  
		{(uchar *)"\x1B",															   NULL},	                    // 3  case1.4  
		{(uchar *)"\x19",															   NULL},                       // 4  case1.2  明文安装TLK,明文为16个字节的0x19
		{(uchar *)"\xE1\x46\x47\xE8\xA1\x35\x06\x1A\xE1\x46\x47\xE8\xA1\x35\x06\x1A", (uchar *)"\x3A\xDE\xBB\xE0"},	// 5  case1.9  SEC_KCV_ZERO安装TMK(算法为DES),由TLK发散,明文为16个字节的0x17
		{(uchar *)"\x1D",															   NULL},						// 6  case1.6
		{(uchar *)"\x51\xE9\x6A\x1A\x86\x9F\x3B\x7C\x9B\xD3\x3F\x6C\x34\x4C\x76\x32", (uchar *)"\x59\x4F\x26\x2E"},	// 7  case1.7  SEC_KCV_ZERO安装TMK(由TLK发散),明文为16个字节的0X1F  
		{(uchar *)"\x21",															  (uchar *)"\x6C\xD7\x6C\x59"},	// 8  case1.8  以SEC_KCV_ZERO方式明文安装TMK,明文为16个字节的0X21 
		{(uchar *)"\x23",															   NULL},						// 9  case1.10 (异常)SEC_KCV_NONE安装TAK,id=3(由TLK,id=4发散),明文为16个字节的0X23    
		{(uchar *)"\x25",															   NULL},						//10  case1.11 (异常)SEC_KCV_NONE安装TAK,id=3(由TYPE=100发散),明文为16个字节的0X25  
		{(uchar *)"\xC0\xD3\xEB\x69\xB1\x06\x47\xEF\x76\xD2\x49\x62\x75\xA4\x8B\xFB",(uchar *)"\x93\x38\x75\xD7"},	//11  case1.12 SEC_KCV_ZERO安装TAK(由TMK发散),明文为16个字节的0X27 
		{(uchar *)"\x29",															   NULL},						//12  case1.13 (异常)SEC_KCV_NONE安装TPK(由TAK发散),明文为16个字节的0X29 
		{(uchar *)"\x2A",															   NULL},						//13  case1.14 (异常)SEC_KCV_NONE安装TMK(由TAK发散),明文为16个字节的0X2B 
		{(uchar *)"\x44\x4E\x8D\x2A\x2A\xAA\x91\xB4\xAD\x47\x94\x78\xE6\x7D\x71\x75",(uchar *)"\x2E\x36\x8F\x3C"},  //14  case1.15和case1.16  SEC_KCV_ZERO安装TPK(由TMK发散),明文为16个字节的0X2D 
		{(uchar *)"\x2F",															   NULL},						//15  case1.17 (异常)SEC_KCV_NONE安装TDK(由TMK发散,长度为8),明文0X2F 
		{(uchar *)"\x31",															   NULL},						//16  case1.18 (异常)SEC_KCV_NONE安装TPK(由TMK发散,长度为24),明文0X31
		{(uchar *)"\x33",															   NULL},						//17  case1.19  SEC_KCV_NONE安装TDK(由TMK发散),明文为16个字节的0X33 
		{(uchar *)"\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39",(uchar *)"\xA1\xE5\xF0\xD6"},  //18  case1.20  (异常)以TR31打包和SEC_KCV_ZERO方式安装TDK(由TMK发散),明文为16个字节的0X39 
		{(uchar *)"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41",(uchar *)"\x42\x65\x95\x98"},  //19  case1.21  (异常)以TR31打包和SEC_KCV_ZERO方式安装TDK(由TMK发散),明文为16个字节的0X41
		{(uchar *)"\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43",(uchar *)"\x04\x97\x88\x04"},  //20  case1.22  (异常)以TR31打包和SEC_KCV_ZERO方式安装TDK(由TMK发散),明文为16个字节的0X43 
		{(uchar *)"\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45\x45",(uchar *)"\xE4\x80\x14\x7D"},  //21  case1.23  以TR31打包和SEC_KCV_ZERO方式安装TDK(由TMK发散),明文为16个字节的0X45 
		{(uchar *)"\x47",															   NULL},						//22  case1.24  (异常)SEC_KCV_NONE安装type=100(由TMK发散),明文0X47
		{(uchar *)"\xB7\xCE\x64\x11\xD8\xF0\xFA\xED\x20\xFD\xF1\x58\x9C\x5C\xD2\x1E",(uchar *)"\xED\xA3\x96\x39"},	//23  case1.25  (异常)SEC_KCV_ZERO安装TDK(由TMK发散),明文0X49
		{(uchar *)"\x51",															   NULL},						//24  case2.1	(异常)明文安装TAK,明文为16个字节的0X51,长度为sizeof(ST_EXTEND_KEYBLOCK)) 
		{(uchar *)"\x53",															 (uchar *)"\xE5\x24\x34\x5A"},	//25  case2.2   SEC_KCV_ZERO明文安装TAK,明文为16个字节的0X53 
		{(uchar *)"\x55",															 (uchar *)"\x25\xF5\x7A\xEB"},	//26  case2.3   SEC_KCV_ZERO明文安装TDK,明文为16个字节的0X55 
		{(uchar *)"\x57",															 (uchar *)"\xFE\xBE\x78\x43"},	//27  case2.4   SEC_KCV_ZERO明文安装TDK,明文为16个字节的0X57
		{(uchar *)"\x61",															   NULL},						//28  case2.5   (异常)SEC_KCV_NONE明文安装type=100,明文0X61
		{(uchar *)"\x63",															   NULL},						//29  case2.6   (异常)SEC_KCV_NONE明文安装TAK,明文0X63
		{(uchar *)"\x65",															   NULL},						//30  case2.7   (异常)SEC_KCV_NONE明文安装TAK,明文0X65
		{(uchar *)"\x12\x41\x41\x41\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39\x39",		   NULL},				//31 新增case1.26
	};
	uchar ch = 0x00;

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI); 

	//测试前置
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//case1:MK/SK体系
	//case1.1:异常测试:以SEC_KCV_NONE方式明文安装目的密钥(密钥类型TLK,密钥算法sm4,索引号为2),由于TLK索引ID只能为1,应返回失败
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[0].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case1.2:正常测试:以SEC_KCV_NONE、密钥算法SM4方式明文安装TLK(密钥类型TLK,索引号为1)
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[4].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//不允许使用SM4算法密钥安装DES算法密钥
	//case1.3:异常测试:源密钥(密钥类型TLK,密钥算法SM4,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,密钥算法DES),应该返回失败
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_DES;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[2].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//DES算法密钥体系与SM4算法密钥体系相互独立，各自拥有独立的存储空间。
	//case1.4:异常测试:源密钥(密钥类型TLK,密钥算法DES,索引号为1)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TMK,密钥算法DES,索引号为2),由于DES未安装TLK，应返回失败。
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_DES;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_DES;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[3].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;

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

	//case1.5:正常测试:以SEC_KCV_NONE、密钥算法DES方式明文安装TLK(密钥类型TLK,索引号为1),安装DES的TLK应该成功
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_DES;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[1].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//不允许使用DES算法密钥安装SM4算法密钥
	//case1.6:异常测试:源密钥(密钥类型TLK,密钥算法DES,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,密钥算法sm4),应该返回失败
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_DES;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[6].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.26:TMK索引不在范围内
	//范围已调整为0-255,故此测试点无效,暂不实现

	//case1.7:正常测试:源密钥(密钥类型TLK,索引号为1)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TMK,索引号为2)
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[7].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[7].pkcv, stKcvInfoIn.nLen);
	
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//允许以不同密钥值重复安装到ID、类型、用途都不变的密钥存储空间上（允许不同值覆盖安装）
	//case1.8:正常测试:以SEC_KCV_ZERO方式明文安装目的密钥(密钥类型TMK,索引号为2)(覆盖安装)
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[8].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[8].pkcv, stKcvInfoIn.nLen);
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.9:源密钥(密钥类型TLK,密钥算法DES,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,密钥算法DES,索引号为2)应该成功
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_DES;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_DES;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[5].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[5].pkcv, stKcvInfoIn.nLen);
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.10:异常测试:源密钥(密钥类型TMK,索引号为4)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TAK,索引号为3),由于ID=4没任何密钥，应返回失败。
	stKeyInfo.ucScrKeyIdx=4;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[9].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.11:异常测试:源密钥(密钥type=100,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TAK,索引号为3),由于type参数错误，应返回失败
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=100;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[10].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.12:正常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TAK,索引号为3)
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[11].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[11].pkcv, stKcvInfoIn.nLen);

	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//在一个存在层次结构的密钥体系（主要指MKSK）中，低级的密钥不应用于安装同级的或比它高级的密钥
	//case1.13:异常测试:源密钥(密钥类型TAK,索引号为3)以SEC_KCV_NONE方式安装目的密钥(密钥类型TPK,索引号为4),应返回失败
	stKeyInfo.ucScrKeyIdx=3;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[12].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case1.14:异常测试:源密钥(密钥类型TAK,索引号为3)以SEC_KCV_NONE方式安装目的密钥(密钥类型TMK,索引号为4),应返回失败
	stKeyInfo.ucScrKeyIdx=3;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[13].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.15:正常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TPK,索引号为4)
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[14].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[14].pkcv, stKcvInfoIn.nLen);

	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//允许以同一个密钥值重复安装到ID、类型、用途都不变的密钥存储空间上（允许同值重复安装）
	//case1.16:正常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TPK,索引号为4)
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[14].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[14].pkcv, stKcvInfoIn.nLen);

	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

 	//case1.17:异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TDK,索引号为5,长度为8),应返回失败,SM4密钥长度只能为16
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=8;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[15].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
 	//case1.18:异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TDK,索引号为5,长度为24),应返回失败
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=24;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[16].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
 	//case1.19:正常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥类型TDK,索引号为5,长度为16)
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[17].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if TR31_ENABLE	//支持TR31格式密钥安装的设备才进行TR31测试
	//case1.20:异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TDK,索引号为5,长度为sizeof(ST_EXTEND_KEYBLOCK)),ST_SEC_KEY_INFO.len不对
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, *SecLoad[8].pkey, 16);//对应于TMK 明文key SecLoad[8].pkey

	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = SecLoad[18].pkey;
	tr31_st.keylen = 16;
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	p=(ST_EXTEND_KEYBLOCK *)(stKeyInfo.sDstKeyValue);
	p->format=SEC_KEYBLOCK_FMT_TR31;
	p->len=1;
	p->pblock=(char *)block;
	
	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[18].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_PARAM)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.21:异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TDK,索引号为5,长度为sizeof(ST_EXTEND_KEYBLOCK)),ST_SEC_KEY_INFO.block不对
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, *SecLoad[8].pkey, 16);//对应于TMK 明文key SecLoad[8].pkey

	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = SecLoad[19].pkey;
	tr31_st.keylen = 16;
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	p=(ST_EXTEND_KEYBLOCK *)(stKeyInfo.sDstKeyValue);
	p->format=SEC_KEYBLOCK_FMT_TR31;
	p->len=len;
	memset(p->pblock, 0x00, len);
	
	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[19].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_PARAM)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.22:异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TDK,索引号为5,长度为sizeof(ST_EXTEND_KEYBLOCK)),ST_SEC_KEY_INFO.format不对
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, *SecLoad[8].pkey, 16);//对应于TMK 明文key SecLoad[8].pkey

	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = SecLoad[20].pkey;
	tr31_st.keylen = 16;
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	p=(ST_EXTEND_KEYBLOCK *)(stKeyInfo.sDstKeyValue);
	p->format=SEC_KEYBLOCK_FMT_TR31+1;
	p->len=len;
	memset(p->pblock, 0x00, len);
	
	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[20].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_PARA)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.23:正常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TDK,索引号为5,长度为sizeof(ST_EXTEND_KEYBLOCK))
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, *SecLoad[8].pkey, 16);//对应于TMK 明文key SecLoad[8].pkey

	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = SecLoad[21].pkey;
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
	
	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[21].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if 1//ANDIROD_ENABLE  //目前是安卓平台支持 后续其他平台支持可开放 20180328 add  //所有平台产品都应该支持
	//case1.26:新增用DES算法的TLK以TR31格式下发安装SM4算法的工作密钥
	//源密钥(密钥类型TLK,索引号为1)以SEC_KCV_NONE方式安装目的密钥(密钥类型TDK,索引号为8,长度为sizeof(ST_EXTEND_KEYBLOCK))
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_DES;
	stKeyInfo.ucDstKeyIdx=8;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, *SecLoad[1].pkey, 16);//对应于TLK 明文key SecLoad[1].pkey
	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = SecLoad[31].pkey;
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
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
#endif

	//case1.24:异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_NONE方式安装目的密钥(密钥type=100,索引号为6),由于type参数错误，应返回失败
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=6;
	stKeyInfo.ucDstKeyType=100;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[22].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//不允许以同一个密钥值重复安装到不同ID或类型或用途的密钥存储空间上（尤其对于同一密钥体系的，必须满足“各类密钥的值不能相同"):
	//case1.25:异常测试:源密钥(密钥类型TMK,索引号为2)以SEC_KCV_ZERO方式安装目的密钥(密钥类型TDK,索引号为7)应成功,再安装目的密钥(密钥类型TAK,索引号为8),应返回失败
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=7;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[23].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[23].pkcv, stKcvInfoIn.nLen);

	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	stKeyInfo.ucDstKeyIdx=8;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
#if ANDIROD_ENABLE//defined IM81||defined N900||
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)	/*还未执行*/
#else
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_INSTALL_REC)	/*应失败*/
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:FIXED体系
	//case2.1:异常测试:以SEC_KCV_NONE方式明文安装目的密钥(密钥类型TAK,索引号为4,长度为sizeof(ST_EXTEND_KEYBLOCK)),由于len=sizeof(ST_EXTEND_KEYBLOCK) 属于MK/SK体系,故应报错
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(stKeyInfo.sDstKeyValue, *SecLoad[24].pkey, 16);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_PARA) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

 	//case2.2:正常测试:以SEC_KCV_ZERO方式明文安装目的密钥(密钥类型TAK,索引号为4,长度为16)
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[25].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[25].pkcv, stKcvInfoIn.nLen);

	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
 	//case2.3:正常测试:以SEC_KCV_ZERO方式明文安装目的密钥(密钥类型TDK,索引号为5,长度为16)
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[26].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[26].pkcv, stKcvInfoIn.nLen);

	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
 	//case2.4:正常测试:以SEC_KCV_ZERO方式明文安装目的密钥(密钥类型TPK,索引号为3,长度为16)
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[27].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[27].pkcv, stKcvInfoIn.nLen);

	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
 	//case2.5:异常测试:以SEC_KCV_NONE方式明文安装目的密钥(密钥type=100,索引号为3,长度为16),由于type参数错误，应返回失败
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=100;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[28].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); 
		GOTOERR;
	}

 	//case2.6:异常测试:以SEC_KCV_NONE方式明文安装目的密钥(密钥类型TDK,索引号为9,长度为16),同一组密钥安装到不同的ID上,ID9应该成功,ID10应该失败
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=9;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[29].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	stKeyInfo.ucDstKeyIdx=10;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
#if ANDIROD_ENABLE//defined IM81||defined N900||
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)	/*还未执行*/
#else
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_INSTALL_REC)	/*应失败*/
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
 	//case2.7:异常测试:仅key的奇偶校验位不同,应视为同一个key,以SEC_KCV_NONE方式明文安装目的密钥(密钥类型TDK,索引号为9,长度为16),再安装目的密钥(密钥类型TAK,索引号为10),应返回安装密钥记录错误
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=9;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[30].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	ch = (*(SecLoad[30].pkey)&0xfe)|(*(SecLoad[30].pkey)&0x01?0x00:0x01);
	memset(stKeyInfo.sDstKeyValue, ch/**(SecLoad[30].pkey)-1*/, stKeyInfo.nDstKeyLen);
	stKeyInfo.ucDstKeyIdx=10;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
#if ANDIROD_ENABLE//defined IM81||defined N900||
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)/*还未实际执行*/
#else
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_INSTALL_REC )/*应失败*/
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//在NDK_SecLoadTIK用例中测试
	send_result("%s测试通过", TESTAPI);
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}
