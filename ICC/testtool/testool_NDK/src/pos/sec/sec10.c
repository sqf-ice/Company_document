/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec3.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecSetIntervaltime接口功能
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
#define TESTAPI	"NDK_SecSetIntervaltime"	

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
void sec10(void)
{
	int ret = 0, i = 0, j =0;
	uchar *szPan=(uchar *)"6225885916163157";
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar szDataIn[9]={0},szMac[9]={0},szPinOut[8+1]={0};
	
	cls_show_msg1(2, "测试%s...", TESTAPI); 

#if SEC_SM4_ENABLE
	int algnum = 2;//算法数量
	EM_SEC_KEY_ALG key_flag[2] = {SEC_KEY_DES , SEC_KEY_SM4};
	uchar macmode = SEC_MAC_SM4;
	uchar pinmode = SEC_PIN_SM4_1;
#else
	int algnum = 1;
	int key_flag[2] = {0,0};
	uchar macmode = SEC_MAC_X99;
	uchar pinmode = SEC_PIN_ISO9564_0;
#endif

	for(j = 0;j<algnum;j++)
	{
		//测试前置
		if((ret=NDK_SecKeyErase())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return;
		}

		//安装TAK, ID=1
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=1;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|key_flag[i];
		stKeyInfo.nDstKeyLen=16;
		memset(stKeyInfo.sDstKeyValue, 0x11, stKeyInfo.nDstKeyLen);
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}

		//安装TPK, ID=2
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=2;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK|key_flag[i];
		stKeyInfo.nDstKeyLen=16;
		memset(stKeyInfo.sDstKeyValue, 0x13, stKeyInfo.nDstKeyLen);
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		
		//测试两次计算MAC之间最小间隔时间
		if(auto_flag==0 || auto_flag==1)
		{
			//case1:unTAKIntervalTimeMs=60000,设置60秒可执行1次,240秒内只能执行4次,所以循环5次调用NDK_SecGetMac,前四次成功,第五次失败
			memset(szDataIn, 0x11, 8);
			if((ret=NDK_SecSetIntervaltime(10, 60000))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			for(i=0; i<4; i++)
			{
				if((ret=NDK_SecGetMac(1, szDataIn, 8, szMac, macmode))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
					GOTOERR;
				}
			}

			if((ret=NDK_SecGetMac(1, szDataIn, 8, szMac, macmode)) != NDK_ERR_SECP_OVERRUN)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			
			//case2:unTAKIntervalTimeMs=1,MAC计算间隔设置成1ms,所以循环10次都应成功
			if((ret=NDK_SecSetIntervaltime(10, 1))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			for(i=0; i<10; i++)
			{
				if((ret=NDK_SecGetMac(1, szDataIn, 8, szMac, macmode))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
					GOTOERR;
				}
			}

			//case3:unTAKIntervalTimeMs=0xFFFFFFFF,表示不改变原值,所以循环10次都应成功
			if((ret=NDK_SecSetIntervaltime(10, 0xFFFFFFFF))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			for(i=0; i<10; i++)
			{
				if((ret=NDK_SecGetMac(1, szDataIn, 8, szMac, macmode))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
					GOTOERR;
				}
			}
		}

		//测试两次计算PINBlock之间最小间隔时间
		if(auto_flag==0 || auto_flag==2)
		{
			//case4:PINBLOCK间隔时间的计算方式： 默认为120秒那只能调用4次,即TPKIntervalTimeMs默认值为30秒,调用该函数重新设置后,限制为4*TPKIntervalTimeMs时间内只能调用4次 
			//unTPKIntervalTimeMs=0表示使用默认值,所以循环5次调用NDK_SecGetPin,每次都直接回车,前四次成功,第五次失败
			cls_printf("请连续按[确认]...");
			if((ret=NDK_SecSetIntervaltime(0, 100))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}

			for(i=0; i<4; i++)
			{
				if((ret=NDK_SecGetPin(2, (uchar *)"0", szPan, szPinOut, pinmode, 2*1000))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret,i);
					GOTOERR;
				}
			}
			if((ret=NDK_SecGetPin(2, (uchar *)"0", szPan, szPinOut, pinmode, 2*1000))!= NDK_ERR_SECP_OVERRUN)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				goto ERR;
			}
		}
		else
			send_result("line %d:%s部分子用例需手动测试", __LINE__, TESTAPI);
		
	}
	
	send_result("%s测试通过", TESTAPI);
ERR://测试后置:擦除所有密钥
	NDK_SecSetIntervaltime(0,0);
	NDK_SecKeyErase();
	return;
}

