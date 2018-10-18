/*
 * FileName:       
 * Author:         zhangnw  Version: 1.6.12  Date: 2009-12-27
 * Description:    
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version>  <desc>
 */
#ifndef SEC_H
#define SEC_H

/*-----------------------includes-------------------------------*/

/*---------------constants/macro definition---------------------*/
#define PINKEY_TYPE			0x05
#define MAINKEYENPKEY_TYPE	0x06
#define MAINKEY_TYPE		0x09
#define MACKEY_TYPE			0x0a
#define TRANSKEY_TYPE		0x0b
#define DUKPT_INITKEY_TYPE	0x12

//#define	TR31_CRYPTLEN		(72)

/*      Required Key Type (Usage + Algorithm + Mode)    */
#define KT_MST_TYPE             "K0TD"
#define KT_MST_TR31_TYPE        "K1TD"
#define KT_PIN_TYPE             "P0TE"
#define KT_MAC_TYPE             "M0TG"
#define KT_PRI_TYPE             "D0RS"
#define KT_MAG_TYPE             "D0TB"
#define KT_BDK_TYPE             "B1TE"

typedef struct{
	unsigned char *key;			/*  key pointer */
	unsigned short keylen;			/* keylen*/
	unsigned char fid;				/* filed ID*/
	unsigned char gid;				/* group ID*/
	unsigned char usage[2];       	/* Key Usage */
    unsigned char algorithm;      	/* The approved algorithm to be used by the protected key */
    unsigned char mode;           	/* the operation the protected key can perform */
    unsigned char ver_num[2];     	/* Key Version Number */
    unsigned char export;         	/* Defines the protected key's exportablility */
	unsigned char ksn[10];			/* Key Serial Number */
	unsigned char reserv[3];      	/* Reserved for future use */
}tr31_key_t;

typedef struct {
	int keyarch;
	int keytype;
	int namelen;
	int keylen;
}KEY_PACKAGE_HEADER;

/*密钥体系*/
enum sec_key_arch {
	KEY_ARCH_FIXED = 0x100,
	KEY_ARCH_MKSK,
	KEY_ARCH_DUKPT
};

/*密钥类型*/
enum sec_key_type {
	KEY_TYPE_MASTER_SESSION = 1,
	KEY_TYPE_DUKPT,
	KEY_TYPE_AES,
	KEY_TYPE_RSAPRIVATE,


	KEY_TYPE_DUKPT_INIT =0x20,		/*DUKPT初始密钥IK*/
	KEY_TYPE_DUKPT_TRANSATION,		/*DUKPT交易密钥TK，一次一密，根据初始密钥IK和KSN号动态计算生成*/
	KEY_TYPE_DUKPT_PIN,				/*DUKPT的PIN加密密钥，由交易密钥TK派生获得*/
	KEY_TYPE_DUKPT_MAC,				/*DUKPT的MAC加密密钥，由交易密钥TK派生获得*/

	KEY_TYPE_DES_MASTER = 0x30,		/*主密钥*/
	KEY_TYPE_DES_PIN,				/*PIN加密密钥*/
	KEY_TYPE_DES_MAC,				/*MAC加密密钥*/
	KEY_TYPE_DES_DATA,				/*数据加密密钥*/

	KEY_TYPE_FIXED_DES = 0x40,				/*FIXED密钥体系DES密钥*/
};

/*----------global variables declaration("extern")--------------*/
extern int g_seckeyowner;
extern int flag_sdk3;

/*---------------functions declaration--------------------------*/
//extern int CreateTR31KeyBlock( unsigned char *pkey ,unsigned int len ,unsigned char *pmainkey,unsigned char keytype,unsigned char *pblock );
extern int sec_tr31block_format(unsigned char * out, unsigned int *outlen, unsigned char * kek, int keklen, tr31_key_t * keyst);

#endif
