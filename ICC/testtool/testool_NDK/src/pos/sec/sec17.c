/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec6.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecLoadTIK接口功能
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SecLoadTIK"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author		date		remarks
*			  	  	   linrq       20130228	created
*						jiangym			20150125	modified
*****************************************************************/
void sec17(void)
{
	int ret = 0;
	ST_SEC_KEY_INFO stKeyInfo;
	ST_SEC_KCV_INFO KcvInfoIn;
	uchar KeyValueIn[17]={0}, KsnIn[11]={0};
#if TR31_ENABLE
	tr31_key_t tr31_st;
	unsigned int len = 0;
	uchar block[512]={0},mkey[17]={0},skey[72]={0};
#endif

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI); 

	//测试前置:擦除所有密钥
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//安装TLK, ID=1
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x11, stKeyInfo.nDstKeyLen);
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//安装TMK, ID=2
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1:非法测试
#if 0 //101合法
	//case1.1:ucGroupIdx>100
	memset(KeyValueIn, 0x15, 16);
	memset(KsnIn, 0x10, 10);
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadTIK(101, 2, 16, KeyValueIn, KsnIn, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	//case1.2:ucSrcKeyIdx=10, 101
	memset(KeyValueIn, 0x17, 16);
	if ((ret=NDK_SecLoadTIK(3, 10, 16, KeyValueIn, KsnIn, &KcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0 //101合法
	memset(KeyValueIn, 0x19, 16);
	if ((ret=NDK_SecLoadTIK(3, 101, 16, KeyValueIn, KsnIn, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	//case1.3:psKeyValueIn=NULL  
	if ((ret=NDK_SecLoadTIK(3, 1, 16, NULL, KsnIn, &KcvInfoIn))!=NDK_ERR_PARA)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.4:psKsnIn=NULL
	memset(KeyValueIn, 0x1b, 16);
	if ((ret=NDK_SecLoadTIK(3, 1, 16, KeyValueIn, NULL, &KcvInfoIn))!=NDK_ERR_PARA)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case1.5:pstKcvInfoIn=NULL       开发解释:DUKPT体系可不传KCV
	memset(KeyValueIn, 0x1d, 16);
	if ((ret=NDK_SecLoadTIK(3, 1, 16, KeyValueIn, KsnIn, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	/*约定:以下KCV均以不校验方式安装*/
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	
	//case2:	ucSrcKeyIdx=0, ucGroupIdx=3, ucKeyLen=8,应失败
	memset(KeyValueIn, 0x1f, 8);
	memset(KsnIn, 0x12, 10);
	if ((ret=NDK_SecLoadTIK(3, 0, 8, KeyValueIn, KsnIn, &KcvInfoIn))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:ucSrcKeyIdx=0, ucGroupIdx=3, ucKeyLen=16,应成功
	memset(KeyValueIn, 0x21, 16);
	if ((ret=NDK_SecLoadTIK(3, 0, 16, KeyValueIn, KsnIn, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:ucSrcKeyIdx=1, ucGroupIdx=4, ucKeyLen=16,psKeyValueIn=16个0, 应失败   开发解释会成功
	memset(KeyValueIn, 0x00, 16);
	if ((ret=NDK_SecLoadTIK(4, 1, 16, KeyValueIn, KsnIn, &KcvInfoIn))!=NDK_OK/*NDK_ERR_SECKM_PARAM*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if TR31_ENABLE	//支持TR31格式密钥安装的设备才进行TR31测试
	//case5:ucSrcKeyIdx=1, ucGroupIdx=4, ucKeyLen=16,psKeyValueIn=好的TR-31包, 应成功
	memset(mkey, 0x11, 16);
	memset(skey, 0x27, 16);

	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = 4;
	memcpy(tr31_st.usage, KT_BDK_TYPE, 4);	
	tr31_st.key = skey;
	tr31_st.keylen = 16;
	
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadTIK(4, 1, len, block, KsnIn, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//允许在装载TR31格式密钥时传入KSN参数为空
	//case8:ucSrcKeyIdx=1, ucGroupIdx=7, ucKeyLen=16,psKeyValueIn=16的TR-31包, 应成功
	memset(mkey, 0x11, 16);
	memset(skey, 0x25, 16);

	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = 7;
	memcpy(tr31_st.usage, KT_BDK_TYPE, 4);	
	tr31_st.key = skey;
	tr31_st.keylen = 16;
	
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadTIK(7, 1, len, block, NULL, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case6:ucSrcKeyIdx=1, ucGroupIdx=5, ucKeyLen=16,psKeyValueIn=16字节密钥密文, 应成功
	memcpy(KeyValueIn,"\x8D\x6A\xDB\xB0\xAF\xED\x87\xDA\x8D\x6A\xDB\xB0\xAF\xED\x87\xDA", 16);   /*对应的密钥明文key为16个字节的0x23*/
	if ((ret=NDK_SecLoadTIK(5, 1, 16, KeyValueIn, KsnIn, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7:ucSrcKeyIdx=2, ucGroupIdx=6, ucKeyLen=16,psKeyValueIn=16字节密钥密文, 应失败(TMK与DUKPT属同级,不能进行发散)
	memcpy(KeyValueIn,"\x09\xAE\x2A\x30\xF0\x94\xFB\xBF\x09\xAE\x2A\x30\xF0\x94\xFB\xBF", 16);/*对应的密钥明文key为16个字节的0x25*/
	if ((ret=NDK_SecLoadTIK(6, 2, 16, KeyValueIn, KsnIn, &KcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	send_result("%s测试通过", TESTAPI);
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}

