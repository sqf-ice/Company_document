/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name		: sec24.c
* Author 		:  jiangym
* version		: 
* DATE			:  
* directory 		: 测试通过安全设置可以允许NDK_SecCalcDesDukpt加解密函数使用各种类型密钥
* description		: 安全配置
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
#define		TESTAPI		"安全设置DUKPT加解密函数"

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

/*---------------functions declaration--------------------------*/

/*---------------functions definition---------------------------*/
/****************************************************************
* function name 	 	: 
* functional description 	: 提供安全等级的设置与获取功能,密钥唯一性,密钥专用性,密钥保护强度的验证
* input parameter	 	:无
* output parameter	 	:无
* return value		 	:无
* history 		 		:  author			date				remarks
*			  	  	  	jiangym 			20150306	      created
*****************************************************************/
void sec24(void)
{
	/*private & local definition*/
	int ret = 0;
	uint punCfgInfo;
	uchar DataIn[33]={0}, DataOut[33]={0}, KSN[11]={0}, KSN1[11]={0};
	ST_SEC_KCV_INFO KcvInfoIn;
	uchar DukptKeyValue[16]={0xD5,0xD4,0x4F,0xF7,0x20,0x68,0x3D,0x0D,0x70,0xBE,0x25,0x28,0x18,0xE2,0xA1,0x8A};//DUKPY初始明文密钥	
	memset(DataIn, 0x33, sizeof(DataIn));//初始化数据

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI);

	//测试前置
	NDK_SecSetKeyOwner("");
	if(((ret=NDK_SecGetCfg(&punCfgInfo))!=NDK_OK)||(punCfgInfo!=0x06)) //默认安全等级为0x06
	{
		send_result("line %d:%s测试失败(%d,%x)", __LINE__, TESTAPI, ret, punCfgInfo);
		return;
	}
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	
	if((ret=NDK_SecSetCfg(0x18))!=NDK_OK) //修改安全等级跟默认值相反 0x18
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}
	//明文安装一组DUKPT, ID=6, KSN为10个字节0x00 /*约定:以下KCV均以不校验方式安装*/
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(KSN, 0x00, 10);//初始KSN
	if ((ret=NDK_SecLoadTIK(1, 0, 16, DukptKeyValue, KSN, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//DUKPT 安装初始密钥无唯一性,即同一密钥可以NDK_SecLoadTIK到不同id上
	//case1:验证密钥惟一性限制被关闭，同一个密钥值重复安装到不同ID  
	
	//case2:密钥专用性限制关闭:允许使用主密钥、PIN密钥、MAC密钥进行任意数据的加解密操作
	KcvInfoIn.nCheckMode=SEC_KCV_NONE;
	memset(KSN, 0x00, 10);
	if ((ret=NDK_SecLoadTIK(6, 0, 16, DukptKeyValue, KSN, &KcvInfoIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//case2.1:DUKPT派生的des密钥做DES加解密
	memset(DataOut, 0, sizeof(DataOut));
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TDK, NULL, 8, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_8))!=NDK_OK)  
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if defined ME31S
	//使用8字节密钥BFA9CA10BF791459(该密钥是由派生密钥6ef92795e5ed714dcdeb767b330ea9bd 进行自加密得到)对8字节数据3333333333333333进行加密后拼接得到结果20180511 modify 因开发导入新的安全库
	if(memcmp(DataOut,"\x0C\x54\x7B\x0E\x8F\x24\x27\x5F", 8))
#else
	//使用8字节密钥6ef92795e5ed714d(该密钥由16字节DUKPT密钥的前8字节密钥组成)对8字节数据3333333333333333进行加密后拼接得到结果
	if(memcmp(DataOut,"\xE4\x87\xC1\x16\xA5\x48\xB9\x2C", 8))
#endif
	{
		send_result("line %d:%s测试失败(0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7]);
		GOTOERR;
	}
	memset(DataOut, 0, sizeof(DataOut));
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TDK, NULL, 8, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_8))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if defined ME31S
	//使用8字节密钥BFA9CA10BF791459(该密钥是由派生密钥6ef92795e5ed714dcdeb767b330ea9bd 进行自加密得到)对8字节数据3333333333333333进行解密后得到结果
	if(memcmp(DataOut,"\x05\x26\x90\x36\x97\x17\x19\x01", 8))
#else
	//本预期结果使用8字节密钥6ef92795e5ed714d对8字节数据3333333333333333在加解密工具\PosDebug工具解密得出1EB577120A087B59
	if(memcmp(DataOut,"\x1E\xB5\x77\x12\x0A\x08\x7B\x59", 8))
#endif
	{
		send_result("line %d:%s测试失败(0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x)", __LINE__, TESTAPI, DataOut[0], DataOut[1], DataOut[2], DataOut[3], DataOut[4], DataOut[5], DataOut[6], DataOut[7]);
		GOTOERR;
	}
	
	//case2.2:PIN密钥做DES加解密
	memset(DataOut, 0, sizeof(DataOut));
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TPK, NULL, 16, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_16))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//使用安装的16字节DUKPT密钥派生成PIN密钥6ef92795e51271b2cdeb767b33f1a942密钥对2组8字节数据3333333333333333进行加密后拼接得到结果AE7CF265E0D6E0B8
	if(memcmp(DataOut,"\xAE\x7C\xF2\x65\xE0\xD6\xE0\xB8\xAE\x7C\xF2\x65\xE0\xD6\xE0\xB8", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	memset(DataOut, 0, sizeof(DataOut));
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TPK, NULL, 16, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_16))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//使用安装的16字节DUKPT密钥派生成PIN密钥6ef92795e51271b2cdeb767b33f1a942密钥对2组8字节数据3333333333333333进行解密后拼接得到结果5EEEE57CE7262C1B
	if(memcmp(DataOut,"\x5E\xEE\xE5\x7C\xE7\x26\x2C\x1B\x5E\xEE\xE5\x7C\xE7\x26\x2C\x1B", 16))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	
	//case2.3:MAC密钥做DES加解密
	memset(DataOut, 0, sizeof(DataOut));
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TAK, NULL, 24, DataIn, DataOut, KSN1, SEC_DES_ENCRYPT|SEC_DES_KEYLEN_DEFAULT))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//使用安装的16字节DUKPT密钥派生成MAC密钥6ef92795e5128e4dcdeb767b33f156bd密钥对3组8字节数据3333333333333333进行加密后拼接得到结果090C8A5EDB362B8D
	if(memcmp(DataOut,"\x09\x0C\x8A\x5E\xDB\x36\x2B\x8D\x09\x0C\x8A\x5E\xDB\x36\x2B\x8D\x09\x0C\x8A\x5E\xDB\x36\x2B\x8D", 24))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		GOTOERR;
	}
	memset(DataOut, 0, sizeof(DataOut));
	if((ret=NDK_SecCalcDesDukpt(6,SEC_KEY_TYPE_TAK, NULL, 24, DataIn, DataOut, KSN1, SEC_DES_DECRYPT|SEC_DES_KEYLEN_DEFAULT/*|SEC_DES_ECBMODE*/))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
	//使用安装的16字节DUKPT密钥派生成MAC密钥6ef92795e5128e4dcdeb767b33f156bd密钥对3组8字节数据3333333333333333进行解密后拼接得到结果15066CFCCBAE36FF
	if(memcmp(DataOut,"\x15\x06\x6C\xFC\xCB\xAE\x36\xFF\x15\x06\x6C\xFC\xCB\xAE\x36\xFF\x15\x06\x6C\xFC\xCB\xAE\x36\xFF", 24))
	{
		send_result("line %d:%s测试失败", __LINE__, TESTAPI);
		goto ERR;
	}
	
	//DUKPT体系没有判断短密钥是否加密长密钥 (开发解释说)
	//case3:密钥保护强度限制打开，应该不允许使用短长度的密钥安装长长度的密钥
	
	//测试通过
	send_result("%s测试通过", TESTAPI);
	
ERR://测试后置:擦除所有密钥
	NDK_SecSetCfg(punCfgInfo);
	NDK_SecKeyErase();
	return;
}

