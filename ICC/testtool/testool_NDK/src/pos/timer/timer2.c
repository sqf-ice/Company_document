/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: timer模块
* file name		: timer2.c
* Author 			: huangbs
* version			: 
* DATE			: 20120905
* directory 		: 
* description		: 测试NDK_WaitAndFlag和NDK_CancelTimer接口功能
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

#define TESTAPI	"NDK_WaitAndFlag,NDK_CancelTimer"
//#define MAXTIMEOUT 20

/****************************************************************
* function name 	 	: void timer2(void)
* functional description 	: 延时标识位复位以及取消延时功能
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		: author		date		remarks
*			  	  	  huangbs     20120906	created
*****************************************************************/
void timer2(void)
{
	uint TmrFlag1=0, TmrFlag2=0;
	NDK_HANDLE tmr1=NULL;
	NDK_HANDLE tmr2=NULL;
	int cnt=0;
	time_t oldtime;

	cls_printf("测试%s...", TESTAPI); 

	//case1 :传入非法参数
	if((tmr1=NDK_WaitAndFlag(ONESECOND*5, NULL))!=NULL)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	NDK_CancelTimer(NULL);       

	//case 3: 先注册一个tmr1,时间到时再注册tmr2
	if((tmr1=NDK_WaitAndFlag(ONESECOND*3, &TmrFlag1))==NULL)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	oldtime=time(NULL);
	while(1)
	{
		// Check for timer expiration
		if(TmrFlag1)
		{
			NDK_CancelTimer(tmr1);
			//注册tmr2
			if((tmr2=NDK_WaitAndFlag(ONESECOND*5, &TmrFlag2))==NULL)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				return;
			}
			break;
		}
		//超时防死循环
		if((time(NULL)-oldtime)>5)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return;
		}
	}
	oldtime=time(NULL);
	while(1)
	{
		if(TmrFlag2)
		{
			NDK_CancelTimer(tmr2);
			break;
		}
		//超时防死循环
		if((time(NULL)-oldtime)>8)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return;
		}
	}

	//case 4:调NDK_WaitAndFlag后,TmrFlag1应清0.时间未到前,主动调NDK_CancelTime.在释放后，再次cancel，应无副作用
	oldtime=time(NULL);
	if((tmr1=NDK_WaitAndFlag(ONESECOND*10, &TmrFlag1))==NULL)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	if((time(NULL)-oldtime)>2)//非阻塞的话,调用花不了什么时间
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}	
	if(TmrFlag1!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, TmrFlag1);
		RETURN;
	}
	oldtime=time(NULL);
	while(1)
	{
		// Check for timer expiration
		if(TmrFlag1)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return;
		}
		if(cnt++==3)
		{
			NDK_CancelTimer(tmr1);
			break;
		}
		//超时防死循环
		if((time(NULL)-oldtime)>12)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return;
		}
	}	
	NDK_CancelTimer(tmr1);

	//case 5: 同时注册2个tmr. 其中,tmr1到期时间为3s,tmr2到期时间为20s,tmr1应先到期,到期后取消tmr2,过期标志应未置上
	if((tmr1=NDK_WaitAndFlag(ONESECOND*3, &TmrFlag1))==NULL)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	if((tmr2=NDK_WaitAndFlag(ONESECOND*20, &TmrFlag2))==NULL)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	oldtime=time(NULL);
	while(1)
	{
		// Check for timer expiration
		if(TmrFlag1)
		{
			NDK_CancelTimer(tmr1);
			NDK_CancelTimer(tmr2);
			break;
		}
		if(TmrFlag2)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return;
		}
		//超时防死循环
		if((time(NULL)-oldtime)>22)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return;
		}
	}
	if(TmrFlag2)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}	

	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}

