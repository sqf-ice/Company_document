#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//#include"define.h"
#include "NDK.h"
#include "station.h"

//gprs_station_info station_info;


#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//#include"define.h"
#include "NDK.h"
#include "station.h"

//gprs_station_info station_info;


int qw200_AnalyzeNcell(char *pData,ST_MOBILE_STATION_INFO *pstStationInfo)
{
    char *pStr=NULL;
    int nCellSerial=0;
    char Command[80][40];
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }
    if(pStr=strstr(pData,"+CCED:")) {
        //fprintf(stderr,"%s",pStr);
        AnalyzeData(pStr+7, Command);

        while(nCellSerial<6) {
            //fprintf(stderr,"nCellserial=%d\n",nCellSerial);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unMCC=atoi(Command[0+nCellSerial*7]);
            //fprintf(stderr,"n[nCellSerial].MMC=%d\n",pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unMCC);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unMNC=atoi(Command[1+nCellSerial*7]);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unLac=atoi(Command[2+nCellSerial*7]);
            //fprintf(stderr,"n[nCellSerial].Lac=%d\n",pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unLac);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unCi=atoi(Command[3+nCellSerial*7]);
            //fprintf(stderr,"n[nCellSerial].Ci=%d\n",pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unCi);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].ndbm=atoi(Command[6+nCellSerial*7]);
            //fprintf(stderr,"%x,%x,%d\n",pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unLac,pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unCi,pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].ndbm);
            nCellSerial++;
        }
    }
}
int qw200_AnalyzeLAI(char *pData,ST_MOBILE_STATION_INFO* pstStationInfo)
{

    char *pStr=NULL;
    char Command[30][40];
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }
    if(pStr=strstr(pData,"+CCED:")) {
        AnalyzeData(pStr+7, Command);
        //fprintf(stderr,"command[0]=%s\n",Command[0]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMCC=atoi(Command[0]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMNC=atoi(Command[1]);
        //fprintf(stderr,"command[0]=%s\n",Command[1]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unLac=atoi(Command[2]);
        //fprintf(stderr,"command[0]=%s\n",Command[2]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unCi=atoi(Command[3]);
        //fprintf(stderr,"command[0]=%s\n",Command[3]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.ndbm=atoi(Command[6]);
        //fprintf(stderr,"%d,%d,%x,%x,%d",pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMCC,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMNC,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unLac,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unCi,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.ndbm);
    }

    return STATION_RETURN_OK;
}


int qw200_AnalyzeStationData(char *pData,ST_MOBILE_STATION_INFO * pstStationInfo)
{
    return STATION_RETURN_OK;
}


int qw200_mobileStation(void * pstStationInfo)
{
    char sRecvStr[4096];
    int nRet;
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;

    /*获取主基站信息*/
    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="+CCED=0,1";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,2000,&pemStatus);
    if(nRet!=WLM_RET_OK) {
        return STATION_WLM_ERROR;
    }
    qw200_AnalyzeLAI(sRecvStr,pstStationInfo);


    /*获取临近基站信息*/
    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="+CCED=0,2";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,2000,&pemStatus);
    if(nRet!=WLM_RET_OK) {
        return STATION_WLM_ERROR;
    }
    qw200_AnalyzeNcell(sRecvStr,pstStationInfo);
//    qw200_AnalyzeStationData(sRecvStr,pstStationInfo);
    //fprintf(stderr,"%s__%d\n",__func__,__LINE__);
    return STATION_RETURN_OK;
}

int qw200_AnalyzeLBSData(char *pData,LBSinfo *lbsinfo)
{
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }
    char *pStr=NULL;
    char Command[20][40];
    if(pStr=strstr(pData,"+CIPGSMLOC:")) {
        AnalyzeData(pStr, Command);
        fprintf(stderr,"command[2]=%s\n",Command[2]);
        fprintf(stderr,"command[1]=%s\n",Command[1]);
        lbsinfo->RoughLatitude=atof(Command[2]);
        lbsinfo->RoughLongitude=atof(Command[1]);
        fprintf(stderr,"%f,%f\n",lbsinfo->RoughLatitude,lbsinfo->RoughLongitude);
    } else {
        return STATION_LAI_ERROR;
    }
    fprintf(stderr,"HELLOHELLO\n");
    return STATION_RETURN_OK;
}



int qw200_LBSinfo(LBSinfo *lbsinfo)
{
    //fprintf(stderr,"---------------Start qw200LBSINFO--------------\n");
    char sRecvStr[4096];
    int nRet;
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;

    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="+CZGLBS";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,2000,&pemStatus);
    if(nRet!=WLM_RET_OK) {
        fprintf(stderr,"LBS AT response error nRet=%d,srecvstr =%s\n",nRet,sRecvStr);
        return STATION_WLM_ERROR;
    }
    fprintf(stderr,"%s\n",sRecvStr);
    qw200_AnalyzeLBSData(sRecvStr,lbsinfo);
    return STATION_RETURN_OK;
}

