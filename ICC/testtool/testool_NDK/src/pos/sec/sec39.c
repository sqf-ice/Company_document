/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name		: sec39.c
* Author 		:  
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试NDK_SecGetKeyOwner接口功能
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
#define TESTAPI	"NDK_SecGetKeyOwner"	

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
*			  	  	   jiangym			20180529	created
*****************************************************************/
void sec39(void)
{
	int ret = -1;
	char Owner[32] = {0}, OldOwner[32] = {0};
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI); 

	//测试前置获取密钥属主
	if((ret = NDK_SecGetKeyOwner(sizeof(OldOwner),OldOwner)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	
	//case1:参数非法
	if((ret = NDK_SecGetKeyOwner(sizeof(Owner),NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SecGetKeyOwner(-1,Owner)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:设置不同密钥属主,能够获取正确的属主值
	if((ret=NDK_SecSetKeyOwner("*"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SecGetKeyOwner(sizeof(Owner),Owner)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	//比较
	if(strcmp("*",Owner))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, Owner);
		GOTOERR;
	}
	if((ret=NDK_SecSetKeyOwner(""))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SecGetKeyOwner(sizeof(Owner),Owner)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较
	if(strcmp("com.example.highplattest",Owner))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, Owner);
		GOTOERR;
	}
	if((ret=NDK_SecSetKeyOwner("appname"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SecGetKeyOwner(sizeof(Owner),Owner)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较
	if(strcmp("appname",Owner))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, Owner);
		GOTOERR;
	}

	//case3:不停切换设置密钥属主后应该获取最后一次的密钥属主
	if((ret=NDK_SecSetKeyOwner("*"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecSetKeyOwner("appname"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecSetKeyOwner(""))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SecGetKeyOwner(sizeof(Owner),Owner)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较
	if(strcmp("com.example.highplattest",Owner))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, Owner);
		GOTOERR;
	}

	//case4:设置密钥属主进行相关操作(删除)后 再获取
	NDK_SecKeyErase();
	if((ret=NDK_SecSetKeyOwner("appname"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//安装TDK3(密钥16字节),ID=3
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=3;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;   
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecKeyDelete(3, SEC_KEY_TYPE_TDK))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SecGetKeyOwner(sizeof(Owner),Owner)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//比较
	if(strcmp("appname",Owner))
	{
		send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, Owner);
		RETURN;
	}	
	send_result("%s测试通过", TESTAPI);
	
ERR:
	NDK_SecSetKeyOwner(OldOwner);
	NDK_SecKeyErase();
	return;
}

