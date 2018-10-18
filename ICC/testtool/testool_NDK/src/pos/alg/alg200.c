/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 算法模块 
* file name			: 
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 算法模块内随机组合测试
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI	"算法模块内随机组合"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
	AlgTDes=0 ,			 
	AlgSHA1,
	AlgSHA256,
#if !K21_ENABLE   
	AlgSHA512,	
#endif
#if !K21_ENABLE ||defined ME30||defined ME30MH
	AlgRSAKeyPairGen,
	AlgRSARecover,
	AlgRSAKeyPairVerify,
#endif
#if CPU5892_ENABLE ||CPU5830X_ENABLE||ANDIROD_ENABLE||defined ME30||defined ME30MH
	AlgSM2KeyPairGen,
	AlgSM2Encrypt,
	AlgSM2Decrypt,
	AlgSM2GenE,
	AlgSM2Sign,
	AlgSM2Verify,
	AlgSM3Start,
	AlgSM3Update,
	AlgSM3Final,
	AlgSM3Compute,
	AlgSM4Compute,
#endif
#if CPU5810X_ENABLE ||defined ME32THM ||defined ME32GX||defined ME32||defined ME30THM ||defined ME30GX || defined ME30S
	AlgSM4Compute,
#endif
	MaxNum,//新增枚举值时在此值前面加,要保证此值为最后一个值
}ALG_MAG;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: 
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20170928		created
*****************************************************************/
static int alg_randtest(int funnum)
{
	int ret = -1;
	uchar desin[8] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77}, desout[8] = {0};
	uchar key8[8] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77};
	uchar datain[1024] = {0}, shaout[20] = {0}, shaout256[32] = {0};
#if !(defined ME30S ||defined ME50NX ||defined ME68)
	uchar  CBC_data_buf[16]={0x36, 0xf7, 0x81, 0x18, 0x29, 0xee, 0x22, 0x6b,
		  			        0x7f, 0xa5, 0x54, 0x92, 0x70, 0xe4, 0xf6, 0xe6}, 
		  CBC_vector[16]={0x2c, 0x5c, 0x0e, 0x04, 0x2c, 0x8f, 0x06, 0xd2,
		  			      0x33, 0xff, 0xd0, 0x25, 0x83, 0x65, 0xdf, 0x49},
		  CBC_key[16]={0xb1, 0x0b, 0x0e, 0x1f, 0x50, 0x74, 0x71, 0xe4,
				   		  0x07, 0x5e, 0x82, 0x6d, 0x09, 0x88, 0x15, 0xd8};
	uchar out_buf[33] = {0};
#endif
#if CPU5892_ENABLE ||CPU5830X_ENABLE||ANDIROD_ENABLE||defined ME30||defined ME32 ||defined ME30MH
	uchar data_buf_64[]="`~!/1@新$#[^%]*(大)'-_=+|&{}:陆;?/><.,abcdefghijklmnopqrst012345";
	uchar sign_data[65]={0}, real_message[]="newland", prikey1[33]={0}, pubkey1[65]={0}, summary[33]={0}, ID[]="test", sm2pubkey[64] = {0},sm2prikey[32]={0};
	ushort cryp_len=0;
#endif
#if !K21_ENABLE
	uchar shaout512[64] = {0};
#endif
#if !K21_ENABLE ||defined ME30||defined ME32 ||defined ME30MH
	uchar dataout1024enc[129]={0}, pubexp_10001[6]="10001";
	uchar datain1024[129]="THISISATESTTHISISATESTTHISISATESTTHISISATESISATESTTHISISATESTTTTTHISISATESTTHISISATESTTHISISATESTTHISISATESISATESTTHISISATESTTTT";
	uchar module1024_10001[257]="C0929407549DAAA509EB7F56C8B57C8EC9E08E84ACC611E7023516067A44467A039A1B3C2FCD7BAE2CFA640C17AE770A4C12F1B21697FCFE4287E7C4EA2D23C3DDEE57B24A794A91CDBD0897A5D770A5B4BCE4234A7CC757E45FA3037E170B2CF135DD4A1F086431D11A37C46003CEDA02B38479BF05AA45B17A13A6D77BFC69";
	ST_RSA_PRIVATE_KEY prikey={0};
	ST_RSA_PUBLIC_KEY pubkey={0};
#endif
	memset(datain,'1',sizeof(datain));
	switch(funnum)
	{	
		case AlgTDes:  
			if((ret = NDK_AlgTDes(desin, desout, key8, sizeof(key8), rand()%2)) !=NDK_OK)
				return ret;
			break;
		case AlgSHA1:  
			if((ret = NDK_AlgSHA1(datain, sizeof(datain), shaout)) !=NDK_OK)
				return ret;
			break;
		case AlgSHA256:	
			if((ret = NDK_AlgSHA256(datain, sizeof(datain), shaout256)) !=NDK_OK)
				return ret;
			break;
#if !K21_ENABLE   
		case AlgSHA512:  
			if((ret = NDK_AlgSHA512(datain, sizeof(datain), shaout512)) !=NDK_OK)
				return ret;
			break;
#endif
#if !K21_ENABLE ||defined ME30||defined ME30MH
		case AlgRSAKeyPairGen:  
			if((ret = NDK_AlgRSAKeyPairGen(RSA_KEY_LEN_1024,RSA_EXP_10001, &pubkey,&prikey)) != NDK_OK)
				return ret;
			break;
		case AlgRSARecover:  
			if((ret = NDK_AlgRSARecover(module1024_10001,RSA_KEY_LEN_1024/8,pubexp_10001,datain1024,dataout1024enc)) !=NDK_OK)
				return ret;
			break;	
		case AlgRSAKeyPairVerify:
			NDK_AlgRSAKeyPairVerify(&pubkey,&prikey);
			break;
#endif
#if CPU5892_ENABLE ||CPU5830X_ENABLE||ANDIROD_ENABLE||defined ME30||defined ME30MH
		case AlgSM2KeyPairGen:
			NDK_AlgSM2KeyPairGen(pubkey1, prikey1);
			break;
		case AlgSM2Encrypt:
			NDK_AlgSM2Encrypt(sm2pubkey, real_message, strlen((char*)real_message), datain, &cryp_len);
			break;
		case AlgSM2Decrypt:
			NDK_AlgSM2Decrypt(sm2prikey, datain, cryp_len, real_message, &cryp_len);
			break;
		case	AlgSM2GenE:
			NDK_AlgSM2GenE(strlen((char *)ID), ID, strlen((char *)real_message), real_message, sm2pubkey, summary);
			break;
		case AlgSM2Sign:
			NDK_AlgSM2Sign(sm2prikey, summary, sign_data);
			break;
		case AlgSM2Verify:
			NDK_AlgSM2Verify(sm2pubkey, summary, sign_data);
			break;
		case AlgSM3Start:
			NDK_AlgSM3Start();
			break;
		case AlgSM3Update:
			NDK_AlgSM3Update(data_buf_64, strlen((char *)data_buf_64));
			break;
		case AlgSM3Final:
			NDK_AlgSM3Final(real_message, strlen((char *)real_message), out_buf);
			break;
		case AlgSM3Compute:
			NDK_AlgSM3Compute(data_buf_64, strlen((char *)data_buf_64), out_buf);
			break;
		case AlgSM4Compute:		
			if((ret=NDK_AlgSM4Compute(CBC_key, CBC_vector, sizeof(CBC_data_buf), CBC_data_buf, out_buf, ALG_SM4_ENCRYPT_CBC))!=NDK_OK)
				return ret;
			break;
#endif
#if CPU5810X_ENABLE ||defined ME32THM ||defined ME32GX||defined ME30THM ||defined ME30GX ||defined ME32
		case AlgSM4Compute:		
			if((ret=NDK_AlgSM4Compute(CBC_key, CBC_vector, sizeof(CBC_data_buf), CBC_data_buf, out_buf, ALG_SM4_ENCRYPT_CBC))!=NDK_OK)
				return ret;
			break;
#endif
	}	
	return SUCC;
}

void alg200(void)
{
	/*private & local definition*/
	int  i = 0, bak = 0, num[30] ={0}, j = 0; 
	int ret = -1,cnt =50, len = 0;//cnt 待定
#if ANDIROD_ENABLE
	char *algstr[] = {"NDK_AlgTDes","AlgSHA1","AlgSHA256","AlgSM2KeyPairGen","AlgSM2Encrypt","AlgSM2Decrypt","AlgSM2GenE","AlgSM2Sign","AlgSM2Verify","AlgSM3Start","AlgSM3Update","AlgSM3Final","AlgSM3Compute","AlgSM4Compute"};
#elif OVERSEAS_ENABLE
	char *algstr[] = {"NDK_AlgTDes","AlgSHA1","AlgSHA256","AlgSHA512","AlgRSAKeyPairGen","AlgRSARecover","AlgRSAKeyPairVerify"};
#elif CPU5810X_ENABLE ||defined ME32THM ||defined ME32GX||defined ME30THM ||defined ME30GX ||defined ME32
	char *algstr[] = {"NDK_AlgTDes","AlgSHA1","AlgSHA256","AlgSM4Compute"};
#elif defined ME30S||defined ME50NX ||defined ME68
	char *algstr[] = {"NDK_AlgTDes","AlgSHA1","AlgSHA256"};
#else
	char *algstr[] = {"NDK_AlgTDes","AlgSHA1","AlgSHA256","AlgSHA512","AlgRSAKeyPairGen","AlgRSARecover","AlgRSAKeyPairVerify","AlgSM2KeyPairGen","AlgSM2Encrypt","AlgSM2Decrypt","AlgSM2GenE","AlgSM2Sign","AlgSM2Verify","AlgSM3Start","AlgSM3Update","AlgSM3Final","AlgSM3Compute","AlgSM4Compute"};
#endif		

	/*process body*/
	len = (MaxNum>20)?MaxNum:20;
	while(cnt)
	{	
		cnt--;
		bak++;
		
		//产生随机数
		cls_printf("第%d次随机顺序:",bak);
		srand((unsigned)time(NULL));
		for(i=0;i<len;i++)
		{	
			num[i] = rand()%MaxNum;
			NDK_ScrPrintf("%d ", num[i]);
			NDK_ScrRefresh();
		}
		
		//进行随机测试
		for(i=0;i<len;i++)
		{
			if((ret = alg_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",algstr[num[j]]);
				return;
			}
		}	
	}
	//test over
	send_result("%s测试通过", TESTAPI);
	return;
}

