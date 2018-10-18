/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: asyn模块 
* file name			: 
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: asyn模块内随机组合测试
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI	"异步模块内随机组合"
#define MAXWAITTIME 10


/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
	MdmAsynInit,
	MdmDial,
	MdmCheck,
	MdmWrite,
	MdmRead,
	MdmHangup,
	MdmClrbuf,
	MdmGetreadlen,
	MdmReset,
	MdmAdapt,
	MaxNum,//新增枚举值时在此值前面加,要保证此值为最后一个值
}EM_ASYN;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: 
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  sull	         20180212	   	    created
*****************************************************************/
static int asyn_randtest(int funnum)
{
	int ret = -1, j = 0;
	EM_MDMSTATUS MdmStatus;	
	char buf[ASYNPCKTMAXLEN+1] = {0}, rbuf[512] = {0};
	uint rlen = 0, GetReadLen = -1;

	memset(buf, 0, sizeof(buf));
	for(j=0; j<sizeof(buf); j++)
		buf[j] = rand()%256;
	switch(funnum)
	{
		case MdmAsynInit:
			//if((ret = NDK_MdmAsynInit(rand()%5+1)) != NDK_OK)  在成功连接后台的情况下，会初始化失败，去掉判断 20180309 modify
				//return ret;
			NDK_MdmAsynInit(rand()%5+1);
			break;
		case MdmDial:
			NDK_MdmDial(MDMDialStr);
			break;
		case MdmCheck:
			NDK_MdmCheck(&MdmStatus);
			break;
		case MdmWrite:
			NDK_MdmWrite(buf,sizeof(buf));
			break;
		case MdmRead:
			memset(rbuf, 0, sizeof(rbuf));
			rlen = sizeof(buf)+1;//期望读取到的数据长度要大于等于实际能读取到的数据长度。
			NDK_MdmRead(rbuf, &rlen, MAXWAITTIME);
		case MdmHangup:
			NDK_MdmHangup();
			break;
		case MdmClrbuf:
			if((ret = NDK_MdmClrbuf()) != NDK_OK)
				return ret;
			break;
		case MdmGetreadlen:
			NDK_MdmGetreadlen(&GetReadLen);
			break;
		case MdmReset:
			if((ret = NDK_MdmReset()) != NDK_OK)
				return ret;
			break;
		case MdmAdapt:
			NDK_MdmAdapt(MODEM_DIAL_TYPE_ASYN, MDMDialStr);
			break;
	}		
	return SUCC;
}

void asyn200(void)
{
	/*private & local definition*/
	int  i = 0, bak = 0, num[30] ={0}, j = 0; 
	int ret = -1,cnt =50, len = 0;//cnt 待定
	char *asynstr[] = {"NDK_MdmAsynInit", "NDK_MdmDial", "NDK_MdmCheck", "NDK_MdmWrite", "NDK_MdmRead", "NDK_MdmHangup", "NDK_MdmClrbuf","NDK_MdmGetreadlen", "NDK_MdmReset", "NDK_MdmAdapt"};

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
			if((ret = asyn_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",asynstr[num[j]]);
				goto ERR;
			}
		}	
	}
	//test over
	send_result("%s测试通过", TESTAPI);
ERR:
	NDK_MdmHangup();
	return;
}

