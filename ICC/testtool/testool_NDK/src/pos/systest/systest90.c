/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 蓝牙打印机综合测试	
* file name			: systest90.c
* Author 			: 	
* version			: 
* DATE				: 
* directory 			: 
* description			: 蓝牙打印机综合测试
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20131202  chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"
#include <pthread.h>

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"蓝牙打印机综合"
#define PRNT_DATALEN   1024
#define CMD_LEN   3
#define TIMEOUT   10

#define FS   0x1c



/*------------global variables definition-----------------------*/
int comm_type = 0;
char bt_mac[7]={0};
int initflag=0;
/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/


/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 	:
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 				: author			date			remarks
					 	jiangym			20150611	created
*****************************************************************/	
static int ME50P_data_factory(char *cmd, int cmdlen)
{
	if(comm_type == PORT_NUM_BT) //蓝牙通讯方式
	{
		int ret=0;
		//清指定设备缓冲区NDK_BTMasterClrBuf
		if((ret = NDK_BTMasterClrBuf(bt_mac)) != NDK_OK)
			return ret;
#if 0		
		for(i=0; i<cmdlen; i++) //输出具体指令内容
			fprintf(stderr,"0x%02X ",cmd[i]);
#endif
		//往从设备写数据
		if((ret=NDK_BTMasterWrite(bt_mac, cmdlen, cmd)) != NDK_OK)
			return ret;
		
		return NDK_OK;
	}
	else
	{
		cls_show_msg("通讯方式配置错误，请重新配置");
		return NDK_ERR;
	}
}

static int ME50P_data_recv(int *len, char *recvbuf, int *recvlen)
{
	if(comm_type == PORT_NUM_BT) //蓝牙通讯方式
	{
		int ret=0;
		//往从设备读数据
		if((ret=NDK_BTMasterRead(bt_mac, len, recvbuf, TIMEOUT, recvlen)) != NDK_OK)
			return ret;	
		return NDK_OK;
	}
	else
	{
		cls_show_msg("通讯方式配置错误，请重新配置");
		return NDK_ERR;
	}
}

static int ME50P_cmd_Prnt(char *databuf, int datalen) //打印数据
{
	char data[PRNT_DATALEN]={0};
	int ret=0;
	
	memset(data, 0, sizeof(data));
	memcpy(data, databuf, datalen);
	data[datalen]=0x0a; //数据发送完要加0a
	if( (ret=ME50P_data_factory(data, datalen+1)) != NDK_OK )
		return ret;
	
	return NDK_OK;
}

static int ME50P_cmd_Send(char *databuf, int datalen) //将数据发送到缓冲区
{
	char data[PRNT_DATALEN]={0};
	int ret=0;
	
	memset(data, 0, sizeof(data));
	memcpy(data, databuf, datalen);
	if( (ret=ME50P_data_factory(data, datalen)) != NDK_OK )
		return ret;
	
	return NDK_OK;
}

static int ME50P_cmd_SetHT(void) //设置水平位置
{
	char cmd[CMD_LEN-2]={0};
	int ret=0;
	
	cmd[0]=0x09;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetLF(void) //设置打印并换行
{
	char cmd[CMD_LEN-2]={0};
	int ret=0;
	
	cmd[0]=0x0A;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_TransRealState(char n, int *len, char *recvbuf, int *recvlen) //实时状态传送
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	cmd[0]=0x10;
	cmd[1]=0x04;
	cmd[2]=n;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	memset(recvbuf,0,sizeof(recvbuf));
	if( (ret=ME50P_data_recv(len, recvbuf, recvlen)) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetSpaceRight(char n) //设置字符右间距
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1b;
	cmd[1]=0x20;
	cmd[2]=n;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetPrntMode(char mode) //设置打印模式
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1b;
	cmd[1]=0x21;
	cmd[2]=mode;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetDefaultLS(void) //设置默认行间距
{
	char cmd[CMD_LEN-1]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1b;
	cmd[1]=0x32;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetLS(char n) //设置行间距
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1b;
	cmd[1]=0x33;
	cmd[2]=n;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_InitPrnt(void)   //打印初始化
{
	char cmd[CMD_LEN-1]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1b;
	cmd[1]=0x40;
	//cls_show_msg("0x%2x,0x%2x",cmd[0],cmd[1]);
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetPrntPaper(char n)   //打印并走纸
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1B;
	cmd[1]=0x4A;
	cmd[2]=n;	
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetPrntFont(char f) //设置字体
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1b;
	cmd[1]=0x4d;
	cmd[2]=f;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetR90DC(char n) //选择/取消顺时针旋转90度
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=ESC;
	cmd[1]='V';
	cmd[2]=n;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetAlignment(char n) //选择对齐方式
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1b;
	cmd[1]=0x61;
	cmd[2]=n;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetInvertedPrntMode(char mode) //选择/取消倒置打印模式
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1b;
	cmd[1]=0x7b;
	cmd[2]=mode;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetFontSize(char size)	//设置字体大小
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1d;
	cmd[1]=0x21;
	cmd[2]=size;
	//cls_show_msg("0x%2x,0x%2x,0x%2x",cmd[0],cmd[1],cmd[2]);
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetReShowMode(char mode) //选择/取消黑白返显打印模式
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1d;
	cmd[1]=0x42;
	cmd[2]=mode;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetLeftMargin(char nL, char nH) //设置左边距
{
	char cmd[CMD_LEN+1]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1d;
	cmd[1]=0x4c;
	cmd[2]=nL;
	cmd[3]=nH;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetHVMovingUnit(char x, char y) //设置横向和纵向移动单位
{
	char cmd[CMD_LEN+1]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1d;
	cmd[1]=0x50;
	cmd[2]=x;
	cmd[3]=y;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetPrntAreaWidth(char nL, char nH) //设置打印区域宽度
{
	char cmd[CMD_LEN+1]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1d;
	cmd[1]=0x57;
	cmd[2]=nL;
	cmd[3]=nH;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetCUnderlineMode(char n) //选择/取消汉字下划线模式
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1c;
	cmd[1]=0x2d;
	cmd[2]=n;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

static int ME50P_cmd_SetCHeightWidth(char n) //选择/取消汉字倍高倍宽
{
	char cmd[CMD_LEN]={0};
	int ret=0;
	
	memset(cmd, 0, sizeof(cmd));
	cmd[0]=0x1c;
	cmd[1]=0x57;
	cmd[2]=n;
	if( (ret=ME50P_data_factory(cmd, sizeof(cmd))) != NDK_OK )
		return ret;
	return NDK_OK;
}

//指令1
static void test_ME50P_cmd_SetHT(void)
{
	int ret=0;
	char *databuf[]={"测试水平定位中...",
					  "本数据0123456789前应首行缩进96个点，第二行数据应该是正常打印无缩进",
					  "结束水平定位"};
	//前置
	cls_printf("测试水平定位中...");
	if( (ret=ME50P_cmd_Prnt(databuf[0], strlen(databuf[0]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}

	//case1.1:标准模式下，首行应缩进96个点
	if( (ret=ME50P_cmd_SetPrntMode(0x00)) != NDK_OK ) 
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_SetHT()) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[1], strlen(databuf[1]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}

	//其他模式下的水平定位设计如何,是否需要考虑

	//后置
	ME50P_cmd_Prnt(databuf[2], strlen(databuf[2]));
	return;
}

//指令2
//设置默认行间距后，换行的距离为5mm，与3.75mm不一致
static void test_ME50P_cmd_SetLF(void)
{
	int ret=0;
	char *databuf[]={"测试LF指令，打印数据0123456789abc",
					   "该行数据应是从新的一行开始打印"};
	
	cls_printf("测试打印并换行中...");
	//case1:先发送打印数据到蓝牙打印机缓冲区,调用SetLF指令后才会开始打印,并且打印后会换行
	if( (ret=ME50P_cmd_Send(databuf[0], strlen(databuf[0]))) != NDK_OK )
	{
		cls_show_msg("line %d:测试失败(%d)", __LINE__, ret);
		return;
	}
	if(cls_show_msg("蓝牙打印机应不打印出任何数据,是[ENTER],否[其他]") != ENTER)
	{
		cls_show_msg("line %d:测试失败", __LINE__);
		return;
	}
	if( (ret=ME50P_cmd_SetLF()) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[1], strlen(databuf[1]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}

	return;
}

//指令3
static void test_ME50P_cmd_TransRealState(char n, char *databuf, int datalen, int *len, char *recvbuf, int *recvlen)
{
	//开发还未实现,但可以先开始设计

	//case1:未送打印数据情况下获取到的状态应该是准备好的状态

	//case2:发送大量打印数据,让打印正在打印的时候获取打印状态,应该要获取到打印机忙的状态

	//case3:跟开发沟通还有哪些打印机状态,构造相应的用例去验证这些状态的获取是否正确
	int ret=0;
	if( (ret=ME50P_cmd_TransRealState(n, len, recvbuf, recvlen)) != NDK_OK )
		return;
	if(n==2)
	if( (ret=ME50P_cmd_Prnt(databuf, datalen)) != NDK_OK )
		return;
	
	return;
}

//指令6
static void test_ME50P_cmd_SetSpaceRight(void)
{
	int ret=0;
	char *databuf[]={"测试最大右间距31.91mm",
					 "打印的数据12345678abc右间距为10mm"};

	cls_printf("测试右间距中...");
	//case4.1:在标准模式下，设置右间距为最大右间距，实际应与预期相符
	if( (ret=ME50P_cmd_SetPrntMode(0x00)) != NDK_OK ) 
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_SetSpaceRight(255)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[0], strlen(databuf[0]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}

	//case4.3:设置右间距为10mm(n=80)，实际应与预期相符
	if( (ret=ME50P_cmd_SetSpaceRight(80)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[1], strlen(databuf[1]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	//case4.2:当设置超过最大右间距的时候，会自动转换为最大右间距.由于btwrite传入的buf为char型，最大只有255，无法测大于255的情况

	//放大模式下,右间距放大相应倍数,另外间距的距离换算应该要引入公式计算,不要使用常量,应该用n和换算规律来计算

ERR:
	//设置回原先的右间距
	ME50P_cmd_SetSpaceRight(0);
	return;
}

//指令7 
static void test_ME50P_cmd_SetPrntMode(void) //待验证
{
	int ret=0, i=0;
	char mode[4]={0x00,0x10,0x20,0x80};

	//这里还有压缩ASCII码字体B等其它模式没考虑到?0x01
	char *databuf[]={"本行打印出的数据应该是标准ASCII码",
					  "本行打印出的数据应该是倍高模式",
					  "本行打印出的数据应该是倍宽模式",
					  "本行打印出的数据应该是下划线模式"};
	
	cls_printf("测试打印模式中...");
	//case5:测试标准/倍高/倍宽/下划线打印模式
	for(i=0;i<4;i++)
	{
		if( (ret=ME50P_cmd_SetPrntMode(mode[i])) != NDK_OK )
		{
			cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
			goto ERR;
		}
		if( (ret=ME50P_cmd_Prnt(databuf[i], strlen(databuf[i]))) != NDK_OK )		
		{
			cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
			goto ERR;
		}
	}

ERR:
	ME50P_cmd_SetPrntMode(0x00);
	return;
}

//指令13和指令14
//最大行间距并不为956mm，实际只有670mm，显示数字有问题
static void test_ME50P_cmd_SetLS(void)
{
	int ret=0;
	char *databuf[]={"打印数据0123456789abc，并且该段数据的行间距应为956mm",
					 "打印数据abc9876543210，并且该段数据的行间距应为500mm",
					 "打印数据0123456789abc，并且该段数据的行间距为默认行间距3.75mm",
					 "结束测试行间距"};

	cls_printf("测试行间距中...");
	//case6.1:正常测试:在标准模式下，行间距设置为最大行间距，实际应与预期相符
	if( (ret=ME50P_cmd_SetPrntMode(0x00)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_SetLS(255)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[0], strlen(databuf[0]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}

	//case6.2:正常测试:在标准模式下，行间距设置为中等间距，实际应与预期相符 
	if( (ret=ME50P_cmd_SetLS(133)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[1], strlen(databuf[1]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}

	//case6.3:正常测试:在标准模式下，行间距设置超过最大间距的值，应取最大间距。由于btwrite传入的buf为char型，最大只有255，无法测大于255的情况

	//case6.4:设置为默认行间距
	if( (ret=ME50P_cmd_SetDefaultLS()) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[2], strlen(databuf[2]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}

	if( (ret=ME50P_cmd_Prnt(databuf[3], strlen(databuf[3]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
ERR:
	ME50P_cmd_SetLS(0);
	return;
}

//指令17
//初始化后，原先在缓冲区的数据仍然会被打印出来。若单独跑用例，原先缓冲区的数据会被清空。
static void test_ME50P_cmd_InitPrnt(void)
{
	int ret=0;
	char *databuf[]={"应不该打印出数据abc0123456789\n",
					 "应能打印出数据9876543210abc"};
	
	cls_printf("测试打印机初始化中...");
	//case1:打印机初始化，可以清除打印缓冲区的数据
	if( (ret=ME50P_cmd_InitPrnt()) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_Send(databuf[0], strlen(databuf[0]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_InitPrnt()) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[1], strlen(databuf[1]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret); 
		return;
	}

	return;
}

//指令21
//无打印走纸的效果
static void test_ME50P_cmd_SetPrntPaper(void)
{
	int ret=0;
	char *databuf[]={"打印并走纸,走纸距离应为956mm",
					 "打印并走纸,走纸距离应为375mm",
					 "打印并走纸,走纸距离应为375mm并且此时行间距为最大行间距\n测试结束"};
	ME50P_cmd_SetHVMovingUnit(0,0);
	
	cls_printf("测试打印机走纸中...");
	//case8.1:标准模式下，设置走纸距离为最大走纸距离，实际应与预期一致
	if( (ret=ME50P_cmd_SetPrntMode(0x00)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_SetPrntPaper(255)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[0], strlen(databuf[0]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}

	//case8.2:设置走纸距离n=100，实际应与预期一致
	if( (ret=ME50P_cmd_SetPrntPaper(100)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[1], strlen(databuf[1]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	
	//case8.3:设置超过最大走纸距离，应取最大距离.由于btwrite传入的buf为char型，最大只有255，无法测大于255的情况
	//case8.4:行间距设置为最大时，走纸距离不应该受影响
	if( (ret=ME50P_cmd_SetLS(255)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[2], strlen(databuf[2]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	
ERR:
	//后置
	ME50P_cmd_SetPrntPaper(0);
	ME50P_cmd_SetLS(0);
	return;
}

//指令22
//压缩字体带下划线
static void test_ME50P_cmd_SetPrntFont(void)
{
	int ret=0, i=0, nKeyin=0;
	char prntdata[MAXDISPBUFSIZE]="1234567890123456789";//打印数据选择不合适,风格不一样,建议参考test_ME50P_cmd_SetR90DC命令构造打印数据
	int mode[4]={1,49,0,48};
	
	cls_printf("测试设置打印字体中...");
	//case9.1:正常测试字体大小
	for(i=0; i<4; i++)
	{
		if( (ret=ME50P_cmd_SetPrntFont(mode[i])) != NDK_OK )
		{
			cls_show_msg("line %d:发送指令失败(i=%d, ret=%d)", __LINE__, i, ret);
			goto ERR;
		}
		if( (ret=ME50P_cmd_Prnt(prntdata, strlen(prntdata))) != NDK_OK )		
		{
			cls_show_msg("line %d:发送指令失败(i=%d, ret=%d)", __LINE__, i, ret);
			goto ERR;
		}
		if(i==0 || i==1)
			nKeyin=cls_show_msg("是否打印出压缩ASCII码且数据为%s,是[ENTER],否[其他]", prntdata);
		else
			nKeyin=cls_show_msg("是否打印出标准ASCII码且数据为%s,是[ENTER],否[其他]", prntdata);  
		if(nKeyin != ENTER)
		{
			cls_show_msg("line %d:测试失败(i=%d)", __LINE__, i);
			ME50P_cmd_SetPrntFont(0);
			return;
		}
	}

	//case9.2:异常测试,当传入非n范围中的数值，应还是最后一次设置的字符类型
	if( (ret=ME50P_cmd_SetPrntFont(2)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(prntdata, strlen(prntdata))) != NDK_OK )		
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if(cls_show_msg("是否打印出标准ASCII码且数据为%s,是[ENTER],否[其他]", prntdata) != ENTER)
	{
		cls_show_msg("line %d:测试失败", __LINE__);
		ME50P_cmd_SetPrntFont(0);
		return;
	}
	
ERR:	
	ME50P_cmd_SetPrntFont(0);
	return;
}

//指令24
//1.n=48本应取消旋转，可实际并没有取消2.n为非范围内的值，是旋转90度3.倍宽倍高旋转的方向和正常的一样
static void test_ME50P_cmd_SetR90DC(void)
{
	int ret=0, i=0;
	char *databuf[]={"打印的数据12345678ABC顺时针旋转90度",
					  "打印的数据ABC87654321取消顺时针旋转90度",
					  "打印的数据12345678abc顺时针旋转90度且无下划线",
					  "打印的数据abc87654321顺时针旋转90度且倍高倍宽,且方向与上面数据旋转的方向相反"};
	char mode[4]={1,49,0,48};
	
	cls_printf("测试顺时针旋转90度中...");
	//仅在标准模式下有效果
	if( (ret=ME50P_cmd_SetPrntMode(0x00)) != NDK_OK ) 
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	
	//case10.1:正常测试
	for(i=0; i<4; i++)
	{
		if( (ret=ME50P_cmd_SetR90DC(mode[i])) != NDK_OK )
		{
			cls_show_msg("line %d:发送指令失败(i=%d, ret=%d)", __LINE__, i, ret);
			goto ERR;
		}
		if( (ret=ME50P_cmd_Prnt(databuf[i/2], strlen(databuf[i/2]))) != NDK_OK )		
		{
			cls_show_msg("line %d:发送指令失败(i=%d, ret=%d)", __LINE__, i, ret);
			goto ERR;
		}
	}
	//case10.2:异常测试,当传入非n范围中的数值，应还是最后一次设置的取消90度旋转
	if( (ret=ME50P_cmd_SetR90DC(50)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;//实际为都是顺时针旋转
	}
	if( (ret=ME50P_cmd_Prnt(databuf[1], strlen(databuf[1]))) != NDK_OK )		
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}

	//case10.3:当选择下划线模式时,下划线不能顺时针选择90度,打印时不加下划线
	if( (ret=ME50P_cmd_SetCUnderlineMode(1)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_SetR90DC(1)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetCUnderlineMode(0);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[2], strlen(databuf[2]))) != NDK_OK )		
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetCUnderlineMode(0);
		goto ERR;
	}
	//取消下划线	
	if( (ret=ME50P_cmd_SetCUnderlineMode(0)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetCUnderlineMode(0);
		goto ERR;
	}

	//倍高和倍宽  要分别调用GS ! n (0x1D 0x21 n)  指令设置后进行测试,实际效果设置倍高,旋转90度后变成倍宽
	
	//case10.4:顺时针旋转90度模式下的倍高和倍宽与正常模式下的方向相反。  //实际效果并没有
	if( (ret=ME50P_cmd_SetCHeightWidth(1)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[3], strlen(databuf[3]))) != NDK_OK )		
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetCHeightWidth(0);
		goto ERR;
	}
	//取消倍高倍宽模式	
	if( (ret=ME50P_cmd_SetCHeightWidth(0)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetCHeightWidth(0);
		goto ERR;
	}

ERR:
	ME50P_cmd_SetR90DC(0);//取消顺时针旋转90度
	ME50P_cmd_SetCUnderlineMode(0);
	return;
}

//指令26
//n=49本应中间对齐,n=50本应右对齐,可实际是左对齐;n为非范围内的值，是左对齐
static void test_ME50P_cmd_SetAlignment(void)
{
	int ret=0, i=0;
	char *databuf[]={"打印的数据1234567890\n以中间对齐方式打印",
					 "打印的数据0987654321\n以右对齐方式打印",
					 "打印的数据1234567890\n以左对齐方式打印"};
	int mode[6]={1,49,2,50,0,48};

	cls_printf("测试左右中对齐中...");
	//仅在标准模式下有效果
	if( (ret=ME50P_cmd_SetPrntMode(0x00)) != NDK_OK ) 
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	
	//case11.1:正常测试
	for(i=0; i<6; i++)
	{
		if( (ret=ME50P_cmd_SetAlignment(mode[i])) != NDK_OK )
		{
			cls_show_msg("line %d:发送指令失败(i=%d, ret=%d)", __LINE__, i, ret);
			goto ERR;
		}
		if((ret=ME50P_cmd_Prnt(databuf[i/2], strlen(databuf[i/2]))) != NDK_OK)
		{
			cls_show_msg("line %d:发送指令失败(i=%d, ret=%d)", __LINE__, i, ret);
			goto ERR;
		}
	}
	//case11.2:异常测试,当传入非n范围中的数值，应还是最后一次设置的对齐方式左对齐
	if( (ret=ME50P_cmd_SetAlignment(3)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[2], strlen(databuf[2]))) != NDK_OK )		
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}	

ERR:
	ME50P_cmd_SetAlignment(0);	
	return;
}

//指令33
//n的最低位为0时预期应取消倒置，实际并没有取消,除了n=0会取消
static void test_ME50P_cmd_SetInvertedPrntMode(void)
{
	int ret=0;
	char *databuf[]={"打印的数据1234567890是倒置打印",
					 "打印的数据0987654321取消倒置打印"};

	cls_printf("测试打印倒置中...");
	//仅在标准模式下有效果
	if( (ret=ME50P_cmd_SetPrntMode(0x00)) != NDK_OK ) 
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	
	//case12.1:正常测试，倒置打印
	if( (ret=ME50P_cmd_SetInvertedPrntMode(255)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[0], strlen(databuf[0]))) != NDK_OK )		
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	
	//case12.2:取消倒置打印
	if( (ret=ME50P_cmd_SetInvertedPrntMode(0)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[1], strlen(databuf[1]))) != NDK_OK )		
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}

	//case12.3:n取值超过范围，不应该跑飞.由于btwrite传入的buf为char型，最大只有255，无法测大于255的情况

ERR:
	ME50P_cmd_SetInvertedPrntMode(0);
	return;
}
//指令36
static void test_ME50P_cmd_SetFontSize(void)
{
	int ret=0, i=0;
	char *databuf[]={"打印12345678abc为2倍宽2倍高,且右间距也随之放大",
					 "打印87654321abc为2倍宽正常高,且右间距也随之放大",
					 "打印12345678abc为正常宽2倍高,右间距无放大",
					 "打印87654321abc为正常宽正常高",
				     "打印12345678abc为2倍宽正常高,且顺时针旋转90度",
				     "打印87654321abc为正常宽2倍高,且顺时针旋转90度",};
	char mode[4]={0x11,0x10,0x01,0x00};
	
	cls_printf("测试设置打印字体大小中...");
	//case13.1:当字符放大时，右间距随之放大相同的倍数
	for(i=0;i<4;i++)
	{
		if( (ret=ME50P_cmd_SetFontSize(mode[i])) != NDK_OK )
		{
			cls_show_msg("line %d:发送指令失败(i=%d, ret=%d)", __LINE__, i, ret);
			goto ERR;
		}
		if( (ret=ME50P_cmd_Prnt(databuf[i], strlen(databuf[i]))) != NDK_OK )
		{
			cls_show_msg("line %d:发送指令失败(i=%d, ret=%d)", __LINE__, i, ret);
			goto ERR;
		}
	}
	//case13.2:异常测试,当传入非n范围中的数值，该命令忽视.由于btwrite传入的buf为char型，最大只有255，无法测大于255的情况

	//case13.3:顺时针选择90°时，横向和纵向颠倒。
	if( (ret=ME50P_cmd_SetR90DC(1)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_SetFontSize(0x10)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[4], strlen(databuf[4]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_SetFontSize(0x01)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[5], strlen(databuf[5]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}

	//后置
ERR:
	ME50P_cmd_SetFontSize(0x00);
	ME50P_cmd_SetR90DC(0);
	return;
}

//指令39
//设置右间距后，间距预期应返显，实际未返显。
static void test_ME50P_cmd_SetReShowMode(void)
{
	int ret=0;
	char *databuf[]={"打印的数据12345678abc应黑白返显",
					 "打印的数据87654321abc为黑白返显,无下划线",
					 "打印的数据12345678abc取消返显",
					 "打印的数据87654321abc无黑白返显,有下划线",
					 "打印的数据12345678abc右间距加大,字符和间距都黑白返显",
					 "打印的数据87654321abc字符前空白处不应黑白返显\n\n上一行空白行不应该返显"};

	cls_printf("测试设置打印字体大小中...");
	if( (ret=ME50P_cmd_SetPrntMode(0x00)) != NDK_OK ) 
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	//case14.1:n最低位为1时,表示选择黑白返显打印模式
	if( (ret=ME50P_cmd_SetReShowMode(255)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[0], strlen(databuf[0]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}

	//case14.2:在黑白返显打印模式下，设置下划线，下划线应该不起作用
	if( (ret=ME50P_cmd_SetCUnderlineMode(1)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[1], strlen(databuf[1]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	
	//case14.6:在黑白返显打印模式下，设置字符间距，字符间距也应返显。
	if( (ret=ME50P_cmd_SetSpaceRight(100)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[4], strlen(databuf[4]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetSpaceRight(0);
		goto ERR;
	}
	if( (ret=ME50P_cmd_SetSpaceRight(0)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetSpaceRight(0);
		goto ERR;
	}
	
	//case14.7:在黑白返显打印模式下，HT设置，空白部分不会返显。
	if( (ret=ME50P_cmd_SetHT()) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[5], strlen(databuf[5]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	
	//case14.3:n最低位为0时,表示取消黑白返显打印模式
	if( (ret=ME50P_cmd_SetReShowMode(0)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[2], strlen(databuf[2]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	
	//case14.4:在取消黑白返显打印模式下，设置下划线，下划线应该起作用
	if( (ret=ME50P_cmd_SetCUnderlineMode(2)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[3], strlen(databuf[3]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	//case14.5:当n不在0~255范围内，不应跑飞.由于btwrite传入的buf为char型，最大只有255，无法测大于255的情况

	//后置
ERR:
	ME50P_cmd_SetReShowMode(0);//取消黑白返显模式
	ME50P_cmd_SetCUnderlineMode(0);//取消下划线模式
	return;
}

//指令41, 手册里限制只是用nL字节
//改变横向纵向移动单位，预期应不影响当前左边距，但实际有影响
static void test_ME50P_cmd_SetLeftMargin(void)
{
	int ret=0;
	char *databuf[]={"该行数据的左边距为32.8mm",
					 "改变横向纵向移动单位，不影响当前左边距"};

	cls_printf("测试左边距中...");
	ME50P_cmd_SetHVMovingUnit(0,0);
	if( (ret=ME50P_cmd_SetPrntMode(0x00)) != NDK_OK ) 
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	//case15.1:设置左边距
 	if( (ret=ME50P_cmd_SetLeftMargin(255,255)) != NDK_OK ) //255/200英寸么
 	{
 		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
 		goto ERR;
 	}
	if( (ret=ME50P_cmd_Prnt(databuf[0], strlen(databuf[0]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
 		goto ERR;
	}

	//case15.3:改变横向纵向移动单位，不影响当前左边距
	if( (ret=ME50P_cmd_SetHVMovingUnit(255,255)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
 		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[1], strlen(databuf[1]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetHVMovingUnit(0,0);
 		goto ERR;
	}
	//case15.2:设置超过最大可用打印宽度，取最大可用打印宽度.由于btwrite传入的buf为char型，最大只有255，无法测大于255的情况
	
	
	//测试后置
ERR:
	ME50P_cmd_SetLeftMargin(0,0);
	return;
}

//指令42,默认x=1,y=0.5
//为何不会变化
static void test_ME50P_cmd_SetHVMovingUnit(void)
{
	int ret=0;
	char *databuf[]={"测试横向和纵向移动单位中...",
					 "本数据横向距离为1/8mm,纵向距离大约为1/7mm,但右间距,左间距和打印区域不变",
					 "本数据倒置,但横向距离仍为1/8mm,纵向距离仍为1/7mm",
					 "本数据顺时针旋转90°,但横向距离仍为1/8mm,纵向距离仍为1/7mm"};	
	
	cls_printf("测试横向和纵向移动单位中...");
	//恢复成默认
	ME50P_cmd_SetHVMovingUnit(0,0);
	if( (ret=ME50P_cmd_Prnt(databuf[0], strlen(databuf[0]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	
	//case16.1:改变横向或纵向移动单位，不改变当前右间距,右间距和打印区域。
	if( (ret=ME50P_cmd_SetHVMovingUnit(1,1)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[1], strlen(databuf[1]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}

	//case16.2:设置倒置,字符移动单位也不会改变
	if( (ret=ME50P_cmd_SetInvertedPrntMode(1)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[2], strlen(databuf[2]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetInvertedPrntMode(0);
		goto ERR;
	}
	if( (ret=ME50P_cmd_SetInvertedPrntMode(0)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetInvertedPrntMode(0);
		goto ERR;
	}

	//case16.3:设置顺时针旋转90°,字符移动单位也不会改变
	if( (ret=ME50P_cmd_SetR90DC(1)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[3], strlen(databuf[3]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetR90DC(0);
		goto ERR;
	}	
	if( (ret=ME50P_cmd_SetR90DC(0)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetR90DC(0);
		goto ERR;
	}
	
	//恢复成默认
ERR:	
	ME50P_cmd_SetHVMovingUnit(0,0);
	return;
}

//指令43
//无法打印出任何内容来，且影响后面打印内容
static void test_ME50P_cmd_SetPrntAreaWidth(void)
{
	int ret=0;
	char *databuf[]={"测试打印区域宽度...",
					 "测试打印区域宽度1..."};
	
	cls_printf("测试打印区域宽度中...");
	if( (ret=ME50P_cmd_SetPrntMode(0x00)) != NDK_OK ) 
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	//case:默认情况下看打印出来是什么样的
	if( (ret=ME50P_cmd_SetPrntAreaWidth(1,1)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[0], strlen(databuf[0]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}	

	//case17.1:如果超出可打印区域，则打印区域宽度为可打印区宽度减去左边距
	if( (ret=ME50P_cmd_SetPrntAreaWidth(255,255)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[0], strlen(databuf[0]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}	
	//case:改变横向和纵向移动单位不影响当前左边距和区域宽度
	return;
}

//指令53
//只有n=0的时候才会打印出下划线
static void test_ME50P_cmd_SetCUnderlineMode(void)
{
	int ret=0, i=0;
	char n[6]={2, 50, 1, 49, 0, 48};
	char *databuf[]={"打印的数据12345678abc正常大小且下划线为2点宽",
					 "打印的数据abc87654321正常大小且下划线为1点宽",
					 "打印的数据12345678abc倍高倍宽且下划线为1点宽",
					 "打印的数据abc87654321正常大小、下划线为1点宽且HT引起的空格没有下划线",
					 "打印的数据12345678abc正常大小、下划线为1点宽且右间距也有下划线",
					 "打印的数据12345678abc正常大小且无下划线"};
	
	cls_printf("测试打印下划线中...");
	if( (ret=ME50P_cmd_SetPrntMode(0x00)) != NDK_OK ) 
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	
	//case18.1:设置下划线(2点宽和1点宽)
	for(i=0;i<4;i++)
	{
		if( (ret=ME50P_cmd_SetCUnderlineMode(n[i])) != NDK_OK )
		{
			cls_show_msg("line %d:发送指令失败(i=%d, ret=%d)", __LINE__,i, ret);
			goto ERR;
		}
		if( (ret=ME50P_cmd_Prnt(databuf[i/2], strlen(databuf[i/2]))) != NDK_OK )
		{
			cls_show_msg("line %d:发送指令失败(i=%d, ret=%d)", __LINE__,i, ret);
			goto ERR;
		}	
	}

	//case18.2:改变字体大小，下划线线宽应该不变
	if( (ret=ME50P_cmd_SetFontSize(0x11)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[2], strlen(databuf[2]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetFontSize(0x00);
		goto ERR;
	}	
	if( (ret=ME50P_cmd_SetFontSize(0x00)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetFontSize(0x00);
		goto ERR;
	}

	//case18.3:HT命令引起的空格不会加下划线
	if( (ret=ME50P_cmd_SetHT()) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[3], strlen(databuf[3]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}

	//case18.4:左右间距应该也会有下划线
	if( (ret=ME50P_cmd_SetSpaceRight(20)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[4], strlen(databuf[4]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetSpaceRight(0);
		goto ERR;
	}
	if( (ret=ME50P_cmd_SetSpaceRight(0)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		ME50P_cmd_SetSpaceRight(0);
		goto ERR;
	}

	//case18.5:取消下划线
	for(i=4;i<6;i++)
	{
		if( (ret=ME50P_cmd_SetCUnderlineMode(n[i])) != NDK_OK )
		{
			cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
			goto ERR;
		}
		if( (ret=ME50P_cmd_Prnt(databuf[5], strlen(databuf[5]))) != NDK_OK )
		{
			cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
			goto ERR;
		}	
	}
ERR:
	ME50P_cmd_SetCUnderlineMode(0);//取消下划线
	return;
}

//指令57
static void test_ME50P_cmd_SetCHeightWidth(void)
{
	int ret=0;
	char *databuf[]={"打印的数据12345678abc仅汉字倍高倍宽,数字字母仍为正常大小",
					  "打印的数据abc87654321汉字和数字字母都正常大小"};

	cls_printf("测试倍高倍宽中...");
	//case.1:当n的最低位为1，表示为选择汉字倍高倍宽模式
	if( (ret=ME50P_cmd_SetCHeightWidth(1)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[0], strlen(databuf[0]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	//case.1:当n的最低位为0，表示为取消汉字倍高倍宽模式，应打印出正常大小
	if( (ret=ME50P_cmd_SetCHeightWidth(0)) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		goto ERR;
	}
	if( (ret=ME50P_cmd_Prnt(databuf[1], strlen(databuf[1]))) != NDK_OK )
	{
		cls_show_msg("line %d:发送指令失败(%d)", __LINE__, ret);
		return;
	}

ERR:
	ME50P_cmd_SetCHeightWidth(0);
	return;
}
	
void ME50P_test_function(void)
{
	/*private & local definition*/
	PF pfunc[] = {test_ME50P_cmd_SetHT, test_ME50P_cmd_SetLF, test_ME50P_cmd_SetSpaceRight, test_ME50P_cmd_SetPrntMode, 
				  test_ME50P_cmd_SetLS, test_ME50P_cmd_InitPrnt, test_ME50P_cmd_SetPrntPaper, test_ME50P_cmd_SetPrntFont,
				  test_ME50P_cmd_SetR90DC, test_ME50P_cmd_SetAlignment, test_ME50P_cmd_SetInvertedPrntMode, test_ME50P_cmd_SetFontSize,
				  test_ME50P_cmd_SetReShowMode, test_ME50P_cmd_SetLeftMargin, test_ME50P_cmd_SetHVMovingUnit, /*test_ME50P_SetPrntAreaWidth,*/
				  test_ME50P_cmd_SetCUnderlineMode, test_ME50P_cmd_SetCHeightWidth};
	int id = 0;
	
	/*process body*/
	ME50P_cmd_SetPrntMode(2);
	if(initflag!=1)
	{
		cls_show_msg("未配置参数，请重新配置");
		return;
	}
	for(id=0; id<SZ_ARRAY(pfunc); id++)
		pfunc[id]();
	
	cls_show_msg("测试通过");
	return;
}

void testpress_ME50P_testpage(void) //打印测试页
{
	/*private & local definition*/
	char buf[1024] = {0};
	int i = 0, ret = 0, j=0;
	int mode[3]={0x01,0x10,0x11};

	/*process body*/
	memset(buf,0,sizeof(buf));
	for (i=0;i<sizeof(buf);i++)
	{
		if(!(buf[i]=rand()%256))
			break;
	}
	
	ME50P_cmd_Prnt("---以下打印正常大小随机字符---\n" , strlen("---以下打印正常大小随机字符---\n"));
	//case1:设置正常大小，打印随机数
	if( (ret=ME50P_cmd_SetPrntMode(0x00)) != NDK_OK )
	{
		cls_show_msg("line %d:打印测试页失败(%d)", __LINE__, ret);
		return;
	}
	if( (ret=ME50P_cmd_Prnt(buf , strlen(buf))) != NDK_OK ) 	
	{
		cls_show_msg("line %d:打印测试页失败(%d)", __LINE__, ret);
		return;
	}
	
	for(j=0;j<3;j++)
	{
		memset(buf,0,sizeof(buf));
		for (i=0;i<sizeof(buf);i++)
		{
			if(!(buf[i]=rand()%256))
				break;
		}
		//case2:设置正常宽倍高模式/设置倍宽正常高模式/设置倍高倍宽模式,打印随机数
		if(j==0)
			ME50P_cmd_Prnt("\n\n---以下打印正常宽倍高大小随机字符---\n" , strlen("\n\n---以下打印正常宽倍高大小随机字符---\n"));
		else if(j==1)
			ME50P_cmd_Prnt("\n\n---以下打印倍宽正常高大小随机字符---\n" , strlen("\n\n---以下打印倍宽正常高大小随机字符---\n"));
		else
			ME50P_cmd_Prnt("\n\n---以下打印倍高倍宽大小随机字符---\n" , strlen("\n\nk---以下打印倍高倍宽大小随机字符---\n"));
		if( (ret=ME50P_cmd_SetFontSize(mode[j])) != NDK_OK )
		{
			cls_show_msg("line %d:打印测试页失败(%d)", __LINE__, ret);
			goto ERR;
		}
		if( (ret=ME50P_cmd_Prnt(buf , strlen(buf))) != NDK_OK )		
		{
			cls_show_msg("line %d:打印测试页失败(%d)", __LINE__, ret);
			goto ERR;
		}
	}
	
ERR:
	ME50P_cmd_SetFontSize(0x00);
	return;
}

void testpress_ME50P_prntguo(void) //打印国字页，观察是否卡纸，是否对齐
{
	/*private & local definition*/
	char data[1]={0x0a};
	int i = 0, ret = 0;

	/*process body*/
	ME50P_cmd_SetPrntMode(0x00);
	ME50P_cmd_Prnt("---以下打印240个\"国\"---\n" , strlen("---以下打印240个\"国\"---\n"));
	NDK_BTMasterClrBuf(bt_mac);
	for (i=0;i<240;i++) //要等很久才打印?
	{
		if( (ret=NDK_BTMasterWrite(bt_mac, strlen("国"), "国" )) != NDK_OK ) 	
		{
			cls_show_msg("line %d:打印\"国\"字页失败!(%d)", __LINE__, ret);
			return;
		}
	}
	NDK_BTMasterWrite(bt_mac, 1, data);
	
	return;
}

void testpress_ME50P_prntbill(void) //打印票据
{
	/*private & local definition*/
	int i = 0;
    char billbuf[35][1024]={"银联POS签购单\n",
					        "--------------------------------\n",
					        "持卡人存根         CUSTOMER COPY\n",
					        "--------------------------------\n",
					        "商户名称(MERCHANT Name):\n  测试商户名称\n",
					        "商户号(MERCHANT NO.):\n  123456789012345\n",
					        "终端编号(TERMINAL NO.):12345678\n",
					        "操作员号(Operator NO.):01\n",
					        "--------------------------------\n",
					        "收单行(ACQUIRER):\n  收单银行 12345678\n",
					        "发卡行(ISSUER):发卡银行 12345678\n",
					        "卡号(CARD NO.):\n",
					        "  6200123456789012345\n\n",
					        "交易类型(TRANS TYPE):消费/SALE\n",
		 			        "有效期(EXP Date):2015/01\n",
					        "批次号(BATCH  NO.):123456\n",
					        "凭证号(VOUCHER  NO.):789012\n",
					        "授权码(AUTH  NO.):345678\n",
					        "交易参考号(REFER NO.):\n  901234567890\n",
					        "日期/时间(DATE/TIME):\n  2019/07/18  12:34:56\n",
					        "交易金额(AMOUNT):",
					        "12345678.90\n",
					        "--------------------------------\n",
					        "应用标识(AID)1234567890123456\n",
					        "备注/REFERENCE\n",
					        "这里是发卡银行备注信息\n",
					        "这里是发卡银行备注信息\n",
					        "这里是发卡银行备注信息\n",
					        "郑重申明:该笔交易为测试状态,属无效交易!\n",
					        "--------------------------------\n",
					        "持卡人签名:\n\n\n\n\n",
					        "本人确认以上交易，同意将其记入本卡账户\n",
					        "I ACKNOWLEDGE SATISFACTORY RECEIPT OF RELATIVE GOODS/SERVICE\n",
					        "\n\n",
					        "- - - - - - - X - - - - - - - X - - - - - - - \n\n\n\n\n\n"};

	/*process body*/
	ME50P_cmd_SetPrntMode(0x00);
	ME50P_cmd_Prnt("---以下打印票据---\n" , strlen("---以下打印票据---\n"));
	for(i=0;i<35;i++)
		ME50P_cmd_Prnt(billbuf[i] , strlen(billbuf[i]));
	
	return;
}

void testpress_ME50P_prntenter(void) //打印回车
{
	/*private & local definition*/
	int i=0;

	/*process body*/
	ME50P_cmd_SetPrntMode(0x00);
	
	ME50P_cmd_Prnt("---以下连续打印5个回车--\n" , strlen("---以下连续打印5个回车--\n"));
	for(i=0;i<5;i++)
		ME50P_cmd_SetLF();
	
	ME50P_cmd_Prnt("---以下连续打印20个回车--\n" , strlen("---以下连续打印20个回车--\n"));
	for(i=0;i<20;i++)
		ME50P_cmd_SetLF();
	
	ME50P_cmd_Prnt("----连续打印回车结束----\n" , strlen("----连续打印回车结束----\n"));
	
	return;
}

void testpress_ME50P_prntcompress(void) //打印压缩,奇偶空白行,奇偶文本行,奇偶回车行
{
	/*private & local definition*/
	int i=0, ret=0 ;
	char data[1]={0x0a};
	
	/*process body*/
	//奇偶空白行
	ME50P_cmd_SetPrntMode(0x01);
	ME50P_cmd_Prnt("---以下打印压缩效果---\n" , strlen("---以下打印压缩效果---\n"));
	
	ME50P_cmd_Prnt("          \r" , strlen("          \r"));//换行一次
	cls_printf("\n正在打印,请稍等\n奇偶行\n");
	for (i=1;i<=24;i++)
	{
		if( (ret=NDK_BTMasterWrite(bt_mac, strlen("国"), "国" )) != NDK_OK ) 	
		{
			cls_show_msg("line %d:打印\"国\"字页失败!(%d)", __LINE__, ret);
			return;
		}
	}
	NDK_BTMasterWrite(bt_mac, 1, data);

	ME50P_cmd_Prnt("          \r" , strlen("          \r"));
	ME50P_cmd_Prnt("          \r" , strlen("          \r"));//换行二次
	for (i=1;i<=24;i++)
	{
		if( (ret=NDK_BTMasterWrite(bt_mac, strlen("国"), "国" )) != NDK_OK )	
		{
			cls_show_msg("line %d:打印\"国\"字页失败!(%d)", __LINE__, ret);
			return;
		}
	}
	NDK_BTMasterWrite(bt_mac, 1, data);
	
	ME50P_cmd_Prnt("          \r" , strlen("          \r"));
	ME50P_cmd_Prnt("          \r" , strlen("          \r"));
	ME50P_cmd_Prnt("          \r" , strlen("          \r"));//换行三次
	for (i=1;i<=24;i++)
	{
		if( (ret=NDK_BTMasterWrite(bt_mac, strlen("国"), "国" )) != NDK_OK )	
		{
			cls_show_msg("line %d:打印\"国\"字页失败!(%d)", __LINE__, ret);
			return;
		}
	}
	NDK_BTMasterWrite(bt_mac, 1, data);

	ME50P_cmd_Prnt("\n打印前奇偶空白行\n" , strlen("\n打印前奇偶空白行\n"));//提示

	//奇偶文本行
	ME50P_cmd_Prnt("行间奇偶空格\n" , strlen("行间奇偶空格\n"));
	ME50P_cmd_Prnt("国  国   国国国国  国国 \r" , strlen("国  国   国国国国  国国 \r"));
	ME50P_cmd_Prnt(" 国 国  国国国  国国 \r", strlen(" 国 国  国国国  国国 \r"));

	//奇偶回车行
	ME50P_cmd_Prnt("\r" , strlen("\r"));
	for (i=1;i<=24;i++)
	{
		if( (ret=NDK_BTMasterWrite(bt_mac, strlen("国"), "国" )) != NDK_OK )	
		{
			cls_show_msg("line %d:打印\"国\"字页失败!(%d)", __LINE__, ret);
			return;
		}
	}
	NDK_BTMasterWrite(bt_mac, 1, data);
	
	ME50P_cmd_Prnt("打印前一个回车行\n" , strlen("打印前一个回车行\n"));//提示

	ME50P_cmd_Prnt("\r\r\r" , strlen("\r\r\r"));
	for (i=1;i<=24;i++)
	{
		if( (ret=NDK_BTMasterWrite(bt_mac, strlen("国"), "国" )) != NDK_OK )	
		{
			cls_show_msg("line %d:打印\"国\"字页失败!(%d)", __LINE__, ret);
			return;
		}
	}
	NDK_BTMasterWrite(bt_mac, 1, data);
	ME50P_cmd_Prnt("打印前三个回车行\n" , strlen("打印前三个回车行\n"));//提示

	ME50P_cmd_Prnt("\r\r\r\r" , strlen("\r\r\r\r"));
	for (i=1;i<=24;i++)
	{
		if( (ret=NDK_BTMasterWrite(bt_mac, strlen("国"), "国" )) != NDK_OK )	
		{
			cls_show_msg("line %d:打印\"国\"字页失败!(%d)", __LINE__, ret);
			return;
		}
	}
	NDK_BTMasterWrite(bt_mac, 1, data);
	ME50P_cmd_Prnt("打印前四个回车行\n" , strlen("打印前四个回车行\n"));//提示
	
	return;
}

void testpress_ME50P_prntlandi(void) //打印联迪
{
	int i=0;
	char databuf[12][1024]={"备份电池：3.02V\n",
						    "WELCOME TO POS EQUIPMENT\n",
						    "国国国国国国国国国国国国国国国国国国国国国国\n",
						    "国国国国国国国国国国国国国国国国国国国国国国\n",
						    "档位打印效果：\n",
						    "档位4(#2)\n",
						    "||  ||  ||  ||  ||\n",
						    "||  ||  ||  ||  ||\n",
						    "||  ||  ||  ||  ||\n",
						    "各针出针效果\n",
						    " 1 2 3 4 5 6 7 8 9 \n",
						    "--------------------------------------------------------------------\n"};
	
	ME50P_cmd_SetPrntMode(0x00);
	for(i=0;i<12;i++)
		ME50P_cmd_Prnt(databuf[i] , strlen(databuf[i]));
	
	return;
}

void testpress_ME50P_prntxinguodu(void) //打印新国都
{
	int i=0;
	char databuf[12][1024]={"\r\r",
						    "福州市仓山区个性服饰店\r\r",
						    "            309350156911410\r",
						    "       11100495         01\r\r",
						    "   622200*********7950\r\r",
						    "     01020000    兴业银行\r\r",
						    "   消费/SALE      2000/00\r\r",
						    "   000003          000934\r\r",
						    "   012345\r",
						    "   151019019346\r",
						    "   RMB 0.01\r\r",
						    "   RMB 0.01"};

	ME50P_cmd_SetPrntMode(0x00);
	for(i=0;i<12;i++)
		ME50P_cmd_Prnt(databuf[i] , strlen(databuf[i]));
	
	return;
}

void ME50P_test_press(void)
{
	/*private & local definition*/
#define	DEFAULT_CNT_STR	"1"//"200"	//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)	//(200)
	int cnt = DEFAULT_CNT_VLE, bak = 0, ret = 0;
	uint len = 0;
	char CntStr[8] = DEFAULT_CNT_STR;

	//三角块/填充单/竖条纹/空白单/图形打印需要打印图形，不支持
	//打印测试页、随机数、国字页、票据、模式字体组合、回车、压缩、联迪单据、新国都单据
	PF pfunc[] = {testpress_ME50P_testpage, testpress_ME50P_prntguo, testpress_ME50P_prntbill, testpress_ME50P_prntenter, 
				  testpress_ME50P_prntcompress, testpress_ME50P_prntlandi, testpress_ME50P_prntxinguodu};
	int id = 0;
	
	/*process body*/
	cls_printf("默认测试次数:\n");
	if ((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &len, INPUTDISP_OTHER, 5, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);

	while(cnt)
	{
		if (ESC==cls_show_msg1(2, "压力测试中...\n还剩%d次,请注意打印效果,ESC中断测试", cnt))
			break;
		cnt--;

		for(id=0; id<SZ_ARRAY(pfunc); id++)
		{
			pfunc[id]();
			if(wait_anykey(1)==ESC)
				goto OUT;
		}
	}

OUT:
	cls_show_msg_record(FILENAME ,FUNCNAME ,"压力测试完成,已执行次数为%d", bak-cnt);
	return;
}

void ME50P_test_abnormal(void)
{
	/*private & local definition*/
	int nKeyin = 0;

	/*process body*/
	while(1)
	{
		nKeyin = cls_show_msg("1.长时间放置\n"
							  "2.休眠异常\n");
		//case1:测试长时间放置蓝牙打印机，然后连接，看是否能连接成功并打印。
		switch (nKeyin)
		{
		case '1':
			cls_show_msg("请长时间放置蓝牙打印机后,再进行正常打印,蓝牙打印机与POS应正常连接，并且打印应能正常");
 			break;
		case '2':
			cls_show_msg("请确保已设置休眠功能，POS休眠唤醒后，与蓝牙打印机应正常连接，且打印应能正常");
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

void init_connect(int conn_type)
{
	/*private & local definition*/
	int ret = -1;
	char btmac[7] = {0};
	
	if(comm_type == PORT_NUM_BT)
	{
		//测试前置
		cls_show_msg("请打开蓝牙打印机后,任意键继续");
		NDK_PortClose(PORT_NUM_BT);
		if((ret=NDK_PortOpen(PORT_NUM_BT,NULL))!=NDK_OK)
		{
			cls_show_msg("line %d:打开蓝牙串口失败(%d)", __LINE__, ret);
			return ;
		}
		//扫描获取mac
		if((ret=lib_btmaster_scan(btmac, conn_type))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备扫描获取mac失败(%d)", __LINE__, ret);
			goto ERR;	
		}
		//连接(从设备提示连接)
		if((ret=NDK_BTMasterConnect(btmac,conn_type))!=NDK_OK)
		{
			cls_show_msg("line %d:主设备连接从设备失败(%d)", __LINE__, ret);
			goto ERR;	
		}
		//未配对的会进行配对,已配对的就直接连接上
		if((ret=connect_pair_process(btmac,1))!=NDK_OK)
		{
			cls_show_msg("line %d:主从设备连接配对失败(%d)", __LINE__, ret);
			goto ERR;
		}
		memcpy(bt_mac, btmac, 8);
	}
	
	initflag=1;
	return;
	
ERR:
	//关闭串口
	NDK_PortClose(comm_type);
	return;
}

void bt_set(void)
{
	int keyin=0;
	int conn_type = 0;

	/*process body*/
	while(1)
	{
		keyin = cls_show_msg("请选择连接方式:\n"
							 "1.默认方式\n"
							 "2.SPP方式\n" 
							 "3.BLE 方式");
		switch(keyin) 
		{
			case '1':
			case '2':
			case '3':
				conn_type = keyin-'1';
				init_connect(conn_type);
				return;
				break;
			case ESC:
				return;
				break;
			default:
				cls_show_msg1(3,"请选择正确的蓝牙连接方式，退出请按取消键");
				break;
		}
	}
	return;
}

void comm_set(void)
{
	int nKeyin=0;

	/*process body*/
	while(1)
	{
		nKeyin = cls_show_msg("请选择通讯方式:"
							  "1.蓝牙通讯\n");
		switch(nKeyin) 
		{
			case '1':
				comm_type = PORT_NUM_BT;
				bt_set();				
				return;
				break;
			case ESC:
				return;
				break;
			default:
				cls_show_msg1(3,"请选择正确的通讯方式，退出请按取消键");
				break;
		}
	}	
	return;
}

void systest90(void)
{
	if(auto_flag == 1)
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}
	
	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);
	
	setlinktype(BT);//设置通讯类型为BT，避免数据包太大引起失败
	while(1)
	{
		int nKeyin = -1;
		nKeyin=cls_show_msg("1.功能 2.压力\n"
							"3.异常\n"
							"0.配置\n");
		switch(nKeyin)
		{
			case '1':
				ME50P_test_function();
				break;		
			case '2':
				ME50P_test_press();
				break;		
			case '3':
				ME50P_test_abnormal();
				break;		
			case '0':
				comm_set();
				break;
			case ESC:
				NDK_PortClose(comm_type);
				return;
			default:
				continue;
				break;
		}
	}
	return;
}

