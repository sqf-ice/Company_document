/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 安全模块 
* file name			: 
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 安全模块内随机组合测试
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"
#define PINTIMEOUT	(10*1000)

/*---------------constants/macro definition---------------------*/
#define	TESTAPI	"安全模块内随机组合"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
	SecGetRandom=0 ,			 
	SecLoadKey,
	SecGetKcv,	
	SecGetMac,
	SecGetPin,
	SecGetPinResult,
	SecCalcDes,
	SecKeyErase,
	SecKeyDelete,
	SecLoadTIK,
	SecGetDukptKsn,
	SecIncreaseDukptKsn,
	SecGetPinDukpt,
	SecGetPinResultDukpt,
	SecGetMacDUKPT,
	SecCalcDesDUKPT,
#if CPU5892_ENABLE||CPU5830X_ENABLE
	SecSetCfg,
	SecGetCfg,
	SecSetIntervaltime,//实际无测试
	//SecSetFunctionKey,//实际无测试
	SecSetKeyOwner,
	SecLoadRsaKey,
	SecRecover,
#endif
#if ANDIROD_ENABLE
	SecVppTpInit,
	SecGetDrySR,
	SecClear,
	//getVKeybPin,//此函数不测
	//SecLoadDukptKey,//后面可增加此函数测试
#endif
	MaxNum,//新增枚举值时在此值前面加,要保证此值为最后一个值
}EM_SEC;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: 
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						 
*****************************************************************/
static int sec_randtest(int funnum)
{
	int ret = -1, nStatus = 0;
	uchar ucBuf[8] = {0}, KsnOut[11]={0}, sIV[9]={0};
	ST_SEC_KEY_INFO stKeyInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;
	uchar szDataIn[31]={0}, szMac[9]={0}, szPinOut[8+1]={0}, MacOut[9]={0};
	uchar *szPan=(uchar *)"6225885916163157";
	uchar udesout[31]={0}, udesin[31]={0}, DataIn[33] = {0}, DataOut[1024+8]={0};
	uchar KeyValueIn[17]={0}, KsnIn[11]={0};
	uchar KSN[11]={0};//初始KSN
#if CPU5892_ENABLE || CPU5830X_ENABLE
	uint punCfgInfo = 0;
	ST_SEC_RSA_KEY RsaKeyIn;
	uchar *uModulus1=(uchar *)"4FB2D08918F766B30B2144FE25D09A160FFCF256086D82773FB04D4BD7E4772F2557FC25EE130A0B984467995BE80EE1751F577BD3E5CAFF60712722F33D0479";
	uchar sDataIn[MAX_RSA_MODULUS_LEN]={0}, sDataOut[MAX_RSA_MODULUS_LEN]={0};
#endif
#if ANDIROD_ENABLE
	uchar num[80] = {0}, fun[36] = {0};
	int x0 = 0, x1 = 135, x2 = 270, x3 = 405, x4 = 540, y0 = 462, y1 = 568, y2 = 674, y3 = 780, y4 = 886;
	int i = 0, j = 0, val = 0;
#if 1//defined N910AZ||defined N700
	int fp = -1;
	char touch_hr[] = {"1280x720"};//高分辨率是1280x720
	char touch_hr_850[] = {"1024x600"};//850的分辨率是 1024x600 
	char readbuf[20] = {0};
	if((fp=open("/sys/class/touchscreen/resolution", O_RDONLY))<0)
	{
		cls_show_msg1(2,"line %d: 打开文件失败(%d)", __LINE__, fp);
		return FAIL;
	}
	memset(readbuf, 0, sizeof(readbuf));
	read(fp,readbuf, sizeof(readbuf));
	//读取分辨率获取到高分辨率需转换坐标如果是低分辨率不做改变
	if(!memcmp(touch_hr,readbuf,4))//使用前四个数比较才行
	{
#if defined N920
		x0 = 0; x1 = 180; x2 = 360; x3 = 540;x4 = 720;
		y0 =614; y1 = 755; y2 = 896; y3 = 1037; y4 = 1178;
#else
		x0 = 0; x1 = 180; x2 = 360; x3 = 540;x4 = 720;
		y0 = 665; y1 = 818; y2 = 972; y3 = 1126; y4 = 1280;
#endif
	}
	//增加N850的分辨率
	else if(!memcmp(touch_hr_850,readbuf,4))  //850
	{
		x0 = 0; x1 = 150; x2 = 300; x3 = 450; x4 = 600;
		y0 = 495; y1 = 609; y2 = 723; y3 = 837; y4 = 952;
	}
	else   //900
	{
		x0 = 0; x1 = 135; x2 = 270; x3 = 405; x4 = 540;
		y0 = 462;y1 = 568; y2 = 674; y3 = 780; y4 = 886;
	}
	close(fp);
#endif	
	int numInt[40] = { 
			x0, y0, x1, y1, x1, y0, x2, y1, x2, y0, x3, y1,
	
			x0, y1, x1, y2, x1, y1, x2, y2, x2, y1, x3, y2,
	
			x0, y2, x1, y3, x1, y2, x2, y3, x2, y2, x3, y3,
	
			x1, y3, x2, y4};
	int funInt[18] = { //选择传入的功能键为:取消，退格，确认
			0x1B, 0, x3, y0, x4, y1,
			
			0x0A, 0, x3, y1, x4, y2,
			
			0x0D, 0, x3, y2, x4, y4};
#endif
	switch(funnum)
	{	
		case SecGetRandom:  
			if((ret = NDK_SecGetRandom(5, ucBuf)) != NDK_OK)
				return ret;
			break;
		case SecLoadKey:  
			stKeyInfo.ucScrKeyIdx=0;
			stKeyInfo.ucScrKeyType=0;
			stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK;
			stKeyInfo.ucDstKeyIdx=1;
			stKeyInfo.nDstKeyLen=16;
			memset(stKeyInfo.sDstKeyValue, 0x11, stKeyInfo.nDstKeyLen);
			stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
			NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn);
			break;
		case SecGetKcv:	
			NDK_SecGetKcv(SEC_KEY_TYPE_TMK, 1, &stKcvInfoIn);
			break;
		case SecGetMac:  
			NDK_SecGetMac(2, szDataIn, 8, szMac, SEC_MAC_X99);
			break;
		case SecGetPin:  
			touchscrean_getnum();
			NDK_SecGetPin(3, (uchar *)"8,6,4", szPan, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT);
			break;
		case SecGetPinResult:
			NDK_SecGetPinResult(szPinOut, &nStatus);
			break;
		case SecCalcDes:
			memset(udesin, 0x20, sizeof(udesin));
			NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 3, udesin, 24, udesout, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24);
			break;
		case SecKeyErase:
			if((ret=NDK_SecKeyErase())!=NDK_OK)
				return ret;
			break;
		case SecKeyDelete:
			NDK_SecKeyDelete(2, SEC_KEY_TYPE_TAK);
			break;
		case SecLoadTIK:
			memset(KsnIn, 0xff, 10);
			memset(KeyValueIn, 0x11, 16);
			stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
			NDK_SecLoadTIK(1, 0, 16, KeyValueIn, KsnIn, &stKcvInfoIn);
			break;
		case SecGetDukptKsn:
			NDK_SecGetDukptKsn(1, KsnOut);
			break;
		case SecIncreaseDukptKsn:
			NDK_SecIncreaseDukptKsn(1);
			break;
		case SecGetPinDukpt:
			touchscrean_getnum();
			NDK_SecGetPinDukpt(6, (uchar *)"0", szPan, KSN, szPinOut, SEC_PIN_ISO9564_0, PINTIMEOUT);
			break;
		case SecGetPinResultDukpt:
			NDK_SecGetPinResultDukpt(szPinOut, KsnOut, &nStatus);
			break;
		case SecGetMacDUKPT:
			memset(DataIn, 0x22, sizeof(DataIn));
			NDK_SecGetMacDukpt(2, DataIn, 18, MacOut, KSN, rand()%3);
			break;
		case SecCalcDesDUKPT:
			memset(sIV, 0x12, sizeof(sIV));
			memset(DataOut, 0, sizeof(DataOut));
			NDK_SecCalcDesDukpt(3,SEC_KEY_TYPE_TDK, sIV, 24, DataIn, DataOut, KSN, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_24|0);
			break;
#if CPU5892_ENABLE||CPU5830X_ENABLE
		case SecSetCfg:
			NDK_SecGetCfg(&punCfgInfo);
			if((ret=NDK_SecSetCfg(2))!=NDK_OK)
				return ret;
			NDK_SecSetCfg(punCfgInfo);
			break;
		case SecGetCfg:
			if(((ret=NDK_SecGetCfg(&punCfgInfo))!=NDK_OK)||(punCfgInfo!=6)) 
				return ret;
			break;
		case SecSetIntervaltime:
			NDK_SecSetIntervaltime(10, 1);
			break;
		case SecSetKeyOwner:
			NDK_SecSetKeyOwner(NULL);
			break;
		case SecLoadRsaKey:
			memset(&RsaKeyIn, 0,sizeof(ST_SEC_RSA_KEY));
			RsaKeyIn.usBits=RSA_KEY_LEN_512;
			strcpy((char *)RsaKeyIn.sExponent, "03");
			memcpy(RsaKeyIn.sModulus, uModulus1, RsaKeyIn.usBits*2/8);
			if((ret=NDK_SecLoadRsaKey(1, &RsaKeyIn))!= NDK_OK)
				return ret;
			break;
		case SecRecover:
			NDK_SecRecover(1, sDataIn, 64, sDataOut);
			break;
#endif
#if ANDIROD_ENABLE
		case SecVppTpInit:  
			for (i = 0, j = 0; i < SZ_ARRAY(numInt); i++, j++) 
			{
				num[j] = (uchar) (numInt[i] & 0xff);
				j++;
				num[j] = (uchar) ((numInt[i] >> 8) & 0xff);
			}
			for (i = 0, j = 0; i < SZ_ARRAY(funInt); i++, j++) 
			{
				fun[j] = (uchar) (funInt[i] & 0xff);
				j++;
				fun[j] = (uchar) ((funInt[i] >> 8) & 0xff);
			}
			NDK_SecVppTpInit(num, fun, NULL);
			break;	
		case SecGetDrySR:
			if((ret = NDK_SecGetDrySR(&val)) != NDK_OK)
				return ret;
			break;
		case SecClear:
			NDK_SecClear();
			break;		
#endif
	}	
	return SUCC;
}

void sec200(void)
{
	/*private & local definition*/
	int  i = 0, bak = 0, num[50] ={0}, j = 0; 
	int ret = -1,cnt =50, len = 0;//cnt 待定
#if CPU5892_ENABLE||CPU5830X_ENABLE
	char *secstr[] = {"NDK_SecGetRandom","NDK_SecLoadKey","NDK_SecGetKcv","NDK_SecGetMac","NDK_SecGetPin","NDK_SecGetPinResult","NDK_SecCalcDes","NDK_SecKeyErase","NDK_SecKeyDelete","NDK_SecLoadTIK","NDK_SecGetDukptKsn","NDK_SecIncreaseDukptKsn","NDK_SecGetPinDukpt","NDK_SecGetPinResultDukpt","NDK_SecGetMacDukpt","NDK_SecCalcDesDukpt","NDK_SecSetCfg","NDK_SecGetCfg","NDK_SecSetIntervaltime","NDK_SecSetKeyOwner","NDK_SecLoadRsaKey","NDK_SecRecover"};
#elif ANDIROD_ENABLE
	char *secstr[] = {"NDK_SecGetRandom","NDK_SecLoadKey","NDK_SecGetKcv","NDK_SecGetMac","NDK_SecGetPin","NDK_SecGetPinResult","NDK_SecCalcDes","NDK_SecKeyErase","NDK_SecKeyDelete","NDK_SecLoadTIK","NDK_SecGetDukptKsn","NDK_SecIncreaseDukptKsn","NDK_SecGetPinDukpt","NDK_SecGetPinResultDukpt","NDK_SecGetMacDukpt","NDK_SecCalcDesDukpt","NDK_SecVppTpInit","NDK_SecGetDrySR","NDK_SecClear"};
#else
	char *secstr[] = {"NDK_SecGetRandom","NDK_SecLoadKey","NDK_SecGetKcv","NDK_SecGetMac","NDK_SecGetPin","NDK_SecGetPinResult","NDK_SecCalcDes","NDK_SecKeyErase","NDK_SecKeyDelete","NDK_SecLoadTIK","NDK_SecGetDukptKsn","NDK_SecIncreaseDukptKsn","NDK_SecGetPinDukpt","NDK_SecGetPinResultDukpt","NDK_SecGetMacDukpt","NDK_SecCalcDesDukpt"};
#endif		

	/*process body*/
	len = (MaxNum>20)?MaxNum:20;
	while(cnt)
	{	
		cnt--;
		bak++;
		
		//产生随机数
		cls_printf("第%d次随机顺序:",bak);
		srand((unsigned)time(NULL));
		for(i=0;i<len;i++)
		{	
			num[i] = rand()%MaxNum;
			NDK_ScrPrintf("%d ", num[i]);
			NDK_ScrRefresh();
		}
		
		//进行随机测试
		for(i=0;i<len;i++)
		{
			if((ret =sec_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",secstr[num[j]]);
				goto ERR;
			}
		}	
	}
	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
#if CPU5892_ENABLE||CPU5830X_ENABLE
	NDK_SecSetIntervaltime(0,0);
#endif
	NDK_SecKeyErase();
	return;
}


