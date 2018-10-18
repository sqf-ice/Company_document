/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线模块
* file name		: wlm3.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_WlSendATCmd接口功能
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
#define TESTAPI	"NDK_WlSendATCmd"	
#define	MAXWAITTIME (40)

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
 
/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlm6(void)
* functional description 	: 向无线模块发送和接收AT指令。当pATCmdPack为空NULL时表明直接等待接收,否则发送AT指令;如果pcOutput为空NULL则表明不接收AT指令 
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void wlm3(void)
{
	/*private & local definition*/
	char szOutbuf[1024] = {0};//拨号命令会受这个缓冲区长度影响，应该是个BUG
	ST_ATCMD_PACK STatcmdpack = {0};
	char *pos = NULL;
	//time_t diff = 0, oldtime = 0;
	LinkType type = getlinktype();
	EM_WLM_STATUS emStatus = 0;
	int nRet = 0;
	
	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI);
	
	memset(&STatcmdpack,0,sizeof(ST_ATCMD_PACK));
	//case1:异常测试
	memset(szOutbuf, 0, sizeof(szOutbuf));
	if((nRet = NDK_WlSendATCmd(NULL, szOutbuf,sizeof(szOutbuf), 30000,&emStatus)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}	

	STatcmdpack.AtCmdNo = WLM_CMD_E0;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, NULL,0, 30000,&emStatus)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	
	memset(szOutbuf, 0, sizeof(szOutbuf));
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf,0, 30000,NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	
	//正常命令序列测试
	//case2:测试WLM_CMD_E0
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_E0;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf,sizeof(szOutbuf), 30000,&emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//NDK_OK,但需要进一步看一下接收以确定send的正确性
	{
		if(strstr(szOutbuf, "OK"))
			;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}		
	}

	//case3:测试WLM_CMD_CSQ
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_CSQ;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{		
		if(strstr(szOutbuf, "OK"))
			;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}	
	}	

	//case4:测试WLM_CMD_CPIN
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_CPIN;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{
		if(strstr(szOutbuf, "READY"))//测试的卡默认是没设置PIN的,返回READY
				;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}		
	}	

#if 0 //不要设置PIN,不做设置PIN的设置
	//测试WLM_CMD_CPIN0
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_CPIN0;
	STatcmdpack.pcAddParam = "1234";
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{
		if(strstr(szOutbuf, "OK"))
				;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}		
	}
#endif

	//case5:测试WLM_CMD_CREG(是指GSM网络的注册状态,不是GPRS,所以一般来说,模块一上电就自动注册了)
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_CREG;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{		
		//上电时自动注册,所以查询结果应为:一般已注册1(注册本地)或5(漫游)
		if(strchr(szOutbuf, '1')||strchr(szOutbuf, '5'))
			;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}	
	}

	//case6:测试WLM_CMD_CGATT1
	//命令态
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_CGATT1;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{	
		if(strstr(szOutbuf, "OK"))
			;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}	
	}

	//case7:测试WLM_CMD_COPS取网络注册信息(CDMA卡不测试(不支持),目前仅以GPRS测试卡为准预设结果)
	if(type==GPRS)
	{
		memset(szOutbuf, 0, sizeof(szOutbuf));
		STatcmdpack.AtCmdNo = WLM_CMD_COPS;
		STatcmdpack.pcAddParam = NULL;
		STatcmdpack.pcAtCmd = NULL;
		if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		else//SUCC,但需要进一步看一下接收以确定send的正确性
		{			
			if(strstr(szOutbuf, "CHINA MOBILE")||strstr(szOutbuf, "CHN-UNICOM")||strstr(szOutbuf, "CHINA  MOBILE")||strstr(szOutbuf, "China Mobile")||strstr(szOutbuf, "china mobile")||strstr(szOutbuf, "CHINA UNICOM")||strstr(szOutbuf, "China Unicom")||strstr(szOutbuf, "china unicom")||strstr(szOutbuf, "46000")||strstr(szOutbuf, "46001"))
				;
			else
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				RETURN;
			}	
		}
	}

	//case8:测试WLM_CMD_CGATT0
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_CGATT0;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{		
		if(strstr(szOutbuf, "OK"))
			;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}	
	}
#if 0//20131107,根据严明要求关闭测试通过AT命令进行拨号的用例
	//case9:测试WLM_CMD_D2通过拔号至数据态
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_D2;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{		
		if(strstr(szOutbuf, "OK"))
			;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}	
	}	
	
	sleep(5);//TD模式下WM_D2可能会影响到本子项测试,而GPRS不存在此问题.目前暂时延时来规避
	
	//case10:测试WLM_CMD_DIAL拨号,数通状态下不响应命令
	//拔号前,会先注册网络(可不必手工注册)
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_DIAL;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{
		if(strstr(szOutbuf, "CONNECT"))
			;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}
		
#if 0	//zhangnw20130418:因为调用的返回不确定,不再测试.
		//一般地,模块在数通态下,不再响应AT命令
		memset(szOutbuf, 0, sizeof(szOutbuf));
		STatcmdpack.AtCmdNo = WLM_CMD_E0;
		STatcmdpack.pcAddParam = NULL;
		STatcmdpack.pcAtCmd = NULL;
		if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		if(emStatus!=WLM_STATUS_NO_CARRIER/*WLM_STATUS_ERROR*/)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,emStatus);
			RETURN;
		}
#endif
	}
	
	//断开PPP,如果下面的招不起作用,就来狠的,调resetwirelessmodem
	if(type==GPRS)
	{
		oldtime = time(NULL);
		while((diff=time(NULL)-oldtime)<2*MAXWAITTIME)
			cls_printf("正在断开链路,请等待%d秒...", 2*MAXWAITTIME-diff);//静置40秒,让移动后台自动释放链路//参数略大,保险//由于上个用例减少了40秒时间，此处放大等待时间
	}
	else if(type==CDMA)
	{
		NDK_PppHangup(0);
	}

	//此时应回到命令态,发AT命令应响应
	//支持WLM_CMD_CGMR,就用它;否则就用WLM_CMD_E0
#if 1
	//case11:测试WLM_CMD_CGMR
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_CGMR;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{
		if(strstr(szOutbuf, "OK"))
			;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}	
	}
#else
	//WLM_CMD_E0
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_E0;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf,sizeof(szOutbuf), 30000,&emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//NDK_OK,但需要进一步看一下接收以确定send的正确性
	{
		if(strstr(szOutbuf, "OK"))
			;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}		
	}
#endif
#endif
	//case12:测试WLM_CMD_CGSN
	// 1.本命令对GPRS模块来说是是取IMEI.
	//在手机上输入 *#06# 输完这5 个键符后，手机屏幕上会出现一行阿拉伯数字(15个)，即为该手机的串号。
	//以上说的使用方法为G网手机，C网手机用上述办法则无法查看。
	// 2.本命令对CDMA模块来说是是取ESN,32bits长度.
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_CGSN;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{
		if(type==GPRS)//再进一步看看
		{
			if((pos=strstr(szOutbuf, "\x0D\x0A"))==NULL) //没有0D0A  返回类型为:862996013998543....
			{
				*(szOutbuf+15) = '\0';//15位数字
				if(is_digit_str(szOutbuf))
					;
				else
				{
					send_result("line %d:%s测试失败", __LINE__, TESTAPI);
					RETURN;
				}
			}
			else if((pos=strstr(szOutbuf, "\""))!=NULL)//有引号，则返回数据类型为(0d)(0a)+GSN:(20)"862996013998543"(0d)(0a)(0d)(0a)OK(0d)(0a)
			{
				pos++;//指针移到引号后一个位置  //linwl20130515新增对G610模块返回信息的提取
			}
			else if((pos=strstr(szOutbuf, "\x0D\x0A"))!=NULL)//没有" 引号如果没有 则返回数据类型为 
			{
				if(pos==szOutbuf)//收回数据以0d0a开始且没有引号的话,过滤掉开始的0d0a  (0d)(0a)862996013998543....
					pos+=strlen("\x0D\x0A");
				else
					pos = szOutbuf;//0D0A在字符串后面，则862996013998543(0d)(0a)
			}
			
			*(pos+15) = '\0';//15位数字
			if(is_digit_str(pos))
				;
			else
			{
				send_result("line %d:%s测试失败%s", __LINE__, TESTAPI, pos);
				RETURN;
			}
		}
	}	
	
#if 0
	//case:测试WLM_CMD_CCID
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_CCID;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = NULL;
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{
		if(strstr(szOutbuf, "OK"))
			;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}	
	}	
#endif

	//case13:"非法"参数WLM_CMD_END(事实上开放给应用的h文件中不应该有这个宏!),应失败(不是按UNDEFINE处理)
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_END;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = "E1";
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}

	//case14:测试WLM_CMD_UNDEFINE
	//subcase1:文档说" 如果是未定义指令，先判断pszAtCmd命令是否能搜索到对应的AtCmdNo号， 如果是则转为相应的atcmdno进行处理。"
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_UNDEFINE;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = "+CSQ";
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{		
		if(strstr(szOutbuf, "OK"))
			;
		else
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}	
	}		
	
	//subcase2:确实是未定义的命令,应执行pszAtCmd
	if(type==GPRS)//CDMA无此命令
	{
		memset(szOutbuf, 0, sizeof(szOutbuf));
		STatcmdpack.AtCmdNo = WLM_CMD_UNDEFINE;
		STatcmdpack.pcAddParam = NULL;
		STatcmdpack.pcAtCmd = "+CIMI";
		if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		else//SUCC,但需要进一步看一下接收以确定send的正确性
		{
			//46001代表是中国联通,46000代表中国移动
			//460对应于86，00对应于135---139号段...
			if((pos=strstr(szOutbuf, "460"))==NULL)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				RETURN;
			}
			*(pos+15) = '\0';//15位数字
			if(is_digit_str(pos))
				;
			else
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				RETURN;
			}
		}	
	}

	//case15:无效命令"welcome"
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_UNDEFINE;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = "welcome";
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}

	//case16:无效命令 "+CPIN="
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_UNDEFINE;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = "+CPIN=";
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	
	//case17:测试已定义的命令,将忽略AtCmdNo
	memset(szOutbuf, 0, sizeof(szOutbuf));
	STatcmdpack.AtCmdNo = WLM_CMD_E0;
	STatcmdpack.pcAddParam = NULL;
	STatcmdpack.pcAtCmd = "+CPIN?";
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	else//SUCC,但需要进一步看一下接收以确定send的正确性
	{	
		if(strstr(szOutbuf, "READY"))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}
		else//eg. "OK"
			;	
	}
#if CPU5810X_ENABLE||defined ME50NX||defined ME68
	time_t oldtime = 0, diff = 0;
	//case18:硬件复位后马上调用会返回NDK_ERR_GPRS_UNINIT
	if((nRet=NDK_WlModemReset())!=NDK_OK)//硬件复位
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	if ((nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 30000, &emStatus)) != NDK_ERR_GPRS_UNINIT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	
	//case19:硬件复位未完成调用会返回NDK_ERR_GPRS_BUSY
	oldtime = time(NULL);
	while(1)
	{
		NDK_SysMsDelay(50);
		nRet = NDK_WlSendATCmd(&STatcmdpack, szOutbuf, sizeof(szOutbuf), 1000, &emStatus);
		if(nRet==NDK_ERR_GPRS_BUSY)
			break;
		if(nRet==NDK_ERR_GPRS_UNINIT)
			;
		if(nRet==NDK_OK)
			break;
		if((diff=time(NULL)-oldtime)>20)
		{
			send_result("line %d:长时间未返回NDK_ERR_GPRS_BUSY(%d)", __LINE__, nRet);
			return;
		}
	}
	sleep(3);//增加足够的延时确保复位完成
#endif
	
	send_result("%s测试通过", TESTAPI);
	return;
}

