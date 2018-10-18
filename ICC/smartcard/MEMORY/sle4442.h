#ifndef _SLE4442_H_
#define _SLE4442_H_
//定义对各存储器的操作命令   


 #define  RD_MAIN_RAM         0X30       //读主存储器 ic卡模式:outgoing data   
 #define WR_MAIN_RAM          0X38       //写主存储器 ic卡模式:processing   
 #define RD_P_RAM             0X34      //读保护存储器 ic卡模式:outgoing data   
 #define WR_P_RAM             0X3C      //写保护存储器 ic卡模式:processing   
 #define RD_PSC_RAM           0X31        //读安全存储器 ic卡模式:outgoing data   
 #define WR_PSC_RAM           0X39        //写安全存储器 ic卡模式:processing   
 #define COMP_PSC_RAM         0X33      //比较安全存储器 ic卡模式:processing 
 
 extern int sle4442_poweron(unsigned char *atrbuf);
 extern int sle4442_pro_main(const unsigned char *sendbuf,unsigned char *rcvlen,unsigned char *recebuf);

#endif


