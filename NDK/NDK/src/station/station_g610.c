#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//#include"define.h"
#include "NDK.h"
#include "station.h"

//gprs_station_info station_info;

int AnalyzeStationData(char *Data, char Command[][128])
{
    if(NULL==Data||Command[0]==NULL) {
        fprintf(stderr,"Analyze Data  param error");
        return -1;
    }

    int j = 0, k = 0;
    char *pTemp=Data;
    while(*pTemp!='\0') {
        if(*pTemp=='\n') {
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
int G610_AnalyzeNcell(char *pData,ST_MOBILE_STATION_INFO *pstStationInfo)
{
    char *pStr=NULL;
    int nCellSerial=1;
    char Command[80][128];
    char szTmpBuf[128];
    char *pszStart=NULL,*pszEnd=NULL;
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }

    if(pStr=strstr(pData,"MCC")) {

	AnalyzeStationData(pStr, Command);
        while(nCellSerial<7/*5*/) {
            //fprintf(stderr,"nCellserial=%d\n",nCellSerial);
            
            //fprintf(stderr,"%s__%d\n",Command[4*nCellSerial],__LINE__);
            if (pszStart=strstr(Command[/*4*/nCellSerial], "LAC:")) {
                pszStart += strlen("LAC:");
				#if 0
                if ((pszEnd=strchr(pszStart, ','))!=NULL) {
                    *pszEnd = '\0';
                }
                memset(szTmpBuf,0,sizeof(szTmpBuf));
                strcpy(szTmpBuf, pszStart);
				#endif
                pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial-1].unLac=atoi(pszStart);          

            }
            //AnalyzeStationData(pStr, Command);

            if (pszStart=strstr(Command[/*4*/nCellSerial], "MCC:")) {
                pszStart += strlen("MCC:");
				#if 0
                if ((pszEnd=strchr(pszStart, ','))!=NULL) {
                    *pszEnd = '\0';
                }
				
                memset(szTmpBuf,0,sizeof(szTmpBuf));
                strcpy(szTmpBuf, pszStart);
				#endif
                pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial-1].unMCC=atoi(pszStart);

            }
           // AnalyzeStationData(pStr, Command);

            if (pszStart=strstr(Command[/*4*/nCellSerial], "MNC:")) {
                pszStart += strlen("MNC:");
				#if 0
                if ((pszEnd=strchr(pszStart, ','))!=NULL) {
                    *pszEnd = '\0';
                }
                memset(szTmpBuf,0,sizeof(szTmpBuf));
                strcpy(szTmpBuf, pszStart);
				#endif
                pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial-1].unMNC=atoi(pszStart);

            }
            //AnalyzeStationData(pStr, Command);

            if (pszStart=strstr(Command[/*4*/nCellSerial], "Cell ID:")) {
                pszStart += strlen("Cell ID:");
				#if 0
                if ((pszEnd=strchr(pszStart, ','))!=NULL) {
                    *pszEnd = '\0';
                }
                memset(szTmpBuf,0,sizeof(szTmpBuf));
                strcpy(szTmpBuf, pszStart);
				#endif
                pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial-1].unCi=atoi(pszStart);         
            }
            //AnalyzeStationData(pStr, Command);

            if (pszStart=strstr(Command[/*4*/nCellSerial], "RxDbm:")) {
                pszStart += strlen("RxDbm:");
				#if 0
                if ((pszEnd=strchr(pszStart, ' '))!=NULL) {
                    *pszEnd = '\0';
                }
                memset(szTmpBuf,0,sizeof(szTmpBuf));
                strcpy(szTmpBuf, pszStart);
				#endif

                pstStationInfo->ModuleStationInfo.GPRS_STATION.stNeighborStation[nCellSerial-1].ndbm=atoi(pszStart);

            }
            nCellSerial++;
        }
    }
}
int G610_AnalyzeLAI(char *pData,ST_MOBILE_STATION_INFO* pstStationInfo)
{

    char *pStr=NULL;
    char Command[30][128];
    char szTmpBuf[128];
    char *pszStart=NULL,*pszEnd=NULL;


    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }
    if(pStr=strstr(pData,"MCC")) {

        AnalyzeStationData(pStr, Command);
        //fprintf(stderr,"%s__%d\n",Command[0],__LINE__);
        if (pszStart=strstr(Command[0], "MCC:")) {
            pszStart += strlen("MCC:");

            pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMCC=atoi(pszStart);

        }
        AnalyzeStationData(pStr, Command);
        if (pszStart=strstr(Command[0], "MNC:")) {
            pszStart += strlen("MNC:");

            pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMNC=atoi(pszStart);
        }

        AnalyzeStationData(pStr, Command);
        // fprintf(stderr,"%s__%d\n",Command[0],__LINE__);
        if (pszStart=strstr(Command[0], "LAC:")) {
            pszStart += strlen("LAC:");

            pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unLac=atoi(pszStart);

        }
        AnalyzeStationData(pStr, Command);
        if (pszStart=strstr(Command[0], "Cell ID:")) {
            pszStart += strlen("Cell ID:");

            pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unCi=atoi(pszStart);

        }
        AnalyzeStationData(pStr, Command);
        if (pszStart=strstr(Command[0], "RxDbm:")) {
            pszStart += strlen("RxDbm:");

            pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.ndbm=atoi(pszStart);

        }
//fprintf(stderr,"\n MainStation ndbm =%d\n",atoi(szTmpBuf));
        //  fprintf(stderr,"%s__%d\n",__func__,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMCC);
        //pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMNC=atoi(Command[0][12]);
        //pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unLac=strtol(Command[0][19],NULL,10);
        //pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unCi=strtol(Command[0][33],NULL,10);
        //pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.ndbm=atoi(Command[0][83]);
        //fprintf(stderr,"%d,%d,%x,%x,%d",pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMCC,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unMNC,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unLac,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.unCi,pstStationInfo->ModuleStationInfo.GPRS_STATION.stMainStation.ndbm);
    }

    return STATION_RETURN_OK;
}

int G610_AnalyzeStationData(char *pData,ST_MOBILE_STATION_INFO * pstStationInfo)
{
    if(NULL==pData) {
        return STATION_PARAM_ERROR;
    }
    //mobile_station_info * pstation_info=(mobile_station_info *)
    pstStationInfo->emModuleType=MODULE_TYPE_GPRS;

    G610_AnalyzeLAI(pData,pstStationInfo);
    G610_AnalyzeNcell(pData,pstStationInfo);
    return STATION_RETURN_OK;
}


int G610_mobileStation(void * pstStationInfo)
{
    char sRecvStr[4096];
    int nRet;
    EM_WLM_STATUS  pemStatus;
    ST_ATCMD_PACK atm_cmd;
    atm_cmd.AtCmdNo=WLM_CMD_UNDEFINE;
    atm_cmd.pcAtCmd="+MCELL=0,21";
    atm_cmd.pcAddParam=NULL;

    ndk_stop_sq_flush();
    nRet = ndk_NET_WLMSendAtCmd(atm_cmd.AtCmdNo, atm_cmd.pcAtCmd, atm_cmd.pcAddParam);
    if (nRet < 0) {
        ndk_allow_sq_flush();
        return STATION_WLM_ERROR;
    }


    nRet = ndk_NET_WLMGetAtRet(sRecvStr, 4096, 2000, 0);
    if (nRet < 0) {
        pemStatus = WLM_STATUS_ERROR;
        ndk_allow_sq_flush();
        return STATION_WLM_ERROR;
    } else {
        pemStatus = nRet;
        ndk_NET_WLMGetAtRet(sRecvStr, 4096, 100, 0);
        ndk_allow_sq_flush();
        G610_AnalyzeStationData(sRecvStr,pstStationInfo);
        return STATION_RETURN_OK;
    }

    
}

int G610_AnalyzeLBSData(char *pData,LBSinfo *lbsinfo)
{

    return 0;

}



int G610_LBSinfo(LBSinfo *lbsinfo)
{

    return 0;
}
