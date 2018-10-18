#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "NDK.h"
#include "NDK_debug.h"

#include "openssl/safestack.h"
#include "openssl/crypto.h"
#include "openssl/bio.h"
#include "openssl/evp.h"
#include "openssl/rand.h"
#include "openssl/bn.h"
#include "openssl/md4.h"
#include "openssl/md5.h"
#include "openssl/sha.h"
#include "openssl/rsa.h"
#include "openssl/objects.h"
#include "openssl/err.h"
#include "openssl/pem.h"
#include "openssl/ossl_typ.h"
#include "openssl/aes.h"

#include "gm_drv.h"

extern int NDK_des(int direction, const uchar *ibuf, uchar *obuf ,const uchar *ikey, int keylen);

/**
 *@brief    计算des
 *@param    psDataIn    加密数据缓冲
 *@param    psKey       密钥缓冲,长度8,16,24
 *@param    nKeyLen     密钥长度，值只能为8,16,24
 *@param    nMode       加密模式 参见\ref ALG_TDS_MODE "ALG_TDS_MODE"
 *@retval   psDataOut   输出数据
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA        参数非法(psDataIn/psDataOut/psKey为NULL、密钥长度值不是8/16/24、加密模式非法)
 */
int NDK_AlgTDes(uchar *psDataIn, uchar *psDataOut, uchar *psKey, int nKeyLen, int nMode)
{
	if ((psDataIn==NULL) || (psDataOut==NULL) || (psKey==NULL)) {
		return NDK_ERR_PARA;
	}
	if ((nKeyLen!=8) && (nKeyLen!=16 )&& (nKeyLen!=24)) {
		return NDK_ERR_PARA;
	}
	if((nMode!=ALG_TDS_MODE_ENC) && (nMode!=ALG_TDS_MODE_DEC)) {
		return NDK_ERR_PARA;
	}
	return NDK_des(nMode, psDataIn, psDataOut, psKey, nKeyLen);
}

/**
 *@brief    计算shaX
 *@param    psDataIn    输入数据
 *@param    nInlen      数据长度
 *@param    nMode       加密模式(0,sha1,1,sha256,2,sha512)
 *@retval   psDataOut   输出数据（sha1计算结果长度为20字节）
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(psDataIn/psDataOut为NULL、nInlen<0、加密模式非法)
 *@li   NDK_ERR         操作失败
 */
static int NDK_AlgSHAX(uchar *psDataIn, int nInlen, uchar *psDataOut, int nMode)
{
	int ret;
	uint len;
	EVP_MD_CTX ctx;
	const EVP_MD *md;

	if ((psDataIn==NULL) || (psDataOut==NULL) || (nInlen<0)) {
		return NDK_ERR_PARA;
	}

	EVP_MD_CTX_init(&ctx);

	//使EVP_Digest系列函数支持所有有效的信息摘要算法
	OpenSSL_add_all_digests();

	switch(nMode) {
		case 0:
			md = EVP_sha1();
			break;
		case 1:
			md = EVP_sha256();
			break;
		case 2:
			md = EVP_sha512();
			break;
		default :
			return NDK_ERR_PARA;
	}

	//使用md的算法结构设置ctx结构，impl为NULL，即使用缺省实现的算法（openssl本身提供的信息摘要算法）
	ret = EVP_DigestInit_ex(&ctx, md, NULL);
	if (ret!=1) {
		return NDK_ERR;
	}

	//可以多次调用该函数，处理更多的数据，这里只调用了一次
	ret = EVP_DigestUpdate(&ctx, psDataIn, nInlen);
	if (ret != 1) {
		return NDK_ERR;
	}

	//完成信息摘要计算过程，将完成的摘要信息存储在pvDataOut里面,长度信息存储在len里面
	ret = EVP_DigestFinal_ex(&ctx, psDataOut, &len);
	if (ret != 1) {
		return NDK_ERR;
	}

	EVP_MD_CTX_cleanup(&ctx);
	return NDK_OK;
}

/**
 *@brief    计算sha1
 *@param    psDataIn    输入数据
 *@param    nInlen      数据长度
 *@retval   psDataOut   输出数据（sha1计算结果长度为20字节）
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(psDataIn/psDataOut为NULL、nInlen<0、加密模式非法)
 *@li   NDK_ERR         操作失败
 */
int NDK_AlgSHA1(uchar *psDataIn, int nInlen, uchar *psDataOut)
{
	return NDK_AlgSHAX(psDataIn, nInlen, psDataOut, 0);
}

/**
 *@brief    计算sha256
 *@param    psDataIn    输入数据
 *@param    nInlen      数据长度
 *@retval   psDataOut   输出数据（sha256计算结果长度为  字节）
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(psDataIn/psDataOut为NULL、nInlen<0、加密模式非法)
 *@li   NDK_ERR         操作失败
 */
int NDK_AlgSHA256(uchar *psDataIn, int nInlen, uchar *psDataOut)
{
	return NDK_AlgSHAX(psDataIn, nInlen, psDataOut, 1);
}

/**
 *@brief    计算sha512
 *@param    psDataIn    输入数据
 *@param    unInlen     加密模式
 *@retval   psDataOut   输出数据（sha512计算结果长度为 字节）
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(psDataIn/psDataOut为NULL、nInlen<0、加密模式非法)
 *@li   NDK_ERR         操作失败
 */
int NDK_AlgSHA512(uchar *psDataIn, int nInlen, uchar *psDataOut)
{
	return NDK_AlgSHAX(psDataIn, nInlen, psDataOut, 2);
}

/**
 *@brief    RSA密钥对生成
 *@param    nProtoKeyBit    密钥位数，当前支持512、1024和2048位 参考\ref EM_RSA_KEY_LEN "EM_RSA_KEY_LEN"
 *@param    nPubEType       指数类型，参考\ref EM_RSA_EXP "EM_RSA_EXP"
 *@retval   pstPublicKeyOut 公钥
 *@retval   pstPrivateKeyOut    私钥
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(nProtoKeyBit密钥位数非法、pstPublicKeyOut\pstPrivateKeyOut为NULL、nPubEType指数类型非法)
 *@li   NDK_ERR         操作失败
 */
int NDK_AlgRSAKeyPairGen( int nProtoKeyBit, int nPubEType, ST_RSA_PUBLIC_KEY *pstPublicKeyOut, ST_RSA_PRIVATE_KEY *pstPrivateKeyOut)
{
	int bits,ret;
	RSA *r;
	unsigned long e;
	BIGNUM *bne;

	if ((nProtoKeyBit!=RSA_KEY_LEN_512) && (nProtoKeyBit!=RSA_KEY_LEN_1024) && (nProtoKeyBit!=RSA_KEY_LEN_2048)) {
		return  NDK_ERR_PARA;
	}
	if((pstPublicKeyOut==NULL)||(pstPrivateKeyOut==NULL)) {
		return  NDK_ERR_PARA;
	}
	switch(nPubEType) {
		case RSA_EXP_3:
			e = RSA_3;
			break;
		case RSA_EXP_10001:
			e = RSA_F4;
			break;
		default:
			return NDK_ERR_PARA;
	}

	bne = BN_new();
	ret = BN_set_word(bne,e);
	bits = nProtoKeyBit;
	r = RSA_new();
	ret = RSA_generate_key_ex(r, bits, bne, NULL);
	if (ret!=1) {
		return NDK_ERR;
	}
	pstPublicKeyOut->bits = BN_num_bits(r->n);
	memcpy(pstPublicKeyOut->modulus,BN_bn2hex(r->n),strlen(BN_bn2hex(r->n)));
	memcpy(pstPublicKeyOut->exponent,BN_bn2hex(r->e),strlen(BN_bn2hex(r->e)));
	pstPrivateKeyOut->bits = BN_num_bits(r->n);
	memcpy(pstPrivateKeyOut->modulus,BN_bn2hex(r->n),strlen(BN_bn2hex(r->n)));
	memcpy(pstPrivateKeyOut->publicExponent,BN_bn2hex(r->e),strlen(BN_bn2hex(r->e)));
	memcpy(pstPrivateKeyOut->exponent,BN_bn2hex(r->d),strlen(BN_bn2hex(r->d)));
	memcpy(pstPrivateKeyOut->prime[0],BN_bn2hex(r->p),strlen(BN_bn2hex(r->p)));
	memcpy(pstPrivateKeyOut->prime[1],BN_bn2hex(r->q),strlen(BN_bn2hex(r->q)));
	return NDK_OK;
}

/**
 *@brief    RSA密钥对加解密
 *@details  该函数进行RSA加密或解密运算,加密或解密通过选用不同的密钥实现。如(Modul,Exp)选用私有密钥,则进行加密;如选用公开密钥,则进行解密。
 psDataIn的第一个字节必须小于psModule的第一个字节。 该函数可实现长度不超过2048 bits 的RSA运算。
 输入的数据开辟的缓冲须是模长度+1。
 *@param    psModule        模缓冲,字符串的形式存入,如"31323334"
 *@param    nModuleLen  模的长度 只有三种选择512/8,1024/8,2048/8
 *@param    psExp           存放RSA运算的指数缓冲区指针。就是e.按高位在前,低位在后的顺序存储,如"10001"
 *@param    psDataIn        数据缓冲,缓冲区的大小须比模的长度大1
 *@retval   psDataOut       输出数据,输出的缓冲区大小和输入的缓冲区相同.
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(nModuleLen模的长度非法、psModule\psExp\psDataIn\psDataOut为NULL)
 *@li   NDK_ERR         操作失败
 */
int NDK_AlgRSARecover(uchar *psModule, int nModuleLen, uchar *psExp, uchar *psDataIn,uchar *psDataOut)
{
	int i,j,k;
	BIGNUM *n,*exp,*f,*ret;
	int nRet=NDK_ERR,iret;
	int nModule=nModuleLen;
	uchar Buff[512+1]= {0};
	if ((nModuleLen!=512/8) && (nModuleLen!=1024/8) && (nModuleLen!=2048/8)) {
		return  NDK_ERR_PARA;
	}
	if((psModule==NULL)||(psExp==NULL)||(psDataIn==NULL)||(psDataOut==NULL)) {
		return NDK_ERR_PARA;
	}
	NDK_HexToAsc(psDataIn,nModuleLen*2,1,Buff);
	iret = memcmp(Buff,psModule,nModuleLen*2);
	//fprintf(stderr,"iret = %d nModuleLen=%d\npsDataIn:   %s\npsModule:   %s\n",iret,nModuleLen,Buff,psModule);
	if((iret>0)||(iret==0)) {
		//fprintf(stderr,"iret = %d--------errror!!!------\n",iret);
		return  NDK_ERR_PARA;
	}
	n = BN_new();
	exp = BN_new();
	f = BN_new();
	ret = BN_new();

	BN_CTX *ctx=NULL;
	BN_hex2bn(&n,(char*)psModule);
	BN_hex2bn(&exp,(char*)psExp);

	ctx = BN_CTX_new();
	if (BN_num_bits(n)>2048) {
		goto err;
	}
	if (BN_ucmp(n, exp) <= 0) {
		goto err;
	}
	if (BN_bin2bn(psDataIn, nModule,f) == NULL) {
		goto err;
	}

	if (BN_ucmp(f,n) > 0) {
		goto err;
	}
	nRet = BN_mod_exp(ret, f, exp, n, ctx);
	j = BN_num_bytes(ret);
	i = BN_bn2bin(ret,&(psDataOut[nModule-j]));
	for (k = 0; k < (nModule-i); k++) {
		psDataOut[k]=0;
	}

	if (nRet>=0) {
		nRet = NDK_OK;
	}

err:
	if (ctx != NULL) {
//		BN_CTX_end(ctx);
		BN_CTX_free(ctx);
	}
	BN_free(n);
	BN_free(exp);
	BN_free(f);
	BN_free(ret);
	return nRet;
}

/**
 *@brief    RSA密钥对校验
 *@param    pstPublicKey        公钥
 *@param    pstPrivateKey       私钥
 *@return
 *@li   NDK_OK          操作成功
 *@li   NDK_ERR_PARA        参数非法(pstPublicKey\pstPrivateKey为NULL)
 *@li   NDK_ERR         操作失败
 */
int NDK_AlgRSAKeyPairVerify(ST_RSA_PUBLIC_KEY *pstPublicKey, ST_RSA_PRIVATE_KEY *pstPrivateKey)
{
#if 0
	int len=0;
	BIGNUM *r1=NULL,*r2=NULL;
	BN_CTX *ctx=NULL;
	int nRet=NDK_ERR;
	unsigned char from[513]= {0},to[513]= {0},outer[513]= {0};
	int flen;
	int i;

	if((pstPublicKey == NULL)||(pstPrivateKey == NULL)) {
		fprintf(stderr,"error param !!!\n");
		return NDK_ERR_PARA;
	}
	ctx=BN_CTX_new();
	RSA *r = RSA_new();
	BIGNUM *mod = BN_new();
	BIGNUM *exp = BN_new();
	BIGNUM *pubexp = BN_new();
	BIGNUM *pexp = BN_new();
	BIGNUM *qexp = BN_new();
	BIGNUM *dmp = BN_new();
	BIGNUM *dmq = BN_new();
	BIGNUM *qmp = BN_new();

	if (ctx == NULL) goto err;
	BN_CTX_start(ctx);
	r1 = BN_CTX_get(ctx);
	r2 = BN_CTX_get(ctx);
	BN_hex2bn(&mod,(char *)pstPublicKey->modulus);
	r->n = mod;
	BN_hex2bn(&exp,(char *)pstPrivateKey->exponent);
	r->d = exp;
	BN_hex2bn(&pubexp,(char *)pstPrivateKey->publicExponent);
	r->e = pubexp;
	BN_hex2bn(&pexp,(char *)pstPrivateKey->prime[0]);
	r->p = pexp;
	BN_hex2bn(&qexp,(char *)pstPrivateKey->prime[1]);
	r->q = qexp;

	if (!BN_sub(r1,r->p,BN_value_one())) goto err;  /* p-1 */
	if (!BN_sub(r2,r->q,BN_value_one())) goto err;  /* q-1 */
	/* calculate d mod (p-1) */
	if (!BN_mod(dmp,r->d,r1,ctx)) goto err;
	r->dmp1 = dmp;
	/* calculate d mod (q-1) */
	if (!BN_mod(dmq,r->d,r2,ctx)) goto err;
	r->dmq1 = dmq;
	/* calculate inverse of q mod p */
	if (!BN_mod_inverse(qmp,qexp,pexp,ctx)) goto err;
	r->iqmp = qmp;

	flen=RSA_size(r);
	flen-=11;
	for(i=0; i<flen; i++) {
		from[i] = i&0xff;
	}
	len = RSA_private_encrypt(flen,from,to,r,RSA_PKCS1_PADDING);
	RSA_public_decrypt(len,to,outer,r,RSA_PKCS1_PADDING);
	if (memcmp(from,outer,flen)==0) {
		nRet = 0;
	}
err:
	if (ctx != NULL) {
		BN_CTX_end(ctx);
		BN_CTX_free(ctx);
	}
	BN_free(mod);
	BN_free(exp);
	BN_free(pubexp);
	BN_free(pexp);
	BN_free(qexp);
	BN_free(dmp);
	BN_free(dmq);
	BN_free(qmp);
	//RSA_free(r);
	return nRet;
#endif
	uchar Test_Buff[256+1]= {0};
	uchar Buffer[256+1]= {0},Buff1[256+1]= {0};
	//uchar Temp[1024]={0};
	int ret=0;
	if((pstPublicKey==NULL)||(pstPrivateKey==NULL)) {
		return NDK_ERR_PARA;
	}
	if(0!=memcmp(pstPublicKey->modulus,pstPrivateKey->modulus,strlen((char *)pstPublicKey->modulus)))
		return NDK_ERR;
	memset(Test_Buff,0x01,pstPublicKey->bits/8);
	ret = NDK_AlgRSARecover(pstPublicKey->modulus,pstPublicKey->bits/8,pstPublicKey->exponent,Test_Buff,Buffer);
	if(ret!=0)
		return ret;
	//NDK_HexToAsc(Buffer,pstPublicKey->bits/8,1,Temp);
	//fprintf(stderr,"Temp %d:    %s :\nModule:    %s\n",pstPublicKey->bits/8,Temp,pstPrivateKey->modulus);
	ret = NDK_AlgRSARecover(pstPrivateKey->modulus,pstPrivateKey->bits/8,pstPrivateKey->exponent,Buffer,Buff1);
	if(ret!=0)
		return ret;
	if (0 == strcmp((char *)Test_Buff,(char *)Buff1)) {
		return 0;
	} else {
		return -1;
	}
}

extern int SM2KeyPairGen(unsigned char * eubkey, unsigned char * erikey);
extern  int SM2Encrypt(unsigned char * eubkey, unsigned char * Message, unsigned short MessageLen, unsigned char * Crypto, unsigned short * CryptoLen);
extern  int SM2Decrypt(unsigned char * erikey, unsigned char * Crypto, unsigned short CryptoLen, unsigned char * Message, unsigned short * MessageLen);
extern  int SM2Sign(unsigned char * erikey, unsigned char * e, unsigned char * output);
extern  int SM2Verify(unsigned char * eubkey, unsigned char * e, unsigned char * pSignedData);
extern int SM2GenE(unsigned short usID, unsigned char * pID, unsigned short usM, unsigned char * pM, unsigned char * pubKey, unsigned char * pHashData);
extern int SM3Start(void);
extern int SM3Update(unsigned char * pDat, unsigned int len);
extern int SM3Final(unsigned char * pDat, unsigned int len, unsigned char * pHashDat);
extern int SM3Compute(unsigned char * pDat, unsigned int len, unsigned char * pHashDat);
extern int SM4Compute(unsigned char * pKey, unsigned char * pIVector, unsigned int len, unsigned char * pSm4Input, unsigned char * pSm4Output, unsigned char mode);

extern int ccpSM2KeyPairGen(unsigned char * eubkey, unsigned char * erikey);
extern  int ccpSM2Encrypt(unsigned char * eubkey, unsigned char * Message, unsigned short MessageLen, unsigned char * Crypto, unsigned short * CryptoLen);
extern  int ccpSM2Decrypt(unsigned char * erikey, unsigned char * Crypto, unsigned short CryptoLen, unsigned char * Message, unsigned short * MessageLen);
extern  int ccpSM2Sign(unsigned char * erikey, unsigned char * e, unsigned char * output);
extern  int ccpSM2Verify(unsigned char * eubkey, unsigned char * e, unsigned char * pSignedData);
extern int ccpSM2GenE(unsigned short usID, unsigned char * pID, unsigned short usM, unsigned char * pM, unsigned char * pubKey, unsigned char * pHashData);
extern int ccpSM3Start(void);
extern int ccpSM3Update(unsigned char * pDat, unsigned int len);
extern int ccpSM3Final(unsigned char * pDat, unsigned int len, unsigned char * pHashDat);
extern int ccpSM3Compute(unsigned char * pDat, unsigned int len, unsigned char * pHashDat);
extern int ccpSM4Compute(unsigned char * pKey, unsigned char * pIVector, unsigned int len, unsigned char * pSm4Input, unsigned char * pSm4Output, unsigned char mode);


typedef struct {
	int (*SM2KeyPairGen)(unsigned char * eubkey, unsigned char * erikey);
	int (*SM2Encrypt)(unsigned char * eubkey, unsigned char * Message, unsigned short MessageLen, unsigned char * Crypto, unsigned short * CryptoLen);
	int (*SM2Decrypt)(unsigned char * erikey, unsigned char * Crypto, unsigned short CryptoLen, unsigned char * Message, unsigned short * MessageLen);
	int (*SM2Sign)(unsigned char * erikey, unsigned char * e, unsigned char * output);
	int (*SM2Verify)(unsigned char * eubkey, unsigned char * e, unsigned char * pSignedData);
	int (*SM2GenE)(unsigned short usID, unsigned char * pID, unsigned short usM, unsigned char * pM, unsigned char * pubKey, unsigned char * pHashData);
	int (*SM3Start)(void);
	int (*SM3Update)(unsigned char * pDat, unsigned int len);
	int (*SM3Final)(unsigned char * pDat, unsigned int len, unsigned char * pHashDat);
	int (*SM3Compute)(unsigned char * pDat, unsigned int len, unsigned char * pHashDat);
	int (*SM4Compute)(unsigned char * pKey, unsigned char * pIVector, unsigned int len, unsigned char * pSm4Input, unsigned char * pSm4Output, unsigned char mode);
} sm_alg_func_t;

sm_alg_func_t *sm_alg_func;

const sm_alg_func_t sm_hw_func = {
	.SM2KeyPairGen = ccpSM2KeyPairGen,
	.SM2Encrypt = ccpSM2Encrypt,
	.SM2Decrypt = ccpSM2Decrypt,
	.SM2Sign = ccpSM2Sign,
	.SM2Verify = ccpSM2Verify,
	.SM2GenE = ccpSM2GenE,
	.SM3Start = ccpSM3Start,
	.SM3Update = ccpSM3Update,
	.SM3Final = ccpSM3Final,
	.SM3Compute = ccpSM3Compute,
	.SM4Compute = ccpSM4Compute,
};

const sm_alg_func_t sm_sf_func = {
	.SM2KeyPairGen = SM2KeyPairGen,
	.SM2Encrypt = SM2Encrypt,
	.SM2Decrypt = SM2Decrypt,
	.SM2Sign = SM2Sign,
	.SM2Verify = SM2Verify,
	.SM2GenE = SM2GenE,
	.SM3Start = SM3Start,
	.SM3Update = SM3Update,
	.SM3Final = SM3Final,
	.SM3Compute = SM3Compute,
	.SM4Compute = SM4Compute,
};

int SmAlgInit(void)
{
    char type[256]= {0};
    static char init_flag = 0;
    char tmp[20];

    if(init_flag == 1)
        return 0;

    ndk_getHardWareInfo(tmp);
    PDEBUG("tmp: %d", tmp[SYS_HWTYPE_GM]);
    if(tmp[SYS_HWTYPE_GM] == 0xff) {
        PDEBUG("*****soft alf***\n");
        sm_alg_func = &sm_sf_func;
        init_flag = 1;
        return 0;
    }

    if(gm_open() != 0)
        return NDK_ERR_OPEN_DEV;

	if(gm_chip_very() != 0)
		return NDK_ERR;

    PDEBUG("****hw alf***\n");
    sm_alg_func = &sm_hw_func;
    init_flag = 1;
    return 0;
}

////////////////////////////// SM2LIB FUNCTION /////////////////////////////////////
/*
 * 函数原型 NDK_AlgSM2KeyPairGen( unsigned char *eccpubkey, unsigned char *eccprikey )
 * 功能说明 生成SM2密钥对
 * 参数说明
 * 输出数据
 *     [OUT]  *eccpubKey         公钥 64字节
 *     [OUT]  *eccprikey         私钥 32字节
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int NDK_AlgSM2KeyPairGen( unsigned char *eccpubkey, unsigned char *eccprikey )
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM2KeyPairGen(eccpubkey, eccprikey);
}

/*
 * 函数原型 NDK_AlgSM2Encrypt( unsigned char *eccpubkey, unsigned char *Message, unsigned short MessageLen, unsigned short *Crypto, unsigned short *CryptoLen )
 * 功能说明 Crypto=Enc(msg,key)
 * 参数说明
 *     [IN]   *eccpubkey         加密公钥
 *     [IN]   *Message           明文数据
 *     [IN]   MessageLen         数据长度
 *     [OUT]  *Crypto            密文数据(按照C1C3C2的顺序进行排列)
 *     [OUT]  *CryptoLen         密文数据长度(密文数据长度比明文数据长96个字节)
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法 (输入为空、明文长度 >（1024 - 96）字节)
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int NDK_AlgSM2Encrypt( unsigned char *eccpubkey, unsigned char *Message, unsigned short MessageLen, unsigned char *Crypto, unsigned short *CryptoLen )
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM2Encrypt( eccpubkey, Message, MessageLen, Crypto, CryptoLen);
}

/*
 * 简介：SM2解密函数，目前版本应对的密文应按C1C3C2排列
 * 函数原型 NDK_AlgSM2Decrypt( unsigned char *eccprikey, unsigned char *Crypto, unsigned short CryptoLen, unsigned char *Message, unsigned short *MessageLen )
 * 功能说明 密钥协商
 * 参数说明
 *     [IN]   *eccprikey         解密私钥
 *     [IN]   *Crypto            密文数据
 *     [IN]   CryptoLen          密文数据长度
 *     [OUT]  *Message           明文数据
 *     [OUT]  *MessageLen        数据长度
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法(输入为空、密文长度 > 1024字节)
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int NDK_AlgSM2Decrypt( unsigned char *eccprikey, unsigned char *Crypto, unsigned short CryptoLen, unsigned char *Message, unsigned short *MessageLen )
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM2Decrypt(eccprikey, Crypto, CryptoLen, Message, MessageLen);
}


/*
 * 简介：SM2签名，无摘要生成功能，需要直接输入计算完毕的e
 * 函数原型 NDK_AlgSM2Sign(unsigned char *eccprikey, unsigned char *e, unsigned char *output )
 * 功能说明 (r,s)=sign(e,key)
 * 参数说明
 *     [IN]   *eccprikey         签名私钥
 *     [IN]   *e                 被签名数据的摘要值（32字节）
 *     [OUT]  *output            签名后数据（64字节）
 * 返值说明
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法 （输入为空）
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int NDK_AlgSM2Sign(unsigned char *eccprikey, unsigned char *e, unsigned char *output )
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM2Sign(eccprikey, e, output);
}

/*
 * 简介：SM2验签函数
 * 函数原型 unsigned char NDK_AlgSM2Verify( unsigned char *pPublicKey, unsigned char *e, unsigned char *pSignedData )
 * 功能说明 if(signarture is ture)
 * 参数说明
 *     [IN]   *pPublicKey        验证公钥
 *     [IN]   *e                 被签名数据的摘要值（32字节）
 *     [IN]   *pSignedData       签名后数据（64字节）
 * 返值说明 -  NDK_OK 验签成功
 *             NDK_ERR_PARA 参数非法(输入参数为空)
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 验签失败
 */
int NDK_AlgSM2Verify( unsigned char *pPublicKey, unsigned char *e, unsigned char *pSignedData )
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM2Verify(pPublicKey,  e, pSignedData);
}

/*
 * 简介：用于签名摘要生成
 * 函数原型 int NDK_AlgSM2GenE( unsigned short usID, unsigned char *pID, unsigned short usM, unsigned char *pM, unsigned char *pubKey, unsigned char *pHashData);
 * 功能说明 根据输入ID,Message和公钥，计算出用于签名的摘要数据
 * 参数说明
 *     [IN]  usID    ID长度
 *     [IN]  pID     ID数据输入(*当传入为NULL时,使用PBOC3.0默认ID做运算)
 *     [IN]  usM     Message长度
 *     [IN]  pM      Message数据输入
 *     [IN]  pubKey  公钥数据输入
 *     [OUT] pHashData: 根据输入计算出用于签名的32字节摘要
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法（除pID外参数输入为空）
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int NDK_AlgSM2GenE( unsigned short usID, unsigned char *pID, unsigned short usM, unsigned char *pM, unsigned char *pubKey, unsigned char *pHashData)
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM2GenE(usID, pID, usM, pM, pubKey, pHashData);
}

////////////////////////////// SM3LIB FUNCTION /////////////////////////////////////
/*
 * 函数原型 NDK_AlgSM3Start( void )
 * 功能说明 启动CCP运算
 * 参数说明
 *       无
 * 输出数据
 *       无
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
static char sm3_flag = 0;
int NDK_AlgSM3Start(void)
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

    iret =  sm_alg_func -> SM3Start();

    if(iret == NDK_OK)
        sm3_flag = 1 ;

    return iret;
}
/*
 * 函数原型 NDK_AlgSM3Update( unsigned char *pDat )
 * 功能说明 update一个分组数据，数据为64字节整数倍
 * 参数说明
 *       [IN]    pDat 一个分组数据，64字节整数倍
 * 输出数据
 *       无
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR_PARA 参数非法
 *             NDK_ERR 操作失败
 */
int NDK_AlgSM3Update( unsigned char *pDat,  unsigned int len )
{
    int iret = 0;

    if(sm3_flag != 1)
        return NDK_ERR;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM3Update(pDat, len);
}

/*
 * 函数原型 NDK_AlgSM3Final( unsigned char *pDat, unsigned char len, unsigned char *pHashDat )
 * 功能说明 计算最后一组数据，输出摘要
 * 参数说明
 *       [IN]    pDat    最后一个分组数据
 *       [IN]    len     最后一组数据长度
 * 输出数据
 *       [OUT]   pHashDat 输出摘要数据，32字节
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int NDK_AlgSM3Final( unsigned char *pDat, unsigned int len, unsigned char *pHashDat )
{
	int iret = 0;

    if(sm3_flag != 1)
        return NDK_ERR;

	//clear flag
	sm3_flag = 0;

    if((iret = SmAlgInit()) != 0)
        return iret;

    return sm_alg_func -> SM3Final(pDat, len, pHashDat);
}

/*
 * 函数原型 NDK_AlgSM3Compute( unsigned char *pDat, unsigned char len, unsigned char *pHashDat )
 * 功能说明 计算输入数据的摘要，函数内部完成填充，输出摘要
 * 参数说明
 *       [IN]    pDat    输入数据
 *       [IN]    len     输入数据长度
 * 输出数据
 *       [OUT]   pHashDat 输出摘要数据，32字节
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法（输入为空）
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int NDK_AlgSM3Compute( unsigned char *pDat, unsigned int len, unsigned char *pHashDat )
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM3Compute(pDat, len, pHashDat);
}

////////////////////////////// SM4LIB FUNCTION /////////////////////////////////////
/*
 * 函数原型 int NDK_AlgSM4Compute(unsigned char *pKey, unsigned char *pIVector, unsigned int len, unsigned char *pSm4Input, unsigned char *pSm4Output, unsigned char mode);
 * 功能说明 根据输入的密钥和模式对输入的数据（16字节）进行SM4运算
 * 参数说明
 *       [IN]    pKey    输入密钥，长度为16字节
 *       [IN]    pIVector    初始向量，长度为16字节（ECB模式允许为空）
 *       [IN]    len 输入数据长度
 *       [IN]    pSm4Input   输入数据
 *       [IN]    mode    运算模式(参考”EM_ALG_SM4“)
 * 输出数据
 *       [OUT]   pSm4Output 输出数据，16字节
 * 返值说明 -  NDK_OK 操作成功
 *             NDK_ERR_PARA 参数非法（输入为空，数据长度不是16的整数倍，模式非法）
 *             NDK_ERR_OPEN_DEV  打开设备文件失败
 *             NDK_ERR 操作失败
 */
int NDK_AlgSM4Compute(unsigned char *pKey, unsigned char *pIVector, unsigned int len, unsigned char *pSm4Input, unsigned char *pSm4Output, unsigned char mode)
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM4Compute(pKey, pIVector, len, pSm4Input, pSm4Output, mode);
}



