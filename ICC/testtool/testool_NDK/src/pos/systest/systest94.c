/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 综合模块
* file name			: 94.c
* Author 			: 
* version			: V1.0
* DATE				:
* directory 			: 
* description			: SP100测试
* related document	: SP100_Newland External_Pinpad_Specifications_02_12.doc文档
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
#define	TESTITEM	"SP100密码键盘"

#define	MAXWAITTIME  135  //135s
#define    MAXCNT		1000
#define	LOGOBMP	"abclogo.bmp"

/*------------global variables definition-----------------------*/
#define IRAN_CUSTOM  0   //伊朗定制版本需要把此值改成1
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
*			  		  jiangym 		   	2018/01/12		created
*****************************************************************/
static int select_port(void)
{
	/*private & local definition*/
	int nKeyin = 0, port = 0;

	/*process body*/
	while(1)
	{
		nKeyin = cls_show_msg("请选择连接外设的串口:\n①串口1\n②串口2\n③USB串口\n");
		switch (nKeyin)
		{
		case '1':
			port = PORT_NUM_COM1;
			break;
		case '2':
			port = PORT_NUM_COM2;
			break;
		case '3':
			port = PORT_NUM_USB_HOST;
			break;
		default:
			continue;
			break;
		}
		return port;
	}
}

//获取功能按键及响应
static ushort SP100_Option_Select(uchar *display, int dislen, uchar *funkey)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_OPTION_SELECT_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN,display,dislen);
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+dislen, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_OPTION_SELECT_RESPONSE,2) && appendlen == 2 &&  append[0]==SP100_SEPERATOR_CMD )
	{
		*funkey = append[1];
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_OPTION_SELECT;	
	}
}

//PIN输入请求及响应 
static ushort SP100_Pin_Entry(uchar id, uchar type, uchar acid, uchar actype,uchar aclen, uchar *pan, uchar *textpan, int textpanlen, uchar encmode, uchar enclen, uchar digist, uchar keyenter, uchar timeout, uchar isoformat,uchar *display, int dislen, uchar *respcode, uchar *fuckey, uchar *pinlen, uchar *pinblock, uchar *ksn)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_PIN_ENTRY_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = id;
	data[4] = type;
	data[5] = acid;
	if(acid==0x00) //0是明文安装
	{
		memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+3,textpan,textpanlen);
		data[textpanlen+6] = 0x1C;
		data[textpanlen+7] = encmode;
		data[textpanlen+8] = enclen;
		data[textpanlen+9] = digist;
		data[textpanlen+10] = keyenter;
		data[textpanlen+11] = timeout;
		data[textpanlen+12] = isoformat;
		memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+textpanlen+10,display,dislen);
		if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+10+dislen+textpanlen, answer, append, &appendlen, 0))!=SP100_SUCC)
			return ret;
	}
	else  //密文安装
	{
		data[6] =actype;
		data[7] =aclen;
		memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+5,pan,16);
		data[24] = encmode;
		data[25] = enclen;
		data[26] = digist;
		data[27] = keyenter;
		data[28] = timeout;
		data[29] = isoformat;
		memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+27,display,dislen);
		if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+27+dislen, answer, append, &appendlen, 0))!=SP100_SUCC)
			return ret;	
	}
	if(!memcmp(answer,SP100_PIN_ENTRY_RESPONSE,2) &&  append[0]==SP100_SEPERATOR_CMD)
	{
		memcpy(respcode,append+1,2);
		*fuckey = append[3];
		*pinlen = append[4];
		if(type == 0x02)
			memcpy(pinblock,append+5,16);
		else if(type == 0x01)
		{
			memcpy(pinblock,append+5,8);
			memcpy(ksn,append+13,10);
		}
		else
			memcpy(pinblock,append+5,8);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_PIN_ENTRY;	
	}
}

//获取DUKPT KSN请求及响应
static ushort SP100_Get_Dukpt_Ksn(uchar keyid, uchar *ksn)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_GET_DUKPT_KSN_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = keyid;
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+1, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_GET_DUKPT_KSN_RESPONSE,2) && appendlen == 11 &&  append[0]==SP100_SEPERATOR_CMD )
	{
		memcpy(ksn,append+1,10);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_GET_DUKPT_KSN;	
	}
}

#if 0  //不支持 只是个测试命令
//DUKPT PIN输入请求及响应 
static ushort SP100_Dukpt_Pin_Entry(uchar keyid, uchar *pan, uchar *keynum, uchar *pinblock)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_DUKPT_PIN_ENTRY_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = keyid;
	memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+1,pan,19);
	memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+20,"\1C",1);
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+21, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_DUKPT_PIN_ENTRY_RESPONSE,2)  &&  append[0]==SP100_SEPERATOR_CMD )
	{
		
		memcpy(keynum,append+1,(appendlen-11));
		memcpy(pinblock,append+(appendlen-17),10);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_DUKPT_PIN_ENTRY;	
	}
}
#endif

//PIN输入取消请求及响应
static ushort SP100_Cancel_Pin_Entry(void)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, answer[1] = {0};
	
	/*process body*/
	memcpy(data,SP100_CANCEL_PIN_ENTRY_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	if((ret=SP100_frame_factory_ACK(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN, answer, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_ACK_RESPONSE,1) && appendlen == 1) 
		return SP100_SUCC;
	else
	{
		return SP100_ERR_CANCEL_PIN_ENTRY;	
	}
}

//蜂鸣器控制请求及响应
static ushort SP100_Active_Buzzer(uchar tone,uchar *dura)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, answer[1] = {0};
	
	/*process body*/
	memcpy(data,SP100_ACTIVE_BUZZER_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = tone;
	memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+1,dura,2);
	if((ret=SP100_frame_factory_ACK(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+3, answer, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_ACK_RESPONSE,1) && appendlen == 1)
		return SP100_SUCC;
	else
	{
		return SP100_ERR_ACTIVE_BUZZER;	
	}
}

//显示文本请求及响应
static ushort SP100_Display_Text(uchar *display, int dislen)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, answer[1] = {0};
	
	/*process body*/
	memcpy(data,SP100_DISPLAY_TEXT_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN,display,dislen);
	if((ret=SP100_frame_factory_ACK(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+dislen, answer, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_ACK_RESPONSE,1) && appendlen == 1)
		return SP100_SUCC;
	else
	{
		return SP100_ERR_DISPLAY_TEXT;	
	}
}

//显示图片请求及响应
static ushort SP100_Display_Image(uchar id, uchar *x, uchar *y, uchar * len, uchar *imgdata, int imglen)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, answer[1] = {0};
	
	/*process body*/
	memcpy(data,SP100_DISPLAY_IMAGE_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = id;
	memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+1,x,2);
	memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+3,y,2);
	if(data[3] == 0x00)
	{
		memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+5,len,2);
		memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+7,imgdata,imglen);
		
		if((ret=SP100_frame_factory_ACK(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+7+imglen, answer, &appendlen, 0))!=SP100_SUCC)
			return ret; 
	}
	else
	{
		if((ret=SP100_frame_factory_ACK(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+5, answer, &appendlen, 0))!=SP100_SUCC)
			return ret;	
	}
	if(!memcmp(answer,SP100_ACK_RESPONSE,1) && appendlen == 1)
		return SP100_SUCC;
	else
	{
		return SP100_ERR_LOAD_IMAGE;	
	}
}

//加载图片请求及响应 
static ushort SP100_Load_Image(uchar imgid, uchar *imglen, uchar *imgdata, int datalen, uchar *respcode)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_LOAD_IMAGE_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = imgid;
	memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+1,imglen,2);
	memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+3,imgdata,datalen);
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+3+datalen, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_LOAD_IMAGE_RESPONSE,2) && appendlen == 3  && append[0]==SP100_SEPERATOR_CMD )
	{
		memcpy(respcode,append+1,2);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_LOAD_IMAGE;	
	}
}

//安装密钥请求及响应 
static ushort SP100_Load_Key_Block(uchar id, uchar format, uchar keylen, uchar type, uchar keyid, uchar *bllen, uchar *bldata, int datalen, uchar kcvtype, uchar *kcv, uchar *respcode)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_LOAD_KEY_BLOCK_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = id;
	data[4] = format;
	if(format  == 0x01)//AES 格式
	{
		data[5] = keylen;
		data[6] = type;
		data[7] = keyid;
		memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+5,bllen,2);
		memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+7,bldata,datalen);
		if(kcvtype!=0x00)
		{	
			memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+7+datalen,kcv,3);
			if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+10+datalen, answer, append, &appendlen, 0))!=SP100_SUCC)
				return ret; 
		}
		else
		{
			if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+7+datalen, answer, append, &appendlen, 0))!=SP100_SUCC)
				return ret; 
		}
	}
	else// TR31格式或者DES密文安装
	{
		data[5] = type;
		data[6] = keyid;
		memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+4,bllen,2);
		memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+6,bldata,datalen);
		if(kcvtype!=0)
		{
			memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN+6+datalen,kcv,3);
			if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+9+datalen, answer, append, &appendlen, 0))!=SP100_SUCC)
				return ret; 
		}
		else
		{
			if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+6+datalen, answer, append, &appendlen, 0))!=SP100_SUCC)
				return ret; 
		}
	}
	if(!memcmp(answer,SP100_LOAD_KEY_BLOCK_RESPONSE,2) && appendlen == 3  &&  append[0]==SP100_SEPERATOR_CMD )
	{
		memcpy(respcode,append+1,2);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_LOAD_IMAGE;	
	}
}

//获取密钥kcv校验请求及响应 
static ushort SP100_Get_Key_Check(uchar id, uchar keytype, uchar *keyid, uchar *keyvalue)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_GET_KEY_CHECK_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = id;
	data[4] = keytype;
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+2, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_GET_KEY_CHECK_RESPONSE,2) && appendlen == 5  &&  append[0]==SP100_SEPERATOR_CMD)
	{
		*keyid = append[1];
		memcpy(keyvalue,append+2,3);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_GET_KEY_CHECK;	
	}
}

//获取SN码请求及响应 
static ushort SP100_Get_Serial_Num(uchar *senum ) 
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_GET_SERIAL_NUM_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	memcpy(data+SP100_LEN_CMD+SP100_SEPERATOR_LEN,senum,16);
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+16, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_GET_SERIAL_NUM_RESPONSE,2) && appendlen == 17  &&  append[0]==SP100_SEPERATOR_CMD )
	{
		memcpy(senum,append+1,16);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_GET_SERIAL_NUM;	
	}
}

//数据加解密请求及响应 
static ushort SP100_Data_Enc_Dec(uchar id, uchar mode, uchar *inlen, uchar *indata, int datalen, uchar keymode, uchar keylen, uchar *keyid, uchar *resp, uchar *encdatalen, uchar *encdata)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_DATA_ENC_DEC_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = id;
	data[4] = mode;
	memcpy(data+5,inlen,2);
	memcpy(data+7,indata,datalen);
	data[7+datalen] = keymode;
	data[8+datalen] = keylen;
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+6+datalen, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_DATA_ENC_DEC_RESPONSE,2) &&  append[0]==SP100_SEPERATOR_CMD )
	{
		*keyid = append[1];
		memcpy(resp,append+2,2);
		//加解密datalen   data   
		memcpy(encdatalen,append+4,2);
		memcpy(encdata,append+6,appendlen-6);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_DATA_ENC_DEC;	
	}
}

//mac计算请求及响应 
static ushort SP100_Mac_Generation(uchar id, uchar type, uchar mode, uchar flag, uchar *inlen, uchar *indata, int datalen, uchar keymode, uchar keylen, uchar *keyid, uchar *resp, uchar *macdata, uchar *ksn)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_MAC_GENERATION_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = id;
	data[4] = type;
	data[5] = mode;
	data[6] = flag;
	memcpy(data+7,inlen,2);
	memcpy(data+9,indata,datalen);
	data[9+datalen] = keymode;
	data[10+datalen] = keylen;
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+8+datalen, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_MAC_GENERATION_RESPONSE,2) &&  append[0]==SP100_SEPERATOR_CMD )
	{
		*keyid = append[1];
		memcpy(resp,append+2,2);
		if(type == 0x00) //DES
			memcpy(macdata,append+4,8);
		else if(type == 0x01)//DUKPT
		{
			memcpy(macdata,append+4,8);
			memcpy(ksn,append+12,10);
		}
		else  //AES
			memcpy(macdata,append+4,16);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_MAC_GENERATION;	
	}
}

//设备硬件属性请求及响应
static ushort SP100_Device_Attribute(uchar *feature, uchar *fwid)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_DEVICE_ATTRIBUTE_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN,  answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_DEVICE_ATTRIBUTE_RESPONSE,2)  &&  append[0]==SP100_SEPERATOR_CMD )
	{
		memcpy(feature,append+1,2);
		memcpy(fwid,append+1,appendlen-3);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_DEVICE_ATTRIBUTE;	
	}
}

//加载配置请求及响应 
static ushort SP100_Config_Load(uchar *rate, uchar mode, uchar beepctl, uchar *resp)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_CONFIG_LOAD_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	memcpy(data+3,rate,2);
	data[5] = mode;
	data[6] = beepctl;
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+4, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_CONFIG_LOAD_RESPONSE,2) && appendlen == 3  &&  append[0]==SP100_SEPERATOR_CMD)
	{
		memcpy(resp,append+1,2);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_CONFIG_LOAD;	
	}
}

//获取配置请求及响应 
static ushort SP100_Get_Config(uchar *rate, uchar *mode, uchar *beepctl)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_GET_CONFIG_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_GET_CONFIG_RESPONSE,2) && appendlen == 5  &&  append[0]==SP100_SEPERATOR_CMD)
	{
		memcpy(rate,append+1,2);
		*mode = append[3];
		*beepctl = append[4];
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_CONFIG_LOAD;	
	}
}

//AES计算请求及响应 
static ushort SP100_AES_Enc_Dec(uchar id, uchar mode, uchar *inlen, uchar *indata, int datalen, uchar keylen, uchar *keyid, uchar *resp, uchar *encdatalen, uchar *encdata)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_AES_ENC_DEC_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = id;
	data[4] = mode;
	memcpy(data+5,inlen,2);
	memcpy(data+7,indata,datalen);
	data[7+datalen] = keylen;
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+5+datalen, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_AES_ENC_DEC_RESPONSE,2) &&  append[0]==SP100_SEPERATOR_CMD )
	{
		*keyid = append[1];
		memcpy(resp,append+2,2);
		//加解密datalen   data   
		memcpy(encdatalen,append+4,2);
		memcpy(encdata,append+6,appendlen-6);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_MAC_GENERATION;	
	}
}

//EMV相关指令
//配置命令:初始化和检查状态
static ushort SP100_Init_Reader(uchar *respcode)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_SMART_CARD_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = SP100_INIT_READER; 
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+SP100_FUNCTION_ID_LEN, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_SMART_CARD_RESPONSE,2) && appendlen == 4  &&  append[0]==SP100_SEPERATOR_CMD && append[1]==SP100_INIT_READER)
	{
		memcpy(respcode,append+2,2);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_INIT_READER;	
	}
}

//检查读卡器
static ushort SP100_Check_Reader(uchar *respcode, uchar *respdata)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_SMART_CARD_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = SP100_CHECK_READER; 
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+SP100_FUNCTION_ID_LEN, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_SMART_CARD_RESPONSE,2) && appendlen == 20  &&  append[0]==SP100_SEPERATOR_CMD && append[1]==SP100_CHECK_READER)
	{
		memcpy(respcode,append+2,2);
		memcpy(respdata,append+4,16);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_CHECK_READER;	
	}
}

//交易处理命令:开始交易,交易处理, 结束交易
static ushort SP100_Start_Transaction(uchar *cmddata, int cmdlen, uchar *respcode, uchar *status)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_SMART_CARD_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = SP100_START_TRANS; 
	memcpy(data+4,cmddata,cmdlen);
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+SP100_FUNCTION_ID_LEN+cmdlen, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_SMART_CARD_RESPONSE,2) && appendlen == 5 && append[0]==SP100_SEPERATOR_CMD && append[1]==SP100_START_TRANS)
	{
		memcpy(respcode,append+2,2);
		*status = append[4];
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_START_TRANS;	
	}
}

static ushort SP100_Process_Transaction(uchar *timeout, uchar UIctl, uchar *respcode, uchar *status)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_SMART_CARD_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = SP100_PROCESS_TRANS; 
	memcpy(data+4,timeout,2);
	data[6] = UIctl;
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+SP100_FUNCTION_ID_LEN+3, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_SMART_CARD_RESPONSE,2) &&  append[0]==SP100_SEPERATOR_CMD && append[1]==SP100_PROCESS_TRANS)
	{
		memcpy(respcode,append+2,2);
		*status = append[4];
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_PROCESS_TRANS;	
	}
}

static ushort SP100_Stop_Transaction(uchar UIctl, uchar *respcode)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_SMART_CARD_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = SP100_STOP_TRANS; 
	data[4] = UIctl;
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+SP100_FUNCTION_ID_LEN+1, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_SMART_CARD_RESPONSE,2) && appendlen == 4  &&  append[0]==SP100_SEPERATOR_CMD && append[1]==SP100_STOP_TRANS)
	{
		memcpy(respcode,append+2,2);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_STOP_TRANS;	
	}
}

//非接卡相关指令:激活  检测是否有卡片   APDU交互   关闭场强
//激活
static ushort SP100_Activate_Field(uchar *respcode)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_SMART_CARD_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = SP100_ACTIVATE_FIELD; 
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+SP100_FUNCTION_ID_LEN, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_SMART_CARD_RESPONSE,2) && appendlen == 4  &&  append[0]==SP100_SEPERATOR_CMD && append[1]==SP100_ACTIVATE_FIELD)
	{
		memcpy(respcode,append+2,2);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_ACTIVATE_FIELD;	
	}
}

//检测卡片是否存在
static ushort SP100_Check_Card(uchar *timeout, uchar *respcode)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_SMART_CARD_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = SP100_CHECK_CARD; 
	memcpy(data+4,timeout,2);
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+SP100_FUNCTION_ID_LEN+2, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_SMART_CARD_RESPONSE,2) && appendlen == 4  &&  append[0]==SP100_SEPERATOR_CMD  && append[1]==SP100_CHECK_CARD)
	{
		memcpy(respcode,append+2,2);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_CHECK_CARD;	
	}
}

//APDU交互(密文)
static ushort SP100_Exchange_APDU(uchar keyid, uchar *cmdlen, uchar *cmddata, int datalen, uchar *respcode, uchar *resplen, uchar *respdata)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_SMART_CARD_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = SP100_EXCHANGE_APDU; 
	data[4] = keyid;
	memcpy(data+5,cmdlen,2);
	memcpy(data+7,cmddata,datalen);
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+SP100_FUNCTION_ID_LEN+3+datalen, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_SMART_CARD_RESPONSE,2) &&  append[0]==SP100_SEPERATOR_CMD && append[1]==SP100_EXCHANGE_APDU)
	{
		memcpy(respcode,append+2,2);
		memcpy(resplen,append+4,2);
		memcpy(respdata,append+6,appendlen-6);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_EXCHANGE_APDU;	
	}
}

//伊朗定制版本新增:APDU交互(明文)
static ushort SP100_Exchange_APDU_Plaintext(uchar *cmddata, int datalen, uchar *respcode, uchar *respdata)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_SMART_CARD_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = SP100_EXCHANGE_APDU_PLAINTEXT; 
	memcpy(data+4,cmddata,datalen);
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+SP100_FUNCTION_ID_LEN+datalen, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_SMART_CARD_RESPONSE,2) &&  append[0]==SP100_SEPERATOR_CMD && append[1]==SP100_EXCHANGE_APDU_PLAINTEXT)
	{
		memcpy(respcode,append+2,2);
		memcpy(respdata,append+4,appendlen-4);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_EXCHANGE_APDU_PLAINTEXT;	
	}
}

//关闭场强
static ushort SP100_Deactivate_Field(uchar *respcode)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_SMART_CARD_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = SP100_DEACTIVATE_FIELD; 
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+SP100_FUNCTION_ID_LEN, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_SMART_CARD_RESPONSE,2) && appendlen == 4  &&  append[0]==SP100_SEPERATOR_CMD && append[1]==SP100_DEACTIVATE_FIELD)
	{
		memcpy(respcode,append+2,2);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_DEACTIVATE_FIELD;	
	}
}

//控制射频灯
static ushort SP100_Flash_LED(uchar ledid, uchar *blinktime, uchar *respcode)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_SMART_CARD_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	data[3] = SP100_FLASH_LED; 
	data[4] = ledid;
	memcpy(data+5,blinktime,2);
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN+SP100_FUNCTION_ID_LEN+3, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_SMART_CARD_RESPONSE,2) && appendlen == 4  &&  append[0]==SP100_SEPERATOR_CMD && append[1]==SP100_FLASH_LED)
	{
		memcpy(respcode,append+2,2);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_FLASH_LED;	
	}
}

//重启
static ushort SP100_Reboot(uchar *respcode)
{
	/*private & local definition*/
	ushort ret = 0;
	int appendlen = 0;
	uchar data[SP100_MAXLEN_DATA] = {0}, append[SP100_MAXLEN_DATA] = {0}, answer[2] = {0};
	
	/*process body*/
	memcpy(data,SP100_REBOOT_REQUEST,SP100_LEN_CMD);
	// separator 2F
	data[2] = SP100_SEPERATOR_CMD;
	if((ret=SP100_frame_factory(data, SP100_LEN_CMD+SP100_SEPERATOR_LEN, answer, append, &appendlen, 0))!=SP100_SUCC)
		return ret;	
	if(!memcmp(answer,SP100_REBOOT_RESPONSE,2) && appendlen == 3  &&  append[0]==SP100_SEPERATOR_CMD)
	{
		memcpy(respcode,append+1,2);
		return SP100_SUCC;
	}
	else
	{
		return SP100_ERR_REBOOT;	
	}
}

static void test_load_key(int ability_flag)//0--功能测试 1--性能测试
{
	ushort ret = SP100_FAIL;
	uchar aes_tpk[] = {0x94,0x06,0xB7,0x9D,0x9F,0x17,0x33,0xAC,0x15,0xAD,0x53,0xB6,0x66,0x71,0xAC,0x0F,0x94,0x06,0xB7,0x9D,0x9F,0x17,0x33,0xAC,0x15,0xAD,0x53,0xB6,0x66,0x71,0xAC,0x0F};//0a
	uchar aes_tak[] = {0xD2,0xB7,0x6F,0x4A,0xCB,0x59,0x80,0x33,0x5A,0xFA,0x95,0x99,0xC4,0x82,0x6A,0x6E,0xD2,0xB7,0x6F,0x4A,0xCB,0x59,0x80,0x33,0x5A,0xFA,0x95,0x99,0xC4,0x82,0x6A,0x6E };//0b
	uchar aes_tdk[] = {0x01,0xAC,0x56,0xF3,0x17,0xDF,0xF3,0xC5,0x63,0x0F,0xB1,0xCA,0x71,0xBB,0x3A,0xC3,0x01,0xAC,0x56,0xF3,0x17,0xDF,0xF3,0xC5,0x63,0x0F,0xB1,0xCA,0x71,0xBB,0x3A,0xC3}; //0c
	uchar aes_tdk2[] = {0xA5,0x74,0xB2,0x43,0x57,0x86,0x0C,0x4B,0x9C,0x58,0x0D,0x4C,0xBE,0x6C,0x22,0xC7,0xA5,0x74,0xB2,0x43,0x57,0x86,0x0C,0x4B,0x9C,0x58,0x0D,0x4C,0xBE,0x6C,0x22,0xC7};//D6
	uchar resp[2] = {0};
#if  IRAN_CUSTOM
	//伊朗定制版本使用DES密文安装
	uchar des_tpk[] = {0xB7,0xF3,0x8B,0xEE,0x0B,0x63,0x30,0x06,0x96,0x92,0x45,0x89,0x48,0xBE,0xD2,0xFD};//06
	uchar des_tak[] = {0x96,0x92,0x45,0x89,0x48,0xBE,0xD2,0xFD,0xB7,0xF3,0x8B,0xEE,0x0B,0x63,0x30,0x06};//07
	uchar des_tdk[] = {0xB7,0xF3,0x8B,0xEE,0x0B,0x63,0x30,0x06,0xA5,0xBB,0xC9,0x23,0x15,0xA0,0xF0,0xD4};//0a
	uchar des_tdk2[] = {0x2B,0xF0,0x00,0xE3,0x4A,0x11,0xE2,0xA2,0x2B,0xF0,0x00,0xE3,0x4A,0x11,0xE2,0xA2};//83
	uchar same_tak[] = {0x96,0x92,0x45,0x89,0x48,0xBE,0xD2,0xFD,0x96,0x92,0x45,0x89,0x48,0xBE,0xD2,0xFD};//0b 明文为16位0x33
	uchar keyvalue[3] = {0}, keyid = 0; 
#endif
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	tr31_key_t tr31_st;
	unsigned int len = 0;
	uchar mkey[17]={0}, skey[17]={0}, block[512]={0};
	float tpk_time = 0.0, tak_time = 0.0, tdk_time = 0.0;
	uchar nonekcv[3] = {0x00,0x00,0x00}, kcvtype = 0; //kcv不校验的模式的值
	/* 
	AES tmk id=1 32个0x00
	DES tmk id=1 16个0x13
	DUKPT id=7 D5D44FF720683D0D70BE252818E2A18A  ksn 10个0x00
	AES 密文安装对应的明文: 			
							id  0x0a:TPK  32位 0x11		KCVNONE
					   		id  0x0b:TAK  32位 0x33		KCVNONE
					   		id  0x0c:TDK  32位 0x44		KCVNONE
					   		id  0xD6:TDK  32位0x22		KCVNONE
	DES TR31格式的明文:
							id  0x04:TPK	8位0x11和8位0x22	KCVNONE
							id  0x05:TAK	16位0x33					KCVNONE
							id  0x08:TDK	8位0x22和8位0x11	KCVNONE
							id  0x82:TDK	8位0x22和8位0x33	KCVNONE
	DES 密文安装对应的明文:
							id  0x06:TPK	8位0x11和8位0x33	KCVZERO 校验
							id  0x07:TAK	8位0x33和8位0x11	KCVNONE
							id  0x0a:TDK	8位0x11和8位0x88	KCVZERO 校验
							id  0x83:TDK	8位16位0x22			KCVNONE
	*/	
	//case1:正常安装AES PIN密钥id为0x0a
	cls_printf("进行AES密钥安装测试...");
	lib_StartStopwatch();
	if((ret=SP100_Load_Key_Block(0x01,0x01,0x20,0x00,0x0a,(uchar *)"\x00\x20",aes_tpk,sizeof(aes_tpk),kcvtype,nonekcv,resp))!=SP100_SUCC)//"\x00\x20"
	{	
		cls_show_msg("line %d:AES PIN密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	tpk_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES PIN密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"AES PIN密钥安装时间:%fs",tpk_time);
	
	//case2:正常安装AES MAC密钥id为0x0b
	lib_StartStopwatch();
	if((ret=SP100_Load_Key_Block(0x01,0x01,0x20,0x01,0x0b,(uchar *)"\x00\x20",aes_tak,sizeof(aes_tak),kcvtype,nonekcv,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES MAC密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	tak_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES MAC密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"AES MAC密钥安装时间:%fs",tak_time);
	
	//case3:正常安装AES data密钥id为0x0c
	lib_StartStopwatch();
	if((ret=SP100_Load_Key_Block(0x01,0x01,0x20,0x02,0x0c,(uchar *)"\x00\x20",aes_tdk,sizeof(aes_tdk),kcvtype,nonekcv,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES DATA密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	tdk_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES DATA密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"AES DATA密钥安装时间:%fs",tdk_time);
	
	//case3.1:正常安装AES data密钥用于后面的PIN输入测试id为0xD6(214)
	if((ret=SP100_Load_Key_Block(0x01,0x01,0x20,0x02,0xD6,(uchar *)"\x00\x20",aes_tdk2,sizeof(aes_tdk2),kcvtype,nonekcv,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES DATA密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES DATA密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}

	//case4:异常测试:AES相同密钥(相同的TPK密钥)不能安装不同id上id为0x09
	if((ret=SP100_Load_Key_Block(0x01,0x01,0x20,0x02,0x09,(uchar *)"\x00\x20",aes_tpk,sizeof(aes_tpk),kcvtype,nonekcv,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES DATA密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_BAD_KEY_INDEX)
	{	
		cls_show_msg("line %d:AES DATA密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}

	
	cls_printf("进行DES密钥TR31格式安装测试...");
	//case5:正常安装DES PIN密钥 (使用代码生成tr31格式然后转成hex格式) id为0x04
	NDK_SecKeyErase();
	//安装TMK, ID=1  用于生成测试数据
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=1;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, "\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13", stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		cls_show_msg("line %d:安装TMK失败(%d)", __LINE__,ret);
		return;
	}
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=4;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, 0x13, 16);/*由TMK发散*/
	memset(skey, 0x11, 8);	/*TPK密钥明文:8个字节的0x11+8个字节0x22*/
	memset(skey+8, 0x22, 8);	
	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_PIN_TYPE, 4);	/*TPK*/
	tr31_st.key = skey;
	tr31_st.keylen = 16;
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		cls_show_msg("line %d:生成tr31格式TPK数据失败(%d)", __LINE__, ret);
		return;
	}
	lib_StartStopwatch();
	if((ret=SP100_Load_Key_Block(0x01,0x00,0x18,0x00,0x04,(uchar *)"\x00\x58",block,len,kcvtype,nonekcv,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES PIN密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	tpk_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DES PIN密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"DES PIN密钥TR31格式安装时间:%fs",tpk_time);
	
	//case6:正常安装DES MAC密钥 id为0x05
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, 0x13, 16);/*由TMK发散*/
	memset(skey, 0x33, 16);	/*TAK密钥明文:16个字节0x33*/
	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAC_TYPE, 4);	/*TAK*/
	tr31_st.key = skey;
	tr31_st.keylen = 16;
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		cls_show_msg("line %d:生成tr31格式TAK数据失败(%d)", __LINE__, ret);
		return;
	}
	lib_StartStopwatch();
	if((ret=SP100_Load_Key_Block(0x01,0x00,0x18,0x01,0x05,(uchar *)"\x00\x58",block,len,kcvtype,nonekcv,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES MAC密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	tak_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DES MAC密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"DES MAC密钥TR31格式安装时间:%fs",tak_time);

	//case7:正常安装DES data密钥id为0x08
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=8;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, 0x13, 16);/*由TMK发散*/
	memset(skey, 0x22, 8);	/*TDK密钥明文:8个字节的0x22+8个字节0x11*/
	memset(skey+8, 0x11, 8);
	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = skey;
	tr31_st.keylen = 16;
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		cls_show_msg("line %d:生成tr31格式TDK数据失败(%d)", __LINE__, ret);
		return;
	}
	lib_StartStopwatch();
	if((ret=SP100_Load_Key_Block(0x01,0x00,0x18,0x02,0x08,(uchar *)"\x00\x58",block,len,kcvtype,nonekcv,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES DATA密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	tdk_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DES DATA密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"DES DATA密钥TR31格式安装时间:%fs",tdk_time);
	
	//case7.1:正常安装DES data密钥用于后面的pin输入测试id为0x82(130)
	stKeyInfo.ucScrKeyIdx=1;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=82;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=sizeof(ST_EXTEND_KEYBLOCK);
	memset(mkey, 0x13, 16);/*由TMK发散*/
	memset(skey, 0x22, 8);	/*TDK密钥明文:8个字节的0x22+8个字节0x33*/
	memset(skey+8, 0x33, 8);
	memset(&tr31_st, 0, sizeof(tr31_st));
	tr31_st.gid = stKeyInfo.ucDstKeyIdx;
	memcpy(tr31_st.usage, KT_MAG_TYPE, 4);	/*TDK*/
	tr31_st.key = skey;
	tr31_st.keylen = 16;
	if((ret=sec_tr31block_format(block, &len,  mkey, 16, &tr31_st))!=0)
	{
		cls_show_msg("line %d:生成tr31格式TDK数据失败(%d)", __LINE__, ret);
		return;
	}
	if((ret=SP100_Load_Key_Block(0x01,0x00,0x18,0x02,0x82,(uchar *)"\x00\x58",block,len,kcvtype,nonekcv,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES TDK密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DES DATA密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}

	//case8:异常测试:DES相同密钥(相同的TDK密钥)不能安装不同id上id为09
	if((ret=SP100_Load_Key_Block(0x01,0x00,0x18,0x02,0x09,(uchar *)"\x00\x58",block,len,kcvtype,nonekcv,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES DATA密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_BAD_KEY_INDEX)
	{	
		cls_show_msg("line %d:DES DATA密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//擦除pos上的密钥
	NDK_SecKeyErase();

#if IRAN_CUSTOM
	//DES密文安装(TPK TAK TDK) (伊朗定制版本)
	//case9: 密文安装DES TPK  id为0x06  明文为8个字节的0x11+8个字节0x33  KCV ZERO 校验
	cls_printf("进行DES密钥密文安装测试...");
	lib_StartStopwatch();
	if((ret=SP100_Load_Key_Block(0x01,0x02,0x18,0x00,0x06,(uchar *)"\x00\x10",des_tpk,sizeof(des_tpk),0x01,(uchar *)"\x92\xA3\x29",resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES TPK密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	tpk_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DES TPK密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//增加获取Kcv校验
	if((ret=SP100_Get_Key_Check(0x06,0x01,&keyid,keyvalue))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"DES PIN密钥密文格式安装时间:%fs",tpk_time);
	
	//case10: 密文安装DES TAK  id为0x07  明文为8个字节的0x33+8个字节0x11
	lib_StartStopwatch();
	if((ret=SP100_Load_Key_Block(0x01,0x02,0x18,0x01,0x07,(uchar *)"\x00\x10",des_tak,sizeof(des_tak),kcvtype,nonekcv,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES TAK密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	tak_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DES TAK密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"DES MAC密钥密文格式安装时间:%fs",tak_time);
	
	//case11: 密文安装DES TDK  id为0x0a 明文为8位0x11和8位0x88   KCVZERO校验 
	lib_StartStopwatch();
	if((ret=SP100_Load_Key_Block(0x01,0x02,0x18,0x02,0x0a,(uchar *)"\x00\x10",des_tdk,sizeof(des_tdk),0x01,(uchar *)"\x98\x59\x09",resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES TDK密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	tdk_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DES TDK密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if((ret=SP100_Get_Key_Check(0x0a,0x03,&keyid,keyvalue))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"DES DATA密钥密文格式安装时间:%fs",tdk_time);

	//case11.1: 密文安装DES TDK  id为0x83 明文为16个字节的0x22
	if((ret=SP100_Load_Key_Block(0x01,0x02,0x18,0x02,0x83,(uchar *)"\x00\x10",des_tdk2,sizeof(des_tdk2),kcvtype,nonekcv,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES TDK密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DES TDK密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}

	//case12:使用DES tr31格式的密文数据进行不同id(id为0x0b)安装 预期返回失败(相同密钥不能安装不同id)
	if((ret=SP100_Load_Key_Block(0x01,0x02,0x18,0x01,0x0b,(uchar *)"\x00\x10",same_tak,sizeof(same_tak),kcvtype,nonekcv,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES TAK密钥安装失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_BAD_KEY_INDEX)
	{	
		cls_show_msg("line %d:DES TAK密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
#endif

	//case13:参数异常测试:format不在范围内;key type 不在范围内;AES data长度不对
	if(!ability_flag)
	{
		//case13.1:主密钥不正确的情况AES  DES  
		if((ret=SP100_Load_Key_Block(0x02,0x01,0x20,0x00,0x0a,(uchar *)"\x00\x20",aes_tpk,32,kcvtype,nonekcv,resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:AES PIN密钥安装失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_BAD_MASTER_KEY)
		{	
			cls_show_msg("line %d:AES PIN密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}
		if((ret=SP100_Load_Key_Block(0x02,0x00,0x20,0x02,0x82,(uchar *)"\x00\x58",block,len,kcvtype,nonekcv,resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:DES TDK密钥安装失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_BAD_MASTER_KEY)
		{	
			cls_show_msg("line %d:DES DATA密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}
		//case13.2:DES密钥装载时keytype不在范围内,传入的是0x03预期返回General error
		if((ret=SP100_Load_Key_Block(0x02,0x00,0x18,0x03,0x82,(uchar *)"\x00\x58",block,len,kcvtype,nonekcv,resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:DES TDK密钥安装失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_GENERAl_ERROR)
		{	
			cls_show_msg("line %d:DES DATA密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}
		//case13.3:AES安装密钥时data长度不是预期的,数据是24,data长度传入32 预期返回Bad data length
		if((ret=SP100_Load_Key_Block(0x02,0x01,0x20,0x00,0x0a,(uchar *)"\x00\x18",aes_tpk,32,kcvtype,nonekcv,resp))!=SP100_SUCC)//"\x00\x20"
		{	
			cls_show_msg("line %d:AES PIN密钥安装失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_BAD_DATA_LEN)
		{	
			cls_show_msg("line %d:AES PIN密钥安装失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}
	}
	cls_show_msg("密钥装载测试通过!");
	return;
}

static void test_pin_entry(void)
{
	ushort ret = SP100_FAIL;
	uchar aespan[] = {0xE9,0xB3,0x5D,0xE8,0xD3,0x95,0xE2,0xB1,0x3D,0x54,0xEB,0x24,0x20,0x9E,0x4F,0x91};//6225885916163157与32位0x22 AES加密得到
	uchar despan[] = {0x95,0x05,0x36,0x30,0x96,0x4C,0x0E,0x0C,0xC5,0x96,0x1E,0x62,0xBF,0xCE,0x71,0x10};//6225885916163157与8位0x11 8位0x22 DES加密得到8位数据后面的8位随便
	uchar display[] = {0x50,0x49,0x4E,0x20,0x45,0x6E,0x74,0x72,0x79,0x3A,0x1C,0x1C,0x1C};
	uchar resp[2] = {0}, fuckey ;
	uchar pinlen = 0, pinblock[16] = {0}, ksn[10] = {0}, PinDesOut[9]={0}, PinDesOut1[9]={0};
	uchar *textpan=(uchar *)"6225885916163157";	
	uchar szPan1[8] = {0x00,0x00,0x58,0x85,0x91,0x61,0x63,0x15}; //pan格式前面2字节为0,主账号最后一个数舍去，从倒数第2位开始往前取，取到12个数字
	uchar PinKeyValue[16] = {0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22};//des 密钥id为04
	//uchar DukptPinKeyValue[16]={0x6E,0xF9,0x27,0x95,0xE5,0x12,0x71,0xB2,0xCD,0xEB,0x76,0x7B,0x33,0xF1,0xA9,0x42};//DUKPY派生出的pin密钥 ksn为0000000001
	uchar DukptPinKeyValue1[16]={0x56,0x35,0xB1,0x84,0xDD,0x5D,0x8F,0xBE,0x56,0x17,0xB6,0x60,0x0C,0x6E,0x47,0x30};//DUKPY派生出的pin密钥 ksn为0000000002
	int i = 0;
	int textpanlen = strlen(textpan);
#if 0 //AES算法 在中端产品上还未导入,无法通过逆向解密来比较结果后续导入可以开启
	uchar szPan2[16]={0x46,0x22,0x58,0x85,0x91,0x61,0x63,0x15,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //aes 的pan计算后的格式
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	uchar  PinAESOut[16] = {0}, PinAESOut1[16] = {0}, PinAESOut2[16] = {0};
	//测试前置 擦除密钥
	NDK_SecKeyErase();
	//以SEC_KCV_NONE方式明文安装TPK1, ID=2 32位0x11
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK|SEC_KEY_AES;
	stKeyInfo.nDstKeyLen=32;
	memset(stKeyInfo.sDstKeyValue, 0x11, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{	
		cls_show_msg("line %d:AES 密钥安装失败(%d)", __LINE__, ret);
		return;
	}	
#endif

	//case1:正常测试AES PIN 输入PAN是密文输入
	cls_printf("请在密码键盘上输入1234后按确认");
	if((ret=SP100_Pin_Entry(0x0a,0x02,0xD6,0x01,0x10,aespan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x04,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
#if 0 //AES算法 在中端产品上还未导入,无法通过逆向解密来比较结果后续导入可以开启
	memset(PinAESOut1, 0, sizeof(PinAESOut1));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 2, pinblock, 16, PinAESOut1, SEC_AES_DECRYPT))!=NDK_OK)
	{
		cls_show_msg("line %d:11(%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, PinAESOut1[0],PinAESOut1[1], PinAESOut1[2],PinAESOut1[3], PinAESOut1[4],PinAESOut1[5], PinAESOut1[6],PinAESOut1[7],PinAESOut1[8],PinAESOut1[9], PinAESOut1[10],PinAESOut1[11], PinAESOut1[12],PinAESOut1[13], PinAESOut1[14],PinAESOut1[15]);
		//return;
	}
	//cls_show_msg("line %d:11(%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, PinAESOut1[0],PinAESOut1[1], PinAESOut1[2],PinAESOut1[3], PinAESOut1[4],PinAESOut1[5], PinAESOut1[6],PinAESOut1[7],PinAESOut1[8],PinAESOut1[9], PinAESOut1[10],PinAESOut1[11], PinAESOut1[12],PinAESOut1[13], PinAESOut1[14],PinAESOut1[15]);
	memset(PinAESOut2, 0, sizeof(PinAESOut2));
	for (i = 0; i < 16; i++) 
	{
		PinAESOut2[i] = PinAESOut1[i] ^ szPan2[i];
	}
	cls_show_msg("line %d:AES解密失败(%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, PinAESOut2[0],PinAESOut2[1], PinAESOut2[2],PinAESOut2[3], PinAESOut2[4],PinAESOut2[5], PinAESOut2[6],PinAESOut2[7],PinAESOut2[8],PinAESOut2[9], PinAESOut2[10],PinAESOut2[11], PinAESOut2[12],PinAESOut2[13], PinAESOut2[14],PinAESOut2[15]);
	memset(PinAESOut, 0, sizeof(PinAESOut));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 2, PinAESOut2, 16, PinAESOut, SEC_AES_DECRYPT))!=NDK_OK)
	{
		cls_show_msg("line %d:AES解密失败(%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, PinAESOut[0],PinAESOut[1], PinAESOut[2],PinAESOut[3], PinAESOut[4],PinAESOut[5], PinAESOut[6],PinAESOut[7],PinAESOut[8],PinAESOut[9], PinAESOut[10],PinAESOut[11], PinAESOut[12],PinAESOut[13], PinAESOut[14],PinAESOut[15]);
		//return;
	}
	if(memcmp(PinAESOut, "\x44\x12\x34", 3))
	{
		cls_show_msg("line %d:数据比较失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, PinAESOut[0],PinAESOut[1],PinAESOut[2],PinAESOut[3],PinAESOut[4],PinAESOut[5],PinAESOut[6],PinAESOut[7]);
		return;
	}
#endif

#if IRAN_CUSTOM
	//case2:正常测试AES PIN 输入PAN是明文输入
	cls_printf("请在密码键盘上输入12345后按确认");
	if((ret=SP100_Pin_Entry(0x0a,0x02,0x00,0x01,0x10,aespan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x04,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
#if 0 //AES算法 在中端产品上还未导入,无法通过逆向解密来比较结果后续导入可以开启
	memset(PinAESOut1, 0, sizeof(PinAESOut1));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 2, pinblock, 16, PinAESOut1, SEC_AES_DECRYPT))!=NDK_OK)
	{
		cls_show_msg("line %d:11(%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, PinAESOut1[0],PinAESOut1[1], PinAESOut1[2],PinAESOut1[3], PinAESOut1[4],PinAESOut1[5], PinAESOut1[6],PinAESOut1[7],PinAESOut1[8],PinAESOut1[9], PinAESOut1[10],PinAESOut1[11], PinAESOut1[12],PinAESOut1[13], PinAESOut1[14],PinAESOut1[15]);
		//return;
	}
	//cls_show_msg("line %d:11(%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, PinAESOut1[0],PinAESOut1[1], PinAESOut1[2],PinAESOut1[3], PinAESOut1[4],PinAESOut1[5], PinAESOut1[6],PinAESOut1[7],PinAESOut1[8],PinAESOut1[9], PinAESOut1[10],PinAESOut1[11], PinAESOut1[12],PinAESOut1[13], PinAESOut1[14],PinAESOut1[15]);
	memset(PinAESOut2, 0, sizeof(PinAESOut2));
	for (i = 0; i < 16; i++) 
	{
		PinAESOut2[i] = PinAESOut1[i] ^ szPan2[i];
	}
	cls_show_msg("line %d:AES解密失败(%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, PinAESOut2[0],PinAESOut2[1], PinAESOut2[2],PinAESOut2[3], PinAESOut2[4],PinAESOut2[5], PinAESOut2[6],PinAESOut2[7],PinAESOut2[8],PinAESOut2[9], PinAESOut2[10],PinAESOut2[11], PinAESOut2[12],PinAESOut2[13], PinAESOut2[14],PinAESOut2[15]);
	memset(PinAESOut, 0, sizeof(PinAESOut));
	if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TPK, 2, PinAESOut2, 16, PinAESOut, SEC_AES_DECRYPT))!=NDK_OK)
	{
		cls_show_msg("line %d:AES解密失败(%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, PinAESOut[0],PinAESOut[1], PinAESOut[2],PinAESOut[3], PinAESOut[4],PinAESOut[5], PinAESOut[6],PinAESOut[7],PinAESOut[8],PinAESOut[9], PinAESOut[10],PinAESOut[11], PinAESOut[12],PinAESOut[13], PinAESOut[14],PinAESOut[15]);
		//return;
	}
	if(memcmp(PinAESOut, "\x45\x12\x34", 3))
	{
		cls_show_msg("line %d:数据比较失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, PinAESOut[0],PinAESOut[1],PinAESOut[2],PinAESOut[3],PinAESOut[4],PinAESOut[5],PinAESOut[6],PinAESOut[7]);
		return;
	}
	NDK_SecKeyErase();
#endif
#endif

	//case3:正常测试DES PIN输入pan密文解出来明文是 6225885916163157
	cls_printf("请在密码键盘上输入123456后按确认");
	if((ret=SP100_Pin_Entry(0x04,0x00,0x82,0x00,0x10,despan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x00,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//计算后的pin(06123456FFFFFFFF)与pan(0000588591616315)异或后再与密钥加密得到pinblock 密文AD16C95447E78521
	if(memcmp(pinblock,"\xAD\x16\xC9\x54\x47\xE7\x85\x21",8))
	{	
		cls_show_msg("line %d:数据比较失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, pinblock[0],pinblock[1], pinblock[2],pinblock[3], pinblock[4],pinblock[5], pinblock[6],pinblock[7]);
		return;
	}

	//case4:正常测试DES PIN输入pan是密文输入isoformat为3通过对pinblock解密得到明文再与主账号异或得到前几位数是固定的
	cls_printf("请在密码键盘上输入123456789012后按确认");
	if((ret=SP100_Pin_Entry(0x04,0x00,0x82,0x00,0x10,despan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x03,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//输出的密文通过解密得出明文,明文再与卡号异或后可以得出固定的前几位数  added jiangym 20150125
	if((ret=NDK_AlgTDes(pinblock, PinDesOut1, PinKeyValue, sizeof(PinKeyValue), ALG_TDS_MODE_DEC)) !=NDK_OK)
	{
		cls_show_msg("line %d:DES 解密失败(%04x)", __LINE__, ret);
		return;
	}
	//明文为PinDesOut1 然后与卡号右边12位(去掉最后一位,前面补0)  即:0000588591616315异或得出结果再比较	
	memset(PinDesOut, 0, sizeof(PinDesOut));
	for (i = 0; i < 8; i++) 
	{
		PinDesOut[i] = PinDesOut1[i] ^ szPan1[i];
	}
	//异或后的结果:第一位表示计算模式为SEC_PIN_ISO9564_3,第二位表示密码有10位(a),后面是密码123456789012剩余的是随机数
	if(memcmp(PinDesOut, "\x3c\x12\x34\x56\x78\x90\x12", 7))
	{		
		cls_show_msg("line %d:数据比较失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, PinDesOut[0],PinDesOut[1], PinDesOut[2],PinDesOut[3], PinDesOut[4],PinDesOut[5], PinDesOut[6],PinDesOut[7]);
		return;
	}

#if IRAN_CUSTOM  
	//case5:正常测试DES PIN输入pan是明文输入isoformat为1通过对pinblock解密得到pin前几位进行比较
	cls_printf("请在密码键盘上输入12345678后按确认");
	if((ret=SP100_Pin_Entry(0x04,0x00,0x00,0x00,0x10,despan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x01,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//输出的密文通过des解密后可以得到明文,明文的前几位数据是固定的可以判断 
	if((ret=NDK_AlgTDes(pinblock, PinDesOut, PinKeyValue, sizeof(PinKeyValue), ALG_TDS_MODE_DEC)) !=NDK_OK)
	{
		cls_show_msg("line %d:DES 解密失败(%04x)", __LINE__, ret);
		return;
	}
	//明文为1812345678xxxxxx (16位)第一位表示计算模式为SEC_PIN_ISO9564_1,第二位表示密码有8位,后面是密码12345678剩余的是随机数
	if(memcmp(PinDesOut, "\x18\x12\x34\x56\x78", 5))
	{
		cls_show_msg("line %d:数据比较失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, PinDesOut[0],PinDesOut[1], PinDesOut[2],PinDesOut[3], PinDesOut[4],PinDesOut[5], PinDesOut[6],PinDesOut[7]);
		return;
	}
#endif

	//case6:直接按确认
	cls_printf("请在密码键盘上按确认");
	if((ret=SP100_Pin_Entry(0x04,0x00,0x82,0x00,0x10,despan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x00,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if(memcmp(pinblock,"\x00\x00\x00\x00\x00\x00\x00\x00",8))
	{	
		cls_show_msg("line %d:数据比较失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, pinblock[0],pinblock[1], pinblock[2],pinblock[3], pinblock[4],pinblock[5], pinblock[6],pinblock[7]);
		return;
	}
	
	//***注意***:安装主密钥后首次运行此用例,dukpt的KSN会随着测试逐步增加的,故按顺序进行的话一般是从1开始,到执行mac时就会变成3 4
	//case6:正常测试DUKPT PIN输入pan密文输入,isoformat为0  (DUKPT密码键盘反应比较慢需要等待才会出现输入框)
	cls_printf("请在密码键盘上输入123456后按确认");
	if((ret=SP100_Pin_Entry(0x07,0x01,0x82,0x00,0x10,despan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x00,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DUKPT PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if(!memcmp(ksn,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01",10))	//ksn为0000000001时进行判断
	{
		//计算后的pin(06123456FFFFFFFF)与pan(0000588591616315)异或后再与派生密钥加密得到pinblock 密文8FA87358DC95C477
		if(memcmp(pinblock,"\x8F\xA8\x73\x58\xDC\x95\xC4\x77",8))
		{	
			cls_show_msg("line %d:数据比较失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, pinblock[0],pinblock[1], pinblock[2],pinblock[3], pinblock[4],pinblock[5], pinblock[6],pinblock[7]);
			return;
		}
	}

#if IRAN_CUSTOM
	//case7:正常测试DUKPT PIN输入pan明文输入,isoformat为1(DUKPT密码键盘反应比较慢需要等待才会出现输入框)
	cls_printf("请在密码键盘上输入1234后按确认");
	if((ret=SP100_Pin_Entry(0x07,0x01,0x00,0x00,0x10,despan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x01,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DUKPT PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if(!memcmp(ksn,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02",10))  //ksn为0000000002时进行判断
	{
		//输出的密文通过des解密后可以得到明文,明文的前几位数据是固定的可以判断 
		if((ret=NDK_AlgTDes(pinblock, PinDesOut, DukptPinKeyValue1, sizeof(DukptPinKeyValue1), ALG_TDS_MODE_DEC)) !=NDK_OK)
		{
			cls_show_msg("line %d:DES 解密失败(%04x)", __LINE__, ret);
			return;
		}
		//明文为141234xxxxxxxxxx  (16位)第一位表示计算模式为SEC_PIN_ISO9564_1,第二位表示密码有4位,后面是密码1234剩余的是随机数
		if(memcmp(PinDesOut, "\x14\x12\x34", 3))
		{
			cls_show_msg("line %d:数据比较失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, PinDesOut[0],PinDesOut[1], PinDesOut[2],PinDesOut[3], PinDesOut[4],PinDesOut[5], PinDesOut[6],PinDesOut[7]);
			return;
		}
	}
#endif

	//case8:异常参数测试
	//id不正确预期返回Bad Key Index
	if((ret=SP100_Pin_Entry(0x0b,0x02,0xD6,0x01,0x10,aespan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x04,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_BAD_KEY_INDEX)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if((ret=SP100_Pin_Entry(0x05,0x00,0x82,0x00,0x10,despan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x01,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_BAD_KEY_INDEX)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//key type 不在范围内预期返回General Error
	if((ret=SP100_Pin_Entry(0x0a,0x03,0xD6,0x01,0x10,aespan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x04,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_GENERAl_ERROR)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//account key tpye 不正确预期返回Bad Key Index
	if((ret=SP100_Pin_Entry(0x0a,0x02,0xD6,0x00,0x10,aespan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x04,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_BAD_KEY_INDEX)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if((ret=SP100_Pin_Entry(0x04,0x00,0x82,0x01,0x10,despan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x01,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_BAD_KEY_INDEX)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//ISO9564 Format (正确的情况是4是aes的format，其他都是des的format) 传入不正确的情况预期返回General Error
	if((ret=SP100_Pin_Entry(0x0a,0x02,0xD6,0x01,0x10,aespan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x03,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_GENERAl_ERROR)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	if((ret=SP100_Pin_Entry(0x04,0x00,0x82,0x00,0x10,despan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x04,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_GENERAl_ERROR)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}

	//case9:进行pin输入时提示按取消操作
	cls_printf("请在密码键盘上按取消键");
	if((ret=SP100_Pin_Entry(0x04,0x00,0x82,0x00,0x10,despan,textpan,textpanlen,0x00,0x00,0x0c,0x00,0x10,0x03,display,sizeof(display),resp,&fuckey,&pinlen,pinblock,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES PIN输入请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_PIN_CANCELLED)
	{	
		cls_show_msg("line %d:AES PIN输入请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	cls_show_msg("PIN 输入测试通过!");
	return;
}

static void test_dec_enc(int ability_flag) //0--功能测试 1--性能测试
{
	ushort ret = SP100_FAIL;
	uchar data[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
	uchar encdata_aes[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
	uchar decdata_aes[] = {0xC3,0x91,0xFB,0x46,0xF5,0x4C,0x15,0x2B,0xDE,0x8E,0x8F,0xCB,0x63,0x5C,0x96,0xAC,0xC3,0x91,0xFB,0x46,0xF5,0x4C,0x15,0x2B,0xDE,0x8E,0x8F,0xCB,0x63,0x5C,0x96,0xAC};
	uchar resp[2] = {0}, keyid, enc_len[2]= {0}, enc_dec_data[32] = {0};
	float enc_time = 0.0, dec_time = 0.0;
	
	//des 加解密 
	cls_printf("DES 加解密测试...");
	//case1:CBC不支持所以会返回参数失败(未来有可能会支持)
	if((ret=SP100_Data_Enc_Dec(0x08,0x01,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,enc_len,enc_dec_data))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES加密失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_GENERAl_ERROR)
	{	
		cls_show_msg("line %d:DES加密失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	
	//case2:使用TDK 进行DES加密(ECB模式)
	lib_StartStopwatch();
	if((ret=SP100_Data_Enc_Dec(0x08,0x02,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,enc_len,enc_dec_data))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES加密失败(%04x)", __LINE__, ret);
		return;
	}
	enc_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DES加密失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//加密数据进行比较 3439985D3E9A749E
	if(memcmp(enc_len,"\x00\x08",2) || memcmp(enc_dec_data,"\x34\x39\x98\x5D\x3E\x9A\x74\x9E",8))
	{	
		cls_show_msg("line %d:DES加密失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, enc_dec_data[0],enc_dec_data[1], enc_dec_data[2],enc_dec_data[3], enc_dec_data[4],enc_dec_data[5], enc_dec_data[6],enc_dec_data[7]);
		return;
	} 
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"DES 加密时间:%fs",enc_time);
	
	//case3:使用TDK进行DES解密(ECB模式)
	lib_StartStopwatch();
	if((ret=SP100_Data_Enc_Dec(0x08,0x04,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,enc_len,enc_dec_data))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES解密失败(%04x)", __LINE__, ret);
		return;
	}
	dec_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DES解密失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//解密数据进行比较  5CC6CF955E9A17DD
	if(memcmp(enc_len,"\x00\x08",2)  || memcmp(enc_dec_data,"\x5C\xC6\xCF\x95\x5E\x9A\x17\xDD",8))
	{	
		cls_show_msg("line %d:DES解密失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, enc_dec_data[0],enc_dec_data[1], enc_dec_data[2],enc_dec_data[3], enc_dec_data[4],enc_dec_data[5], enc_dec_data[6],enc_dec_data[7]);
		return;
	} 
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"DES 解密时间:%fs",dec_time);
	
	//aes加解密
	cls_printf("AES 加解密测试...");
	//case4:使用TDK进行AES加密(数据是16字节)
	lib_StartStopwatch();
	if((ret=SP100_AES_Enc_Dec(0x0c,0x01,(uchar *)"\x00\x10",encdata_aes,sizeof(encdata_aes),0x00,&keyid,resp,enc_len,enc_dec_data))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES加密失败(%04x)", __LINE__, ret);
		return;
	}
	enc_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES加密失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//加密数据进行比较 0C AA DF 35 9A 8A 7D E7 F6 92 8F F6 06 62 E0 17  
	if(memcmp(enc_len,"\x00\x10",2)  || memcmp(enc_dec_data,"\x0C\xAA\xDF\x35\x9A\x8A\x7D\xE7\xF6\x92\x8F\xF6\x06\x62\xE0\x17 ",16))
	{	
		cls_show_msg("line %d:AES加密失败(%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, enc_dec_data[0],enc_dec_data[1], enc_dec_data[2],enc_dec_data[3], enc_dec_data[4],enc_dec_data[5], enc_dec_data[6],enc_dec_data[7],enc_dec_data[8],enc_dec_data[9], enc_dec_data[10],enc_dec_data[11], enc_dec_data[12],enc_dec_data[13], enc_dec_data[14],enc_dec_data[15]);
		return;
	} 
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"AES 加密时间:%fs",enc_time);
	
	//case5:使用TDK进行AES解密(数据是32字节)
	lib_StartStopwatch();
	if((ret=SP100_AES_Enc_Dec(0x0c,0x02,(uchar *)"\x00\x20",decdata_aes,sizeof(decdata_aes),0x00,&keyid,resp,enc_len,enc_dec_data))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES解密失败(%04x)", __LINE__, ret);
		return;
	}
	enc_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES解密失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//解密数据进行比较43 05 F2 D9 96 37 14 19 0B 69 0C 73 14 DB 86 21 43 05 F2 D9 96 37 14 19 0B 69 0C 73 14 DB 86 21  
	if(memcmp(enc_len,"\x00\x20",2)  || memcmp(enc_dec_data,"\x43\x05\xF2\xD9\x96\x37\x14\x19\x0B\x69\x0C\x73\x14\xDB\x86\x21\x43\x05\xF2\xD9\x96\x37\x14\x19\x0B\x69\x0C\x73\x14\xDB\x86\x21",32))
	{	
		cls_show_msg("line %d:AES解密失败(%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, enc_dec_data[0],enc_dec_data[1], enc_dec_data[2],enc_dec_data[3], enc_dec_data[4],enc_dec_data[5], enc_dec_data[6],enc_dec_data[7],enc_dec_data[8],enc_dec_data[9], enc_dec_data[10],enc_dec_data[11], enc_dec_data[12],enc_dec_data[13], enc_dec_data[14],enc_dec_data[15]);
		return;
	} 
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"AES 解密时间:%fs",dec_time);

	//case6:异常参数测试
	if(!ability_flag)
	{
		//case6.1:id不正确 预期返回Bad master key index
		if((ret=SP100_Data_Enc_Dec(0x07,0x02,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,enc_len,enc_dec_data))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:DES加密失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_BAD_MASTER_KEY)
		{	
			cls_show_msg("line %d:DES加密失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}
		if((ret=SP100_AES_Enc_Dec(0x0d,0x02,(uchar *)"\x00\x20",decdata_aes,sizeof(decdata_aes),0x00,&keyid,resp,enc_len,enc_dec_data))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:AES解密失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_BAD_MASTER_KEY)
		{	
			cls_show_msg("line %d:AES解密失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}
		//case6.2:mode不正确预期返回General error
		if((ret=SP100_Data_Enc_Dec(0x08,0x05,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,enc_len,enc_dec_data))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:DES加密失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_GENERAl_ERROR)
		{	
			cls_show_msg("line %d:DES加密失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}
		if((ret=SP100_AES_Enc_Dec(0x0c,0x03,(uchar *)"\x00\x20",decdata_aes,sizeof(decdata_aes),0x00,&keyid,resp,enc_len,enc_dec_data))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:AES解密失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_GENERAl_ERROR)
		{	
			cls_show_msg("line %d:AES解密失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}
	}
	cls_show_msg("数据加解密测试通过!");
}

static void test_generate_mac(int ability_flag) //0--功能测试 1--性能测试
{
	ushort ret = SP100_FAIL;
	uchar data[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
	uchar data2[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
	uchar resp[2] = {0}, keyid;
	uchar macdata[16] = {0}, ksn[10] = {0};
	float mac_time = 0.0;
	
	//case1:正常计算AES mac
	cls_printf("AES MAC计算测试...");
	lib_StartStopwatch();
	if((ret=SP100_Mac_Generation(0x0b,0x02,0x05,0x03,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:AES MAC计算失败(%04x)", __LINE__, ret);
		return;
	}
	mac_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:AES MAC计算失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//mac计算结果进行比较7E 58 18 8D DA 46 6C F2 8A 31 A2 5D 3F 26 DB 63 
#if IRAN_CUSTOM
	if(memcmp(macdata,"\x7E\x58\x18\x8D\xDA\x46\x6C\xF2\x8A\x31\xA2\x5D\x3F\x26\xDB\x63 ",16))
	{	
		cls_show_msg("line %d:AES MAC计算失败(%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, macdata[0],macdata[1], macdata[2],macdata[3], macdata[4],macdata[5], macdata[6],macdata[7],macdata[8],macdata[9], macdata[10],macdata[11], macdata[12],macdata[13], macdata[14],macdata[15]);
		return;
	} 
#else
	if(memcmp(macdata,"\x7E\x58\x18\x8D\xDA\x46\x6C\xF2",8))
	{	
		cls_show_msg("line %d:AES MAC计算失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, macdata[0],macdata[1], macdata[2],macdata[3], macdata[4],macdata[5], macdata[6],macdata[7]);
		return;
	} 
#endif
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"AES MAC计算时间:%fs",mac_time);
	
	//case2:正常计算DES mac  mode:0(X99)
	cls_printf("DES MAC计算测试...");
	lib_StartStopwatch();
	if((ret=SP100_Mac_Generation(0x05,0x00,0x00,0x03,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES MAC计算失败(%04x)", __LINE__, ret);
		return;
	}
	mac_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DES MAC计算失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}	
	//mac计算结果进行比较33F2784289467615 
#if IRAN_CUSTOM
	if(memcmp(macdata,"\x33\xF2\x78\x42\x89\x46\x76\x15",8))
	{	
		cls_show_msg("line %d:DES MAC计算失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, macdata[0],macdata[1], macdata[2],macdata[3], macdata[4],macdata[5], macdata[6],macdata[7]);
		return;
	} 
#else
	if(memcmp(macdata,"\x33\xF2\x78\x42",4))
	{	
		cls_show_msg("line %d:DES MAC计算失败(%02x%02x%02x%02x)", __LINE__, macdata[0],macdata[1], macdata[2],macdata[3]);
		return;
	}
#endif
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"DES MAC计算时间:%fs",mac_time);
	
	//case2.1 正常计算DES mac   mode 3(9606)
	if((ret=SP100_Mac_Generation(0x05,0x00,0x03,0x03,(uchar *)"\x00\x10",data2,sizeof(data2),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DES MAC计算失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DES MAC计算失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	//mac计算结果进行比较ADC67D8473BF2F06 
#if IRAN_CUSTOM
	if(memcmp(macdata,"\xAD\xC6\x7D\x84\x73\xBF\x2F\x06",8))
	{	
		cls_show_msg("line %d:DES MAC计算失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, macdata[0],macdata[1], macdata[2],macdata[3], macdata[4],macdata[5], macdata[6],macdata[7]);
		return;
	} 
#else
	if(memcmp(macdata,"\xAD\xC6\x7D\x84",4))
	{	
		cls_show_msg("line %d:DES MAC计算失败(%02x%02x%02x%02x)", __LINE__, macdata[0],macdata[1], macdata[2],macdata[3]);
		return;
	} 
#endif

	//case3:正常计算DUKPT mac  mode:0(X99)
	cls_printf("DUKPT MAC计算测试...");
	lib_StartStopwatch();
	if((ret=SP100_Mac_Generation(0x07,0x01,0x00,0x03,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DUKPT MAC计算失败(%04x)", __LINE__, ret);
		return;
	}
	mac_time = lib_ReadStopwatch();
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DUKPT MAC计算失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}	
	if(!memcmp(ksn,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03",10))  //ksn为0000000003时进行判断 ,随着上面pin输入的测试到这ksn会变成3
	{	
		//mac计算结果进行比较EE74E70FE146AC20
#if IRAN_CUSTOM
		if(memcmp(macdata,"\xEE\x74\xE7\x0F\xE1\x46\xAC\x20",8))
		{	
			cls_show_msg("line %d:DUKPT MAC计算失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, macdata[0],macdata[1], macdata[2],macdata[3], macdata[4],macdata[5], macdata[6],macdata[7]);
			return;
		} 
#else
		if(memcmp(macdata,"\xEE\x74\xE7\x0F",4))
		{	
			cls_show_msg("line %d:DUKPT MAC计算失败(%02x%02x%02x%02x)", __LINE__, macdata[0],macdata[1], macdata[2],macdata[3]);
			return;
		} 
#endif
	}
	if(ability_flag)
		cls_show_msg_record(FILENAME,FUNCNAME,"DUKPT MAC计算时间:%fs",mac_time);
	
	//case 3.1: 正常计算DUKPT mac  mode:3(9609)
	if((ret=SP100_Mac_Generation(0x07,0x01,0x03,0x03,(uchar *)"\x00\x10",data2,sizeof(data2),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:DUKPT MAC计算失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:DUKPT MAC计算失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}	
	if(!memcmp(ksn,"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04",10))  //ksn为0000000004时进行判断 ,随着上面pin输入的测试到这ksn会变成4
	{	
		//mac计算结果进行比较946921591F13B7FE
#if IRAN_CUSTOM
		if(memcmp(macdata,"\x94\x69\x21\x59\x1F\x13\xB7\xFE",8))
		{	
			cls_show_msg("line %d:DUKPT MAC计算失败(%02x%02x%02x%02x%02x%02x%02x%02x)", __LINE__, macdata[0],macdata[1], macdata[2],macdata[3], macdata[4],macdata[5], macdata[6],macdata[7]);
			return;
		} 
#else
		if(memcmp(macdata,"\x94\x69\x21\x59",4))
		{	
			cls_show_msg("line %d:DUKPT MAC计算失败(%02x%02x%02x%02x)", __LINE__, macdata[0],macdata[1], macdata[2],macdata[3]);
			return;
		} 
#endif
	}
	
	if(!ability_flag)
	{
		//case4:异常测试:传入不存在的TAK id预期会返回
		cls_printf("MAC计算异常测试...");
		//AES mac计算传入不存在的TAK id  预期会返回0x43:Bad key index
		if((ret=SP100_Mac_Generation(0x03,0x02,0x05,0x03,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:AES MAC计算失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_BAD_KEY_INDEX)
		{	
			cls_show_msg("line %d:AES MAC计算失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}	
		//DES mac计算传入不存在的TAK id  预期会返回0x43:Bad key index
		if((ret=SP100_Mac_Generation(0x06,0x00,0x00,0x03,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:DES MAC计算失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_BAD_KEY_INDEX)
		{	
			cls_show_msg("line %d:DES MAC计算失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}	
		//DUKPT mac计算传入不存在的TAK id  预期会返回0x43:Bad key index
		if((ret=SP100_Mac_Generation(0x01,0x01,0x00,0x03,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:DUKPT MAC计算失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_BAD_KEY_INDEX)
		{	
			cls_show_msg("line %d:DUKPT MAC计算失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}	
		// mode 不在范围内(DES对应的是0 1 2 3,AES对应的是5) des mac计算传入的是0x04预期返回General error
		if((ret=SP100_Mac_Generation(0x05,0x00,0x04,0x03,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:DES MAC计算失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_GENERAl_ERROR)
		{	
			cls_show_msg("line %d:DES MAC计算失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}
		//aes mac计算传入的是0x01
		if((ret=SP100_Mac_Generation(0x0b,0x02,0x01,0x03,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:AES MAC计算失败(%04x)", __LINE__, ret);
			return;
		}
		if(SP100_GETRESPVAL(resp) != SP100_BAD_KEY_INDEX)
		{	
			cls_show_msg("line %d:AES MAC计算失败(%02x%02x)", __LINE__, resp[0],resp[1]);
			return;
		}
	}
	cls_show_msg("计算MAC测试通过!");
}

static void test_get_key_check(void)
{
	ushort ret = SP100_FAIL;
	uchar keyvalue[3] = {0}, keyid = 0; 
	uchar *check_kcv[] = {
					(uchar *)"\xA8\xB7\xB5",  //DES 主密钥
					(uchar *)"\xD2\xB9\x1C",  //DES tpk
					(uchar *)"\xAD\xC6\x7D",  //DES tak
					(uchar *)"\x5A\x26\x30",  //DES tdk
					(uchar *)"\xDC\x95\xC0",  //AES 主密钥
					(uchar *)"\xDF\x98\xD9",  //AES tpk
					(uchar *)"\xA3\x86\x9D",  //AES tak
					(uchar *)"\xD5\x2F\xCD",  //AES tdk
					};

	cls_printf("获取KCV测试...");	
	//case1:正常测试:获取主密钥
	if((ret=SP100_Get_Key_Check(0x01,0x00,&keyid,keyvalue))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}
	if(keyid != 0x01|| memcmp(keyvalue,check_kcv[0],3))
	{	
		cls_show_msg("line %d:获取KCV失败(%02x,%02x%02x%02x)", __LINE__, keyid, keyvalue[0], keyvalue[1], keyvalue[2]);
		return;
	}

	//case2:获取DES PIN密钥 id为04,kcv为"\xD2\xB9\x1C"
	if((ret=SP100_Get_Key_Check(0x04,0x01,&keyid,keyvalue))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}
	if(keyid != 0x04 || memcmp(keyvalue,check_kcv[1],3))
	{	
		cls_show_msg("line %d:获取KCV失败(%02x,%02x%02x%02x)", __LINE__, keyid, keyvalue[0], keyvalue[1], keyvalue[2]);
		return;
	}

	//case3:获取DES MAC密钥id为05,kcv为"\xAD\xC6\x7D"
	if((ret=SP100_Get_Key_Check(0x05,0x02,&keyid,keyvalue))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}
	if(keyid != 0x05 || memcmp(keyvalue,check_kcv[2],3))
	{	
		cls_show_msg("line %d:获取KCV失败(%02x,%02x%02x%02x)", __LINE__, keyid, keyvalue[0], keyvalue[1], keyvalue[2]);
		return;
	}

	//case4:获取DES DATA密钥id为08,kcv为"\x5A\x26\x30"
	if((ret=SP100_Get_Key_Check(0x08,0x03,&keyid,keyvalue))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}
	if(keyid != 0x08 || memcmp(keyvalue,check_kcv[3],3))
	{	
		cls_show_msg("line %d:获取KCV失败(%02x,%02x%02x%02x)", __LINE__, keyid, keyvalue[0], keyvalue[1], keyvalue[2]);
		return;
	}

	//case5:获取AES 主密钥
	if((ret=SP100_Get_Key_Check(0x01,0x04,&keyid,keyvalue))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}
	if(keyid != 0x01 || memcmp(keyvalue,check_kcv[4],3))
	{	
		cls_show_msg("line %d:获取KCV失败(%02x,%02x%02x%02x)", __LINE__, keyid, keyvalue[0], keyvalue[1], keyvalue[2]);
		return;
	}

	//case6:获取AES PIN密钥id为0a,kcv为
	if((ret=SP100_Get_Key_Check(0x0a,0x05,&keyid,keyvalue))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}
	if(keyid != 0x0a || memcmp(keyvalue,check_kcv[5],3))
	{	
		cls_show_msg("line %d:获取KCV失败(%02x,%02x%02x%02x)", __LINE__, keyid, keyvalue[0], keyvalue[1], keyvalue[2]);
		return;
	}
	
	//case7:获取AES MAC密钥 id为0b,kcv为
	if((ret=SP100_Get_Key_Check(0x0b,0x06,&keyid,keyvalue))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}
	if(keyid != 0x0b || memcmp(keyvalue,check_kcv[6],3))
	{	
		cls_show_msg("line %d:获取KCV失败(%02x,%02x%02x%02x)", __LINE__, keyid, keyvalue[0], keyvalue[1], keyvalue[2]);
		return;
	}
	
	//case8:获取AES DATA密钥id为0c,kcv为
	if((ret=SP100_Get_Key_Check(0x0c,0x07,&keyid,keyvalue))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}
	if(keyid != 0x0c || memcmp(keyvalue,check_kcv[7],3))
	{	
		cls_show_msg("line %d:获取KCV失败(%02x,%02x%02x%02x)", __LINE__, keyid, keyvalue[0], keyvalue[1], keyvalue[2]);
		return;
	}

	cls_printf("异常测试中...");
	//case9:异常测试:key tpye 不在范围内的 预期返回失败
	if((ret=SP100_Get_Key_Check(0x04,0x08,&keyid,keyvalue))==SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}

	//case10:异常测试:keyid与key tpye不对应时预期返回失败
	if((ret=SP100_Get_Key_Check(0x04,0x03,&keyid,keyvalue))==SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}
	
	//case11:参数异常:不存在的主密钥id  只有返回06无其他返回值
	if((ret=SP100_Get_Key_Check(0x02,0x00,&keyid,keyvalue))==SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KCV失败(%04x)", __LINE__, ret);
		return;
	}
	cls_show_msg("获取KCV测试通过!");
}

static void test_get_dukpt_ksn(void)
{
	ushort ret = SP100_FAIL;
	uchar ksnvalue[10] = {0};

	cls_printf("获取KSN测试...");
	//参数异常:不存在的主密钥id  只有返回06无其他返回值
	if((ret=SP100_Get_Dukpt_Ksn(0x03,ksnvalue))==SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KSN失败(%04x)", __LINE__, ret);
		return;
	}
	
	//正常测试
	if((ret=SP100_Get_Dukpt_Ksn(0x07,ksnvalue))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取KSN失败(%04x)", __LINE__, ret);
		return;
	}	
	cls_show_msg("当前KSN值:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",ksnvalue[0],ksnvalue[1],ksnvalue[2],ksnvalue[3],ksnvalue[4],ksnvalue[5],ksnvalue[6],ksnvalue[7],ksnvalue[8],ksnvalue[9]);
	//当前KSN前不能够进行直接比对
	cls_show_msg("获取KSN测试通过!");
}

static void test_cancel_pin_entry(void)
{
	ushort ret = SP100_FAIL;
	int len = 0, answerlen = 0;
	uchar data[] = {0x33,0x32,0x2F,0x0a,0x02,0xD6,0x01,0x10,0xE9,0xB3,0x5D,0xE8,0xD3,0x95,0xE2,0xB1,0x3D,0x54,0xEB,0x24,0x20,0x9E,0x4F,0x91,0x00,0x00,0x0c,0x00,0x10,0x04,0x50,0x49,0x4E,0x20,0x45,0x6E,0x74,0x72,0x79,0x3A,0x1C,0x1C,0x1C};
	uchar frame_cmd[SP100_MAXLEN_FRAME] = {0}, answer[2] = {0}, recvbuf[32] = {0};
	uchar canceldata[] = {0x33,0x38,0x2F};

	cls_printf("PIN输入取消测试...");
	//前提没有pin输入直接进行pin取消的话预期成功
	if((ret=SP100_Cancel_Pin_Entry())!=SP100_SUCC)
	{	
		cls_show_msg("line %d:取消PIN输入失败(%04x)", __LINE__, ret);
		return;
	}

	//进行AES  PIN输入,未输入密码就直接进行pin取消 预期应该成功,使用组包 发包形式
	len = SP100_pack_data(data, sizeof(data), frame_cmd);
	if((ret=SP100_send_frame(frame_cmd, len))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:发包失败(%04x)", __LINE__, ret);
		return;
	}	
	if((ret=SP100_recv_ACKorNAK(answer, &answerlen, 0))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:收包失败(%04x)", __LINE__, ret);
		return;
	}
	if((ret=SP100_Cancel_Pin_Entry())!=SP100_SUCC)
	{	
		cls_show_msg("line %d:取消PIN输入失败(%04x)", __LINE__, ret);
		return;
	}

	cls_show_msg("取消PIN输入测试通过!");
}

//通用功能测试
static void test_option_select(void)
{
	ushort ret = SP100_FAIL;
	uchar display[] = {0x70,0x72,0x65,0x73,0x73,0x20,0x66,0x75,0x6E,0x63,0x74,0x69,0x6F,0x6E,0x1C,0x6B,0x65,0x79,0x21,0x1C,0x1C,0x1C};
	uchar fuckey = 0;
	
	//正常测试:测试确认键
	cls_printf("在SP100密码键盘上按确认键");
	if((ret=SP100_Option_Select(display,sizeof(display),&fuckey))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:功能键获取失败(%04x)", __LINE__, ret);
		return;
	}
	if(fuckey != 0x30)
	{	
		cls_show_msg("line %d:获取键值错误(%02x)", __LINE__, fuckey);
		return;
	}
	//测试F1键
	cls_printf("在SP100密码键盘上按F1键");
	if((ret=SP100_Option_Select(display,sizeof(display),&fuckey))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:功能键获取失败(%04x)", __LINE__, ret);
		return;
	}
	if(fuckey != 0x31)
	{	
		cls_show_msg("line %d:获取键值错误(%02x)", __LINE__, fuckey);
		return;
	}
	//测试F2键
	cls_printf("在SP100密码键盘上按F2键");
	if((ret=SP100_Option_Select(display,sizeof(display),&fuckey))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:功能键获取失败(%04x)", __LINE__, ret);
		return;
	}
	if(fuckey != 0x32)
	{	
		cls_show_msg("line %d:获取键值错误(%02x)", __LINE__, fuckey);
		return;
	}
	//测试F3键
	cls_printf("在SP100密码键盘上按F3键");
	if((ret=SP100_Option_Select(display,sizeof(display),&fuckey))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:功能键获取失败(%04x)", __LINE__, ret);
		return;
	}
	if(fuckey != 0x33)
	{	
		cls_show_msg("line %d:获取键值错误(%02x)", __LINE__, fuckey);
		return;
	}
	//测试退格键
	cls_printf("在SP100密码键盘上按退格键");
	if((ret=SP100_Option_Select(display,sizeof(display),&fuckey))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:功能键获取失败(%04x)", __LINE__, ret);
		return;
	}
	if(fuckey != 0x35)
	{	
		cls_show_msg("line %d:获取键值错误(%02x)", __LINE__, fuckey);
		return;
	}
	//测试取消键
	cls_printf("在SP100密码键盘上按取消键");
	if((ret=SP100_Option_Select(display,sizeof(display),&fuckey))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:功能键获取失败(%04x)", __LINE__, ret);
		return;
	}
	if(fuckey != 0x36)
	{	
		cls_show_msg("line %d:获取键值错误(%02x)", __LINE__, fuckey);
		return;
	}
	cls_show_msg("功能键测试通过!");
}

static void test_display_text(void)
{
	ushort ret = SP100_FAIL;
	uchar display[] = {0x77,0x65,0x6C,0x63,0x6F,0x6D,0x65,0x1C,0x62,0x61,0x62,0x79,0x1C,0x12,0x1C,0x13,0x1C};
	uchar display2[] = {0x4E,0x65,0x77,0x2A,0x24,0x26,0x40,0x6C,0x61,0x6E,0x64,0x1C,0x1C,0x1C,0x1C};
#if IRAN_CUSTOM
	//伊朗文:123(456)789
	uchar iran_dis[] = {0xDB,0xB0,0xDB,0xB1,0xDB,0xB2,0x28,0xDB,0xB3,0xDB,0xB4,0xDB,0xB5,0x29,0xDB,0xB6,0xDB,0xB7,0xDB,0xB8,0xDB,0xB9,0x1C,0xDB,0xB0,0xDB,0xB1,0xDB,0xB2,0x28,0xDB,0xB3,0xDB,0xB4,0xDB,0xB5,0x29,0xDB,0xB6,0xDB,0xB7,0xDB,0xB8,0xDB,0xB9,0x1C,
			0xDB,0xB0,0xDB,0xB1,0xDB,0xB2,0x28,0xDB,0xB3,0xDB,0xB4,0xDB,0xB5,0x29,0xDB,0xB6,0xDB,0xB7,0xDB,0xB8,0xDB,0xB9,0x1C,0xDB,0xB0,0xDB,0xB1,0xDB,0xB2,0x28,0xDB,0xB3,0xDB,0xB4,0xDB,0xB5,0x29,0xDB,0xB6,0xDB,0xB7,0xDB,0xB8,0xDB,0xB9,0x1C};
#endif

	//正常测试:两行显示
	cls_printf("SP100将显示:\nwelcome\nbaby\n");
	if((ret=SP100_Display_Text(display,sizeof(display)))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:显示文本失败(%04x)", __LINE__, ret);
		return;
	}
	if(cls_show_msg("SP100密码键盘上是否正确显示:\nwelcome\nbaby\n是[Enter],否[其他]")!=ENTER)
	{	
		cls_show_msg("line %d:显示文本失败(%04x)", __LINE__, ret);
		return;
	}
	//国外产品不显示中文 故只测英文和特殊字符
	cls_printf("SP100将显示:\nNew*$&@land");
	if((ret=SP100_Display_Text(display2,sizeof(display2)))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:显示文本失败(%04x)", __LINE__, ret);
		return;
	}
	if(cls_show_msg("SP100密码键盘上是否正确显示:\nNew*$&@land\n是[Enter],否[其他]")!=ENTER)
	{	
		cls_show_msg("line %d:显示文本失败(%04x)", __LINE__, ret);
		return;
	}
#if IRAN_CUSTOM
	//显示伊朗文(伊朗定制版本)
	if((ret=SP100_Display_Text(iran_dis,sizeof(iran_dis)))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:显示文本失败(%04x)", __LINE__, ret);
		return;
	}
	if(cls_show_msg("SP100密码键盘上是否正确显示两行123(456)789的伊朗文,是[Enter],否[其他]")!=ENTER)
	{	
		cls_show_msg("line %d:显示文本失败(%04x)", __LINE__, ret);
		return;
	}
#endif
	cls_show_msg("文本显示测试通过!");
}

static void test_load_image(void)
{
	ushort ret = SP100_FAIL;
	uchar imgdata[] = {
		0x42,0x4D,0x3E,0x02,0x00,0x00,0x00,0x00,
		0x00,0x00,0x3E,0x00,0x00,0x00,0x28,0x00,
		0x00,0x00,0x80,0x00,0x00,0x00,0x20,0x00,
		0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,
		0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFC,0x7F,0xFF,0x1F,0xFF,0xFF,0xFF,0x0F,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0x7F,0xFF,0xEF,0xFF,0xFF,0xFF,0x77,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,0x71,
		0xC3,0x08,0x38,0x77,0xFC,0x6A,0xE1,0x4E,
		0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xBD,
		0xFD,0xAF,0x7B,0x77,0xFF,0x75,0x6D,0xBE,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xDD,
		0xF3,0xB7,0x7C,0x6B,0xFF,0x75,0x71,0x8E,
		0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xDD,
		0xEF,0xB7,0x7B,0x6D,0xFF,0x74,0x6D,0xB6,
		0xDF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x6C,
		0xF1,0x87,0xBC,0x64,0x7F,0x39,0x71,0xC1,
		0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x6F,
		0xFF,0xFF,0xBF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x6F,
		0xFF,0xFF,0xBF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x1F,
		0x7F,0xFF,0x1F,0xFF,0xFF,0xDF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x77,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,
		0x06,0x78,0x61,0xFC,0x6A,0xE1,0x4E,0x3F,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xB6,0xBB,0x69,0xFF,0x75,0x6D,0xBE,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xBE,0xDC,0x6D,0xFF,0x75,0x71,0x8E,0x1F,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xBE,0xDB,0x6D,0xFF,0x74,0x6D,0xB6,0xDF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xDF,0x1C,0x72,0xFF,0x39,0x71,0xC1,0x1F,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xDF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xDF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xC7,0xFF,0xFE,0x7F,0xDF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
	uchar resp[2] = {0};

	//case1:参数异常测试id不在1-255范围内resp返回02 - General error
	if((ret=SP100_Load_Image(0x00,(uchar *)"\x2\x3E",(uchar *)imgdata,sizeof(imgdata),resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:图片装载失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_GENERAl_ERROR)
	{	
		cls_show_msg("line %d:图片装载失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	
	//case2:正常测试
	if((ret=SP100_Load_Image(0x01,(uchar *)"\x2\x3E",(uchar *)imgdata,sizeof(imgdata),resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:图片装载失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:图片装载失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	cls_show_msg("图片装载测试通过!");
}

static void test_display_image(void)
{
	ushort ret = SP100_FAIL;
	int  fp = 0;
	uint punSize = 0, readlen = 0;
	char buf[1024] = {0}; 
	uchar lenbuf[2] = {0};

	//使用NDK接口进行图片大小及数据的读取
	if(cls_show_msg("确保已下载图片%s到main所在目录下,ESC退出",LOGOBMP)==ESC)
		return;
	fp=NDK_FsOpen(LOGOBMP, "r");
	if((ret=NDK_FsFileSize(LOGOBMP, &punSize))!= NDK_OK)
	{	
		cls_show_msg("获取文件大小失败(ret:%d)",ret);
		return;
	}
	if((readlen=NDK_FsRead(fp,buf,punSize))!=punSize)
	{	
		cls_show_msg("读文件失败(readlen:%d)",readlen);
		return;
	}
	//关闭文件
	NDK_FsClose(fp);
	NDK_IntToC2(lenbuf,punSize);
	//id 为0
	if((ret=SP100_Display_Image(0x00,(uchar *)"\x00\x25",(uchar *)"\x00\x00",lenbuf,(uchar *)buf,punSize))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:显示图片失败(%04x)", __LINE__, ret);
		return;
	}
	if(cls_show_msg("SP100密码键盘上是否正确显示%s,是[Enter],否[其他]",LOGOBMP)!=ENTER)
	{	
		cls_show_msg("line %d:显示图片失败(%04x)", __LINE__, ret);
		return;
	}

	//正常测试
	if((ret=SP100_Display_Image(0x01,(uchar *)"\x00\x05",(uchar *)"\x00\x05",lenbuf,(uchar *)buf,punSize))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:显示图片失败(%04x)", __LINE__, ret);
		return;
	}
	if(cls_show_msg("SP100密码键盘上是否正确显示图片,是[Enter],否[其他]")!=ENTER)
	{	
		cls_show_msg("line %d:显示图片失败(%04x)", __LINE__, ret);
		return;
	}
	cls_show_msg("图片显示测试通过!");
}

static void test_device_attribute(void)
{
	ushort ret = SP100_FAIL;
	uchar value[2] = {0}, fwid[32] = {0};

	//正常测试
	if((ret=SP100_Device_Attribute(value,fwid))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取硬件属性失败(%04x)", __LINE__, ret);
		return;
	}
	//进行判断
	if(cls_show_msg("SP100密码键盘的硬件属性值:%02x%02x,请根据文档进行判断,正确[Enter],错误[其他]",value[0],value[1])!=ENTER)
	{	
		cls_show_msg("line %d:显示图片失败(%04x)", __LINE__, ret);
		return;
	}
	cls_show_msg("获取硬件属性测试通过!");
}
static void test_get_serial_num()
{
	int ret = -1;
	uchar num[16] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
	
	//正常测试
	if((ret=SP100_Get_Serial_Num(num))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取sn码失败(%04x)", __LINE__, ret);
		return;
	}
	cls_show_msg("获取到的SN码为:%s,请与自检里获取的进行比对",num);
	
	cls_show_msg("获取SN码测试通过!");
}

static void test_active_buzzer(void)
{
	ushort ret = SP100_FAIL;

	//参数异常测试tone不在范围内
	if((ret=SP100_Active_Buzzer(0x03,(uchar *)"\x01\x00"))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:蜂鸣器请求失败(%04x)", __LINE__, ret);
		return;
	}
	
	//正常测试:默认的HZ
	if((ret=SP100_Active_Buzzer(0x00,(uchar *)"\x01\x00"))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:蜂鸣器请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(cls_show_msg("听到蜂鸣器响,正确[Enter],错误[其他]")!=ENTER)
	{	
		cls_show_msg("line %d:蜂鸣器请求失败(%04x)", __LINE__, ret);
		return;
	}	
	//750Hz
	if((ret=SP100_Active_Buzzer(0x01,(uchar *)"\x01\x00"))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:蜂鸣器请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(cls_show_msg("听到蜂鸣器响,正确[Enter],错误[其他]")!=ENTER)
	{	
		cls_show_msg("line %d:蜂鸣器请求失败(%04x)", __LINE__, ret);
		return;
	}	
	//1500Hz
	if((ret=SP100_Active_Buzzer(0x02,(uchar *)"\x01\x00"))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:蜂鸣器请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(cls_show_msg("听到蜂鸣器响,正确[Enter],错误[其他]")!=ENTER)
	{	
		cls_show_msg("line %d:蜂鸣器请求失败(%04x)", __LINE__, ret);
		return;
	}		
	cls_show_msg("蜂鸣器测试通过!");
}

static void test_get_config(void)
{
	ushort ret = SP100_FAIL;
	int i = 0;
	uchar rate[2] = {0}, mode =0, beepctl = 0;
	char *bps[] = {"19200","2400", "9600",  "38400", "57600", "115200"};
	
	//正常测试
	if((ret=SP100_Get_Config(rate,&mode,&beepctl))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:获取波特率失败(%04x)", __LINE__, ret);
		return;
	}
	//进行判断
	switch(rate[1])
	{
		case 0x32:  
			i = 0;
			break;
		case 0x34:  
			i = 1;
			break;
		case 0x35:  
			i = 2;
			break;
		case 0x37:  
			i = 3;
			break;
		case 0x38:  
			i = 4;
			break;
		case 0x39:  
			i = 5;
			break;
		default:
			cls_show_msg("line %d:获取波特率失败(%02x%02x)", __LINE__,rate[0], rate[1]);
			return;
	}
	if(cls_show_msg("当前通讯波特率为:%s,正确[Enter],错误[其他]",bps[i])!=ENTER)
	{	
		cls_show_msg("line %d:获取波特率失败(%02x%02x)", __LINE__, rate[0], rate[1]);
		return;
	}
	cls_show_msg("波特率获取测试通过!");
}

static void test_sp100_reboot()
{
	ushort ret = SP100_FAIL;
	uchar resp[2] = {0};

	cls_printf("SP100密码键盘将会重启...");
	if((ret=SP100_Reboot(resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:重启请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:重启请求失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		return;
	}
	cls_show_msg("重启测试通过!");
}

//密码键盘配置:初始化，激活读卡器
static void test_card_config(void)
{
	/*private & local definition*/
	ushort ret = SP100_FAIL;
	uchar resp[2] = {0}, data[16] = {0};
	
	/*process body*/
	//未调用初始化进行check会失败(此点验证有点难)
	cls_show_msg("请在SP100感应区放置1张A卡,按任意键继续...");
	//case1:初始化后进行check预期成功
	if((ret=SP100_Init_Reader(resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:初始化读卡器失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:初始化读卡器失败(%02x%02x)", __LINE__, resp[0],resp[1]);
		goto ERR;
	}			
	if((ret=SP100_Check_Reader(resp,data))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:激活读卡器失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:激活读卡器失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}	
	
	//case2:继续重复check预期会失败
	if((ret=SP100_Check_Reader(resp,data))==SP100_SUCC)
	{	
		cls_show_msg("line %d:激活读卡器失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:激活读卡器失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}	

	//case3:关闭场强后再进行check预期会成功
	if((ret=SP100_Deactivate_Field(resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:关闭场强失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:关闭场强失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}	
	if((ret=SP100_Check_Reader(resp,data))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:激活读卡器失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:激活读卡器失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}	
	
	//测试后置:关闭场强
	if((ret=SP100_Deactivate_Field(resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:关闭场强失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:关闭场强失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}	
	
	cls_show_msg("密码键盘配置测试通过");
ERR:
	SP100_Deactivate_Field(resp);
	return;
}

//EMV交易测试
static void test_card_transaction(void)
{
	/*private & local definition*/
	ushort ret = SP100_FAIL;
	uchar cmddata[] = {0x9F,0x01,0x01,0x00,0x9F,0x02,0x06,0x00,0x00,0x00,0x00,0x00,0x01,0x9F,0x03,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x9A,0x03,0x17,0x06,0x20,0x9F,0x21,0x03,0x14,0x30,0x30,0x9C,0x01,0x00,0x1F,0x6B,0x02,0x00,0x00,0x5F,0x2A,0x02,0x00,0x00,0x9F,0x53,0x01,0x00};
	/*
	9F 01 01 00 （不用管）
	9F 02 06 00 00 00 00 00 01 （交易金额）
	9F 03 06 00 00 00 00 00 00 （其他金额）（不用管）
	9A 03 17 06 20 （交易日期 17年6月20）
	9F 21 03 14 30 30 （交易时间 14:30:30）
	9C 01 00 （交易类型：消费）
	1F 6B 02 00 00 （不用管）
	5F 2A 02 00 00 （不用管）
	9F 53 01 00 （不用管）
	*/
	uchar resp[2] = {0}, status = 0;
	
	/*process body*/
	cls_show_msg("请在SP100感应区放置1张银行卡,按任意键继续...");
	//case1:开始交易--停止交易
	if((ret=SP100_Start_Transaction(cmddata,sizeof(cmddata),resp,&status))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:开始交易请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:开始交易请求失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		return;
	}
	if((ret=SP100_Stop_Transaction(0x01,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:停止交易请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:停止交易请求失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		return;
	}
	
	//case2:开始交易--处理交易 --停止交易
	if((ret=SP100_Start_Transaction(cmddata,sizeof(cmddata),resp,&status))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:开始交易请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:开始交易请求失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		return;
	}
	if((ret=SP100_Process_Transaction((uchar *)"\x03\xE8",0x01,resp,&status))!=SP100_SUCC)  //超时10s
	{	
		cls_show_msg("line %d:处理交易请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:处理交易请求失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		return;
	}
	if((ret=SP100_Stop_Transaction(0x01,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:停止交易请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:停止交易请求失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		return;
	}
	
	//case3:开始交易-- 处理交易--开始交易--结束交易
	if((ret=SP100_Start_Transaction(cmddata,sizeof(cmddata),resp,&status))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:开始交易请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:开始交易请求失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		return;
	}
	if((ret=SP100_Process_Transaction((uchar *)"\x03\xE8",0x01,resp,&status))!=SP100_SUCC)  //超时10s
	{	
		cls_show_msg("line %d:处理交易请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:处理交易请求失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		return;
	}
	if((ret=SP100_Start_Transaction(cmddata,sizeof(cmddata),resp,&status))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:开始交易请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:开始交易请求失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		return;
	}
	if((ret=SP100_Stop_Transaction(0x01,resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:停止交易请求失败(%04x)", __LINE__, ret);
		return;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:停止交易请求失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		return;
	}
	
	cls_show_msg("EMV交易相关测试通过");
	return;
}

//射频卡相关功能测试
static void test_rfid_function(void)
{
	/*private & local definition*/
	ushort ret = SP100_FAIL;
	int i = 0;
	uchar cmddata[] = {0xBD,0x8C,0x64,0x66,0xF4,0x37,0x16,0x53};
	uchar resp[2] = {0}, resplen[2] = {0}, respdata[32] = {0};
	char *card[] = {"CPU_A", "CPU_B"};
	uchar plaintext[] = {0x00,0x84,0x00,0x00,0x08};
	
	/*process body*/
	//异常测试
	if((ret=SP100_Activate_Field(resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:场强开启失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:场强开启失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}
	//case1:进行检测卡片时 不放置卡片预期返回	无卡
	cls_show_msg("请确保SP100感应区无射频卡,按任意键继续...");
	if((ret=SP100_Check_Card((uchar *)"\x00\x20",resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:寻卡失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if(SP100_GETRESPVAL(resp) != SP100_CARD_NOT_PRESENT)
	{	
		cls_show_msg("line %d:寻卡失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}
	//case2:进行检测卡片时 放置2张卡片预期返回	无卡
	cls_show_msg("请在SP100感应区放置2张A卡,按任意键继续...");
	if((ret=SP100_Check_Card((uchar *)"\x00\x20",resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:寻卡失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if(SP100_GETRESPVAL(resp) != SP100_CARD_NOT_PRESENT)
	{	
		cls_show_msg("line %d:寻卡失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}
	if((ret=SP100_Deactivate_Field(resp))!=SP100_SUCC)
	{	
		cls_show_msg("line %d:关闭场强失败(%04x)", __LINE__, ret);
		goto ERR;
	}
	if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:关闭场强失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}

	//case3:正常流程测试(A卡) 
	for(i = 0;i<2;i++)
	{
		cls_show_msg("请确保SP100感应区有1张%s卡,按任意键继续...",card[i]);
		if((ret=SP100_Activate_Field(resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:场强开启失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg("line %d:场强开启失败(%02x%02x)", __LINE__,resp[0],resp[1]);
			goto ERR;
		}
		if((ret=SP100_Check_Card((uchar *)"\x00\x20",resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:寻卡失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg("line %d:寻卡失败(%02x%02x)", __LINE__,resp[0],resp[1]);
			goto ERR;
		}
		if((ret=SP100_Exchange_APDU(0x82,(uchar *)"\x00\x05",cmddata,sizeof(cmddata),resp,resplen,respdata))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:apdu交互失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		//cls_show_msg("line:%d,%02x:%02x",__LINE__,resp[0],resp[1]);
		if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg("line %d:apdu交互失败(%02x%02x)", __LINE__,resp[0],resp[1]);
			goto ERR;
		}
		if((ret=SP100_Deactivate_Field(resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:关闭场强失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg("line %d:关闭场强失败(%02x%02x)", __LINE__,resp[0],resp[1]);
			goto ERR;
		}
	}
	
	//case4:控制射频led 闪烁
	cls_printf("SP100射频蓝灯将会亮...");
	for(i=0;i<3;i++)
	{
		if((ret=SP100_Flash_LED(0x01,(uchar *)"\x01\x00",resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:关闭场强失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg("line %d:射频灯闪烁失败(%02x%02x)", __LINE__,resp[0],resp[1]);
			goto ERR;
		}
	}
	if(cls_show_msg("SP100密码键盘射频蓝灯亮,正确[Enter],错误[其他]")!=ENTER)
	{	
		cls_show_msg("line %d:射频灯闪烁失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}
	cls_printf("SP100射频黄灯将会亮...");
	for(i=0;i<3;i++)
	{
		if((ret=SP100_Flash_LED(0x02,(uchar *)"\x01\x00",resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:关闭场强失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg("line %d:射频灯闪烁失败(%02x%02x)", __LINE__,resp[0],resp[1]);
			goto ERR;
		}
	}
	if(cls_show_msg("SP100密码键盘射频黄灯亮,正确[Enter],错误[其他]")!=ENTER)
	{	
		cls_show_msg("line %d:射频灯闪烁失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}
	cls_printf("SP100射频绿灯将会亮...");
	for(i=0;i<3;i++)
	{
		if((ret=SP100_Flash_LED(0x03,(uchar *)"\x01\x00",resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:关闭场强失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg("line %d:射频灯闪烁失败(%02x%02x)", __LINE__,resp[0],resp[1]);
			goto ERR;
		}
	}
	if(cls_show_msg("SP100密码键盘射频绿灯亮,正确[Enter],错误[其他]")!=ENTER)
	{	
		cls_show_msg("line %d:射频灯闪烁失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}
	cls_printf("SP100射频红灯将会亮...");
	for(i=0;i<3;i++)
	{
		if((ret=SP100_Flash_LED(0x04,(uchar *)"\x01\x00",resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:关闭场强失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg("line %d:射频灯闪烁失败(%02x%02x)", __LINE__,resp[0],resp[1]);
			goto ERR;
		}
	}
	if(cls_show_msg("SP100密码键盘射频红灯亮,正确[Enter],错误[其他]")!=ENTER)
	{	
		cls_show_msg("line %d:射频灯闪烁失败(%02x%02x)", __LINE__,resp[0],resp[1]);
		goto ERR;
	}

#if IRAN_CUSTOM
	//case5:伊朗定制版本新增了明文的apdu指令的测试
	for(i=0;i<2;i++)
	{
		cls_show_msg("请确保SP100感应区有1张%s卡,按任意键继续...",card[i]);
		if((ret=SP100_Activate_Field(resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:场强开启失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg("line %d:场强开启失败(%02x%02x)", __LINE__,resp[0],resp[1]);
			goto ERR;
		}
		if((ret=SP100_Check_Card((uchar *)"\x00\x20",resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:寻卡失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg("line %d:寻卡失败(%02x%02x)", __LINE__,resp[0],resp[1]);
			goto ERR;
		}
		if((ret=SP100_Exchange_APDU_Plaintext(plaintext,sizeof(plaintext),resp,respdata))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:apdu交互失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		cls_show_msg("line:%d,%02x:%02x",__LINE__,resp[0],resp[1]);
		if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg("line %d:apdu交互失败(%02x%02x)", __LINE__,resp[0],resp[1]);
			goto ERR;
		}
		if((ret=SP100_Deactivate_Field(resp))!=SP100_SUCC)
		{	
			cls_show_msg("line %d:关闭场强失败(%04x)", __LINE__, ret);
			goto ERR;
		}
		if(SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg("line %d:关闭场强失败(%02x%02x)", __LINE__,resp[0],resp[1]);
			goto ERR;
		}
	}
#endif

	cls_show_msg("射频卡测试通过");
ERR:
	SP100_Deactivate_Field(resp);
	return;
}

static void test_rfid_ability(SMART_t type)
{
#define	nTestTime	(10)
	ushort ret = SP100_FAIL;
	int count = 0;	
	uchar cmddata[] = {0xBD,0x8C,0x64,0x66,0xF4,0x37,0x16,0x53};
	uchar resp[2] = {0}, resplen[2] = {0}, respdata[32] = {0};
	char *card[] = {"CPU_A", "CPU_B"};
	float rfid_time = 0.0;
	
	cls_show_msg("在SP100感应区放置一张%s卡后,任意键继续",card[type]);

	//开启场强
	if((ret=SP100_Activate_Field(resp))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{
		cls_show_msg1(g_keeptime, "line %d:开启场强失败(%04x,%02x%02x)", __LINE__, ret, resp[0], resp[1]);
		goto ERR;			
	}
	//检测卡片是否存在
	if((ret=SP100_Check_Card((uchar *)"\x00\x20",resp))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg1(g_keeptime, "line %d:寻卡失败(%04x,%02x%02x)", __LINE__,ret, resp[0], resp[1]);
		goto ERR;			
	}
	cls_printf("正在测试读写速度...\n");
	count = 0;
	lib_StartStopwatch();
	while(SP100_Exchange_APDU(0x82,(uchar *)"\x00\x05",cmddata,sizeof(cmddata),resp,resplen,respdata)==SP100_SUCC && SP100_GETRESPVAL(resp) == SP100_RESP_SUCC)
	{
		count++;
		if((rfid_time=lib_ReadStopwatch())>nTestTime)
			break;
	}
	if(rfid_time>nTestTime)
		cls_show_msg("%s卡每秒读卡:%.0f字节",card[type], count*4/rfid_time);
	else 
		cls_show_msg("line %d:读写中发生失败(cnt=%d)\n", __LINE__, count);
	//关闭场强
	if((ret=SP100_Deactivate_Field(resp))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg1(g_keeptime, "line %d:关闭场强失败(%04x,%02x%02x)", __LINE__, ret, resp[0], resp[1]);
		goto ERR;
	}
	cls_show_msg("%s卡读写性能测试通过!",card[type]);
ERR:
	SP100_Deactivate_Field(resp);
	return;
}
static void test_sp100_abnormity(void)
{
	ushort ret = SP100_FAIL;
	int datalen = 0;
	uchar frame_cmd[SP100_MAXLEN_FRAME] = {0};
	uchar resp[SP100_MAXLEN_DATA] = {0};

	cls_printf("异常测试,请耐心等待...");
	//case1:错误的帧格式
	//组包
	memset(frame_cmd, 0xaa, sizeof(frame_cmd));
	//发包
	if((ret=SP100_send_frame(frame_cmd, sizeof(frame_cmd)))!=SP100_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret);
		return;
	}
	//收包
	if((ret=SP100_recv_frame(resp, &datalen, 40/(strlen(g_devicecombps)-2)))!=SP100_ERR_FRAME_STX)//40是按1K多数据以BPS300来传输计算出的
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret);
		return;
	}

	//case2:不识别的命令
	//组包
	datalen = SP100_pack_data((uchar *)"\xff\xff\x2f\x30", strlen("\xff\xff\x2f\x30"), frame_cmd);
	//发包
	if((ret=SP100_send_frame(frame_cmd, datalen))!=SP100_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret);
		return;
	}
	//收包
	if((ret=SP100_recv_frame(resp, &datalen, 40/(strlen(g_devicecombps)-2)))!=SP100_ERR_FRAME_STX)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret);
		return;
	}

	//case3:命令头部错误 中部错误及尾部错误
	//头部错误(非02开头)
	if((ret=SP100_send_frame((uchar *)"\x03\x00\x03\x39\x30\x2F\x03\x25", 8))!=SP100_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret);
		return;
	}
	if((ret=SP100_recv_frame(resp, &datalen, 0))!=SP100_ERR_FRAME_STX)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret);
		return;
	}
	//中间错误
	if((ret=SP100_send_frame((uchar *)"\x02\x00\x03\x39\x30\x2D\x03\x27", 8))!=SP100_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret);
		return;
	}
	if((ret=SP100_recv_frame(resp, &datalen, 0))!=SP100_ERR_FRAME_STX)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret);
		return;
	}
	//尾部错误
	if((ret=SP100_send_frame((uchar *)"\x02\x00\x03\x39\x30\x2F\x03\x27", 8))!=SP100_SUCC)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret);
		return;
	}
	if((ret=SP100_recv_frame(resp, &datalen, 0))!=SP100_ERR_FRAME_STX)
	{
		cls_show_msg("line %d:测试失败(%04x)", __LINE__, ret);
		return;
	}
	cls_show_msg("异常测试通过!");
}
static void test_seckey_function(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	while(1)
       {
		keyin = cls_show_msg("1.密钥装载 2.PIN输入\n"
						     "3.加解密 4.MAC计算 \n"
						     "5.获取KCV 6.获取KSN\n"
						     "7.取消PIN输入\n");
		switch(keyin) 
		{
		case '1':
			test_load_key(0);
			break;
		case '2':
			test_pin_entry();
			break;
		case '3':
			test_dec_enc(0);
			break;	
		case '4':
			test_generate_mac(0);
			break;		
		case '5':
			test_get_key_check();
			break;
		case '6':
			test_get_dukpt_ksn();
			break;
		case '7':
			test_cancel_pin_entry();
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

static void test_common_function(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	while(1)
       {
		keyin = cls_show_msg("1.功能键 2.显示文字\n"
						     "3.加载图片 4.显示图片\n"
						     "5.硬件属性 6.蜂鸣器\n"
						     "7.SN码 8.获取波特率\n");   
		switch(keyin) 
		{
		case '1':
			test_option_select();
			break;
		case '2':
			test_display_text();
			break;
		case '3':
			test_load_image();
			break;	
		case '4':
			test_display_image();
			break;		
		case '5':
			test_device_attribute();
			break;
		case '6':
			test_active_buzzer();
			break;
		case '7':
			test_get_serial_num();
			break;
		case '8':
			test_get_config();
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

static void test_card_function(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	cls_show_msg("请确保SP100密码键盘已经以文件形式下载了ca.pk和kernel1.app,任意键继续");
	while(1)
       {
		keyin = cls_show_msg("1.配置 2.交易处理\n"
						     "3.非接卡 4.重启\n");  
		switch(keyin) 
		{
		case '1':
			test_card_config();
			break;
		case '2':
			test_card_transaction();
			break;
		case '3':
			test_rfid_function();
			break;	
		case '4':
			test_sp100_reboot();
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

void function_test(void)
{
	int keyin = 0;

	/*process body*/
	while(1)
    	{
        	keyin = cls_show_msg("1.密钥相关\n"
						 	"2.通用指令\n"
							 "3.非接卡\n"
						 	 "4.异常");
		switch(keyin) 
		{
		case '1':
			test_seckey_function();
			break;
		case '2':
			test_common_function();
			break;
		case '3':
			test_card_function();
			break;
		case '4':
			test_sp100_abnormity();
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
static void test_mac_press(void)
{
	int ret = -1, cnt = 100, i = 0, nLen = 0,aes_succ = 0,des_succ = 0,dukpt_succ = 0;
	uchar data[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
	uchar resp[2] = {0}, keyid;
	uchar macdata[16] = {0}, ksn[10] = {0};
	char CntStr[6] = {"100"};
	
	cls_show_msg1(2, "压力测试中...\n");
	cls_printf("输入压力测试次数:");
	if((ret=lib_kbgetinput(CntStr, 2, sizeof(CntStr)-1, (uint *)&nLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = MAXCNT;
	else
		cnt = atoi(CntStr);
	cls_printf("计算AES MAC压力测试中...");
	for(i=0; i<cnt; i++)
	{
		if((ret=SP100_Mac_Generation(0x0b,0x02,0x05,0x03,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			if(cls_show_msg1_record(FILENAME,FUNCNAME, g_keeptime,"line %d第%d次AES MAC计算失败(%04x,%02x%02x),按ESC键退出", __LINE__, i+1, ret, resp[0],resp[1])==ESC)
				break;
			else
				continue;
		}
		aes_succ++;
	}
	cls_show_msg1_record(FILENAME,FUNCNAME, g_keeptime,"AES MAC运算压力测试完毕!总共进行%d次,成功%d次",i,aes_succ);
	cls_printf("计算DES MAC压力测试中...");
	for(i=0; i<cnt; i++)
	{
		if((ret=SP100_Mac_Generation(0x05,0x00,0x00,0x03,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			if(cls_show_msg1_record(FILENAME,FUNCNAME, g_keeptime,"line %d第%d次DES MAC计算失败(%04x,%02x%02x),按ESC键退出", __LINE__, i+1, ret, resp[0],resp[1])==ESC)
				break;
			else
				continue;
		}
		des_succ++;
	}
	cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"DES MAC运算压力测试完毕!总共进行%d次,成功%d次",i,des_succ);
	cls_printf("计算DUKPT MAC压力测试中...");
	for(i=0; i<cnt; i++)
	{
		if((ret=SP100_Mac_Generation(0x07,0x01,0x00,0x03,(uchar *)"\x00\x08",data,sizeof(data),0x00,0x00,&keyid,resp,macdata,ksn))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			if(cls_show_msg1_record(FILENAME,FUNCNAME, g_keeptime,"line %d第%d次DUKPT MAC计算失败(%04x,%02x%02x),按ESC键退出", __LINE__, i+1, ret, resp[0],resp[1])==ESC)
				break;
			else
				continue;
		}
		dukpt_succ++;
	}
	cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"DUKPT MAC运算压力测试完毕!总共进行%d次,成功%d次",i,dukpt_succ);
}

static void test_transaction_press(void)
{
	int ret =-1, cnt = 0, bak = 0, succ = 0, nLen = 0;
	uchar cmddata[] = {0x9F,0x01,0x01,0x00,0x9F,0x02,0x06,0x00,0x00,0x00,0x00,0x00,0x01,0x9F,0x03,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x9A,0x03,0x17,0x06,0x20,0x9F,0x21,0x03,0x14,0x30,0x30,0x9C,0x01,0x00,0x1F,0x6B,0x02,0x00,0x00,0x5F,0x2A,0x02,0x00,0x00,0x9F,0x53,0x01,0x00};
	uchar resp[2] = {0}, status = 0;
	char CntStr[6] = {"1000"};
	
	cls_show_msg("在SP100上放置一张银行卡后任意键继续");
	cls_printf("输入压力测试次数:");
	if((ret=lib_kbgetinput(CntStr, 2, sizeof(CntStr)-1, (uint *)&nLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = MAXCNT;
	else
		bak = cnt = atoi(CntStr);

	while(cnt)
	{
		if (ESC==cls_show_msg1(1, "压力测试中...\n还剩%d次(已成功%d次),ESC中断测试", cnt, succ))
			break;
		cnt--;
		//开始交易
		if((ret=SP100_Start_Transaction(cmddata,sizeof(cmddata),resp,&status))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			if(cls_show_msg1_record(FILENAME,FUNCNAME, g_keeptime,"line %d第%d次开始交易请求失败(%04x,%02x%02x),按ESC键退出", __LINE__, bak-cnt, ret, resp[0],resp[1])==ESC)
				break;
			else
				continue;
		}
		//处理交易
		if((ret=SP100_Process_Transaction((uchar *)"\x03\xE8",0x01,resp,&status))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)  //超时10s
		{	
			if(cls_show_msg1_record(FILENAME,FUNCNAME, g_keeptime,"line %d第%d次处理交易请求失败(%04x,%02x%02x),按ESC键退出", __LINE__, bak-cnt, ret, resp[0],resp[1])==ESC)
				break;
			else
				continue;
		}
		//停止交易
		if((ret=SP100_Stop_Transaction(0x01,resp))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			if(cls_show_msg1_record(FILENAME,FUNCNAME, g_keeptime,"line %d第%d次停止交易请求失败(%04x,%02x%02x),按ESC键退出", __LINE__, bak-cnt, ret, resp[0],resp[1])==ESC)
				break;
			else
				continue;
		}
		succ++;
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"EMV交易压力测试完毕!总共进行%d次,成功%d次",bak-cnt,succ);
}

static void test_rfid_press(SMART_t type)
{
	int ret =-1, cnt = 0, bak = 0, succ = 0, nLen = 0;	
	uchar cmddata[] = {0xBD,0x8C,0x64,0x66,0xF4,0x37,0x16,0x53};
	uchar resp[2] = {0}, resplen[2] = {0}, respdata[32] = {0};
	char *card[] = {"CPU_A", "CPU_B"};
	char CntStr[6] = {"1000"};
	//uchar plaintext[] = {0x00,0x84,0x00,0x00,0x08};
	
	cls_show_msg("在SP100感应区放置一张%s卡后,任意键继续",card[type]);
	
	cls_printf("输入压力测试次数:");
	if((ret=lib_kbgetinput(CntStr, 2, sizeof(CntStr)-1, (uint *)&nLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = MAXCNT;
	else
		cnt = atoi(CntStr);
	
	while(cnt)
	{
		if (ESC==cls_show_msg1(2, "压力测试中...\n还剩%d次(已成功%d次),ESC中断测试", cnt, succ))
			break;
		cnt--;

		//开启场强
		if((ret=SP100_Activate_Field(resp))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次:开启场强失败(%04x,%02x%02x)", __LINE__, bak-cnt, ret, resp[0], resp[1]);
			continue;			
		}
		//检测卡片是否存在
		if((ret=SP100_Check_Card((uchar *)"\x00\x20",resp))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			SP100_Deactivate_Field(resp);
			cls_show_msg1(g_keeptime, "line %d:第%d次:寻卡失败(%04x,%02x%02x)", __LINE__, bak-cnt, ret, resp[0], resp[1]);
			continue;
		}
		//apdu交互
		if((ret=SP100_Exchange_APDU(0x82,(uchar *)"\x00\x05",cmddata,sizeof(cmddata),resp,resplen,respdata))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			SP100_Deactivate_Field(resp);
			cls_show_msg1(g_keeptime, "line %d:第%d次:读写失败(%04x,%02x%02x)", __LINE__, bak-cnt, ret, resp[0], resp[1]);
			continue;
		}
		//关闭场
		if((ret=SP100_Deactivate_Field(resp))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:关闭场强失败(%04x,%02x%02x)", __LINE__, bak-cnt, ret, resp[0], resp[1]);
			continue;
		}
		succ++;
	}
	//关闭场强
	if((ret=SP100_Deactivate_Field(resp))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:关闭场强失败(%04x,%02x%02x)", __LINE__, ret, resp[0], resp[1]);
		return;
	}	
	cls_show_msg_record(FILENAME,FUNCNAME,"射频%s卡综合压力测试完毕!总共进行%d次,成功%d次",card[type],bak-cnt,succ);
}

static void test_rfid_rw_press(SMART_t type)
{
	int ret =-1, cnt = 0, bak = 0, succ = 0, nLen = 0;	
	uchar cmddata[] = {0xBD,0x8C,0x64,0x66,0xF4,0x37,0x16,0x53};
	uchar resp[2] = {0}, resplen[2] = {0}, respdata[32] = {0};
	char *card[] = {"CPU_A", "CPU_B"};
	char CntStr[6] = {"3000"};
	//uchar plaintext[] = {0x00,0x84,0x00,0x00,0x08};
	
	cls_show_msg("在SP100感应区放置一张%s卡后,任意键继续",card[type]);
	
	cls_printf("输入读写压力测试次数:");
	if((ret=lib_kbgetinput(CntStr, 2, sizeof(CntStr)-1, (uint *)&nLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = MAXCNT;
	else
		bak = cnt = atoi(CntStr);

	while(cnt)
	{
		//开启场强
		if((ret=SP100_Activate_Field(resp))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{
			cnt--;
			if(ESC==cls_show_msg1(g_keeptime, "line %d:第%d次:开启场强失败(%04x,%02x%02x)", __LINE__, bak-cnt, ret, resp[0], resp[1]))
				break;
			else
				continue;
		}
		//检测卡片是否存在
		if((ret=SP100_Check_Card((uchar *)"\x00\x20",resp))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
		{	
			cnt--;
			if(ESC==cls_show_msg1(g_keeptime, "line %d:第%d次:寻卡失败(%04x,%02x%02x)", __LINE__, bak-cnt, ret, resp[0], resp[1]))
				break;
			else
				continue;
		}
		
		while(cnt)
		{
			if (ESC==cls_show_msg1(2, "压力测试中...\n还剩%d次(已成功%d次),请耐心等待", cnt, succ))
				break;
			cnt--;
			//apdu交互
			if((ret=SP100_Exchange_APDU(0x82,(uchar *)"\x00\x05",cmddata,sizeof(cmddata),resp,resplen,respdata))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
			{	
				SP100_Deactivate_Field(resp);
				cls_show_msg1(g_keeptime, "line %d:第%d次:读写失败(%04x,%02x%02x)", __LINE__, bak-cnt, ret, resp[0], resp[1]);
				break;
			}
			succ++;
		}
	}
	//关闭场强
	if((ret=SP100_Deactivate_Field(resp))!=SP100_SUCC || SP100_GETRESPVAL(resp) != SP100_RESP_SUCC)
	{	
		cls_show_msg("line %d:关闭场强失败(%04x,%02x%02x)", __LINE__, ret, resp[0], resp[1]);
		return;
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"射频%s卡读写压力测试完毕!总共进行%d次,成功%d次",card[type],bak-cnt,succ);
}

void rfid_press(void)
{
	int keyin = 0;
	SMART_t type = CPU_A;	
	
	/*process body*/
	cls_show_msg("测试前先配置,注意:只支持A卡和B卡,任意键继续");
	while(1)
    	{
        	keyin = cls_show_msg("1.综合压力\n"
						 "2.读写压力\n"
						 "0.配置");
		switch(keyin) 
		{
		case '1':
			test_rfid_press(type);
			break;
		case '2':
			test_rfid_rw_press(type);
			break;
		case '0':
			type = select_smarttype();
			cls_show_msg1(2,"配置成功!");
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

void press_test(void)
{
	int keyin = 0;

	/*process body*/
	while(1)
    	{
        	keyin = cls_show_msg("1.计算MAC\n"
						 "2.EMV交易\n"
						 "3.非接卡");
		switch(keyin) 
		{
		case '1':
			test_mac_press();
			break;
		case '2':
			test_transaction_press();
			break;
		case '3':
			rfid_press();
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

void seckey_ability_test(void)
{
	int keyin = 0;

	/*process body*/
	while(1)
    	{
        	keyin = cls_show_msg("1.装载密钥\n"
						 "2.数据加解密\n"
						 "3.计算MAC\n");
		switch(keyin) 
		{
		case '1':
			test_load_key(1);
			break;
		case '2':
			test_dec_enc(1);
			break;
		case '3':
			test_generate_mac(1);
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

void ability_test(void)
{
	int keyin = 0;
	SMART_t type = CPU_A;	
	
	/*process body*/
	while(1)
    	{
        	keyin = cls_show_msg("1.密钥相关\n"
						     "2.非接卡\n"
						     "0.配置");
		switch(keyin) 
		{
		case '1':
			seckey_ability_test();
			break;
		case '2':
			test_rfid_ability(type);
			break;
		case '0':
			type = select_smarttype();
			cls_show_msg1(2,"配置成功!");
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

static int test_bps_conf(void)
{
	/*private & local definition*/
	int nKeyin = 0, loop = 0, ret = -1;
	char *bps[] = {"19200","2400", "9600",  "38400", "57600", "115200"};
	static int first = TRUE;//首次开机上电的标志
	char *bpsrate[6] = {"\x31\x32","\x31\x34","\x31\x35","\x31\x37","\x31\x38","\x31\x39"};
	uchar  resp[2] = {0};
	
	/*process body*/
	// 1.配置与SP100连接的POS串口参数
	cls_show_msg1(2, "将选择外接设备连接的物理口");
	g_devicecom = select_port();

	if(g_devicecom!=PORT_NUM_USB_HOST)
	{
		// 2.配置SP100串口参数
		cls_show_msg1(2, "将进行外接设备通讯波特率bps的配置");
		do
		{
			nKeyin = cls_show_msg("1.19200  2.2400\n3.9600   4.38400\n5.57600  6.115200");
#define MAXMENUITEM ('6')

			switch(nKeyin)
			{
			case ESC:
				cls_show_msg1(2, "将设置为BPS115200(默认)...");
				nKeyin = '6';//转义
			case '1':
			case '2':
			case '3':
			case '4':	
			case '5':	
			case MAXMENUITEM:
				if(NDK_PortOpen(g_devicecom, (first==TRUE)?"115200":g_devicecombps)!=NDK_OK)
				{
					cls_show_msg1(2, "line %d:初始化POS串口失败", __LINE__);
					return FAIL;
				}
				cls_printf("设置外设BPS中...");
				if(SP100_Config_Load((uchar *)bpsrate[nKeyin-'1'],0x30,0x32,resp)!=SP100_SUCC)
				{
					cls_printf("外设BPS自动适配中...请耐心等待");
					//若不成功,对所有可能的BPS进行尝试
					for(; loop<SZ_ARRAY(bps); loop++)
					{
						NDK_PortOpen(g_devicecom, bps[loop]);
						if(SP100_Config_Load((uchar *)bpsrate[nKeyin-'1'],0x30,0x32,resp)==SP100_SUCC)
							break;
					}
					if(loop==SZ_ARRAY(bps))
					{
						cls_show_msg("line %d:设置SP100的BPS失败,请重启后再尝试", __LINE__);
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
		
		// 3.根据SP100当前BPS调整POS串口的BPS
		if((ret=NDK_PortOpen(g_devicecom, bps[nKeyin-'1']))!=NDK_OK)
		{
			cls_show_msg("line %d:初始化POS串口失败ret=%d", __LINE__, ret);
			return FAIL;
		}
		strcpy(g_devicecombps, bps[nKeyin-'1']);//g_bps根据配置进行调整备份
		first = FALSE;
	}
	else 
	{
		if((ret=NDK_PortOpen(g_devicecom, "115200,8,N,1,20"))!=NDK_OK) 
		{
			cls_show_msg("line %d:初始化POS串口失败ret=%d", __LINE__, ret);
			return FAIL;
		}
	}
	
	return SUCC;
}

void systest94()
{
	int keyin = 0;

	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);
	/*
	测试前先进行主密钥的安装,可以使用母pos安装,也可以使用mapp_SP100_initkey.NLP安装
	安装的主密钥id以及数据一定要是如下,否则会影响密钥相关的测试,测试数据都是根据这些主密钥设计的
	AES tmk id=1 32个0x00
	DES tmk id=1 16个0x13
	DUKPT id=7 D5D44FF720683D0D70BE252818E2A18A  ksn 10个0x00
	*/
	//测试前置 先进行主密钥安装,安装一次即可
	cls_show_msg("请确保测试前已经对SP100密码键盘下载且运行了mapp_SP100_initkey.NLP,装密钥后还需重新下测试固件");
	cls_show_msg("测试前先进行配置,任意键继续");
	
	while(1)
    	{
        	keyin = cls_show_msg("1.功能 2.压力\n"
						     "3.性能 0.配置\n");
		switch(keyin) 
		{
		case '1':
			function_test();
			break;
		case '2':
			press_test();
			break;
		case '3':
			ability_test();
			break;
		case '0':
			cls_show_msg("请确保POS与外设已连接!任意键继续...");
			if(test_bps_conf()!=SUCC)
				cls_show_msg("line %d:配置失败!请重启POS和外设后重新配置.", __LINE__);
			else
				cls_show_msg1(2, "链路配置成功!");
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


