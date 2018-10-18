/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: spfs模块
* file name			: 
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: spfs模块内随机组合情况
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define DESTFILE "/appfs/test"
#define TESTDIR     "./test1"
#define  RANDFILE	"/appfs/randfile"	//绝对路径 

#define	TESTAPI	"spfs模块内随机组合"

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
	CopyFileToSecMod,

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
*						linying				20180829	created
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
			fp = NDK_SP_FsOpen(RANDFILE, "w");
			NDK_SP_FsClose(fp); //增加关闭句柄,为防止退出后测试其他模块由于打开句柄数太多导致没有句柄数的问题20180801 modify
			break;
		case FsClose:
			NDK_SP_FsClose(fp);
			break;
		case FsSeek:
			NDK_SP_FsSeek(fp, 0l, SEEK_SET); 
			break;
		case FsWrite:
			NDK_SP_FsWrite(fp, writebuf, 200); 
			break;
		case FsDel:
			NDK_SP_FsDel(RANDFILE);
			break;
		case FsRead:
			NDK_SP_FsRead(fp, readbuf, 200); 
			break;
		case FsTell:
			NDK_SP_FsTell(fp, &pulRet); 
			break;
		case FsRename:
			NDK_SP_FsRename(RANDFILE,RANDFILE); 
			break;
		case FsTruncate:
			NDK_SP_FsTruncate(RANDFILE, 200); 
			break;
		case FsExist:
			NDK_SP_FsExist(RANDFILE);
			break;
		case FsFileSize:
			NDK_SP_FsFileSize(RANDFILE,&punSize);
			break;
		case CopyFileToSecMod:
			NDK_CopyFileToSecMod(RANDFILE, DESTFILE);
		break;

	}
	return SUCC;
ERR:
	NDK_SP_FsDel(RANDFILE);
	NDK_SP_FsDel(DESTFILE);
	return ret;	
}

void spfs200(void)
{
	/*private & local definition*/
	int  i = 0, bak = 0, num[20] ={0}, j = 0;
	int ret = -1,cnt =50, len = 0;//cnt 待定
	char *fsstr[] = {"NDK_SP_FsOpen", "NDK_SP_FsClose", "NDK_SP_FsSeek", "NDK_SP_FsWrite", "NDK_SP_FsDel", "NDK_SP__FsRead", "NDK_SP_FsTell", "NDK_SP_FsRename", "NDK_SP_FsTruncate", "NDK_SP_FsExist", "NDK_SP_FsFileSize", "NDK_CopyFileToSecMod"};

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
	NDK_SP_FsDel(RANDFILE);
	NDK_SP_FsDel(DESTFILE);
	//test over
	send_result("%s测试通过", TESTAPI);
	return;
}



