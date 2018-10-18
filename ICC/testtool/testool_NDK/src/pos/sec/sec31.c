/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name		: sec31.c
* Author 		: 
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试NDK_SecGetDrySR获取安全寄存器值和NDK_SecClear清安全寄存器值功能
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
#define TESTAPI	"NDK_SecGetDrySR,NDK_SecClear"	

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
*			  	  	      jiangym		20160603  created  
*****************************************************************/
void sec31(void)
{
	/*private & local definition*/
	int ret = -1, val = 0;
#if ANDIROD_ENABLE//defined N850
	ST_SEC_KEY_INFO stKeyInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;
#endif
	
	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	
	if(cls_show_msg("是否是拆机后进入此用例,是按[Enter],否按[其他]")!=ENTER)
	{
		//case1:参数异常测试
		if((ret = NDK_SecGetDrySR(NULL)) != NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}

		//case2:未安全触发获取安全寄存器的值为0
		if(((ret = NDK_SecGetDrySR(&val)) != NDK_OK) || val != 0)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, val);
			RETURN;
		}

		cls_show_msg("关机并进行拆机,拆机后再装机,使得安全触发后再进入此用例");
	}
	else
	{
		//case3:人为安全触发(拆机后再装机)获取安全寄存器值不为0,之后清安全寄存器应该成功,同时获取其值应该为0
		if(((ret = NDK_SecGetDrySR(&val)) != NDK_OK) || val == 0)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, val);
			RETURN;
		}
		if((ret = NDK_SecClear()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if(((ret = NDK_SecGetDrySR(&val)) != NDK_OK) || val != 0)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, val);
			RETURN;
		}
#if !defined N910P		 //N910P不具备此功能
		if(flag_sdk3)//sdk3.0清安全和清密钥后默认安装TLK，sdk2.0不安装  20180724  modify
		{
			//case4:清安全和清密钥后会默认安装TLK  20180620 sull add 
			if((ret=NDK_SecSetKeyOwner("*"))!=NDK_OK)
			{
				send_result("line %d:设置密钥属主失败(%d)", __LINE__, ret);
				GOTOERR;
			}
			//以SEC_KCV_ZERO方式安装TMK(密文),ID=2,明文16个0x15
			stKeyInfo.ucScrKeyIdx=1;
			stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_DES;
			stKeyInfo.ucDstKeyIdx=2;
			stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_DES;
			stKeyInfo.nDstKeyLen=16;	
			memcpy(stKeyInfo.sDstKeyValue, "\xCB\x62\x65\x94\x48\xAC\x8A\x72\xCB\x62\x65\x94\x48\xAC\x8A\x72", stKeyInfo.nDstKeyLen);
			stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
			stKcvInfoIn.nLen=4;
			memcpy(stKcvInfoIn.sCheckBuf, "\x26\x5A\x7A\xBF", stKcvInfoIn.nLen);	
			if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
			{
				send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_DES, 2, &stKcvInfoIn))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			if(memcmp(stKcvInfoIn.sCheckBuf, "\x26\x5A\x7A\xBF", 4)) 
			{
				send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
				goto ERR;
			}
		}
#endif
	}
//#endif
		send_result("%s测试通过", TESTAPI); 
	if(flag_sdk3)
	{
ERR:
		if(g_seckeyowner)
			NDK_SecSetKeyOwner("");
		else
			NDK_SecSetKeyOwner("*");
	}
	return;
}
