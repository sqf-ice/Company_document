/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sslƒ£øÈ
* file name		: ssl6.c
* Author 			: chenfm
* version			: 
* DATE			: 20120905
* directory 		: 
* description		: ≤‚ ‘NDK_SSLDisconnectΩ”ø⁄π¶ƒ‹
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"NDK_SSLDisconnect"
#define TIMEOUT 60*1000   //(ms)
#define CERTF   "clientcert.pem"
#define KEY     "clientkey.pem"
#define PRIKEY_PWD	"123456"

/****************************************************************
* function name 	 	:
* functional description 	: 
* input parameter	 	:
* output parameter	 	:
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	  chenfm      20120905	created
*****************************************************************/
void ssl5(void)
{
	int ret;
	size_t send_bye=0, recvlen=0;
	char buf[255];
	NDK_HANDLE handle;
	ST_SOCKET_ADDR ss_addr;
	int ciphers[]={SSL3_CIPHER_RSA_NULL_MD5, SSL3_CIPHER_RSA_NULL_SHA, SSL3_CIPHER_RSA_RC4_40_MD5, SSL3_CIPHER_RSA_RC4_128_MD5, SSL3_CIPHER_RSA_RC4_128_SHA,
		             SSL3_CIPHER_RSA_RC2_40_MD5, SSL3_CIPHER_RSA_IDEA_128_SHA, SSL3_CIPHER_RSA_DES_40_CBC_SHA, SSL3_CIPHER_RSA_DES_64_CBC_SHA,
		             SSL3_CIPHER_RSA_DES_192_CBC3_SHA, SSL3_CIPHER_DH_RSA_DES_192_CBC3_SHA, SSL3_CIPHER_DH_DSS_DES_40_CBC_SHA, SSL3_CIPHER_DH_DSS_DES_64_CBC_SHA,
		             SSL3_CIPHER_DH_DSS_DES_192_CBC3_SHA, SSL3_CIPHER_DH_RSA_DES_40_CBC_SHA, SSL3_CIPHER_DH_RSA_DES_64_CBC_SHA, SSL3_CIPHER_EDH_DSS_DES_40_CBC_SHA,
		             SSL3_CIPHER_EDH_DSS_DES_64_CBC_SHA, SSL3_CIPHER_EDH_DSS_DES_192_CBC3_SHA, SSL3_CIPHER_EDH_RSA_DES_40_CBC_SHA,
		             SSL3_CIPHER_EDH_RSA_DES_64_CBC_SHA, SSL3_CIPHER_EDH_RSA_DES_192_CBC3_SHA, SSL3_CIPHER_ADH_RC4_40_MD5, SSL3_CIPHER_ADH_RC4_128_MD5,
		             SSL3_CIPHER_ADH_DES_40_CBC_SHA, SSL3_CIPHER_FZA_DMS_NULL_SHA, SSL3_CIPHER_CK_FZA_DMS_FZA_SHA, SSL3_CIPHER_CK_FZA_DMS_RC4_SHA,
		             SSL3_CIPHER_CK_ADH_DES_64_CBC_SHA, SSL3_CIPHER_CK_ADH_DES_192_CBC_SHA, 0x0};

	cls_printf("≤‚ ‘%s...", TESTAPI);

	memset(&ss_addr, 0, sizeof(ss_addr));
	ss_addr.unAddrType=SSL_ADDR_IPV4;	
	ss_addr.usPort=netsetting.SvrTcpPort;
	ss_addr.psAddr=netsetting.SvrIP.Str;

	//≤‚ ‘«∞÷√:
	if(NetUp()!=SUCC)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹", __LINE__, TESTAPI);
		goto ERR;
	}
	// π”√HANDSHAKE_SSLv23
	if((handle=NDK_OpenSSLSocket(HANDSHAKE_SSLv23, SSL_AUTH_NONE, ciphers))==NULL)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹", __LINE__, TESTAPI);
		GOTOERR;
	}
	//º”‘ÿ±æµÿ÷§ È
	if((ret=NDK_LoadClientCertificate(handle,CERTF,SSL_FILE_PEM))!=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//º”‘ÿÀΩ‘ø
	if((ret=NDK_LoadClientPrivateKey(handle, KEY, SSL_FILE_PEM, PRIKEY_PWD))!=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SetSSLBlockingMode(handle, NDK_SUSPEND);
	
	//case1:‘⁄NDK_SSLConnect÷Æ∫ÛNDK_SSLDisconnect£¨÷ÿ–¬NDK_SSLConnect”¶≥…π¶£¨≤¢ƒ‹ ’∑¢ ˝æ›°£
	if((ret=NDK_SSLConnect(handle, &ss_addr, TIMEOUT))!=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SSLDisconnect(handle))!=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SSLSend(handle, "hello world", strlen("hello world"), &send_bye))!=NDK_ERR)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SSLConnect(handle, &ss_addr, TIMEOUT))!=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SSLSend(handle, (const char*)("Hello World!"), strlen("Hello World!"), &send_bye))!=NDK_OK||send_bye!=strlen("Hello World!"))
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d,%d)", __LINE__, TESTAPI, ret, send_bye);
		GOTOERR;
	}
	memset(buf, 0, sizeof(buf));
	if((ret=NDK_SSLDataAvailable(handle, 5000))!=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_SSLReceive(handle, (unsigned char*)buf, send_bye, &recvlen))!=NDK_OK||send_bye!=recvlen)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d,%d)", __LINE__, TESTAPI, ret, recvlen);
		GOTOERR;
	}
	if((ret=NDK_SSLDisconnect(handle))!=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:≤ª’˝»∑ π”√£®¥´“ª∏ˆø’µƒSSLæ‰±˙£©,”¶ ß∞‹∑µªÿ°£
	if((ret=NDK_SSLDisconnect(NULL))!=NDK_ERR_SSL_PARAM)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if((ret=NDK_CloseSSLSocket(handle))!=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//≤‚ ‘∫Û÷√
	if(NetDown()!=SUCC)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹", __LINE__, TESTAPI);
		goto ERR;
	}

	//≤‚ ‘Ω· ¯
	send_result("%s≤‚ ‘Õ®π˝", TESTAPI);
	return;
ERR:
	NDK_CloseSSLSocket(handle);
	NetDown();
	return;
}

