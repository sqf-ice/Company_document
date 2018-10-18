/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 键盘模块 
* file name			: 
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 键盘模块内随机组合测试
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTAPI	"kb模块内随机组合"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
	KbFlush=0 ,			 
	KbGetCode,
	KbHit,	
	KbGetInput,
#if !(K21_ENABLE||OVERSEAS_ENABLE)
	KbHZInput,
	KbGetPad,
#endif
	MaxNum,//新增枚举值时在此值前面加,要保证此值为最后一个值
}KB_MAG;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: 
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						jiangym			20170913		created
*****************************************************************/
static int kb_randtest(int funnum)
{
	int  ret = -1, pnCode = 0;
	uint punLen = 0;
	char pszBuf[200] = {0};
#if !(K21_ENABLE||OVERSEAS_ENABLE)
	ST_PADDATA pad_data;
	char pszS[200] = {0};
	int cnt = 0;
#endif
	
#include "kbtable.h"

	switch(funnum)
	{	
		case KbFlush:  
			if ((ret=NDK_KbFlush())!=NDK_OK)
				return ret;
			break;
		case KbGetCode:  
			//if ((ret=NDK_KbGetCode(2,&pnCode))!=NDK_OK)
				NDK_KbGetCode(2,&pnCode);//ME32 1902h和ME50系列超时未按键，返回值不同，故不判断返回值
				//return ret;
			break;
		case KbHit:	
			if ((ret=NDK_KbHit(&pnCode))!=NDK_OK)
				return ret;
			break;
		case KbGetInput:  
			cls_show_msg1(3,"请输2个字符将正常显示,输2字符后自动返回");
			cls_printf("请输入");
			NDK_KbGetInput(pszBuf,0,2,&punLen,INPUTDISP_NORMAL,5,INPUT_CONTRL_NOLIMIT);
			break;
#if !(K21_ENABLE||OVERSEAS_ENABLE)
		case KbHZInput:  
			cls_show_msg1(3,"请输入任意字");
			memset(pszS, 0, sizeof(pszS));
			if ((ret=NDK_KbHZInput(pszS,20,IME_NUMPY))!=NDK_OK)
				return ret;
			break;
		case KbGetPad:  
			cnt = rand()%(SZ_ARRAY(table));
			cls_printf("请按下%s键", table[cnt].keyname);
			memset(&pad_data,0,sizeof(pad_data));
			if ((ret=NDK_KbGetPad(&pad_data,0))!=NDK_OK)
				return ret;
			break;	
#endif
	}	
	return SUCC;
}

void kb200(void)
{
	/*private & local definition*/
	int  i = 0, bak = 0, num[20] ={0}, j = 0; 
	int ret = -1,cnt =50, len = 0;//cnt 待定
#if !(K21_ENABLE||OVERSEAS_ENABLE)
	char *kbstr[] = {"NDK_KbFlush","NDK_KbGetCode","NDK_KbHit","NDK_KbGetInput","NDK_KbHZInput","NDK_KbGetPad"};
#else
	char *kbstr[] = {"NDK_KbFlush","NDK_KbGetCode","NDK_KbHit","NDK_KbGetInput"};
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
			if((ret = kb_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",kbstr[num[j]]);
				return;
			}
		}	
	}
	//test over
	send_result("%s测试通过", TESTAPI);
	return;
}

