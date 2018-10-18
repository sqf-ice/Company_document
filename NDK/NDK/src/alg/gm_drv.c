#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "NDK.h"
//#include "sec_provider.h"
#include "gm_drv.h"

/*#define PDEBUG(fmt, args...) printf("[%s][%d]"fmt"\n",__FILE__,__LINE__, ##args);//udelay(10*1000);*/
#define PDEBUG(fmt, args...)

/**
 * 密钥驱动操作相关全局变量
 */
static int gm_fd = -1;		/*密钥驱动设备文件句柄*/
unsigned char IOBuf[512];


/**
* @fn		int secp_open(void)
* @li		打开密钥驱动设备文件
* @param	无
* @retval	0		成功
* @retval 	<0		失败
* @attention
  @li 无
*/
int gm_open(void)
{
	if (gm_fd >= 0)
		return 0;
	gm_fd = open(GM_DEVICE_NAME, O_RDWR);
	if (gm_fd < 0)
		return NDK_ERR_OPEN_DEV;
	return 0;
}

/**
* @fn		int secp_close(void)
* @li		关闭密钥驱动设备文件
* @param	无
* @retval	0		成功
* @retval 	<0		失败
* @attention
  @li 无
*/
int gm_close(void)
{
	if (gm_fd < 0)
		return 0;
	if (gm_fd >= 0)
		close(gm_fd);
	gm_fd = NDK_ERR_OPEN_DEV;
	return 0;
}

int gm_chip_init(void)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}

	return ioctl(gm_fd, GM_IOCT_CHIP_INIT, NULL);
}

int gm_chip_very(void)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}

	return ioctl(gm_fd, GM_IOCT_CHIP_VERY, NULL);
}

int gm_get_random(GM_GET_RANDOM *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}

	return ioctl(gm_fd, GM_IOCG_RNG_LEN, param);
}

int gm_gen_sm2key(GM_KEY *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}

	return ioctl(gm_fd, GM_IOCT_CIPHER_GENKEY_SM2, param);
}

int gm_cipher_gen_key(GM_KEY *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_CIPHER_GEN_KEY, param);
}

int gm_cipher_key_input(GM_KEY *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_CIPHER_INPUT_KEY, param);
}

int gm_cipher_key_output(GM_KEY *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_CIPHER_OUTPUT_KEY, param);
}

int gm_hash_init(unsigned char Mode)
{
return 0;
}
int gm_hash_start(GM_HASH_START *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_HASH_START, param);
}

//缺陷只能单次传入64字节
int gm_hash_updata(GM_ALG *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_HASH_UPDATA, param);
}
int gm_hash_final(GM_ALG *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_HASH_FINAL, param);
}

int gm_cipher_oper(GM_ALG *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_CIPHER_OPERATION, param);
}

int gm_cipher_key_select(GM_KEY *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_CIPHER_SELECT_KEY, param);
}

#if 0
int gm_cipher_key_input(GM_KEY *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_CIPHER_INPUT_KEY, param);
}

int gm_cipher_key_output(GM_KEY *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_CIPHER_OUTPUT_KEY, param);
}
#endif

int gm_cipher_rootkey_enc(CIPHER_SM4_INTER_KEY *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_CIPHER_ROOTKEY_ENC, param);
}

int gm_cipher_set_tmpkey(GM_KEY  *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_CIPHER_SET_TMPKEY, param);
}

int gm_cipher_get_tmpkey(GM_KEY *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_CIPHER_GET_TMPKEY, param);
}

int gm_cipher_rsa_init(unsigned char param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_CIPHER_RSA_INIT, param);
}

int gm_cipher_sm2_calc(GM_ALG *param)
{
	if(gm_open() < 0){
		PDEBUG("here");
		return NDK_ERR_OPEN_DEV;
	}
	
	return ioctl(gm_fd, GM_IOCT_CIPHER_SM2_CALC, param);
}

