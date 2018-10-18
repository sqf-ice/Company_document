/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec4.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecGetKcv接口功能
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
#define TESTAPI	"NDK_SecGetKcv"	

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
void sec4(void)
{
	int ret = -1, i = 0;
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
//	uchar *pZKcv=(uchar *)"\x82\xE1\x36\x65";   	//SEC_KCV_ZERO方式:TMK密钥为16个0x11，对8个字节0x00进行DES运算，得到的前四个字节
	uchar *pVKcv=(uchar *)"\xD1\xE5\x07\x48";    	//SEC_KCV_VAL方式:TMK密钥为16个0x11，对8个字节\x12\x34x56\x78\x90\x12\x34\x56进行DES运算，得到的前四个字
	uchar *errKcv=(uchar *)"\x26\x5A\x77\xBB";		//"\x26\x5A\x7A\xBF";(正确KCV)	// 2		//SEC_KCV_ZERO方式(TMK id=2,明文KEY=16个0x15)
	uchar *pZKcv[]={
				//DES算法对应的KCV值
				(uchar *)"\x82\xE1\x36\x65",		//SEC_KCV_ZERO方式:TMK密钥为16个0x11，对8个字节0x00进行DES运算，得到的前四个字节
				//SM4算法对应的KCV值
				(uchar *)"\xF8\xD0\x68\x70",		//SEC_KCV_ZERO方式:TMK密钥为16个0x11，对16个字节0x00进行SM4运算，得到的前四个字节
				//AES算法对应的KCV值
				(uchar *)"\xEE\x23\xD8\x1C"			//SEC_KCV_ZERO方式:TMK密钥为16个0x11，经过kcv运算，得到的前四个字节
					};
#if SEC_SM4_ENABLE  //DES+SM4
	int algnum = 2;//算法数量
	EM_SEC_KEY_ALG key_flag[2] = {SEC_KEY_DES , SEC_KEY_SM4};
#else  //DES
	int algnum = 1;
	int key_flag[2] = {0,0} ;
#endif
#if SEC_AES_ENABLE  //AES
	ST_EXTEND_KEYBLOCK stExtendKey;
	char KeyValue[16] = {0};
#endif

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI); 
	
	//测试前置
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
		return;
	}
	
	for(i = 0;i<algnum;i++)
	{	
		//case1:未安装密钥应返回失败
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 1, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			RETURN;
		}
		
		//安装密钥TMK id=1
		stKeyInfo.ucScrKeyIdx=0;/*表示明文安装*/	
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|key_flag[i];
		stKeyInfo.ucDstKeyIdx=1;
		stKeyInfo.nDstKeyLen=16;
		memset(stKeyInfo.sDstKeyValue, 0x11, stKeyInfo.nDstKeyLen);
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			RETURN;
		}
		
		//case2:读取KCV(模式SEC_KCV_ZERO)应返回正确
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 1, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
#if OVERSEAS_ENABLE||defined N910P  //海外版本使用3字节KCV验证 根据国兵邮件 20180116 modify by jiangym 
		if(memcmp(stKcvInfoIn.sCheckBuf, pZKcv[0+i], 3))//pZKcv
#else
		if(memcmp(stKcvInfoIn.sCheckBuf, pZKcv[0+i], 4))//pZKcv
#endif
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}

		//case8:新增读取id为2的kcv预期失败再去读取id为1的kcv预期成功(存在的bug:安装了id1读取id2的kcv预期失败再读取id1预期成功实际返回失败)20180413 add 
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 2, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 1, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}

		//case7:以模式SEC_KCV_VAL读取的KCV,SM4算法不支持应返回失败;DES算法支持应返回成功
		stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
		if(i==1)
		{
			
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 1, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM)
			{
				send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}
		}
		else
		{
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 1, &stKcvInfoIn))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}
#if OVERSEAS_ENABLE||defined N910P  //海外版本使用3字节KCV验证 根据国兵邮件 20180116 modify by jiangym 
			if(memcmp(stKcvInfoIn.sCheckBuf, pVKcv, 3))
#else
			if(memcmp(stKcvInfoIn.sCheckBuf, pVKcv, 4))
#endif
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				GOTOERR;
			}
		}
		
		//case4:读取KCV(模式SEC_KCV_NONE)应返回正确
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 1, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		
		//case6:以SEC_KCV_ZERO方式安装TMK(明文),ID=3，测试传入的KCV不正确应该安装失败，并且获取KCV也应该失败 20140813 addedby linwl
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=3;
		stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|key_flag[i];
		stKeyInfo.nDstKeyLen=16;	
		memset(stKeyInfo.sDstKeyValue, 0x15, stKeyInfo.nDstKeyLen);
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, errKcv, stKcvInfoIn.nLen);
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_KCV_CHK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 3, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		//cls_show_msg("0x%2x,0x%2x,0x%2x,0x%2x", stKcvInfoIn.sCheckBuf[0], stKcvInfoIn.sCheckBuf[1], stKcvInfoIn.sCheckBuf[2], stKcvInfoIn.sCheckBuf[3]);
	}
#if SEC_AES_ENABLE
	//AES密钥相关的测试单独提取出来,方便组合,有的是只支持DES和AES的 20180808 modify
	if(flag_sdk3)	//sdk3.0才导入AES  
	{
		//case1.1:未安装密钥应返回失败
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 1, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		
		//安装密钥TMK id=1
		stKeyInfo.ucScrKeyIdx=0;/*表示明文安装*/	
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
		stKeyInfo.ucDstKeyIdx=1;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 16;
		memset(KeyValue, 0x11, stExtendKey.len);
		stExtendKey.pblock = KeyValue;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));		
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		
		//case2.1:读取KCV(模式SEC_KCV_ZERO)应返回正确
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 1, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#if OVERSEAS_ENABLE||defined N910P  //海外版本使用3字节KCV验证 根据国兵邮件 20180116 modify by jiangym 
		if(memcmp(stKcvInfoIn.sCheckBuf, pZKcv[2], 3))//pZKcv
#else
		if(memcmp(stKcvInfoIn.sCheckBuf, pZKcv[2], 4))//pZKcv
#endif
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//case8.1:新增读取id为2的kcv预期失败再去读取id为1的kcv预期成功(存在的bug:安装了id1读取id2的kcv预期失败再读取id1预期成功实际返回失败)20180413 add 
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 2, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 1, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//case7.1:以模式SEC_KCV_VAL读取的KCV,SM4算法不支持应返回失败;DES算法支持应返回成功
		stKcvInfoIn.nCheckMode=SEC_KCV_VAL;		
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 1, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		
		//case4.1:读取KCV(模式SEC_KCV_NONE)应返回正确
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 1, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		
		//case6.1:以SEC_KCV_ZERO方式安装TMK(明文),ID=3，测试传入的KCV不正确应该安装失败，并且获取KCV也应该失败 20140813 addedby linwl
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=3;
		stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 16;
		memset(KeyValue, 0x15, stExtendKey.len);
		stExtendKey.pblock = KeyValue;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, errKcv, stKcvInfoIn.nLen);
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_ERR_SECP_KCV_CHK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 3, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
#endif	
	
	//case5:传入非法参数
	if((ret=NDK_SecGetKcv(100, 1, &stKcvInfoIn))!=NDK_ERR_SECKM_KEY_TYPE)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(flag_sdk3 && !g_seckeyowner)  //sdk3.0且是在共享密钥区时清安全和清密钥后默认安装TLK，sdk2.0不安装  20180724  modify
	{
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TAK, 1, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	else
	{
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TLK, 1, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK, 2, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC )
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0	//101已合法
	if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK, 101, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK, 1, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	stKcvInfoIn.nCheckMode=100;
	if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK, 1, &stKcvInfoIn))!=NDK_ERR_SECP_PARAM)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	//测试后置:擦除所有密钥
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_SecKeyErase();
	return;
}


