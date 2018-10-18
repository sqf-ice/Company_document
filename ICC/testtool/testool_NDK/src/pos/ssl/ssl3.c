/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: ssl模块
* file name		: ssl4.c
* Author 			: chenfm
* version			: 
* DATE			: 20120905
* directory 		: 
* description		: 测试NDK_Loadxxx函数族
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"NDK_Loadxxx函数族"

#define CERTF   "clientcert.pem"
#define CERTF1  "clientcert.der"
#define KEY     "clientkey.pem"
#define NOPSWKEY "clientkey.der"
#define KEYERR  "errkey.pem"
#define SCERTF  "cacert.pem"
#define TIMEOUT 60*1000   //ms
#define PRIKEY_PWD	"123456"
#define PRIKEY_ERRPWD	"12345678"

/****************************************************************
* function name 	 	: 
* functional description 	:
* input parameter	 	:
* output parameter	 	:
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	  chenfm      20120905	created
*****************************************************************/
void ssl3(void)
{
	int ret;
	NDK_HANDLE handle=NULL;
	ST_SOCKET_ADDR ss_addr;
	int ciphers[]={SSL3_CIPHER_RSA_NULL_MD5, SSL3_CIPHER_RSA_NULL_SHA, SSL3_CIPHER_RSA_RC4_40_MD5, SSL3_CIPHER_RSA_RC4_128_MD5, SSL3_CIPHER_RSA_RC4_128_SHA,
		             SSL3_CIPHER_RSA_RC2_40_MD5, SSL3_CIPHER_RSA_IDEA_128_SHA, SSL3_CIPHER_RSA_DES_40_CBC_SHA, SSL3_CIPHER_RSA_DES_64_CBC_SHA,
		             SSL3_CIPHER_RSA_DES_192_CBC3_SHA, SSL3_CIPHER_DH_RSA_DES_192_CBC3_SHA, SSL3_CIPHER_DH_DSS_DES_40_CBC_SHA, SSL3_CIPHER_DH_DSS_DES_64_CBC_SHA,
		             SSL3_CIPHER_DH_DSS_DES_192_CBC3_SHA, SSL3_CIPHER_DH_RSA_DES_40_CBC_SHA, SSL3_CIPHER_DH_RSA_DES_64_CBC_SHA, SSL3_CIPHER_EDH_DSS_DES_40_CBC_SHA,
		             SSL3_CIPHER_EDH_DSS_DES_64_CBC_SHA, SSL3_CIPHER_EDH_DSS_DES_192_CBC3_SHA, SSL3_CIPHER_EDH_RSA_DES_40_CBC_SHA,
		             SSL3_CIPHER_EDH_RSA_DES_64_CBC_SHA, SSL3_CIPHER_EDH_RSA_DES_192_CBC3_SHA, SSL3_CIPHER_ADH_RC4_40_MD5, SSL3_CIPHER_ADH_RC4_128_MD5,
		             SSL3_CIPHER_ADH_DES_40_CBC_SHA, SSL3_CIPHER_FZA_DMS_NULL_SHA, SSL3_CIPHER_CK_FZA_DMS_FZA_SHA, SSL3_CIPHER_CK_FZA_DMS_RC4_SHA,
		             SSL3_CIPHER_CK_ADH_DES_64_CBC_SHA, SSL3_CIPHER_CK_ADH_DES_192_CBC_SHA, 0x0};

	cls_printf("测试%s...", TESTAPI); 

	memset(&ss_addr, 0, sizeof(ss_addr));
	ss_addr.unAddrType=SSL_ADDR_IPV4;	
	ss_addr.usPort=netsetting.SvrTcpPort;
	ss_addr.psAddr=netsetting.SvrIP.Str;

	//测试前置:
	if(NetUp()!=SUCC)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	//证书存在性
	if(cls_show_msg("确认已下载%s.按Enter,否则按Other",CERTF)!=ENTER)
	{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;			
	}
	if(NDK_FsExist(CERTF)!=NDK_OK)
	{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
	}
	if(cls_show_msg("确认已下载%s.按Enter,否则按Other",CERTF1)!=ENTER)
	{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;			
	}		
	if(NDK_FsExist(CERTF1)!=NDK_OK)
	{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
	}
	if(cls_show_msg("确认已下载%s.按Enter,否则按Other",KEY)!=ENTER)
	{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;			
	}
	if(NDK_FsExist(KEY)!=NDK_OK)
	{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
	}
	if(cls_show_msg("确认已下载%s.按Enter,否则按Other",SCERTF)!=ENTER)
	{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;			
	}
	if(NDK_FsExist(SCERTF)!=NDK_OK)
	{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
	}
	if(cls_show_msg("确认已下载%s.按Enter,否则按Other",KEYERR)!=ENTER)
	{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;			
	}		
	if(NDK_FsExist(KEYERR)!=NDK_OK)
	{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
	}
	if(cls_show_msg("确认已下载%s.按Enter,否则按Other",NOPSWKEY)!=ENTER)
	{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;			
	}		
	if(NDK_FsExist(NOPSWKEY)!=NDK_OK)
	{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
	}

	//case1:auth_opt=SSL_AUTH_CLIENT 在该模式下客户端验证服务器证书，验证结构成功则连接成功，否则失败。
	if((handle=NDK_OpenSSLSocket(HANDSHAKE_SSLv23, SSL_AUTH_CLIENT, ciphers))==NULL)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	//加载本地证书
	if((ret=NDK_LoadClientCertificate(handle,CERTF,SSL_FILE_PEM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//加载私钥
	if((ret=NDK_LoadClientPrivateKey(handle, KEY, SSL_FILE_PEM, PRIKEY_PWD))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SetSSLBlockingMode(handle, NDK_SUSPEND))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LoadServerCertificate(handle, SCERTF, SSL_FILE_PEM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SSLConnect(handle, &ss_addr, TIMEOUT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_CloseSSLSocket(handle))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:auth_opt=SSL_AUTH_CLIENT 在该模式下没有验证服务端证书，应失败返回。
	if((handle=NDK_OpenSSLSocket(HANDSHAKE_SSLv23, SSL_AUTH_CLIENT, ciphers))==NULL)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	//加载本地证书
	if((ret=NDK_LoadClientCertificate(handle,CERTF,SSL_FILE_PEM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//加载私钥
	if((ret=NDK_LoadClientPrivateKey(handle, KEY, SSL_FILE_PEM, PRIKEY_PWD))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SetSSLBlockingMode(handle, NDK_SUSPEND))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SSLConnect(handle, &ss_addr, TIMEOUT))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_CloseSSLSocket(handle))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:auth_opt=SSL_AUTH_CLIENT 验证本地证书(证书格式pem)与私钥文件是否匹配，匹配成功返回。
	if((handle=NDK_OpenSSLSocket(HANDSHAKE_SSLv23, SSL_AUTH_CLIENT, ciphers))==NULL)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_SetSSLBlockingMode(handle, NDK_SUSPEND))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LoadServerCertificate(handle, SCERTF, SSL_FILE_PEM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LoadClientCertificate(handle, CERTF, SSL_FILE_PEM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LoadClientPrivateKey(handle, KEY, SSL_FILE_PEM, PRIKEY_PWD))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_SSLConnect(handle, &ss_addr, TIMEOUT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_CloseSSLSocket(handle))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case4:auth_opt=SSL_AUTH_CLIENT 验证本地证书(证书格式der)与私钥文件是否匹配，不匹配则连接失败返回。
	if((handle=NDK_OpenSSLSocket(HANDSHAKE_SSLv23, SSL_AUTH_CLIENT, ciphers))==NULL)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_SetSSLBlockingMode(handle, NDK_SUSPEND))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LoadServerCertificate(handle, SCERTF, SSL_FILE_PEM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LoadClientCertificate(handle, CERTF1, SSL_FILE_DER))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LoadClientPrivateKey(handle, KEYERR, SSL_FILE_PEM, PRIKEY_PWD))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_SSLConnect(handle, &ss_addr, TIMEOUT))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_CloseSSLSocket(handle))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:明文私钥文件,pwd传NULL
	if((handle=NDK_OpenSSLSocket(HANDSHAKE_SSLv23, SSL_AUTH_CLIENT, ciphers))==NULL)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_SetSSLBlockingMode(handle, NDK_SUSPEND))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LoadServerCertificate(handle, SCERTF, SSL_FILE_PEM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_LoadClientCertificate(handle, CERTF, SSL_FILE_PEM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_LoadClientPrivateKey(handle, NOPSWKEY, SSL_FILE_DER, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SSLConnect(handle, &ss_addr, TIMEOUT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_CloseSSLSocket(handle))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6:异常测试
	//case6.1:句柄为空，应返回NDK_ERR_SSL_PARAM
	if((ret=NDK_LoadClientCertificate(NULL, CERTF, SSL_FILE_PEM))!=NDK_ERR_SSL_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case6.2:证书文件为空，应返回NDK_ERR_SSL_PARAM
	if((handle=NDK_OpenSSLSocket(HANDSHAKE_SSLv23, SSL_AUTH_CLIENT/*SSL_AUTH_CLIENT_FAIL*/, ciphers))==NULL) //linwl20130718,根据NDK V4.3.9变更取消SSL_AUTH_CLIENT_FAIL认证方式，使用支持的认证方式替换验证
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}	
	if((ret=NDK_LoadClientCertificate(handle, NULL, SSL_FILE_PEM))!=NDK_ERR_SSL_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case6.3:证书文件类型不对，应返回NDK_ERR_SSL_MODEUNSUPPORTED
	if((ret=NDK_LoadClientCertificate(handle, CERTF, 234))!=NDK_ERR_SSL_MODEUNSUPPORTED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6.4:句柄为空，应返回NDK_ERR_SSL_PARAM
	if((ret=NDK_LoadClientPrivateKey(NULL, KEY, SSL_FILE_PEM, PRIKEY_PWD))!=NDK_ERR_SSL_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case6.5:证书文件为空，应返回NDK_ERR_SSL_PARAM
	if((ret=NDK_LoadClientPrivateKey(handle, NULL, SSL_FILE_PEM, PRIKEY_PWD))!=NDK_ERR_SSL_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case6.6:私钥文件类型不对，应返回NDK_ERR_SSL_MODEUNSUPPORTED
	if((ret=NDK_LoadClientPrivateKey(handle, KEY, 234, PRIKEY_PWD))!=NDK_ERR_SSL_MODEUNSUPPORTED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 1
	//case6.7:错误password
	if((ret=NDK_LoadClientCertificate(handle, CERTF, SSL_FILE_PEM))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_LoadClientPrivateKey(handle, KEY, SSL_FILE_PEM, PRIKEY_ERRPWD))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif	
#if 1
	//case6.8:私钥文件非明文,但pwd却为NULL
	if((ret=NDK_LoadClientPrivateKey(handle, KEY, SSL_FILE_PEM, NULL))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif	

	//case6.9:句柄为空，应返回orvNOMEMORY
	if((ret=NDK_LoadServerCertificate(NULL, SCERTF, SSL_FILE_PEM))!=NDK_ERR_SSL_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case6.10:证书文件为空，应返回orvNOMEMORY
	if((ret=NDK_LoadServerCertificate(handle, NULL, SSL_FILE_PEM))!=NDK_ERR_SSL_PARAM)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case6.11:证书文件类型不对，应返回NDK_ERR_SSL_MODEUNSUPPORTED
	if((ret=NDK_LoadServerCertificate(handle, SCERTF, 234))!=NDK_ERR_SSL_MODEUNSUPPORTED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//测试后置:
	NDK_CloseSSLSocket(handle);
	if(NetDown()!=SUCC)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_CloseSSLSocket(handle);
	NetDown();
	return;
}

