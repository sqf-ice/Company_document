/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name			: rfc21.c
* Author 			:  
* version			: 
* DATE				:  
* directory 			: 
* description		: 测试NDK_M0Write和NDK_M0Read读写接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_M0Write,NDK_M0Read"

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
*			  		     jiangym			    20180110		created 
*****************************************************************/
void rfc21(void)
{
	//以下为局部变量
	int ret = -1, rec_len = 0, i = 0;
	uchar UIDLen = 0, sak;
	uchar UID[10] = {0};
	uchar rec_data[30] = {0}, wr_data[32] = {0};

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_RfidPiccDeactivate(0);
	//case1:未进行寻卡防冲突就进行读写应该失败
	if((ret=NDK_M0Write(8,4,wr_data))!=NDK_ERR_MI_WriteFifo)//-2041 写错误
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Read(8,&rec_len,rec_data))!=NDK_ERR_MI_WriteFifo)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//寻卡防冲突
	cls_show_msg("请在感应区放置1张不带C的M0卡,任意键继续...");
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,&sak))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case2:异常参数测试	     
	if((ret=NDK_M0Write(8,4,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Write(8,5,wr_data))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Write(8,17,wr_data))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Read(8,NULL,rec_data))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Read(8,&rec_len,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:正常测试:(初始卡片是不需要认证的)不认证进行读写
	for (i=0; i<sizeof(wr_data); i++)
		wr_data[i]=rand()%256;
	//需要使用未改过初始认证密钥的卡
	if((ret=NDK_M0Write(8,4,wr_data))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Write(9,4,wr_data+4))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Write(10,16,wr_data+8))!=NDK_OK)	//实际只写了4字节 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Write(11,16,wr_data+12))!=NDK_OK)  //实际只写了4字节 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Read(8,&rec_len,rec_data))!=NDK_OK)//读8-11数据
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(rec_data,wr_data,16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:认证后修改AUTH0即地址42的值,改为20H,修改后卡片20H后的地址进行操作需要认证才有权限,测试完后再恢复原样
	cls_show_msg("请在感应区放置1张带C的M0卡,任意键继续...");
	//换卡了需要重新上电
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,&sak))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Authen(AUTHKEY_M0))!=NDK_OK) //-2041
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Write(42,4,(uchar*)"\x20\x20\x20\x20"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//修改AUTH0不能立即生效,下电后再重新寻卡
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,&sak))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//未认证对20H后的读写操作会失败,比如对25H(即37地址)应该失败
	if((ret=NDK_M0Write(37,4,wr_data))!=NDK_ERR_MI_NOTAUTHERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//失败后需要重新寻卡
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,&sak))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//进行认证,认证后读写应该成功21H--24H(33-36)
	if((ret=NDK_M0Authen(AUTHKEY_M0))!=NDK_OK) 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	for(i=33;i<37;i++)
	{
		if((ret=NDK_M0Write(i,4,wr_data+(i-33)*4))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
	}	
	if((ret=NDK_M0Read(33,&rec_len,rec_data))!=NDK_OK)//读33-36数据
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(rec_data,wr_data,16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	//恢复原来的不认证
	if((ret=NDK_M0Write(42,4,(uchar*)"\x30\x30\x30\x30"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:验证读最大可操作的页支持43页,如果地址传43开始会从头开始循环,如果从44开始读会失败
	if((ret=NDK_MifareActive(REQA, UID, &UIDLen,&sak))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_M0Read(43,&rec_len,rec_data))!=NDK_OK)//读43-0 1 2数据
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//读44-47数据预期失败
	if((ret=NDK_M0Read(44,&rec_len,rec_data))!=NDK_ERR_MI_CRCERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);
	return;
}

