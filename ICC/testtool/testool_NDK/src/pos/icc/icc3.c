/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: icc模块
* file name		: 
* Author 			: chenfm
* version			: 
* DATE			: 20130107
* directory 		: 
* description		: 测试 NDK_Iccrw接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_Iccrw"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
extern void icpowerdown_all(void);

/*---------------functions definition---------------------------*/
static int test_iccard_errcode(void)
{
	//以下为局部变量
	uchar psAtrbuf[256]={0}, sendbuf[]={'\x00', '\x84', '\x00', '\x00', 8}, recvbuf[256]={0};
	int pnAtrlen=0, recvlen=0, ret=0;
#if !K21_ENABLE
	uchar  overload[256]={0};
#endif
	//以下为函数体
	//case1:NDK_ERR_ICC_CARDNOREADY_ERR  卡未准备好 
	cls_show_msg("请确保IC已移除,按任意键继续...");
	memset(psAtrbuf, 0, sizeof(psAtrbuf));
#if  !K21_ENABLE //K21无法取IC卡插卡状态,直接返回异常
	if((ret=NDK_IccPowerUp(ICTYPE_IC, psAtrbuf, &pnAtrlen))!=NDK_ERR_ICC_CARDNOREADY_ERR)
#else
	if((ret=NDK_IccPowerUp(ICTYPE_IC, psAtrbuf, &pnAtrlen))!=NDK_ERR_IOCTL)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}

	//case2：未上电，使用NDK_Iccrw发取随机数命令，应失败返回。
	cls_show_msg("请确保IC已插入,按任意键继续...");	
	memset(recvbuf, 0, sizeof(recvbuf));
	if((ret=NDK_Iccrw(ICTYPE_IC, sizeof(sendbuf), sendbuf, &recvlen, recvbuf))!=NDK_ERR_IOCTL)//NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	cls_show_msg1(2, "子用例测试通过!");

	//case3：NDK_ERR_ICC_CARDPULL_ERR（卡拔出了）。
	cls_show_msg("请确保IC已插入,按任意键继续...");
	memset(psAtrbuf, 0, sizeof(psAtrbuf));
	if((ret=NDK_IccPowerUp(ICTYPE_IC, psAtrbuf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	cls_show_msg("IC上电成功!请拔插一下IC,按任意键继续...");
	//使用iccrw发取随机数命令
	memset(recvbuf, 0, sizeof(recvbuf));
	if((ret=NDK_Iccrw(ICTYPE_IC, sizeof(sendbuf), sendbuf, &recvlen, recvbuf))!=NDK_ERR_IOCTL)//NDK_ERR_ICC_CARDPULL_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	if((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}	

	//case4:NDK_ERR_ICC_COM_ERR（命令出错）
	cls_show_msg("请确保IC已插入,按任意键继续...");
	memset(psAtrbuf, 0, sizeof(psAtrbuf));
	if((ret=NDK_IccPowerUp(ICTYPE_IC, psAtrbuf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	memset(recvbuf, 0, sizeof(recvbuf));
#if  0//K21_ENABLE&&!ANDIROD_ENABLE&&!defined ME30THM &&!defined ME32
	if((ret=NDK_Iccrw(ICTYPE_IC, sizeof(sendbuf)-2, sendbuf+2, &recvlen, recvbuf))!=NDK_OK)//K21 API直接返回成功，调用结果由底层驱动返回 20140526 详见redmine2325
#else  //跟邱涛确认中低端都统一对命令进行判断，命令格式不对就返回错误 20180129 modify by jym
	if((ret=NDK_Iccrw(ICTYPE_IC, sizeof(sendbuf)-2, sendbuf+2, &recvlen, recvbuf))!=NDK_ERR_IOCTL)//NDK_ERR_ICC_COM_ERR)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}	
	if((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	cls_show_msg1(2, "子用例测试通过!");

#if !K21_ENABLE//20150423 经过和郭忱定位发现K21平台没有254限制，底层只发送指令给卡片处理，卡片的返回无法预期，和卡片有关系，故K21不测试该用例
	//case5:APDU数据长度大于254，对于IC卡读写，数据长度限制在254个字节
	cls_show_msg("请确保IC已插入,按任意键继续...");
	//注意:上一用例执行后(卡被抽走的行为被iccrw捕获),IC卡已下电,需要重新上电
	memset(psAtrbuf, 0, sizeof(psAtrbuf));
	if((ret=NDK_IccPowerUp(ICTYPE_IC, psAtrbuf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	memset(overload, 0, sizeof(overload));
	memcpy(overload, sendbuf, sizeof(sendbuf));
	memset(recvbuf, 0, sizeof(recvbuf));
	if((ret=NDK_Iccrw(ICTYPE_IC, sizeof(overload), overload, &recvlen, recvbuf))!=NDK_ERR_IOCTL)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
	cls_show_msg1(2, "子用例测试通过!");

	//下电
	if((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return FAIL;
	}
#endif

	return SUCC;
}

/****************************************************************
* function name 	 	: icc3
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	 	 chenfm     20130107	created
*				  	 
*****************************************************************/
void icc3(void)
{
	//以下为局部变量
#include "icsam.h"

	int i=0, ret=0, ret1=0, ret2=0, ret3=0, ret4=0, rlen=0, maxid=ICSAMNUM+1/*SZ_ARRAY(CardNo)*/, pnAtrlen=0;
	uchar recvbuf[128]={0}, psAtrbuf[256]={0};
	ushort sw=0;
	uchar sendbuf[]={'\x00', '\x84', '\x00', '\x00', 8};

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//测试前置
	icpowerdown_all();

	//case1:正确参数
	do
	{
		//case1.1:上电,使用NDK_Iccrw发取随机数命令,应该成功返回。
		memset(psAtrbuf, 0, sizeof(psAtrbuf));
		NDK_IccPowerDown(CardNo[i]);
#if 0//linwl20130614 开发修改上下电成功与否于NDK_IccSetType函数设置的卡类型无关，只跟上下电函数一个参数有关
		if((ret=NDK_IccSetType(CardNo[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%s:%d)", __LINE__, TESTAPI, CardNoStr[i], ret);
			GOTOERR;
		}
#endif
		if((ret=NDK_IccPowerUp(CardNo[i], psAtrbuf, &pnAtrlen))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%s:%d)", __LINE__, TESTAPI, CardNoStr[i], ret);
			GOTOERR;
		}
		memset(recvbuf, 0, sizeof(recvbuf));
		if((ret=NDK_Iccrw(CardNo[i], sizeof(sendbuf), sendbuf, &rlen, recvbuf))!=NDK_OK||((sw=SMART_GETSW(recvbuf+rlen-2))!=0x9000&&(sw=SMART_GETSW(recvbuf+rlen-2))!=0x6d00))
		{
			//NDK_IccPowerDown(CardNo[i]);						
			send_result("line %d:%s测试失败(%s:%d,0x%04x)", __LINE__, TESTAPI, CardNoStr[i], ret, sw);
			GOTOERR;
		}

		//case1.2:读写后再次上电下电的子用例
		memset(psAtrbuf, 0, sizeof(psAtrbuf));
		if((ret=NDK_IccPowerUp(CardNo[i], psAtrbuf, &pnAtrlen))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%s:%d)", __LINE__, TESTAPI, CardNoStr[i], ret);
			GOTOERR;
		}		
		if((ret=NDK_IccPowerDown(CardNo[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%s:%d)", __LINE__, TESTAPI, CardNoStr[i], ret);
			GOTOERR;
		}
	}
	while(++i!=(maxid-1));

	//case2:错误命令
	i=0;
	do
	{
		//case2.1:上电，发错误命令，应失败返回。
		memset(psAtrbuf, 0, sizeof(psAtrbuf));
		if((ret=NDK_IccPowerUp(CardNo[i], psAtrbuf, &pnAtrlen))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%s:%d)", __LINE__, TESTAPI, CardNoStr[i], ret);
			GOTOERR;
		}
		memset(recvbuf, 0, sizeof(recvbuf));
		if((ret=NDK_Iccrw(CardNo[i], sizeof(sendbuf)-2, sendbuf+2, &rlen, recvbuf))!=NDK_ERR_IOCTL)//NDK_ERR_IOCTL)
		{
			//NDK_IccPowerDown(CardNo[i]);						
			send_result("line %d:%s测试失败(%s:%d)", __LINE__, TESTAPI, CardNoStr[i], ret);
			GOTOERR;
		}		
		if((ret=NDK_IccPowerDown(CardNo[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%s:%d)", __LINE__, TESTAPI, CardNoStr[i], ret);
			GOTOERR;
		}
	}
	while(++i!=(maxid-1));
	
	//由于N510不支持IC卡,只支持sam1卡,故增加相关异常测试点20180510 add
#if SAM1_ENABLE
	//case5.1:异常参数测试
	if((ret=NDK_IccPowerUp(ICTYPE_SAM1, psAtrbuf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	memset(recvbuf, 0, sizeof(recvbuf));
	if((ret=NDK_Iccrw(-1, sizeof(sendbuf), sendbuf, &rlen, recvbuf))!=NDK_ERR_PARA/*NDK_ERR_OPEN_DEV*/
   	||(ret1=NDK_Iccrw(ICTYPE_SAM1, -1, sendbuf, &rlen, recvbuf))!=NDK_ERR_PARA
   	||(ret2=NDK_Iccrw(ICTYPE_SAM1, sizeof(sendbuf), NULL, &rlen, recvbuf))!=NDK_ERR_PARA
   	||(ret3=NDK_Iccrw(ICTYPE_SAM1, sizeof(sendbuf), sendbuf, NULL, recvbuf))!=NDK_ERR_PARA
   	||(ret4=NDK_Iccrw(ICTYPE_SAM1, sizeof(sendbuf), sendbuf, &rlen, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:错误返回值测试失败(%d,%d,%d,%d,%d)", __LINE__, ret, ret1, ret2, ret3, ret4);
		GOTOERR;
	}
	if((ret=NDK_IccPowerDown(ICTYPE_SAM1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
#if IC1_ENABLE
	if(auto_flag==0 || auto_flag==2)
	{
		//case4:IC卡错误码测试(使用都会有的IC)
		if(test_iccard_errcode()!=SUCC)
			GOTOERR;
	
	//case5:异常参数测试
	cls_show_msg("请确保IC已插入,按任意键继续...");
	}
	if((ret=NDK_IccPowerUp(ICTYPE_IC, psAtrbuf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	memset(recvbuf, 0, sizeof(recvbuf));
	if((ret=NDK_Iccrw(-1, sizeof(sendbuf), sendbuf, &rlen, recvbuf))!=NDK_ERR_PARA/*NDK_ERR_OPEN_DEV*/
   	||(ret1=NDK_Iccrw(ICTYPE_IC, -1, sendbuf, &rlen, recvbuf))!=NDK_ERR_PARA
   	||(ret2=NDK_Iccrw(ICTYPE_IC, sizeof(sendbuf), NULL, &rlen, recvbuf))!=NDK_ERR_PARA
   	||(ret3=NDK_Iccrw(ICTYPE_IC, sizeof(sendbuf), sendbuf, NULL, recvbuf))!=NDK_ERR_PARA
   	||(ret4=NDK_Iccrw(ICTYPE_IC, sizeof(sendbuf), sendbuf, &rlen, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:错误返回值测试失败(%d,%d,%d,%d,%d)", __LINE__, ret, ret1, ret2, ret3, ret4);
		GOTOERR;
	}
	if((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
//NDK_IccSetType和NDK_IccGetType两个函数没有意义，接口还是保留，现在上电交互的类型由调用函数传入的第一个参数决定
#if 0
	//20130407:开发目前调整设计为settype与后续操作的type必须严格一致才能成功???
	//case6:settype、上电、rw的配对测试(从27种场景中抽取部分正确和错误的场景进行测试)
	//case6.1:设置type为SAM1卡，对IC卡上电，读写应成功返回。
	if((ret=NDK_IccSetType(ICTYPE_SAM1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); // 恢复缺省设置
		GOTOERR;
	}
	if((ret=NDK_IccPowerUp(ICTYPE_IC, psAtrbuf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	memset(recvbuf, 0, sizeof(recvbuf));
	if((ret=NDK_Iccrw(ICTYPE_IC, sizeof(sendbuf), sendbuf, &rlen, recvbuf))!=NDK_OK||(sw=SMART_GETSW(recvbuf+rlen-2))!=0x9000)
	{
		//NDK_IccPowerDown(CardNo[i]);						
		send_result("line %d:%s测试失败(%s:%d,0x%04x)", __LINE__, TESTAPI, CardNoStr[i], ret, sw);
		GOTOERR;
	}

	//case6.2:设置type为MEMORY卡（或IC）,对SAM卡上电，读写应成功返回。
	if((ret=NDK_IccSetType(ICTYPE_M_1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_IccPowerUp(ICTYPE_SAM1, psAtrbuf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	memset(recvbuf, 0, sizeof(recvbuf));
	if((ret=NDK_Iccrw(ICTYPE_SAM1, sizeof(sendbuf), sendbuf, &rlen, recvbuf))!=NDK_OK||(sw=SMART_GETSW(recvbuf+rlen-2))!=0x9000)
	{
		//NDK_IccPowerDown(CardNo[i]);						
		send_result("line %d:%s测试失败(%s:%d,0x%04x)", __LINE__, TESTAPI, CardNoStr[i], ret, sw);
		GOTOERR;
	}
	if((ret=NDK_IccSetType(ICTYPE_IC))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	memset(recvbuf, 0, sizeof(recvbuf));
	if((ret=NDK_Iccrw(ICTYPE_SAM1, sizeof(sendbuf), sendbuf, &rlen, recvbuf))!=NDK_OK||(sw=SMART_GETSW(recvbuf+rlen-2))!=0x9000)
	{
		//NDK_IccPowerDown(CardNo[i]);						
		send_result("line %d:%s测试失败(%s:%d,0x%04x)", __LINE__, TESTAPI, CardNoStr[i], ret, sw);
		GOTOERR;
	}

	//case6.3:设置type为IC卡，对IC上电，对MEM卡进行读写，应失败返回。
	if((ret=NDK_IccSetType(ICTYPE_IC))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_IccPowerUp(ICTYPE_IC, psAtrbuf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	memset(recvbuf, 0, sizeof(recvbuf));
	if((ret=NDK_Iccrw(ICTYPE_M_1, sizeof(sendbuf), sendbuf, &rlen, recvbuf))!=NDK_ERR_PARA)
	{
		//NDK_IccPowerDown(CardNo[i]);						
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	//case6.4:设置type为MEM卡，对IC卡上电，对MEM卡进行读写，应失败返回。
	if((ret=NDK_IccSetType(ICTYPE_M_1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_IccPowerUp(ICTYPE_IC, psAtrbuf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	memset(recvbuf, 0, sizeof(recvbuf));
	if((ret=NDK_Iccrw(ICTYPE_M_1, sizeof(sendbuf), sendbuf, &rlen, recvbuf))!=NDK_ERR_PARA)
	{
		//NDK_IccPowerDown(CardNo[i]);						
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
#endif
if(auto_flag==0 || auto_flag==2)
{
#if !K21_ENABLE||defined ME30||defined ME30S||defined ME30THM||defined ME32||defined ME30GX||defined ME32GX||defined ME32THM||ANDIROD_ENABLE //||defined IM81||defined N900
	//case7:测试NDK_Iccrw函数可以对memory卡进行读写(本用例使用24C卡进行读操作进行简单验证，其它memory卡测试在综合用例中进行)
	cls_show_msg("请确保已插入 AT24C032 memory卡,任意键继续...");
	if ((ret=NDK_IccPowerUp(ICTYPE_M_1, psAtrbuf, &pnAtrlen))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_Iccrw(ICTYPE_M_1, 9,(uchar *)"\x00\xD0\x00\x01\x04\x31\x32\x33\x34",&rlen,recvbuf))!=NDK_OK||((sw=SMART_GETSW(recvbuf+rlen-2))!=0x9000&&(sw=SMART_GETSW(recvbuf+rlen-2))!=0x6d00))			
	{
		send_result("line %d:%s测试失败(%d,0x%04x)", __LINE__, TESTAPI, ret, sw);
		GOTOERR;
	}

	if((ret=NDK_Iccrw(ICTYPE_M_1, 5,(uchar *)"\x00\xB0\x00\x01\x04",&rlen,recvbuf))!=NDK_OK||((sw=SMART_GETSW(recvbuf+rlen-2))!=0x9000&&(sw=SMART_GETSW(recvbuf+rlen-2))!=0x6d00))			
	{
		send_result("line %d:%s测试失败(%d,0x%04x)", __LINE__, TESTAPI, ret, sw);
		GOTOERR;
	}

	if(memcmp("\x31\x32\x33\x34",recvbuf,rlen-2))
	{
		send_result("line %d:比较失败(%s)", __LINE__, recvbuf);
		GOTOERR;
	}

	if((ret=NDK_IccPowerDown(ICTYPE_M_1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	if(cls_show_msg("是否进行不符合EMV标准但符合ISO07816标准的特殊IC卡测试?是:[确认],否:[其他]")==ENTER)
	{
		cls_show_msg("请插入只符合了ISO7816的特殊IC卡,后按任意键继续");
		memset(psAtrbuf, 0, sizeof(psAtrbuf));
		NDK_IccPowerDown(ICTYPE_IC);
#if !K21_ENABLE//K21平台产品陆续开始支持7816的卡片，但由于没有支持设置卡类型函数，采用 上电方式修改为先3v再5v 方式可以直接使用7816卡片
		if((ret=NDK_IccSetType(ICTYPE_ISO7816))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ISO7816:%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#endif
		if((ret=NDK_IccPowerUp(ICTYPE_IC, psAtrbuf, &pnAtrlen))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ISO7816:%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		memset(recvbuf, 0, sizeof(recvbuf));
		if((ret=NDK_Iccrw(ICTYPE_IC, sizeof(sendbuf), sendbuf, &rlen, recvbuf))!=NDK_OK||((sw=SMART_GETSW(recvbuf+rlen-2))!=0x9000&&(sw=SMART_GETSW(recvbuf+rlen-2))!=0x6d00))
		{			
			send_result("line %d:%s测试失败(ISO7816:%d,0x%04x)", __LINE__, TESTAPI, ret, sw);
			GOTOERR;
		}
		if((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ISO7816:%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
	}
	
	//测试后置
	cls_show_msg("请确保IC已插入,任意键继续...");
}
else
{
	send_result("line %d:%s部分子用例需手动测试", __LINE__, TESTAPI);
}
#endif
	send_result("%s测试通过", TESTAPI);
ERR:
	icpowerdown_all();
	NDK_IccPowerDown(ICTYPE_M_1);
#if !K21_ENABLE
	NDK_IccSetType(ICTYPE_IC);//恢复默认
#endif
	return;
}

