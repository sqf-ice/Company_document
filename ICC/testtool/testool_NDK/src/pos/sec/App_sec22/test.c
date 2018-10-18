/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 
* file name	: 
* Author 		: huangjb
* version		: 
* DATE			: 20140523
* directory 		: 
* description		: 密钥校验
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
#include "NDK.h"
#include <fcntl.h>

void main(void)
{
	int ret;
	uchar udesin[31]={0},udesout[31]={0};
	memset(udesin, 0x20, sizeof(udesin));
	
	NDK_ScrInitGui();
	
	//case5:B验证B1、B2应成功且正确
	//B验证B1成功且正确
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		NDK_ScrPrintf("line:%d子应用B调用密钥加密失败%d",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	if(memcmp(udesout, "\xb4\x79\x4a\xdf\xd4\x70\xe3\xcf", 8))
	{
		NDK_ScrPrintf("line:%d子应用B加密结果错误\n",__LINE__);
		NDK_ScrPrintf("0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x", udesout[0], udesout[1],udesout[2],udesout[3],udesout[4],udesout[5],udesout[6],udesout[7]);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	
	//B验证B2成功且结果正确
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		NDK_ScrPrintf("line:%d子应用B调用密钥加密失败%d",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 8))
	{
		NDK_ScrPrintf("line:%d子应用B加密结果错误\n",__LINE__);
		NDK_ScrPrintf("0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x", udesout[0], udesout[1],udesout[2],udesout[3],udesout[4],udesout[5],udesout[6],udesout[7]);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	
	//case6:B擦除B1、B2密钥后验证B1、B2都应失败。
	//擦除密钥
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		NDK_ScrPrintf("line:%d B擦除密钥失败\n",__LINE__);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	//验证B1应失败
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 1, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECCR_GET_KEY)
	{
		NDK_ScrPrintf("line:%d子应用B调用密钥加密失败%d",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	//验证B2应失败
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 8, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8/*|SEC_DES_ECBMODE*/))!=NDK_ERR_SECCR_GET_KEY)
	{
		NDK_ScrPrintf("line:%d B擦除密钥后验证失败\n",__LINE__);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	
	NDK_ScrPrintf("应用B执行成功\n");
	NDK_ScrRefresh();
	NDK_KbGetCode(0,NULL);
	return;
}

