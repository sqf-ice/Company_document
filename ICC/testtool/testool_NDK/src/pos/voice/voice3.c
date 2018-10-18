/************************************************************************
* copyright(c)2005-2016 Newland Co. Ltd. All rights reserved
* module			: 音频
* file name			: 3.c
* Author 			: 
* version			: 
* DATE				:20160511
* directory 			: 
* description			: 测试NDK_PlayVoice是否可以正常播放第0段到第11段的音频文件
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_PlayVoice"
#define  MAXWAITTIME 30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: voice3
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*					    zhengry		 20160511		created
*****************************************************************/
void voice3(void)
{
	/*private & local definition*/
	int ret = 0, VoiceId = 0, nKeyin=0;
	char *voice_select[12] = {
		"收款成功",
		"收款失败",
		"交易成功",
		"交易失败",
		"请展示付款码",
		"请挥卡",
		"收款成功！谢谢惠顾！",
		"钱箱声",
		"爆米花声",
		"您好",
		"蜂鸣器声",
		"完成声",
	};

	/*process body*/	
	cls_show_msg1(2,"测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:参数异常测试
	if((ret=NDK_PlayVoice(-1)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_PlayVoice(12)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:正常测试,增加延时,播报完语音后应无噪音出现
	for(VoiceId=0; VoiceId<7; VoiceId++) //12--7
	{
		cls_printf("测试语音播报为:%s中...", voice_select[VoiceId]);
		if((ret=NDK_PlayVoice(VoiceId)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, VoiceId);
			RETURN;
		}
		NDK_SysDelay(30);
		nKeyin=cls_show_msg1(MAXWAITTIME, "语音播报内容为:%s,且播报完语音后应无噪音出现.是[ENTER],否则[其他].", voice_select[VoiceId]);
		if( nKeyin != ENTER && nKeyin != 0)
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			RETURN;
		}
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}
