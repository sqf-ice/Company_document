/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: App16.c
* Author 			:
* version			: 
* DATE			:
* directory 		: 
* description		: 测试可切设备防窃机功能
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#define  TESTAPI	  "可切设备防窃机功能"
#define  FILETEST1   "ENABLEA.NLP"  //可切公钥A
#define  FILETEST2   "MAPPKEY.NLP"  //mapp对应的公钥，也为可切公钥
//#define  FILETEST2   "ENABLEB.NLP"  //可切公钥B
#define  FILETEST3   "DISABLE.NLP"  //不可切公钥

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 应用管理模块的16号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*						sull				20180802			created
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

void app16(void)
{
	/*private & local definition*/
	int ret = 0;

	/*process body*/
	cls_printf("测试%s...", TESTAPI);

	if(auto_flag==1)//此项功能不支持自动测试
	{
		send_result("%s不支持自动测试,请使用手动测试验证", TESTAPI);
		return;
	}

	if((cls_show_msg("是否进行子用例1测试?是按任意键进行测试,否则按ESC跳过测试,请确保机器还未下任何公钥再进行该测试")) != ESC)
	{
		//case1:NDK_AppLoad可切的定制公钥后可以重复下载
		if((cls_show_msg("是否为第一次执行该子用例且机器还未下apppk?是[ENTER],否[其它]")) == ENTER)
		{
			if((ret = NDK_AppLoad((char *)FILETEST1, 0)) != NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			//清公钥前重复NDK_AppLoad相同公钥应该失败
			if((ret = NDK_AppLoad((char *)FILETEST1, 0)) != NDK_ERR_APP_SIGN_DECRYPT)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			cls_show_msg("请用母pos或授权卡清公钥后进入子用例1继续测试");
		}
		else
		{
			if((ret = NDK_AppLoad((char *)FILETEST1, 0)) != NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
		cls_show_msg("子用例1测试通过,已用代码下可切公钥,请用母pos或授权卡清公钥后进行子用例2测试,按任意键重启");
			NDK_SysReboot();
		}
	}
	
	if((cls_show_msg("是否进行子用例2测试?是按任意键进行测试,否则按ESC跳过测试,请确保机器还未下任何公钥再进行该测试")) != ESC)
	{
		//case2:NDK_AppUpdate可切的定制公钥后可以重复下载
		if((cls_show_msg("是否为第一次执行该子用例且机器还未下apppk?是[ENTER],否[其它]")) == ENTER)
		{
			if((ret=app_transdata(FILETEST1, 0, 0))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((ret=NDK_AppDataVerify())!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((ret=NDK_AppUpdate(0))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}	
			//清公钥前重复NDK_AppUpdate相同公钥应该失败
			if((ret=app_transdata(FILETEST1, 0, 0))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((ret=NDK_AppDataVerify())!=NDK_ERR_APP_SIGN_DECRYPT)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_SIGN_DECRYPT)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			cls_show_msg("请用母pos或授权卡清公钥后进入子用例2继续测试");
		}
		else
		{
			if((ret=app_transdata(FILETEST1, 0, 0))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((ret=NDK_AppDataVerify())!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((ret=NDK_AppUpdate(0))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}	
			cls_show_msg("子用例2测试通过,已用代码下可切公钥，请用母pos或授权卡清公钥后进行子用例3测试，按任意键重启");
			NDK_SysReboot();
		}
	}

	if((cls_show_msg("是否进行子用例3测试?是按任意键进行测试,否则按ESC跳过测试,请确保机器还未下任何公钥再进行该测试")) != ESC)
	{
		//case3:下载可切的的定制公钥后可以下载不同客户公钥
		if((cls_show_msg("是否为第一次执行该子用例且机器还未下apppk?是[ENTER],否[其它]\n")) == ENTER)
		{
			if((ret = NDK_AppLoad((char *)FILETEST1, 0)) != NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}		
			cls_show_msg("请用母pos或授权卡清公钥后进入子用例3继续测试");
		}
		else
		{
			if((ret = NDK_AppLoad((char *)FILETEST2, 0)) != NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			cls_show_msg("子用例3测试通过,已用代码下可切公钥,请用母pos或授权卡清公钥后进行子用例4测试,按任意键重启");
			NDK_SysReboot();
		}
	}

	if((cls_show_msg("是否进行子用例4测试?是按任意键进行测试,否则按ESC跳过测试,请确保机器还未下任何公钥再进行该测试")) != ESC)
	{
		//case4:下载可切的的定制公钥后可以下载不同客户公钥
		if((cls_show_msg("是否为第一次执行该子用例且机器还未下apppk?是[ENTER],否[其它]\n")) == ENTER)
		{
			if((ret=app_transdata(FILETEST1, 0, 0))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((ret=NDK_AppDataVerify())!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((ret=NDK_AppUpdate(0))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}		
			cls_show_msg("请用母pos或授权卡清公钥后进入子用例4继续测试");
		}
		else
		{
			if((ret=app_transdata(FILETEST2, 0, 0))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((ret=NDK_AppDataVerify())!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			if((ret=NDK_AppUpdate(0))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				RETURN;
			}
			cls_show_msg("子用例4测试通过,已用代码下mapp对应的公钥,请再运行一遍app模块,运行app模块应该能够运行成功,按任意键退出测试");
		}
	}
	
	send_result("%s测试通过", TESTAPI);			
}



