/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec30.c
* Author 		: zhengry
* version		: 
* DATE			: 20160128
* directory 		: 
* description		: 测试NDK_SecGetCfg,NDK_SecSetCfg安全配置对国密密钥体系的管理功能
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
* functional description 	: 提供安全等级的设置与获取功能,密钥唯一性,密钥专用性,密钥保护强度的验证 (对国密新增部分进行验证)
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author			date		remarks
*			  	  	  		zhengry     20160126		created
*****************************************************************/
void sec30(void)
{
	/*private & local definition*/
	int ret = 0;
	uint punCfgInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar udesin[31]={0},udesout[31]={0},udesout1[31]={0};
	KEY_KCV SecLoad[ ]={
		{(uchar *)"\x13", 																					 NULL},   // 0  明文安装TLK
		{(uchar *)"\x16\x36\x01\xEE\xC3\x53\x04\x00\xF2\x8E\xF1\xE3\x77\x4D\x31\x59", (uchar *)"\x73\x0E\x17\xFB"},   // 1  以SEC_KCV_ZERO方式安装TMK(由TLK发散)，明文为16字节的0X15
		{(uchar *)"\x6F\xD1\xDC\x2C\x2C\x55\xFA\x9F\x8B\x73\x19\xF6\x37\xC4\x77\x00", 						 NULL},   // 2  以SEC_KCV_NONE方式,安装TAK(由TMK发散)  明文为16字节的0X17
		{(uchar *)"\x19", 																					 NULL},   // 3  明文安装TAK
		{(uchar *)"\x1B",															  (uchar *)"\x96\x64\xBE\x8E"},   // 4	明文安装TMK
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
		send_result("line %d:%s测试失败(%d,%02x)", __LINE__, TESTAPI, ret, punCfgInfo);
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
	
	//case1：验证每个应用有独立的安全等级配置，子应用中会重新设置不同的安全密钥等级，应该不影响主应用
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
	
	//以SEC_KCV_NONE方式明文安装TLK,ID=2
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[0].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//以SEC_KCV_ZERO方式安装TMK(由TLK发散),ID=2
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
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

	//case2:密钥惟一性限制被关闭，同一个密钥值重复安装到不同ID或类型或用途的密钥存储空间上，应该成功
	//case2.1:使用同一个KEY,以SEC_KCV_NONE方式,安装TAK1(由TMK发散)
	stKeyInfo.ucScrKeyIdx=2;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;	
	memcpy(stKeyInfo.sDstKeyValue, SecLoad[2].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK) /*应成功*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case2.2:使用同一个KEY,安装TDK1(id=3)(由TMK发散)
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)	/*应成功*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2.3:使用同一个KEY,安装TAK2(id=4)(由TMK发散)
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)	/*应成功*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2.4:仅key的奇偶校验位不同,应视为同一个key,安装TDK(id=9),以SEC_KCV_NONE方式明文安装TAK3(id=10)
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=9;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, *SecLoad[3].pkey, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	ch = (*(SecLoad[3].pkey)&0xfe)|(*(SecLoad[3].pkey)&0x01?0x00:0x01);
	memset(stKeyInfo.sDstKeyValue, ch/**(SecLoad[22].pkey)-1*/, stKeyInfo.nDstKeyLen);
	stKeyInfo.ucDstKeyIdx=10;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK )/*应成功*/
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:密钥专用性限制关闭:允许使用主密钥、PIN密钥、MAC密钥进行任意数据的加解密操作
	//case3.1:主密钥做SM4加解密
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x23, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TMK, 2, udesin, 16, udesout, SEC_SM4_ENCRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout, "\x9E\xE8\x30\xB9\x23\x4A\x7F\x13\xD0\xE0\x3F\x99\xC6\x1C\xF7\x79", 16))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	memset(udesout1,0,sizeof(udesout1));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TMK, 2, udesout, 16, udesout1, SEC_SM4_DECRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout1, "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR ;
	}

	//case3.2:PIN密钥做SM4加解密
	memset(udesout, 0, sizeof(udesout));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x23, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 3, udesin, 16, udesout, SEC_SM4_ENCRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout, "\x9E\xE8\x30\xB9\x23\x4A\x7F\x13\xD0\xE0\x3F\x99\xC6\x1C\xF7\x79", 16))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	memset(udesout1,0,sizeof(udesout1));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 3, udesout, 16, udesout1, SEC_SM4_DECRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout1, "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR ;
	}
	
	//case3.3:MAC密钥做DES加解密
	memset(udesout, 0, sizeof(udesout));
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x23, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TAK, 4, udesin, 16, udesout, SEC_SM4_ENCRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout, "\x9E\xE8\x30\xB9\x23\x4A\x7F\x13\xD0\xE0\x3F\x99\xC6\x1C\xF7\x79", 16))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	memset(udesout1,0,sizeof(udesout1));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TAK, 4, udesout, 16, udesout1, SEC_SM4_DECRYPT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout1, "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR ;
	}
		
	//测试通过
	send_result("%s测试通过", TESTAPI);
	
ERR://测试后置:擦除所有密钥
	NDK_SecSetCfg(0x06);
	NDK_AppDel(TESTAPP);
	NDK_SecKeyErase();
	return;
}
