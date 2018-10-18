
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//#include"define.h"

#include "station.h"
#include "NDK.h"
//cdma_station_info station_info;


static int Ce910_AnalyzeNcell(char *pData,ST_MOBILE_STATION_INFO * station_info)
{
    return 0;
}
static int Ce910_AnalyzeLAI(char *pData,ST_MOBILE_STATION_INFO * station_info)
{
    char *pStr=NULL;
    char Command[80][40];
    if(NULL==pData) {
        //fprintf(stderr,"Analyze Data  param error");
        return STATION_PARAM_ERROR;
    }
    if(pStr=strstr(pData,"#CAI:")) {
        pStr+=6;
        AnalyzeData(pStr, Command);
        //fprintf(stderr,"command[0]=%s\n",Command[0]);
        station_info->ModuleStationInfo.CDMA_STATION.unMCC=460;
        station_info->ModuleStationInfo.CDMA_STATION.unMNC=3;
        //fprintf(stderr,"command[0]=%s\n",Command[1]);
        station_info->ModuleStationInfo.CDMA_STATION.unBandClass=0;
        //fprintf(stderr,"command[0]=%s\n",Command[2]);
        station_info->ModuleStationInfo.CDMA_STATION.unChannel=atoi(Command[4]);
        //fprintf(stderr,"command[0]=%s\n",Command[3]);
        station_info->ModuleStationInfo.CDMA_STATION.unBid=atoi(Command[2]);
        station_info->ModuleStationInfo.CDMA_STATION.unSid=atoi(Command[0]);
        station_info->ModuleStationInfo.CDMA_STATION.unNid=atoi(Command[1]);

        fprintf(stderr,"%d,%d,%d,%d,%d,%d,%d\n",station_info->ModuleStationInfo.CDMA_STATION.unMCC,
                station_info->ModuleStationInfo.CDMA_STATION.unMNC,
                station_info->ModuleStationInfo.CDMA_STATION.unBandClass,
                station_info->ModuleStationInfo.CDMA_STATION.unChannel,
                station_info->ModuleStationInfo.CDMA_STATION.unBid,
                station_info->ModuleStationInfo.CDMA_STATION.unSid,
                station_info->ModuleStationInfo.CDMA_STATION.unNid);
    } else {
        return STATION_LAI_ERROR;
    }
    //fprintf(stderr,"hello\n");
    return 0;
}

static int Ce910_AnalyzeStationData(char *pData,ST_MOBILE_STATION_INFO * station_info)
{
    if(NULL==pData) {
        //fprintf(stderr,"Analyze Data  param error");
        return STATION_PARAM_ERROR;
    }
    station_info->emModuleType=MODULE_TYPE_CDMA;
    return Ce910_AnalyzeLAI(pData,station_info);
    //fprintf(stderr,"hello\n");
    //Ce910_AnalyzeNcell(pData);
    //return 0;
}


int Ce910_mobileStation(ST_MOBILE_STATION_INFO * station_info)
{
    //fprintf(stderr,"--------------Start Ce910_mobileStation-----------\n");
    char sRecvStr[4096];
    int nRet;
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;

    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="#CAI?";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,2000,&pemStatus);


    if(nRet!=WLM_RET_OK) {
        return STATION_WLM_ERROR;
    }
    return Ce910_AnalyzeStationData(sRecvStr,station_info);
    //return STATION_RETURN_OK;
}

static int Ce910_AnalyzeLBSData(char *pData,LBSinfo * lbsinfo)
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

    AnalyzeData(pData, Command);
    fprintf(stderr,"command[0]=%s\n",Command[1]);
    fprintf(stderr,"command[0]=%s\n",Command[2]);
    lbsinfo->RoughLatitude=atof(Command[1]);
    lbsinfo->RoughLongitude=atof(Command[2]);
    fprintf(stderr,"%f,%f\n",lbsinfo->RoughLatitude,lbsinfo->RoughLongitude);

    return 0;
}

char strLBSATcmd[6][26]= {
    "E0",
    "#SGACT=1,1,card,card",
    "#CGPADDR=1",
    "#CACHEDNS=1",
    "#AGPSSND",
    "#SGACT=1,0"
};

int Ce910_LBSinfo(LBSinfo *lbsinfo)
{
//  fprintf(stderr,"--------------Start Ce910_mobileStation-----------\n");
    char sRecvStr[4096];
    int nRet;
    int i,iCount;
    int ret;
    char *pLBSData=NULL;
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;



    for(i=0; i<4; i++) {
        atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
        atm_cmd.pcAtCmd=strLBSATcmd[i];
        atm_cmd.pcAddParam=NULL;
        nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,20*1000,&pemStatus);
        if(nRet!=WLM_RET_OK) {
            ret= STATION_WLM_ERROR;
            goto ERR;
        }
    }
    for(iCount=0; iCount<3; iCount++) {

        atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
        atm_cmd.pcAtCmd="#AGPSSND";
        atm_cmd.pcAddParam=NULL;
        nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,20*1000,&pemStatus);


        if((pLBSData=strstr(sRecvStr,"#AGPSRING:"))) {
            if(strstr(pLBSData,"Unable to get location")) {
                fprintf(stderr,"Get unable to get location retry!\n");
                continue;
            }
            fprintf(stderr,"recv %s\n",sRecvStr);
            ret=Ce910_AnalyzeLBSData(pLBSData,lbsinfo);
            break;
        } else {
            ret= STATION_WLM_ERROR;
            goto ERR;
        }
    }
ERR:
    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="#SGACT=1,0";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,20*1000,&pemStatus);
    if(nRet!=WLM_RET_OK) {
        ret= STATION_WLM_ERROR;
    }
    return ret;
}



