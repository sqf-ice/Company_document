/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: socket模块
* file name			: sckt10.c
* Author 			: 
* version			: 
* DATE				:
* directory 	: 
* description	: 测试NDK_TcpSetKeepAlive socket连接是否能够保活功能
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
#define 	TESTAPI		"NDK_TcpSetKeepAlive"

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
* history 		 				:  author		date		remarks
*			  	  	  			jiangym		20160726 created
*****************************************************************/
void sckt10(void)
{
	//以下为局部变量
	int ret = -1;
	uint h_tcp = 0; 

	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前所在的测试用例
	
	//case1:异常参数测试	
	if((ret=NDK_TcpSetKeepAlive(-1,10,65,9)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TcpSetKeepAlive(2,10,65,9)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TcpSetKeepAlive(1,-1,65,9)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TcpSetKeepAlive(1,36001,65,9)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TcpSetKeepAlive(0,10,-1,9)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TcpSetKeepAlive(0,10,751,9)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TcpSetKeepAlive(1,10,65,-1)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_TcpSetKeepAlive(1,10,65,51)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常使用测试:在网络未建立前设置开启, 服务器抓包工具能够抓到
	if((ret=NDK_TcpSetKeepAlive(1,5,65,9)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(NetUp()!=SUCC)
	{
		send_result("line %d:网络层UP失败", __LINE__);
		GOTOERR;
	}
	if(TransUp(&h_tcp)!=SUCC)
	{
		send_result("line %d:传输层UP失败", __LINE__);
		GOTOERR;
	}
	cls_show_msg("使用抓包工具进行查看服务器端是否有在每间隔5s时间内收到握手包,预期有");
	if(cls_show_msg("预期每间隔5s收到握手包,正确[Enter],错误[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	//case3:在传输层建立的情况下去修改设置为关闭的功能 抓包发现并没有生效
	if((ret=NDK_TcpSetKeepAlive(0,10,0,0)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("使用抓包工具进行查看服务器端是否有在每间隔5s时间内收到握手包,预期有");
	if(cls_show_msg("预期每间隔5s收到握手包,正确[Enter],错误[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	if(TransDown(h_tcp)!=SUCC)
	{
		send_result("line %d:传输层Down失败", __LINE__);
		GOTOERR;
	}
	
	//case4:在传输层挂断的情况下去修改设置为关闭的功能 抓包应该能发现抓不到包
	if((ret=NDK_TcpSetKeepAlive(0,10,0,0)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(TransUp(&h_tcp)!=SUCC)
	{
		send_result("line %d:传输层UP失败", __LINE__);
		GOTOERR;
	}
	cls_show_msg("使用抓包工具进行查看服务器端是否有在每间隔10s时间内收到握手包,预期没有");
	if(cls_show_msg("预期没有收到握手包,正确[Enter],错误[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}	
	if(TransDown(h_tcp)!=SUCC)
	{
		send_result("line %d:传输层Down失败", __LINE__);
		GOTOERR;
	}
	
	//case5:在传输层挂断的情况下设置开启且 参数全部传0 会使用默认值 间隔10s
	if((ret=NDK_TcpSetKeepAlive(1,0,0,0)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(TransUp(&h_tcp)!=SUCC)
	{
		send_result("line %d:传输层UP失败", __LINE__);
		GOTOERR;
	}
	cls_show_msg("使用抓包工具进行查看服务器端是否有在间隔10s时间内收到握手包,预期有");
	if(cls_show_msg("预期每间隔10s收到握手包,正确[Enter],错误[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}

	//case6:验证重发时间及重发次数,在重发过程中需要使用clumsy工具丢弃服务器发出的所有数据包
	if(TransDown(h_tcp)!=SUCC)
	{
		send_result("line %d:传输层Down失败", __LINE__);
		GOTOERR;
	}	
	if((ret=NDK_TcpSetKeepAlive(1,5,2,9)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(TransUp(&h_tcp)!=SUCC)
	{
		send_result("line %d:传输层UP失败", __LINE__);
		GOTOERR;
	}
	cls_show_msg("使用clumsy工具丢弃服务器发出的所有数据包后查看抓包工具,任意键继续");
	if(cls_show_msg("抓包工具是否有收到pos重发数据(2s一次,重发9次),正确[Enter],错误[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	TransDown(h_tcp);
	NDK_TcpSetKeepAlive(0,0,0,0);
	NetDown();
	return;
}

