/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name		: rfc5.c
* Author 			: chenfm
* version			: 
* DATE			: 20130108
* directory 		: 
* description		: 测试NDK_RfidPiccState,NDK_RfidPiccDetect接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_RfidPiccDetect"
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
*			  		    chenfm	        20130108	 	created
*****************************************************************/
void rfc5(void)
{
	//以下为局部变量
	uchar psPicctype= 0x00, psDatabuf[256] ={ 0}; 
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

	//注意:case1并入case6验证即可
#if 0	//该接口设计的参数无用(但因代码已提交银商的原因,仍保留这个无用参数),故不关心NULL参数
	//case1:异常测试
	if((ret=NDK_RfidPiccDetect(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case1:"无卡"
	cls_show_msg("请确保感应区无RF卡,按任意键继续...");
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_ERR_RFID_NOCARD)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	/*
	A卡和B卡不一样，A卡在(上电)激活过程中会发现冲突。
	A卡理论上可以在寻卡阶段发现冲突，但一般天线能量和卡的特性一致性问题，都很难做到。
	实际的防冲突序列也是在上电过程中才进行。
	*/
#if 0	//放到上电激活中测试该错误码
	//case2:"A卡寻卡失败(可能多张卡存在)"
	//"A卡冲突(多张卡存在)"
	cls_show_msg("请在感应区放置多张标准A卡,任意键继续...");
	cls_show_msg("line %d:%d", __LINE__, NDK_RfidPiccDetect(&psPicctype));
	ret = NDK_RfidPiccDetect(&psPicctype);
	if(ret!=NDK_ERR_RFID_ASEEK /* && state!=0x03*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	//case3:"A、B卡同时存在"
	cls_show_msg("请在感应区放置标准A卡与B卡,按任意键继续...");
#if 1
	if((ret=NDK_RfidPiccDetect(&psPicctype))==NDK_OK)//若寻卡返回0x00,则在上电激活时一定要检测到多张卡
	{
		if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_ERR_RFID_ABON)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			NDK_RfidPiccDeactivate(0);//保护动作,万一上电成功为不影响后续测试 需下电卡片 20160726 linwl
			GOTOERR;
		}
	}
	else if(ret!=NDK_ERR_RFID_ABON)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
#else
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_ERR_RFID_ABON)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//放到icpowerup中测试该错误码0x0f,在getDS5002state中情况较为复杂:
	//对A而言,要在激活时才返回0x0f(BYTE0),而对B而言,在寻卡时就BYTE1返回0x0f
#if 0 
	//case4:"不支持ISO14443-4协议的卡"
	cls_show_msg("请在感应区放置MIfare-1卡,任意键继续...");//MIfare-1卡只有A卡一种类型
	//cls_show_msg("line %d:0x%8x", __LINE__, getDS5002state());
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_ERR_RFID_PROTOCOL)
	{
		send_result("line %d:%s测试失败(0x%02x)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case5:"有卡"(虽然从返回值上无法区别是寻到A还是B,但还是分别对两种卡寻一下,保证测试完备性)
	for(loop=0;loop<2;loop++)
	{
		cls_show_msg("请在感应区放置1张标准%s卡,按任意键继续...", (loop==0)?"A":"B");
		if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
			GOTOERR;
		}	
	}

	//case6:寻卡
	//通过方式可以尝试多种,如有屏蔽物.后续可考虑在综合测试中实现
	cls_show_msg("请确保感应区无RF卡,按任意键继续...请等待%ds", MAXWAITTIME);
	oldtime=time(NULL);
	while((diff=time(NULL)-oldtime)<MAXWAITTIME)
	{
		cls_printf("请确保感应区无RF卡,请等待%ds", MAXWAITTIME-diff);
		if((ret=NDK_RfidPiccDetect(NULL/*&psPicctype*/))!=NDK_ERR_RFID_NOCARD)
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
		while(time(NULL)-oldtime<MAXWAITTIME)//GetDS5002state不是阻塞的,需要应用不断查询
		{
			if((ret=NDK_RfidPiccDetect(&psPicctype))==NDK_OK)
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

#if 1
	//case7:"B卡寻卡失败(可能多张卡存在)"
	cls_show_msg("请在感应区放置多张标准B卡,按任意键继续...");
	if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_ERR_RFID_BSEEK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}	
#endif

	//其余值一般情况下不易出现,不测.另有部分值在激活用例中测
	//另外,resetDS对RF是否有影响未知,RF模块不考虑此问题

	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);
	return;
}

