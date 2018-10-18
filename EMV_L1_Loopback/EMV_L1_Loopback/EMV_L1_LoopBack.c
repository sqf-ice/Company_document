#include  "NDK.h"
#include "kb.h"

#include "libdevmgr.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h> 
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <linux/input.h>

#define DEBUG
#undef DEBUG
#ifdef DEBUG
#define ic_printf(fmt, arg...)  printf(fmt, ##arg)
#else
#define ic_printf(fmt, arg...)    
#endif

#define LOOPBACK_VER	"EMV L1 2.1.0"

typedef struct 
{
	unsigned char data[512];	//  261  'CLA INS P1 P2 [Lc + Lc data] [Le]' for command or data
	int length;					// '[Le data] sw1 sw2' for response (max = 256+2)
}T_APDU;
T_APDU C_Apdu, R_Apdu;

#define TypeAPDU	R_Apdu.data[3]
#define LC			R_Apdu.data[4]
#define LE			R_Apdu.data[5]	
const uint8_t selectVisaCredit[]={0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10, 0x00};
const uint8_t SelectPse[] = {0x00, 0xA4, 0x04, 0x00, 0x0E,
		'1','P','A','Y','.','S','Y','S','.','D','D','F','0','1',
		0x00};
uint8_t ComputerLrc(uint16_t len,uint8_t *p)
{
	uint8_t lrc = 0;
	uint16_t i = 0;
	while(i <len)
		lrc ^= p[i++];
	return lrc;
}
void InitSelectVisa(T_APDU *apdu)
{
	memcpy(apdu->data,selectVisaCredit,13);
	apdu->length = 13;
}
void InitSelectPSE(T_APDU *apdu)
{
	memcpy(apdu->data,SelectPse,20);
	apdu->length = 20;
	return;
}
	
int Test_PSE_EMV(void)
{
	uint8_t rbuf[40]={0};
	int ret;
	int rece_len = 0;
	//uint8_t send_LC = 0;
	uint8_t run_num = 0;
	NDK_SysMsDelay(50);
	NDK_IccDetect(&ret);
	if((ret & 0x01) == 0x00){
		ic_printf("please insert iccard \r\n");
		NDK_ScrDispString(0,24," no card ",0);
		NDK_SysMsDelay(1500);
		return -1;
	}
	NDK_SysMsDelay(100);
	ret=NDK_IccPowerUp(ICTYPE_IC,rbuf, &rece_len);
	if(ret < 0){
		ic_printf("icc power up err %d\r\n",ret);
		NDK_ScrDispString(0,24," pow up err ",0);
		return -1;
	}
	else{
		ic_printf("icc power up succ \r\n");
		NDK_ScrDispString(0,24," pow up succ",0);
	}
	InitSelectPSE(&C_Apdu);
	C_Apdu.length -= 1;
	while(1){
		//send_LC = icc_ex.exbuf[4];
		ret = NDK_Iccrw(ICTYPE_IC,C_Apdu.length, C_Apdu.data, &R_Apdu.length, R_Apdu.data);
		if(ret < 0){
			ic_printf("apdu err %d \r\n",ret);
			NDK_ScrDispString(0,24," apdu err   ",0);
			return -1;
		}
		else{
			ic_printf("apdu succ \r\n");
			//NDK_ScrDispString(0,12," apdu succ   ",0);
		}
		if((R_Apdu.data[R_Apdu.length - 2] != 0x90) || (R_Apdu.data[R_Apdu.length - 1] != 0x00)){
			if(R_Apdu.data[R_Apdu.length - 2] == 0x61){
				memcpy(C_Apdu.data, "\x00\xc0\x00\x00",4);
				C_Apdu.data[4] = R_Apdu.data[R_Apdu.length-1];// 请求剩余数据长度
				C_Apdu.length = 5;
				continue;
			}
			break;		 
		}
		if(C_Apdu.data[4] == 0x05){
			ic_printf("succ \r\n");
			NDK_ScrDispString(0,24," succ       ",0);
			break;
		}
		/*rece len < 3*/
		if(R_Apdu.length< 3){
			break;
		}
		/*send rece data - 3*/
		memcpy(C_Apdu.data,R_Apdu.data,(R_Apdu.length- 3));       
		C_Apdu.length= R_Apdu.length- 3;
		run_num ++;
		if(run_num ==  64){
			ic_printf("succ \r\n");
			NDK_ScrDispString(0,24," succ       ",0);
			break;
		}	
	}	  
	ret = NDK_IccPowerDown(ICTYPE_IC);
	return 0;
}
int TestVisa(EM_ICTYPE emtype)
{
	int ret = 0;
	int rece_len  =0;
	uint8_t rbuf[40]={0,};
	uint8_t profil = 0;
	uint16_t DataInsize = 0;
	uint8_t *ptr;
	uint8_t sw1,sw2; 

	NDK_ScrClrs();
	NDK_SysMsDelay(50);
	NDK_IccSetType(ICTYPE_IC);	//set EMV mode
	if(emtype == ICTYPE_IC){
		NDK_IccDetect(&ret);
		if((ret & 0x01) == 0x00){
			ic_printf("please insert iccard \r\n");
			NDK_ScrDispString(0,24," no card     ",0);
			NDK_SysMsDelay(1500);
			return -1;
		}
	}
	NDK_SysMsDelay(100);
	ret=NDK_IccPowerUp(emtype,rbuf, &rece_len);
	if(ret < 0){
		ic_printf("icc power up err %d\r\n",ret);
		NDK_ScrDispString(0,24," pow up err ",0);
		return -1;
	}
	else{
		ic_printf("icc power up succ \r\n");
		NDK_ScrDispString(0,24," pow up succ ",0);
	}
	NDK_SysMsDelay(10);//add by qiut in 2017.03.31 for after power up delay 100etus
	InitSelectVisa(&C_Apdu);
	while(1){
		ret = NDK_Iccrw(emtype,C_Apdu.length, C_Apdu.data, &R_Apdu.length, R_Apdu.data);
		if(ret < 0){
			ic_printf("apdu err %d \r\n",ret);
			//NDK_ScrDispString(0,12," apdu err      ",0);
			break;
		}
		else{
			;//ic_printf("icc power up succ \r\n");
		}
		sw1 = R_Apdu.data[R_Apdu.length - 2];
		sw2 = R_Apdu.data[R_Apdu.length - 1];
		if((sw1 == 0x6A) && (sw2 == 0x82)){
			ic_printf("apdu over \r\n");
			NDK_ScrDispString(0,24," apdu over      ",0);
			break;
		}
		if(R_Apdu.length < 8){
			InitSelectVisa(&C_Apdu);
		}
		else{
			memcpy(C_Apdu.data,R_Apdu.data,3);			
			C_Apdu.length = 4;			
			ptr = C_Apdu.data + 4;			
			if((TypeAPDU > 2) && LC){				
				DataInsize = LC;				
				C_Apdu.length += (1 + DataInsize);				
				*ptr++ =DataInsize;				
				profil = 0;				
					while(DataInsize--){					
					*ptr++ = profil++;				
					}			
			}			
			if((TypeAPDU == 2) || (TypeAPDU == 4)){				
				C_Apdu.length++;				
				*ptr++ = LE;			
			}		
		}
		C_Apdu.data[3] = ComputerLrc(R_Apdu.length,R_Apdu.data);
			
	}
	ret = NDK_IccPowerDown(emtype);
	return 0;
}
int test_loopback(void)
{
	int ret = 0;
	 int rece_len = 0;
	 uint8_t rbuf[40] = {0,};
	 NDK_IccDetect(&ret);
	 if ((ret & 0x01) == 0x00){
	    ic_printf("please insert iccard \r\n");
	    NDK_ScrDispString(0, 24, " no card     ", 0);
	    NDK_SysMsDelay(100);
	    return -1;
	  }
	  //NDK_ScrDispString(0, 12, " delay 10s   ", 0);
	  //NDK_SysDelay(10);
	  ret = NDK_IccPowerUp(ICTYPE_IC,rbuf, &rece_len);
	  if(ret < 0){
	      ic_printf("icc power up err %d\r\n",ret);
	      NDK_ScrDispString(0, 24, " pow up err ", 0);
	      return -1;
	  }
	  else{
	     NDK_ScrDispString(0, 24, " pow up succ ", 0);
	  }
	  NDK_SysMsDelay(10);//add by qiut in 2017.03.31 for after power up delay 100etus
	  InitSelectPSE(&C_Apdu);
	  while(1){
		  ret = NDK_Iccrw(ICTYPE_IC, C_Apdu.length, C_Apdu.data,
		          &R_Apdu.length, R_Apdu.data);
		  if(ret < 0){
			  ic_printf("apdu err %d \r\n",ret);
			  NDK_ScrDispString(0, 24, " apdu err      ", 0);
			  InitSelectPSE(&C_Apdu);
			  break;
		  }
		  else{
			 if(R_Apdu.length < 6) InitSelectPSE(&C_Apdu);
			 else{
				 if(R_Apdu.data[1] == 0x70){
					 ic_printf("apdu over \r\n");
					 NDK_ScrDispString(0, 24, " apdu over      ", 0);
					 break;
				 }
				 else{
					 memcpy(C_Apdu.data,R_Apdu.data,R_Apdu.length - 2);
					 C_Apdu.length = R_Apdu.length - 2;
				 }
			 }
			 NDK_SysMsDelay(200);
		 }
	 }
	 ret = NDK_IccPowerDown(ICTYPE_IC);
	 NDK_ScrDispString(0, 24, "pow down      ", 0);
	 return 0;
}
/*when return 1 then quit test,other return 0*/
int get_quit_test(void)
{
	int key = 0;
	//NDK_KbGetCode(0, &key);
	NDK_KbHit(&key);
	if(key == ESC)return 1;
	else return 0;
}
void emv_hardware_test(void)
{
	int i = 0;
	ic_printf("start hardware test \n");
	while(1){
		Test_PSE_EMV();
		NDK_SysMsDelay(500);
		for(i = 0;i < 25;i++){
			NDK_SysMsDelay(100);
			if(get_quit_test()){
				i = -1;
				ic_printf("quit harware test \n");
				break;
			}
		}
		if(i == -1)break;
	}
}
void emv_protocol_test(void)
{
	int ret = 0;
	int i = 0;
	int key;
	EM_ICTYPE card_type = ICTYPE_M_1;
	ic_printf("start protocol test \n");
	while(1){
		NDK_ScrClrs();
		NDK_ScrPrintf(" loopback version : %s\n", LOOPBACK_VER);
		NDK_ScrPrintf(" 请选择本次测试的卡：\n");
		NDK_ScrPrintf(" 1、IC\n");
		NDK_ScrPrintf(" 2、SAM1\n");
		NDK_ScrPrintf(" 3、SAM2\n");
		NDK_ScrPrintf(" 4、SAM3\n");
		NDK_KbGetCode(0, &key);
		switch(key){
		case ONE: 	card_type = ICTYPE_IC; break;
		case TWO: 	card_type = ICTYPE_SAM1; break;
		case THREE: card_type = ICTYPE_SAM2; break;
		case FOUR: 	card_type = ICTYPE_SAM3; break;
		default:	card_type = ICTYPE_M_1;	break;
		}

		if(card_type != ICTYPE_M_1) break;
	}

	while(1){
		ret = TestVisa(card_type);
		NDK_SysMsDelay(500);
		for(i = 0;i < 25;i++){
			NDK_SysMsDelay(100);
			if(get_quit_test()){
				i = -1;
				ic_printf("quit protocol test \n");
				break;
			}
		}
		if(i == -1)break;
	}
}
void emv_protocol_loopbak(void)
{
	int i = 0;
	int key = 0;
	int time = 0;
	ic_printf("start protocol_loopback test \n");
	NDK_ScrDispString(0, 12, "input delay s 00-99 ", 0);
	while(1){
		NDK_KbGetCode(0,&key);
		if((key >= 0x30) && (key <= 0x39)){
			key -= 0x30;
			time = key;
			NDK_ScrClrLine(24,60);
			NDK_ScrPrintf("%d", key);
		}
		else{
			NDK_ScrDispString(0, 24, "input err data ", 0);
			continue;
		}
		NDK_KbGetCode(0,&key);
		if((key >= 0x30) && (key <= 0x39)){
			key -= 0x30;
			time *= 10;
			time += key;
			NDK_ScrPrintf("%d", key);
			break;
		}
		else{
			NDK_ScrDispString(0, 24, "input err data ", 0);
			continue;
		}
	}
	NDK_ScrClrs();
	NDK_ScrPrintf("loopbacking...%d",time);
	time *= 10;
	while (1) {
	   test_loopback();
	   for (i = 0;i < time;i++){
	       NDK_SysMsDelay(100);
	       if(get_quit_test()){
	            i = -1;
	            ic_printf("quit protocol test \n");
	            break;
	        }
	    }
	    if (i == -1) break;
	}
}
void emv_test(void)
{
	int key=0;
	char ver[30];
	
	while(1){
		NDK_ScrClrs();
    	NDK_ScrPrintf(" 1.loop_back\n");
//    	NDK_ScrPrintf(" 2.protocol\n");
		ic_printf(" 1.loop_back \n");
//		ic_printf(" 2.protocol \n");
		NDK_KbGetCode(0,&key);
		switch(key){
			case ONE :
				NDK_ScrClrs();
				NDK_ScrPrintf("loopbacking...\n");
				emv_protocol_loopbak();
			break;
			case TWO :
				NDK_ScrClrs();
	    		NDK_ScrPrintf(" protocoling...\n");
				ic_printf(" protocoling... \n");
				emv_protocol_test();
			break;
			case THREE:
				NDK_IccGetVersion(ver);
				NDK_ScrClrs();
				NDK_ScrPrintf(" Version:\n  %s\n", ver);
				NDK_KbGetCode(3,&key);
			break;
			case ESC :
				NDK_ScrClrs();
				NDK_ScrPrintf(" quit test! \n");
				ic_printf("quit test \n");
				return ;
			break;
			default:	
			break;
		}
	}
	
}
int main(int arg_t,char **arg_v)
{
	int Oldauto = 0;
	 /*初始化应用界面*/
    if (NDK_ScrInitGui() != NDK_OK){
        return -1;
    }

	NDK_ScrAutoUpdate(1, &Oldauto);
	NDK_ScrClrs();
	NDK_ScrPrintf("IC EMV Level-1测试...");
	NDK_SysMsDelay(5);
	emv_test();
	return 0;
}
