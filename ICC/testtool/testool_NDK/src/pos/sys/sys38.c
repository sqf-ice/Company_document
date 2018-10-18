/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module			: sys模块
* file name			: sys38.c
* Author 			: 
* version			: 
* DATE				: 
* directory 		: 
* description		: 本例测试NDK_SysUpdateBootLogo接口(海外产品支持)
* related document	: NDK.chm
*
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

#define   ERR_SIZE   "errlogo1.bmp"  //240*320大小的bmp图片,位深度24
#define   TESTJPG    "errlogo2.jpg"  //320*240大小的jpg图片,位深度24
#define   TESTPNG    "errlogo3.png"  //320*240大小的png图片,位深度24
#define   ERR_DEPTH  "errlogo4.bmp"  //320*240大小的bmp图片,位深度8
#define   NOT_EXIST  "nonelogo.bmp"
#define   TESTLOGO1  "testlogo1.bmp"
#define   TESTLOGO2  "testlogo2.bmp"
#define   NLLOGO     "newland.bmp"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_SysUpdateBootLogo"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 	: 
* functional description: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author		date		remarks
*			  	  	       sull       20180109      created 
*****************************************************************/
void sys38(void)
{
	//以下为局部变量
	int ret = -1;

	//以下为函数体
	cls_printf("测试%s...", TESTAPI);
	if(cls_show_msg1(5,"请确保已在当前路径下下载所有测试图片,[Enter]继续,其他返回" )!=ENTER && auto_flag!=1)
	{
		send_result("line %d:%s测试中断", __LINE__, TESTAPI);
		return;
	}

	//case1：参数非法
	if((ret = NDK_SysUpdateBootLogo(NULL)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case2：传入一张不存在的图片，返回NDK_ERR
	if((ret = NDK_SysUpdateBootLogo(NOT_EXIST)) != NDK_ERR)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case3:传入各种图片不为320*240且位深度为16或24的bmp图片，返回错误
	if((ret = NDK_SysUpdateBootLogo(ERR_SIZE)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d,%s)", __LINE__, TESTAPI, ret,ERR_SIZE);
		GOTOERR;
	}
	if((ret = NDK_SysUpdateBootLogo(TESTJPG)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SysUpdateBootLogo(TESTPNG)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if((ret = NDK_SysUpdateBootLogo(ERR_DEPTH)) != NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	//case4：新logo图片与原有图片一样，返回NDK_ALREADY_DONE
	//只要一调用NDK_SysUpdateBootLogo成功，flash里就会立即更换图片，不需要重启即可测试该用例
	if((ret = NDK_SysUpdateBootLogo(TESTLOGO1)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if((ret = NDK_SysUpdateBootLogo(TESTLOGO1)) != NDK_ALREADY_DONE)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if(auto_flag == 0 || auto_flag == 2)
	{
		if((cls_show_msg1(5,"第一次进入该案例?\n是[ENTER]重启，否则其它\n重启时观察开机logo是否更换为%s.恢复默认logo需再次进入此案例.",TESTLOGO2)) == ENTER)
		{
			//case5：正常更新logo，重启观察是否生效
			if((ret = NDK_SysUpdateBootLogo(TESTLOGO2)) != NDK_OK)
			{
		 		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
				goto ERR;
			}
			NDK_SysReboot();
		}
		send_result("%s测试通过", TESTAPI);
	}
	else	
		send_result("line %d:%s部分子用例需手动测试", __LINE__, TESTAPI);

ERR:
	//测试后置:换为原来的新大陆logo
	NDK_SysUpdateBootLogo(NLLOGO);
	return;
}


