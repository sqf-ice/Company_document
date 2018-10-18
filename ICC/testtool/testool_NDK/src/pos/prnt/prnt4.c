/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt4.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnInit接口功能
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
#define TESTAPI	"NDK_PrnInit"	
#define MAXWAITTIME 60 
#define MAXPRNSWITCH 1

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void prnt4(void)
* functional description 	: 打印初始化
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt4(void)
{
	//以下为局部变量
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	int nRet = 0;
	int  nFlag = 1;
#if !K21_ENABLE
	int nFirstRun=1;
#else
	int nFirstRun=0;
#endif
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	if(NDK_PrnInit(g_PrnSwitch) != NDK_OK)
	{
		send_result("line %d:%s测试失败",__LINE__,TESTAPI);
		goto prnt_error;	
	}
	
	while(1)
	{
		if((nRet=NDK_PrnGetStatus(&emStatus))!=NDK_OK )
		{
			send_result("line %d:%s测试失败%d",__LINE__,TESTAPI,nRet);
			goto prnt_error;
		}
		if(emStatus != PRN_STATUS_BUSY)
			show_stopwatch(DISABLE, 0);
		
		switch(emStatus)
		{
		case PRN_STATUS_BUSY:
			if(nBusy==FALSE)
			{
				nOldtime = time(NULL);//从第一次收到忙状态开始计时
				nBusy = TRUE;
			}
			if((nDiff=time(NULL)-nOldtime)>MAXWAITTIME)
			{
				send_result("line %d:打印机处于忙状态时间过长", __LINE__);
				goto prnt_error;
			}
			show_stopwatch(ENABLE, nDiff);
			if (nFlag==1)
			{
				cls_printf("测试%s中断打印", TESTAPI);
				if (NDK_OK!=NDK_PrnInit(1))
				{
					send_result("line %d: 打印机复位失败", __LINE__);
					goto prnt_error;
				}
				NDK_SysDelay(20);
				nFlag = 2;
			}
			else if(nFlag==2)
				nFlag = 3;
			continue;
			break;
		case PRN_STATUS_OK:
			nBusy=FALSE;			
			
			if(nFirstRun == 1)
			{
				//case0:测试参数错误返回值"	NDK_ERR_PARA"  20140922 jiangym add
				if((nRet=NDK_PrnInit(MAXPRNSWITCH+2)) != NDK_ERR_PARA)
				{
					send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
					goto prnt_error;	
				}
				//"NDK_ERR_OPEN_DEV打印设备打开失败""NDK_ERR_IOCTL 驱动调试错误""NDK_ERR_MACLLOC 内存空间不足" 这三个返回值暂时无法构造20140922 jiangym add
				
				//case1:测试NDK_PrnInit 能否清空缓冲
				//cls_show_msg("任意键开始打印...");
				if((nRet=NDK_PrnInit(0)) != NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
					goto prnt_error;	
				}
				NDK_PrnStr("看到这句话的时候测试已经失败\n");
				if((nRet=NDK_PrnInit(0)) != NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
					goto prnt_error;	
				}
				if((nRet = NDK_PrnStart()) != NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
					goto prnt_error;
				}
				if (cls_show_msg("若打印了任何数据,按[确认]否则[其他]")==ENTER)
				{
					send_result("%s测试失败", TESTAPI);
					goto prnt_error;//return;
				}				

				//case2: 参数为1能否开启边送边打
				if(NDK_PrnInit(1) != NDK_OK)
				{
					send_result("line %d:%s测试失败",__LINE__,TESTAPI);
					goto prnt_error;	
				}
				
				if((nRet=NDK_PrnStr("---以下先走50点行,再打印\"第1行\"---\n")) != NDK_OK )
				{
					send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
					goto prnt_error;
				}
				NDK_PrnFeedByPixel(50);
				if((nRet=NDK_PrnStr("第1行\n第2行")) != NDK_OK )//第二行(最后一行)因为没有\n,在没调start时不会打出来
				{
					send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
					goto prnt_error;
				}		
				
				if (cls_show_msg("查看打印效果,按[确认]否则[其他]")!=ENTER)
				{
					send_result("line %d:%s测试失败",__LINE__, TESTAPI);
					goto prnt_error;//return;
				}
				cls_show_msg1(2, "子用例通过!");

				if(NDK_PrnInit(1) != NDK_OK)//清空上面"第二行"数据
				{
					send_result("line %d:%s测试失败",__LINE__,TESTAPI);
					goto prnt_error;	
				}
				if((nRet = NDK_PrnStr("\n---以下打印3行---\n第1行\n第2行\n第3行\n")) != NDK_OK )//这里串前先送\n是因为针打与热敏处理有些差异,针打时上一个用例只送到"第1行"为止,没送\n，故此处不送\n的话，会导致数据重叠
				{
					send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
					goto prnt_error;
				}
				if (cls_show_msg("查看打印效果,按[确认]否则[其他]")!=ENTER)
				{
					send_result("%s测试失败", TESTAPI);
					goto prnt_error;//return;
				}
				cls_show_msg1(2, "子用例通过!");
				
				//case3:参数为0能否关闭边送边打
				cls_show_msg("将不打印任何数据,任意键继续...");
				if(NDK_PrnInit(0) != NDK_OK)
				{
					send_result("line %d:%s测试失败",__LINE__,TESTAPI);
					goto prnt_error;	
				}
				
				if((NDK_PrnStr("---以下打印2行---\n第1行\n第2行\n")) != NDK_OK )
				{
					send_result("line %d:%s测试失败",__LINE__,TESTAPI);
					goto prnt_error;
				}
				if (cls_show_msg("若打印了任何数据,按[确认]否则[其他]")==ENTER)
				{
					send_result("%s测试失败", TESTAPI);
					goto prnt_error;//return;
				}
				cls_show_msg1(2, "子用例通过!");

				cls_show_msg("将打印几行数据,任意键继续...");
				if((nRet = NDK_PrnStart()) != NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
					goto prnt_error;
				}
				if (cls_show_msg("查看打印效果,按[确认]否则[其他]")!=ENTER)
				{
					send_result("%s测试失败", TESTAPI);
					goto prnt_error;//return;
				}				
				
				nFirstRun++;
			}

			//case4:测试NDK_PrnInit能否中断打印
			if (nFlag==1)
			{
				if(NDK_PrnInit(1) != NDK_OK)
				{
					send_result("line %d:%s测试失败",__LINE__,TESTAPI);
					goto prnt_error;	
				}
				
				NDK_PrnStr("将打印测试页1与测试页2\n\n\n");//zhangnw20110608add
				//这里打印要有一定的数据量
				NDK_PrnStr("打印测试页1:将打印1---70行\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n\
					11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n\
					31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n\
					51\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n63\n64\n65\n66\n67\n68\n69\n70\n");
#if !(K21_ENABLE||CPU5830X_ENABLE||defined SP930G||defined SP930PG||defined SP630PG) //此处打印比较快的设备不用延时,打印比较慢的设备需要延时,可根据实际情况调整:如果打得太快一直循环打印无法中断就要减少延时,如果未打印就中断则需要加大延时
				NDK_SysDelay(20);
#endif
				//NDK_PrnStart();//\f是打印数据完整性标志
				//delay(10);//延迟一下让测试页1打出一点来
			}
			else if (nFlag==2)
			{
				NDK_PrnStr("测试页1已被中断！\n打印测试页2\n打印测试页2\n打印测试页2\n");
				//NDK_PrnStart();//\f是打印数据完整性标志
			}
			else if(nFlag==3)
			{
				//test over
				nKeyin = cls_show_msg1(MAXWAITTIME, "请查看测试页1与测试页2.[确认]测试通过,[其它]失败");
				if (nKeyin==ENTER || nKeyin==0)
					send_result("%s测试通过", TESTAPI);
				else
					send_result("%s测试失败", TESTAPI);
				goto prnt_error;//return;			
			}

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
				goto prnt_error;//return;
			}
			NDK_PrnStart();
			cls_printf("测试%s...", TESTAPI);
			break;
		case PRN_STATUS_OVERHEAT:
			send_result("line %d: 打印机过热", __LINE__);
			goto prnt_error;
			break;
		case PRN_STATUS_VOLERR:
			send_result("line %d: 打印机电源故障", __LINE__);
			goto prnt_error;
			break;
		case PRN_STATUS_DESTROYED:
			send_result("line %d: 打印机损坏", __LINE__);
			goto prnt_error;
			break;
		default:
			//未知状态，判为错
			send_result("line %d: UNKNOWN STATE(tpstatus=%d)", __LINE__, emStatus);
			goto prnt_error;//return;
			break;
		}
	}
	
prnt_error:
	NDK_PrnInit(g_PrnSwitch);
	return;
}

