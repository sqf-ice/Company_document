/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 综合模块
* file name			: 92.c
* Author 			: 
* version			: V1.0
* DATE				:
* directory 			: 
* description			: SP10_T测试
* related document	: 中国银联电子签字板规范.pdf和电子签名板自定义指令.doc文档
*				  
************************************************************************
* log			: 
* Revision 1.0  20110620 10:10 jennywone163
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"SP10_T签名板"

#define	MAXWAITTIME  135  //135s
#define TESTIMAGE     "test.bmp"
#define DEFAULTIMAGE  "ico_nc.bmp"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	:
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		  jiangym 		   	2017/04/26		created
*****************************************************************/
static int select_port(void)
{
	/*private & local definition*/
	int nKeyin = 0, port = 0;

	/*process body*/
	while(1)
	{
		nKeyin = cls_show_msg("请选择连接外设的串口:\n①串口1\n②串口2\n");
		switch (nKeyin)
		{
		case '1':
			port = PORT_NUM_COM1;
			break;
		case '2':
			port = PORT_NUM_COM2;
			break;
		default:
			continue;
			break;
		}
		return port;
	}
}

static int set_test_bps(void)
{
	/*private & local definition*/
	int ret = -1;
	
	/*process body*/
	// 1.配置与SP10_T连接的POS串口参数
	cls_show_msg1(2, "将选择外接设备连接的物理口");
	g_devicecom = select_port();

	if((ret=NDK_PortOpen(g_devicecom, "115200,8,N,1,20"))!=NDK_OK)
	{
		cls_show_msg("line %d:初始化POS串口失败ret=%d", __LINE__, ret);
		return FAIL;
	}
	cls_show_msg1(2,"打开串口成功");
	return SUCC;
}

//发送握手请求, 握手响应// 输出flag
static ushort SP10_T_SendHandshake(uchar *flag)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, append[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0};
	
	/*process body*/
	memcpy(data,SP10_T_HANDSHAKE_REQUEST,SP10_T_LEN_CMD);
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;	
	if(memcmp(answer,SP10_T_HANDSHAKE_RESPONSE,1))
		return SP10_T_ERR_HANDSHAKE_RESPONSE;
	else
	{
		if(append[0]==0x00)
			return SP10_T_ERR_HANDSHAKE_APPEND;//签名板未准备好,POS会显示"没有正常的签字板"
	}
	flag[0] = append[appendlen-1];
	return SP10_T_SUCC;
}

//前一笔状态通知及响应
//输入cmdappend: 需要作为输入参数,测试不同输入会有不同的响应 ,需要电子签字编号与签字成功响应的编号对应
//输出append:响应的CMD后面的状态值有四种值:0x01 0x02 0x03 0x04
static ushort SP10_T_FrontStatus(uchar *cmdappend,uchar *append)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0};
	
	/*process body*/
	memcpy(data,SP10_T_FRONT_STATUS_INFORM,SP10_T_LEN_CMD);
	memcpy(data+SP10_T_LEN_CMD,cmdappend,SP10_T_FRONT_STATUS_APPENDLEN);
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD+SP10_T_FRONT_STATUS_APPENDLEN, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;	
	if(!memcmp(answer,SP10_T_FRONT_STATUS_RESPONSE,1))
		return SP10_T_SUCC;
	else
		return SP10_T_ERR_FRONT_STATUS_RESPONSE;	
}

//签字输入请求及响应包含成功或不成功//输入cmdappend(即签字板上显示的)输出signnum:电子签字编号
static ushort SP10_T_SignInput(uchar *cmdappend,uchar *signnum,uchar *data2,char *response)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, append[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0};	
	
	/*process body*/
	memcpy(data,SP10_T_SIGN_INPUT_REQUEST,SP10_T_LEN_CMD);
	memcpy(data+SP10_T_LEN_CMD,cmdappend,SP10_T_SIGN_INPUT_APPENDLEN);
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD+SP10_T_SIGN_INPUT_APPENDLEN , answer, append, &appendlen, MAXWAITTIME))!=SP10_T_SUCC)//这边超时时间需要跟签名板上的最长超时时间对应甚至要更长一点
		return ret;	
	if(!memcmp(answer,response,1) || !memcmp(response,"",1)) //跟预期响应值一样再分别处理
	{
		if(!memcmp(answer,SP10_T_SIGN_SUCC_RESPONSE,1))//成功响应输出电子签字编号
		{
			memcpy(signnum,append,3);
			memcpy(data2,append+3,appendlen-3);
			return SP10_T_SUCC;
		}		
		else if(!memcmp(answer,SP10_T_SIGN_FAIL_RESPONSE,1) && (appendlen == 0))//响应失败判断appendlen是否为0
			return SP10_T_SUCC;
		else
			return SP10_T_ERR_SIGN_RESPONSE;	
	}
	else		
		return SP10_T_ERR_SIGN_RESPONSE;	
}

//签字结束请求及响应
static ushort SP10_T_SignEnd(void)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, append[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0};
	
	/*process body*/
	memcpy(data,SP10_T_SIGN_END_REQUEST,SP10_T_LEN_CMD);
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;	
	if(memcmp(answer,SP10_T_SIGN_END_RESPONSE,1))
		return SP10_T_ERR_SIGN_END_RESPONSE;	
	else
	{
		if(appendlen != 0)
			return SP10_T_ERR_SIGN_END_APPENDLEN;
	}
	return SP10_T_SUCC;
}

//批量传输请求及响应//输出signinfnum1位signnum3位
static ushort SP10_T_BulkTransfer(uchar *signinfnum, uchar *signnum)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, append[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0};
	
	/*process body*/
	memcpy(data,SP10_T_BULK_TRANSFER_REQUEST,SP10_T_LEN_CMD);
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;	
	if(!memcmp(answer,SP10_T_BULK_TRANSFER_RESPONSE,1))//正确响应
	{
		signinfnum[0] = append[0];
		memcpy(signnum,append+1,3);
		return SP10_T_SUCC;
	}
	else
		return SP10_T_ERR_BULK_TRANSFER_RESPONSE;	
}

//批量传输接收成功应答后响应是批量传输响应//输出signinfnum1位signnum3位
static ushort SP10_T_BulkTransferSucc(uchar *signinfnum,uchar *signnum)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, append[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0};
	
	/*process body*/
	memcpy(data,SP10_T_BULK_TRANSFER_SUCC,SP10_T_LEN_CMD);
	memcpy(data+1,signnum,SP10_T_BULK_TRANSFER_APPENDLEN);
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD+SP10_T_BULK_TRANSFER_APPENDLEN, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;	
	if(!memcmp(answer,SP10_T_BULK_TRANSFER_RESPONSE,1))//正确响应
	{
		signinfnum[0] = append[0];
		memcpy(signnum,append+1,3);
		return SP10_T_SUCC;
	}
	else
		return SP10_T_ERR_BULK_TRANSFER_RESPONSE;	
}

//批量传输接收失败//输出signinfnum1位signnum3位
static ushort SP10_T_BulkTransferFail(uchar *signinfnum,uchar *signnum)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, append[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0};
	
	/*process body*/
	memcpy(data,SP10_T_BULK_TRANSFER_FAIL,SP10_T_LEN_CMD);
	memcpy(data+1,signnum,SP10_T_BULK_TRANSFER_APPENDLEN);
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD+SP10_T_BULK_TRANSFER_APPENDLEN, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;	
	if(!memcmp(answer,SP10_T_BULK_TRANSFER_RESPONSE,1))//正确响应
	{
		signinfnum[0] = append[0];
		memcpy(signnum,append+1,3);
		return SP10_T_SUCC;
	}
	else
		return SP10_T_ERR_BULK_TRANSFER_RESPONSE;	
}

//批量结束请求及响应
static ushort SP10_T_BulkEnd(void)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, append[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0};
	
	/*process body*/
	memcpy(data,SP10_T_BULK_END_REQUEST,SP10_T_LEN_CMD);
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;	
	if(memcmp(answer,SP10_T_BULK_END_RESPONSE,1))
		return SP10_T_ERR_BULK_END_RESPONSE;	
	else
	{
		if(appendlen != 0)
			return SP10_T_ERR_BULK_END_APPENDLEN;
	}
	return SP10_T_SUCC;
}

//设置类的命令一般都是进行异常指令设置或者正常指令设置后通过签名输入来看效果
//设置命令均有输入，做不同输入测试
//设置切换签名板模式:设置成功后再继续后续的操作,若设置失败直接返回具体失败值
static ushort SP10_T_SwitchSignMode(uchar cmdappend)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0}, append[SP10_T_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, SP10_T_SWITCH_SIGN_MODE, SP10_T_LEN_CMD);
	data[1] = cmdappend;
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD+1, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;
	if(!memcmp(answer,SP10_T_SWITCH_SIGN_MODE,1))
	{
		if(append[0] != SP10_T_RESPONSE_STATUS_SUCC)
			return SP10_T_ERR_SET_RESPONSE_STATUS;
	}
	return SP10_T_SUCC;
}

//设置签名板的待机图片 //输入由于长度不定需要定义输入长度参数
static ushort SP10_T_SignStandbyImage(uchar *cmdappend, int cmdappendlen)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0}, append[SP10_T_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, SP10_T_SIGN_STANDBY_IMAGE, SP10_T_LEN_CMD);	
	memcpy(data+SP10_T_LEN_CMD,cmdappend,cmdappendlen);
	if((ret=SP10_T_frame_factory(data, cmdappendlen+SP10_T_LEN_CMD, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;
	if(!memcmp(answer,SP10_T_SIGN_STANDBY_IMAGE,1))
	{
		if(append[0] != SP10_T_RESPONSE_STATUS_SUCC)
			return SP10_T_ERR_SET_RESPONSE_STATUS;
	}
	return SP10_T_SUCC;
}

//设置签名板签名超时时间 //输入2位
static ushort SP10_T_SignTimeout(uchar *cmdappend)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0}, append[SP10_T_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, SP10_T_SET_SIGN_TIMEOUT, SP10_T_LEN_CMD);
	memcpy(data+SP10_T_LEN_CMD,cmdappend,SP10_T_SET_TIMEOUT_APPENDLEN);
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD+SP10_T_SET_TIMEOUT_APPENDLEN, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;
	if(!memcmp(answer,SP10_T_SET_SIGN_TIMEOUT,1))
	{
		if(append[0] != SP10_T_RESPONSE_STATUS_SUCC)
			return SP10_T_ERR_SET_RESPONSE_STATUS;
	}
	return SP10_T_SUCC;
}

//设置签名板签名的次数输入 1位
static ushort SP10_T_SignNum(uchar cmdappend)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0}, append[SP10_T_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, SP10_T_SET_SIGN_NUM, SP10_T_LEN_CMD);
	data[1] = cmdappend;
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD+1, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;
	if(!memcmp(answer,SP10_T_SET_SIGN_NUM,1))
	{
		if(append[0] != SP10_T_RESPONSE_STATUS_SUCC)
			return SP10_T_ERR_SET_RESPONSE_STATUS;
	}
	return SP10_T_SUCC;
}

//设置签名笔迹粗细//输入1位
static ushort SP10_T_SignWriting(uchar cmdappend)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0}, append[SP10_T_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, SP10_T_SET_SIGN_WRITING, SP10_T_LEN_CMD);
	data[1] = cmdappend;
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD+1, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;
	if(!memcmp(answer,SP10_T_SET_SIGN_WRITING,1))
	{
		if(append[0] != SP10_T_RESPONSE_STATUS_SUCC)
			return SP10_T_ERR_SET_RESPONSE_STATUS;
	}
	return SP10_T_SUCC;
}

//设置输出图片大小 // 输入多位
static ushort SP10_T_OutImageSize(uchar *cmdappend)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0}, append[SP10_T_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, SP10_T_SET_IMAGE_SIZE, SP10_T_LEN_CMD);
	memcpy(data+SP10_T_LEN_CMD,cmdappend,SP10_T_IMAGE_SIZE_APPENDLEN);
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD+SP10_T_IMAGE_SIZE_APPENDLEN, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;
	if(!memcmp(answer,SP10_T_SET_IMAGE_SIZE,1))
	{
		if(append[0] != SP10_T_RESPONSE_STATUS_SUCC)
			return SP10_T_ERR_SET_RESPONSE_STATUS;
	}
	return SP10_T_SUCC;
}

//设置输出图片背景 //输入1位
static ushort SP10_T_OutBackImage(uchar cmdappend)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0}, append[SP10_T_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, SP10_T_SET_IMAGE_BACKGROUND, SP10_T_LEN_CMD);
	data[1] = cmdappend;
	if((ret=SP10_T_frame_factory(data, SP10_T_LEN_CMD+1, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;
	if(!memcmp(answer,SP10_T_SET_IMAGE_BACKGROUND,1))
	{
		if(append[0] != SP10_T_RESPONSE_STATUS_SUCC)
			return SP10_T_ERR_SET_RESPONSE_STATUS;
	}
	return SP10_T_SUCC;
}

//读取声波数据  目前不支持 没有生产此硬件
#if 0
static ushort SP10_T_ReadWaveData(uchar *cmdappend)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP10_T_MAXLEN_DATA] = {0}, answer[1] = {0}, append[SP10_T_MAXLEN_DATA] = {0};
	
	/*process body*/
	memcpy(data, SP10_T_READ_WAVE_DATA, SP10_T_LEN_CMD);
	memcpy(data+SP10_T_LEN_CMD,cmdappend,SP10_T_READ_WAVE_APPENDLEN);
	if((ret=SP10_T_frame_factory(data, SP10_T_READ_WAVE_APPENDLEN+SP10_T_LEN_CMD, answer, append, &appendlen, 0))!=SP10_T_SUCC)
		return ret;
	if(!memcmp(answer,SP10_T_READ_WAVE_DATA,1))
	{
		if(append[0] != SP10_T_RESPONSE_STATUS_SUCC)
			return SP10_T_ERR_SET_RESPONSE_STATUS;
		//else  可以把读取到的数据传出来
	}
	return SP10_T_SUCC;
}
#endif

//上层调用进行测试
//基本流程:握手请求--签字输入请求--签字结束请求 输出当前签字编号及其后面的数据
static void test_base_sign(uchar *signnum,uchar *data,char *response)
{
	ushort ret=SP10_T_FAIL;
	uchar inputdata[8]="newland8",saveflag[1] = {0};
	
	//握手请求
	if((ret=SP10_T_SendHandshake(saveflag))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:握手请求失败(%04x)", __LINE__, ret);
		return;
	}
	//签字输入请求
	if((ret=SP10_T_SignInput(inputdata,signnum,data,response))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:签字输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	//签字结束请求
	if((ret=SP10_T_SignEnd())!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:签字结束请求失败(%04x)", __LINE__, ret);
		return;
	}
}

//电子签字输入传输测试,包括签字输入请求后签字完成成功响应或者是签字完成异常响应以及没有发送签字结束请求
static void test_sign_flow(void)
{
	uchar signnum[3] = {0},data[SP10_T_MAXLEN_DATA] = {0};

	//case1:签字输入请求签字完成成功响应
	cls_show_msg1(2,"签名板提示签名时候请签名并确认");	
	test_base_sign(signnum,data,SP10_T_SIGN_SUCC_RESPONSE);
	cls_show_msg1(2,"签名板提示签名时候请重输签名后并确认");	
	test_base_sign(signnum,data,SP10_T_SIGN_SUCC_RESPONSE);
	
	//case2:签字输入请求签字完成异常响应
	cls_show_msg1(2,"签名板提示签名时候请按取消");	
	test_base_sign(signnum,data,SP10_T_SIGN_FAIL_RESPONSE);
	cls_show_msg1(2,"签名板提示签名时候请重输签名后按取消");	
	test_base_sign(signnum,data,SP10_T_SIGN_FAIL_RESPONSE);
	
	//case3:签字输入请求后超时异常响应
	cls_show_msg1(2,"签名板提示签名时候请等待超时");	
	test_base_sign(signnum,data,SP10_T_SIGN_FAIL_RESPONSE);

	cls_show_msg1(2,"签字输入传输测试通过!");
	return;
}

//前一笔状态通知及其各种不同响应的测试
static void test_front_status(void)
{
	ushort ret=SP10_T_FAIL;
	uchar signnum[3] = {0}, cmdappend[4] = {0}, status[1] = {0};
	uchar data[SP10_T_MAXLEN_DATA] = {0},signinfnum[1] = {0};
	int i = 0,cnt = 0;

	//测试前置:
	//进行一次握手签字输入请求后把输出的签字编号传给前一笔状态请求
	cls_show_msg1(2,"提示签名时需要签名并确认");
	test_base_sign(signnum,data,SP10_T_SIGN_SUCC_RESPONSE);
	
	//握手请求 (只要有过握手请求后续可不需要握手)
	//case1:前一笔状态通知:签字板保存签字响应为电子签字成功存储
	memcpy(cmdappend,signnum,3);
	cmdappend[3] = 0x00;//签字板保存签字
	if((ret=SP10_T_FrontStatus(cmdappend,status))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:前一笔状态请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(status[0] != 0x02)
	{
		cls_show_msg("line %d:前一笔状态返回失败(0x%2x)", __LINE__, status[0]);
		return;
	}
	
	//case2:前一笔状态通知:签字板不保存签字响应为电子签字已删除
	cmdappend[3] = 0x01;//签字板不保存签字
	if((ret=SP10_T_FrontStatus(cmdappend,status))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:前一笔状态请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(status[0] != 0x01)
	{
		cls_show_msg("line %d:前一笔状态返回失败(0x%2x)", __LINE__, status[0]);
		return;
	}
	
	//case3:在上述基础上(签字板已删除签字)再对 同一签字编号请求且要求保存签字的话会返回签字存储失败
	cmdappend[3] = 0x00;//签字板保存签字
	if((ret=SP10_T_FrontStatus(cmdappend,status))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:前一笔状态请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(status[0]!=0x04)
	{
		cls_show_msg("line %d:前一笔状态返回失败(0x%2x)", __LINE__, status[0]);
		return;
	}
	
	//case4:响应为签字成功存储,存储空间少于10笔 //需要签字90笔  
	//通过批量传输获取当前的剩余的签字数
	if((ret=SP10_T_BulkTransfer(signinfnum,signnum))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:批量传输请求失败(%04x)", __LINE__, ret);
		return;
	}
	cnt=90-signinfnum[0];//计算需要进行签字的笔数
	cls_show_msg("需要再进行%d笔签名,任意键开始测试",cnt);
	for(i=0;i<cnt;i++)
	{
		cls_printf("进行第%d笔签名,提示签名时请签名并确认",i+1);
		test_base_sign(signnum,data,SP10_T_SIGN_SUCC_RESPONSE);
		memcpy(cmdappend,signnum,3);
		cmdappend[3] = 0x00;//签字板保存签字
		if((ret=SP10_T_FrontStatus(cmdappend,status))!=SP10_T_SUCC)
		{	
			cls_show_msg("line %d:前一笔状态请求失败(%04x)", __LINE__, ret);
			return;
		}
		if(status[0] != 0x02)
		{
			cls_show_msg("line %d:前一笔状态返回失败(0x%2x)", __LINE__, status[0]);
			return;
		}
	}
	//执行91-100笔均返回0x03:剩余存储空间低于10笔
	for(i=0;i<10;i++)
	{
		cls_printf("进行第%d笔签名,提示签名时请签名并确认",91+i);
		test_base_sign(signnum,data,SP10_T_SIGN_SUCC_RESPONSE);
		memcpy(cmdappend,signnum,3);
		cmdappend[3] = 0x00;//签字板保存签字
		if((ret=SP10_T_FrontStatus(cmdappend,status))!=SP10_T_SUCC)
		{	
			cls_show_msg("line %d:前一笔状态请求失败(%04x)", __LINE__, ret);
			return;
		}
		if(status[0] != 0x03)
		{
			cls_show_msg("line %d:前一笔状态返回失败(0x%2x)", __LINE__, status[0]);
			return;
		}
	}
	//执行第101笔会返回0x04 存储失败
	cls_printf("提示签名时请签名并确认");
    test_base_sign(signnum,data,SP10_T_SIGN_SUCC_RESPONSE);
	memcpy(cmdappend,signnum,3);
	cmdappend[3] = 0x00;//签字板保存签字	
	if((ret=SP10_T_FrontStatus(cmdappend,status))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:前一笔状态请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(status[0]!=0x04)
	{
		cls_show_msg("line %d:前一笔状态返回失败(0x%2x)", __LINE__, status[0]);
		return;
	}
	cls_show_msg("前一笔状态测试通过,请继续在菜单中选择批量传输测试,任意键继续");
	return;
}

//批量传输测试
static void test_bulk_transfer(void)
{
	ushort ret=SP10_T_FAIL;
	uchar signnum[3] = {0}, signinfnum[1] = {0}, signinfnum1[1] = {0},saveflag[1] = {0};
	int i = 0;

	cls_printf("批量传输测试中...");
	//握手请求
	if((ret=SP10_T_SendHandshake(saveflag))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:握手请求失败(%04x)", __LINE__, ret);
		return;
	}
	//case1:批量传输请求--批量传输接收成功应答 --签字板响应的signinfnum要减1,与批量传输请求时响应的不一样
	if((ret=SP10_T_BulkTransfer(signinfnum,signnum))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:批量传输请求失败(%04x)", __LINE__, ret);
		return;
	}
	//获取signinfnum的值,如果为0表示签字板无签字信息就没有的传输就不执行应答,如果大于0才执行应答
	//如果值为0xF1表示不支持存储那么也就无需传输;如果只剩一笔了发送接收成功应答后不再返回(设计流程上决定的)
	if(signinfnum[0]>1 && signinfnum[0] != 0xF1)
	{
		//批量传输接收成功应答
		if((ret=SP10_T_BulkTransferSucc(signinfnum1,signnum))!=SP10_T_SUCC)
		{	
			cls_show_msg("line %d:批量传输成功应答失败(%04x)", __LINE__, ret);
			return;
		}
		if(signinfnum[0] != signinfnum1[0]+1)
		{
			cls_show_msg("line %d:批量传输成功应答失败(0x%2x,0x%2x)", __LINE__, signinfnum[0], signinfnum1[0]);
			return;
		}
	}	
	//case2:批量传输请求--批量传输接收失败应答 --签字板响应的signinfnum没有变化,与批量传输请求时响应的一样
	if((ret=SP10_T_BulkTransfer(signinfnum,signnum))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:批量传输请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(signinfnum[0]>0 && signinfnum[0] != 0xF1) //每次批量传输请求后需要查看signinfnum是否大于0后再做应答
	{
		//批量传输接收失败应答 
		if((ret=SP10_T_BulkTransferFail(signinfnum1,signnum))!=SP10_T_SUCC)
		{	
			cls_show_msg("line %d:批量传输失败应答失败(%04x)", __LINE__, ret);
			return;
		}
		if(signinfnum[0] != signinfnum1[0])
		{
			cls_show_msg("line %d:批量传输失败应答失败(0x%2x,0x%2x)", __LINE__, signinfnum[0], signinfnum1[0]);
			return;
		}
	}
	//case3:循环进行批量请求及接收成功应答
	if((ret=SP10_T_BulkTransfer(signinfnum,signnum))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:批量传输请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(signinfnum[0]>0 && signinfnum[0] != 0xF1) 
	{	
		if(signinfnum[0]>1)
		{	
			for(i=0;i<(signinfnum[0]-1);i++)
			{
				if((ret=SP10_T_BulkTransferSucc(signinfnum1,signnum))!=SP10_T_SUCC)
				{	
					cls_show_msg("line %d:批量传输成功应答失败(%04x)", __LINE__, ret);
					return;
				}
			}
		}
		//只有一笔或者最后一笔发送应答成功不会有返回但实际再执行一次批量传输获取其signinfnum要为0
		if((ret=SP10_T_BulkTransferSucc(signinfnum1,signnum))!=SP10_T_ERR_FRAME_STX)
		{	
			cls_show_msg("line %d:批量传输成功应答失败(%04x)", __LINE__, ret);
			return;
		}
	}
	if((ret=SP10_T_BulkTransfer(signinfnum,signnum))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:批量传输请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(signinfnum[0] != 0)
	{
		cls_show_msg("line %d:批量传输请求响应失败(0x%2x)", __LINE__, signinfnum[0]);
		return;
	}
	//批量结束请求
	if((ret=SP10_T_BulkEnd())!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:批量结束请求失败(%04x)", __LINE__, ret);
		return;
	}
	cls_show_msg1(2,"批量传输测试通过!");
	return;
}

//设置类指令测试
//设置切换签名板模式,设置签名板不保存签名和保存签名
static void test_switch_mode(void)
{
	ushort ret=SP10_T_FAIL;
	uchar  saveflag[1] = {0};

	//case1:验证默认是保存签名的
	//握手请求响应中的flag位应该是0x00 表示签名板支持存储
	if((ret=SP10_T_SendHandshake(saveflag))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:握手请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(saveflag[0] != 0x01)
	{	
		cls_show_msg("line %d:签名板是否支持存储返回失败(0x%2x)", __LINE__, saveflag[0]);
		return;
	}	
	
	//case2:异常参数设置
	if((ret=SP10_T_SwitchSignMode(0x02))!=SP10_T_ERR_SET_RESPONSE_STATUS)
	{	
		cls_show_msg("line %d:签名板模式设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	
	//case2:设置0x00:不保存签名 通过握手请求及响应来判断
	if((ret=SP10_T_SwitchSignMode(0x00))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:签名板模式设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}	
	//握手请求响应中的flag位应该是0x00 表示签名板不支持存储
	if((ret=SP10_T_SendHandshake(saveflag))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:握手请求失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if(saveflag[0] != 0x00)
	{	
		cls_show_msg("line %d:签名板是否支持存储返回失败(0x%2x)", __LINE__, saveflag[0]);
		goto ERR;
	}
	
	//case3:设置0x01:保存签名
	if((ret=SP10_T_SwitchSignMode(0x01))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:签名板模式设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	//握手请求响应中的flag位应该是0x00 表示签名板支持存储
	if((ret=SP10_T_SendHandshake(saveflag))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:握手请求失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if(saveflag[0] != 0x01)
	{	
		cls_show_msg("line %d:签名板是否支持存储返回失败(0x%2x)", __LINE__, saveflag[0]);
		goto ERR;
	}	
	cls_show_msg1(2,"签名板模式设置测试通过!");
ERR:
	if((ret=SP10_T_SwitchSignMode(0x01))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:签名板模式设置失败(%04x)", __LINE__, ret);
		return;
	}
	return;
}

//设置签名板待机图片
static void test_standby_image(void)
{
	ushort ret=SP10_T_FAIL;
	uchar cmdappend[SP10_T_MAXLEN_DATA] = {0};
	uint punSize = 0, readlen = 0; 
	int fp = 0,cnt = 0,num = 0,value = 0,i = 0, j = 0;
	char buf[256] = {0};
	char *imagestr[] = {"彩屏POS开机界面","新大陆logo图标"};

	if(cls_show_msg("确保已下载图片%s,%s到main所在目录下,ESC退出",TESTIMAGE,DEFAULTIMAGE)==ESC)
		return;
	for(j=0;j<2;j++)
	{	
		if(j==0) //更换图片测试
		{
			//先计算图片大小,及分几包传输等
			fp=NDK_FsOpen(TESTIMAGE, "r");
			if((ret=NDK_FsFileSize(TESTIMAGE, &punSize))!= NDK_OK)
				cls_show_msg("获取文件大小失败(ret:%d)",ret);
		}
		else //改为默认图片
		{
			fp=NDK_FsOpen(DEFAULTIMAGE, "r");
			if((ret=NDK_FsFileSize(DEFAULTIMAGE, &punSize))!= NDK_OK)
				cls_show_msg("获取文件大小失败(ret:%d)",ret);
		}
		cnt = punSize/256;
		value = punSize%256;
		if(value != 0)
			num=cnt;
		else
			num=cnt-1;
		cls_printf("图片传输中...耐心等待...");
		for(i=0;i<cnt;i++)
		{
			cmdappend[0]=(i&0xffff)>>8;
			cmdappend[1]=(i)&0xff;
			cmdappend[2]=((num-i)&0xffff)>>8;
			cmdappend[3]=(num-i)&0xff;
			if((readlen=NDK_FsRead(fp,buf,256))!=256)
			{	
				cls_show_msg("读文件失败(readlen:%d)",readlen);
				return;
			}
			memcpy(cmdappend+4,buf,256);
			if((ret=SP10_T_SignStandbyImage(cmdappend,260))!=SP10_T_SUCC)
			{	
				cls_show_msg("line %d:待机图片设置失败(%04x)", __LINE__, ret);
				return;
			}
		}
		if(value != 0)
		{
			cmdappend[0]=(cnt&0xffff)>>8;
			cmdappend[1]=cnt&0xff;
			cmdappend[2]=0x00;
			cmdappend[3]=0x00;
			if((readlen=NDK_FsRead(fp,buf,value))!=value)
			{	
				cls_show_msg("读文件失败(readlen:%d)",readlen);
				return;
			}
			memcpy(cmdappend+4,buf,value);
			if((ret=SP10_T_SignStandbyImage(cmdappend,value+4))!=SP10_T_SUCC)
			{	
				cls_show_msg("line %d:待机图片设置失败(%04x)", __LINE__, ret);
				return;
			}
		}	
		//图片传输完成后会直接刷新故就不需要签名流程验证
		if(cls_show_msg("签名板上的待机图片已变为%s,正确按[Enter],错误按[其他]",imagestr[j])!=ENTER)
		{
			cls_show_msg("line %d:设置签名板签名次数测试失败", __LINE__);
			return;
		}
		//关闭文件
		NDK_FsClose(fp);
	}	
	cls_show_msg1(2,"待机图片设置测试通过!");
	return;
}

//设置签名板签名超时时间
static void test_sign_timeout(void)
{
	ushort ret=SP10_T_FAIL;
	int i = 0;
	uchar cmdappend[2] = {0},signnum[3] = {0},data[SP10_T_MAXLEN_DATA] = {0};
	char *cmdpara[] = {
						 "\x00\x00", 
						 "\x00\x01",
						 "\x01\x2C",
						 "\x05\xDC",
						 };
	char *parastr[] = {
						 "无超时", 
						 "1s",
						 "30s",
						 "150s",
						 };
	//case1:验证默认时间为150s
	cls_show_msg1(2,"观察签名板右上角超时时间是否为150s,请签名后确认或取消");
	test_base_sign(signnum,data,"");
	if(cls_show_msg("签名板超时时间是否为150s,正确按[Enter],错误按[其他]")!=ENTER)
	{
		cls_show_msg("line %d:设置签名板超时时间测试失败", __LINE__);
		goto ERR;
	}
	
	//设置超时时间后通过握手--签字输入请求来查看电子签名板上的时间
	//case2:设置超时时间为0  即不超时//超时1s//超时时间为30s
    for(i=0;i<(SZ_ARRAY(cmdpara)-1);i++)
	 {	
		memcpy(cmdappend,cmdpara[i],2);
		if((ret=SP10_T_SignTimeout(cmdappend))!=SP10_T_SUCC)
		{	
			cls_show_msg("line %d:签名板超时时间设置失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		cls_show_msg1(2,"观察签名板右上角超时时间是否为%s,请签名后确认或取消",parastr[i]);
		test_base_sign(signnum,data,"");
		if(cls_show_msg("签名板超时时间是否为%s,正确按[Enter],错误按[其他]",parastr[i])!=ENTER)
		{
			cls_show_msg("line %d:设置签名板超时时间测试失败", __LINE__);
			goto ERR;
		}
	}
	cls_show_msg1(2,"签名超时时间设置测试通过!");
ERR:  //恢复成默认
	memcpy(cmdappend,cmdpara[3],2);
	if((ret=SP10_T_SignTimeout(cmdappend))!=SP10_T_SUCC)
	{	 
		cls_show_msg("line %d:签名板超时时间设置失败(%04x)", __LINE__, ret);
		return;
	}		 
	return;
}

//设置签名板签名次数:设置后通过签名板上的重输次数来判断
static void test_sign_num(void)
{
	ushort ret=SP10_T_FAIL;
	uchar signnum[3] = {0},data[SP10_T_MAXLEN_DATA] = {0};

	//case1:测试默认的签名次数为2
	cls_show_msg1(2,"提示签名时请签字后按重输1次,查看重输按钮应消失,再签字确认后退出");
	test_base_sign(signnum,data,"");
	if(cls_show_msg("签名板签名重输一次后重输消失,正确按[Enter],错误按[其他]")!=ENTER)
	{
		cls_show_msg("line %d:设置签名板签名次数测试失败", __LINE__);
		goto ERR;
	}
	
	//case2:设置签名次数为无限制
	if((ret=SP10_T_SignNum(0x00))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:签名次数设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}	
	cls_show_msg1(2,"提示签名时请签字后按重输并重复5次以上,退出时按确认");
	test_base_sign(signnum,data,"");
	if(cls_show_msg("签名板上的签名重输次数可反复至少5次,正确按[Enter],错误按[其他]")!=ENTER)
	{
		cls_show_msg("line %d:设置签名板签名次数测试失败", __LINE__);
		goto ERR;
	}
	
	//case3:设置签名次数为1次
	if((ret=SP10_T_SignNum(0x01))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:签名次数设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}	
	cls_show_msg1(2,"验证签名板上的签名不可重输,提示签名时请签字后确认");
	test_base_sign(signnum,data,"");
	if(cls_show_msg("签名板上无重输选项,正确按[Enter],错误按[其他]")!=ENTER)
	{
		cls_show_msg("line %d:设置签名板签名次数测试失败", __LINE__);
		goto ERR;
	}
	
	cls_show_msg1(2,"签名次数设置测试通过!");
ERR:	//恢复默认
	if((ret=SP10_T_SignNum(0x02))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:签名次数设置失败(%04x)", __LINE__, ret);
		return;
	}
	return;
}

//设置签名板签名笔迹粗细程度
static void test_sign_writing(void)
{
	ushort ret=SP10_T_FAIL;
	uchar signnum[3] = {0},data[SP10_T_MAXLEN_DATA] = {0};

	//case1:验证签名板默认笔迹
	cls_show_msg1(2,"验证签名板上的默认笔迹(略细),提示签名时请签字并确认");
	test_base_sign(signnum,data,"");
	if(cls_show_msg("签名板上的默认笔迹略细,正确按[Enter],错误按[其他]")!=ENTER)
	{
		cls_show_msg("line %d:设置签名笔迹粗细测试失败", __LINE__);
		goto ERR;
	}

	//case2:异常参数设置
	if((ret=SP10_T_SignWriting(0x03))!=SP10_T_ERR_SET_RESPONSE_STATUS)
	{	
		cls_show_msg("line %d:签名笔迹粗细设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	
	//case3:设置笔迹粗细为默认*1/2   文档显示支持实际是不支持
	if((ret=SP10_T_SignWriting(0x02))!=SP10_T_ERR_SET_RESPONSE_STATUS)
	{	
		cls_show_msg("line %d:签名笔迹粗细设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	
	//case4:设置笔迹粗细为默认*2
	if((ret=SP10_T_SignWriting(0x01))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:签名笔迹粗细设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	cls_show_msg1(2,"验证签名板上的笔迹略粗,提示签名时请签字并确认");
	test_base_sign(signnum,data,"");
	if(cls_show_msg("签名板上的笔迹略粗,正确按[Enter],错误按[其他]")!=ENTER)
	{
		cls_show_msg("line %d:设置签名笔迹粗细测试失败", __LINE__);
		goto ERR;
	}
	
	//case5:设置笔迹粗细为默认
	if((ret=SP10_T_SignWriting(0x00))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:签名笔迹粗细设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	cls_show_msg1(2,"验证签名板上的笔迹略细,提示签名时请签字并确认");
	test_base_sign(signnum,data,"");
	if(cls_show_msg("签名板上的笔迹略细,正确按[Enter],错误按[其他]")!=ENTER)
	{
		cls_show_msg("line %d:设置签名笔迹粗细测试失败", __LINE__);
		goto ERR;
	}
	cls_show_msg1(2,"签名笔迹设置测试通过!");
ERR:  //恢复默认
	if((ret=SP10_T_SignWriting(0x00))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:签名笔迹粗细设置失败(%04x)", __LINE__, ret);
		return;
	}
	return;
}

//设置输出图片大小
static void test_out_image_size(void)
{
	ushort ret=SP10_T_FAIL;
	int i = 0;
	uchar cmdappend[4] = {0},signnum[3] = {0},data[SP10_T_MAXLEN_DATA] = {0};
	char *err_para[] = { 
						  "\x00\x77\x00\x32", //119*50
						  "\x00\x77\x00\x64", //119*100
						  "\x01\x41\x00\x32", //321*50
						  "\x01\x41\x00\x64", //321*100
						  "\x00\x78\x00\x31", //120*49
						  "\x00\x78\x00\x65", //120*101
						  "\x01\x40\x00\x31", //320*49
						  "\x01\x40\x00\x65", //320*101
						 };
	char *cmd_para[] = { 
						  "\x00\x78\x00\x32", //120*50
						  "\x01\x40\x00\x64", //320*100
						  "\x01\x2C\x00\x64", //300*100
						 };
	char *parastr[] = { 
						  "120*50", 
						  "320*100",
						  "300*100", 
					    };
	
	//case1:验证默认的输出图片大小为300*100
	cls_show_msg1(2,"查看签名板输出图片大小是否为%s,提示签名时请签字并确认",parastr[2]);
	test_base_sign(signnum,data,"");
	if(cls_show_msg("签名板的输出图片大小是否为%s,正确按[Enter],错误按[其他]",parastr[2])!=ENTER)
	{
		cls_show_msg("line %d:设置输出图片大小测试失败", __LINE__);
		goto ERR;
	}
	
	//case2:异常参数设置
    for(i=0;i<SZ_ARRAY(err_para);i++)
	{
		memcpy(cmdappend,err_para[i],4);
		if((ret=SP10_T_OutImageSize(cmdappend))!=SP10_T_ERR_SET_RESPONSE_STATUS)
		{	
			cls_show_msg("line %d:输出图片设置失败(%04x)", __LINE__, ret);
			goto ERR;
 		}
	}
	
	//case2:正常设置后通过签名输入请求流程来看输出图片大小
	//120*50 //320*100 
        for(i=0;i<(SZ_ARRAY(cmd_para)-1);i++)
	{
		memcpy(cmdappend,cmd_para[i],4);
		if((ret=SP10_T_OutImageSize(cmdappend))!=SP10_T_SUCC)
		{	
			cls_show_msg("line %d:输出图片设置失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		cls_show_msg1(2,"查看签名板输出图片大小是否为%s,提示签名时请签字并确认",parastr[i]);
		test_base_sign(signnum,data,"");
		if(cls_show_msg("签名板的输出图片大小是否为%s,正确按[Enter],错误按[其他]",parastr[i])!=ENTER)
		{
			cls_show_msg("line %d:设置输出图片大小测试失败", __LINE__);
			goto ERR;
		}
	}
	cls_show_msg1(2,"输出图片大小设置测试通过!");
ERR:   //恢复成默认300*100
	memcpy(cmdappend,cmd_para[2],4);
	if((ret=SP10_T_OutImageSize(cmdappend))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:输出图片设置失败(%04x)", __LINE__, ret);
		return;
	}
	return;
}

//设置输出图片背景
static void test_background_image(void)
{
	ushort ret=SP10_T_FAIL;
	uchar signnum[3] = {0},data[SP10_T_MAXLEN_DATA] = {0};

	//case1:验证默认是否为白底黑字
	cls_show_msg("请安装打印纸后任意键继续");
	cls_printf("签名板提示签名时请签名...");
	test_base_sign(signnum,data,SP10_T_SIGN_SUCC_RESPONSE);
	//conf_prn();
	lib_initprn(1);//直接开启边送边打
	if((ret=NDK_PrnImage(320, 100, 0, (char *)data+62))!=NDK_OK)//实际图片是data2域中第63个字节开始，62是开发帮忙定位出来的，文档中没有明确说明；
	{
		cls_show_msg("line %d:设置输出背景图片测试失败", __LINE__);
		goto ERR;
	}	
	if(cls_show_msg("打印出来的是否为白底黑字,正确按[Enter],错误按[其他]")!=ENTER)
	{
		cls_show_msg("line %d:设置输出背景图片测试失败", __LINE__);
		goto ERR;
	}
	
	//case1:异常参数设置
	if((ret=SP10_T_OutBackImage(0x11))!=SP10_T_ERR_SET_RESPONSE_STATUS)
	{	
		cls_show_msg("line %d:输出图片背景设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if((ret=SP10_T_OutBackImage(0x01))!=SP10_T_ERR_SET_RESPONSE_STATUS)
	{	
		cls_show_msg("line %d:输出图片背景设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	
	//case2:设置黑底白字
	cls_printf("签名板提示签名时请签名...");
	if((ret=SP10_T_OutBackImage(0x10))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:输出图片背景设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	test_base_sign(signnum,data,SP10_T_SIGN_SUCC_RESPONSE);
	if((ret=NDK_PrnImage(320, 100, 0, (char *)data+62))!=NDK_OK)
	{
		cls_show_msg("line %d:设置输出背景图片测试失败", __LINE__);
		goto ERR;
	}	
	if(cls_show_msg("打印出来的是否为黑底白字,正确按[Enter],错误按[其他]")!=ENTER)
	{
		cls_show_msg("line %d:设置输出背景图片测试失败", __LINE__);
		goto ERR;
	}
	
	//case2:设置白底黑字
	cls_printf("签名板提示签名时请签名...");
	if((ret=SP10_T_OutBackImage(0x00))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:输出图片背景设置失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	test_base_sign(signnum,data,SP10_T_SIGN_SUCC_RESPONSE);
	if((ret=NDK_PrnImage(320, 100, 0, (char *)data+62))!=NDK_OK)//实际图片是data2域中第63个字节开始，62是开发帮忙定位出来的，文档中没有明确说明；
	{
		cls_show_msg("line %d:设置输出背景图片测试失败", __LINE__);
		goto ERR;
	}	
	if(cls_show_msg("打印出来的是否为白底黑字,正确按[Enter],错误按[其他]")!=ENTER)
	{
		cls_show_msg("line %d:设置输出背景图片测试失败", __LINE__);
		goto ERR;
	}
	cls_show_msg1(2,"输出图片背景设置测试通过!");
ERR:  //恢复成默认
	if((ret=SP10_T_OutBackImage(0x00))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:输出图片背景设置失败(%04x)", __LINE__, ret);
		return;
	}
	return;
}

static void abnomal_test(void)
{
	ushort ret=SP10_T_FAIL;
	uchar inputdata[8]="newland8", data[SP10_T_MAXLEN_DATA] = {0}, saveflag[1] = {0};
	uchar signnum[3] = {0},signinfnum[1] = {0},cmdappend[4] = {0}, status[1] = {0};

	//case1:握手请求--签字输入请求后没有发送签字结束请求签名板会在超时时间到后回到待机界面
	cls_show_msg1(2,"签名板提示签名时请签字并确认");	
	if((ret=SP10_T_SendHandshake(saveflag))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:握手请求失败(%04x)", __LINE__, ret);
		return;
	}
	if((ret=SP10_T_SignInput(inputdata,signnum,data,""))!=SP10_T_SUCC)
	{	
		cls_show_msg("line %d:签字输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	sleep(4);//签名板的超时时间为3s
	if(cls_show_msg("签名板是否回到待机界面,正确按[Enter],错误按[其他]")!=ENTER)
	{
		cls_show_msg("line %d:异常测试失败", __LINE__);
		return;
	}
	
	//case2:未进行握手就进行其他指令请求应该均不成功
	cls_show_msg("把签字板与POS进行重新拔插后任意键继续.");
	//要等待超时返回 差不多要等待155s+20s*3=215s
	cls_printf("异常测试进行中...需等待215s");
	//未进行握手就进行签字输入请求
	if((ret=SP10_T_SignInput(inputdata,signnum,data,""))!=SP10_T_ERR_FRAME_STX)
	{	
		cls_show_msg("line %d:签字输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	//未进行握手进行前一笔状态通知应该失败需要等待20s	
	if((ret=SP10_T_FrontStatus(cmdappend,status))!=SP10_T_ERR_FRAME_STX)
	{	
		cls_show_msg("line %d:前一笔状态请求失败(%04x)", __LINE__, ret);
		return;
	}
	//未进行握手进行签字结束请求
	if((ret=SP10_T_SignEnd())!=SP10_T_ERR_FRAME_STX)
	{	
		cls_show_msg("line %d:签字结束请求失败(%04x)", __LINE__, ret);
		return;
	}
	//未进行握手进行批量传输请求
	if((ret=SP10_T_BulkTransfer(signinfnum,signnum))!=SP10_T_ERR_FRAME_STX)
	{	
		cls_show_msg("line %d:批量传输请求失败(%04x)", __LINE__, ret);
		return;
	}
	cls_show_msg1(2,"异常测试通过!");
	return;
}

static void test_set_function(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	while(1)
       {
		keyin = cls_show_msg("1.签名板模式 2.待机图片\n"
							 "3.超时时间 4.签名次数\n"
							 "5.笔迹粗细 6.图片大小\n"
							 "7.图片背景\n");
		switch(keyin) 
		{
		case '1':
			test_switch_mode();
			break;
		case '2':
			test_standby_image();
			break;
		case '3':
			test_sign_timeout();
			break;	
		case '4':
			test_sign_num();
			break;		
		case '5':
			test_sign_writing();
			break;
		case '6':
			test_out_image_size();
			break;
		case '7':
			test_background_image();
			break;	
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
	}
	return;
}

static void test_sign_function(void)
{
	/*private & local definition*/
	int keyin = 0;

	/*process body*/
	while(1)
       {
		keyin = cls_show_msg("1.签字输入传输\n"
							 "2.前一笔状态\n"
							 "3.批量传输\n"
							 "4.异常测试\n");
		switch(keyin) 
		{
		case '1':
			test_sign_flow();
			break;
		case '2':
			test_front_status();
			break;
		case '3':
			test_bulk_transfer();
			break;	
		case '4':
			abnomal_test();
			break;		
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
	}
	return;
}

void systest92()
{
	int keyin = 0;

	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);
	cls_show_msg("测试前先进行串口设置(可先测自定义指令),任意键继续");
	
	while(1)
    {
        keyin = cls_show_msg("1.银联指令\n"
							 "2.自定义指令\n"
							 "0.串口设置\n");
		switch(keyin) 
		{
		case '1':
			test_sign_function();
			break;
		case '2':
			test_set_function();
			break;
		case '0':
			set_test_bps();
			break;
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
	}	
	return;
}


