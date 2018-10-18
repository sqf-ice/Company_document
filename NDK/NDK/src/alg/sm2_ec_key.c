#include "sm2_ec_key.h"

sm2_ec_key * sm2_ec_key_new(ec_param *ecp)
{
	sm2_ec_key *eck;
	eck = (sm2_ec_key *)OPENSSL_malloc(sizeof(sm2_ec_key));
	eck->d = BN_new();
	eck->P = xy_ecpoint_new(ecp);
	return eck;
}
void sm2_ec_key_free(sm2_ec_key *eck)
{
	if (eck)
	{
		BN_free(eck->d);
		xy_ecpoint_free(eck->P);
		OPENSSL_free(eck);
		eck = NULL;
	}
}
int sm2_ec_key_init(sm2_ec_key *eck, char *string_value, ec_param *ecp)
{
	int ret;
	int len;
	char *tmp;
	tmp = NULL;
	len = strlen(string_value);
	//如果长度较长，截取前面部分
	if (len > ecp->point_byte_length * 2)
	{
		len = ecp->point_byte_length * 2;
		tmp = (char *)OPENSSL_malloc(len + 2);
		memset(tmp, 0, len + 2);
		memcpy(tmp, string_value, len);
		BN_hex2bn(&eck->d, tmp);
		OPENSSL_free(tmp);
	}
	else
	{
		BN_hex2bn(&eck->d, string_value);
	}
	ret = xy_ecpoint_mul_bignum(eck->P, ecp->G, eck->d, ecp);

	return ret; 
}

//added wangzy
int sm2_ec_calc_pkey(char *bin_value, int bin_len, ec_param *ecp, char *pkey)
{
	int ret;
	int len;
	char *tmp;
	tmp = NULL;
	sm2_ec_key *eck;

	PDEBUG("here");
	eck = sm2_ec_key_new(ecp);

	PDEBUG("here");
	//如果长度较长，截取前面部分
	if (len > ecp->point_byte_length )
	{
		PDEBUG("here");
		len = ecp->point_byte_length ;
		tmp = (char *)OPENSSL_malloc(len + 2);
		memset(tmp, 0, len + 2);
		memcpy(tmp, bin_value, len);
		BN_bin2bn(tmp, bin_len, &eck->d);
		OPENSSL_free(tmp);
	}
	else
	{
		PDEBUG("here");
		BN_bin2bn( bin_value, bin_len, &eck->d);
	}

	PDEBUG("here");
	ret = xy_ecpoint_mul_bignum(eck->P, ecp->G, eck->d, ecp);
	if(ret != 0)
		return ret;

	PDEBUG("here");
	BN_bn2bin(eck->P->x, pkey);
	BN_bn2bin(eck->P->y, pkey+32);

	PDEBUG("here");
	sm2_ec_key_free(eck);

	return ret; 
}


