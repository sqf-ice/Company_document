/************************************************************************
*
* module		   : 事件机制之事件挂起
* file name 	   : Event5.c
* Author		   : linying
* version		   : 
* DATE			   : 20180521
* directory 	        : 
* description	   : 事件挂起(NDK_SysSuspenedEvent(EM_SYS_EVENT event)) (据严彬反馈该接口只支持射频事件，其他事件注册调用挂起事件都会返回-3303)
* related document :
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SysSuspenedEvent"
#define MAXWAITTIME	30

/*------------global variables definition-----------------------*/
static EM_SYS_EVENT flag = 0;

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
static int notifyEvent(EM_SYS_EVENT eventNum, int msgLen, char * msg)
{
	flag = eventNum;
	NDK_SYS_UnRegisterEvent(eventNum);

	return SUCC;
}
static int notifyEvent1(EM_SYS_EVENT eventNum, int msgLen, char * msg)
{
	flag = eventNum;
	//cls_printf("0x%08x",eventNum);
	return SUCC;
}
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author		date		remarks
*					linying 		   20180521 	   created
*****************************************************************/
void event5(void)
{
	if(auto_flag==1)
	{
		send_result("line %d:%s无纯自动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*private & local definition*/
    	int i=0,ret=0,pnDatalen=0,pnSta=-1;
	uchar psDatabuf[MAXLEN_DATA]={0};
	uchar sendbuf[]={'\x00', '\x84', '\x00', '\x00', 8};

	/*process body*/
	cls_printf("测试%s...", TESTAPI);
    
	//测试前置:
	for(i=SYS_EVENT_MAGCARD;i<=SYS_EVENT_PRNTER;i++)
		NDK_SYS_UnRegisterEvent(i);
	NDK_RfidPiccDeactivate(0);//射频卡下电

	//case1:参数异常测试,传入错误的事件号，应返回错误
	if((ret = NDK_SysSuspenedEvent(SYS_EVENT_NONE-1)!=NDK_ERR_PARA))
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret =  NDK_SysSuspenedEvent(SYS_EVENT_MAX+1))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret =  NDK_SysSuspenedEvent(0X00000016))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2.1: 流程异常，未有其他事件正在运行，调用挂起事件(除非接)，应返回事件不支持(-3303)
	if((ret =NDK_SysSuspenedEvent(SYS_EVENT_MAGCARD))!=NDK_ERR_EVENT_UNREALIZED)
	{
		send_result("line %d: %s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret =NDK_SysSuspenedEvent(SYS_EVENT_PIN))!=NDK_ERR_EVENT_UNREALIZED)
	{
		send_result("line %d: %s测试失败ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret =NDK_SysSuspenedEvent(SYS_EVENT_PRNTER))!=NDK_ERR_EVENT_UNREALIZED)
	{
		send_result("line %d: %s测试失败ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
    	//case2.2: 流程异常，未有其他事件正在运行，调用非接挂起事件，应返回事件未注册(-3301)
	if((ret =NDK_SysSuspenedEvent(SYS_EVENT_RFID))!=NDK_ERR_EVENT_UNREGISTER)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
   	// case5:注册非接事件(超时时间要尽可能的大)，未挂起该事件，进行M1卡的request操作，会返回SPI总线冲突，驱动调用错误(-5)(实际测试情况，因为在服务那边已经添加了suspend挂起事件操作，所以无法构造SPI冲突的情况)
    	if(NDK_RfidInit(NULL)!=NDK_OK)
	{
		cls_show_msg("射频接口器件初始化失败,任意键退出...");
		RETURN;
	}
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_A))!=NDK_OK)
	{
		send_result("line %d:%s设置卡片类型失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_RFID,-1,notifyEvent))!=NDK_OK)
	{
		send_result("line %d:%s非接注册失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_M1Request(REQA, &pnDatalen, psDatabuf))!=NDK_ERR_MI_NOTAGERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
   	 if((ret= NDK_SYS_UnRegisterEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d: %s退出事件监听失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

#if IC1_ENABLE
	// case7:注册射频卡事件后多次挂起射频事件，不应影响到其他事件的操作
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_A))!=NDK_OK)
	{
		send_result("line %d:%s设置卡片类型失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_RFID,MAXWAITTIME*1000,notifyEvent))!=NDK_OK)
	{
		send_result("line %d:%s非接注册失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
    	if((ret =NDK_SysSuspenedEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret =NDK_SysSuspenedEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
   	cls_show_msg("请在30s内插入IC卡后任意键继续");
	if((ret=NDK_IccDetect(&pnSta))!=NDK_OK||(pnSta&0xff)!=0x01)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, (pnSta&0xff));
		GOTOERR;
	}
	//IC上电
	if((ret=NDK_IccPowerUp(ICTYPE_IC,psDatabuf, &pnDatalen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
    	if ((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{				
		send_result("line %d:下电失败(%d)", __LINE__,  ret);
		GOTOERR;
	}
    	if((ret= NDK_SYS_UnRegisterEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d: %s退出事件监听失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

   	// case3.1:注册射频卡事件，挂起射频卡事件后应监听不到射频事件，进行IC卡或磁卡操作，应能正常操作IC卡或磁卡
   	 flag =SYS_EVENT_NONE;
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_RFID,MAXWAITTIME*1000,notifyEvent))!=NDK_OK)
	{
		send_result("line %d:%s非接注册失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
    	if((ret =NDK_SysSuspenedEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_show_msg("请放置射频A卡,放置完毕任意键继续");
	if(flag==SYS_EVENT_RFID)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
    	cls_show_msg("请在30s内插入IC卡后任意键继续");
	if((ret=NDK_IccDetect(&pnSta))!=NDK_OK||(pnSta&0xff)!=0x01)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, (pnSta&0xff));
		GOTOERR;
	}
	//IC上电
	if((ret=NDK_IccPowerUp(ICTYPE_IC,psDatabuf, &pnDatalen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(psDatabuf,0,sizeof(psDatabuf));
	if((ret=NDK_Iccrw(ICTYPE_IC,sizeof(sendbuf),sendbuf,&pnDatalen,psDatabuf))!=NDK_OK)
	{		
		send_result("line %d:IC1读卡失败(%d)", __LINE__, ret);
		GOTOERR;															
	}
    	if ((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{				
		send_result("line %d:下电失败(%d)", __LINE__,  ret);
		GOTOERR;
	}
    	if((ret= NDK_SYS_UnRegisterEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d: %s退出事件监听失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	// case6:注册射频卡事件，挂起射频卡卡事件后立即注销射频卡卡事件，不影响后续操作
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_RFID,-10,notifyEvent))!=NDK_OK)
	{
		send_result("line %d:%s非接注册失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret =NDK_SysSuspenedEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
    	if((ret= NDK_SYS_UnRegisterEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d: %s退出事件监听失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
		
	// case3.2:注册IC卡事件，挂起射频卡事件后进行IC卡操作，应能正常操作IC卡
#if IC1_ENABLE
    	flag =SYS_EVENT_NONE;
     	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_ICCARD,MAXWAITTIME*1000,notifyEvent1))!=NDK_OK)
	{
		send_result("line %d:%sIC注册失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
    	if((ret =NDK_SysSuspenedEvent(SYS_EVENT_RFID))!=NDK_ERR_EVENT_UNREGISTER)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	} 
    	cls_show_msg("请在30s内插入IC卡后任意键继续");
	if(flag!=SYS_EVENT_ICCARD)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
    	if((ret= NDK_SYS_UnRegisterEvent(SYS_EVENT_ICCARD))!=NDK_OK)
	{
		send_result("line %d: %s退出事件监听失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
		
	// case4:注册射频卡事件，挂起射频事件后进行开启IC卡事件，应能正常监听到IC卡插入事件
    	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_RFID,MAXWAITTIME*1000,notifyEvent))!=NDK_OK)
	{
		send_result("line %d:%s非接注册失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret =NDK_SysSuspenedEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
    	flag =SYS_EVENT_NONE;
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_ICCARD,MAXWAITTIME*1000,notifyEvent1))!=NDK_OK)
	{
		send_result("line %d:%sIC注册失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
   	 cls_show_msg("请在30s内插入IC卡后任意键继续");
	if(flag!=SYS_EVENT_ICCARD)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	// 注销射频和IC卡事件
   	if((ret= NDK_SYS_UnRegisterEvent(SYS_EVENT_ICCARD))!=NDK_OK)
	{
		send_result("line %d: %s退出事件监听失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
    	if((ret= NDK_SYS_UnRegisterEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d: %s退出事件监听失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	// case5.1:注册射频卡事件，挂起密码键盘事件，应不影响到射频卡事件的监听
	flag=SYS_EVENT_NONE;
	//注册射频卡事件
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_RFID,MAXWAITTIME*1000,notifyEvent1))!=NDK_OK)
	{
		send_result("line %d:%s非接注册失败(%d)", __LINE__, TESTAPI, ret);
        	RETURN;
	}
	//挂起密码键盘事件
	if((ret =NDK_SysSuspenedEvent(SYS_EVENT_PIN))!=NDK_ERR_EVENT_UNREALIZED)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_show_msg("请在30s内放置射频A卡后任意键继续");
	if(flag!=SYS_EVENT_RFID)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
    	if((ret= NDK_SYS_UnRegisterEvent(SYS_EVENT_RFID))!=NDK_OK)
	{
		send_result("line %d: %s退出事件监听失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
		
#if PRN_ENABLE
	// case5.2:注册打印事件，挂起射频事件，应不影响到打印事件的监听
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//初始化使开始设置的配置有效
	{
		send_result("line %d:%s打印初始化失败",__LINE__,TESTAPI);
		GOTOERR;
	}
	cls_show_msg("请确保装不足10cm的打印纸后按任意键继续...");
	flag=SYS_EVENT_NONE;
	//注册打印事件
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_PRNTER,MAXWAITTIME*1000,notifyEvent1))!=NDK_OK)
	{
		send_result("line %d:%s非接注册失败(%d)", __LINE__, TESTAPI, ret);
        	RETURN;
	}
	if((ret =NDK_SysSuspenedEvent(SYS_EVENT_RFID))!=NDK_ERR_EVENT_UNREGISTER)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
    	NDK_PrnStr ("打印测试页1:将打印1---120行\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n\
			11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n\
			31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n\
			51\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n63\n64\n65\n66\n67\n68\n69\n70\n\
			71\n72\n73\n74\n75\n76\n77\n78\n79\n80\n81\n82\n83\n84\n85\n86\n87\n88\n89\n90\n\
			91\n92\n93\n94\n95\n96\n97\n98\n99\n100\n101\n102\n103\n104\n105\n106\n107\n108\n109\n110\n\
			111\n112\n113\n114\n115\n116\n117\n118\n119\n120\n");
	if(flag != SYS_EVENT_PRNTER)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}	
    	if((ret= NDK_SYS_UnRegisterEvent(SYS_EVENT_PRNTER))!=NDK_OK)
	{
		send_result("line %d: %s退出事件监听失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
#endif

	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);//射频卡下电
	for(i=SYS_EVENT_MAGCARD;i<=SYS_EVENT_PRNTER;i++)
		NDK_SYS_UnRegisterEvent(i);	
	return;	
}

