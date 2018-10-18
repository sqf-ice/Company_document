/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 扫描模块 
* file name			: 
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 扫描模块内随机组合测试
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI	"扫描模块内随机组合"
#define MAXTIMEOUT 5

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
#if defined SP600||defined SP630||defined SP930||defined SP600G||defined ME51||defined SP930G||defined SP930PG||defined SP630PG||defined SP930P
	ScanInit,
	ScanSet,
#endif
#if defined SP600||defined SP630||defined SP930||defined SP600G||defined ME51||defined SP930G||defined SP930PG||defined SP630PG||defined SP930P||defined ME50||defined ME50N
	ScanDoScan,
#endif
#if defined ME50||defined ME50NX||defined ME68
	ScanStart,
	ScanState,
	ScanStop,
#endif
#if SCAN_SOFT_ENABLE	
	ScanExit,
#endif
	MaxNum,//新增枚举值时在此值前面加,要保证此值为最后一个值
}EM_SCAN;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: 
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						  sull	         20180211	   	    created
*****************************************************************/
static int scan_randtest(int funnum)
{
	int plen = 0;
	char pvalue[500] = {0};
	int ret = -1;
	switch(funnum)
	{	
#if defined SP600||defined SP630||defined SP930||defined SP600G||defined ME51||defined SP930G||defined SP930PG||defined SP630PG||defined SP930P
		case ScanInit:
			if((ret = NDK_ScanInit()) != NDK_OK)
				goto ERR;
			break;
		case ScanSet:
			srand(time(NULL));
			NDK_ScanSet(rand()%4,0);
			break;
#endif
#if defined SP600||defined SP630||defined SP930||defined SP600G||defined ME51||defined SP930G||defined SP930PG||defined SP630PG||defined SP930P||defined ME50||defined ME50N
		case ScanDoScan:
			memset(pvalue, 0, sizeof(pvalue));
			NDK_ScanDoScan(MAXTIMEOUT,pvalue,&plen);
			break;
#endif
#if defined ME50||defined ME50NX||defined ME68
		case ScanStart:
			if((ret = NDK_ScanStart()) != NDK_OK)
				goto ERR;
			break;
		case ScanState:
			memset(pvalue, 0, sizeof(pvalue));
			NDK_ScanState(pvalue, &plen);
			break;
		case ScanStop:
			if((ret = NDK_ScanStop()) != NDK_OK)
				goto ERR;
			break;
#endif
#if SCAN_SOFT_ENABLE	
		NDK_ScanExit();
#endif
	}
	return SUCC;
ERR:
#if defined SP600||defined SP630||defined SP930||defined SP600G||defined ME51||defined SP930G||defined SP930PG||defined SP630PG||defined SP930P
	NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT,11);
#elif defined ME50||defined ME50NX||defined ME68
	NDK_ScanStop();
#endif
#if SCAN_SOFT_ENABLE	
	NDK_ScanExit();
#endif
	return ret;
}

void scan200(void)
{
	/*private & local definition*/
	int  i = 0, bak = 0, num[30] ={0}, j = 0; 
	int ret = -1,cnt =50, len = 0;//cnt 待定
#if defined SP600||defined SP630||defined SP930||defined SP600G||defined ME51||defined SP930G||defined SP930PG||defined SP630PG||defined SP930P
#if SCAN_SOFT_ENABLE
	char *scanstr[] = {"NDK_ScanInit", "NDK_ScanSet", "NDK_ScanDoScan", "NDK_ScanExit"};
#else
	char *scanstr[] = {"NDK_ScanInit", "NDK_ScanSet", "NDK_ScanDoScan"};
#endif
#elif defined ME50
	char *scanstr[] = {"NDK_ScanDoScan", "NDK_ScanStart", "NDK_ScanState", "NDK_ScanStop"};
#elif defined ME50N||defined ME50C
	char *scanstr[] = {"NDK_ScanDoScan"};
#elif defined ME50NX||defined ME68
	char *scanstr[] = {"NDK_ScanStart", "NDK_ScanState", "NDK_ScanStop"};
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
			if((ret = scan_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",scanstr[num[j]]);
				return;
			}
		}	
	}

#if defined SP600||defined SP630||defined SP930||defined SP600G||defined ME51||defined SP930G||defined SP930PG||defined SP630PG||defined SP930P
	NDK_ScanSet(SCAN_SETTYPE_FACTORYDEFAULT,11);
#elif defined ME50||defined ME50NX||defined ME68
	NDK_ScanStop();
#endif
#if SCAN_SOFT_ENABLE	
		NDK_ScanExit();
#endif
	//test over
	send_result("%s测试通过", TESTAPI);
	return;
}

