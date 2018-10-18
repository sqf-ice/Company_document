/************************************************************************
*
* copyright(c)2012-20xx Newland Co. Ltd. All rights reserved
* module		: 安全模块
* file name	: sec6.c
* Author 		: linrq
* version		: 
* DATE			: 20130228
* directory 		: 
* description		: 测试NDK_SecLoadRsaKey接口功能
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
#define TESTAPI	"NDK_SecLoadRsaKey"	

/*------------global variables definition-----------------------*/

/*----------global variables declaration("extern")--------------*/

/*---------------structure definition---------------------------*/

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
*****************************************************************/
/*uModulus1:对应512,sExponent=3, ID=1的公钥*/
/*uModulus2, uExponent2:对应1024,ID=2的私钥,sExponent=0x10001*/
/*uModulus3, uExponent3:公私钥对,其中bit=2048.公钥sExponent=0x10001,公钥ID=3,私钥ID=4*/
/*uModulus4: usBits=4096,sExponent=0x10001, ID=5的公钥*/
void sec15(void)
{
	int ret = 0;
	ST_SEC_RSA_KEY RsaKeyIn;
	uchar *uModulus1=(uchar *)"4FB2D08918F766B30B2144FE25D09A160FFCF256086D82773FB04D4BD7E4772F2557FC25EE130A0B984467995BE80EE1751F577BD3E5CAFF60712722F33D0479";
	uchar *uModulus2=(uchar *)"C4743F369BE32A0C171DE3FEA2329DA3E017CD3FE6F127AA4DC10FB6636591BFBF05AA3D7D549B8771A4A3B96D28832407B5F9E7A7239D3C0F7F6100CB4F807CFCB17FC6511843976234E7FDE90D71E0C52BD0A8FE8843A196E878A75D72379B90F0DD6A99A6DC807924DFDD9904A6A1C4D7FD64F917ED6E94247CD121B7F419";
	uchar *uExponent2=(uchar *)"A682D1417C07A30FF4580573420F1CC6559E56FD9EC530158D29E223794411B1303BA0FBD54AED3873FE54374A6A581D25DBFC2178ED1ACC596661D5C67D702D017D8FAE3F363A8C3D5F59F22E847C495B655D75192A3DA40A4AB89234FD1A59EBC791F26D38A6ACCCC262164613AE6006C3372E895A8B5BC7263BC8941EAF71";
	uchar *uModulus3=(uchar *)"A0BB8F525FC69D7E27E6C05350E801747B112D84479D6666FE61EC54E0D12FEC4307E4B08D89BE5B6B655E6FEA556F12C891C24B5BBDC7979A438CAB3EEEC269EFAF6E4BC820BBC07D54B3F021B161209DD011CA8C1EE1C84F4E1908139D2EEFEE6617071D9241A39A7CBF5AA83F0016DF4269A090DAB2341B9035AE92522B4E2C003D29878E3059DC5C34A47DDC856501AB40D146F40200A662C93972EC4BBDE0ECCE9D1992B99878576F71527C432280A6A45871F2630D105D0ED4C287F193D33ABEDB31FD773E665EBE496690091A3300D2CB1E431DC4424251549BD11A6767173A5F4C37C1047D84DFEB555A7D2AF25A0AD27F917B0620989E20973E7839";
	uchar *uExponent3=(uchar *)"5DCC369E46A01C6673989861C6942DCD30C85C7C96624751116049CF5A8718E55254B7E34425BEFD9095CE5F972B962F2AF6CCA0B77FEF8B0C0E217809B8C1A19AAB10BD8EA3738DA85616CFAA89DDF96FBEEDB2FAC446D4635B5B06389400C129A95004E9FFE0E83D8E5108F1A81A403EB77DF6A2B1CD4ED54AF9D8CF30BE6DE49F2F2F33D5AB39920CAD399F07C3A420DA1E77C3D43100129066E7DEEEBD6BEF1DFD80B7487219A75AF5A8901A6E541EEB2B2A29DF98E3BE45EA8E4CFDB1BEB84CB78796488B90DD134F0619D9FFAFCEB38657C365BD93C2EF4D11B1CFE8FC7FBF96A6C17524F885EEBA7D39BBA87BF9E3CFDAF07A4430F8B1A218333D2C35";
//	uchar *uModulus4=(uchar *)"68859AFC8D0BB3804C62E48A8FC8DEBC46D86184E7F5E08320A1B3E97C5C8F895A9D5ADA3DE19E447B52FF9CB830CD238537241A9BC647AA4DE54CCE774B3DF4D8FE421B8DDCCCF9D67A4FEFA7491E40C027E16BA082FD800B9975C2249E10A70AE994B28C6B72925975B3AEFD2798DD5C2BAD80F494D362650745656AE08DFE3450981BF9AFE84BD97BF4390E9110CBE44EEF57F248A9BAA0DE8A292F0EAC4F6ECCF4B2F087E2A4116E073BCCFE27DF0FF0DA0E397B11807E016C5CC8F4982015624F70042D8F885483F5C201DE5DFE30788DFBC713B5F94AD2BB58D1028FDCBD1AE916818888E65285E32E6A39B4084598645F6E4398A9673666360AE0A888B4D96CB02297DBDDC3A924AD4FBC3CF60F1AC34B5440837695AA00EC3B751BD4C6CC95C0CEEC1B2512E1769E31EE0EAC854A2DB4FD27AF91CA6477190CE71EFDFA1DB0943CF5B77BD75A69C3C7917EC5615486BEA0DDB79675C0428029297661A9D8130F5DDAEBFEA796D2EFF92DD6EEC89C0A6F33DD97388267CBA288ED3319A1897F5F408DB2F6A59527820DC64B68F40C06EC2AA6A1F5CCA3307EBEE88F6C312BEE0BF5235424A89E8B2C0E7877FA99850376DF2A1F2669AB6F3E28524B97C0BC9FD4B89A217CF5BA856611D412208DFD096176E95A30D5685C89FBEE835812094E626EDEEEABC62606D891EA3DAA586174FBD1AE0C45AE7C9B4CCCDD66F1";

	if(auto_flag==2)
	{
		//send_result("line %d:%s无纯手动测试用例", __LINE__, TESTAPI);
		return;
	}
	cls_show_msg1(2, "测试%s...", TESTAPI); 
	
	//测试前置:擦除所有密钥
	if((ret=NDK_SecKeyErase())!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		return;
	}

	//case1:非法测试
	//case1.1: pstRsakeyIn==NULL
	if((ret=NDK_SecLoadRsaKey(2, NULL))!=NDK_ERR_PARA)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#if 0  //101已合法	
	//case1.2:ucRSAKeyIndex>100
	RsaKeyIn.usBits=RSA_KEY_LEN_512;
	memset(RsaKeyIn.sModulus, 0x11, 16);
	RsaKeyIn.sExponent[0]=RSA_EXP_3;
	if((ret=NDK_SecLoadRsaKey(101, &RsaKeyIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif

	//case2:安装usBits=512,sModulus由工具生成,sExponent=3, ID=1的公钥
	memset(&RsaKeyIn, 0,sizeof(ST_SEC_RSA_KEY));
	RsaKeyIn.usBits=RSA_KEY_LEN_512;
	strcpy((char *)RsaKeyIn.sExponent, "03");
	memcpy(RsaKeyIn.sModulus, uModulus1, RsaKeyIn.usBits*2/8);
	if((ret=NDK_SecLoadRsaKey(1, &RsaKeyIn))!= NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case3:安装usBits=1024,sModulus和sExponent由工具生成, ID=2的私钥(对应公钥的指数为0x10001)
	memset(&RsaKeyIn, 0,sizeof(ST_SEC_RSA_KEY));
	RsaKeyIn.usBits=RSA_KEY_LEN_1024;
	memcpy(RsaKeyIn.sModulus, uModulus2, RsaKeyIn.usBits*2/8);
	memcpy(RsaKeyIn.sExponent, uExponent2, RsaKeyIn.usBits*2/8);
	if((ret=NDK_SecLoadRsaKey(2, &RsaKeyIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	//case4:安装公私钥对,其中bit=2048.公钥sExponent=0x10001,对应数据由工具生成。公钥ID=3,私钥ID=4
	memset(&RsaKeyIn, 0,sizeof(ST_SEC_RSA_KEY));
	RsaKeyIn.usBits=RSA_KEY_LEN_2048;
	strcpy((char *)RsaKeyIn.sExponent, "10001");
	memcpy(RsaKeyIn.sModulus, uModulus3, RsaKeyIn.usBits*2/8);
	if((ret=NDK_SecLoadRsaKey(3, &RsaKeyIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}

	memcpy(RsaKeyIn.sExponent, uExponent3, RsaKeyIn.usBits*2/8);
	if((ret=NDK_SecLoadRsaKey(4, &RsaKeyIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		goto ERR;
	}

	//case5:安装usBits=4096,sModulus由工具生成,sExponent=0x10001, ID=5的公钥
	//开发反馈不支持
#if 0
	memset(RsaKeyIn, 0,sizeof(RsaKeyIn));
	RsaKeyIn.usBits=4096;
	strcpy(RsaKeyIn.sExponent, "10001");
	memcpy(RsaKeyIn.sModulus, uModulus4, RsaKeyIn.usBits*2/8);

	if((ret=NDK_SecLoadRsaKey(5, &RsaKeyIn))!=NDK_OK)
	{
		send_result("line %d:%s测试失败(%d)", __LINE__, TESTAPI, ret);
		GOTOERR;
	}
#endif

	send_result("%s测试通过", TESTAPI);
ERR://测试后置:擦除所有密钥
	NDK_SecKeyErase();
	return;
}

