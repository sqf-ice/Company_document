/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 射频卡性能,压力测试
* file name		: 13.c
* Author 			: 
* version			: V1.0
* DATE			: 20130118
* directory 		: 
* description		: 射频卡性能,压力测试
* related document	: NL8200程序员参考手册.doc
*				  
************************************************************************
* log			: 
* Revision 1.0  20130118 10.26 chenfm
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"射频卡性能,压力"
#define TESTBGPIC "test_bg.png"	//根据开发要求将测试文件放到main文件所在路径，这样可以一起打包下载 20160513

#define	DEFAULT_CNT_STR	"15000"	//"3000"//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE	atoi(DEFAULT_CNT_STR)	//(1000)

#define	DEFAULT_CNT_STR2	"5000"	//"3000"//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE2	atoi(DEFAULT_CNT_STR2)	//(1000)

#define	DEFAULT_CNT_STR3	"4000"	//"1000"//压力测试最小次数!一般若不是任务紧急不应小于该数值,否则可能测试不出问题
#define	DEFAULT_CNT_VLE3	atoi(DEFAULT_CNT_STR3)	//(1000)

#define	TIMEOUT	30
#define	DEFAULT_WAVE_NUM		"100"

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
*			  		  chenfm 	20130118		created
*****************************************************************/
//设计说明:不关注功能、压力(比如在一定强度下是否成功)等,仅关注性能
//也不关注ability返回,性能值在函数运行过程中给出
static void RFID_ability(SMART_t type)
{
	/*private & local definition*/
#define	nTestTime	(10)	//(5)

	//int nTestTime = 5;
	int count = 0, pUIDlen = 0, rcvlen = 0;
	float fTimeElapsed = 0.0;
#if K21_ENABLE
	char str[32] = {0};
#else
	time_t oldtime = 0, diff = 0;
#endif

/*
	uchar Select_1pay[] = //此处命令应与RFID_iccrw中使用的一致
#if 1	//海信的卡不支持Select_1pay命令,支持取随机数命令
	{'\x00', '\x84', '\x00', '\x00', 8};//取随机数命令
#else
	{0x00, 0xA4, 0x04, 0x00, 0x0E,
	  '1',  'P',  'A',  'Y',  '.',  'S',  'Y',  'S',  '.',  'D',  'D',  'F',  '0',  '1',
	  0x00
	};//查询CPU卡的文件目录结构,mifare-1卡不支持
#endif
*/

	uchar UID[LEN_UID] = {0};
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};

	/*process body*/
	cls_show_msg1(g_keeptime, "请在感应区放置%s卡,任意键继续...", card[type]);
	//寻卡
#if 0
	cls_printf("正在测试寻卡速度...\n");
	//count = 0;
	lib_StartStopwatch();
	while(RFID_detect(type, UID,&pUIDlen)==SUCC)
	{
		count++;
		if((fTimeElapsed=lib_ReadStopwatch())>nTestTime)
			break;
		if(RFID_deactive(type, 10)!=SUCC)
			break;
	}
	if(count>0)//因为不能苛求在此测试场景下一定100%成功,故只计算连续成功的平均时间即可//if(fTimeElapsed>nTestTime)
		cls_show_msg("寻卡速度:%.0f秒/次", fTimeElapsed/count);
	else
	{
		cls_show_msg("line %d:寻卡中发生失败(cnt=%d, err=%x)\n", __LINE__, count, GetErrorCode());
		return;
	}
#else	//仅计算一次寻卡的时间
	cls_printf("正在测试寻卡速度...\n");
	lib_StartStopwatch();
	if(RFID_detect(type, UID,&pUIDlen)!=SUCC)//不关注成功否,由压力测试去关注
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:寻卡失败", __LINE__);
		goto ERR;
	}
#if !K21_ENABLE
	cls_show_msg_record(FILENAME,FUNCNAME,"寻%s卡速度:%.0f秒/次", card[type],lib_ReadStopwatch());//时间很小,基本测不出来
#else
	memset(str,0,sizeof(str));
	ftos(lib_ReadStopwatch(),str);
	cls_show_msg_record(FILENAME,FUNCNAME,"寻%s卡速度:%s秒/次",card[type], str);//时间很小,基本测不出来
#endif
#endif
	
	//激活
	if (RFID_active(type, UID,&pUIDlen)!=SUCC)//不关注成功否,由压力测试去关注
	{	
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:激活失败", __LINE__);
		goto ERR;
	}
	
	//测试iccrw性能
	cls_printf("正在测试读写速度...\n");
	count = 0;
#if !K21_ENABLE//
	oldtime = time(NULL);//从第一次收到忙状态开始计时
	while(1)
	{
		lib_StartStopwatch();
		if(RFID_APDUrw(type, UID, &rcvlen)==SUCC)
		{
			count++;//累加成功次数
			fTimeElapsed+=lib_ReadStopwatch();//累加成功时间   //K21平台跑表精度是20ms以上才能读到时间，所以此处读到的时间都是0，不能用这种统计方式20150325 linwl guoc
		}
		else
		{
			//如果中间失败则下电再上电
			NDK_RfidPiccDeactivate(0);
			RFID_detect(type, UID,&pUIDlen);
			RFID_active(type, UID,&pUIDlen);
		}
	
		if(fTimeElapsed>nTestTime)//累计成功超过10秒则退出
			break;
		if((diff=time(NULL)-oldtime)>5*nTestTime)//总读写时间超过50秒则退出
			break;
	}
#else
	lib_StartStopwatch();
	while(RFID_APDUrw(type, UID, &rcvlen)==SUCC)
	{
		count++;
		if((fTimeElapsed=lib_ReadStopwatch())>nTestTime)
			break;
	}
#endif

#if !K21_ENABLE
	if(fTimeElapsed>nTestTime)
		cls_show_msg_record(FILENAME,FUNCNAME,"每秒读%s卡:%.0f字节",card[type], count*((type==MIFARE_1||type==MIFARE_0)?LEN_BLKDATA:(type==FELICA?29:rcvlen))/fTimeElapsed);//20150728 新增FELICA卡片读性能，该卡不认证读的数据长度为29
#else
	memset(str,0,sizeof(str));
	ftos(count*((type==MIFARE_1||type==MIFARE_0)?LEN_BLKDATA:(type==FELICA?29:rcvlen))/fTimeElapsed,str);//Select_1pay[4]修改成rcvlen 20161116 linwl
	if(fTimeElapsed>nTestTime)
		cls_show_msg_record(FILENAME,FUNCNAME,"每秒读%s卡:%s字节",card[type], str);
#endif
	else 
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:累计成功读写时间不足10秒(fTimeElapsed=%f)\n", __LINE__, fTimeElapsed);
	
	//关闭场
	if (RFID_deactive(type, 0)!=SUCC)//不关注成功否,由压力测试去关注
	{	
		cls_show_msg_record(FILENAME,FUNCNAME,"line %d:关闭场失败", __LINE__);
		return;
	}
	return;
ERR:
	NDK_RfidPiccDeactivate(0);//退出前关闭场
	return;
}

//射频卡压力测试
//设计说明:关注在一定强度下是否仍运行正常
//对于接触/非接的压力测试,除了要判断iccrw类的接口返回成功外,还要对命令状态字(SW)或状态域进行进一步判断!!!
static void RFID_press(SMART_t type)
{
	/*private & local definition*/
	int ret = 0, succ = 0, cnt = DEFAULT_CNT_VLE2, bak = 0,pUIDlen = 0, rcvlen = 0;
	uint unLen = 0;
	char CntStr[8] = DEFAULT_CNT_STR2;
	uchar UID[LEN_UID] = {0};
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};

	/*process body*/
	cls_printf("默认压力测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 5, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)	
		bak = cnt = DEFAULT_CNT_VLE2;
	else	
		bak = cnt = atoi(CntStr);

	cls_show_msg1(g_keeptime, "请在感应区放置%s卡,任意键继续...", card[type]);
	while(cnt)
	{
		if (ESC==cls_show_msg1(1, "压力测试中...\n还剩%d次(已成功%d次),ESC中断测试", cnt, succ))
			break;
		cnt--;

		//寻卡
		if((ret=RFID_detect(type, UID,&pUIDlen))!=SUCC)
		{
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:第%d次:寻卡失败(%d)", __LINE__, bak-cnt, ret);
			continue;//break;			
		}	
		//激活
		if ((ret=RFID_active(type, UID,&pUIDlen))!=SUCC)
		{	
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:第%d次:激活失败(%d)", __LINE__, bak-cnt, ret);
			continue;//break;
		}
		//读写
		if ((ret=RFID_APDUrw(type, UID, &rcvlen))!=SUCC)
		{	
			RFID_deactive(type, 10);
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:第%d次:读写失败(%d)", __LINE__, bak-cnt, ret);
			continue;//break;
		}
		//开启自动休眠后由于是1s检测一次是否处于忙状态,射频的相关操作很快,会导致在检测时会检测到射频不在工作的状态,
		//为了错开检测,加上延时,让忙的状态长一点,就避免被捕捉到空闲状态,就不会进入休眠而导致后续寻卡失败(凯哥解释 20180118)
		NDK_SysDelay(10);//延时1s
		//关闭场
		if ((ret=RFID_deactive(type, 0))!=SUCC)
		{	
			cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime, "line %d:第%d次:关闭场失败(%d)", __LINE__, bak-cnt, ret);
			continue;//break;
		}
		succ++;
	}

	RFID_deactive(type, 0);//退出前关闭场
	cls_show_msg_record(FILENAME,FUNCNAME,"压力测试完成,已执行次数为%d,成功%d次", bak-cnt, succ);
	return;
}

static void RFID_rw_press(SMART_t type)
{
	/*private & local definition*/
	int ret = 0, succ = 0, cnt = DEFAULT_CNT_VLE, bak = 0,pUIDlen = 0, rcvlen = 0;
	uint unLen = 0;
	char CntStr[8] = DEFAULT_CNT_STR;
	uchar UID[LEN_UID] = {0};
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};

	/*process body*/
	//测试使用次数做限制,不用时间(因为读写SAM有关时钟中断,计时有较大偏差)
	cls_printf("默认压力测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 5, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)	
		bak = cnt = DEFAULT_CNT_VLE;
	else
		bak = cnt = atoi(CntStr);

	cls_show_msg1(g_keeptime, "请在感应区放置%s卡,任意键继续...", card[type]);
	while(cnt)
	{
		//寻卡
		if((ret=RFID_detect(type, UID,&pUIDlen))!=SUCC)
		{
			cnt--;
			if(ESC==cls_show_msg1(g_keeptime, "line %d:第%d次:寻卡失败(%d)", __LINE__, bak-cnt, ret))
				break;
			else
				continue;//break;
		}
		//激活
		if ((ret=RFID_active(type, UID,&pUIDlen))!=SUCC)
		{	
			cnt--;
			if(ESC==cls_show_msg1(g_keeptime, "line %d:第%d次:激活失败(%d)", __LINE__, bak-cnt, ret))
				break;
			else
				continue;//break;
		}		

		while(cnt)
		{
			cls_printf("读写压力测试中...\n还剩%d次(已成功%d次),请耐心等待", cnt, succ);
			cnt--;

			//读写
			if ((ret=RFID_APDUrw(type, UID, &rcvlen))!=SUCC)
			{	
				RFID_deactive(type, 10);
				cls_show_msg1(g_keeptime, "line %d:第%d次:读写失败(%d)", __LINE__, bak-cnt, ret);
				break;					
			}
			succ++;
		}
	}

	//关闭场
	if ((ret=RFID_deactive(type, 0))!=SUCC)//测试完毕,场可以永久关闭
	{	
		cls_show_msg1(g_keeptime, "line %d:关闭场失败(0x%x)", __LINE__, ret);
		//此时下电失败,仍允许报告压力测试成功率return;
	}

	cls_show_msg_record(FILENAME,FUNCNAME,"读写压力测试完成,已执行次数为%d,成功%d次", bak-cnt, succ);
	return;
}

//射频卡挥卡测试
//设计说明:关注在应用场景下设备卡是从场外到场内，在从场内到场外的模拟测试
static void RFID_wave_press(SMART_t type)
{
	/*private & local definition*/
	int ret = 0, succ = 0, cnt = 200, bak = 0, pUIDlen = 0, i = 0, rcvlen = 0;
	uint unLen = 0;
	char CntStr[8] = DEFAULT_WAVE_NUM;
	uchar UID[LEN_UID] = {0};
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};

	/*process body*/
	cls_printf("默认压力测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 0, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)	
		bak = cnt = 100;
	else	
		bak = cnt = atoi(CntStr);
		
	cls_show_msg("挥卡测试时请尝试将卡片从较远距离（离开感应区了）的不同的方向移入感应区,按任意键开始测试");
	while(cnt)
	{
		if (ESC==cls_show_msg1(1, "请在3秒内挥%s卡,还剩%d次(已成功%d次),ESC中断测试", card[type], cnt, succ))
			break;
		cnt--;

		//寻卡
		if((ret=RFID_detect_poll(type, UID, &pUIDlen, 3))!=SUCC)
		{
			cls_show_msg1(g_keeptime, "line %d:第%d次:寻卡失败(%d)", __LINE__, bak-cnt, ret);
			continue;//break;			
		}	
		//激活
		if ((ret=RFID_active(type, UID,&pUIDlen))!=SUCC)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:激活失败(%d)", __LINE__, bak-cnt, ret);
			continue;//break;
		}
		for(i = 0;i<3;i++)
		{
			//读写
			if ((ret=RFID_APDUrw(type, UID, &rcvlen))!=SUCC)
			{	
				RFID_deactive(type, 10);
				cls_show_msg1(g_keeptime, "line %d:第%d次:读写失败(%d)(i=%d)", __LINE__, bak-cnt, ret, i);
				continue;//break;
			}
		}
		//关闭场
		if ((ret=RFID_deactive(type, 0))!=SUCC)
		{	
			cls_show_msg1(g_keeptime, "line %d:第%d次:关闭场失败(%d)", __LINE__, bak-cnt, ret);
			continue;//break;
		}
		cls_show_msg1(3,"请在3秒内将%s卡移出感应区",card[type]);
		succ++;
	}

	RFID_deactive(type, 0);//退出前关闭场
	cls_show_msg("挥卡压力测试完成,已执行次数为%d,成功%d次", bak-cnt, succ);
	return;
}

static void RFID_compatibility(SMART_t type)
{
	/*private & local definition*/
	int ret = -1, pUIDlen = 0,keyin = 0, rcvlen = 0;
	uchar UID[LEN_UID] = {0};

	/*process body*/
	switch(type)
	{
#if 0	//移入rf6中
	case MIFARE_1:// 4k M-1卡的测试
		cls_show_msg("请在感应区放置1张4k M1卡,任意键继续...");
		//寻卡
		if((ret=RFID_detect(type, UID,&pUIDlen))!=SUCC)
		{
			cls_show_msg("line %d:寻卡失败(ret=%d)", __LINE__, ret);
			goto ERR;
		}		
		//认证/激活,此处使用外部认证(内部认证类似,但会对卡的存储寿命有所影响)
		//由于不知道4k卡的B密钥是多少,使用A密钥(经测试A密钥为全0xff)
		if((ret= NDK_M1ExternalAuthen(4,UID,AUTHKEY_TYPE_A, AUTHKEY,0xfe))!=NDK_OK)//认证的块在第4K内的扇区
		{
			cls_show_msg("line %d:外部认证失败(ret=%d)", __LINE__, ret);
			goto ERR;	
		}
		//读写,考虑到测试卡寿命,仅做读操作
		if((ret= NDK_M1Read(0xfd,&pnDatalen,out))!=NDK_OK)//0xfe所在扇区的块应都能读
		{
			cls_show_msg("line %d:块读失败(ret=%d)", __LINE__, ret);
			goto ERR;
		}
		//关闭场
		if ((ret=RFID_deactive(type, 0))!=SUCC)
		{	
			cls_show_msg("line %d:关闭场失败(ret=%d)", __LINE__, ret);
			goto ERR;
		}
		break;
#endif
	case CPU_A:
	case CPU_B:
		do
		{
			keyin = cls_show_msg("_非标卡测试_\n"
								"1.屏蔽协议\n"
		  						"2.快速寻卡\n");
		  						
			switch (keyin)
			{
			//case 0://0为超时,默认启用GPRS
			case '1':
				NDK_SetIgnoreProtocol(1);
				break;
			case '2':
				NDK_PiccQuickRequest(1);
				break;
			case ESC:
				return;
				break;
			default:
				cls_show_msg1(1, "若要退出,请按ESC...");
				continue;
				break;
			}
		}while(keyin!=ESC && (keyin<'1'||keyin>'2'));
	  							
	  	cls_show_msg("请在感应区放置1张非标准%c卡,任意键继续...", (type==CPU_A)?'A':'B');

		//寻卡
		if((ret=RFID_detect(type, UID,&pUIDlen))!=SUCC)
		{
			cls_show_msg("line %d:寻卡失败(%d)", __LINE__, ret);
			goto ERR;
		}	
		//激活
		if ((ret=RFID_active(type, UID,&pUIDlen))!=SUCC)
		{	
			cls_show_msg("line %d:激活失败(%d)", __LINE__, ret);
			goto ERR;
		}
		//读写
		if ((ret=RFID_APDUrw(type, UID, &rcvlen))!=SUCC)
		{	
			cls_show_msg("line %d:读写失败(%d)", __LINE__, ret);
			goto ERR;
		}
		//关闭场
		if ((ret=RFID_deactive(type, 0))!=SUCC)
		{	
			cls_show_msg("line %d:关闭场失败(%d)", __LINE__, ret);
			goto ERR;
		}

		//后置(恢复协议判断等)
		//NDK_SetIgnoreProtocol(0);//用例测试毕,恢复默认	
		//NDK_PiccQuickRequest(0);//关闭简易快速寻卡功能
		break;
	case MIFARE_1:
	case FELICA:
	case ISO15693:
	case MIFARE_0:	
		cls_show_msg("暂无兼容性测试项目!请选择其它卡类型");
		return;
		break;		
	default:
		cls_show_msg("不支持的卡类型!");
		return;
		break;
	}

	cls_show_msg("兼容性测试已完成");
	//return;
ERR:
	NDK_SetIgnoreProtocol(0);// 用例测试毕,恢复默认	
	NDK_PiccQuickRequest(0);//关闭简易快速寻卡功能
	RFID_deactive(type, 0);//退出前关闭场
	return;
}

//M1卡跨扇区读写测试20170409 linying added
static void M1_multisection_rw(SMART_t type)
{
    /*private & local definition*/
	int ret = -1,s=1,k=0,succ=0, pnDatalen=16,i=0;
	int pUIDlen = 0, rcvlen = 0;
	uchar UID[LEN_UID] = {0},out[MAXLEN_DATA] = {0},out1[MAXLEN_DATA] = {0};
	int cnt = DEFAULT_CNT_VLE3, bak = 0;
	char CntStr[8] = DEFAULT_CNT_STR3;
	uint unLen = 0;	
	uchar RF_bak[64][LEN_BLKDATA+8];
	for(i=0;i<64;i++)
	{
	    memcpy(RF_bak[i],DATA16,LEN_BLKDATA);
	}
    /*process body*/
	cls_printf("默认压力测试次数:\n");
	if((ret=lib_kbgetinput(CntStr, 0, sizeof(CntStr)-1, &unLen,INPUTDISP_OTHER, 5, INPUT_CONTRL_LIMIT))==NDK_ERR||ret==NDK_ERR_TIMEOUT)	
		bak = cnt = DEFAULT_CNT_VLE3;
	else	
		bak = cnt = atoi(CntStr);
	cls_show_msg1(g_keeptime, ("请将M-1卡放置在感应区,按任意键开始测试"));
	while(cnt)
	{
		//寻卡
		if((ret=RFID_detect(type, UID,&pUIDlen))!=SUCC)
		{
			cnt--;
			if(ESC==cls_show_msg1(g_keeptime, "line %d:第%d次:寻卡失败(%d)", __LINE__, bak-cnt, ret))
				break;
			else
				continue; 
		}
	    	while(cnt)
		{
		   	cls_printf("读写压力测试中...\n还剩%d次(已成功%d次),请耐心等待", cnt, succ);
			cnt--;
		        s=rand()%15;//随机选取读取的扇区
		        //激活
		        if ((ret=NDK_M1ExternalAuthen(pUIDlen, UID, AUTHKEY_TYPE_A, AUTHKEY,4*s))!=SUCC)
		        {	 
				RFID_deactive(type, 0);
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:第%d次:激活失败(%d)", __LINE__, bak-cnt, ret);
				break;
		         }
		        //顺序读取该扇区前两个块号,失败不下电
		        for(k=4*s;k<4*s+2;k++)
		        {
				if((ret=NDK_M1Read(k, &rcvlen, out))!=NDK_OK) 
				 {
					  //RFID_deactive(type, 0); 
					  cls_show_msg1(g_keeptime, "%s,line %d:M-1块读失败(%d)",__FILE__ , __LINE__, ret);
					  break;
				  }
				 //预期rcvlen应等于LEN_BLKDATA
				 if(!memcmp(RF_bak[k],DATA16, rcvlen))//首次
					memcpy(RF_bak[k], out, rcvlen);
				 else if(memcmp(RF_bak[k], out, rcvlen))//其它次
				 { 
					 //RFID_deactive(type, 0); 
					 cls_show_msg1(g_keeptime, "%s,line %d:M-1卡读数据校验失败(%d)",__FILE__ , __LINE__,  rcvlen);
					 break;
				 }
			}
			//跨下一个扇区第一块进行写操作，进行写的数据校验，读写失败不下电
			//激活
			if ((ret=NDK_M1ExternalAuthen(pUIDlen, UID, AUTHKEY_TYPE_A, AUTHKEY,4*(s+1)))!=SUCC)
			{	
				RFID_deactive(type, 0);
				cls_show_msg1_record(FILENAME,FUNCNAME,g_keeptime,"line %d:第%d次:激活失败(%d)", __LINE__, bak-cnt, ret);
				break;
			}
			//读出该块原有数据
			if((ret=NDK_M1Read(4*(s+1), &rcvlen, out))!=NDK_OK) 
			{
				//RFID_deactive(type, 0); 
				cls_show_msg1(g_keeptime, "%s,line %d:M-1块读失败(%d)",__FILE__ , __LINE__, ret);
				break;
			}
			//写入原有数据
			if((ret=NDK_M1Write(4*(s+1), &pnDatalen, out))!=NDK_OK)
			{
				//RFID_deactive(type, 0); 
				cls_show_msg1(g_keeptime, "%s,line %d:M-1卡数据写失败(%d)",__FILE__ , __LINE__,  ret);
				break;
			}
			//读取所写的数据
			if((ret=NDK_M1Read(4*(s+1), &rcvlen, out1))!=NDK_OK) 
			{
				//RFID_deactive(type, 0); 
				cls_show_msg1(g_keeptime, "%s,line %d:M-1块读失败(%d)",__FILE__ , __LINE__, ret);
				break;
			}
			//对比数据
			if(memcmp(out1, out, rcvlen))//比较实际写入数据是否和预期写入数据一致
			{ 
				//RFID_deactive(type, 0); 
				cls_show_msg1(g_keeptime, "%s,line %d:M-1卡写数据校验失败(%d)",__FILE__ , __LINE__,  rcvlen);
				break;
			}
			succ++;
		}
	}
	RFID_deactive(type, 0);//退出前关闭场
	cls_show_msg_record(FILENAME,FUNCNAME,"压力测试完成,已执行次数为%d,成功%d次", bak-cnt, succ);
	return;	
}

void systest13(void)
{
	/*private & local definition*/
	int keyin = 0;
	SMART_t type = CPU_A;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0","15693"};
	
	/*process body*/	
#if !K21_ENABLE
	int ret = -1;
	//20140611 linwl added 补充背景图片验证有背景图片情况下液显函数调用不会引起背景图片显示异常
	if(cls_show_msg1(g_keeptime,"是否要设置背景图片,是按[确认],否则按[其它]")==ENTER)
	{
		cls_show_msg_record(FILENAME,FUNCNAME,"请先下载%s背景图片后按任意键继续",TESTBGPIC);
		if((ret=NDK_ScrSetbgPic(TESTBGPIC))!=NDK_OK)
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:显示背景图片失败(%d)", __LINE__, ret);
			return;
		}
	}
#endif
	cls_show_msg1(g_keeptime, "%s测试中...请先进行配置,按任意键开始", TESTITEM);
	if(auto_flag==1)
	{
		memcpy(g_RF_bak/*gen31_bak*/, DATA16, LEN_BLKDATA);//测试前,恢复一下全局以避免换M-1不能进行正确读卡的问题
		//自动化测试内容
		//自动配置参数，失败则退出
		type = select_smarttype();
		if(RFID_init(type)!=SUCC)
		{
			cls_show_msg_record(FILENAME,FUNCNAME,"line %d:初始化失败!请检查配置是否正确.", __LINE__);
			return;
		}
		else
			cls_show_msg1(2, "%s初始化成功!", card[type]);
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行压力测试
		RFID_press(type);

		//执行性能测试
		RFID_ability(type);

		//执行读写测试
		RFID_rw_press(type);
		
		//M1卡跨扇区读写测试
		if(type==MIFARE_1)	
			M1_multisection_rw(type);
		
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		NDK_SetIgnoreProtocol(0);//用例测试毕,恢复默认	
		NDK_PiccQuickRequest(0);//关闭简易快速寻卡功能,机器下电后也会恢复默认设置0.此处是为了防止使用海信B卡代替标准B卡时测试会通过 
		RFID_finish(type);//在ESC分支中增加这句为了和case '0'中的RFID_init对应
		return;
	}
	while(1)
	{
		keyin = cls_show_msg("1.综合压力 2.性能\n"
							"3.读写   4.挥卡\n"
							"5.兼容性 0.配置\n"
							"6.M1跨扇区 ");
		
		memcpy(g_RF_bak/*gen31_bak*/, DATA16, LEN_BLKDATA);//测试前,恢复一下全局以避免换M-1不能进行正确读卡的问题
		switch(keyin) 
		{
		case '1':// 1.一定强度的压力测试;
			cls_show_msg1(2, "RFID综合压力测试...");
			RFID_press(type);
			break;
		case '2':// 2.性能测试
			cls_show_msg1(2, "RFID性能测试...");
			RFID_ability(type);
			break;
		case '3':
			if(ENTER!=cls_show_msg("RFID读写专项压力测试(测试开始后,不接受ESC中断测试),ENTER键继续..."))
				break;
			RFID_rw_press(type);
			break;
		case '4':
			if(ENTER!=cls_show_msg("RFD挥卡压力测试,ENTER键继续..."))
				break;
			RFID_wave_press(type);
			break;
		case '5':
			cls_show_msg1(2, "RFID兼容性测试...");
			RFID_compatibility(type);
			break;
		case '6':
			cls_show_msg1(2, "M1跨扇区读写测试...");
			M1_multisection_rw(MIFARE_1);
			break;
		case '0':
			type = select_smarttype();
			//cls_show_msg("请在感应区放置相应的卡!任意键继续...");
			if(RFID_init(type)!=SUCC)
				cls_show_msg("line %d:初始化失败!请检查配置是否正确.", __LINE__);
			else
				cls_show_msg1(2, "%s初始化成功!", card[type]);
			break;
		case ESC:
			NDK_SetIgnoreProtocol(0);//用例测试毕,恢复默认	
			NDK_PiccQuickRequest(0);//关闭简易快速寻卡功能,机器下电后也会恢复默认设置0.此处是为了防止使用海信B卡代替标准B卡时测试会通过	
			RFID_finish(type);//在ESC分支中增加这句为了和case '0'中的RFID_init对应
			cls_show_msg("请换其它RFID重新测试!任意键继续...");
#if !K21_ENABLE
			NDK_ScrClrbgPic();
#endif
			return;
			break;
		default:
			continue;
			break;
		}
	}
	
	return;
}

