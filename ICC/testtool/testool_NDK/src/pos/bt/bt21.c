/************************************************************************
* copyright(c)2005-2015 Newland Co. Ltd. All rights reserved
* module			:蓝牙
* file name			: bt21.c
* Author 			: 
* version			: 
* DATE				:
* directory 			: 
* description			: 测试蓝牙主模式下NDK_BTMasterGetScanResults 是否可以扫描到想要的扫描结果
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_BTMasterGetScanResults"	
#define EXPMAXNUM 10

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt21
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  zhengry			20150602		created
*****************************************************************/

void bt21(void)	//无参数无返回
{
	/*private & local definition*/
	ST_BT_DEV sresult[50],sresult1[50];
	int ret=0,i=0,snum=0,snum1=0,snum2=0;	
	char *inbuf[3]={"newland",
				    "蓝牙",
				    "`~!\"@$#[^%]*()-_=+|&{}:;?/><,"};
	memset(sresult,0,sizeof(sresult));
	memset(sresult1,0,sizeof(sresult1));
	
	/*process body*/
	cls_printf("测试%s...", TESTAPI); 

	//测试前置
	NDK_PortClose(PORT_NUM_BT);

	//case1：蓝牙串口未打开，获取扫描结果应该失败
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult,EXPMAXNUM,&snum))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;	
	}
	
	//打开蓝牙串口
	if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//case2:刚打开串口，未调用开始扫描函数情况下调用获取扫描函数预期成功,但应该扫描到0个
	memset(sresult,0,sizeof(sresult));
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult,5,&snum))!=NDK_OK||snum!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, snum);//如果执行完20号用例再执行本用例在此处返回snum=5
		GOTOERR;	
	}

	//case3：参数异常返回失败	
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_BREDR))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterGetScanResults(NULL,NULL,EXPMAXNUM,&snum))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult,EXPMAXNUM,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}		
	if((ret=NDK_BTMasterStopScan())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}

	//case4：设定指定的设备名称（名称可以为：正常英文，特殊字符，中文名称），扫描结果应只是该名称的设备且获取数量为1 
	for(i=0;i<3;i++)// 3只能搜索到长度为29字节的蓝牙名称
	{
		cls_show_msg("请将1台蓝牙从设备的名称修改为:%s，修改后请按任意键继续。",inbuf[i]); 
		if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_DUMO))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;	
		}
		sleep(10);//给一定的扫描时间
		memset(sresult,0,sizeof(sresult));//每次获取前清空 以避免干扰下次的结果
		if((ret=NDK_BTMasterGetScanResults(inbuf[i],sresult,EXPMAXNUM,&snum))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;	
		}
		//cls_show_msg("snum=%d,i=%d",snum, i);
		//扫描第一个字符串newland，结果snum=0,i=0。而实际pos可以扫描到多台蓝牙设备，但扫描结果数目却为0
		if(strcmp(sresult[0].sBtName,inbuf[i])) 
		{
			send_result("line %d:%s测试失败(%s)", __LINE__, TESTAPI, sresult[0].sBtName);
			GOTOERR;
		}	
		if((ret=NDK_BTMasterStopScan())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;	
		}
	}
	
	//case5：设定2台相同名称的设备，扫描结果应可以扫描到这两台设备
	cls_show_msg("请将2台蓝牙从设备的名称修改为:%s，修改后请按任意键继续。",inbuf[0]); 
	cls_printf("正在扫描中，请等待30秒...");
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_DUMO))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	sleep(30);//给一定的扫描时间
	memset(sresult,0,sizeof(sresult));
	if((ret=NDK_BTMasterGetScanResults(inbuf[0],sresult,EXPMAXNUM,&snum))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	if(snum<2)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, snum);
		GOTOERR;	
	}
	for(i=0;i<snum;i++)
		cls_show_msg("%d.蓝牙名称:%s\nMAC地址:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x:0x%2x:\nRSSI:%d\n设备类型:%d",i+1,sresult[i].sBtName,sresult[i].sBtMac[0],sresult[i].sBtMac[1],sresult[i].sBtMac[2],sresult[i].sBtMac[3],sresult[i].sBtMac[4],sresult[i].sBtMac[5],sresult[i].nRssi,sresult[i].emBtType);
	if(cls_show_msg("扫描到的蓝牙名为%s的蓝牙设备信息是否正确:是[Enter],否[其他]",inbuf[0])!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	if((ret=NDK_BTMasterStopScan())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	
	//case6：设定希望最大蓝牙数目为0台时，判断扫描的结果数目是否为0台
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_DUMO))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	sleep(10);//给一定的扫描时间
	memset(sresult,0,sizeof(sresult));
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult,0,&snum))!=NDK_OK||snum!=0)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, snum);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterStopScan())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}

	//case7:设定希望最大的蓝牙数目，当实际的数目小于设定的蓝牙数目，扫描的结果数目应等于实际的蓝牙数目
	cls_show_msg("请确保此时的设备小于20台,确认后请按任意键继续");
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_DUMO))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	sleep(10);//给一定的扫描时间
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult1,20,&snum1))!=NDK_OK||snum1>20)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, snum1);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterStopScan())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}

	//实际测试到扫描数目为6台，与预期2台的最大扫描数目不符合
	//case8：设定希望最大的蓝牙数目，当实际的数目大于设定的蓝牙数目，扫描的结果数目应等于设定的蓝牙数目	
	cls_show_msg("请确保此时的设备大于%d台,确认后请按任意键继续",EXPMAXNUM);
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_DUMO))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	sleep(10);//给一定的扫描时间
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult1,2,&snum1))!=NDK_OK||snum1!=2)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, snum1);
		GOTOERR;	
	}
	if((ret=NDK_BTMasterStopScan())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}

	//case9:开始扫描设备后，再继续增加设备，获取扫描结果应该能及时发现新增的设备
	cls_show_msg("请打开1台名为\"6bt\"的蓝牙从设备，打开后请按任意键继续");
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_DUMO))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	sleep(10);//给一定的扫描时间
	memset(sresult1,0,sizeof(sresult1));
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult1,50,&snum1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	for(i=0;i<snum1;i++)
	{
		if(!strcmp(sresult1[i].sBtName,"6bt"))
			break;
	}
	if(i==snum1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case9:关闭某台设备后,如果没有重新扫描则应该仍然可以扫描到该关闭的设备
	cls_show_msg("请关闭名为\"6bt\"的设备，关闭后请按任意键继续");
	sleep(10);//给一定的扫描时间
	memset(sresult1,0,sizeof(sresult1));
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult1,50,&snum2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	for(i=0;i<snum2;i++)
	{
		if(!strcmp(sresult1[i].sBtName,"6bt"))
				break;
	}
	if(i==snum1)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//扫描结束,停止扫描
	if((ret=NDK_BTMasterStopScan())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}

	//case10:关闭某台设备后,重新发起扫描,应该扫描不到被关闭的设备
	cls_show_msg("请关闭名为\"6bt\"的设备，关闭后请按任意键继续");
	if((ret=NDK_BTMasterScan(BT_DEVICE_TYPE_DUMO))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	sleep(10);//给一定的扫描时间
	memset(sresult1,0,sizeof(sresult1));
	if((ret=NDK_BTMasterGetScanResults(NULL,sresult1,50,&snum2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;	
	}
	for(i=0;i<snum2;i++)
	{
		if(!strcmp(sresult1[i].sBtName,"6bt"))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}
	}
	if((ret=NDK_BTMasterStopScan())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;	
	}

	//测试结束
	send_result("%s测试通过", TESTAPI); 
ERR:
	NDK_BTMasterStopScan();
	NDK_PortClose(PORT_NUM_BT);
	return;
}

