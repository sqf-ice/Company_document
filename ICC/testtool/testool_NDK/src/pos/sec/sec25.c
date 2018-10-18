/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name		: sec25.c
* Author 		: zhengry
* version		: 
* DATE			:  
* directory 		: 测试NDK_SecVppTpInit初始化PIN输入的触摸屏键盘
* description		: 安全配置
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>

/*---------------constants/macro definition---------------------*/
#define	TESTAPI	"NDK_SecVppTpInit"
#define PINTIMEOUT (200*1000)
#define CONSTVALUE  0x4321
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 提供安全等级的设置与获取功能,密钥唯一性,密钥专用性,密钥保护强度的验证
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author			date				remarks
*			  	  	  	zhengry 			20150827	      created
*****************************************************************/
void sec25(void)
{
	/*private & local definition*/
	int ret = 0, i = 0, j = 0, nStatus = 0;
	uchar num[80] = {0}, err_num1[80] = {0}, fun[36] = {0}, err_fun1[36] = {0}, err_fun2[36] = {0}, numserial[11] = {0}, szPinOut[8+1] = {0};
	ST_SEC_KCV_INFO stKcvInfoIn;
	ST_SEC_KEY_INFO stKeyInfo;
	int cnt= 10000, bak =0;
#if 1
	int x0 = 0, x1 = 135, x2 = 270, x3 = 405, x4 = 540, y0 = 462, y1 = 568, y2 = 674, y3 = 780, y4 = 886;
#elif defined N910AZ
	int x0 = 0, x1 = 180, x2 = 240, x3 = 360,x4 = 480,x5 = 540, x6 = 720, y0 = 0, y1 = 320, y2 = 640, y3 = 960, y4 = 1280;
#else//后续需要调整键盘坐标的在此处增加
		;
#endif
	
	//N900分辨率保持不变不从文件去获取而是直接写死的  N910的需要去读取文件获取分辨率如果是高分辨率就改成不一样的坐标
#if 1//defined N910AZ||defined N700
	int fp = -1;
	char touch_hr[] = {"1280x720"};//高分辨率是1280x720
	char touch_hr_850[] = {"1024x600"};//850的分辨率是 1024x600 
	char readbuf[20] = {0};
	if((fp=open("/sys/class/touchscreen/resolution", O_RDONLY))<0)
	{
		cls_show_msg1(2,"line %d: 打开文件失败(%d)", __LINE__, fp);
		return ;
	}
	memset(readbuf, 0, sizeof(readbuf));
	read(fp,readbuf, sizeof(readbuf));
	//读取分辨率获取到高分辨率需转换坐标如果是低分辨率不做改变
	if(!memcmp(touch_hr,readbuf,4))//使用前四个数比较才行
	{
#if defined N920
		x0 = 0; x1 = 180; x2 = 360; x3 = 540;x4 = 720;
		y0 =614; y1 = 755; y2 = 896; y3 = 1037; y4 = 1178;
#else
		x0 = 0; x1 = 180; x2 = 360; x3 = 540;x4 = 720;
		y0 = 665; y1 = 818; y2 = 972; y3 = 1126; y4 = 1280;
#endif
	}
	//增加N850的分辨率
	else if(!memcmp(touch_hr_850,readbuf,4))  //850
	{
		x0 = 0; x1 = 150; x2 = 300; x3 = 450;x4 = 600;
		y0 = 495; y1 = 609; y2 = 723; y3 = 837; y4 = 952;
	}
	else   //900
	{
		x0 = 0; x1 = 135; x2 = 270; x3 = 405;
		x4 = 540;y0 = 462;y1 = 568; y2 = 674; y3 = 780; y4 = 886;
	}
	close(fp);
#endif	

	int numInt[40] = { 
			x0, y0, x1, y1, x1, y0, x2, y1, x2, y0, x3, y1,
	
			x0, y1, x1, y2, x1, y1, x2, y2, x2, y1, x3, y2,
	
			x0, y2, x1, y3, x1, y2, x2, y3, x2, y2, x3, y3,
	
			x1, y3, x2, y4};
	int funInt[18] = { //选择传入的功能键为:取消，退格，确认
			0x1B, 0, x3, y0, x4, y1,
			
			0x0A, 0, x3, y1, x4, y2,
			
			0x0D, 0, x3, y2, x4, y4};

	int numInt1[40] = { 
			x1, y1, x0, y0, x1, y0, x2, y1, x2, y0, x3, y1,
			
			x0, y1, x1, y2, x1, y1, x2, y2, x2, y1, x3, y2,
			
			x0, y2, x1, y3, x1, y2, x2, y3, x2, y2, x3, y3,
			
			x1, y3, x2, y4};
	int funInt1[18] = { 
			0x1B, 0, x4, y1, x3, y0,
			
			0x0A, 0, x3, y1, x4, y2,
			
			0x0D, 0, x3, y2, x4, y4};
			
	int funInt2[18] = { 
			0x1d, 0, x4, y1, x3, y0,
	
			0x0A, 0, x3, y1, x4, y2,
	
			0x0D, 0, x3, y2, x4, y4};

	int funInt3[18] = { //选择传入的功能键为:退出，清空，确认
			0x9B, 0,  x3, y0, x4, y1,
	
			0x9C, 0, x3, y1, x4, y2,
	
			0x0D, 0, x3, y2, x4, y4};
			
	int cmpvalue = CONSTVALUE;

	if(auto_flag==1)
	{
		//send_result("line %d:%s无纯自动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI);
			
	for (i = 0, j = 0; i < SZ_ARRAY(numInt); i++, j++) 
	{
		num[j] = (uchar) (numInt[i] & 0xff);
		j++;
		num[j] = (uchar) ((numInt[i] >> 8) & 0xff);
	}
	for (i = 0, j = 0; i < SZ_ARRAY(funInt); i++, j++) 
	{
		fun[j] = (uchar) (funInt[i] & 0xff);
		j++;
		fun[j] = (uchar) ((funInt[i] >> 8) & 0xff);
	}
	for (i = 0, j = 0; i < SZ_ARRAY(numInt1); i++, j++) 
	{
		err_num1[j] = (uchar) (numInt1[i] & 0xff);
		j++;
		err_num1[j] = (uchar) ((numInt1[i] >> 8) & 0xff);
	}
	for (i = 0, j = 0; i < SZ_ARRAY(funInt1); i++, j++) 
	{
		err_fun1[j] = (uchar) (funInt1[i] & 0xff);
		j++;
		err_fun1[j] = (uchar) ((funInt1[i] >> 8) & 0xff);
	}
	for (i = 0, j = 0; i < SZ_ARRAY(funInt2); i++, j++) 
	{
		err_fun2[j] = (uchar) (funInt2[i] & 0xff);
		j++;
		err_fun2[j] = (uchar) ((funInt2[i] >> 8) & 0xff);
	}
	//安装TMK, ID=5
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=5;
	stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.nDstKeyLen=16;
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(stKeyInfo.sDstKeyValue, 0x11, stKeyInfo.nDstKeyLen);
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//安装TPK1(8bytes), ID=2,密文安装 //为了能测海外产品需改成16字节安装 20180809 modify 
	stKeyInfo.ucScrKeyIdx=5;
	stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK;
	stKeyInfo.nDstKeyLen=16/*8*/;
	memcpy(stKeyInfo.sDstKeyValue, "\x14\x5F\x5C\x6E\x3D\x91\x44\x57\x14\x5F\x5C\x6E\x3D\x91\x44\x57", stKeyInfo.nDstKeyLen);/*对应的key明文为16个字节的0x15*/
	stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
	if ((ret=NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case1:参数错误
	if((ret=NDK_SecVppTpInit(NULL, fun, numserial))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SecVppTpInit(num, NULL, numserial))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	/*
	if((ret=NDK_SecVppTpInit(num, fun, NULL))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}*/
	//case2.1:异常测试:数字键左上角的坐标点大于等于右下角的坐标点时，则应该返回失败
	if((ret=NDK_SecVppTpInit(err_num1, fun, numserial))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case2.2:异常测试:功能键左上角的坐标点大于等于右下角的坐标点时，则应该返回失败
	if((ret=NDK_SecVppTpInit(num, err_fun1, numserial))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case2.3:异常测试:功能键键值错误时，应返回失败
	if((ret=NDK_SecVppTpInit(num, err_fun2, numserial))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case8:新增:针对第三个参数传NULL可能出现密码键盘异常(堆栈被改会跑飞)的情况新增压力测试,一直调用此函数看是否会出现异常20171109add
	while(cnt)
	{
		bak++;
		cnt--;
		if((ret=NDK_SecVppTpInit(num, fun, NULL))!=NDK_OK)
		{
			send_result("line %d:%s第%d次测试失败(%d)", __LINE__, TESTAPI, bak, ret);
			return;
		}
	}

	//case3:正常测试:显示随机键盘，调用完后，触屏机应能正常使用
	memset(numserial, 0, sizeof(numserial));
	if((ret=NDK_SecVppTpInit(num, fun, numserial))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_printf("此时触屏机上随机按键顺序为\n%c,%c,%c,取消\n%c,%c,%c,退格\n%c,%c,%c,确认\n  %c\n", numserial[0], numserial[1], numserial[2], numserial[3], numserial[4], numserial[5], numserial[6], numserial[7], numserial[8], numserial[9]);
	NDK_ScrPrintf("请尽快输入4321后按退格键修改成1234并确认...");
	if((ret=NDK_SecGetPin(2, (uchar *)"4,4", NULL, szPinOut, SEC_PIN_ISO9564_2, PINTIMEOUT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szPinOut, "\xa8\x89\xc1\xdf\x10\xc8\xb0\x47", 8)) 
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szPinOut[0],szPinOut[1],szPinOut[2],szPinOut[3],szPinOut[4],szPinOut[5],szPinOut[6],szPinOut[7]);
		GOTOERR;
	}
	if(cls_show_msg("此时安卓端的机器应可以正常使用...通过[ENTER]，不通过[其它]")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:正常测试:显示固定排序的键盘，并且功能键选择取消 退格，测试取消键功能和密码输入，调用完后，触屏机应能正常使用
	if((ret=NDK_SecVppTpInit(num, fun, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_printf("此时触屏机上数字键盘的顺序为\n0,1,2,取消\n3,4,5,退格\n6,7,8, 确认\n  9\n");
	NDK_ScrPrintf("请尽快输入4321并按1次取消后修改成1234并确认...");
	if((ret=NDK_SecGetPin(2, (uchar *)"4,4", NULL, szPinOut, SEC_PIN_ISO9564_2, PINTIMEOUT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(szPinOut, "\xa8\x89\xc1\xdf\x10\xc8\xb0\x47", 8)) 
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szPinOut[0],szPinOut[1],szPinOut[2],szPinOut[3],szPinOut[4],szPinOut[5],szPinOut[6],szPinOut[7]);
		GOTOERR;
	} 
	if(cls_show_msg("此时安卓端的机器应可以正常使用...通过[ENTER]，不通过[其它]")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:修改功能键使用退出，清空，确认，测试退出功能键
	for (i = 0, j = 0; i < SZ_ARRAY(funInt3); i++, j++) 
	{
		fun[j] = (uchar) (funInt3[i] & 0xff);
		j++;
		fun[j] = (uchar) ((funInt3[i] >> 8) & 0xff);
	}
	memset(numserial, 0, sizeof(numserial));
	if((ret=NDK_SecVppTpInit(num, fun, numserial))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_printf("此时触屏机上随机按键顺序为\n%c,%c,%c,退出\n%c,%c,%c,清空\n%c,%c,%c确认\n  %c\n", numserial[0], numserial[1], numserial[2], numserial[3], numserial[4], numserial[5], numserial[6], numserial[7], numserial[8], numserial[9]);
	NDK_ScrPrintf("请尽快输入1234后按退出键");
	if((ret=NDK_SecGetPin(2, (uchar *)"4,4", NULL, szPinOut, SEC_PIN_ISO9564_2, PINTIMEOUT))!=NDK_ERR_SECVP_GET_ESC)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if(cls_show_msg("此时安卓端的机器应可以正常使用...通过[ENTER]，不通过[其它]")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6:修改功能键使用退出，清空，确认，测试清空功能键
	if((ret=NDK_SecVppTpInit(num, fun, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_printf("此时触屏机上数字键盘的顺序为\n0,1,2,退出\n3,4,5,清空\n6,7,8,确认\n  9\n");
	NDK_ScrPrintf("请尽快输入4321并按清空后修改成1234并确认...");
	if((ret=NDK_SecGetPin(2, (uchar *)"4,4", NULL, szPinOut, SEC_PIN_ISO9564_2, PINTIMEOUT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if(memcmp(szPinOut, "\xa8\x89\xc1\xdf\x10\xc8\xb0\x47", 8)) 
	{
		send_result("line %d:%s测试失败(%x,%x,%x,%x,%x,%x,%x,%x)", __LINE__, TESTAPI, szPinOut[0],szPinOut[1],szPinOut[2],szPinOut[3],szPinOut[4],szPinOut[5],szPinOut[6],szPinOut[7]);
		GOTOERR;
	} 
	if(cls_show_msg("按清空后已输入的密码被全部清空，且此时安卓端的机器应可以正常使用...通过[ENTER]，不通过[其它]")!=ENTER)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7:测试功能键的返回状态
	if((ret=NDK_SecVppTpInit(num, fun, NULL))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	cls_printf("此时触屏机上数字键盘的顺序为\n0,1,2,退出\n3,4,5,清空\n6,7,8,确认\n  9\n");
	NDK_ScrPrintf("可多次输入数字后按清空键后应能获取到清空键,按退出键可退出\n");
	if((ret=NDK_SecGetPin(2, (uchar *)"4,4", NULL, NULL, SEC_PIN_ISO9564_2, PINTIMEOUT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	do 
	{
		ret=NDK_SecGetPinResult(szPinOut, &nStatus);
		if (ret != 0) 
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		if (nStatus == SEC_VPP_KEY_CLEAR) 
		{
			NDK_ScrPrintf("获取到清空键,子用例测试通过\n");
		}
		if (nStatus == SEC_VPP_KEY_ESC) 
		{
			cls_printf("获取到退出键,子用例测试通过");
			break;
		}
		if (nStatus == SEC_VPP_KEY_PIN || nStatus == SEC_VPP_KEY_BACKSPACE ||  nStatus == SEC_VPP_KEY_NULL) 
		{   
			//由于本函数在存在pinblock时，界面由状态栏处理，这里对其他键不处理
			//继续循环等待
			//由于取PIN函数需要非阻塞，即使没值也要返回应用程序处理（判断是否中断退出），因此增加SEC_VPP_NULL 返回值
		}
		NDK_SysMsDelay(10);
	} while(1);

	//最后进行值比较,看内存是否改变20171109 add 
	if(cmpvalue != CONSTVALUE)
	{
		send_result("line %d:内存值比较失败(%d)", __LINE__, TESTAPI, cmpvalue);
		goto ERR;
	}
	//测试通过
	send_result("%s测试通过", TESTAPI);
	
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}


