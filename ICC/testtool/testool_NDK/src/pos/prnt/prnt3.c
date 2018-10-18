/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 打印模块
* file name		: prnt3.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_PrnStr接口功能
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
#define TESTAPI	"NDK_PrnStr"	
#define MAXWAITTIME 180

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void prnt3(void)
* functional description 	: 打印字串
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void prnt3(void)
{
	//以下为局部变量
	EM_PRN_STATUS emStatus=0;
	int nBusy = FALSE,i=0,nKeyin=0;
	time_t nOldtime=0,nDiff=0;
	char sPrintBuf[1024+32] = {0};
	int j = 0;
	int nRet=0;
	int nType = lib_getprintertype();
	
	//以下为函数体
	cls_printf("测试%s...", TESTAPI); 

	//测试前置:
	/*if(NDK_PrnInit(g_PrnSwitch) != NDK_OK)*/
	if(lib_initprn(g_PrnSwitch) != NDK_OK)//初始化使开始设置的配置有效modify 20131122 jiangym
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
			continue;
			break;

		case PRN_STATUS_OK:
			//test begin
			nBusy=FALSE;

			switch(i)
			{
			case 0: /**<打印随机数*/
				cls_printf("打印随机数");
				NDK_PrnStr("\n---打印随机数开始---\n");
				
				for (j = 0; j<1024; j++)
				{
					if (!(sPrintBuf[j]=rand()%256))
						break;
				}
				
				if(NDK_PrnStr(sPrintBuf) != NDK_OK)
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					return;
				}
				
				NDK_PrnStr("\n---打印随机数完毕---\n");
				break;
				
			case 1:	/**<打印固定数*/
				cls_printf("打印固定数");
				NDK_PrnStr("\n---打印固定数开始---\n");
				//added 20130905 打印要能够打出生僻字仡和仴
				NDK_PrnSetFont(PRN_HZ_FONT_24x24,PRN_ZM_FONT_8x16 );
				if(NDK_PrnStr("请观察 1 I l 三个字符能否清晰区分,能够打印出生僻字:单人旁月(仴)和(仡)")!=NDK_OK)
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					goto prnt_error;//return;
				}
				
				NDK_PrnSetFont(0,PRN_ZM_FONT_8x16 );
				if(NDK_PrnStr("\n[8x16] !\"#%&()*/0")!=NDK_OK)
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					goto prnt_error;//return;
				}

				NDK_PrnSetFont(0,PRN_ZM_FONT_8x8);	
				if(NDK_PrnStr("\n[8x8] 2369;<=>?") != NDK_OK)
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					goto prnt_error;//return;
				}
				
				NDK_PrnSetFont(0,PRN_ZM_FONT_10x16);
				if(NDK_PrnStr("\n[10x16] ACIKPQUY\\")!= NDK_OK)
				{
		                	send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					goto prnt_error;//return;
				}
				
				NDK_PrnSetFont(0,PRN_ZM_FONT_5x16);
				if(NDK_PrnStr("\n[5x16] \"#$%&()*+,-./0123456789:;<=>?@AHIJOPUVXYZabdew\r")!= NDK_OK)
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					goto prnt_error;//return;
				}

				NDK_PrnSetFont(0,PRN_ZM_FONT_5x7);
				if(NDK_PrnStr("\n[5x7] \"#$%&()*+,-./0123456789:;<=>?@AHIJOPUVXYZabdew\r")!= NDK_OK)
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					goto prnt_error;//return;
				}

				NDK_PrnSetFont(PRN_HZ_FONT_24x24,PRN_ZM_FONT_8x16);
				NDK_PrnStr(" [24x24]惜缘\n");

				NDK_PrnSetFont(PRN_HZ_FONT_16x16,0);
				NDK_PrnStr(" [16x16]惜福\n");

				NDK_PrnSetFont(PRN_HZ_FONT_12x16,0);
				NDK_PrnStr(" [12x16]惭愧\n");

				NDK_PrnSetFont(PRN_HZ_FONT_16x32,0);
				NDK_PrnStr(" [16x32]感恩\n");

				NDK_PrnSetFont(PRN_HZ_FONT_32x32,0);
				NDK_PrnStr(" [32x32]国国");
	
				NDK_PrnStr("\n---打印固定数完毕---\n");
				
				NDK_PrnStr("\n---请查看上面字体的打印效果---\n");
				break;
				
			case 2:  /**<打印回车，换行，空格*/
				cls_printf("打印回车，换行，空格");
				NDK_PrnStr("\n---打印回车，换行，空格开始---\n");

				//打印\t 应该是无效果(文档也没提到)
				if ((nRet = NDK_PrnStr("打印\\r效果(会换行)\r打印\\t效果(无效果)\t打印\\n效果(会换行)\n"))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
					goto prnt_error;//return;
				}
	
				if ((nRet = NDK_PrnStr("\nSTART: :END(请检查START/END之间应有一空格)\n"))!=NDK_OK)
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
					goto prnt_error;//return;
				}
	
				if ((nRet = NDK_PrnStr("NEWLand新大陆!#@%$%^&*()__+|><:;")!=NDK_OK))
				{
					send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,nRet);
					goto prnt_error;//return;
				}
				
				NDK_PrnStr("\n---打印回车，换行，空格完毕---\n");
				
				break;
				
			case 3:	/**< 测试缓冲区越界*/
				cls_printf("打印缓冲区大小测试");
				
				switch (nType)
				{
				case PRN_TYPE_HIP:
				case PRN_TYPE_FIP:
					memset(sPrintBuf, 'H', sizeof(sPrintBuf));
					memcpy(sPrintBuf, "如果你看到这行,说明打印有误!", strlen("如果你看到这行,说明打印有误!"));

					if((nRet = NDK_PrnStr(sPrintBuf))!=NDK_ERR_PARA)
					{
						send_result("line %d: 打印缓冲区大小测试失败(%d)", __LINE__,nRet);
						goto prnt_error;//return;
					}
					NDK_PrnInit(g_PrnSwitch);
					break;
				case PRN_TYPE_TP:
#if !K21_ENABLE //K21平台只支持边送边打，不支持打印缓冲区
					nRet  = NDK_PrnStr ("打印测试页1:将打印1---199行\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n\
							11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n\
							31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n\
							51\n52\n53\n54\n55\n56\n57\n58\n59\n60\n61\n62\n63\n64\n65\n66\n67\n68\n69\n70\n\
							71\n72\n73\n74\n75\n76\n77\n78\n79\n80\n81\n82\n83\n84\n85\n86\n87\n88\n89\n90\n\
							91\n92\n93\n94\n95\n96\n97\n98\n99\n100\n101\n102\n103\n104\n105\n106\n107\n108\n109\n110\n\
							111\n112\n113\n114\n115\n116\n117\n118\n119\n120\n121\n122\n123\n124\n125\n126\n127\n128\n129\
							\n130\n131\n132\n133\n134\n135\n136\n137\n138\n139\n140\n141\n142\n143\n144\n145\n146\n147\n148\n149\
							\n150\n151\n152\n153\n154\n155\n156\n157\n158\n159\n160\n161\n162\n163\n164\n165\n166\n167\n168\n169\
							\n170\n171\n172\n173\n174\n175\n176\n177\n178\n179\n180\n181\n182\n183\n184\n185\n186\n187\n188\n189\
							\n190\n191\n192\n193\n194\n195\n196\n197\n198\n199\n200\n201\n");
#if OVERSEAS_OS70_ENABLE  //海外版7.0版本返回值修改  20180424 modify
					if(nRet !=NDK_ERR_MACLLOC)
#else
					if(nRet !=NDK_ERR_PARA)
#endif
					{
						send_result("line %d: 打印缓冲区大小测试失败(%d)", __LINE__,nRet);				
						goto prnt_error;//return;
					}
					NDK_PrnInit(g_PrnSwitch);
#endif
					break;
				default:
					send_result("line %d:未能获取到打印机类型", __LINE__);
					goto prnt_error;
					break;
				}

				break;
				
			case 4: /**<测试打印结束符*/
				sPrintBuf[0] = '\0';
				strcpy(sPrintBuf+1, "如果你看到这行,说明打印有误!");
				if((nRet = NDK_PrnStr(sPrintBuf))!=NDK_OK)
				{
					send_result("line %d:%s测试失败%d", __LINE__, TESTAPI,nRet);
					goto prnt_error;//return;		
				}
				break;
			case 5: /*case 5:根据V4.3.3更新热敏机器单独调用"\r\n"处理为走8点行故添加此案例*/
				if(nType==PRN_TYPE_TP)
				{
					cls_printf("下面测试热敏打印机打印走8点行用例");
					if((nRet=NDK_PrnStr("打印\\r\\n效果为走8个点行\r\n于上一行字符中有8个点行"))!=NDK_OK)
					{
						send_result("line %d:%s测试失败(%d)",__LINE__,TESTAPI,nRet);
						goto prnt_error;
					}
				}
				break;
			case 6: //新增用例验证连续调用NDK_PrnStr且中间的NDK_PrnStr没有\n时是否能在一行打印
				NDK_PrnStr("冒号后的字应该在同一行:\n");
				NDK_PrnStr("银联商务");
				NDK_PrnStr("全民付");
				NDK_PrnStr("交易\n");
				NDK_PrnStr("上面打印的字在同一行\n");
				break;
			default:
				nKeyin = cls_show_msg1(MAXWAITTIME, "检查打印效果,按ENTER通过,其它键不通过");
				if (nKeyin!=ENTER && nKeyin!=0)//zhangnw20061031 modify
					send_result("%s测试失败", TESTAPI);
				else
					send_result("%s测试通过", TESTAPI);
				goto prnt_error;//return;
				break;
			}		
			NDK_PrnStart();	
			i++;
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
