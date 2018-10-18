/************************************************************************
*
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			: 算法模块
* file name		    : alg16.c
* Author 			: zhengry	
* version			:
* DATE				: 20150914
* directory 		:
* description		: 测试NDK_AlgSM3Compute函数根据SM3算法以一次性传入数据方式生成数字摘要（32字节）的功能（国密算法）
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
#define 	TESTAPI		"NDK_AlgSM3Compute"

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
*			  	  	      zhengry           20150914  	    created
*****************************************************************/
void alg16(void)
{
	int ret=0;
	uchar out_buf[32]={0}, out_buf1[33]={0};
	uchar data_buf_8[]="newland!";
	uchar data_buf_2625[]="daferjaqbhkjnd新anflktoek4ngklnfdlkvnkdmxfkjvnalkjdfpo大powejiefad`2112312陆3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())u&*^&^&%&^$%%#$%~!@#{}{}:>>>?DANJGEURJHDJHFAGEIUGRJHBJKBZJEIURjaoidfknfnejwhioufahoijpofjsdpoi[-o2=i40u12098uy498ytuhrfjgjcxzh98er0-320u5r0989sduifhajbdfnbzmn,mmlkpi-0i-2u!!~$%$&**)!@%#)$#:,^#dfaheruhaiejgrjgrgergfbgefraddfadsaerwtrtytugfvhnbvczxwaeq5rtyuiukbvcadfyr86798776543242fvdhgjiytdfgvqewwriuyjhgbvfqdertyuiuuhgew234567~!@#$%^&*()_+{}[]:?|<>?/.,1';]puguhaepohnjdmyujolyhdafyhbjkwejhiugfghjvnm,.m,cvcvxbtrrteryuuoughjh68uidfcjkllhgjkjl9o87656787907yhiunjkanfjkfadsfffffffffffffffffffffffeeeerrrrwtttttqertyuiytrewrtyuiouyterwetyuppoiuyterwrfdszfvbah2981y58u4tiiiiiii36-o=-op[sdflknvkkpoigfgsrdtyuiyuopoo234789-0876546tyughbjkv@$&*()(&^%&*(YTUHGJKERUTrYTTYIOGo**(&^%*(&&)&hgfgjhghjhklkhgy&i^t^%^*()_ikljh)_lk*/*-++kjljhgdfbmvnmiourtuiop)(**&^$%^*()*&tugjkjhgcvjbknklYUTIYDOUPAFUIHERYTWT106557daferjaqbhkjndskanflktoek4ngklnfdlkvnkdmxfkjvnalkjdfpojapowejiefad`21123128u3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())u&*^&^&%&^$%%#$%~!@#{}{}:>>>?DANJGEURJHDJHFAGEIUGRJHBJKBZJEIURjaoidfknfnejwhioufahoijpofjsdpoi[-o2=i40u12098uy498ytuhrfjgjcxzh98er0-320u5r0989sduifhajbdfnbzmn,mmlkpi-0i-2u!!~$%$&**)!@%#)$#:,^#dfaheruhaiejgrjgrgergfbgefraddfadsaerwtrtytugfvhnbvczxwaeq5rtyuiukbvcadfyr86798776543242fvdhgjiytdfgvqewwriuyjhgbvfqdertyuiuuhgew234567~!@#$%^&*()_+{}[]:?|<>?/.,0';]puguhaepohnjdmyujolyhdafyhbjkwejhiugfghjvnm,.m,cvcvxbtrrteryuuoughjh68uidfcjkllhgjkjl9o87656787907yhiunjkanfjkfadsfffffffffffffffffffffffeeeerrrrwtttttqertyuiytrewrtyuiouyterwetyuppoiuyterwrfdszfvbah2981y58u4tiiiiiii36-o=-op[sdflknvkkpoigfgsrdtyuiyuopoo234789-0876546tyughbjkv@$&*()(&^%&*(YTUHGJKERUTrYTTYIOGo**(&^%*(&&)&hgfgjhghjhklkhgy&i^t^%^*()_ikljh)_lk*/*-++kjljhgdfbmvnmiourtuiop)(**&^$%^*()*&tugjkjhgcvjbknklYUTIYDOUPAFUIHERYTWT106557`~!01@新$#[^%]*(大)'-_=+|&{}:陆;?/><.,dlkvnkdmxfkjvnalkjdfpojapowejiefad`21123128u349kanflktoekc8f8a82a567c6df5f500f421335a05aa47a3721887ca0b2acc37a905eb3ee94c03782ff3c0d90987dc875aa23c94094c4712c9b9cf4ee6c40e0d70aa271d24f8fc13fb6461774879141b5e9925e0c664d4706e1a296d8d8c83e6f61fd810c50002c21f36c9ab6d4dd0a1c1def150973ad868f44e1e592286b8f5e144bee9263b3bdc5b642f10777bcd4993829bda479ac89f90aeec76e7671f75079161904a4639725aa0d8b1774ea05d1f0e774b8f51d3f4c0d223c43a913a9b443170bf8ccafde060e525688434a96c5b704ac287d212a7d051130bdd91061cc4a5dd0f30e4c9d139cdb3897633394d324117b894754d1f503bec3f57f32ccb4dfad6060fb13db03df728a01166ddfpo大powejiefad`2112312陆3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())nalkjdfpojapowejief.`1ds12213344243155fdgsaer172t4687981uhgaguyt8";
	uchar data_buf_2560[]="daferjaqbhkjnd新anflktoek4ngklnfdlkvnkdmxfkjvnalkjdfpo大powejiefad`2112312陆3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())u&*^&^&%&^$%%#$%~!@#{}{}:>>>?DANJGEURJHDJHFAGEIUGRJHBJKBZJEIURjaoidfknfnejwhioufahoijpofjsdpoi[-o2=i40u12098uy498ytuhrfjgjcxzh98er0-320u5r0989sduifhajbdfnbzmn,mmlkpi-0i-2u!!~$%$&**)!@%#)$#:,^#dfaheruhaiejgrjgrgergfbgefraddfadsaerwtrtytugfvhnbvczxwaeq5rtyuiukbvcadfyr86798776543242fvdhgjiytdfgvqewwriuyjhgbvfqdertyuiuuhgew234567~!@#$%^&*()_+{}[]:?|<>?/.,1';]puguhaepohnjdmyujolyhdafyhbjkwejhiugfghjvnm,.m,cvcvxbtrrteryuuoughjh68uidfcjkllhgjkjl9o87656787907yhiunjkanfjkfadsfffffffffffffffffffffffeeeerrrrwtttttqertyuiytrewrtyuiouyterwetyuppoiuyterwrfdszfvbah2981y58u4tiiiiiii36-o=-op[sdflknvkkpoigfgsrdtyuiyuopoo234789-0876546tyughbjkv@$&*()(&^%&*(YTUHGJKERUTrYTTYIOGo**(&^%*(&&)&hgfgjhghjhklkhgy&i^t^%^*()_ikljh)_lk*/*-++kjljhgdfbmvnmiourtuiop)(**&^$%^*()*&tugjkjhgcvjbknklYUTIYDOUPAFUIHERYTWT106557daferjaqbhkjndskanflktoek4ngklnfdlkvnkdmxfkjvnalkjdfpojapowejiefad`21123128u3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())u&*^&^&%&^$%%#$%~!@#{}{}:>>>?DANJGEURJHDJHFAGEIUGRJHBJKBZJEIURjaoidfknfnejwhioufahoijpofjsdpoi[-o2=i40u12098uy498ytuhrfjgjcxzh98er0-320u5r0989sduifhajbdfnbzmn,mmlkpi-0i-2u!!~$%$&**)!@%#)$#:,^#dfaheruhaiejgrjgrgergfbgefraddfadsaerwtrtytugfvhnbvczxwaeq5rtyuiukbvcadfyr86798776543242fvdhgjiytdfgvqewwriuyjhgbvfqdertyuiuuhgew234567~!@#$%^&*()_+{}[]:?|<>?/.,0';]puguhaepohnjdmyujolyhdafyhbjkwejhiugfghjvnm,.m,cvcvxbtrrteryuuoughjh68uidfcjkllhgjkjl9o87656787907yhiunjkanfjkfadsfffffffffffffffffffffffeeeerrrrwtttttqertyuiytrewrtyuiouyterwetyuppoiuyterwrfdszfvbah2981y58u4tiiiiiii36-o=-op[sdflknvkkpoigfgsrdtyuiyuopoo234789-0876546tyughbjkv@$&*()(&^%&*(YTUHGJKERUTrYTTYIOGo**(&^%*(&&)&hgfgjhghjhklkhgy&i^t^%^*()_ikljh)_lk*/*-++kjljhgdfbmvnmiourtuiop)(**&^$%^*()*&tugjkjhgcvjbknklYUTIYDOUPAFUIHERYTWT106557`~!01@新$#[^%]*(大)'-_=+|&{}:陆;?/><.,dlkvnkdmxfkjvnalkjdfpojapowejiefad`21123128u349kanflktoekc8f8a82a567c6df5f500f421335a05aa47a3721887ca0b2acc37a905eb3ee94c03782ff3c0d90987dc875aa23c94094c4712c9b9cf4ee6c40e0d70aa271d24f8fc13fb6461774879141b5e9925e0c664d4706e1a296d8d8c83e6f61fd810c50002c21f36c9ab6d4dd0a1c1def150973ad868f44e1e592286b8f5e144bee9263b3bdc5b642f10777bcd4993829bda479ac89f90aeec76e7671f75079161904a4639725aa0d8b1774ea05d1f0e774b8f51d3f4c0d223c43a913a9b443170bf8ccafde060e525688434a96c5b704ac287d212a7d051130bdd91061cc4a5dd0f30e4c9d139cdb3897633394d324117b894754d1f503bec3f57f32ccb4dfad6060fb13db03df728a01166ddfpo大powejiefad`2112312陆3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())";
	uchar pre_hash[32]={0x82,0x24,0x93,0x44,0xf0,0xb1,0xb0,0x02,
					 	0x47,0x6a,0xb6,0xa2,0xd1,0xe8,0x31,0xde,
					 	0x7d,0x87,0x5f,0x2e,0xbe,0x05,0x38,0xf8,
					 	0x53,0x6f,0x38,0xeb,0x6f,0x2b,0xea,0xd4};//SM3工具算得data_buf_2625数据的实际输出摘要
	uchar pre_hash1[32]={0xae,0x33,0x34,0xf9,0x28,0x85,0x50,0x60,
						 0x7f,0xc4,0x86,0x73,0x45,0x18,0x75,0x66,
						 0x42,0x15,0x9f,0x11,0xbd,0x8d,0x79,0x2e,
						 0x3d,0x46,0x4d,0x8c,0xc1,0xc4,0xa9,0xa3};//SM3工具算得data_buf_2560数据的实际输出摘要
	
	//process body
	cls_printf("测试%s...", TESTAPI);

	//case1:参数异常测试
	if((ret=NDK_AlgSM3Compute(NULL, strlen((char *)data_buf_8), out_buf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM3Compute(data_buf_8, 0, out_buf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN; 
	}
	if((ret=NDK_AlgSM3Compute(data_buf_8, strlen((char *)data_buf_8), NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case2:正常测试:取非64字节的整数倍(2625字节)， 输出的摘要值应与工具生成的值一致，且输出的字节为32字节
	if((ret=NDK_AlgSM3Compute(data_buf_2625, strlen((char *)data_buf_2625), out_buf1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(out_buf1[32]!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	if(memcmp(out_buf1, pre_hash, sizeof(pre_hash)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case3:正常测试: 取64字节的整数倍(2560字节)， 输出的摘要值应与工具生成的值一致
	if((ret=NDK_AlgSM3Compute(data_buf_2560, strlen((char *)data_buf_2560), out_buf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(out_buf, pre_hash1, sizeof(out_buf)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI); 	
	return;
}

