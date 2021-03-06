/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name		: fs4.c
* Author 			: chenfm
* version			: 
* DATE			: 20121217
* directory 		: 
* description		: 测试NDK_FsWrite接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if !K21_ENABLE || OVERSEAS_ENABLE || defined SP610
#define  FILETEST4 "FT4"
#else	//K21平台只支持绝对路径
#define  FILETEST4 "/appfs/FT4"
#endif
#define	TESTAPI		"NDK_FsWrite"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : 
* functional description : 文件系统模块的4号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*
*****************************************************************/
void fs4(void)
{
	/*private & local definition*/
	//int countnum = 0;
	int fp=0, writelen=0, readlen=0, ret=0, punSize=0;
	char writebuf[200]={0}, readbuf[200]={0};
#if CPU5810X_ENABLE//低端30\32内存不够，会导致机器死机,中端在综合测试有测,ac35平台最多只能写20K,故此处只增加对5810x平台产品测试20180515 modify
	char writebuf1[21504]={0}, readbuf1[21504]={0};
#endif
	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_printf("测试%s...", TESTAPI); //屏幕提示当前测试所在模块与用例

	//测试前置
	NDK_FsDel(FILETEST4);

	//case1: 以"w"方式打开一个特定文件,并写入200字节数据0x41，先写0个数据进去，再写200个数据，应该成功返回。
	if((fp=NDK_FsOpen(FILETEST4, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf, 0x41, sizeof(writebuf));
	if((writelen=NDK_FsWrite(fp, writebuf, 0))!=0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((writelen=NDK_FsWrite(fp, writebuf, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}

	//case2: 将参数中的长度设置为-1进行写入操作
	if((writelen=NDK_FsWrite(fp, writebuf, -1))!=NDK_ERR_WRITE)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:写一个已经关闭的文件句柄，应该失败返回
	if((ret=NDK_FsWrite(fp, writebuf, 200))!=NDK_ERR_WRITE)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4: 以"r"方式打开该文件,写入200字节数据0x42(应该不能写入),失败返回
	memset(writebuf, 0x42, sizeof(writebuf));
	if((fp=NDK_FsOpen(FILETEST4, "r"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((writelen=NDK_FsWrite(fp, writebuf, 200))!=NDK_ERR_WRITE)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case5: 以"r"方式打开文件并加以检验，校验内容，前面所写的内容应与所读到的内容应一致。
	if((fp=NDK_FsOpen(FILETEST4, "r"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(readbuf, 0, sizeof(readbuf));
	if((readlen=NDK_FsRead(fp, readbuf, 200))!=200)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR;
	}
	memset(writebuf, 0x41, sizeof(writebuf));
	if(memcmp(writebuf, readbuf, 200))
	{
		send_result("line %d: 校验内容出错", __LINE__);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case6:异常测试
	if((fp=NDK_FsOpen(FILETEST4, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if((writelen=NDK_FsWrite(fp, NULL, 200))!=NDK_ERR_PARA)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}	
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsDel(FILETEST4))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if CPU5810X_ENABLE
    //case7:测试一次性可以写入21K的数据20180502 linying added
    if((fp=NDK_FsOpen(FILETEST4, "w"))<0)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	memset(writebuf1, 0x13, sizeof(writebuf1));
	if((writelen=NDK_FsWrite(fp, writebuf1, 21504))!=21504)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, writelen);
		GOTOERR;
	}
	NDK_FsClose(fp);
	if ((fp=NDK_FsOpen(FILETEST4, "r"))<0) 
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if ((ret=NDK_FsFileSize(FILETEST4,&punSize))!=NDK_OK||punSize!=21504)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, ret,punSize);
		GOTOERR;
	}
	memset(readbuf1, 0, sizeof(readbuf1));
	if((readlen=NDK_FsRead(fp,readbuf1, 21504))!=21504)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, readlen);
		GOTOERR;
	}
	else
	{
		if (memcmp(writebuf1, readbuf1,21504)) // 校验写入与读出数据的内容
		{
			send_result("line %d: %s测试失败", __LINE__, TESTAPI);
			goto ERR;
		}		
	}
#endif
	send_result("%s测试通过", TESTAPI);

ERR:
	NDK_FsClose(fp);
	NDK_FsDel(FILETEST4);
	return;
}

