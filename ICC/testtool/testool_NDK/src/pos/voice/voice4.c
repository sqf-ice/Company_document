/************************************************************************
* copyright(c)2005-2016 Newland Co. Ltd. All rights reserved
* module			: 音频
* file name			: 4.c
* Author 			: 
* version			: 
* DATE				:20170615
* directory 			: 
* description			: 测试NDK_PlayWavFile能否正常播放WAV音频文件
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_PlayWavFile"
#define  NOEXIST		  "noexist.wav"
#define  RIGHTFILE 	  "test.wav"

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
*						  jiangym			20170613     created
*****************************************************************/
void voice4(void)
{
	/*private & local definition*/
	int ret = 0;

	/*process body*/	
	if(cls_show_msg1(30, "测试%s...请确保音频文件%s已下载,ESC退出", TESTAPI,RIGHTFILE)==ESC)
		return;
	
	//case1:参数异常测试
	if((ret=NDK_PlayWavFile(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_PlayWavFile(NOEXIST)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case2:正常测试
	if((ret=NDK_PlayWavFile(RIGHTFILE)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(cls_show_msg("是否听到音乐播放,正确按[Enter],错误按[其他]")!=ENTER)
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		RETURN;
	}
	
	//case3:音频文件格式不正确,会播放,但是解码不正确,播出来的是吵杂的
	//开发文档已说明只能播放WAV的故此点不测
	
	send_result("%s测试通过", TESTAPI);
	return;
}

