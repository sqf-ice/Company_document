/*
 * �´�½��˾ ��Ȩ����(c) 2011-2015
 *
 * modemģ��-v80Э��
 * ��    �ߣ�    ��Ʒ������
 * ��    �ڣ�    2013-12-10
 * ��    ����    V1.00
 * ����޸��ˣ�
 * ����޸����ڣ�
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "modem.h"
#include "mdm_v80.h"
#include "mdm_debug.h"

static const char s_V80EMChar[V80_EM_CHAR_NUM] = {
    V80_ESCAPE_EM19, V80_ESCAPE_EM99, V80_ESCAPE_DC1, V80_ESCAPE_DC3
};

//���ʱescת����
static const char s_V80ChTransTable[V80_EM_CHAR_NUM][V80_EM_CHAR_NUM + 1] = {
    { 0x5d, 0xa4, 0xa5, 0xa6, 0x5c },       //0x19 0x19, 0x19 0x99, 0x19 0x11, 0x19 0x13, 0x19
    { 0xa7, 0x77, 0xa8, 0xa9, 0x76 },       //0x99 0x19, 0x99 0x99, 0x99 0x11, 0x99 0x13, 0x99
    { 0xaa, 0xab, 0xa2, 0xac, 0xa0 },       //0x11 0x19, 0x11 0x99, 0x11 0x11, 0x11 0x13, 0x11
    { 0xad, 0xae, 0xaf, 0xa3, 0xa1 },       //0x13 0x19, 0x13 0x99, 0x13 0x11, 0x13 0x13, 0x13
};

static const ST_CTRLBYTE_OFFSET ctrlbyte_offset[] = {
    { V80_CMD_MARK,   0  },
    { V80_CMD_FLAG,   0  },
    { V80_CMD_ERR,    0  },
    { V80_CMD_HUNT,   0  },
    { V80_CMD_UNDER,  0  },
    { V80_CMD_TOVER,  0  },
    { V80_CMD_ROVER,  0  },
    { V80_CMD_RESUME, 0  },
    { V80_CMD_BNUM,   2  },
    { V80_CMD_UNUM,   2  },
    { V80_CMD_EOT,    0  },
    { V80_CMD_ECS0,   0  },
    { V80_CMD_RRN,    0  },
    { V80_CMD_RTN,    0  },
    { V80_CMD_RATE,   2  },
    { 0,          -1 }
};

/********��ƫ�Ʒ���ƫ����;��ƫ�Ʒ���0;����δ֪����-1*********/
int get_ctrlbyte_offset(char c)
{
    int i = 0;

    while (ctrlbyte_offset[i].CtrlByte) {
        if (ctrlbyte_offset[i].CtrlByte == c)
            break;
        else
            i++;
    }
    return ctrlbyte_offset[i].offset;
}

static int get_emchar_offset(char c)
{
    int i;

    for (i = 0; i < V80_EM_CHAR_NUM; i++)
        if (c == s_V80EMChar[i])
            break;

    return i;
}

static int change_emchar(char *in, int inlen, char *out, int maxlen)
{
    int off1, off2;
    int i, outlen = 0;

    for (i = 0; i < inlen; i++) {
        if (outlen >= maxlen)
            return -1;
        if ((off1 = get_emchar_offset(in[i])) < V80_EM_CHAR_NUM) { //Ϊ��ת���ַ�
            out[outlen] = V80_ESCAPE_EM19;
            outlen++;
            if (outlen >= maxlen)
                return -1;
            if (i + 1 < inlen) {
                if ((off2 = get_emchar_offset(in[i + 1])) < V80_EM_CHAR_NUM) //�����ַ�Ҳ����ת���ַ�
                    i++;
            } else {
                off2 = V80_EM_CHAR_NUM;
            }
            out[outlen] = s_V80ChTransTable[off1][off2];
            outlen++;
        } else { //�����ַ�
            out[outlen] = in[i];
            outlen++;
        }
    }
    return outlen;
}

/********
 * >0:ת�����������
 * =0:����ת��
 * <0:������泤�Ȳ���
 *********/
static int get_emchar(char c, char *buf, int buflen)
{
    int i, j, bufoff = 0;

    if (buflen) {
        for (i = 0; i < V80_EM_CHAR_NUM; i++) {
            for (j = 0; j < V80_EM_CHAR_NUM + 1; j++) {
                if (c == s_V80ChTransTable[i][j]) {
                    buf[bufoff++] = s_V80EMChar[i];
                    buflen--;
                    if (j == V80_EM_CHAR_NUM) {
                        return bufoff;
                    } else {
                        if (buflen) {
                            buf[bufoff++] = s_V80EMChar[j];
                            buflen--;
                            return bufoff;
                        } else {
                            return -1;
                        }
                    }
                }
            }
        }
        return 0;
    }
    return -1;
}

int mdm_sdlc_pack(ST_SDLC_V80 *pstSdlc)
{
    int off = 0;
    char tmp;
    int len, j;

    switch (pstSdlc->type) {
        case SDLC_FRAME_TYPE_UA:
            if (pstSdlc->outmaxlen < 4)
                return MDM_ERR_SDLC_PACK_PARA;
            pstSdlc->outbuf[off++] = SDLC_CTRL_START;
            pstSdlc->outbuf[off++] = SDLC_CTRL_UA;
            pstSdlc->outbuf[off++] = V80_ESCAPE_EM19;
            pstSdlc->outbuf[off++] = V80_CMD_FLAG;
            pstSdlc->outlen = off;
            V80DEBUG("call %s %d pstSdlc->outbuf[%d](HEX):\n\r", __func__, __LINE__, pstSdlc->outlen);
            for (j = 0; j < pstSdlc->outlen; j++)
                V80DEBUG("%02x ", pstSdlc->outbuf[j]);
            V80DEBUG("\r\n");
            break;

        case SDLC_FRAME_TYPE_RR:
	case SDLC_FRAME_TYPE_REJ:
            if (pstSdlc->outmaxlen < 5)
                return MDM_ERR_SDLC_PACK_PARA;
            pstSdlc->outbuf[off++] = SDLC_CTRL_START;
			if (pstSdlc->type == SDLC_FRAME_TYPE_REJ)
				tmp = ((pstSdlc->recno & 0x07) << 5) | 0x19; //Nr+P��־(0x10)+MODE(RR)(0x00)+S֡(0x01)������֡��
			else 
            	tmp = ((pstSdlc->recno & 0x07) << 5) | 0x11; //Nr+P��־(0x10)+MODE(RR)(0x00)+S֡(0x01)������֡��
            if ((len = change_emchar(&tmp, 1, pstSdlc->outbuf + off, pstSdlc->outmaxlen - off - 2)) <= 0)
                return MDM_ERR_SDLC_PACK_DATALEN;
            off += len;
            pstSdlc->outbuf[off++] = V80_ESCAPE_EM19;
            pstSdlc->outbuf[off++] = V80_CMD_FLAG;
            pstSdlc->outlen = off;
            V80DEBUG("call %s %d pstSdlc->outbuf[%d](HEX):\n\r", __func__, __LINE__, pstSdlc->outlen);
            for (j = 0; j < pstSdlc->outlen; j++)
                V80DEBUG("%02x ", pstSdlc->outbuf[j]);
            V80DEBUG("\r\n");
            break;

        case SDLC_FRAME_TYPE_I:
            if ((pstSdlc->outmaxlen < 5) || (pstSdlc->inlen <= 0))
                return MDM_ERR_SDLC_PACK_PARA;
            V80DEBUG("call %s %d pstSdlc->inbuf[%d](HEX):\n\r", __func__, __LINE__, pstSdlc->inlen);
            for (j = 0; j < pstSdlc->inlen; j++)
                V80DEBUG("%02x ", pstSdlc->inbuf[j]);
            V80DEBUG("\r\n");
            pstSdlc->outbuf[off++] = SDLC_CTRL_START;
            tmp = ((pstSdlc->recno & 0x07) << 5) | ((pstSdlc->sendno & 0x07) << 1) | 0x10; //NR+SR+P��׼(0x10)+I֡(0x00)
            if ((len = change_emchar(&tmp, 1, pstSdlc->outbuf + off, pstSdlc->outmaxlen - off - 2)) <= 0)
                return MDM_ERR_SDLC_PACK_DATALEN;
            off += len;
            if ((len = change_emchar(pstSdlc->inbuf, pstSdlc->inlen, pstSdlc->outbuf + off, pstSdlc->outmaxlen - off - 2)) <= 0)
                return MDM_ERR_SDLC_PACK_DATALEN;
            off += len;
            pstSdlc->outbuf[off++] = V80_ESCAPE_EM19;
            pstSdlc->outbuf[off++] = V80_CMD_FLAG;
            pstSdlc->outlen = off;
            V80DEBUG("call %s %d pstSdlc->outbuf[%d](HEX):\n\r", __func__, __LINE__, pstSdlc->outlen);
            for (j = 0; j < pstSdlc->outlen; j++)
                V80DEBUG("%02x ", pstSdlc->outbuf[j]);
            V80DEBUG("\r\n");
            break;

        default:
            return MDM_ERR_SDLC_PACK_PARA;
    }
    return MDM_OK;
}

/***
 *   ����ֵӦ����inbuf�б�������������ݳ��ȡ�
 ***/
int mdm_sdlc_unpack(ST_SDLC_V80 *pstSdlc)
{
    int off, ret, i = 0, j;
    char *tmpdata = NULL;
    int tdatamaxlen = (pstSdlc->outmaxlen) + 1;

    pstSdlc->type = SDLC_FRAME_TYPE_ERR;
    if ((pstSdlc->inlen <= 0) || (pstSdlc->inbuf == NULL))
        return MDM_ERR_SDLC_UNPACK_PARA;
    V80DEBUG("call %s %d pstSdlc->inbuf[%d](HEX):\n\r", __func__, __LINE__, pstSdlc->inlen);
    for (j = 0; j < pstSdlc->inlen; j++)
        V80DEBUG("%02x ", pstSdlc->inbuf[j]);
    V80DEBUG("\r\n");
    //����ͷ��
    for (off = 0; off < pstSdlc->inlen; off++) {
        //��⵽��ʼ��־
        if (pstSdlc->inbuf[off] == SDLC_CTRL_START) {
            off++;
            break;
        } else if (pstSdlc->inbuf[off] == V80_ESCAPE_EM19) { //ת���ַ�
            off++;
            if (off == pstSdlc->inlen) {
                return 0;
            } else if (pstSdlc->inbuf[off] == V80_CMD_ERR) {        //��19 B2֡���ж��Ƿ�����Ч֡
                if (off >= 2)                                   //���19 B2ǰ����������˵���Ǵ��������֡��
                    pstSdlc->type = SDLC_FRAME_TYPE_INVALILD;
                return off + 1;
            } else if ((ret = get_ctrlbyte_offset(pstSdlc->inbuf[off])) >= 0) { //��������������
                off += ret;
                if (off >= pstSdlc->inlen)
                    return 0;
                else
                    return off + 1;
            }
        }
    }
    if (off == pstSdlc->inlen)
        return 0;
    if ((tmpdata = malloc(tdatamaxlen)) == NULL)
        return MDM_ERR_SDLC_UNPACK_PARA;
    memset(tmpdata, 0, tdatamaxlen);
    for (; off < pstSdlc->inlen; off++) {
        if (pstSdlc->inbuf[off] == V80_ESCAPE_EM19) { //ת���ַ�
            off++;
            if (off == pstSdlc->inlen) {
                free(tmpdata);
                return 0;
            } else if (pstSdlc->inbuf[off] == V80_CMD_FLAG) {       //��Ч����
                break;
            } else if (pstSdlc->inbuf[off] == V80_CMD_ERR) {        //��19 B2֡���ж��Ƿ�����Ч֡
                if (off >= 2)                                   //���19 B2ǰ����������˵���Ǵ��������֡��
                    pstSdlc->type = SDLC_FRAME_TYPE_INVALILD;
                free(tmpdata);
                return off + 1;
            } else if ((ret = get_emchar(pstSdlc->inbuf[off], tmpdata + i, tdatamaxlen - i)) != 0) { // ת��
                if (ret > 0) {
                    i += ret;
                } else {
                    free(tmpdata);
                    return MDM_ERR_SDLC_UNPACK_DATALEN;
                }
            } else if ((ret = get_ctrlbyte_offset(pstSdlc->inbuf[off])) >= 0) { //��������������
                off += ret;
                free(tmpdata);
                if (off >= pstSdlc->inlen)
                    return 0;
                else
                    return off + 1;
            }
        } else {
            if (i < tdatamaxlen) {
                tmpdata[i++] = pstSdlc->inbuf[off];
            } else {
                free(tmpdata);
                return MDM_ERR_SDLC_UNPACK_DATALEN;
            }
        }
    }
    if (off == pstSdlc->inlen) {
        free(tmpdata);
        return 0;
    }

    V80DEBUG("call %s %d tmpdata[%d](HEX):\n\r", __func__, __LINE__, i);
    for (j = 0; j < i; j++)
        V80DEBUG("%02x ", tmpdata[j]);
    V80DEBUG("\r\n");
    //�ж�֡����
    if (0x01 == (tmpdata[0] & 0x0F)) { //RR֡
        pstSdlc->type = SDLC_FRAME_TYPE_RR;
        pstSdlc->recno = ((tmpdata[0]) & 0xE0) >> 5;
        mdmprint("RECEIVE RR [%d]\n\r", pstSdlc->recno);
    } else if (0x00 == (tmpdata[0] & 0x01)) { //I֡
        pstSdlc->type = SDLC_FRAME_TYPE_I;
        pstSdlc->recno = ((tmpdata[0]) & 0xE0) >> 5;
        pstSdlc->sendno = ((tmpdata[0]) & 0x0E) >> 1;
        pstSdlc->outlen = i - 1;                                //i-1,�������������ݳ��ȣ���ΪҪ��ȥ tmpdata[0] ���֡��λ
        memcpy(pstSdlc->outbuf, tmpdata + 1, pstSdlc->outlen);  //���� tmpdata[0] ���֡��λ
        mdmprint("RECEIVE I [%d:%d]:[%d]\n\r", pstSdlc->sendno, pstSdlc->recno, pstSdlc->outlen);
    } else if (SDLC_CTRL_SNRM == tmpdata[0]) {                      //SNRM֡
        pstSdlc->type = SDLC_FRAME_TYPE_SNRM;
        mdmprint("RECEIVE SNRM\n\r");
    } else {
        free(tmpdata);
        return off + 1;
    }
    free(tmpdata);
    return off + 1;
}
