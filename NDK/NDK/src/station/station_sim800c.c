#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//#include"define.h"
#include "NDK.h"
#include "station.h"

//gprs_station_info station_info;


int sim800c_AnalyzeNcell(char *pData,ST_MOBILE_STATION_INFO *pstStationInfo)
{
    char *pStr=NULL;
    int nCellSerial=0;
    char Command[80][40];
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }
    if(pStr=strstr(pData,"0,")) {
        //fprintf(stderr,"%s",pStr);
        pData = pStr;
        pData = pData+10;
    }
    while(nCellSerial<6) {
        if(pStr=strstr(pData,"+CENG: ")) {
            //fprintf(stderr,"%s",pStr);
            pData = pStr;
            pData = pData+10;
            AnalyzeData(pStr+10, Command);
            //fprintf(stderr,"pData = %s \n",pData);

            //fprintf(stderr,"nCellserial=%d\n",nCellSerial);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unMCC=atoi(Command[4]);
            //fprintf(stderr,"n[nCellSerial].MMC=%d\n",pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unMCC);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unMNC=atoi(Command[5]);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unLac=strtol(Command[6],NULL,16);
            //fprintf(stderr,"n[nCellSerial].Lac=%d\n",pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unLac);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unCi=strtol(Command[3],NULL,16);
            //fprintf(stderr,"n[nCellSerial].Ci=%d\n",pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unCi);
            pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].ndbm=atoi(Command[1]);
            //fprintf(stderr,"%x,%x,%d\n",pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unLac,pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].unCi,pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial].ndbm);
            nCellSerial++;
        }
    }
}
int sim800c_AnalyzeLAI(char *pData,ST_MOBILE_STATION_INFO* pstStationInfo)
{
    char *pStr=NULL;
    char Command[30][40];
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }
    if(pStr=strstr(pData,"+CENG: 0")) {
        AnalyzeData(pStr+10, Command);
        //fprintf(stderr,"LAI.MMC=%s\n",Command[3]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMCC=atoi(Command[3]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMNC=atoi(Command[4]);
        //fprintf(stderr,"LAI.MNC=%s\n",Command[4]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unLac=strtol(Command[9],NULL,16);
        //fprintf(stderr,"LAI.LAC=%s\n",Command[9]);
        //fprintf(stderr,"LAI.CID=%s\n",Command[6]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unCi=strtol(Command[6],NULL,16);
        //fprintf(stderr,"LAI.DBM=%s\n",Command[1]);
        pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.ndbm=atoi(Command[1]);
        //fprintf(stderr,"%d,%d,%x,%x,%d",pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMCC,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMNC,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unLac,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unCi,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.ndbm);
    }

    return STATION_RETURN_OK;
}

int sim800c_AnalyzeStationData(char *pData,ST_MOBILE_STATION_INFO * pstStationInfo)
{
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }
    //mobile_station_info * pstation_info=(mobile_station_info *)
    pstStationInfo->emModuleType=MODULE_TYPE_GPRS;
    sim800c_AnalyzeLAI(pData,pstStationInfo);
    sim800c_AnalyzeNcell(pData,pstStationInfo);
    return STATION_RETURN_OK;
}


int sim800c_mobileStation(void * pstStationInfo)
{
    char sRecvStr[4096];
    int nRet;
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;
    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="+CENG=1,1";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,2000,&pemStatus);
    if(nRet!=WLM_RET_OK) {
        return STATION_WLM_ERROR;
    }
    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="+CENG?";
    atm_cmd.pcAddParam=NULL;
    nRet=NDK_WlSendATCmd(&atm_cmd,sRecvStr,4096,2000,&pemStatus);
    if(nRet!=WLM_RET_OK) {
        return STATION_WLM_ERROR;
    }
    fprintf(stderr,"sRecvStr = %s\n",sRecvStr);
    sim800c_AnalyzeStationData(sRecvStr,pstStationInfo);
    //fprintf(stderr,"%s__%d\n",__func__,__LINE__);
    return STATION_RETURN_OK;
}

int sim800c_AnalyzeLBSData(char *pData,LBSinfo *lbsinfo)
{
    return STATION_RETURN_OK;
}



int sim800c_LBSinfo(LBSinfo *lbsinfo)
{
    return STATION_RETURN_OK;
}
