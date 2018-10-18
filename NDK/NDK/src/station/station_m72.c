#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//#include"define.h"
#include "NDK.h"
#include "station.h"

//gprs_station_info station_info;


int m72_AnalyzeNcell(char *pData,ST_MOBILE_STATION_INFO *pstStationInfo)
{
    char *pStr=NULL;
    int nCellSerial=0;
    char Command[80][40];
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }
    if(pStr=strstr(pData+20,"+QENG: 1")) {
        //fprintf(stderr,"%s",pStr);
        AnalyzeData(pStr, Command);

        while(nCellSerial<6) {
            //fprintf(stderr,"nCellserial=%d\n",nCellSerial);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unMCC=atoi(Command[7+nCellSerial*10]);
            //fprintf(stderr,"n[nCellSerial].MMC=%d\n",pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unMCC);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unMNC=atoi(Command[8+nCellSerial*10]);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unLac=strtol(Command[9+nCellSerial*10],NULL,16);
            //fprintf(stderr,"n[nCellSerial].Lac=%d\n",pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unLac);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unCi=strtol(Command[10+nCellSerial*10],NULL,16);
           // fprintf(stderr,"n[nCellSerial].Ci=%d\n",pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unCi);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].ndbm=atoi(Command[3+nCellSerial*10]);
            //fprintf(stderr,"%x,%x,%d\n",pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unLac,pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unCi,pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].ndbm);
            nCellSerial++;
        }
    }
}
int m72_AnalyzeLAI(char *pData,ST_MOBILE_STATION_INFO* pstStationInfo)
{

    char *pStr=NULL;
    char Command[30][40];
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }
    if(pStr=strstr(pData,"+QENG: 0")) {
        AnalyzeData(pStr, Command);
        //fprintf(stderr,"command[0]=%s\n",Command[0]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMCC=atoi(Command[1]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMNC=atoi(Command[2]);
        //fprintf(stderr,"command[0]=%s\n",Command[1]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unLac=strtol(Command[3],NULL,16);
        //fprintf(stderr,"command[0]=%s\n",Command[2]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unCi=strtol(Command[4],NULL,16);
        //fprintf(stderr,"command[0]=%s\n",Command[3]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.ndbm=atoi(Command[7]);
        //fprintf(stderr,"%d,%d,%x,%x,%d",pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMCC,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMNC,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unLac,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unCi,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.ndbm);
    }

    return STATION_RETURN_OK;
}

int m72_AnalyzeStationData(char *pData,ST_MOBILE_STATION_INFO * pstStationInfo)
{
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }
    //mobile_station_info * pstation_info=(mobile_station_info *)
    pstStationInfo->emModuleType=MODULE_TYPE_GPRS;
    m72_AnalyzeLAI(pData,pstStationInfo);
    m72_AnalyzeNcell(pData,pstStationInfo);
    return STATION_RETURN_OK;
}


int m72_mobileStation(void * pstStationInfo)
{
    char sRecvStr[4096];
    int nRet;
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;
    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="+QENG=1,1";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,2000,&pemStatus);
    if(nRet!=WLM_RET_OK) {
        return STATION_WLM_ERROR;
    }
    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="+QENG?";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,2000,&pemStatus);
    if(nRet!=WLM_RET_OK) {
        return STATION_WLM_ERROR;
    }
    m72_AnalyzeStationData(sRecvStr,pstStationInfo);
    //fprintf(stderr,"%s__%d\n",__func__,__LINE__);
    return STATION_RETURN_OK;
}

int m72_AnalyzeLBSData(char *pData,LBSinfo *lbsinfo)
{
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }
    char *pStr=NULL;
    char Command[20][40];
    if(pStr=strstr(pData,"+QGSMLOC:")) {
        AnalyzeData(pStr, Command);
        fprintf(stderr,"command[0]=%s\n",Command[2]);
        fprintf(stderr,"command[0]=%s\n",Command[1]);
        lbsinfo->RoughLatitude=atof(Command[2]);
        lbsinfo->RoughLongitude=atof(Command[1]);
        fprintf(stderr,"%f,%f\n",lbsinfo->RoughLatitude,lbsinfo->RoughLongitude);
    } else {
        return STATION_LAI_ERROR;
    }
    fprintf(stderr,"HELLOHELLO\n");
    return 0;

    return STATION_RETURN_OK;
}



int m72_LBSinfo(LBSinfo *lbsinfo)
{
    //fprintf(stderr,"---------------Start m72LBSINFO--------------\n");
    char sRecvStr[4096];
    int nRet;
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;

    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="+QGSMLOC=1";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,2000,&pemStatus);
    if(nRet!=WLM_RET_OK) {
        fprintf(stderr,"LBS AT response error nRet=%d,srecvstr =%s\n",nRet,sRecvStr);
        return STATION_WLM_ERROR;
    }
    fprintf(stderr,"%s\n",sRecvStr);
    m72_AnalyzeLBSData(sRecvStr,lbsinfo);
    return STATION_RETURN_OK;
}

