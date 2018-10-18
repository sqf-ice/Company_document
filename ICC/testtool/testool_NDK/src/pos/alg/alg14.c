/************************************************************************
*
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			: 算法模块
* file name		    : alg14.c
* Author 			: zhengry	
* version			:
* DATE				: 20150914
* directory 		:
* description		: 测试NDK_AlgSM3Start和NDK_AlgSM3Update函数根据SM3算法以分组方式（除最后一个分组以外，每个分组长度须为64字节整数倍）生成数字摘要（32字节）的功能（国密算法）
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
#define 	TESTAPI		"NDK_AlgSM3Start、NDK_AlgSM3Update"

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
void alg14(void)
{
	int ret=0, i=0;
	uchar null_buf[64]={0}, out_data[33]={0};
	uchar data_buf_64[]="`~!/1@新$#[^%]*(大)'-_=+|&{}:陆;?/><.,abcdefghijklmnopqrst012345";
	uchar err_buf_64[]="abcdefghijklmnopqrstuvwxyz`~!01@新$#[^%]*(大)'-_=+|&{}:陆;?/><.,";
	uchar long_buf_2560[]="daferjaqbhkjnd新anflktoek4ngklnfdlkvnkdmxfkjvnalkjdfpo大powejiefad`2112312陆3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())u&*^&^&%&^$%%#$%~!@#{}{}:>>>?DANJGEURJHDJHFAGEIUGRJHBJKBZJEIURjaoidfknfnejwhioufahoijpofjsdpoi[-o2=i40u12098uy498ytuhrfjgjcxzh98er0-320u5r0989sduifhajbdfnbzmn,mmlkpi-0i-2u!!~$%$&**)!@%#)$#:,^#dfaheruhaiejgrjgrgergfbgefraddfadsaerwtrtytugfvhnbvczxwaeq5rtyuiukbvcadfyr86798776543242fvdhgjiytdfgvqewwriuyjhgbvfqdertyuiuuhgew234567~!@#$%^&*()_+{}[]:?|<>?/.,1';]puguhaepohnjdmyujolyhdafyhbjkwejhiugfghjvnm,.m,cvcvxbtrrteryuuoughjh68uidfcjkllhgjkjl9o87656787907yhiunjkanfjkfadsfffffffffffffffffffffffeeeerrrrwtttttqertyuiytrewrtyuiouyterwetyuppoiuyterwrfdszfvbah2981y58u4tiiiiiii36-o=-op[sdflknvkkpoigfgsrdtyuiyuopoo234789-0876546tyughbjkv@$&*()(&^%&*(YTUHGJKERUTrYTTYIOGo**(&^%*(&&)&hgfgjhghjhklkhgy&i^t^%^*()_ikljh)_lk*/*-++kjljhgdfbmvnmiourtuiop)(**&^$%^*()*&tugjkjhgcvjbknklYUTIYDOUPAFUIHERYTWT106557daferjaqbhkjndskanflktoek4ngklnfdlkvnkdmxfkjvnalkjdfpojapowejiefad`21123128u3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())u&*^&^&%&^$%%#$%~!@#{}{}:>>>?DANJGEURJHDJHFAGEIUGRJHBJKBZJEIURjaoidfknfnejwhioufahoijpofjsdpoi[-o2=i40u12098uy498ytuhrfjgjcxzh98er0-320u5r0989sduifhajbdfnbzmn,mmlkpi-0i-2u!!~$%$&**)!@%#)$#:,^#dfaheruhaiejgrjgrgergfbgefraddfadsaerwtrtytugfvhnbvczxwaeq5rtyuiukbvcadfyr86798776543242fvdhgjiytdfgvqewwriuyjhgbvfqdertyuiuuhgew234567~!@#$%^&*()_+{}[]:?|<>?/.,0';]puguhaepohnjdmyujolyhdafyhbjkwejhiugfghjvnm,.m,cvcvxbtrrteryuuoughjh68uidfcjkllhgjkjl9o87656787907yhiunjkanfjkfadsfffffffffffffffffffffffeeeerrrrwtttttqertyuiytrewrtyuiouyterwetyuppoiuyterwrfdszfvbah2981y58u4tiiiiiii36-o=-op[sdflknvkkpoigfgsrdtyuiyuopoo234789-0876546tyughbjkv@$&*()(&^%&*(YTUHGJKERUTrYTTYIOGo**(&^%*(&&)&hgfgjhghjhklkhgy&i^t^%^*()_ikljh)_lk*/*-++kjljhgdfbmvnmiourtuiop)(**&^$%^*()*&tugjkjhgcvjbknklYUTIYDOUPAFUIHERYTWT106557`~!01@新$#[^%]*(大)'-_=+|&{}:陆;?/><.,dlkvnkdmxfkjvnalkjdfpojapowejiefad`21123128u349kanflktoekc8f8a82a567c6df5f500f421335a05aa47a3721887ca0b2acc37a905eb3ee94c03782ff3c0d90987dc875aa23c94094c4712c9b9cf4ee6c40e0d70aa271d24f8fc13fb6461774879141b5e9925e0c664d4706e1a296d8d8c83e6f61fd810c50002c21f36c9ab6d4dd0a1c1def150973ad868f44e1e592286b8f5e144bee9263b3bdc5b642f10777bcd4993829bda479ac89f90aeec76e7671f75079161904a4639725aa0d8b1774ea05d1f0e774b8f51d3f4c0d223c43a913a9b443170bf8ccafde060e525688434a96c5b704ac287d212a7d051130bdd91061cc4a5dd0f30e4c9d139cdb3897633394d324117b894754d1f503bec3f57f32ccb4dfad6060fb13db03df728a01166ddfpo大powejiefad`2112312陆3498324y58y098u-45irosdakjniuhoidso!!@@#$*(Y489DAHI())";
	uchar pre_hash[32]={0xda,0xda,0x9c,0x49,0x6a,0x36,0x64,0x35,
		                0x8d,0x2b,0x23,0x3e,0x71,0x36,0x06,0x80,
		                0xc2,0x22,0xce,0x3b,0xc4,0xe7,0xcd,0xde,
		                0x42,0x07,0xb3,0xd7,0x08,0x77,0x75,0xd7};//SM3工具算得data_buf_64数据的实际输出摘要
	uchar pre_hash1[32]={0xae,0x33,0x34,0xf9,0x28,0x85,0x50,0x60,
						 0x7f,0xc4,0x86,0x73,0x45,0x18,0x75,0x66,
						 0x42,0x15,0x9f,0x11,0xbd,0x8d,0x79,0x2e,
						 0x3d,0x46,0x4d,0x8c,0xc1,0xc4,0xa9,0xa3}; //SM3工具算得long_buf_2560数据的实际输出摘要
	
	//process body
	cls_printf("测试%s...", TESTAPI);

	//测试前置
	memset(null_buf, 0, sizeof(null_buf));
	memset(out_data, 0, sizeof(out_data));

	//case1:异常测试:若未启动，update应该失败
	if((ret=NDK_AlgSM3Update(data_buf_64, strlen((char *)data_buf_64)))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN; 
	}
	
	//case2:参数异常测试 pDat为Null，数据为非64字节
	if((ret=NDK_AlgSM3Start())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM3Update(NULL, strlen((char *)data_buf_64)))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM3Update(data_buf_64, 65))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN; 
	}
	if((ret=NDK_AlgSM3Update(data_buf_64, 0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:第一次调用start，第一次传垃圾数据、第二次再调用start，数据应该跟新,跟工具计算出的摘要应该一致
	if((ret=NDK_AlgSM3Update(err_buf_64, strlen((char *)err_buf_64)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#if !(ANDIROD_ENABLE||CPUMDM_ENABLE)  //中低端涉及国密的新增错误码，新增子用例验证 20180312 by linying
	//case3.1:没有释放锁之前，再调用一次star会返回-6001 NDK_ERR_GM_SM2_3_BUSY
	if((ret=NDK_AlgSM3Start())!=NDK_ERR_GM_SM2_3_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//需要调用Final释放锁
	if((ret=NDK_AlgSM3Final(null_buf, 0, out_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif
	if((ret=NDK_AlgSM3Start())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM3Update(data_buf_64, strlen((char *)data_buf_64)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM3Final(null_buf, 0, out_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(out_data[32]!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	if(memcmp(out_data, pre_hash, sizeof(pre_hash)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case4: 数据为2560字节, 循环更新，输出摘要应跟工具计算出的摘要一致
	memset(out_data, 0, sizeof(out_data));
	if((ret=NDK_AlgSM3Start())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	for(i=0; i<40; i++)
	{
		if((ret=NDK_AlgSM3Update(long_buf_2560+64*i, 64))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
	}
	if((ret=NDK_AlgSM3Final(null_buf, 0, out_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(out_data, pre_hash1, sizeof(pre_hash1)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case5:数据为2560字节，一次性更新，输出摘要应跟工具计算出的摘要一致
	memset(out_data, 0, sizeof(out_data));
	if((ret=NDK_AlgSM3Start())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM3Update(long_buf_2560, strlen((char *)long_buf_2560)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM3Final(null_buf, 0, out_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(out_data, pre_hash1, sizeof(pre_hash1)))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
#if !(ANDIROD_ENABLE || CPUMDM_ENABLE) //中低端涉及国密的新增错误码，新增子用例验证 20180312 by linying
    	//case6:测试进行SM3运算过程中，未释放锁，再进行SM2运算，会返回-6001 NDK_ERR_GM_SM2_3_BUSY
	if((ret=NDK_AlgSM3Start())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM3Update(data_buf_64, strlen((char *)data_buf_64)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
    if((ret=NDK_AlgSM2KeyPairGen(NULL, NULL))!=NDK_ERR_GM_SM2_3_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AlgSM3Final(null_buf, 0, out_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
#endif
	//测试结束
	send_result("%s测试通过", TESTAPI); 	
	return;
}
