
#ifndef SDK_VPP_H
#define SDK_VPP_H

typedef enum{
	SEC_VPP_INVALID_KEY=0,  		//无效密钥,内部使用.
	SEC_VPP_NOT_ACTIVE,  			//VPP没有激活，第一次调用VPPInit.
	SEC_VPP_TIMED_OUT,				//已经超过VPP初始化的时间.
	SEC_VPP_ENCRYPT_ERROR,			//按确认键后，加密错误.
	SEC_VPP_BUFFER_FULL,			//输入BUF越界，（键入的PIN太长）
	SEC_VPP_PIN_KEY,  				//数据键按下，回显"*".
	SEC_VPP_ENTER_KEY,				//确认键按下，PIN处理.
	SEC_VPP_BACKSPACE_KEY,			//退格键按下.
	SEC_VPP_CLEAR_KEY,  			//清除键按下，清除所有'*'显示.
	SEC_VPP_CANCEL_KEY,  			//取消键被按下.
	SEC_VPP_GENERALERROR,  			//该进程无法继续。内部错误.
	SEC_VPP_CUSTOMERCARDNOTPRESENT, //IC卡被拔出
	SEC_VPP_HTCCARDERROR,  			//访问智能卡错误.
	SEC_VPP_WRONG_PIN_LAST_TRY,  	//智能卡-密码不正确，重试一次.
	SEC_VPP_WRONG_PIN, 				//智能卡-最后尝试一次.
	SEC_VPP_ICCERROR,  				//智能卡-重试太多次
	SEC_VPP_PIN_BYPASS,  			//智能卡-PIN验证通过,并且PIN是0长度
	SEC_VPP_ICCFAILURE,  			//智能卡-致命错误.
	SEC_VPP_GETCHALLENGE_BAD,  		//智能卡-应答不是90 00.
	SEC_VPP_GETCHALLENGE_NOT8,  	//智能卡-无效的应答长度.
 	SEC_VPP_PIN_ATTACK_TIMER,  		//PIN攻击定时器被激活
 	SEC_VPP_NULL=128,				//虚拟键盘事件为NULL，即没获取到任何信息，有添加类型应该要加在该定义前
}EM_VPP_EVENTS;

typedef enum{
	SEC_VPP_DUKPT=1,				//DUKPT session.
	SEC_VPP_DUKPT_NO_ITERATE,  		//DUKPT session, does not iterate DUKPT key.
	SEC_VPP_MASTER_SESSION,			//Master Session.
	SEC_VPP_EMV_OFFLINE_CLEARPIN,  	//EMV offline with clear PIN.
	SEC_VPP_EMV_OFFLINE_ENCPIN,		//EMV offline with encrypted password.
	SEC_VPP_EMV_PIN_VERIFY_CLEARPIN,//EMV offline with clear verification using a PIN block.pinblock是由应用使用index指定的密钥加密后传入
	SEC_VPP_EMV_PIN_VERIFY_ENCPIN,	//EMV offline with encrypted password using a PIN block.pinblock是由应用使用index指定的密钥加密后传入
	SEC_VPP_INVALID_SESSION,		//For parameter testing purposes.
}EM_SEC_VPP;

typedef enum{
	SEC_PINBLKFMT_0,  				//ANSI X9.8 Format 0 PIN Block (most widely used)
	SEC_PINBLKFMT_1,  				//ANSI X9.8 Format 1 PIN Block.
	SEC_PINBLKFMT_2,				//ANSI X9.8 Format 2 PIN Block.
	SEC_PINBLKFMT_3,  				//ANSI X9.8 Format 3 PIN Block.
	SEC_PINBLKFMT_IRREV,  			//Spanish specific PIN Block format.
	SEC_PINBLKFMT_SM4_1,
	SEC_PINBLKFMT_SM4_2,
	SEC_PINBLKFMT_SM4_3,
	SEC_PINBLKFMT_SM4_4,
	SEC_PINBLKFMT_SM4_5,
	SEC_PINBLKFMT_MAX,
}EM_SEC_PINBLKFMT;

typedef enum{
	KEY_SIMULATED_NULL=0,
	KEY_SIMULATED_ESC,
	KEY_SIMULATED_ENTER,
}EM_KEY_SIMULATED;

typedef enum{
    SEC_VPP_TYPE_NDK=0,
    SEC_VPP_TYPE_SDK=(1<<0),
    SEC_VPP_TYPE_MASK=(1<<0),
    SEC_VPP_TYPE_STATUS_DISABLE=(0<<1),
    SEC_VPP_TYPE_STATUS_ENABLE=(1<<1),
    SEC_VPP_TYPE_STATUS_MASK=(1<<1),
}EM_SEC_VPP_TYPE;

typedef struct{
	unsigned short  timeout;		//超时时间5~200秒
	unsigned short  size;			//这个结构的大小，后期扩展使用
	int  indexKey;					//密钥序号
	int  type;						//类型
	char * pPAN;					//主账号
	unsigned char *  pPINBlock;		//pinblock
	void *  pAD;					//附加数据，例如dukpt的ksn号
	int  sizeAD;					//附加数据长度
//	int  keySimulated ;				//key仿真为ESC键或ENTER键
	unsigned int  Exponent; 		//RSA公钥指数
	unsigned char  SWBytes [2] ;	//校验命令,例如“9000”
	int  PINBlockFormat; 			//pin block格式
	unsigned char *  pSCRC;			//校验代码，暂时不用
}sec_vpp_data;

#endif

