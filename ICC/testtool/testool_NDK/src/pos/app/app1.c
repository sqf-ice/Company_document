/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: app模块
* file name		: App1.c
* Author 			: dait
* version			: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_AppLoad,NDK_AppGetReboot接口
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#if K21_ENABLE&&!CPU5810X_ENABLE&&!defined ME50NX&&!defined ME68
#define  FILETEST1 "/appfs/test1.NLP"   //字母数字
#define  FILETEST2 "/appfs/自检1.NLP"    //汉字数字
#define  FILETEST3 "/appfs/k#~a.NLP"   //特殊字符 之前的&符号不支持
#define  FILETEST4 "/appfs/App_NULL.NLP" //不存在的应用,无需打包
#define  FILETEST5 "/appfs/ErrDg.NLP" 	//签名错误的应用
#define  FILETEST6 "/appfs/shutdow.NLP" //用于测试低端产品安装应用过程中断电，重启后要能自动继续安装的用例 //目前仅31支持，且必须设置测试程序自动运行该功能才生效//改成11字节+结束符12字节20180810modify
#elif CPU5810X_ENABLE||defined ME50NX||defined ME68//defined SP610||defined ME50||defined ME50N||defined ME62
#define  FILETEST1 "test1.NLP"   //字母数字
#define  FILETEST2 "自检1.NLP"    //汉字数字
#define  FILETEST3 "k#~a.NLP"   //特殊字符 之前的&符号不支持
#define  FILETEST4 "App_NULL.NLP" //不存在的应用,无需打包
#define  FILETEST5 "ErrDg.NLP" 	//签名错误的应用
#define  FILETEST6 "shutdow.NLP" //用于测试低端产品安装应用过程中断电，重启后要能自动继续安装的用例 //目前仅31支持，且必须设置测试程序自动运行该功能才生效//68长度若为12字节会出错,且5810产品在出货整理后测试也不支持12字节,故改成11字节20180612 modify
#define  FILETEST7 "overboot.NLP"	//打包时.xml文件中的<address>地址小于44KB的测试nlp文件,预期下载应该失败
#if defined SP610
#define  FILETEST8 "ApppkS4.NLP"   //打包地址0x0314C000(APPPK地址)，应用小于4k，预期应该装载失败
#define  FILETEST9 "ApppkS8.NLP"  //打包地址0x0314A000(DEBUGAPPPK地址),应用大于4k、小于8k，预期应装载失败
#define  FILETEST10 "FSigS4.NLP"  //打包地址0x0314D000(Font24*24-SIG地址),应用小于4k，预期应该装载失败
#define  FILETEST11 "SigB4.NLP"   //打包地址0x0314B000(APPPK-SIG)地址，应用大于4k，预期应该装载失败
#define  FILETEST12 "SigS4.NLP"  //打包地址0x0314B000(APPPK-SIG)地址，应用小于4k，预期应该装载失败
#endif
#elif OVERSEAS_ENABLE
//海外产品7.0正式版本，不支持root权限，不支持开控制台，不支持开发版，为了方便，放在当前路径下，且不测root权限相关用例  20180420 sull modify
#define  FILETEST1 "typeAunMaster.NLD"  //非主控类型普通应用
#define  FILETEST2 "typeAMaster.NLD" 	//主控类型普通应用
#define  FILETEST5 "typeI.NLD" 		//应用目录下的共享文件补丁
#define  FILETEST6 "typeS_A.NLD" //A类型主控应用补丁包
#define  FILETEST9 "App_NULL.NLD" //不存在的包 
#define  FILETEST10 "fw_patch_5.2.0.0.NLD" //版本不正确的固件补丁包 需要自行打包,签名要正确
#define  FILETEST11 "fw_patch_5.3.1.0.NLD" //版本正确的 需要自行打包,签名要正确,且测试一次后因为固件版本被升级,补丁包失效需要重新打包才能再测试
#define  FILETEST12 "test01234567890123456789012345678.NLD"
#define  FILESYS  "/appfs/fsys"   //海外版本需要使用绝对路径(旧主控被删除相对路径不可用)
#define  FILESYS1 "/appfs/fsys1"
#if !OVERSEAS_OS70_ENABLE
#define  FILETEST3 "typeRunMaster.NLD" 	//非主控类型系统应用
#define  FILETEST4 "typeRMaster.NLD" 	//主控类型系统应用
#define  FILETEST7 "typeS_R_RMaster.NLD" //R类型主控应用补丁包
#define  FILETEST8 "typeS_R_RunMaster.NLD" //R类型非主控应用补丁包
#endif
#elif defined ME50S || defined ME50NS
#define  FILETEST1 "/appfs/apps/download/App_U_NonMaster.NLD"  //此文件需在/appfs/download/temp目录下
#define  FILETEST2 "/appfs/apps/download/App_U_Master.NLD"  //此文件需在/appfs/download/temp目录下
#define  FILETEST3 "/appfs/apps/download/App_A_NonMaster.NLD" 
#define  FILETEST4 "/appfs/apps/download/App_A_Master.NLD" 
#define  FILETEST5 "/appfs/apps/download/App_P.NLD" 
#define  FILETEST6 "/appfs/apps/download/App_L.NLD" 
#define  FILETEST7 "/appfs/apps/download/App_A_NonMaster_pri.NLD" //给应用打补丁包
#define  FILETEST8 "/appfs/apps/download/fw_patch_1.2.3.4_2013.NLD" //给固件打补丁包（版本不正确）
#define  FILETEST9 "/appfs/apps/download/App_ErrorDigist.NLD"
#define  FILETEST10 "/appfs/apps/download/App_NULL.NLD"
#define  FILETEST11 "/appfs/apps/download/NDK_AppEXECV.NLD"
#define  FILETEST12 "/appfs/apps/download/fw_patch_1.2.3.5_2013.NLD" //给固件打补丁包（请测试人员自行打包一个正确的固件补丁包进行验证，固件版本名称和POS上的版本前3位要一致）
#define  FILETEST13 "/appfs/apps/download/a` $&();'a.NLD"//该文件中的字符 目前无法支持，后续是否需要
#define  FILETEST14 "/appfs/apps/download/a~!@#%^_=+,.[{]}丁.NLD"
#define  FILETEST15 "/appfs/apps/download/Upt_CERT_TEST.NLD"
#define  FILETEST16 "/appfs/apps/download/test01234567890123456789012345678.NLD"
#define  FILESYS  "fsys"
#define  FILESYS1 "fsys1"
#else
#define  FILETEST1 "/appfs/download/App_U_NonMaster.NLD"  //此文件需在/appfs/download/temp目录下
#define  FILETEST2 "/appfs/download/App_U_Master.NLD"  //此文件需在/appfs/download/temp目录下
#define  FILETEST3 "/appfs/download/App_A_NonMaster.NLD" 
#define  FILETEST4 "/appfs/download/App_A_Master.NLD" 
#define  FILETEST5 "/appfs/download/App_P.NLD" 
#define  FILETEST6 "/appfs/download/App_L.NLD" 
#define  FILETEST7 "/appfs/download/App_A_NonMaster_pri.NLD" //给应用打补丁包
#define  FILETEST8 "/appfs/download/fw_patch_1.2.3.4_2013.NLD" //给固件打补丁包（版本不正确）
#define  FILETEST9 "/appfs/download/App_ErrorDigist.NLD"
#define  FILETEST10 "/appfs/download/App_NULL.NLD"
#define  FILETEST11 "/appfs/download/NDK_AppEXECV.NLD"
#define  FILETEST12 "/appfs/download/fw_patch_1.2.3.5_2013.NLD" //给固件打补丁包（请测试人员自行打包一个正确的固件补丁包进行验证，固件版本名称和POS上的版本前3位要一致）
#define  FILETEST13 "/appfs/download/a` $&();'a.NLD"//该文件中的字符 目前无法支持，后续是否需要
#define  FILETEST14 "/appfs/download/a~!@#%^_=+,.[{]}丁.NLD"
#define  FILETEST15 "/appfs/download/Upt_CERT_TEST.NLD"
#define  FILETEST16 "/appfs/download/test01234567890123456789012345678.NLD"
#define  FILESYS  "fsys"
#define  FILESYS1 "fsys1"
#endif
#if K21_ENABLE	
#define 	TESTAPI	  "NDK_AppLoad"
#else
#define 	TESTAPI	  "NDK_AppLoad,NDK_AppGetReboot"
#endif
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	 : app1
* functional description : 应用管理模块的1号用例主函数
* input parameter	  	 :
* output parameter	 	 : 
* return value		 	 :
* history 		 		 : author			date	 	  remarks
*
*****************************************************************/
#if K21_ENABLE
void app1(void)
{
	/*private & local definition*/
	int  ret = 0;
#if defined ME68 || defined ME50NX	
	int  fd = 0;
#endif
	/*uchar *appname[] = {
							(uchar *)"test1",
							(uchar *)"自检1",
							(uchar *)"k#~a"
						};*/ //暂时不需要用到,消除警告信息

	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	if(cls_show_msg1(30, "测试%s...请确保测试文件存在,ESC退出", TESTAPI)==ESC)
		return;//屏幕提示当前测试所在模块与用例

	//case5:新增用例验证，测试安装过程中断电，重启后能自动安装前面断电的应用包
	if(cls_show_msg1(30, "请设置自动运行后,按确认键进入安装过程中断电测试，其它键跳过测试", TESTAPI)==ENTER)
	{
		cls_show_msg("按任意键开始安装应用,请在开始安装后2秒内断电，断电后重启应该能够自动完成应用程序的安装");
		//夏新平台的工具不支持以文件形式下载NLP文件，因为工具会识别文件头，若头为Scorpiop则会被工具识别，故采用故意将头改错，再用代码将头改为正确的形式下载NLP文件
#if defined ME68 || defined ME50NX  
		if((fd = NDK_FsOpen(FILETEST6, "w")) <0)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fd);
			RETURN;
		}
		if((ret = NDK_FsWrite(fd, "Scorpiop", 8)) != 8)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret = NDK_FsClose(fd)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
#endif
		cls_printf("请断电");
		if(( ret = NDK_AppLoad((char *)FILETEST6,0))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,  ret);
			RETURN;
		}
	}
	
	//case1:正常装载
#if defined ME68 || defined ME50NX
	if((fd = NDK_FsOpen(FILETEST1, "w")) <0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fd);
		RETURN;
	}
	if((ret = NDK_FsWrite(fd, "Scorpiop", 8)) != 8)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_FsClose(fd)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif
	if(( ret = NDK_AppLoad((char *)FILETEST1,0))!=NDK_OK)//uchar改成char 是为了消除警告 
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,  ret);
		RETURN;
	}
	
#if defined ME68 || defined ME50NX
	if((fd = NDK_FsOpen(FILETEST2, "w")) <0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fd);
		RETURN;
	}
	if((ret = NDK_FsWrite(fd, "Scorpiop", 8)) != 8)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_FsClose(fd)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif	
	if(( ret = NDK_AppLoad((char *)FILETEST2,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

#if defined ME68 || defined ME50NX
	if((fd = NDK_FsOpen(FILETEST3, "w")) <0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fd);
		RETURN;
	}
	if((ret = NDK_FsWrite(fd, "Scorpiop", 8)) != 8)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_FsClose(fd)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif
	if(( ret = NDK_AppLoad((char *)FILETEST3,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case2:文件不存在情况下测试,操作失败
	if(( ret = NDK_AppLoad((char *)FILETEST4,0))!=NDK_ERR_OPEN_DEV)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	
	//case3:文件不存在情况下测试,安装失败
	if(( ret = NDK_AppLoad(NULL,0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case4:文件签名不正确,安装失败 20141104 linwl added
#if defined ME68 || defined ME50NX
	if((fd = NDK_FsOpen(FILETEST5, "w")) <0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, fd);
		RETURN;
	}
	if((ret = NDK_FsWrite(fd, "Scorpiop", 8)) != 8)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_FsClose(fd)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif
	if(( ret = NDK_AppLoad((char *)FILETEST5,0))!=NDK_ERR_APP_SIGN_DECRYPT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}

	//case5:打包地址不对，应装载失败20180604 sull added
#if defined SP610
	if(( ret = NDK_AppLoad((char *)FILETEST8,0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL/*NDK_ERR*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(( ret = NDK_AppLoad((char *)FILETEST9,0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL/*NDK_ERR*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(( ret = NDK_AppLoad((char *)FILETEST10,0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL/*NDK_ERR*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(( ret = NDK_AppLoad((char *)FILETEST11,0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL/*NDK_ERR*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(( ret = NDK_AppLoad((char *)FILETEST12,0))!=NDK_ERR_APP_ADDRESS_NOT_LEGAL/*NDK_ERR*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

#if (CPU5810X_ENABLE&&!defined SP610) //defined ME50NX||defined ME68||defined ME50||defined ME50N||defined ME62  夏新平台暂不对地址作限制
	//case4:地址小于44K的应用包,应该安装失败
	if(( ret = NDK_AppLoad((char *)FILETEST7,0))!=NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	//测试通过
	send_result("%s测试通过,如果退出程序或重启后需重新下载测试程序", TESTAPI);//由于FILETEST6 是安装在测试程序位置，所以测试程序会被替换，需重新安装
/*ERR:
	for(i = 0;i<SZ_ARRAY(appname);i++)
		NDK_AppDel((char *)appname[i]);*/  //不支持NDK_AppDel
	return;
}
#elif OVERSEAS_ENABLE
void app1(void)
{
	/*private & local definition*/
	int  ret = 0, i = 0;
	int nRebootFlag=-1;
	int fp = -1, fp2 = -1;
	uchar *appname[] = {
							(uchar *)"typeAunMaster",
							(uchar *)"typeAMaster",
							(uchar *)"typeI",
							(uchar *)"typeS_A",
#if !OVERSEAS_OS70_ENABLE  //7.0版本开始不支持root权限 应用包 20180420 modify
							(uchar *)"typeRunMaster", 
							(uchar *)"typeRMaster",
							(uchar *)"typeS_R_RMaster",
							(uchar *)"typeS_R_RunMaster",
#endif
						};
	
	struct tm stSetRtcTime;//海外版本使用rtc硬时间判断证书 20180202
	struct tm stOldRtcTime;
	
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	if(cls_show_msg1(30, "测试%s...请确保测试文件存在,且当前用户程序为root权限,ESC退出", TESTAPI)==ESC)
		return;//屏幕提示当前测试所在模块与用例

	//测试前置:取当前时间为后续修改测试时间做准备
	NDK_FsDel(FILESYS);
	NDK_FsDel(FILESYS1);
	if((ret=NDK_SysGetRtcTime(&stOldRtcTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

#if !OVERSEAS_OS70_ENABLE //7.0版本开始不支持root权限  20180420 modify
	//case1:正常装载R类型非主控应用,应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST3,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:装载R类型非主控应用的补丁包应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST8,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//为避免冲掉主应用，把其它的应用也冲掉，将此测试项移到最后测	
#endif
	
	//case5:装载A类型主控应用的补丁包应该失败返回NDK_ERR_APP_NOT_EXIST
	if(( ret = NDK_AppLoad((uchar *)FILETEST6,0))!=NDK_ERR_APP_NOT_EXIST)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case6:正常装载A类型非主控应用,应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST1,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

  	//为避免冲掉主应用，把其它的应用也冲掉，将此测试项移到最后测

	//case9:正常装载I类型应用,应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST5,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case10:文件不存在情况下测试,安装失败
	if(( ret = NDK_AppLoad((uchar *)FILETEST9,0))!=NDK_ERR_APP_FILE_STAT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case11:文件不存在情况下测试,安装失败
	if(( ret = NDK_AppLoad(NULL,0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case12:验证文件名超长(超过32字节)情况下应该要返回NDK_ERR_APP_FILE_NAME_TOO_LONG
	if(( ret = NDK_AppLoad((uchar *)FILETEST12,0))!=NDK_ERR_APP_FILE_NAME_TOO_LONG)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,  ret);
		GOTOERR;
	}

	//case13:nRebootFlag参数为NULL,应该返回参数错误NDK_ERR_PARA
	if((ret = NDK_AppGetReboot(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case14:非固件补丁包都不需要重启，重启标志位应该为0
	if((ret = NDK_AppGetReboot(&nRebootFlag))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(nRebootFlag!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRebootFlag);
		GOTOERR;
	}
	
	//case15:版本不正确固件补丁包测试,安装应该返回NDK_ERR_APP_FIRM_PATCH_VERSION
	if(( ret = NDK_AppLoad((uchar *)FILETEST10,0))!=NDK_ERR_APP_FIRM_PATCH_VERSION)//测试开发版时版本不正确的固件补丁
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);//-821
		GOTOERR;
	}

	//case16:版本正确的固件补丁，应该能够成功下载，请自行打包修改,注意:运行一次后版本会升级再跑一次会报错,需要重新打包
	if(( ret = NDK_AppLoad((uchar *)FILETEST11,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case17:装载正确的固件补丁包后应该要重启，获取到的重启标志应该是1
	if((ret = NDK_AppGetReboot(&nRebootFlag))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(nRebootFlag!=1)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRebootFlag);
		GOTOERR;
	}
		
	//case18:验证NDK_AppLoad调用后是否把句柄关闭，句柄关闭情况下前后两次打开文件的句柄应该连续，如果不连续则说明NDK_AppLoad未关闭句柄
	if ((fp=NDK_FsOpen(FILESYS, "w"))<0) // 以写的文件打开刚刚删除的文件
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if(( ret = NDK_AppLoad((uchar *)FILETEST1,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,  ret);
		GOTOERR;
	}
	if ((fp2=NDK_FsOpen(FILESYS1, "w"))<0) // 以写的文件打开刚刚删除的文件
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp2);
		GOTOERR;
	}
	if((fp2-fp)!=1)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, fp2, fp);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp2))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case19:证书尚未生效 需要根据证书的有效时间来确定,目前以2013-2023年为参考
	//设置POS时间2012.10.10 10:10:10   设置2000年以前时间,正式未生效
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2012 - 1900;
	stSetRtcTime.tm_mon = 10 - 1;
	stSetRtcTime.tm_mday = 10;
	stSetRtcTime.tm_hour = 10;
	stSetRtcTime.tm_min = 10;
	stSetRtcTime.tm_sec = 10;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if(( ret = NDK_AppLoad((uchar *)FILETEST1,0))!=NDK_ERR_APP_CERT_NOT_YET_VALID)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	
	//case20:证书已经失效
	//设置POS时间2024.10.10 10:10:10 设置2025年以后证书已失效
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2024 - 1900;
	stSetRtcTime.tm_mon = 10 - 1;
	stSetRtcTime.tm_mday = 10;
	stSetRtcTime.tm_hour = 10;
	stSetRtcTime.tm_min = 10;
	stSetRtcTime.tm_sec = 10;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if(( ret = NDK_AppLoad((uchar *)FILETEST1,0))!=NDK_ERR_APP_CERT_HAS_EXPIRED)//海外版本判断证书时间
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}

	//为后面主控应用能成功装载，设置为证书有效时间
	memset(&stSetRtcTime, 0, sizeof(stSetRtcTime));
	stSetRtcTime.tm_year = 2018 - 1900;
	stSetRtcTime.tm_mon = 10 - 1;
	stSetRtcTime.tm_mday = 10;
	stSetRtcTime.tm_hour = 10;
	stSetRtcTime.tm_min = 10;
	stSetRtcTime.tm_sec = 10;
	if((ret=NDK_SysSetRtcTime(stSetRtcTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}

	//case7:正常装载A类型主控应用,应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST2,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR2;
	}

#if 0  //由于主应用已经被冲掉，故文件不存在，无法验证
	//case8:装载A类型主控应用的补丁包应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST6,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2:正常装载R类型主控应用,应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST4,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:装载R类型主控应用的补丁包应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST7,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	//测试通过
	send_result("%s测试通过,请退出程序或重启重新下载测试程序,否则影响之后用例测试" , TESTAPI);
ERR2:
	NDK_SysSetRtcTime(stOldRtcTime);
	goto ERR;
ERR:
	for(i = 0;i<SZ_ARRAY(appname);i++)
		NDK_AppDel((char *)appname[i]);
	NDK_FsClose(fp);
	NDK_FsClose(fp2);
	NDK_FsDel(FILESYS);
	NDK_FsDel(FILESYS1);
	return;
}
#else
void app1(void)
{
	/*private & local definition*/
	int  ret = 0, i = 0;
	int nRebootFlag=-1;
	int fp = -1, fp2 = -1;
	uchar *appname[] = {
											(uchar *)"App_U_NonMaster",
											(uchar *)"App_U_Master",
											(uchar *)"App_A_NonMaster",
											(uchar *)"App_A_Master",
											(uchar *)"App_P",
											(uchar *)"App_L",
											(uchar *)"App_A_NonMaster_pri",
											//(uchar *)"fw_patch_1.2.3.4_2013",
											(uchar *)"NDK_AppEXECV",
											(uchar *)"a` $&();'a",
											(uchar *)"a~!@#%^_=+,.[{]}丁"
										};

	struct tm stSetPosTime;
	struct tm stOldPosTime;
	
	/*process body*/
	if(auto_flag==1)
	{
		send_result("%s不支持自动测试，请使用手动测试验证", TESTAPI);
		return;
	}
	if(cls_show_msg1(30, "测试%s...请确保测试文件存在,且当前用户程序为root权限,ESC退出", TESTAPI)==ESC)
		return;//屏幕提示当前测试所在模块与用例
	
	//测试前置:取当前时间为后续修改测试时间做准备
	NDK_FsDel(FILESYS);
	NDK_FsDel(FILESYS1);
	if((ret=NDK_SysGetPosTime(&stOldPosTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//case21:验证文件名超长(超过32字节)情况下应该要返回NDK_ERR_APP_FILE_NAME_TOO_LONG
	if(( ret = NDK_AppLoad((uchar *)FILETEST16,0))!=NDK_ERR_APP_FILE_NAME_TOO_LONG)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,  ret);
		GOTOERR;
	}
	
	//case1:正常装载U类型非主控应用，应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST1,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,  ret);
		GOTOERR;
	}
	
	//case2::正常装载U类型主控应用，应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST2,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3::正常装载A类型非主控应用，应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST3,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4:正常装载A类型主控应用，应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST4,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case5:正常装载P类型参数文件，应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST5,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case6:正常装载L类型库文件，应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST6,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case7:装载错误的消息摘要.安装失败
	if(( ret = NDK_AppLoad((uchar *)FILETEST9,0))!=NDK_ERR_APP_SIGN_CHECK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case8:普通应用安装成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST11,0))!=NDK_OK)//用于验证NDK_AppEXECV测试包
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case9:文件不存在情况下测试,安装失败
	if(( ret = NDK_AppLoad((uchar *)FILETEST10,0))!=NDK_ERR_APP_FILE_STAT)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case10:文件不存在情况下测试,安装失败
	if(( ret = NDK_AppLoad(NULL,0))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case11:应用补丁包测试,安装应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST7,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case14:nRebootFlag参数为NULL,应该返回参数错误NDK_ERR_PARA
	if((ret = NDK_AppGetReboot(NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case15:非固件补丁包都不需要重启，重启标志位应该为0
	if((ret = NDK_AppGetReboot(&nRebootFlag))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(nRebootFlag!=0)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRebootFlag);
		GOTOERR;
	}
	
#if 1
	//case12:版本不正确固件补丁包测试,安装应该返回NDK_ERR_APP_FIRM_PATCH_VERSION//daitao 20130711 下载固件增量包，最新的NDK要求有判断版本是否匹配，如果匹配则容许下载，否则禁止
	if(( ret = NDK_AppLoad((uchar *)FILETEST8,0))!=NDK_ERR_APP_FIRM_PATCH_VERSION)//版本不正确的固件补丁
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case13:版本正确的固件补丁，应该能够成功下载，请自行打包修改
	if(( ret = NDK_AppLoad((uchar *)FILETEST12,0))!=NDK_OK)//版本正确的固件补丁，请自行打包修改
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case16:装载正确的固件补丁包后应该要重启，获取到的重启标志应该是1
	if((ret = NDK_AppGetReboot(&nRebootFlag))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(nRebootFlag!=1)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, nRebootFlag);
		GOTOERR;
	}
#endif

	//20140603 added by huangjianb
	//case17:不支持的特殊字符测试,安装应该失败
	if(( ret = NDK_AppLoad((uchar *)FILETEST13,0))!=NDK_ERR/*NDK_ERR_APP_FILE_STAT*/)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case18:支持的特殊字符测试,安装应该成功
	if(( ret = NDK_AppLoad((uchar *)FILETEST14,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//End add

	//case21:验证NDK_AppLoad调用后是否把句柄关闭，句柄关闭情况下前后两次打开文件的句柄应该连续，如果不连续则说明NDK_AppLoad未关闭句柄
	if ((fp=NDK_FsOpen(FILESYS, "w"))<0) // 以写的文件打开刚刚删除的文件
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp);
		GOTOERR;
	}
	if(( ret = NDK_AppLoad((uchar *)FILETEST1,0))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI,  ret);
		GOTOERR;
	}
	if ((fp2=NDK_FsOpen(FILESYS1, "w"))<0) // 以写的文件打开刚刚删除的文件
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, fp2);
		GOTOERR;
	}
	if((fp2-fp)!=1)
	{
		send_result("line %d: %s测试失败(%d,%d)", __LINE__, TESTAPI, fp2, fp);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret=NDK_FsClose(fp2))!=NDK_OK)
	{
		send_result("line %d: %s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case19:证书尚未生效 需要根据证书的有效时间来确定,目前以2013-2023年为参考
	//设置POS时间2012.10.10 10:10:10   设置2000年以前时间,正式未生效
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2012 - 1900;
	stSetPosTime.tm_mon = 10 - 1;
	stSetPosTime.tm_mday = 10;
	stSetPosTime.tm_hour = 10;
	stSetPosTime.tm_min = 10;
	stSetPosTime.tm_sec = 10;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if(( ret = NDK_AppLoad((uchar *)FILETEST15,0))!=NDK_OK/*NDK_ERR_APP_CERT_NOT_YET_VALID*/)//20140901 NDK 5.0.0版本中不再校验证书时间，允许安装未生效，或过期的应用
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	
	//case20:证书已经失效
	//设置POS时间2024.10.10 10:10:10 设置2025年以后证书已失效
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 2024 - 1900;
	stSetPosTime.tm_mon = 10 - 1;
	stSetPosTime.tm_mday = 10;
	stSetPosTime.tm_hour = 10;
	stSetPosTime.tm_min = 10;
	stSetPosTime.tm_sec = 10;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
#if 0//OVERSEAS_ENABLE
	if(( ret = NDK_AppLoad((uchar *)FILETEST15,0))!=NDK_ERR_APP_CERT_HAS_EXPIRED)//海外版本仍然会判断证书时间
#else		
	if(( ret = NDK_AppLoad((uchar *)FILETEST15,0))!=NDK_OK/*NDK_ERR_APP_CERT_HAS_EXPIRED*/)//20140901 NDK 5.0.0版本中不再校验证书时间，允许安装未生效，或过期的应用
#endif	
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR2;
	}
#if 0
	//case21:公钥已经失效
	//设置POS时间2099.10.10 10:10:10 设置2098年以后公钥已经失效NDK_ERR_APP_PUBKEY_EXPIRED
	memset(&stSetPosTime, 0, sizeof(stSetPosTime));
	stSetPosTime.tm_year = 1970 - 1900;
	stSetPosTime.tm_mon = 10 - 1;
	stSetPosTime.tm_mday = 10;
	stSetPosTime.tm_hour = 10;
	stSetPosTime.tm_min = 10;
	stSetPosTime.tm_sec = 10;
	if((ret=NDK_SysSetPosTime(stSetPosTime)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
	if(( ret = NDK_AppLoad((uchar *)FILETEST15,0))!=NDK_ERR_APP_PUBKEY_EXPIRED)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR2;
	}
#endif
	//测试通过
	send_result("%s测试通过,如果退出程序或重启后需重新下载测试程序", TESTAPI);
ERR2:
	NDK_SysSetPosTime(stOldPosTime);
	goto ERR;
ERR:
	for(i = 0;i<SZ_ARRAY(appname);i++)
		NDK_AppDel((char *)appname[i]);
	NDK_FsClose(fp);
	NDK_FsClose(fp2);
	NDK_FsDel(FILESYS);
	NDK_FsDel(FILESYS1);
	return;
}

#endif
