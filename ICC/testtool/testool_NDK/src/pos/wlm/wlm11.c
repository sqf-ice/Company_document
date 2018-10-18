/************************************************************************
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 无线模块
* file name		: wlm11.c
* Author 		:  
* version		: 
* DATE			:
* directory 		:
* description		: 测试NDK_WlGetStationInfoGeneral接口功能
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
#define TESTAPI	"NDK_WlGetStationInfoGeneral"	
#define	MAXWAITTIME	30
#define	WAITTIME	2

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void wlm8(void)
* functional description 	: 获取无线基站信息
* input parameter	 				:无
* output parameter	 			:无
* return value		 			:无
* history 		 				:  author		date		remarks
*			  	  	  				
*****************************************************************/
void wlm11(void)
{
	if(auto_flag==1)
	{
		send_result("%s自动测试不能作为最终测试结果，请结合手动测试验证", TESTAPI);
		//return;
	}
	/*private & local definition*/
	int ret = 0, i = 1;
	ST_MOBILE_STATION_INFO_GENERAL pstinfo;
	char *typestr[] = {"GPRS模块","CDMA模块","WCDMA模块" ,"TDSCDMA模块" ,"4G模块"};
   	 ST_STATION_INFO p;
    	ST_STATION_INFO *p1;
	ST_CDMA_STATION_INFO c;
	ST_STATION_INFO_IMPROVE g ;
	ST_STATION_INFO_IMPROVE *g1 ;
	float getinfo_time = 0.0;

	/*process body*/
	cls_printf("测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	
	cls_show_msg1(MAXWAITTIME, "本次测试要求在插卡和未插卡两种情况下分别测试", TESTAPI);
	//case1:异常测试
	if((ret = NDK_WlGetStationInfoGeneral(STATION_TYPE_DEFAULT-1,&pstinfo))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WlGetStationInfoGeneral(STATION_TYPE_CDMAHDR+1,&pstinfo))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_WlGetStationInfoGeneral(STATION_TYPE_DEFAULT,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:不论是否成功,快速定位耗时均不应超过2s
	memset(&pstinfo,0,sizeof(pstinfo)); 
	lib_StartStopwatch();
	if((ret = NDK_WlGetStationInfoGeneral(STATION_TYPE_DEFAULT,&pstinfo))!=NDK_OK)
	{
		send_result("line %d: %s获取基站信息失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((getinfo_time=lib_ReadStopwatch())> WAITTIME)
	{
		send_result("line %d: %s测试失败,定位时间超过2秒(%f)", __LINE__, TESTAPI, getinfo_time);
		RETURN;
	}
	else
	{
		send_result("快速定位时间为%f", getinfo_time);
	}
	
	//case3:正常测试:Station_type为0既能获取4G模块基站又能获取2G模块基站
	if(pstinfo.emModuleType<MODULE_TYPE_GPRS||pstinfo.emModuleType>MODULE_TYPE_LTE)
	{
		send_result("line %d: %s测试失败(未知的模块类型:%d)", __LINE__, TESTAPI, pstinfo.emModuleType);
		RETURN;
	}
	send_result("获取到的模块类型为:%s,请判断是否正确", typestr[pstinfo.emModuleType]);	
	switch(pstinfo.emModuleType)
	{
		case MODULE_TYPE_GPRS:
			cls_show_msg1(1,"GPRS模块");
            		p=pstinfo.ModuleStationInfo.GPRS_STATION.stMainStation;
			send_result("主基站信息,国家号:%d,网络号:%d,位置区号码:%d,小区号码:%d,信号强度:%d,请判断是否正确",p.unMCC,p.unMNC,p.unLac,p.unCi,p.ndbm);
          		p1=pstinfo.ModuleStationInfo.GPRS_STATION.stNeighborStation;
            		for(i=1;i<7;i++,p1++)
				send_result("相邻基站%d信息,国家号:%d,网络号:%d,位置区号码:%d,小区号码:%d,信号强度:%d,请判断是否正确",i,(*p1).unMCC,(*p1).unMNC,(*p1).unLac,(*p1).unCi,(*p1).ndbm); 
            		break;
		case MODULE_TYPE_CDMA:
			cls_show_msg1(2,"CDMA模块");
			c=pstinfo.ModuleStationInfo.CDMA_STATION;
			send_result("主基站信息,国家号:%d,网络号:%d,宽带类型:%d,频道:%d,系统识别码:%d,网络识别码:%d,网络中的某小区:%d,请判断是否正确", c.unMCC, c.unMNC, c.unBandClass, c.unChannel, c.unBid, c.unSid, c.unNid);
			break;
		case MODULE_TYPE_WCDMA:
			cls_show_msg1(2,"WCDMA模块");
			break;
		case MODULE_TYPE_TDSCDMA:
			cls_show_msg1(2,"TDSCDMA模块");
			break;
		case MODULE_TYPE_LTE:
			cls_show_msg1(1,"4G模块");
            		g=pstinfo.ModuleStationInfo.LTE_STATION.stMainStation;
			send_result("主基站信息,国家号:%d,网络号:%d,位置区号码:%d,小区号码:%d,信号强度:%d,请判断是否正确",g.unMCC,g.unMNC,g.unLac,g.unCi,g.ndbm);
          		g1=pstinfo.ModuleStationInfo.LTE_STATION.stNeighborStation;
            		for(i=1;i<11;i++,g1++)
				send_result("相邻基站%d信息,国家号:%d,网络号:%d,位置区号码:%d,小区号码:%d,信号强度:%d,请判断是否正确",i,(*g1).unMCC,(*g1).unMNC,(*g1).unLac,(*g1).unCi,(*g1).ndbm); 
			break;
		default:
			cls_show_msg("不存在的模块");
			break;
	}
	
	//case4:如果EM_MODULE_TYPE为MODULE_TYPE_LTE那么就可以设Station_type测试不同的情况
	if(pstinfo.emModuleType==MODULE_TYPE_LTE)
	{	
		//station_type = 1 获取4G模块GSM基站信息
		memset(&pstinfo,0,sizeof(pstinfo)); 
		memset(&g,0,sizeof(g)); 
		if((ret = NDK_WlGetStationInfoGeneral(STATION_TYPE_GSM,&pstinfo))!=NDK_OK)
		{
			send_result("line %d: %s获取基站信息失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		g=pstinfo.ModuleStationInfo.LTE_STATION.stMainStation;
		send_result("主基站信息,国家号:%d,网络号:%d,位置区号码:%d,小区号码:%d,信号强度:%d,请判断是否正确",g.unMCC,g.unMNC,g.unLac,g.unCi,g.ndbm);
		g1=pstinfo.ModuleStationInfo.LTE_STATION.stNeighborStation;
		for(i=1;i<11;i++,g1++)
			send_result("相邻GSM基站%d信息,国家号:%d,网络号:%d,位置区号码:%d,小区号码:%d,信号强度:%d,请判断是否正确",i,(*g1).unMCC,(*g1).unMNC,(*g1).unLac,(*g1).unCi,(*g1).ndbm); 

		//station_type = 2 获取4G模块LTE基站信息 (上面已经获取过4G/LTE信息了这里不再获取)
		//station_type = 3 获取4G模块TDSCDMA基站信息
		memset(&pstinfo,0,sizeof(pstinfo)); 
		if((ret = NDK_WlGetStationInfoGeneral(STATION_TYPE_TDSCDMA,&pstinfo))!=NDK_OK)
		{
			send_result("line %d: %s获取基站信息失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		g1=pstinfo.ModuleStationInfo.LTE_STATION.stNeighborStation;
		for(i=1;i<11;i++,g1++)
			send_result("相邻TDSCDMA基站%d信息,国家号:%d,网络号:%d,位置区号码:%d,小区号码:%d,信号强度:%d,请判断是否正确",i,(*g1).unMCC,(*g1).unMNC,(*g1).unLac,(*g1).unCi,(*g1).ndbm); 

		//station_type = 5 获取4G模块STATION_TYPE_CDMAHDR基站信息
		memset(&pstinfo,0,sizeof(pstinfo)); 
		if((ret = NDK_WlGetStationInfoGeneral(STATION_TYPE_CDMAHDR,&pstinfo))!=NDK_OK)
		{
			send_result("line %d: %s获取基站信息失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		g1=pstinfo.ModuleStationInfo.LTE_STATION.stNeighborStation;
		for(i=1;i<11;i++,g1++)
			send_result("相邻CDMAHDR基站%d信息,国家号:%d,网络号:%d,位置区号码:%d,小区号码:%d,信号强度:%d,请判断是否正确",i,(*g1).unMCC,(*g1).unMNC,(*g1).unLac,(*g1).unCi,(*g1).ndbm); 
	}
	else // 如果为2G模块 传入除0外的参数均会返回参数失败
	{	
		memset(&pstinfo,0,sizeof(pstinfo)); 
		if((ret = NDK_WlGetStationInfoGeneral(STATION_TYPE_GSM,&pstinfo))!=NDK_ERR_PARA)
		{
			send_result("line %d: %s获取基站信息失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret = NDK_WlGetStationInfoGeneral(STATION_TYPE_TDSCDMA,&pstinfo))!=NDK_ERR_PARA)
		{
			send_result("line %d: %s获取基站信息失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret = NDK_WlGetStationInfoGeneral(STATION_TYPE_CDMAHDR,&pstinfo))!=NDK_ERR_PARA)
		{
			send_result("line %d: %s获取基站信息失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
	}
	
	//测试结束
	send_result("%s测试通过", TESTAPI);
	return;
}




