/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name			: systest95.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			:devmgr/dbus交叉
* related document	: NDK.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"
	
/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"DBUS多线程"
#define TestApp1    "test1.NLD"
#define TestApp2    "test2.NLD"
#define TestApp3    "test3.NLD"
#define TestApp4    "test4.NLD"
	
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 		:
* functional description	: 
* input parameter		:
* output parameter		: 
* return value			:
* history				: author			date			remarks
*					      sull			  2018/04/11		created
*****************************************************************/
//针对农行MIS应用运行退出故障问题新增测试用例
//dbus服务包含comserv、secp、kpserv、libpms及devmgr , kpserv暂不考虑
static void info_vol()
{
	/*private & local definition*/	
	int i = 0, cnt = 200, ret = -1, fd = 0;
	
	/*process body*/
	if((cls_show_msg1(30,"请确保已下载%s，是按任意键继续进行测试，否则ESC退出", TestApp1)) == ESC)
		return;
	if((ret = NDK_AppLoad(TestApp1, 0)) != NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:装载子应用失败(%d)", __LINE__, ret);
		return;
	}
	for(i=1;i<=cnt;i++)
	{
		cls_printf("第%d次运行中...\n",i);
		fprintf(stderr,"运行第%d次\n",i);
		if((fd = NDK_FsOpen("/root/flag","w")) <0)
		{
			cls_show_msg1(g_keeptime, "line %d:创建文件失败(%d)", __LINE__, ret);
			NDK_FsClose(fd);
			NDK_FsDel("/root/flag");
			continue;
		}
		if((ret = NDK_FsClose(fd)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:关闭句柄失败(%d)", __LINE__, ret);
			NDK_FsClose(fd);
			NDK_FsDel("/root/flag");
			continue;
		}
		if((ret = NDK_AppRun("test1")) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:运行子应用失败(%d)", __LINE__, ret);
			NDK_FsClose(fd);
			NDK_FsDel("/root/flag");
			continue;
		}
		if( NDK_FsExist("/root/flag")==NDK_OK)
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME, g_keeptime, "line %d:子应用异常退出\n", __LINE__);
			goto ERR;
		}

	}
	cls_show_msg1_record(FILENAME ,FUNCNAME, g_keeptime, "line %d:取硬件信息/取电量子应用测试通过\n", __LINE__);

ERR:
	NDK_FsClose(fd);
	NDK_FsDel("/root/flag");
	return;
}

static void vol_sec()
{
	/*private & local definition*/	
	int i = 0, cnt = 200, ret = -1, fd = 0;
	
	/*process body*/
	if((cls_show_msg1(30,"请确保已下载%s，是按任意键继续进行测试，否则ESC退出", TestApp2)) == ESC)
		return;
	if((ret = NDK_AppLoad((const char *)TestApp2, 0)) != NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:装载子应用失败(%d)", __LINE__, ret);
		return;
	}
	for(i=1;i<=cnt;i++)
	{
		cls_printf("第%d次运行中...\n",i);
		fprintf(stderr,"运行第%d次\n",i);
		if((fd = NDK_FsOpen("/root/flag","w")) <0)
		{
			cls_show_msg1(g_keeptime, "line %d:创建文件失败(%d)", __LINE__, ret);
			NDK_FsClose(fd);
			NDK_FsDel("/root/flag");
			continue;
		}
		if((ret = NDK_FsClose(fd)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:关闭句柄失败(%d)", __LINE__, ret);
			NDK_FsClose(fd);
			NDK_FsDel("/root/flag");
			continue;
		}
		if((ret = NDK_AppRun("test2")) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:运行子应用失败(%d)", __LINE__, ret);
			NDK_FsClose(fd);
			NDK_FsDel("/root/flag");
			continue;
		}
		if( NDK_FsExist("/root/flag")==NDK_OK)
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME, g_keeptime, "line %d:子应用异常退出\n", __LINE__);
			goto ERR;
		}

	}
	cls_show_msg1_record(FILENAME ,FUNCNAME, g_keeptime, "line %d:取硬件信息/安全子应用测试通过\n", __LINE__);

ERR:
	NDK_FsClose(fd);
	NDK_FsDel("/root/flag");
	return;
}

static void sec_wlm()
{
	/*private & local definition*/	
	int i = 0, cnt = 200, ret = -1, fd = 0;
	
	/*process body*/
	if((cls_show_msg1(30,"请确保已下载%s,是按任意键继续进行测试，否则ESC退出", TestApp3)) == ESC)
		return;
	if((ret = NDK_AppLoad((const char *)TestApp3, 0)) != NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:装载子应用失败(%d)", __LINE__, ret);
		return;
	}
	for(i=1;i<=cnt;i++)
	{
		cls_printf("第%d次运行中...\n",i);
		fprintf(stderr,"运行第%d次\n",i);
		if((fd = NDK_FsOpen("/root/flag","w")) <0)
		{
			cls_show_msg1(g_keeptime, "line %d:创建文件失败(%d)", __LINE__, ret);
			NDK_FsClose(fd);
			NDK_FsDel("/root/flag");
			continue;
		}
		if((ret = NDK_FsClose(fd)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:关闭句柄失败(%d)", __LINE__, ret);
			NDK_FsClose(fd);
			NDK_FsDel("/root/flag");
			continue;
		}
		if((ret = NDK_AppRun("test3")) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:运行子应用失败(%d)", __LINE__, ret);
			NDK_FsClose(fd);
			NDK_FsDel("/root/flag");
			continue;
		}
		if( NDK_FsExist("/root/flag")==NDK_OK)
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME, g_keeptime, "line %d:子应用异常退出\n", __LINE__);
			goto ERR;
		}

	}
	cls_show_msg1_record(FILENAME ,FUNCNAME, g_keeptime, "line %d:取硬件信息/wifi子应用测试通过\n", __LINE__);
	
ERR:
	NDK_FsClose(fd);
	NDK_FsDel("/root/flag");
	return;
}

static void info_wlm()
{
	/*private & local definition*/	
	int i = 0, cnt = 200, ret = -1, fd = 0;
	
	/*process body*/
	if((cls_show_msg1(30,"请确保已下载%s,是按任意键继续进行测试，否则ESC退出", TestApp4)) == ESC)
		return;
	if((ret = NDK_AppLoad((const char *)TestApp4, 0)) != NDK_OK)
	{
		cls_show_msg1(g_keeptime, "line %d:装载子应用失败(%d)", __LINE__, ret);
		return;
	}
	for(i=1;i<=cnt;i++)
	{
		cls_printf("第%d次运行中...\n",i);
		fprintf(stderr,"运行第%d次\n",i);
		if((fd = NDK_FsOpen("/root/flag","w")) <0)
		{
			cls_show_msg1(g_keeptime, "line %d:创建文件失败(%d)", __LINE__, ret);
			NDK_FsClose(fd);
			NDK_FsDel("/root/flag");
			continue;
		}
		if((ret = NDK_FsClose(fd)) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:关闭句柄失败(%d)", __LINE__, ret);
			NDK_FsClose(fd);
			NDK_FsDel("/root/flag");
			continue;
		}
		if((ret = NDK_AppRun("test4")) != NDK_OK)
		{
			cls_show_msg1(g_keeptime, "line %d:运行子应用失败(%d)", __LINE__, ret);
			NDK_FsClose(fd);
			NDK_FsDel("/root/flag");
			continue;
		}
		if( NDK_FsExist("/root/flag")==NDK_OK)
		{
			cls_show_msg1_record(FILENAME ,FUNCNAME, g_keeptime, "line %d:子应用异常退出\n", __LINE__);
			goto ERR;
		}

	}
	cls_show_msg1_record(FILENAME ,FUNCNAME, g_keeptime, "line %d:取硬件信息/wifi子应用测试通过\n", __LINE__);
	
ERR:
	NDK_FsClose(fd);
	NDK_FsDel("/root/flag");
	return;
}

void systest95(void)
{	
	/*private & local definition*/	
	int nKeyin = 0;
	
	/*process body*/
	while(1)
    {
		nKeyin = cls_show_msg( "1.取硬件信息/取电量\n"
						       "2.取电量/安全\n"
						       "3.安全/无线\n"
						       "4.取硬件信息/无线\n");
		switch(nKeyin) 
		{
			case '1':
				info_vol();
				break;
			case '2':
				vol_sec();
				break;
			case '3':
				sec_wlm();
				break;
			case '4':
				info_wlm();
				break;
			case ESC:
				return;
				break;
			default:
				continue;
				break;
		}
	}
	return;
}
