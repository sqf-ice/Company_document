
/**
*@file      stationc
*@brief     基站接口API
*@version   1.0.0
*@author        linsx
*@date      2012-09-06
*@section   history     修改历史
                \<author\> \<time\> \<version\> \<desc\>
*/

#include <stdio.h>
#include <string.h>

//#include "define.h"
#include "NDK.h"
#include "station.h"
#include "../wirelessmodem/wirelessmodem.h"

extern int m72_mobileStation(void * pstStationInfo);
extern int m72_LBSinfo(LBSinfo *lbsinfo);
extern int Ce910_mobileStation(ST_MOBILE_STATION_INFO * station_info);
extern int Ce910_LBSinfo(LBSinfo * lbsinfo);
extern int De910_mobileStation(ST_MOBILE_STATION_INFO * station_info);
extern int De910_LBSinfo(LBSinfo * lbsinfo);
extern int G610_mobileStation(ST_MOBILE_STATION_INFO * station_info);
extern int G610_LBSinfo(LBSinfo * lbsinfo);
extern int Mc8618_mobileStation(ST_MOBILE_STATION_INFO * station_info);
extern int Mc8618_LBSinfo(LBSinfo * lbsinfo);
extern int qw200_mobileStation(ST_MOBILE_STATION_INFO * station_info);
extern int qw200_LBSinfo(LBSinfo * lbsinfo);
extern int sim800c_mobileStation(ST_MOBILE_STATION_INFO * station_info);
extern int sim800c_LBSinfo(LBSinfo * lbsinfo);
int m_StationInitFlag=0;
//static int m_WlmModuleType=-1;
ST_MOBILE_STATION_INFO   g_mobileStationInfo;
//int gInfo_len=0;
LBSinfo g_lbsinfo;

WLMStationFuc WlmFucList[]= {
    //{WLM_GPRS_SM300, sim300_mobileStation,sim300_LBSinfo},
    {WLM_GPRS_M72, m72_mobileStation,m72_LBSinfo},
    //{WLM_CDMA_DTM228C,Dtg228c_mobileStation,Dtg228c_LBSinfo},
    {WLM_CDMA_CE910,Ce910_mobileStation,Ce910_LBSinfo},
    {WLM_EVDO_DE910,De910_mobileStation,De910_LBSinfo},
    {WLM_GPRS_G610,G610_mobileStation,G610_LBSinfo},
    {WLM_EVDO_DE910_USB,De910_mobileStation,De910_LBSinfo},
    {WLM_GPRS_QW200,qw200_mobileStation,qw200_LBSinfo},
    {WLM_GPRS_SIM800C,sim800c_mobileStation,sim800c_LBSinfo},
    {WLM_CDMA_MC8618,Mc8618_mobileStation,Mc8618_LBSinfo},
    {WLM_NONE, NULL,}
};
WLMStationFuc *g_pstWlmFuc=(WLMStationFuc *)0;

/**
*@fn        int init_station_env(void)
*@brief     初始化基站信息以及无线模块
*@param     无
*@return        SUCC ---0，FAIL----(-1)
*@section   history     修改历史
            \<author\>  \<time\>    \<desc\>
*/
static int  init_station_env()
{
    int wlm_type_no;
    if (!m_StationInitFlag) {
        wlm_type_no=ndk_WLMGetType();
        g_pstWlmFuc=&WlmFucList;
        while(g_pstWlmFuc->type!=WLM_NONE) {
            if (g_pstWlmFuc->type==wlm_type_no) {
                m_StationInitFlag=1;
                return 0;
            }
            g_pstWlmFuc ++;
        }
        g_pstWlmFuc = NULL;
        return UNKOWN_WLM_DRIVER;
    }
    return 0;
}
int AnalyzeData(char *Data, char Command[][40])
{
    if(NULL==Data||Command[0]==NULL) {
        fprintf(stderr,"Analyze Data  param error");
        return -1;
    }
    //int i = 0;
    int j = 0, k = 0;
    char *pTemp=Data;
    while(*pTemp!='\0') {
        if(*pTemp=='\x0d'||*pTemp=='\x0a') {
            break;
        }
        if(*pTemp == ',') {
            Command[j][k] = 0;
            j ++;
            k = 0;
            pTemp ++;
            continue;
        }
        Command[j][k] =*pTemp;
        k ++;
        pTemp ++;
    }
    Command[j][k]=0;
    return j;
}

int ReadStationInfo()
{
    init_station_env();

    if(g_pstWlmFuc==NULL) {
        fprintf(stderr,"g_pstWlmFuc is NULL\n");
        return UNKOWN_WLM_DRIVER;
    } else
        return (g_pstWlmFuc->ReadStationFuc(&g_mobileStationInfo));
}
int ReadLBSInfo()
{
    init_station_env();
    if(g_pstWlmFuc==NULL) {
        fprintf(stderr,"g_pstWlmFuc is NULL\n");
        return UNKOWN_WLM_DRIVER;
    } else
        return (g_pstWlmFuc->ReadLBSFuc(&g_lbsinfo));
}

int WLM_GetNeighborStation(void *neighborStation,int len)
{
    return 0;
//  int nRet;
//  //nRet=ReadStationInfo();
//  if(nRet>=0)
//  {
//
//  }
}

int WLM_GetCurrentStation(int *Lac,int *Cellid)
{
    return 0;
//  int nRet;
//  //nRet=ReadStationInfo();
//  if(nRet>=0)
//  {
//
//  }
}


/**
 *@brief    获取本基站和相邻基站的信息，包括运营商，位置区号码，小区号

 *@retval   pstStationInfo  执行成功返回基站信息，失败返回 错误
 *@return
 *@li   NDK_OK              操作成功
 *@li   NDK_ERR_PARA    参数非法(pstStationInfo为NULL)
*/


NEXPORT int NDK_WlGetStationInfo(ST_MOBILE_STATION_INFO * pstStationInfo)
{
    if(pstStationInfo==NULL)
        return NDK_ERR_PARA;
    int nRet;
    //gInfo_len=len;
    nRet=ReadStationInfo();
    if(nRet>=0) {
        memcpy(pstStationInfo,&g_mobileStationInfo,sizeof(g_mobileStationInfo));
        return 0;
    } else
	{
		if(nRet == UNKOWN_WLM_DRIVER)
			return NDK_ERR_NOT_SUPPORT;
		return nRet;
	}
}

int WLM_GetLBSInfo(LBSinfo *lbsinfo)
{
    if(lbsinfo==NULL)
        return STATION_PARAM_ERROR;
    int nRet;
    memset(&g_lbsinfo,0,sizeof(LBSinfo));
    nRet=ReadLBSInfo();
    if(nRet>=0) {
        int len1=sizeof(g_lbsinfo);
        int len2 =sizeof(LBSinfo);
        fprintf(stderr,"len1=%d,len2=%d\n",len1,len2);
        memcpy(lbsinfo,&g_lbsinfo,sizeof(g_lbsinfo));
        return 0;
    } else
        return nRet;
}


