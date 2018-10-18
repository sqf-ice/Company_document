/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线模块
* file name		: wlm1.c
* Author 		: chensj
* version		: 
* DATE			: 20130105
* directory 		: 
* description		: 测试NDK_WlInit接口功能
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
#define TESTAPI	"NDK_WlInit"	
#define	WLM_TIMEOUT	2000
#define	MAXWAITTIME	30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlm1(void)
* functional description 	: 无线MODEM初始化，切换串口到无线并判断模块AT指令能否正常响应，检测SIM卡 
* input parameter	 				:无
* output parameter	 			:无
* return value		 				:无
* history 		 						:  author		date		remarks
*			  	  	  						chensj   20130105	created
*****************************************************************/
void wlm1(void)
{
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证",TESTAPI);
		return;
	}
	/*private & local definition*/
	int nKeyin = 0;
	char skip = 0x00;
	EM_WLM_STATUS emStatus = -1;
	int nRet=0;
	
	/*process body*/
	/*依次测试如下情况:
	-201  无卡   （应用层一开机返回这个值时有可能是模块刚起卡还没有上电，需多调用几次进行验证）
	下面两种情况不是检测时出现的情况，而是用于解锁时出现的情况
	-203  PIN码锁定(在卡锁定且用为NULL的PIN串去解锁的情况)
	-202  PIN码输入错误(在卡锁定且用为错误的PIN串去解锁的情况)
	-204  未定义的情况
	*/
	//case1:异常参数测试
	if((nRet = NDK_WlInit(-1,NULL,&emStatus)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	if((nRet = NDK_WlInit(WLM_TIMEOUT,NULL,NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}

	//正常卡片的测试
	if((nRet=NDK_WlModemClose())!=NDK_OK)//关闭无线模块
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "测试%s...请先确保机具内已插入了正常SIM卡,ENTER确认", TESTAPI);//插上正常SIM卡
	if (nKeyin==ENTER)//视为测试者已确保了插入正常卡
	{
#if  !(CPU5810X_ENABLE||defined ME50NX||defined ME68) //低端无线模块设计开机后自动上电初始化,且不提供关闭函数,该用例不测试
		//case7:无线模块未打开情况下，初始化应该返回AT命令响应失败NDK_ERR_WLM_SEND_AT_FAIL 20140617 added linwl
		if ((nRet = NDK_WlInit(WLM_TIMEOUT,NULL,&emStatus))!=NDK_ERR_WLM_SEND_AT_FAIL||emStatus==WLM_STATUS_OK)//NDK_ERR_WLM_SEND_AT_FAIL
		{
			send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, nRet, emStatus);
			RETURN;
		}
#endif
#if !(defined ME68 || defined ME50NX) //AC35平台产品不支持复位  20180416 sull modify
		if((nRet=NDK_WlModemReset())!=NDK_OK)//插卡后无线重新上电
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		sleep(20);//等待复位完成 复位时间各模块不一样最长的可能需要20s 20170921 
#endif
		//case6:插入正常SIM卡应该能够初始化成功
		if ((nRet = NDK_WlInit(WLM_TIMEOUT,NULL,&emStatus))!=NDK_OK||emStatus!=WLM_STATUS_OK)
		{
			send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, nRet, emStatus);
			RETURN;
		}
	}
	
	//无卡情况测试
	if((nRet=NDK_WlModemClose())!=NDK_OK)//关闭无线模块
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "测试%s...请先确保机具内没有SIM卡(关机后拔卡),ENTER确认无卡", TESTAPI);//拔卡
	if (nKeyin==ENTER)//视为测试者已确保了无卡
	{
#if !(defined ME68 || defined ME50NX) //AC35平台产品不支持复位  20180416 sull modify
		if((nRet=NDK_WlModemReset())!=NDK_OK)//拔卡后无线重新上电
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		sleep(20);//等待复位完成 复位时间各模块不一样最长的可能需要20s 20170921 
#endif
		//case2:测试NDK_ERR_NO_SIMCARD,无卡
		if ((nRet = NDK_WlInit(WLM_TIMEOUT,"1234",&emStatus))!=NDK_ERR_NO_SIMCARD)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
	}
	else if (nKeyin==0)//超时
	{
		skip |= 0x01;
	}
	else
	{
		send_result("line %d:用户中断测试",  __LINE__);
		RETURN;
	}

	//被锁定的卡片测试
	if((nRet=NDK_WlModemClose())!=NDK_OK)//关闭无线模块
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
		RETURN;
	}
	nKeyin = cls_show_msg1(MAXWAITTIME, "测试%s...请先确保机具内已插入了被锁SIM卡(关机后拔插卡),ENTER确认", TESTAPI);//插上被锁的SIM卡
	if (nKeyin==ENTER)//视为测试者已确保了插入被锁卡
	{
#if !(defined ME68 || defined ME50NX) //AC35平台产品不支持复位  20180416 sull modify
		if((nRet=NDK_WlModemReset())!=NDK_OK)//插卡后无线重新上电
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		sleep(20);//等待复位完成 复位时间各模块不一样最长的可能需要20s 20170921 
#endif

		//case3:测试NDK_ERR_PIN_LOCKED,PIN码锁定(在卡锁定且用为NULL的PIN串去解锁的情况)
		if ((nRet = NDK_WlInit(WLM_TIMEOUT,NULL,&emStatus))!=NDK_ERR_PIN_LOCKED)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}

		//case4:测试NDK_ERR_PIN,PIN码输入错误(在卡锁定且用为错误的PIN 串去解锁的情况)
		if ((nRet = NDK_WlInit(WLM_TIMEOUT,"test",&emStatus))!=NDK_ERR_PIN)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}
		
		//case5:测试NDK_OK,正常解锁(一般厂默认是1234),卡索的后使用正确的PIN码进行解锁应该成功
		if ((nRet = NDK_WlInit(WLM_TIMEOUT,"1234",&emStatus))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, nRet);
			RETURN;
		}		
	}
	else if (nKeyin==0)
	{
		skip |= (0x01<<1);
	}
	else
	{
		send_result("line %d:用户中断测试", __LINE__);
		NDK_WlModemReset();//避免取消测试时无线模块没有复位，处在关闭状态引起后续用例失败 20140707 linwl
		sleep(20);//等待复位完成  复位时间各模块不一样最长的可能需要20s 20170921 
		RETURN;
	}
	
	//NDK_ERR_PIN_UNDEFINE未定义的情况暂时无法构造
	//emStatus状态参数其它状态值怎么构造

	//test over 
	if (skip!=0x00)
		send_result("%s测试未进行(%d)", TESTAPI, skip);
	else
		send_result("%s测试通过", TESTAPI);
	return;
	
}

