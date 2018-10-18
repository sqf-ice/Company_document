/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: À„∑®ƒ£øÈ
* file name		: alg1.c
* Author 			: wangzy
* version			:
* DATE				: 20130319
* directory 	:
* description	: ≤‚ ‘NDK_AlgTDesΩ”ø⁄π¶ƒ‹
* related document	: NDK.chm
*
************************************************************************
* log			:
* Revision
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"

/*---------------constants/macro definition---------------------*/
#define 	TESTAPI		"NDK_AlgTDes"

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
*			  	  	     wangzy         20130319  	   created
*****************************************************************/
void alg1(void)
{
	//æ÷≤ø±‰¡ø≥ı ºªØ
	uchar datain[8] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77}, dataout[8] = {0}, dataout2[8] = {0};
	uchar key8[8] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77};
	uchar key16[16] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
	uchar key24[24] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00};
	int ret = 0;
	uchar result[8] = {0xE4,0xFF,0x10,0x81,0x23,0x63,0x07,0x2D};
	uchar result2[8] = {0xE7,0xA6,0xAE,0xFD,0x18,0x04,0xD4,0x97};
	uchar result3[8] = {0x13,0x7E,0xF8,0x21,0xE1,0x52,0x6E,0xEA};

	cls_printf("≤‚ ‘%s...", TESTAPI);
	
	//case1: ∑«∑® ‰»Î≤‚ ‘
	//case1.1¥´»Î∑«∑® ‰»Î≤Œ ˝£∫NULL 
	if((ret = NDK_AlgTDes(NULL, dataout, key8, sizeof(key8), ALG_TDS_MODE_ENC)) != NDK_ERR_PARA)
	{	
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case1.2:¥´»Î∑«∑®√‹‘øµÿ÷∑£∫NULL
	if((ret = NDK_AlgTDes(datain, NULL, key8, sizeof(key8), ALG_TDS_MODE_ENC)) != NDK_ERR_PARA)
	{	
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case1.3:¥´»Î∑«∑® ‰≥ˆµÿ÷∑£∫NULL 
	if((ret = NDK_AlgTDes(datain, dataout, NULL, sizeof(key8), ALG_TDS_MODE_ENC)) != NDK_ERR_PARA)
	{	
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	//case1.4:¥´»Î∑«∑®√‹‘ø≥§∂»£∫-1,7  
	if((ret = NDK_AlgTDes(datain, dataout,key8,-1,ALG_TDS_MODE_ENC)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_AlgTDes(datain, dataout,key8,7,ALG_TDS_MODE_ENC)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
		
	//case1.5:¥´»Î∑«∑®º”√‹ƒ£ Ω
#if 1
	if((ret = NDK_AlgTDes(datain, dataout,key8,sizeof(key8),ALG_TDS_MODE_ENC-1)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if((ret = NDK_AlgTDes(datain, dataout,key8,sizeof(key8),ALG_TDS_MODE_DEC+1)) !=NDK_ERR_PARA)
	{	
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
#endif
	
 	//case2:’˝≥£≤‚ ‘
	//case2.1 8Œª√‹‘øº”Ω‚√‹
	if((ret = NDK_AlgTDes(datain, dataout, key8, sizeof(key8), ALG_TDS_MODE_ENC)) !=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(result, dataout, sizeof(dataout)))
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹", __LINE__, TESTAPI);
		RETURN;
	}
	if((ret = NDK_AlgTDes(dataout, dataout2, key8, sizeof(key8), ALG_TDS_MODE_DEC)) !=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(dataout2, datain, sizeof(dataout2)))
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹", __LINE__, TESTAPI);
		RETURN;
	}

	//case2.2 16Œª√‹‘øº”Ω‚√‹
	if((ret = NDK_AlgTDes(datain, dataout, key16, sizeof(key16), ALG_TDS_MODE_ENC)) !=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(result2, dataout, sizeof(result2)))
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹", __LINE__, TESTAPI);
		RETURN;
	}
	if((ret = NDK_AlgTDes(dataout, dataout2, key16, sizeof(key16), ALG_TDS_MODE_DEC)) !=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(dataout2, datain, sizeof(datain)))
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹", __LINE__, TESTAPI);
		RETURN;
	}

	//case2.3 24Œª√‹‘øº”Ω‚√‹
	if((ret = NDK_AlgTDes(datain, dataout, key24, sizeof(key24), ALG_TDS_MODE_ENC)) !=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(result3, dataout, sizeof(result3)))
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹", __LINE__, TESTAPI);
		RETURN;
	}
	if((ret = NDK_AlgTDes(dataout, dataout2, key24, sizeof(key24), ALG_TDS_MODE_DEC)) !=NDK_OK)
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹(%d)", __LINE__, TESTAPI, ret);
		RETURN;
	}
	if(memcmp(dataout2, datain, sizeof(datain)))
	{
		send_result("line %d:%s≤‚ ‘ ß∞‹", __LINE__, TESTAPI);
		RETURN;
	}

	//≤‚ ‘Ω· ¯
	send_result("%s≤‚ ‘Õ®π˝", TESTAPI);
	return;
}

