/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec3.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecLoadKey接口功能
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
typedef struct key_kcv {
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
* history 		 		:  author		date		remarks
*			  	  	   linrq       20130228	created
*****************************************************************/
void sec3(void)
{
	int ret = -1, ret1 = -1;
	ST_SEC_KEY_INFO stKeyInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;
#if TR31_ENABLE
	ST_EXTEND_KEYBLOCK* p;
	unsigned char block[512]={0};
	unsigned char mkey[17]={0};
	tr31_key_t tr31_st;
	unsigned int len = 0;
#endif	
	KEY_KCV SecLoad[ ]={
		{(uchar *)"\x11",																							NULL}, 					// 0		//case2.1
		{(uchar *)"\x13", 																							NULL}, 					// 1		//case2.2	(key=16个0x13, TLK,ID=1)
		{(uchar *)"\xE1\x46\x47\xE8\xA1\x35\x06\x1A\xE1\x46\x47\xE8\xA1\x35\x06\x1A", 								(uchar *)"\x3A\xDE\xBB\xE0"}, 		// 2		//case2.4  	SEC_KCV_ZERO方式(TMK id=2, 密文由TLK发散,明文KEY=16个0x17)
		{(uchar *)"\x15", 																							(uchar *)"\x26\x5A\x7A\xBF"},		// 3		//case2.20	SEC_KCV_ZERO方式
		{(uchar *)"\xA5\x94\x96\x77\x49\x26\x1C\xDF\xA5\x94\x96\x77\x49\x26\x1C\xDF", 								NULL},					// 4		//case2.5:以SEC_KCV_NONE方式去安装TAK,明文key 0x19(由ID=4,type=TMK发散),ID=3
		{(uchar *)"\x1b", 																							(uchar *)"\x4D\x01\xE5\x75"},		// 5		//case2.6 		SEC_KCV_VAL方式
		{(uchar *)"\xA6\x9F\xC5\x1F\x50\x3C\xED\xE6\xA6\x9F\xC5\x1F\x50\x3C\xED\xE6", 								(uchar *)"\x45\xFA\x42\xA2"},		// 6		//case2.7		SEC_KCV_VAL方式(TMK发散,key明文16个0x1d,失败)
		{(uchar *)"\x2E\x38\x1D\x92\x01\x88\x58\x22\x2E\x38\x1D\x92\x01\x88\x58\x22", 								(uchar *)"\x54\x8C\x54\xFE"},		// 7		//case2.8/case2.22		SEC_KCV_VAL方式(TMK发散,key明文16个0x1f   安装TAK，ID=3)
		{(uchar *)"\x97\x70\xAF\x26\xA8\x22\xA2\xE3\x97\x70\xAF\x26\xA8\x22\xA2\xE3", 								(uchar *)"\x70\x8D\xE9\xF7"},		// 8		//case2.9		SEC_KCV_VAL方式(安装TPK,由TAK 发散,key明文16个字节0x21)
		{(uchar *)"\xF9\x1B\xFD\x82\x5F\x5F\x7B\x41\xF9\x1B\xFD\x82\x5F\x5F\x7B\x41", 								(uchar *)"\x55\x30\x7D\xF8"},		// 9		//case2.10	SEC_KCV_VAL方式(安装TMK,由TAK发散，key明文16个0x23 )
		{(uchar *)"\xFF\x4C\xAF\xA1\x33\x09\x76\xEC\xFF\x4C\xAF\xA1\x33\x09\x76\xEC",								(uchar *)"\xCB\xF7\x20\x63"}, 		// 10	//case2.11	SEC_KCV_VAL方式(安装TPK(由ID=2,type=TMK发散) ID=4  ,key明文16个0x25)
		{(uchar *)"\xDC\xEE\xF5\xC2\x06\x5E\x27\x5E", 																(uchar *)"\x0F\x35\x9E\x84"},		// 11	//case2.12	SEC_KCV_VAL方式(安装TDK(由ID=2,type=TMK发散),len=20(非法)  失败  KEY 0x27)
		{(uchar *)"\x8A\xEE\xE9\x1A\x8C\x2F\x71\x97", 																(uchar *)"\xA4\x70\xF9\x9E"},		// 12	//case2.13	SEC_KCV_VAL方式key:8位(安装TDK(由ID=2,type=TMK发散),len=8,ID=5  , key明文8个0x29)
		{(uchar *)"\x79\x3C\xA1\xEA\x4F\xC8\x5C\xFB\x79\x3C\xA1\xEA\x4F\xC8\x5C\xFB", 								(uchar *)"\x16\x55\x7C\x70"},		// 13	//case2.14	SEC_KCV_VAL方式(安装TDK(由ID=2,type=TMK发散),len=16,ID=5  , key明文16个0x2b)
		{(uchar *)"\xDE\xD9\x76\x77\x94\xFC\xD9\x05\xDE\xD9\x76\x77\x94\xFC\xD9\x05\xDE\xD9\x76\x77\x94\xFC\xD9\x05",(uchar *)"\xDD\xA4\x94\xE6"},		// 14	//case2.15	SEC_KCV_VAL方式key:24位(安装TDK(由ID=2,type=TMK发散),len=24,ID=5  , key明文24个0x2d)
		{(uchar *)"\x7F\x9D\x4F\x7D\x2F\x17\x34\x4D\x7F\x9D\x4F\x7D\x2F\x17\x34\x4D", 								(uchar *)"\xBE\xBA\xC0\x29"},		// 15	//case2.19	SEC_KCV_VAL方式安装type=100(由ID=2,type=TMK发散,明文key=16个0x2f)
		{(uchar *)"\x47\x7C\xD6\x84\x20\x43\xFE\xC0\x47\x7C\xD6\x84\x20\x43\xFE\xC0",								NULL},    					// 16	//case2.21	SEC_KCV_NONE(安装TDK(id=7), TAK(id=8)(由TMK发散)   key明文16个0x31)
		{(uchar *)"\x33", 																							NULL},					// 17	//case3.2:	SEC_KCV_NONE(安装TAK(明文),ID=4,len=8)
		{(uchar *)"\x35", 																							(uchar *)"\xD2\xDB\x51\xF1"}, 		// 18	//case3.3:       SEC_KCV_ZERO(安装TDK(明文),ID=5,len=16)
		{(uchar *)"\x37",																							(uchar *)"\x52\xA1\xC0\x88"},		// 19	//case3.4:   	SEC_KCV_VAL(TPK(明文),ID=3,len=24)
		{(uchar *)"\x39",																							NULL},					// 20	//case3.5:   	非法方式(100) 安装TPK(明文),ID=3,len=24
		{(uchar *)"\x3b", 																							NULL},					// 21	//case3.6:	SEC_KCV_NONE(安装TDK(id=9), TAK(id=10)(明文))
		{(uchar *)"\x3d",																							NULL},					// 22	//case3.7		SEC_KCV_NONE(明文)
		{(uchar *)"\x3f\x3f\x3f\x3f\x3f\x3f\x3f\x3f\x3f\x3f\x3f\x3f\x3f\x3f\x3f\x3f", 								(uchar *)"\xCE\xE8\xAC\x95"},		// 23	//case2.16:以SEC_KCV_VAL方式去安装TDK(由ID=2,type=TMK发散)
		{(uchar *)"\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41",								(uchar *)"\x49\x17\xA0\xA3"}, 		//24		//case2.17:安装TDK(由ID=2,type=TMK发散),len=sizeof(ST_EXTEND_KEYBLOCK) ,ST_SEC_KEY_INFO.len=1,ID=5
		{(uchar *)"\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43\x43", 								(uchar *)"\xB3\xE4\x23\x06"},		//25		//case2.18:以SEC_KCV_VAL方式去安装TDK(由ID=2,type=TMK发散),len=sizeof(ST_EXTEND_KEYBLOCK) ,ID=5
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
			
	//case1: 传入非法参数
	if(((ret = NDK_SecLoadKey(NULL, &stKcvInfoIn)) != NDK_ERR_PARA)
	||((ret1= NDK_SecLoadKey(&stKeyInfo, NULL)) != NDK_ERR_PARA))
	{
		send_result("line %d:%s测试失败(%d, %d)", __LINE__, TESTAPI, ret, ret1);
		RETURN;
	}

	//约定:若测试设计无特殊说明,则都安装16字节长key
	//case2:MK/SK体系
	//case2.1:TLK索引不在范围内id=2 (索引号自1至1)
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[0].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;

	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2.2:以SEC_KCV_NONE方式安装TLK,ID=1
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[1].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2.3:TMK索引不在范围内
	//范围已调整为0-255,故此测试点无效,暂不实现
		
	//case2.4:以SEC_KCV_ZERO方式安装TMK(密文),ID=2
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[2].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[2].pkcv, stKcvInfoIn.nLen);	

	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2.20:以SEC_KCV_ZERO方式安装TMK(明文),ID=2
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[3].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[3].pkcv, stKcvInfoIn.nLen);

	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.5:以SEC_KCV_NONE方式去安装TAK(由ID=4,type=TMK发散),ID=3
	stKeyInfo.ucScrKeyIdx=4;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[4].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_READ_REC) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if 0 //ID=101已合法,不测试
	//case2.6:以SEC_KCV_VAL 方式去安装TAK(由ID=101,type=TMK发散),ID=3
	stKeyInfo.ucScrKeyIdx=101;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[5].uckey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[5].loadkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != 100)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		//GOTOERR;
	}
#endif	

	//case2.7:以SEC_KCV_VAL 方式去安装TAK(由ID=2,type=100发散),ID=3
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=100;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[6].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[6].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_PARAM)	  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.8:以SEC_KCV_VAL方式去安装TAK(由ID=2,type=TMK发散),ID=3
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[7].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[7].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.22:以SEC_KCV_VAL方式去安装TAK(由ID=2,type=TMK发散),ID=3(重复case2.8,以验证覆盖安装,并且密钥值一样.UPDATE方式)
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[7].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[7].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.9:以SEC_KCV_VAL方式去安装TPK(由ID=3,type=TAK发散),ID=4
	stKeyInfo.ucScrKeyIdx=3;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TAK;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[8].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[8].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) !=NDK_ERR_SECP_PARAM)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2.10:以SEC_KCV_VAL方式去安装TMK(由ID=3,type=TAK发散),ID=4
	stKeyInfo.ucScrKeyIdx=3;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TAK;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[9].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[9].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_PARAM)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2.11:以SEC_KCV_VAL方式去安装TPK(由ID=2,type=TMK发散),ID=4
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[10].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[10].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.12:以SEC_KCV_VAL方式去安装TDK(由ID=2,type=TMK发散),len=6(非法),ID=5
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=6;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[11].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[11].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.13:以SEC_KCV_VAL方式去安装TDK(由ID=2,type=TMK发散),len=8,ID=5
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=8;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[12].pkey, stKeyInfo.nDstKeyLen);
	
	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[12].pkcv, stKcvInfoIn.nLen);
#if OVERSEAS_ENABLE	//海外版本不支持8字节密钥,所以要安装失败
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_PARAM)
#elif defined ME30S ||defined N910P//ME30s返回参数非法-6
    if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_PARA)
#else
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.14:以SEC_KCV_VAL方式去安装TDK(由ID=2,type=TMK发散),len=16,ID=5
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[13].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[13].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.15:以SEC_KCV_VAL方式去安装TDK(由ID=2,type=TMK发散),len=24,ID=5
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=24;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[14].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[14].pkcv, stKcvInfoIn.nLen);
#if !(OVERSEAS_ENABLE||defined ME30S||defined N910P)	//海外版本固件不支持低长度密钥安装高长度密钥
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
#else
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_KEY_LEN)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if TR31_ENABLE	//支持TR31格式密钥安装的设备才进行TR31测试
	//case2.16:以SEC_KCV_VAL方式去安装TDK(由ID=2,type=TMK发散),len=sizeof(ST_EXTEND_KEYBLOCK) ,ST_SEC_KEY_INFO.format!=TR31,ID=5
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	
	memset(mkey, 0x15, 16);//对应于TMK 明文key SecLoad[2].uckey

	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = SecLoad[23].pkey;
	tr31_st.keylen = 16;
	
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	p=(ST_EXTEND_KEYBLOCK *)(stKeyInfo.sDstKeyValue);
	p->format=SEC_KEYBLOCK_FMT_TR31+1;
	p->len=len;
	p->pblock=(char *)block;
	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[23].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_PARA)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.17:以SEC_KCV_VAL方式去安装TDK(由ID=2,type=TMK发散),len=sizeof(ST_EXTEND_KEYBLOCK) ,ST_SEC_KEY_INFO.len=1,ID=5
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, 0x15, 16);//对应于TMK 明文key SecLoad[2].uckey

	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = SecLoad[24].pkey;
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
	
	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[24].pkcv, 4);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_PARAM)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.23:以SEC_KCV_NONE方式去安装TDK(由ID=2,type=TMK发散),len=sizeof(ST_EXTEND_KEYBLOCK) ,ST_EXTEND_KEYBLOCK.len=72,ST_EXTEND_KEYBLOCK.pblock为72个0x00,ID=5	
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);

	memset(mkey, 0x15, 16);//对应于TMK 明文key SecLoad[2].uckey
	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = SecLoad[25].pkey;
	tr31_st.keylen = 16;
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	p=(ST_EXTEND_KEYBLOCK*)(stKeyInfo.sDstKeyValue);
	p->format=SEC_KEYBLOCK_FMT_TR31;
	p->len=len;
	memset(p->pblock, 0x00, len);
	
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.18:以SEC_KCV_VAL方式去安装TDK(由ID=2,type=TMK发散),len=sizeof(ST_EXTEND_KEYBLOCK) ,ID=5
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, 0x15, 16);//对应于TMK 明文key SecLoad[2].uckey

	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = SecLoad[25].pkey;
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

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[25].pkcv, stKcvInfoIn.nLen);
	
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case2.19:以SEC_KCV_VAL方式去安装type=100(由ID=2,type=TMK发散),ID=6
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=6;
	stKeyInfo.ucDstKeyType=100;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[15].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[15].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_PARAM)   
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.21:使用同一个KEY，安装TDK(id=7), TAK(id=8)(由TMK发散)
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=7;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[16].pkey, stKeyInfo.nDstKeyLen);
	
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	stKeyInfo.ucDstKeyIdx=8;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
#if ANDIROD_ENABLE&!defined N910P//defined IM81||defined N900|| 
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)	/*经开发确认由于IM81的需求会返回成功 20150414 依妹和国兵确认*/
#else
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_INSTALL_REC)	/*应失败*/
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:FIXED体系
	//case3.1:以SEC_KCV_NONE方式去安装TAK(明文),ID=4,len=sizeof(ST_EXTEND_KEYBLOCK)  //len=sizeof(ST_EXTEND_KEYBLOCK) 属于MK/SK体系,故应报错
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_PARA)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3.2:以SEC_KCV_NONE方式去安装TAK(明文),ID=4,len=16
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[17].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3.3:以SEC_KCV_ZERO方式去安装TDK(明文),ID=5,len=16
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[18].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[18].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3.4:以SEC_KCV_VAL方式去安装TPK(明文),ID=3,len=24
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=24;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[19].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[19].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3.5:以100方式去安装TPK(明文),ID=3,len=24
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=100;
	stKeyInfo.nDstKeyLen=24;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[20].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=100;
	stKcvInfoIn.nLen=4;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_PARAM)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3.6:使用同一个KEY，安装TDK(id=9), TAK(id=10)(明文)
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=9;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[21].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	stKeyInfo.ucDstKeyIdx=10;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
#if ANDIROD_ENABLE&!defined N910P//defined IM81||defined N900||
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)	/*经开发确认由于IM81的需求会返回成功*/
#else
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_INSTALL_REC)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3.7:仅key的奇偶校验位不同,应视为同一个key,安装TDK(id=9), TAK(id=10)(明文)
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=9;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[22].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	ch = (*(SecLoad[22].pkey)&0xfe)|(*(SecLoad[22].pkey)&0x01?0x00:0x01);
	memset(stKeyInfo.sDstKeyValue, ch/**(SecLoad[22].pkey)-1*/, stKeyInfo.nDstKeyLen);
	stKeyInfo.ucDstKeyIdx=10;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
#if ANDIROD_ENABLE&!defined N910P//defined IM81||defined N900||
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)/*经开发确认由于IM81的需求会返回成功*/
#else
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_INSTALL_REC )/*应失败*/
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//case4:DUKPT体系	
	//在NDK_SecLoadTIK用例中测试
	send_result("%s测试通过", TESTAPI);
	
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}

