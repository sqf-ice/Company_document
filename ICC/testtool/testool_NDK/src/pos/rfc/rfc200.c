/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: rfc 模块
* file name			: 
* Author 			: liny
* version			: 
* DATE				: 20170901
* directory 			: 
* description			: 随机组合调用rfc 模块函数，是否会跑飞
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI	"rfc模块内随机组合"

/*------------global variables definition-----------------------*/
/*----------global variables declaration("extern")--------------*/
/*---------------structure definition---------------------------*/
//定义枚举
typedef enum {
    	RfidInit=0, 
    	RfidPiccType,
	RfidPiccActivate,
	RfidPiccDeactivate,
	RfidPiccApdu,
	RfidPiccState,
	RfidPiccDetect,
#if !(defined ME30MH||defined ME32||defined ME30GX||defined ME32GX||defined ME15B) //开发说ME30MH不支持密钥存储\装载\内部认证这个几个接口故相关用例不测 20170527
    	M1Request,
    	M1Anti,
    	M1Select,
    	M1KeyStore,
#endif
#if !K21_ENABLE
    	M1KeyLoad,
    	M1InternalAuthen,
#endif
    	M1Read,
    	M1Write,
    	M1Increment,
    	M1Decrement,
    	M1Transfer,
    	M1ExternalAuthen,
    	RfidSuspend,
    	RfidResume,
#if !ANDIROD_ENABLE
    	RfidTypeARats,
#endif
#if !(K21_ENABLE||GP_ENABLE)
	RfidFelicaPoll,
	RfidFelicaApdu,
#endif
	MaxNum,
}EM_RFC;
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
//随机调用rfc模块函数
static int Rfc_RandTest(int funnum)
{
    //以下为局部变量
        uchar psPicctype=0x00; 
	int pnDatalen = 0,rcvlen=0,pUIDlen=0;
	uchar psRecebuf[300]={0}, psDatabuf[256]={0},UID[LEN_UID]={0},psSakbuf[256]={0};
	uchar out[MAXLEN_DATA]={0};
    uchar inc[]={0x01, 0x00, 0x00, 0x00};
	uchar Select_1pay[]= {'\x00', '\x84', '\x00', '\x00', 8};
#if !(K21_ENABLE||GP_ENABLE)
	int rec_len=0;
	uchar rec_buf[300]={0}, send_buf[300]={0}, UID1[300]={0};;
	//构造send_buf
	send_buf[0] = 16;
	send_buf[1] = 0x06;
	send_buf[10] = 0x01;
	send_buf[11] = 0x09;
	send_buf[12] = 0x00;
	send_buf[13] = 0x01;
	send_buf[14] = 0x80;
	send_buf[15] = 0x00;
#endif
	switch(funnum)
    	{
	        case RfidInit:
			NDK_RfidInit (NULL) ;
	    	case RfidPiccType:
			NDK_RfidPiccType((rand()%2==0)?RFCARD_TYPE_AB : RFCARD_TYPE_A);
			break;
		case RfidPiccActivate:
			NDK_RfidPiccActivate(&psPicctype, &pnDatalen, psDatabuf);
			break;
	    	case RfidPiccDeactivate:
			NDK_RfidPiccDeactivate(0);
			break;
		case RfidPiccApdu:
			NDK_RfidPiccApdu(sizeof(Select_1pay),Select_1pay,&rcvlen,psRecebuf);
			break;
		case RfidPiccState:
			NDK_RfidPiccState();
			break;
		case RfidPiccDetect:
			NDK_RfidPiccDetect(&psPicctype);
			break;
#if !(defined ME30MH||defined ME32||defined ME30GX||defined ME32GX||defined ME15B)
		case M1Request:
			NDK_M1Request(REQA,&pnDatalen, psDatabuf);
		    break;
		case M1Anti:
			NDK_M1Anti(&pnDatalen, UID);
			break;
		case M1Select:
			NDK_M1Select(pnDatalen, UID, psSakbuf);
			break;
		case M1KeyStore:
			NDK_M1KeyStore((rand()%2==0)?AUTHKEY_TYPE_A: AUTHKEY_TYPE_B, 0x00, AUTHKEY);
			break;
#endif
#if !K21_ENABLE
		case M1KeyLoad:
			NDK_M1KeyLoad((rand()%2==0)?AUTHKEY_TYPE_A: AUTHKEY_TYPE_B, 0x00);
			break;		
		case M1InternalAuthen:
			NDK_M1InternalAuthen(pUIDlen, UID,(rand()%2==0)?AUTHKEY_TYPE_A: AUTHKEY_TYPE_B,0x01);
			break;
#endif
		case M1Read:
			NDK_M1Read(0x02, &pnDatalen, out);
			break;
	        case M1Write:
			pnDatalen=LEN_BLKDATA;
		        NDK_M1Write(0x02, &pnDatalen, BLK02DATA_ORI);
			break;
		case M1Increment:
			NDK_M1Increment(0x02, sizeof(inc), inc);
			break;
		case M1Decrement:
			NDK_M1Decrement(0x02, sizeof(inc), inc);
			break;
		case M1Transfer:
			NDK_M1Transfer(0x02);
			break;
		case M1ExternalAuthen:
			NDK_M1ExternalAuthen(pUIDlen, UID, AUTHKEY_TYPE_B, AUTHKEY, 0x01);
			break;
		case RfidSuspend:
			NDK_RfidSuspend();
			break;
		case RfidResume:
			NDK_RfidResume();	
			break;
#if !ANDIROD_ENABLE
		case RfidTypeARats:
			NDK_RfidTypeARats(0,&pnDatalen, psDatabuf);
			break;
#endif
#if !(K21_ENABLE||GP_ENABLE)
		case RfidFelicaPoll:
			NDK_RfidFelicaPoll(UID1, &pUIDlen);
			break;
		case RfidFelicaApdu:
			memcpy(&send_buf[2], &UID1[2], 8);
		        NDK_RfidFelicaApdu(send_buf[0], send_buf, &rec_len, rec_buf);	
			break;
#endif
    	}
	return SUCC;
}
//产生随机数，调用 函数ICC_RandTest进行随机测试
void rfc200()
{
	/*private & local definition*/
	int i = 0, bak = 0,ret = -1,cnt =50,len=0;//cnt 待定   
	int num[30]={0}, j = 0;	
#if ANDIROD_ENABLE
	char *rfcstr[] = {"NDK_RfidInit","NDK_RfidPiccType","NDK_RfidPiccActivate","NDK_RfidPiccDeactivate","NDK_RfidPiccApdu","NDK_RfidPiccState","NDK_RfidPiccDetect","NDK_M1Request",
				"NDK_M1Anti","NDK_M1Select","NDK_M1KeyStore","NDK_M1KeyLoad","NDK_M1InternalAuthen","NDK_M1Read","NDK_M1Write","NDK_M1Increment","NDK_M1Decrement",
				"NDK_M1Transfer","NDK_M1ExternalAuthen","NDK_RfidSuspend","NDK_RfidResume"};
#elif K21_ENABLE
	char *rfcstr[] = {"NDK_RfidInit","NDK_RfidPiccType","NDK_RfidPiccActivate","NDK_RfidPiccDeactivate","NDK_RfidPiccApdu","NDK_RfidPiccState","NDK_RfidPiccDetect","NDK_M1Request",
				"NDK_M1Anti","NDK_M1Select","NDK_M1KeyStore","NDK_M1Read","NDK_M1Write","NDK_M1Increment","NDK_M1Decrement",
				"NDK_M1Transfer","NDK_M1ExternalAuthen","NDK_RfidSuspend","NDK_RfidResume","NDK_RfidTypeARats"};
#else
	char *rfcstr[] = {"NDK_RfidInit","NDK_RfidPiccType","NDK_RfidPiccActivate","NDK_RfidPiccDeactivate","NDK_RfidPiccApdu","NDK_RfidPiccState","NDK_RfidPiccDetect","NDK_M1Request",
			"NDK_M1Anti","NDK_M1Select","NDK_M1KeyStore","NDK_M1KeyLoad","NDK_M1InternalAuthen","NDK_M1Read","NDK_M1Write","NDK_M1Increment","NDK_M1Decrement",
			"NDK_M1Transfer","NDK_M1ExternalAuthen","NDK_RfidSuspend","NDK_RfidResume","NDK_RfidTypeARats","NDK_RfidFelicaPoll","NDK_RfidFelicaApdu"};
#endif

	/*process body*/
	if(MaxNum>20)
	        len = MaxNum;    
        else
	        len = 20;
	while(cnt)
	{	
		cnt--;
		bak++;
		
		//产生随机数
		srand((unsigned)time(NULL));
		for(i=0;i<len;i++)
		{	
			num[i] = rand()%MaxNum;
		}
		cls_printf("第%d次随机顺序:",bak);
		for(i=0;i<len;i++)
		{
			NDK_ScrPrintf("%d ", num[i]);
			NDK_ScrRefresh();
		}
		
		//进行随机测试
		for(i=0;i<len;i++)
		{
			if((ret =Rfc_RandTest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",rfcstr[num[j]]);
				goto ERR;
			}
		}
	}
	
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_RfidPiccDeactivate(0);//关闭射频
	return;
}

