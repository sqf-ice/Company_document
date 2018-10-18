/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: App11.c
* Author 			:
* version			: 
* DATE			:
* directory 		: 
* description		: 测试NDK_AppDataSave,NDK_AppDataVerify,NDK_AppUpdate数据传输、数据校验、app更新
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define  TESTAPI		"NDK_AppDataSave,NDK_AppDataVerify,NDK_AppUpdate"
#if defined SP610
#define  APPTEST11		"right.NLP"
#define  ERRSIGNFILE	"wrong.NLP"//文件签名错误
#define  APPTEST12     "master.NLP"
#define  APPTEST3      "ApppkS4.NLP"   //打包地址0x0314C000(APPPK地址)，应用小于4k，预期应该更新失败
#define  APPTEST4      "DApppkS8.NLP"  //打包地址0x0314A000(DEBUGAPPPK地址),应用大于4k、小于8k，预期应装载失败
#define  APPTEST5     "FSigS4.NLP"  //打包地址0x0314D000(Font24*24-SIG地址),应用小于4k，预期应该装载失败
#define  APPTEST6     "SigB4.NLP"   //打包地址0x0314B000(APPPK-SIG)地址，应用大于4k，预期应该装载失败
#define  APPTEST7     "SigS4.NLP"  //打包地址0x0314B000(APPPK-SIG)地址，应用小于4k，预期应该装载失败
#else
#define  APPTEST11		"/appfs/right.NLP"
#define  ERRSIGNFILE	"/appfs/wrong.NLP"//文件签名错误
#define  APPTEST12      "/appfs/master.NLP"
#define  FWTESTFILE    "/appfs/fwupdat.img"
#endif

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
*
*****************************************************************/
//pszName:应用程序文件路径，i
//NLP格式:16字节的头 + 256字节的签名 + 4个字节的固件长度 + 实际数据
//16字节头错误返回NDK_ERR_APP_NLD_HEAD_LEN
//256字节签名错返回NDK_ERR_APP_SIGN_DECRYPT
//4字节长度和实际数据错返回NDK_ERR_APP_SIGN_CHECK
int app_transdata(const char *pszName, int offseterrflag, int buferrflag)
{
	int  ret = 0, offset=0, fp=0, tmp = 0;
	uint len = 0;
	char buf[1024]={0};
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
#if defined ME68||defined ME50NX  //夏新平台更新app:offset = 256*1024+offset；更新固件:offset = 0+ offset
		if((ret=NDK_AppDataSave(256*1024+offset,buf,len))!=NDK_OK)
#else
		if((ret=NDK_AppDataSave(offset, buf, len))!=NDK_OK)
#endif
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
void app11(void)
{
	/*private & local definition*/
	int  ret = 0, fp=0, nRebootFlag = 0,fd = 0;
	uint offset=0, len=0, size = 0, i = 0;
	char buf[1024]={0};
	
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
#if !(defined ME15C||defined ME15CHX||defined ME15B)
	if(cls_show_msg1(30, "测试%s...请确保%s,%s和%s测试文件存在,ESC退出", TESTAPI, APPTEST11, ERRSIGNFILE, APPTEST12)==ESC)
#elif defined SP610
	if(cls_show_msg1(30, "测试%s...请确保所有的测试文件存在,ESC退出", TESTAPI)==ESC)
#else
	if(cls_show_msg1(30, "测试%s...请确保%s测试文件存在,ESC退出", TESTAPI, APPTEST11)==ESC)
#endif
		return;//屏幕提示当前测试所在模块与用例
		
#if defined ME68 || defined ME50NX
	if((fp = NDK_FsOpen(APPTEST11, "w")) <0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
		RETURN;
	}
	if((ret = NDK_FsWrite(fp, "Scorpiop", 8)) != 8)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_FsClose(fp)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	if(cls_show_msg("是否进行1号子用例测试，是按确认键，否则按其它键")==ENTER)
	{
		//case1:正常测试：文件传输过程中，断电后，开启后仍能继续传输，重启标志为1
		if(cls_show_msg1(30, "是否要跳过之前传输的数据部分(重启后要求跳过)?跳过[ENTER],不跳过[其它]。")!=ENTER)
		{
			if((fp=NDK_FsOpen(APPTEST11, "r"))<0)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
				return;
			}
			if((len=NDK_FsRead(fp, (char *)buf, 1024))<0)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, len);
				goto ERR;
			}
#if defined ME68||defined ME50NX
			if((ret=NDK_AppDataSave(256*1024+offset,buf,len))!=NDK_OK)
#else
			if((ret=NDK_AppDataSave(offset, buf, len))!=NDK_OK)
#endif
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,  ret);
				goto ERR;
			}
		}
#if defined ME50N || defined ME50 ||defined ME50C||defined ME50H||defined ME62 ||defined ME50NX||defined ME68//ME50N机子的取消键为电源键
		if(cls_show_msg("按确认键重启POS。(若之前已进行此操作，按其他键继续)")==ENTER)
			NDK_SysReboot();
#else
		cls_show_msg("请按电源重启POS。(若之前已进行此操作，可按任意键继续)");
#endif
		if((fp=NDK_FsOpen(APPTEST11, "r"))<0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
			return;
		}
		if((len=NDK_FsRead(fp, (char *)buf, 1024))<0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, len);
			goto ERR;
		}
		offset+=len;
		while(1)
		{
			if((len=NDK_FsRead(fp, (char *)buf, 1024))<0)
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, len);
				goto ERR;
			}
#if defined ME68||defined ME50NX  //夏新平台更新app:offset = 256*1024+offset；更新固件:offset = 0+ offset
			if((ret=NDK_AppDataSave(256*1024+offset,buf,len))!=NDK_OK)
#else
			if((ret=NDK_AppDataSave(offset, buf, len))!=NDK_OK)
#endif
			{
				send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,  ret);
				goto ERR;
			}
			offset+=len;
			if(len<1024)
				break;
		}
		NDK_FsClose(fp);
#if !(defined ME15C||defined ME15CHX||defined ME15B)	//ME15C不支持NDK_AppDataVerify函数和NDK_AppUpdate(0)
		if((ret=NDK_AppDataVerify())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return;
		}
#endif	
		cls_show_msg1(3,"3S后重启...重启后观察应用是否更新为right的程序,若成功更新,该用例视为通过。");
		if((ret=NDK_AppUpdate(1))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return;
		}
	}

	if(cls_show_msg("是否进行2号子用例测试，是按确认键，否则按其它键")==ENTER)
	{
		//case2:文件签名正确，传输文件正确，数据检验操作成功，app更新成功，重启标志为0
		if((ret=app_transdata(APPTEST11, 0, 0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return;
		}
#if !(defined ME15C||defined ME15CHX||defined ME15B)	//ME15C不支持NDK_AppDataVerify函数和NDK_AppUpdate(0)
		if((ret=NDK_AppDataVerify())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return;
		}
#if defined ME68 || defined ME50NX //夏新平台原有应用被覆盖掉后会跑飞，现象为重启
		cls_show_msg1(3,"3秒后将进行应用更新，应用更新后会重启(若出现乱码为正常现象)");
#else
		cls_show_msg1(3,"3S后将进行应用更新，更新后应该不重启");
#endif
		nRebootFlag = 0;
#else
		cls_show_msg1(3,"3S后重启...重启后观察应用是否更新为right的程序,若成功更新,该用例视为通过。");
		nRebootFlag = 1;
		if((ret=NDK_AppUpdate(0))!=NDK_ERR_NOT_SUPPORT)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return;
		}
#endif
		if((ret=NDK_AppUpdate(nRebootFlag))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			return;
		}
	}

#if !(defined ME15C||defined ME15CHX||defined ME15B)	//ME15C不支持NDK_AppDataVerify函数,所以APP更新无法知道是否有问题.  zhengry  20160419  modified
	//case3:异常测试：文件签名错误，传输文件成功，数据校验和数据更新都应返回签名校验错误NDK_ERR_APP_SIGN_DECRYPT,
#if defined ME68 || defined ME50NX
	if((fp = NDK_FsOpen(ERRSIGNFILE, "w")) <0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
		RETURN;
	}
	if((ret = NDK_FsWrite(fp, "Scorpiop", 8)) != 8)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_FsClose(fp)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif
	if((ret=app_transdata(ERRSIGNFILE, 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
#if defined ME68||defined ME50NX
	if((ret=NDK_AppDataVerify())!=NDK_ERR_APP_SIGN_CHECK)
#else
	if((ret=NDK_AppDataVerify())!=NDK_ERR_APP_SIGN_DECRYPT)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
#if defined ME68||defined ME50NX
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_SIGN_CHECK)
#else
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_SIGN_DECRYPT)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//case4:异常测试：文件传输中，offset相对地址传递错误引起app不完成，数据校验和数据更新应返回sha256校验失败NDK_ERR_APP_SIGN_CHECK
	if((ret=app_transdata(APPTEST11, 1, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_AppDataVerify())!=NDK_ERR_APP_SIGN_CHECK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_SIGN_CHECK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	
	//case5:异常测试：文件传输中，buf其中一个数据发生错误，数据校验和数据更新应返回sha256校验失败NDK_ERR_APP_SIGN_CHECK
	if((ret=app_transdata(APPTEST11, 0, 1))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_AppDataVerify())!=NDK_ERR_APP_SIGN_CHECK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_SIGN_CHECK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
#endif

#if defined SP610
	if((ret=app_transdata(APPTEST3, 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL/*NDK_ERR*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	if((ret=app_transdata(APPTEST4, 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL/*NDK_ERR*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	if((ret=app_transdata(APPTEST5, 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL/*NDK_ERR*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if((ret=app_transdata(APPTEST6, 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL/*NDK_ERR*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	if((ret=app_transdata(APPTEST7, 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL/*NDK_ERR*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif	

	//case6:异常测试：文件签名正确，传输文件正确，数据检验操作成功，将数据传输更改一个数据，更新程序应校验失败。
	if((ret=app_transdata(APPTEST11, 0, 0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
#if !(defined ME15C||defined ME15CHX||defined ME15B)	//ME15C不支持NDK_AppDataVerify函数
	if((ret=NDK_AppDataVerify())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
#endif	
#if defined ME50NX||defined ME68
	if((ret=NDK_AppDataSave(256*1024+3, (uchar *)'1', 1))!=NDK_OK)
#else
	if((ret=NDK_AppDataSave(0, (uchar *)'1', 1))!=NDK_OK)  //将数据传输更改一个数据
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,  ret);
		return;
	}
#if !(defined ME15C||defined ME15CHX||defined ME15B)
#if defined ME68||defined ME50NX  //夏新平台升级失败都是返回-821  
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_SIGN_CHECK)
#else
	if((ret=NDK_AppUpdate(0))!=NDK_ERR_APP_NLD_HEAD_LEN)
#endif
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
#else
	//ME15C NDK_AppUpdate无法判断更新是否成功    zhengry 20160421 modified
	cls_show_msg1(3,"3S后重启...自动重启会进入下载,再次重启后观察到应用并没有更新成功,仍是原先的应用。");
	if((ret=NDK_AppUpdate(1))!=NDK_ERR_APP_NLD_HEAD_LEN)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
#endif

	//case7:新增更新master用例，观察是否有问题 20171120 sull
#if CPU5810X_ENABLE //只有5810平台支持这个功能，目前只有ME50N支持 20171122
	//注意:如果boot更换需要先更新boot再进行此用例测试
	cls_printf("升级固件测试中...");
	offset = 0;
	if((fd = NDK_FsOpen(APPTEST12,"r"))<0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fd);
		goto ERR1;
	}
	NDK_FsFileSize(APPTEST12,&size);
	len = size /sizeof(buf);
	for(i=0;i<len;i++)
	{
		memset(buf,0x00,sizeof(buf));
		if((ret = NDK_FsRead(fd,buf,sizeof(buf)))<0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR1;
		}
		if((ret = NDK_AppDataSave(offset,buf,sizeof(buf)))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR1;
		}
		offset += sizeof(buf);
	}
	memset(buf,0x00,sizeof(buf));
	if((ret = NDK_FsRead(fd,buf,size%sizeof(buf)))<0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR1;
	}
	if((ret = NDK_AppDataSave(offset,buf,size%sizeof(buf)))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR1;
	}
	if((ret = NDK_FsClose(fd))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR1;
	}
	if((ret = NDK_FsDel(APPTEST12))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR1;
	}
	if(cls_show_msg("按确认键重启POS,重启后进入自检查看版本是否更新,注意不同产品的master不同,可找开发提供,且测完后需要重新刷回原来的master")==ENTER)
		NDK_SysReboot();
#endif

#if defined ME68||defined ME50NX  //夏新平台新增升级固件用例:更新固件:offset = 0+ offset
	cls_printf("升级固件测试中...");
	offset = 0;
	if((fp=NDK_FsOpen(FWTESTFILE, "r"))<0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fp);
		goto ERR;
	}
	while(1)
	{
		if((len=NDK_FsRead(fp, (char *)buf, 1024))<0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, len);
			goto ERR;
		}
		if((ret=NDK_AppDataSave(offset, buf, len))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			goto ERR;
		}
		offset+=len;
		if(len<1024)
			break;
	}
	if((ret = NDK_FsClose(fp)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}	
	//	固件升级 要在 data save 完之后调一次NDK_AppDataSave(0xffffffff, NULL, 0),ac35特有
	if((ret = NDK_AppDataSave(0xffffffff, NULL, 0)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}	
  	//ac35的固件升级不用校验，固件校验由ac35底包完成
	if(cls_show_msg("按确认键重启POS,重启后观察固件是否升级,测试完后需要更为原来的固件")==ENTER)
		NDK_SysReboot();
#endif
		
	//测试通过
	send_result("%s测试通过", TESTAPI);	
ERR:
	NDK_FsClose(fp);
	return;
ERR1:
	NDK_FsClose(fd);
	NDK_FsDel(APPTEST12);
	return;
}


