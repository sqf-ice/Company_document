/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 音频
* file name			: 2.c
* Author 			: 
* version			: 
* DATE				:20131029
* directory 			: 
* description			: 测试NDK_VoiceCtrl是否可以正常进行播放控制
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_VoiceCtrl"
#define    RIGHTFILE   "yuyin.wav"
#define  MAXWAITTIME 30

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: voice2
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*					  jiangym		20131030		created
*
*****************************************************************/
#if K21_ENABLE
void voice2(void)
{
	/*private & local definition*/
	int ret = 0, nKeyin=0;
	uint VoiceId = 5, i = 0, memTmp = 0;
	uint outvalue =0,err_value=0;
	char tmp[256] = {0};
#if defined ME68
	int j=0;
#endif

	/*process body*/	
	cls_show_msg1(2,"测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例

	//case1:开机默认音量为最大值6
	if(((ret=NDK_VoiceCtrl(0,VOICE_CTRL_GET_VOLUME,&outvalue))!=NDK_OK)/*||(outvalue!=6)*/)
	{
		send_result("line %d:%s测试失败(ret=%d, volume=%d)", __LINE__, TESTAPI, ret, outvalue);
		RETURN;
	}
#if defined ME68 //夏新默认最大值为5
	if(outvalue!=5)
#else
	if(outvalue!=6)
#endif
	{
		send_result("line %d:%s测试失败(ret=%d, volume=%d)", __LINE__, TESTAPI, ret, outvalue);
		RETURN;
	}
		
	//case2:不支持的设置类型0~5预期返回参数错误
	for(i=VOICE_CTRL_START;i<VOICE_CTRL_VOLUME;i++)
	{
		if((ret=NDK_VoiceCtrl(0,i,&outvalue)) != NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(i=%d,ret=%d)", __LINE__, TESTAPI, i, ret);
			RETURN;
		}
	}

	//case3:设置音量值为非法值0和7预期返回失败
	err_value = 0;
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_VOLUME,&err_value)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	err_value = 7;
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_VOLUME,&err_value)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#if defined ME68 //ME68新增设置蜂鸣器音量参数
	err_value = 7;
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_BEPP_VOLUME,&err_value)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	//case4:获取音量时,存放值的地址为NULL预期返回失败
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_GET_VOLUME,NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case5:设置音量为1~6应该成功,设置后获取的值应该为1~6,且音频播放的实际效果为音量逐渐增大
	for(i=1;i<=6;i++)
	{
		if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_VOLUME,&i)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(invalue=%d,ret=%d)", __LINE__, TESTAPI, i, ret);
			RETURN;
		}
		if(((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_GET_VOLUME,&outvalue))!=NDK_OK)||(outvalue!=i))
		{
			send_result("line %d:%s测试失败(ret=%d, outvalue=%d, invalue=%d)", __LINE__, TESTAPI, ret, outvalue, i);
			RETURN;
		}
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "本语音播放的音量为%d", i);
		if((ret=NDK_VoiceLoadFile(12, tmp)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(invalue=%d,ret=%d)", __LINE__, TESTAPI, i, ret);
			RETURN;
		}
		NDK_SysDelay(30);
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "前面播放的语音音量是否逐渐增大.是[ENTER],否则[其他].");
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	sprintf(tmp, "请确认本语音播放的音量每2秒增大1级");//语音播报只支持最多37字节数据，汉字最多17个。
	if((ret=NDK_VoiceLoadFile(12, tmp)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//语音播报只支持最多37字节数据，汉字最多17个   20180717 add by wangkai
	sprintf(tmp, "语音播报的内容最多只支持37字节数据.");
	if((ret=NDK_VoiceLoadFile(12, tmp)) != NDK_ERR_WLM_SEND_AT_FAIL)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
#if defined ME68 
	//case7:设置蜂鸣器音量为0~6应该成功,且蜂鸣器播放的实际效果为音量逐渐增大
	for(i=0;i<=6;i++)
	{
		cls_printf("以下蜂鸣器播放音量为%d",i);
		if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_BEPP_VOLUME,&i)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(invalue=%d,ret=%d)", __LINE__, TESTAPI, i, ret);
			RETURN;
		}
		for(j=0;j<20;j++)
			NDK_SysBeep();
		NDK_SysMsDelay(100);
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "前面播放的语音音量是否逐渐增大.是[ENTER],否则[其他].");
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
#endif
	
	//case6:验证语音播放过程中调节音量可以立即生效
	for(i=1;i<=6;i++)
	{
		
		//NDK_SysDelay(20);
		if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_VOLUME,&i)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(i=%d,ret=%d)", __LINE__, TESTAPI, i, ret);
			RETURN;
		}
		if(((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_GET_VOLUME,&outvalue))!=NDK_OK)||(outvalue!=i))
		{
			send_result("line %d:%s测试失败(ret=%d, volume=%d, i=%d)", __LINE__, TESTAPI, ret, outvalue, i);
			RETURN;
		}
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "前面播放的语音音量是否逐渐增大.是[ENTER],否则[其他].");
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//结束测试	
	send_result("%s测试通过", TESTAPI);
	return;
}
#else
void voice2(void)
{
	/*private & local definition*/
	int ret = 0,nKeyin =0;
	uint VoiceId = 5, ivalue = 0;
		
	/*process body*/	
	cls_show_msg1(2,"测试%s...", TESTAPI);//屏幕提示当前测试所在模块与用例
	//语音内容:请刷卡,请重刷,余额不足,刷卡成功
	cls_show_msg(MAXWAITTIME, "确保音频文件yuyin.wav已经下载到测试程序路径目录下,任意键继续");
	
	//为了跳过上面的提示信息,下面的提示信息将显示到文件加载成功正常播放后
	cls_printf("语音内容:请刷卡,请重刷,余额不足,刷卡成功,刷卡成功,谢谢...");
	
	//case1:未调用NDK_VoiceLoadFile,返回失败
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_START,2)) != NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:正常装载音频文件后参数非法:emCtrlId非法 unValue非法(不是0-4)
	VoiceId = 3;
	if((ret=NDK_VoiceLoadFile(VoiceId,RIGHTFILE)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_START-1,2)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_VOLUME+1,2)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_START,5)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case3:重新播放--停止播放
	cls_printf("测试播放-停止");
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_START,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(2);
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_STOP,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(1);
	nKeyin=cls_show_msg1(MAXWAITTIME, "语音播放\"请刷卡\"后停止,正确[Enter],错误[其他]");
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case4:播放-暂停-恢复-停止
	cls_printf("测试播放-暂停-恢复-停止");
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_START,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(2);
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_PAUSE,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(2);
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_RESUME,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(1);
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_STOP,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(1);
	nKeyin=cls_show_msg1(MAXWAITTIME, "播放\"请刷卡\"后暂停,又恢复播放\"请重刷\"后停止,正确[Enter],错误[其他]");
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case5:播放-打开静音-关闭静音-停止
	//打开静音时候也在读，已经把"请重刷,余额不足"读完了,关闭静音后就读"刷卡成功"
	cls_printf("测试播放-打开静音-关闭静音-停止");
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_START,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(2);
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_MUTE,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(3);
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_NMUTE,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(2);
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_STOP,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	sleep(1);
	nKeyin=cls_show_msg1(MAXWAITTIME, "播放\"请刷卡\"后静音,又恢复播放\"刷卡成功\"后停止,正确[Enter],错误[其他]");
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//case6:播放-音量控制-停止
	cls_printf("测试播放-音量控制-停止");
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_START,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	for(ivalue=0;ivalue<=4;ivalue++)
	{
		if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_VOLUME,ivalue)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		sleep(2);
	}
	if((ret=NDK_VoiceCtrl(VoiceId,VOICE_CTRL_STOP,2)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	nKeyin=cls_show_msg1(MAXWAITTIME, "播放音量从小到大,正确[Enter],错误[其他]");
	if( nKeyin != ENTER && nKeyin != 0)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}

	//结束测试	
	send_result("%s测试通过", TESTAPI);
	return;
}
#endif

