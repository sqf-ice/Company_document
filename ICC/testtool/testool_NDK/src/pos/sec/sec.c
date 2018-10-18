/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module						: sec安全模块
* file name					: 
* Author 						: linrq
* version						: 
* DATE							: 
* directory 				: 
* description				: sec模块总入口
* related document	: DNKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"sec模块"

/*------------global variables definition-----------------------*/
int g_seckeyowner = 0;
int flag_sdk3 = 0; 
#if !(defined ME15C||defined ME15CHX||defined ME15B)	//ME15C应用空间有限,将不使用的变量条件编译掉
#define InvaildTR31Block 0x46
#define GeneralError 		0x02

static  unsigned char Key[64];
static  unsigned char output[64];
static  unsigned char input[64];
static  unsigned char Kn[16][48];

/* table 1 : permutation initiale */
static  unsigned char T1[] = {
        57,49,41,33,25,17, 9, 1,
        59,51,43,35,27,19,11, 3,
        61,53,45,37,29,21,13, 5,
        63,55,47,39,31,23,15, 7,
        56,48,40,32,24,16, 8, 0,
        58,50,42,34,26,18,10, 2,
        60,52,44,36,28,20,12, 4,
        62,54,46,38,30,22,14, 6
};

/* table 2 : permutation finale */
static  unsigned char T2[] = {
        39, 7,47,15,55,23,63,31,
        38, 6,46,14,54,22,62,30,
        37, 5,45,13,53,21,61,29,
        36, 4,44,12,52,20,60,28,
        35, 3,43,11,51,19,59,27,
        34, 2,42,10,50,18,58,26,
        33, 1,41, 9,49,17,57,25,
        32, 0,40, 8,48,16,56,24
};

static const unsigned char T3[] = {
        31, 0, 1, 2, 3, 4,
         3, 4, 5, 6, 7, 8,
         7, 8, 9,10,11,12,
        11,12,13,14,15,16,
        15,16,17,18,19,20,
        19,20,21,22,23,24,
        23,24,25,26,27,28,
        27,28,29,30,31, 0
};

/* table 7 : table de choix 1 */
static const unsigned char T7_1_2[56] =
{
        56,48,40,32,24,16, 8,
         0,57,49,41,33,25,17,
         9, 1,58,50,42,34,26,
        18,10, 2,59,51,43,35,

        62,54,46,38,30,22,14,
         6,61,53,45,37,29,21,
        13, 5,60,52,44,36,28,
        20,12, 4,27,19,11, 3
};

/* table 8 : table de d\202calage */
//  code unsigned char T8[16] =
static const unsigned char T8[] =
{
         0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0
};

static const  unsigned char T9[] =
{
        13,16,10,23, 0, 4,
         2,27,14, 5,20, 9,
        22,18,11, 3,25, 7,
        15, 6,26,19,12, 1,
        40,51,30,36,46,54,
        29,39,50,44,32,47,
        43,48,38,55,33,52,
        45,41,49,35,28,31
};

static const unsigned char T5[] = {
        15, 6,19,20,
        28,11,27,16,
         0,14,22,25,
         4,17,30, 9,
         1, 7,23,13,
        31,26, 2, 8,
        18,12,29, 5,
        21,10, 3,24
};

static const unsigned char T6[][64] =
{
/* S1 */
        {
        14, 4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7,
         0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8,
         4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0,
        15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13
        },

/* S2 */
        {
        15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10,
         3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5,
         0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15,
        13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9
        },

/* S3 */
        {
        10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8,
        13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1,
        13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7,
         1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12
        },

/* S4 */
        {
         7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15,
        13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9,
        10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4,
         3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14
        },

/* S5 */
        {
         2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9,
        14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6,
         4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14,
        11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3
        },

/* S6 */
        {
        12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11,
        10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8,
         9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6,
         4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13
        },

/* S7 */
        {
         4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1,
        13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6,
         1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2,
         6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12
        },

/* S8 */
        {
        13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7,
         1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2,
         7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8,
         2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11
        }
};

//增加设置变量TE为4字节对齐,避免在ME30的M0编译器平台上赋值给long数据类型时地址不对齐引起卡死问题 20160803 linwl
__attribute__((__aligned__(4))) 
static const unsigned char TE[][4] =
{
   {0,0,0,0},
   {0,0,0,1},
   {0,0,1,0},
   {0,0,1,1},
   {0,1,0,0},
   {0,1,0,1},
   {0,1,1,0},
   {0,1,1,1},
   {1,0,0,0},
   {1,0,0,1},
   {1,0,1,0},
   {1,0,1,1},
   {1,1,0,0},
   {1,1,0,1},
   {1,1,1,0},
   {1,1,1,1}
};

#define XORDATA( data1 , data2 , outdata , datalen ) {\
	int j;\
	for( j=0 ; j<(datalen) ; j++ )\
		(outdata)[j] = (data1)[j]^(data2)[j];\
}
#endif

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
#if !(defined ME15C||defined ME15CHX||defined ME15B)	//ME15C应用空间有限,将不使用的变量条件编译掉
static void eclater(unsigned char *buf_bit, uchar *byte)
{
	volatile int i;
	unsigned char m;

	for (i = 0; i < 8; i++)
	{
		for (m = 0x80; m != 0;  )  // m >>= 1)
		{
			if ((buf_bit[i] & m) != 0)
				*byte = 1;
			else
				*byte = 0;
			byte++;
			m=m/2 ;
		}
	}
}

static void compacter(uchar *byte, uchar *buf_bit)
{
	volatile int i;
	unsigned char m, n;

	for (i = 0; i < 8; i++)
	{
		n = 0;
		for (m = 0x80; m != 0; )  //  m >>= 1)
		{
			if (*byte++)
				n = n | m;
			m=m/2 ;

		}
		buf_bit[i] = n;
	}
}

static void Ks(uchar *Key, uchar Kn[][48])
{
	uchar cd[56];
	uchar zt[60] ;

	volatile int n;
	unsigned char tmp11, tmp12, tmp21, tmp22;
	volatile int i;
	unsigned char *Knn;

	/* choix 1 */
	for (i = 0; i < 56; i++)
	{
		cd[i] = Key[T7_1_2[i]];
	}

	for (n = 0; n < 16; n++)
	{
		/* rotation \205 gauche du vecteur en fonction de l'indice */
		if (T8[n] == 0)
		{
			tmp11 = cd[0];
			tmp21 = cd[28];
			memcpy( zt , &cd[1] , 55 );
			memcpy( cd , zt     , 55 );
			//   memmove(&cd[0], &cd[1], 55); /* ce qui est en 1 va en0 */
			cd[27] = tmp11;
			cd[55] = tmp21;
		}
		else
		{
			tmp11 = cd[0];
			tmp12 = cd[1];
			tmp21= cd[28];
			tmp22 = cd[29];

			memcpy( zt , &cd[2] , 54 );
			memcpy( cd , zt     , 54 );

			//                    memmove(&cd[0], &cd[2], 54); /* ce qui est en 2 va en 0*/

			cd[26] = tmp11;
			cd[27] = tmp12;
			cd[54] = tmp21;
			cd[55] = tmp22;
		}
		/* choix 2 */
		Knn = Kn[n];
		for (i = 0; i < 48; i++)
		{
			Knn[i] = cd[T9[i]];
		}
	}
}

static void permutation(uchar *org, uchar *tab)
{
    unsigned char tmp[64];
	volatile int i;//for循环变量i为避免低端K21编译器引起的卡死问题,申明成volatile int类型


	memcpy(tmp, org, 64);
	for (i = 0; i < 64; i++)
	{
		org[i] = tmp[tab[i]];
	}
}

static void fonction(uchar *Knn, uchar *r, uchar *s)
{
/* n est l'indice de 1 a 16 pour choisir la cle
r est R.
	s est le r\202sultat. */

	unsigned char x[32];
	unsigned long *px;
	volatile int i, l;
	unsigned char c;
	unsigned char t;

	/* fonction E */    /*  + OU exclusif */
	/* S\202lection Sn */
    for (i = 0, l = 0, px = (unsigned long *) x; i < 8;)
	{
		c = 32 * (r[T3[l]] ^ Knn[l]);
        l++;
		c += 8 * (r[T3[l]] ^ Knn[l]);
        l++;
		c += 4 * (r[T3[l]] ^ Knn[l]);
        l++;
		c += 2 * (r[T3[l]] ^ Knn[l]);
        l++;
		c += 1 * (r[T3[l]] ^ Knn[l]);
        l++;
		c += 16 * (r[T3[l]] ^ Knn[l]);
        l++;
        /* extraction de la valeur */
        t = T6[i][c];
        i++;
        /* Eclatement de la valeur; 4 bits -> 4 bytes */
        *px = *(ulong *)TE[t];
        px++;
    }
	/* fonction P */
	for (i = 0; i < 32; i++)
	{
		s[i] = x[T5[i]];
	}
}

static void chiffrement(uchar *xi, uchar *xo, unsigned char Kn[][48])
{
	unsigned char r[32], l[32];
	unsigned char rp[32], lp[32];

	volatile int i;
	volatile int n;

	memcpy(l, &xi[0], 32);
	memcpy(r, &xi[32], 32);

	for (n = 0; n < 16; n++)
	{
		memcpy(lp, r, 32);

		fonction(Kn[n], r, rp);
		for (i = 0; i < 32; i++)
		{
			r[i] =( ( l[i]) ^ (rp[i] )  ) ;
		}
		memcpy(l, lp, 32);
	}
	memcpy(&xo[0], r, 32);
	memcpy(&xo[32], l, 32);

}

static void des(uchar *binput, uchar *boutput, uchar *bkey)
{
	eclater(binput, input);
	eclater(bkey, Key);
	Ks(Key, Kn);

    permutation(input, T1);
    chiffrement(input, output, Kn);
    permutation(output, T2);
    compacter(output, boutput);
}

static void dechiffrement(uchar *xi, uchar *xo, unsigned char Kn[][48])
{
    unsigned char r[32], l[32], rp[32], lp[32];

	int i;
	int n;

	memcpy(l, &xi[0], 32);
	memcpy(r, &xi[32], 32);

	for (n = 0; n < 16; n++)
	{
		memcpy(lp, r, 32);
		fonction(Kn[15 - n], r, rp);
		for (i = 0; i < 32; i++)
		{
			r[i] =( ( l[i] ) ^ ( rp[i] )) ;
		}
		memcpy(l, lp, 32);
	}

	memcpy(&xo[0], r, 32);
	memcpy(&xo[32], l, 32);
}

static void undes(uchar *binput, uchar *boutput, uchar *bkey)
{
 	eclater(binput, input);
	eclater(bkey, Key);
	Ks(Key, Kn);

	permutation(input, T1);
	dechiffrement(input, output, Kn);
	permutation(output, T2);
	compacter(output, boutput);
 }

//  3DES传入的KEY须为24字节
static void des3(unsigned char *binput, unsigned char *boutput, unsigned char *bkey)
{
	unsigned char tmpibuf[8],tmpobuf[8],tmpkey[24];
	memcpy(tmpibuf,binput,8);
	memcpy(tmpkey,bkey,24);
	des(tmpibuf,tmpobuf,tmpkey);
	 undes(tmpobuf,tmpibuf,tmpkey + 8);
	des(tmpibuf,tmpobuf,tmpkey + 16);
	memcpy(boutput,tmpobuf,8);
}

#if 0
static void undes3(uchar *binput, uchar *boutput, uchar *bkey)
{
	uchar tmpibuf[8],tmpobuf[8],tmpkey[24];
	memcpy(tmpibuf,binput,8);
	memcpy(tmpkey,bkey,24);
	undes(tmpibuf,tmpobuf,tmpkey + 16);
	des(tmpobuf,tmpibuf,tmpkey + 8);
	undes(tmpibuf,tmpobuf,tmpkey);
	memcpy(boutput,tmpobuf,8);
}

static void AsciiToBcd(char *bcd_buf ,char *ascii_buf ,int conv_len ,char type )
{
    int      cnt;
    char     ch,ch1;
    if ( (conv_len & 0x01) && (type) )/*判别是否为奇数以及往那边对齐*/
        ch1 = 0 ;
    else
        ch1 = 0x55 ;

    for ( cnt = 0 ; cnt < conv_len ; ascii_buf ++ , cnt ++ )
	{
        if ( *ascii_buf >= 'a' )
            ch = *ascii_buf - 'a' + 10 ;
        else if ( *ascii_buf >= 'A' )
            ch = *ascii_buf- 'A' + 10 ;
        else if ( *ascii_buf >= '0' )
            ch = *ascii_buf-'0' ;
        else if ( *ascii_buf == 0 )
            ch = 0x0f ;
        else
		{

			ch = (*ascii_buf);
			ch &= 0x0f;//保留低四位
		}
        if ( ch1 == 0x55 )
            ch1 = ch ;
        else
        {
            *bcd_buf ++ = ch1 << 4 | ch ;
            ch1 = 0x55 ;
        }
    }//for
    if ( ch1 != 0x55 )
        *bcd_buf = ch1 << 4 ;

    return ;
}
#endif
#endif

#if TR31_ENABLE  //支持TR31格式安装密钥，才编译TR31相关的功能函数
#define KEYLEN 24
/************************************************************************
TR31加密计算
密钥头格式：
  0	Key Block Version ID	A'0x41	当前格式版本
1~4	Key Block Length		包总长，高位在前，ASCII码、十进制
5~6	Key Usage	"P0" or "M0" or "D0" or "K0"	"P0"PIN Encryption;"M0"ISO 16609 MAC algorithm1(using TDEA);"D0"Data Encrypton; "K0" Key Encryption
7	Algorithm	T'	0x54 Triple DEA(TDEA)
8	Mode of Use	B or C or E				'B'Both Encrypt and Decrypt;'C'MAC Calculate (Generate or Verify);'E'Encrypt only
9~10	Key Version Number		"00"
11	Exportability	E'	只能是这个值才能更新
12~13	Number of Optional Blocks		"00"     value:00~99
14~15	Reserved for future use		"00"

函数功能：由主密钥将需要加密的密钥明文加密，生成TR-31格式的加密块数据
输入：	*pkey	--	需要加密传输的密钥明文
		len	--	密钥明文长度
		*pmainkey	--	用于TR-31加密算法的主密钥
		keytype	--	当前所要加密的密钥类型
输出：	*pblock	--	数据输出指针，用于存放TR-31加密生成的加密数据块，
返回：	SUCC -- 成功	FAIL -- 失败
************************************************************************/
int CreateTR31KeyBlock( unsigned char *pkey ,unsigned int len ,unsigned char *pmainkey,unsigned char keytype,unsigned char *pblock )
{
	unsigned char indata[256];
	unsigned char outdata[256];
	unsigned char initdata[8];
	unsigned char tmpkey[KEYLEN];
	unsigned char mainkey[KEYLEN];
	unsigned char keyblock[256];
	int i,ret;

	memcpy( mainkey , pmainkey , KEYLEN );
	//制作key的头信息
	memcpy(indata,"A000000000000000",16);
	switch(keytype)
	{
	case PINKEY_TYPE:
		memcpy( indata+5 , "P0" , 2 );
		indata[7] = 'T';
		indata[8] = 'E';
		indata[11] = 'E';
		break;
	case MACKEY_TYPE:
		memcpy( indata+5 , "M0" , 2 );
		indata[7] = 'T';
		indata[8] = 'C';
		indata[11] = 'E';
		break;
	case TRANSKEY_TYPE:
		memcpy( indata+5 , "D0" , 2 );
		indata[7] = 'T';
		indata[8] = 'B';
		indata[11] = 'E';
		break;
	case MAINKEY_TYPE:
		memcpy( indata+5 , "K0" , 2 );
		indata[7] = 'T';
		indata[8] = 'D';
		indata[11] = 'E';
		break;
	case MAINKEYENPKEY_TYPE:
		memcpy( indata+5 , "B0" , 2 );
		indata[7] = 'T';
		indata[8] = 'B';
		indata[11] = 'E';
		break;
	case DUKPT_INITKEY_TYPE:
		memcpy( indata+5 , "C0" , 2 );
		indata[7] = 'T';
		indata[8] = 'B';
		indata[11] = 'E';
		break;	
	default:
		return -1;
		break;
	}
	
	memcpy( keyblock , indata , 16 );		//TR31密钥块头,header: A 0000 P0 T E 00 E 0000

	//对24位的主密钥进行异或操作，异或结果用于对密钥明文加密
	for(i=0;i<KEYLEN;i++)
		tmpkey[i] = mainkey[i]^'E';
	//128bits密钥，长度填写为0x0080
	indata[0] = (len*8)/256;
	indata[1] = (len*8)%256;
	memcpy( indata+2 , pkey , len );		// 00 08 - 长度
	//取随机数
//	ret = BSP_RNG_GetRng( outdata , 8 );
//	if(FAIL == ret)
//		return FAIL;
//	memcpy( indata+2+KEYLEN , outdata , 8-((2+KEYLEN)%8) );
	//memcpy(indata+2+KEYLEN,"\x72\x0D\xf5\x63\xBB\x07",6);	//for debug ????
	srand((unsigned)time(0));
	for (i=0; i<8-(2+len)%8; i++)						//随机数，添加在padding field
	{
		indata[2+len+i] = (unsigned char)(rand()%0x100);
	}
	//所有数据长度都已经整合成8的倍数
	//生成密钥密文块
	ret = 2+len;						//16+2 = 18
	ret = (ret%8)?(ret/8+1):(ret/8);			//ret/8+1 = 3
	//修改头部的长度信息
	sprintf( (char*)outdata , "%04d" , 16+8*ret*2+8 );	//16+8*3*2+8 = 72
	memcpy( keyblock+1 , outdata , 4 );		//Header: A 0072 P0 T E 00 E 0000
	memcpy( initdata , keyblock , 8 );
	for(i=0;i<ret;i++)
	{
		XORDATA( initdata , indata+i*8 , indata+i*8 , 8 );
		des3( indata+i*8 , outdata+i*8 , tmpkey );
		memcpy( initdata , outdata+i*8 , 8 );
	}
	memcpy( keyblock+16 , outdata , 8*ret );

	//对24位的主密钥进行异或操作，异或结果用于计算MAC
	for(i=0;i<KEYLEN;i++)
		tmpkey[i] = mainkey[i]^'M';
	memset( initdata, 0 , 8 );
	for( i=0 ; i<(ret+2) ; i++ )
	{
		XORDATA( initdata , keyblock+i*8 , indata , 8 );
		des3( indata , outdata , tmpkey );
		memcpy( initdata , outdata , 8 );
	}
	memcpy( keyblock+16+ret*8 , outdata , 4 );

	//将数据拷出
	memcpy( pblock , keyblock , 16 );
	for( i=0 ; i<(8*ret+4) ; i++ )
	{
		sprintf( (char*)indata, "%02x" , (keyblock+16)[i] );
		memcpy( pblock+16+i*2 , indata , 2 );
	}
	return 0;
}

/************************************************************************
函数功能：使用主密钥将TR-31加密的密文数据块解密，还原生成密钥明文
输入：	*pkeyblock	--	需要解密的TR-31密文块数据
		len	--	密文数据块长度
		*pmainkey	--	用于TR-31加密算法的主密钥
输出：	*poutkey	--	存放解密输出的密钥明文
		*pkeylen	--	解密所得密钥的长度
		*pkeytype	--	解密所得密钥的类型
返回：	SUCC -- 成功	FAIL -- 失败
************************************************************************/
#if 0
int DecryptTR31KeyBlock( unsigned char *pkeyblock,
							   int len,
							   unsigned char *pmainkey,
							   unsigned char *poutkey,
							   unsigned int *pkeylen,
							   unsigned char *pkeytype )
{
	unsigned char indata[256] ={0};
	unsigned char outdata[256] ={0};
	unsigned char initdata[8] ={0};			//初始化向量
	unsigned char tmpkey[KEYLEN] ={0};
	unsigned char keyblock[256] ={0};
	unsigned char tmpkeyblock[256] ={0};
	unsigned char header[16]={0};
	unsigned char keytype;
	int i,ret;
	//先进行长度判断
	//判断头信息是否正确
	memcpy(header , pkeyblock , 16);
	memset(header+1,'0',8);
	header[11]='0';
	if(memcmp(header,"A000000000000000",16)
		||(  'E' != pkeyblock[11] ))
		return InvaildTR31Block;
	switch(pkeyblock[5])
	{
	case 'P':
		if( memcmp(pkeyblock+6,"0TE",3 ) )
			return InvaildTR31Block;
		keytype = PINKEY_TYPE;
		break;
	case 'M':
		if( memcmp(pkeyblock+6,"0TC",3 ) )
			return InvaildTR31Block;
		keytype = MACKEY_TYPE;
		break;
	case 'D':
		if( memcmp(pkeyblock+6,"0TB",3 ) )
			return InvaildTR31Block;
		keytype = TRANSKEY_TYPE;
		break;
        case 'K':
		if( memcmp(pkeyblock+6,"0TD",3 ) )
			return InvaildTR31Block;
		keytype = MAINKEY_TYPE;
		break;
	case 'B':
		if( memcmp(pkeyblock+6,"0TB",3 ) )
			return InvaildTR31Block;
		keytype = MAINKEYENPKEY_TYPE;
		break;
	case 'C':
		if( memcmp(pkeyblock+6,"0TB",3 ) )
			return InvaildTR31Block;
		keytype = DUKPT_INITKEY_TYPE;
		break;
	default:
		return InvaildTR31Block;
		break;
	}
	//先把后续的数据转化为十六进制数
	memcpy( keyblock , pkeyblock , 16 );//将头拷走
	AsciiToBcd( (char*)keyblock+16 , (char*)pkeyblock+16 , len-16 ,0 );
	ret = 16+(len-16)/2;
	ret = ret/8;
	//验证MAC值是否正确
	for(i=0;i<KEYLEN;i++)
		tmpkey[i] = pmainkey[i]^'M';
	memset( initdata, 0 , 8 );
	for( i=0 ; i<ret ; i++ )
	{
		XORDATA( initdata , keyblock+i*8 , indata , 8 );
		des3( indata , outdata , tmpkey );
		memcpy( initdata , outdata , 8 );
	}
	if(!(memcmp( outdata , keyblock+(ret*8)-4 , 4 )))
		return GeneralError;//MAC 验证错

	//解密密文
	for(i=0;i<KEYLEN;i++)
		tmpkey[i] = pmainkey[i]^'E';
	memcpy( initdata , keyblock , 8 );
	for(i=0;i<(ret-2);i++)
	{
		undes3( keyblock+16+i*8 , outdata , tmpkey );
		XORDATA( initdata , outdata , indata , 8 );
		memcpy( tmpkeyblock+i*8 , indata , 8 );
		memcpy( initdata , keyblock+16+i*8 , 8 );
	}
	ret = (tmpkeyblock[0]*255+tmpkeyblock[1])/8;
        if(ret >( (len-16)/2-4-2))
       {
            return InvaildTR31Block;
        }
	memcpy(poutkey, tmpkeyblock+2 , ret );
	*pkeylen = ret ;
	//*pkeytype = keytype ;

	if (MAINKEY_TYPE == keytype)
	{
		*pkeytype = SEC_KEY_TYPE_TMK; 
	}
	else if (DUKPT_INITKEY_TYPE == keytype)
	{
	/*	*pkeytype = DUKPT_INITKEY;*/
	}

	return 0;
}
#endif
#endif

#if TR31_ENABLE	//支持TR31格式安装密钥才编译TR31相关的功能函数
/*
 * TR-31 CBC MAC Key Block
 * ----------24-----------------------24----------------------------4-----
 * | header | header opt | length | Key | KSN(Condition) |Padding | MAC |
 * -----------------------------------------------------------------------
 */

#define XORDATAJ(j, data1, data2, datalen, outdata) {\
        for( j=0 ; j<(datalen) ; j++ ) \
                (outdata)[j] = (data1)[j]^(data2)[j]; \
}

#ifndef DES3_KEY_LEN
#define DES3_KEY_LEN 16
#endif

#define USE_CMAC_TDEA
//#define DEBUG_TR31_DATA

#define TR31_HEAD_LEN			(16 + 8 )
#define TR31_HEAD_OPT2_LEN		(2 + 2 + 20)
#ifdef USE_CMAC_TDEA
#define KBH_VERID               0x42
#define MAC_LEN                 8
#else
#define KBH_VERID               0x41
#define MAC_LEN 				4
#endif
#define DUKPT_KSN_LEN			10

#define TR31RSA_KEY_LEN					256
#define TR31_DATA_BCD_MAX 				(2 + TR31RSA_KEY_LEN + 6)	/*2[length] + [data] + 6[padding]*/
#define TR31_ENCRYPTED_BLOCK_BCD_MAX 	(TR31_HEAD_LEN + TR31_HEAD_OPT2_LEN + TR31_DATA_BCD_MAX + MAC_LEN)


/*	key usage (support only as below )*/
#define KEY_USAGE(usage)        (((usage)[0] << 8) | (usage)[1])

#define KEY_USAGE_BDK           	"B1"          /* B0:KT_BDK BASE Derivation Key */
#define KEY_USAGE_PRI          		"D0"          /* D0:KT_PRI  */
#define KEY_USAGE_MST           	"K0"          /* K0:KT_MST */
#define KEY_USAGE_TR31           	"K1"          /* K1:KT_TR31 */
#define KEY_USAGE_MAC      			"M0"          /* M0:KT_MAC*/
#define KEY_USAGE_PIN            	"P0"          /* P0:KT_PIN  */
#define KEY_USAGE_MAG            	"D0"          /* D0:KT_MAG, DATA Enc */

/*	key algorithm (support only as below )*/
#define KEY_ALGORITHM_RSA       	0x52            /* 'R':RSA */
#define KEY_ALGORITHM_TDEA      	0x54            /* 'T':Triple DEA */

/*	key mode of use  (support only as below )*/
#define KEY_USEMODE_MAC        		0x43            /* 'C':MAC Calculate (Generate or Verify) */
#define KEY_USEMODE_MST     		0x44            /* 'D':KT_MST */
#define KEY_USEMODE_ENCRYPT     	0x45            /* 'E':KT_PIN, KT_BDK */
#define KEY_USEMODE_SIG         	0x53            /* 'S': KT_PRI */

#define KEY_EXPORT					'E'
#define OPT_ID						"KS"
#define OPT_NR						"01"
#define OPT_LEN						"08"
#define OPT2_ID						"01"
#define OPT2_NR						"02"
#define OPT2_LEN					"18"

//#define KT_BDK_TYPE             "B1TE"

//	-----key filed ID ----
#define KEYFD_MT					0x00
#define KEYFD_FIX					0x01
#define KEYFD_DUKPT					0x02

typedef struct {
        unsigned char ver_id;         /* Key Block Version ID --- 'A'(0x41) */
        unsigned char length[4];      /* Key Block Length, including Header + DATA + MAC */
        unsigned char usage[2];       /* Key Usage */
        unsigned char algorithm;      /* The approved algorithm to be used by the protected key */
        unsigned char mode;           /* the operation the protected key can perform */
        unsigned char ver_num[2];     /* Key Version Number */
        unsigned char export;         /* Defines the protected key's exportablility */
        unsigned char option_nr[2];   /* Numbers of optional blocks */
        unsigned char reserv[2];      /* Reserved for future use */

		unsigned char opt_id[2];      /* The first optional block, for key index information*/
		unsigned char opt_len[2];		/*optional block length*/
		unsigned char fid;			/*key field*/
		unsigned char gid[3];			/*Group ID, ASCII Numeric Digits*/

		unsigned char opt2_id[2];		/* The second optional block, for DUKPT KSN value*/
		unsigned char opt2_len[2];	/*optional block length*/
		unsigned char ksn[20];		/*ASCII-encoded KSN for dukpt*/
}tr31_kbh_t;

/* Return Value of tr31block_decode Fuction*/
#define TR31BLK_HERR_VER         -1             /*block header version error*/
#define TR31BLK_HERR_EXP         -2             /*block header Exportability error*/
#define TR31BLK_HERR_USG         -3             /*block header usage error*/
#define TR31BLK_HERR_LEN         -4             /*block header length byte error*/
#define TR31BLK_HERR_OPT         -5             /*block header option error*/

#define TR31BLK_ERR_LEN          -10            /*the Decoded block length error*/
#define TR31BLK_ERR_MAC          -11            /*block MAC error*/
#define TR31BLK_ERR_KLEN         -12            /*block KEY length error*/
#define TR31BLK_ERR_NULL         -13            /*Pointer NULL*/
#define TR31BLK_ERR_KSN         -14            /*KSN error*/

#ifdef USE_CMAC_TDEA
static unsigned char CMAC_K1[8];
static unsigned char CMAC_K2[8];
static unsigned char CMAC_KBEK[24];
static unsigned char CMAC_KBMK[24];
//static unsigned char CMAC_MAC[8];
#endif

static int xauth_data_3des(char *in, int inlen, char *out, int do_encrypt, char *key, int keylen)
{
	char tmpkey[24];
	switch(keylen){
	case 8:
		des((uchar *)in, (uchar *)out, (uchar *)key);
		break;
	case 16:
		memcpy(tmpkey, key, 16);
		memcpy(tmpkey+16, key, 8);
		des3((uchar *)in, (uchar *)out, (uchar *)tmpkey);
		break;
	default:
		des3((uchar *)in, (uchar *)out, (uchar *)key);
		break;
	}
    return 0;
}

static int _tr31_des3(int enc, unsigned char *indata, unsigned char *outdata, unsigned char *key, unsigned char keylen)
{
	return xauth_data_3des((char*)indata, 8, (char*)outdata, enc, (char*)key, keylen);
}

int rng_get_string( unsigned char *out, unsigned int reqlen )
{
	volatile unsigned int i;

	srand((int) time(0));
	for( i = 0; i < reqlen; ++i ) {
		*(out + i) = rand() & 0xFF;
	}
	return 0;
}

static int _tr31_cmac_subkey_derivation(unsigned char *key, int klen, unsigned char *K1, unsigned char *K2)
{
	volatile unsigned int i;
	unsigned char tmpdata[8];
	unsigned char S[8];
	unsigned int s_msb;
	//const unsigned char R64[8]= {0,0,0,0,0,0,0,0x1B};

#ifdef DEBUG_TR31_DATA
	volatile unsigned int j;
	fprintf(stderr,"CMAC_Key:");
	for(j=0; j<klen; j++) {
		fprintf(stderr,"%02X ",key[j]);
	}
	fprintf(stderr,"\n");
#endif
	//TDEA encrypt the 8-byte all-zero input data hex 0000000000000000 with K, yielding the 8-byte output,
	//which we will call S.
	memset(tmpdata,0,sizeof(tmpdata));
	_tr31_des3(1, tmpdata, S, key, klen);	/* S */
#ifdef DEBUG_TR31_DATA
	fprintf(stderr,"CMAC_S:");
	for(j=0; j<8; j++) {
		fprintf(stderr,"%02X ",S[j]);
	}
	fprintf(stderr,"\n");
#endif

	//BEGIN//////////////////////////////////////////////////////////////////////
	s_msb = (S[0] & 0x80) ? 1 : 0;
	for(i = 0; i < 7; i ++) {
		S[i] = S[i] << 1;
		if(S[i + 1] & 0x80)	//get the LSB from the MSB of next byte
			S[i] |= 1;
	}
	S[i] = S[i] << 1;	//left the LSB of last byte zero
	if(s_msb)		// XOR with R64 if MSB!=0
		S[7] = S[7]^ 0x1B;
	memcpy(K1, S, 8);
#ifdef DEBUG_TR31_DATA
	fprintf(stderr,"CMAC_K1:");
	for(j=0; j<8; j++) {
		fprintf(stderr,"%02X ",K1[j]);
	}
	fprintf(stderr,"\n");
#endif

	s_msb = (S[0] & 0x80) ? 1 : 0;
	for(i = 0; i < 7; i ++) {
		S[i] = S[i] << 1;
		if(S[i + 1] & 0x80)
			S[i] |= 1;
	}
	S[i] = S[i] << 1;
	if(s_msb)
		S[7] = S[7]^ 0x1B;

	memcpy(K2, S, 8);

#ifdef DEBUG_TR31_DATA
	fprintf(stderr,"CMAC_K2:");
	for(j=0; j<8; j++) {
		fprintf(stderr,"%02X ",K2[j]);
	}
	fprintf(stderr,"\n");
#endif

	return 0;
}

/*Key derivation method, refer to Section "5.3.2" of TR31-2010 specs*/
static int _tr31_key_derivation_method(unsigned char *key, int klen, unsigned char *K1, unsigned char *input, unsigned char *out)
{
	volatile unsigned int i;
	unsigned char tmpdata[8];

	for(i=0; i<8; i++) {
		tmpdata[i] = K1[i] ^ input[i];
	}
	return _tr31_des3(1, tmpdata, out, key, klen);
}

int _bcd_2_asc(unsigned char * bcd, int asclen, unsigned char * asc, int align)
{
	volatile volatile int cnt;
	unsigned char *pbcd = bcd;
	unsigned char * pasc = asc;

	if ((asclen & 0x01) && align) {  /*判别是否为奇数以及往那边对齐 */
		cnt = 1;
		asclen ++;
	} else {
		cnt = 0;
	}
	for (; cnt < asclen; cnt++, pasc++) {
		*pasc = ((cnt & 0x01) ? (*pbcd ++ & 0x0f) : (*pbcd >> 4));
		*pasc += ((*pasc > 9) ? ('A' - 10) : '0');
	}
	*pasc = 0;
	return 0;
}

int tr31block_format(unsigned char * out, unsigned int *outlen,
                     unsigned char * kek, int keklen, tr31_key_t * keyst )
{
	tr31_kbh_t kbh;
	unsigned char data[TR31_DATA_BCD_MAX];
	unsigned int datalen;
	unsigned char blocks[TR31_ENCRYPTED_BLOCK_BCD_MAX];
	unsigned int kbh_len;
	unsigned int keybit;
	unsigned char tmpkey[24], iv[8], indata[8], outdata[8],tmpdata[8];
	volatile unsigned int i = 0, j = 0;
#ifdef DEBUG_TR31_DATA
	volatile unsigned int k = 0;
#endif
	unsigned char ksnflag = 0;/* DUKPT KSN exist flag*/
	unsigned char CMAC_KM1[8];
	unsigned char CMAC_KM2[8];
	unsigned char CMAC_MAC[8];

	if( out == NULL || outlen == NULL || kek == NULL ||
	        keyst == NULL )
		return -1;

	if( keklen != 2*8 && keklen != 3*8 )
		return -1;

	memset(tmpkey, 0, sizeof(tmpkey));
	/* TR31Header*/
	memset( (unsigned char *)&kbh, '0', sizeof( kbh ));
	kbh.ver_id = KBH_VERID;
	memcpy( kbh.usage, keyst->usage, 2 );
	kbh.algorithm = keyst->algorithm;
	kbh.mode = keyst->mode;
	memcpy( kbh.ver_num, keyst->ver_num, 2 );
	kbh.export = keyst->export;
	/*optional block*/
	if( keyst->algorithm == KEY_ALGORITHM_TDEA &&
	        keyst->mode == KEY_USEMODE_ENCRYPT &&
	        memcmp( keyst->usage, KEY_USAGE_BDK, 2 ) == 0 ) {		//KSN 存在
		if( keyst->ksn == NULL )
			return -1;
		ksnflag = 1;
		memcpy( kbh.option_nr, OPT2_NR, 2 );
	} else {
		memcpy( kbh.option_nr, OPT_NR, 2 );
	}
	memcpy( kbh.opt_id, OPT_ID, 2 );
	memcpy( kbh.opt_len,OPT_LEN, 2 );
	kbh.fid = '0' + keyst->fid;
	kbh.gid[0] = '0';
	kbh.gid[1] = '0' + keyst->gid / 10;
	kbh.gid[2] = '0' + keyst->gid % 10;
	if (ksnflag) {
		memcpy( kbh.opt2_id, OPT2_ID, 2 );
		memcpy( kbh.opt2_len,OPT2_LEN, 2 );
		_bcd_2_asc( keyst->ksn, DUKPT_KSN_LEN * 2, kbh.ksn, 0 );
	}

	/*封装数据段*/
	keybit = keyst->keylen << 3;
	datalen = 0;
	data[datalen++] = keybit >> 8;
	data[datalen++] = keybit % 256;
	memcpy( data + datalen, keyst->key, keyst->keylen );
	datalen += keyst->keylen;

	if( (datalen % 8) != 0 ) {			//填充随机数为8倍数
		rng_get_string( data + datalen, 8 - (datalen % 8) );
		datalen += 8 - (datalen % 8);
	}

	/*copy the KBH header to tmp[]*/
	kbh_len = TR31_HEAD_LEN + ksnflag * TR31_HEAD_OPT2_LEN + datalen * 2 + MAC_LEN * 2;
	kbh.length[0] = '0' + (kbh_len / 1000) % 10;
	kbh.length[1] = '0' + (kbh_len / 100) % 10;
	kbh.length[2] = '0' + (kbh_len / 10) % 10;
	kbh.length[3] = '0' + kbh_len % 10;
	memcpy( blocks, (unsigned char *)&kbh, TR31_HEAD_LEN + ksnflag * TR31_HEAD_OPT2_LEN );
	kbh_len = TR31_HEAD_LEN + ksnflag * TR31_HEAD_OPT2_LEN;

#ifdef USE_CMAC_TDEA	/*TR31-2010*/
	/*CMAC Key derivation*/
	_tr31_cmac_subkey_derivation(kek, keklen, CMAC_K1, CMAC_K2);
	/*Derivate the key block encryption key (KBEK)*/
	memset(indata, 0, sizeof(indata));
	if (keklen == 16) {
		indata[7] = 0x80;	/*length of 2-key TDEA*/
	} else {
		indata[5] = 0x01;	/*algorithm 3-key TDEA*/
		indata[7] = 0xC0;	/*length of 3-key TDEA*/
	}
	for(i=0; i<keklen; i+=8) {
		indata[0] += 1;	/*increase counter*/
		_tr31_key_derivation_method(kek, keklen, CMAC_K1, indata, CMAC_KBEK + i);
	}
#ifdef DEBUG_TR31_DATA
	fprintf(stderr,"CMAC_KBEK:");
	for(j=0; j<keklen; j++) {
		fprintf(stderr,"%02X ",CMAC_KBEK[j]);
	}
	fprintf(stderr,"\n");
#endif
	/*Derivate the key block MAC key (KBMK)*/
	indata[2] = 0x01;	/*set key usage indicator as MAC*/
	indata[0] = 0;	/*reset the counter*/
	for(i=0; i<keklen; i+=8) {
		indata[0] += 1;	/*increase counter*/
		_tr31_key_derivation_method(kek, keklen, CMAC_K1, indata, CMAC_KBMK + i);
	}
#ifdef DEBUG_TR31_DATA
	fprintf(stderr,"CMAC_KBMK:");
	for(j=0; j<keklen; j++) {
		fprintf(stderr,"%02X ",CMAC_KBMK[j]);
	}
	fprintf(stderr,"\n");
#endif
	/*CMAC subkey derivation from the key block MAC key (KBMK)*/
	_tr31_cmac_subkey_derivation(CMAC_KBMK, keklen, CMAC_KM1, CMAC_KM2);

	/*CMAC generation*/
#ifdef DEBUG_TR31_DATA
	/*Calculate CMAC from Raw KBH*/
	fprintf(stderr,"CMAC KBH\n");
#endif
	memset(iv, 0, 8); /* Initial Vector is 0  */
	for (i = 0; i < kbh_len; i += 8) {
		memcpy(tmpdata, blocks + i,8);
#ifdef DEBUG_TR31_DATA
		for(k=0; k<8; k++) {
			fprintf(stderr,"%02X ",tmpdata[k]);
		}
		fprintf(stderr,"\n");
#endif
		XORDATAJ(j, iv, tmpdata, 8, indata);
		_tr31_des3(1, indata, outdata, CMAC_KBMK, keklen);	// encrypt
#ifdef DEBUG_TR31_DATA
		for(k=0; k<8; k++) {
			fprintf(stderr,"%02X ",outdata[k]);
		}
		fprintf(stderr,">\n");
#endif
		memcpy(iv, outdata, 8);
	}
	/*Calculate CMAC from Raw KSt*/
#ifdef DEBUG_TR31_DATA
	fprintf(stderr,"CMAC KSt\n");
#endif
	for (i = 0; i < datalen; i += 8) {
		memcpy(tmpdata, data + i, 8);
#ifdef DEBUG_TR31_DATA
		for(k=0; k<8; k++) {
			fprintf(stderr,"%02X ",tmpdata[k]);
		}
		fprintf(stderr,">\n");
#endif
		if(i+8 >= datalen) { //
			/* Block size of TDEA block cipher is 8, and the blklen is always multiple of 8.
			 * so we only use K1 to xor with the last block.
			 * Please note that, for AES block cipher, block size b is 16, so if the message length ("blklen")
			 * isn't multiple of 16, then this last block should be xor'ed with K2*/
			XORDATAJ(j,  tmpdata, CMAC_KM1, 8,  tmpdata);
		}
		XORDATAJ(j, iv, tmpdata, 8, indata);
		_tr31_des3(1, indata, outdata, CMAC_KBMK, keklen);	// encrypt

#ifdef DEBUG_TR31_DATA
		for(k=0; k<8; k++) {
			fprintf(stderr,"%02X ",outdata[k]);
		}
		fprintf(stderr,"\n");
#endif
		memcpy(iv, outdata, 8);
	}
	memcpy(CMAC_MAC, outdata, MAC_LEN);

	/*Confidential data encryption*/
#ifdef DEBUG_TR31_DATA
	fprintf(stderr,"CMAC Encrypt Data\n");
#endif
	memcpy(iv, CMAC_MAC, MAC_LEN); /* Initial Vector is CMAC */
	for (i = 0; i < datalen; i += 8) {
#ifdef DEBUG_TR31_DATA
		for(k=0; k<8; k++) {
			fprintf(stderr,"%02X ",data[i + k]);
		}
		fprintf(stderr,">\n");
#endif
		XORDATAJ(j, iv, data + i, 8, indata);
		_tr31_des3(1, indata, outdata, CMAC_KBEK, keklen);
		memcpy(iv, outdata, 8);
		memcpy(blocks + kbh_len + i, outdata, 8);
#ifdef DEBUG_TR31_DATA
		for(k=0; k<8; k++) {
			fprintf(stderr,"%02X ",outdata[k]);
		}
		fprintf(stderr,"\n");
#endif
	}
	memcpy(blocks + kbh_len + datalen, CMAC_MAC, MAC_LEN);

	/*输出TR31格式*/
	memcpy(out, blocks, kbh_len);
	_bcd_2_asc( blocks + kbh_len, (datalen + MAC_LEN) * 2,
	            out + kbh_len, 0 );
	*outlen = kbh_len + (datalen + MAC_LEN) * 2;

#else	/*TR31-2005*/

	/*加密数据*/
	for (i = 0; i < keklen; i++)
		tmpkey[i] = kek[i]^'E';
	if (keklen == 16)
		memcpy(tmpkey+16, tmpkey, 8);

	memcpy(iv, (unsigned char *)&kbh, 8); /* Initial Vector is byte 0 - 7 of the KBH */
	for (i = 0; i < datalen; i += 8) {
		XORDATAJ(j, iv, data + i, 8, indata);
		_tr31_des3(1, indata, outdata, tmpkey);
		memcpy(iv, outdata, 8);
		memcpy( blocks + kbh_len, outdata, 8 );
		kbh_len += 8;
	}

	/*计算MAC*/
	for (i = 0; i < keklen; i++)
		tmpkey[i] = kek[i]^'M';
	if (keklen == 16)
		memcpy(tmpkey+16, tmpkey, 8);

	memset(iv, 0, 8); /* Initial Vector is byte 0  */
	for (i = 0; i < kbh_len; i += 8) {
		XORDATAJ(j, iv, blocks + i, 8, indata);
		_tr31_des3(1, indata, outdata, tmpkey);
		memcpy(iv, outdata, 8);
	}

	memcpy( blocks + kbh_len, outdata, MAC_LEN );
	kbh_len += MAC_LEN;

	/*输出TR31格式*/
	*outlen = TR31_HEAD_LEN + ksnflag * TR31_HEAD_OPT2_LEN;
	memcpy( out, blocks, *outlen);
	_bcd_2_asc( blocks + *outlen, (kbh_len - *outlen) * 2,
	            out + *outlen, 0 );
	*outlen += (kbh_len - *outlen) * 2;
#endif

	return 0;
}

int sec_tr31block_format(unsigned char * out, unsigned int *outlen, unsigned char * kek, int keklen, tr31_key_t * keyst)
{
	int ret;

	if( out == NULL || outlen == NULL || kek == NULL || keyst == NULL )
		return -1;

	keyst->fid = 0;
	keyst->export = 'E';
	keyst->ver_num[0] = '0';
	keyst->ver_num[1] = '0';

	ret=tr31block_format(out, outlen,  kek, keklen, keyst);

	return ret;
}
#endif

/****************************************************************
* function name 	 				:sec
* functional description 	: sec模块入口主函数,可根据需要扩展
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						: author		date		remarks
*			  	  	  				  linrq       20130301	created
*****************************************************************/
void sec(PF* list, unsigned char id[])
{
	/*private & local definition*/
#if !K21_ENABLE	
	int ret = -1, status = 0;
#endif
	/*process body*/
	cls_show_msg1(1, "%s测试...", TESTMODULE);

#if ANDIROD_ENABLE
	int ret = -1, keyin = 0;
	unsigned int unLen=0;
	char sBuf[128]={0};

	while(1)
   {
		keyin = cls_show_msg1(30,"1.不设置密钥属主\n"	//不设置的话是根据上一次的设置生效
						"2.设置密钥属主为*\n"	//设置后密钥存在K21端
						"3.设置密钥属主为空串\n"//设置后密钥存在安卓端
						"4.设置密钥属主为appname\n"//设置后密钥存在安卓端
						);
		switch(keyin) 
		{
		case '1':
			if(cls_show_msg("请确保选此项之前有进行设置密钥属主,若无先设置后再选此项进行测试,ESC退出")==ESC)
				return;
			break;
		case '2':
			if((ret=NDK_SecSetKeyOwner("*"))!=NDK_OK)
			{
				send_result("line %d:设置密钥属主失败(%d)", __LINE__, ret);
				return;
			}
			g_seckeyowner = 0;
			break;
		case '3':
			if((ret=NDK_SecSetKeyOwner(""))!=NDK_OK)
			{
				send_result("line %d:设置密钥属主失败(%d)", __LINE__, ret);
				return;
			}
			g_seckeyowner = 1;
			break;
		case '4':
			if((ret=NDK_SecSetKeyOwner("appname"))!=NDK_OK)
			{
				send_result("line %d:设置密钥属主失败(%d)", __LINE__, ret);
				return;
			}
			g_seckeyowner = 1;
			break;
		default:
		       if(auto_flag==1)//自动化测试时直接设为*密钥存在K21端
			{
				if((ret=NDK_SecSetKeyOwner("*"))!=NDK_OK)
				{
					send_result("line %d:设置密钥属主失败(%d)", __LINE__, ret);
					return;
				}
				g_seckeyowner = 0;
		       	}
			else   //非自动化测试需要进行设置
				continue; 
			break;
		}
		break;
	}
#if defined N910P  // 强制设为*  不管设为什么都将被设为*存在K21端 20180809 
	NDK_SecSetKeyOwner("*");
	g_seckeyowner = 0;
#endif
	//判断是sdk2.0还是sdk3.0版本  20180724 sull add
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_BIOS_VER, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:获取BIOS信息失败(%d)", __LINE__, ret);
		return;
	}
	if((sBuf[5]-'0')==3 ||(sBuf[5]-'0')==0 ) //第6位为3代表sdk3.0，其它的都是sdk2.0   PCI版本2.07_00_01025801 第6位为0
		flag_sdk3 = 1;
#endif

#if !K21_ENABLE	
	if(((ret=NDK_SecGetTamperStatus(&status))!=NDK_OK)&&(status!=SEC_TAMPER_STATUS_NONE))
	{
		cls_show_msg("line %d:安全触发,请检查!(%d,%d)", __LINE__, ret, status);
		return;
	}
	
	NDK_SecKeyErase();
	process(list, id);
	NDK_SecKeyErase();
#else
	process(list, id);
#endif
	//模块测试结束

#if ANDIROD_ENABLE//为让高端产品的其它程序能够正常使用根密钥,在退出安全模块时进行一个16字节0x31默认根密钥安装,20170302 linqy提出
	ST_SEC_KEY_INFO stKeyInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;	
	
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, 0x31, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:装载TLK测试失败(%d)", __LINE__, ret);
		return;
	}
#endif
	return;
}

