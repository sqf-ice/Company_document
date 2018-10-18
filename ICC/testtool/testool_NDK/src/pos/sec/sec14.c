/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec6.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecSetKeyOwner设置新的应用密钥表及设置共享密钥表接口功能
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
#define TESTAPI	"NDK_SecSetKeyOwner"
#define		SUBAPP1_NAME	"a@#^+,.[}丁_14"
#define 	SEC_SUBAPP_NLD1	"a@#^+,.[}丁_14.NLD" //根据开发要求将测试文件放到main文件所在路径，这样可以一起打包下载 20170621

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
*			  	  	        linrq         20130228	      created
						jiangym      20140814		modify
*****************************************************************/
void sec14(void)
{
	int ret = -1/*, MAXCNT = 5000, i = 0*/;
	char ab_name[257]={0};
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar udesout[31]={0}, udesin[31]={0};
	//char sKey[8+1]={0x34,0x78,0x12,0x67,0x93,0x11,0x54,0x28};
	//uchar sOutData[8+1]={0};
	//uchar szBuf[]={"879423714192374092137"};

	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
#if !ANDIROD_ENABLE
	if(cls_show_msg1(30, "测试%s...,请确保%s存在,ESC退出", TESTAPI, SEC_SUBAPP_NLD1)==ESC)
		return;
#else
	cls_printf("测试%s...",TESTAPI);
#endif

	//测试前置:擦除所有密钥
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
#if ANDIROD_ENABLE
	if((ret = NDK_SecSetKeyOwner("")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
#endif	
	//先对应用自身的密钥表进行装载密钥
	memset(udesin, 0x20, sizeof(udesin));

	//安装TDK3(密钥8字节),ID=3
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16/*8*/;  //海外产品不支持8位密钥，全部改成16位安装 20180419 sull modify
	memset(stKeyInfo.sDstKeyValue, 0x35, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//安装TPK(密钥16字节), ID=6
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK;
	stKeyInfo.ucDstKeyIdx=6;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x1b, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1:非法测试传入参数数据长度大于255
	memset(ab_name, 0x32, 256);
#if ANDIROD_ENABLE
	if((ret = NDK_SecSetKeyOwner(ab_name)) != NDK_ERR_SECP_PARAM)
#else
	if((ret=NDK_SecSetKeyOwner(ab_name))!=NDK_ERR_PARA)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:切换到共享密钥表,设置*类型的密钥表 进行密钥的安装
	if((ret=NDK_SecSetKeyOwner("*"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:验证share的权限不会被修改,此处设置高安全级别后,后续测试低安全级别应该还是成功
#if !ANDIROD_ENABLE  //ANDROID无法设置高安全级别
	if(( ret = NDK_SecSetCfg(0x1F))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//share表安装TMK, ID=5
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=16/*8*/; 
	memset(stKeyInfo.sDstKeyValue, 0x11, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//share表安装密钥ID2（TDK2(密钥8字节),ID=2,0x13）
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16/*8*/; 
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:安装密钥相同的TDK:ID9与ID2相同,验证share表格 不限制密钥惟一性
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=9;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16/*8*/; 
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//share表安装TPK, ID=8
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK;
	stKeyInfo.ucDstKeyIdx=8;
	stKeyInfo.nDstKeyLen=16/*8*/; 
	memset(stKeyInfo.sDstKeyValue, 0x1b, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5：验证share表格具有和应用表格独立的密钥组：share有TDK2 DES计算应该成功,无TDK3 DES计算应该失败
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 16/*8*/, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16/*SEC_DES_KEYLEN_8*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 16/*8*/))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 16/*8*/, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16/*SEC_DES_KEYLEN_8*/))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6:share表格密钥专用性限制关闭：允许传递TLK/TPK/TMK/TAK/TDK等密钥执行des加解密运算	
#if !ANDIROD_ENABLE  //ANDROID不支持，只支持data加解密
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TMK, 5, udesin, 16/*8*/, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16/*SEC_DES_KEYLEN_8*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(udesout, "\x77\xC4\xB0\xF4\x22\x56\xD2\x84\x77\xC4\xB0\xF4\x22\x56\xD2\x84", 16/*8*/))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
#endif
/*
	//case7:share表格敏感次数限制关闭,可以进行连续的交易
	//安装TAK用于敏感服务限制测试
	memset(&stKeyInfo,0,sizeof(stKeyInfo));
	memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK;
	stKeyInfo.ucDstKeyIdx = 1;
	stKeyInfo.nDstKeyLen = 8;
	memcpy(stKeyInfo.sDstKeyValue,sKey,8);
	if ((ret=NDK_SecLoadKey (&stKeyInfo,&stKcvInfoIn))!=NDK_OK)
	{
		cls_show_msg("line %d:NDK_SecLoadKey装载密钥失败(ret=%d)", __LINE__ , ret);
		GOTOERR;
	}
	cls_printf("敏感服务无限制测试中，请稍等!");
	for(i=0; i<MAXCNT; i++)
	{
		//cls_printf("敏感服务超次测试中,第%d次MAC运算中...", i+1);
		if(NDK_SecGetMac(1,szBuf,sizeof(szBuf),sOutData,SEC_MAC_ECB)!=NDK_OK)
		{
			cls_show_msg("敏感服务超次测试完成(已成功使用%d次,可能已达使用上限)", i+1);
			GOTOERR;
		}
	}
	*/
	//case8:验证子应用1可以使用*密钥表中的密钥进行calcdes
#if ANDIROD_ENABLE
	cls_show_msg("切换到子应用B(JDK sec17的应用B),验证子应用可以使用共享密钥表");
#else
	cls_printf("验证子应用1可以使用共享密钥表");
#if defined ME31SLS 
	system("cd /appfs/download/");
	system("./sec14");
#else
	if(( ret = NDK_AppLoad((uchar *)SEC_SUBAPP_NLD1,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(( ret = NDK_AppRun((uchar *)SUBAPP1_NAME))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
#endif
	if (cls_show_msg("请确认子应用执行结果:成功[确认],失败[其它]")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case9: B应用2中擦除了密钥，*表中的密钥已不能使用了
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 16/*8*/, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16/*SEC_DES_KEYLEN_8|SEC_DES_ECBMODE*/))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case10:传递空指针或者空串""则切换回应用自身的密钥表  
	if((ret=NDK_SecSetKeyOwner(NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	//"*"端的密钥应不存在
	memset(udesout, 0, sizeof(udesout));
	if((ret = NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 8, udesin, 16/*8*/, udesout, SEC_DES_ENCRYPT/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//Android端的密钥
	memcpy(udesin, "\x11\x22\x33\x44\x55\x66\x77\x88\x11\x22\x33\x44\x55\x66\x77\x88", 16/*8*/);
	memset(udesout, 0, sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 16/*8*/, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16/*SEC_DES_KEYLEN_8*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(udesout, "\xB6\xBD\x05\xE8\x4C\xAE\x15\xAD\xB6\xBD\x05\xE8\x4C\xAE\x15\xAD", 16/*8*/))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}	
	//传递空字符串则切换回应用自身的密钥表
	if((ret=NDK_SecSetKeyOwner(""))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case11:这时用ID3进行NDK_SecCalcDes就会成功
	memset(udesin, 0x20, sizeof(udesin));
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 16/*8*/, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16/*SEC_DES_KEYLEN_8*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(udesout, "\x3B\xEE\x4A\x4A\xFE\xD0\x67\xD7\x3B\xEE\x4A\x4A\xFE\xD0\x67\xD7", 16/*8*/))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

#if !ANDIROD_ENABLE  //ANDROID不支持
	//case12:这时开关限制开启了,对于非TDK 的(TPK,ID6)就不能进行des计算了
	//NDK_SecSetCfg(0x06);//默认安全级别
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 6, udesin, 16, udesout, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT))!=NDK_ERR_SECP_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}	
#endif
	
	send_result("%s测试通过", TESTAPI);
	//测试后置:
ERR:
#if ANDIROD_ENABLE   //恢复成入口处的设置
	if(g_seckeyowner)
		NDK_SecSetKeyOwner("");
	else
		NDK_SecSetKeyOwner("*");
#else
	NDK_AppDel(SUBAPP1_NAME);
	NDK_SecSetCfg(0x06);//默认安全级别
	NDK_SecSetKeyOwner(NULL);
#endif
	NDK_SecKeyErase();	
	return;
}

