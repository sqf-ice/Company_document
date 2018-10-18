

#ifndef __WLM__STATION__H__
#define __WLM__STATION__H__


/*
typedef enum DATA_TYPE
{
	NCELL_DATA,
	LAI_DATA,
}DATA_TYPE;
*/
#ifdef _DEBUG_STATION_
#define TRACE_STATION(fmt, args...) pos_debug(LOG_TYPE_STATION_API, fmt, ##args)
#else
#define TRACE_AT(fmt, args...)
#endif



#define WLSMDM_DEV_MAGIC    'W'
#define WLM_IOCG_OPEN			_IO(WLSMDM_DEV_MAGIC, 0)
#define WLM_IOCG_CLOSE  		_IO(WLSMDM_DEV_MAGIC, 1)
#define WLM_IOCG_RST			_IO(WLSMDM_DEV_MAGIC, 2)
#define WLM_IOCG_HOT_RST		_IO(WLSMDM_DEV_MAGIC, 3)
#define WLM_IOCG_GET_TYPE		_IO(WLSMDM_DEV_MAGIC, 4)
#define WLM_IOCG_GET_OPEN		_IO(WLSMDM_DEV_MAGIC, 5)
#define WLM_IOCG_GET_WORK		_IO(WLSMDM_DEV_MAGIC, 6)
#define WLM_IOCG_CTL_DTR		_IOW(WLSMDM_DEV_MAGIC, 7, int)
#define WLM_IOCG_GET_DCD		_IO(WLSMDM_DEV_MAGIC, 8)

/**
 * Һ����ʾ�ߴ�
 */
////typedef enum{
//	LCDSIZE_128X64,
//	LCDSIZE_128X160,
//	LCDSIZE_240X320,s
//	LCDSIZE_MAX
//}SCR_SIZE;




enum station_ReturnCode 
{
	STATION_RETURN_OK=0,
	STATION_WLM_ERROR=-1,
	STATION_LAI_ERROR=-2,
	STATION_NCELL_ERROR=-3,
	LBS_UNSUPPORT=-4,
	STATION_PARAM_ERROR=-5,
	UNKOWN_WLM_DRIVER=-6,
};

typedef struct
{
	float RoughLatitude;
	float RoughLongitude;
	char RoughAdress[1024];
}LBSinfo;


typedef struct WLMFuc{
	int type;
	int  (*ReadStationFuc)(void *);
	int  (*ReadLBSFuc)(LBSinfo *);
}WLMStationFuc;

#define WM_STRING_MODE		0 /*�������ַ���ģʽ����*/
/*����modem�����붨�壨8510�¶���ʹ�õģ�ǰ�涨��ĸ���ǰ�ɵĳ���ʹ��*/
typedef enum {
    WLM_RET_Illegal = -1000,			/*�Ƿ�*/
    WLM_NO_SIMCARD=-11,
    WLM_PIN_LOCKED=-12,
    WLM_PIN_ERR=-13,
    WLM_PIN_UNDEFINE=-14,
    WLM_RET_ErrTimeout = -8,			/*��ʱ*/
    WLM_RET_ErrOverFlow = -6,			/*�������*/
    WLM_RET_EMPTY = -4,					/*��*/
    WLM_RET_ERROR = -2,					/*���ش���*/
    WLM_RET_OK = 0,						/*����OK��*/
    WLM_RET_CONNECT,					/*����connect��*/
    WLM_RET_RING,						/*����RING��*/
    WLM_RET_NOCARRIER,					/*����NO CARRIER��*/
    WLM_RET_UNTYPED						/*���ش�δ����*/
} WLMResponseType ;


#if 0
extern int WLM_GetStationInfo(mobile_station_info * station_info);
extern int sim300_mobileStation(mobile_station_info * station_info);
extern int m72_mobileStation(void * station_info);
extern int Dtg228c_mobileStation(mobile_station_info * station_info);
extern int Ce910_mobileStation(mobile_station_info * station_info);
extern int Dtg228c_LBSinfo(LBSinfo *lbsinfo);
extern int sim300_LBSinfo(LBSinfo *lbsinfo);
extern int m72_LBSinfo(LBSinfo *lbsinfo);
extern int Ce910_LBSinfo(LBSinfo *lbsinfo);

#endif



#endif
