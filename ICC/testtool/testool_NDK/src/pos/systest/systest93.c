/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name			: systest93.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 休眠/各模块交叉测试
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
#include <pthread.h>

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"休眠/各模块交叉"
#if !K21_ENABLE || OVERSEAS_ENABLE || defined SP610
#define  TESTFILE "FT6"
#else	//K21平台只支持绝对路径
#define  TESTFILE "/appfs/FT6"
#endif
#define WAKE_UP_TIME 128
//#define MAXWAITTIME  600
#define	ITEMPERPAGE	(4)

/*------------global variables definition-----------------------*/
int thread_run_flag = 0;
static pthread_mutex_t mutex;
static int pthread_retval = 0;
SMART_t type;

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
*			  			jiangym			20171121 		created
*****************************************************************/
//休眠与打印线程,关注打印效果,关注一卷纸后可以使用循环打印纸进行压力测试,测试完后继续测打印其他相关的
static void *pthread_prnt(void *arg)
{
	int ret = -1 ;
	EM_PRN_STATUS pemStatus;

	//打印初始化
	if((ret=lib_initprn(g_PrnSwitch))!= SUCC)//初始化
	{
		cls_show_msg("line %d: 打印机复位失败", __LINE__);
		goto ERR1;
	}
	NDK_PrnSetFont(PRN_HZ_FONT_24x24A,PRN_ZM_FONT_12x24A);
	NDK_PrnSetForm(0,0,6);
	NDK_PrnSetMode(PRN_MODE_NORMAL,1);
	//thread_run_flag 置1时 线程退出
	while(thread_run_flag == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_prnt is runing\n",__LINE__);
		NDK_PrnStr("商户名称(MERCHANT Name):\n  测试商户名称\n");
		NDK_PrnStr("商户号(MERCHANT NO.):\n  123456789012345\n");
		NDK_PrnStr("终端编号(TERMINAL NO.):12345678\n");
		NDK_SysMsDelay(4);
		NDK_PrnStr("收单行(ACQUIRER):\n  收单银行 12345678\n");
		NDK_PrnStr("发卡行(ISSUER):发卡银行 12345678\n");
		NDK_PrnStr("交易类型(TRANS TYPE):消费/SALE\n");
		NDK_PrnStart();
		if((ret = NDK_PrnGetStatus(&pemStatus))!=NDK_OK)
		{
			cls_show_msg1(3,"获取打印状态失败(%d)",ret);
			goto ERR1;
		}
	}
ERR1:
	pthread_mutex_lock(&mutex);
	thread_run_flag = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);
}
//经实测 休眠线程会一直抢占,刷卡线程很难执行到刷卡,除非进行了刷卡才会停在磁卡线程中
static void *pthread_mag(void *arg)
{
	int  ret = -1, nErrorCode = 0;
	char pszTk1[MAXTRACKLEN] = {0}, pszTk2[MAXTRACKLEN] = {0}, pszTk3[MAXTRACKLEN] = {0};
	//打开磁卡	
	if((ret=NDK_MagOpen())!=NDK_OK)
	{
		cls_show_msg1(3,"line %d:磁卡打开失败",__LINE__);
		goto ERR1;
	}
	//thread_run_flag 置1时 线程退出
	while(thread_run_flag == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_mag is runing\n",__LINE__);
		NDK_MagReset();
		cls_show_msg("请刷卡后按任意键继续");
		memset(pszTk1,0,sizeof(pszTk1));
		memset(pszTk2,0,sizeof(pszTk2));
		memset(pszTk3,0,sizeof(pszTk3));
		NDK_MagReadNormal(pszTk1, pszTk2, pszTk3, &nErrorCode);
	/*	//显示磁道信息
		if((ret=check_stripe_result_tk1(pszTk1))!=0x00)
		{
			cls_show_msg("line %d:读卡失败(0x%x)", __LINE__, ret);
			goto ERR1;
		}
		if((ret=check_stripe_result_tk2(pszTk2))!=0x00)
		{
			cls_show_msg("line %d:读卡失败(0x%x)", __LINE__, ret);
			goto ERR1;
		}
		if((ret=check_stripe_result_tk3(pszTk3))!=0x00)
		{
			cls_show_msg("line %d:读卡失败(0x%x)", __LINE__, ret);
			goto ERR1;
		}	*/
		NDK_SysMsDelay(4);
	}
ERR1:
	NDK_MagClose();
	pthread_mutex_lock(&mutex);
	thread_run_flag = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);
}
//休眠与ic卡线程,一般也是休眠为主,主要通过此测试验证后续是否会有问题
static void *pthread_ic(void *arg)
{
	int  ret = -1, buflen = 0;
	uchar buf[256] = {0};

	cls_show_msg("请插入IC1卡,任意键继续测试");
	//thread_run_flag 置1时 线程退出
	while(thread_run_flag == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_ic is runing\n",__LINE__);
		memset(buf,0,sizeof(buf));
		if ((ret=NDK_IccPowerUp(ICTYPE_IC,buf,&buflen))!=NDK_OK)
		{
			cls_show_msg1(3,"line %d: IC1上电失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		if((ret=NDK_IccPowerDown(ICTYPE_IC))!=NDK_OK)
		{
			cls_show_msg1(3,"line %d: IC1下电失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		NDK_SysMsDelay(4);
	}
ERR1:
	pthread_mutex_lock(&mutex);
	thread_run_flag = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);
}
//休眠与射频卡线程需要在设置中对射频卡设置后再进行
static void *pthread_rfid(void *arg)
{
	int ret = -1;
	uchar UID[LEN_UID] = {0}, pUIDlen = 0, rcvlen = 0;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "15693"};

	/*process body*/
	cls_show_msg1(g_keeptime, "请在感应区放置%s卡,任意键继续...", card[type]);
	//thread_run_flag 置1时 线程退出
	while(thread_run_flag == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_rfid is runing\n",__LINE__);
		//初始化设置寻卡策略
		if(RFID_init(type)!=SUCC)
		{
			cls_show_msg1(3,"line %d:初始化失败!请检查配置是否正确.", __LINE__);
			goto ERR1;
		}
		//寻卡
		if((ret=RFID_detect(type, UID,&pUIDlen))!=SUCC)
		{
			cls_show_msg1(3,"line %d:寻卡失败(%d)", __LINE__, ret);
			goto ERR1;
		}	
		//激活
		if ((ret=RFID_active(type, UID,&pUIDlen))!=SUCC)
		{	
			cls_show_msg1(3,"line %d:激活失败(%d)", __LINE__,  ret);
			goto ERR1;
		}
		//读写
		if ((ret=RFID_APDUrw(type, UID, &rcvlen))!=SUCC)
		{	
			RFID_deactive(type, 10);
			cls_show_msg1(3,"line %d:读写失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		//关闭场
		if ((ret=RFID_deactive(type, 0))!=SUCC)
		{	
			cls_show_msg1(3, "line %d:关闭场失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		RFID_finish(type);
		NDK_SysMsDelay(4);
	}
ERR1:
	pthread_mutex_lock(&mutex);
	thread_run_flag = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);
}
/*休眠与通讯线程,通讯过程中也有可能进入休眠,因为禁止进休眠是禁止自动休眠,此处无效,可以考虑不测此点
若测试过程中出现通讯失败问题也有可能,实际应用过程中是禁止进休眠*/
#if 0  //为了避免测试人员误报,此测试项暂时不测
static void *pthread_comm(void *arg)
{
	int slen = 0,  rlen = 0, i = 0, timeout = SO_TIMEO, ret = -1;
	uint h_tcp = 0;
	char sendbuf[PACKMAXLEN] = {0}, recbuf[PACKMAXLEN] = {0};

	//生成测试数据 8k数据
	for (i=0; i<sizeof(sendbuf); i++)
		sendbuf[i]=rand()%256;
	//thread_run_flag 置1时 线程退出
	while(thread_run_flag == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_comm is runing\n",__LINE__);
		if(LinkUp()!=SUCC)
		{
			cls_show_msg1(3,"line %d:LinkUp失败", __LINE__);
			goto ERR1;
		}
		if(NetUp()!=SUCC)
		{
			LinkDown();
			cls_show_msg1(3,"line %d:NetUp失败", __LINE__);
			goto ERR1;
		}
		if(TransUp(&h_tcp)!=SUCC)
		{
			NetDown();
			LinkDown();
			cls_show_msg1(3,"line %d:TransUp失败", __LINE__);
			goto ERR1;
		}
		//发送数据
		if ((slen=sock_send(h_tcp, sendbuf, PACKMAXLEN, timeout))!=PACKMAXLEN)
		{
			NetDown();
			LinkDown();
			cls_show_msg1(3,"line %d:发送数据失败(%d)", __LINE__, slen);
			goto ERR1;
		}
		//接收数据
		memset(recbuf, 0, sizeof(recbuf));
		if((rlen=sock_recv(h_tcp, recbuf, PACKMAXLEN, timeout))!=PACKMAXLEN)	
		{
			NetDown();
			LinkDown();
			cls_show_msg1(3,"line %d:接收数据失败(%d)", __LINE__, rlen);
			goto ERR1;
		}
		if (TransDown(h_tcp)!=SUCC)
		{
			NetDown();
			LinkDown();
			cls_show_msg1(3,"line %d:TransDown失败", __LINE__);
			goto ERR1;
		}
		if(NetDown()!=SUCC)
		{
			LinkDown();
			cls_show_msg1(3,"line %d:NetDown失败", __LINE__);
			goto ERR1;
		}
		if(LinkDown()!=SUCC)
		{
			cls_show_msg1(3,"line %d:LinkDown失败", __LINE__);
			goto ERR1;
		}
		NDK_SysMsDelay(4);
	}
ERR1:
	pthread_mutex_lock(&mutex);
	thread_run_flag = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);
}
#endif

static void *pthread_bt(void *arg)
{
#if BT_ENABLE
	int  ret = -1;

	//thread_run_flag 置1时 线程退出
	while(thread_run_flag == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_bt is runing\n",__LINE__);
		//打开蓝牙串口
		if((ret=NDK_PortOpen(PORT_NUM_BT, NULL))!=NDK_OK)
		{
			cls_show_msg1(3, "line %d:蓝牙打开失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		//ME30MH蓝牙需要开启广播
#if 0//defined ME30MH||defined ME30GX
		if((ret=NDK_BTSetDiscoverableStatus(1))!=NDK_OK)
		{
			cls_show_msg1(3, "line %d:蓝牙设置可见失败(%d)", __LINE__, ret);
			goto ERR1;
		}
#endif
		if((ret=lib_bt_pairing(1))!=NDK_OK)
		{
			cls_show_msg1(3, "line %d:蓝牙连接失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		if((ret=bt_comm_once(12,0))!=NDK_OK)
		{
			cls_show_msg1(3, "line %d:蓝牙通讯失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		if((ret=NDK_BTDisconnect())!=NDK_OK)
		{
			cls_show_msg1(3, "line %d:蓝牙断开失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		NDK_PortClose(PORT_NUM_BT);
		NDK_SysMsDelay(4);
	}
ERR1:
	NDK_PortClose(PORT_NUM_BT);
	pthread_mutex_lock(&mutex);
	thread_run_flag = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);
#else 
	cls_show_msg("不支持蓝牙,按任意键退出");
	return;
#endif
}

//扫描代码日后可能需要优化,如果有摄像头相关的函数时需要改代码

static void *pthread_scan(void *arg)
{
#if SCAN_ENABLE||SCANASYCN_ENABLE
	int ret = 0;

	if((ret=NDK_ScanInit())!=NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:设备初始化失败(%d)", __LINE__, ret);
		return;
	}
#if SCAN_ENABLE
	NDK_ScanSet(SCAN_SETTYPE_FLOODLIGHT, g_Floodlight);
	NDK_ScanSet(SCAN_SETTYPE_FOCUSLIGHT, g_Focuslight);
	NDK_ScanSet(SCAN_SETTYPE_SENSITIVITY, g_Sensitivity);
#endif
	//thread_run_flag 置1时 线程退出
	while(thread_run_flag == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_scan is runing\n",__LINE__);
		cls_printf("请扫码");
		if((ret=lib_doscan(g_ScanType, 15))!=NDK_OK)
		{
			cls_show_msg1(3,"line %d:扫描失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		NDK_SysMsDelay(4);
	}
ERR1:
	pthread_mutex_lock(&mutex);
	thread_run_flag = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
#if SCAN_SOFT_ENABLE
	NDK_ScanExit();
#endif
	return ((void *) &pthread_retval);
#else
	cls_show_msg("不支持扫描，按任意键退出");
	return;
#endif
}

static void *pthread_usb(void *arg)
{
	int  ret = -1;

	cls_show_msg("请插入U盘,任意键继续");
	//thread_run_flag 置1时 线程退出
	while(thread_run_flag == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_usb is runing\n",__LINE__);
		if ((ret=NDK_PortOpen(PORT_NUM_USB, "115200,8,N,1"))!=NDK_OK)
		{
			cls_show_msg1(3, "line %d:USB打开失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		NDK_PortClose(PORT_NUM_USB);
		
		NDK_SysMsDelay(4);
	}
ERR1:
	pthread_mutex_lock(&mutex);
	thread_run_flag = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);
}

static void *pthread_udisk(void *arg)
{
	int  ret = -1;

	cls_show_msg("请插入U盘,任意键继续");
	//thread_run_flag 置1时 线程退出
	while(thread_run_flag == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_udisk is runing\n",__LINE__);
		if((ret=NDK_DiskOpen(UDISK,10))!=NDK_OK)
		{
			cls_show_msg1(3, "line %d:P盘打开失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		//关闭设备
		if((ret=NDK_DiskClose(UDISK))!=NDK_OK)
		{
			cls_show_msg1(3, "line %d:U盘关闭失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		
		NDK_SysMsDelay(4);
	}
ERR1:
	pthread_mutex_lock(&mutex);
	thread_run_flag = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);
}

static void *pthread_fs(void *arg)
{
	int  ret = -1;
	int fp=0, writelen=0, readlen=0;
	char writebuf[200]={0}, readbuf[200]={0};

	//thread_run_flag 置1时 线程退出
	while(thread_run_flag == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_fs is runing\n",__LINE__);
		NDK_FsDel(TESTFILE);
		if((fp=NDK_FsOpen(TESTFILE, "w"))<0)
		{
			cls_show_msg1(3, "line %d:文件打开失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		memset(writebuf, 0x08, sizeof(writebuf));
		if((writelen=NDK_FsWrite(fp, writebuf, 200))!=200)
		{
			cls_show_msg1(3, "line %d:写文件失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		NDK_FsSeek(fp, 0l, SEEK_SET);
		memset(readbuf, 0, sizeof(readbuf));
		if((readlen=NDK_FsRead(fp, readbuf, 200))!=200)
		{
			cls_show_msg1(3, "line %d:读文件失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		if((ret=NDK_FsClose(fp))!=NDK_OK)
		{
			cls_show_msg1(3, "line %d:关闭文件失败(%d)", __LINE__, ret);
			goto ERR1;
		}
		NDK_SysMsDelay(4);
	}
ERR1:
	pthread_mutex_lock(&mutex);
	thread_run_flag = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);
}

static void *pthread_sec(void *arg)
{
	int  ret = -1;
	ST_SEC_KEY_INFO stKeyInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;
	uchar udesout[31]={0}, udesin[31]={0};

	//安装密钥
	//测试前置:擦除所有密钥
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		cls_show_msg1(3,"line %d:擦除密钥失败(%d)", __LINE__, ret);
		goto ERR1;
	}
	memset(udesin, 0x20, sizeof(udesin));
	//安装TMK, ID=5
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=16;
	memset(stKeyInfo.sDstKeyValue, 0x11, stKeyInfo.nDstKeyLen);
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		cls_show_msg1(3,"line %d:安装TMK密钥测试失败(%d)", __LINE__, ret);
		goto ERR1;
	}
	//安装TDK2(密钥16字节),ID=2,密文安装
	stKeyInfo.ucScrKeyIdx=5;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK;
	stKeyInfo.nDstKeyLen=16;
	memcpy(stKeyInfo.sDstKeyValue, "\x14\x5F\x5C\x6E\x3D\x91\x44\x57\x38\xBE\xDB\x24\xA6\xD3\x80\x18", stKeyInfo.nDstKeyLen); /*对应的key明文为8个字节的0x15+8个字节的0x17，修改前8后8密钥不一致*/
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		cls_show_msg1(3,"line %d:安装密钥失败(%d)", __LINE__, ret);
		goto ERR1;
	}
	//thread_run_flag 置1时 线程退出
	while(thread_run_flag == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_sec is runing\n",__LINE__);
		//进行加解密运算		
		if((ret=NDK_SecCalcDes(SEC_KEY_TYPE_TDK, 2, udesin, 16, udesout, SEC_DES_DECRYPT|SEC_DES_KEYLEN_16))!=NDK_OK)
		{
			cls_show_msg1(3,"line %d:DES解密失败(%d)", __LINE__, ret);
			goto ERR1;
		}	
		if(memcmp(udesout, "\x1E\x91\xC7\xC6\x2A\xBC\xCC\xA7\x1E\x91\xC7\xC6\x2A\xBC\xCC\xA7", 16))
		{
			cls_show_msg1(3,"line %d:结果校验失败", __LINE__);
			goto ERR1;
		}
		NDK_SysMsDelay(4);
	}
ERR1:
	NDK_SecKeyErase();
	pthread_mutex_lock(&mutex);
	thread_run_flag = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);
}

static void *pthread_sleep(void *arg)
{
	int  ret = -1 ;
	
	//thread_run_flag 置1时 线程退出
	while(thread_run_flag == 0)
	{	
		//为了确认确实有线程在跑
		fprintf(stderr,"line:[%d]thread_sleep is runing\n",__LINE__);
		if((ret = NDK_SysSetSuspendDuration(WAKE_UP_TIME))!=NDK_OK)
		{
			cls_show_msg1(3,"设置休眠唤醒时间失败(%d)",ret);
			goto ERR;
		}
	/*	if((ret = NDK_SysGoSuspend())!=NDK_OK && ret != NDK_ERR_PRN_BUSY && ret != NDK_ERR_USB_BUSY)  //返回打印忙允许
		{			
			cls_show_msg1(3,"进入休眠失败(%d)",ret);
			goto ERR;
		}*/
		NDK_SysGoSuspend();	
		NDK_SysMsDelay(3);//延时3ms	
	}
ERR:
	pthread_mutex_lock(&mutex);
	thread_run_flag = 1;
	pthread_retval = ret;
	pthread_mutex_unlock(&mutex);
	return ((void *) &pthread_retval);

}
static void sleep_other_test(int casekey)
{	
	/*private & local definition*/
	pthread_t tidp,tidp1;
    	int error=-1, error1=-1;
	//pthread_attr_t attr;
	time_t oldtime;
	int nkey = 0;
	PTHREAD pfunc[] = {pthread_prnt,pthread_mag,pthread_ic,pthread_rfid,/*pthread_comm,*/pthread_bt,pthread_scan,
				pthread_usb,pthread_udisk,pthread_fs,pthread_sec};
	char *func_info[] = {"休眠与打印", "休眠与磁卡", "休眠与IC卡", "休眠与射频卡", /*"休眠与无线/WIFI",*/ "休眠与蓝牙", "休眠与扫描", 
					"休眠与USB", "休眠与U盘", "休眠与文件系统", "休眠与安全"};

	/*process body*/
	cls_printf("测试%s多线程,若进入休眠可唤醒",func_info[casekey]);	
	thread_run_flag = 0;
	pthread_mutex_init(&mutex, NULL);
	if((error=NDK_PthreadCreate(&tidp,NULL,pfunc[casekey],NULL))!=0)
	{
		cls_show_msg1(g_keeptime,"line %d:创建打印线程失败(%d)", __LINE__, error);
		return;
	}
	if((error1=NDK_PthreadCreate(&tidp1,NULL,pthread_sleep,NULL))!=0)
	{
		cls_show_msg1(g_keeptime,"line %d:创建休眠线程失败(%d)", __LINE__, error1);
		return;
	}
	oldtime=time(NULL);
	while(1)
	{
		if(thread_run_flag==1)	
			break;
	/*	if((time(NULL)-oldtime)>MAXWAITTIME)
		{
			thread_run_flag=1;
			break;
		}*/
		cls_printf("%s双线程执行中,可按ESC退出",func_info[casekey]);
		NDK_KbGetCode(1, &nkey);
		if(nkey == ESC)
		{
			thread_run_flag=1;
			break;
		}
	}
	NDK_PthreadJoin(tidp,NULL);
	NDK_PthreadJoin(tidp1,NULL);
	pthread_mutex_destroy(&mutex);
	cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试通过",func_info[casekey]);
	return;
}

//函数体实现
void  select_sleep_func(void)
{
	/*private & local definition*/	
	int nKeyin = 0, nPageSel = 0;//从0开始计算(第一页码为0)
	int cnt = 0, menuid = 0, maxpage = 0, sn =0;
	char *menustr[] = {
		"休眠与打印",
		"休眠与磁卡",
		"休眠与IC卡",
		"休眠与射频卡",
		/*"休眠与无线/WIFI",*/
		"休眠与蓝牙",
		"休眠与扫描", 
		"休眠与USB",
		"休眠与U盘",
		"休眠与文件系统",
		"休眠与安全",
		};
	/*process body*/
#if SCR_DISABLE
	char str[1024]={0};
#endif
	char* tmpstr = NULL;

	/*process body*/
	maxpage = (SZ_ARRAY(menustr)-1)/ITEMPERPAGE;//从0开始计算(第一页码为0)
	cls_show_msg1(2, "选择测试项..\n" "↑键:往回翻页\n" "↓键:往后翻页\n");

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
			sprintf(str,"%d.%s\n", cnt+1, tmpstr);
			NDK_PortWrite(g_com, strlen(str), str);
#endif
			NDK_ScrPrintf("%d.%s\n", cnt+1, tmpstr);
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
				sleep_other_test(sn);
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
			return;
			break;
		default:
			continue;
			break;
		}
	}
	return;
}

//配置函数
void set_config(void)
{
	int keyin = 0;

	/*process body*/
	while(1)
       {
		keyin = cls_show_msg(/*"1.WLM配置\n"
						     "2.WLAN配置\n"*/
						     "3.BT设置\n"
						     "4.打印配置\n"
						     "5.射频卡配置\n");
		switch(keyin) 
		{
	/*	case '1':
			conf_conn_WLM(TRUE);
			break;
		case '2':
			conf_conn_WLAN();
			break;*/
		case '3':
			conf_bt();
			break;
		case '4':
			conf_prn();
			break;
		case '5':
			type = select_smarttype();
			break;			
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
	}
	return ;
}

void systest93(void)
{
	/*private & local definition*/	
	int keyin = 0;

	/*process body*/
	cls_show_msg1(2, "%s测试中...", TESTITEM);
	
	while(1)
	{
		keyin = cls_show_msg("1.休眠与其他模块并发\n"
						      "0.设置\n");
		switch(keyin) 
		{
		case '1':
			select_sleep_func();
			break;
		case '0':
			set_config();
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


