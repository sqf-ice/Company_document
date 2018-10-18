/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 综合模块
* file name		: 27.c
* Author 			: 
* version			: V1.0
* DATE			: 20110809
* directory 		: .\SRC\general\
* description		: PP60测试
* related document	: PP60程序员使用手册.doc等相关文档
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
#define	TESTITEM	"PP60"

//用于临时屏蔽一些测不过的用例,测试完毕后去掉!
#define	TEST	ENABLE	//待开发修复所有问题后,将此宏打开,下一个宏关闭
//#define	TEST	DISABLE

#define	DEFAULT_CNT_STR	"15000"	//"3000"//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)	//(1000)

#define	DEBUG_CMD_ALL		(-1)		//for PP60
//调试哪条CMD就定义DEBUG_CMD为哪条(会输出发送与返回数据);正常使用时,关闭此宏
//#define	DEBUG_CMD			'H'	//CMD_H
//#define	DEBUG_CMD			DEBUG_CMD_ALL	//全部命令都调试

#define ONEHZSIZE (64)

/*------------global variables definition-----------------------*/
static int g_PP60type = DEVICE_PP60RF/*DEVICE_PP60PT*/;
static int g_adjust = 0;
static int g_algtype = PP60_TDESFLAG;//支持国密SM4算法导入的开关20160303
static int g_LCDSize = 0; //大屏密码键盘 开关 支持大屏则置为1,例如SP10_T彩屏;否则为0

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef struct data_result {
	int len;
	uchar *data;
} DATA_RESULT;
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
int conf_NL829_bps(void)
{
	/*private & local definition*/
	int nKeyin = 0, loop = 0;
	char *bps[] = {"9600", "19200", "38400", "57600", "115200"};
	static int first = TRUE;//首次开机上电的标志
	
	/*process body*/
	// 1.配置与NL829连接的POS串口参数
	cls_show_msg1(2, "将进行连接外设串口的参数配置");
	g_devicecom = select_serial();

	// 2.配置NL829串口参数
	cls_show_msg1(2, "将进行NL829串口的参数(bps)配置");
	do
	{
		nKeyin = cls_show_msg("1.9600   2.19200\n3.38400  4.57600\n5.115200");
#define MAXMENUITEM ('5')

		switch(nKeyin)
		{
		case ESC:
			cls_show_msg1(2, "将设置为BPS57600(默认)...");
			nKeyin = '4';//转义
		case '1':
		case '2':
		case '3':
		case '4':	
		case MAXMENUITEM://'5':
			//if(initaux(g_devicecom, (first==TRUE)?BPS57600:g_devicecombps, DB8|STOP1|NP)!=SUCC)//first之后的初始化不是必须的,还可以做一下
			if(NDK_PortOpen(g_devicecom, (first==TRUE)?"57600":g_devicecombps)!=NDK_OK)//first之后的初始化不是必须的,还可以做一下
			{
				cls_show_msg1(2, "line %d:初始化POS串口失败", __LINE__);
				return FAIL;
			}
			cls_printf("设置外设BPS中...");
			if(NL829_SetBPS(nKeyin-'1')!=NL829_SUCC)
			{
				cls_printf("外设BPS自动适配中...请耐心等待");
				//若不成功,对所有可能的BPS进行尝试
				for(; loop<SZ_ARRAY(bps); loop++)
				{
					NDK_PortOpen(g_devicecom, bps[loop]);//initaux(g_devicecom, bps[loop], DB8|STOP1|NP);
					if(NL829_SetBPS(nKeyin-'1')==NL829_SUCC)
						break;
				}
				if(loop==SZ_ARRAY(bps))
				{
					cls_show_msg1(2, "line %d:设置NL829的BPS失败,请重启后再尝试", __LINE__);
					return FAIL;
				}
			}
			break;
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;			
		}
	}while(nKeyin<'1'||nKeyin>MAXMENUITEM);
#undef MAXMENUITEM	

	// 3.根据NL829当前BPS调整POS串口的BPS
	if(NDK_PortOpen(g_devicecom, bps[nKeyin-'1'])!=NDK_OK)//if(initaux(g_devicecom, bps[nKeyin-'1'], DB8|STOP1|NP)!=SUCC)
	{
		cls_show_msg1(2, "line %d:初始化POS串口失败", __LINE__);
		return FAIL;
	}
	strcpy(g_devicecombps, bps[nKeyin-'1']);//g_devicecombps = bps[nKeyin-'1'];//g_bps根据配置进行调整备份
	first = FALSE;
	return SUCC;
}

static int PP60_code(uchar *in, int inlen, uchar *out)
{
	/*private & local definition*/
	int id = 0;
	
	/*process body*/
	for(; id<inlen; id++)
	{
		out[id*2] = PP60_CODE_H(in[id]);
		out[id*2+1] = PP60_CODE_L(in[id]);
	}
	return inlen*2;
}

static int PP60_decode(uchar *in, int inlen, uchar *out)
{
	/*private & local definition*/
	int id = 0;
	
	/*process body*/
	if(inlen%2)
		return FAIL;
	for(; id<inlen; id+=2)
		out[id/2] = PP60_DECODE(*(in+id), *(in+id+1));
	return inlen/2;
}

//data与NL829打包接口的data不同,指除命令(1byte)以外的其它数据.故可以为NULL,长度也可以为0
static int PP60_pack_data(uchar cmd, uchar *data, int datalen, uchar *frame)
{
	/*private & local definition*/
	int TailOffset = PP60_OFFSET_DATA+datalen;
	int framelen = TailOffset+PP60_LEN_TAIL;
	
	/*process body*/
#if 0
	if(datalen<0||datalen>PP60_MAXLEN_DATA)
		return FAIL;
#endif
	frame[PP60_OFFSET_HEAD] = ESC;
	frame[PP60_OFFSET_CMD] = cmd;
	if(data!=NULL&&datalen!=0)
		memcpy(frame+PP60_OFFSET_DATA, data, datalen);
	memcpy(frame+TailOffset, PP60_TAIL, PP60_LEN_TAIL);
	return framelen;
}

static ushort PP60_send_frame(uchar *cmdstr, int cmdlen)
{
	/*private & local definition*/
	
	/*process body*/
	if(g_PP60type==DEVICE_PP60PT)
	{
		serial_clrportbuf_all(g_devicecom);
		//外设的命令帧长度一般不会超过串口最大缓冲,故portwrite一般都会成功,可不判或简单判断返回
		return (NDK_PortWrite(g_devicecom, (uint)cmdlen, (char *)cmdstr)==NDK_OK)?NL829_PP60_SUCC:NL829_PP60_FAIL_SEND;
	}
	else if(g_PP60type==DEVICE_PP60RF)
		return NL829_pack_PP60cmd(cmdstr, cmdlen);
	else
		return NL829_PP60_NOTSUPPORT;
}

//PP60返回的数据有几种情况:0xaa, 0x55, 0x56, 0x02+长度+字符串, 不定长数据(如,版本信息,sn.当然,此情况下,格式也是不定的)
//但这些情况中的第一个数据一般是不同的
//形式上不统一且多数情况下数据较为简单,故不提供统一的解包函数,而仅提供接收函数
static ushort PP60PT_recv_frame(uchar *out, ushort *outlen)
{
	/*private & local definition*/
	ushort len = 0;
	uchar frame[PP60_MAXLEN_FRAME] = {0};
	int rlen = 0, ret = 0;
	
	/*process body*/
	//if(portread(g_devicecom, 1, (char *)frame, PP60_TIMEOUT_BASE+g_adjust)==1)
	if(NDK_PortRead(g_devicecom, 1, (char *)frame, (PP60_TIMEOUT_BASE+g_adjust)*1000, &rlen)==NDK_OK&&rlen==1)
	{
		if(frame[0]==0xaa||frame[0]==0x55||frame[0]==0x56)
		{
			*outlen = 1;
			*out = frame[0];
		}
		else if(frame[0]==PP60_CIPHERRSP_HEAD)
		{
			//if(portread(g_devicecom, 1, (char *)frame+1, /*0*/PP60_TIMEOUT_BASE+g_adjust)==1)
			if(NDK_PortRead(g_devicecom, 1, (char *)frame+1, /*0*/(PP60_TIMEOUT_BASE+g_adjust)*1000, &rlen)==NDK_OK&&rlen==1)
			{
				//if(portread(g_devicecom, frame[1], (char *)frame+2, /*0*/PP60_TIMEOUT_BASE+g_adjust)==frame[1])
				if(NDK_PortRead(g_devicecom, frame[1], (char *)frame+2, /*0*/(PP60_TIMEOUT_BASE+g_adjust)*1000, &rlen)==NDK_OK&&rlen==frame[1])
				{
					*outlen = frame[1]+2;
					memcpy(out, frame, *outlen);
				}
				else
					return PP60_ERR_FRAME1;
			}
			else
				return PP60_ERR_FRAME2;
		}
		else//获取不定长不定格式数据的情况
		{
			//if((len=portread(g_devicecom, PP60_MAXLEN_FRAME-1, (char *)frame+1, PP60_TIMEOUT_BASE+g_adjust))>0)//对于不定长的情况,读到多少算多少,返回给上层去判断
			if((ret=NDK_PortRead(g_devicecom, PP60_MAXLEN_FRAME-1, (char *)frame+1, (PP60_TIMEOUT_BASE+g_adjust)*1000, &rlen))==NDK_OK&&rlen>0)
			{
				len = rlen;
				*outlen = len+1;
				memcpy(out, frame, *outlen);
			}
			else if(ret==NDK_ERR_TIMEOUT)//else if(len==NDK_ERR_TIMEOUT)
			{
				*outlen = 1;
				*out = frame[0];
			}
			else
				return PP60_ERR_FRAME3;
		}
	}
	else	
		return NL829_PP60_FAIL_COMM;

	return NL829_PP60_SUCC;
}

//inlen为希望返回的数据长度,传入合适的值在调用到NL829_get_PP60rsp时会提高效率
//注意:本函数成败只说明链路接收的成败,具体返回数据代表的成败意义还需要进一步验证out及outlen(即解包的任务)!
static ushort PP60_recv_frame(ushort inlen, uchar *out, ushort *outlen)
{
	/*private & local definition*/
	
	/*process body*/
	if(g_PP60type==DEVICE_PP60PT)
		return PP60PT_recv_frame(out, outlen);
	else if(g_PP60type==DEVICE_PP60RF)
		return NL829_get_PP60rsp(/*10*/NL829_TIMEOUT_BASE+g_adjust, inlen, out, outlen);
	else
		return NL829_PP60_NOTSUPPORT;
}

//关于out与outlen是否符合预期,由上一层判断.即本函数返回成功时,并不保证out与outlen符合上一层的预期.与此类似的是NL829_unpack_data也不对append进行解析校验
static ushort PP60_frame_factory(uchar cmd, uchar *data, int datalen, ushort inlen, uchar *out, ushort *outlen)
{
	/*private & local definition*/
	int len = 0;
	ushort ret1 = NL829_FAIL;
	uchar frame_cmd[PP60_MAXLEN_FRAME] = {0};
	
	/*process body*/
	//组包
	len = PP60_pack_data(cmd, data, datalen, frame_cmd);
#if defined DEBUG_CMD//调试命令范例
	if(cmd==DEBUG_CMD||DEBUG_CMD==DEBUG_CMD_ALL)
	{
		int i = 0;
		cls_printf("line %d:", __LINE__);//__FILE__太长了,影响后续数据的显示,故此处不显示它
		for(; i<len; i++)
			NDK_ScrPrintf(" %02X", *(frame_cmd+i));
		NDK_ScrRefresh();
		lib_getch();
	}
#endif
	//发包
	if((ret1=PP60_send_frame(frame_cmd, len))!=NL829_PP60_SUCC)
		return ret1;//return PP60_ERR_FRAME_SND;
	//收包
	if((ret1=PP60_recv_frame(inlen, out, outlen))!=NL829_PP60_SUCC)
		return ret1;//返回NL829_ERR_FRAME_XXX(具体的接收错误)
       
#if defined DEBUG_CMD//调试命令范例
	if(cmd==DEBUG_CMD||DEBUG_CMD==DEBUG_CMD_ALL)
	{
		int i = 0;
		cls_printf("line %d:", __LINE__);//__FILE__太长了,影响后续数据的显示,故此处不显示它
		for(; i<*outlen; i++)
			NDK_ScrPrintf(" %02X", *(out+i));
		NDK_ScrRefresh();
		lib_getch();
	}
#endif
	//解包(out未有明确的数据域,不解包.关于out的意义较为复杂,由上一层解析正确与否)
	return NL829_PP60_SUCC;
}

//F实际上没用,可以随便填写
static ushort PP60_cmd_A(uchar M, uchar N, uchar F)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	int datalen = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = M;
	data[1] = N;
	data[2] = F;
	datalen = (F==PP60_NODESFLAG)?2:3;
	if((ret=PP60_frame_factory('A', data, datalen, PP60_CMDA_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDA_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

static ushort __PP60_cmd_Bb(uchar cmd, uchar bps)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = bps;
	if((ret=PP60_frame_factory(cmd, data, 1, PP60_CMDB_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDB_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;	
}

//ESC+B及ESC+b在PP60RF上不支持,机器会回复0x0001
static ushort PP60_cmd_B(uchar bps){return __PP60_cmd_Bb('B' ,bps);}

//static ushort PP60_cmd_b(uchar bps){return __PP60_cmd_Bb('b' ,bps);}

extern /*static*/ ushort PP60_cmd_C(void)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	if((ret=PP60_frame_factory('C', NULL, 0, PP60_CMDC_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDC_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if((out[0]==PP60_FAIL) || (out[0]==PP60_ERR)) //发送扫描动画指令时如果也发送此条指令会返回0xFF，故增加PP60_ERR状态  20170828 sull
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

//id传入数值即可,不必传入字符
extern /*static*/ ushort PP60_cmd_D(uchar id, uchar *str, int len)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	int datalen = 1+len;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = id+'0';
	memcpy(data+1, str, len);
	if((ret=PP60_frame_factory('D', data, datalen, PP60_CMDD_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDD_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if((out[0]==PP60_FAIL) || (out[0]==PP60_ERR)) //发送扫描动画指令时如果也发送此条指令会返回0xFF，故增加PP60_ERR状态  20170828 sull
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

static ushort PP60_cmd_E(uchar id)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = id+'0';
	if((ret=PP60_frame_factory('E', data, 1, PP60_CMDE_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDE_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

static ushort PP60_cmd_F(uchar id, uchar *out, ushort *outlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen1 = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out1[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = id+'0';
	if((ret=PP60_frame_factory('F', data, 1, PP60_CMDF_RCVLEN, out1, &outlen1))!=NL829_PP60_SUCC)
		return ret;
	if(outlen1==1&&out1[0]==PP60_FAIL)//预期长度是否与实际长度一致
		return NL829_PP60_FAIL;
	else if(outlen1% ((g_algtype==PP60_TDESFLAG)?PP60_CIPHERRSP_LEN:PP60_SM4_CIPHERRSP_LEN) == 2)
	{
		if(out1[0]!=PP60_CIPHERRSP_HEAD||out1[1]% ((g_algtype==PP60_TDESFLAG)?PP60_CIPHERRSP_LEN:PP60_SM4_CIPHERRSP_LEN) != 0)
			return PP60_ERR_CMD_BASE+1+out1[0];
		else//成功情况下,只送出密文数据即可,头与长度不用送(因为是确认的,可知的)
		{
			*outlen = out1[1];
			memcpy(out, out1+2, out1[1]);
			return NL829_PP60_SUCC;
		}
	}
	else
		return PP60_ERR_CMD_BASE;
}

//返回去掉02头与长度域以外的数据存于out,out的长度存在outlen
static ushort PP60_cmd_G(uchar *out, ushort *outlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen1 = 0;
	uchar out1[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	if((ret=PP60_frame_factory('G', NULL, 0, PP60_CMDG_RCVLEN, out1, &outlen1))!=NL829_PP60_SUCC)
		return ret;
	if(outlen1==1&&out1[0]==PP60_FAIL)//预期长度是否与实际长度一致
		return NL829_PP60_FAIL;
	else if(outlen1%2==0)
	{
		if(out1[0]!=PP60_CIPHERRSP_HEAD||out1[1]%2==1)//out1[1]% ((g_algtype==PP60_TDESFLAG)?PP60_CIPHERRSP_LEN:PP60_SM4_CIPHERRSP_LEN) !=0)
			return PP60_ERR_CMD_BASE+1+out1[0];
		else//成功情况下,只送出密文数据即可,头与长度不用送(因为是确认的,可知的)
		{
			*outlen = out1[1];
			memcpy(out, out1+2, out1[1]);
			return NL829_PP60_SUCC;
		}
	}
	else
		return PP60_ERR_CMD_BASE;
}

//注意:若要正确使用本函数,需要要之前先调用一下PP60_code
//str的原始数据不要求是字符串,而只要是数据串即可
static ushort PP60_cmd_H(uchar id, uchar *str, uchar inlen, uchar *out, ushort *outlen)
{
	/*private & local definition*/
	ushort ret = 0/*, expectlen = (id==0)?PP60_CMDH0_RCVLEN:PP60_MAXLEN_DATA PP60_CMDH1_RCVLEN*/, outlen1 = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out1[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = id+'0';
	data[1] = inlen;
	memcpy(data+2, str, inlen);
	if((ret=PP60_frame_factory('H', data, 2+inlen, (id==0)?PP60_CMDH0_RCVLEN:PP60_MAXLEN_DATA, out1, &outlen1))!=NL829_PP60_SUCC)
		return ret;
	//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
	if(id==1)
	{
		if(outlen1==1&&out1[0]==PP60_FAIL)//预期长度是否与实际长度一致
			return NL829_PP60_FAIL;
		else if(outlen1% ((g_algtype==PP60_TDESFLAG)?PP60_CIPHERRSP_LEN:PP60_SM4_CIPHERRSP_LEN) == 2)
		{
			if(out1[0]!=PP60_CIPHERRSP_HEAD||out1[1]% ((g_algtype==PP60_TDESFLAG)?PP60_CIPHERRSP_LEN:PP60_SM4_CIPHERRSP_LEN) != 0)
				return PP60_ERR_CMD_BASE+1+out1[0];
			else
			{
				*outlen = out1[1];
				memcpy(out, out1+2, out1[1]);
				return NL829_PP60_SUCC;
			}
		}
		else
			return PP60_ERR_CMD_BASE;
	}
	else //if(id==0)//id非0非1的情况下只判断out[0]
	{
		if(outlen1==PP60_CMDH0_RCVLEN)//预期长度是否与实际长度一致
		{
			if(out1[0]==PP60_SUCC)
				return NL829_PP60_SUCC;
			else if(out1[0]==PP60_FAIL)
				return NL829_PP60_FAIL;
			else
				return PP60_ERR_CMD_BASE+1+out1[0];
		}
		else
			return PP60_ERR_CMD_BASE;
	}
}

static ushort PP60_cmd_I(uchar id, uchar *cardno, int cardnolen, uchar *out, ushort *outlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen1 = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out1[PP60_MAXLEN_DATA] = {0};	
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = id+'0';
	memcpy(data+1, cardno, cardnolen);
	if((ret=PP60_frame_factory('I', data, 1+cardnolen, ((g_algtype==PP60_TDESFLAG)?PP60_CMDI_RCVLEN:PP60_CMDI_SM4_RCVLEN), out1, &outlen1))!=NL829_PP60_SUCC)
		return ret;
	if(outlen1==1&&out1[0]==PP60_FAIL)//预期长度是否与实际长度一致
		return NL829_PP60_FAIL;
	else if(outlen1== ((g_algtype==PP60_TDESFLAG)?PP60_CMDI_RCVLEN:PP60_CMDI_SM4_RCVLEN) )
	{
		if(out1[0]!=PP60_CIPHERRSP_HEAD||out1[1]!= ((g_algtype==PP60_TDESFLAG)?PP60_CIPHERRSP_LEN:PP60_SM4_CIPHERRSP_LEN) )
			return PP60_ERR_CMD_BASE+1+out1[0];
		else
		{
			*outlen = out1[1];
			memcpy(out, out1+2, out1[1]);
			return NL829_PP60_SUCC;
		}
	}
	else
		return PP60_ERR_CMD_BASE;
}

static ushort PP60_cmd_X(uchar id, uchar *cardno, int cardnolen, uchar *out, ushort *outlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen1 = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out1[PP60_MAXLEN_DATA] = {0};	
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = id+'0';
	memcpy(data+1, cardno, cardnolen);
	if((ret=PP60_frame_factory('X', data, 1+cardnolen, ((g_algtype==PP60_TDESFLAG)?PP60_CMDX_RCVLEN:PP60_CMDX_SM4_RCVLEN), out1, &outlen1))!=NL829_PP60_SUCC)
		return ret;
	if(outlen1==1)//预期长度是否与实际长度一致
	{
		out[0] = out1[0];
		*outlen = outlen1;
		if(out1[0]==0x56)
			return NL829_PP60_SUCC;
		else// if(out1[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
	}
	else if(outlen1== ((g_algtype==PP60_TDESFLAG)?PP60_CMDX_RCVLEN:PP60_CMDX_SM4_RCVLEN) )
	{
		if(out1[0]!=PP60_CIPHERRSP_HEAD||out1[1]!= ((g_algtype==PP60_TDESFLAG)?PP60_CIPHERRSP_LEN:PP60_SM4_CIPHERRSP_LEN) )
			return PP60_ERR_CMD_BASE+1+out1[0];
		else
		{
			*outlen = out1[1];
			memcpy(out, out1+2, out1[1]);
			return NL829_PP60_SUCC;
		}
	}
	else
		return PP60_ERR_CMD_BASE;
}

static ushort PP60_cmd_J(uchar id, uchar *cardno, int cardnolen, uchar *STR, int STRlen, uchar *out, ushort *outlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen1 = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out1[PP60_MAXLEN_DATA] = {0};	
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = id+'0';
	memcpy(data+1, cardno, cardnolen);
	memcpy(data+1+cardnolen, STR, STRlen);
	if((ret=PP60_frame_factory('J', data, 1+cardnolen+STRlen, ((g_algtype==PP60_TDESFLAG)?PP60_CMDJ_RCVLEN:PP60_CMDJ_SM4_RCVLEN), out1, &outlen1))!=NL829_PP60_SUCC)
		return ret;
	if(outlen1==1&&out1[0]==PP60_FAIL)//预期长度是否与实际长度一致
		return NL829_PP60_FAIL;
	else if(outlen1== ((g_algtype==PP60_TDESFLAG)?PP60_CMDJ_RCVLEN:PP60_CMDJ_SM4_RCVLEN) )
	{
		if(out1[0]!=PP60_CIPHERRSP_HEAD||out1[1]!= ((g_algtype==PP60_TDESFLAG)?PP60_CIPHERRSP_LEN:PP60_SM4_CIPHERRSP_LEN) )
			return PP60_ERR_CMD_BASE+1+out1[0];
		else
		{
			*outlen = out1[1];
			memcpy(out, out1+2, out1[1]);
			return NL829_PP60_SUCC;
		}
	}
	else
		return PP60_ERR_CMD_BASE;
}

//注意:若要正确使用本函数,需要要之前先调用一下PP60_code
static ushort PP60_cmd_K(uchar M, uchar *STR, int STRlen, uchar *out, ushort *outlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen1 = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out1[PP60_MAXLEN_DATA] = {0};	
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = M;
	memcpy(data+1, STR, STRlen);
	if((ret=PP60_frame_factory('K', data, 1+STRlen, ((g_algtype==PP60_TDESFLAG)?PP60_CMDK_RCVLEN:PP60_CMDK_SM4_RCVLEN), out1, &outlen1))!=NL829_PP60_SUCC)
		return ret;
	if(outlen1==1&&out1[0]==PP60_FAIL)//预期长度是否与实际长度一致
		return NL829_PP60_FAIL;
	else if(outlen1== ((g_algtype==PP60_TDESFLAG)?PP60_CMDK_RCVLEN:PP60_CMDK_SM4_RCVLEN) )
	{
		if(out1[0]!=PP60_CIPHERRSP_HEAD||out1[1]!= ((g_algtype==PP60_TDESFLAG)?PP60_CIPHERRSP_LEN:PP60_SM4_CIPHERRSP_LEN) )
			return PP60_ERR_CMD_BASE+1+out1[0];
		else
		{
			*outlen = out1[1];
			memcpy(out, out1+2, out1[1]);
			return NL829_PP60_SUCC;
		}
	}
	else
		return PP60_ERR_CMD_BASE;
}

extern /*static*/ ushort PP60_cmd_L(uchar maxPINlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = maxPINlen;
	if((ret=PP60_frame_factory('L', data, 1, PP60_CMDL_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDL_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

//注意:若要正确使用本函数,需要要之前先分别对OK与NK调用一下PP60_code
static ushort PP60_cmd_M(uchar P, uchar *OK, int OKlen, uchar *NK, int NKlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = P;
	memcpy(data+1, OK, OKlen);
	memcpy(data+1+OKlen, NK, NKlen);
	if((ret=PP60_frame_factory('M', data, 1+OKlen+NKlen, PP60_CMDM_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDM_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

extern /*static*/ ushort PP60_cmd_N(uchar minPINlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = minPINlen;
	if((ret=PP60_frame_factory('N', data, 1, PP60_CMDN_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDN_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

static ushort PP60_cmd_O(uchar *out, ushort *outlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen1 = 0;
	uchar out1[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	if((ret=PP60_frame_factory('O', NULL, 0, PP60_MAXLEN_DATA, out1, &outlen1))!=NL829_PP60_SUCC)
		return ret;
	if(outlen1==1&&out1[0]==PP60_FAIL)//预期长度是否与实际长度一致
		return NL829_PP60_FAIL;
	else if(outlen1==out1[1]+2)
	{
		if(out1[0]!=PP60_CIPHERRSP_HEAD)
			return PP60_ERR_CMD_BASE+1+out1[0];
		else
		{
			*outlen = out1[1];
			memcpy(out, out1+2, out1[1]);
			return NL829_PP60_SUCC;
		}
	}
	else
		return PP60_ERR_CMD_BASE;
}

static ushort PP60_cmd_P(uchar *STR, int STRlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	memcpy(data, STR, STRlen);
	if((ret=PP60_frame_factory('P', data, STRlen, PP60_CMDP_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDP_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

//该命令目前不支持,实现它可用于异常测试(不识别命令的测试)
static ushort PP60_cmd_Q(void)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
#if 0
	return PP60_frame_factory('Q', NULL, 0, PP60_MAXLEN_DATA, out, &outlen);
#else
	if((ret=PP60_frame_factory('Q', NULL, 0, PP60_CMDQ_RCVLEN/*PP60_MAXLEN_DATA*/, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDQ_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
#endif
}

//注意:若要正确使用本函数,需要要之前先调用一下PP60_code
static ushort PP60_cmd_R(uchar *XX, int XXlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	memcpy(data, XX, XXlen);
	if((ret=PP60_frame_factory('R', data, XXlen, PP60_CMDR_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDR_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

//注意:若要正确使用本函数,需要要之前先调用一下PP60_code
static ushort PP60_cmd_S(uchar M, uchar N, uchar *STR, int STRlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = M;
	data[1] = N;
	data[2] = STRlen;
	memcpy(data+3, STR, STRlen);
	if((ret=PP60_frame_factory('S', data, 3+STRlen, PP60_CMDS_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDS_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

//注意:若要正确使用本函数,需要要之前先调用一下PP60_code
static ushort PP60_cmd_T(uchar XX, uchar *STR, int STRlen, uchar *out, ushort *outlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen1 = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out1[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = XX;
	memcpy(data+1, STR, STRlen);
	if((ret=PP60_frame_factory('T', data, 1+STRlen, ((g_algtype==PP60_TDESFLAG)?PP60_CMDT_RCVLEN:PP60_CMDT_SM4_RCVLEN), out1, &outlen1))!=NL829_PP60_SUCC)
		return ret;
	if(outlen1==1&&out1[0]==PP60_FAIL)//预期长度是否与实际长度一致
		return NL829_PP60_FAIL;
	else if(outlen1== ((g_algtype==PP60_TDESFLAG)?PP60_CMDT_RCVLEN:PP60_CMDT_SM4_RCVLEN) )
	{
		if(out1[0]!=PP60_CIPHERRSP_HEAD||out1[1]!= ((g_algtype==PP60_TDESFLAG)?PP60_CIPHERRSP_LEN:PP60_SM4_CIPHERRSP_LEN) )
			return PP60_ERR_CMD_BASE+1+out1[0];
		else
		{
			*outlen = out1[1];
			memcpy(out, out1+2, out1[1]);
			return NL829_PP60_SUCC;
		}
	}
	else
		return PP60_ERR_CMD_BASE;
}

//注意:若要正确使用本函数,需要要之前先调用一下PP60_code
static ushort PP60_cmd_U(uchar M, uchar N, uchar *MK, int MKlen, uchar *UK, int UKlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = M;
	data[1] = N;
	memcpy(data+2, MK, MKlen);
	memcpy(data+2+MKlen, UK, UKlen);
	if((ret=PP60_frame_factory('U', data, 2+MKlen+UKlen, PP60_CMDU_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDU_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

static ushort PP60_cmd_V(uchar *out, ushort *outlen){return PP60_frame_factory('V', NULL, 0, PP60_MAXLEN_DATA, out, outlen);}

//注意:若要正确使用本函数,需要要之前先调用一下PP60_code
//即DD是编码后的数据.code不由本函数执行是为了可以测试到DDlen为奇数的情况
static ushort PP60_cmd_W(uchar XX, uchar YY, uchar *DD, int DDlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = XX;
	data[1] = YY;
	memcpy(data+2, DD, DDlen);
	if((ret=PP60_frame_factory('W', data, 2+DDlen, PP60_CMDW_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDW_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

static ushort PP60_cmd_Z(void)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	if((ret=PP60_frame_factory('Z', NULL, 0, PP60_CMDZ_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDZ_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

//注意:若要正确使用本函数,需要要之前先调用一下PP60_code
//调用说明:id==1时,不关心后两个参数;id!=1时,后两个参数要由应用预定义.为2时,前两个参数不关心
static ushort PP60_cmd_x(uchar id, uchar *in, int inlen, uchar *out, ushort *outlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen1 = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out1[PP60_MAXLEN_DATA] = {0};
	int outlen2 = 0;
	
	/*process body*/
	if(id!=1&&(out==NULL||outlen==NULL))
		return PP60_ERR_CMD_BASE;
	
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = id+'0';
	memcpy(data+1, in, inlen);
	if((ret=PP60_frame_factory('x', data, 1+inlen, (id==1)?PP60_CMDx1_RCVLEN:PP60_MAXLEN_DATA, out1, &outlen1))!=NL829_PP60_SUCC)
		return ret;
	if(id==1)
	{
		if(outlen1==PP60_CMDx1_RCVLEN)//预期长度是否与实际长度一致
		{	
			//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
			if(out1[0]==PP60_SUCC)
				return NL829_PP60_SUCC;
			else if(out1[0]==PP60_FAIL)
				return NL829_PP60_FAIL;
			else
				return PP60_ERR_CMD_BASE+1+out1[0];//加1以区分一旦out[0]为0x00的情况
		}
		else
			return PP60_ERR_CMD_BASE;
	}
	else //if(id==2)//id为非1非2的异常命令
	{
		if(outlen1==1&&out1[0]==PP60_FAIL)//预期长度是否与实际长度一致
			return NL829_PP60_FAIL;
		else if(outlen1==out1[1]+2)
		{
			if(out1[0]!=PP60_CIPHERRSP_HEAD||out1[1]%2!=0)
				return PP60_ERR_CMD_BASE+1+out1[0];
			else
			{
				if((outlen2=PP60_decode(out1+2, outlen1-2, out))==FAIL)
					return PP60_ERR_DECODE;
				*outlen = outlen2;
				return NL829_PP60_SUCC;
			}
		}
		else
			return PP60_ERR_CMD_BASE;
	}
}

static ushort PP60_cmd_h(uchar yy,uchar *in, int inlen)
{
	/*private & local definition*/
	ushort ret = 0, outlen = 0;
	uchar data[PP60_MAXLEN_DATA] = {0}, out[PP60_MAXLEN_DATA] = {0};
	
	/*process body*/
	//data域指ESC+cmd与0D 0A之间的数据
	data[0] = yy;
	memcpy(data+1, in, inlen);
	if((ret=PP60_frame_factory('h', data, 1+inlen, PP60_CMDh_RCVLEN, out, &outlen))!=NL829_PP60_SUCC)
		return ret;
	if(outlen==PP60_CMDh_RCVLEN)//预期长度是否与实际长度一致
	{	
		//由于PP60返回值多是1byte的,而829是2bytes的,为了统一处理,对PP60返回进行转义
		if(out[0]==PP60_SUCC)
			return NL829_PP60_SUCC;
		else if(out[0]==PP60_FAIL)
			return NL829_PP60_FAIL;
		else
			return PP60_ERR_CMD_BASE+1+out[0];//加1以区分一旦out[0]为0x00的情况
	}
	else
		return PP60_ERR_CMD_BASE;
}

//id只能送入1,2
static ushort __test_PP60_cmd_D(uchar id)
{
	/*private & local definition*/
	ushort staval = NL829_PP60_FAIL, ret = NL829_PP60_FAIL;
	
	/*process body*/
	PP60_cmd_C();//测试前置
	if((staval=PP60_cmd_D(id, NULL, 0))!=NL829_PP60_SUCC||cls_show_msg("第%d行若有字符显示,按[ENTER];否则按[其它]", id)==ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示	
	if((staval=PP60_cmd_D(id, (uchar *)"\xf0\xf1\xf2", strlen("\xf0\xf1\xf2")))!=NL829_PP60_FAIL||cls_show_msg("第%d行若有字符显示,按[ENTER];否则按[其它]", id)==ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示	
	if((staval=PP60_cmd_D(id, (uchar *)"\x31\xf0\xf1\xf2", strlen("\x31\xf0\xf1\xf2")))!=NL829_PP60_FAIL||cls_show_msg("第%d行若有字符显示,按[ENTER];否则按[其它]", id)==ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=PP60_cmd_D(id, (uchar *)"\x80\x81\x82\x83\x84\x31\x32\x33\x34", strlen("\x80\x81\x82\x83\x84\x31\x32\x33\x34")))!=NL829_PP60_SUCC||cls_show_msg("第%d行若显示\"请输入密码1234\",按[ENTER];否则按[其它]", id)!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=PP60_cmd_D(id, (uchar *)"\x80\x81\x82\xBB\xBC\x31\x32\x33\x34", strlen("\x80\x81\x82\xBB\xBC\x31\x32\x33\x34")))!=NL829_PP60_SUCC||cls_show_msg("第%d行若显示\"请输入预约1234\",按[ENTER];否则按[其它]", id)!=ENTER)//字库新增字体
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	if((staval=PP60_cmd_D(id, (uchar *)"\x80\x81\x82\x83\x84\x80\x81\x82\x83\x84", strlen("\x80\x81\x82\x83\x84\x80\x81\x82\x83\x84")))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	
	ret = NL829_PP60_SUCC;
ERR:
	PP60_cmd_C();//测试后置
	return ret;
}

static ushort test_PP60_cmd_D(void)
{
	/*private & local definition*/
	ushort staval = NL829_PP60_FAIL;
	int i = 1, n = 8;
	
	/*process body*/
	//cls_printf("测试D命令中,请耐心等待...");
	n=(g_LCDSize==1)?8:3;
	for(i=1;i<n;i++)
	{
		if((staval=__test_PP60_cmd_D(i))!=NL829_PP60_SUCC)
			return NL829_PP60_FAIL;
	}
	if((staval=PP60_cmd_D(n, (uchar *)"\x80\x81\x82\x83\x84\x31\x32\x33\x34", strlen("\x80\x81\x82\x83\x84\x31\x32\x33\x34")))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	return NL829_PP60_SUCC;
}

static ushort test_PP60_cmd_O(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0};
	ushort staval = NL829_PP60_FAIL, outlen = 0;
	
	/*process body*/
	cls_printf("请在PED上第2行输入%s及[ENTER],请耐心等待...", "1234");//这里用cls_show_msg会引起操作误会,使用cls_printf即可
	memset(out, 0, sizeof(out));//从不用根据outlen来截短
	if((staval=PP60_cmd_O(out, &outlen))!=NL829_PP60_SUCC||memcmp(out, "1234", outlen))
	{
		//DEBUG_HEX(out, outlen, NULL);//返回02 04 31 32 33 34
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	cls_printf("请在PED上先按[ENTER]多下,再按[ESC]...");
	
	if(g_LCDSize==1)
		memset(out, WHITE, sizeof(out));//彩屏设备0为黑色,所以白屏要修改成WHITE
	else
		memset(out, 0, sizeof(out));//从不用根据outlen来截短
	if((staval=PP60_cmd_O(out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	return NL829_PP60_SUCC;
}

static ushort test_PP60_cmd_P(void)
{
	/*private & local definition*/
	ushort staval = NL829_PP60_FAIL;
	
	/*process body*/
	//异常
	if((staval=PP60_cmd_P((uchar *)"\x90\x91\x8c\x8d\xa8\xa9\xaa\xac\xb5", strlen("\x90\x91\x8c\x8d\xa8\xa9\xaa\xac\xb5")))!=NL829_PP60_FAIL)//串为"深圳银联小灵通话费"
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}	
	if((staval=PP60_cmd_P((uchar *)"\x90\xf1\xfc\xfd", strlen("\x90\xf1\xfc\xfd")))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}

	cls_printf("请在PED上按[ENTER]...");
	if((staval=PP60_cmd_P(NULL, 0))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	cls_printf("请在PED上按[ESC]...");
	if((staval=PP60_cmd_P(NULL, 0))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}

	//正常
	cls_printf("PED第2行显示\"深圳银联\",请在PED上按[ENTER]...");
	if((staval=PP60_cmd_P((uchar *)"\x90\x91\x8c\x8d", strlen("\x90\x91\x8c\x8d")))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	cls_printf("PED第2行显示\"深圳银联\",请在PED上按[ESC]...");
	if((staval=PP60_cmd_P((uchar *)"\x90\x91\x8c\x8d", strlen("\x90\x91\x8c\x8d")))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}

	cls_printf("PED第2行显示\"深圳银联小灵通\",请在PED上按[ENTER]...");
	if((staval=PP60_cmd_P((uchar *)"\x90\x91\x8c\x8d\xa8\xa9\xaa", strlen("\x90\x91\x8c\x8d\xa8\xa9\xaa")))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);//????
		return NL829_PP60_FAIL;
	}
	cls_printf("PED第2行显示\"深圳银联小灵通\",请在PED上按[ESC]...");
	if((staval=PP60_cmd_P((uchar *)"\x90\x91\x8c\x8d\xa8\xa9\xaa", strlen("\x90\x91\x8c\x8d\xa8\xa9\xaa")))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}	

	return NL829_PP60_SUCC;
}

//测试思路:先调用W命令来下载LOGO,再调用Z命令来显示效果以验证W命令
//根据W的实现,是一旦下载成功,屏幕就会进行LOGO显示,故封掉Z命令及cls_show_msg进行确认的步骤
static ushort test_PP60_cmd_W(void)
{
	/*private & local definition*/
	//中国银联//244bytes原始数据
	uchar logo[] = {0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0X08,0X08,0X08,0X08,0XFF,0X08,
				0X08,0X08,0X08,0X0F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7F,
				0X40,0X48,0X49,0X49,0X49,0X4F,0X49,0X49,0X49,0X48,0X40,0X7F,0X00,0X00,0X00,0X00,0X00,
				0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X0C,0X34,0XE7,
				0X24,0X24,0X00,0X7F,0X49,0X49,0X49,0X49,0X7F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
				0X00,0X00,0X00,0X40,0X40,0X7F,0X48,0X48,0X7F,0X40,0X08,0X88,0X68,0X0F,0X08,0X28,0XC8,
				0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
				0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XE0,0X40,0X40,0X40,
				0X40,0XFF,0X40,0X40,0X40,0X40,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
				0X00,0X00,0XFF,0X02,0X12,0X12,0X12,0X12,0XF2,0X12,0X52,0X32,0X12,0X02,0XFF,0X00,0X00,
				0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,
				0X80,0X80,0XFE,0X84,0X88,0X80,0XFF,0X02,0XC4,0X30,0X18,0X24,0X46,0X04,0X00,0X00,0X00,
				0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X06,0XFC,0X88,0X88,0XFF,0X09,0X82,0X8C,0XB0,0XC0,
				0XB0,0X8C,0X83,0X82,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
				0X00,0X00,0X00,0X00,0X00,0X00,0XFF};//最后一个0XFF是错误的,正常使用时要把它去掉
	uchar out[PP60_MAXLEN_DATA] = {0}, tmp[244] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, ret = NL829_PP60_FAIL;
	int outlen = 0;
	
	/*process body*/
	//测试前置
  	/*if(g_LCDSize==1)//20170401朱琼反馈开发已经修改SP10和SP10-T的白色统一为0
		memset(tmp, WHITE, sizeof(tmp));
	else*/
		memset(tmp, 0, sizeof(tmp));
	outlen = PP60_code(tmp, sizeof(tmp), out);
#if TEST
	if((staval=PP60_cmd_W(0, 0, out, outlen))!=NL829_PP60_SUCC||(staval1=PP60_cmd_W(0, 1, out, outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	//cls_show_msg("按任意键PP60白屏");
	if(/*(staval=PP60_cmd_Z())!=NL829_PP60_SUCC||*/cls_show_msg("1.若PP60白屏按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
#endif
	
	outlen = PP60_code(logo, sizeof(logo), out);
#if 0	//定位连发会失败的BUG用
	if((staval=PP60_cmd_W(0, 0, out, (outlen-2)/2))!=NL829_PP60_SUCC)//只有上半行数据,但设备仍会平均分成上下半行数据
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//cls_show_msg("按任意键继续...");
	if((staval=PP60_cmd_W(0, 1, out, (outlen-2)/2))!=NL829_PP60_SUCC)//只有上半行数据,但设备仍会平均分成上下半行数据
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg("按任意键发送第1条命令\n");
	show_msg("ret:%04x", PP60_cmd_W(0, 0, out, (outlen-2)/2));
	cls_show_msg("按任意键发送第2条命令\n");
	show_msg("ret:%04x", PP60_cmd_W(0, 1, out, (outlen-2)/2));
	while(1);
#endif
	if((staval=PP60_cmd_W(122, 0, out, outlen-2))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//cls_show_msg("按任意键PP60白屏");
	if(/*(staval=PP60_cmd_Z())!=NL829_PP60_SUCC||*/cls_show_msg("2.若PP60白屏按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=PP60_cmd_W(0, 2, out, outlen-2))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//cls_show_msg("按任意键PP60白屏");
	if(/*(staval=PP60_cmd_Z())!=NL829_PP60_SUCC||*/cls_show_msg("3.若PP60白屏按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
#if TEST
	if((staval=PP60_cmd_W(0, 0, NULL, 0))!=NL829_PP60_FAIL||(staval1=PP60_cmd_W(0, 1, NULL, 0))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
#endif
	if((staval=PP60_cmd_W(0, 0, out, outlen))!=NL829_PP60_FAIL||(staval1=PP60_cmd_W(0, 1, out, outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	//cls_show_msg("按任意键PP60白屏");
	if(/*(staval=PP60_cmd_Z())!=NL829_PP60_SUCC||*/cls_show_msg("4.若PP60白屏按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=PP60_cmd_W(0, 0, out, (outlen-2)/2+1))!=NL829_PP60_FAIL||(staval1=PP60_cmd_W(0, 1, out, (outlen-2)/2+1))!=NL829_PP60_FAIL)//数据量为奇数,无法分成上下半行就报错
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	//cls_show_msg("按任意键PP60白屏");
	if(/*(staval=PP60_cmd_Z())!=NL829_PP60_SUCC||*/cls_show_msg("5.若PP60白屏按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

#if TEST	
	//上行与下行的权限测试
	if((staval=PP60_cmd_W(0, 1, out, (outlen-2)/2))!=NL829_PP60_FAIL||(staval1=PP60_cmd_W(0, 0, out, (outlen-2)/2))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	if((staval=PP60_cmd_W(0, 0, out, (outlen-2)/2))!=NL829_PP60_SUCC||(staval1=PP60_cmd_W(0, 1, out, (outlen-2)/2))!=NL829_PP60_SUCC)//只有上半行数据,但设备仍会平均分成上下半行数据
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	if((staval1=PP60_cmd_W(0, 1, out, (outlen-2)/2))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval1);
		goto ERR;
	}
	//cls_show_msg("按任意键PP60花屏");
	if(/*(staval=PP60_cmd_Z())!=NL829_PP60_SUCC||*/cls_show_msg("6.若PP60花屏按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//end of 上行与下行的权限测试
	//cls_show_msg("请连PC串口...");//用于调试时从串口获得命令数据
	if((staval=PP60_cmd_W(60, 0, out, outlen-2))!=NL829_PP60_SUCC||(staval1=PP60_cmd_W(60, 1, out, outlen-2))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	//cls_show_msg("按任意键PP60显示右半屏");
	if(/*(staval=PP60_cmd_Z())!=NL829_PP60_SUCC||*/cls_show_msg("若PP60显示右半屏按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
#endif
	if((staval=PP60_cmd_W(0, 0, out, outlen-2))!=NL829_PP60_SUCC||(staval1=PP60_cmd_W(0, 1, out, outlen-2))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	//cls_show_msg("按任意键PP60上下行均显示\"中国银联\"");
	if(/*(staval=PP60_cmd_Z())!=NL829_PP60_SUCC||*/cls_show_msg("若PP60上下行均显示\"中国银联\"按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	ret = NL829_PP60_SUCC;
ERR:
	//测试后置
	outlen = PP60_code(tmp, sizeof(tmp), out);	
	PP60_cmd_W(0, 0, out, outlen);
	PP60_cmd_Z();
	return ret;
}

static ushort test_PP60_cmd_x(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, outlen = 0;
	
	/*process body*/
	if((staval=PP60_cmd_x(0, NULL, 0, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
#if TEST
	if((staval=PP60_cmd_x(2, (uchar *)"\x41\x42", strlen("\x41\x42"), out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
#endif

	memset(out, 0, sizeof(out));
	if((staval=PP60_cmd_x(2, NULL, 0, out, &outlen))==NL829_PP60_FAIL)//未下载过sn
	{
		if((staval=PP60_cmd_x(1, NULL, 0, NULL, NULL))!=NL829_PP60_FAIL)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			return NL829_PP60_FAIL;
		}
		if((staval=PP60_cmd_x(1, (uchar *)"\x41", strlen("\x41"), NULL, NULL))!=NL829_PP60_FAIL||(staval1=PP60_cmd_x(1, (uchar *)"\x41\x42\x43", strlen("\x41\x42\x43"), NULL, NULL))!=NL829_PP60_FAIL)
		{
			cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
			return NL829_PP60_FAIL;
		}
		outlen = PP60_code((uchar *)"abcdefghijklmnopqrstuvwxy", strlen("abcdefghijklmnopqrstuvwxy"), out);
		if((staval=PP60_cmd_x(1, out, outlen, NULL, NULL))!=NL829_PP60_FAIL)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			return NL829_PP60_FAIL;
		}
		cls_show_msg1(2, "未下载过SN,重新下载SN中...");
		outlen = PP60_code((uchar *)PP60_SN, strlen(PP60_SN), out);
		if((staval=PP60_cmd_x(1, out, outlen, NULL, NULL))!=NL829_PP60_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			return NL829_PP60_FAIL;
		}
		if((staval=PP60_cmd_x(1, out, outlen, NULL, NULL))!=NL829_PP60_FAIL)//已下成功了,再下一次应失败
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			return NL829_PP60_FAIL;
		}
		memset(out, 0, sizeof(out));
		if((staval=PP60_cmd_x(2, NULL, 0, out, &outlen))!=NL829_PP60_SUCC||outlen!=strlen(PP60_SN))//再获取出来校验
		{
			cls_show_msg("line %d:测试失败(%04x,%d)", __LINE__, staval, outlen);
			return NL829_PP60_FAIL;
		}
		else
		{
			if(memcmp(out, PP60_SN, outlen))
			{
				cls_show_msg("line %d:测试失败", __LINE__);
				return NL829_PP60_FAIL;
			}
		}
	}
	else if(outlen==0)
	{
		cls_show_msg("line %d:测试失败", __LINE__);
		return NL829_PP60_FAIL;	
	}

	//已下载过sn
	if(cls_show_msg("获取到SN:%s,正确按[ENTER],否则按[其它]", out)!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}

	if((staval=PP60_cmd_x(2, (uchar *)"\x41\x42", strlen("\x41\x42"), out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	
	return NL829_PP60_SUCC;
}

static ushort test_PP60_cmd_h(void)
{
	/*private & local definition*/
 	uchar HZtest[] = {
 						0x08, 0xc8, 0x38, 0xcf, 0x08, 0x28, 0x90, 0xa8, 
						0x08, 0x19, 0x09, 0xff, 0x05, 0x04, 0xff, 0x4a, 
						0xa4, 0xa3, 0x24, 0x28, 0x10, 0xb0, 0x10, 0x00, 
						0x8a, 0x7f, 0x00, 0x5f, 0x80, 0x7f, 0x00, 0x00,
						0x08, 0xc8, 0x38, 0xcf, 0x08, 0x28, 0x90, 0xa8, 
						0x08, 0x19, 0x09, 0xff, 0x05, 0x04, 0xff, 0x4a, 
						0xa4, 0xa3, 0x24, 0x28, 0x10, 0xb0, 0x10, 0x00, 
						0x8a, 0x7f, 0x00, 0x5f, 0x80, 0x7f, 0x00, 0x00,
						0x08, 0xc8, 0x38, 0xcf, 0x08, 0x28, 0x90, 0xa8, 
						0x08, 0x19, 0x09, 0xff, 0x05, 0x04, 0xff, 0x4a, 
						0xa4, 0xa3, 0x24, 0x28, 0x10, 0xb0, 0x10, 0x00, 
						0x8a, 0x7f, 0x00, 0x5f, 0x80, 0x7f, 0x00, 0x00,
						0x08, 0xc8, 0x38, 0xcf, 0x08, 0x28, 0x90, 0xa8, 
						0x08, 0x19, 0x09, 0xff, 0x05, 0x04, 0xff, 0x4a, 
						0xa4, 0xa3, 0x24, 0x28, 0x10, 0xb0, 0x10, 0x00, 
						0x8a, 0x7f, 0x00, 0x5f, 0x80, 0x7f, 0x00, 0x00,
						0x08, 0xc8, 0x38, 0xcf, 0x08, 0x28, 0x90, 0xa8, 
						0x08, 0x19, 0x09, 0xff, 0x05, 0x04, 0xff, 0x4a, 
						0xa4, 0xa3, 0x24, 0x28, 0x10, 0xb0, 0x10, 0x00, 
						0x8a, 0x7f, 0x00, 0x5f, 0x80, 0x7f, 0x00, 0x00,
						0x08, 0xc8, 0x38, 0xcf, 0x08, 0x28, 0x90, 0xa8, 
						0x08, 0x19, 0x09, 0xff, 0x05, 0x04, 0xff, 0x4a, 
						0xa4, 0xa3, 0x24, 0x28, 0x10, 0xb0, 0x10, 0x00, 
						0x8a, 0x7f, 0x00, 0x5f, 0x80, 0x7f, 0x00, 0x00,
						0x08, 0xc8, 0x38, 0xcf, 0x08, 0x28, 0x90, 0xa8, 
						0x08, 0x19, 0x09, 0xff, 0x05, 0x04, 0xff, 0x4a, 
						0xa4, 0xa3, 0x24, 0x28, 0x10, 0xb0, 0x10, 0x00, 
						0x8a, 0x7f, 0x00, 0x5f, 0x80, 0x7f, 0x00, 0x00,
						0x08, 0xc8, 0x38, 0xcf, 0x08, 0x28, 0x90, 0xa8, 
						0x08, 0x19, 0x09, 0xff, 0x05, 0x04, 0xff, 0x4a, 
						0xa4, 0xa3, 0x24, 0x28, 0x10, 0xb0, 0x10, 0x00, 
						0x8a, 0x7f, 0x00, 0x5f, 0x80, 0x7f, 0x00, 0x00
					 };
	uchar out[PP60_MAXLEN_DATA] = {0}, tmp[244] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, ret = NL829_PP60_FAIL;
	int i = 0, outlen = 480, cnt1 = 0, cnt2 = 0, n = 8;

	/*process body*/
	//测试前置
	n=(g_LCDSize==1)?8:2;
	for (i = 0; i<n; i++)
	{
		if((staval=PP60_cmd_h(i, NULL, 0))!=NL829_PP60_SUCC)
		{
			cls_show_msg("line %d:测试失败(%d,%04x)", __LINE__, i, staval);
			goto ERR;
		}
	}
	if(cls_show_msg("7.若PP60白屏按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	//错误参数
	memset(out, 0x31, sizeof(out));
	if((staval=PP60_cmd_h(0, out, outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}	
	PP60_code(HZtest, sizeof(HZtest), out);
	if((staval=PP60_cmd_h(n, out, outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=PP60_cmd_h(0, out, 1+outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}	
	if((staval=PP60_cmd_h(0, out, 2+outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	for (i = 0; i<n; i++)
	{
		if((staval=PP60_cmd_h(i, out, cnt1=((rand()%outlen)|1)))!=NL829_PP60_FAIL)
		{
			cls_show_msg("line %d:测试失败(%d,%04x)", __LINE__, i, staval);
			goto ERR;
		}	
	}
	if(cls_show_msg("8.若PP60白屏按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//正常使用
	for (i = 0; i<n; i++)
	{
		if((staval=PP60_cmd_h(i, out, outlen))!=NL829_PP60_SUCC)
		{
			cls_show_msg("line %d:测试失败(%d,%04x)", __LINE__, i, staval);
			goto ERR;
		}	
	}
	if(cls_show_msg("若PP60写满%d行输字[ENTER],否则按[其它]",(g_LCDSize==1)?(n-1):n)!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	for (i = 0; i<n; i++)
	{
		if((staval=PP60_cmd_h(i, NULL, 0))!=NL829_PP60_SUCC)
		{
			cls_show_msg("line %d:测试失败(%d,%04x)", __LINE__, i, staval);
			goto ERR;
		}	
	}
	if(cls_show_msg("9.若PP60白屏按[ENTER],否则按[其它]")!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	memset(tmp, 0x00, sizeof(tmp));
	for (i = 0; i<n; i++)
	{
		cnt2 = (rand()%6+1)*ONEHZSIZE;
		PP60_code(HZtest, sizeof(HZtest), out);
		if((staval=PP60_cmd_h(i, out, cnt2))!=NL829_PP60_SUCC)
		{
			cls_show_msg("line %d:测试失败(%04x)%d,%04x", __LINE__, staval, i, cnt2);
			goto ERR;
		}
		if(cls_show_msg("若PP60(第%d行)有%d个汉字,按[ENTER],否则按[其它]", (i==0)?2:i, cnt2/ONEHZSIZE)!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
			goto ERR;
		}
		PP60_code(tmp, sizeof(tmp), out);
		if((staval=PP60_cmd_h(i, out, outlen))!=NL829_PP60_SUCC)
		{
			cls_show_msg("line %d:测试失败(%d)%04x", __LINE__, i, staval);
			goto ERR;
		}
	}
	ret = NL829_PP60_SUCC;
ERR:
	//测试后置
	PP60_cmd_Z();
	return ret;
}

static ushort test_PP60_cmd_R(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0};	
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, outlen = 0;
	
	/*process body*/
	cls_printf("复位密钥区中,请耐心等待...");
	if((staval=PP60_cmd_R((uchar *)"\x41", strlen("\x41")))!=NL829_PP60_FAIL||(staval1=PP60_cmd_R((uchar *)"\x41\x41\x41", strlen("\x41\x41\x41")))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
	outlen = PP60_code((uchar *)"\xa8", 1, out);//168
	if((staval=PP60_cmd_R(out, outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	//重置第0,167组主,用密钥
	if((staval=PP60_cmd_R((uchar *)"\x41\x41", strlen("\x41\x41")))!=NL829_PP60_SUCC
	||(staval1=PP60_cmd_R((uchar *)"\x4b\x48", strlen("\x4b\x48")))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
	
	return NL829_PP60_SUCC;
}

//DES或SM4下对8字节/16字节整数倍数据做加密/解密计算
static int AlgCompute(const int Alg_Type, uchar *data_in, int data_len, uchar *data_out, uchar *pkey, int key_len, int nMode)
{
	int j=0, num=0, ret=0;
	if( (Alg_Type!=PP60_TDESFLAG&&Alg_Type!=PP60_SM4FLAG) || data_in==NULL || pkey==NULL)
		return NDK_ERR;
	if(Alg_Type == PP60_TDESFLAG)
	{	
		num=data_len/8;
		for(;j<num;j++)
		{
			if(NDK_AlgTDes(data_in+8*j, data_out+8*j, pkey, key_len, nMode) != NDK_OK)
			{
				cls_show_msg("line %d:测试失败", __LINE__);
				return NDK_ERR;
			}
		}
	}
#if ALG_SM_ENABLE
	else
	{
		num=data_len/16;
		for(;j<num;j++)
		{			
			if((ret=NDK_AlgSM4Compute(pkey, NULL, data_len, data_in+16*j, data_out+16*j, nMode))!= NDK_OK)
			{
				cls_show_msg("line %d:测试失败 %d ",  __LINE__,ret);		
				return NDK_ERR;
			}
		}
	}
#endif	
	return NDK_OK;
}

//本函数成功执行的效果:00组mkey改为全0x42, ukey(00)改为全0x38, ukey(01)改为全0x40;168组不做修改(使用默认,mkey为全0x38,ukey(x)为全0x00)
static ushort test_PP60_cmd_MSU(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0}, out1[PP60_MAXLEN_DATA] = {0}, out2[PP60_MAXLEN_DATA] = {0}, OK[16] = {0}, NK[16] = {0}, tmp[16] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, outlen = 0, outlen1 = 0, outlen2 = 0;
	
	/*process body*/
	cls_printf("设置密钥中,请耐心等待...");//耗时较长的操作(如,经过829转发,时间就会加长)增加提示,很快完成的及需要手工确认的操作可不用示
	
	//test M
	//前置
	memset(OK, 0x38, sizeof(OK));
	memset(NK, 0x42/*0x39与0x38认为是一样的key*/, sizeof(NK));
	memset(tmp, 0x40, sizeof(tmp));
	outlen = PP60_code(OK, sizeof(OK), out);
	outlen1= PP60_code(NK, sizeof(NK), out1);
	outlen2= PP60_code(tmp, sizeof(tmp), out2);
	
	if((staval=PP60_cmd_M(0xa8, out, outlen, out1, outlen1))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	if((staval=PP60_cmd_M(0x00, NULL, 0, NULL, 0))!=NL829_PP60_FAIL||(staval1=PP60_cmd_M(0x00, OK, sizeof(OK), NK, sizeof(NK)))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
	if((staval=PP60_cmd_M(0x00, out1, outlen1, out, outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	if((staval=PP60_cmd_M(0x00, out, outlen, out1, outlen1-2))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}	
	if((staval=PP60_cmd_M(0x00, out, outlen, out1, outlen1))!=NL829_PP60_SUCC)//00组mkey改为全0x42
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}

	//test U
#if TEST
	if((staval=PP60_cmd_U(0xa8, 0x00, out1, outlen1, out, outlen))!=NL829_PP60_FAIL||(staval1=PP60_cmd_U(0x00, 0x02, out1, outlen1, out, outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
#endif
	if((staval=PP60_cmd_U(0x00, 0x00, NULL, 0, NULL, 0))!=NL829_PP60_FAIL||(staval1=PP60_cmd_U(0x00, 0x00, NK, sizeof(NK), OK, sizeof(OK)))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
	if((staval=PP60_cmd_U(0x00, 0x00, out, outlen, out1, outlen1))!=NL829_PP60_FAIL||(staval1=PP60_cmd_U(0x00, 0x01, out, outlen, out2, outlen2))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
	if((staval=PP60_cmd_U(0x00, 0x00, out1, outlen1, out, outlen-2))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	if((staval=PP60_cmd_U(0x00, 0x00, out1, outlen1, out, outlen))!=NL829_PP60_SUCC)//00组ukey(00)改为全0x38
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
#if 0	//临时调试for test S,最终效果与test S相同
	if((staval=PP60_cmd_U(0x00, 0x01, out1, outlen1, out2, outlen2))!=NL829_PP60_SUCC)//00组ukey(01)改为全0x40
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	return NL829_PP60_SUCC;
#endif
	
	//test S
	//前置
	//if(mdes3(ENCRYPT, tmp, OK, NK)!=SUCC||mdes3(ENCRYPT, tmp+8, OK+8, NK)!=SUCC)//OK已无用了,用于作中间缓冲
	//if(NDK_AlgTDes(tmp, OK, NK, sizeof(NK), ALG_TDS_MODE_ENC)!=NDK_OK||NDK_AlgTDes(tmp+8, OK+8, NK, sizeof(NK), ALG_TDS_MODE_ENC)!=NDK_OK)//OK已无用了,用于作中间缓冲
	if(AlgCompute(g_algtype, tmp, sizeof(tmp), OK, NK, sizeof(NK), ((g_algtype==PP60_TDESFLAG)?ALG_TDS_MODE_ENC:ALG_SM4_ENCRYPT_ECB) )!=NDK_OK)//OK已无用了,用于作中间缓冲
	{
		cls_show_msg("line %d:测试失败", __LINE__);
		return NL829_PP60_FAIL;
	}
	outlen2 = PP60_code(OK, sizeof(OK), out2);

	if((staval=PP60_cmd_A(0x00, 0x01, g_algtype))!=NL829_PP60_SUCC)//在S命令解密之前，需激活成相应的算法类型
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}//如果把该激活动作放到M指令前,则S指令会失败,原因是:设计上激活过的SM4密钥,在经过M指令修改后不会被使用,需要重新激活才会被使用 20160309 zhenglw解释
#if TEST
	if((staval=PP60_cmd_S(0xa8, 0x00, out2, outlen2))!=NL829_PP60_FAIL||(staval1=PP60_cmd_S(0x00, 0x02, out2, outlen2))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
	if((staval=PP60_cmd_S(0x00, 0x01, NULL, 0))!=NL829_PP60_FAIL||(staval1=PP60_cmd_S(0x00, 0x01, OK, sizeof(OK)))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
#endif
	if((staval=PP60_cmd_S(0x00, 0x01, out2, outlen2-2))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}	
	if((staval=PP60_cmd_S(0x00, 0x01, out2, outlen2))!=NL829_PP60_SUCC)//00组ukey(01)改为全0x40
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	
	return NL829_PP60_SUCC;
}

static ushort test_PP60_cmd_K(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0}, str[PP60_CMDK_SM4_STRLEN] = {0}/*str其实只要8字节即可,这里蛮开大些*/, STR[PP60_CMDK_SM4_STRLEN*2] = {0}, key[16] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, outlen = 0;
	int STRlen = 0, data_len = 0;
	
	/*process body*/
	//前置
	data_len=(g_algtype==PP60_TDESFLAG)?PP60_CMDK_STRLEN:PP60_CMDK_SM4_STRLEN;
	memset(key, 0x38, sizeof(key));
	memset(str, 0x31, data_len);
	STRlen = PP60_code(str, data_len, STR);

	if((staval=PP60_cmd_K(0xa8, STR, data_len, out, &outlen) )!=NL829_PP60_FAIL||(staval1=PP60_cmd_K(0xa7, STR, STRlen, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
#if TEST
	if((staval=PP60_cmd_K(0xa7, NULL, 0, out, &outlen))!=NL829_PP60_FAIL||(staval1=PP60_cmd_K(0xa7, str, data_len, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
#endif
	if((staval=PP60_cmd_K(0xa7, STR, data_len-1, out, &outlen))!=NL829_PP60_FAIL||(staval1=PP60_cmd_K(0xa7, STR, data_len+1, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
	if((staval=PP60_cmd_K(0xa7, STR, data_len-2, out, &outlen))!=NL829_PP60_FAIL||(staval1=PP60_cmd_K(0xa7, STR, data_len+2, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
	if((staval=PP60_cmd_K(0xa7, STR, data_len, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	else//进一步校验
	{
		//DEBUG_HEX(out, outlen, NULL);
		if((STRlen=PP60_decode(out, outlen, STR))!=data_len/2)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, STRlen);
			return NL829_PP60_FAIL;			
		}
		if(AlgCompute(g_algtype, str, data_len/2, out, key, sizeof(key), ((g_algtype==PP60_TDESFLAG)?ALG_TDS_MODE_DEC:ALG_SM4_DECRYPT_ECB) ) != NDK_OK)
		{
			cls_show_msg("line %d:测试失败", __LINE__);
			return NL829_PP60_FAIL;
		}
		if(memcmp(out, STR, STRlen))
		{
			//DEBUG_HEX(out, STRlen, NULL);
			//DEBUG_HEX(STR, STRlen, NULL);
			cls_show_msg("line %d:测试失败", __LINE__);
			return NL829_PP60_FAIL;
		}
	}

	return NL829_PP60_SUCC;
}

static ushort test_PP60_cmd_T(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0}, out1[PP60_MAXLEN_DATA] = {0}, D[700] = {0}, data[350] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, outlen = 0;
	int Dlen = 0, i=-1;
	//以下结果可以由PC工具先预先算出来
	DATA_RESULT result[ ]={{8,(uchar *)"\xFB\x04\xBE\xE7\x8F\x3A\x38\x53"}, //0 使用方法1  数据为21个字节的0x31 密钥为8个字节的0x00
						   {8,(uchar *)"\x62\x2E\x2C\xD5\x2C\x3F\xDC\x83"}, //1 使用方法2  数据为21个字节的0x31 密钥为8个字节的0x00
						   {8,(uchar *)"\x8C\xA6\x4D\xE9\xC1\xB1\x23\xA7"}, //2 使用方法1  数据为8个字节的0x00 密钥为8个字节的0x00
						   {8,(uchar *)"\x8C\xA6\x4D\xE9\xC1\xB1\x23\xA7"}, //3 使用方法2  数据为8个字节的0x00 密钥为8个字节的0x00
						   {8,(uchar *)"\xDC\x64\xBE\x0D\x5B\x0E\x71\x43"}, //4 使用方法1  数据为250个字节的0x31 密钥为8个字节的0x00
						   {8,(uchar *)"\x14\x40\x0F\x74\xED\xAD\x49\x59"}, //5 使用方法2  数据为250个字节的0x31 密钥为8个字节的0x00
						   {16,(uchar *)"\xC7\xCE\x1F\x68\xD5\xC8\x4A\xF3\xB4\xBD\x6D\xDD\x07\x0C\x97\x37"}, //0+6 使用方法1  数据为21个字节的0x31 密钥为16个字节的0x00
						   {16,(uchar *)"\x4C\xC0\xBB\xAE\x84\x0D\xE2\x16\x97\x84\x15\x9A\x31\xB8\x0F\x6F"}, //1+6 使用方法2  数据为21个字节的0x31 密钥为16个字节的0x00
						   {16,(uchar *)"\x9F\x1F\x7B\xFF\x6F\x55\x11\x38\x4D\x94\x30\x53\x1E\x53\x8F\xD3"}, //2+6 使用方法1  数据为16个字节的0x00 密钥为16个字节的0x00
						   {16,(uchar *)"\x9F\x1F\x7B\xFF\x6F\x55\x11\x38\x4D\x94\x30\x53\x1E\x53\x8F\xD3"}, //3+6 使用方法2  数据为16个字节的0x00 密钥为16个字节的0x00
						   {16,(uchar *)"\x4C\xC1\xAE\x75\xA2\xCD\x61\x95\x63\x71\x37\x04\xC2\x05\xE8\x32"}, //4+6 使用方法1  数据为250个字节的0x31 密钥为16个字节的0x00
						   {16,(uchar *)"\xAD\x80\x65\xFC\xCF\x8B\x26\x40\x41\x9D\xD6\x1C\x88\x22\x03\x6F"}  //5+6 使用方法2  数据为250个字节的0x31 密钥为16个字节的0x00
						  };
	
	/*process body*/
	cls_printf("测试T命令中,请耐心等待...");
	//前置
	i=(g_algtype==PP60_TDESFLAG)?0:1;//根据算法选择验证数据
	memset(data, 0x31, sizeof(data));
	Dlen = PP60_code(data, sizeof(data), D);

#if TEST
	if((staval=PP60_cmd_T(0x04, D, 8, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	if(g_PP60type!=DEVICE_PP60RF&&(staval=PP60_cmd_T(0x00, D, Dlen, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}	
	if((staval=PP60_cmd_T(0x00, data, sizeof(data), out, &outlen))!=NL829_PP60_FAIL||(staval1=PP60_cmd_T(0x01, data, sizeof(data), out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
	//mac算法允许没有数据的(因为有初始向量)
	if((staval=PP60_cmd_T(0x00, NULL, 0, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}	
	else//进一步校验
	{
		if((Dlen=PP60_decode(out, outlen, out1))!=result[2+i*6].len ||memcmp(out1, result[2+i*6].data, Dlen))
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, Dlen);
			return NL829_PP60_FAIL;
		}
	}
#endif
	if((staval=PP60_cmd_T(0x01, NULL, 0, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}	
	else//进一步校验
	{
		if((Dlen=PP60_decode(out, outlen, out1))!=result[3+i*6].len ||memcmp(out1, result[3+i*6].data, Dlen))
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, Dlen);
			return NL829_PP60_FAIL;
		}
	}
	//再算一遍,结果不应改变
#if TEST
	if((staval=PP60_cmd_T(0x00, NULL, 0, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}	
	else//进一步校验
	{
		if((Dlen=PP60_decode(out, outlen, out1))!=result[2+i*6].len ||memcmp(out1, result[2+i*6].data, Dlen))
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, Dlen);
			return NL829_PP60_FAIL;
		}
	}
#endif
	if((staval=PP60_cmd_T(0x01, NULL, 0, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}	
	else//进一步校验
	{
		if((Dlen=PP60_decode(out, outlen, out1))!=result[3+i*6].len ||memcmp(out1, result[3+i*6].data, Dlen))
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, Dlen);
			return NL829_PP60_FAIL;
		}
	}
	
	if((staval=PP60_cmd_T(0x00, D, 21*2-1, out, &outlen))!=NL829_PP60_FAIL||(staval1=PP60_cmd_T(0x01, D, 21*2+1, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
	if((staval=PP60_cmd_T(0x00, D, 21*2, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	else//进一步校验
	{
		if((Dlen=PP60_decode(out, outlen, out1))!=result[i*6].len ||memcmp(out1, result[i*6].data, Dlen))
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, Dlen);
			return NL829_PP60_FAIL;
		}
	}
	if((staval=PP60_cmd_T(0x01, D, 21*2, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	else//进一步校验
	{
		if((Dlen=PP60_decode(out, outlen, out1))!=result[1+i*6].len ||memcmp(out1, result[1+i*6].data, Dlen))
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, Dlen);
			return NL829_PP60_FAIL;
		}
	}

	//密钥长度取250，此处为了验证密钥长度在150-300之间报错的问题
       if((staval=PP60_cmd_T(0x00, D, 250*2, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	else//进一步校验
	{
		if((Dlen=PP60_decode(out, outlen, out1))!=result[4+i*6].len ||memcmp(out1, result[4+i*6].data, Dlen))
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, Dlen);
			return NL829_PP60_FAIL;
		}
	}
	if((staval=PP60_cmd_T(0x01, D, 250*2, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	else//进一步校验
	{
		if((Dlen=PP60_decode(out, outlen, out1))!=result[5+i*6].len ||memcmp(out1, result[5+i*6].data, Dlen))
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, Dlen);
			return NL829_PP60_FAIL;
		}
	} 

	return NL829_PP60_SUCC;
}

static ushort test_PP60_cmd_H(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0}, str[PP60_SM4_CMDH_LS] = {0}/*str其实只要8字节即可,这里蛮开大些*/, STR[PP60_SM4_CMDH_LS*2] = {0}, key[16] = {0}, tmp[16] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, outlen = 0;
	int STRlen = 0, tmplen = 0, data_len = 0;
	
	/*process body*/
	cls_printf("测试H命令中,请耐心等待...");
	//前置
	data_len=(g_algtype==PP60_TDESFLAG)?PP60_CMDH_LS:PP60_SM4_CMDH_LS;
	memset(key, 0, sizeof(key));
	memset(str, 0x31, data_len);
	STRlen = PP60_code(str, data_len, STR);

	if((staval=PP60_cmd_H(3, STR, data_len, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
#if TEST
	if((staval=PP60_cmd_H(0, NULL, 0, out, &outlen))!=NL829_PP60_FAIL||(staval1=PP60_cmd_H(1, str, data_len, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
#endif
	if((staval=PP60_cmd_H(0, STR, data_len-1, out, &outlen))!=NL829_PP60_FAIL||(staval1=PP60_cmd_H(1, STR, data_len+1, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
#if TEST
	if((staval=PP60_cmd_H(0, STR, data_len-2, out, &outlen))!=NL829_PP60_FAIL||(staval1=PP60_cmd_H(1, STR, data_len+2, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
#endif
	if((staval=PP60_cmd_H(0, STR, data_len, out, &outlen))!=NL829_PP60_SUCC||(staval1=PP60_cmd_G(out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		return NL829_PP60_FAIL;
	}
	else//进一步校验
	{
		//DEBUG_HEX(out, outlen, NULL);
		if((tmplen=PP60_decode(out, outlen, tmp))!= data_len/2)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			return NL829_PP60_FAIL;
		}
		//if(NDK_AlgTDes(str, out, key, sizeof(key), ALG_TDS_MODE_ENC)!=NDK_OK)//if(mdes3(ENCRYPT, str, out, key)!=SUCC)
		if(AlgCompute(g_algtype, str, data_len/2 , out, key, sizeof(key), ((g_algtype==PP60_TDESFLAG)?ALG_TDS_MODE_ENC:ALG_SM4_ENCRYPT_ECB) ) != NDK_OK)
		{
			cls_show_msg("line %d:测试失败", __LINE__);
			return NL829_PP60_FAIL;
		}		
		if(memcmp(out, tmp, tmplen))
		{
			//DEBUG_HEX(out, tmplen, NULL);
			//DEBUG_HEX(tmp, tmplen, NULL);
			cls_show_msg("line %d:测试失败", __LINE__);
			return NL829_PP60_FAIL;
		}
	}
	if((staval=PP60_cmd_H(1, STR, data_len, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return NL829_PP60_FAIL;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!= data_len/2)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			return NL829_PP60_FAIL;
		}
		if(AlgCompute(g_algtype, str, data_len/2, out, key, sizeof(key), ((g_algtype==PP60_TDESFLAG)?ALG_TDS_MODE_ENC:ALG_SM4_ENCRYPT_ECB) ) != NDK_OK)
		{
			cls_show_msg("line %d:测试失败", __LINE__);
			return NL829_PP60_FAIL;
		}		
		if(memcmp(out, tmp, tmplen))
		{
			cls_show_msg("line %d:测试失败", __LINE__);
			return NL829_PP60_FAIL;
		}
	}

	return NL829_PP60_SUCC;
}

static ushort test_PP60_cmd_LN(void)
{
	/*private & local definition*/
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL;
	
	/*process body*/
	//以下设置L与N时不考虑L<N的实测情况,放到异常测试中做
	//注意:最大密码长度要大于0,最小密码长度则可以为0
#if TEST
	if((staval=PP60_cmd_N(0x1e))!=NL829_PP60_SUCC||(staval1=PP60_cmd_L(0x01))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	if((staval1=PP60_cmd_N(0x00))!=NL829_PP60_SUCC||(staval=PP60_cmd_L(0x00))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
#endif
	if((staval=PP60_cmd_N(0x1e))!=NL829_PP60_SUCC||(staval1=PP60_cmd_L(0x1e))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	if((staval=PP60_cmd_N(0x1f))!=NL829_PP60_FAIL||(staval1=PP60_cmd_L(0x1f))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}

	return NL829_PP60_SUCC;
ERR://恢复默认设置
	PP60_cmd_N(0x04);
	PP60_cmd_L(0x10);
	return NL829_PP60_FAIL;
}

static ushort test_PP60_cmd_F(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0}, tmp[32] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, outlen = 0, ret = NL829_PP60_FAIL;
	int tmplen = 0, i=0;
	DATA_RESULT result[ ]={{16,(uchar *)"\x04\x16\x19\xB0\x79\x4C\xF5\x60\x5C\x60\xEE\xEF\x2D\x98\xE8\x41"}, // 0 数据为13个字节的0x31+3个字节的0x00 key为8字节的0X38 
						   {8 ,(uchar *)"\x1C\x6D\xDA\x6E\x97\xDC\xC7\xD3"}, // 1 数据为6个字节的0x31+10个字节的0x00 key为8字节的0X38
						   {24,(uchar *)"\x04\x16\x19\xB0\x79\x4C\xF5\x60\x04\x16\x19\xB0\x79\x4C\xF5\x60\xA9\x94\x82\x48\xB4\x19\x48\xEB"}, // 2 数据为23个字节的0x31+9个字节的0x00 key为16字节的0X38 
						   {16,(uchar *)"\x14\xFD\x55\xAD\x41\xEB\x3D\x89\x79\x5E\x6A\xCD\x0B\x68\x91\x03"}, // 0+3 数据为13个字节的0x31+3个字节的0x00 key为16字节的0X38 
						   {16,(uchar *)"\xAE\xD8\x5B\xE2\x38\x73\x19\xE6\x14\x9E\x10\xF0\xD0\x31\x54\x08"}, // 1+3 数据为6个字节的0x31+10个字节的0x00 key为16字节的0X38 
						   {32,(uchar *)"\xAF\x93\xE6\x2F\x52\xCB\xE9\x73\x4C\x1B\x70\x35\x00\x45\x34\x07\x23\x89\xD6\x6A\x4A\x7D\x8A\x1D\x0A\x7B\x55\x9A\xD0\x8D\x7E\xCC"}  // 2+3 数据为23个字节的0x31+9个字节的0x00 key为16字节的0X38 
						  };

	/*process body*/
	//前置
	PP60_cmd_C();
	i=(g_algtype==PP60_TDESFLAG)?0:1;
	
	//异常
	if((staval=PP60_cmd_F(6, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	//正常
	//前置
	if((staval=PP60_cmd_L(0x0d))!=NL829_PP60_SUCC||(staval1=PP60_cmd_N(0x0d))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	cls_printf("请在PP60上输入13个1+[ENTER],提示\"输入\"+*+语音");//设备对13个1进行加密时,不正好8倍数长度的会补0x00
	if((staval=PP60_cmd_F(0, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=result[3*i].len||memcmp(result[3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}
	
	//前置
	if((staval=PP60_cmd_L(0x17))!=NL829_PP60_SUCC||(staval1=PP60_cmd_N(0x17))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	cls_printf("请在PP60上输入23个1+[ENTER],提示\"再输入\"+*+语音");
	if((staval=PP60_cmd_F(1, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=result[2+3*i].len||memcmp(result[2+3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"再输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}
	
	//前置
	if((staval=PP60_cmd_L(0x06))!=NL829_PP60_SUCC||(staval1=PP60_cmd_N(0x06))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}	
	cls_printf("请在PP60上输入6个2再改为6个1+[ENTER],提示\"输入\"+明文+语音");
	if((staval=PP60_cmd_F(2, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=result[1+3*i].len||memcmp(result[1+3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+明文+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	cls_printf("请在PP60上输入6个1+[ENTER],提示\"再输入\"+明文+语音");
	if((staval=PP60_cmd_F(3, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=result[1+3*i].len||memcmp(result[1+3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"再输入\"+明文+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}
	
	cls_printf("请在PP60上输入6个1+[ENTER],提示\"输入\"+*+无语音");
	if((staval=PP60_cmd_F(4, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=result[1+3*i].len||memcmp(result[1+3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+*+无语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	cls_printf("请在PP60上输入6个1+[ENTER],提示\"输入\"+明文+无语音");
	if((staval=PP60_cmd_F(5, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=result[1+3*i].len||memcmp(result[1+3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+明文+无语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	ret = NL829_PP60_SUCC;
ERR:
	PP60_cmd_C();
	return ret;
}

static ushort test_PP60_cmd_E(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0}, tmp[16] = {0}, tmp1[32] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, outlen = 0, ret = NL829_PP60_FAIL;
	int tmplen = 0, i = 0;
	DATA_RESULT result[ ]={{8 ,(uchar *)"\x90\xC4\x75\xFC\xA1\xBA\x89\x83"}, // 0 数据为31 32 33 34 35 36 37 00 key为8字节的0X38  
						   {8 ,(uchar *)"\x2D\x07\x18\x5E\xE5\x3A\x44\x48"}, // 1 数据为31 32 33 34 35 00 00 00 key为8字节的0X38 
						   {16,(uchar *)"\x2A\xAB\xAD\xB8\x86\x4A\xDD\x74\x7E\xE6\xEA\x2E\xB7\x7D\x84\x59"}, // 0+2 数据为31 32 33 34 35 36 37 + 11个字节的0x00 key为8字节的0X38  
						   {16,(uchar *)"\x30\xE9\x98\x4A\x43\x68\x73\xE3\x52\x3D\x4E\x28\x26\x69\xFD\xDD"}  // 1+2 数据为31 32 33 34 35 + 11个字节的0x00 key为8字节的0X38 
						  };
	
	/*process body*/
	//前置
	PP60_cmd_C();
	i=(g_algtype==PP60_TDESFLAG)?0:1;
	
	//异常
	if((staval=PP60_cmd_E(8))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	//正常
	//前置//本例仅设置密码范围为[4,8],即加密结果为8bytes.设置位数多于8的情况在F中已测试,本例不测
	//修改PIN的测试也在F中做了,本例不测
	if((staval=PP60_cmd_L(0x08))!=NL829_PP60_SUCC||(staval1=PP60_cmd_N(0x04))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	
	cls_printf("请在PP60上输入1234567+[ENTER],提示\"输入\"+*+语音");//设备对13个1进行加密时,不正好8倍数长度的会补0
	if((staval=PP60_cmd_E(0))!=NL829_PP60_SUCC||(staval1=PP60_cmd_G(out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=result[2*i].len||memcmp(result[2*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			//DEBUG_HEX(out, outlen, NULL);
			//DEBUG_HEX(tmp, 8, NULL);
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}
	
	cls_printf("请在PP60上输入1234567+[ENTER],提示\"再输入\"+*+语音");
	if((staval=PP60_cmd_E(1))!=NL829_PP60_SUCC||(staval1=PP60_cmd_G(out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=result[2*i].len||memcmp(result[2*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"再输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	cls_printf("请在PP60上输入12345+[ENTER],提示\"输入\"+明文+语音");
	if((staval=PP60_cmd_E(2))!=NL829_PP60_SUCC||(staval1=PP60_cmd_G(out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=result[1+2*i].len||memcmp(result[1+2*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+明文+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}
	
	cls_printf("请在PP60上输入12345+[ENTER],提示\"再输入\"+明文+语音");
	if((staval=PP60_cmd_E(3))!=NL829_PP60_SUCC||(staval1=PP60_cmd_G(out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=result[1+2*i].len||memcmp(result[1+2*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"再输入\"+明文+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	cls_printf("请在PP60上输入12345+[ENTER],提示\"输入\"+*+语音");
	if((staval=PP60_cmd_E(4))!=NL829_PP60_SUCC||(staval1=PP60_cmd_G(out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=strlen("12345")||memcmp("12345", tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}
	
	cls_printf("请在PP60上输入12345+[ENTER],提示\"再输入\"+*+语音");
	if((staval=PP60_cmd_E(5))!=NL829_PP60_SUCC||(staval1=PP60_cmd_G(out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=strlen("12345")||memcmp("12345", tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"再输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	//注意:以下两条子命令只支持大写字母,不支持小写
	cls_printf("请在PP60上输入1A2Y3+[ENTER],提示\"输入\"+*+语音");
	if((staval=PP60_cmd_E(6))!=NL829_PP60_SUCC||(staval1=PP60_cmd_G(out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=strlen("1A2Y3")||memcmp("1A2Y3", tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}
	
	cls_printf("请在PP60上输入1A2Y3+[ENTER],提示\"再输入\"+*+语音");
	if((staval=PP60_cmd_E(7))!=NL829_PP60_SUCC||(staval1=PP60_cmd_G(out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=strlen("1A2Y3")||memcmp("1A2Y3", tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"再输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	//验证LN位数设置的有效性
	if((staval=PP60_cmd_N(0x01))!=NL829_PP60_SUCC||(staval1=PP60_cmd_L(0x01))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}	
	cls_printf("请在PP60上输入1+[ENTER],提示\"输入\"+*+语音");
	if((staval=PP60_cmd_E(4))!=NL829_PP60_SUCC||(staval1=PP60_cmd_G(out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!=strlen("1")||memcmp("1", tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	if((staval=PP60_cmd_N(0x1e))!=NL829_PP60_SUCC||(staval1=PP60_cmd_L(0x1e))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}	
	cls_printf("请在PP60上输入30个1+[ENTER],提示\"输入\"+*+语音");
	if((staval=PP60_cmd_E(4))!=NL829_PP60_SUCC||(staval1=PP60_cmd_G(out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	else//进一步校验
	{
		memset(tmp1, 0x31, sizeof(tmp1));
		tmp1[30] = 0x00;
		if((tmplen=PP60_decode(out, outlen, tmp))!=strlen((char *)tmp1)||memcmp(tmp1, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	ret = NL829_PP60_SUCC;
ERR:
	PP60_cmd_C();
	return ret;	
}

static ushort __test_PP60_cmd_IX(ushort (*func)(uchar , uchar *, int, uchar *, ushort *))
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0}, tmp[16] = {0};
	uchar cardno[PP60_CARDNOLEN*2] = {0}, cardno1[PP60_CARDNOLEN*2] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, outlen = 0, ret = NL829_PP60_FAIL;
	int tmplen = 0, i = 0, data_len = 0;
	DATA_RESULT result[ ]={{8 ,(uchar *)"\xFA\x0B\x71\x38\x95\x29\xE5\xCE"}, // 0 数据为PIN(05 12 34 5F FF FF FF FF) 异或上 PAN(00 00 75 01 23 45 67 89)   key为8字节的0X40 
						   {8 ,(uchar *)"\x71\xCB\xE5\x0C\xB5\xE0\xF0\xC6"}, // 1 数据为PIN(07 12 34 56 7F FF FF FF) 异或上 PAN(00 00 75 01 23 45 67 89)   key为8字节的0X40
						   {8 ,(uchar *)"\xB3\x2C\xAD\x1B\xDF\xCE\xFC\xED"}, // 2 数据为PIN(0B 11 11 11 11 11 1F FF) 异或上 PAN(00 00 75 01 23 45 67 89)   key为8字节的0X40
						   {16,(uchar *)"\xF9\x93\x0B\x77\x81\xE1\xAD\x2B\x44\x19\xF0\x19\x99\x79\x21\x78"}, // 0+3 数据为PIN(05 12 34 5F FF FF FF FF) 异或上 PAN(00 00 75 01 23 45 67 89)   key为16字节的0X40
						   {16,(uchar *)"\x37\x08\x7D\x7A\xD5\x12\x5B\x09\x10\x3A\x5F\xD5\x25\x5C\x03\xEA"}, // 1+3 数据为PIN(07 12 34 56 7F FF FF FF) 异或上 PAN(00 00 75 01 23 45 67 89)   key为16字节的0X40
						   {16,(uchar *)"\x71\x64\xD7\x79\x8E\x9A\x52\x26\x27\x8E\xF4\xEA\x01\x0C\x00\x9E"}  // 2+3 数据为PIN(0B 11 11 11 11 11 1F FF) 异或上 PAN(00 00 75 01 23 45 67 89)   key为16字节的0X40
						  };

	/*process body*/
	//前置
	PP60_cmd_C();
	data_len=(g_algtype==PP60_TDESFLAG)?PP60_LEN_PINBLOCK:PP60_SM4_LEN_PINBLOCK;
	i=(g_algtype==PP60_TDESFLAG)?0:1;

	//前置
	if((staval=PP60_cmd_L(0x0f))!=NL829_PP60_SUCC||(staval1=PP60_cmd_N(0x00))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	
	//异常
	//cls_printf("请在PP60上输入1234567+[ENTER],提示\"输入\"+*+语音");
#if TEST
	//异常1
	//cls_printf("请在PP60上输入1234567+[ENTER],提示\"输入\"+*+语音");	
	memset(cardno, 0x01, sizeof(cardno));
	strcpy((char *)cardno1, "11111111111a");
	if((staval=func(0, cardno, PP60_CARDNOLEN, out, &outlen))!=NL829_PP60_FAIL||(staval1=func(0, cardno1, PP60_CARDNOLEN, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
#endif
	//异常2
	//cls_printf("请在PP60上输入1234567+[ENTER],提示\"输入\"+*+语音");
	memset(cardno, 0x31, sizeof(cardno));
	if((staval=func(0, cardno, 0, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=func(0, cardno, PP60_CARDNOLEN-1, out, &outlen))!=NL829_PP60_FAIL||(staval1=func(0, cardno, PP60_CARDNOLEN+1, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}	
	if((staval=func(0, cardno, PP60_CARDNOLEN-2, out, &outlen))!=NL829_PP60_FAIL||(staval1=func(0, cardno, PP60_CARDNOLEN+2, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	//异常3
	//memset(cardno, 0x31, sizeof(cardno));
	if((staval=func(3, cardno, PP60_CARDNOLEN, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//异常4,输入超长的PIN
	cls_printf("请在PP60上输入15个1+[ENTER],提示\"输入\"+*+语音");
	//memset(cardno, 0x31, sizeof(cardno));
	if((staval=func(0, cardno, PP60_CARDNOLEN, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	//正常
	strcpy((char *)cardno, "750123456789");
	cls_printf("请在PP60上输入12345+[ENTER],提示\"输入\"+*+语音");
	if((staval=func(0, cardno, PP60_CARDNOLEN, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!= data_len ||memcmp(result[3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			//DEBUG_HEX(out, outlen, NULL);
			//DEBUG_HEX(tmp, tmplen, NULL);
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}
	
	cls_printf("请在PP60上输入1234567+[ENTER],提示\"再输入\"+*+语音");
	if((staval=func(1, cardno, PP60_CARDNOLEN, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!= data_len ||memcmp(result[1+3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"再输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	cls_printf("请在PP60上输入11个1+[ENTER],提示\"输入\"+*+无语音");
	if((staval=func(2, cardno, PP60_CARDNOLEN, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!= data_len ||memcmp(result[2+3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+*+无语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	//追加直接按确认返回的用例
	if(func==PP60_cmd_X)
	{
	cls_printf("请在PP60上输入[ENTER],提示\"输入\"+*+语音");
	if((staval=func(0, cardno, PP60_CARDNOLEN, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if(outlen!=1||cls_show_msg("若输入过程中,有提示\"输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, outlen);
			goto ERR;
		}
	}

	cls_printf("请在PP60上输入[ENTER],提示\"再输入\"+*+语音");
	if((staval=func(1, cardno, PP60_CARDNOLEN, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if(outlen!=1||cls_show_msg("若输入过程中,有提示\"再输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, outlen);
			goto ERR;
		}
	}

	cls_printf("请在PP60上输入[ENTER],提示\"输入\"+*+无语音");
	if((staval=func(2, cardno, PP60_CARDNOLEN, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if(outlen!=1||cls_show_msg("若输入过程中,有提示\"输入\"+*+无语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, outlen);
			goto ERR;
		}
	}		
	}

	ret = NL829_PP60_SUCC;
ERR:
	PP60_cmd_C();
	return ret;
}

static ushort test_PP60_cmd_I(void){return __test_PP60_cmd_IX(PP60_cmd_I);}

static ushort test_PP60_cmd_X(void){return __test_PP60_cmd_IX(PP60_cmd_X);}

static ushort test_PP60_cmd_J(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0}, tmp[16] = {0};
	uchar cardno[PP60_CARDNOLEN*2] = {0}, cardno1[PP60_CARDNOLEN*2] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, outlen = 0, ret = NL829_PP60_FAIL;
	int tmplen = 0 , i = 0, data_len = 0;
	DATA_RESULT result[ ]={{8 ,(uchar *)"\xFA\x0B\x71\x38\x95\x29\xE5\xCE"}, // 0 数据为PIN(05 12 34 5F FF FF FF FF) 异或上 PAN(00 00 75 01 23 45 67 89)   key为8字节的0X40 
						   {8 ,(uchar *)"\x71\xCB\xE5\x0C\xB5\xE0\xF0\xC6"}, // 1 数据为PIN(07 12 34 56 7F FF FF FF) 异或上 PAN(00 00 75 01 23 45 67 89)   key为8字节的0X40
						   {8 ,(uchar *)"\xB3\x2C\xAD\x1B\xDF\xCE\xFC\xED"}, // 2 数据为PIN(0B 11 11 11 11 11 1F FF) 异或上 PAN(00 00 75 01 23 45 67 89)   key为8字节的0X40
						   {16,(uchar *)"\xF9\x93\x0B\x77\x81\xE1\xAD\x2B\x44\x19\xF0\x19\x99\x79\x21\x78"}, // 0+3 数据为PIN(05 12 34 5F FF FF FF FF) 异或上 PAN(00 00 75 01 23 45 67 89)   key为16字节的0X40
						   {16,(uchar *)"\x37\x08\x7D\x7A\xD5\x12\x5B\x09\x10\x3A\x5F\xD5\x25\x5C\x03\xEA"}, // 1+3 数据为PIN(07 12 34 56 7F FF FF FF) 异或上 PAN(00 00 75 01 23 45 67 89)   key为16字节的0X40
						   {16,(uchar *)"\x71\x64\xD7\x79\x8E\x9A\x52\x26\x27\x8E\xF4\xEA\x01\x0C\x00\x9E"}  // 2+3 数据为PIN(0B 11 11 11 11 11 1F FF) 异或上 PAN(00 00 75 01 23 45 67 89)   key为16字节的0X40
						  };
	
	/*process body*/
	//前置
	PP60_cmd_C();
	data_len=(g_algtype==PP60_TDESFLAG)?PP60_LEN_PINBLOCK:PP60_SM4_LEN_PINBLOCK;
	i=(g_algtype==PP60_TDESFLAG)?0:1;

	//前置
	if((staval=PP60_cmd_L(0x0f))!=NL829_PP60_SUCC||(staval1=PP60_cmd_N(0x00))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	
	//异常
	//cls_printf("请在PP60上输入1234567+[ENTER],提示\"输入\"+*+语音");
#if TEST
	//异常1
	//cls_printf("请在PP60上输入1234567+[ENTER],提示\"输入\"+*+语音");	
	memset(cardno, 0x01, sizeof(cardno));
	strcpy((char *)cardno1, "11111111111a");
	if((staval=PP60_cmd_J(0, cardno, PP60_CARDNOLEN, NULL, 0, out, &outlen))!=NL829_PP60_FAIL||(staval1=PP60_cmd_J(0, cardno1, PP60_CARDNOLEN, NULL, 0, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
#endif
	//异常2
	//cls_printf("请在PP60上输入1234567+[ENTER],提示\"输入\"+*+语音");
	memset(cardno, 0x31, sizeof(cardno));
	if((staval=PP60_cmd_J(0, cardno, 0, NULL, 0, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=PP60_cmd_J(0, cardno, PP60_CARDNOLEN-1, NULL, 0, out, &outlen))!=NL829_PP60_FAIL/*多的1位卡号会当成STR标题||(staval1=PP60_cmd_J(0, cardno, PP60_CARDNOLEN+1, NULL, 0, out, &outlen))!=NL829_PP60_FAIL*/)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}	
	if((staval=PP60_cmd_J(0, cardno, PP60_CARDNOLEN-2, NULL, 0, out, &outlen))!=NL829_PP60_FAIL/*多的2位卡号会当成STR标题||(staval1=PP60_cmd_J(0, cardno, PP60_CARDNOLEN+2, NULL, 0, out, &outlen))!=NL829_PP60_FAIL*/)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//异常3
	//memset(cardno, 0x31, sizeof(cardno));
	if((staval=PP60_cmd_J(3, cardno, PP60_CARDNOLEN, NULL, 0, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//异常4,输入超长的PIN
	cls_printf("请在PP60上输入15个1+[ENTER],提示\"输入\"+*+语音");
	//memset(cardno, 0x31, sizeof(cardno));
	if((staval=PP60_cmd_J(0, cardno, PP60_CARDNOLEN, NULL, 0, out, &outlen))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示
	//异常5,STRlen超长,STR含不可显示字符(除可显示ASC及自定义的汉字转义码外)
	//cls_printf("请在PP60上输入15个1+[ENTER],提示\"输入\"+*+语音");
	//memset(cardno, 0x31, sizeof(cardno));
	if((staval=PP60_cmd_C())!=NL829_PP60_SUCC)//先清屏,要保证前置条件正确
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=PP60_cmd_J(0, cardno, PP60_CARDNOLEN, (uchar *)"\x80\x81\x82\x83\x84\x80\x81\x82", strlen("\x80\x81\x82\x83\x84\x80\x81\x82"), out, &outlen))!=NL829_PP60_FAIL||cls_show_msg("若PP60有任何提示或语音,按[ENTER],否则按[其它]")==ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg1(2, "子用例测试通过!");//屏幕有变化,以区别于之后的屏幕提示
	//cls_printf("请在PP60上输入1234+[ENTER],提示\"7元\"+\"输入\"+*+语音");
	if((staval=PP60_cmd_J(0, cardno, PP60_CARDNOLEN, (uchar *)"\x37\xff\xff\x87", strlen("\x37\xff\xff\x87"), out, &outlen))!=NL829_PP60_FAIL||cls_show_msg("若PP60有任何提示或语音,按[ENTER],否则按[其它]")==ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	//正常
	strcpy((char *)cardno, "750123456789");
	cls_printf("请在PP60上输入12345+[ENTER],提示\"欢迎\"+\"输入\"+*+语音");
	if((staval=PP60_cmd_J(0, cardno, PP60_CARDNOLEN, (uchar *)"\x92\x93", strlen("\x92\x93"), out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!= data_len ||memcmp(result[3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"欢迎\"+\"输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}
	
	cls_printf("请在PP60上输入1234567+[ENTER],提示\"欢迎\"+\"再输入\"+*+语音");
	if((staval=PP60_cmd_J(1, cardno, PP60_CARDNOLEN, (uchar *)"\x92\x93", strlen("\x92\x93"), out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!= data_len ||memcmp(result[1+3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"欢迎\"+\"再输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	cls_printf("请在PP60上输入11个1+[ENTER],提示\"欢迎\"+\"输入\"+*+无语音");
	if((staval=PP60_cmd_J(2, cardno, PP60_CARDNOLEN, (uchar *)"\x92\x93", strlen("\x92\x93"), out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp)) != data_len ||memcmp(result[2+3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"欢迎\"+\"输入\"+*+无语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	//相比IX命令,追加STR为空的测试即可
	cls_printf("请在PP60上输入12345+[ENTER],提示\"输入\"+*+语音");
	if((staval=PP60_cmd_J(0, cardno, PP60_CARDNOLEN, NULL, 0, out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	else//进一步校验
	{
		if((tmplen=PP60_decode(out, outlen, tmp))!= data_len ||memcmp(result[3*i].data, tmp, tmplen)||cls_show_msg("若输入过程中,有提示\"输入\"+*+语音,按[ENTER],否则按[其它]")!=ENTER)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, tmplen);
			goto ERR;
		}
	}

	ret = NL829_PP60_SUCC;
ERR:
	PP60_cmd_C();
	return ret;
}

static void test_scr_function(void)
{
	/*private & local definition*/
	ushort staval = NL829_PP60_FAIL;

	/*process body*/
	//测试前置
	//case1:屏幕类
	cls_show_msg1(2, "屏幕类指令测试...");//加提示以刷掉测试菜单
	// 输出加密类型
	// cls_show_msg("设置的算法类型为: 0x%x", g_algtype);
	if((staval=PP60_cmd_C())!=NL829_PP60_SUCC)//顺便充当本case的测试前置
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(test_PP60_cmd_D()!=NL829_PP60_SUCC)
		goto ERR;
	PP60_cmd_C();//测试后置
	if(test_PP60_cmd_O()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_P()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_W()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_h()!=NL829_PP60_SUCC)
		goto ERR;
	
	cls_show_msg("屏幕类指令集测试通过");
	
ERR://清理环境,恢复默认
	PP60_cmd_C();
	PP60_cmd_Z();
	PP60_cmd_R((uchar *)"\x41\x41", strlen("\x41\x41"));
	PP60_cmd_R((uchar *)"\x4b\x48", strlen("\x4b\x48"));
	return;

}
static void test_key_function(void)
{
	/*private & local definition*/
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, staval2 = NL829_PP60_FAIL;
	
	//case2:密钥密码类
	//注意以下测试顺序的安排是前者为后者的前置的(不是相互独立的),随意变化测试顺序会引起测试不过
	//G命令不单独测试,与其它加密命令相互验证即可
	cls_show_msg1(2, "密钥密码类指令测试...");//加提示以刷掉测试菜单
	if((staval=PP60_cmd_Z())!=NL829_PP60_SUCC)//顺便充当本case的测试前置//注意本命令与"开机复位"的区别在于"开机复位"时还会默认激活第0组第0个ukey
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(test_PP60_cmd_R()!=NL829_PP60_SUCC)//顺便充当本case的测试前置//执行成功的话,会重置第0,167组主,用密钥
		goto ERR;
	if(test_PP60_cmd_MSU()!=NL829_PP60_SUCC)//顺便充当本case的测试前置
		goto ERR;
	//用于MAC等其它加解密运算(K,T,H)
	//if(test_PP60_cmd_K()!=NL829_PP60_SUCC)//K命令有密钥号域,故不必激活
	//	goto ERR;
#if TEST
	if((staval=PP60_cmd_A(0xa8, 0x00, g_algtype))!=NL829_PP60_FAIL||(staval1=PP60_cmd_A(0x00, 0x02, g_algtype))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
#endif
	if((staval=PP60_cmd_A(0xa7, 0x00, PP60_NODESFLAG))!=NL829_PP60_SUCC||(staval1=PP60_cmd_A(0xa7, 0x00, PP60_DESFLAG))!=NL829_PP60_SUCC||(staval2=PP60_cmd_A(0xa7, 0x00, PP60_TDESFLAG))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x,%04x)", __LINE__, staval, staval1, staval2);
		goto ERR;
	}	
	if((staval=PP60_cmd_A(0xa7, 0x00, g_algtype))!=NL829_PP60_SUCC)//以TDES方式激活第168组的00号工作密钥,另一个工作密钥也是全0x00
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(test_PP60_cmd_K()!=NL829_PP60_SUCC)//由于新增SM4国密算法,需A命令先激活后方可使用 20160303导入国密算法支持引起
		goto ERR;	
	if(test_PP60_cmd_T()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_H()!=NL829_PP60_SUCC)
		goto ERR;
	//用于PIN输入(L,N,E,F)
	if((staval=PP60_cmd_A(0x00, 0x00, g_algtype))!=NL829_PP60_SUCC)//ukey(00)改为全0x38//前置条件
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(test_PP60_cmd_LN()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_F()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_E()!=NL829_PP60_SUCC)
		goto ERR;
	//用于PINBLOCK的计算(I,J,X)
	if((staval=PP60_cmd_A(0x00, 0x01, g_algtype))!=NL829_PP60_SUCC)//ukey(01)改为全0x40//前置条件
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(test_PP60_cmd_I()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_X()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_J()!=NL829_PP60_SUCC)
		goto ERR;
	
	cls_show_msg("密钥密码类指令集测试通过");
ERR://清理环境,恢复默认
	PP60_cmd_C();
	PP60_cmd_Z();
	PP60_cmd_R((uchar *)"\x41\x41", strlen("\x41\x41"));
	PP60_cmd_R((uchar *)"\x4b\x48", strlen("\x4b\x48"));
	return;

}

static void test_other_function(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0};
	ushort staval = NL829_PP60_FAIL, outlen = 0;

	//case3:其它
	cls_show_msg1(2, "其它类指令测试...");//加提示以刷掉测试菜单
	memset(out, 0, sizeof(out));//从不用根据outlen来截短
	if((staval=PP60_cmd_V(out, &outlen))!=NL829_PP60_SUCC||cls_show_msg("取到版本:%s,正确按[ENTER];否则按[其它]", out)!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(test_PP60_cmd_x()!=NL829_PP60_SUCC)
		goto ERR;

	cls_show_msg("其他类指令集测试通过");
ERR://清理环境,恢复默认
	PP60_cmd_C();
	PP60_cmd_Z();
	PP60_cmd_R((uchar *)"\x41\x41", strlen("\x41\x41"));
	PP60_cmd_R((uchar *)"\x4b\x48", strlen("\x4b\x48"));
	return;
}
static void test_function(void)
{
	/*private & local definition*/
	int keyin = 0;

	/*process body*/
	while(1)
       {
		keyin = cls_show_msg("1.屏幕类指令\n"
							 "2.密钥密码类指令\n"
							 "3.其他指令\n");
		switch(keyin) 
		{
		case '1':
			test_scr_function();
			break;
		case '2':
			test_key_function();
			break;
		case '3':
			test_other_function();
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
#if 0 //把原来的一系类测试拆分
static void test_function(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, staval2 = NL829_PP60_FAIL, outlen = 0;

	/*process body*/
	//测试前置
	//case1:屏幕类
	cls_show_msg1(2, "屏幕类指令测试...");//加提示以刷掉测试菜单
	// 输出加密类型
	// cls_show_msg("设置的算法类型为: 0x%x", g_algtype);
	if((staval=PP60_cmd_C())!=NL829_PP60_SUCC)//顺便充当本case的测试前置
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(test_PP60_cmd_D()!=NL829_PP60_SUCC)
		goto ERR;
	PP60_cmd_C();//测试后置
	if(test_PP60_cmd_O()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_P()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_W()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_h()!=NL829_PP60_SUCC)
		goto ERR;

	//case2:密钥密码类
	//注意以下测试顺序的安排是前者为后者的前置的(不是相互独立的),随意变化测试顺序会引起测试不过
	//G命令不单独测试,与其它加密命令相互验证即可
	cls_show_msg1(2, "密钥密码类指令测试...");//加提示以刷掉测试菜单
	if((staval=PP60_cmd_Z())!=NL829_PP60_SUCC)//顺便充当本case的测试前置//注意本命令与"开机复位"的区别在于"开机复位"时还会默认激活第0组第0个ukey
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(test_PP60_cmd_R()!=NL829_PP60_SUCC)//顺便充当本case的测试前置//执行成功的话,会重置第0,167组主,用密钥
		goto ERR;
	if(test_PP60_cmd_MSU()!=NL829_PP60_SUCC)//顺便充当本case的测试前置
		goto ERR;
	//用于MAC等其它加解密运算(K,T,H)
	//if(test_PP60_cmd_K()!=NL829_PP60_SUCC)//K命令有密钥号域,故不必激活
	//	goto ERR;
#if TEST
	if((staval=PP60_cmd_A(0xa8, 0x00, g_algtype))!=NL829_PP60_FAIL||(staval1=PP60_cmd_A(0x00, 0x02, g_algtype))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
#endif
	if((staval=PP60_cmd_A(0xa7, 0x00, PP60_NODESFLAG))!=NL829_PP60_SUCC||(staval1=PP60_cmd_A(0xa7, 0x00, PP60_DESFLAG))!=NL829_PP60_SUCC||(staval2=PP60_cmd_A(0xa7, 0x00, PP60_TDESFLAG))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x,%04x)", __LINE__, staval, staval1, staval2);
		goto ERR;
	}	
	if((staval=PP60_cmd_A(0xa7, 0x00, g_algtype))!=NL829_PP60_SUCC)//以TDES方式激活第168组的00号工作密钥,另一个工作密钥也是全0x00
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(test_PP60_cmd_K()!=NL829_PP60_SUCC)//由于新增SM4国密算法,需A命令先激活后方可使用 20160303导入国密算法支持引起
		goto ERR;	
	if(test_PP60_cmd_T()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_H()!=NL829_PP60_SUCC)
		goto ERR;
	//用于PIN输入(L,N,E,F)
	if((staval=PP60_cmd_A(0x00, 0x00, g_algtype))!=NL829_PP60_SUCC)//ukey(00)改为全0x38//前置条件
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(test_PP60_cmd_LN()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_F()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_E()!=NL829_PP60_SUCC)
		goto ERR;
	//用于PINBLOCK的计算(I,J,X)
	if((staval=PP60_cmd_A(0x00, 0x01, g_algtype))!=NL829_PP60_SUCC)//ukey(01)改为全0x40//前置条件
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(test_PP60_cmd_I()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_X()!=NL829_PP60_SUCC)
		goto ERR;
	if(test_PP60_cmd_J()!=NL829_PP60_SUCC)
		goto ERR;
	
	//case3:其它
	cls_show_msg1(2, "其它类指令测试...");//加提示以刷掉测试菜单
	memset(out, 0, sizeof(out));//从不用根据outlen来截短
	if((staval=PP60_cmd_V(out, &outlen))!=NL829_PP60_SUCC||cls_show_msg("取到版本:%s,正确按[ENTER];否则按[其它]", out)!=ENTER)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if(test_PP60_cmd_x()!=NL829_PP60_SUCC)
		goto ERR;

	cls_show_msg("指令集测试通过");
	//return;
ERR://清理环境,恢复默认
	PP60_cmd_C();
	PP60_cmd_Z();
	PP60_cmd_R((uchar *)"\x41\x41", strlen("\x41\x41"));
	PP60_cmd_R((uchar *)"\x4b\x48", strlen("\x4b\x48"));
	return;
}
#endif

//设计思路:选择一个流程中不需要手工输入的,同时让PP60每次收到的命令是变长或变化的做压力测试
static void test_press_DCWAT(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0}, out1[PP60_MAXLEN_DATA] = {0}, D[64] = {0}, data[32] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL;
	int succ = 0, cnt = DEFAULT_CNT_VLE, bak = 0, outlen = 0, outlen1 = 0, Dlen = 0, ret = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uint len = 0, i=0;
	uchar tmp0[32], tmp1[32];
	DATA_RESULT result[ ]={{8, (uchar *)"\xfb\x04\xbe\xe7\x8f\x3a\x38\x53"}, //0 使用方法1  数据为21个字节的0x31 密钥为8个字节的0x00 
						   {8, (uchar *)"\x62\x2e\x2c\xd5\x2c\x3f\xdc\x83"}, //1 使用方法2  数据为21个字节的0x31 密钥为8个字节的0x00 
						   {16,(uchar *)"\xC7\xCE\x1F\x68\xD5\xC8\x4A\xF3\xB4\xBD\x6D\xDD\x07\x0C\x97\x37"}, //0+2 使用方法1  数据为21个字节的0x31 密钥为16个字节的0x00 
						   {16,(uchar *)"\x4C\xC0\xBB\xAE\x84\x0D\xE2\x16\x97\x84\x15\x9A\x31\xB8\x0F\x6F"}, //1+2 使用方法2  数据为21个字节的0x31 密钥为16个字节的0x00 
						  };
	//中国银联//244bytes原始数据
	uchar logo[] = {0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0X08,0X08,0X08,0X08,0XFF,0X08,
				0X08,0X08,0X08,0X0F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7F,
				0X40,0X48,0X49,0X49,0X49,0X4F,0X49,0X49,0X49,0X48,0X40,0X7F,0X00,0X00,0X00,0X00,0X00,
				0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X0C,0X34,0XE7,
				0X24,0X24,0X00,0X7F,0X49,0X49,0X49,0X49,0X7F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
				0X00,0X00,0X00,0X40,0X40,0X7F,0X48,0X48,0X7F,0X40,0X08,0X88,0X68,0X0F,0X08,0X28,0XC8,
				0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
				0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XE0,0X40,0X40,0X40,
				0X40,0XFF,0X40,0X40,0X40,0X40,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
				0X00,0X00,0XFF,0X02,0X12,0X12,0X12,0X12,0XF2,0X12,0X52,0X32,0X12,0X02,0XFF,0X00,0X00,
				0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,
				0X80,0X80,0XFE,0X84,0X88,0X80,0XFF,0X02,0XC4,0X30,0X18,0X24,0X46,0X04,0X00,0X00,0X00,
				0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X06,0XFC,0X88,0X88,0XFF,0X09,0X82,0X8C,0XB0,0XC0,
				0XB0,0X8C,0X83,0X82,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
				0X00,0X00,0X00,0X00,0X00,0X00,0XFF};//最后一个0XFF是错误的,正常使用时要把它去掉

	/*process body*/
	//测试前置
	outlen = PP60_code(logo, sizeof(logo), out);
	i=(g_algtype==PP60_TDESFLAG)?0:1;
	PP60_code(result[2*i].data, result[2*i].len, tmp0);
	PP60_code(result[1+2*i].data, result[1+2*i].len, tmp1);
	
	memset(data, 0x31, sizeof(data));
	Dlen = PP60_code(data, sizeof(data), D);
	if((staval=PP60_cmd_R((uchar *)"\x4b\x48", strlen("\x4b\x48")))!=NL829_PP60_SUCC)//重置第167组主,用密钥
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}

	//正式测试
	cls_printf("默认压力测试次数:\n");
	//if ((ret=getnumstr(CntStr, sizeof(CntStr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default
	if ((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);

	while(cnt)
	{
		cls_printf("压力测试中...\n还剩%d次(已成功%d次),请耐心等待", cnt, succ);
		cnt--;

		if((staval=PP60_cmd_D(rand()%2+1, (uchar *)"\x80\x81\x82\x83\x84\x31\x32\x33\x34", strlen("\x80\x81\x82\x83\x84\x31\x32\x33\x34")))!=NL829_PP60_SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次:测试失败(%04x)", __LINE__, bak-cnt, staval);
			continue;
		}
		if((staval=PP60_cmd_C())!=NL829_PP60_SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次:测试失败(%04x)", __LINE__, bak-cnt, staval);
			continue;
		}
#if TEST
		if((staval=PP60_cmd_W(0, 0, out, outlen-2))!=NL829_PP60_SUCC||(staval1=PP60_cmd_W(0, 1, out, outlen-2))!=NL829_PP60_SUCC)//连发两条大数据的W命令
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次:测试失败(%04x,%04x)", __LINE__, bak-cnt, staval, staval1);
			continue;
		}
#endif
		if((staval=PP60_cmd_A(/*rand()%0xa8*/0xa7, rand()%2, g_algtype))!=NL829_PP60_SUCC)//激活的密钥号不可随便改,目前写死成0xa7,因为后面的密码运算要以此为前置条件
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次:测试失败(%04x)", __LINE__, bak-cnt, staval);
			continue;
		}
#if TEST
		if((staval=PP60_cmd_T(0x00, D, 21*2, out1, (ushort *)&outlen1))!=NL829_PP60_SUCC||outlen1!=(result[2*i].len*2)||memcmp(out1, tmp0, outlen1))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次:测试失败(%04x,%d)", __LINE__, bak-cnt, staval, outlen1);
			continue;
		}
		if((staval=PP60_cmd_T(0x01, D, 21*2, out1, (ushort *)&outlen1))!=NL829_PP60_SUCC||outlen1!=(result[1+2*i].len*2)||memcmp(out1, tmp1, outlen1))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次:测试失败(%04x,%d)", __LINE__, bak-cnt, staval, outlen1);
			continue;
		}
#endif
		
		succ++;
	}

	cls_show_msg("DCWAT命令压力测试完成,已执行次数为%d,成功%d次", bak-cnt, succ);
	return;
}

static void test_press_A(void)
{
	/*private & local definition*/
	ushort staval = NL829_PP60_FAIL;
	int succ = 0, cnt = DEFAULT_CNT_VLE, bak = 0, ret = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uint len = 0;

	/*process body*/
	//正式测试
	cls_printf("默认压力测试次数:\n");
	//if ((ret=getnumstr(CntStr, sizeof(CntStr)-1, 2, 0))==QUIT||ret==TIMEOUT)//按取消,则使用default
	if ((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &len, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);

	while(cnt)
	{
		cls_printf("压力测试中...\n还剩%d次(已成功%d次),请耐心等待", cnt, succ);
		cnt--;

		if((staval=PP60_cmd_A(rand()%0xa8, rand()%2, g_algtype))!=NL829_PP60_SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次:测试失败(%04x)", __LINE__, bak-cnt, staval);
			continue;
		}
		succ++;
	}

	cls_show_msg("A命令压力测试完成,已执行次数为%d,成功%d次", bak-cnt, succ);
	return;
}

static void test_press(void)
{
	/*private & local definition*/
	int keyin = 0;
	PF func[] = {test_press_DCWAT, test_press_A}; 

	/*process body*/
	while(1)
       {
		keyin = cls_show_msg("1.深度压力\n"
							"2.快速压力");//A命令在低BPS下更容易快速测出问题;深度压力则进行较大数据量的多条命令混合,在不同的命令里也会出错(但会久一些才出错).从原因上分析,每条命令都会出错的,只是短命令出错效率更高而已(并非其它命令就测不出问题的)

		switch(keyin)
		{
		case '1':
		case '2':	
			func[keyin-'1']();
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

//可根据测试需求,进行扩展
static void test_ability(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0}, str[PP60_CMDK_SM4_STRLEN] = {0}/*str其实只要8字节即可,这里蛮开大些*/, STR[PP60_CMDK_SM4_STRLEN*2] = {0}, key[16] = {0};
	ushort staval = NL829_PP60_FAIL, outlen = 0;
	int STRlen = 0, data_len = 0;
	float time = 0;
	
	/*process body*/
	cls_printf("性能测试...");
	data_len=(g_algtype==PP60_TDESFLAG)?PP60_CMDK_STRLEN:PP60_CMDK_SM4_STRLEN;

	// 1.密码运算
	if((staval=PP60_cmd_R((uchar *)"\x4b\x48", strlen("\x4b\x48")))!=NL829_PP60_SUCC)//重置第167组主,用密钥
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}

	memset(key, 0x38, sizeof(key));
	memset(str, 0x31, data_len);
	STRlen = PP60_code(str, data_len, STR);

	lib_StartStopwatch();
	if((staval=PP60_cmd_A(0xa7, 0x00, g_algtype))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval);
		return;
	}
	if((staval=PP60_cmd_K(0xa7, STR, data_len, out, &outlen))!=NL829_PP60_SUCC)//K命令有密钥号域,故不必激活
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	else//进一步校验
	{
		time = lib_ReadStopwatch();
		//DEBUG_HEX(out, outlen, NULL);
		if((STRlen=PP60_decode(out, outlen, STR))!= data_len/2)
		{
			cls_show_msg("line %d:测试失败(%d)", __LINE__, STRlen);
			return;		
		}
		//if(NDK_AlgTDes(str, out, key, sizeof(key), ALG_TDS_MODE_DEC)!=NDK_OK)//if(mdes3(DISCRYPT, str, out, key)!=SUCC)
		if(AlgCompute(g_algtype, str, data_len/2 , out, key, sizeof(key), ((g_algtype==PP60_TDESFLAG)?ALG_TDS_MODE_DEC:ALG_SM4_DECRYPT_ECB) )!=NDK_OK)//OK已无用了,用于作中间缓冲
		{
			cls_show_msg("line %d:测试失败", __LINE__);
			return;
		}
		if(memcmp(out, STR, STRlen))
		{
			//DEBUG_HEX(out, STRlen, NULL);
			//DEBUG_HEX(STR, STRlen, NULL);
			cls_show_msg("line %d:测试失败", __LINE__);
			return;
		}
	}
	if(g_algtype==PP60_TDESFLAG)
		cls_show_msg("进行DES运算加(解)密运算耗时:%fs", time);
	else
		cls_show_msg("进行SM4运算加(解)密运算耗时:%fs", time);
	// 2.取版本
	memset(out, 0, sizeof(out));//从不用根据outlen来截短
	lib_StartStopwatch();
	if((staval=PP60_cmd_V(out, &outlen))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		return;
	}
	time = lib_ReadStopwatch();
	cls_show_msg("取到版本:%s(耗时:%fs)", out, time);
}

//本测试函数不单进行异常测试,还有一些不放在功能测试中的非常规测试(比如耗时较多的测试)也放在此处
static void test_abnormity(void)
{
	/*private & local definition*/
	uchar out[PP60_MAXLEN_DATA] = {0}, out1[PP60_MAXLEN_DATA] = {0}, frame_cmd[NL829_MAXLEN_FRAME] = {0}, OK[16] = {0}, NK[16] = {0};
	ushort staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL, outlen = 0, outlen1 = 0;
	int len = 0, subcase = 0;
	
	/*process body*/
	cls_show_msg1(2, "异常命令测试...");

	// 1.不支持的命令,如Q命令
	if((staval=PP60_cmd_Q())!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg1(2, "%d#子用例测试通过!", ++subcase);//屏幕有变化,以区别于之后的屏幕提示

	// 2.不识别的命令
	if((staval=PP60_send_frame((uchar *)"1234567", strlen("1234567")))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=PP60_recv_frame(1, out, &outlen))==NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg1(2, "%d#子用例测试通过!", ++subcase);//屏幕有变化,以区别于之后的屏幕提示

	// 3.正常命令的头前,中部,尾后有干扰数据,以C命令为例
	//中间干扰
	if((staval=PP60_send_frame((uchar *)"\x1b\x43\x0a\x0d\x0a", strlen("\x1b\x43\x0a\x0d\x0a")))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=PP60_recv_frame(PP60_CMDC_RCVLEN, out, &outlen))!=NL829_PP60_SUCC||out[0]!=PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, out[0]);
		goto ERR;
	}
	//头部干扰(非1b开头)
	if((staval=PP60_send_frame((uchar *)"\x0a\x1b\x43\x0d\x0a", strlen("\x0a\x1b\x43\x0d\x0a")))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=PP60_recv_frame(PP60_CMDC_RCVLEN, out, &outlen))!=NL829_PP60_SUCC||out[0]!=PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, out[0]);
		goto ERR;
	}
	//尾部干扰
	if((staval=PP60_send_frame((uchar *)"\x1b\x43\x0d\x0a\x0a", strlen("\x1b\x43\x0d\x0a\x0a")))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=PP60_recv_frame(PP60_CMDC_RCVLEN, out, &outlen))!=NL829_PP60_SUCC||out[0]!=PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x, %02x)", __LINE__, staval, out[0]);
		goto ERR;
	}
	cls_show_msg1(2, "%d#子用例测试通过!", ++subcase);//屏幕有变化,以区别于之后的屏幕提示

	//以下测试的前置条件
	g_adjust = PP60_TIMEOUT_MAX;

	// 4.所有的key全部重置
	cls_printf("重置PP60所有key中,请耐心等待...");
	if((staval=PP60_cmd_R(NULL, 0))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_printf("重置成功,验证中...");
	//验证mkey置为了全0x38,ukey置为了全0x30
	memset(OK, 0x38, sizeof(OK));
	memset(NK, 0x55/*底层认为0x39与0x38是一样的key*/, sizeof(NK));
	outlen = PP60_code(OK, sizeof(OK), out);
	outlen1= PP60_code(NK, sizeof(NK), out1);
	if((staval=PP60_cmd_M(0x03, out, outlen, out1, outlen1))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	if((staval=PP60_cmd_U(0x03, 0x00, out1, outlen1, out1, outlen1))!=NL829_PP60_SUCC||(staval1=PP60_cmd_U(0x03, 0x01, out1, outlen1, out1, outlen1))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	cls_show_msg1(2, "%d#子用例测试通过!", ++subcase);//屏幕有变化,以区别于之后的屏幕提示

	// 5.输入不足位的PIN+[ENTER],应超时返回0x55
	if((staval=PP60_cmd_L(0x1e))!=NL829_PP60_SUCC||(staval1=PP60_cmd_N(0x10))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	cls_printf("请在PP60上输入1234+[ENTER]后不再按键,请耐心等待响应...");//不再按键以免PP60重新计算超时
	if((staval=PP60_cmd_E(0x02))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg1(2, "%d#子用例测试通过!", ++subcase);//屏幕有变化,以区别于之后的屏幕提示

	// 6.最大长度小于最小长度时,无法输入确认,应超时返回0x55
	if((staval=PP60_cmd_L(0x01))!=NL829_PP60_SUCC||(staval1=PP60_cmd_N(0x1e))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x,%04x)", __LINE__, staval, staval1);
		goto ERR;
	}
	cls_printf("请在PP60上输入1+[ENTER]后不再按键,请耐心等待响应...");
	if((staval=PP60_cmd_E(0x02))!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	cls_show_msg1(2, "%d#子用例测试通过!", ++subcase);//屏幕有变化,以区别于之后的屏幕提示
	
	// 7.最大长度小于最小长度时,无法输入确认,再用一条命令来让PP60退出等待超时态,这条命令是作废的,PP60不会应答
	//cls_printf("请在PP60上输入1+[ENTER]...");
	//组包
	len = PP60_pack_data('E', (uchar *)"2", 1, frame_cmd);
	//发包
	if((staval=PP60_send_frame(frame_cmd, len))!=NL829_PP60_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}
	//不接收正常的返回,而用另一个发包来中断设备的接收
	if((staval=PP60_cmd_C())!=NL829_PP60_FAIL)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, staval);
		goto ERR;
	}

	cls_show_msg("异常测试通过");
ERR://清理环境,恢复默认
	g_adjust = 0;
	PP60_cmd_Z();
	PP60_cmd_R((uchar *)"\x41\x44", strlen("\x41\x44"));
	return;
}

static int select_PP60(void)
{
	/*private & local definition*/
	int nKeyin = 0;
	int type[] = {DEVICE_PP60PT, DEVICE_PP60RF, DEVICE_PP60RF};

	/*process body*/
	while (1)
	{
		nKeyin = cls_show_msg("_选择PP60类型_\n"
							"1.普通PP60\n"
							"2.PP60RF\n"
							"3.大屏PP60RF\n");
		switch(nKeyin)
		{
		case '1':
		case '2':
			g_LCDSize= 0;
			return type[nKeyin-'1'];
			break;
		case '3':
			g_LCDSize = 1;//大屏密码键盘开关
			return type[nKeyin-'1'];
			break;
		case ESC:
			g_LCDSize = 0;
			cls_show_msg1(2, "将设置为普通型(默认)...");
			return DEVICE_PP60PT;
			break;
		default:
			continue;
			break;
		}
	}
}

static int select_PP60_ALGORITHM(void)
{
	/*private & local definition */
	int nKeyin = 0;
	int alg_select[] = {PP60_TDESFLAG, PP60_SM4FLAG};
	/* process body */
	while(1)
	{
		nKeyin = cls_show_msg("选择PP60加密算法类型\n"
							"1.DES算法\n"
							"2.SM4算法\n");
		switch(nKeyin)
		{
			case '1':
			case '2':
				return alg_select[nKeyin-'1'];
				break;
			case ESC:
				cls_show_msg1(2, "加密算法类型将设置为DES...");
				return PP60_TDESFLAG;
				break;
			default:
				continue;
				break;
		}
	}
}
			
static int conf_PP60PT_bps(void)
{
	/*private & local definition*/
	int nKeyin = 0, loop = 0;
	//int bps[] = {BPS1200, BPS2400, BPS4800, BPS9600, /*BPS14400*/BPS9600, BPS19200, BPS38400, BPS57600};//注意POS上并不提供BPS14400,故PED不要设置为这个BPS
	char *bps[] = {"1200",  "2400", "4800", "9600", "9600", "19200", "38400", "57600"};
	int bpsid=0;
	
	/*process body*/
	// 1.配置与NL829连接的POS串口参数
	cls_show_msg1(2, "将进行连接外设串口的参数配置");
	g_devicecom = select_serial();
	if(cls_show_msg("请将POS串口bps设置为与PP60一致,[ENTER]继续[其它]退出")==ENTER)
	{
		strcpy(g_devicecombps, conf_serial(&bpsid));//g_devicecombps = conf_serial();
		if(NDK_PortOpen(g_devicecom, g_devicecombps)!=NDK_OK)//if(initaux(g_devicecom, g_devicecombps, DB8|STOP1|NP)!=SUCC)
		{
			cls_show_msg1(2, "line %d:初始化POS串口失败", __LINE__);
			return FAIL;
		}
		return SUCC;
	}
	else
		return FAIL;

	// 2.配置NL829串口参数
	if(cls_show_msg("重新设置PP60PT BPS吗?\n"
					"[确认]是[其它]否\n")!=ENTER)
		return SUCC;
	
	do
	{
		nKeyin = cls_show_msg("1.1200   2.2400\n3.4800   4.9600\n5.14400  6.19200\n7.38400  8.57600");
		switch(nKeyin)
		{
		case ESC:
		case '5':
			cls_show_msg1(2, "不支持BPS14400,将设置为BPS9600(默认)...");
			nKeyin = '4';//转义
		case '1':
		case '2':
		case '3':
		case '4':	
		case '6':
		case '7':
		case '8':
			cls_printf("设置外设BPS中...");
			if(PP60_cmd_B(nKeyin-'0')!=NL829_PP60_SUCC)//使用大B,以保证下电后该值仍有效
			{
				cls_printf("外设BPS自动适配中...请耐心等待");
				//若不成功,对所有可能的BPS进行尝试
				for(loop=0; loop<SZ_ARRAY(bps); loop++)
				{
					NDK_PortOpen(g_devicecom, bps[loop]);//initaux(g_devicecom, bps[loop], DB8|STOP1|NP);
					if(PP60_cmd_B(nKeyin-'1')==NL829_PP60_SUCC)
						break;
				}
				if(loop==SZ_ARRAY(bps))
				{
					cls_show_msg1(2, "line %d:重新设置PP60PT BPS失败,请重启后再尝试", __LINE__);
					return FAIL;
				}
			}
			break;
		default:
			cls_show_msg1(1, "若要退出,请按ESC...");
			continue;
			break;
		}
	}while(nKeyin<'1'||nKeyin>'8');

	// 3.根据NL829当前BPS调整POS串口的BPS
	if(NDK_PortOpen(g_devicecom, bps[nKeyin-'1'])!=NDK_OK)//if(initaux(g_devicecom, bps[nKeyin-'1'], DB8|STOP1|NP)!=SUCC)
	{
		cls_show_msg1(2, "line %d:初始化POS串口失败", __LINE__);
		return FAIL;
	}
	strcpy(g_devicecombps, bps[nKeyin-'1']);//g_devicecombps = bps[nKeyin-'1'];//g_bps根据配置进行调整备份
	return SUCC;
}

static void conf_PP60(void)
{
	/*private & local definition*/
	ushort staval = NL829_PP60_FAIL;

	/*process body*/
	// 1.配置PP60实际的类型(PT or RF)
	g_PP60type = select_PP60();

	// 2.配置PP60加密算法(SM4、3DES or DES)
	g_algtype = select_PP60_ALGORITHM();

	// 3.根据实际类型进行相应配置
	cls_show_msg("请确保POS与外设已连接!任意键继续...");
	if((g_PP60type==DEVICE_PP60PT)?conf_PP60PT_bps():conf_NL829_bps()!=SUCC)
		cls_show_msg("line %d:配置失败!请重启POS和外设后重新配置.", __LINE__);
	else
	{
		if(g_PP60type==DEVICE_PP60PT)
			cls_show_msg1(2, "链路配置成功!");
		else//对于邋PP60RF,进一步做连通测试
		{
			sleep(2);//20120221add
			staval = NL829_cnnt_PP60();
			cls_show_msg1(2, "链路配置%s(%04x)!", staval==NL829_PP60_SUCC?"成功":"失败", staval);
		}
	}
}

/****************************************************************
* function name 	 	:
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  		  jennywone163 	20110819		created
*****************************************************************/
void systest35(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	cls_show_msg("%s测试中...请先进行配置,按任意键开始\n", TESTITEM);
	g_PP60type = DEVICE_PP60PT;

	while(1)
       {
		keyin = cls_show_msg("1.功能 2.压力\n"
							"3.性能 4.异常\n"
							"0.配置");

		switch(keyin)
		{
		case '1':
			test_function();
			break;
		case '2'://压力测试一方面是测试功能的压力,另一方面也是串口通讯的压力
			if(ENTER!=cls_show_msg("压力测试(本项测试不接受ESC中断测试),ENTER键继续..."))
				break;			
			test_press();
			break;
		case '3':
			test_ability();
			break;
		case '4':
			test_abnormity();
			break;
		case '9'://后门,用于删除SN
			{
				uchar out[PP60_MAXLEN_DATA] = {0};
				ushort /*staval = NL829_PP60_FAIL, staval1 = NL829_PP60_FAIL,*/ outlen = 0;
				PP60_cmd_x(3, NULL, 0, out, &outlen);
			}
			break;
		case '0'://不同的BPS会让测试现象(如,压力测试)不一样,所以除默认工作BPS外,最高最低的BPS也要测试一下
			conf_PP60();
			break;
		case ESC:
			cls_show_msg("请配置其它BPS重新测试!任意键继续...");
			if(g_PP60type==DEVICE_PP60RF)
				cls_show_msg("请另行进行射频部分的测试!");	
			return;
			break;
		default:
			continue;
			break;
		}
	}
	
	return;
}

