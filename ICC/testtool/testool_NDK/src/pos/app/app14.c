/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: App14.c
* Author 			:
* version			: 
* DATE			:
* directory 		: 
* description		: 测试防切机功能
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#define  TESTAPI	  "防切机功能"
#define  FILETEST1   "ErrApppk.NLP"
#define  FILETEST2   "Apppk.NLP"
#define  FILETEST3   "Apppk2.NLP"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的10号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*						sull					20180605		created
*****************************************************************/
/**
* 验证防切机功能里以下两点:
* 1.当apppk存在且内容不对时NDK_AppLoad和NDK_AppUpdate直接报错，不再使用系统公钥验签
* 2.apppk移出白名单，当apppk已经下载时NDK_AppLoad和NDK_AppUpdate不可以安装其它客户密钥
**注意:测试时候需要把此用例打包(打包需要使用master.key)后给开发签名,签名后的应用进行下载测试
*/
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

void app14(void)
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
	if((cls_show_msg("是否为第一次执行该用例且机器还未下apppk?是[ENTER],否[其它]")) == ENTER)
	{
		//case1:在还未装载apppk的情况下，应能用NDK_AppLoad和NDK_Update正常装载
		if((ret = NDK_AppLoad((char *)FILETEST2, 0)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}

		//case2:安装apppk之后，NDK_AppLoad不能再重复安装相同密钥
		if((ret = NDK_AppLoad((char *)FILETEST2, 0)) != NDK_ERR_APP_SIGN_DECRYPT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}

		//case3:安装apppk之后，NDK_AppUpdate不能再重复安装相同密钥
		if((ret=app_transdata(FILETEST2, 0, 0))!=NDK_OK)
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

		//case4:安装apppk之后，NDK_AppLoad不能再安装其它的密钥
		if((ret = NDK_AppLoad((char *)FILETEST3, 0)) != NDK_ERR_APP_SIGN_DECRYPT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}

		//case5:安装apppk之后，NDK_AppUpdate不能再安装其它的密钥
		if((ret=app_transdata(FILETEST3, 0, 0))!=NDK_OK)
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
		
		//case6:安装apppk之后，apppk的值错误时，NDK_AppLoad应该直接报错
		if((ret = NDK_AppLoad((char *)FILETEST1, 0)) != NDK_ERR_APP_SIGN_DECRYPT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}

		//case7:安装apppk之后，apppk的值错误时，NDK_AppUpdate应该直接报错
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

		send_result("%s测试通过(已通过程序下载apppk,请退出应用,重新打包应用)", TESTAPI);
	}
	else
	{
		cls_show_msg("请确保已删除apppk后再执行该用例,按任意键退出测试");
		return;
	}			
}

