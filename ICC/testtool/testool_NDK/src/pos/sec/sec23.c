/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec23.c
* Author 		: huangjianb
* version		: 
* DATE			: 20140814
* directory 		: 
* description		: 安全配置
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
#define		TESTAPI		"NDK_SecGetCfg,NDK_SecSetCfg"
#define		TESTAPP		"a@#^+,.[}丁_23"
#define 	FILETEST23	"a@#^+,.[}丁_23.NLD" //根据开发要求将测试文件放到main文件所在路径，这样可以一起打包下载 20170621

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
* functional description 	: 提供安全等级的设置与获取功能,密钥唯一性,密钥专用性,密钥保护强度的验证
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author			date				remarks
*			  	  	  		 huangjianb 20140814		created
*****************************************************************/
void sec23(void)
{
	/*private & local definition*/
	int ret = 0;
	uint punCfgInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar udesin[31]={0},udesout[31]={0},udesout1[31]={0};
	KEY_KCV SecLoad[ ]={
		{(uchar *)"\x13", 																							NULL}, 
		{(uchar *)"\xE1\x46\x47\xE8\xA1\x35\x06\x1A\xE1\x46\x47\xE8\xA1\x35\x06\x1A", 								(uchar *)"\x3A\xDE\xBB\xE0"}, 
		{(uchar *)"\x15", 																							(uchar *)"\x26\x5A\x7A\xBF"},	
		{(uchar *)"\xA5\x94\x96\x77\x49\x26\x1C\xDF\xA5\x94\x96\x77\x49\x26\x1C\xDF", 								NULL},
		{(uchar *)"\x8A\xEE\xE9\x1A\x8C\x2F\x71\x97", 																(uchar *)"\xA4\x70\xF9\x9E"},
		{(uchar *)"\x79\x3C\xA1\xEA\x4F\xC8\x5C\xFB\x79\x3C\xA1\xEA\x4F\xC8\x5C\xFB", 								(uchar *)"\x16\x55\x7C\x70"},	
		{(uchar *)"\xDE\xD9\x76\x77\x94\xFC\xD9\x05\xDE\xD9\x76\x77\x94\xFC\xD9\x05\xDE\xD9\x76\x77\x94\xFC\xD9\x05",(uchar *)"\xDD\xA4\x94\xE6"},
		{(uchar *)"\x3d",																							NULL},
		//{(uchar *)"\x97\x70\xAF\x26\xA8\x22\xA2\xE3\x97\x70\xAF\x26\xA8\x22\xA2\xE3", 							(uchar *)"\x70\x8D\xE9\xF7"},
	};
	uchar ch = 0x00;
	
	memset(udesin, 0x20, sizeof(udesin));//初始化测试数据

	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	if(cls_show_msg1(30, "测试%s...请确保%s存在,ESC退出", TESTAPI, FILETEST23)==ESC)
		return;

	//测试前置
	NDK_SecSetKeyOwner("");
	if(((ret=NDK_SecGetCfg(&punCfgInfo))!=NDK_OK)||(punCfgInfo!=0x06)) //默认安全等级为0x06
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, punCfgInfo);
		return;
	}
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	
	if((ret=NDK_SecSetCfg(0x18))!=NDK_OK) //修改安全等级跟默认值相反 0x18
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//case4：验证每个应用有独立的安全等级配置，子应用中会重新设置不同的安全密钥等级，应该不影响主应用
#if defined ME31SLS //乐刷定制不能使用app相关函数,故打包成补丁包后使用系统函数进行运行子应用
	system("cd /appfs/download/");
	system("./sec23");
#else
	if(( ret = NDK_AppLoad((uchar *)FILETEST23,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(( ret = NDK_AppRun((uchar *)TESTAPP))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	if (cls_show_msg("请确认应用B执行结果:成功[确认],失败[其它]")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[0].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
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
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if  defined ME31SLS  //由于乐刷的应用不能在打包的时候新建一个目录,使用原有的目录会导致安全等级被改回去,这里再设置一次
	if((ret=NDK_SecSetCfg(0x18))!=NDK_OK) //修改安全等级跟默认值相反 0x18
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
#endif

	//case1:验证密钥惟一性限制被关闭，同一个密钥值重复安装到不同ID或类型或用途的密钥存储空间上，应该成功
	//case1.1:使用同一个KEY,以SEC_KCV_NONE方式,安装TAK3(id=3)(由TMK发散)
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[3].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK) /*应成功*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.2:使用同一个KEY,安装TDK3(id=3)(由TMK发散)
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)	/*应成功*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.3:使用同一个KEY,安装TAK4(id=4)(由TMK发散)
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)	/*应成功*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.4:仅key的奇偶校验位不同,应视为同一个key,安装TDK(id=9), TAK(id=10)(明文)
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=9;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=8;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[7].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	ch = (*(SecLoad[7].pkey)&0xfe)|(*(SecLoad[7].pkey)&0x01?0x00:0x01);
	memset(stKeyInfo.sDstKeyValue, ch/**(SecLoad[22].pkey)-1*/, stKeyInfo.nDstKeyLen);
	stKeyInfo.ucDstKeyIdx=10;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK )/*应成功*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:密钥专用性限制关闭:允许使用主密钥、PIN密钥、MAC密钥进行任意数据的加解密操作
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=8;
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case2.1:主密钥做DES加解密
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TMK, 2, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 8))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	memset(udesout1,0,sizeof(udesout1));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TMK, 2, udesout, 8, udesout1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout1, "\x20\x20\x20\x20\x20\x20\x20\x20", 8))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR ;
	}
	
	//case2.2:PIN密钥做DES加解密
	memset(udesout, 0, sizeof(udesout));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=8;
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 3, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 8))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	memset(udesout1,0,sizeof(udesout1));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 3, udesout, 8, udesout1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout1, "\x20\x20\x20\x20\x20\x20\x20\x20", 8))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	
	//case2.3:MAC密钥做DES加解密
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
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TAK, 4, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 8))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	memset(udesout1,0,sizeof(udesout1));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TAK, 4, udesout, 8, udesout1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout1, "\x20\x20\x20\x20\x20\x20\x20\x20", 8))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	
	//case3:密钥保护强度限制打开，应该不允许使用短长度的密钥安装长长度的密钥
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
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3.1:16长度主密钥安装8长度TDK应该成功:以SEC_KCV_VAL方式去安装TDK(由ID=2,type=TMK发散),len=8,ID=5
	stKeyInfo.ucScrKeyIdx=3;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=8;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[4].pkey, stKeyInfo.nDstKeyLen);
	
	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[4].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3.2:16长度主密钥安装16长度TDK应该成功:以SEC_KCV_VAL方式去安装TDK(由ID=2,type=TMK发散),len=16,ID=5
	stKeyInfo.ucScrKeyIdx=3;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[5].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[5].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3.3:16长度主密钥安装24长度TDK应该失败:以SEC_KCV_VAL方式去安装TDK(由ID=2,type=TMK发散),len=24,ID=5
	stKeyInfo.ucScrKeyIdx=3;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=24;
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[6].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[6].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECKM_KEY_LEN)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试通过
	send_result("%s测试通过", TESTAPI);
	
ERR://测试后置:擦除所有密钥
	NDK_SecSetCfg(0x06);
	NDK_AppDel(TESTAPP);
	NDK_SecKeyErase();
	return;
}
