
#ifndef SDK_VPP_H
#define SDK_VPP_H

typedef enum{
	SEC_VPP_INVALID_KEY=0,  		//��Ч��Կ,�ڲ�ʹ��.
	SEC_VPP_NOT_ACTIVE,  			//VPPû�м����һ�ε���VPPInit.
	SEC_VPP_TIMED_OUT,				//�Ѿ�����VPP��ʼ����ʱ��.
	SEC_VPP_ENCRYPT_ERROR,			//��ȷ�ϼ��󣬼��ܴ���.
	SEC_VPP_BUFFER_FULL,			//����BUFԽ�磬�������PIN̫����
	SEC_VPP_PIN_KEY,  				//���ݼ����£�����"*".
	SEC_VPP_ENTER_KEY,				//ȷ�ϼ����£�PIN����.
	SEC_VPP_BACKSPACE_KEY,			//�˸������.
	SEC_VPP_CLEAR_KEY,  			//��������£��������'*'��ʾ.
	SEC_VPP_CANCEL_KEY,  			//ȡ����������.
	SEC_VPP_GENERALERROR,  			//�ý����޷��������ڲ�����.
	SEC_VPP_CUSTOMERCARDNOTPRESENT, //IC�����γ�
	SEC_VPP_HTCCARDERROR,  			//�������ܿ�����.
	SEC_VPP_WRONG_PIN_LAST_TRY,  	//���ܿ�-���벻��ȷ������һ��.
	SEC_VPP_WRONG_PIN, 				//���ܿ�-�����һ��.
	SEC_VPP_ICCERROR,  				//���ܿ�-����̫���
	SEC_VPP_PIN_BYPASS,  			//���ܿ�-PIN��֤ͨ��,����PIN��0����
	SEC_VPP_ICCFAILURE,  			//���ܿ�-��������.
	SEC_VPP_GETCHALLENGE_BAD,  		//���ܿ�-Ӧ����90 00.
	SEC_VPP_GETCHALLENGE_NOT8,  	//���ܿ�-��Ч��Ӧ�𳤶�.
 	SEC_VPP_PIN_ATTACK_TIMER,  		//PIN������ʱ��������
 	SEC_VPP_NULL=128,				//��������¼�ΪNULL����û��ȡ���κ���Ϣ�����������Ӧ��Ҫ���ڸö���ǰ
}EM_VPP_EVENTS;

typedef enum{
	SEC_VPP_DUKPT=1,				//DUKPT session.
	SEC_VPP_DUKPT_NO_ITERATE,  		//DUKPT session, does not iterate DUKPT key.
	SEC_VPP_MASTER_SESSION,			//Master Session.
	SEC_VPP_EMV_OFFLINE_CLEARPIN,  	//EMV offline with clear PIN.
	SEC_VPP_EMV_OFFLINE_ENCPIN,		//EMV offline with encrypted password.
	SEC_VPP_EMV_PIN_VERIFY_CLEARPIN,//EMV offline with clear verification using a PIN block.pinblock����Ӧ��ʹ��indexָ������Կ���ܺ���
	SEC_VPP_EMV_PIN_VERIFY_ENCPIN,	//EMV offline with encrypted password using a PIN block.pinblock����Ӧ��ʹ��indexָ������Կ���ܺ���
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
	unsigned short  timeout;		//��ʱʱ��5~200��
	unsigned short  size;			//����ṹ�Ĵ�С��������չʹ��
	int  indexKey;					//��Կ���
	int  type;						//����
	char * pPAN;					//���˺�
	unsigned char *  pPINBlock;		//pinblock
	void *  pAD;					//�������ݣ�����dukpt��ksn��
	int  sizeAD;					//�������ݳ���
//	int  keySimulated ;				//key����ΪESC����ENTER��
	unsigned int  Exponent; 		//RSA��Կָ��
	unsigned char  SWBytes [2] ;	//У������,���硰9000��
	int  PINBlockFormat; 			//pin block��ʽ
	unsigned char *  pSCRC;			//У����룬��ʱ����
}sec_vpp_data;

#endif

