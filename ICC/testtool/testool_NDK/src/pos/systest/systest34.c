/************************************************************************
*
* copyright(c)2005-2012 Newland Co. Ltd. All rights reserved
* module			: 卡
* file name		:
* Author 			: 
* version			: V1.0
* DATE			: 
* directory 		: 
* description		: MEMORY卡测试
* related document	: 程序员参考手册
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h" 

/*---------------constants/macro definition---------------------*/
#define	TESTITEM		"MEMORY卡测试"
#define	DEFAULT_CNT_STR	"200"	
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)	//(200)
#define ITEMPERPAGE 4
#define	DEFAULT_CNT_STR2	"50"	
#define	DEFAULT_CNT_VLE2	atoi(DEFAULT_CNT_STR2)	//(200)

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
*			  	          		   	 	    
*****************************************************************/
//指令封装读写数据,返回recbuf的长度指令格式
static int rw_data(char *head,int headlen,char *data,int datalen,char *recbuf,MEMORY_t type)
{
	/*private & local definition*/
	uchar sendbuf[300] = {0},buf[300] = {0};
	int sendlen = 0,rlen = 0;
	EM_ICTYPE cardId[]={ICTYPE_M_1_1,ICTYPE_M_1_2,ICTYPE_M_1_4,ICTYPE_M_1_8,ICTYPE_M_1_16,ICTYPE_M_1_32,ICTYPE_M_1_64,
		ICTYPE_M_2,ICTYPE_M_3,ICTYPE_M_3,ICTYPE_M_4,ICTYPE_M_5,ICTYPE_M_6,ICTYPE_M_7};
    
	/*process body*/
	memcpy(sendbuf,head,headlen);
	sendbuf[headlen] = datalen&0xff;
	if(NULL == data)
		sendlen = headlen+1;
	else
	{
		memcpy(sendbuf+headlen+1,data,datalen);
		sendlen = headlen+1+datalen;
	}
	if( NDK_Iccrw(cardId[type]/*ICTYPE_IC*/,sendlen,sendbuf,&rlen,buf)!=NDK_OK)			
		return FAIL;  
	else
	{
		memcpy(recbuf,buf,rlen/*(int)buf[0]*/); 
		return rlen;
	}
}

//读命令:cla:一般情况下是固定0x00，但是如果是税控卡的话就是不确定的数值了，len为要读取的数据长度
//p1,p2根据不同的卡有不同的意义，但是都是这两个参数。recbuf为返回的数据
static int memory_cmd_r(char cla,char p1,char p2,int len,char *recbuf,MEMORY_t type)
{
	/*private & local definition*/
	char head[300] = {0};
	
	/*process body*/
	head[0] = cla;
	head[1] = 0xB0;
	head[2] = p1;
	head[3] = p2;

	return rw_data(head,4,NULL,len,recbuf,type);
}

//写命令cla:一般情况下是固定0x00，但是如果是税控卡的话就是不确定的数值了，data为要写的数据，len为data的长度
static int memory_cmd_w(char cla,char p1,char p2,char *data,int len,char *recbuf,MEMORY_t type)
{
	/*private & local definition*/
	char head[300] = {0};

	/*process body*/
	head[0] = cla;
	head[1] = 0xD0;
	head[2] = p1;
	head[3] = p2;
	return rw_data(head,4,data,len,recbuf,type);
}

//校验密钥命令:data为密钥，len为密钥长度
static int memory_cmd_k(char p1,char p2,char *data,int len,char *recbuf,MEMORY_t type)
{
	/*private & local definition*/
	char head[300] = {0};

	/*process body*/
	head[0] = 0x00;
	head[1] = 0x20;
	head[2] = p1;
	head[3] = p2;
	return rw_data(head,4,data,len,recbuf,type);
}

//认证校验:data为认证密钥，len为认证密钥长度
static int memory_cmd_a(char p1,char p2,char *data,int len,char *recbuf,MEMORY_t type)
{
	/*private & local definition*/
	char head[300] = {0};

	/*process body*/
	head[0] = 0x00;
	head[1] = 0x82;
	head[2] = p1;
	head[3] = p2;
	return rw_data(head,4,data,len,recbuf,type);
}

//擦除:len为要擦除的字节数，len为要擦除的长度
static int memory_cmd_e(char p1,char p2,int len,char *recbuf,MEMORY_t type)
{
	/*private & local definition*/
	char head[300] = {0};

	/*process body*/
	head[0] = 0x00;
	head[1] = 0x0E;
	head[2] = p1;
	head[3] = p2;
	return rw_data(head,4,NULL,len,recbuf,type);
}

//测试4442函数 
static unsigned char test_4432_42_func(MEMORY_t type)
{
	/*private & local definition*/
	char keydata[] = "\xff\xff\xff";
	char writedata[] = "\x01\x02\x03\x04";
	char recbuf[300]={0};
	int reclen = 0;
	ushort sw=0;
	
	/*process body*/
	//case1:4442卡读写用户区测试
	//step1:校验密钥
	if((reclen=memory_cmd_k(0x00,0x00,keydata, strlen(keydata),recbuf,SLE4432_42))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1(g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step2:写数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x00,0x20,writedata,strlen(writedata),recbuf,SLE4432_42))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:写数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step3:读数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_r(0x00,0x00,0x20,strlen(writedata),recbuf,SLE4432_42))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	if(memcmp(writedata,recbuf,reclen-2))
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:比较数据区数据失败", __LINE__);
		return FAIL;
	}
	//case2:4442卡读写用户区测
	//step1:写保护区测试
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x80,0x00,writedata,strlen(writedata),recbuf,SLE4432_42))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:写数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step2:读保护区测试
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_r(0x00,0x80,0x00,strlen(writedata),recbuf,SLE4432_42))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	return SUCC;
}

//1608测试主函数
static unsigned char test_1608_func(MEMORY_t type)
{
	/*private & local definition*/
	char keydata[] = "\xFF\xFF\xFF";//\x01\x02\x03   //新买的MEMORY卡片 密钥为全ff
	char akeydata[] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
	char writedata[] = "\x01\x02\x03\x04",recbuf[300] = {0};
	int reclen = 0;
	ushort sw=0;
	
	/*process body*/
	//case:1608卡的读写操作测试
	//step1:认证测试
	if((reclen=memory_cmd_a(0x00,0x00,akeydata,strlen(akeydata),recbuf,AT88SC1608))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step2:校验密钥
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_k(0x03,0x00,keydata,strlen(keydata),recbuf,AT88SC1608))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验读KEY失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_k(0x03,0x08,keydata,strlen(keydata),recbuf,AT88SC1608))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验写KEY失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}	
	//step3:用户页面选择操作
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x02,0x01,NULL,0,recbuf,AT88SC1608))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验写KEY失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step4:写操作
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x00,0x0E,writedata,strlen(writedata),recbuf,AT88SC1608))==FAIL)//地址从 0x20改为0x0E是因为写是16字节的写，当到地址16之后要翻页需要重新写地址,是为了验证是否能够正确翻页写，修改该用例：4字节数据写于其实地址为14的位置，在16的时候底层翻页处理 jiangym 20150325 modify
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:写数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step5:读操作
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_r(0x00,0x01,0x0E,strlen(writedata),recbuf,AT88SC1608))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step6:比较数据
	if(memcmp(writedata,recbuf,reclen-2))
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:比较数据失败", __LINE__);
		return FAIL;
	}
	return SUCC;

}

//sle4418_28测试
static unsigned char test_4418_28_func(MEMORY_t type)
{
	/*private & local definition*/
	char keydata[] = "\xFF\xFF";
	char writedata[] = "\x31\x32\x33\x34",recbuf[12]={0},recbufold[12]={0};
	int reclen = 0;
	ushort sw=0;
	
	/*process body*/
	//case1:4418_28卡读写测试，写无保护位数据
	//step1:校验密钥
	if((reclen=memory_cmd_k(0x00,0x00,keydata, strlen(keydata),recbuf,SLE4418_28))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	
	//step2:读卡上的原始数据,保护动作
	memset(recbufold,0,sizeof(recbufold));
	if((reclen=memory_cmd_r(0x00,0x02,0xff,4,recbufold,SLE4418_28))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbufold+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//为保证原始数据和测试数据要不一致
	if(!memcmp(recbufold,writedata,sizeof(writedata)))
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:原始数据和测试数据一致,请修改测试数据", __LINE__);
		return FAIL;
	}
	
	//step3:写无保护位数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x02,0xff,writedata,strlen(writedata),recbuf,SLE4418_28))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:写数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	
	//step4:读无保护位数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_r(0x00,0x02,0xff,strlen(writedata),recbuf,SLE4418_28))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//数据比较
	if(memcmp(writedata,recbuf,reclen-2))
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:比较数据失败", __LINE__);
		return FAIL;
	}
	
	//step5:写回卡上的原始数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x02,0xff,recbufold,strlen(recbufold)-2,recbuf,SLE4418_28))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:写数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	return SUCC;  
}
//sle5528测试
static unsigned char test_5528_func(MEMORY_t type)
{
	/*private & local definition*/
	char keydata[] = "\xff\xff";//"\xa9\xbc";  新卡密钥为全ffff  郑德才
	char writedata[] = "\x31\x32\x33\x34",recbuf[12]={0},recbufold[12]={0};
	int reclen = 0;
	ushort sw=0;
	
	/*process body*/
	//case1:5528卡读写测试，写无保护位数据
	//step1:校验密钥
	if((reclen=memory_cmd_k(0x00,0x00,keydata, strlen(keydata),recbuf,SLE5528))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(a9 bc)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	
	//step2:读卡上原始数据,保护动作
	memset(recbufold,0,sizeof(recbufold));
	if((reclen=memory_cmd_r(0x00,0x02,0xff,4,recbufold,SLE5528))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(a9 bc)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbufold+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//为保证原始数据和测试数据要不一致
	if(!memcmp(recbufold,writedata,sizeof(writedata)))
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:原始数据和测试数据一致,请修改测试数据", __LINE__);
		return FAIL;
	}
	
	//step3:写无保护位数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x02,0xff,writedata,strlen(writedata),recbuf,SLE5528))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:写数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	
	//step4:读无保护位数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_r(0x00,0x02,0xff,strlen(writedata),recbuf,SLE5528))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//数据比较
	if(memcmp(writedata,recbuf,reclen-2))
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:比较数据失败", __LINE__);
		return FAIL;
	}
	
	//step5:写回卡上的原始数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x02,0xff,recbufold,strlen(recbufold)-2,recbuf,SLE5528))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:写数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	return SUCC;  
}

//AT24C64卡的测试入口  同样适合AT24C128和AT24C256
static unsigned char test_24C_func(MEMORY_t type)
{
	/*private & local definition*/
	char writedata[] = "\x31\x32\x33\x34",recbuf[300] = {0};
	int reclen = 0;
	ushort sw=0;
	
	/*process body*/
	//case:at24c64卡的读写操作，同样适用(AT24C128和AT24C256)
	//写数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x00,0x01,writedata,strlen(writedata),recbuf,type))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:写数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//读数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_r(0x00,0x00,0x01,strlen(writedata),recbuf,type))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//数据比较
	if(memcmp(writedata,recbuf,reclen-2))
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:比较失败", __LINE__);
		return FAIL;
	}
	return SUCC;
}

#if 0
//AT45DB041税控卡测试入口
static unsigned char test_at45_func(void)
{
	/*private & local definition*/
	char writedata[] = "\x31\x32\x33\x34",recbuf[300] = {0};
	int reclen = 0;
	ushort sw=0;
	
	/*process body*/
	//case:at45db041税控卡的读写测试
	//写数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x00,0x00,writedata,strlen(writedata),recbuf))==FAIL)
	{
		cls_show_msg1(g_keeptime, "line %d:写数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1(g_keeptime, "line %d:写数据失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//读数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_r(0x00,0x00,0x00,strlen(writedata),recbuf))==FAIL)
	{
		cls_show_msg1(g_keeptime, "line %d:读数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1(g_keeptime, "line %d:读数据失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//数据比较
	if(memcmp(writedata,recbuf,reclen-2))
	{
		cls_show_msg1(g_keeptime, "line %d:比较失败", __LINE__);
		return FAIL;
	}
	return SUCC;

}
#endif

//测试102存储卡
static unsigned char test_102_func(MEMORY_t type)
{
	/*private & local definition*/
	char writedata[] = "\x31\x32",recbuf[300] = {0};
	int reclen = 0;
	ushort sw=0;
	char keydata[] = "\xf0\xf0";//102卡片的默认密钥,如果默认密钥不是这个密钥则错4次卡片会锁死,所以新买的卡片要问清楚密钥
	
	/*process body*/
	//case:102存储卡的读写测试
	//step1:擦除测试区
	memset(recbuf,0,sizeof(recbuf)); 
	if((reclen=memory_cmd_e(0x00,0xB0,strlen(writedata),recbuf,AT88SC102))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:擦除失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step2:写数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x00,0xB0,writedata,strlen(writedata),recbuf,AT88SC102))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:写数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step3:读数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_r(0x00,0x00,0xB0,strlen(writedata),recbuf,AT88SC102))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step4:数据比较
	if(memcmp(writedata,recbuf,reclen-2))
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:比较失败", __LINE__);
		return FAIL;
	}

	//case2:102卡片密钥修改
	//step1:校验密钥
	if((reclen=memory_cmd_k(0x00,0x00,keydata, strlen(keydata),recbuf,AT88SC102))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(f0 f0)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1(g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step2:擦除密钥
	memset(recbuf,0,sizeof(recbuf)); 
	if((reclen=memory_cmd_e(0x00,0x0a,strlen(keydata),recbuf,AT88SC102))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:擦除失败(f0 f0)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step3:读取密钥
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_r(0x00,0x00,0x0A,strlen(keydata),recbuf,AT88SC102))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step4:修改密钥
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x00,0x0a,keydata,strlen(keydata),recbuf,AT88SC102))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:写数据区失败(f0 f0)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step5:读取密钥
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_r(0x00,0x00,0x0A,strlen(keydata),recbuf,AT88SC102))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(f0 f0)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step6:数据比较
	if(memcmp(keydata,recbuf,reclen-2))
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:比较失败", __LINE__);
		return FAIL;
	}
	return SUCC;
}

//测试1604测试卡
static unsigned char test_1604_func(MEMORY_t type)
{
	/*private & local definition*/
	char writedata[] = "\x31\x32",recbuf[300] = {0};
	int reclen = 0;
	ushort sw=0;
	
	/*process body*/
	//case:1604存储卡的读写测试
	//step1:擦除测试区
	//擦除0xD4-0xD7的内容，传0xD6无法擦除0xD5的内容  20180706 modify
	memset(recbuf,0,sizeof(recbuf)); 
	if((reclen=memory_cmd_e(0x07,0xD4/*0xD6*/,4/*strlen(writedata)*/,recbuf,AT88SC1604))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:擦除失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step2:写数据  (新的卡片写数据区域有限制，故D5改成D6  20180621 modify)
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x07,0xD5/*0xD6*/,writedata,strlen(writedata),recbuf,AT88SC1604))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:写数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step3:读数据
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_r(0x00,0x07,0xD5/*0xD6*/,strlen(writedata),recbuf,AT88SC1604))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step4:数据比较
	if(memcmp(writedata,recbuf,reclen-2))
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:比较失败", __LINE__);
		return FAIL;
	}
	return SUCC;
}
//测试AT88SC153测试卡
static unsigned char test_153_func(MEMORY_t type)
{
	/*private & local definition*/
	char keydata[] = "\xFF\xFF\xFF";//\x01\x02\x03
	char akeydata[] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF";
	char writedata[] = "\x01\x02\x03\x04\x11\x12\x13\x14\x21\x22\x23\x24",recbuf[300] = {0};
	int reclen = 0;
	ushort sw=0;
	
	/*process body*/
	//case:153卡的读写操作测试
	//step1:认证测试(可以不认证)
	if((reclen=memory_cmd_a(0x00,0x00,akeydata,strlen(akeydata),recbuf,AT88SC153))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step2:校验密钥
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_k(0x0B,0x34,keydata,strlen(keydata),recbuf,AT88SC153))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验读KEY失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_k(0x03,0x30,keydata,strlen(keydata),recbuf,AT88SC153))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验写KEY失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step3:写操作
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_w(0x00,0x00,0x06,writedata,strlen(writedata),recbuf,AT88SC153))==FAIL)//地址使用0x06是因为写是8字节的写，当到地址8或F之后要翻页需要重新写地址,为了验证是否能够正确的写故写了12字节的数据
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:写数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step4:读操作
	memset(recbuf,0,sizeof(recbuf));
	if((reclen=memory_cmd_r(0x00,0x01,0x06,strlen(writedata),recbuf,AT88SC153))==FAIL)
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:读数据区失败(ff ff)", __LINE__);
		return FAIL;
	}
	else
	{
		if((sw=SMART_GETSW(recbuf+reclen-2))!=0x9000)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:校验KEY失败(0x%04x)", __LINE__,sw);
			return FAIL;
		}
	}
	//step5:比较数据
	if(memcmp(writedata,recbuf,reclen-2))
	{
		cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:比较数据失败", __LINE__);
		return FAIL;
	}
	return SUCC;
}

//异常测试
void memroy_abnormal_test(MEMORY_t type)
{
	/*private & local definition*/
	uchar buf[300] = {0};
	int ret = 0,pnAtrlen = 0;
	
	char *card[] = {"AT24C01","AT24C02","AT24C04","AT24C08","AT24C016","AT24C032","AT24C064","SLE4432_42","SLE4418_28","SLE5528","AT88SC102","AT88SC1604","AT88SC1608","AT88SC153"};
	EM_ICTYPE cardId[]={ICTYPE_M_1_1,ICTYPE_M_1_2,ICTYPE_M_1_4,ICTYPE_M_1_8,ICTYPE_M_1_16,ICTYPE_M_1_32,ICTYPE_M_1_64,ICTYPE_M_2,ICTYPE_M_3,ICTYPE_M_3,ICTYPE_M_4,ICTYPE_M_5,ICTYPE_M_6,ICTYPE_M_7};
	EM_ICTYPE errcardID[]={ICTYPE_M_2,ICTYPE_M_3,ICTYPE_M_4,ICTYPE_M_5,ICTYPE_M_6,ICTYPE_M_6,ICTYPE_M_5,ICTYPE_M_1,ICTYPE_M_1,ICTYPE_M_5,ICTYPE_M_3,ICTYPE_M_6,ICTYPE_M_1,ICTYPE_M_5};//错误的卡类型
	unsigned char (*pfunc[])(MEMORY_t type) = 
	{
		test_24C_func,
		test_24C_func,
		test_24C_func,
		test_24C_func, 
		test_24C_func,
		test_24C_func,
		test_24C_func,
		test_4432_42_func,
		test_4418_28_func,
		test_5528_func,
		test_102_func,
		test_1604_func,
		test_1608_func,
		test_153_func
	};
	
	/*process body*/
	cls_printf("异常测试开始\n");

	//case1:测试在卡座上没有卡的情况下，上电是否成功?(上电应该是失败的)
	cls_show_msg("请将%s卡从卡座上移走,按任意键继续...",card[type]);
	//NDK_IccSetType(cardId[type]);
	if ((ret=NDK_IccPowerUp(cardId[type], buf,&pnAtrlen))!=NDK_ERR/*-64*/)
	{
		NDK_IccPowerDown(cardId[type]);	
		cls_show_msg1(5,"line %d:错误返回值测试失败(预期:-1, 实际:%d)", __LINE__, ret);
		return;
	}
	cls_show_msg1(2, "子用例测试通过!");

	//case2:测试的上电的卡类型传为IC卡类型和插入的卡片类型不一致(上电一定失败)
	cls_show_msg("请确保%s卡已经插上,按任意键继续...",card[type]);
	//DK_IccSetType(ICTYPE_IC);
	if ((ret=NDK_IccPowerUp(ICTYPE_IC, buf,&pnAtrlen))==NDK_OK)
	{
		NDK_IccPowerDown(ICTYPE_IC);	
		cls_show_msg1(5,"line %d:异常测试失败,错误类型上电成功!", __LINE__);
		return;
	}
	cls_show_msg1(2, "子用例测试通过!");

	//case3:测试的卡类型传入错误插入的卡片为MEM卡，应该失败
	//传入错误的memory卡类型，上电有可能失败(返回NDK_ERR_IOCTL)，也有可能成功
	cls_show_msg("请确保%s卡已经插上,按任意键继续...",card[type]);
	if ((ret=NDK_IccPowerUp(errcardID[type], buf,&pnAtrlen))==NDK_OK)
	{
		//上电能成功，读写也应该失败
		if(SUCC == pfunc[type](type))
		{
			if(type==AT88SC153)//||type==AT88SC1608 type==SLE4418_28||type==SLE5528||   由于SLE4428/SLE5528卡，不需要经过严格的上电时序（即只要给卡片供电），就能正常读写卡片(邱涛解释)20171023 modify
			{
				NDK_IccPowerDown(errcardID[type]);
				cls_show_msg1(5, "line %d:读写数据应该失败", __LINE__);
				return;
			}
			else
				;//郭忱解释AT24CXX、SLE4432_42、AT88SC102、AT88SC1604、AT88SC1608卡上电只需要满足vcc拉高就好，故使用任何类型MEM卡对其上电读写都成功,因此102卡在读写成功情况下不报错
		}
		NDK_IccPowerDown(errcardID[type]);
	}
	cls_show_msg1(2, "子用例测试通过!");

	//保护动作，避免多次失败引起卡片锁死，程序中进行一次正常上下电读写测试
	NDK_IccPowerUp(cardId[type], buf,&pnAtrlen);
	pfunc[type](type);
	NDK_IccPowerDown(cardId[type]);
	
#if !defined ME30GX //国芯没有8035芯片没有办法检测到拔插卡动作 故不支持 20170726 邱涛解释
	//case4:正常上电后，拔插memory卡，进行读写操作(应该失败)
	cls_show_msg("请确保%s卡已经插上,按任意键继续...",card[type]);
	//NDK_IccSetType(cardId[type]);
	if ((ret=NDK_IccPowerUp(cardId[type], buf,&pnAtrlen))!=NDK_OK)
	{
		NDK_IccPowerDown(cardId[type]);	
		cls_show_msg1(5,"line %d:上电失败!", __LINE__);
		return;
	}
	cls_show_msg("拔插%s卡,按任意键继续...",card[type]);
	//step:读写，验证密码等操作
	if(SUCC == pfunc[type](type))
	{ 
		NDK_IccPowerDown(cardId[type]);
		cls_show_msg1(5, "line %d:读写数据失败", __LINE__);
		return;
	}
	NDK_IccPowerDown(cardId[type]);
	cls_show_msg1(2, "子用例测试通过");
#endif	
	//保护动作，避免多次失败引起卡片锁死，程序中进行一次正常上下电读写测试
	NDK_IccPowerUp(cardId[type], buf,&pnAtrlen);
	pfunc[type](type);
	NDK_IccPowerDown(cardId[type]);

 	cls_show_msg("异常测试结束!请执行正常测试");
	return;
}

//读写压力测试
void memory_rw_press(MEMORY_t type)
{
	/*private & local definition*/
	int ret = 0, succ = 0, cnt = DEFAULT_CNT_VLE, bak = 0,pnAtrlen = 0;
	uint unLen = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uchar buf[300] = {0};
	char *card[] = {"AT24C01","AT24C02","AT24C04","AT24C08","AT24C016","AT24C032","AT24C064","SLE4432_42","SLE4418_28","SLE5528","AT88SC102","AT88SC1604","AT88SC1608","AT88SC153"};
	EM_ICTYPE cardId[]={ICTYPE_M_1_1,ICTYPE_M_1_2,ICTYPE_M_1_4,ICTYPE_M_1_8,ICTYPE_M_1_16,ICTYPE_M_1_32,ICTYPE_M_1_64,ICTYPE_M_2,ICTYPE_M_3,ICTYPE_M_3,ICTYPE_M_4,ICTYPE_M_5,ICTYPE_M_6,ICTYPE_M_7};
	unsigned char (*pfunc[])(MEMORY_t type) = 
	{
		test_24C_func, 
		test_24C_func,
		test_24C_func,
		test_24C_func, 
		test_24C_func,
		test_24C_func,
		test_24C_func,
		test_4432_42_func,
		test_4418_28_func,
		test_5528_func,
		test_102_func,
		test_1604_func,
		test_1608_func,
		test_153_func
	};
		
	/*process body*/
	cls_printf("默认压力测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)	
			bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);

	//上电
	//NDK_IccSetType(cardId[type]);
	if ((ret=NDK_IccPowerUp(cardId[type]/*ICTYPE_IC*/, buf,&pnAtrlen))!=NDK_OK)
	{
		if(ESC == cls_show_msg1(g_keeptime, "line %d:上电失败", __LINE__))
			return;
	}

	while(cnt)
	{
	
		//if(ESC == cls_show_msg1(2,"%s读写压力测试中...\n还剩%d次(已成功%d次),请耐心等待", card[type], cnt, succ))
			//break;
		cls_printf("%s读写压力测试中...\n还剩%d次(已成功%d次),请耐心等待", card[type], cnt, succ);//linwl20130617为提高测试效率此处修改成禁止退出并减少2秒延时
		cnt--;

		//读写操作，有的卡还需要密钥验证和认证操作等等
		switch(type) 
		{
			case AT24C01:
			case AT24C02:
			case AT24C04:	
			case AT24C08:	
			case AT24C016:	
			case AT24C032:	
			case AT24C064:	
			case SLE4432_42:
			case SLE4418_28:
			case SLE5528:	
			case AT88SC102:	
			case AT88SC1604:
			case AT88SC1608:
			case AT88SC153:
			
			if(SUCC != pfunc[type](type))
			{
				//NDK_IccPowerDown(cardId[type]/*ICTYPE_IC*/);//memory卡读写失败后就进行下电，又没有重新上电，导致接下来读写均失败,故中间读写失败不下电 20140124 zhuq
				continue;
			}
			break;
		default:
			continue;
			break;
		}

		succ++; 
	}
	//下电
	if ((ret=NDK_IccPowerDown(cardId[type]/*ICTYPE_IC*/))!=NDK_OK)
	{	
		cls_show_msg1(g_keeptime, "line %d:下电失败", __LINE__);
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"%s读写压力测试完成,总共进行%d次,成功%d次", card[type],bak-cnt,succ);
	return;
}

//综合压力测试
void memory_press(MEMORY_t type)
{
	/*private & local definition*/
	int ret = 0, succ = 0, cnt = DEFAULT_CNT_VLE2, bak = 0,pnAtrlen = 0;
	uint unLen = 0;
	char CntStr[8] = DEFAULT_CNT_STR2;
	uchar buf[300] = {0};
	char *card[] = {"AT24C01","AT24C02","AT24C04","AT24C08","AT24C016","AT24C032","AT24C064","SLE4432_42","SLE4418_28","SLE5528","AT88SC102","AT88SC1604","AT88SC1608","AT88SC153"};
	EM_ICTYPE cardId[]={ICTYPE_M_1_1,ICTYPE_M_1_2,ICTYPE_M_1_4,ICTYPE_M_1_8,ICTYPE_M_1_16,ICTYPE_M_1_32,ICTYPE_M_1_64,ICTYPE_M_2,ICTYPE_M_3,ICTYPE_M_3,ICTYPE_M_4,ICTYPE_M_5,ICTYPE_M_6,ICTYPE_M_7};
	unsigned char (*pfunc[])(MEMORY_t type) = 
	{
		test_24C_func, 
		test_24C_func,
		test_24C_func,
		test_24C_func, 
		test_24C_func,
		test_24C_func,
		test_24C_func,
		test_4432_42_func,
		test_4418_28_func,
		test_5528_func,
		test_102_func,
		test_1604_func,
		test_1608_func,
		test_153_func
	};

	/*process body*/
	cls_printf("默认压力测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen, INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)	
		bak = cnt = DEFAULT_CNT_VLE2;
	else
		bak = cnt = atoi(CntStr);

	while(cnt)
	{
		if(ESC == cls_show_msg1(2,"%s压力测试中...\n还剩%d次(已成功%d次),请耐心等待", card[type], cnt, succ))
			break;
		cnt--;

		//上电

		//NDK_IccSetType(cardId[type]); 
		if ((ret=NDK_IccPowerUp(cardId[type]/*ICTYPE_IC*/, buf,&pnAtrlen))!=NDK_OK)
		{
			if(ESC==cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次:%s上电失败(0x%x)", __LINE__, bak-cnt,card[type], ret))
				break;
			else
				continue;
		}
        
            
		//读写操作，有的卡还需要密钥验证和认证操作等等
		switch(type) 
		{
			case AT24C01:
			case AT24C02:
			case AT24C04:	
			case AT24C08:	
			case AT24C016:	
			case AT24C032:	
			case AT24C064:	
			case SLE4432_42:
			case SLE4418_28:
			case SLE5528:	
			case AT88SC102:	
			case AT88SC1604:
			case AT88SC1608:
			case AT88SC153:	

			if(SUCC != pfunc[type](type))
			{
				NDK_IccPowerDown(cardId[type]/*ICTYPE_IC*/); 
				continue;
			}
			break;
		default:
			continue;
			break;
		}
		//下电
		if ((ret=NDK_IccPowerDown(cardId[type]/*ICTYPE_IC*/))!=NDK_OK)
		{	
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次:%s下电失败(0x%x)", __LINE__, bak-cnt, card[type], ret);
			continue;
		}	
		succ++; 
	}
	cls_show_msg_record(FILENAME,FUNCNAME,"memory卡测试完成,总共进行%d次,成功%d次",bak-cnt,succ);
}

static MEMORY_t select_memorytype(void)
{
	/*private & local definition*/
	int nKeyin = 0, nPageSel = 0;//从0开始计算(第一页码为0)
	int cnt = 0, menuid = 0, maxpage = 0, sn =0;
#if defined ME30MH ||defined ME30THM||defined ME32||defined ME32THM//不支持102 1604卡
	MEMORY_t type[] = {AT24C01,AT24C02,AT24C04,AT24C08,AT24C016,AT24C032,AT24C064,SLE4432_42,SLE4418_28,SLE5528,AT88SC1608,AT88SC153}; 
	char* menustr[] =
	{
		"AT24C01",
		"AT24C02",
		"AT24C04",
		"AT24C08",
		"AT24C016",
		"AT24C032",
		"AT24C064",
		"SLE4432_42",
		"SLE4418_28",
		"SLE5528",
		"AT88SC1608",
		"AT88SC153"
	};
#elif defined ME30GX||defined ME32//不支持102 1604卡，5528卡国芯只有3V供电故也不支持
	MEMORY_t type[] = {AT24C01,AT24C02,AT24C04,AT24C08,AT24C016,AT24C032,AT24C064,SLE4432_42,SLE4418_28,AT88SC1608,AT88SC153}; 
	char* menustr[] =
	{
		"AT24C01",
		"AT24C02",
		"AT24C04",
		"AT24C08",
		"AT24C016",
		"AT24C032",
		"AT24C064",
		"SLE4432_42",
		"SLE4418_28",
		"AT88SC1608",
		"AT88SC153"
	};
#else
	MEMORY_t type[] = {AT24C01,AT24C02,AT24C04,AT24C08,AT24C016,AT24C032,AT24C064,SLE4432_42,SLE4418_28,SLE5528,AT88SC102,AT88SC1604,AT88SC1608,AT88SC153};	
	char* menustr[] =
	{
		"AT24C01",
		"AT24C02",
		"AT24C04",
		"AT24C08",
		"AT24C016",
		"AT24C032",
		"AT24C064",
		"SLE4432_42",
		"SLE4418_28",
		"SLE5528",
		"AT88SC102",
		"AT88SC1604",
		"AT88SC1608",
		"AT88SC153"
	};
#endif
#if SCR_DISABLE
	char str[1024]={0};
#endif
	char* tmpstr = NULL;

	/*process body*/
	maxpage = (SZ_ARRAY(menustr)-1)/ITEMPERPAGE;//从0开始计算(第一页码为0)
	cls_show_msg1(2, "选择测试卡类型..\n" "↑键:往回翻页\n" "↓键:往后翻页\n");

	while (1)
	{
		NDK_ScrClrs();
		for (cnt=0; cnt<ITEMPERPAGE; cnt++)
		{
			menuid = cnt+ITEMPERPAGE*nPageSel;
			if (menuid==-1)
				break;
			else if (menuid>SZ_ARRAY(menustr)-1)
				break;
			else
				tmpstr = menustr[menuid];

#if SCR_DISABLE	//重定位到串口显示
			sprintf(str,"%d.测试%s\n", cnt+1, tmpstr);
#if 0//ANDIROD_ENABLE
			NDK_AZPortWrite(g_com, strlen(str), str);
#else
			NDK_PortWrite(g_com, strlen(str), str);
#endif
#endif
			NDK_ScrPrintf("%d.测试%s\n", cnt+1, tmpstr);
			NDK_ScrRefresh();
		}

		//处理按键
		switch ((nKeyin=lib_getch()))
		{
		case '1':
		case '2':
		case '3':
		case '4':
			if((sn=nKeyin-'1'+ITEMPERPAGE*nPageSel)>SZ_ARRAY(menustr)-1)
				break;
			else
				return type[sn];
			break;
#if K21_ENABLE||defined ME50S
		case '7':
#else
		case DOWNKEY:
#endif
			/*下翻↓*/
			if (nPageSel>=maxpage)
				nPageSel = 0;
			else
				nPageSel++;
			break;
#if K21_ENABLE||defined ME50S
		case '8':
#else
		case UPKEY:
#endif
			/*上翻↑*/
			if (nPageSel==0)
				nPageSel = maxpage;
			else
				nPageSel--;
			break;
		case ESC:
			cls_show_msg1(2, "将设置为SLE4432_42(默认)...");
			return SLE4432_42;
			break;
		default:
			continue;
			break;
		}
	}
}

//用例41号
void systest34(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	int keyin = 0;
	MEMORY_t type = SLE4432_42;
	char *card[] = {"AT24C01","AT24C02","AT24C04","AT24C08","AT24C016","AT24C032","AT24C064","SLE4432_42","SLE4418_28","SLE5528","AT88SC102","AT88SC1604","AT88SC1608","AT88SC153"};

	/*process body*/
	cls_show_msg("%s测试中...请插上MEMORY卡,按任意键开始\n", TESTITEM);

	while(1)
	{
		keyin = cls_show_msg("1.综合压力测试\n"
			"2.读写压力测试\n"
			"3.异常测试\n"
			"0.配置");
		switch(keyin) 
		{
		case '1':
			cls_show_msg1(2, "综合压力测试");
			memory_press(type);
			break;
		case '2':
			if(ENTER!=cls_show_msg("memory卡读写专项压力测试(本项测试不接受ESC中断测试),ENTER键继续..."))
				break;
			memory_rw_press(type);
			break;
		case '3':
			cls_show_msg1(4,"异常测试，执行一次异常测试后请执行一次正常测试，避免卡锁死");
			memroy_abnormal_test(type);
			break;
		case '0':
			type = select_memorytype(); 
			cls_show_msg1(2, "%s设置成功!", card[type]);
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

#if 0  //MEMORY卡原来测试的测试代码!
/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 卡
* file name		: 1.c
* Author 			: jiangwx
* version			: V1.0
* DATE			: 2006117
* directory 		: .\SRC\card\
* description		: MEMORY卡测试
* related document	: 程序员参考手册
*
************************************************************************
* log			: 
* Revision 1.0  2006117 jiangwx
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "general.h"

/*---------------constants/macro definition---------------------*/
#define  UCID  26

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
*			  	          jiangwx		   2006116	 	    created
*****************************************************************/
//////////////////////////////////////////////////////////////////////////
//===============测试MEM卡函数说明=======================
static void testmemcard(void);   //测试MEM卡
static void test4442card(void);
static void test1608card(void);
static void test1021604card(void);
static void test4428card(void);
static void testat24c64card(void);
static void testat24c128card(void);
static void testat24c256card(void);
//void testmifare1(void);
static void testat45(void);
//=======================================================

//================================
void testmemcard(void)   //测试MEM卡
{
	while (1)
	{
		clrscr();
		printf(
			"①测试Sle4432_42\n"
			"②测试AT88SC1608\n"
			"③测试102或1604\n"
			"④其它类型卡\n"
			);
		switch(getkeycode(0)) 
		{
		case '1':
            		test4442card();
			break;
		case '2':
            		test1608card();
			break;
		case '3':
            		test1021604card();
			break;
		case '4':
			clrscr();
			printf(
				"①测试Sle4418_28\n"
				"②测试at24c64   \n"
				"③测试Sle5528\n"
				"④其它类型卡\n"
				);
			switch(getkeycode(0))
			{
			case '1':
				test4428card();
				break;
			case '2':
				testat24c64card();
				break;
			case '3':
//                		testmifare1();				
				break;
			case '4':
                		clrscr();
				printf(
					"①测试AT45税控卡\n"
					"②测试at24c128 \n"
					"③测试at24c256 \n"
					"④其它类型卡\n"
					);
				switch(getkeycode(0))
				{
				case '1':
					testat45();
					break;
				case '2':
					testat24c128card();
//					testat128();
					break;
				case '3':
					testat24c256card();
//					testat256();
					break;
				case ESC:
					return;
				default:
					break;
				}
				break;
			case ESC:
				return;
			default:
				break;
			}
			break;
		case ESC:
			return;
		default:
			break;
		}
	}
}

//=============================
//    09H：	Sle4432/4442  卡
//=============================
static void poweronsle4442(void);
static unsigned char comparesle4442key(unsigned char data_in1,unsigned char data_in2,unsigned char data_in3);
static void readsle4442maindata(void);
static void writesle4442maindata(void);
static void powerdownsle4442(void);
static void readsle4442securitymemory(void);
static void readsle4442protectionmemory(void);
static unsigned char writesle4442securitymemory(unsigned char data_in1,unsigned char data_in2,unsigned char data_in3);
static unsigned char comparesle4442maindata(unsigned char data_in);
static void writeandcompareandrenewkey(void);
static void writesle4442protectionmemory(void);

unsigned char readdata;
char icbuf[300],icreadbuf[300];
#define readaddress 0x08

static void test4442card(void)  //zjh 测试 SLE4442 卡
{
	//int i;
	//int ret;
	    	
	while (1)
	{
		clrscr();
		printf(
			"①上电  ②下电\n"
			"③读数据⑥读密码\n"
			"④写数据⑦读保护\n"
			"⑤验密钥⑧写密钥\n"
			);
		switch(getkeycode(0))
		{
		case '1':
			poweronsle4442();
			break;
		case '2':
			powerdownsle4442();
			break;
		case '3':
			readsle4442maindata();
			break;
		case '4':
			writesle4442maindata();
			break;
		case '5':
			comparesle4442key(0xff,0xff,0xff);
			break;
		case '6':
			readsle4442securitymemory();
			break;
		case '7':
			readsle4442protectionmemory();
			break;
		case '8':
			writeandcompareandrenewkey();
			break;
		case '9':
			writesle4442protectionmemory();
			break;
		case ESC:
			return;
		default:
			break;
		}
	}
}

static void poweronsle4442(void)
{
	int i;
	//int ret;

	SetICType(9,0); //这里有对5002的状态初始化，如果没有这句，当睡眠后，无法上电
    	clrscr();
	if (icpowerup(0,icbuf)==SUCC)
	    printf("IC卡上电成功\n");
	else
	{
        printf("IC卡上电失败\n");
		getkeycode(0);
		return;
	}
    	for (i=1;i<(icbuf[0]+1);i++)
	{
		printf("%02x",icbuf[i]);
		printf(" ");
	}
	getkeycode(5);
}
///////////////////////////////////////////
////         校验SLE4442卡密钥         ////
////     【00 20 00 00 03 FF FF FF】   ////  
///////////////////////////////////////////
static unsigned char comparesle4442key(unsigned char data_in1,unsigned char data_in2,unsigned char data_in3)
{
	int i;
	int ret;
	
	icbuf[0]=0x00;
    	icbuf[1]=0x20;
	icbuf[2]=0x00;
	icbuf[3]=0x00;
	icbuf[4]=0x03;
	icbuf[5]=data_in1;  
	icbuf[6]=data_in2;
	icbuf[7]=data_in3;
	clrscr();
	ret = iccrw(0,8,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			printf("密钥校验成功\n");
		else 
		{
			printf("密钥校验失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				printf(" ");
			}
			getkeycode(0);
			return SUCC+1;
		}
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
		    	printf(" ");
		}
 		printf(" \n");
 	}
	else 
	{
		printf("密钥校验失败\n");
        	getkeycode(0);
		return SUCC+1;
	}
	getkeycode(5);
	return SUCC;
}
///////////////////////////////////////////
////       读SLE4442卡数据存贮区       ////
////        【00 B0 00 20 04】         ////	
///////////////////////////////////////////
static void readsle4442maindata(void)
{
	int i;
	int ret;
    	char buf[8];

    	icbuf[0]=0x00;
    	icbuf[1]=0xB0;
	icbuf[2]=0x00;

	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	printf("0-255\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[3]=(ret&0xff);

    	strcpy(buf,"");          //输入长度
	clrscr();
	printf("请设置长度\n");
	printf("0-252\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[4]=ret;

	clrscr();
	icreadbuf[0] = 0;
	ret = iccrw(0,5,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			printf("读数据区成功\n");
		else 
		{
			printf("读数据区失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				printf(" ");
			}
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
		    	printf(" ");
		}
 		printf(" \n");
		readdata = icreadbuf[1]; //保存数据
 	}
	else 
	{
		printf("读数据区失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(5);
}
///////////////////////////////////////////
////       写SLE4442卡数据存贮区       ////
////   【00 D0 00 20 04 01 02 03 04】  ////	
///////////////////////////////////////////
static void writesle4442maindata(void)
{
	int i;
	int ret;
    	char buf[8];

    	icbuf[0]=0x00;
    	icbuf[1]=0xD0;
	icbuf[2]=0x00;

	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	printf("0-255\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[3]=(ret&0xff);

    	strcpy(buf,"");          //输入长度
	clrscr();
	printf("请设置长度\n");
	printf("0-252\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[4]=ret;

    	strcpy(buf,"");          //输入数据
	clrscr();
	printf("请设置要写入的数值\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
    
	for (i=0;i<icbuf[4];i++)
		icbuf[5+i]=ret;

	clrscr();
	ret = iccrw(0,5+icbuf[4],icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
		{
            		if (comparesle4442maindata(readdata)==SUCC)
			    printf("写数据区成功\n");
			else 
			{
                		printf("写数据区失败1\n");
				for (i=0;i<icreadbuf[0];i++)
				{
					printf("%02x",icreadbuf[i+1]);
					printf(" ");
				}
				getkeycode(0);
				return;
			}
		}
		else 
		{
			printf("写数据区失败2\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				printf(" ");
			}
			getkeycode(0);
			return;
		}
 	}
	else 
	{
		printf("写数据区失败3\n");
        	getkeycode(0);
		return;
	}
	
	getkeycode(0);
}	
///////////////////////////////////////////
////       下电SLE4442卡               ////
////                                   ////	
///////////////////////////////////////////
static void powerdownsle4442(void)
{
	clrscr();
	if (icpowerdown(0)==SUCC)
	{
        	printf("IC卡下电成功\n");
		getkeycode(2);
	}
	else
	{
        	printf("IC卡下电失败\n");
		getkeycode(0);
	}
}

///////////////////////////////////////////
////       比较SLE4442卡数据存贮区     ////
////        【00 B0 00 20 04】         ////	
///////////////////////////////////////////
static unsigned char comparesle4442maindata(unsigned char data_in)
{
	int i;
	int ret;
//	char buf[8];

    	icbuf[0]=0x00;
    	icbuf[1]=0xB0;
	icbuf[2]=0x00;
	icbuf[3]=readaddress;  //读受写保护的区域&不受写保护的区域
	icbuf[4]=0x04;
	clrscr();
	ret = iccrw(0,5,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]!=0x90)||(icreadbuf[icreadbuf[0]]!=0x00))
			return SUCC+1;
		
		for (i=1;i<(icreadbuf[0]+1-2);i++)
		{
            		if (icreadbuf[i]!=data_in)
			    return SUCC+1;
		}
		return SUCC;
 	}
	else 
	    return SUCC+1;	
}
///////////////////////////////////////////
////       读SLE4442卡密码区           ////
////      【00 B0 C0 00 04】           ////	
///////////////////////////////////////////
static void readsle4442securitymemory(void)
{
    	int i;
	int ret;

    	icbuf[0]=0x00;
    	icbuf[1]=0xB0;
	icbuf[2]=0xC0;
	icbuf[3]=0x00;  
	icbuf[4]=0x04;
	clrscr();
	ret = iccrw(0,5,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			printf("读密码区成功\n");
		else 
		{
			printf("读密码区失败\n");
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
		    	printf(" ");
		}
  	}
	else 
	{
		printf("读密码区失败\n");
        	getkeycode(0);
		return;
	}
	getkeycode(5);
}
///////////////////////////////////////////
////       读SLE4442卡写保护区         ////
////        【00 B0 80 00 04】         ////	
///////////////////////////////////////////
static void readsle4442protectionmemory(void)
{
    	int i;
	int ret;

    	icbuf[0]=0x00;
    	icbuf[1]=0xB0;
	icbuf[2]=0x80;
	icbuf[3]=0x00;  
	icbuf[4]=0x04;
	clrscr();
	ret = iccrw(0,5,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			printf("读写保护区成功\n");
		else 
		{
			printf("读写保护区失败\n");
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
		    	printf(" ");
		}
  	}
	else 
	{
		printf("读写保护区失败\n");
        	getkeycode(0);
		return;
	}
	getkeycode(5);
}
///////////////////////////////////////////
////       写SLE4442卡密码区           ////
////   【00 D0 C0 01 03 xx xx xx】     ////	
///////////////////////////////////////////
static unsigned char writesle4442securitymemory(unsigned char data_in1,unsigned char data_in2,unsigned char data_in3)
{
	int i;
	int ret;

    	icbuf[0]=0x00;
    	icbuf[1]=0xD0;
	icbuf[2]=0xC0;
	icbuf[3]=0x01;  
	icbuf[4]=0x03;
	icbuf[5]=data_in1;
	icbuf[6]=data_in2;
	icbuf[7]=data_in3;
	clrscr();
	ret = iccrw(0,8,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			printf("写密钥成功\n");
		else 
		{
			printf("写密钥失败\n");
			getkeycode(0);
			return SUCC+1;
		}
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
		    	printf(" ");
		}
  	}
	else 
	{
		printf("写密钥失败\n");
        	getkeycode(0);
		return SUCC+1;
	}
	getkeycode(5);	
	return SUCC;
}

static void writeandcompareandrenewkey(void)
{
     	if (writesle4442securitymemory(0x88,0x89,0x8a)!=SUCC) 
     		return;
     	readsle4442securitymemory();
     	if (comparesle4442key(0x88,0x89,0x8a)!=SUCC) 
     		return;
	if (writesle4442securitymemory(0xff,0xff,0xff)!=SUCC) 
		return;
	readsle4442securitymemory();
     	if (comparesle4442key(0xff,0xff,0xff)!=SUCC) 
     		return;
}

///////////////////////////////////////////
////       写SLE4442卡写保护区         ////
////    【00 D0 80 Add Lc Data】       ////
///////////////////////////////////////////
//   	Add 00h到1Fh 
//      若P2+Lc>32则返回（6A 86 ）P1 P2参数错误
//

static void writesle4442protectionmemory(void)
{
	int i;
	int ret;

    	icbuf[0]=0x00;
    	icbuf[1]=0xD0;
	icbuf[2]=0x80;
	icbuf[3]=0x04;  
	icbuf[4]=0x01;
	icbuf[5]=0x00;  //擦除第4字节保护位
	clrscr();
	ret = iccrw(0,6,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			printf("写保护区成功\n");
		else 
		{
			printf("写保护区失败\n");
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
		    	printf(" ");
		}
  	}
	else 
	{
		printf("写保护区失败\n");
        	getkeycode(0);
		return;
	}
	getkeycode(5);	
	return;
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//==========================================$
//  0CH:    AT88SC102  1604 卡
//==========================================

static void poweron1021604(void);
static void powerdown1021604(void);
static void read1021604(void);
static void write1021604(void);
static void erase1021604(void);
static void verify1021604password(void);


static void test1021604card(void)
{
	while (1)
	{
		clrscr();
		printf(
			"①上电  ②下电\n"
			"③擦除  ⑥\n"
			"④读数据⑦\n"
			"⑤写数据⑧验密钥\n"
			);
		switch(getkeycode(0))
		{
		case '1':
			poweron1021604();
			break;
		case '2':
			powerdown1021604();
			break;
		case '3':
			erase1021604();
			break;
		case '4':
			read1021604();
			break;
		case '5':
			write1021604();
			break;
		case '6':
			
			break;
		case '7':
			
			break;
		case '8':
			verify1021604password();			
			break;
		case '9':
			
			break;
		case ESC:
			return;
		default:
			break;
		}
	}
}

//===============================================
static void poweron1021604(void)
{
	int i;

	clrscr();
	printf("按1选102卡\n");
	printf("按2选1604卡\n");
	switch(getkeycode(0))
	{
	case '1':
		SetICType(0x0a,0);
	    	break;
	case '2':
		SetICType(0x0b,0);
	    	break;
	default:
		return;
	}
	
    	clrscr();
	if (icpowerup(0,icbuf)==SUCC)
	    	printf("IC卡上电成功\n");
	else
	{
        	printf("IC卡上电失败\n");
		getkeycode(0);
		return;
	}
    	for (i=1;i<(icbuf[0]+1);i++)
	{
		printf("%02x",icbuf[i]);
		printf(" ");
	}
	getkeycode(5);
}
//=================================================
static void powerdown1021604(void)
{
	clrscr();
	if (icpowerdown(0)==SUCC)
	{
        	printf("IC卡下电成功\n");
		getkeycode(2);
	}
	else
	{
        	printf("IC卡下电失败\n");
		getkeycode(0);
	}
}
//=====================================================
//             读102/1604
//    00  B0  highadd  lowadd  length
//=====================================================
static void read1021604(void)
{
	int i;
	int ret;
	char buf[5];

	icbuf[0]=0x00;
    	icbuf[1]=0xB0;

    	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[2] = (ret & 0xff00)>>8;	
	icbuf[3] = (ret & 0xff);  //地址  ret 是INT
		
	strcpy(buf,"");        //输入长度
	clrscr();
	printf("请输入长度\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 8;
	ret=atol(buf);	
    	icbuf[4]=(ret & 0xff);

	printf("%02x",icbuf[2]);  //debug
	portwrite(AUX1,1,icbuf+3);//debug
	printf("%02x",icbuf[3]);  //debug
	portwrite(AUX1,1,icbuf+4);//debug
	printf("%02x",icbuf[4]);  //debug
	portwrite(AUX1,1,icbuf+5);//debug
	getkeycode(0); //debug
   	
	clrscr();
	ret = iccrw(0,5,icbuf,icreadbuf);

    	if(ret==SUCC)
	{   
        if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("读失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				printf(" ");
			}
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0]-2;i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 		readdata = icreadbuf[i-1]; //保存数据
 	}
	else 
	{
		printf("读失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
//=====================================================
//             写102/1604
//      00   D0   00   Add   length  xx  xx  ....
//=====================================================
static void write1021604(void)
{
	int i;
	int ret;
	char buf[5];
	
	icbuf[0]=0x00;
    	icbuf[1]=0xD0;

    	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[2] = (ret & 0xff00)>>8;	
	icbuf[3] = (ret & 0xff);  //地址  ret 是INT

	strcpy(buf,"");          //选择写的长度
	clrscr();
	printf("请选择要写的长度\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[4] = (ret & 0xff); 

    	strcpy(buf,"");          //输入要写的数据数据
	clrscr();
	printf("请输入要写的数据\n");
	printf("0--255\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=(atol(buf)& 0xff);

	for (i=0;i<icbuf[4];i++)
		 icbuf[5+i] = ret;  

	printf("%02x",icbuf[2]);  //debug
	portwrite(AUX1,1,icbuf+3);//debug
	printf("%02x",icbuf[3]);  //debug
	portwrite(AUX1,1,icbuf+4);//debug
	printf("%02x",icbuf[4]);  //debug
	portwrite(AUX1,1,icbuf+5);//debug
	getkeycode(0); //debug
	
	clrscr();
	ret = iccrw(0,5+icbuf[4],icbuf,icreadbuf);

    	if(ret==SUCC)
	{   
        if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("写失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				printf(" ");
			}
			getkeycode(0);
			return;
		}
		printf("写成功\n");
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 	}
	else 
	{
		printf("写失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
//=====================================================
//             擦除102/1604
//      00   0e   highadd  lowadd  length
//=====================================================
static void erase1021604(void)
{
	int i;
	int ret;
	char buf[5];

	icbuf[0]=0x00;
    	icbuf[1]=0x0e;


	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[2] = (ret & 0xff00)>>8;	
	icbuf[3] = (ret & 0xff);  //地址  ret 是INT
	
	strcpy(buf,"");          //选择擦除的长度
	clrscr();
	printf("请选择要擦除的长度\n");
	printf("0--255\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[4] = (ret & 0xff); 

    	clrscr();
	ret = iccrw(0,5,icbuf,icreadbuf);

    	if(ret==SUCC)
	{   
        	if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("擦除失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				printf(" ");
			}
			getkeycode(0);
			return;
		}
		printf("擦除成功\n");
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 	}
	else 
	{
		printf("擦除失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
//=====================================================
//              校验102/1604密钥
//        00   20   P1  P2  length   data1 data2 ....
//=====================================================
static void verify1021604password(void)
{
	int i;
	int ret;
	char buf[5];

	icbuf[0]=0x00;
    	icbuf[1]=0x20;
	
	strcpy(buf,"");          //输入P1值
	clrscr();
	printf("请输入P1值\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
    	icbuf[2]=(ret & 0xff);

    	strcpy(buf,"");          //输入P2值
	clrscr();
	printf("请输入P2值\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[3] = (ret & 0xff);  //密钥区  ret 是INT
	
    	strcpy(buf,"");          //输入密钥长度
	clrscr();
	printf("请输入密钥长度\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[4] = (ret & 0xff);
	
	for (i=0;i<icbuf[4];i++)
	{
		clrscr();
		strcpy(buf,"");
		printf("请输入密钥值\n");
		ret=getnumstr(buf,4,2,0);
		ret=atol(buf);
		icbuf[5+i]=(ret & 0xff);
	}
	
    	for (i=0;i<icbuf[4];i++)
	{
		printf("%02x",icbuf[5+i]);  //debug
		portwrite(AUX1,1,icbuf+5+i);//debug
	}
	
	getkeycode(0); //debug
   	
	clrscr();
	ret = iccrw(0,5+icbuf[4],icbuf,icreadbuf);

    	if(ret==SUCC)
	{   
        	if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("密钥校验失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				printf(" ");
			}
			getkeycode(0);
			return;
		}
		printf("密钥校验成功\n");
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 	}
	else 
	{
		printf("密钥校验失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//==========================================$
//  0CH:    AT88SC1608  卡
//==========================================
static void poweron1608(void);
static void powerdown1608(void);
static void Authentication1608(void);
static void read1608(void);
static void read1608userzone(void);
static void read1608configzone(void);
static void verifypassword(void);
static void write1608(void);
static void write1608userzone(void); 
static void write1608configzone(void);
static void select1608userzone(void);

static void test1608card(void)
{
	while (1)
	{
		clrscr();
		printf(
			"①上电  ②下电\n"
			"③认证  ⑥\n"
			"④读数据⑦\n"
			"⑤写数据⑧验密钥\n"
			);
		switch(getkeycode(0))
		{
		case '1':
			poweron1608();
			break;
		case '2':
			powerdown1608();
			break;
		case '3':
			Authentication1608();
			break;
		case '4':
			read1608();
			break;
		case '5':
			write1608();
			break;
		case '6':
			
			break;
		case '7':
			
			break;
		case '8':
			verifypassword();			
			break;
		case '9':
			
			break;
		case ESC:
			return;
		default:
			break;
		}
	}
}

//===============================================
static void poweron1608(void)
{
	int i;

	SetICType(0x0c,0); //这里有对5002的状态初始化，如果没有这句，当睡眠后，无法上电
    	clrscr();
	if (icpowerup(0,icbuf)==SUCC)
	    printf("IC卡上电成功\n");
	else
	{
        	printf("IC卡上电失败\n");
		getkeycode(0);
		return;
	}
    	for (i=1;i<(icbuf[0]+1);i++)
	{
		printf("%02x",icbuf[i]);
		printf(" ");
	}
	getkeycode(5);
}
//=================================================
static void powerdown1608(void)
{
	clrscr();
	if (icpowerdown(0)==SUCC)
	{
        	printf("IC卡下电成功\n");
		getkeycode(2);
	}
	else
	{
        	printf("IC卡下电失败\n");
		getkeycode(0);
	}
}
//=====================================================
//             认证 Authentication
// 00   82   00   00   10   Q0(0)…Q0(7)  Gc(0)…Gc(7)
//=====================================================
static void Authentication1608(void)
{
	int i;
	int ret;
	
	icbuf[0]=0x00;
    	icbuf[1]=0x82;
	icbuf[2]=0x00;
	icbuf[3]=0x00;
	icbuf[4]=0x10;

	icbuf[5]=0x88; icbuf[6]=0x88; icbuf[7]=0x88; icbuf[8]=0x88; icbuf[9]=0x88; icbuf[10]=0x88; icbuf[11]=0x88; icbuf[12]=0x88;
	icbuf[13]=0x11; icbuf[14]=0x11; icbuf[15]=0x11; icbuf[16]=0x11; icbuf[17]=0x11; icbuf[18]=0x11; icbuf[19]=0x11; icbuf[20]=0x11;

	clrscr();
	ret = iccrw(0,21,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			printf("认证成功\n");
		else 
		{
			printf("认证失败\n");
			getkeycode(0);
		}
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
		    	printf(" ");
		}
 		printf(" \n");
 	}
	else 
	{
		printf("通讯失败\n");
        	getkeycode(0);
		return;
	}
	getkeycode(5);
}

//=====================================================
//             读1608
// 
//=====================================================
static void read1608(void)
{
	while (1)
	{
		clrscr();
		printf(
			"①读用户区\n"
			"②读设置区\n"
			"③选择用户区\n"
			);
		switch(getkeycode(0))
		{
		case '1':
			read1608userzone();
			break;
		case '2':
			read1608configzone();
			break;
		case '3':
			select1608userzone();
			break;
		case ESC:
			return;
		default:
			break;
		}
	}
}
//=====================================================
//             读1608用户区
//    00  B0  01  add  length
//=====================================================
static void read1608userzone(void)
{
	int i;
	int ret;
	char buf[8];

	icbuf[0]=0x00;
    	icbuf[1]=0xB0;
	icbuf[2]=0x01;
    
	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	printf("0-255\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

	icbuf[3] = (ret & 0xff);  //地址  ret 是INT
		
	strcpy(buf,"");        //输入长度
	clrscr();
	printf("请输入长度\n");
	printf("0-250\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 8;
	ret=atol(buf);
	
    	icbuf[4]=(ret & 0xff);

	printf("%02x",icbuf[3]);  //debug
	portwrite(AUX1,1,icbuf+3);//debug
	printf("%02x",icbuf[4]);  //debug
	portwrite(AUX1,1,icbuf+4);//debug
	getkeycode(0); //debug
   	
	clrscr();
	ret = iccrw(0,5,icbuf,icreadbuf);

    	if(ret==SUCC)
	{   
        	if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("读用户区失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				printf(" ");
			}
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0]-2;i++)
		{
           		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 		readdata = icreadbuf[i-1]; //保存数据
 	}
	else 
	{
		printf("读用户区失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
//=====================================================
//              读1608设置区
//        00  B0  05  add  length
//=====================================================
static void read1608configzone(void)
{
	int i;
	int ret;
	char buf[5];

	icbuf[0]=0x00;
    	icbuf[1]=0xB0;
	icbuf[2]=0x05;
    
	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	printf("0-128\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

	icbuf[3] = (ret & 0xff);  //地址  ret 是INT
		
	strcpy(buf,"");         //输入长度
	clrscr();
	printf("请输入长度\n");
	printf("0-128\n");        //包括熔丝
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 8;
	ret=atol(buf);
	
    	icbuf[4]=(ret & 0xff);

	printf("%02x",icbuf[3]);  //debug
	portwrite(AUX1,1,icbuf+3);//debug
	printf("%02x",icbuf[4]);  //debug
	portwrite(AUX1,1,icbuf+4);//debug
	getkeycode(0); //debug
   	
	clrscr();
	ret = iccrw(0,5,icbuf,icreadbuf);

    	if(ret==SUCC)
	{   
        	if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("读设置区失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				printf(" ");
			}
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0]-2;i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 		readdata = icreadbuf[i-1]; //保存数据
 	}
	else 
	{
		printf("读设置区失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}

//=====================================================
//              校验1608密钥
//        00   20   03  P2  03   Pw(0)Pw(1)Pw(2)
//=====================================================
static void verifypassword(void)
{
	int i;
	int ret;
	char buf[5];

	icbuf[0]=0x00;
    	icbuf[1]=0x20;
	icbuf[2]=0x03;
    
	strcpy(buf,"");          //输入密钥区  指出要验证的密钥区 (00-0F)
	clrscr();
	printf("请密钥区\n");
	printf("0到7写8到f读\n");
	printf("0-15\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

	icbuf[3] = (ret & 0xff);  //密钥区  ret 是INT
	icbuf[4] = 3; 
	
	clrscr();
	strcpy(buf,"");
	printf("请输入第一个密钥\n");
	printf("0-255\n");        //包括熔丝
	ret=getnumstr(buf,4,2,0);
	ret=atol(buf);
	icbuf[5]=(ret & 0xff);

	clrscr();
	strcpy(buf,"");
	printf("请输入第二个密钥\n");
	printf("0-255\n");        //包括熔丝
	ret=getnumstr(buf,4,2,0);
	ret=atol(buf);
    	icbuf[6]=(ret & 0xff);

	clrscr();
	strcpy(buf,"");
	printf("请输入第三个密钥\n");
	printf("0-255\n");        //包括熔丝
	ret=getnumstr(buf,4,2,0);
	ret=atol(buf);
    	icbuf[7]=(ret & 0xff);


	printf("%02x",icbuf[5]);  //debug
	portwrite(AUX1,1,icbuf+5);//debug
	printf("%02x",icbuf[6]);  //debug
	portwrite(AUX1,1,icbuf+6);//debug
	printf("%02x",icbuf[7]);  //debug
	portwrite(AUX1,1,icbuf+7);//debug
	getkeycode(0); //debug
   	
	clrscr();
	ret = iccrw(0,8,icbuf,icreadbuf);

    	if(ret==SUCC)
	{   
        	if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("密钥校验失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				printf(" ");
			}
			getkeycode(0);
			return;
		}
		printf("密钥校验成功\n");
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 	}
	else 
	{
		printf("密钥校验失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
//=====================================================
//             写1608
// 
//=====================================================
static void write1608(void)
{
	while (1)
	{
		clrscr();
		printf(
			"①写用户区\n"
			"②写设置区\n"
			"③选择用户区\n"
			);
		switch(getkeycode(0))
		{
		case '1':
			write1608userzone();
			break;
		case '2':
			write1608configzone();
			break;
		case '3':
			select1608userzone();
			break;
		case ESC:
			return;
		default:
			break;
		}
	}
}
//=====================================================
//             写1608用户区
//      00   D0   00   Add   length  xx  xx  。。。
//=====================================================
static void write1608userzone(void)
{
	int i;
	int ret;
	char buf[5];

	icbuf[0]=0x00;
    	icbuf[1]=0xD0;
	icbuf[2]=0x00;
    
	strcpy(buf,"");          //选择地址
	clrscr();
	printf("请设置地址\n");
	printf("0--255\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[3] = (ret & 0xff);  

    	strcpy(buf,"");          //输入长度
	clrscr();
	printf("请设置长度\n");
	printf("0-252\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[4]=ret;

    	strcpy(buf,"");          //输入数据
	clrscr();
	printf("请设置要写入的数值\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
    
	for (i=0;i<icbuf[4];i++)
    	icbuf[5+i]=ret;

	clrscr();
	ret = iccrw(0,5+icbuf[4],icbuf,icreadbuf);

    	if(ret==SUCC)
	{   
        	if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("用户区写失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				printf(" ");
			}
			getkeycode(0);
			return;
		}
		printf("用户区写成功\n");
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 	}
	else 
	{
		printf("用户区写失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
//=====================================================
//             写1608设置区
//         00   D0   04   Add   length=1  xx
//=====================================================
static void write1608configzone(void)
{
	int i;
	int ret;
	char buf[5];

	icbuf[0]=0x00;
    	icbuf[1]=0xD0;
	icbuf[2]=0x04;
    
	strcpy(buf,"");          //选择地址
	clrscr();
	printf("请选择设置区地址\n");
	printf("0--128\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

	icbuf[3] = (ret & 0xff);  //用户区地址  ret 是INT
	icbuf[4] = 3;  //长度

    	strcpy(buf,"");          //输入第一个数据
	clrscr();
	printf("请输入第一个字节\n");
	printf("0--255\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
    	icbuf[5] = (ret & 0xff);  
	
    	strcpy(buf,"");          //输入第二个数据
	clrscr();
	printf("请输入第二个字节\n");
	printf("0--255\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
    	icbuf[6] = (ret & 0xff);  

	strcpy(buf,"");          //输入第三个数据
	clrscr();
	printf("请输入第三个字节\n");
	printf("0--255\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
    	icbuf[7] = (ret & 0xff);  

	clrscr();
	ret = iccrw(0,8,icbuf,icreadbuf);

    	if(ret==SUCC)
	{   
	        if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("用户区写失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				printf(" ");
			}
			getkeycode(0);
			return;
		}
		printf("用户区写成功\n");
		for (i=0;i<icreadbuf[0];i++)
		{
	            	printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 	}
	else 
	{
		printf("用户区写失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
//=====================================================
//             选择1608用户区
//     00   D0   02   Add   0  
//=====================================================
static void select1608userzone(void)
{
	int i;
	int ret;
	char buf[5];

	icbuf[0]=0x00;
    	icbuf[1]=0xD0;
	icbuf[2]=0x02;
    
	strcpy(buf,"");          //选择用户区
	clrscr();
	printf("请选择用户区\n");
	printf("0--7\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

	icbuf[3] = (ret & 0xff);  //用户区  ret 是INT
	icbuf[4] = 0; 
	
	clrscr();
	ret = iccrw(0,5,icbuf,icreadbuf);

    	if(ret==SUCC)
	{   
        	if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("用户区选择失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				printf(" ");
			}
			getkeycode(0);
			return;
		}
		printf("用户区选择成功\n");
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 	}
	else 
	{
		printf("用户区选择失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//=============================
//    08H：	Sle4428  卡
//=============================
static void poweronsle4428(void);
static unsigned char comparesle4428key(unsigned char data_in1,unsigned char data_in2);
static void readsle4428maindata(void);
static void writesle4428maindata(void);
static void powerdownsle4428(void);
static void readsle4428securitymemory(void);

static unsigned char writesle4428securitymemory(unsigned char data_in1,unsigned char data_in2,unsigned char data_in3);
static unsigned char comparesle4428maindata(unsigned char data_in);

static void test4428card(void)  //zjh 测试 SLE4442 卡
{
	//int i;
	//int ret;
	    	
	while (1)
	{
		clrscr();
		printf(
			"①上电  ②下电\n"
			"③读数据⑥读密码\n"
			"④写数据⑦错密钥\n"
			"⑤验密钥\n"
			);
		switch(getkeycode(0))
		{
		case '1':
			poweronsle4428();
			break;
		case '2':
			powerdownsle4428();
			break;
		case '3':
			readsle4428maindata();
			break;
		case '4':
			writesle4428maindata();
			break;
		case '5':
			comparesle4428key(0xff,0xff);
			break;
		case '6':
			readsle4428securitymemory();
			break;
		case '7':
			comparesle4428key(0x20,0x20);  //错误的密钥
			break;
		
		case ESC:
			return;
		default:
			break;
		}
	}
}

static void poweronsle4428(void)
{
	int i;
	//int ret;

	SetICType(0x0d,0); //这里有对5002的状态初始化，如果没有这句，当睡眠后，无法上电
    	clrscr();
	if (icpowerup(0,icbuf)==SUCC)
	    printf("IC卡上电成功\n");
	else
	{
        printf("IC卡上电失败\n");
		getkeycode(0);
		return;
	}
    	for (i=1;i<(icbuf[0]+1);i++)
	{
		printf("%02x",icbuf[i]);
		printf(" ");
	}
	getkeycode(0);
}
///////////////////////////////////////////
////         校验SLE4428卡密钥         ////
////     【00 20 00 00 02 67 c6】      ////  
///////////////////////////////////////////
static unsigned char comparesle4428key(unsigned char data_in1,unsigned char data_in2)
{
	int i;
	int ret;
	
	icbuf[0]=0x00;
    	icbuf[1]=0x20;
	icbuf[2]=0x00;
	icbuf[3]=0x00;
	icbuf[4]=0x02;
	icbuf[5]=data_in1;  
	icbuf[6]=data_in2;
	clrscr();
	ret = iccrw(0,7,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			printf("密钥校验成功\n");
		else 
		{
			printf("密钥校验失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				printf(" ");
			}
			getkeycode(0);
			return SUCC+1;
		}
		for (i=0;i<icreadbuf[0];i++)
		{
            		printf("%02x",icreadbuf[i+1]);
		    	printf(" ");
		}
 		printf(" \n");
 	}
	else 
	{
		printf("密钥校验失败\n");
        	getkeycode(0);
		return SUCC+1;
	}
	getkeycode(5);
	return SUCC;
}
///////////////////////////////////////////
////       读SLE4428卡数据存贮区       ////
////        【00 B0 80 00 0c】 带保护位////	
///////////////////////////////////////////
static void readsle4428maindata(void)
{
	int i;
	int ret;
	unsigned char bflag=1;
	char buf[5];

	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	printf("0-1023\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

	icbuf[2] = (ret >> 8);
    	icbuf[3] = (ret & 0xff);
	
	strcpy(buf,"");        //输入长度
	clrscr();
	printf("请输入长度\n");
	printf("0-126或者0-252\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 12;
	ret=atol(buf);
    	icbuf[4]=(ret & 0xff);

	printf("如果要读保护位，输入1\n");  //要不要读保护位
	if (getkeycode(0)=='1')
	    icbuf[2]=icbuf[2]|0x80;  

    	icbuf[0]=0x00;
    	icbuf[1]=0xB0;
    	
	clrscr();
	icreadbuf[0] = 0;
	ret = iccrw(0,5,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
        	if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("读数据区失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				if (bflag!=1)
				{ 
					bflag = 1;
					printf(" ");
				}
				else
					bflag = 0;
			}
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0]-2;i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
	            	if ( (icbuf[2]&0x80)==0x80 ) //如果有读保护位
	            	{
				if (bflag!=1)
				{ 
					bflag = 1;
					printf(" ");
				}
				else
					bflag = 0;
			}
			else
				printf(" ");
		}
 		printf(" \n");
		readdata = icreadbuf[i-1]; //保存数据
 	}
	else 
	{
		printf("读数据区失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
///////////////////////////////////////////
////       写SLE4428卡数据存贮区       ////
////   【00 D0 00 08 04 01 02 03 04】  ////	
///////////////////////////////////////////
//        Add1 ----位地址最低两位 b1、b0 为启始地址的高字节，取值范围为(0--3)
//             ----位地址b5~b2 为0
//             ----位地址 b7_b6 = 00时，写数据，不写保护位
//                     	b7_b6 = 10时，写数据和保护位
//                  	    b7_b6 = 11时，比较数据和写保护位，
//        Add2 ---- 为启始地址的低字节，取值范围(0--0ffh)

static void writesle4428maindata(void)
{
	int i;
	int ret;
	char buf[8];

    	icbuf[0]=0x00;
    	icbuf[1]=0xD0;

    	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	printf("0-1023\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[2]=(ret>>8);
	icbuf[3]=(ret&0xff);

    	strcpy(buf,"");          //输入长度
	clrscr();
	printf("请设置长度\n");
	printf("0-252\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	icbuf[4]=ret;

    	strcpy(buf,"");          //输入数据
	clrscr();
	printf("请设置要写入的数值\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
    
	for (i=0;i<icbuf[4];i++)
		icbuf[5+i]=ret;
	
	clrscr();
	ret = iccrw(0,5+icbuf[4],icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
		{
#if 0
	            	if (comparesle4442maindata(readdata)==SUCC)
				    printf("写数据区成功\n");
			else 
			{
	                	printf("写数据区失败1\n");
				getkeycode(0);
				return;
			}
#endif
		}
		else 
		{
			printf("写数据区失败2\n");
			getkeycode(0);
			return;
		}
 	}
	else 
	{
		printf("写数据区失败3\n");
        	getkeycode(0);
		return;
	}
	getkeycode(5);
}	
///////////////////////////////////////////
////       下电SLE4428卡               ////
////                                   ////	
///////////////////////////////////////////
static void powerdownsle4428(void)
{
	clrscr();
	if (icpowerdown(0)==SUCC)
	{
        	printf("IC卡下电成功\n");
		getkeycode(2);
	}
	else
	{
        	printf("IC卡下电失败\n");
		getkeycode(0);
	}
}

///////////////////////////////////////////
////       比较SLE4428卡数据存贮区     ////
////        【00 B0 00 20 04】         ////	
///////////////////////////////////////////
static unsigned char comparesle4428maindata(unsigned char data_in)
{
	int i;
	int ret;

    icbuf[0]=0x00;
    icbuf[1]=0xB0;
	icbuf[2]=0x00;
	icbuf[3]=readaddress;  //读受写保护的区域&不受写保护的区域
	icbuf[4]=0x08;
	clrscr();
	ret = iccrw(0,5,icbuf,icreadbuf);
    if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]!=0x90)||(icreadbuf[icreadbuf[0]]!=0x00))
			return SUCC+1;
		
		for (i=1;i<(icreadbuf[0]+1-2);i++)
		{
            if (icreadbuf[i]!=data_in)
			    return SUCC+1;
		}
		return SUCC;
 	}
	else 
	    return SUCC+1;	
}
///////////////////////////////////////////
////       读SLE4428卡密码区           ////
////      【00 B0 03 fd 03】           ////	
////       0x3fd 错误计数器            ////
////       0x3fe,0x3fe密钥             ////
///////////////////////////////////////////
static void readsle4428securitymemory(void)
{
    int i;
	int ret;

    icbuf[0]=0x00;
    icbuf[1]=0xB0;
	icbuf[2]=0x03;
	icbuf[3]=0xfd;  
	icbuf[4]=0x03;
	clrscr();
	ret = iccrw(0,5,icbuf,icreadbuf);
    if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			printf("读密码区成功\n");
		else 
		{
			printf("读密码区失败\n");
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0];i++)
		{
            printf("%02x",icreadbuf[i+1]);
		    printf(" ");
		}
  	}
	else 
	{
		printf("读密码区失败\n");
        getkeycode(0);
		return;
	}
	getkeycode(0);
}

///////////////////////////////////////////
////       写SLE4442卡密码区           ////
////   【00 D0 C0 01 03 xx xx xx】     ////	
///////////////////////////////////////////
static unsigned char writesle4428securitymemory(unsigned char data_in1,unsigned char data_in2,unsigned char data_in3)
{
	int i;
	int ret;

    icbuf[0]=0x00;
    icbuf[1]=0xD0;
	icbuf[2]=0xC0;
	icbuf[3]=0x01;  
	icbuf[4]=0x03;
	icbuf[5]=data_in1;
	icbuf[6]=data_in2;
	icbuf[7]=data_in3;
	clrscr();
	ret = iccrw(0,8,icbuf,icreadbuf);
    if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			printf("写密钥成功\n");
		else 
		{
			printf("写密钥失败\n");
			getkeycode(0);
			return SUCC+1;
		}
		for (i=0;i<icreadbuf[0];i++)
		{
            printf("%02x",icreadbuf[i+1]);
		    printf(" ");
		}
  	}
	else 
	{
		printf("写密钥失败\n");
        getkeycode(0);
		return SUCC+1;
	}
	getkeycode(5);	
	return SUCC;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//=============================
//    07H：	AT24c64  卡
//=============================
static void poweronat24c64(void);
static void readat24c64(void);
static void writeat24c64(void);
static void powerdownat24c64(void);

static void poweronat24c128(void);
static void readat24c128(void);
static void writeat24c128(void);
static void powerdownat24c128(void);

static void poweronat24c256(void);
static void readat24c256(void);
static void writeat24c256(void);
static void powerdownat24c256(void);

static void testat24c64card(void)  //zjh 测试 at24c64 卡
{	    	
	while (1)
	{
		clrscr();
		printf(
			"①上电  ②下电\n"
			"③读数据\n"
			"④写数据\n"
			  );
		switch(getkeycode(0))
		{
		case '1':
			poweronat24c64();
			break;
		case '2':
			powerdownat24c64();
			break;
		case '3':
			readat24c64();
			break;
		case '4':
			writeat24c64();
			break;
				
		case ESC:
			return;
		default:
			break;
		}
	}
}

static void testat24c128card(void)  //zjh 测试 at24c64 卡
{	    	
	while (1)
	{
		clrscr();
		printf(
			"①上电  ②下电\n"
			"③读数据\n"
			"④写数据\n"
			  );
		switch(getkeycode(0))
		{
		case '1':
			poweronat24c128();
			break;
		case '2':
			powerdownat24c128();
			break;
		case '3':
			readat24c128();
			break;
		case '4':
			writeat24c128();
			break;
				
		case ESC:
			return;
		default:
			break;
		}
	}
}
static void testat24c256card(void)  //zjh 测试 at24c64 卡
{	    	
	while (1)
	{
		clrscr();
		printf(
			"①上电  ②下电\n"
			"③读数据\n"
			"④写数据\n"
			  );
		switch(getkeycode(0))
		{
		case '1':
			poweronat24c256();
			break;
		case '2':
			powerdownat24c256();
			break;
		case '3':
			readat24c256();
			break;
		case '4':
			writeat24c256();
			break;
				
		case ESC:
			return;
		default:
			break;
		}
	}
}

///////////////////////////////////////////
////       上电at24c64卡               ////
////                                   ////	
///////////////////////////////////////////
static void poweronat24c64(void)
{
	int i;

	SetICType(7,0); //这里有对5002的状态初始化，如果没有这句，当睡眠后，无法上电
    clrscr();
	if (icpowerup(0,icbuf)==SUCC)
	    printf("IC卡上电成功\n");
	else
	{
        printf("IC卡上电失败\n");
		getkeycode(0);
		return;
	}
    for (i=1;i<(icbuf[0]+1);i++)
	{
		printf("%02x",icbuf[i]);
		printf(" ");
	}
	getkeycode(0);
}
static void poweronat24c128(void)
{
	int i;

	SetICType(0x0e,0); //这里有对5002的状态初始化，如果没有这句，当睡眠后，无法上电
    clrscr();
	if (icpowerup(0,icbuf)==SUCC)
	    printf("IC卡上电成功\n");
	else
	{
        printf("IC卡上电失败\n");
		getkeycode(0);
		return;
	}
    for (i=1;i<(icbuf[0]+1);i++)
	{
		printf("%02x",icbuf[i]);
		printf(" ");
	}
	getkeycode(0);
}
static void poweronat24c256(void)
{
	int i;

	SetICType(0x0f,0); //这里有对5002的状态初始化，如果没有这句，当睡眠后，无法上电
    clrscr();
	if (icpowerup(0,icbuf)==SUCC)
	    printf("IC卡上电成功\n");
	else
	{
        printf("IC卡上电失败\n");
		getkeycode(0);
		return;
	}
    for (i=1;i<(icbuf[0]+1);i++)
	{
		printf("%02x",icbuf[i]);
		printf(" ");
	}
	getkeycode(0);
}

///////////////////////////////////////////
////       下电at24c64卡               ////
////                                   ////	
///////////////////////////////////////////
static void powerdownat24c64(void)
{
	clrscr();
	if (icpowerdown(0)==SUCC)
	{
        printf("IC卡下电成功\n");
		getkeycode(2);
	}
	else
	{
        printf("IC卡下电失败\n");
		getkeycode(0);
	}
}
static void powerdownat24c128(void)
{
	clrscr();
	if (icpowerdown(0)==SUCC)
	{
        printf("IC卡下电成功\n");
		getkeycode(2);
	}
	else
	{
        printf("IC卡下电失败\n");
		getkeycode(0);
	}
}
static void powerdownat24c256(void)
{
	clrscr();
	if (icpowerdown(0)==SUCC)
	{
        printf("IC卡下电成功\n");
		getkeycode(2);
	}
	else
	{
        printf("IC卡下电失败\n");
		getkeycode(0);
	}
}

//////////////////////////////////////////////
////       读at24c64卡数据存贮区          ////
////    【00  B0  Add1 Add2  Le】         ////	
//////////////////////////////////////////////
static void readat24c64(void)
{
	int i;
	int ret;
	unsigned char bflag=1;
	char buf[5];

	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	printf("0-8191\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	printf("%02x",ret);
	getkeycode(2);

	icbuf[2] = (ret >> 8);
    	icbuf[3] = (ret & 0xff);
	
	strcpy(buf,"");        //输入长度
	clrscr();
	printf("请输入长度\n");
	printf("0-252\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 12;
	ret=atol(buf);
	printf("%02x",ret);
	getkeycode(2);
	
    	icbuf[4]=(ret & 0xff);


    	icbuf[0]=0x00;
    	icbuf[1]=0xB0;
    	
	clrscr();
	icreadbuf[0] = 0;
	ret = iccrw(0,5,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
        	if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("读数据区失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				if (bflag!=1)
				{ 
					bflag = 1;
					printf(" ");
				}
				else
					bflag = 0;
			}
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0]-2;i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 		printf(" \n");
		readdata = icreadbuf[i-1]; //保存数据
 	}
	else 
	{
		printf("读数据区失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
static void readat24c128(void)
{
	int i;
	int ret;
	unsigned char bflag=1;
	char buf[6];

	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	printf("0-16383\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	printf("%02x",ret);
	getkeycode(2);

	icbuf[2] = (ret >> 8);
    	icbuf[3] = (ret & 0xff);
	
	strcpy(buf,"");        //输入长度
	clrscr();
	printf("请输入长度\n");
	printf("0-252\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 12;
	ret=atol(buf);
	printf("%02x",ret);
	getkeycode(2);
	
    	icbuf[4]=(ret & 0xff);


    	icbuf[0]=0x00;
    	icbuf[1]=0xB0;
    	
	clrscr();
	icreadbuf[0] = 0;
	ret = iccrw(0,5,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
        	if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("读数据区失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				if (bflag!=1)
				{ 
					bflag = 1;
					printf(" ");
				}
				else
					bflag = 0;
			}
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0]-2;i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 		printf(" \n");
		readdata = icreadbuf[i-1]; //保存数据
 	}
	else 
	{
		printf("读数据区失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
static void readat24c256(void)
{
	int i;
	int ret;
	unsigned char bflag=1;
	char buf[6];

	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	printf("0-32767\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	printf("%02x",ret);
	getkeycode(2);

	icbuf[2] = (ret >> 8);
    	icbuf[3] = (ret & 0xff);
	
	strcpy(buf,"");        //输入长度
	clrscr();
	printf("请输入长度\n");
	printf("0-252\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 12;
	ret=atol(buf);
	printf("%02x",ret);
	getkeycode(2);
	
    	icbuf[4]=(ret & 0xff);


    	icbuf[0]=0x00;
    	icbuf[1]=0xB0;
    	
	clrscr();
	icreadbuf[0] = 0;
	ret = iccrw(0,5,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
        	if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("读数据区失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				if (bflag!=1)
				{ 
					bflag = 1;
					printf(" ");
				}
				else
					bflag = 0;
			}
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0]-2;i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 		printf(" \n");
		readdata = icreadbuf[i-1]; //保存数据
 	}
	else 
	{
		printf("读数据区失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}


//////////////////////////////////////////////
////       写at24c64卡数据存贮区          ////
////    【 00   D0  Add1 Add2  Lc  Data】 ////	
//////////////////////////////////////////////
static void writeat24c64(void)
{
	int ret,i;
	char buf[5];

	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	printf("0-8191\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

	icbuf[2] = (ret >> 8);
    	icbuf[3] = (ret & 0xff);

    	icbuf[0]=0x00;
    	icbuf[1]=0xD0;

    	strcpy(buf,"");          //输入长度
	clrscr();
	printf("请设置长度\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
  	icbuf[4]=(ret&0xff);  //长度

    	strcpy(buf,"");          //输入数据
	clrscr();
	printf("请设置要写的数据\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

	for (i=0;i<icbuf[4];i++)
		icbuf[5+i]=(ret&0xff);
	
	clrscr();
	ret = iccrw(0,5+icbuf[4],icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
		    	printf("写数据区成功\n");
		else 
		{
			printf("写数据区失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
			}
			getkeycode(0);
			return;
		}
 	}
	else 
	{
		printf("写数据区失败\n");
        	getkeycode(0);
		return;
	}
	getkeycode(5);
}


static void writeat24c128(void)
{
	int ret,i;
	char buf[6];

	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	printf("0-16383\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

	icbuf[2] = (ret >> 8);
    	icbuf[3] = (ret & 0xff);

    	icbuf[0]=0x00;
    	icbuf[1]=0xD0;

    	strcpy(buf,"");          //输入长度
	clrscr();
	printf("请设置长度\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
  	icbuf[4]=(ret&0xff);  //长度

    	strcpy(buf,"");          //输入数据
	clrscr();
	printf("请设置要写的数据\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

	for (i=0;i<icbuf[4];i++)
		icbuf[5+i]=(ret&0xff);
	
	clrscr();
	ret = iccrw(0,5+icbuf[4],icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
		    	printf("写数据区成功\n");
		else 
		{
			printf("写数据区失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
			}
			getkeycode(0);
			return;
		}
 	}
	else 
	{
		printf("写数据区失败\n");
        	getkeycode(0);
		return;
	}
	getkeycode(5);
}
static void writeat24c256(void)
{
	int ret,i;
	char buf[6];

	strcpy(buf,"");          //输入地址
	clrscr();
	printf("请设置地址\n");
	printf("0-32767\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

	icbuf[2] = (ret >> 8);
    	icbuf[3] = (ret & 0xff);

    	icbuf[0]=0x00;
    	icbuf[1]=0xD0;

    	strcpy(buf,"");          //输入长度
	clrscr();
	printf("请设置长度\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
  	icbuf[4]=(ret&0xff);  //长度

    	strcpy(buf,"");          //输入数据
	clrscr();
	printf("请设置要写的数据\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

	for (i=0;i<icbuf[4];i++)
		icbuf[5+i]=(ret&0xff);
	
	clrscr();
	ret = iccrw(0,5+icbuf[4],icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
		    	printf("写数据区成功\n");
		else 
		{
			printf("写数据区失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
			}
			getkeycode(0);
			return;
		}
 	}
	else 
	{
		printf("写数据区失败\n");
        	getkeycode(0);
		return;
	}
	getkeycode(5);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//=============================
//    0dH：	AT45db041b  卡
//=============================
static void poweronat45(void);
static void readat45(void);
static void writeat45(void);
static void powerdownat45(void);

static void testat45(void)  
{	    	
	while (1)
	{
		clrscr();
		printf(
			"①上电  ②下电\n"
			"③读数据\n"
			"④写数据\n"
			  );
		switch(getkeycode(0))
		{
		case '1':
			poweronat45();
			break;
		case '2':
			powerdownat45();
			break;
		case '3':
			readat45();
			break;
		case '4':
			writeat45();
			break;
				
		case ESC:
			return;
		default:
			break;
		}
	}
}
///////////////////////////////////////////
////       上电at45db041b卡            ////
////                                   ////	
///////////////////////////////////////////
static void poweronat45(void)
{
	int i;

	SetICType(0x08,0); //这里有对5002的状态初始化，如果没有这句，当睡眠后，无法上电
    	clrscr();
	if (icpowerup(0,icbuf)==SUCC)
	    	printf("IC卡上电成功\n");
	else
	{
        	printf("IC卡上电失败\n");
		getkeycode(0);
		return;
	}
    	for (i=1;i<(icbuf[0]+1);i++)
	{
		printf("%02x",icbuf[i]);
		printf(" ");
	}
	getkeycode(0);
}

///////////////////////////////////////////
////       下电at45db041b卡            ////
////                                   ////	
///////////////////////////////////////////
static void powerdownat45(void)
{
	clrscr();
	if (icpowerdown(0)==SUCC)
	{
        	printf("IC卡下电成功\n");
		getkeycode(2);
	}
	else
	{
        	printf("IC卡下电失败\n");
		getkeycode(0);
	}
}
////////////////////////////////////////////////////
////       读at45db041b卡                       ////
////    【highadd  B0  midadd  lowadd  Le】     ////	
////////////////////////////////////////////////////
static void readat45(void)
{
	int i;
	int ret;
	unsigned char bflag=1;
	char buf[5];

	strcpy(buf,"0");          //输入page地址
	clrscr();
	printf("请设置page地址\n");
	printf("0-2047\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	ret = (ret<<1);  //highadd的低4位和midadd的高7位是page地址
    	icbuf[0]=((ret>>8)&0xff);  //highadd
	icbuf[1]=0xB0;             //读命令
    	icbuf[2]=(ret&0xff);     //midadd

    	strcpy(buf,"0");          //输入byte地址
	clrscr();
	printf("请设置byte地址\n");
	printf("0-263\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	if (ret>0xff)
		icbuf[2] = (icbuf[2]|1);
	    else
		icbuf[2] = (icbuf[2]&0xfe);
	icbuf[3] = (ret & 0xff);
	
    	strcpy(buf,"4");        //输入长度
	clrscr();
	printf("请输入长度\n");
	printf("0-252\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 12;
	ret=atol(buf);
	icbuf[4]=(ret & 0xff);
		
    	printf("%02x%02x%02x%02x",icbuf[0],icbuf[2],icbuf[3],icbuf[4]);
	getkeycode(0);   
    	
	clrscr();
	icreadbuf[0] = 0;
	ret = iccrw(0,5,icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
        	if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
			clrscr();
		else 
		{
			printf("读数据区失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
				if (bflag!=1)
				{ 
					bflag = 1;
					printf(" ");
				}
				else
					bflag = 0;
			}
			getkeycode(0);
			return;
		}
		for (i=0;i<icreadbuf[0]-2;i++)
		{
            		printf("%02x",icreadbuf[i+1]);
			portwrite(AUX1,1,icreadbuf+i+1);//debug 2005-1-13
            		printf(" ");
		}
 		printf(" \n");
		readdata = icreadbuf[i-1]; //保存数据
 	}
	else 
	{
		printf("读数据区失败\n");
		printf("%02x",ret);
		getkeycode(0);
		return;
	}
	getkeycode(0);
}
///////////////////////////////////////////////////////
////       写at45db041b卡                          ////
////    【 highadd  D0  midadd  lowadd  Lc  Data】 ////	
///////////////////////////////////////////////////////
static void writeat45(void)
{
	int ret,i;
	char buf[5];

	strcpy(buf,"0");          //输入page地址
	clrscr();
	printf("请设置page地址\n");
	printf("0-2047\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	ret = (ret<<1);  //highadd的低4位和midadd的高7位是page地址
    	icbuf[0]=((ret>>8)&0xff);  //highadd
	icbuf[1]=0xd0;             //写命令
    	icbuf[2]=(ret&0xff);     //midadd

    	strcpy(buf,"0");          //输入byte地址
	clrscr();
	printf("请设置byte地址\n");
	printf("0-263\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);
	if (ret>0xff)
		icbuf[2] = (icbuf[2]|1);
	    else
		icbuf[2] = (icbuf[2]&0xfe);
	icbuf[3] = (ret & 0xff);
	
    	strcpy(buf,"4");        //输入长度
	clrscr();
	printf("请输入长度\n");
	printf("0-252\n");
	ret=getnumstr(buf,4,2,0);
	if(ret<=0)
		ret = 12;
	ret=atol(buf);
	icbuf[4]=(ret & 0xff);
		
    	printf("%02x%02x%02x%02x",icbuf[0],icbuf[2],icbuf[3],icbuf[4]);
	getkeycode(0);

	strcpy(buf,"0");          //输入要写的数据
	clrscr();
	printf("请设置要写的数据\n");
	ret=getnumstr(buf,5,2,0);
	if(ret<=0)
		ret = 0;
	ret=atol(buf);

    	for (i=0;i<icbuf[4];i++)  //赋值
		icbuf[5+i] = ret;
	
	clrscr();
	ret = iccrw(0,5+icbuf[4],icbuf,icreadbuf);
    	if(ret==SUCC)
	{   
		if((icreadbuf[icreadbuf[0]-1]==0x90)&&(icreadbuf[icreadbuf[0]]==0x00))
		    	printf("写数据区成功\n");
		else 
		{
			printf("写数据区失败\n");
			for (i=0;i<icreadbuf[0];i++)
			{
				printf("%02x",icreadbuf[i+1]);
				portwrite(AUX1,1,icreadbuf+i);//debug 2005-1-13
			}
			getkeycode(0);
			return;
		}
 	}
	else 
	{
		printf("写数据区失败\n");
        	getkeycode(0);
		return;
	}
	getkeycode(5);
}

void general26(void)
{
	testmemcard();
	return;
}

#endif

