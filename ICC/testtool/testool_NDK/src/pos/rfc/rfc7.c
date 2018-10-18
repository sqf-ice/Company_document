/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name		: rfc7.c
* Author 			: chenfm
* version			: 
* DATE			: 20130108
* directory 		: 
* description		: 测试NDK_PiccQuickRequest接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_PiccQuickRequest"

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
void rfc7(void)
{
	//以下为局部变量
	uchar psPicctype;
	uchar psRecebuf[300]={0}, psDatabuf[256]={0};
	int ret= -1, pnDatalen=0, rcvlen=0, i=0, j=0;
	ushort sw=0;
#if 1	//海信的卡不支持Select_1pay命令,支持取随机数命令
	uchar Select_1pay[]= {'\x00', '\x84', '\x00', '\x00', 8};
#else
	uchar Select_1pay[]={0x00, 0xA4, 0x04, 0x00, 0x0E, '1', 'P', 'A', 'Y', '.', 'S', 'Y', 'S', '.', 'D', 'D', 'F', '0', '1', 0x00};
#endif		

	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_RfidPiccDeactivate(0);
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//原先for海信B卡的读写
	/*
	要用已有的统一API读写海信的卡，需要在初始时(GetDS5002state前)调用一个函数
	setpiccquickseek(1);即简易的快速寻卡
	预留这个接口主要有2个目的：
	1、针对有问题的非标准卡
	2、某些非EMV测试，强调寻卡速度等的测试
	当然,对标准的B卡若是调用了这个接口,也不会有问题就是了.但海信的卡不是标准的,不以它的行为为标准的,测试中一般不建议使用到此接口
	*/	

	//case1:测试快速寻卡模式
	//case2:测试普通寻卡模式
	for(i=0;i<2;i++)
	{
		//设置寻卡模式（快速寻卡，普通寻卡）
		if((ret=NDK_PiccQuickRequest(i==0?0:rand()%MAX_SIGNINT+1))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		for(j=0;j<2;j++)
		{
			//寻A或B卡
			cls_show_msg("请在感应区放置标准%s卡，按任意键继续...", j==0?"A":"B");
			if((ret=NDK_RfidPiccDetect(&psPicctype))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
			if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=(j==0?RFCARD_TYPE_A:RFCARD_TYPE_B))
			{
				send_result("line %d:%s测试失败(%d, 0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
				GOTOERR;
			}	
			if((ret=NDK_RfidPiccApdu(sizeof(Select_1pay), Select_1pay, &rcvlen, psRecebuf))!=NDK_OK||((sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x9000&&(sw=SMART_GETSW(psRecebuf+rcvlen-2))!=0x6d00))
			{
				send_result("line %d:%s测试失败(%d,0x%04x)", __LINE__, TESTAPI, ret, sw);
				GOTOERR;
			}
			if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				GOTOERR;
			}
		}
	}

	//测试结束
	send_result("%s测试通过", TESTAPI);
	goto ERR;
ERR:
	NDK_PiccQuickRequest(0);//关闭简易快速寻卡功能,机器下电后也会恢复默认设置0.此处是为了防止使用海信B卡代替标准B卡时测试会通过
	NDK_RfidPiccDeactivate(0);
	return;
}

