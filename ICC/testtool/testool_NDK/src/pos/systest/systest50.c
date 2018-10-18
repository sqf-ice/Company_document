/************************************************************************
*
* copyright(c)2005-2008 Newland Co. Ltd. All rights reserved
* module			: 射频/键盘交叉
* file name			: systest50.c
* Author 			: 	chensj
* version			: 
* DATE				: 
* directory 			: 
* description			:  射频/键盘交叉
* related document	: NDKapi.chm
*				  
************************************************************************
* log			: 
* Revision 1.0  20130822  chensj
* no message(created for ARM250 platform)
************************************************************************/
/*-----------------------includes-------------------------------*/
#include "mode.h"
#include "systest.h"

/*---------------constants/macro definition---------------------*/
#define	TESTITEM	"射频/键盘交叉"

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
* history 				: author			date			remarks
					 chensj 			2013/10/08     created
*****************************************************************/
void rfid_input(SMART_t type)
{
#include "kbtable.h"
	int nCode = 0, pUIDlen = 0, i = 0;
	uchar SAK[LEN_SAK] = {0}, UID[32] = {0};
#if ANDIROD_ENABLE
	uint cardcount = 0, maxcards = 20;
	iso15693ProximityCard_t cards[20];
#endif
	NDK_KbFlush();
	while(1)
	{
		cls_printf("请输入[%s]",table[i].keyname);
		if(NDK_KbHit(&nCode)!=NDK_OK)
		{
			cls_show_msg("line %d:测试失败(0x%2x)", __LINE__, nCode);
			return;
		}
		else
		{	
			if(nCode!=0)
			{
				if(nCode!=table[i].keycode)//按键键码和预期不一致则报错
				{
					cls_show_msg("line %d,%s测试失败,预期(0x%2x)实际(0x%2x)", __LINE__, TESTITEM, table[i].keycode, nCode);
					return;
				}
				else
				{
					i++;//按键键码和预期一致则测试下一个按键
					NDK_KbFlush();
				}
			}
			if(i == SZ_ARRAY(table))
			{
				if(cls_show_msg("%s测试完成,是否会感觉到卡键\n[ENTER]是,[其他]否", TESTITEM)==ENTER)
					cls_show_msg("line %d,%s测试失败", __LINE__, TESTITEM);
				else
					cls_show_msg("%s测试通过", TESTITEM);
				return;
			}
		}
			
		switch(type)
		{
		case CPU_A:
		case CPU_B:
			NDK_RfidPiccDetect(NULL);
			continue;
			break;
		case MIFARE_1:
			NDK_M1Request(REQA, &pUIDlen, UID);
			NDK_M1Anti(&pUIDlen, UID);
			NDK_M1Select(pUIDlen, UID, SAK);
			continue;
			break;
		case FELICA:
			NDK_RfidFelicaPoll(UID, &pUIDlen);
			continue;
			break;
#if M0_ENABLE 
		case MIFARE_0:			
			NDK_MifareActive(REQA, UID, pUIDlen,SAK);	
			continue;
			break;
#endif
#if ISO15693_ENABLE
		case ISO15693:
			NDK_Iso15693_Inventory(ISO15693_NUM_SLOTS_16,maxcards,&cardcount,cards);
			continue;
			break;
#endif
		default:
			cls_show_msg("未知类型的卡类");
			break;
		}	
	}
	return;
}

void systest50(void)
{
	if(auto_flag==1)//不支持自动测试
	{
		cls_show_msg_record(FILENAME ,FUNCNAME ,"%s不支持自动测试，请使用手动测试验证", TESTITEM);
		return;
	}

	int ret = 0;
	SMART_t type = CPU_A;
	char *card[] = {"CPU_A", "CPU_B", "M-1", "FELICA", "M-0", "15693"};
	
	cls_show_msg1(1, "%s测试中...", TESTITEM);

	while (1)
	{	
		ret = cls_show_msg("1.射频键盘交叉\n"
							"0.射频卡配置");
		switch(ret)
		{
		case '1':
			rfid_input(type);
			break;
		case '0':
			switch(type=select_smarttype())
			{
			case CPU_A:
			case CPU_B:
			case MIFARE_1:
			case FELICA:
			case MIFARE_0:	
			case ISO15693:
				if ((ret=RFID_init(type))!=SUCC)
					cls_show_msg("line %d:初始化失败(%d)!请检查配置是否正确.", __LINE__, ret);
				else
					cls_show_msg1(2, "%s初始化成功!", card[type]);
				break;
			default:
				cls_show_msg("line %d:配置错误!请配置RFID(%d).", __LINE__, type);
				break;
			}
			break;		
		case ESC:
			return;
			break;
		default:
			continue;
			break;
		}
	}
}

