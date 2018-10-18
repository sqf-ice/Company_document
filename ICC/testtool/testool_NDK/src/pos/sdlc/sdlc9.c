/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sdlc模块
* file name		: sdlc9.c
* Author 			: chenjings
* version			: 
* DATE			: 20130108
* directory 		: src/pos/sdlc
* description		: 测试NDK_MdmExCommand 能否正确执行AT命令
* related document	: SDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_MdmExCommand"
#define MAXLEN_PERATCOMMAND_SYS	52

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	chenjings      20130108	created
*****************************************************************/
void sdlc9(void)
{
	//以下为局部变量
	int ret;
	uchar pucCmdstr[128]={0},pszRespData[128]={0};
	uint rLen,unTimeout=30;
	
	//以下为函数体
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前所在的测试用

	//case1:测试输入非法参数情况
	sprintf((char*)pucCmdstr,"AT\r");
	if((ret=NDK_MdmExCommand(NULL,pszRespData,&rLen,unTimeout))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_MdmExCommand(pucCmdstr,NULL,&rLen,unTimeout))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_MdmExCommand(pucCmdstr,pszRespData,NULL,unTimeout))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_MdmExCommand(pucCmdstr,pszRespData,&rLen,-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:测试AT命令格式不对的情况
	sprintf((char*)pucCmdstr,"bT\r");
	if((ret=NDK_MdmExCommand(pucCmdstr,pszRespData,&rLen,unTimeout))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sprintf((char*)pucCmdstr,"AT\n");
	if((ret=NDK_MdmExCommand(pucCmdstr,pszRespData,&rLen,unTimeout))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3: 测试AT命令过长情况
	memset(pucCmdstr,'\r',MAXLEN_PERATCOMMAND_SYS+1);
	pucCmdstr[0]='A';
	pucCmdstr[1]='T';
	pucCmdstr[MAXLEN_PERATCOMMAND_SYS+2]=0;
	if((ret=NDK_MdmExCommand(pucCmdstr,pszRespData,&rLen,unTimeout))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case4:测试正常情况
	sprintf((char*)pucCmdstr,"ATE1\r");
	rLen=-1;
	if((ret=NDK_MdmExCommand(pucCmdstr,pszRespData,&rLen,unTimeout))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((strstr((char*)pszRespData,"OK")==NULL)||(rLen==-1))
	{
		send_result("line %d:%s测试失败(rData=%s,rlen=%d)", __LINE__, TESTAPI, pszRespData, rLen);
		RETURN;
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

