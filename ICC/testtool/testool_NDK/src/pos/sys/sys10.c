/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys10.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_SysGoSuspend接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysGoSuspend"

#define	MAXWAITTIME 30
#if defined SP600G||defined ME51//SP600G 只支持SAM2
#define TESTSAMTYPE ICTYPE_SAM2
#else
#define TESTSAMTYPE ICTYPE_SAM1
#endif

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
*			  		      linwei	        20130121	 	created
						jiangym		 20140821		modify
*****************************************************************/
#if !K21_ENABLE  //K21平台休眠完全由应用自己控制，NDK_SysGoSuspend函数调用了就一定会进休眠，所以不支持工作状态下不休眠功能
//IC 与SAM上电过程中不能进休眠,以及二者嵌套
static int ic_sam_suspend(void)
{
	int ret = - 1, buflen = 0, buflen1 = 0;
	int nKeyin;
	uchar buf[256]={0}, buf1[256]={0};
	
	//case5:IC/SAM卡操作过程中(上电后)GotoSleep无效
	//IC卡测试
	cls_show_msg("请插入IC1卡,任意键继续测试(需在自动休眠前)");
	memset(buf,0,sizeof(buf));
	if (NDK_IccPowerUp(ICTYPE_IC,buf,&buflen)!=NDK_OK)
	{
		send_result("line %d: IC1上电失败", __LINE__);
		return FAIL;  
	}
	if((ret = NDK_SysGoSuspend())!=NDK_ERR_ICCARD_BUSY/*NDK_ERR*/)//根据应用需求休眠失败返回IC卡状态忙20140613 jiangym modify
	{
		send_result("line %d:%s测试失败%d", __LINE__, TESTAPI,ret);
		NDK_IccPowerDown(ICTYPE_IC);
		return FAIL;
	} 
	if(NDK_IccPowerDown(ICTYPE_IC)!=NDK_OK)
	{
		send_result("line %d: IC1下电失败", __LINE__);
		return FAIL;
	}	
	if(cls_show_msg("插入IC1卡后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	//下电后，且插卡情况下能够进入休眠20140613  jiangym add
	cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	NDK_SysDelay(5);
	nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	//唤醒后能正常工作20140613  jiangym add
	memset(buf,0,sizeof(buf));
	if (NDK_IccPowerUp(ICTYPE_IC,buf,&buflen)!=NDK_OK)
	{
		send_result("line %d: IC1上电失败", __LINE__);
		return FAIL;
	}
	if(NDK_IccPowerDown(ICTYPE_IC)!=NDK_OK)
	{
		send_result("line %d: IC1下电失败", __LINE__);
		return FAIL;
	}	
	
#if SAM1_ENABLE
	//sam卡测试
	cls_show_msg("请插入SAM1卡,任意键继续测试(需在自动休眠前)");
	memset(buf,0,sizeof(buf));
	if (NDK_IccPowerUp(TESTSAMTYPE,buf,&buflen)!=NDK_OK)
	{
		send_result("line %d: SAM1上电失败", __LINE__);
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_ICCARD_BUSY/*NDK_ERR*/)//根据应用需求休眠失败返回IC卡状态忙20140613 jiangym modify
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_IccPowerDown(TESTSAMTYPE);
		return FAIL;
	} 
	if(NDK_IccPowerDown(TESTSAMTYPE)!=NDK_OK)
	{
		send_result("line %d: SAM1下电失败", __LINE__);
		return FAIL;
	}	
	if(cls_show_msg("插入SAM1卡后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	//下电后能够进入休眠20140616  jiangym add
	cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	NDK_SysDelay(5);
	nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	//唤醒后能正常工作20140616  jiangym add
	memset(buf,0,sizeof(buf));
	if (NDK_IccPowerUp(TESTSAMTYPE,buf,&buflen)!=NDK_OK)
	{
		send_result("line %d: SAM1上电失败", __LINE__);
		return FAIL;
	}
	if(NDK_IccPowerDown(TESTSAMTYPE)!=NDK_OK)
	{
		send_result("line %d: SAM1下电失败", __LINE__);
		return FAIL;
	}

	//case9:打开两个设备IC/SAM,关闭一个设备后 还有一个设备仍然在忙，还是不应该进入休眠都关闭后就能进入休眠
	cls_show_msg("请确保已插入IC1与SAM1卡,任意键继续测试");
	memset(buf,0,sizeof(buf));
	if (NDK_IccPowerUp(ICTYPE_IC,buf,&buflen)!=NDK_OK)
	{
		send_result("line %d: IC1上电失败", __LINE__);
		return FAIL;
	}
	memset(buf1,0,sizeof(buf1));
	if (NDK_IccPowerUp(TESTSAMTYPE,buf1,&buflen1)!=NDK_OK)
	{
		send_result("line %d: SAM1上电失败", __LINE__);
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_ICCARD_BUSY)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_IccPowerDown(ICTYPE_IC);
		NDK_IccPowerDown(TESTSAMTYPE);
		return FAIL;
	} 
	if(NDK_IccPowerDown(TESTSAMTYPE)!=NDK_OK)
	{
		send_result("line %d: SAM1下电失败", __LINE__);
		NDK_IccPowerDown(ICTYPE_IC);
		return FAIL;
	}	
	if(NDK_SysGoSuspend()!=NDK_ERR_ICCARD_BUSY)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_IccPowerDown(ICTYPE_IC);
		return FAIL;
	} 
	if(NDK_IccPowerDown(ICTYPE_IC)!=NDK_OK)
	{
		send_result("line %d: IC1下电失败", __LINE__);
		return FAIL;
	}	
	if(cls_show_msg("插入卡后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	NDK_SysDelay(5);
	nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
#endif
	return SUCC;
}

//打印与磁卡的嵌套,在磁卡打开情况下打印中不能进休眠
static int prn_mag_suspend(void)
{
	int ret = -1, nBusy = FALSE, nErrorCode = 0;
	int nKeyin;
	EM_PRN_STATUS emStatus;
	time_t diff = 0, oldtime = 0;
	char pszTk1[MAXTRACKLEN] = {0}, pszTk2[MAXTRACKLEN] = {0}, pszTk3[MAXTRACKLEN] = {0};
	
	//先打开磁卡进行嵌套测试
	if((ret=NDK_MagOpen())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
#if PRN_ENABLE
	//case4:测试休眠能否打断打印
	cls_show_msg("请安装好打印纸,任意键继续(需在自动休眠前)");
	if (NDK_OK!=NDK_PrnInit(1))
	{
		send_result("line %d: 打印机初始化失败", __LINE__);
		goto ERR;
	}
	
	while(1)
	{
		if((ret=NDK_PrnGetStatus(&emStatus))!=NDK_OK )
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI,ret);
			goto ERR;
		}
		if(emStatus != PRN_STATUS_BUSY)
			show_stopwatch(DISABLE, 0);
		
		switch(emStatus)
		{
		case PRN_STATUS_BUSY:
			if(NDK_SysGoSuspend()!=NDK_ERR_PRN_BUSY/*NDK_ERR*/)//跟郑凯确认在不能进休眠的情况下，返回失败
			//根据应用需求休眠失败返回打印忙20140613 jiangym modify
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ERR;
			} 
			if(nBusy==FALSE)
			{
				oldtime = time(NULL);//从第一次收到忙状态开始计时
				nBusy = TRUE;
			}
			if((diff=time(NULL)-oldtime)>MAXWAITTIME)
			{
				send_result("line %d:打印机处于忙状态时间过长", __LINE__);
				goto ERR;
			}
			show_stopwatch(ENABLE, diff);
			if(NDK_PrnStr("休眠之后>")!=NDK_OK)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ERR;
			}		
			NDK_PrnStr("\n---打印完毕---\n");	
			NDK_PrnStart();
			if(cls_show_msg("打印过程是否进入过休眠\n[ENTER]是  [其他]否")==ENTER)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ERR;
			}
			goto END;
			show_stopwatch(ENABLE, diff);
			continue;
			break;

		case PRN_STATUS_OK:
			nBusy=FALSE;
			NDK_PrnSetFont(PRN_HZ_FONT_24x24,PRN_ZM_FONT_8x16);
			if(NDK_PrnStr("休眠之前>\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n")!=NDK_OK)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				goto ERR;
			}		
			//NDK_PrnStart();
			
			break;
		case PRN_STATUS_NOPAPER:
			cls_printf("打印机缺纸...装纸后,任意键继续,ESC放弃");
			lib_kbflush(); //清KB BUF
			while ((nKeyin=lib_kbhit())==0)
			{
				NDK_SysBeep();
				//正常装纸
				NDK_PrnFeedByPixel(10);
				sleep(2);
			}
			if (nKeyin==ESC)
			{
				send_result("line %d:打印机缺纸,用户放弃", __LINE__);
				goto ERR;
			}
			NDK_PrnStart();
			cls_printf("测试%s...", TESTAPI);
			break;
		case PRN_STATUS_OVERHEAT:
			send_result("line %d: 打印机过热", __LINE__);
			goto ERR;
			break;
		case PRN_STATUS_VOLERR:
			send_result("line %d: 打印机电源故障", __LINE__);
			goto ERR;
			break;
		default:
			//未知状态，判为错
			send_result("line %d: UNKNOWN STATE(tpstatus=%d)", __LINE__, emStatus);
			goto ERR;
			break;
		}	
	}
END:	
#endif
	cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	NDK_SysDelay(5);//0.5s延时
	nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	NDK_MagReset();
	cls_show_msg("请刷卡,后按任意键继续(需在自动休眠前)");
	memset(pszTk1,0,sizeof(pszTk1));
	memset(pszTk2,0,sizeof(pszTk2));
	memset(pszTk3,0,sizeof(pszTk3));
	if((ret=NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &nErrorCode))!=NDK_OK)	
	{
		send_result("line %d:读卡失败(%d)", __LINE__, ret);
		goto ERR;
	}
	//显示磁道信息
	if((ret=check_stripe_result_tk1(pszTk1))!=0x00)
	{
		send_result("line %d:读卡失败(0x%x)", __LINE__, ret);
		goto ERR;
	}
	if((ret=check_stripe_result_tk2(pszTk2))!=0x00)
	{
		send_result("line %d:读卡失败(0x%x)", __LINE__, ret);
		goto ERR;
	}
	if((ret=check_stripe_result_tk3(pszTk3))!=0x00)
	{
		send_result("line %d:读卡失败(0x%x)", __LINE__, ret);
		goto ERR;
	}
	cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}
	NDK_SysDelay(5);//0.5s延时
	nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	NDK_MagClose();	
	return SUCC;
ERR:
	NDK_MagClose();
	return FAIL;
}
//射频与usb、U盘设备打开情况下不能进休眠,以及射频与USB的嵌套
static int rfid_usb_suspend(void)
{
	int ret = -1,flag = 0, pnDatalen = 0;
	int nKeyin, i = 0;
	time_t diff = 0, oldtime = 0;
	uchar ucPicctype = 0, psPicctype = 0x00, psDatabuf[256] = {0}, UID[LEN_UID]={0},psSakbuf[10]={0};
#if 0//ANDIROD_ENABLE
	iso15693ProximityCard_t cards[20];
	iso15693PiccMemoryBlock_t r_memBlock;
	uint cardcount = 0, maxcards = 20;
	uchar ucBlocknum = 0;
	iso15693PiccSystemInformation_t sysinfo;
#endif
	//case6:测试射频卡(A.B.M)操作过程中(寻卡成功后,读写)GotoSleep无效,
	cls_show_msg("保证感应区不放置任何卡,任意键继续");
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
	{
		send_result("line %d:应该检测到无卡", __LINE__, ret);
		return FAIL;
	}	
	if((ret=NDK_RfidPiccDetect(&ucPicctype))!=NDK_ERR_RFID_NOCARD)
	{
		send_result("line %d:寻卡应该失败(%d)", __LINE__, ret);
		return FAIL;
	}	
	cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
	if(NDK_SysGoSuspend()!=NDK_OK)//寻卡失败后会进入休眠20140613 jiangym add
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	} 
	NDK_SysDelay(5);
	nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	for(i=0;i<2;i++)//增加B 卡的操作寻到卡就不会进入休眠 NDK5.0
	{
		cls_show_msg("请在感应区放置1张标准%s卡,任意键继续(需在自动休眠前)",i==0?"A":"B");
		if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}	
		oldtime = time(NULL);
		while((diff = time(NULL)-oldtime)<10)  //设置10秒寻卡时间
		{
			if((ret=NDK_RfidPiccDetect(&ucPicctype))==NDK_OK)
			{
				flag=1;
				break;
			}	
		}
		if(flag != 1)
		{
			send_result("line %d:寻卡失败", __LINE__);
			NDK_RfidPiccDeactivate(0);//关闭场强
			return FAIL;
		}
		if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY/*NDK_ERR*/)//寻卡成功后休眠失败返回射频卡状态忙20140616 jiangym modify
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			NDK_RfidPiccDeactivate(0);//关闭场强
			return FAIL;
		} 
		if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=(i==0?RFCARD_TYPE_A:RFCARD_TYPE_B))
		{
			send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
			NDK_RfidPiccDeactivate(0);//关闭场强
			return FAIL;
		}
		if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY/*NDK_ERR*/)//根据应用需求休眠失败返回射频卡状态忙20140613 jiangym modify
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			NDK_RfidPiccDeactivate(0);//关闭场强
			return FAIL;
		}
		if(NDK_RfidPiccDeactivate(0)!=NDK_OK)
		{
			send_result("line %d:下电失败", __LINE__);
			return FAIL;
		}	
		if(cls_show_msg("放置%s卡后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否",i==0?"A":"B")==ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
	}
	
	//对M卡的测试寻到卡就不会进入休眠 NDK5.0
	cls_show_msg("请在感应区放置一张Mifare-1卡,任意键继续...");
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_A))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}	
	oldtime = time(NULL);
	while((diff = time(NULL)-oldtime)<10)  //设置10秒寻卡时间
	{
		if((ret=NDK_M1Request(REQA, &pnDatalen, psDatabuf))==NDK_OK)
		{
			flag=1;
			break;
		}	
	}
	if(flag != 1)
	{
		send_result("line %d:寻卡失败", __LINE__);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY)//寻卡成功后休眠失败返回射频卡状态忙
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	} 
	if((ret=NDK_M1Anti(&pnDatalen, UID))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	} 
	if(((ret=NDK_M1Select(pnDatalen, UID, psSakbuf))!=NDK_OK)||((psSakbuf[0]&0x20)!=0x00))//增加psSakbuf[0] bit5位判断是否为M1卡
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY)//根据应用需求休眠失败返回射频卡状态忙
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	}
	if((ret=NDK_M1ExternalAuthen(pnDatalen, UID, AUTHKEY_TYPE_B, AUTHKEY, 0x01))!=NDK_OK)	
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY)//根据应用需求休眠失败返回射频卡状态忙
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	}
	if(NDK_RfidPiccDeactivate(0)!=NDK_OK)
	{
		send_result("line %d:下电失败", __LINE__);
		return FAIL;
	}	
	if(cls_show_msg("放置M1卡后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}

#if 0//felica卡暂时未支持
	//对felica卡的测试寻到卡就不会进入休眠
	cls_show_msg("请在感应区放置Felica卡，按任意键继续...");
	oldtime = time(NULL);
	while((diff = time(NULL)-oldtime)<10)  //设置10秒寻卡时间
	{
		if((ret=NDK_RfidFelicaPoll(UID, &pnDatalen))==NDK_OK)
		{
			flag=1;
			break;
		}	
	}
	if(flag != 1)
	{
		send_result("line %d:寻卡失败", __LINE__);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	}
	//下电
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if(cls_show_msg("放置felica卡后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
#endif

#if 0 //还未有产品导入,为了不影响其他产品编译先关闭,后续导入时开启
	//对M0卡的测试寻到卡就不会进入休眠 
	cls_show_msg("请在感应区放置一张Mifare-0卡,任意键继续...");
	oldtime = time(NULL);
	while((diff = time(NULL)-oldtime)<10)  //设置10秒寻卡时间
	{
		if((ret=NDK_MifareActive(REQA, UID, &pnDatalen,psSakbuf))==NDK_OK)
		{
			flag=1;
			break;
		}	
	}
	if(flag != 1)
	{
		send_result("line %d:寻卡失败", __LINE__);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY)//寻卡成功后休眠失败返回射频卡状态忙
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	} 
	if((ret=NDK_M0Authen(AUTHKEY_M0))!=NDK_OK)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY)//根据应用需求休眠失败返回射频卡状态忙
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	}
	if(NDK_RfidPiccDeactivate(0)!=NDK_OK)
	{
		send_result("line %d:下电失败", __LINE__);
		return FAIL;
	}	
	if(cls_show_msg("放置M0卡后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
#endif
#if 0//ANDIROD_ENABLE 暂时适用于高端产品,高端产品此用例未测后续中端若支持把开关打开
	//15693卡片寻到卡就不会进入休眠
	NDK_ISO15693_init();
	cls_show_msg("请在感应区放置一张15693卡,任意键继续...");
	oldtime = time(NULL);
	while((diff = time(NULL)-oldtime)<10)  //设置10秒寻卡时间
	{
		if((ret=NDK_ISO15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,&cardcount,cards))==NDK_OK)
		{
			flag=1;
			break;
		}	
	}
	if(flag != 1)
	{
		send_result("line %d:寻卡失败", __LINE__);
		NDK_ISO15693_Deinit();//关闭场强
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY)//寻卡成功后休眠失败返回射频卡状态忙
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_ISO15693_Deinit();//关闭场强
		return FAIL;
	} 
	if((ret=NDK_iso15693SelectPicc(cards[0])) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_ISO15693_Deinit();//关闭场强
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY) 
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_ISO15693_Deinit();//关闭场强
		return FAIL;
	}
	if((ret=NDK_iso15693GetPicc_SystemInfo(cards[0],&sysinfo)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_ISO15693_Deinit();//关闭场强
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY) 
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_ISO15693_Deinit();//关闭场强
		return FAIL;
	}
	if(NDK_ISO15693_Deinit()!=NDK_OK)
	{
		send_result("line %d:下电失败", __LINE__);
		return FAIL;
	}	
	if(cls_show_msg("放置15693卡后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
#endif

#if USB_ENABLE
	//case 8:补充用例验证U盘打开情况下不进休眠
	//根据应用需求休眠失败返回U盘状态忙20140617 jiangym add
	cls_show_msg("请插入U盘,任意键继续(需在自动休眠前)");
	if((ret=NDK_DiskOpen(UDISK,10))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_USB_BUSY)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_DiskClose(UDISK);
		return FAIL;
	} 
	//关闭设备
	if((ret=NDK_DiskClose(UDISK))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if(cls_show_msg("插入U盘后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
#endif
	//case 7: usb打开后不该进入休眠
	cls_show_msg("请将POS和PC通过USB线连接,任意键继续(需在自动休眠前)");
	if ((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_USB_BUSY/*NDK_ERR*/)//根据应用需求休眠失败返回USB状态忙20140613 jiangym modify
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_PortClose(PORT_NUM_USB);
		return FAIL;
	} 
	NDK_PortClose(PORT_NUM_USB);
	if(cls_show_msg("连接USB线后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}

	//case9:打开两个设备usb与射频,关闭一个设备后 还有一个设备仍然在忙，还是不应该进入休眠都关闭后就能进入休眠
	cls_show_msg("请将POS和PC通过USB线连接,任意键继续");
	if ((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	cls_show_msg("请在感应区放置1张标准A卡,任意键继续");
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_A))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_PortClose(PORT_NUM_USB);
		return FAIL;
	}	
	oldtime = time(NULL);
	while((diff = time(NULL)-oldtime)<10)  //设置10秒寻卡时间
	{
		if((ret=NDK_RfidPiccDetect(&ucPicctype))==NDK_OK)
		{
			flag=1;
			break;
		}	
	}
	if(flag != 1)
	{
		send_result("line %d:寻卡失败", __LINE__);
		goto ERR1;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR1;
	} 
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=RFCARD_TYPE_A)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		goto ERR1;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR1;
	}
	if(NDK_RfidPiccDeactivate(0)!=NDK_OK)
	{
		send_result("line %d:下电失败", __LINE__);
		goto ERR1;
	}
	if(NDK_SysGoSuspend()!=NDK_ERR_USB_BUSY)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		NDK_PortClose(PORT_NUM_USB);
		return FAIL;
	} 
	NDK_PortClose(PORT_NUM_USB);
	if(cls_show_msg("连接USB线后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	NDK_SysDelay(5);
	nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	return SUCC;
ERR1:
	NDK_RfidPiccDeactivate(0);
	NDK_PortClose(PORT_NUM_USB);
	return FAIL;
}

#if BT_ENABLE
//蓝牙模块新增工作过程中休眠返回蓝牙忙状态 20150128 linwl
static int bt_suspend(void)
{
	int ret = -1,recvLen = 0;
	char sendBuf[12]={0},recvBuf[12]={0};

	conf_conn_BT();

	cls_show_msg("请打开BTFWATS工具下[手动测试],并使用手机蓝牙工具进行搜索配对连接,按任意键开始测试");	
	if((ret = NDK_PortOpen(PORT_NUM_BT,"115200,8,N,1"))!=NDK_OK)
	{
    	send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
    	goto END;
	}
	if((ret=lib_bt_pairing(1))!=NDK_OK)
	{
		send_result("line %d:%s测试配对失败(%d)", __LINE__, TESTAPI, ret);
		goto END;
	}
	//配对成功后马上进入休眠应该返回NDK_ERR_BT_BUSY
	if((ret=NDK_SysGoSuspend()) != NDK_ERR_BT_BUSY)
	{
		send_result("line %d:休眠失败(%d)", __LINE__, ret);
		goto END;
	}
	NDK_ScrPrintf(",按任意键继续");
	NDK_ScrRefresh();
	NDK_KbGetCode(0, NULL);
	memset(sendBuf, 0x38, sizeof(sendBuf));
	if((ret=NDK_PortWrite(PORT_NUM_BT, sizeof(sendBuf),sendBuf))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto END;
	}
	if((ret=NDK_SysGoSuspend()) != NDK_ERR_BT_BUSY)
	{
		send_result("line %d:休眠失败(%d)", __LINE__, ret);
		goto END;
	}
	cls_show_msg("BleTest点开始回传或蓝牙串口发送接收到的数据后,任意键继续");
	if((ret = NDK_PortRead(PORT_NUM_BT,sizeof(sendBuf),recvBuf, MAXWAITTIME*1000,&recvLen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret:%d)", __LINE__, TESTAPI, ret);
		goto END;
	}
	if(recvLen!=sizeof(sendBuf))
	{
		send_result("line %d:%s测试失败(实际:%d,预期:%d)", __LINE__, TESTAPI, recvLen, sizeof(sendBuf));
		goto END;
	}
	if(memcmp(sendBuf, recvBuf, recvLen))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto END;
	}
	cls_show_msg("子用例测试通过,任意键继续");
	return SUCC;
END:
	NDK_BTDisconnect();
	NDK_PortClose(PORT_NUM_BT);
	return FAIL;
}
#else
static int bt_suspend(void){return SUCC;}
#endif
#endif

#if defined  ME11||defined ME15||defined ME20||defined ME15C //需要用带有物理串口的机器测，同时代码需要修改为物理串口上送结果
void sys10(void)
{ 
	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	int ret = -1;
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI);
#if defined ME11		
	cls_printf("红灯长亮5s后进入休眠,休眠后LED灯不灭,使用手机app唤醒后红灯闪3s,绿灯闪烁3s,人工判断是否通过");
#else
	cls_printf("红灯长亮5s后进入休眠,休眠后LED灯不灭,使用手机蓝牙工具连接\"ME15sys10\"唤醒(BM77模块需发送数据如:12345后才能唤醒)或者使用[电源键]唤醒,唤醒后红灯闪3s,蓝灯闪烁3s,人工判断是否通过");
#endif
	if((ret=NDK_LedStatus(LED_RFID_RED_ON)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(5);
#if defined ME15||defined ME20||defined ME15C||defined ME15CHX||defined ME15B
	NDK_PortOpen(PORT_NUM_BT, NULL);
	NDK_BTEnterCommand();
	NDK_BTSetLocalName("ME15sys10");
	NDK_BTExitCommand();
#endif
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(1);//休眠函数需要时间生效
	//红灯闪3s
	cls_printf("蓝牙唤醒成功,红灯闪烁3秒后灭掉");
	if((ret=NDK_LedStatus(LED_RFID_RED_FLICK)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(3);
	NDK_LedStatus(LED_RFID_RED_OFF);
#if defined ME11
	//"绿灯将闪烁3s"
	cls_printf("绿灯闪烁3秒后灭掉");
	if((ret=NDK_LedStatus(LED_RFID_GREEN_FLICK)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(3);
	NDK_LedStatus(LED_RFID_GREEN_OFF);
#else
	//"蓝灯将闪烁3s"
	cls_printf("蓝灯闪烁3秒后灭掉,用例退出前亮起");
	if((ret=NDK_LedStatus(LED_RFID_BLUE_FLICK)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(3);
	NDK_LedStatus(LED_RFID_BLUE_OFF);
#endif
	//测试结束
	send_result("%s测试通过", TESTAPI);
	NDK_LedStatus(LED_RFID_BLUE_FLICK);//蓝灯表示应用在运行
	return;
}
#else
void sys10(void)
{
	if(auto_flag==1)	
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	//以下为局部变量
	int ret = -1;
	int nKeyin;
#if defined ME15CHX||defined ME15B||defined ME66
	int handle;
#endif	
	
	/*process body*/
#if !K21_ENABLE //K21设置菜单中没有休眠模式和时间测试
	//[设置]中的休眠时间参数不影响GotoSleep的调用,只有"深""浅"模式会影响,并且GotoSleep不受开关影响,都会进休眠
	if(ENTER!=cls_show_msg("请在管理中分别设置休眠模式为:关,开(深),开(浅)3种!后按ENTER开始测试"))
	{
		send_result("line %d:%s测试中断", __LINE__, TESTAPI);
		return;
	}
#endif
	cls_show_msg1(2, "测试%s(将按[设置]中的模式进入休眠)...", TESTAPI);

	//case1: NDK_SysSetSuspend设置允许自动进入休眠，立即进入休眠 
	cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
#if !K21_ENABLE
	NDK_SysSetSuspend(1);
#endif
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
#if defined ME15CHX||defined ME15B||defined ME66
	lib_kbflush();//由于是从串口读入输入,所以会被后面的判断语句接收,要先清一下
#endif
	nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败:0x%2x", __LINE__, TESTAPI, nKeyin);
		RETURN;
	}

#if !(defined SP60||defined ME31S||defined ME31SLS||defined SP33||defined SP600||defined SP60G||defined SP33G||defined SP600G||defined ME51||CPU5830X_ENABLE||defined SP930G||defined SP930PG||defined SP630PG)
	//case2:按键唤醒休眠时，唤醒按键不应被应用获取到(应丢弃)
	cls_show_msg("按任意键将立即进入休眠,休眠后按键唤醒");
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(1);
#if defined ME15CHX||defined ME15B||defined ME66
	lib_kbflush();//由于是从串口读入输入,所以会被后面的判断语句接收,要先清一下
#endif
	nKeyin = cls_show_msg1(MAXWAITTIME, "唤醒后马上显示本界面,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
#endif

	//case3: NDK_SysSetSuspend设置不允许自动进入休眠，立即进入休眠 
	cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");//测试人员需要注意如果出现本界面按键无法进入休眠，说明存在丢键，不必现
#if !K21_ENABLE
	NDK_SysSetSuspend(0);
#endif
	if((ret=NDK_SysGoSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	NDK_SysDelay(5);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
#if defined ME15CHX||defined ME15B||defined ME66
	lib_kbflush();//由于是从串口读入输入,所以会被后面的判断语句接收,要先清一下
#endif
	nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR2;
	}
#if !K21_ENABLE
	NDK_SysSetSuspend(1);//前面有关闭自动进入休眠开关，测试完要打开，避免该用例执行完后其他程序无法进入休眠 20140509 linwl
#endif
	
#if !K21_ENABLE  //K21平台休眠完全由应用自己控制，NDK_SysGoSuspend函数调用了就一定会进休眠，所以不支持工作状态下不休眠功能
	cls_show_msg("请先准备好IC卡、SAM卡、射频卡、磁卡和U盘,任意键继续");
	if((ret=ic_sam_suspend()) != SUCC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if((ret=prn_mag_suspend()) != SUCC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if((ret=rfid_usb_suspend()) != SUCC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if((ret=bt_suspend()) != SUCC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
#endif
	//测试结束
	send_result("%s测试通过", TESTAPI);
	goto ERR2;
ERR2:
#if !K21_ENABLE
	NDK_SysSetSuspend(1);//测试完打开休眠开关，避免其他程序无法进入休眠 20140509 linwl
#endif

#if defined ME15CHX||defined ME15B
	handle = NDK_LedOpen("blue");
	NDK_LedOn(handle);
#endif
	return;
}
#endif

