/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name		: systest97.c
* Author 			: 
* version			: 
* DATE			: 
* directory 	: 
* description	: 	HID串口综合测试
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	MAXWAITTIME 	30	//30
#define	TESTITEM		"HID串口测试"
#define DEFAULT_CNT_VLE	2048//3000
#define	DEFAULT_CNT_STR	"2048"//"3000"
#define DEFAULT_TEST_VLE  10
#define DEFAULT_TEST_STR  "10"

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
* history 				: author			date			remarks
*					          sull              20180910          created
*****************************************************************/
static void hid_openclosepress()
{
	int ret = 0, i = 0, cnt = 0, succ = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uint unLen = 0;
	
	/*process body*/
	cls_printf("默认测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	
	//注意：测试过程中，相当于键盘处于一直拔插的状态，所以打字时会变慢
	if(cls_show_msg("请确保POS和PC已通过USB线连接,确认键继续...")!=ENTER)
		return; 

	//测试前置:关闭HID串口
	NDK_PortClose(PORT_NUM_HID);

	while(1)
	{
		cls_printf("HID串口开关压力测试中\n总共:%d次,已执行:%d次,已成功:%d次",cnt,i,succ);
		if(i++>=cnt)//达到测试次数后退出
			break;	
		if((ret=NDK_PortOpen(PORT_NUM_HID, "115200,8,N,1")) != NDK_OK)
		{
			if(cls_show_msg1(g_keeptime, "line %d:第%d次打开HID串口失败", __LINE__, i)==ESC)
				break;
			else
				continue;
		}
		sleep(3);
		if((ret=NDK_PortClose(PORT_NUM_HID))!=NDK_OK)
		{
			if(cls_show_msg1(g_keeptime, "line %d:第%d次关闭HID串口失败", __LINE__, i)==ESC)
				break;
			else
				continue;
		}
		succ++;
	}
	
	cls_show_msg1(g_keeptime,"总共进行%d次开关HID压力测试,已成功%d次",i-1,succ);
	NDK_PortClose(PORT_NUM_HID);
	return;

}

static void hid_open_comn_close()
{
	/*private & local definition*/
	int ret = -1, i = 0, succconut = 0,cnt=0;
	char CntStr[8] = DEFAULT_CNT_STR,sendBuf[BUFSIZE_SERIAL+1]={0};
	uint unLen=0;
	char str1[ ] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ~`!@#$%^&*()_-+={[]}\\ |\'\";:<,>.?/";
	
	/*process body*/	
	cls_printf("默认测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);

	//测试前置：关闭HID串口
	NDK_PortClose(PORT_NUM_HID);
	
	//生产测试数据包并发送给后台
	for(i=0;i<BUFSIZE_SERIAL;i++)
		sendBuf[i] = str1[rand()%(sizeof(str1)-1)];

	cls_show_msg1(5,"请打开任一txt文档,并将鼠标移到该位置,测试过程中请不要移走鼠标,5s后开始发送数据");	
	
	//读写压力测试
	i=0;
	while(1)
	{
		cls_printf("HID串口写压力中\n总共:%d次\n已进行:%d次\n",cnt,i);
		if(i++>=cnt)
			break;
		
		if((ret = NDK_PortOpen(PORT_NUM_HID, "115200,8,N,1")) != NDK_OK)
		{
			cls_show_msg1(g_keeptime,"line %d:打开HID失败(%d)", __LINE__, ret);
			NDK_PortClose(PORT_NUM_HID);
			return;
		}
		if((ret=NDK_PortWrite(PORT_NUM_HID, BUFSIZE_SERIAL, sendBuf))!=NDK_OK)
		{
			if(cls_show_msg1(g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret)==ESC)
				break;
			else
				continue;
		}
		succconut++;//通讯成功次数增加	
		NDK_PortClose(PORT_NUM_HID);
	}
	cls_show_msg1(g_keeptime,"HID串口写压力测试完成,执行次数:%d次,成功%d次", i-1, succconut);
	NDK_PortClose(PORT_NUM_HID);
	return;

}

static void hid_send_data()
{
	int ret = -1, i = 0, cnt=0, succ = 0;
	char sendBuf[1024+1]={0},CntStr[8] = DEFAULT_CNT_STR;
	uint unLen=0;
	char str1[ ] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ~`!@#$%^&*()_-+={[]}\\ |\'\";:<,>.?/";

	/*process body*/	
	cls_printf("默认POS发送数据到PC次数:\n");
	if((ret=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_CNT_VLE;
	else
		cnt = atoi(CntStr);
	
	//测试前置:关闭HID串口
	NDK_PortClose(PORT_NUM_HID);
	
	cls_show_msg1(5,"请打开任一txt文档，将鼠标移到该位置，测试过程中请不要移走鼠标,5s后开始发送数据");	
	if((ret = NDK_PortOpen(PORT_NUM_HID, "115200,8,N,1")) != NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:打开HID失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_HID);
		return;
	}
	//生产测试数据包并发送给后台
	memset(sendBuf, 0, sizeof(sendBuf));
	for(i=0;i<sizeof(sendBuf)-1;i++)
	{
		sendBuf[i] = str1[rand()%(sizeof(str1)-1)];
	}
	//写数据
	NDK_PortWrite(PORT_NUM_COM1, 1024, sendBuf);
	for(i=0;i<cnt;i++)
	{
		if((ret=NDK_PortWrite(PORT_NUM_HID, 1024, sendBuf))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i+1, ret);
			continue; 
		}
		succ++;
	}
	if(cls_show_msg("PC端接收到的数据是否为%dB?是按[ENTER],否为[其它]",cnt*1024)!=ENTER)
	{
		cls_show_msg1(g_keeptime, "line %d:连续发送数据测试失败", __LINE__);
		NDK_PortClose(PORT_NUM_HID);
		return; 
	}
	cls_show_msg1(2,"测试完毕");
	NDK_PortClose(PORT_NUM_HID);
	return;

}

static void hid_usb_test()
{
	char CntStr[8] = DEFAULT_TEST_STR;
	char sendBuf[BUFSIZE_SERIAL+1]={0}, recvBuf[BUFSIZE_SERIAL+1]={0};
	uint unLen = 0;
	int ret = 0, cnt = 0, i = 0, j = 0, recvLen = 0;
	int succ1 = 0, succ2 = 0;
	char str1[ ] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ~`!@#$%^&*()_-+={[]}\\ |\'\";:<,>.?/";

	/*process body*/	
	cls_printf("默认交叉测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_TEST_VLE;
	else
		cnt = atoi(CntStr);

	while(i<cnt*2)
	{
		i++;
		memset(sendBuf, 0, sizeof(sendBuf));
		for(j=0;j<sizeof(sendBuf)-1;j++)
		{
			sendBuf[j] = str1[rand()%(sizeof(str1)-1)];
		}
		if(i%2)
		{
			cls_printf("第%d次HID测试...", i/2+1);
			if((ret = NDK_PortOpen(PORT_NUM_HID, "115200,8,N,1")) != NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:第%d次打开HID失败(%d)", __LINE__, i,ret);
				NDK_PortClose(PORT_NUM_HID);
				continue;
			}
			cls_show_msg1(5,"请打开任一txt文档，将鼠标移到该位置，测试过程中请不要移走鼠标,5s后将发送%dB数据",sizeof(sendBuf));
			if((ret=NDK_PortWrite(PORT_NUM_HID, BUFSIZE_SERIAL, sendBuf))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
				NDK_PortClose(PORT_NUM_HID);
				continue;
			}
			//为了验证HID发送的数据是否正确，故往COM1口发送相同数据以进行比较
			NDK_PortWrite(PORT_NUM_COM1,BUFSIZE_SERIAL,sendBuf);
			if((ret = NDK_PortClose(PORT_NUM_HID)) != NDK_OK)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次关闭HID串口失败(%d)", __LINE__, i, ret);
				NDK_PortClose(PORT_NUM_HID);
				continue;
			}
			succ1++;
		}
		else
		{
			cls_printf("第%d次USB测试...", i/2);
			if((ret = NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
			{
				cls_show_msg1(g_keeptime,"line %d:第%d次打开USB串口失败(%d)", __LINE__, i,ret);
				NDK_PortClose(PORT_NUM_USB);
				continue;
			}
			cls_show_msg("请打开accessport串口，按任意键继续");
			if((ret = NDK_PortClrBuf(PORT_NUM_USB)) != NDK_OK)
			{				
				cls_show_msg1(g_keeptime,"line %d:第%d次清缓冲区失败(%d)", __LINE__, i,ret);
				NDK_PortClose(PORT_NUM_USB);
				continue;
			}
			if((ret=NDK_PortWrite(PORT_NUM_USB, BUFSIZE_SERIAL, sendBuf))!=NDK_OK)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
				NDK_PortClose(PORT_NUM_USB);
				continue;
			}
			cls_show_msg("请将AccessPort接收到的数据复制到发送框并发送，按任意键继续");
			memset(recvBuf, 0, sizeof(recvBuf));
			if((ret=NDK_PortRead(PORT_NUM_USB,BUFSIZE_SERIAL, recvBuf, 10*1000, &recvLen)) != NDK_OK)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, recvLen, BUFSIZE_SERIAL);
				NDK_PortClose(PORT_NUM_USB);
				continue;
			}
			if(recvLen != BUFSIZE_SERIAL)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i, recvLen, BUFSIZE_SERIAL);
				NDK_PortClose(PORT_NUM_USB);
				continue;
			}
			if(MemCmp(sendBuf, recvBuf,  BUFSIZE_SERIAL))
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次数据校验失败(recvlen=%d)", __LINE__, i, recvLen);
				NDK_PortClose(PORT_NUM_USB);
				continue;
			}
			if((ret = NDK_PortClose(PORT_NUM_USB)) != NDK_OK)
			{
				cls_show_msg1(g_keeptime, "line %d:第%d次关闭USB串口失败(%d)", __LINE__, i, ret);
				NDK_PortClose(PORT_NUM_USB);
				continue;
			}
			cls_show_msg("请关闭accessport串口，按任意键继续");
			succ2++;
		}
	}
	cls_show_msg("HID总通讯次数%d次，成功%d次", cnt, succ1);
	cls_show_msg("USB总通讯次数%d次，成功%d次", cnt, succ2);
	return;		
}

static void com_hid_test()
{
	int ret = 0, j = 0, cnt = 0, i =0, succ = 0;
	char buf[BUFSIZE_SERIAL+1]={0}, rbuf[BUFSIZE_SERIAL+1]={0};
	int len = 0;
	char CntStr[8] = DEFAULT_TEST_STR;
	uint unLen = 0;
	char str1[ ] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ~`!@#$%^&*()_-+={[]}\\ |\'\";:<,>.?/";

	cls_printf("默认测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_TEST_VLE;
	else
		cnt = atoi(CntStr);

	//产生测试数据
	memset(buf, 0, sizeof(buf));
	for(j=0;j<sizeof(buf)-1;j++)
	{
		buf[j] = str1[rand()%(sizeof(str1)-1)];
	}

	//打开串口
	if((ret = NDK_PortOpen(PORT_NUM_COM1, "115200,8,N,1")) != NDK_OK)
	{
		cls_show_msg1(1,"line %d:打开HID失败(%d)", __LINE__, ret);
		return;
	}
	if((ret=NDK_PortOpen(PORT_NUM_HID, "115200,8,N,1"))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:发送失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_HID);
		return;
	}

	while(i++ <cnt)
	{
		//清缓冲区
		if((ret = NDK_PortClrBuf(PORT_NUM_COM1)) != NDK_OK)
		{				
			cls_show_msg1(1,"line %d:第%d次清缓冲区失败(%d)", __LINE__, i,ret);
			continue;
		}
		//COM1写数据
		if((ret=NDK_PortWrite(PORT_NUM_COM1, BUFSIZE_SERIAL, buf))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:发送失败(%d)", __LINE__, ret);
			continue;
		}
		cls_show_msg("打开串口工具,将发送的内容复制到串口工具发送框,并发送,按任意键继续");
		//COM1接收数据
		memset(rbuf, 0, sizeof(rbuf));
		if((ret=NDK_PortRead(PORT_NUM_COM1,BUFSIZE_SERIAL, rbuf, 10*1000, &len)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, len, BUFSIZE_SERIAL);
			continue;
		}
		//数据比较
		if(len != BUFSIZE_SERIAL)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i, len, BUFSIZE_SERIAL);
			continue;
		}
		if(MemCmp(buf, rbuf,  BUFSIZE_SERIAL))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次数据校验失败(recvlen=%d)", __LINE__, i, len);
			continue;
		}
		//写HID
		cls_show_msg1(5, "请打开txt文档,并将鼠标移动到该位置,测试过程中请不要移走鼠标,5s后将发送数据");
		if((ret=NDK_PortWrite(PORT_NUM_HID, BUFSIZE_SERIAL, buf))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次发送失败(%d)", __LINE__, i, ret);
			continue;
		}
		succ++;
	}
	//关闭HID串口
	if((ret=NDK_PortClose(PORT_NUM_HID))!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:关闭HID串口失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_HID);
		return;
	}

	cls_show_msg1(g_keeptime,"通讯总次数%d次，成功%d次", cnt, succ);
	return;
}

static void com_usb_test()
{
	int ret = 0, j = 0, cnt = 0, i =0, succ = 0;
	char buf1[BUFSIZE_SERIAL+1]={0}, buf2[BUFSIZE_SERIAL+1]={0},rbuf[BUFSIZE_SERIAL+1]={0};
	int len = 0;
	char CntStr[8] = DEFAULT_TEST_STR;
	uint unLen = 0;
	char str1[ ] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ~`!@#$%^&*()_-+={[]}\\ |\'\";:<,>.?/";

	cls_printf("默认测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 1, sizeof(CntStr)-1, &unLen,2, 0, INPUT_CONTRL_LIMIT_ERETURN))==NDK_ERR||ret==NDK_ERR_TIMEOUT)
		cnt = DEFAULT_TEST_VLE;
	else
		cnt = atoi(CntStr);

	memset(buf1, 0, sizeof(buf1));
	for(j=0;j<sizeof(buf1)-1;j++)
	{
		buf1[j] = str1[rand()%(sizeof(str1)-1)];
	}
	memset(buf2, 0, sizeof(buf2));
	for(j=0;j<sizeof(buf2)-1;j++)
	{
		buf2[j] = str1[rand()%(sizeof(str1)-1)];
	}

	//打开串口
	if((ret = NDK_PortOpen(PORT_NUM_COM1, "115200,8,N,1")) != NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:打开HID失败(%d)", __LINE__, ret);
		return;
	}
	if((ret = NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1")) != NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:打开HID失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB);
		return;
	}
	cls_show_msg("请打开accessport和串口工具并连接,按任意键开始测试");

	while(i++ <cnt)
	{	
		cls_show_msg("请清空accessport和串口工具发送框数据,按任意键继续测试");
		//写数据
		if((ret=NDK_PortWrite(PORT_NUM_COM1, BUFSIZE_SERIAL, buf1))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:发送失败(%d)", __LINE__, ret);
			continue;
		}
		if((ret=NDK_PortWrite(PORT_NUM_USB, BUFSIZE_SERIAL, buf2))!=NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:发送失败(%d)", __LINE__, ret);
			continue;
		}
		//读数据
		cls_show_msg("将串口接收到的数据复制到串口工具并发送，按任意键继续");
		memset(rbuf, 0, sizeof(rbuf));
		if((ret=NDK_PortRead(PORT_NUM_COM1,BUFSIZE_SERIAL, rbuf, 10*1000, &len)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, len, BUFSIZE_SERIAL);
			continue;
		}
		if(len != BUFSIZE_SERIAL)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i, len, BUFSIZE_SERIAL);
			continue;
		}
		if(MemCmp(buf1, rbuf,  BUFSIZE_SERIAL))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次数据校验失败(recvlen=%d)", __LINE__, i, len);
			continue;
		}
		cls_show_msg("将accessport接收到的数据复制到accessport发送框并发送，按任意键继续");
		memset(rbuf, 0, sizeof(rbuf));
		if((ret=NDK_PortRead(PORT_NUM_USB,BUFSIZE_SERIAL, rbuf, 10*1000, &len)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次接收错(ret=%d)(实际%d, 预期%d)", __LINE__, i, ret, len, BUFSIZE_SERIAL);
			continue;
		}
		if(len != BUFSIZE_SERIAL)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次数据收发长度不一致(实际%d, 预期%d)", __LINE__, i, len, BUFSIZE_SERIAL);
			continue;
		}
		if(MemCmp(buf2, rbuf,  BUFSIZE_SERIAL))
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次数据校验失败(recvlen=%d)", __LINE__, i, len);
			continue;
		}
		succ++;
	}
	if((ret = NDK_PortClose(PORT_NUM_USB)) != NDK_OK)
	{
		cls_show_msg1(g_keeptime,"line %d:关闭USB串口失败(%d)", __LINE__, ret);
		NDK_PortClose(PORT_NUM_USB);
		return;
	}

	cls_show_msg1(g_keeptime,"测试完毕,总通讯次数%d次,成功%d次", cnt, succ);
	return;
}

void systest97(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	/*private & local definition*/
	int keyin = 0;

	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);	
	while(1)
    {
		keyin = cls_show_msg("1.开关压力 2.流程压力\n"
						     "3.POS->PC   4.HID/USB\n"  
						     "5.COM1/USB  6.COM1/HID\n");
		switch(keyin) 
		{
			case '1':
				hid_openclosepress();
				break;
			case '2':
				hid_open_comn_close();
				break;
			case '3':
				hid_send_data();
				break;
			case '4':
				hid_usb_test();
				break;
			case '5':
				com_usb_test();
				break;
			case '6':
				com_hid_test();
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



