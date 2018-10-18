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
 *@brief    ����des
 *@param    psDataIn    �������ݻ���
 *@param    psKey       ��Կ����,����8,16,24
 *@param    nKeyLen     ��Կ���ȣ�ֵֻ��Ϊ8,16,24
 *@param    nMode       ����ģʽ �μ�\ref ALG_TDS_MODE "ALG_TDS_MODE"
 *@retval   psDataOut   �������
 *@return
 *@li   NDK_OK              �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psDataIn/psDataOut/psKeyΪNULL����Կ����ֵ����8/16/24������ģʽ�Ƿ�)
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
 *@brief    ����shaX
 *@param    psDataIn    ��������
 *@param    nInlen      ���ݳ���
 *@param    nMode       ����ģʽ(0,sha1,1,sha256,2,sha512)
 *@retval   psDataOut   ������ݣ�sha1����������Ϊ20�ֽڣ�
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psDataIn/psDataOutΪNULL��nInlen<0������ģʽ�Ƿ�)
 *@li   NDK_ERR         ����ʧ��
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

	//ʹEVP_Digestϵ�к���֧��������Ч����ϢժҪ�㷨
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

	//ʹ��md���㷨�ṹ����ctx�ṹ��implΪNULL����ʹ��ȱʡʵ�ֵ��㷨��openssl�����ṩ����ϢժҪ�㷨��
	ret = EVP_DigestInit_ex(&ctx, md, NULL);
	if (ret!=1) {
		return NDK_ERR;
	}

	//���Զ�ε��øú����������������ݣ�����ֻ������һ��
	ret = EVP_DigestUpdate(&ctx, psDataIn, nInlen);
	if (ret != 1) {
		return NDK_ERR;
	}

	//�����ϢժҪ������̣�����ɵ�ժҪ��Ϣ�洢��pvDataOut����,������Ϣ�洢��len����
	ret = EVP_DigestFinal_ex(&ctx, psDataOut, &len);
	if (ret != 1) {
		return NDK_ERR;
	}

	EVP_MD_CTX_cleanup(&ctx);
	return NDK_OK;
}

/**
 *@brief    ����sha1
 *@param    psDataIn    ��������
 *@param    nInlen      ���ݳ���
 *@retval   psDataOut   ������ݣ�sha1����������Ϊ20�ֽڣ�
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psDataIn/psDataOutΪNULL��nInlen<0������ģʽ�Ƿ�)
 *@li   NDK_ERR         ����ʧ��
 */
int NDK_AlgSHA1(uchar *psDataIn, int nInlen, uchar *psDataOut)
{
	return NDK_AlgSHAX(psDataIn, nInlen, psDataOut, 0);
}

/**
 *@brief    ����sha256
 *@param    psDataIn    ��������
 *@param    nInlen      ���ݳ���
 *@retval   psDataOut   ������ݣ�sha256����������Ϊ  �ֽڣ�
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psDataIn/psDataOutΪNULL��nInlen<0������ģʽ�Ƿ�)
 *@li   NDK_ERR         ����ʧ��
 */
int NDK_AlgSHA256(uchar *psDataIn, int nInlen, uchar *psDataOut)
{
	return NDK_AlgSHAX(psDataIn, nInlen, psDataOut, 1);
}

/**
 *@brief    ����sha512
 *@param    psDataIn    ��������
 *@param    unInlen     ����ģʽ
 *@retval   psDataOut   ������ݣ�sha512����������Ϊ �ֽڣ�
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(psDataIn/psDataOutΪNULL��nInlen<0������ģʽ�Ƿ�)
 *@li   NDK_ERR         ����ʧ��
 */
int NDK_AlgSHA512(uchar *psDataIn, int nInlen, uchar *psDataOut)
{
	return NDK_AlgSHAX(psDataIn, nInlen, psDataOut, 2);
}

/**
 *@brief    RSA��Կ������
 *@param    nProtoKeyBit    ��Կλ������ǰ֧��512��1024��2048λ �ο�\ref EM_RSA_KEY_LEN "EM_RSA_KEY_LEN"
 *@param    nPubEType       ָ�����ͣ��ο�\ref EM_RSA_EXP "EM_RSA_EXP"
 *@retval   pstPublicKeyOut ��Կ
 *@retval   pstPrivateKeyOut    ˽Կ
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(nProtoKeyBit��Կλ���Ƿ���pstPublicKeyOut\pstPrivateKeyOutΪNULL��nPubETypeָ�����ͷǷ�)
 *@li   NDK_ERR         ����ʧ��
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
 *@brief    RSA��Կ�Լӽ���
 *@details  �ú�������RSA���ܻ��������,���ܻ����ͨ��ѡ�ò�ͬ����Կʵ�֡���(Modul,Exp)ѡ��˽����Կ,����м���;��ѡ�ù�����Կ,����н��ܡ�
 psDataIn�ĵ�һ���ֽڱ���С��psModule�ĵ�һ���ֽڡ� �ú�����ʵ�ֳ��Ȳ�����2048 bits ��RSA���㡣
 ��������ݿ��ٵĻ�������ģ����+1��
 *@param    psModule        ģ����,�ַ�������ʽ����,��"31323334"
 *@param    nModuleLen  ģ�ĳ��� ֻ������ѡ��512/8,1024/8,2048/8
 *@param    psExp           ���RSA�����ָ��������ָ�롣����e.����λ��ǰ,��λ�ں��˳��洢,��"10001"
 *@param    psDataIn        ���ݻ���,�������Ĵ�С���ģ�ĳ��ȴ�1
 *@retval   psDataOut       �������,����Ļ�������С������Ļ�������ͬ.
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(nModuleLenģ�ĳ��ȷǷ���psModule\psExp\psDataIn\psDataOutΪNULL)
 *@li   NDK_ERR         ����ʧ��
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
 *@brief    RSA��Կ��У��
 *@param    pstPublicKey        ��Կ
 *@param    pstPrivateKey       ˽Կ
 *@return
 *@li   NDK_OK          �����ɹ�
 *@li   NDK_ERR_PARA        �����Ƿ�(pstPublicKey\pstPrivateKeyΪNULL)
 *@li   NDK_ERR         ����ʧ��
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
 * ����ԭ�� NDK_AlgSM2KeyPairGen( unsigned char *eccpubkey, unsigned char *eccprikey )
 * ����˵�� ����SM2��Կ��
 * ����˵��
 * �������
 *     [OUT]  *eccpubKey         ��Կ 64�ֽ�
 *     [OUT]  *eccprikey         ˽Կ 32�ֽ�
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
 */
int NDK_AlgSM2KeyPairGen( unsigned char *eccpubkey, unsigned char *eccprikey )
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM2KeyPairGen(eccpubkey, eccprikey);
}

/*
 * ����ԭ�� NDK_AlgSM2Encrypt( unsigned char *eccpubkey, unsigned char *Message, unsigned short MessageLen, unsigned short *Crypto, unsigned short *CryptoLen )
 * ����˵�� Crypto=Enc(msg,key)
 * ����˵��
 *     [IN]   *eccpubkey         ���ܹ�Կ
 *     [IN]   *Message           ��������
 *     [IN]   MessageLen         ���ݳ���
 *     [OUT]  *Crypto            ��������(����C1C3C2��˳���������)
 *     [OUT]  *CryptoLen         �������ݳ���(�������ݳ��ȱ��������ݳ�96���ֽ�)
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ� (����Ϊ�ա����ĳ��� >��1024 - 96���ֽ�)
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
 */
int NDK_AlgSM2Encrypt( unsigned char *eccpubkey, unsigned char *Message, unsigned short MessageLen, unsigned char *Crypto, unsigned short *CryptoLen )
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM2Encrypt( eccpubkey, Message, MessageLen, Crypto, CryptoLen);
}

/*
 * ��飺SM2���ܺ�����Ŀǰ�汾Ӧ�Ե�����Ӧ��C1C3C2����
 * ����ԭ�� NDK_AlgSM2Decrypt( unsigned char *eccprikey, unsigned char *Crypto, unsigned short CryptoLen, unsigned char *Message, unsigned short *MessageLen )
 * ����˵�� ��ԿЭ��
 * ����˵��
 *     [IN]   *eccprikey         ����˽Կ
 *     [IN]   *Crypto            ��������
 *     [IN]   CryptoLen          �������ݳ���
 *     [OUT]  *Message           ��������
 *     [OUT]  *MessageLen        ���ݳ���
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ�(����Ϊ�ա����ĳ��� > 1024�ֽ�)
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
 */
int NDK_AlgSM2Decrypt( unsigned char *eccprikey, unsigned char *Crypto, unsigned short CryptoLen, unsigned char *Message, unsigned short *MessageLen )
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM2Decrypt(eccprikey, Crypto, CryptoLen, Message, MessageLen);
}


/*
 * ��飺SM2ǩ������ժҪ���ɹ��ܣ���Ҫֱ�����������ϵ�e
 * ����ԭ�� NDK_AlgSM2Sign(unsigned char *eccprikey, unsigned char *e, unsigned char *output )
 * ����˵�� (r,s)=sign(e,key)
 * ����˵��
 *     [IN]   *eccprikey         ǩ��˽Կ
 *     [IN]   *e                 ��ǩ�����ݵ�ժҪֵ��32�ֽڣ�
 *     [OUT]  *output            ǩ�������ݣ�64�ֽڣ�
 * ��ֵ˵��
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ� ������Ϊ�գ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
 */
int NDK_AlgSM2Sign(unsigned char *eccprikey, unsigned char *e, unsigned char *output )
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM2Sign(eccprikey, e, output);
}

/*
 * ��飺SM2��ǩ����
 * ����ԭ�� unsigned char NDK_AlgSM2Verify( unsigned char *pPublicKey, unsigned char *e, unsigned char *pSignedData )
 * ����˵�� if(signarture is ture)
 * ����˵��
 *     [IN]   *pPublicKey        ��֤��Կ
 *     [IN]   *e                 ��ǩ�����ݵ�ժҪֵ��32�ֽڣ�
 *     [IN]   *pSignedData       ǩ�������ݣ�64�ֽڣ�
 * ��ֵ˵�� -  NDK_OK ��ǩ�ɹ�
 *             NDK_ERR_PARA �����Ƿ�(�������Ϊ��)
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ��ǩʧ��
 */
int NDK_AlgSM2Verify( unsigned char *pPublicKey, unsigned char *e, unsigned char *pSignedData )
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM2Verify(pPublicKey,  e, pSignedData);
}

/*
 * ��飺����ǩ��ժҪ����
 * ����ԭ�� int NDK_AlgSM2GenE( unsigned short usID, unsigned char *pID, unsigned short usM, unsigned char *pM, unsigned char *pubKey, unsigned char *pHashData);
 * ����˵�� ��������ID,Message�͹�Կ�����������ǩ����ժҪ����
 * ����˵��
 *     [IN]  usID    ID����
 *     [IN]  pID     ID��������(*������ΪNULLʱ,ʹ��PBOC3.0Ĭ��ID������)
 *     [IN]  usM     Message����
 *     [IN]  pM      Message��������
 *     [IN]  pubKey  ��Կ��������
 *     [OUT] pHashData: ����������������ǩ����32�ֽ�ժҪ
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ�����pID���������Ϊ�գ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
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
 * ����ԭ�� NDK_AlgSM3Start( void )
 * ����˵�� ����CCP����
 * ����˵��
 *       ��
 * �������
 *       ��
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
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
 * ����ԭ�� NDK_AlgSM3Update( unsigned char *pDat )
 * ����˵�� updateһ���������ݣ�����Ϊ64�ֽ�������
 * ����˵��
 *       [IN]    pDat һ���������ݣ�64�ֽ�������
 * �������
 *       ��
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR_PARA �����Ƿ�
 *             NDK_ERR ����ʧ��
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
 * ����ԭ�� NDK_AlgSM3Final( unsigned char *pDat, unsigned char len, unsigned char *pHashDat )
 * ����˵�� �������һ�����ݣ����ժҪ
 * ����˵��
 *       [IN]    pDat    ���һ����������
 *       [IN]    len     ���һ�����ݳ���
 * �������
 *       [OUT]   pHashDat ���ժҪ���ݣ�32�ֽ�
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
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
 * ����ԭ�� NDK_AlgSM3Compute( unsigned char *pDat, unsigned char len, unsigned char *pHashDat )
 * ����˵�� �����������ݵ�ժҪ�������ڲ������䣬���ժҪ
 * ����˵��
 *       [IN]    pDat    ��������
 *       [IN]    len     �������ݳ���
 * �������
 *       [OUT]   pHashDat ���ժҪ���ݣ�32�ֽ�
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ�������Ϊ�գ�
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
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
 * ����ԭ�� int NDK_AlgSM4Compute(unsigned char *pKey, unsigned char *pIVector, unsigned int len, unsigned char *pSm4Input, unsigned char *pSm4Output, unsigned char mode);
 * ����˵�� �����������Կ��ģʽ����������ݣ�16�ֽڣ�����SM4����
 * ����˵��
 *       [IN]    pKey    ������Կ������Ϊ16�ֽ�
 *       [IN]    pIVector    ��ʼ����������Ϊ16�ֽڣ�ECBģʽ����Ϊ�գ�
 *       [IN]    len �������ݳ���
 *       [IN]    pSm4Input   ��������
 *       [IN]    mode    ����ģʽ(�ο���EM_ALG_SM4��)
 * �������
 *       [OUT]   pSm4Output ������ݣ�16�ֽ�
 * ��ֵ˵�� -  NDK_OK �����ɹ�
 *             NDK_ERR_PARA �����Ƿ�������Ϊ�գ����ݳ��Ȳ���16����������ģʽ�Ƿ���
 *             NDK_ERR_OPEN_DEV  ���豸�ļ�ʧ��
 *             NDK_ERR ����ʧ��
 */
int NDK_AlgSM4Compute(unsigned char *pKey, unsigned char *pIVector, unsigned int len, unsigned char *pSm4Input, unsigned char *pSm4Output, unsigned char mode)
{
	int iret = 0;

	if((iret = SmAlgInit()) != 0)
		return iret;

	return sm_alg_func -> SM4Compute(pKey, pIVector, len, pSm4Input, pSm4Output, mode);
}



