/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: icc模块
* file name		: 
* Author 			: chenfm
* version			: 
* DATE			: 20130107
* directory 		: 
* description		: 测试 NDK_IccSetType, NDK_IccGetType接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI		"NDK_IccSetType,NDK_IccGetType"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: icc2
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	 	 chenfm     20130107	created
*				  	 
*****************************************************************/
void icc2(void)
{
	if(auto_flag==2)
		return;
	//以下为局部变量
	int ret= -1, j=0;
	EM_ICTYPE pemIctype;
	char IC_TYPE[][16]={
#if IC1_ENABLE	 //默认有修改为IC1_ENABLE 20180510 
						"IC", 
#endif
#if SAM1_ENABLE	//20140218 跟开发确认K21不支持小卡
						"SAM1", 
#endif
#if SAM2_ENABLE	//20140218 跟开发确认K21不支持小卡
						"SAM2",
#endif
#if SAM3_ENABLE
						"SAM3",
#endif
#if SAM4_ENABLE
						"SAM4",
#endif
						"at24c32", "at24c01", "at24c02", "at24c04", "at24c08", "at24c16", "at24c32", "at24c64", "sle44x2", "sle44x8", "sle5528", "at88sc102", "at88sc1604", "at88sc1608", "at88sc153"};
	EM_ICTYPE IcType[]={
#if IC1_ENABLE	 //默认有修改为IC1_ENABLE 20180510 
						ICTYPE_IC, 
#endif
#if SAM1_ENABLE	//20140218 跟开发确认K21不支持小卡	
						ICTYPE_SAM1, 
#endif
#if SAM2_ENABLE //20140218 跟开发确认K21不支持小卡
						ICTYPE_SAM2,
#endif						
#if SAM3_ENABLE	
						ICTYPE_SAM3,
#endif
#if SAM4_ENABLE	
						ICTYPE_SAM4,
#endif
						ICTYPE_M_1, ICTYPE_M_1_1, ICTYPE_M_1_2, ICTYPE_M_1_4, ICTYPE_M_1_8, ICTYPE_M_1_16, ICTYPE_M_1_32, ICTYPE_M_1_64, ICTYPE_M_2, ICTYPE_M_3, ICTYPE_M_3, ICTYPE_M_4, ICTYPE_M_5, ICTYPE_M_6, ICTYPE_M_7};

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);

	//测试前置

	//case1:设置卡类型，获取卡类型应与所设置的一致
	for(j=0;j<SZ_ARRAY(IC_TYPE);j++)
	{
		if((ret=NDK_IccSetType(IcType[j]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%s:%d)", __LINE__, TESTAPI, IC_TYPE[j], ret);
			RETURN;
		}
		if((ret=NDK_IccGetType(&pemIctype))!=NDK_OK||pemIctype!=IcType[j])
		{
			send_result("line %d:%s测试失败(%d:%s, %d)", __LINE__, TESTAPI, pemIctype, IC_TYPE[j], ret);
			RETURN;
		}
	}

	//case3:先设置IC卡再设置SAM1卡，看获取到的应该是最后一次设置的SAM1卡 20140603 added by jiangym
	if((ret=NDK_IccSetType(IcType[0]))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%s:%d)", __LINE__, TESTAPI, IC_TYPE[0], ret);
		RETURN;
	}
	if((ret=NDK_IccSetType(IcType[1]))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%s:%d)", __LINE__, TESTAPI, IC_TYPE[1], ret);
		RETURN;
	}
	if((ret=NDK_IccGetType(&pemIctype))!=NDK_OK||pemIctype!=IcType[1])
	{
		send_result("line %d:%s测试失败(%s:%d)", __LINE__, TESTAPI, IC_TYPE[j], ret);
		RETURN;
	}

	//case2:异常测试
	if((ret=NDK_IccSetType(-1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_IccSetType(ICTYPE_M_1_64+1))!=NDK_ERR_PARA)//增加了07816类型设置，所以非法值要有+1改成+2 20140815 linwl  又新增了153卡20150325
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_IccGetType(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}	

	//恢复默认的设置
	if((ret=NDK_IccSetType(ICTYPE_IC))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret); // 恢复缺省设置
		RETURN;
	}
	if((ret=NDK_IccGetType(&pemIctype))!=NDK_OK||pemIctype!=ICTYPE_IC)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pemIctype);
		RETURN;
	}

	//test over
	send_result("%s测试通过", TESTAPI);
	return;
}

