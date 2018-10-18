/************************************************************************
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: sdusb通讯模块
* file name			: 
* Author 			: liny
* version			: 
* DATE				: 20171207
* directory 			: 
* description			: 测试U盘SD卡模块随机组合情况
* related document	:  NDK.chm
*
************************************************************************/

/*-----------------------includes-------------------------------*/
#include "mode.h"


/*---------------constants/macro definition---------------------*/

#define	TESTAPI	"U盘SD卡模块内随机组合"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef enum {
  	DiskOpen,
	DiskClose,
	DiskGetState,	
	DiskGetRootDirName,
	DiskGetInfo,
	MaxNum,
}EM_SDUSB;
/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 		: sdusb 通讯模块随机组合函数
* functional description 	: 测试sdusb 通讯模块随机组合情况
* input parameter	 		:
* output parameter	 	: 
* return value		 		:
* history 		 			: author		date			remarks
*							liny				20171207		created
*****************************************************************/
static int sdusb_randtest(int funnum)
{ 
    	int disktype = get_disktype();
	int diskstate = 0;
	char rootdir[][16] = {{0}};
	ST_DISK_INFO pstInfo;
   	 switch(funnum)
	{	
	    case DiskOpen:
			NDK_DiskOpen(disktype,0);
			break;
		case DiskClose:
			NDK_DiskClose(disktype);
			break;
		case DiskGetState:
	        	NDK_DiskGetState(disktype,&diskstate);
			break;
		case DiskGetRootDirName:
			NDK_DiskGetRootDirName(disktype,(char **)rootdir);
			break;
		case DiskGetInfo:
			NDK_DiskGetInfo(rootdir[0],&pstInfo);
			break;
    }
    return SUCC;
}

void sdusb200(void)
{
	/*private & local definition*/
	int num[50] ={0}, i = 0, bak = 0;
	int ret = -1,cnt =50, len = 0,j=0;//cnt 待定
	int disktype = get_disktype();
	char *sdusbstr[] = {"NDK_DiskOpen","NDK_DiskClose","NDK_DiskGetState","NDK_DiskGetRootDirName","NDK_DiskGetInfo"};	

	/*process body*/
	len = (MaxNum>20)?MaxNum:20;
	while(cnt)
	{	
		cnt--;
		bak++;

		//产生随机数
		srand((unsigned)time(NULL));
		for(i=0;i<len;i++)
		{	
			num[i] = rand()%MaxNum;
		}
		cls_printf("第%d次随机顺序:",bak);
		for(i=0;i<len;i++)
		{
			NDK_ScrPrintf("%d ", num[i]);
			NDK_ScrRefresh();
		}
			
		//进行随机测试
		for(i=0;i<len;i++)
		{
			if((ret = sdusb_randtest(num[i])) != NDK_OK)
			{
				send_result("line %d:%s第%d次测试失败(%d,%d)", __LINE__, TESTAPI, bak,ret,num[i]);
				for(j=0;j<=i;j++)
					send_result("%s ",sdusbstr[num[j]]);
			}
		}	
		
	}
	//测试后置
	NDK_DiskClose(disktype);
	send_result("%s测试通过", TESTAPI);
	return;
}


