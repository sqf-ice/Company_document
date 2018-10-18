/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec6.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecGetDukptKsn,NDK_SecIncreaseDukptKsn接口功能
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
#define TESTAPI	"NDK_SecGetDukptKsn,NDK_SecIncreaseDukptKsn"	

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
*****************************************************************/
void sec18(void)
{
	int ret = 0, i = 0;
	uchar KsnIn[17]={0},KeyValueIn[17]={0},KsnOut[11]={0};
	ST_SEC_KCV_INFO KcvInfoIn;
#if !(defined ME11||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B)//无键盘设备不支持密钥输入函数
	uchar PinBlockOut[9]={0};
#endif
		
	cls_show_msg1(2, "测试%s...", TESTAPI); 

	//测试前置:擦除所有密钥
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//明文安装一组DUKPT, ID=1, psKsnIn为10个字节0xff  /*约定:以下KCV均以不校验方式安装*/
	memset(KsnIn, 0xff, 10);
	memset(KeyValueIn, 0x11, 16);
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadTIK(1, 0, 16, KeyValueIn, KsnIn, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
if(auto_flag==0 || auto_flag==1)
{
	//case1:非法测试
	//case1.1: NDK_SecGetDukptKsn: ucGroupIdx=2,101
	if ((ret=NDK_SecGetDukptKsn(2, KsnOut))!=NDK_ERR_SECKM_READ_REC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0 //101合法
	if ((ret=NDK_SecGetDukptKsn(101, KsnOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case1.2:NDK_SecGetDukptKsn:psKsnOut =NULL
	if ((ret=NDK_SecGetDukptKsn(1, NULL))!= NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1.3: NDK_SecIncreaseDukptKsn: ucGroupIdx=2,101
	if ((ret=NDK_SecIncreaseDukptKsn(2))!=NDK_ERR_SECKM_READ_REC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0 //101合法
	if ((ret=NDK_SecIncreaseDukptKsn(101))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	//case2:NDK_SecGetDukptKsn psKsnOut后21个bit应为1(KSN值从1开始往上增)
	if ((ret=NDK_SecGetDukptKsn(1, KsnOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(((KsnOut[7]&0x1f)!=0)||(KsnOut[8]!=0)||(KsnOut[9]!=0x01))
	{
		send_result("line %d:%s测试失败(0x%02x,0x%02x,0x%02x)", __LINE__, TESTAPI, KsnOut[7], KsnOut[8], KsnOut[9]);
		GOTOERR;
	}

	//case3:调用NDK_SecIncreaseDukptKsn两次,再NDK_SecGetDukptKsn, psKsnOut后21个bit值应为3
	memset(KsnOut, 0x00, sizeof(KsnOut));
	for(i=0; i<2; i++)
	{
		if ((ret=NDK_SecIncreaseDukptKsn(1))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
	}
	if ((ret=NDK_SecGetDukptKsn(1, KsnOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(((KsnOut[7]&0x1f)!=0)||(KsnOut[8]!=0)||(KsnOut[9]!=0x03))
	{
		send_result("line %d:%s测试失败(0x%02x,0x%02x,0x%02x)", __LINE__, TESTAPI, KsnOut[7], KsnOut[8], KsnOut[9]);
		goto ERR;
	}
}

#if !(defined ME11||defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B)//无键盘设备不支持密钥输入函数
if(auto_flag==0 || auto_flag==2)
{
	//case4:验证执行DUKPT的密钥操作,	KSN应不变(KSN值应为2). 以下以GETPIN为例
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if ANDIROD_ENABLE
	NDK_ScrPrintf("尽快输入1234并确认...");
#else
	cls_printf("尽快输入1234并确认...");
#endif
	if((ret=NDK_SecGetPinDukpt(1,  (uchar *)"4,5,6",  (uchar *)"6225885916163157",  KsnOut,  PinBlockOut,  SEC_PIN_ISO9564_0, 120*1000))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(0x%x)", __LINE__, TESTAPI, KsnOut);
		GOTOERR;
	}
	memset(KsnOut, 0x00, sizeof(KsnOut));
	if ((ret=NDK_SecGetDukptKsn(1, KsnOut))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if(((KsnOut[7]&0x1f)!=0)||(KsnOut[8]!=0)||(KsnOut[9]!=0x03))
	{
		send_result("line %d:%s测试失败(0x%02x,0x%02x,0x%02x)", __LINE__, TESTAPI, KsnOut[7], KsnOut[8], KsnOut[9]);
		goto ERR;
	}
}
else
	send_result("line %d:%s部分子用例需手动测试", __LINE__, TESTAPI);
#endif

	send_result("%s测试通过", TESTAPI);
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}

