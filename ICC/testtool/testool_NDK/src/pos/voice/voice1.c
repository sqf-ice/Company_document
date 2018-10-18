/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 音频
* file name			: 1.c
* Author 			: 
* version			: 
* DATE				:20131028
* directory 			: 
* description			: 测试NDK_VoiceLoadFile是否可以正常加载音频文件
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_VoiceLoadFile"
#if K21_ENABLE

#else
#define  NOEXIST  "noexist.wav"
#define  ERRFILE   "errfile.wav"
#define  RIGHTFILE   "shuaka.wav"
#define  RIGHTFILE1   "duka.wav"
#endif
#define  MAXWAITTIME 30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: voice1
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*					  jiangym		20131030		created
*
*****************************************************************/
#if K21_ENABLE
void voice1(void)
{
	/*private & local definition*/
	int ret = 0, nKeyin=0;
	uint VoiceId = 0;
	char *voice_select[14] = {
		"收款成功",		
		"收款失败",		
		"交易成功",		
		"交易失败",
		"请展示付款码",	//实际:请展示付款码
		"请挥卡",
		"收款成功！谢谢惠顾！",
		"钱箱声",		//实际:特殊音效TTS暂未支持
		"爆米花声",		//实际:特殊音效TTS暂未支持
		"您好",
		"蜂鸣器声",		//实际:特殊音效TTS暂未支持
		"完成声",		//实际:特殊音效TTS暂未支持
		"自定义的播放内容ABC",
		"CHINESE"
	};

	/*process body*/	
	cls_show_msg1(2,"测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:参数异常测试
	if((ret=NDK_VoiceLoadFile(-1, NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_VoiceLoadFile(12, NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:正常测试,增加延时,播报完语音后应无噪音出现
#if !defined ME50NX //夏新ME50N中VoiceId不支持0-11，其它的支持
	for(VoiceId=0; VoiceId<12; VoiceId++)
	{
		cls_printf("测试语音播报为:%s中...", voice_select[VoiceId]);
		if((ret=NDK_VoiceLoadFile(VoiceId, NULL)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, VoiceId);
			RETURN;
		}
		NDK_SysDelay(40);//如果没有播放完语音,就快速按键开始下一段语音播放会失败返回-320,所以增加延时
		nKeyin=cls_show_msg1(MAXWAITTIME, "语音播报内容为:%s,且播报完语音后应无噪音出现.是[ENTER],否则[其他].", voice_select[VoiceId]);
		if( nKeyin != ENTER && nKeyin != 0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, VoiceId);
			RETURN;
		}
	}
#endif

	//case3:正常测试,测试id无效时pszFile可以传入自定义的播放内容,并正确播放
	VoiceId = 12;
	cls_printf("测试语音播报为:%s中...", voice_select[VoiceId]);
	if((ret=NDK_VoiceLoadFile(VoiceId, voice_select[VoiceId])) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, VoiceId);
		RETURN;
	}
	//case4:上一段语音未完成的时候就马上调用下一段语音,预期返回-320
	if((ret=NDK_VoiceLoadFile(VoiceId, voice_select[VoiceId])) != NDK_ERR_WLM_SEND_AT_FAIL)//注意:EC20模块返回成功是正常的。EC20模块和G510模块不同，G510是先发AT指令的模块优先级更高，EC20模块是后发AT指令的语音优先级更高
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, VoiceId);
		RETURN;
	}
	NDK_SysDelay(40);
	nKeyin=cls_show_msg1(MAXWAITTIME, "语音播报内容为:%s,且播报完语音后应无噪音出现.是[ENTER],否则[其他].", voice_select[VoiceId]);
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, VoiceId);
		RETURN;
	}

	//case5:正常测试,测试id无效时pszFile可以传入自定义的英语字母内容,并正确播放
	VoiceId = 13;
	cls_printf("测试语音播报为英语字母串:%s中...", voice_select[VoiceId]);
	if((ret=NDK_VoiceLoadFile(VoiceId, voice_select[VoiceId])) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, VoiceId);
		RETURN;
	}
	NDK_SysDelay(40);
	nKeyin=cls_show_msg1(MAXWAITTIME, "语音播报内容为英语字母串:%s,且播报完语音后应无噪音出现.是[ENTER],否则[其他].", voice_select[VoiceId]);
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case6:测试unVoiceId优先播放,在ID合法为1时即使pszFile有内容也应该播放ID1的语音
	VoiceId = 1;
	cls_printf("测试语音播报为:%s中...", voice_select[VoiceId]);
	if((ret=NDK_VoiceLoadFile(VoiceId, "如果播放本语音则测试失败")) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%d)", __LINE__, TESTAPI, ret, VoiceId);
		RETURN;
	}
	NDK_SysDelay(40);
	nKeyin=cls_show_msg1(MAXWAITTIME, "语音播报内容为:%s,且播报完语音后应无噪音出现.是[ENTER],否则[其他].", voice_select[VoiceId]);
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	send_result("%s测试通过", TESTAPI);
	return;
}
#else
void voice1(void)
{
	/*private & local definition*/
	int ret = 0, nKeyin=0;
	float loadtime = 0.0;
	uint VoiceId = 0;
		
	/*process body*/
	cls_show_msg1(2,"测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	cls_show_msg1(MAXWAITTIME, "确保音频文件errfile.wav、shuaka.wav、duka.wav已经下载到测试程序路径目录下,任意键继续");
	
	//case1: pszFile为NULL
	if((ret=NDK_VoiceLoadFile(VoiceId,NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:不存在的音频文件
	if((ret=NDK_VoiceLoadFile(VoiceId,NOEXIST)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3:音频文件格式(指文件内容)不对
	if((ret=NDK_VoiceLoadFile(VoiceId,ERRFILE)) != NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case4:正常测试
	VoiceId = 1;
	cls_printf("语音将播放:请刷卡");
	lib_StartStopwatch();
	if((ret=NDK_VoiceLoadFile(VoiceId,RIGHTFILE)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	loadtime=lib_ReadStopwatch();
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_START,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(2);//延时为了听到播放效果
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_STOP,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "播放内容为:请刷卡,正确[Enter],错误[其他]");
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case5:相同音频文件加载同一位置
	VoiceId = 1;
	cls_printf("语音继续播放:请刷卡");
	if((ret=NDK_VoiceLoadFile(VoiceId,RIGHTFILE)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_START,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(2);//延时为了听到播放效果
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_STOP,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "播放内容为:请刷卡,正确[Enter],错误[其他]");
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case6:不同音频文件加载到同一位置
	VoiceId = 1;
	cls_printf("语音将播放:读卡成功");
	if((ret=NDK_VoiceLoadFile(VoiceId,RIGHTFILE1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_START,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(2);//延时为了听到播放效果
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_STOP,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "播放内容为:读卡成功,正确[Enter],错误[其他]");
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case7:要支持mp3格式音频文件(目前未支持,之后补上)
	
	//结束测试
	send_result("%s测试通过(NDK_VoiceLoadFile耗时:%f)", TESTAPI, loadtime);
	return;
}
#endif
