/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: ssl模块
* file name		: ssl2.c
* Author 			: chenfm
* version			: 
* DATE			: 20120905
* directory 		: 
* description		: 测试NDK_SetSSLBlockingMode接口功能
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"NDK_SetSSLBlockingMode,NDK_GetSSLBlockingMode"

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	:
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	  chenfm      20120905	created
*****************************************************************/
void ssl2(void)
{
	int ret;
	NDK_HANDLE handle=NULL;	
	int ciphers[]={SSL3_CIPHER_RSA_NULL_MD5, SSL3_CIPHER_RSA_NULL_SHA, SSL3_CIPHER_RSA_RC4_128_MD5, 0x0};

	cls_printf("测试%s...", TESTAPI); 

	//测试前置:

	//case1: 不先创建一个ssl句柄，传一个空的句柄调用NDK_SetSSLBlockingMode应失败
	if((ret=NDK_SetSSLBlockingMode(NULL, NDK_SUSPEND))!=NDK_ERR_SSL_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_GetSSLBlockingMode(NULL))!=NDK_ERR_SSL_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:以非空handle进行正确调用
	if((handle=NDK_OpenSSLSocket(HANDSHAKE_SSLv23, SSL_AUTH_NONE, ciphers))==NULL)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//case2.1: 传错误的参数，应返回orvMODEUNSUPPORTED
	if((ret=NDK_SetSSLBlockingMode(handle, 234))!=NDK_ERR_SSL_MODEUNSUPPORTED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case2.2: 调用前先创建一个ssl句柄，再正确调用NDK_SetSSLBlockingMode应成功返回NDK_OK,获取当前的状态应跟前面设置的保持一致。
	if((ret=NDK_SetSSLBlockingMode(handle, NDK_NOWAIT))!=NDK_OK||NDK_GetSSLBlockingMode(handle)!=NDK_NOWAIT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if((ret=NDK_SetSSLBlockingMode(handle, NDK_SUSPEND))!=NDK_OK||NDK_GetSSLBlockingMode(handle)!=NDK_SUSPEND)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//测试后置:
	if((ret=NDK_CloseSSLSocket(handle))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}


