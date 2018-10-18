/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: spfs模块
* file name		: spfs3.c
* Author 			: linying
* version			: 
* DATE			: 20180828
* directory 		: 
* description		: 测试NDK_SP_FsSeek接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/

#define  FILETEST3 "/appfs/FT3"
#define	TESTAPI		"NDK_SP_FsSeek"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的3号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*                                  linying            20180828       created
*****************************************************************/
void spfs3(void)
{
	/*private & local definition*/
	//int countnum = 0;
	int fp=0, writelen=0, readlen=0, ret=0, seeklen=0;
	int loopcount=0;
	ulong pulRet=0;
	char writebuf[200]={0}, readbuf[200]={0};

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前测试所在模块与用例

	//测试前置
	NDK_SP_FsDel(FILETEST3);

	//case1:创建一个空的文件，对空文件移指针SEEK_CUR位置[0/1/-2]，[0/1]应成功返回,[-2]应失败返回。
	if((fp=NDK_SP_FsOpen(FILETEST3, "w"))<0) // 创建文件失败
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}	
	// 对空文件进行移指针(SEEK_CUR [0/1/-2])
	if((ret=NDK_SP_FsSeek(fp, 0l, SEEK_CUR))!=NDK_OK) // 空文件移指针SEEK_CUR位置0,应该成功
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_SP_FsSeek(fp, 1l, SEEK_CUR))!=NDK_OK) // 空文件移指针SEEK_CUR位置1,应返回成功
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//此时位于EOF之后的下一个字节位置,-2l以构造超出文件头的情况
	if((ret=NDK_SP_FsSeek(fp, -2l, SEEK_CUR))!=NDK_ERR) // 空文件移指针SEEK_CUR位置-2,应返回失败
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case2:将指针复位到开头，写入200字节内容为0x03的数据，应成功。
	memset(writebuf, 0x03, sizeof(writebuf));
	if((ret=NDK_SP_FsSeek(fp, 0l, SEEK_SET))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((writelen=NDK_SP_FsWrite(fp, writebuf, 200))!=200) // 写入200字节数据
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}

	NDK_SP_FsClose(fp);

	//case3:对已关闭的句柄，操作应失败返回。
	if((ret=NDK_SP_FsSeek(fp, 0l, SEEK_SET))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:打开一个非空文件，移动文件指针。
	if((fp=NDK_SP_FsOpen(FILETEST3, "w"))<0) // 创建文件失败
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	//case4.1:非空文件移指针SEEK_CUR位置0,应该成功
	if((ret=NDK_SP_FsSeek(fp, 0l, SEEK_CUR))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case4.2:非空文件移指针SEEK_CUR位置200,应该成功
	if((ret=NDK_SP_FsSeek(fp, 200l, SEEK_CUR))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case4.3:非空文件移指针SEEK_SET位置200,应该成功
	if((ret=NDK_SP_FsSeek(fp, 200l, SEEK_SET))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5:对非空文件移动SEEK_SET位置201，读取文件流位置，应该返回正确的位置201
	if((ret=NDK_SP_FsSeek(fp, 201l, SEEK_SET))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret= NDK_SP_FsTell(fp, &pulRet))!=NDK_OK||pulRet!=200) //只会到200
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pulRet);
		GOTOERR;
	}

	//case6:对非空文件移指针SEEK_SET位置0,应该成功。
	if((ret=NDK_SP_FsSeek(fp, 0l, SEEK_SET))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case7:非空文件移指针SEEK_SET位置-1,应该失败
	if((ret=NDK_SP_FsSeek(fp, -1l, SEEK_SET))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret= NDK_SP_FsTell(fp, &pulRet))!=NDK_OK||pulRet!=0)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret, pulRet);
		GOTOERR;
	}


	//case8:随机偏移,空文件移指针偏移SEEK_SET介于0到200之间(因为前面只写入200字节的数据)的随机数
	//获取文件流位置，应等于所设置的随机数。
	for(loopcount=0;loopcount<20;loopcount++)
	{
		seeklen=(long)(rand()%200);
		if((ret=NDK_SP_FsSeek(fp, seeklen, SEEK_SET))!=NDK_OK)
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret= NDK_SP_FsTell(fp, &pulRet))!=NDK_OK||pulRet!=seeklen) // 偏移后指针位置不对
		{
			send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}

	//case9:非空文件移指针SEEK_END位置0,应该成功
	if((ret=NDK_SP_FsSeek(fp, 0l, SEEK_END))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	if((ret= NDK_SP_FsTell(fp, &pulRet))!=NDK_OK||pulRet!=200) // 指针位置错
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}


	//这个用例和case5设计冲突
	//case10:对非空文件移动SEEK_END位置201，读取文件流位置，应该返回正确的位置201
	if((ret=NDK_SP_FsSeek(fp, 1l, SEEK_END))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret= NDK_SP_FsTell(fp, &pulRet))!=NDK_OK||pulRet!=200)

	{
		send_result("line %d: %s测试失败(%d，%d)", __LINE__, TESTAPI, ret, pulRet);
		GOTOERR;
	}
	//case11:非空文件移指针SEEK_END位置-200,应该成功,获取文件流位置，应返回流位置为0.
	if((ret=NDK_SP_FsSeek(fp, -200l, SEEK_END))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret= NDK_SP_FsTell(fp, &pulRet))!=NDK_OK||pulRet!=0)
	{
		send_result("line %d: %s测试失败(%d，%d)", __LINE__, TESTAPI, ret, pulRet);
		GOTOERR;
	}
	//case12:非空文件移指针SEEK_END位置-201,应该失败,此时获取文件流位置应为0的位置。
	if((ret=NDK_SP_FsSeek(fp, -201l, SEEK_END))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret= NDK_SP_FsTell(fp, &pulRet))!=NDK_OK||pulRet!=0)
	{
		send_result("line %d: %s测试失败(%d，%d)", __LINE__, TESTAPI, ret, pulRet);
		GOTOERR;
	}
	NDK_SP_FsClose(fp);

	//case13:以"r"方式打开文件，移文件指针SEEK_SET位置100，读取文件内容200，实际读取应返回100.
	if((fp=NDK_SP_FsOpen(FILETEST3, "r"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((ret=NDK_SP_FsSeek(fp, 100l, SEEK_SET))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((readlen=NDK_SP_FsRead(fp, readbuf, 200))!=100) // 读取校验内容
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR;
	}

	if(memcmp(readbuf, writebuf+100, 100))
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case14:异常测试
	if((ret=NDK_SP_FsSeek(fp, 0l, SEEK_END+10))!=NDK_ERR)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//测试清理
	NDK_SP_FsClose(fp);
	if((ret=NDK_SP_FsDel(FILETEST3))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	send_result("%s测试通过", TESTAPI); // 测试通过
	return;
ERR:
	NDK_SP_FsClose(fp);
	NDK_SP_FsDel(FILETEST3);
	return;
}

