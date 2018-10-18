/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 新Wifi 模块
* file name		: wifi5.c
* Author 		:  
* version		: 
* DATE			:  
* directory 		: 
* description		: 测试NDK_WifiSetFunc是否能够正确设置是否开机自动连接功能和自动重连等功能
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_WifiSetFunc"	
#define WIFI_LEN 2047
#define MAXTIMEOUT 60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
void set_auto_connect(void)
{
	/*private & local definition*/
	int nkey = 0, ret = 0, data = 0;

	/*process body*/
	while(1)
	{
		nkey = cls_show_msg("1.开机自动连接\n"
							"2.开机不自动连接\n");
			switch(nkey)
			{	
				case '1':
					data = 1;
					if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_AUTO_START,&data)) != NDK_OK)
					{
						send_result("line %d: %s设置失败(%d)", __LINE__, TESTAPI, ret);
						return;
					}
					cls_show_msg1(2,"设置开机自动连接成功");
					return;
					break;
				case '2':
					data = 0;
					if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_AUTO_START,&data)) != NDK_OK)
					{
						send_result("line %d: %s设置失败(%d)", __LINE__, TESTAPI, ret);
						return;
					}
					cls_show_msg1(2,"设置开机不自动连接成功");
					return;
					break;
				case ESC:
					return;
					break;
				default:
					continue;
					break;
			}
	}
}

void set_table_and_start(void)
{
	/*private & local definition*/
	int ret = -1;
	ST_WIFI_STATUS status;
	ST_WIFI_PROFILE_LIST list_s;
	ST_WIFI_AP_ENTRY_T st_ap;
	list_s.stored_ap = &st_ap;
	//此处可以做修改AP名字及密码
#if defined ME68 || defined ME50NX  //夏新平台不支持逗号  20180515  modify
	char  test_ap[] = "`~!\"@$#[^%]*() -_=+|\\&{}:;?/><.";
#else
	char  test_ap[] = "`~!\"@$#[^%]*() -_=+|\\&{}:;?,/><.";
#endif
	char pwd[] = "1234567890";

	/*process body*/
	//设置好表格
	memset(&st_ap,0,sizeof(st_ap));
	strcpy(list_s.stored_ap->ssid,test_ap); 
	strcpy(list_s.stored_ap->pwd,pwd);
	//memcpy(list_s.stored_ap->mac.octet,ap_mac,6);
	list_s.stored_ap->flag = WIFI_AP_FLAG_DHCP_SET ;//| WIFI_AP_FLAG_MAC_SET
	list_s.stored_ap->priority = 0;
	list_s.stored_num = 1;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, test_ap) )
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		return;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	cls_show_msg1(2,"连接%s成功,接下来重启",test_ap);
#if 0//应开发要求测最基本功能:在不清空列表情况下验证能够自动连接 20180604 modify
	memset(&st_ap,0,sizeof(st_ap));
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
#endif
	return;
}
/*
正基版本:清空列表后不能自动连接,不清空列表后能够自动连接,故不自动连接验证不了
乐鑫:清不清列表后均能够自动连接
九九物联:目前是清空列表后不能自动连接,不清空列表后能够自动连接(未来是否会变待定)
由以上说明在验证不自动连接这点上目前验证不了,只进行基础功能验证:
	在不清空列表情况下要保证能够自动连接上
*/
void verify_after_set(void)
{
	/*private & local definition*/
	int nkey = 0, ret = 0;
	ST_WIFI_STATUS status;
#if defined ME68 || defined ME50NX  //夏新平台不支持逗号  20180515  modify
	char  test_ap[] = "`~!\"@$#[^%]*() -_=+|\\&{}:;?/><.";
#else
	char  test_ap[] = "`~!\"@$#[^%]*() -_=+|\\&{}:;?,/><.";
#endif
	
	/*process body*/
#if 0  //验证开机自动连接不需要调用start,正基模块验证,九九物联和乐鑫不验证
	//应陈镇江要求，增加前置NDK_WifiStart 20180503 sull modify
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		return;
	}
#endif
	while(1)
	{
		nkey = cls_show_msg("1.验证自动连接\n"
							"2.验证不自动连接\n");
			switch(nkey)
			{
				case '1':
					cls_printf("正在验证自动连接...");
					memset(&status,0,sizeof(status));
					if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, test_ap) )
					{
						send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
						return;
					}
					cls_show_msg1(2,"自动连接验证通过");
					return;
					break;
				case '2':
					memset(&status,0,sizeof(status));
#if 0//WIFI_PRIORITY_ENABLE //跟开发沟通后这点由于不同模块不尽相同,暂时验证不了,如果测出问题可以跟开发确认下20180604 modify
					if( ((ret=NDK_WifiStatus(&status))==NDK_OK) || (status.status!=WIFI_WLAN_STATUS_FAIL) || strcmp(status.ssid,""))//本用例等后续返回值文档确定后再修改返回值(文档要求)
#else
					if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!= WIFI_WLAN_STATUS_FAIL) )
#endif
					{
						send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
						return;
					}
					cls_show_msg1(2,"不自动连接验证通过");
					return;
					break;
				default:
					continue;
					break;
				case ESC:
					return;
					break;
			}
	}
}

void verify_relink(void)
{
	/*private & local definition*/
	int ret = 0, retime = -1, i = 0, data = 0;
	ST_WIFI_PROFILE_LIST list_s;
	ST_WIFI_AP_ENTRY_T st_ap;
	list_s.stored_ap = &st_ap;
	ST_WIFI_AP_LIST list_scan;
	ST_WIFI_AP_INFO ap[32];
	list_scan.list = ap;
	ST_WIFI_STATUS status;
	time_t oldtime;
	//此处可以做修改AP名字及密码和对应的MAC
#if defined ME68 || defined ME50NX  //夏新平台不支持逗号  20180515  modify
	char  test_ap[] = "`~!\"@$#[^%]*() -_=+|\\&{}:;?/><.";
#else
	char  test_ap[] = "`~!\"@$#[^%]*() -_=+|\\&{}:;?,/><.";
#endif
	char pwd[] = "1234567890"; 
	uint unLen = 0;
	char sBuf[128] = {0};

	/*process body*/
	//测试前置
	//九九物联和乐鑫模块需要设置开机自动连接(其实是自动重连功能,而原来的开机自动连接功能没有,只有正基模块有开机自动连接功能)
#if !WIFI_PRIORITY_ENABLE
	data = 1;
	if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_AUTO_START,&data)) != NDK_OK)
	{
		send_result("line %d: %s设置失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif
	//获取硬件信息 判断WiFi模块 20180703 modify
	memset(sBuf,0xff,sizeof(sBuf));
	if((ret=NDK_SysGetPosInfo(SYS_HWINFO_GET_HARDWARE_INFO, &unLen, sBuf)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	
	NDK_WifiStop();
	memset(&st_ap,0,sizeof(st_ap));
	strcpy(list_s.stored_ap->ssid,test_ap); 
	strcpy(list_s.stored_ap->pwd,pwd);
	list_s.stored_ap->flag = WIFI_AP_FLAG_DHCP_SET ;
	list_s.stored_ap->priority = 0;
	list_s.stored_num = 1;
	if((ret = NDK_WifiProfileSet(&list_s)) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if(sBuf[8]==0x03||sBuf[8]==0x02)  //如果是九九物联模块\乐鑫模块需要先进行一次start后才会自动重连
	{
		if((ret = NDK_WifiStart()) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		memset(&status,0,sizeof(status));
		if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, test_ap))
		{
			send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
			GOTOERR;
		}
	}		

	//case1:测试自动重连时间为5000ms
	//step1:路由器先关闭
	cls_show_msg("请关闭AP:%s路由器的电源后任意键继续", test_ap);
	//step2:设置自动重连时间，该函数设置的值在下一次start中生效
	retime = 5000;
	if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_RELINK_TIME,&retime)) != NDK_OK)//此函数时间设置九九物联和乐鑫模块无效
	{
		send_result("line %d: %s设置失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(sBuf[8]!=0x03&&sBuf[8]!=0x02)// 九九物联模块\乐鑫模块不需要再调用start,其他模块需要调用
	{	
		//step3:重新调用AP，但由于AP没有开，预期应该连接失败
		if((ret = NDK_WifiStart()) != NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		memset(&status,0,sizeof(status));
		if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_FAIL) || strcmp(status.ssid,""))
		{
			send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
			GOTOERR;
		}
	}
	//step4:给AP上电，此处通过扫描到AP名来判断AP启动成功
	cls_show_msg1(5,"请开启AP:%s路由器的电源后任意键继续", test_ap);
	oldtime=time(NULL);
	while(1)
	{
		cls_show_msg1(1,"扫描AP信息中...");
		memset(ap,0,sizeof(ap));
		list_scan.num = 128;//希望数量设置成足够大,测试环境也要尽量构造足够多的AP进行扫描,测试负载
		ret=NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan);
		if((ret==NDK_OK) && (list_scan.num>0))
		{
			for(i=0;i<list_scan.num;i++)
			{
				if(!strcmp(test_ap, ap[i].ssid))
					break;
			}
			if(i<list_scan.num)
				break;
		}
		//超时退出
		if(time(NULL)-oldtime>MAXTIMEOUT)
		{	
			send_result("line %d:未扫描到AP:%s,超时退出,%s测试失败", __LINE__, test_ap, TESTAPI); 
			break;
		}
	}
	//step5:AP打开后，应用层不用再调用NDK_WifiStart函数，底层应该能够自动发起重连，并连接成功
	NDK_SysMsDelay(retime+10000);//延时5s  retime//根据RDM上BUG2017030600726问题增加10秒延时 20170306
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_SUCC) || strcmp(status.ssid, test_ap))
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:测试未调用NDK_WifiStart的情况下调用NDK_WifiSetFunc应该成功
	retime = 5000;
	if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_RELINK_TIME,&retime)) != NDK_OK)
	{
		send_result("line %d: %s设置失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:测试设置不自动重连-1
	//step1:路由器先关闭
	cls_show_msg("请关闭AP:%s路由器的电源后任意键继续", test_ap);
	//step2:设置不重连
	retime = -1;
	if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_RELINK_TIME,&retime)) != NDK_OK)
	{
		send_result("line %d: %s设置失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//step3:开始连接AP，但由于AP没有开，预期应该连接失败
	if((ret = NDK_WifiStart()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_FAIL) || strcmp(status.ssid,""))
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	//step4:给AP上电，此处通过扫描到AP名来判断AP启动成功
	cls_show_msg1(5,"请开启AP:%s路由器的电源后任意键继续", test_ap);
	oldtime=time(NULL);
	while(1)
	{
		cls_show_msg1(1,"扫描AP信息中...");
		memset(ap,0,sizeof(ap));
		list_scan.num = 128;//希望数量设置成足够大,测试环境也要尽量构造足够多的AP进行扫描,测试负载
		if( ((ret=NDK_WifiGetFunc(WIFI_GET_FUNC_SCAN_RESULT,&list_scan))!=NDK_OK) || (list_scan.num==0) )
		{
			send_result("line %d:%s测试失败(ret=%d,num=%d)", __LINE__, TESTAPI, ret, list_scan.num);
			GOTOERR;
		}
		if(list_scan.num>0)
		{
			for(i=0;i<list_scan.num;i++)
			{
				if(!strcmp(test_ap, ap[i].ssid))
					break;
			}
			if(i<list_scan.num)
				break;
		}
		//超时退出
		if(time(NULL)-oldtime>MAXTIMEOUT)
		{	
			send_result("line %d:未扫描到AP:%s,超时退出,%s测试失败", __LINE__, test_ap, TESTAPI); 
			break;
		}
	}
	//step5:AP打开后，应用层不用再调用NDK_WifiStart函数，底层应该不会发起自动重连，连接应该失败
	memset(&status,0,sizeof(status));
	if( ((ret=lib_getwifistatus(&status))!=NDK_OK) || (status.status!=WIFI_WLAN_STATUS_FAIL) || strcmp(status.ssid,""))
	{
		send_result("line %d: %s测试失败(%d,%d,%s)", __LINE__, TESTAPI, ret, status.status, status.ssid);
		GOTOERR;
	}
	if((ret = NDK_WifiStop()) != NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:设置重连时间为0 立即重连 这个很难构造 因为立即重连的话实现起来pos上的表现就会卡在那，会卡是什么情况表现?????
	
	cls_show_msg1(3,"重连测试通过");
ERR:
	NDK_WifiStop();
	return;
}

/****************************************************************
* function name 	 				: void wifi5(void)
* functional description 			:  
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  			jiangym		20160226   created 		 
*****************************************************************/
void wifi5(void)
{
	/*private & local definition*/
	int nkey = 0, ret = 0, data = 1;
	
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);

	//case1:参数异常 WIFI_SET_FUNC_AUTO_START-1 WIFI_SET_FUNC_UNDEFINE+1
	if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_AUTO_START-1,&data)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WifiSetFunc(WIFI_GET_FUNC_UNDEFINE,&data)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WifiSetFunc(WIFI_GET_FUNC_UNDEFINE+1,&data)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_AUTO_START,NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_RELINK_TIME,NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//设置默认情况
	data = 0;
	if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_AUTO_START,&data)) != NDK_OK)
	{
		send_result("line %d: %s设置失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2: 各种组合测试
	while(1)
	{
		nkey = cls_show_msg("1.设置是否开机自动连接\n"
							"2.重启\n"
							"3.设置后的验证\n"
							"4.重连测试");
			switch(nkey)
			{	
				case '1':
					set_auto_connect();
					break;
				case '2':
					cls_show_msg1(2,"重启前先进行一次ap连接...");//根据开发设计开机自动连接只会尝试上次连接成功的AP，如果上次连接成功的AP被删除(AP未开,或者表格中被删除了)就会自动连接失败，并且不会再根据新表格去尝试连接，此处需要要求开发补充文档说明
					set_table_and_start();
					NDK_SysReboot();
					break;
				case '3':
					verify_after_set();
					break;
				case '4':
					verify_relink();
					break;
				case ESC:
					//恢复默认值重启有效
					cls_show_msg("注意:如果设置过开机自动连接需重启后再进行其它wifi用例的测试，任意键继续退出");
					data = 0;
					if((ret = NDK_WifiSetFunc(WIFI_SET_FUNC_AUTO_START,&data)) != NDK_OK)
					{
						cls_show_msg1(2,"line %d: 恢复设置默认值失败(%d)", __LINE__, ret);
						return;
					}
					NDK_WifiStop();//测试完不论中间是否测试成功，都要关闭wifi
					return;
					break;
			}
	}
}

