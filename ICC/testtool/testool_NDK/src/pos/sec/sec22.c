/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec19.c
* Author 		: huangjb
* version		: 
* DATE			: 20140523
* directory 		: 
* description		: 测试应用A给应用B安装密钥
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
#define		TESTAPI		"跨应用安装密钥"
#if OVERSEAS_ENABLE	//由于海外版本不支持特殊字符和汉字字符的打包,故修改打包文件的文件名
#define		TESTAPP		"test22"
#define 	FILETEST22	"/appfs/download/test22.NLD"  
#else
#define		TESTAPP		"a@#^+,.[}丁_22"
#define 	FILETEST22	"a@#^+,.[}丁_22.NLD"  //根据开发要求将测试文件放到main文件所在路径，这样可以一起打包下载 20170621
#endif

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
* history 		 		:  author			date				remarks
*			  	  	  		 huangjb    20140523		created
*****************************************************************/
void sec22(void)
{
	/*private & local definition*/
	int ret = 0;
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar udesin[31]={0},udesout[31]={0};
#if !OVERSEAS_ENABLE
	uint unLen=0;
	char sBuf[128]={0};
#endif
	
	memset(udesin, 0x20, sizeof(udesin));//初始化测试数据

	/*process body*/
	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//appfs/download/temp/App_encTest.NLD 请确保证NLD包存在
	if(cls_show_msg1(30, "测试%s...请确保%s存在,ESC退出", TESTAPI, FILETEST22)==ESC)
		return;
	
	//测试前置:安装应用B和擦除所有密钥
#if !defined ME31SLS	
	if(( ret = NDK_AppLoad((uchar *)FILETEST22,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}

	//A给自己装密钥ID=1的密钥、安装TDK1(密钥8字节),0x13
	if((ret=NDK_SecSetKeyOwner(""))!=NDK_OK)//切换回当前应用A
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:A给B安装密钥B1、B2
	if((ret=NDK_SecSetKeyOwner(TESTAPP))!=NDK_OK)//为子应用B设置密钥归属，后续安装的密钥为应用B的密钥
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
	
	//A给B密钥B1（TDK1(密钥8字节),ID=1,0x15）
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x15, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//A给B安装密钥2（TDK1(密钥8字节),ID=2,0x13）
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:属主正确情况下:A验证密钥加密结果应该和A自己的密钥的结果一致，说明应用A和B有独立的密钥空间和ID
	if((ret=NDK_SecSetKeyOwner(""))!=NDK_OK)//切换回A应用
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 8))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	
	//case4:属主为A的情况下:A验证B2密钥失败，说明应用A和B有独立的密钥空间和ID,属主不正确情况下A不能使用B的密钥
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}

	//case12:切换到应用B的属主后，应用A应该可以使用应用B的密钥2进行加解密，属主正确情况下A可以使用B的密钥
	if((ret=NDK_SecSetKeyOwner(TESTAPP))!=NDK_OK)//切换回B应用
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 8))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}

	//case5:运行子应用B，子应用B中B使用自己的密钥B1,B2进行DES加密应和预期一致，且应用B可以删除自己的密钥
	if((ret=NDK_SecSetKeyOwner(TESTAPP))!=NDK_OK)//切换回B应用
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
#if defined ME31SLS //乐刷定制不能使用app相关函数,故打包成补丁包后使用系统函数进行运行子应用
	system("cd /appfs/download/");
	system("./sec22");
#else
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

	//case6:B应用中擦除了密钥，应该不影响A的密钥
	if((ret=NDK_SecSetKeyOwner(""))!=NDK_OK)//切换回A应用
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 8))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	
	//A应用重新给B应用安装两组密钥用于后续测试属主正确情况下A擦除B密钥应该成功
	if((ret=NDK_SecSetKeyOwner(TESTAPP))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x15, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if !OVERSEAS_ENABLE  //海外版本没有进行获取TUSN   
	//case11:新增获取TUSN后不能改变应用A的密钥属主(原先的bug是获取TUSN后会修改应用当前密钥属主从而导致会获取不到应用的密钥)20170727 added by jym
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_POS_TUSN, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s获取TUSN失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//此时属主还是B表,使用B表的密钥应该成功
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 8))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
#endif	
	//case7.1:当密钥属主属于B时 应用A擦除B中指定的ID=2的密钥应该成功,删除后B使用ID=2的密钥加解密应该失败
	if((ret=NDK_SecSetKeyOwner(TESTAPP))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
	if((ret=NDK_SecKeyDelete(2,SEC_KEY_TYPE_TDK))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
#if defined ME31SLS
	system("cd /appfs/download/");
	system("./sec22");
#else
	if(( ret = NDK_AppRun((uchar *)TESTAPP))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	if (cls_show_msg("请确认是否在50行子应用B调用密钥加密失败:是[确认],否[其它]")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case7.2:A擦除B中所有密钥应该成功,删除后B使用ID=1的密钥加解密应该失败
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
#if defined ME31SLS
	system("cd /appfs/download/");
	system("./sec22");
#else
	if(( ret = NDK_AppRun((uchar *)TESTAPP))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	if (cls_show_msg("请确认是否在33行子应用B调用密钥加密失败:是[确认],否[其它]")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case10:验证应用A调用删除B密钥时，不应该删除自己的密钥，所以A的密钥还应该能够使用
	if((ret=NDK_SecSetKeyOwner(""))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 8))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	
	//case8:A擦除A后验证A失败
	if((ret=NDK_SecSetKeyOwner(""))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		GOTOERR;
	}
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECCR_GET_KEY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	
	
	//case9：调用NDK_SecSetKeyOwner后A给自己装密钥应成功
	//安装TDK1(密钥8字节),0x13
	if((ret=NDK_SecSetKeyOwner(""))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__,TESTAPI,ret);
		return;
	}
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR ;
	}
	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 8))
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试通过
	send_result("%s测试通过", TESTAPI);
	//测试后置:擦除所有密钥和测试应用
ERR:
	NDK_AppDel(TESTAPP);
	NDK_SecSetKeyOwner("");
	NDK_SecKeyErase();
	return;
}
