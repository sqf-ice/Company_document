/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: App18.c
* Author 			:
* version			: 
* DATE			:
* directory 		: 
* description		: 测试防呆机制NDK_AppUpdate功能
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  TESTAPI		"防呆机制NDK_AppUpdate"
//针对SP610新增的防呆机制，新增对应测试用例
#if 0
#define  FILETEST13 "factor.NLP" //分区为factory，flash信息正确，打包地址0x030D6000，应用8K，预期应该装载成功
#define  FILETEST14 "selftes.NLP"  //分区为selftest，flash信息正确，打包地址0x030F0000，应用8k，预期应该装载成功
#define  FILETEST15 "klaa.NLP"  //分区为kla，flash信息正确，打包地址0x032FB000，应用8k，预期应该装载成功
#define  FILETEST16 "mapp.NLP" //分区为mapp，flash信息正确，打包地址0x0330A000，应用8k，预期应该装载成功
#define  FILETEST17 "mErr1.NLP" //分区为mapp，flash信息正确，打包地址0x033FF000，应用大于4k，预期应该装载失败
#define  FILETEST18 "sErr1.NLP" //分区为selftest，flash信息正确，打包地址0x030F9000，应用大于4k，预期应该装载失败
#define  FILETEST19 "mErr2.NLP" //分区为mapp，flash信息错误，打包地址0x0330A000，应用8k，预期应该装载失败
#define  FILETEST20 "fErr3.NLP" //分区为factory，flash信息正确，打包地址错误0x030EF000，应用小于4k，预期应该装载失败
#define  FILETEST21 "sErr3.NLP" //分区为selftest，flash信息正确，打包地址错误0x030FB000，应用小于4k，预期应该装载失败
#define  FILETEST22 "kErr3.NLP" //分区为kla，flash信息正确，打包地址错误0x03305000，应用小于4k，预期应该装载失败
#define  FILETEST23 "mErr3.NLP" //分区为mapp，flash信息正确，打包地址错误0x03308000，应用小于4k，预期应该装载失败
#define  FILETEST24 "debug.NLP" //分区为debugapppk，flash信息正确，打包地址0x0314B000，应用小于4k，预期应该装载成功
#define  FILETEST25 "apppk.NLP" //分区为apppk，flash信息正确，打包地址0x0314E000，应用小于4k，预期应该装载成功
#define  FILETEST26 "font24.NLP" //分区为font24_h，flash信息正确，打包地址0x0314E000，应用小于4k，预期应该装载成功
#define  FILETEST27 "sec.NLP"  //分区为sec_config，flash信息正确，打包地址0x0340000，应用小于4k，预期应该装载成功
#define  FILETEST28 "font32.NLP" //分区为font32_h，flash信息正确，打包地址0x0340000，应用小于4k，预期应该装载成功
#define  FILETEST29 "font.NLP" //分区为font-h，flash信息正确，打包地址0x003B9000，应用小于4k，预期应该装载成功
#define  FILETEST30 "xuii.NLP" //分区为xui，flash信息正确，打包地址0x003E7000，应用小于4k，预期应该装载成功
#define  FILETEST31 "status.NLP" //分区为statusicon，flash信息正确，打包地址0x003F2000，应用小于4k，预期应该装载成功
#define  FILETEST32 "debug1.NLP" //分区为debugapppk，flash信息错误，打包地址0x0314E000，应用小于4k，预期应该装载成功
#define  FILETEST33 "xui1.NLP" //分区为xui，flash信息错误，打包地址0x003E7000，应用小于4k，预期应该装载成功
#define  FILETEST34 "dERR.NLP" //分区为debugapppk，flash信息正确，打包地址0x0314A000，应用大于4k，预期应该装载失败
#define  FILETEST35 "boot1.NLP"  //分区为boot(name设置为品号)，flash信息正确，打包地址0x0040000，应用小于4k，预期应该装载成功
#define  FILETEST36 "boot2.NLP"  //分区为boot(name设置为品号)，flash信息错误，打包地址0x0040000，应用小于4k，预期应该装载成功
#define  FILETEST37 "bErr.NLP" //分区为boot(name设置为品号)，flash信息正确，打包地址0x00400000，应用大于52k，预期应该装载失败
#define  FILETEST38 "other1.NLP"  //分区为mapp，flash信息正确，打包地址0x0336D000，应用8k，预期应该装载成功
#define  FILETEST39 "boot3.NLP" //name设为boot，flash信息正确，打包地址0x03372000，应用8k，预期应该装载成功
#define  FILETEST40 "other2.NLP" //分区为mapp，flash信息错误，打包地址0x03309000，应用8k，预期应该装载失败
#define  FILETEST41 "other3.NLP" //分区为mapp，flash信息正确，打包地址0x033FF000，应用大于4k，预期应该装载失败
#define  FILETEST42 "other4.NLP" //分区为mapp，flash信息正确，打包地址错误0x0340A000，应用8k，预期应该装载失败
#define  FILETEST43 "boot4.NLP" //name设为boot，flash信息正确，打包地址错误0x0340A000，应用8k，预期应该装载失败
#endif

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的18号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*                                      sull                  20180806          created
*****************************************************************/
static int app_transdata(const char *pszName, int offseterrflag, int buferrflag)
{
	int  ret = 0, offset=0, fp=0, tmp = 0;
	uint len = 0;
	uchar buf[1024]={0};
	if((fp=NDK_FsOpen(pszName, "r"))<0)
	{
		cls_show_msg1(5, "line %d:%s文件打开失败(%d)", __LINE__, pszName, fp);
		return NDK_ERR;
	}
	while(1)
	{
		if((len=NDK_FsRead(fp, (char *)buf, 1024))<0)
		{
			cls_show_msg1(5, "line %d:%s文件读取数据失败(%d)", __LINE__, pszName, len);
			NDK_FsClose(fp);
			return NDK_ERR;
		}
		if(buferrflag==1)
		{
			tmp = rand()%(len-272)+272;
			if(buf[tmp]==0)
				buf[tmp]=0xff;
			else
				buf[tmp]=0;//任意修改第一次读到数据中的任意一个字节为0,构造数据内容错误
			buferrflag = 0;
		}
		if((ret=NDK_AppDataSave(offset, buf, len))!=NDK_OK)
		{
			cls_show_msg1(5, "line %d:%s文件App数据传输失败(%d)", __LINE__, pszName, ret);
			NDK_FsClose(fp);
			return NDK_ERR;
		}
		if(offseterrflag!=0)
		{
			offset+=offseterrflag;//将首地址修改成offseterrflag,使相对地址不正确构造数据保存错误
			offseterrflag=0;
		}
		offset+=len;
		
		if(len<1024)
			break;
	}
	NDK_FsClose(fp);
	return NDK_OK;
}

void app18(void)
{
	/*private & local definition*/
	int  ret = 0;
	uint i = 0;
	char *package[] = {"factor.NLP", "selftes.NLP", "klaa.NLP", "mapp.NLP", "mErr1.NLP", "sErr1.NLP", 
						"mErr2.NLP", "fErr3.NLP", "sErr3.NLP", "kErr3.NLP", "mErr3.NLP", "apppk.NLP", 
						"debug.NLP", "font24.NLP", "sec.NLP", "font32.NLP", "font.NLP", "xuii.NLP", 
						"status.NLP", "debug1.NLP", "xui1.NLP", "dErr.NLP", "boot1.NLP", "boot2.NLP",
						"bErr.NLP", "other1.NLP", "boot3.NLP","other2.NLP", "other3.NLP", "other4.NLP", "boot4.NLP"};
	char *pName[] = {"factor", "selftes", "klaa", "mapp", "mErr1", "sErr1", "mErr2", "fErr3", "sErr3", "kErr3",
					"mErr3", "debug", "apppk", "font24", "sec", "font32", "font", "xuii", "status", "debug1", 
					"xui1", "dErr", "boot1", "boot2", "bErr", "other1", "boot","other2", "other3", "other4", "boot4"};
	
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	if(cls_show_msg1(30, "测试%s...请确保所有的测试文件存在,ESC退出", TESTAPI)==ESC)
		return;//屏幕提示当前测试所在模块与用例
	
	//case1:查找得到对应的分区信息，NLP包为可执行程序，NLP包地址在对应的分区信息描述的地址范围内，应用包大小+打包地址<=分区地址范围，应成功下载
	for(i=0;i<=2;i++)
	{
		if((ret=app_transdata(package[i], 0, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret,package[i]);
			goto ERR;
		}
		if((ret=NDK_AppDataVerify())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret,package[i]);
			goto ERR;
		}
		if((ret=NDK_AppUpdate(0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret,package[i]);
			goto ERR;
		}
	}
	if((cls_show_msg1(30, "按确认键进入子用例1测试(3秒后会自动重启，重启后运行应用屏幕提示mapp，请重新下测试用例后继续测试)，其它键跳过测试")) == ENTER)
	{
		if((ret=app_transdata(package[3], 0, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret,package[3]);
			goto ERR;
		}
		if((ret=NDK_AppDataVerify())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret,package[3]);
			goto ERR;
		}
		if((ret=NDK_AppUpdate(1))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret,package[3]);
			goto ERR;
		}

	}

	//case2:查找得到对应的分区信息，NLP包为可执行程序，NLP包地址在对应的分区信息描述的地址范围内，flash信息错误，应下载失败
	if((ret=app_transdata(package[4], 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret,package[4]);
		goto ERR;
	}
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret,package[4]);
			goto ERR;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret,package[4]);
		goto ERR;
	}

	//case3:查找得到对应的分区信息，NLP包为可执行程序，NLP包地址在对应的分区信息描述的地址范围内，应用包大小+打包地址>分区地址范围，应下载失败
	for(i=5; i<=6; i++)
	{		
		if((ret=app_transdata(package[i], 0, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
		if((ret=NDK_AppDataVerify())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
		if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
	}

	//case4:查找得到对应的分区信息，NLP包为可执行程序，NLP包地址不在对应的分区 信息描述的地址范围内，应下载失败
	for(i=7; i<=10; i++)
	{
		if((ret=app_transdata(package[i], 0, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
		if((ret=NDK_AppDataVerify())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
		if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
	}

	//case5:查找得到对应的分区信息，NLP包为不可执行程序，NLP包对应的分区地址+应用包大小在分区地址范围内，无论flash信息是否正确，都应成功下载
	//由于接口不能重复下载公钥，且测试时已下载公钥，故此处应会报错
	if((ret=app_transdata(package[11], 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[11]);
		goto ERR;
	}
	if((ret=NDK_AppDataVerify())!=NDK_ERR_APP_SIGN_DECRYPT)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[11]);
		goto ERR;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_SIGN_DECRYPT)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[11]);
		goto ERR;
	}
	for(i=12; i<=20; i++)
	{
		if((ret=app_transdata(package[i], 0, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
		if((ret=NDK_AppDataVerify())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
		if((ret=NDK_AppUpdate(0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
	}

	//case6:查找得到对应的 分区信息，NLP包为不可执行程序，NLP包对应的分区地址+应用包大小不在分区地址范围内，应下载失败
	if((ret=app_transdata(package[21], 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[21]);
		goto ERR;
	}
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[21]);
		goto ERR;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[21]);
		goto ERR;
	}

	//case7:查找不到对应的分区信息，NLP包在白名单中，boot分区地址+boot升级包大小在boot分区地址范围内，不管flash信息正不正确，应成功下载
	for(i=22; i<=23; i++)
	{
		if((ret=app_transdata(package[i], 0, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
		if((ret=NDK_AppDataVerify())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
		if((ret=NDK_AppUpdate(0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
	}

	//case8:查找不到对应的分区信息，NLP包在白名单中，boot分区地址+boot升级包大小不在boot分区地址范围内，应下载失败
	if((ret=app_transdata(package[24], 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[24]);
		goto ERR;
	}
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[24]);
		goto ERR;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[24]);
		goto ERR;
	}

	//case9:查找不到对应的分区信息，NLP包不在白名单中，NLP包地址在mapp分区信息表描述的地址范围内，且打包地址+应用大小在mapp分区地址范围内，应成功下载
	if((ret=app_transdata(package[25], 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[25]);
		goto ERR;
	}
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[25]);
		goto ERR;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[25]);
		goto ERR;
	}
	cls_show_msg1(2,"屏幕将提示other1，提示other1后按任意键进行测试");
	if((ret = NDK_AppRun(pName[25])) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret,package[25]);
		goto ERR;
	}
	
	if((ret=app_transdata(package[26], 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[26]);
		goto ERR;
	}
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[26]);
		goto ERR;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[26]);
		goto ERR;
	}
	cls_show_msg1(2,"屏幕将提示boot，提示boot后按任意键进行测试");
	if((ret = NDK_AppRun(pName[26])) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret,package[26]);
		goto ERR;
	}

	//case10:查找不到对应的分区信息，NLP包不在白名单中，NLP包地址在mapp分区信息表描述的地址范围内，flash信息错误，打包地址+应用大小在mapp分区地址范围内，应下载失败
	if((ret=app_transdata(package[27], 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[27]);
		goto ERR;
	}
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[27]);
		goto ERR;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[27]);
		goto ERR;
	}

	//case11:查找不到对应的分区信息，NLP包不在白名单中，NLP包地址在mapp分区信息表描述的地址范围内，且打包地址+应用大小不在mapp分区地址范围内，应下载失败
	if((ret=app_transdata(package[28], 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[28]);
		goto ERR;
	}
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[28]);
		goto ERR;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[28]);
		goto ERR;
	}

	//case12:查找不到对应的分区信息，NLP包不在白名单中，NLP包地址不在mapp分区信息表描述的地址范围内，应下载失败
	for(i=29; i<=30; i++)
	{
		if((ret=app_transdata(package[i], 0, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
		if((ret=NDK_AppDataVerify())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
		if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL)
		{
			send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret, package[i]);
			goto ERR;
		}
	}
		
	//测试通过
	send_result("%s测试通过,自检已被替换，需重新下自检程序", TESTAPI);	
ERR:
	for(i=0;i<=30;i++)
		NDK_AppDel(pName);
}



