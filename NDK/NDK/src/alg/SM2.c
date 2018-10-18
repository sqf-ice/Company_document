#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "sm2.h"

/* 获取随机数结构 */
typedef struct RNG_VECTORLEN_S {
    int     len;        /*要获取的随机数长度*/
    char    *buffer;        /*随机数缓存指针*/
} RNG_VECTORLEN_ST;

#define SEC_DEVICE_NAME     "/dev/sec"  /*安全设备全路径*/
/*
 * IOCTL definitions
 */
#define SEC_IOC_MAGIC 'S'
/* RNG definitions */
#define SEC_IOCS_RNG_WORD       _IOW(SEC_IOC_MAGIC, 1, unsigned int)
#define SEC_IOCS_RNG_VECTOR     _IOW(SEC_IOC_MAGIC, 2, unsigned int)
#define SEC_IOCS_RNG_VECTORLEN  _IOW(SEC_IOC_MAGIC, 3, unsigned int)
#define SEC_IOCG_RNG_VECTORLEN  _IOR(SEC_IOC_MAGIC, 4, unsigned int)


/**
* @fn       int secp_get_rng(char *pbuf, int rnglen)
* @li       获取随机数
* @param    无
* @retval   0       成功
* @retval   <0      失败
* @attention    无
  @li 无
*/
int secp_get_rng(unsigned char *pbuf, int rnglen)
{
    int sec_fd;
    int ret;
    RNG_VECTORLEN_ST rngst;

    sec_fd = open(SEC_DEVICE_NAME, O_RDWR);


    ret = ioctl(sec_fd, SEC_IOCS_RNG_VECTORLEN, rnglen);
    if(ret < 0) {
        printf("secp_get_rng SEC_IOCS_RNG_VECTORLEN err! iret = %d", ret);
	close(sec_fd);
        return ret;
    }

    rngst.buffer = pbuf;
    rngst.len = rnglen;
    ret = ioctl(sec_fd, SEC_IOCG_RNG_VECTORLEN, &rngst);
    if(ret < 0) {
        printf("secp_get_rng SEC_IOCG_RNG_VECTORLEN err! iret = %d", ret);
	close(sec_fd);
        return ret;
    }

	close(sec_fd);

    return 0;
}

//added wangzy
void sm2_sign_e(ec_param *ecp, BYTE *e, sm2_sign_st *sign)
{
    BIGNUM *e_bn;

    BIGNUM *r;
    BIGNUM *s;
    BIGNUM *tmp1;
    BIGNUM *d;
    BIGNUM *k;
    xy_ecpoint *xy1;

    e_bn = BN_new();
    r = BN_new();
    s = BN_new();
    tmp1 = BN_new();
    d = BN_new();
    k = BN_new();
    xy1 = xy_ecpoint_new(ecp);

    BN_bin2bn(sign->private_key, ecp->point_byte_length, d);
    BN_bin2bn(sign->k, ecp->point_byte_length, k);

//e
    BN_bin2bn(e, HASH_BYTE_LENGTH, e_bn);

    xy_ecpoint_mul_bignum(xy1, ecp->G, k, ecp);
    BN_zero(r);
    BN_mod_add(r, e_bn, xy1->x, ecp->n, ecp->ctx);

    BN_one(s);
    BN_add(s, s, d);
    BN_mod_inverse(s, s, ecp->n, ecp->ctx);  //求模反

    BN_mul(tmp1, r, d, ecp->ctx);
    BN_sub(tmp1, k, tmp1);
    BN_mod_mul(s, s, tmp1, ecp->n, ecp->ctx);

    sm2_bn2bin(r, sign->r, ecp->point_byte_length);
    sm2_bn2bin(s, sign->s, ecp->point_byte_length);


    DEFINE_SHOW_BIGNUM(r);
    DEFINE_SHOW_BIGNUM(s);

    BN_free(e_bn);
    BN_free(r);
    BN_free(s);
    BN_free(tmp1);
    BN_free(d);
    BN_free(k);
    xy_ecpoint_free(xy1);
}

int  sm2_verify_e(ec_param *ecp, BYTE *e, sm2_sign_st *sign)
{
    BIGNUM *e_bn;
    BIGNUM *t;
    BIGNUM *R;
    xy_ecpoint *result;
    xy_ecpoint *result1;
    xy_ecpoint *result2;
    xy_ecpoint *P_A;
    BIGNUM *r;
    BIGNUM *s;
    BIGNUM *P_x;
    BIGNUM *P_y;
    int iret;

    e_bn = BN_new();
    t = BN_new();
    R = BN_new();
    result = xy_ecpoint_new(ecp);
    result1 = xy_ecpoint_new(ecp);
    result2 = xy_ecpoint_new(ecp);
    P_A = xy_ecpoint_new(ecp);
    r = BN_new();
    s = BN_new();
    P_x = BN_new();
    P_y = BN_new();

    BN_bin2bn(sign->r, ecp->point_byte_length, r);
    BN_bin2bn(sign->s, ecp->point_byte_length, s);
    BN_bin2bn(sign->public_key.x, ecp->point_byte_length, P_x);
    BN_bin2bn(sign->public_key.y, ecp->point_byte_length, P_y);
    xy_ecpoint_init_xy(P_A, P_x, P_y, ecp);

    BN_bin2bn(e, HASH_BYTE_LENGTH, e_bn);
    DEFINE_SHOW_BIGNUM(e_bn);

    BN_mod_add(t, r, s, ecp->n, ecp->ctx);
    xy_ecpoint_mul_bignum(result1, ecp->G, s, ecp);
    xy_ecpoint_mul_bignum(result2, P_A, t, ecp);
    xy_ecpoint_add_xy_ecpoint(result, result1, result2, ecp);

    BN_mod_add(R, e_bn, result->x, ecp->n, ecp->ctx);

    sm2_bn2bin(R, sign->R, ecp->point_byte_length);

    DEFINE_SHOW_STRING(sign->R, ecp->point_byte_length);

    iret = memcmp(sign->R, sign->r, 32);

    BN_free(e_bn);
    BN_free(t);
    BN_free(R);
    xy_ecpoint_free(result);
    xy_ecpoint_free(result1);
    xy_ecpoint_free(result2);
    xy_ecpoint_free(P_A);
    BN_free(r);
    BN_free(s);
    BN_free(P_x);
    BN_free(P_y);

    return iret;
}

int sm2_encrypt(ec_param *ecp, message_st *message_data)
{
    BIGNUM *P_x;
    BIGNUM *P_y;
    //BIGNUM *d;
    BIGNUM *k;
    xy_ecpoint *P;
    xy_ecpoint *xy1;
    xy_ecpoint *xy2;
    int pos1;
    BYTE *t;
    int i;
    sm2_hash local_C_3;

    P_x = BN_new();
    P_y = BN_new();
    k = BN_new();
    P = xy_ecpoint_new(ecp);
    xy1 = xy_ecpoint_new(ecp);
    xy2 = xy_ecpoint_new(ecp);

    BN_bin2bn(message_data->public_key.x, ecp->point_byte_length, P_x);
    BN_bin2bn(message_data->public_key.y, ecp->point_byte_length, P_y);
    BN_bin2bn(message_data->k, ecp->point_byte_length, k);

    DEFINE_SHOW_BIGNUM(k);
    xy_ecpoint_init_xy(P, P_x, P_y, ecp);
    xy_ecpoint_mul_bignum(xy1, ecp->G, k, ecp);
    DEFINE_SHOW_BIGNUM(k);
    DEFINE_SHOW_BIGNUM(xy1->x);
    DEFINE_SHOW_BIGNUM(xy1->y);
    xy_ecpoint_mul_bignum(xy2, P, k, ecp);
    DEFINE_SHOW_BIGNUM(k);
    DEFINE_SHOW_BIGNUM(P_x);
    DEFINE_SHOW_BIGNUM(P_y);

    pos1 = 0;
//    del by wangzy
//  message_data->C_1[0] = '\x04';
//  pos1 = pos1 + 1;
    BUFFER_APPEND_BIGNUM(message_data->C_1, pos1, ecp->point_byte_length, xy1->x);
    BUFFER_APPEND_BIGNUM(message_data->C_1, pos1, ecp->point_byte_length, xy1->y);

    pos1 = 0;
    BUFFER_APPEND_BIGNUM(message_data->C_2, pos1, ecp->point_byte_length, xy2->x);
    BUFFER_APPEND_BIGNUM(message_data->C_2, pos1, ecp->point_byte_length, xy2->y);

    DEFINE_SHOW_BIGNUM(xy2->x);
    DEFINE_SHOW_BIGNUM(xy2->y);

	PDEBUG_DATA("message_data_C2", message_data->C_2, ecp->point_byte_length + ecp->point_byte_length);
	
    t = KDF((BYTE *)message_data->C_2, message_data->klen_bit, ecp->point_byte_length + ecp->point_byte_length);
	PDEBUG_DATA("t", t, message_data->message_byte_length);
    for (i = 0; i < message_data->message_byte_length; i++) {
        message_data->C_2[i] = t[i] ^ message_data->message[i];
    }
    OPENSSL_free(t);

    //计算C_3
    memset(&local_C_3, 0, sizeof(local_C_3));
    BUFFER_APPEND_BIGNUM(local_C_3.buffer, local_C_3.position, ecp->point_byte_length
                         , xy2->x);
    BUFFER_APPEND_STRING(local_C_3.buffer, local_C_3.position, message_data->message_byte_length
                         , message_data->message);
    BUFFER_APPEND_BIGNUM(local_C_3.buffer, local_C_3.position, ecp->point_byte_length
                         , xy2->y);
    sm3((BYTE *)local_C_3.buffer, local_C_3.position, local_C_3.hash);
    memcpy(message_data->C_3, (char *)local_C_3.hash, HASH_BYTE_LENGTH);

    pos1 = 0;

    BUFFER_APPEND_STRING(message_data->C, pos1, ecp->point_byte_length + ecp->point_byte_length
                         , message_data->C_1);
    BUFFER_APPEND_STRING(message_data->C, pos1, HASH_BYTE_LENGTH
                         , message_data->C_3);
    BUFFER_APPEND_STRING(message_data->C, pos1, message_data->message_byte_length
                         , message_data->C_2);

    /*DEFINE_SHOW_STRING(message_data->C_2, message_data->message_byte_length);*/
    PDEBUG_DATA("ENC  C1:", message_data->C_1, 64);
    /*DEFINE_SHOW_STRING(message_data->C_2, message_data->message_byte_length);*/
    PDEBUG_DATA("ENC  C2:", message_data->C_2, message_data->message_byte_length);
    /*DEFINE_SHOW_STRING(message_data->C_2, message_data->message_byte_length);*/
    PDEBUG_DATA("ENC  C3:", message_data->C_3, 32);

    BN_free(P_x);
    BN_free(P_y);
    BN_free(k);
    xy_ecpoint_free(P);
    xy_ecpoint_free(xy1);
    xy_ecpoint_free(xy2);

    return SUCCESS;
}

int sm2_decrypt(ec_param *ecp, message_st *message_data)
{
    int pos1;
    int pos2;
    xy_ecpoint *xy1;
    xy_ecpoint *xy2;
    BIGNUM *d;
    BYTE KDF_buffer[MAX_POINT_BYTE_LENGTH * 2];
    BYTE *t;
    int i;
    sm2_hash local_C_3;
    int iret = 0;

    xy1 = xy_ecpoint_new(ecp);
    xy2 = xy_ecpoint_new(ecp);
    d = BN_new();

    pos1 = 0;
    pos2 = 0;

    BUFFER_APPEND_STRING(message_data->C_1, pos1, ecp->point_byte_length + ecp->point_byte_length
                         , &message_data->C[pos2]);
    pos2 = pos2 + pos1;
    pos1 = 0;
    BUFFER_APPEND_STRING(message_data->C_3, pos1, HASH_BYTE_LENGTH
                         , &message_data->C[pos2]);
    pos2 = pos2 + pos1;
    pos1 = 0;
    BUFFER_APPEND_STRING(message_data->C_2, pos1, message_data->message_byte_length
                         , &message_data->C[pos2]);
    pos2 = pos2 + pos1;

    /*DEFINE_SHOW_STRING(message_data->C_2, message_data->message_byte_length);*/
    PDEBUG_DATA("ENC  C1:", message_data->C_1, 64);
    /*DEFINE_SHOW_STRING(message_data->C_2, message_data->message_byte_length);*/
    PDEBUG_DATA("ENC  C2:", message_data->C_2, message_data->message_byte_length);
    /*DEFINE_SHOW_STRING(message_data->C_2, message_data->message_byte_length);*/
    PDEBUG_DATA("ENC  C3:", message_data->C_3, 32);



    BN_bin2bn(&message_data->C_1[0], ecp->point_byte_length, xy1->x);
    BN_bin2bn(&message_data->C_1[ecp->point_byte_length], ecp->point_byte_length, xy1->y);

    BN_bin2bn(message_data->private_key, ecp->point_byte_length, d);
    xy_ecpoint_init_xy(xy1, xy1->x, xy1->y, ecp);
    xy_ecpoint_mul_bignum(xy2, xy1, d, ecp);

    pos1 = 0;
    memset(KDF_buffer, 0, sizeof(KDF_buffer));
    BUFFER_APPEND_BIGNUM(KDF_buffer, pos1, ecp->point_byte_length, xy2->x);
    BUFFER_APPEND_BIGNUM(KDF_buffer, pos1, ecp->point_byte_length, xy2->y);
    DEFINE_SHOW_BIGNUM(d);
    DEFINE_SHOW_BIGNUM(xy2->x);
    DEFINE_SHOW_BIGNUM(xy2->y);

	PDEBUG_DATA("message_data-C2", KDF_buffer, ecp->point_byte_length + ecp->point_byte_length);

    t = KDF((BYTE *)KDF_buffer, message_data->klen_bit, ecp->point_byte_length + ecp->point_byte_length);
	PDEBUG_DATA("t", t, message_data->message_byte_length);
    for (i = 0; i < message_data->message_byte_length; i++) {
        message_data->decrypt[i] = t[i] ^ message_data->C_2[i];
    }

    //比较C_3
    memset(&local_C_3, 0, sizeof(local_C_3));
    BUFFER_APPEND_BIGNUM(local_C_3.buffer, local_C_3.position, ecp->point_byte_length
                         , xy2->x);
    BUFFER_APPEND_STRING(local_C_3.buffer, local_C_3.position, message_data->message_byte_length
                         , message_data->decrypt);
    BUFFER_APPEND_BIGNUM(local_C_3.buffer, local_C_3.position, ecp->point_byte_length
                         , xy2->y);
    sm3((BYTE *)local_C_3.buffer, local_C_3.position, local_C_3.hash);
    if(memcmp(message_data->C_3, (char *)local_C_3.hash, HASH_BYTE_LENGTH) != 0) {
		PDEBUG_DATA("Message C2" ,  message_data->decrypt, message_data->message_byte_length);
        PDEBUG_DATA("error C3:" ,local_C_3.hash, 32);
        iret = -1;
    }

    OPENSSL_free(t);

    xy_ecpoint_free(xy1);
    xy_ecpoint_free(xy2);
    BN_free(d);

    return iret;
}


