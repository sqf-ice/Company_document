#ifndef _GM_H_
#define _GM_H_

//#define WDEBUG
#ifdef WDEBUG
#define PDEBUG(fmt, args...) printf("[%s][%d]"fmt"\n",__FILE__,__LINE__, ##args);//udelay(10*1000);
#define PDEBUG_DATA(info, data, len)        {int i;\
                                        printf("[%s][%d]%s: len=%d data=",__FILE__,__LINE__, (info), (len));\
                                        for(i=0; i<(len); i++){\
                                        printf("%02x ", *(data+i));\
                                        }\
                                        printf("\n");\
                                        }
#else
#define PDEBUG(fmt, args...)
#define PDEBUG_DATA(info, data, len)
#endif

#define PDEBUG_PRINTF(info, data, len)        {int i;\
                                        printf("%s", (info));\
                                        for(i=0; i<(len); i++){\
                                        printf("%02X", *(data+i));\
                                        }\
                                        }



#define GM_DRV_VER "1.0"

#define GM_DEV_STRING  "gm"
#define GM_DEV_MINOR   200

#define GM_DEVICE_NAME      "/dev/gm"   /*安全设备全路径*/

/*
 * IOCTL definitions
 */
#define GM_IOC_MAGIC 'G'
#define GM_IOCG_VER     _IOR(GM_IOC_MAGIC, 0, unsigned int)

/* RNG definitions */
#define GM_IOCS_RNG_WORD        _IOW(GM_IOC_MAGIC, 1, unsigned int)
#define GM_IOCS_RNG_VECTOR      _IOW(GM_IOC_MAGIC, 2, unsigned int)
#define GM_IOCS_RNG_VECTORLEN   _IOW(GM_IOC_MAGIC, 3, unsigned int)
#define GM_IOCG_RNG_LEN _IOR(GM_IOC_MAGIC, 4, unsigned int)

/* GM ALG ENC/DEC */
#define GM_IOCT_CIPHER_INIT _IO(GM_IOC_MAGIC, 1) //算法模块初始化
#define GM_IOCT_CIPHER_GEN_KEY  _IO(GM_IOC_MAGIC, 2) //明文方式、密文方式...
#define GM_IOCT_CIPHER_INPUT_KEY    _IO(GM_IOC_MAGIC, 3)
#define GM_IOCT_CIPHER_OUTPUT_KEY   _IO(GM_IOC_MAGIC, 4)
#define GM_IOCT_CIPHER_SELECT_KEY   _IO(GM_IOC_MAGIC, 5)
#define GM_IOCT_CIPHER_OPERATION  _IO(GM_IOC_MAGIC, 6)
#define GM_IOCT_CIPHER_ROOTKEY_ENC    _IO(GM_IOC_MAGIC, 7)
#define GM_IOCT_CIPHER_SET_TMPKEY    _IO(GM_IOC_MAGIC, 8)
#define GM_IOCT_CIPHER_GET_TMPKEY   _IO(GM_IOC_MAGIC, 9)

/*GM HASH FUNC*/
#define GM_IOCT_HASH_INIT   _IOW(GM_IOC_MAGIC, 10, unsigned char)
#define GM_IOCT_HASH_START  _IO(GM_IOC_MAGIC, 11)
#define GM_IOCT_HASH_UPDATA _IO(GM_IOC_MAGIC, 12)
#define GM_IOCT_HASH_FINAL  _IO(GM_IOC_MAGIC, 13)
#define GM_IOCT_HASH_COMPUTE    _IO(GM_IOC_MAGIC, 14)
#define GM_IOCT_HASH_SM2_GEN_Z  _IO(GM_IOC_MAGIC, 15)

#define GM_IOCT_CIPHER_SM2_CALC  _IO(GM_IOC_MAGIC, 16) //large data
/*SM2/RSA key Generation*/
#define GM_IOCT_CIPHER_GENKEY_SM2   _IO(GM_IOC_MAGIC, 20)
#define GM_IOCT_CIPHER_RSA_INIT   _IOW(GM_IOC_MAGIC, 21, unsigned char)

/*COMMON Operation*/
#define GM_IOCT_CHIP_INIT   _IO(GM_IOC_MAGIC, 30)
#define GM_IOCT_CHIP_VERY   _IO(GM_IOC_MAGIC, 31)
#define GM_IOCT_SWITCH_BOOT_MODE    _IO(GM_IOC_MAGIC, 32)
#define GM_IOCT_POKER_TEST    _IO(GM_IOC_MAGIC, 33)
#define GM_IOCT_CMD_RW    _IO(GM_IOC_MAGIC, 34)

#ifndef BSP_DES_KEY_SIZE_BYTES
#define BSP_DES_KEY_SIZE_BYTES          8
#endif
#ifndef BSP_3DES_KEY_SIZE_BYTES
#define BSP_3DES_KEY_SIZE_BYTES         24
#endif
#define DES_ENCRYPT 0
#define DES_DECRYPT 1

#define VERY_SUCC     0
#define  VERY_FAILED      1

#define GM_VERY_DATA  "NEWLANDTMCTHK88AA55AA55A"
#define VERY_MAX_TRY_TIME 5

extern unsigned char IOBuf[512];

typedef unsigned int    uint;

typedef enum {
    ONLY_PUBKEY = 0,
    ONLY_PRIKEY,
    BOTH_KEYS,
} KEY_OUTPUT_MODE;

typedef enum {
    LOOP_MODE_48M = 0,
    LOOP_MODE_30M = 1,
} GM_CCP_CALC_MODE;

typedef enum {
    SM2_PUB,
    SM2_PRI,
    RSA2048_PUB,
    RSA2048_PRI_STD,
    RSA2048_PRI_CRT,
    SM4_16BYTE_PLAINTEXT,
    SM4_16BYTE_CIPHERTEXT,
    TDES_16BYTE,
    TDES_24BYTE,
    RSA1024_PUB,
    RSA1024_PRI_STD,
    RSA1024_PRI_CRT,
} GM_KEY_TYPE;

typedef enum {
    SM2_ENC,
    SM2_DEC,
    SM2_SIGN_E,
    SM2_VERY_E,
    SM4_ENC_ECB,
    SM4_DEC_ECB,
    SM4_ENC_CBC,
    SM4_DEC_CBC,
    RSA_ENC,
    RSA_DEC,
    SM3_HASH,
    SHA256_HASH,
    SM2_GEN_Z,
    SM2_SET_ID,
    SM2_GEN_E,
    SHA1_HASH,
} GM_ALG_ID;

#if 0
typedef enum {
    SM3_ALG,
    SHA256_ALG,
    SM2_GEN_Z,
} GM_HASH_ID;
#endif

typedef enum {
    ERR_CCP_NOT_INIT = -4000,
    ERR_CCP_PARAM = (ERR_CCP_NOT_INIT - 1),
    ERR_WRONG_SW = (ERR_CCP_NOT_INIT - 2),
    ERR_INTER_FAILED = (ERR_CCP_NOT_INIT - 3),
} GM_ERR;


typedef struct  GM_GET_RANDOM_S {
    unsigned int len;
    unsigned char *random_buf;
} GM_GET_RANDOM;

typedef struct  GM_HASH_START_S {
    char AlgID;
    char Mode;
} GM_HASH_START;

typedef struct  GM_ALG_S {
    char AlgID;
    unsigned int iLen;
    unsigned char *iData;
    unsigned int oLen;
    unsigned char *oData;
} GM_ALG;

typedef struct {
    unsigned int RSA_Len;               //RSA 数据字节长度
    unsigned long RSA_ulE;              //公钥幂指数
    unsigned char RSA_CRT;              // 1:CRT mode 0:no CRT mode
    unsigned char RSA_Key_N[256];           //公钥(大端格式存储)
    unsigned char RSA_Key_D[256];           //私钥(大端格式存储)
    unsigned char RSA_Key_P[128];           //私钥(大端格式存储)
    unsigned char RSA_Key_Q[128];           //私钥(大端格式存储)
    unsigned char RSA_Key_DP[128];          //私钥(大端格式存储)
    unsigned char RSA_Key_DQ[128];          //私钥(大端格式存储)
    unsigned char RSA_Key_QINV[128];        //私钥(大端格式存储)
} stRSAParameter;

typedef struct {
    unsigned int PubLen;
    unsigned char PubKey[64];
    unsigned int PriLen;
    unsigned char PriKey[32];
} stSM2Parameter;

typedef struct  GM_KEY_S {
    char KeyType;
    char KeyID;
    unsigned char *KeyBuf;
} GM_KEY;

typedef struct  GM_HASH_S {
    char AlgID;
    unsigned int iLen;
    unsigned char *iData;
    unsigned int oLen;
    unsigned char *oData;
} GM_HASH;



/* 使用根密钥对32字节数据进行SM4加密的数据结构 */
typedef struct CIPHER_SM4_INTER_KEY_S {
    unsigned char   inData[32];     /*要加密的数据*/
    unsigned char   outData[32];    /*加密后的数据*/
} CIPHER_SM4_INTER_KEY;
int gm_open(void);
int gm_close(void);
int gm_get_random(GM_GET_RANDOM *param);


int gm_chip_init(void);

int gm_chip_very(void);

int gm_gen_sm2key(GM_KEY *param);

int gm_cipher_gen_key(GM_KEY *param);
int gm_cipher_key_input(GM_KEY *param);
int gm_cipher_key_output(GM_KEY *param);
int gm_hash_init(unsigned char Mode);
int gm_hash_start(GM_HASH_START *param);

//缺陷只能单次传入64字节
int gm_hash_updata(GM_ALG *param);
int gm_hash_final(GM_ALG *param);
int gm_cipher_oper(GM_ALG *param);
int gm_cipher_key_select(GM_KEY *param);
int gm_cipher_rootkey_enc(CIPHER_SM4_INTER_KEY *param);

int gm_cipher_set_tmpkey(GM_KEY  *param);

int gm_cipher_get_tmpkey(GM_KEY *param);
int gm_cipher_rsa_init(unsigned char param);

#endif
