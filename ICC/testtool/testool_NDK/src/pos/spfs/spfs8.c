/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: spfs模块
* file name			: spfs8.c
* Author 			: wangk
* version			: 
* DATE				: 20180829
* directory 		: 
* description		: 测试NDK_SP_FsRename接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI		"NDK_SP_FsRename"
#define NAMECZ1    "/appfs/nameCZ1"
#define NAMECZ2    "/appfs/nameCZ2"
#define NAMENEW1   "/appfs/nameNEW1"
#define NAMENEW2   "/appfs/nameNEW2"
#define INPUTHZ    "/appfs/123中國大中" //K21平台文件名只支持最大12个字节 // "123中國大中國"


/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的11号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*                          wangk          20180829        created
*****************************************************************/
void spfs8(void)
{
	/*private & local definition*/
	//int countnum = 0;
	int fp=0, writelen=0, readlen=0, ret=0;
	char writebuf[200]={0}, readbuf[200]={0};

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前测试所在模块与用例

	//测试前置
	NDK_SP_FsDel(NAMECZ1);
	NDK_SP_FsDel(NAMECZ2);
	NDK_SP_FsDel(NAMENEW1);
	NDK_SP_FsDel(NAMENEW2);
	if((fp=NDK_SP_FsOpen(NAMECZ1, "w"))<0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x11, sizeof(writebuf));
	if((writelen=NDK_SP_FsWrite(fp, writebuf, 200))!=200)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//以"w"方式打开一个特定文件(nameCZ2),并写入200字节数据:0x12
	if((fp=NDK_SP_FsOpen(NAMECZ2, "w"))<0) // 创建文件出错
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x12, sizeof(writebuf));
	if((writelen=NDK_SP_FsWrite(fp, writebuf, 200))!=200)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	/**********************以上动作是创建两个测试文件**************************************************/
	//case1:对一个已存在的文件将自己重命名为自己,应成功
	if((ret=NDK_SP_FsRename(NAMECZ1, NAMECZ1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2: 调用NDK_SP_FsRename将文件nameCZ1重命名为nameCZ2(目标名存在,更名操作应该失败)
#if 0
	if ((ret=NDK_SP_FsRename("nameCZ1", "nameCZ2"))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//case3: 删除文件nameCZ1,测试源文件不存在的情况
	if((ret=NDK_SP_FsDel(NAMECZ1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsRename(NAMECZ1, "123"))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsRename(NAMECZ1, NAMECZ1))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:测试更名的目标名不合法的情况(源文件合法也存在)
	if((ret=NDK_SP_FsRename(NAMECZ2, "/"))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4: 调用NDK_SP_FsRename更改文件nameCZ2为nameNEW1(合法目标名不存在,更名操作应该成功)
	if((ret=NDK_SP_FsRename(NAMECZ2, NAMENEW1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5: 删除文件nameCZ2(文件已被更名为nameNEW1,删除操作应该失败)
	if((ret=NDK_SP_FsDel(NAMECZ2))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6: 用"r"方式打开nameNEW1文件(改名后的文件,操作应该成功)
	if((fp=NDK_SP_FsOpen(NAMENEW1, "r"))<0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	//YAFFS文件系统允许对打开的文件进行更名操作，应该成功
	if((ret=NDK_SP_FsRename(NAMENEW1, NAMENEW2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7: 以"r"方式打开nameNEW2,以验证更名成功,读取200字节数据并校验(应为:0x12)
	if((fp=NDK_SP_FsOpen(NAMENEW2, "r"))<0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(readbuf, 0, sizeof(readbuf));
	if((readlen=NDK_SP_FsRead(fp, readbuf, 200))!=200)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR;
	}

	if(memcmp(writebuf, readbuf, 200))
	{
		send_result("line %d: 校验内容出错", __LINE__);
		GOTOERR;
	}
	NDK_SP_FsClose(fp);
	//更名为合法中文名(简/繁)
	if((ret=NDK_SP_FsRename(NAMENEW2, INPUTHZ))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsRename(INPUTHZ, NAMENEW2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsDel(NAMENEW2))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case8:源文件名不合法
	if((ret=NDK_SP_FsRename("/", "123"))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case9:异常测试
	if((ret=NDK_SP_FsRename(NULL, NAMECZ1))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}	
	if((ret=NDK_SP_FsRename(NAMECZ1, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	// 测试通过
	send_result("%s测试通过", TESTAPI);
	return;
ERR:
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(NAMECZ1);
	NDK_SP_FsDel(NAMECZ2);
	NDK_SP_FsDel(NAMENEW1);
	NDK_SP_FsDel(NAMENEW2);
	return;
}


