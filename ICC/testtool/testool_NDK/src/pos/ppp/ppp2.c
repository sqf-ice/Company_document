/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: ppp模块
* file name		: ppp2.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PppAddrChange接口功能
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
#define TESTAPI	"NDK_PppAddrChange"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void ppp2(void)
* functional description 	: 将一个点分十进制的IP转换成一个长整数型数
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void ppp2(void)
{
	//以下为局部变量
	Value addr;
	ulong ulIpAddr=0;
	int nRet=0;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);
	
	//case1:正确值192.168.2.15, 但不论何种字节序,其值应都为192<<24+168<<16+2<<8+15
	memset(&addr, 0, sizeof(addr));
	
	if(LITTLEENDIAN==GetCurrentEndianType())
	{addr.asc[0] = 15;addr.asc[1] = 2;addr.asc[2] = 168;addr.asc[3] = 192;}
	else
	{addr.asc[0] = 192;addr.asc[1] = 168;addr.asc[2] = 2;addr.asc[3] = 15;}
	
	if((nRet = NDK_PppAddrChange("192.168.2.15",&ulIpAddr))!=NDK_OK )
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		return;
	}
	if(addr.data != ulIpAddr)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}
	
	//case2:正确值192.168.2.1
	memset(&addr, 0, sizeof(addr));
	
	if(LITTLEENDIAN==GetCurrentEndianType())
	{addr.asc[0] = 1;addr.asc[1] = 2;addr.asc[2] = 168;addr.asc[3] = 192;}
	else
	{addr.asc[0] = 192;addr.asc[1] = 168;addr.asc[2] = 2;addr.asc[3] = 1;}
	if((nRet = NDK_PppAddrChange("192.168.2.1",&ulIpAddr))!=NDK_OK )
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		return;
	}
	if(addr.data != ulIpAddr)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}
	
	//case3:正确值192.168.2.254
	memset(&addr, 0, sizeof(addr));

	if(LITTLEENDIAN==GetCurrentEndianType())
	{addr.asc[0] = 254;addr.asc[1] = 2;addr.asc[2] = 168;addr.asc[3] = 192;}
	else
	{addr.asc[0] = 192;addr.asc[1] = 168;addr.asc[2] = 2;addr.asc[3] = 254;}
	if((nRet = NDK_PppAddrChange("192.168.2.254",&ulIpAddr))!=NDK_OK )
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		return;
	}
	if(addr.data != ulIpAddr)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return;
	}
	
	//case4:异常测试
	//memset(&addr, 0, sizeof(addr));
	//addr.data = INADDR_NONE; //如果传入的字符串不是一个合法的IP地址，将返回INADDR_NONE
	if((nRet = NDK_PppAddrChange("192.168.2.a",&ulIpAddr)) != NDK_ERR )
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		return;
	}
	
	//case5:空格
	if((nRet = NDK_PppAddrChange("192.168.2. 1",&ulIpAddr)) != NDK_ERR )
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		return;
	}
	
	//case6:非法IP
	if((nRet = NDK_PppAddrChange("",&ulIpAddr)) != NDK_ERR )
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		return;
	}
	
	//case7:不完整IP:192.168.2.	
	if((nRet = NDK_PppAddrChange("192.168.2.",&ulIpAddr)) != NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		return;
	}

	//case8:NULL
	if((nRet = NDK_PppAddrChange(NULL,&ulIpAddr)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		return;
	}
	if((nRet = NDK_PppAddrChange("192.168.2.1",NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
		return;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

