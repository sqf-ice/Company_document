/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: icc模块
* file name		: 
* Author 			: chenfm
* version			: 
* DATE			: 20130107
* directory 		: 
* description		: 测试 NDK_IccPowerUp,NDK_IccPowerDown是否上下电成功
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_IccPowerUp,NDK_IccPowerDown"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
extern void icpowerdown_all(void);

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: icc4
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	 	 chenfm     20130107	created
*				  	 
*****************************************************************/
void icc4(void)
{
	//if(auto_flag==2)
	//	return; //新增case后需要手动操作20171011

	//以下为局部变量
#include "icsam.h"

	int i=0, maxid=ICSAMNUM+1, rlen=0, pnAtrlen=0;
	int ret=0, ret1=0, ret2=0, ret3=0;
	uchar buf[300]={0};
	uchar sendbuf[]={'\x00', '\x84', '\x00', '\x00', 8};
	uchar req0[] = { 0x00,0xA4,0x00,0x00,0x02,0x00,0x11};
	uchar req1[] = { 0x80, 0x30, 0x00,0x00,0x80, 
				0x22,0x22, 0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				0x22,0x22,0x22,0x22,0x22, 0x22,0x22,0x22,0x22,
				 0x22,0x22,0x00};
	uchar req6100[] = { 0x80,0x30,0x00,0x80, 0x80, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22, 
				0x22,0x22,0x00};
	
	//以下为函数体
	cls_printf("%s测试中...", TESTAPI);

	//测试前置
	icpowerdown_all();

	//case1:异常参数测试
	if((ret=NDK_IccPowerUp(-1, buf, &pnAtrlen))!=NDK_ERR_PARA/*NDK_ERR_OPEN_DEV*/
   	||(ret1=NDK_IccPowerUp(ICTYPE_IC, NULL, &pnAtrlen))!=NDK_ERR_PARA
   	||(ret2=NDK_IccPowerUp(ICTYPE_IC, buf, NULL))!=NDK_ERR_PARA
   	||(ret3=NDK_IccPowerDown(-1))!=NDK_ERR_PARA/*NDK_ERR_OPEN_DEV*/)
	{
		send_result("line %d:%s测试失败(%d,%d,%d,%d)", __LINE__, TESTAPI, ret, ret1, ret2, ret3);
		goto ERR;
	}

	//case2:IC/SAM卡正常测试
	cls_show_msg1(3, "请插入接触式IC卡和SAM卡槽的所有SAM卡,任意键继续...");
	i=0;
	do
	{
		//case1:IC卡上电测试
		//case2:IC卡下电测试
		memset(buf, 0, sizeof(buf));
		if((ret=NDK_IccPowerUp(CardNo[i], buf, &pnAtrlen))!=NDK_OK||(ret1=NDK_IccPowerUp(CardNo[i], buf, &pnAtrlen))!=NDK_OK)//连续上
		{
			if(CardNo[i]!=0xff)
			{
				send_result("line %d:%s测试失败(%s:%d,%d)", __LINE__, TESTAPI, CardNoStr[i], ret, ret1);
				GOTOERR;
			}
		}
		else
		{
			//case3:非法参数测试，上电应失败
			if(CardNo[i]==0xff)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				GOTOERR;
			}
		}
		if((ret=NDK_IccPowerDown(CardNo[i]))!=NDK_OK||(ret=NDK_IccPowerDown(CardNo[i]))!=NDK_OK)//连续下
		{
			if(CardNo[i]!=0xff)
			{
				send_result("line %d:%s测试失败(%s:%d,%d)", __LINE__, TESTAPI, CardNoStr[i], ret, ret1);
				GOTOERR;
			}
		}
		else//下电已成功
		{
			if(CardNo[i]==0xff)
			{
				send_result("line %d:%s测试失败", __LINE__, TESTAPI);
				GOTOERR;
			}
			else if((ret=NDK_Iccrw(CardNo[i], sizeof(sendbuf), sendbuf, &rlen, buf))!=NDK_ERR_IOCTL)//zhangnw201105add 下电后读写应失败
			{
				//NDK_IccPowerDown(CardNo[i]);//防一手
				send_result("line %d:%s测试失败(%s:%d)", __LINE__, TESTAPI, CardNoStr[i], ret);
				GOTOERR;
			}
		}
	}
	while(++i<=(maxid-1));
	
#if	IC1_ENABLE&&(SAM1_ENABLE||SAM2_ENABLE)//支持SAM1卡的产品才进行进行以下测试//有支持IC卡才测试 20180510
	//case 3:IC卡上电后，SAM也上电，接着IC卡读写，然后SAM卡读写操作(所有SAM卡操作完毕)，IC卡下电
	memset(buf,0,sizeof(buf));
	if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&pnAtrlen))!=NDK_OK)
	{	
		send_result("line %d:IC1上电失败(%d)", __LINE__, ret);
		GOTOERR;
	}
	//SAM卡操作
	i=1;
	do
	{
		memset(buf,0,sizeof(buf));
		if ((ret=NDK_IccPowerUp(CardNo[i],buf,&pnAtrlen))!=NDK_OK)
		{	
			send_result("line %d:%s上电失败(%d)", __LINE__, CardNoStr[i], ret);
			GOTOERR;
		}

		//IC读写
		memset(buf,0,sizeof(buf));
		if((ret=NDK_Iccrw(ICTYPE_IC,sizeof(sendbuf),sendbuf,&pnAtrlen,buf))!=NDK_OK)
		{		
			send_result("line %d:IC1读卡失败(%d)", __LINE__, ret);
			GOTOERR;															
		}
		if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
		{
			send_result("line %d:IC1取随机数失败(SW:0x%04x)", __LINE__,ret);
			GOTOERR;								
		}	
		
		//sam读写
		memset(buf,0,sizeof(buf));
		if((ret=NDK_Iccrw(CardNo[i],sizeof(sendbuf),sendbuf,&pnAtrlen,buf))!=NDK_OK)
		{						
			send_result("line %d:%s读卡失败(%d)", __LINE__, CardNoStr[i], ret);
			GOTOERR;															
		}
		if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
		{
			send_result("line %d:%s取随机数失败(SW:0x%04x)", __LINE__, CardNoStr[i], ret);
			GOTOERR;								
		}	
		
		//sam卡下电
		if ((ret=NDK_IccPowerDown(CardNo[i]))!=NDK_OK)
		{				
			send_result("line %d:%s下电失败(0x%x)", __LINE__, CardNoStr[i], ret);
			RETURN;
		}
		//sam卡操作结束
	}while(++i<(maxid-1));
	//IC下电
	if ((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
	{				
		send_result("line %d:%s下电失败(0x%x)", __LINE__, CardNoStr[0], ret);
		RETURN;
	}

	//case 4:IC、SAM均上电，IC卡读写并下电，然后，SAM卡读写并下电；	
	i=1;
	do
	{
		memset(buf,0,sizeof(buf));
		if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&pnAtrlen))!=NDK_OK)
		{	
			send_result("line %d:IC1上电失败(%d)", __LINE__, ret);
			GOTOERR;
		}
		memset(buf,0,sizeof(buf));
		if ((ret=NDK_IccPowerUp(CardNo[i],buf,&pnAtrlen))!=NDK_OK)
		{	
			send_result("line %d:%s上电失败(%d)", __LINE__,CardNoStr[i], ret);
			GOTOERR;
		}
		//IC读写
		memset(buf,0,sizeof(buf));
		pnAtrlen = 0;
		if((ret=NDK_Iccrw(ICTYPE_IC,sizeof(sendbuf),sendbuf,&pnAtrlen,buf))!=NDK_OK)
		{		
			send_result("line %d:IC1读卡失败(%d)", __LINE__, ret);
			GOTOERR;															
		}
		if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
		{
			send_result("line %d:IC1取随机数失败(SW:0x%04x)", __LINE__,ret);
			GOTOERR;								
		}	
		//IC下电
		if ((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
		{				
			send_result("line %d:%s下电失败(%d)", __LINE__, CardNoStr[0], ret);
			RETURN;
		}
		//sam读写
		memset(buf,0,sizeof(buf));
		pnAtrlen = 0;
		if((ret=NDK_Iccrw(CardNo[i],sizeof(sendbuf),sendbuf,&pnAtrlen,buf))!=NDK_OK)
		{						
			send_result("line %d:%s读卡失败(%d)", __LINE__,CardNoStr[i], ret);
			GOTOERR;															
		}
		if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
		{
			send_result("line %d:%s取随机数失败(SW:0x%04x)", __LINE__,CardNoStr[i],ret);
			GOTOERR;								
		}	
		//sam卡下电
		if ((ret=NDK_IccPowerDown(CardNo[i]))!=NDK_OK)
		{				
			send_result("line %d:%s下电失败(%d)", __LINE__, CardNoStr[i], ret);
			RETURN;
		}
		//sam卡操作结束
	}while(++i<(maxid-1));
	
	//case5:IC、SAM均上电，SAM卡读写并下电，然后，IC卡读写并下电；
	i=1;
	do
	{
		//ic卡上电
		memset(buf,0,sizeof(buf));
		if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&pnAtrlen))!=NDK_OK)
		{	
			send_result("line %d:IC1上电失败(%d)", __LINE__, ret);
			GOTOERR;
		}
		//sam卡上电
		memset(buf,0,sizeof(buf));
		if ((ret=NDK_IccPowerUp(CardNo[i],buf,&pnAtrlen))!=NDK_OK)
		{	
			send_result("line %d:%s上电失败(%d)", __LINE__, CardNoStr[i], ret);
			GOTOERR;
		}
		//sam读写
		memset(buf,0,sizeof(buf));
		pnAtrlen = 0;
		if((ret=NDK_Iccrw(CardNo[i],sizeof(sendbuf),sendbuf,&pnAtrlen,buf))!=NDK_OK)
		{						
			send_result("line %d:%s读卡失败(%d)", __LINE__, CardNoStr[i], ret);
			GOTOERR;
		}
		if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
		{
			send_result("line %d:%s取随机数失败(SW:0x%04x)", __LINE__, CardNoStr[i], ret);
			GOTOERR;
		}	
		//sam卡下电
		if ((ret=NDK_IccPowerDown(CardNo[i]))!=NDK_OK)
		{				
			send_result("line %d:%s下电失败(%d)", __LINE__, CardNoStr[i], ret);
			RETURN;
		}
		//IC读写
		memset(buf,0,sizeof(buf));
		pnAtrlen = 0;
		if((ret=NDK_Iccrw(ICTYPE_IC,sizeof(sendbuf),sendbuf,&pnAtrlen,buf))!=NDK_OK)
		{		
			send_result("line %d:IC1读卡失败(%d)", __LINE__, ret);
			GOTOERR;
		}
		if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
		{
			send_result("line %d:IC1取随机数失败(SW:0x%04x)", __LINE__,ret);
			GOTOERR;
		}	
		//IC下电
		if ((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
		{				
			send_result("line %d:%s下电失败(%d)", __LINE__, CardNoStr[0], ret);
			GOTOERR;
		}
	}while(++i<(maxid-1));
#endif
#if	SAM1_ENABLE
	if(auto_flag==0 || auto_flag==2)
	{
		if(cls_show_msg("是否进行6100SAM卡测试?是:[确认],否:[其他]")==ENTER)
		{
			//case6:针对6100SAM卡，执行一系列特定的apdu指令应该都能够成功(原来的bug是执行最后一条指令会返回异常)20171011根据高端代码新增
			cls_show_msg("请插入6100的SAM卡，任意键继续");
			//SAM上电
			memset(buf,0,sizeof(buf));
			if ((ret=NDK_IccPowerUp(ICTYPE_SAM1,buf,&pnAtrlen))!=NDK_OK)
			{	
				send_result("line %d:IC1上电失败(%d)", __LINE__, ret);
				GOTOERR;
			}
			//SAM APDU  req0  并进行校验
			memset(buf,0,sizeof(buf));
			pnAtrlen = 0;
			if((ret=NDK_Iccrw(ICTYPE_SAM1,sizeof(req0),req0,&pnAtrlen,buf))!=NDK_OK)
			{						
				send_result("line %d:SAM卡读卡失败(%d)", __LINE__, ret);
				GOTOERR;
			}
			if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
			{
				send_result("line %d:SAM卡取随机数失败(SW:0x%04x)", __LINE__, ret);
				GOTOERR;
			}	
			//SAM APDU  req1  并进行校验
			memset(buf,0,sizeof(buf));
			pnAtrlen = 0;
			if((ret=NDK_Iccrw(ICTYPE_SAM1,sizeof(req1),req1,&pnAtrlen,buf))!=NDK_OK)
			{						
				send_result("line %d:SAM卡读卡失败(%d)", __LINE__, ret);
				GOTOERR;
			}
			if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
			{
				send_result("line %d:SAM卡取随机数失败(SW:0x%04x)", __LINE__, ret);
				GOTOERR;
			}	
			//SAM APDU  req6100  并进行校验
			memset(buf,0,sizeof(buf));
			pnAtrlen = 0;
			if((ret=NDK_Iccrw(ICTYPE_SAM1,sizeof(req6100),req6100,&pnAtrlen,buf))!=NDK_OK)
			{						
				send_result("line %d:SAM卡读卡失败(%d)", __LINE__, ret);
				GOTOERR;
			}
			if((ret=SMART_GETSW(buf+pnAtrlen-2))!=0x9000&&(ret=SMART_GETSW(buf+pnAtrlen-2))!=0x6d00)
			{
				send_result("line %d:SAM卡取随机数失败(SW:0x%04x)", __LINE__, ret);
				GOTOERR;
			}	
			//下电
			if ((ret=NDK_IccPowerDown(ICTYPE_SAM1))!=NDK_OK)
			{				
				send_result("line %d:SAM卡下电失败(%d)", __LINE__,  ret);
				GOTOERR;
			}
		}
	}
#endif
	send_result("%s测试通过", TESTAPI);		
ERR://测试后置	
	icpowerdown_all();
	//NDK_IccSetType(ICTYPE_IC);//恢复默认
	return;
}

