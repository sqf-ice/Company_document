/************************************************************************
*
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			: 算法模块
* file name		    : alg11.c
* Author 			: zhengry	
* version			:
* DATE				: 20150908
* directory 		:
* description		: 测试NDK_AlgSM2GenE函数生成SM2非对称公私钥对对应的数字摘要（32字节）的功能（注意：在该算法中，运算还需要用到ID、对应SM2非对称公私钥对的公钥等数据）（国密算法）
* related document	: NDK.chm
*
************************************************************************
* log			:
* Revision
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_AlgSM2GenE"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	:
* functional description 	:
* input parameter	 	:
* output parameter	 	:
* return value		 	:
* history 		 		: author			date			remarks
*			  	  	      zhengry           20150908  	    created
*****************************************************************/
void alg11(void)
{
	int ret=0;
	uchar prikey[32]={0}, pubkey[64]={0}, summary[32]={0}, summary1[33]={0}, sign_data[64]={0}, message[]="newland";
	uchar cha_message[]="`~!0a@新$#[^%]*(大)'-_=+|&{}:陆;?/><.,";//支持中文和特殊字符
	uchar message_1023[]="daferjaqbhkjndskanflktoek4ngklnfdlkvnkdmxfkjvnalkjdfpojapowejiefad`21123128u3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())u&*^&^&%&^$%%#$%~!@#{}{}:>>>?DANJGEURJHDJHFAGEIUGRJHBJKBZJEIURjaoidfknfnejwhioufahoijpofjsdpoi[-o2=i40u12098uy498ytuhrfjgjcxzh98er0-320u5r0989sduifhajbdfnbzmn,mmlkpi-0i-2u!!~$%$&**)!@%#)$#:,^#dfaheruhaiejgrjgrgergfbgefraddfadsaerwtrtytugfvhnbvczxwaeq5rtyuiukbvcadfyr86798776543242fvdhgjiytdfgvqewwriuyjhgbvfqdertyuiuuhgew234567~!@#$%^&*()_+{}[]:?|<>?/.,0';]puguhaepohnjdmyujolyhdafyhbjkwejhiugfghjvnm,.m,cvcvxbtrrteryuuoughjh68uidfcjkllhgjkjl9o87656787907yhiunjkanfjkfadsfffffffffffffffffffffffeeeerrrrwtttttqertyuiytrewrtyuiouyterwetyuppoiuyterwrfdszfvbah2981y58u4tiiiiiii36-o=-op[sdflknvkkpoigfgsrdtyuiyuopoo234789-0876546tyughbjkv@$&*()(&^%&*(YTUHGJKERUTrYTTYIOGo**(&^%*(&&)&hgfgjhghjhklkhgy&i^t^%^*()_ikljh)_lk*/*-++kjljhgdfbmvnmiourtuiop)(**&^$%^*()*&tugjkjhgcvjbknklYUTIYDOUPAFUIHERYTWT106557`~!01@新$#[^%]*(大)'-_=+|&{}:陆;?/><.,dlkvnkdmxfkjvnalkjdfpojapowejiefad`21123128u349kanflktoek";
	uchar message_3668[]="daferjaqbhkjnd新anflktoek4ngklnfdlkvnkdmxfkjvnalkjdfpo大powejiefad`2112312陆3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())u&*^&^&%&^$%%#$%~!@#{}{}:>>>?DANJGEURJHDJHFAGEIUGRJHBJKBZJEIURjaoidfknfnejwhioufahoijpofjsdpoi[-o2=i40u12098uy498ytuhrfjgjcxzh98er0-320u5r0989sduifhajbdfnbzmn,mmlkpi-0i-2u!!~$%$&**)!@%#)$#:,^#dfaheruhaiejgrjgrgergfbgefraddfadsaerwtrtytugfvhnbvczxwaeq5rtyuiukbvcadfyr86798776543242fvdhgjiytdfgvqewwriuyjhgbvfqdertyuiuuhgew234567~!@#$%^&*()_+{}[]:?|<>?/.,1';]puguhaepohnjdmyujolyhdafyhbjkwejhiugfghjvnm,.m,cvcvxbtrrteryuuoughjh68uidfcjkllhgjkjl9o87656787907yhiunjkanfjkfadsfffffffffffffffffffffffeeeerrrrwtttttqertyuiytrewrtyuiouyterwetyuppoiuyterwrfdszfvbah2981y58u4tiiiiiii36-o=-op[sdflknvkkpoigfgsrdtyuiyuopoo234789-0876546tyughbjkv@$&*()(&^%&*(YTUHGJKERUTrYTTYIOGo**(&^%*(&&)&hgfgjhghjhklkhgy&i^t^%^*()_ikljh)_lk*/*-++kjljhgdfbmvnmiourtuiop)(**&^$%^*()*&tugjkjhgcvjbknklYUTIYDOUPAFUIHERYTWT106557daferjaqbhkjndskanflktoek4ngklnfdlkvnkdmxfkjvnalkjdfpojapowejiefad`21123128u3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())u&*^&^&%&^$%%#$%~!@#{}{}:>>>?DANJGEURJHDJHFAGEIUGRJHBJKBZJEIURjaoidfknfnejwhioufahoijpofjsdpoi[-o2=i40u12098uy498ytuhrfjgjcxzh98er0-320u5r0989sduifhajbdfnbzmn,mmlkpi-0i-2u!!~$%$&**)!@%#)$#:,^#dfaheruhaiejgrjgrgergfbgefraddfadsaerwtrtytugfvhnbvczxwaeq5rtyuiukbvcadfyr86798776543242fvdhgjiytdfgvqewwriuyjhgbvfqdertyuiuuhgew234567~!@#$%^&*()_+{}[]:?|<>?/.,0';]puguhaepohnjdmyujolyhdafyhbjkwejhiugfghjvnm,.m,cvcvxbtrrteryuuoughjh68uidfcjkllhgjkjl9o87656787907yhiunjkanfjkfadsfffffffffffffffffffffffeeeerrrrwtttttqertyuiytrewrtyuiouyterwetyuppoiuyterwrfdszfvbah2981y58u4tiiiiiii36-o=-op[sdflknvkkpoigfgsrdtyuiyuopoo234789-0876546tyughbjkv@$&*()(&^%&*(YTUHGJKERUTrYTTYIOGo**(&^%*(&&)&hgfgjhghjhklkhgy&i^t^%^*()_ikljh)_lk*/*-++kjljhgdfbmvnmiourtuiop)(**&^$%^*()*&tugjkjhgcvjbknklYUTIYDOUPAFUIHERYTWT106557`~!01@新$#[^%]*(大)'-_=+|&{}:陆;?/><.,dlkvnkdmxfkjvnalkjdfpojapowejiefad`21123128u349kanflktoekc8f8a82a567c6df5f500f421335a05aa47a3721887ca0b2acc37a905eb3ee94c03782ff3c0d90987dc875aa23c94094c4712c9b9cf4ee6c40e0d70aa271d24f8fc13fb6461774879141b5e9925e0c664d4706e1a296d8d8c83e6f61fd810c50002c21f36c9ab6d4dd0a1c1def150973ad868f44e1e592286b8f5e144bee9263b3bdc5b642f10777bcd4993829bda479ac89f90aeec76e7671f75079161904a4639725aa0d8b1774ea05d1f0e774b8f51d3f4c0d223c43a913a9b443170bf8ccafde060e525688434a96c5b704ac287d212a7d051130bdd91061cc4a5dd0f30e4c9d139cdb3897633394d324117b894754d1f503bec3f57f32ccb4dfad6060fb13db03df728a01166d!@#$%^&*()dhh(&#$UUDFHBdjw~d8u823uhuhbdhfb9239794375694698977*xi9u93u49098034y57981yhjbjkfdsa09-09u98432yu9841y新大陆.,><<<<}{}|:;~!@#$fduaghufgeuafuuhuidhafoihewoihdaferjaqbhkjndskanflktoek4ngklnfdlkvnkdmxfkjvnalkjdfpojapowejiefad`21123128u3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())u&*^&^&%&^$%%#$%~!@#{}{}:>>>?DANJGEURJHDJHFAGEIUGRJHBJKBZJEIURjaoidfknfnejwhioufahoijpofjsdpoi[-o2=i40u12098uy498ytuhrfjgjcxzh98er0-320u5r0989sduifhajbdfnbzmn,mmlkpi-0i-2u!!~$%$&**)!@%#)$#:,^#dfaheruhaiejgrjgrgergfbgefraddfadsaerwtrtytugfvhnbvczxwaeq5rtyuiukbvcadfyr86798776543242fvdhgjiytdfgvqewwriuyjhgbvfqdertyuiuuhgew234567~!@#$%^&*()_+{}[]:?|<>?/.,0';]puguhaepohnjdmyujolyhdafyhbjkwejhiugfghjvnm,.m,cvcvxbtrrteryuuoughjh68uidfcjkllhgjkjl9o87656787907yhiunjkanfjkfadsfffffffffffffffffffffffeeeerrrrwtttttqertyuiytrewrtyuiouyterwetyuppoiuyterwrfdszfvbah2981y58u4tiiiiiii36-o=-op[sdflknvkkpoigfgsrdtyuiyuopoo234789-0876546tyughbjkv@$&*()(&^%&*(YTUHGJKERUTrYTTYIOGo**(&^%*(&&)&hgfgjhghjhklkhgy&i^t^%^*()_ikljh)_lk*/*-++kjljhgdfbmvnmiourtuiop)(**&^$%^*()*&tugjkjhgcvjbknklYUTIYDOUPAFUIHERYTWT106557`~!01@新$#[^%]*(大)'-_=+|&{}:陆;?/><.,dlkvnkdmxfkjvnalkjdfpojapowejiefad`21123128u349kanflktoek";
	uchar ID[]="test", ID_CH[]="测试";
	uchar pre_pubkey[64]={0x97, 0xbc, 0x43, 0x9c, 0x57, 0xe1, 0xd6, 0x2e,
						  0x4d, 0xb8, 0x1b, 0x70, 0x72, 0x6a, 0xdb, 0x5f,
						  0xf7, 0xc7, 0x3a, 0xe3, 0xb6, 0xcb, 0xc5, 0x9b,
						  0xad, 0xb8, 0x2d, 0x65, 0xf7, 0x5e, 0xd6, 0xc1,
						  0x9d, 0xf6, 0xcb, 0xe6, 0x31, 0x21, 0x3a, 0xe6,
						  0x21, 0x24, 0xe7, 0xd7, 0xb2, 0x92, 0x19, 0x69,
						  0x20, 0x2c, 0x64, 0xd1, 0x11, 0xce, 0xa5, 0x1c,
						  0x83, 0x08, 0x54, 0x58, 0xa2, 0x8e, 0xcc, 0x19};
	uchar pre_summary[32]={0xE2, 0xD6, 0xEF, 0x30, 0x48, 0x40, 0xA0, 0xDD,
						   0xB0, 0x53, 0x61, 0x34, 0x6B, 0x47, 0xEB, 0x7E,
						   0x37, 0xC0, 0x67, 0x10, 0xF5, 0xE9, 0x5C, 0xC4,
						   0x38, 0xED, 0x09, 0x19, 0x60, 0xB3, 0x40, 0x06};//message下对应的摘要
	uchar pre_summary1[32]={0xF1,0xBB,0xC4,0xFA,0x5A,0x27,0xCB,0x0C,
							0x6F,0x5B,0x8F,0x5E,0xCE,0x59,0xFC,0x28,
							0xAD,0x82,0x65,0x4D,0x7F,0x39,0x6F,0x9C,
							0x78,0x94,0xF6,0xAB,0x4C,0x43,0x36,0x67};//message_1023下对应的摘要	
	uchar pre_summary2[32]={0x0B,0x2F,0xE8,0x75,0xD2,0x4A,0xBE,0xBD,
							0xD2,0xAA,0x93,0xF9,0x30,0x56,0xF5,0x4F,
							0x96,0x3B,0x4C,0x9F,0x00,0x8F,0x56,0x21,
							0x7F,0x38,0x09,0x25,0xB5,0xD1,0xD0,0x67};//ID_CH下对应的摘要	
	uchar pre_summary3[32]={0x58,0x4A,0xB1,0xCB,0x4E,0xB3,0xCD,0xE2,
							0xEA,0xB5,0x03,0xC1,0x8A,0xB8,0x9E,0x4C,
							0x58,0xFD,0xBB,0xA5,0x96,0x51,0x29,0x8D,
							0xEA,0x55,0x37,0x3B,0xE9,0x90,0xDA,0x18};//ID=NULL,message_1023下对应的摘要	
	uchar pre_summary4[32]={0x62,0xAB,0xD6,0x22,0x5F,0x32,0xA4,0xB4,
							0xE8,0x19,0x9D,0x3B,0x61,0x0A,0x10,0x45,
							0x8A,0xE1,0xA0,0xD6,0x58,0x0E,0x86,0xD0,
							0xFB,0xB0,0xD8,0xB7,0x0D,0xED,0x78,0x92};//message_3668下对应的摘要
	//process body
	cls_printf("测试%s...", TESTAPI);

	//测试前置
	memset(prikey, 0, sizeof(prikey));
	memset(pubkey, 0, sizeof(pubkey));
	memset(summary, 0, sizeof(summary));
	memset(summary1, 0, sizeof(summary1));
	if((ret=NDK_AlgSM2KeyPairGen(pubkey, prikey))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	//case1:参数异常
	if((ret=NDK_AlgSM2GenE(0, ID, strlen((char *)message), message, pubkey, summary))!=NDK_ERR_PARA)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2GenE(strlen((char *)ID), ID, 0, message, pubkey, summary))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2GenE(strlen((char *)ID), ID, strlen((char *)message), NULL, pubkey, summary))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2GenE(strlen((char *)ID), ID, strlen((char *)message), message, NULL, summary))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2GenE(strlen((char *)ID), ID, strlen((char *)message), message, pubkey, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//生成摘要接口对传入的公钥值不做判断，传入私钥也会成功
/*
	//case2:异常测试:输入私钥，则生成摘要应该失败
	if((ret=NDK_AlgSM2GenE(strlen(ID), ID, strlen(message), message, prikey, summary))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
*/
	//case2:正常测试，当明文为1023字节，生成的摘要值应该为32字节，sm2应能成功
	if((ret=NDK_AlgSM2GenE(strlen((char *)ID), ID, strlen((char *)message_1023), message_1023, pubkey, summary1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(summary1[32]!=0)
	{
		send_result("line %d:%s生成摘要失败", __LINE__, TESTAPI);
		RETURN; 
	}
	if((ret=NDK_AlgSM2Sign(prikey, summary1, sign_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2Verify(pubkey, summary1, sign_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3:正常测试:明文中有特殊符号，生成摘要也能成功
	memset(summary, 0, sizeof(summary));
	if((ret=NDK_AlgSM2GenE(strlen((char *)ID), ID, strlen((char *)cha_message), cha_message, pubkey, summary))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2Sign(prikey, summary, sign_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM2Verify(pubkey, summary, sign_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case4:正常测试:预设的公钥和明文小数据，生成摘要应该与预设的一致
	memset(summary, 0, sizeof(summary));
	if((ret=NDK_AlgSM2GenE(strlen((char *)ID), ID, strlen((char *)message), message, pre_pubkey, summary))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(summary, pre_summary, sizeof(pre_summary)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case5:正常测试: 预设的公钥和明文1023字节数据,生成摘要应该与预设的一致
	memset(summary, 0, sizeof(summary));
	if((ret=NDK_AlgSM2GenE(strlen((char *)ID), ID, strlen((char *)message_1023), message_1023, pre_pubkey, summary))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(summary, pre_summary1, sizeof(pre_summary1)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case6:正常测试:带中文的ID，生成摘要应该与预设的一致
	memset(summary, 0, sizeof(summary));
	if((ret=NDK_AlgSM2GenE(strlen((char *)ID_CH), ID_CH, strlen((char *)message), message, pre_pubkey, summary))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(summary, pre_summary2, sizeof(pre_summary2)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case7:正常测试:当ID数据为NULL时候，默认使用ID=1234567812345678
	memset(summary, 0, sizeof(summary));
	if((ret=NDK_AlgSM2GenE(strlen((char *)ID), NULL, strlen((char *)message), message, pre_pubkey, summary))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(summary, pre_summary3, sizeof(pre_summary3)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case8:正常测试: (大数据)预设的公钥和明文2647字节数据,生成摘要应该与预设的一致
	memset(summary, 0, sizeof(summary));
	if((ret=NDK_AlgSM2GenE(strlen((char *)ID), ID, strlen((char *)message_3668), message_3668, pre_pubkey, summary))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(summary, pre_summary4, sizeof(pre_summary4)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI); 	
	return;
}



