/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec7.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecKeyErase接口功能能删除所有密钥，NDK_SecKeyDelete函数能删除指定的密钥类型的密钥
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
#define TESTAPI	"NDK_SecKeyErase,NDK_SecKeyDelete"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/
typedef struct key_kcv {
	uchar *pkey;
	uchar *pkcv;
} KEY_KCV;

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/

/****************************************************************
* function name 	 	: 
* functional description 	: 
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author		date		remarks
*			  	  	   linrq       20130228	created
*						linwl 		20140813 modified	
*****************************************************************/
void sec9(void)
{
	/*private & local definition*/
	int ret = 0, i = 0;
	ST_SEC_KEY_INFO stKeyInfo;
	ST_SEC_KCV_INFO stKcvInfoIn;
	KEY_KCV SecLoad[ ]={
		//前6组为DES算法密钥
		{(uchar *)"\x13", 																NULL}, 					// 0	//TLK (key=16个0x13, TLK,ID=1)
		{(uchar *)"\xE1\x46\x47\xE8\xA1\x35\x06\x1A\xE1\x46\x47\xE8\xA1\x35\x06\x1A",	(uchar *)"\x3A\xDE\xBB\xE0"}, 	// 1	//TMK SEC_KCV_ZERO方式(TMK ID=2, 密文由TLK发散,明文KEY=16个0x17)
		{(uchar *)"\x15", 																(uchar *)"\x26\x5A\x7A\xBF"},	// 2	//TMK SEC_KCV_ZERO方式(TMK ID=3, 明文安装,明文KEY=16个0x15)
		{(uchar *)"\x2E\x38\x1D\x92\x01\x88\x58\x22\x2E\x38\x1D\x92\x01\x88\x58\x22",	(uchar *)"\x54\x8C\x54\xFE"},	// 3	//TAK SEC_KCV_VAL方式(TAK ID=4, TMK发散,key明文16个0x1f)
		{(uchar *)"\xFF\x4C\xAF\xA1\x33\x09\x76\xEC\xFF\x4C\xAF\xA1\x33\x09\x76\xEC",	(uchar *)"\xCB\xF7\x20\x63"}, 	// 4	//TPK SEC_KCV_VAL方式(TPK ID=5, 由ID=3,type=TMK发散,key明文16个0x25)
		{(uchar *)"\x8A\xEE\xE9\x1A\x8C\x2F\x71\x97", 									(uchar *)"\xA4\x70\xF9\x9E"},	// 5	//TDK SEC_KCV_VAL方式key:8位(TDK ID=6, 由ID=3,type=TMK发散,len=8,key明文8个0x29)
		//中间6组为SM4算法密钥
		{(uchar *)"\x19",															   NULL},                       // 6  //TLK (key=16个0x19, TLK,ID=1)
		{(uchar *)"\x51\xE9\x6A\x1A\x86\x9F\x3B\x7C\x9B\xD3\x3F\x6C\x34\x4C\x76\x32",   (uchar *)"\x59\x4F\x26\x2E"}, // 7  //TMK SEC_KCV_ZERO方式(TMK ID=2, 密文由TLK发散,明文KEY=16个0X1F)
		{(uchar *)"\x21",															    (uchar *)"\x6C\xD7\x6C\x59"}, // 8  //TMK SEC_KCV_ZERO方式(TMK ID=3, 明文安装,明文KEY=16个0X21) 
	        {(uchar *)"\xC0\xD3\xEB\x69\xB1\x06\x47\xEF\x76\xD2\x49\x62\x75\xA4\x8B\xFB",   (uchar *)"\x93\x38\x75\xD7"},	//9  //TAK SEC_KCV_ZERO方式(TAK ID=4, TMK发散,key明文16个0X27 )
		{(uchar *)"\x44\x4E\x8D\x2A\x2A\xAA\x91\xB4\xAD\x47\x94\x78\xE6\x7D\x71\x75",   (uchar *)"\x2E\x36\x8F\x3C"},	//10  //TPK SEC_KCV_ZERO方式(TPK ID=5, 由ID=3,type=TMK发散,key明文16个0X2D)
	        {(uchar *)"\x08\x48\x69\x83\x77\xF7\x1B\x33\xC5\x4D\x5C\x4F\xF5\x13\xB2\xC7",	(uchar *)"\xA4\x3F\xDD\xA6"},   //11  //TDK SEC_KCV_ZERO方式(TDK ID=11, 由ID=6,type=TMK发散,key明文16个0X33) 
		//后6组为AES算法密钥
		{(uchar *)"\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13\x13",		   NULL},						// 12  //TLK (key=16个0x19, TLK,ID=1)
		{(uchar *)"\xFD\x5E\x76\xD2\xF0\x7E\xBD\x04\xF0\xAA\x2D\x96\x67\x81\xC6\x73",	(uchar *)"\x18\x19\x45\xEC"}, // 13 //TMK SEC_KCV_ZERO方式(TMK ID=2, 密文由TLK发散,明文KEY=16个0X15)
		{(uchar *)"\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17",	(uchar *)"\x8E\x68\x02\x75"}, // 14 //TMK SEC_KCV_ZERO方式(TMK ID=3, 明文安装,明文KEY=32个0X17) 
		{(uchar *)"\x02\x3E\xA9\xBD\x97\x9B\xDC\x73\x49\x8E\xC5\x78\x3B\x02\x32\xA9 ",	(uchar *)"\x64\x43\xCD\x29"},	//15  //TAK SEC_KCV_ZERO方式(TAK ID=4, TMK发散,key明文16个0X21 )
		{(uchar *)"\xF3\xCC\x71\x93\x89\x93\xE2\xE9\x7B\x28\x65\xE1\xC6\x1A\x2F\x93\x8F\xBC\x69\x18\x11\xD1\x43\x1A\x61\xDD\xDF\xED\x9C\xA5\xA8\xB3",	(uchar *)"\x93\xCC\x9E\xAC"},	//16  //TPK SEC_KCV_ZERO方式(TPK ID=5, 由ID=3,type=TMK发散,key明文24个0X27)
		{(uchar *)"\x81\xE9\xF4\xAB\x89\xB9\x4D\xE4\x8D\xA7\x26\x18\xA1\xB6\x8B\xD1",	NULL},	//17  //TDK SEC_KCV_NONE方式(TDK ID=6, 由ID=3,type=TMK发散,key明文16个0X33) 
		//用于case11  重复使用上面的数据
	//	{(uchar *)"\x11", 																(uchar *)"\x82\xE1\x36\x65"}, 	// 18	//TMK SEC_KCV_ZERO方式TLK (TMK ID=1, 明文安装,明文KEY=16个0x11)
	 // {(uchar *)"\xC0\xB7\x57\x41\x3B\x84\x79\x25\xC0\xB7\x57\x41\x3B\x84\x79\x25",                   (uchar *)"\xA8\xB7\xB5\xBD"}, //19 //TAK SEC_KCV_ZERO方式(TAK ID=3, TMK发散,key明文16个0X13 )
	};
#if  SEC_SM4_ENABLE
	int algnum = 2;//算法数量
	EM_SEC_KEY_ALG key_flag[2] = {SEC_KEY_DES , SEC_KEY_SM4};
#else
	int algnum = 1;
	int key_flag[2] = {0,0} ;
#endif
#if SEC_AES_ENABLE
	ST_EXTEND_KEYBLOCK stExtendKey;
#endif

	/*process body*/
	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI); 

	for(i = 0; i< algnum; i++)
	{
		//测试前置安装各类密钥
		//以SEC_KCV_NONE方式安装TLK,ID=1
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=1;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK|key_flag[i];
		stKeyInfo.nDstKeyLen=16;	
		memset(stKeyInfo.sDstKeyValue, *SecLoad[0+i*6].pkey, stKeyInfo.nDstKeyLen);
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		//以SEC_KCV_ZERO方式安装TMK(密文),ID=2
		stKeyInfo.ucScrKeyIdx=1;
		stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|key_flag[i];
		stKeyInfo.ucDstKeyIdx=2;
		stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|key_flag[i];
		stKeyInfo.nDstKeyLen=16;	
		memcpy(stKeyInfo.sDstKeyValue, SecLoad[1+i*6].pkey, stKeyInfo.nDstKeyLen);

		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[1+i*6].pkcv, stKcvInfoIn.nLen);	

		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		//以SEC_KCV_ZERO方式安装TMK(明文),ID=3
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=3;
		stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|key_flag[i];
		stKeyInfo.nDstKeyLen=16;	
		memset(stKeyInfo.sDstKeyValue, *SecLoad[2+i*6].pkey, stKeyInfo.nDstKeyLen);

		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[2+i*6].pkcv, stKcvInfoIn.nLen);

		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		//在des算法下,以SEC_KCV_VAL方式去安装TAK(由ID=3,type=TMK发散),ID=4;在sm4算法下,以SEC_KCV_ZERO方式去安装TAK(由ID=3,type=TMK发散),ID=4
		stKeyInfo.ucScrKeyIdx=3;
		stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|key_flag[i];
		stKeyInfo.ucDstKeyIdx=4;
		stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|key_flag[i];
		stKeyInfo.nDstKeyLen=16;	
		memcpy(stKeyInfo.sDstKeyValue, SecLoad[3+i*6].pkey, stKeyInfo.nDstKeyLen);
		
		if(i==0)
			stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
		else
			stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[3+i*6].pkcv, stKcvInfoIn.nLen);
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		//在des算法下,以SEC_KCV_VAL方式去安装TPK(由ID=3,type=TMK发散),ID=5;在sm4算法下,以SEC_KCV_ZERO方式去安装TPK(由ID=3,type=TMK发散),ID=5
		stKeyInfo.ucScrKeyIdx=3;
		stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|key_flag[i];
		stKeyInfo.ucDstKeyIdx=5;
		stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TPK|key_flag[i];
		stKeyInfo.nDstKeyLen=16;	
		memcpy(stKeyInfo.sDstKeyValue, SecLoad[4+i*6].pkey, stKeyInfo.nDstKeyLen);
		if(i==0)
			stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
		else
			stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[4+i*6].pkcv, stKcvInfoIn.nLen);
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		
#if !(OVERSEAS_ENABLE||defined ME30S||defined N910P)//ME30S是海外产品
		//在des算法下,以SEC_KCV_VAL方式去安装TDK(由ID=3,type=TMK发散),len=8,ID=6;在sm4算法下,以SEC_KCV_ZERO方式去安装TDK(由ID=3,type=TMK发散),len=16,ID=6
		stKeyInfo.ucScrKeyIdx=3;
		stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|key_flag[i];
		stKeyInfo.ucDstKeyIdx=6;
		stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TDK|key_flag[i];
		memcpy(stKeyInfo.sDstKeyValue, SecLoad[5+i*6].pkey, stKeyInfo.nDstKeyLen);
		if(i==0)
		{
			stKeyInfo.nDstKeyLen=8;
			stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
		}
		else
		{
			stKeyInfo.nDstKeyLen=16;
			stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		}
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[5+i*6].pkcv, stKcvInfoIn.nLen);
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
#endif

#if !(/*defined ME50||defined ME50N||defined ME50C||defined ME50H||*/defined ME15C||defined ME15CHX||defined ME20||defined ME30||defined ME30S/*||defined ME30THM||defined ME32THM*/||defined ME30MH||defined ME15B)
		/*
		//case1:删除不存在的密钥id=100
		if((ret=NDK_SecKeyDelete(100,SEC_KEY_TYPE_TDK|key_flag[i]))!=NDK_ERR)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		//case2:删除错误ID=256为非法值
		if((ret=NDK_SecKeyDelete(256,SEC_KEY_TYPE_TDK|key_flag[i]))!=NDK_ERR)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		//case3:删除密钥的ID和类型不匹配ID=6的密钥类型为SEC_KEY_TYPE_TDK
		if((ret=NDK_SecKeyDelete(6,SEC_KEY_TYPE_TMK|key_flag[i]))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		*/
		//case2:删除错误的密钥类型SEC_KEY_TYPE_TLK-1,SEC_KEY_TYPE_TDK+1预期返回失败
		if((ret=NDK_SecKeyDelete(2,(SEC_KEY_TYPE_TLK|key_flag[i])-1))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		if((ret=NDK_SecKeyDelete(2,(SEC_KEY_TYPE_TDK|key_flag[i])+1))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		
		//case4:删除正确的ID=6和类型=TDK的密钥后获取KCV应该失败
		if((ret=NDK_SecKeyDelete(6,SEC_KEY_TYPE_TDK|key_flag[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);//ret=-6,i=1
			GOTOERR;
		}
		if(i==0)
			stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
		else
			stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		//ANDIROD_ENABLE	开发说当设置成共享密钥表的时候，也就是在K21判断返回时是返回mac错；
		//而设置成安卓端的密钥表的时候，由于mac校验是在K21端判断的，安卓端首先判断到的是密钥不存在，所以就返回密钥不存在
#if  defined N900AZ ||defined N910AZ||defined IM81AZ//ANDIROD_ENABLE  老产品有区分新产品不再区分 统一返回NDK_ERR_SECKM_READ_REC 20180416 modify  N910P是海外产品没有安装id6的密钥,没安装密钥情况下删除是返回-1309,在有装密钥情况下删除会返回-1311 20180830
		if (g_seckeyowner) //密钥在安卓端
		{
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TDK|key_flag[i], 6, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
			{
				send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}	
		}
		else  //密钥在k21端
		{	
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TDK|key_flag[i], 6, &stKcvInfoIn))!=NDK_ERR_SECKM_KEY_MAC)
			{
				send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}
		}
#elif   ( K21_ENABLE && !ANDIROD_ENABLE)  //K21_ENABLE包括 IM81  N900
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TDK|key_flag[i], 6, &stKcvInfoIn))!=NDK_ERR_SECKM_KEY_MAC)//经开发确认目前只有IM81导入此函数,且机制与中端不同,返回值不同
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
#else
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TDK|key_flag[i], 6, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}	
#endif
		
		//case5:删除正确的ID=2和类型=TMK的密钥后获取KCV应该失败
		if((ret=NDK_SecKeyDelete(2, SEC_KEY_TYPE_TMK|key_flag[i]))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);//ret=-6,i=1
			GOTOERR;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
#if  defined N900AZ ||defined N910AZ||defined IM81AZ||defined N910P//ANDIROD_ENABLE  老产品有区分新产品不再区分 统一返回NDK_ERR_SECKM_READ_REC 20180416 modify
		if (g_seckeyowner) //密钥在安卓端
		{	
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 2, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
			{
				send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}
		}
		else  //密钥在k21端
		{	
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 2, &stKcvInfoIn))!=NDK_ERR_SECKM_KEY_MAC)
			{
				send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}
		}
#elif   ( K21_ENABLE && !ANDIROD_ENABLE)  //K21_ENABLE包括 IM81  N900
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 2, &stKcvInfoIn))!=NDK_ERR_SECKM_KEY_MAC)//经开发确认目前只有IM81导入此函数,且机制与中端不同,返回值不同
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
#else
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 2, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
#endif
#else
		if((ret=NDK_SecKeyDelete(6,SEC_KEY_TYPE_TDK|key_flag[i]))!=NDK_ERR_NOT_SUPPORT)//部分设备还不支持NDK_SecKeyDelete函数
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
#endif
		//case6:删除后不影响其它未删除的ID的密钥,获取ID=3的主密钥的KCV应该成功
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 3, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
#if OVERSEAS_ENABLE||defined N910P  //海外版本使用3字节KCV验证 根据国兵邮件 20180116 modify by jiangym 
		if(memcmp(stKcvInfoIn.sCheckBuf, SecLoad[2+i*6].pkcv, 3))
#else			
		if(memcmp(stKcvInfoIn.sCheckBuf, SecLoad[2+i*6].pkcv, 4))
#endif
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		
		//case7:正常调用全删除函数
		if((ret=NDK_SecKeyErase())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			RETURN;
		}

		//case8:验证所有密钥已擦除
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
#if !defined N910P		//高端产品910P 不具备默认安装TLK功能 
		//sdk3.0且密钥在共享密钥区时清安全和清密钥后默认安装TLK，sdk2.0不安装  20180724  modify
		if(!g_seckeyowner &&  i==0 && flag_sdk3)
		{
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TLK|key_flag[i], 1, &stKcvInfoIn))!=NDK_OK)
			{
				send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				RETURN;
			}
		}
		else
		{
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TLK|key_flag[i], 1, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
			{
				send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				RETURN;
			}
		}
#else
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TLK|key_flag[i], 1, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			RETURN;
		}
#endif
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|key_flag[i], 3, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			RETURN;
		} 
		if(i==0)
			stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
		else
			stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TAK|key_flag[i], 4, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			RETURN;
		}
		if(i==0)
			stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
		else
			stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TPK|key_flag[i], 5, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			goto ERR;
		}
	}
	
	//AES 单独提取出来 //aes 安装使用扩展方式安装 20180808 modify
#if SEC_AES_ENABLE 
	if(flag_sdk3) //sdk3.0才有导入AES相关
	{
		i = 2;
		//以SEC_KCV_NONE方式安装TLK,ID=1
		memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
		memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=1;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TLK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 16;
		stExtendKey.pblock =(char*)SecLoad[0+i*6].pkey;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		stKcvInfoIn.nLen=0;
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		//以SEC_KCV_ZERO方式安装TMK(密文),ID=2
		memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
		memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=1;
		stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_AES;
		stKeyInfo.ucDstKeyIdx=2;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 16;
		stExtendKey.pblock = (char*)SecLoad[1+i*6].pkey;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[1+i*6].pkcv, stKcvInfoIn.nLen);	
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//以SEC_KCV_ZERO方式安装TMK(明文),ID=3
		memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
		memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=3;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 16;
		stExtendKey.pblock = (char*)SecLoad[2+i*6].pkey;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[2+i*6].pkcv, stKcvInfoIn.nLen);
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//以SEC_KCV_ZERO方式去安装TAK(由ID=3,type=TMK发散),ID=4
		memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
		memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=3;
		stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
		stKeyInfo.ucDstKeyIdx=4;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 16;
		stExtendKey.pblock = (char*)SecLoad[3+i*6].pkey;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
		
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[3+i*6].pkcv, stKcvInfoIn.nLen);
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

#if !defined N910P  //  N910P是PCI版本不能使用低字节安装高字节,故不安装 20180809 modify
		//以SEC_KCV_ZERO方式去安装TPK(由ID=3,type=TMK发散),ID=5
		memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
		memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=3;
		stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
		stKeyInfo.ucDstKeyIdx=5;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TPK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 24;
		stExtendKey.pblock = (char*)SecLoad[4+i*6].pkey;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
		
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[4+i*6].pkcv, stKcvInfoIn.nLen);
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#endif

		//以SEC_KCV_ZERO方式去安装TDK(由ID=3,type=TMK发散),len=16,ID=6
		memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
		memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=3;
		stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
		stKeyInfo.ucDstKeyIdx=6;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TDK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 16;
		stExtendKey.pblock = (char*)SecLoad[5+i*6].pkey;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	
		//case2.1:删除错误的密钥类型SEC_KEY_TYPE_TLK-1,SEC_KEY_TYPE_TDK+1预期返回失败
		if((ret=NDK_SecKeyDelete(2,(SEC_KEY_TYPE_TLK|SEC_KEY_AES)-1))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		if((ret=NDK_SecKeyDelete(2,(SEC_KEY_TYPE_TDK|SEC_KEY_AES)+1))!=NDK_ERR_PARA)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		
		//case4.1:删除正确的ID=6和类型=TDK的密钥后获取KCV应该失败
		if((ret=NDK_SecKeyDelete(6,SEC_KEY_TYPE_TDK|SEC_KEY_AES))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);//ret=-6,i=1
			GOTOERR;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		//ANDIROD_ENABLE	开发说当设置成共享密钥表的时候，也就是在K21判断返回时是返回mac错；
		//而设置成安卓端的密钥表的时候，由于mac校验是在K21端判断的，安卓端首先判断到的是密钥不存在，所以就返回密钥不存在
#if  defined N900AZ ||defined N910AZ||defined IM81AZ||defined N910P//ANDIROD_ENABLE  老产品有区分新产品不再区分 统一返回NDK_ERR_SECKM_READ_REC 20180416 modify
		if (g_seckeyowner) //密钥在安卓端
		{
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TDK|SEC_KEY_AES, 6, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
			{
				send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}	
		}
		else  //密钥在k21端
		{	
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TDK|SEC_KEY_AES, 6, &stKcvInfoIn))!=NDK_ERR_SECKM_KEY_MAC)
			{
				send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}
		}
#else
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TDK|SEC_KEY_AES, 6, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}	
#endif
		
		//case5.1:删除正确的ID=2和类型=TMK的密钥后获取KCV应该失败
		if((ret=NDK_SecKeyDelete(2, SEC_KEY_TYPE_TMK|SEC_KEY_AES))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
#if  defined N900AZ ||defined N910AZ||defined IM81AZ||defined N910P//ANDIROD_ENABLE  老产品有区分新产品不再区分 统一返回NDK_ERR_SECKM_READ_REC 20180416 modify
		if (g_seckeyowner) //密钥在安卓端
		{	
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 2, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
			{
				send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}
		}
		else  //密钥在k21端
		{	
			if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 2, &stKcvInfoIn))!=NDK_ERR_SECKM_KEY_MAC)
			{
				send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}
		}
#else
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 2, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
#endif

		//case6.1:删除后不影响其它未删除的ID的密钥,获取ID=3的主密钥的KCV应该成功
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 3, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
#if OVERSEAS_ENABLE||defined N910P  //海外版本使用3字节KCV验证 根据国兵邮件 20180116 modify by jiangym 
		if(memcmp(stKcvInfoIn.sCheckBuf, SecLoad[2+i*6].pkcv, 3))
#else			
		if(memcmp(stKcvInfoIn.sCheckBuf, SecLoad[2+i*6].pkcv, 4))
#endif
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			GOTOERR;
		}
		
		//case7.1:正常调用全删除函数
		if((ret=NDK_SecKeyErase())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			RETURN;
		}

		//case8.1:验证所有密钥已擦除
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TLK|SEC_KEY_AES, 1, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			RETURN;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 3, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			RETURN;
		} 
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TAK|SEC_KEY_AES, 4, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			RETURN;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TPK|SEC_KEY_AES, 5, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
			goto ERR;
		}	
	}	
#endif				
	
#if SEC_SM4_ENABLE
	//case9:NDK_SecKeyDelete应不会删除其它算法的密钥 
	//安装DES算法的密钥:以SEC_KCV_ZERO方式明文安装TMK
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_DES;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[2].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[2].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret); 
		GOTOERR;
	}
	//安装SM4算法的密钥
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[8].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[8].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if !(/*defined ME50||defined ME50N||defined ME50C||defined ME50H||*/defined ME50NX||defined ME68||defined ME15C||defined ME15CHX||defined ME20||defined ME30||defined ME30S/*||defined ME30THM||defined ME32THM*/||defined ME30MH||defined ME30GX||defined ME15B)
	//删除SM4算法的密钥后，DES算法的密钥仍然存在
	if((ret=NDK_SecKeyDelete(2, SEC_KEY_TYPE_TMK|SEC_KEY_SM4))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_DES, 2, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(stKcvInfoIn.sCheckBuf, SecLoad[2].pkcv, 4))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//重新安装SM4算法的密钥
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_SM4;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[8].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[8].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
#if !(defined ME50||defined ME50N||defined ME50C||defined ME50H||defined ME50NX||defined ME68||defined ME15C||defined ME15CHX||defined ME20||defined ME30||defined ME30S/*||defined ME30THM||defined ME32THM*/||defined ME30MH||defined ME30GX||defined ME15B)	
	//删除DES算法的密钥后，SM4算法的密钥仍然存在
	if((ret=NDK_SecKeyDelete(2, SEC_KEY_TYPE_TMK|SEC_KEY_DES))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif
	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_SM4, 2, &stKcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	if(memcmp(stKcvInfoIn.sCheckBuf, SecLoad[8].pkcv, 4))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}

	//case10:NDK_SecKeyErase应能删除指定ID全部算法的密钥
	//重新安装DES算法的密钥:以SEC_KCV_NONE方式明文安装TMK
	stKeyInfo.ucScrKeyIdx=0;
	stKeyInfo.ucScrKeyType=0;
	stKeyInfo.ucDstKeyIdx=2;
	stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_DES;
	stKeyInfo.nDstKeyLen=16;	
	memset(stKeyInfo.sDstKeyValue, *SecLoad[2].pkey, stKeyInfo.nDstKeyLen);

	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	stKcvInfoIn.nLen=4;
	memcpy(stKcvInfoIn.sCheckBuf, SecLoad[2].pkcv, stKcvInfoIn.nLen);
	if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
	if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_DES, 2, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
	{
		send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
		RETURN;
	}
	if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_SM4, 2, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
	{
		send_result("line %d:%s测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
		RETURN;
	}
#endif
	// case11:高端产品应用那边反馈删除主密钥两次后进行kcv校验主密钥会报存在，发现工作密钥同样存在该问题（必现问题）linying added 20171214	
	//新增sm4以及AES密钥的相关验证  20180418 add
#if !(/*defined ME50||defined ME50N||defined ME50C||defined ME50H||*/defined ME15C||defined ME15CHX||defined ME20||defined ME30||defined ME30S/*||defined ME30THM||defined ME32THM*/||defined ME30MH||defined ME15B)
	for(i = 0; i< algnum; i++)
	{
		if(i==0 || i==1)
		{	
			// 装载主密钥	
			stKeyInfo.ucScrKeyIdx=0;
			stKeyInfo.ucScrKeyType=0;
			stKeyInfo.ucDstKeyIdx=1;
			stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|key_flag[i];
			stKeyInfo.nDstKeyLen=16;	
			memset(stKeyInfo.sDstKeyValue,*SecLoad[2+i*6].pkey, stKeyInfo.nDstKeyLen);
			stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
			stKcvInfoIn.nLen=4;
			memcpy(stKcvInfoIn.sCheckBuf,SecLoad[2+i*6].pkcv, stKcvInfoIn.nLen);
			if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
			{
				send_result("line %d:%s装载主密钥测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}
			//装载MAC工作密钥TAK,index = 4
			stKeyInfo.ucScrKeyIdx=1;
			stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|key_flag[i];
			stKeyInfo.ucDstKeyIdx=3;
			stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TAK|key_flag[i];
			stKeyInfo.nDstKeyLen=16;	
			memcpy(stKeyInfo.sDstKeyValue,SecLoad[3+i*6].pkey, stKeyInfo.nDstKeyLen);
			if(i==0)
				stKcvInfoIn.nCheckMode=SEC_KCV_VAL;
			else
				stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
			stKcvInfoIn.nLen=4;
			memcpy(stKcvInfoIn.sCheckBuf,SecLoad[3+i*6].pkcv, stKcvInfoIn.nLen);	
			if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
			{
				send_result("line %d:%s装载MAC工作密钥测试失败(ret=%d,i=%d)", __LINE__, TESTAPI, ret, i);
				GOTOERR;
			}
		}
		else
		{
#if SEC_AES_ENABLE
			if(flag_sdk3)
			{
				//以SEC_KCV_ZERO方式安装TMK(明文),ID=3
				memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
				memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
				memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
				stKeyInfo.ucScrKeyIdx=0;
				stKeyInfo.ucScrKeyType=0;
				stKeyInfo.ucDstKeyIdx=1;
				stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
				stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
				stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
				stExtendKey.len = 16;
				stExtendKey.pblock = (char*)SecLoad[2+i*6].pkey;
				memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));

				stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
				stKcvInfoIn.nLen=4;
				memcpy(stKcvInfoIn.sCheckBuf, SecLoad[2+i*6].pkcv, stKcvInfoIn.nLen);
				if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
				{
					send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
					GOTOERR;
				}
				//以SEC_KCV_ZERO方式去安装TAK(由ID=3,type=TMK发散),ID=4
				memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
				memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
				memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
				stKeyInfo.ucScrKeyIdx=1;
				stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_AES;
				stKeyInfo.ucDstKeyIdx=3;
				stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TAK|SEC_KEY_AES;
				stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
				stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
				stExtendKey.len = 16;
				stExtendKey.pblock = (char*)SecLoad[3+i*6].pkey;
				memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
			
				stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
				stKcvInfoIn.nLen=4;
				memcpy(stKcvInfoIn.sCheckBuf, SecLoad[3+i*6].pkcv, stKcvInfoIn.nLen);
				if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
				{
					send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
					GOTOERR;
				}
			}	
#endif
		}
		//删除主密钥两次
		if((ret=NDK_SecKeyDelete(1, SEC_KEY_TYPE_TMK)!=NDK_OK))
		{
			send_result("line %d:%s主密钥删除失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_SecKeyDelete(1, SEC_KEY_TYPE_TMK))!=NDK_OK)
		{
			send_result("line %d:%s主密钥删除失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		//校验主密钥
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK, 1, &stKcvInfoIn))==NDK_OK)
		{
			send_result("line %d:%s主密钥校验错误(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		//删除mac工作密钥
		if((ret=NDK_SecKeyDelete(3, SEC_KEY_TYPE_TAK))!=NDK_OK)
		{
			send_result("line %d:%s删除MAC密钥测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if((ret=NDK_SecKeyDelete(3, SEC_KEY_TYPE_TAK))!=NDK_OK)
		{
			send_result("line %d:%s删除MAC密钥测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		//校验mac密钥
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TAK, 3, &stKcvInfoIn))==NDK_OK)
		{
			send_result("line %d:%sMAC密钥校验错误((ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
#endif
	//新增AES相关验证20180418 add
#if  SEC_AES_ENABLE
	if(flag_sdk3)
	{
		//case12:NDK_SecKeyDelete应不会删除其它算法的密钥 
		//安装DES算法的密钥:以SEC_KCV_ZERO方式明文安装TMK
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=2;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_DES;
		stKeyInfo.nDstKeyLen=16;	
		memset(stKeyInfo.sDstKeyValue, *SecLoad[2].pkey, stKeyInfo.nDstKeyLen);

		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[2].pkcv, stKcvInfoIn.nLen);
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret); 
			GOTOERR;
		}
		//安装AES算法的密钥
		memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
		memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=2;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 16;
		stExtendKey.pblock = (char *)SecLoad[14].pkey;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
		
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[14].pkcv, stKcvInfoIn.nLen);
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//删除AES算法的密钥后，DES算法的密钥仍然存在
		if((ret=NDK_SecKeyDelete(2, SEC_KEY_TYPE_TMK|SEC_KEY_AES))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_DES, 2, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
#if defined N910P
		if(memcmp(stKcvInfoIn.sCheckBuf, SecLoad[2].pkcv, 3))
#else			
		if(memcmp(stKcvInfoIn.sCheckBuf, SecLoad[2].pkcv, 4))
#endif
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//重新安装AES算法的密钥
		memset(&stKcvInfoIn,0,sizeof(stKcvInfoIn));
		memset(&stKeyInfo,0,sizeof(ST_SEC_KEY_INFO));
		memset(&stExtendKey, 0, sizeof(ST_EXTEND_KEYBLOCK));
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=2;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_AES;
		stKeyInfo.nDstKeyLen = sizeof(ST_EXTEND_KEYBLOCK);
		stExtendKey.format = SEC_KEYBLOCK_FMT_AES;
		stExtendKey.len = 16;
		stExtendKey.pblock = (char *)SecLoad[14].pkey;
		memcpy(stKeyInfo.sDstKeyValue, &stExtendKey, sizeof(ST_EXTEND_KEYBLOCK));
		
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[14].pkcv, stKcvInfoIn.nLen);
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}

		//删除DES算法的密钥后，AES算法的密钥仍然存在
		if((ret=NDK_SecKeyDelete(2, SEC_KEY_TYPE_TMK|SEC_KEY_DES))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 2, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		if(memcmp(stKcvInfoIn.sCheckBuf, SecLoad[14].pkcv, 3))
		{
			send_result("line %d:%s测试失败", __LINE__, TESTAPI);
			GOTOERR;
		}

		//case10:NDK_SecKeyErase应能删除指定ID全部算法的密钥
		//重新安装DES算法的密钥:以SEC_KCV_NONE方式明文安装TMK
		stKeyInfo.ucScrKeyIdx=0;
		stKeyInfo.ucScrKeyType=0;
		stKeyInfo.ucDstKeyIdx=2;
		stKeyInfo.ucDstKeyType = SEC_KEY_TYPE_TMK|SEC_KEY_DES;
		stKeyInfo.nDstKeyLen=16;	
		memset(stKeyInfo.sDstKeyValue, *SecLoad[2].pkey, stKeyInfo.nDstKeyLen);

		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		stKcvInfoIn.nLen=4;
		memcpy(stKcvInfoIn.sCheckBuf, SecLoad[2].pkcv, stKcvInfoIn.nLen);
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	
		if((ret=NDK_SecKeyErase())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_DES, 2, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_AES, 2, &stKcvInfoIn))!=NDK_ERR_SECKM_READ_REC)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			RETURN;
		}
	}
#endif

#if ANDIROD_ENABLE&&!defined N910P //N910P是特殊产品不具备此功能 故不测 20180809 modify
	if(flag_sdk3)
	{
		//sdk3.0清安全和清密钥后默认安装TLK，sdk2.0不安装  20180724  modify
		//case11:验证清安全和清密钥后会默认安装16字节的0x31的TLK  20180620 sull add
		if((ret=NDK_SecKeyErase())!=NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			RETURN;
		} 
		if((ret=NDK_SecSetKeyOwner("*"))!=NDK_OK)
		{
			send_result("line %d:设置密钥属主失败(%d)", __LINE__, ret);
			GOTOERR;
		}
		//以SEC_KCV_NONE方式安装TMK(密文),ID=2,明文16个0x15 通过getkcv获取进行验证
		stKeyInfo.ucScrKeyIdx=1;
		stKeyInfo.ucScrKeyType=SEC_KEY_TYPE_TLK|SEC_KEY_DES;
		stKeyInfo.ucDstKeyIdx=2;
		stKeyInfo.ucDstKeyType=SEC_KEY_TYPE_TMK|SEC_KEY_DES;
		stKeyInfo.nDstKeyLen=16;	
		memcpy(stKeyInfo.sDstKeyValue, "\xCB\x62\x65\x94\x48\xAC\x8A\x72\xCB\x62\x65\x94\x48\xAC\x8A\x72", stKeyInfo.nDstKeyLen);//密文是16字节0x15与16字节0x31加密的结果
		stKcvInfoIn.nCheckMode=SEC_KCV_NONE;
		if((ret = NDK_SecLoadKey(&stKeyInfo, &stKcvInfoIn)) != NDK_OK)
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
		stKcvInfoIn.nCheckMode=SEC_KCV_ZERO;
		if((ret=NDK_SecGetKcv(SEC_KEY_TYPE_TMK|SEC_KEY_DES, 2, &stKcvInfoIn))!=NDK_OK)
		{
			send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}	
		if(memcmp(stKcvInfoIn.sCheckBuf, "\x26\x5A\x7A\xBF", 4)) 
		{
			send_result("line %d:%s测试失败(ret=%d)", __LINE__, TESTAPI, ret);
			GOTOERR;
		}
	}
#endif
	send_result("%s测试通过", TESTAPI);
ERR: 
//恢复成入口处的设置
	if(flag_sdk3)
	{
		if(g_seckeyowner)
			NDK_SecSetKeyOwner("");
		else
			NDK_SecSetKeyOwner("*");
	}
	NDK_SecKeyErase();
	return;
}

