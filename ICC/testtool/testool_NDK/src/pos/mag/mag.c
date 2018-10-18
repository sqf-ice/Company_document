/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: mag模块
* file name		: 
* Author 			: chenfm
* version			: 
* DATE			: 20130104
* directory 		: 
* description		: fs模块总入口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define	TESTMODULE	"磁卡模块"
#define  MAXWAITTIME 30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
#if !K21_ENABLE||ANDIROD_ENABLE     //中高端产品异常卡结果判断||defined IM81||defined N900
static void check_stripe_errcode(int cardno, char *track2, char *track3, int *perrcard, int *err)
{
	/*private & local definition*/
	char track[MAXTRACKLEN]={0};

	/*process body*/
	//以下预设结果是根据《磁卡测试案例.doc》
	//实际测试中,仅测试二三道(不测一道),因为errcode是哪道都是一样的

	switch(cardno)
	{
  	case 1://测试1#卡        1号卡 有起始标志和结束标志正确的LRC ，无数据， 凯哥解释底层判断到数据域为空 就直接返回7f并且LRC置为错误
		if(((track2[0]!=0x7e)&&(track2[0]!=0x7f))||(((*err)&MAGCARD_TK2_LRC_ERR)!=MAGCARD_TK2_LRC_ERR))//无数据 //20140822 linwl added 允许底层返回7f，底层无法区分7E 7F
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错track2[0]=0x%02x,err=0x%02x",__FUNCTION__, __LINE__, cardno,track2[0], *err);
			*perrcard|=(1<<(cardno-1));
		}
		if(((track3[0]!=0x7e)&&(track3[0]!=0x7f))||(((*err)&MAGCARD_TK3_LRC_ERR)!=MAGCARD_TK3_LRC_ERR))//无数据 //20140822 linwl added 允许底层返回7f，底层无法区分7E 7F
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错track3[0]=0x%02x,err=0x%02x",__FUNCTION__, __LINE__, cardno,track3[0], *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//不清除曾经出错的位,避免正刷错误,反刷正确时,清掉了错误位
		//*perrcard &= ~(1<<(cardno-1));
		break;
  	case 2://测试2#卡
  	case 4://测试4#卡
  	case 5://测试5#卡
		if(track2[0]!=0x7f||(((*err)&MAGCARD_TK2_LRC_ERR)!=MAGCARD_TK2_LRC_ERR))//读卡错
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错track2[0]=0x%02x,err=0x%02x", __FUNCTION__, __LINE__, cardno,track2[0], *err);
			*perrcard|=(1<<(cardno-1));
		}
		if(track3[0]!=0x7f||(((*err)&MAGCARD_TK3_LRC_ERR)!=MAGCARD_TK3_LRC_ERR))//读卡错
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错track3[0]=0x%02x,err=0x%02x", __FUNCTION__, __LINE__, cardno,track3[0], *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 3://测试3#卡
  	case 6://测试6#卡
  	case 7://测试7#卡
#if 0
		if(cardno==6)
		{
			cls_show_msg("GetErrorCode = %x\n",*err);
			cls_show_msg("track2(%d):%s\n",strlen(track2),track2);
			cls_show_msg("track3(%d):%s\n",strlen(track3),track3);
		}
#endif
		if(((*err)&MAGCARD_TK2_LRC_ERR)==MAGCARD_TK2_LRC_ERR)	//LRC校验错
		{
			memset(track, '2', sizeof(track));
			track[37]=0;
			if(strcmp(track2, track))
			{
				cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
				*perrcard|=(1<<(cardno-1));
			}
		}
		else
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1));
		}
		if(((*err)&MAGCARD_TK3_LRC_ERR)==MAGCARD_TK3_LRC_ERR)	//LRC校验错
		{
			memset(track, '3', sizeof(track));
			track[104]=0;
			if(strcmp(track3, track))
			{
				cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
				*perrcard|=(1<<(cardno-1+16));
			}
		}
		else
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 8://测试8#卡
		memset(track, '1', sizeof(track));
		track[37]=0;
		if(strcmp(track2, track)||(((*err)&MAGCARD_TK2_LRC_ERR)!=MAGCARD_TK2_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1));
		}
		if(((track3[0]!=0x7e)&&(track3[0]!=0x7f))||(((*err)&MAGCARD_TK3_LRC_ERR)!=MAGCARD_TK3_LRC_ERR))//无数据 //20140822 linwl added 允许底层返回7f，底层无法区分7E 7F
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错track3[0]=0x%02x, 0x%02x",  __FUNCTION__,__LINE__, cardno,track3[0], *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 9://测试9#卡
		if(((track2[0]!=0x7e)&&(track2[0]!=0x7f))||(((*err)&MAGCARD_TK2_LRC_ERR)!=MAGCARD_TK2_LRC_ERR))//无数据 //20140822 linwl added 允许底层返回7f，底层无法区分7E 7F
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错track2[0]=0x%02x,err=0x%02x",  __FUNCTION__, __LINE__, cardno,track2[0], *err);
			*perrcard|=(1<<(cardno-1));
		}
		memset(track, '2', sizeof(track));
		track[89]=0;
		if(strcmp(track3, track)||(((*err)&MAGCARD_TK3_LRC_ERR)!=MAGCARD_TK3_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 10://测试10#卡
		memset(track, '1', sizeof(track));
		track[32]=0;
		if(strcmp(track2, track)||(((*err)&MAGCARD_TK2_LRC_ERR)==MAGCARD_TK2_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1));
		}
		memset(track, '2', sizeof(track));
		track[100]=0;
		if(strcmp(track3, track)||(((*err)&MAGCARD_TK3_LRC_ERR)==MAGCARD_TK3_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 11://测试11#卡
		memset(track, '1', sizeof(track));
		track[37]=0;
		if(strcmp(track2, track)||(((*err)&MAGCARD_TK2_LRC_ERR)==MAGCARD_TK2_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1));
		}
		memset(track, '2', sizeof(track));
		track[104]=0;
		if(strcmp(track3, track)||(((*err)&MAGCARD_TK3_LRC_ERR)==MAGCARD_TK3_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 12://测试12#卡
		if(((*err)&MAGCARD_TK2_LRC_ERR)==MAGCARD_TK2_LRC_ERR)	//LRC校验错
		{
			memset(track, '1', sizeof(track));
			track[30]=';';
			track[31]=0;
			if(strcmp(track2, track))
			{
				cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
				*perrcard|=(1<<(cardno-1));
			}
		}
		else
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1));
		}
		if(((*err)&MAGCARD_TK3_LRC_ERR)==MAGCARD_TK3_LRC_ERR)	//LRC校验错
		{
			memset(track, '2', sizeof(track));
			track[100]=';';
			track[101]=0;
			if(strcmp(track3, track))
			{
				cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
				*perrcard|=(1<<(cardno-1+16));
			}
		}
		else
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 13://测试13#卡 数据的奇偶校验引起LRC校验错
		if(((*err)&MAGCARD_TK2_LRC_ERR)==MAGCARD_TK2_LRC_ERR)	//LRC校验错
		{
			memset(track, '1', sizeof(track));
			track[19]=0;
			if(strcmp(track2, track))
			{
				cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
				*perrcard|=(1<<(cardno-1));
			}
		}
		else
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1));
		}
		if(((*err)&MAGCARD_TK3_LRC_ERR)==MAGCARD_TK3_LRC_ERR)	//LRC校验错
		{
			memset(track, '2', sizeof(track));
			track[29]=0;
			if(strcmp(track3, track))
			{
				cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
				*perrcard|=(1<<(cardno-1+16));
			}
		}
		else
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 14://测试14#卡   2道LRC校验正确，3道LRC校验错误
		strcpy(track, "1234567890123456789012345678901233");
		if(strcmp(track2, track)||(((*err)&MAGCARD_TK2_LRC_ERR)==MAGCARD_TK2_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1));
		}
		strcpy(track, "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567892");
		if(strcmp(track3, track)||(((*err)&MAGCARD_TK3_LRC_ERR)!=MAGCARD_TK3_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 15://测试15#卡
		strcpy(track, "1234567890123456789012345678901234567");
		if(strcmp(track2, track)||(((*err)&MAGCARD_TK2_LRC_ERR)==MAGCARD_TK2_LRC_ERR))//if(!memcmp(track2, track, 37))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1));
		}
		strcpy(track, "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234");
		if(strcmp(track3, track)||(((*err)&MAGCARD_TK3_LRC_ERR)==MAGCARD_TK3_LRC_ERR))//if(!memcmp(track3, track, 104))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	default:
		cls_show_msg1(2, "不支持对%d#磁卡的测试!", cardno);
		return;
		break;
	}

	return;
}
#else
static void check_stripe_errcode(int cardno, char *track2, char *track3, int *perrcard, int *err)
{
	/*private & local definition*/
	char track[MAXTRACKLEN]={0};

	/*process body*/
	//以下预设结果是根据《磁卡测试案例.doc》
	//实际测试中,仅测试二三道(不测一道),因为errcode是哪道都是一样的

	switch(cardno)
	{
  	case 1://测试1#卡
		if(((track2[0]!=0x7e)&&(track2[0]!=0x7f))||(((*err)&MAGCARD_TK2_LRC_ERR)!=MAGCARD_TK2_LRC_ERR))//无数据 //20140822 linwl added 允许底层返回7f，底层无法区分7E 7F
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错track2[0]=0x%02x,err=0x%02x",__FUNCTION__, __LINE__, cardno,track2[0], *err);
			*perrcard|=(1<<(cardno-1));
		}
		if(((track3[0]!=0x7e)&&(track3[0]!=0x7f))||(((*err)&MAGCARD_TK3_LRC_ERR)!=MAGCARD_TK3_LRC_ERR))//无数据 //20140822 linwl added 允许底层返回7f，底层无法区分7E 7F
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错track3[0]=0x%02x,err=0x%02x",__FUNCTION__, __LINE__, cardno,track3[0], *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//不清除曾经出错的位,避免正刷错误,反刷正确时,清掉了错误位
		//*perrcard &= ~(1<<(cardno-1));
		break;
  	case 2://测试2#卡
  	case 4://测试4#卡
  	case 5://测试5#卡
  	
  	case 3://测试3#卡
  	case 6://测试6#卡
  	case 7://测试7#卡
  	
	case 12://测试12#卡	
	case 13://测试13#卡
		if(track2[0]!=0x7f||(((*err)&MAGCARD_TK2_LRC_ERR)!=MAGCARD_TK2_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错track2[0]=0x%02x,err=0x%02x",__FUNCTION__, __LINE__, cardno,track2[0], *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		if(track3[0]!=0x7f||(((*err)&MAGCARD_TK3_LRC_ERR)!=MAGCARD_TK3_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错track3[0]=0x%02x,err=0x%02x",__FUNCTION__, __LINE__, cardno,track3[0], *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		break;
  	
  	case 8://测试8#卡
		//memset(track, '1', sizeof(track));
		//track[37]=0;
		if(track2[0]!=0x7f||(((*err)&MAGCARD_TK2_LRC_ERR)!=MAGCARD_TK2_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错track2[0]=0x%02x,err=0x%02x",__FUNCTION__, __LINE__, cardno,track2[0], *err);
			*perrcard|=(1<<(cardno-1));
		}
		if(((track3[0]!=0x7e)&&(track3[0]!=0x7f))||(((*err)&MAGCARD_TK3_LRC_ERR)!=MAGCARD_TK3_LRC_ERR))//无数据 //20140822 linwl added 允许底层返回7f，底层无法区分7E 7F
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错track3[0]=0x%02x,err=0x%02x",__FUNCTION__, __LINE__, cardno,track3[0], *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 9://测试9#卡
		if(((track2[0]!=0x7e)&&(track2[0]!=0x7f))||(((*err)&MAGCARD_TK2_LRC_ERR)!=MAGCARD_TK2_LRC_ERR))//无数据 //20140822 linwl added 允许底层返回7f，底层无法区分7E 7F
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错track2[0]=0x%02x,err=0x%02x",__FUNCTION__, __LINE__, cardno,track2[0], *err);
			*perrcard|=(1<<(cardno-1));
		}
		//memset(track, '2', sizeof(track));
		//track[89]=0;
		if(track3[0]!=0x7f||(((*err)&MAGCARD_TK3_LRC_ERR)!=MAGCARD_TK3_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错track3[0]=0x%02x,err=0x%02x",__FUNCTION__, __LINE__, cardno,track3[0], *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 10://测试10#卡
		memset(track, '1', sizeof(track));
		track[32]=0;
		if(strcmp(track2, track)||(((*err)&MAGCARD_TK2_LRC_ERR)==MAGCARD_TK2_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1));
		}
		memset(track, '2', sizeof(track));
		track[100]=0;
		if(strcmp(track3, track)||(((*err)&MAGCARD_TK3_LRC_ERR)==MAGCARD_TK3_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 11://测试11#卡
		memset(track, '1', sizeof(track));
		track[37]=0;
		if(strcmp(track2, track)||(((*err)&MAGCARD_TK2_LRC_ERR)==MAGCARD_TK2_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1));
		}
		memset(track, '2', sizeof(track));
		track[104]=0;
		if(strcmp(track3, track)||(((*err)&MAGCARD_TK3_LRC_ERR)==MAGCARD_TK3_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 14://测试14#卡
		strcpy(track, "1234567890123456789012345678901233");
		if(strcmp(track2, track)||(((*err)&MAGCARD_TK2_LRC_ERR)==MAGCARD_TK2_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1));
		}
		strcpy(track, "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567892");
		if(track3[0]!=0x7f||(((*err)&MAGCARD_TK3_LRC_ERR)!=MAGCARD_TK3_LRC_ERR))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	case 15://测试15#卡
		strcpy(track, "1234567890123456789012345678901234567");
		if(strcmp(track2, track)||(((*err)&MAGCARD_TK2_LRC_ERR)==MAGCARD_TK2_LRC_ERR))//if(!memcmp(track2, track, 37))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第2磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1));
		}
		strcpy(track, "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234");
		if(strcmp(track3, track)||(((*err)&MAGCARD_TK3_LRC_ERR)==MAGCARD_TK3_LRC_ERR))//if(!memcmp(track3, track, 104))
		{
			cls_show_msg1(3, "%s,line %d:第%d号卡第3磁道有错(0x%02x)", __FUNCTION__, __LINE__, cardno, *err);
			*perrcard|=(1<<(cardno-1+16));
		}
		//*perrcard &=  ~(1<<(cardno-1));
		break;
  	default:
		cls_show_msg1(2, "不支持对%d#磁卡的测试!", cardno);
		return;
		break;
	}

	return;
}

#endif

#define	MAXCARDCNT			(15)
//增加15张用例磁卡的专项测试
int test_mag_errcode(int *perrcard1, int *perrcard2, int *perrcard3, int *perrcard4)
{
	/*private & local definition*/
	char track1[MAXTRACKLEN]={0}, track2[MAXTRACKLEN]={0}, track3[MAXTRACKLEN]={0}, cardnostr[4]={0};
	int ret=SUCC, mode=0, cardno=0, err=0;
	uint unLen=0;
	int *intptr[4];
#if SCR_DISABLE
	char pMsg[64]={0};
#endif
	/*process body*/
	*perrcard1= *perrcard2= *perrcard3= *perrcard4=0;
	intptr[0]=perrcard1;intptr[1]=perrcard2;intptr[2]=perrcard3;intptr[3]=perrcard4;
	while(1)
	{
		if(ESC==cls_show_msg1(MAXWAITTIME, "请准备%d张用例磁卡,即将进行磁卡ErrCode测试,ESC退出本子项测试", MAXCARDCNT))
			return FAIL;

		if(++cardno>MAXCARDCNT)
			cardno=1;

#if ANDIROD_ENABLE//这里IM81使用会有问题,NDK_KbGetInput缓冲区中已存在的字符无法显示在串口上,所以将缓冲区清空后提示要测试的卡号defined IM81||defined N900||
		memset(cardnostr, 0, sizeof(cardnostr));
		cls_printf("每张卡将以4种模式进行测试.请输入用例磁卡号%d", cardno);
#else
		sprintf(cardnostr, "%d", cardno);
		cls_printf("每张卡将以4种模式进行测试.请输入用例磁卡号.当前磁卡号:");
#endif	
		if((ret=lib_kbgetinput(cardnostr, 0, sizeof(cardnostr)-1, &unLen, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
			continue;
		else
			cardno=atoi(cardnostr);

		if(cardno<1||cardno>MAXCARDCNT)
		{
			cardno=0;
			continue;
		}

		mode=0;//复位
		while(mode<4)
		{
#if SCR_DISABLE	//重定向到串口
			sprintf(pMsg,"请%s速%s刷%d号用例卡(%02d秒)→\n",(mode/2)?"快":"慢", (mode%2)?"反":"正",cardno, MAXWAITTIME);
#if 0//ANDIROD_ENABLE
			NDK_AZPortWrite(g_com, strlen(pMsg), pMsg);
#else
			NDK_PortWrite(g_com, strlen(pMsg), pMsg);
#endif
#else
#if ANDIROD_ENABLE
			cls_printf("请%s速%s刷%d号用例卡(%02d秒)→\n",(mode/2)?"快":"慢", (mode%2)?"反":"正",cardno, MAXWAITTIME);
#else
			cls_printf("请  速  刷%d号用例卡(%02d秒)→\n", cardno, MAXWAITTIME);
			GUI_DispStringEx(12, 0, (mode/2)?"快":"慢", TEXT_ATTRIBUTE_REVERSE);
			GUI_DispStringEx(36, 0, (mode%2)?"反":"正", TEXT_ATTRIBUTE_REVERSE);
#endif
#endif
			ret=readcard3(TK1_2_3, MAXWAITTIME, track2, track3, track1, &err);

			switch(ret)
			{
			case NDK_ERR_TIMEOUT:
				continue;
			case STRIPE:
				check_stripe_errcode(cardno, track2, track3, intptr[mode], &err);
				mode++;
				break;
			case NDK_ERR_QUIT://ESC
			default:
				cls_show_msg("line %d:读%d号用例卡失败(%d)", __LINE__, cardno, ret);
				*intptr[mode]|=(1<<(cardno-1));
				*intptr[mode]|=(1<<(cardno-1+16));
				mode++;
				break;
			}
		}
	}

	if(*perrcard1==0&&*perrcard2==0&&*perrcard3==0&&*perrcard4==0)
		return SUCC;
	else
		return FAIL;
}

/****************************************************************
* function name 	 	: mag
* functional description 	: mag模块入口主函数,可根据需要扩展
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	  chenfm         20130104	created
*				  	
*****************************************************************/
void mag(PF *list, unsigned char id[])
{
	/*private & local definition*/

	/*process body*/
	if(auto_flag==1)//不支持自动测试
	{
		g_UCID = 0;
		send_result("%s不支持自动测试，请使用手动测试验证", TESTMODULE);
		return;
	}
	cls_show_msg1(1, "%s测试...", TESTMODULE);
	process(list, id);
	return;
}

