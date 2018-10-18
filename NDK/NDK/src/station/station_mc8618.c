
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//#include"define.h"

#include "station.h"
#include "NDK.h"
//cdma_station_info station_info;


static int Mc8618_AnalyzeNcell(char *pData,ST_MOBILE_STATION_INFO * station_info)
{
    return 0;
}
static int Mc8618_AnalyzeLAI(char *pData,ST_MOBILE_STATION_INFO * station_info)
{
    char *pStr=NULL;
    char Command[80][40];
    if(NULL==pData) {
        //fprintf(stderr,"Analyze Data  param error");
        return STATION_PARAM_ERROR;
    }
    if(pStr=strstr(pData,"+CCED:")) {
        pStr+=7;
        AnalyzeData(pStr, Command);
        //fprintf(stderr,"command[0]=%s\n",Command[0]);
        station_info->ModuleStationInfo.CDMA_STATION.unMCC=460;
        station_info->ModuleStationInfo.CDMA_STATION.unMNC=3;
        //fprintf(stderr,"command[0]=%s\n",Command[1]);
        station_info->ModuleStationInfo.CDMA_STATION.unBandClass=0;
        //fprintf(stderr,"command[0]=%s\n",Command[2]);
        station_info->ModuleStationInfo.CDMA_STATION.unChannel=atoi(Command[1]);
        //fprintf(stderr,"command[0]=%s\n",Command[3]);
        station_info->ModuleStationInfo.CDMA_STATION.unBid=atoi(Command[2]);   
        station_info->ModuleStationInfo.CDMA_STATION.unSid=atoi(Command[3]);    
        station_info->ModuleStationInfo.CDMA_STATION.unNid=atoi(Command[6]);   

        fprintf(stderr,"%d,%d,%d,%d,%d,%d,%d\n",station_info->ModuleStationInfo.CDMA_STATION.unMCC,
                station_info->ModuleStationInfo.CDMA_STATION.unMNC,
                station_info->ModuleStationInfo.CDMA_STATION.unBandClass,
                station_info->ModuleStationInfo.CDMA_STATION.unChannel,
                station_info->ModuleStationInfo.CDMA_STATION.unBid,
                station_info->ModuleStationInfo.CDMA_STATION.unSid,
                station_info->ModuleStationInfo.CDMA_STATION.unNid);
    } else {
        fprintf(stderr,"I am here!\n");
        return STATION_LAI_ERROR;
    }
    //fprintf(stderr,"hello\n");
    return 0;
}

static int Mc8618_AnalyzeStationData(char *pData,ST_MOBILE_STATION_INFO * station_info)
{
    if(NULL==pData) {
        //fprintf(stderr,"Analyze Data  param error");
        return STATION_PARAM_ERROR;
    }
    station_info->emModuleType=MODULE_TYPE_CDMA;
    return Mc8618_AnalyzeLAI(pData,station_info);
    //fprintf(stderr,"hello\n");
    //Ce910_AnalyzeNcell(pData);
    //return 0;
}


int Mc8618_mobileStation(ST_MOBILE_STATION_INFO * station_info)
{
    //fprintf(stderr,"--------------Start Mc8618_mobileStation-----------\n");
    char sRecvStr[4096];
    int nRet;
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;

    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="+ZCED=0,1";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,2000,&pemStatus);


    if(nRet!=WLM_RET_OK) {
        return STATION_WLM_ERROR;
    }
    return Mc8618_AnalyzeStationData(sRecvStr,station_info);
    //return STATION_RETURN_OK;
}

static int Mc8618_AnalyzeLBSData(char *pData,LBSinfo * lbsinfo)
{
     if(NULL==pData) {
        //fprintf(stderr,"Analyze Data  param error");
        return STATION_PARAM_ERROR;
    }

    char *pStr=NULL;
    char Command[20][40];
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }

    if(pStr=strstr(pData,"+BSLATLONG:")) {
        AnalyzeData(pStr+12, Command);
        fprintf(stderr,"command[0]=%s\n",Command[0]);
        fprintf(stderr,"command[1]=%s\n",Command[1]);
        lbsinfo->RoughLatitude=atof(Command[0]);
        lbsinfo->RoughLongitude=atof(Command[1]);
        fprintf(stderr,"%f,%f\n",lbsinfo->RoughLatitude,lbsinfo->RoughLongitude);
    }
    
    return STATION_RETURN_OK;
}

int Mc8618_LBSinfo(LBSinfo *lbsinfo)
{
//  fprintf(stderr,"--------------Start Mc8618_mobileStation-----------\n");
    char sRecvStr[4096];
    int nRet;
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;

    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="+BSLATLONG?";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,2000,&pemStatus);
    if(nRet!=WLM_RET_OK) {
        fprintf(stderr,"LBS AT response error nRet=%d,srecvstr =%s\n",nRet,sRecvStr);
        return STATION_WLM_ERROR;
    }
    fprintf(stderr,"%s\n",sRecvStr);
    Mc8618_AnalyzeLBSData(sRecvStr,lbsinfo);

    return STATION_RETURN_OK;
}
