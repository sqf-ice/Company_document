/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: RF卡模块
* file name		: 
* Author 			: zhangnw
* version			: V1.0
* DATE			: 20081107
* directory 		: .\SRC\RFcard\
* description		: 测试iccrw_new命令交互的功能(APDU读写)
* related document	: 程序员参考手册
*
************************************************************************
* log			: 
* Revision 1.0  20081107 zhangnw
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  UCID  4
#define	TESTAPI		"iccrw_new"
#define	GetDS5002state()	((getDS5002state()&0x0000ff00)>>8)	//RF使用byte1

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
*			  		    zhangnw	        20091209	 	created
*****************************************************************/
void rfcard4(void)
{
	/*private & local definition*/
	int state = 0, rcvlen = 0;
	char atrbuf[300] = {0};
	char Select_1pay[] = 
#if 1	//海信的卡不支持Select_1pay命令,支持取随机数命令
	{'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
#else
	uchar Select_1pay[] = 
	{0x00, 0xA4, 0x04, 0x00, 0x0E,
	  '1',  'P',  'A',  'Y',  '.',  'S',  'Y',  'S',  '.',  'D',  'D',  'F',  '0',  '1',
	  0x00
	};//查询CPU卡的文件目录结构,mifare-1卡不支持
#endif

	/*process body*/
	cls_printf("测试%s...", TESTAPI);

	//测试前置
	icpowerdown(RFCARD);//保护
	if(SetICType(0xff, RFCARD_TYPE_AB)!=SUCC)
	{
		send_result("line %d:SetICType(RFCARD_TYPE_AB)失败", __LINE__);
		GOTOERR;
	}
	
	//case1:"-7 未上电(寻卡激活)"
	cls_show_msg("请在感应区放置1张标准A卡,任意键继续...");
	if((state=iccrw_new(RFCARD, sizeof(Select_1pay), Select_1pay, &rcvlen, atrbuf))!=-7)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, state);
		GOTOERR;
	}
	cls_show_msg1(2, "子用例测试通过!");//用以区别子case间相同的操作提示信息

	//case2:"-1 表示读写失败"
	cls_show_msg("请在感应区放置1张标准A卡,任意键继续...");
	if((state=GetDS5002state())!=0x00)
	{
		send_result("line %d:寻卡失败(0x%02x)", __LINE__, state);
		GOTOERR;
	}	
	if((state=icpowerup(RFCARD, atrbuf))!=0x00||atrbuf[0]!=RFCARD_TYPE_A)
	{
		send_result("line %d:激活卡失败(0x%02x, 0x%02x)", __LINE__, state, atrbuf[0]);
		GOTOERR;
	}	
	cls_show_msg("寻卡成功!请将感应区的标准A卡移开,任意键继续...");
	if((state=iccrw_new(RFCARD, sizeof(Select_1pay), Select_1pay, &rcvlen, atrbuf))!=-1)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, state);
		GOTOERR;	
	}
	//虽然iccrw_new失败,但已激活成功(底层处于激活态),故再寻卡会提示"已经激活(上电)"
	//设计变更了!(按EMV安全要求执行),故上一行文字无法测试到
	if((state=GetDS5002state())!=0x0d)//因为底层的设计为APDU交互出错,会下电,回到初始态
	{
		send_result("line %d:寻卡应失败(0x%02x)", __LINE__, state);
		GOTOERR;
	}
#if 0 //(根据EMV要求),目前这个动作已由底层完成!
	//"必须下电（清除已激活状态标志），然后再寻卡、激活。"
	if((state=icpowerdown(RFCARD))!=SUCC)
	{
		send_result("line %d:icpowerdown失败(%d)", __LINE__, state);
		GOTOERR;
	}
#endif

	//case3:A卡的读写
	cls_show_msg("请在感应区放置1张标准A卡,任意键继续...");
	if((state=GetDS5002state())!=0x00)
	{
		send_result("line %d:寻卡失败(0x%02x)", __LINE__, state);
		GOTOERR;
	}	
	if((state=icpowerup(RFCARD, atrbuf))!=0x00||atrbuf[0]!=RFCARD_TYPE_A)
	{
		send_result("line %d:激活卡失败(0x%02x, 0x%02x)", __LINE__, state, atrbuf[0]);
		GOTOERR;
	}	
	if((state=iccrw_new(RFCARD, sizeof(Select_1pay), Select_1pay, &rcvlen, atrbuf))!=SUCC
		||atrbuf[rcvlen-2]!=0x90||atrbuf[rcvlen-1]!=0x00)
	{
		send_result("line %d:%s测试失败(%d, 0x%02x, 0x%02x)", __LINE__, TESTAPI, state, atrbuf[rcvlen-2], atrbuf[rcvlen-1]);
		GOTOERR;
	}
	//下面的用例需要换B卡测试，应先下电关闭场
	if((state=icpowerdown(RFCARD))!=SUCC)
	{
		send_result("line %d:icpowerdown失败(%d)", __LINE__, state);
		GOTOERR;
	}	

	//case4:标准B卡的读写
	cls_show_msg("请在感应区放置1张标准B卡,任意键继续...");
	if((state=GetDS5002state())!=0x00)
	{
		send_result("line %d:寻卡失败(0x%02x)", __LINE__, state);
		GOTOERR;
	}
	if((state=icpowerup(RFCARD, atrbuf))!=0x00||atrbuf[0]!=RFCARD_TYPE_B)
	{
		send_result("line %d:激活卡失败(0x%02x, 0x%02x)", __LINE__, state, atrbuf[0]);
		GOTOERR;
	}
	if((state=iccrw_new(RFCARD, sizeof(Select_1pay), Select_1pay, &rcvlen, atrbuf))!=SUCC
		||atrbuf[rcvlen-2]!=0x90||atrbuf[rcvlen-1]!=0x00)
	{
		send_result("line %d:%s测试失败(%d, 0x%02x, 0x%02x)", __LINE__, TESTAPI, state, atrbuf[rcvlen-2], atrbuf[rcvlen-1]);
		GOTOERR;
	}
	//下面的用例需要换卡测试，应先下电关闭场
	if((state=icpowerdown(RFCARD))!=SUCC)
	{
		send_result("line %d:icpowerdown失败(%d)", __LINE__, state);
		GOTOERR;
	}	

#if defined GP730	//||defined NL8510GP||defined GP730CP
	//case5:海信B卡的读写
	/*
	要用已有的统一API读写海信的卡，需要在初始时(GetDS5002state前)调用一个函数
	setpiccquickseek(1);即简易的快速寻卡
	预留这个接口主要有2个目的：
	1、针对有问题的非标准卡
	2、某些非EMV测试，强调寻卡速度等的测试
	当然,对标准的B卡若是调用了这个接口,也不会有问题就是了.但海信的卡不是标准的,不以它的行为为标准的,测试中一般不建议使用到此接口
	*/
	if(cls_show_msg("请在感应区放置1张海信B卡\n[ESC]略过,[其它]继续")!=ESC)
	{
		if((state=GetDS5002state())!=0x0b)//0x0b表示寻(选)卡失败//==0x00)
		{
			send_result("line %d:寻卡应失败(0x%02x)", __LINE__, state);
			GOTOERR;
		}
		if((state=setpiccquickseek(1))!=SUCC)
		{
			send_result("line %d:简易快速寻卡失败(0x%02x)", __LINE__, state);
			GOTOERR;
		}
		if((state=GetDS5002state())!=0x00)
		{
			send_result("line %d:寻卡失败(0x%02x)", __LINE__, state);
			GOTOERR;
		}
		if((state=icpowerup(RFCARD, atrbuf))!=0x00||atrbuf[0]!=RFCARD_TYPE_B)
		{
			send_result("line %d:激活卡失败(0x%02x, 0x%02x)", __LINE__, state, atrbuf[0]);
			GOTOERR;
		}	
		if((state=iccrw_new(RFCARD, sizeof(Select_1pay), Select_1pay, &rcvlen, atrbuf))!=SUCC
			||atrbuf[rcvlen-2]!=0x90||atrbuf[rcvlen-1]!=0x00)
		{
			send_result("line %d:%s测试失败(%d, 0x%02x, 0x%02x)", __LINE__, TESTAPI, state, atrbuf[rcvlen-2], atrbuf[rcvlen-1]);
			GOTOERR;
		}
	}
#endif

	//test over
	send_result("%s测试通过", TESTAPI);
	goto ERR;
ERR:
#if defined GP730	//||defined NL8510GP||defined GP730CP
	setpiccquickseek(0);//关闭简易快速寻卡功能,机器下电后也会恢复默认设置0.此处是为了防止使用海信B卡代替标准B卡时测试会通过
#endif
	icpowerdown(RFCARD);
	return;
}

