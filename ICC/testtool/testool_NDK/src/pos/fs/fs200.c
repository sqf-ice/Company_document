/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: fs模块
* file name			: 
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: fs模块内随机组合情况
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define DESTFILE "/appfs/test"
#define TESTDIR     "./test1"
#if !K21_ENABLE //K21平台只支持绝对路径
#define  RANDFILE	"/tmp/randfile"	//绝对路径 
#elif OVERSEAS_ENABLE||defined SP610//SP610文件名不需要带路径,底层会自动增加/appfs路径名
#define  RANDFILE	"randfile" 
#else
#define  RANDFILE	"/appfs/randfile"	//绝对路径 
#endif

#define	TESTAPI	"fs模块内随机组合"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
	FsOpen=0,
	FsClose,
	FsSeek,
	FsWrite,
	FsDel,
	FsRead,
	FsTell,
	FsRename,
	FsTruncate,
	FsExist,
	FsFileSize,
#if K21_ENABLE
	//FsFormat,低端平台调用此函数会重启，不测
#if ANDIROD_ENABLE
	CopyFileToSecMod,
#else
	FsGetDiskSpace,
	FsDir,
	FsClearAppFile,
#endif
#else
	FsGetDiskSpace,
	FsDir,
	FsCreateDirectory,
	FsRemoveDirectory,
#endif
	MaxNum,
}EM_FS;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: 
* functional description 	: 
* input parameter	 		:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*						sull					20180208		created
*****************************************************************/
static int fs_randtest(int funnum)
{
	int ret = -1, fp = -1;
	char writebuf[200] = {0}, readbuf[200] = {0};
	ulong pulRet = 0,spaceofdisk1=0;
	char s[3000]={0};
	uint dirnum=0, punSize = 0; 
	switch(funnum)
	{	
		case FsOpen:
			fp = NDK_FsOpen(RANDFILE, "w");
			NDK_FsClose(fp); //增加关闭句柄,为防止退出后测试其他模块由于打开句柄数太多导致没有句柄数的问题20180801 modify
			break;
		case FsClose:
			NDK_FsClose(fp);
			break;
		case FsSeek:
			NDK_FsSeek(fp, 0l, SEEK_SET); 
			break;
		case FsWrite:
			NDK_FsWrite(fp, writebuf, 200); 
			break;
		case FsDel:
			NDK_FsDel(RANDFILE);
			break;
		case FsRead:
			NDK_FsRead(fp, readbuf, 200); 
			break;
		case FsTell:
			NDK_FsTell(fp, &pulRet); 
			break;
		case FsRename:
			NDK_FsRename(RANDFILE,RANDFILE); 
			break;
		case FsTruncate:
			NDK_FsTruncate(RANDFILE, 200); 
			break;
		case FsExist:
			NDK_FsExist(RANDFILE);
			break;
		case FsFileSize:
			NDK_FsFileSize(RANDFILE,&punSize);
			break;
#if K21_ENABLE
#if ANDIROD_ENABLE
		case CopyFileToSecMod:
			NDK_CopyFileToSecMod(RANDFILE, DESTFILE);
		break;
#else
		case FsGetDiskSpace:
			if((ret = NDK_FsGetDiskSpace(rand()%2, &spaceofdisk1)) != NDK_OK)
				goto ERR;
			break;
		case FsDir:
			memset(s, 0, sizeof(s));
			if(( ret = NDK_FsDir("./", s, &dirnum)) != NDK_OK)
				goto ERR;
			break;
		case FsClearAppFile:
			if((ret = NDK_FsClearAppFile()) != NDK_OK)
				goto ERR;
			break;
#endif 
#else
			case FsGetDiskSpace:
				if((ret = NDK_FsGetDiskSpace(rand()%2, &spaceofdisk1)) != NDK_OK)
					goto ERR;
				break;
			case FsDir:
				memset(s, 0, sizeof(s));
				NDK_FsDir("/appfs", s, &dirnum);
				break;
			case FsCreateDirectory:
				NDK_FsCreateDirectory(TESTDIR);
				break;
			case FsRemoveDirectory:
				NDK_FsRemoveDirectory(TESTDIR);
				break;
#endif
	}
	return SUCC;
ERR:
	NDK_FsDel(RANDFILE);
	NDK_FsDel(DESTFILE);
#if !K21_ENABLE
	NDK_FsRemoveDirectory(TESTDIR);
#endif
	return ret;	
}

void fs200(void)
{
	/*private & local definition*/
	int  i = 0, bak = 0, num[20] ={0}, j = 0;
	int ret = -1,cnt =50, len = 0;//cnt 待定
#if K21_ENABLE&&!ANDIROD_ENABLE
	char *fsstr[] = {"NDK_FsOpen", "NDK_FsClose", "NDK_FsSeek", "NDK_FsWrite", "NDK_FsDel", "NDK_FsRead", "NDK_FsTell", "NDK_FsRename", "NDK_FsTruncate", "NDK_FsExist", "NDK_FsFileSize",  "NDK_FsGetDiskSpace", "NDK_FsDir", "NDK_FsClearAppFile"};
#elif ANDIROD_ENABLE
	char *fsstr[] = {"NDK_FsOpen", "NDK_FsClose", "NDK_FsSeek", "NDK_FsWrite", "NDK_FsDel", "NDK_FsRead", "NDK_FsTell", "NDK_FsRename", "NDK_FsTruncate", "NDK_FsExist", "NDK_FsFileSize", "NDK_CopyFileToSecMod"};
#else
	char *fsstr[] = {"NDK_FsOpen", "NDK_FsClose", "NDK_FsSeek", "NDK_FsWrite", "NDK_FsDel", "NDK_FsRead", "NDK_FsTell", "NDK_FsRename", "NDK_FsTruncate", "NDK_FsExist", "NDK_FsFileSize", "NDK_FsGetDiskSpace", "NDK_FsDir", "NDK_FsCreateDirectory", "NDK_FsRemoveDirectory"};
#endif

	/*process body*/
	len = (MaxNum>20)?MaxNum:20;
	while(cnt)
	{	
		cnt--;
		bak++;
		
		//产生随机数
		cls_printf("第%d次随机顺序:",bak);
		srand((unsigned)time(NULL));
		for(i=0;i<len;i++)
		{	
			num[i] = rand()%MaxNum;
			NDK_ScrPrintf("%d ", num[i]);
			NDK_ScrRefresh();
		}

		//进行随机测试
		for(i=0;i<len;i++)
		{		
			if((ret = fs_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",fsstr[num[j]]);
				return;
			}
		}
	}
	//测试后置 删除文件
	NDK_FsDel(RANDFILE);
	NDK_FsDel(DESTFILE);
#if !K21_ENABLE
	NDK_FsRemoveDirectory(TESTDIR);
#endif
	//test over
	send_result("%s测试通过", TESTAPI);
	return;
}



