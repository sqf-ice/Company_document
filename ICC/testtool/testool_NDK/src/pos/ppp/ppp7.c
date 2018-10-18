/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: pppģ��
* file name		: ppp7.c
* Author 		: zhengry	
* version		: 
* DATE			: 20160512
* directory 		: 
* description		: ����NDK_PppGetNetAddr��ȡ����IP����
* related document	: NDKapi.chm
*
************************************************************************
* log			: 
* Revision 
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define TESTAPI	"NDK_PppGetNetAddr"	
#define MAXTIMEOUT 60

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/
extern int IsPPPTimeout(int state, int *err);

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 				: void ppp7(void)
* functional description 	: ��ȡ���ص�ַ
* input parameter	 				:��
* output parameter	 			:��
* return value		 				:��
* history 		 						:  author		date		remarks
*			  	  	  				     zhengry     20160512	 created
*****************************************************************/
void ppp7(void)
{
	//����Ϊ�ֲ�����
	int nRet;
	EM_PPP_STATUS nStatus=PPP_STATUS_DISCONNECT;
	char LocalIPAddr[17]={0};
	int tpstatus=0;
	int times=0;
	
	//����Ϊ������
	cls_printf("����%s...", TESTAPI); 

	//����ǰ��:
	if((getlinktype()!=ASYN&&LinkUp()!=SUCC)
	||(getlinktype()==ASYN&&LinkUp4AsynPPP()!=SUCC))//if(LinkUp()!=SUCC)
	{
		send_result("line %d:������·ʧ��", __LINE__);
		goto ppp_err;
	}
	if((nRet = PppSetUsrCfg())!=SUCC)
	{
		send_result("line %d:%s����ʧ��(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;	
	}
	
	//case1:δ����ȡ���ص�ַӦ���ؿջ��ߺ������÷���ʧ�� 
	if((nRet=NDK_PppGetNetAddr(LocalIPAddr))==NDK_OK  || strcmp(LocalIPAddr ,"" ))
	{
		send_result("line %d:%s����ʧ��(%d,%s)", __LINE__, TESTAPI, nRet, LocalIPAddr);
		goto ppp_err;
	}

	if(getlinktype()==ASYN&&(nRet=WM_PPP_16300_DIAL())<0)
	{
		send_result("line %d:%s����ʧ��(%d)", __LINE__, TESTAPI, nRet);
		goto ppp_err;
	}

	while (1)
	{
		NDK_SysDelay(10);//����ʱ��Ϊ�˽���������״̬��оƬ��ѹ��
		if((tpstatus = NDK_PppCheck(&nStatus,NULL))!=NDK_OK)
		{
	    		send_result("line %d:%s����ʧ��(%d)", __LINE__, TESTAPI,tpstatus);
	    		goto ppp_err;
		}		
		
		switch (nStatus)
		{   
		case PPP_STATUS_DISCONNECT ://ppp�ر�                                                                           	
			cls_show_msg1(2, "PPPδ����\n");                                                                                                	
			if(++times==3)
			{
				send_result("line %d:%s����ʧ��", __LINE__, TESTAPI);
				goto ppp_err;                                                                                                          	
			}
			if ((nRet=NDK_PppDial(PPPUSR, PPPPWD))!=NDK_OK) 
			{
			    send_result("line %d:%s����ʧ��(%d)", __LINE__, TESTAPI, nRet);
			    goto ppp_err;
			}                                                                                                             	
			break;
		case PPP_STATUS_CONNECTING: //PPP��������                                                                 	                                     	
			cls_show_msg1(2, "PPP����������...\n");
			if(IsPPPTimeout(PPP_STATUS_CONNECTING, NULL))
			{
				send_result("line %d:%s����ʧ��", __LINE__, TESTAPI);
				goto ppp_err; 			
			}
			break;                                                                                      	                                     	                                                                          	
		case PPP_STATUS_CONNECTED://PPP������
			cls_show_msg1(2, "PPP�ѽ�������,��ȡIP��...\n");                                                                                                    	
			//��һ�������붯̬IP�Ƿ�ɹ�(����һ��ȡ����,��ȡ����)         
			if(IsPPPTimeout(PPP_STATUS_CONNECTED,NULL))
			{
				send_result("line %d:%s����ʧ��", __LINE__, TESTAPI);
				goto ppp_err;                                                                                                             	
			}

			//case2:���Բ���Ϊ�յ�ʱ��Ӧ����ʧ��
			if((nRet = NDK_PppGetNetAddr(NULL) )!= NDK_ERR_PARA)
			{
				send_result("line %d:%s����ʧ��(%d)", __LINE__, TESTAPI, nRet);
				goto ppp_err;
			}			

			//case3:������ȡ���ص�ַ
			if (((nRet = NDK_PppGetNetAddr(LocalIPAddr))!=NDK_OK) || !strcmp(LocalIPAddr ,"" ))
			{
				send_result("line %d:%s����ʧ��(%d,%s)", __LINE__, TESTAPI, nRet, LocalIPAddr);
				goto ppp_err;
			}
			cls_show_msg1(2,"��ȡ���ı���IP��ַΪ:%s",LocalIPAddr);
			//�ɹ��Ļ�,�ر�PPP,��ʼ��һ�ֵĲ���
			if((nRet=NDK_PppHangup(0))!=SUCC)//ʹ�÷�����ʽ�Ҷ�                                                                                 	
			{
				send_result("line %d:%s����ʧ��(%d)", __LINE__, TESTAPI, nRet);
				goto ppp_err;                                                                                                         	
			}

			//case4:�ҶϺ�,Ӧȡ������ַ
			if((nRet=NDK_PppGetNetAddr(LocalIPAddr))==NDK_OK)
			{
				send_result("line %d:%s����ʧ��(%d)", __LINE__, TESTAPI, nRet);
				goto ppp_err;
			}

			//test over
			send_result("%s����ͨ��", TESTAPI);	                                    		                                                            	
			goto ppp_err;   
			break;			                                                            	
		case PPP_STATUS_DISCONNECTING://ppp���ڶϿ�                                                                                                       	
			cls_show_msg1(2, "ppp���ڶϿ�...");                                                                                                        	
			break;                                                                                                                           	
		default://δ֪״̬����Ϊ��		
			send_result("line %d:%s����ʧ��(%d)", __LINE__, TESTAPI, nStatus);
			goto ppp_err;
			break;
		}	
	}
	
ppp_err:
	NDK_PppHangup(0);  //��֧�ַ������Ҷ�
	LinkDown();
	return;
}

