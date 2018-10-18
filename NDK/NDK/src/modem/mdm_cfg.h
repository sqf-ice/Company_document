/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-配置管理
 * 作    者：    产品开发部
 * 日    期：    2013-12-10
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */

#ifndef MDM_CFG_INCLUDE_H
#define MDM_CFG_INCLUDE_H

#define USER_CONFIG_FILE        "/appfs/etc/sys.conf"
#define MDM_SETTING_NAME        "mdm"
#define ASYN_CFG_NAMEHEAD       "asyn_"

typedef struct {
    int nDbmLevel;              //modem_volt
    int nAnserToneTime;         //answertone_detect_time
    int nDialoneValue;          //modem_voice
    int nCountryFlag;           //choose_country
    int nDialoneTimeValue;      //modem_voice_time
    int nBps;                   //baud_freq
    int nCommType;              //ccitt_bell
    int nFrameS7;               //frame_s7
    int nFrameS10;              //frame_s10
    int nDTMF;                  //modem_dtmf
    int nLineVolt;              //line_volt
    int nDataTimeOut;           //frame_data
    int nFrameRst;              //frame_rst
    int nImpedance;             //conexant.Impedance
    int nParamAdaptFlag;        //parameter adapt flag
    int nRecSignaldetFlag;      //RecSignalDetect flag
	int	nCetRRdetFlag;         //Connect judge with RR frame  detect flag : 0, not detect RR 1, detect RR
	int nCetRejSendFlag;		//Recieve Repeat package whether send REJ Package : 0, no 1, ye
    int nAsynProtocolFlag;      //asyn_protocol
    int nAsynDbmLevel;          //asyn_modem_volt
    int nAsynAnserToneTime;     //asyn_answertone_detect_time
    int nAsynDialoneValue;      //asyn_modem_voice
    int nAsynDialoneTimeValue;  //asyn_modem_voice_time
    int nAsynFrameS7;           //asyn_frame_s7
    int nAsynFrameS10;          //asyn_frame_s10
    int nAsynDTMF;              //asyn_modem_dtmf
    int nAsynImpedance;         //conexant.AYSN Impedance
} ST_MDM_CFG;

typedef struct {
    const char *    cfgname;        //配置项名称
    int *       cfgvalueadr;    //配置值的地址
} ST_MDM_CFG_LIST;

int mdm_cfg_init(void);
int mdm_cfg_save(void);

#endif
