/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: bt通讯模块
* file name			: 
* Author 			: liny
* version			: 
* DATE				: 20180108
* directory 			: 
* description			: 测试bt 通讯模块随机组合情况
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"
#if BTMFI_ENABLE||defined ME15CHX||defined ME30MH||defined ME30GX||defined ME15B||defined ME32||defined ME32GX||defined ME65
#define  MAXNAME 		23
#else
#define  MAXNAME 		15
#endif
#define   MAXPIN    	6
#define     MAXWAITTIME		10
#define MASTER_MAX_SIZE 4096//没有最大值
#define TIME_OUT 30000



/*---------------constants/macro definition---------------------*/

#define	TESTAPI	"蓝牙模块内随机组合"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
#if !(defined ME15CHX ||defined ME15B||defined ME15C ||defined ME20)
		BTReset,
	    	BTEnterCommand,
	    	BTExitCommand,
	    	BTSetPIN,
	    	BTGetPIN,
	    	BTSetPairingMode,
    		BTGetPairingStatus,
   	 	BTConfirmPairing,
#endif
		BTSetLocalName,
		BTGetLocalName,
		BTGetLocalMAC,
	    	BTStatus,
	    	PortOpen,
	    	PortClose,
	    	PortRead,
	    	PortWrite,
	    	PortClrBuf,
	    	PortReadLen,
    		BTDisconnect,
    		BTSetLocalMAC,
#if K21_ENABLE 
    		BTSetDiscoverableStatus,
    		BTSetType,
#endif
#if CPU5830X_ENABLE||OVERSEAS_ENABLE||CPU5892_ENABLE
    		BTMasterScan,
    		BTMasterStopScan,
    		BTMasterGetScanResults ,
    		BTMasterBond,
    		BTMasterGetBondStatus,
    		BTMasterBondConfirm,
    		BTMasterConnect,
    		BTMasterGetConStatus,
    		BTMasterWrite,
		BTMasterRead,
		BTMasterReadLen,
		BTMasterClrBuf,
		BTMasterDisconnect,
#endif
		MaxNum,
}EM_BT;
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: bt 通讯模块随机组合函数
* functional description 	: 测试bt 通讯模块随机组合情况
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						liny					20171212		created
*****************************************************************/
static int bt_randtest(int funnum)
{ 
    	char inbuf[MAXNAME+1] = {"newland"}, outbuf[MAXNAME+1] = {0}, oldbuf[MAXNAME+1] = {0},outbuf1[20]={0},oldmac[20] = {0},outpin[MAXPIN+1] = {0}, oldpin[MAXPIN+1] = {0},pkey[7] = {0};
	char sendbuf[MASTER_MAX_SIZE]={0},recbuf[MASTER_MAX_SIZE]={0};
	char *inpin[] ={"654321", 
					"123",
					"ne#1^3"};
	int ret=-1,status=0,cmdlen = 0, recvLen = 0,len=0,flag=0,snum=0,readlen=0;
	char cmdrecvBuf[BUFSIZE_BT] = {0}, cmdsendBuf[BUFSIZE_BT] = {0};
	char inmac[20] = {0x35,0x34,0x33,0x32,0x31,0x30};
#if K21_ENABLE
	char ph_type[3]={0x0c,0x02,0x5a}, ear_type[3]={0x04,0x04,0x24};
#endif
	char PAIRING_MODE[]={PAIRING_MODE_JUSTWORK , PAIRING_MODE_PINCODE,PAIRING_MODE_SSP,PAIRING_MODE_PASSKEY};
#if !K21_ENABLE
	char BT_DEVICE_TYPE[]={BT_DEVICE_TYPE_BREDR,BT_DEVICE_TYPE_BLE,BT_DEVICE_TYPE_DUMO};
	ST_BT_DEV sresult[50];
	char smac[6]={0x11,0x22,0x33,0x44,0x55,0x66};//任意的mac地址
	EM_PAIRING_MODE pair_mode=0;
#endif	

    	switch(funnum)
	{	
	//测试前置，防止进行随机函数调用的时候调到空指针，程序跑飞
#if !(defined ME15CHX ||defined ME15B||defined ME15C|| defined ME20)
		case BTReset:
	        	NDK_BTReset();
		    	break;
	    	case BTEnterCommand:
			NDK_BTEnterCommand();
			break;
		case BTExitCommand:
			NDK_BTExitCommand();
			break;
		case BTSetPIN:
			memset(oldpin,0,sizeof(oldpin));
			NDK_BTGetPIN(oldpin);
			NDK_BTSetPIN(inpin[rand()%3]);
			NDK_BTSetPIN(oldpin);
		case BTGetPIN:
			memset(outbuf,0,sizeof(outpin));
			NDK_BTGetPIN(outpin);
			break;
		case BTSetPairingMode:
			NDK_BTSetPairingMode(PAIRING_MODE[rand()%4]);
			break;
        	case BTGetPairingStatus:
			NDK_BTGetPairingStatus(pkey,&flag);
			break;
        	case BTConfirmPairing:
			NDK_BTConfirmPairing(pkey,rand()%2);
			break;
#endif
	        case BTSetLocalName:
			memset(oldbuf,0,sizeof(oldbuf));
			NDK_BTGetLocalName(oldbuf);
			NDK_BTSetLocalName(inbuf);
			NDK_BTSetLocalName(oldbuf);
			break;
	        case BTGetLocalName:
			memset(outbuf,0,sizeof(outbuf));
			NDK_BTGetLocalName(outbuf);
			break;
	        case BTGetLocalMAC:
			memset(outbuf1,0,sizeof(outbuf1));
			NDK_BTGetLocalMAC(outbuf1);
			break;
	        case BTStatus:
			NDK_BTStatus(&status);
			break;
	        case PortOpen:
			//以前的NDK底层代码，二次调用open会跑飞
			if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
		    	{
			   	return ret;
		    	}
			break;
	        case PortClose:
			//if((ret = NDK_PortClose(PORT_NUM_BT)) != NDK_OK)
			    //return ret;
			    NDK_PortClose(PORT_NUM_BT);
			break;
	        case PortRead:
			NDK_PortRead(PORT_NUM_BT, cmdlen, cmdrecvBuf, MAXWAITTIME*1000, &recvLen);
			break;
	        case PortWrite:
			NDK_PortWrite(PORT_NUM_BT, cmdlen, cmdsendBuf);
			break;
	        case PortClrBuf:
			NDK_PortClrBuf(PORT_NUM_BT);
			break;
	        case PortReadLen:
			NDK_PortReadLen(PORT_NUM_BT,&len);
			break;
	        case BTDisconnect:
			NDK_BTDisconnect();
			break;
	        case BTSetLocalMAC:
			memset(oldmac,0,sizeof(oldmac));
	        	NDK_BTGetLocalMAC(oldmac);
			NDK_BTSetLocalMAC(inmac);
			NDK_BTSetLocalMAC(oldmac);
			break;
#if K21_ENABLE
        	case BTSetDiscoverableStatus:
			NDK_BTSetDiscoverableStatus(rand()%3);
			break;	
		  case BTSetType:
		 	NDK_BTSetType(rand()%2==0?ph_type:ear_type);
			break;	
#endif
#if CPU5830X_ENABLE||OVERSEAS_ENABLE||CPU5892_ENABLE
        	case BTMasterScan:
			NDK_BTMasterScan(BT_DEVICE_TYPE[rand()%3]);
			break;
       	 	case BTMasterStopScan:
			NDK_BTMasterStopScan();
			break;			
        	case BTMasterGetScanResults:
			NDK_BTMasterGetScanResults(NULL,sresult,10,&snum);
			break;
        	case BTMasterBond:
			NDK_BTMasterBond(smac);
			break;
        	case BTMasterGetBondStatus:
			NDK_BTMasterGetBondStatus(&pair_mode,pkey,&status);
			break;
        	case BTMasterBondConfirm:
			NDK_BTMasterBondConfirm(pkey,rand()%2==0);
			break;
        	case BTMasterConnect:
			NDK_BTMasterConnect(smac,BT_DEVICE_TYPE[rand()%3]);
			break;
        	case BTMasterGetConStatus:
		   	NDK_BTMasterGetConStatus(smac,&status);
		    	break;
        	case BTMasterWrite:
			NDK_BTMasterWrite(smac,MASTER_MAX_SIZE,sendbuf);
			break;
        	case BTMasterRead:
			memset(recbuf,0,sizeof(recbuf));
	        	NDK_BTMasterRead(smac,MASTER_MAX_SIZE,recbuf,TIME_OUT,&readlen);
			break;
        	case BTMasterReadLen:
			NDK_BTMasterReadLen(smac,&readlen);
			break;
        	case BTMasterClrBuf:
			NDK_BTMasterClrBuf(smac);
			break;
        	case BTMasterDisconnect:
			NDK_BTMasterDisconnect(smac);
			break;
#endif
	}
    return SUCC;
}

void bt200(void)
{
	/*private & local definition*/
	int num[50] ={0}, i = 0, bak = 0;
	int ret = -1,cnt =50, len = 0,j=0;//cnt 待定
#if defined ME15CHX||defined ME15B||defined ME15C||defined ME20
	char *btstr[] = {"NDK_BTSetLocalName","NDK_BTGetLocalName","NDK_BTGetLocalMAC","NDK_BTStatus","NDK_PortOpen","NDK_PortClose","NDK_PortRead","NDK_PortWrite","NDK_PortClrBuf","NDK_PortReadLen",
		"NDK_BTDisconnect","NDK_BTSetLocalMAC","NDK_BTSetDiscoverableStatus","NDK_BTSetType"};	
#elif K21_ENABLE && !(defined ME15CHX||defined ME15B||defined ME15C||defined ME20)
	char *btstr[] = {"NDK_BTReset","NDK_BTEnterCommand","NDK_BTExitCommand","NDK_BTSetPIN","NDK_BTGetPIN","NDK_BTSetPairingMode","NDK_BTGetPairingStatus","NDK_BTConfirmPairing","NDK_BTSetLocalName","NDK_BTGetLocalName","NDK_BTGetLocalMAC",
		"NDK_BTStatus","NDK_PortOpen","NDK_PortClose","NDK_PortRead","NDK_PortWrite","NDK_PortClrBuf","NDK_PortReadLen","NDK_BTDisconnect","NDK_BTSetLocalMAC","NDK_BTSetDiscoverableStatus","NDK_BTSetType"};
#else
	char *btstr[] = {"NDK_BTReset","NDK_BTEnterCommand","NDK_BTExitCommand","NDK_BTSetPIN","NDK_BTGetPIN","NDK_BTSetPairingMode","NDK_BTGetPairingStatus","NDK_BTConfirmPairing","NDK_BTSetLocalName","NDK_BTGetLocalName","NDK_BTGetLocalMAC",
		"NDK_BTStatus","NDK_PortOpen","NDK_PortClose","NDK_PortRead","NDK_PortWrite","NDK_PortClrBuf","NDK_PortReadLen","NDK_BTDisconnect","NDK_BTSetLocalMAC","NDK_BTMasterScan","NDK_BTMasterStopScan","NDK_BTMasterGetScanResults ",
		"NDK_BTMasterBond","NDK_BTMasterGetBondStatus","NDK_BTMasterBondConfirm","NDK_BTMasterConnect","NDK_BTMasterGetConStatus","NDK_BTMasterWrite","NDK_BTMasterRead","NDK_BTMasterReadLen","NDK_BTMasterClrBuf","NDK_BTMasterDisconnect"};
#endif
	
	/*process body*/
	len = (MaxNum>20)?MaxNum:20;
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
			if((ret = bt_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",btstr[num[j]]);
				goto ERR;
			}
		}	
		
	}

ERR:
#if K21_ENABLE	
	NDK_BTSetDiscoverableStatus(1);
#endif
	NDK_PortClose(PORT_NUM_BT);//关闭蓝牙串口
	return;
}


