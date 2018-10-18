/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: tsec23.c
* Author 		: huangjianb
* version		: 
* DATE			: 20140815
* directory 		: 
* description		: 安全配置校验
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
#include "NDK.h"
#include <fcntl.h>

typedef struct key_kcv {
	uchar *pkey;
	uchar *pkcv;
} KEY_KCV;

void main(void)
{
	/*private & local definition*/
	int ret = 0;
	uint punCfgInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar udesin[31]={0},udesout[31]={0},udesout1[31]={0};
	KEY_KCV SecLoad[ ]={
		{"\x13", 																							NULL}, 
		{"\xE1\x46\x47\xE8\xA1\x35\x06\x1A\xE1\x46\x47\xE8\xA1\x35\x06\x1A", 									"\x3A\xDE\xBB\xE0"}, 
		{"\x15", 																							"\x26\x5A\x7A\xBF"},	
		{"\xA5\x94\x96\x77\x49\x26\x1C\xDF\xA5\x94\x96\x77\x49\x26\x1C\xDF", 								NULL},
		{"\x8A\xEE\xE9\x1A\x8C\x2F\x71\x97", 																"\xA4\x70\xF9\x9E"},
		{"\x79\x3C\xA1\xEA\x4F\xC8\x5C\xFB\x79\x3C\xA1\xEA\x4F\xC8\x5C\xFB", 								"\x16\x55\x7C\x70"},	
		{"\xDE\xD9\x76\x77\x94\xFC\xD9\x05\xDE\xD9\x76\x77\x94\xFC\xD9\x05\xDE\xD9\x76\x77\x94\xFC\xD9\x05", 	"\xDD\xA4\x94\xE6"},
		{"\x3d",																							NULL},
		{"\x97\x70\xAF\x26\xA8\x22\xA2\xE3\x97\x70\xAF\x26\xA8\x22\xA2\xE3", 									"\x70\x8D\xE9\xF7"},
	};
	uchar ch = 0x00;
	
	NDK_ScrInitGui();
	//测试前置
	memset(udesin, 0x20, sizeof(udesin));//初始化测试数据
	if((ret=NDK_SecGetCfg(&punCfgInfo))!=NDK_OK) //获取当前安全等级
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}

	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	
	//case1:唯一性限制打开，其他限制关闭
	if((ret=NDK_SecSetCfg(2))!=NDK_OK)
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	//以SEC_KCV_NONE方式安装TLK,ID=1
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[0].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}
	
	//以SEC_KCV_ZERO方式安装TMK(密文),ID=2
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[1].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[1].pkcv, stKcvInfoIn.nLen);	

	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}

	//以SEC_KCV_NONE方式,安装TAK(id=3)(由TMK发散)
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[3].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK) /*应成功*/
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}
	
	//case1.1:使用跟TAK3相同的密钥安装TDK3应该失败
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_INSTALL_REC)	/*应失败*/
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}
	
	//安装TDK(id=9), TAK(id=10)(明文)
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=9;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=8;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[7].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}
	
	//case1.2:跟TDK9仅key的奇偶校验位不同,应视为同一个key,安装应该失败
	ch = *(SecLoad[7].pkey)&0xfe|(*(SecLoad[7].pkey)&0x01?0x00:0x01);//基偶校验位一致的密钥应该失败
	memset(stKeyInfo.sDstKeyValue, ch/**(SecLoad[22].pkey)-1*/, stKeyInfo.nDstKeyLen);
	stKeyInfo.ucDstKeyIdx=10;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_INSTALL_REC )/*应失败*/
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}
	
	//case2: 密钥专用性限制打开，其他限制关闭
	if((ret=NDK_SecSetCfg(4))!=NDK_OK)
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}
	//安装主密钥TMK2
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=8;
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}
	//case2.1:限制打开时使用TMK计算DES应该失败
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TMK, 2, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECP_PARAM)
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}

	//case2.2: 惟一性限制关闭后安装相同的密钥应该成功于TLK1相同
	memset(udesout, 0, sizeof(udesout));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK;
	stKeyInfo.nDstKeyLen=8;
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}
	
	//case3:限制开关全关，16长度主密钥可以安装24长度的TDK
	if((ret=NDK_SecSetCfg(0x00))!=NDK_OK)
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}
	
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[2].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[2].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}

	stKeyInfo.ucScrKeyIdx=3;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=24;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[6].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[6].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		NDK_ScrPrintf("line %d:测试失败(%d)",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		goto ERR;
	}
	
	NDK_ScrPrintf("应用B执行成功\n");
	NDK_ScrRefresh();
	NDK_KbGetCode(0,NULL);
ERR://测试后置:擦除所有密钥
	NDK_SecSetCfg(0x06);
	NDK_SecKeyErase();
	NDK_SecSetKeyOwner("");
	NDK_SecKeyErase();
	return;
}
