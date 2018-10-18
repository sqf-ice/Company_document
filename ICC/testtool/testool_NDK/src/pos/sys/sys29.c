/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys10.c
* Author 			: linwei
* version			: 
* DATE				: 20130121
* directory 		: 
* description		: 测试NDK_SysReadyToSuspend,NDK_SysGoSuspend_Extern接口
* related document	: NDK.chm
*
************************************************************************/
#include "mode.h"
#include <linux/delay.h>

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_SysReadyToSuspend,NDK_SysGoSuspend_Extern"

#define	MAXWAITTIME 30

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
*						linwl		 20150713		modify
*****************************************************************/
//选择休眠唤醒的方式
//IC 与SAM上电过程中不能进休眠,以及二者嵌套
static int ic_sam_suspend(int flag_wakeup)
{
	int ret = - 1, buflen = 0, buflen1 = 0;
	int nKeyin;
	uchar buf[256]={0}, buf1[256]={0};
	
	//case5:IC/SAM卡操作过程中(上电后)GotoSleep无效
	//IC卡测试
	cls_show_msg("请插入IC1卡,任意键继续测试");
	memset(buf,0,sizeof(buf));
	if((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&buflen))!=NDK_OK)
	{
		send_result("line %d: IC1上电失败(%d)", __LINE__, ret);
		return FAIL;  
	}
	if((ret = NDK_SysReadyToSuspend())!=NDK_ERR_ICCARD_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_IccPowerDown(ICTYPE_IC);
		return FAIL;
	}
	if((ret = NDK_SysGoSuspend_Extern())!=NDK_ERR_ICCARD_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_IccPowerDown(ICTYPE_IC);
		return FAIL;
	}
	if((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{
		send_result("line %d: IC1下电失败(%d)", __LINE__, ret);
		return FAIL;
	}	
	if(cls_show_msg("插入IC1卡后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	//下电后，且插卡情况下能够进入休眠
	if(flag_wakeup==0)
	{
		cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
		if (nKeyin!=ENTER && nKeyin!=0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
	}
	else
	{
		cls_show_msg1(3, "3s后进入休眠,请等待5s后，将自动唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		if((ret=NDK_SysWakeUp()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if(cls_show_msg("已唤醒,没按任何按键也可休眠唤醒,正确[Enter],错误[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
	}
	//唤醒后能正常工作
	memset(buf,0,sizeof(buf));
	if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&buflen))!=NDK_OK)
	{
		send_result("line %d: IC1上电失败(%d)", __LINE__, ret);
		return FAIL;
	}
	if((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{
		send_result("line %d: IC1下电失败(%d)", __LINE__, ret);
		return FAIL;
	}	

	//sam卡测试
	cls_show_msg("请插入SAM1卡,任意键继续测试(需在自动休眠前)");
	memset(buf,0,sizeof(buf));
	if ((ret=NDK_IccPowerUp(ICTYPE_SAM1,buf,&buflen))!=NDK_OK)
	{
		send_result("line %d: SAM1上电失败(%d)", __LINE__, ret);
		return FAIL;
	}
	if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_ICCARD_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_IccPowerDown(ICTYPE_SAM1);
		return FAIL;
	} 
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_ICCARD_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_IccPowerDown(ICTYPE_SAM1);
		return FAIL;
	} 
	if((ret=NDK_IccPowerDown(ICTYPE_SAM1))!=NDK_OK)
	{
		send_result("line %d: SAM1下电失败(%d)", __LINE__, ret);
		return FAIL;
	}	
	if(cls_show_msg("插入SAM1卡后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	//下电后能够进入休眠
	if(flag_wakeup==0)
	{
		cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
		if (nKeyin!=ENTER && nKeyin!=0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
	}
	else
	{
		cls_show_msg1(3, "3s后进入休眠,请等待5s后，将自动唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		if((ret=NDK_SysWakeUp()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if(cls_show_msg("已唤醒,没按任何按键也可休眠唤醒,正确[Enter],错误[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
	}
	//唤醒后能正常工作
	memset(buf,0,sizeof(buf));
	if ((ret=NDK_IccPowerUp(ICTYPE_SAM1,buf,&buflen))!=NDK_OK)
	{
		send_result("line %d: SAM1上电失败(%d)", __LINE__, ret);
		return FAIL;
	}
	if((ret=NDK_IccPowerDown(ICTYPE_SAM1))!=NDK_OK)
	{
		send_result("line %d: SAM1下电失败(%d)", __LINE__, ret);
		return FAIL;
	}

	//case9:打开两个设备IC/SAM,关闭一个设备后 还有一个设备仍然在忙，还是不应该进入休眠都关闭后就能进入休眠
	cls_show_msg("请确保已插入IC1与SAM1卡,任意键继续测试");
	memset(buf,0,sizeof(buf));
	if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&buflen))!=NDK_OK)
	{
		send_result("line %d: IC1上电失败(%d)", __LINE__, ret);
		return FAIL;
	}
	memset(buf1,0,sizeof(buf1));
	if ((ret=NDK_IccPowerUp(ICTYPE_SAM1,buf1,&buflen1))!=NDK_OK)
	{
		send_result("line %d: SAM1上电失败(%d)", __LINE__, ret);
		return FAIL;
	}
	if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_ICCARD_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_IccPowerDown(ICTYPE_IC);
		NDK_IccPowerDown(ICTYPE_SAM1);
		return FAIL;
	} 
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_ICCARD_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_IccPowerDown(ICTYPE_IC);
		NDK_IccPowerDown(ICTYPE_SAM1);
		return FAIL;
	} 
	if((ret=NDK_IccPowerDown(ICTYPE_SAM1))!=NDK_OK)
	{
		send_result("line %d: SAM1下电失败(%d)", __LINE__, ret);
		NDK_IccPowerDown(ICTYPE_IC);
		return FAIL;
	}	
	if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_ICCARD_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_IccPowerDown(ICTYPE_IC);
		return FAIL;
	} 
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_ICCARD_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_IccPowerDown(ICTYPE_IC);
		return FAIL;
	} 
	if((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{
		send_result("line %d: IC1下电失败(%d)", __LINE__, ret);
		return FAIL;
	}	
	if(cls_show_msg("插入卡后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	if(flag_wakeup==0)
	{
		cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
		if (nKeyin!=ENTER && nKeyin!=0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
	}
	else
	{
		cls_show_msg1(3, "3s后进入休眠,请等待5s后，将自动唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		if((ret=NDK_SysWakeUp()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if(cls_show_msg("已唤醒,没按任何按键也可休眠唤醒,正确[Enter],错误[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
	}
	return SUCC;
}

//打印与磁卡的嵌套,在磁卡打开情况下打印中不能进休眠
static int prn_mag_suspend(int flag_wakeup)
{
	int ret = -1, nBusy = FALSE, nErrorCode = 0;
	int nKeyin;
	EM_PRN_STATUS emStatus;
	time_t diff = 0, oldtime = 0;
	char pszTk1[MAXTRACKLEN] = {0}, pszTk2[MAXTRACKLEN] = {0}, pszTk3[MAXTRACKLEN] = {0};
	
	//先打开磁卡进行嵌套测试
	if((ret=NDK_MagOpen())!=NDK_OK)
	{
		cls_show_msg("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
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
			send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,ret);
			goto ERR;
		}
		if(emStatus != PRN_STATUS_BUSY)
			show_stopwatch(DISABLE, 0);
		
		switch(emStatus)
		{
		case PRN_STATUS_BUSY:
			if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_PRN_BUSY)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				goto ERR;
			} 
			if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_PRN_BUSY)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
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
			if((ret=NDK_PrnStr("休眠之后>"))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
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
			if((ret=NDK_PrnStr("休眠之前>\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20"))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
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
	if(flag_wakeup==0)
	{
		cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
		if (nKeyin!=ENTER && nKeyin!=0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			goto ERR;
		}
	}
	else
	{
		cls_show_msg1(3, "3s后进入休眠,请等待5s后，将自动唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		if((ret=NDK_SysWakeUp()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if(cls_show_msg("已唤醒,没按任何按键也可休眠唤醒,正确[Enter],错误[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			goto ERR;
		}
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
	if(flag_wakeup==0)
	{
		cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
		if (nKeyin!=ENTER && nKeyin!=0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
	}
	else
	{
		cls_show_msg1(3, "3s后进入休眠,请等待5s后，将自动唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		if((ret=NDK_SysWakeUp()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if(cls_show_msg("已唤醒,没按任何按键也可休眠唤醒,正确[Enter],错误[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
	}
	NDK_MagClose();	
	return SUCC;
ERR:
	NDK_MagClose();
	return FAIL;
}

//射频与usb、U盘设备打开情况下不能进休眠,以及射频与USB的嵌套
static int rfid_tc_suspend(int flag_wakeup)
{
	int ret = -1,flag = 0, pnDatalen = 0;
	int nKeyin, i = 0;
	time_t diff = 0, oldtime = 0;
	uchar ucPicctype = 0, psPicctype = 0x00, psDatabuf[256] = {0}, UID[LEN_UID]={0},psSakbuf[10]={0};

	//case6:测试射频卡(A.B.M)操作过程中(寻卡成功后,读写)GotoSleep无效,
	cls_show_msg("保证感应区不放置任何卡,任意键继续");
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_AB))!=NDK_OK)
	{
		send_result("line %d:应该检测到无卡(%d)", __LINE__, ret);
		return FAIL;
	}	
	if((ret=NDK_RfidPiccDetect(&ucPicctype))!=NDK_ERR_RFID_NOCARD)
	{
		send_result("line %d:寻卡应该失败(%d)", __LINE__, ret);
		return FAIL;
	}	
	if(flag_wakeup==0)
	{
		cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
		if (nKeyin!=ENTER && nKeyin!=0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
	}
	else
	{
		cls_show_msg1(3, "3s后进入休眠,请等待5s后，将自动唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		if((ret=NDK_SysWakeUp()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if(cls_show_msg("已唤醒,没按任何按键也可休眠唤醒,正确[Enter],错误[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
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
		if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_RFID_BUSY)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			NDK_RfidPiccDeactivate(0);
			return FAIL;
		} 
		if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_RFID_BUSY)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			NDK_RfidPiccDeactivate(0);
			return FAIL;
		} 
		if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=(i==0?RFCARD_TYPE_A:RFCARD_TYPE_B))
		{
			send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
			NDK_RfidPiccDeactivate(0);
			return FAIL;
		}
		if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_RFID_BUSY)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			NDK_RfidPiccDeactivate(0);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_RFID_BUSY)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			NDK_RfidPiccDeactivate(0);
			return FAIL;
		}
		if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
		{
			send_result("line %d:下电失败(%d)", __LINE__, ret);
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
	if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_RFID_BUSY)//寻卡成功后休眠失败返回射频卡状态忙
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	} 
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_RFID_BUSY)//寻卡成功后休眠失败返回射频卡状态忙
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);//关闭场强
		return FAIL;
	} 
	if((ret=NDK_M1Anti(&pnDatalen, UID))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);
		return FAIL;
	}
	if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);
		return FAIL;
	} 
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);
		return FAIL;
	} 
	if(((ret=NDK_M1Select(pnDatalen, UID, psSakbuf))!=NDK_OK)||((psSakbuf[0]&0x20)!=0x00))//增加psSakbuf[0] bit5位判断是否为M1卡
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);
		return FAIL;
	}
	if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_RFID_BUSY)//根据应用需求休眠失败返回射频卡状态忙
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);
		return FAIL;
	}
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);
		return FAIL;
	}
	if((ret=NDK_M1ExternalAuthen(pnDatalen, UID, AUTHKEY_TYPE_B, AUTHKEY, 0x01))!=NDK_OK)	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);
		return FAIL;
	}
	if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);
		return FAIL;
	}
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_RfidPiccDeactivate(0);
		return FAIL;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:下电失败(%d)", __LINE__, ret);
		return FAIL;
	}	
	if(cls_show_msg("放置M1卡后到看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	
	//case 8:补充用例验证K21接管触屏情况下不进休眠
	cls_show_msg("任意键K21将接管触屏");
	if((ret=NDK_TSKbd_Ctrl(1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_TSKbd_Ctrl(0);
		return FAIL;
	}
	if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_PIN_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_TSKbd_Ctrl(0);
		return FAIL;
	}
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_PIN_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		NDK_TSKbd_Ctrl(0);
		return FAIL;
	} 
	//K21端释放触屏
	if((ret=NDK_TSKbd_Ctrl(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if(cls_show_msg("看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	
	//case9:打开两个设备触屏与射频,关闭一个设备后 还有一个设备仍然在忙，还是不应该进入休眠都关闭后就能进入休眠
	cls_show_msg("任意键K21将接管触屏");
	if((ret=NDK_TSKbd_Ctrl(1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	cls_show_msg1(20,"请在20s内在感应区放置1张标准A卡");
	if((ret=NDK_RfidPiccType(RFCARD_TYPE_A))!=NDK_OK)
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
		goto ERR1;
	}
	if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR1;
	} 
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR1;
	} 
	if((ret=NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf))!=NDK_OK||psPicctype!=RFCARD_TYPE_A)
	{
		send_result("line %d:%s测试失败(%d,0x%02x)", __LINE__, TESTAPI, ret, psPicctype);
		goto ERR1;
	}
	if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR1;
	}
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_RFID_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR1;
	}
	if((ret=NDK_RfidPiccDeactivate(0))!=NDK_OK)
	{
		send_result("line %d:下电失败(%d)", __LINE__, ret);
		goto ERR1;
	}
	if((ret=NDK_SysReadyToSuspend())!=NDK_ERR_PIN_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if((ret=NDK_SysGoSuspend_Extern())!=NDK_ERR_PIN_BUSY)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	} 
	//K21端释放触屏
	if((ret=NDK_TSKbd_Ctrl(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if(cls_show_msg("看到本信息前是否进入过休眠\n[ENTER]是,[其他]否")==ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		return FAIL;
	}
	if(flag_wakeup==0)
	{
		cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
		if (nKeyin!=ENTER && nKeyin!=0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
	}
	else
	{
		cls_show_msg1(3, "3s后进入休眠,请等待5s后，将自动唤醒");
		if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		//NDK_SysDelay(5);
		usleep(5000000);
		if((ret=NDK_SysWakeUp()) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return FAIL;
		}
		if(cls_show_msg("已唤醒,没按任何按键也可休眠唤醒,正确[Enter],错误[其他]")!=ENTER)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			return FAIL;
		}
	}
	return SUCC;
ERR1:
	NDK_RfidPiccDeactivate(0);
	return FAIL;
}

void sys29(void)
{
	//以下为局部变量
	int ret = -1, flag_wakeup=0;
	int nKeyin;
	
	if(auto_flag==1)
	{
		//send_result("line %d:%s无纯自动测试用例", __LINE__, TESTAPI);
		return;
	}
	/*process body*/
	//case1: NDK_SysSetSuspend设置允许自动进入休眠，立即进入休眠 
#if !KB_DISABLE	
	cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");
	NDK_SysSetSuspend(1);
	if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//NDK_SysDelay(5);
	usleep(5000000);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
	nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
#endif

	cls_show_msg1(3, "3s后进入休眠,请等待5s后，将自动唤醒");
	NDK_SysSetSuspend(1);
	if((ret=NDK_SysReadyToSuspend()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//NDK_SysDelay(5);
	usleep(5000000);
	if((ret=NDK_SysWakeUp()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(cls_show_msg("已唤醒,没按任何按键也可休眠唤醒,正确[Enter],错误[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case2:按键唤醒休眠时，唤醒按键不应被应用获取到(应丢弃)
#if !KB_DISABLE	
	cls_show_msg("按任意键将立即进入休眠,休眠后按键唤醒");
	if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "唤醒后马上显示本界面,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
#endif

	//case3: NDK_SysSetSuspend设置不允许自动进入休眠，立即进入休眠 
#if !KB_DISABLE	
	cls_show_msg("按任意键将立即进入休眠,若进入休眠,按任意键唤醒");//测试人员需要注意如果出现本界面按键无法进入休眠，说明存在丢键，不必现
	NDK_SysSetSuspend(0);
	if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	//NDK_SysDelay(5);
	usleep(5000000);//避免唤醒的电源键被当成 [其它]键被下面的语句识别到引起测试失败
	nKeyin = cls_show_msg1(MAXWAITTIME, "若立即进入休眠,[确认]通过,[其它]失败");
	if (nKeyin!=ENTER && nKeyin!=0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR2;
	}
	NDK_SysSetSuspend(1);//前面有关闭自动进入休眠开关，测试完要打开，避免该用例执行完后其他程序无法进入休眠 20140509 linwl
#endif

	cls_show_msg1(3, "3s后进入休眠,请等待5s后，将自动唤醒");
	NDK_SysSetSuspend(0);
	if((ret=NDK_SysGoSuspend_Extern()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//NDK_SysDelay(5);
	usleep(5000000);
	if((ret=NDK_SysWakeUp()) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(cls_show_msg("已唤醒,没按任何按键也可休眠唤醒,正确[Enter],错误[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	//按键唤醒休眠
#if !KB_DISABLE	
	cls_show_msg("请先准备好IC卡、SAM卡、射频卡、磁卡,任意键继续");
	if((ret=ic_sam_suspend(flag_wakeup)) != SUCC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if((ret=prn_mag_suspend(flag_wakeup)) != SUCC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if((ret=rfid_tc_suspend(flag_wakeup)) != SUCC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
#endif
	//调用函数唤醒休眠
	flag_wakeup=1;
	cls_show_msg("请先准备好IC卡、SAM卡、射频卡、磁卡,任意键继续");
	if((ret=ic_sam_suspend(flag_wakeup)) != SUCC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if((ret=prn_mag_suspend(flag_wakeup)) != SUCC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if((ret=rfid_tc_suspend(flag_wakeup)) != SUCC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	//测试结束
	send_result("%s测试通过", TESTAPI);
ERR2:
#if !K21_ENABLE
	NDK_SysSetSuspend(1);//测试完打开休眠开关，避免其他程序无法进入休眠 20140509 linwl
#endif
	return;
}

