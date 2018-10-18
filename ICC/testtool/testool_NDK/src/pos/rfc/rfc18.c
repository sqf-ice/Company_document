/************************************************************************
*
* copyright(c)2012-2015 Newland Co. Ltd. All rights reserved
* module			: rfc模块
* file name			: rfc18.c
* Author 			:  
* version			: 
* DATE				:  
* directory 			: 
* description			: 测试NDK_Iso15693Write_SingleBlock,NDK_Iso15693Read_SingleBlock,NDK_Iso15693Read_MultipleBlocks能否正确进行读写操作(Iso15693协议)
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_Iso15693Write_SingleBlock,NDK_Iso15693Read_SingleBlock,NDK_Iso15693Read_MultipleBlocks"

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
*			  		   	 jiangym			20180127		created
*****************************************************************/
void rfc18(void)
{
	int ret = -1, i = 0;
	uint cardcount = 0, maxcards = 20;
	uchar cmd = 0x00;
	uchar ucBlocknum = 0;
	uchar startblock = 0;
	uchar count = 0;
	iso15693ProximityCard_t cards[20];		//定义最多支持20张ISO1569卡的读写
	iso15693PiccMemoryBlock_t w_memBlock, w2_memBlock, r_memBlock;
	iso15693PiccSystemInformation_t sysInfo;
	iso15693PiccMemoryBlock_t memBlock[64];
	
	//初始化
	if((ret=NDK_Iso15693_init()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	
	//case1:参数异常,返回失败
	if((ret=NDK_Iso15693Read_SingleBlock(cards[0],ucBlocknum,NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	
	//case2:防冲突激活后未进行写块操作直接读??
	cls_show_msg("放置一张15693卡片,任意键继续");
	if((ret=NDK_Iso15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,&cardcount,cards)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_Iso15693SelectPicc(cards[0])) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	ucBlocknum = 0x01;
	memset(&r_memBlock, 0, sizeof(iso15693PiccMemoryBlock_t));
	if((ret=NDK_Iso15693Read_SingleBlock(cards[0],ucBlocknum,&r_memBlock)) != NDK_OK)//读到什么??
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//读多块内容??
	count = 5;
	if((ret=NDK_Iso15693Read_MultipleBlocks(cards[0],startblock,count,memBlock)) != NDK_OK)//读到什么??
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//case3:进行写块操作
	ucBlocknum = 0x01;
	w_memBlock.data[0] = 0x02;
	w_memBlock.data[1] = 0x08;
	w_memBlock.data[2] = 0x04;
	w_memBlock.data[3] = 0x09;	
	if((ret=NDK_Iso15693Write_SingleBlock(cards[0],w_memBlock,ucBlocknum)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	
	//case4:写完进行正常读
	ucBlocknum = 0x01;
	memset(&r_memBlock, 0, sizeof(iso15693PiccMemoryBlock_t));
	if((ret=NDK_Iso15693Read_SingleBlock(cards[0],ucBlocknum,&r_memBlock)) != NDK_OK)//读到什么??
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	//读完进行比较内容是否一样??
	if((r_memBlock.data[0]!=w_memBlock.data[0])||(r_memBlock.data[1]!=w_memBlock.data[1])||(r_memBlock.data[2]!=w_memBlock.data[2])||(r_memBlock.data[3]!=w_memBlock.data[3]))
	{
		send_result("line %d:%s测试失败(%02x:%02x:%02x:%02x)", __LINE__, TESTAPI, r_memBlock.data[0], r_memBlock.data[1], r_memBlock.data[2], r_memBlock.data[3]);
		GOTOERR;
	}	
	
	//case5:对同一块写2次读的内容是第二次的?
	ucBlocknum = 0x01;
	w_memBlock.data[0] = 0x02;
	w_memBlock.data[1] = 0x08;
	w_memBlock.data[2] = 0x04;
	w_memBlock.data[3] = 0x09;	
	if((ret=NDK_Iso15693Write_SingleBlock(cards[0],w_memBlock,ucBlocknum)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	w2_memBlock.data[0] = 0x01;
	w2_memBlock.data[1] = 0x02;
	w2_memBlock.data[2] = 0x03;
	w2_memBlock.data[3] = 0x04;
	if((ret=NDK_Iso15693Write_SingleBlock(cards[0],w2_memBlock,ucBlocknum)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	ucBlocknum = 0x01;
	memset(&r_memBlock, 0, sizeof(iso15693PiccMemoryBlock_t));
	if((ret=NDK_Iso15693Read_SingleBlock(cards[0],ucBlocknum,&r_memBlock)) != NDK_OK)//读到什么??
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((r_memBlock.data[0]!=w2_memBlock.data[0])||(r_memBlock.data[1]!=w2_memBlock.data[1])||(r_memBlock.data[2]!=w2_memBlock.data[2])||(r_memBlock.data[3]!=w2_memBlock.data[3]))
	{
		send_result("line %d:%s测试失败(%02x:%02x:%02x:%02x)", __LINE__, TESTAPI, r_memBlock.data[0], r_memBlock.data[1], r_memBlock.data[2], r_memBlock.data[3]);
		GOTOERR;
	}	
	
	//case6:再读1次内容一样?
	ucBlocknum = 0x01;
	memset(&r_memBlock, 0, sizeof(iso15693PiccMemoryBlock_t));
	if((ret=NDK_Iso15693Read_SingleBlock(cards[0],ucBlocknum,&r_memBlock)) != NDK_OK)//读到什么??
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((r_memBlock.data[0]!=w2_memBlock.data[0])||(r_memBlock.data[1]!=w2_memBlock.data[1])||(r_memBlock.data[2]!=w2_memBlock.data[2])||(r_memBlock.data[3]!=w2_memBlock.data[3]))
	{
		send_result("line %d:%s测试失败(%02x:%02x:%02x:%02x)", __LINE__, TESTAPI, r_memBlock.data[0], r_memBlock.data[1], r_memBlock.data[2], r_memBlock.data[3]);
		GOTOERR;
	}	

	//case7:连续写5块内容,然后读5块内容,再进行比较??
	for(i=1;i<6;i++)
	{
		ucBlocknum = i;
		w_memBlock.data[0] = 0x02;
		w_memBlock.data[1] = 0x04;
		w_memBlock.data[2] = 0x06;
		w_memBlock.data[3] = 0x08;	
		if((ret=NDK_Iso15693Write_SingleBlock(cards[0],w_memBlock,ucBlocknum)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}	
	}
	count = 5;
	if((ret=NDK_Iso15693Read_MultipleBlocks(cards[0],startblock,count,memBlock)) != NDK_OK)//读到什么??
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	

	//测试结束
	send_result("%s测试通过", TESTAPI);
	
ERR:
	NDK_Iso15693_Deinit();
	return;
}

