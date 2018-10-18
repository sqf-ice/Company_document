/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name			: rfc15.c
* Author 			:  
* version			: 
* DATE				:  
* directory 			: 
* description		: 测试NDK_RfidPiccDetect_Atq能否正常返回ATQA和ATQB(即在寻卡的基础上新增两个返回值)
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_RfidPiccDetect_Atq"
#define MAXWAITTIME  10

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
*			  		   jiangym				20171110		created
*****************************************************************/
void rfc15(void)
{
	//以下为局部变量
	uchar psPicctype= 0x00, Atqbuf[24] = {0}, psDatabuf[256] ={ 0}; 
	int Atqlen = 0;
	int ret= -1, pnDatalen = 0, loop = 0;
	time_t oldtime = 0, diff = 0;
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_RfidPiccDeactivate(0);
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case1:参数异常
	if((ret=NDK_RfidPiccDetect_Atq(NULL,&Atqlen,Atqbuf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDetect_Atq(&psPicctype,NULL,Atqbuf))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_RfidPiccDetect_Atq(&psPicctype,&Atqlen,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:无卡片时应该返回NoCard
	cls_show_msg("请确保感应区无RF卡,按任意键继续...");
	if((ret=NDK_RfidPiccDetect_Atq(&psPicctype,&Atqlen,Atqbuf))!=NDK_ERR_RFID_NOCARD)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:"A、B卡同时存在"
	cls_show_msg("请在感应区放置标准A卡与B卡,按任意键继续...");
	if((ret=NDK_RfidPiccDetect_Atq(&psPicctype,&Atqlen,Atqbuf))==NDK_OK)//若寻卡返回0x00,则在上电激活时一定要检测到多张卡
	{
		if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_ERR_RFID_ABON)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			NDK_RfidPiccDeactivate(0);//保护动作,万一上电成功为不影响后续测试 需下电卡片
			GOTOERR;
		}
	}
	else if(ret!=NDK_ERR_RFID_ABON)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case5:有卡,A卡返回2字节,B卡返回12字节
	cls_show_msg("请在感应区放置1张标准A卡,按任意键继续...");
	memset(Atqbuf,0,sizeof(Atqbuf));
	if((ret=NDK_RfidPiccDetect_Atq(&psPicctype,&Atqlen,Atqbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		GOTOERR;
	}	
	//输出内容
	if(Atqlen != 2)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, Atqlen);
		GOTOERR;
	}
	if(cls_show_msg("返回的ATQ值为:0x%02x:0x%02x,请确认是否正确,正确按[Enter],不正确按[其他]",Atqbuf[0],Atqbuf[1])!=ENTER)
	{
		send_result("line %d:%s测试失败(0x%02x:0x%02x)", __LINE__, TESTAPI, Atqbuf[0],Atqbuf[1]);
		GOTOERR;
	}
	
	cls_show_msg("请在感应区放置1张标准B卡,按任意键继续...");
	memset(Atqbuf,0,sizeof(Atqbuf));
	if((ret=NDK_RfidPiccDetect_Atq(&psPicctype,&Atqlen,Atqbuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		GOTOERR;
	}	
	//输出内容
	if(Atqlen != 12)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, Atqlen);
		GOTOERR;
	}
	if(cls_show_msg("返回的ATQ值为:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x,请确认是否正确,正确按[Enter],不正确按[其他]",Atqbuf[0],Atqbuf[1],Atqbuf[2],Atqbuf[3],Atqbuf[4],Atqbuf[5],Atqbuf[6],Atqbuf[7],Atqbuf[8],Atqbuf[9],Atqbuf[10],Atqbuf[11])!=ENTER)
	{
		send_result("line %d:%s测试失败(%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x)", __LINE__, TESTAPI, Atqbuf[0],Atqbuf[1],Atqbuf[2],Atqbuf[3],Atqbuf[4],Atqbuf[5],Atqbuf[6],Atqbuf[7],Atqbuf[8],Atqbuf[9],Atqbuf[10],Atqbuf[11]);
		GOTOERR;
	}

	//case6:寻卡
	//通过方式可以尝试多种,如有屏蔽物.后续可考虑在综合测试中实现
	cls_show_msg("请确保感应区无RF卡,按任意键继续...请等待%ds", MAXWAITTIME);
	oldtime=time(NULL);
	while((diff=time(NULL)-oldtime)<MAXWAITTIME)
	{
		cls_printf("请确保感应区无RF卡,请等待%ds", MAXWAITTIME-diff);
		memset(Atqbuf,0,sizeof(Atqbuf));
		if((ret=NDK_RfidPiccDetect_Atq(&psPicctype,&Atqlen,Atqbuf))!=NDK_ERR_RFID_NOCARD)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
	for(loop=0;loop<2;loop++)
	{
		cls_show_msg("请在%ds内让1张标准%s卡从无感区通过感应区,按按任意键计时开始!", MAXWAITTIME, (loop==0)?"A":"B");
		cls_printf("寻卡...");
		oldtime=time(NULL);
		ret=0xff;
		memset(Atqbuf,0,sizeof(Atqbuf));
		while(time(NULL)-oldtime<MAXWAITTIME)//GetDS5002state不是阻塞的,需要应用不断查询
		{
			if((ret=NDK_RfidPiccDetect_Atq(&psPicctype,&Atqlen,Atqbuf))==NDK_OK)
				break;
		}
		if(ret==NDK_OK)
		{
			NDK_SysBeep();
			show_msg1(1, "成功!");
		}
		else
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}

	//case7:"B卡寻卡失败(可能多张卡存在)"
	cls_show_msg("请在感应区放置多张标准B卡,按任意键继续...");
	if((ret=NDK_RfidPiccDetect_Atq(&psPicctype,&Atqlen,Atqbuf))!=NDK_ERR_RFID_BSEEK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}	

	//其余值一般情况下不易出现,不测.另有部分值在激活用例中测
	//另外,resetDS对RF是否有影响未知,RF模块不考虑此问题

	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);
	return;
}

