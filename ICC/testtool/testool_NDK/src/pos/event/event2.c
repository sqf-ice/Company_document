/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: event模块
* file name		: event2.c
* Author 		:  
* version		: 
* DATE			: 20160914
* directory 		: 
* description		: 测试NDK_SYS_RegisterEvent注册事件监听功能和NDK_UnRegisterEvent退出事件监听功能
* related document	: 高端平台统一API动态链接库编程手册V2.00.doc
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SYS_RegisterEvent,NDK_UnRegisterEvent"
#define MAXWAITTIME	30
#define PINTIMEOUT_MAX	(200*1000)

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
*			  	  	 	
*****************************************************************/
void event2(void)
{
	if(auto_flag==1)
	{
		send_result("line %d:%s无纯自动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*private & local definition*/
	int ret = -1, i = 0;
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar szPinOut[8+1]={0};
	int pnErrorCode=0;
	char pszTk1[MAXTRACKLEN]={0}, pszTk2[MAXTRACKLEN]={0}, pszTk3[MAXTRACKLEN]={0};
	EM_PRN_STATUS emStatus=0;

	/*process body*/
	cls_printf("测试%s...", TESTAPI);

	//测试前置:
	for(i=SYS_EVENT_MAGCARD;i<=SYS_EVENT_PRNTER;i++)
		NDK_SYS_UnRegisterEvent(i);
	NDK_MagClose();//磁卡关闭
	NDK_RfidPiccDeactivate(0);//射频卡下电

	//case1:参数异常测试
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_NONE-1,MAXWAITTIME*1000,notifyEvent))!=NDK_ERR_POSNDK_EVENT_NUM)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_MAX+1,MAXWAITTIME*1000,notifyEvent))!=NDK_ERR_POSNDK_EVENT_NUM)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_ICCARD,MAXWAITTIME*1000,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2: 注册事件同时设置多项时应该返回NDK_ERR_POSNDK_EVENT_NUM 错误的事件号
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_MAGCARD|SYS_EVENT_ICCARD,MAXWAITTIME*1000,notifyEvent))!=NDK_ERR_POSNDK_EVENT_NUM)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3:相同事件反复注册应该返回NDK_ERR_ POSNDK EVENT_REG_TWICE
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_ICCARD,MAXWAITTIME*1000,notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_ICCARD,MAXWAITTIME*1000,notifyEvent))!=NDK_ERR_POSNDK_EVENT_REG_TWICE)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SYS_UnRegisterEvent(SYS_EVENT_ICCARD);

#if 1	
	//case4:不同事件反复注册会成功:要想测事件被占用需要2个相同的进程,这里单进程无法实现
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_MAGCARD,MAXWAITTIME*1000,notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_ICCARD,MAXWAITTIME*1000,notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SYS_UnRegisterEvent(SYS_EVENT_MAGCARD);
	NDK_SYS_UnRegisterEvent(SYS_EVENT_ICCARD);
	sleep(1);//等待系统处理完  需要一点点的延时 与超时时间无关  
#endif

	//case5:测试监听超时时间<=0,不超时永不退出 需要调用退出监听函数,延时1s后再监听相同事件时应该会失败,说明之前的事件未注销	
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_MAGCARD,-10,notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(1);
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_MAGCARD,MAXWAITTIME*1000,notifyEvent))!=NDK_ERR_POSNDK_EVENT_REG_TWICE)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SYS_UnRegisterEvent(SYS_EVENT_MAGCARD);

	//case6:测试注册监听事件后超时退出应该返回SYS_EVENT_NONE(超时后会自动注销)
	flag = SYS_EVENT_MAGCARD;
	if((ret = NDK_SYS_RegisterEvent(SYS_EVENT_ICCARD, 1*1000,notifyEvent1))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	cls_show_msg1(2,"等待2s");
	if(flag != SYS_EVENT_NONE)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}	
	//case7:正常测试:监听无事件，回调函数标志位应该返回SYS_EVENT_NONE
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_NONE, 1*1000, notifyEvent))!=NDK_ERR_POSNDK_PERMISSION_UNDEFINED)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);//-4021
		GOTOERR;
	}
	//case8:正常测试:磁卡打开,监听磁卡事件
	//打开磁卡
	NDK_MagOpen();
	//case8.1:注册监听磁卡事件,提示刷卡,刷卡后事件应该被监听到,并调用回调函数将标志位设置成磁卡事件,然后退出监听事件应该成功
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_MAGCARD, MAXWAITTIME*1000, notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(30,"请在30秒内刷磁卡,刷完卡后按任意键继续");
	if(flag != SYS_EVENT_MAGCARD)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	//case8.2:退出监听事件后再刷卡,应该不会调用回调函数,flag保持0;
	flag = SYS_EVENT_NONE;
	cls_show_msg1(30,"请再刷磁卡,刷完卡后按任意键继续");
	if(flag != SYS_EVENT_NONE)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	NDK_MagClose();

	//case9:监听IC卡插入事件
	//case9.1:注册监听IC卡事件,提示插入IC卡,插卡后事件应该被监听到,并调用回调函数将标志位设置成IC卡事件,然后退出监听事件应该成功
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_ICCARD, MAXWAITTIME*1000, notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(30,"请在30秒插入IC卡后按任意键继续");
	if(flag != SYS_EVENT_ICCARD)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}	
	//case9.2:退出监听事件后拔IC卡(插拔IC卡都会触发事件的),应该不会调用回调函数,flag保持0;
	flag = SYS_EVENT_NONE;
	cls_show_msg1(30,"请在30s内拔掉IC卡后按任意键继续");
	if(flag != SYS_EVENT_NONE)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}

	//case10:监听非接卡事件
	if(NDK_RfidInit(NULL)!=NDK_OK)
	{
		cls_show_msg("射频接口器件初始化失败,任意键退出...");
		RETURN;
	}
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case10.1:注册监听非接卡事件,提示放射频卡后事件应该被监听到,并调用回调函数将标志位设置成非接卡事件,然后退出监听事件应该成功
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_RFID, MAXWAITTIME*1000, notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(30,"请在30秒内在感应区放置A卡后按任意键继续");
	if(flag != SYS_EVENT_RFID)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}	
	cls_show_msg("移走A卡后任意键继续");
	//case10.2:退出监听事件后再放非接卡,应该不会调用回调函数,flag保持0;
	flag = SYS_EVENT_NONE;
	cls_show_msg1(30,"请在感应区放置A卡后按任意键继续"); 
	if(flag != SYS_EVENT_NONE)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	//下电
	NDK_RfidPiccDeactivate(0);
	cls_show_msg("移走A卡后任意键继续");

	//case11:监听pin输入事件 
	//前置://安装TPK1(8bytes), ID=1 明文安装
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=8;
	memset(stKeyInfo.sDstKeyValue, 0x19, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//初始化
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case11.1:注册监听pin输入事件,提示输入之后会调用回调函数标志位返回
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_PIN, MAXWAITTIME*1000, notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_printf("尽快按确认...");//阻塞式可以用,非阻塞式也可以用 
	if((ret=NDK_SecGetPin(1, (uchar *)"0", NULL, szPinOut, SEC_PIN_ISO9564_1, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysDelay(5);//延时0.5s,可能底层处理需要点时间
	if(flag != SYS_EVENT_PIN)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}	
	//case11.2:退出监听事件后再调用pin输入,应该不会调用回调函数,flag保持0;
	flag = SYS_EVENT_NONE;
	if((ret=touchscrean_getnum())!=NDK_OK)
	{
		send_result("line %d:%s随机数字键盘初始化失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	} 
	cls_printf("尽快再次按确认...");//这里使用非阻塞式改成使用阻塞式(关注点是事件机制而不是getpin使用阻塞还是非阻塞)
	if((ret=NDK_SecGetPin(1, (uchar *)"0", NULL, szPinOut, SEC_PIN_ISO9564_1, PINTIMEOUT_MAX))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	if(flag != SYS_EVENT_NONE)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		RETURN;
	}
	
#if PRN_ENABLE  
	//case12:监听打印机状态(状态改变才会有返回值,其中正常状态到忙的状态检测不到)
	//前置:打印机初始化
	cls_show_msg("请确保有纸后按任意键继续...");
	if(lib_initprn(g_PrnSwitch) != NDK_OK)
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		NDK_PrnInit(g_PrnSwitch);
		RETURN;
	}
	//case12.1:注册监听打印机状态,当打印机从有纸状态变成无纸状态时能够让回调函数标志位返回打印机状态
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_PRNTER, MAXWAITTIME*1000, notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(30,"请在30s内取出打印纸后按任意键继续");
	if(flag != SYS_EVENT_PRNTER)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}	
	//case12.2:退出监听事件后,打印机从缺纸状态到有纸状态并不能使回调函数起作用,标志位保持为0
	flag = SYS_EVENT_NONE;
	cls_show_msg1(30,"请在30s内给打印机装纸后按任意键继续");
	if(flag != SYS_EVENT_NONE)
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}

	//case12.3:打印机状态从ok到busy,不会触发事件不好构造,打印完又成了ok,即从busy到ok会触发事件
	flag = SYS_EVENT_NONE;
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_PRNTER, MAXWAITTIME*1000, notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_PrnStr("打印新大陆NEWLAND打印测试页1:将打印1---80行\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n\
			11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n\
			31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n\
			51\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n63\n64\n65\n66\n67\n68\n69\n70\n\
			71\n72\n73\n74\n75\n76\n77\n78\n79\n80\n");
	//if(flag != SYS_EVENT_NONE)
	//打印函数NDK_PrnStr是非阻塞,会直接往下执行,返回busy状态,需要等到到ok状态后继续20170912
	while(1)
	{
		NDK_PrnGetStatus(&emStatus);
		if(emStatus==PRN_STATUS_OK)
			break;
	}
	NDK_SysDelay (3);//延时0.3s , 没有这个延时flag值不能改变
	if(flag != SYS_EVENT_PRNTER) 
	{
		send_result("line %d: %s测试失败(0x%08x,%d)", __LINE__, TESTAPI, flag,emStatus);
		GOTOERR;
	}

	//case12.4:打印机状态从busy到缺纸状态改变会触发事件
	flag = SYS_EVENT_NONE;
	cls_show_msg("装不足10cm的纸后任意键继续");
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_PRNTER, MAXWAITTIME*1000, notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
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
#endif

	//case13:注册的事件是IC卡事件,事件提示进行磁卡刷卡,flag保持不变	
	flag = SYS_EVENT_NONE;
	NDK_MagOpen();
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_ICCARD, MAXWAITTIME*1000, notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg1(30,"请在30秒内刷磁卡,刷完卡后按任意键继续");
	if(flag != SYS_EVENT_NONE)  
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	NDK_MagClose();
	NDK_SYS_UnRegisterEvent(SYS_EVENT_ICCARD);

	//case14:注册磁卡事件,回调函数并没有退出监听事件,进行一次刷卡后等会儿再进行刷卡应该能够触发事件把标志位置成磁卡的值
	NDK_MagOpen();
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_MAGCARD, 60*1000, notifyEvent1))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_show_msg("请在20秒内刷磁卡,刷完卡后按任意键继续");
	if(flag != SYS_EVENT_MAGCARD)  
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	//目前磁卡需要关闭打开后再刷卡才会触发  这里需要等待开发人员确认是否需要修改
	//开发确认需要调用NDK_MagReadNormal才会触发20170912
	//开发确认需要调用关闭打开后再刷卡才会触发20180130
	if((ret=NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &pnErrorCode))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pnErrorCode);
		GOTOERR;
	}
	NDK_MagClose();
	NDK_MagOpen();
	flag = SYS_EVENT_NONE;
	cls_show_msg("请等待5秒后继续刷磁卡,刷完卡后按任意键继续");
	if(flag != SYS_EVENT_MAGCARD)  
	{
		send_result("line %d: %s测试失败(0x%08x)", __LINE__, TESTAPI, flag);
		GOTOERR;
	}
	flag = SYS_EVENT_NONE;//清零
	NDK_SYS_UnRegisterEvent(SYS_EVENT_MAGCARD);//退出监听事件

	//case15:反复注销会失败返回NDK_ERR_POSNDK_EVENT_UNREG_TWICE  -4008
	if((ret=NDK_SYS_RegisterEvent(SYS_EVENT_ICCARD, 60*1000, notifyEvent))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SYS_UnRegisterEvent(SYS_EVENT_ICCARD))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	NDK_SysDelay(50);//延时50ms保证已经退出成功后 再次调用退出才会返回失败
	if((ret=NDK_SYS_UnRegisterEvent(SYS_EVENT_ICCARD))!=NDK_ERR_POSNDK_EVENT_UNREG_TWICE)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_MagClose();//磁卡关闭
	NDK_RfidPiccDeactivate(0);//射频卡下电
	for(i=SYS_EVENT_MAGCARD;i<=SYS_EVENT_PRNTER;i++)
		NDK_SYS_UnRegisterEvent(i);	
	return;	
}

