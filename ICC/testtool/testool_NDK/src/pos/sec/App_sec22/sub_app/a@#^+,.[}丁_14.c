/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 
* file name	: 
* Author 		: jiangym
* version		: 
* DATE			: 20140815
* directory 		: 
* description		: 验证子应用可以使用*表的密钥进行calcdes
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
#include "NDK.h"

void main(void)
{
	int ret;
	uchar udesin[31]={0},udesout[31]={0};
	memset(udesin, 0x20, sizeof(udesin));
	
	NDK_ScrInitGui();
	
	//设置共享密钥表
	if((ret=NDK_SecSetKeyOwner("*"))!=NDK_OK)
	{
		NDK_ScrPrintf("line:%d设置密钥表失败%d",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	
	//验证使用ID2成功且结果正确
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 16/*8*/, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16/*SEC_DES_KEYLEN_8|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		NDK_ScrPrintf("line:%d子应用1调用ID2密钥加密失败%d",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	if(memcmp(udesout, "\x38\x77\x48\x93\xF5\x5F\xF4\xC9\x38\x77\x48\x93\xF5\x5F\xF4\xC9", 16/*8*/))
	{
		NDK_ScrPrintf("line:%d子应用1加密结果错误\n",__LINE__);
		NDK_ScrPrintf("0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x", udesout[0], udesout[1],udesout[2],udesout[3],udesout[4],udesout[5],udesout[6],udesout[7],udesout[8],udesout[9],udesout[10],udesout[11],udesout[12],udesout[13],udesout[14],udesout[15]);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	
	//case7:*表的安全限制被关闭,验证子应用可以用*表进行TPK等calcdes
	//TPK
	memset(udesout,0,sizeof(udesout));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 8, udesin, 16/*8*/, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16/*SEC_DES_KEYLEN_8*/))!=NDK_OK)
	{
		NDK_ScrPrintf("line:%d子应用1调用TPK密钥加密失败%d",__LINE__,ret);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	if(memcmp(udesout, "\x7B\x5D\x1D\x52\x9D\x34\xCA\x8E\x7B\x5D\x1D\x52\x9D\x34\xCA\x8E", 16/*8*/))
	{
		NDK_ScrPrintf("line:%d子应用1加密结果错误\n",__LINE__);
		NDK_ScrPrintf("0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x", udesout[0], udesout[1],udesout[2],udesout[3],udesout[4],udesout[5],udesout[6],udesout[7],udesout[8],udesout[9],udesout[10],udesout[11],udesout[12],udesout[13],udesout[14],udesout[15]);
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
	//验证B2应失败
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 16/*8*/, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16/*SEC_DES_KEYLEN_8|SEC_DES_ECBMODE*/))!=NDK_ERR_SECCR_GET_KEY)
	{
		NDK_ScrPrintf("line:%d B擦除密钥后验证失败\n",__LINE__);
		NDK_ScrRefresh();
		NDK_KbGetCode(0,NULL);
		return;
	}
	
	NDK_ScrPrintf("子应用1执行成功\n");
	NDK_ScrRefresh();
	NDK_KbGetCode(0,NULL);
	return;
}

