/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: systest
* file name			: systest67.c
* Author 			: 
* version			: 
* DATE				: 
* directory 			: 
* description			: 多线程类
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
#include <pthread.h>
#include "libconfig.h"

#define  MAXWAITTIME  200
#if defined SP630PG || defined SP830PG||defined SP930P
#define  FILEPATH "/app/data"
#else
#define  FILEPATH "/appfs/etc/sys.conf"
#endif
/*---------------constants/macro definition---------------------*/
#define	TESTITEM		"多线程类"
typedef struct __my_thread
{
	char *thread_name;
	int thread_flag;
	int thread_exit;
}MYPTHREAD;

#define PTHREAD_NUM 6
int g_thread_exit = 0;
/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:
* output parameter	 	: 
* return value		 	:
* history 		 		: author			date			remarks
*			  			jiangym 			20140619     created
*****************************************************************/
//kbhit线程 获取键值,按键为1、 9,键值应该为0x31与0x39,如果不是这两者报错
static void *pthread_hit(void *arg)
{
	int nKeyin = 0;
	//g_thread_exit 置1时 线程退出
	while(g_thread_exit == 0)
	{	
		NDK_KbHit(&nKeyin);
		if(nKeyin!=0)
		{
			//为了确认确实有线程在跑
			fprintf(stderr,"line:[%d]thread_hit is runing\n",__LINE__);
			if(nKeyin!=0x31 && nKeyin!=0x39)
			{
				//控制台看出错信息
				fprintf(stderr,"line:%d 获取键值失败(%02x)\n",__LINE__,nKeyin);
				//pos端提示出错信息
			 	cls_show_msg1(2,"line:%d 获取键值失败(%02x)",__LINE__,nKeyin);
				sleep(3);
				g_thread_exit=1;
			}	
		}
	}
	//退出线程
	return (void *)0;
}
//NDK_KbGetCode和NDK_KbGetPad很难复现出
//线程 获取键值,按键为1、 9,键值应该为0x31与0x39,如果不是这两者报错
static void *pthread_getcode(void *arg)
{
	int nKeyin = 0;
	//g_thread_exit 置1时 线程退出
	while(g_thread_exit == 0)
	{	
		NDK_KbGetCode(1, &nKeyin);
		if(nKeyin!=0)
		{
			fprintf(stderr,"line:[%d]pthread_getcode is runing\n",__LINE__);
			if(nKeyin!=0x31 && nKeyin!=0x39)
			{
				fprintf(stderr,"line:%d 获取键值失败(%02x)\n",__LINE__,nKeyin);
			 	cls_show_msg1(2,"line:%d 获取键值失败(%02x)",__LINE__,nKeyin);
				sleep(3);
				g_thread_exit=1;
			}	
		}
	}
	//退出线程
	return (void *)0;
}
//线程 获取键值,按键为1、 9,键值应该为0x31与0x39,如果不是这两者报错
static void *pthread_getpad(void *arg)
{
	ST_PADDATA pad_data;
	
	//g_thread_exit 置1时 线程退出
	while(g_thread_exit == 0)
	{	
		NDK_KbGetPad(&pad_data,1);
		if(pad_data.emPadState== PADSTATE_KEY)
		{
			if(pad_data.unKeycode!=0x31 && pad_data.unKeycode!=0x39)
			{
				fprintf(stderr,"line:%d 获取键值失败(%02x)\n",__LINE__,pad_data.unKeycode);
				cls_show_msg1(2,"line:%d 获取键值失败(%02x)",__LINE__,pad_data.unKeycode);
				sleep(3);
				g_thread_exit=1;
			}
		}
	}
	//退出线程
	return (void *)0;
}

//创建6个kbhit线程,pos按键1和9,线程收到后会返回键值,键值不是预期值时出错
//g_thread_exit为全局变量,被置1时全部线程退出,同时主进程也退出
static void kbhit_pthread(void)
{
	/*private & local definition*/	
	pthread_t tidp[PTHREAD_NUM];
	int error=-1, i=0;
	time_t oldtime = 0;
	g_thread_exit = 0;
	
	/*process body*/
	cls_show_msg("测试键盘NDK_KbHit多线程,请快速连续输入1和9,任意键继续...");
	//要使用pthread_join来等待子线程结束,需要在非分离属性下,pthread_create第二个参数为NULL时是默认属性,默认属性中包含非分离属性
	//创建6个子线程
	for(i=0;i<PTHREAD_NUM;i++)
	{
		if((error=NDK_PthreadCreate(&tidp[i],NULL,(void *)pthread_hit,NULL))!=0) 
		{
			cls_show_msg1(g_keeptime,"line %d:NDK_KbHit NDK_PthreadCreate fail(%d)\n", __LINE__, error);
			return;
		}
	}
	oldtime=time(NULL);
	while(1)
	{
		if(g_thread_exit==1)	
			break;
		//超时时间到,g_thread_exit置1,线程全部结束,主进程也结束
		if((time(NULL)-oldtime)>MAXWAITTIME)
		{
			g_thread_exit=1;
			cls_show_msg("测试通过,任意键继续");
			break;
		}
	}
	//等待子线程结束
	for(i=0;i<PTHREAD_NUM;i++)
	{
		 NDK_PthreadJoin(tidp[i],NULL);
	}
	return;
}

//NDK_KbGetCode和NDK_KbGetPad很难复现出
//创建6个kbgetcode线程,pos端输入1和9,线程收到后返回键值
//g_thread_exit为全局变量,被置1时全部线程退出,同时主进程也退出
static void kbgetcode_pthread(void)
{
	/*private & local definition*/
	pthread_t tidp[PTHREAD_NUM];
	int error=-1, i=0;
	time_t oldtime = 0;
	g_thread_exit = 0;
	
	/*process body*/
	cls_show_msg("测试键盘NDK_KbGetCode多线程,请快速连续输入1和9,任意键继续...");
	//要使用pthread_join来等待子线程结束,需要在非分离属性下,pthread_create第二个参数为NULL时是默认属性,默认属性中包含非分离属性	
	//创建6个子线程
	for(i=0;i<PTHREAD_NUM;i++)
	{
		if((error=NDK_PthreadCreate(&tidp[i],NULL,(void *)pthread_getcode,NULL))!=0) 
		{
			cls_show_msg1(g_keeptime,"line %d:NDK_KbGetCode NDK_PthreadCreate fail(%d)\n", __LINE__, error);
			return;
		}
	}	
	oldtime=time(NULL);
	while(1)
	{
		if(g_thread_exit==1)	
			break;
		//超时时间到,g_thread_exit置1,线程全部结束,主进程也结束
		if((time(NULL)-oldtime)>MAXWAITTIME)
		{
			g_thread_exit=1;
			cls_show_msg("测试通过,任意键继续");
			break;	
		}
	}
	//等待子线程结束
	for(i=0;i<PTHREAD_NUM;i++)
	{
		 NDK_PthreadJoin(tidp[i],NULL);
	}
	return;
}
//创建6个kbgetpad线程,pos端输入1和9,线程收到后返回键值
//g_thread_exit为全局变量,被置1时全部线程退出,同时主进程也退出
static void kbgetpad_pthread(void)
{
	/*private & local definition*/
	pthread_t tidp[PTHREAD_NUM];
	int error=-1, i=0;
	time_t oldtime = 0;
	g_thread_exit = 0;
	
	/*process body*/
	cls_show_msg("测试键盘NDK_KbGetPad多线程,请快速连续输入1和9,任意键继续...");
	//要使用pthread_join来等待子线程结束,需要在非分离属性下,pthread_create第二个参数为NULL时是默认属性,默认属性中包含非分离属性
	//创建6个子线程
	for(i=0;i<PTHREAD_NUM;i++)
	{
		if((error=NDK_PthreadCreate(&tidp[i],NULL,(void *)pthread_getpad,NULL))!=0) 
		{
			cls_show_msg1(g_keeptime,"line %d:NDK_KbGetPad NDK_PthreadCreate fail(%d)\n", __LINE__, error);
			return;
		}
	}	
	oldtime=time(NULL);
	while(1)
	{
		if(g_thread_exit==1)
			break;
		//超时时间到,g_thread_exit置1,线程全部结束,主进程也结束	
		if((time(NULL)-oldtime)>MAXWAITTIME)
		{
			g_thread_exit=1;
			cls_show_msg("测试通过,任意键继续");
			break;	
		}
	}
	//等待子线程结束
	for(i=0;i<PTHREAD_NUM;i++)
	{
		 NDK_PthreadJoin(tidp[i],NULL);
	}
	return;
}


//调用底层函数进行配置文件的读操作
void *pthread_configread(void *arg)
{
	int tmp_language = -1;
	int i= 0,j=0,s = 0;
	MYPTHREAD *tmpphread = NULL;
	tmpphread=(MYPTHREAD *)arg;
	int old_value=0;
	config_t tmp_cfg;
	config_setting_t *dev_class;	
	config_setting_t *tmp_root_setting;
	
	//thread_exit置1时,线程退出	
	while(tmpphread->thread_exit==0)
	{
		tmp_language = -1;
		config_init(&tmp_cfg);
		if (config_read_file(&tmp_cfg, FILEPATH) == CONFIG_FALSE)
		{
			fprintf(stderr,"[%d] %s errs=[%s]\n",__LINE__,tmpphread->thread_name,config_error_text(&tmp_cfg));
			tmpphread->thread_flag=1;
			break;
		} 
		tmp_root_setting = config_root_setting(&tmp_cfg);
		if (tmp_root_setting == NULL)
		{
			fprintf(stderr,"[%d] %s errs=[%s]\n",__LINE__,tmpphread->thread_name,config_error_text(&tmp_cfg));
			tmpphread->thread_flag=1;
    		break;
		}
		//查询配置文件,查到内容为NULL时报错
		dev_class = config_lookup(&tmp_cfg, "sys");	   		
		if(dev_class==NULL){
			s++;
			cls_show_msg("line %d:%s文件内容为NULL,任意键继续",__LINE__,tmpphread->thread_name);
			tmpphread->thread_flag=1;
			break;   			
		}
		if (config_setting_lookup_int(dev_class, "language", (int *)&tmp_language) != CONFIG_TRUE)
		{
			fprintf(stderr,"[%d] %s errs=[%s]\n",__LINE__,tmpphread->thread_name,config_error_text(&tmp_cfg));
			tmpphread->thread_flag=1;
			break;
		}
		old_value=tmp_language;
		NDK_SysMsDelay(10);
		i++;
		if(i%100==0)
			fprintf(stderr,"thread_name=[%s]time:[%ds]\n",tmpphread->thread_name,(j++));
		if(old_value!=tmp_language)
		{
	    	fprintf(stderr,"thread_name=[%s]time:[%ds]value=[%d]\n",tmpphread->thread_name,(j),tmp_language);
	    	old_value=tmp_language;
		}
		config_destroy(&tmp_cfg);
	}
	config_destroy(&tmp_cfg);
	//退出线程
	return (void *)0;
}
//调用底层函数进行配置文件的写操作
void *pthread_configwrite(void *arg)
{
	int language_int = 0;
	MYPTHREAD *tmpphread = NULL;
	tmpphread=(MYPTHREAD *)arg;
	config_t tmp_cfg;
	config_setting_t *dev_class;
	config_setting_t *tmp_root_setting;
	
	//thread_exit置1时,线程退出
	while(tmpphread->thread_exit==0)
	{
		config_init(&tmp_cfg);
		if (config_read_file(&tmp_cfg, FILEPATH) == CONFIG_FALSE) {
			fprintf(stderr,"[%d] %s errs=[%s]\n",__LINE__,tmpphread->thread_name,config_error_text(&tmp_cfg));
			tmpphread->thread_flag=1;
			break;
		}
		tmp_root_setting = config_root_setting(&tmp_cfg);
	    if (tmp_root_setting == NULL) {
			fprintf(stderr,"[%d] %s errs=[%s]\n",__LINE__,tmpphread->thread_name,config_error_text(&tmp_cfg));
			tmpphread->thread_flag=1;
			break;
	    }
	    dev_class = config_lookup(&tmp_cfg, "sys");
	    if(dev_class==NULL){
			cls_show_msg("line %d:%s文件内容为NULL,任意键继续",__LINE__,tmpphread->thread_name);
			tmpphread->thread_flag=1;
			break;
	    }
	    if(dev_class) {
	        dev_class = config_setting_get_member(dev_class, "language");
	        if (dev_class){
	        	if (config_setting_set_int(dev_class, language_int) != CONFIG_TRUE) {
	    			fprintf(stderr,"[%d] %s errs=[%s]\n",__LINE__,tmpphread->thread_name,config_error_text(&tmp_cfg));
	    			tmpphread->thread_flag=1;
	    			break;
	             }
	        }
	        else{
				fprintf(stderr,"[%d] %s errs=[%s]\n",__LINE__,tmpphread->thread_name,config_error_text(&tmp_cfg));
				tmpphread->thread_flag=1;
				break;
	        }
	    }
	    if (! config_write_file(&tmp_cfg, FILEPATH)) {
			fprintf(stderr,"[%d] %s errs=[%s]\n",__LINE__,tmpphread->thread_name,config_error_text(&tmp_cfg));
			tmpphread->thread_flag=1;
			break;
	    }
		fprintf(stderr,"[%d] %s write value=%d\n",__LINE__,tmpphread->thread_name,language_int);
	    config_destroy(&tmp_cfg);
		NDK_SysMsDelay(10);
	}
	config_destroy(&tmp_cfg);
	//退出线程
	return (void *)0;
}
//调用NDK函数实现配置文件的写操作
/*void *pthcreate_wr(void *arg)
{
	int ret = -1, i = 0;
	MYPTHREAD *tmpphread = NULL;
	tmpphread=(MYPTHREAD *)arg;

	while(1)
	{
		if(tmpphread->thread_flag)
		{
			NDK_ScrSetContrast(32);
			break;	
		}
		if((ret=NDK_ScrSetContrast((i++)%63))!=NDK_OK)
		{
			cls_show_msg("line %d:设置对比度失败(%d,%d)", __LINE__, ret, i);
			break;
		}		
		NDK_SysMsDelay(50);
	}
	
	//退出线程	 
	pthread_exit(NULL);	
	
}*/
//读写线程同时进行
void libconfigrw_test()
{	
	/*private & local definition*/
	pthread_t tidp,tidp1;
    int error=-1, error1=-1;
	pthread_attr_t attr;
	MYPTHREAD *myphreadone,*myphreadtwo;
	time_t oldtime;
	
	/*process body*/
	cls_printf("测试配置文件多线程同时读写是否冲突");	
	//分配动态内存
    myphreadone=(MYPTHREAD *)malloc(sizeof(MYPTHREAD));
	myphreadtwo=(MYPTHREAD *)malloc(sizeof(MYPTHREAD));
	//预防内存泄漏
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	//初始化线程的输入参数创建读线程
	myphreadone->thread_name="Pthread read";
	myphreadone->thread_flag=0;
	myphreadone->thread_exit=0;
	if((error=NDK_PthreadCreate(&tidp,&attr,pthread_configread,(void *)myphreadone))!=0)
	{
		cls_show_msg1(g_keeptime,"line %d:创建%s线程失败(%d)", __LINE__,myphreadone->thread_name, error);
		return;
	}
	//初始化线程的输入参数 创建写线程
	myphreadtwo->thread_name="Pthread write";
	myphreadtwo->thread_flag=0;
	myphreadtwo->thread_exit=0;
	if((error1=NDK_PthreadCreate(&tidp1,&attr,pthread_configwrite,(void *)myphreadtwo))!=0)
	{
		cls_show_msg1(g_keeptime,"line %d:创建%s线程失败(%d)", __LINE__, myphreadtwo->thread_name, error1);
		return;
	}
	oldtime=time(NULL);
	//当某个线程中的thread_flag置1时,另一个线程的thread_exit置1 , 退出线程,同时主线程也退出
	while(1)
	{
		if(myphreadone->thread_flag==1||myphreadtwo->thread_flag==1)
		{
			//此处可以判断是哪个线程出错,然后把另一个线程的退出标志位置1,同时等到另一个线程结束后主进程结束
			if(myphreadone->thread_flag==1)
			{
				myphreadtwo->thread_exit=1;
				cls_show_msg1(g_keeptime,"line %d:%s失败,任意键继续",__LINE__,myphreadone->thread_name);
				NDK_PthreadJoin(tidp1,NULL);//等待另一个线程结束
				break;
			}
			else if(myphreadtwo->thread_flag==1)
			{
				myphreadone->thread_exit=1;
				cls_show_msg1(g_keeptime,"line %d:%s失败,任意键继续",__LINE__,myphreadtwo->thread_name);
				NDK_PthreadJoin(tidp,NULL);
				break;
			}
		}
		//超时置线程退出标志位为1,同时主进程也退出
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			myphreadone->thread_exit=1;
			myphreadtwo->thread_exit=1;
			break;
		}
	}
	//释放内存
	cls_show_msg_record(FILENAME ,FUNCNAME ,"读写线程同时进行测试通过");
	free(myphreadone);
	free(myphreadtwo);
	return;
}

//写写线程同时进行
void libconfigww_test()
{
	pthread_t tidp,tidp1;
	int error=-1,error1=-1;
	pthread_attr_t attr;
	MYPTHREAD *myphreadone,*myphreadtwo;
	time_t oldtime;
	
	cls_printf("测试配置文件多线程同时写写是否冲突");
	//动态分配内存
	myphreadone=(MYPTHREAD *)malloc(sizeof(MYPTHREAD));
	myphreadtwo=(MYPTHREAD *)malloc(sizeof(MYPTHREAD));
	//防止内存泄漏
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	//初始化线程的输入参数 创建第一个写线程
	myphreadone->thread_name="Pthread write 1";
	myphreadone->thread_flag=0;
	myphreadone->thread_exit=0;
	if((error=NDK_PthreadCreate(&tidp,&attr,pthread_configwrite,(void *)myphreadone))!=0)
	{
		cls_show_msg1(g_keeptime,"line %d:创建线程失败(%d)", __LINE__, error);
		return;
	}
	//初始化线程的输入参数,创建第二个写线程
	myphreadtwo->thread_name="Pthread write 2";
	myphreadtwo->thread_flag=0;
	myphreadtwo->thread_exit=0;
	if((error1=NDK_PthreadCreate(&tidp1,&attr,pthread_configwrite,(void *)myphreadtwo))!=0)
	{
		cls_show_msg1(g_keeptime,"line %d:创建线程失败(%d)", __LINE__, error1);
		return;
	}
	oldtime=time(NULL);
	//当某个线程中的thread_flag置1时,另一个线程的thread_exit置1 , 退出线程,同时主线程也退出
	while(1)
	{
		if(myphreadone->thread_flag==1||myphreadtwo->thread_flag==1)
		{
			//此处可以判断是哪个线程出错,然后把另一个线程的退出标志位置1,同时等到另一个线程结束后主进程结束			
			if(myphreadone->thread_flag==1)
			{
				myphreadtwo->thread_exit=1;
				cls_show_msg1(g_keeptime,"line %d:%s失败,任意键继续",__LINE__,myphreadone->thread_name);				
				NDK_PthreadJoin(tidp1,NULL);//等待另一个线程结束
				break;
			}
			else if(myphreadtwo->thread_flag==1)
			{
				myphreadone->thread_exit=1;
				cls_show_msg1(g_keeptime,"line %d:%s失败,任意键继续",__LINE__,myphreadtwo->thread_name);
				NDK_PthreadJoin(tidp,NULL);
				break;
			}
		}
		//超时置线程退出标志位为1,同时主进程也退出
		if(time(NULL)-oldtime>MAXWAITTIME)
		{
			myphreadone->thread_exit=1;
			myphreadtwo->thread_exit=1;
			break;
		}
	}
	//释放内存
	cls_show_msg_record(FILENAME ,FUNCNAME ,"写写线程同时进行测试通过");
	free(myphreadone);
	free(myphreadtwo);
	return;
}

//键盘类函数的三个函数的多线程测试的配置选择
void kb_pthread(void)
{
	int keyin = 0;
	while (1)
	{	
		keyin = cls_show_msg("1.KbHit多线程\n"
							 "2.KbGetCode多线程\n"
							 "3.KbGetPad多线程");
		switch(keyin)
		{
		case '1':
			kbhit_pthread();
			break;
		case '2':
			kbgetcode_pthread();
			break;
		case '3':
			kbgetpad_pthread();
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
//配置文件多线程的选择,文件的同时读写与同时写写
void configfile_pthread(void)
{
	int keyin = '0';
	while (1)
	{	
		if(g_SequencePressFlag)  //判断统一模块内是否连续进行压力测试
		{
			//对应下面的菜单数+1，菜单增加此处也要对应的增加
			if((++keyin == '3')||(cls_show_msg1(2, "即将进行配置文件多线程测试,ESC退出")==ESC))
				return ;
		}
		else
		{
			keyin = cls_show_msg("1.文件同时读写\n"
								 "2.文件同时写写");
		}
		switch(keyin)
		{
		case '1':
			libconfigrw_test();
			break;
		case '2':
			libconfigww_test();
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
void systest66(void)
{
	/*private & local definition*/
	int keyin = 0;
	
	/*process body*/
	cls_show_msg1(1, "%s测试中...", TESTITEM);
	
	if(auto_flag==1)
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试开始",TESTITEM);
		//执行配置文件多线程
		configfile_pthread();

		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s测试结束\n",TESTITEM);
		return;
	}
	while (1)
	{	
		keyin = cls_show_msg("1.键盘函数多线程\n"
							 "2.配置文件多线程");

		switch(keyin)
		{
		case '1':
			kb_pthread();
			break;
		case '2':
			configfile_pthread();
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

