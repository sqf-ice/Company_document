/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-设备驱动
 * 作    者：    产品开发部
 * 日    期：    2013-12-10
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mount.h>

#include "NDK.h"
#include "modem.h"
#include "mdm_comm.h"
#include "mdm_cfg.h"
#include "mdm_drv.h"
#include "mdm_debug.h"
#include "mdm_v80.h"
#include "mdm_sdlc_sev.h"
#include "mdm_db.h"
#include "mdm_adapt.h"

extern int g_AuxPrintFlag;
extern ST_MDM_AT_RET_MAP g_stMdmAtRetMap[];
extern ST_MDM_CFG g_stMdmCfg;

static ST_MDM_STATUS s_stMdmSdlcs = {
    .rwLock     = PTHREAD_RWLOCK_INITIALIZER,
    .nDialType  = MODEM_DIAL_TYPE_SDLC,
    .nStatus    = MDM_SDLCS_CLOSED,
    .nCtrl      = MDM_SDLCC_INIT,
    .nErr       = MDM_OK,
};
static pthread_t s_nMdmSdlcPThreadID = 0;
static char s_stSdlcWriteBuff[MAXLEN_SDLCDATA];         //用户发送缓冲区
static int s_nSdlcWriteDataLen;
static char s_stSdlcReadBuff[MAXLEN_SDLCDATA];          //用户接收缓冲区
static int s_nSdlcReadDataLen;
static char s_stSdlcMdmPortBuff[MAXLEN_PORTBUF];        //串口收发缓冲区
static int s_nSdlcMdmPortBufLenf;
static long int s_nLastSendFrameTicks;                  //记录上一次发送帧时刻，用于超时发送握手帧
static long int s_nGetConnectTicks;                     //收到CONNECT的时刻，用于接收SNRM帧超时判断
static char s_nSdlcSendno;                              //发送帧序号
static char s_nSdlcRecno;                               //期望接收序号

static int mdm_send_UA(void)
{
    int ret;
    char tmpdata[MAXLEN_PORTBUF] = { 0 };
    ST_SDLC_V80 pstSdlc;

    pstSdlc.type = SDLC_FRAME_TYPE_UA;
    pstSdlc.outbuf = tmpdata;
    pstSdlc.outmaxlen = sizeof(tmpdata);
    if ((ret = mdm_sdlc_pack(&pstSdlc)) == MDM_OK) {
        mdmprint("SEND UA:\n\r");
        mdm_port_put_string(pstSdlc.outbuf, pstSdlc.outlen);
        s_nLastSendFrameTicks = mdm_get_time() + pstSdlc.outlen * 8 / 20;
        return MDM_OK;
    }
    return ret;
}
static int mdm_send_RR(void)
{
    int ret;
    char tmpdata[MAXLEN_PORTBUF] = { 0 };
    ST_SDLC_V80 pstSdlc;

    pstSdlc.type = SDLC_FRAME_TYPE_RR;
    pstSdlc.recno = s_nSdlcRecno;
    pstSdlc.outbuf = tmpdata;
    pstSdlc.outmaxlen = sizeof(tmpdata);
    if ((ret = mdm_sdlc_pack(&pstSdlc)) == MDM_OK) {
        mdmprint("SEND RR[%d]:\n\r", pstSdlc.recno);
        mdm_port_put_string(pstSdlc.outbuf, pstSdlc.outlen);
        s_nLastSendFrameTicks = mdm_get_time() + pstSdlc.outlen * 8 / 20;
        return MDM_OK;
    }
    return ret;
}
static int mdm_send_REJ(void)
{
	int ret;
	char tmpdata[MAXLEN_PORTBUF] = { 0 };
	ST_SDLC_V80 pstSdlc;

	pstSdlc.type = SDLC_FRAME_TYPE_REJ;
	pstSdlc.recno = s_nSdlcRecno;
	pstSdlc.outbuf = tmpdata;
	pstSdlc.outmaxlen = sizeof(tmpdata);
	if ((ret = mdm_sdlc_pack(&pstSdlc)) == MDM_OK) {
		mdmprint("SEND RR[%d]:\n\r", pstSdlc.recno);
		mdm_port_put_string(pstSdlc.outbuf, pstSdlc.outlen);
		s_nLastSendFrameTicks = mdm_get_time() + pstSdlc.outlen * 8 / 20;
		return MDM_OK;
	}
	return ret;
}
static int mdm_send_I(void)
{
    int ret;
    char tmpdata[MAXLEN_PORTBUF] = { 0 };
    ST_SDLC_V80 pstSdlc;

    pstSdlc.type = SDLC_FRAME_TYPE_I;
    pstSdlc.recno = s_nSdlcRecno;
    pstSdlc.sendno = s_nSdlcSendno;
    pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
    pstSdlc.inbuf = s_stSdlcWriteBuff;
    pstSdlc.inlen = s_nSdlcWriteDataLen;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    pstSdlc.outbuf = tmpdata;
    pstSdlc.outmaxlen = sizeof(tmpdata);
    if ((ret = mdm_sdlc_pack(&pstSdlc)) == MDM_OK) {
        mdmprint("SEND I[%d:%d]:[%d]\n\r", pstSdlc.recno, pstSdlc.sendno, pstSdlc.inlen);
        mdm_port_put_string(pstSdlc.outbuf, pstSdlc.outlen);
        s_nLastSendFrameTicks = mdm_get_time() + pstSdlc.outlen * 8 / 20;
        return MDM_OK;
    }
    return ret;
}

static int mdm_sdlc_hangup(void)
{
    int ret = -1;

    if ((ret = mdm_drv_dtrhungup()) != MDM_OK)
        return ret;
    switch (s_stMdmSdlcs.nStatus) {
        case MDMSTATUS_MS_NODIALTONE:
        case MDMSTATUS_MS_NOCARRIER:
        case MDMSTATUS_MS_BUSY:
        case MDMSTATUS_MS_NOANSWER:
            break;
        default:
            mdm_port_at_cmd_process(NULL, NULL, 0, 1000); //lsi:096305: 0.58s 6992:0.58s;conexant://096305: 0.7s
            break;
    }
    return MDM_OK;
}

static void mdm_get_sdlc_status(void)
{
    char *p, *q;
    int offset;
    ST_MDM_AT_RET_MAP *pstAtRet;

    pstAtRet = g_stMdmAtRetMap;
    if (s_nSdlcMdmPortBufLenf == 0)
        return;
    while (pstAtRet->respone != NULL) {
        if ((p = strstr(s_stSdlcMdmPortBuff, pstAtRet->respone)) != NULL) {
            if ((q = strstr(p, ATSTREND)) == NULL)
                return;
            pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
            switch (pstAtRet->ret) {
                case MDM_AT_RET_CONNECT:
                    s_stMdmSdlcs.nStatus = MDM_SDLCS_CONNECT;
                    g_AuxPrintFlag = 1;
                    s_nGetConnectTicks = mdm_get_time();
                    break;
                case MDM_AT_RET_NO_DIALTONE:
                    s_stMdmSdlcs.nStatus = MDMSTATUS_MS_NODIALTONE;
                    break;
                case MDM_AT_RET_NO_CARRIER:
                    s_stMdmSdlcs.nStatus = MDMSTATUS_MS_NOCARRIER;
                    break;
                case MDM_AT_RET_BUSY:
                    s_stMdmSdlcs.nStatus = MDMSTATUS_MS_BUSY;
                    break;
                case MDM_AT_RET_NO_ANSWER:
                    s_stMdmSdlcs.nStatus = MDMSTATUS_MS_NOANSWER;
                    break;
                case MDM_AT_RET_RING:
                    s_stMdmSdlcs.nStatus = MDMSTATUS_MS_RING;
                    break;
                case MDM_AT_RET_ERROR:
                    s_stMdmSdlcs.nStatus = MDMSTATUS_MS_ERROR;
                    break;
                default:
                    break;
            }
            pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
            /**需要对读取到的数据删除**/
            offset = q + strlen(ATSTREND) - s_stSdlcMdmPortBuff;
            memmove(s_stSdlcMdmPortBuff, s_stSdlcMdmPortBuff + offset, s_nSdlcMdmPortBufLenf - offset);
            memset(s_stSdlcMdmPortBuff + s_nSdlcMdmPortBufLenf - offset, 0, offset);
            s_nSdlcMdmPortBufLenf -= offset;
            break;
        }
        pstAtRet++;
    }
}
static int mdm_sdlc_serv(void)
{
    static int s_nSendUAs = 0;      //接收到SNRM帧的个数，用于自适应判断
    static int s_nSendRR0 = 0;      //收发RR0帧的个数，用于接收载波干扰情况的处理
    static int s_nSendI0s = 0;      //发送到I0帧的个数，用于自适应判断
    static int s_nRecInvalids = 0;  //接收到无效帧的个数，用于自适应判断
    int ret, wrdatalen;
    EM_MDM_SDLC_SENDTYPE sendframetype = MDM_SDLC_SEND_NONE;
    char tmpdata[MAXLEN_PORTBUF] = { 0 };
    ST_SDLC_V80 pstSdlc = {
        .type       = SDLC_FRAME_TYPE_ERR,
        .recno      = 0,
        .sendno     = 0,
        .inbuf      = NULL,
        .inlen      = 0,
        .outbuf     = NULL,
        .outmaxlen  = 0,
        .outlen     = 0,
    };

    /************读取串口数据****************/
    if ((ret = mdm_port_get_line(s_stSdlcMdmPortBuff + s_nSdlcMdmPortBufLenf, MAXLEN_PORTBUF - s_nSdlcMdmPortBufLenf, 0)) > 0)
        s_nSdlcMdmPortBufLenf += ret;

    /************分析串口数据****************/
    switch (s_stMdmSdlcs.nStatus) {
        case MDM_SDLCS_DIALED:
            s_nSendUAs = 0;
            s_nSendRR0 = 0;
            s_nSendI0s = 0;
            s_nRecInvalids = 0;
            mdm_get_sdlc_status();
            if (s_stMdmSdlcs.nStatus == MDMSTATUS_MS_NOCARRIER) {
                pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
                s_stMdmSdlcs.nErr = MDM_ERR_ADAPT_HIGHVOL;
                mdmprint("HIGHVOL ERR\n\r");
                pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
            }
            break;
        case MDM_SDLCS_CONNECT:
            mdm_get_sdlc_status();
            if (s_stMdmSdlcs.nStatus != MDM_SDLCS_CONNECT) {
                if ((s_stMdmSdlcs.nStatus == MDMSTATUS_MS_NOCARRIER) && (s_nSendUAs == 0)) {
                    pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
                    s_stMdmSdlcs.nErr = MDM_ERR_ADAPT_HIGHVOL;
                    mdmprint("HIGHVOL ERR AFTER CONNECT\n\r");
                    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                }
                mdmprint("CONNECT BREAK OFF [status %d]\n\r", s_stMdmSdlcs.nStatus);
                return MDM_ERR_SDLC_STATUS;
            }
            if (s_nSdlcMdmPortBufLenf != 0) {
                pstSdlc.inbuf = s_stSdlcMdmPortBuff;
                pstSdlc.inlen = s_nSdlcMdmPortBufLenf;
                pstSdlc.outbuf = tmpdata;
                pstSdlc.outmaxlen = sizeof(tmpdata);
                if ((ret = mdm_sdlc_unpack(&pstSdlc)) < 0) {
                    mdmprint("UNPACK DATA ERR [%d]\n\r", ret);
                    return ret;
                }
                memmove(s_stSdlcMdmPortBuff, s_stSdlcMdmPortBuff + ret, s_nSdlcMdmPortBufLenf - ret);
                memset(s_stSdlcMdmPortBuff + s_nSdlcMdmPortBufLenf - ret, 0, ret);
                s_nSdlcMdmPortBufLenf -= ret;
            }
            switch (pstSdlc.type) {
                case SDLC_FRAME_TYPE_SNRM:
                    /*收发帧号清零*/
                    s_nSdlcSendno = 0;
                    s_nSdlcRecno = 0;
                    if ((ret = mdm_send_UA()) != MDM_OK) {
                        mdmprint("SEND UA ERR [%d]\n\r", ret);
                        return ret;
                    }
                    s_nSendUAs++;
                    break;
                case SDLC_FRAME_TYPE_RR:
                    if ((ret = mdm_send_RR()) != MDM_OK) {
                        mdmprint("SEND RR ERR [%d]\n\r", ret);
                        return ret;
                    }
                    s_nSendRR0++;
                    break;
                case SDLC_FRAME_TYPE_INVALILD:
                    s_nRecInvalids++;
                    mdmprint("RECEIVE ERR FRAME[%d]\n\r", s_nRecInvalids);
                    break;
                default:
                    if (g_stMdmCfg.nRecSignaldetFlag) {
                        if ((mdm_get_time() - s_nGetConnectTicks) > (5 + g_stMdmCfg.nFrameRst) * TICKS_PERSECOND) {
                            pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
                            s_stMdmSdlcs.nStatus = MDMSTATUS_SDLCS_ERR_GETRRTIMEOUT;
                            if (s_nSendUAs == 0)
                                s_stMdmSdlcs.nErr = MDM_ERR_ADAPT_ANSWERTONE;
                            pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                            memset(s_stSdlcMdmPortBuff, 0, s_nSdlcMdmPortBufLenf);
                            s_nSdlcMdmPortBufLenf = 0;
                            mdmprint("WAIT RR TIMEOUT [%d]\n\r", g_stMdmCfg.nFrameRst + 5);
                            return MDM_ERR_SDLC_WAITRR_TIMEOUT;
                        }
                    } else {
                        if ((mdm_get_time() - s_nGetConnectTicks) > g_stMdmCfg.nFrameRst * TICKS_PERSECOND) {
                            pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
                            s_stMdmSdlcs.nStatus = MDMSTATUS_SDLCS_ERR_GETSNRMTIMEOUT;
                            s_stMdmSdlcs.nErr = MDM_ERR_ADAPT_ANSWERTONE;
                            mdmprint("ANSWERTONE ERR\n\r");
                            pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                            memset(s_stSdlcMdmPortBuff, 0, s_nSdlcMdmPortBufLenf);
                            s_nSdlcMdmPortBufLenf = 0;
                            mdmprint("WAIT SNRM TIMEOUT [%d]\n\r", g_stMdmCfg.nFrameRst);
                            return MDM_ERR_SDLC_WAITSNRM_TIMEOUT;
                        }
                    }
                    break;
            }
		if (((!g_stMdmCfg.nRecSignaldetFlag) && (!g_stMdmCfg.nCetRRdetFlag) && (s_nSendUAs >= 1)) ||    //无效帧不检测并且不检测RR帧时，只要发送UA帧后就认为连接成功
		    ((!g_stMdmCfg.nRecSignaldetFlag) && (g_stMdmCfg.nCetRRdetFlag) && (s_nSendRR0 >= 1)) ||   //无效帧不检测并且检测RR帧时，收发1次RR帧后才认为连接成功
                ((g_stMdmCfg.nRecSignaldetFlag) && (s_nSendRR0 >= 3))) {
                pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
                s_stMdmSdlcs.nStatus = MDM_SDLCS_RR;
                pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                mdmprint("CONNECTTED\n\r");
            } else if ((g_stMdmCfg.nRecSignaldetFlag) && (s_nRecInvalids >= 15)) {
                pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
                s_stMdmSdlcs.nErr = MDM_ERR_ADAPT_RECSIGNAL;
                s_stMdmSdlcs.nStatus = MDMSTATUS_SDLCS_ERR_RECSIGNAL;
                pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                mdmprint("SDLC_RECSIGNAL ERR\n\r");
                return MDM_ERR_SDLC_RECSIGNAL;
		} else if (s_nSendUAs >= 5) {
			pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
			s_stMdmSdlcs.nErr = MDM_ERR_DIAL_RECSNRMMORE;
			s_stMdmSdlcs.nStatus = MDMSTATUS_SDLCS_ERR_RECSNRMMORE;
			pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
			mdmprint("SDLC_RECEIVE SNRM ERR\n\r");
			return MDM_ERR_DIAL_RECSNRMMORE;
            }
            break;
        case MDM_SDLCS_RR:
            if ((s_nSdlcMdmPortBufLenf == 0)) {
                if ((mdm_get_time() - s_nLastSendFrameTicks > 2 * TICKS_PERSECOND)) {
                    mdmprint("RESEND RR\n\r");
                    sendframetype = MDM_SDLC_SEND_RR;
                }
            }
            while (s_nSdlcMdmPortBufLenf) {
                pstSdlc.inbuf = s_stSdlcMdmPortBuff;
                pstSdlc.inlen = s_nSdlcMdmPortBufLenf;
                pstSdlc.outbuf = tmpdata;
                pstSdlc.outmaxlen = sizeof(tmpdata);
                ret = mdm_sdlc_unpack(&pstSdlc);
                if (ret < 0) {
                    mdmprint("UNPACK DATA ERR [%d]\n\r", ret);
                    return ret;
                } else if (ret == 0) {
                    break;
                }
                /*清除已被处理的数据*/
                memmove(s_stSdlcMdmPortBuff, s_stSdlcMdmPortBuff + ret, s_nSdlcMdmPortBufLenf - ret);
                memset(s_stSdlcMdmPortBuff + s_nSdlcMdmPortBufLenf - ret, 0, ret);
                s_nSdlcMdmPortBufLenf -= ret;
                switch (pstSdlc.type) {
                    case SDLC_FRAME_TYPE_SNRM:
                        /*收发帧号清零*/
                        s_nSdlcSendno = 0;
                        s_nSdlcRecno = 0;
                        /*发送UA帧*/
                        if ((ret = mdm_send_UA()) != MDM_OK) {
                            mdmprint("SEND UA ERR [%d]\n\r", ret);
                            return ret;
                        }
                        s_nSendUAs++;
                        break;
                    case SDLC_FRAME_TYPE_RR:
                        pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
                        wrdatalen = s_nSdlcWriteDataLen;
                        pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                        if (wrdatalen) {
                            if (pstSdlc.recno == NextNo(s_nSdlcSendno)) {   //表示服务器希望接收下一帧数据，说明之前发送的数据成功
                                AddNo(s_nSdlcSendno);                   //发送帧号+1
                                pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
                                s_nSdlcWriteDataLen = 0;                //没有数据等待发送
                                pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                            } else {                                        //表示服务器希望接收该帧数据，则发送该帧数据或重新发送该帧数据
                                sendframetype = MDM_SDLC_SEND_I;
                                break;
                            }
                        }
                        sendframetype = MDM_SDLC_SEND_RR;
                        break;
                    case SDLC_FRAME_TYPE_I:
                        pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
                        wrdatalen = s_nSdlcWriteDataLen;
                        pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                        if (pstSdlc.sendno == s_nSdlcRecno) { //表示服务器的发送帧号就是pos希望接收的帧号，说明需要保存该帧数据
                            pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
                            if (s_nSdlcReadDataLen + pstSdlc.outlen > MAXLEN_SDLCDATA) {
                                pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                                mdmprint("SDLC READ BUFFER_LEN ERR\n\r");
                                return MDM_ERR_SDLC_READ_BUFERR;
                            }
                            memcpy(s_stSdlcReadBuff + s_nSdlcReadDataLen, pstSdlc.outbuf, pstSdlc.outlen);
                            s_nSdlcReadDataLen += pstSdlc.outlen;
                            pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                            AddNo(s_nSdlcRecno); //希望接收下一帧数据
						} else if(g_stMdmCfg.nCetRejSendFlag && (pstSdlc.sendno == UpNo(s_nSdlcRecno))) {
							sendframetype = MDM_SDLC_SEND_REJ;
							break;
                        }
                        if (wrdatalen) {
                            if (pstSdlc.recno == NextNo(s_nSdlcSendno)) {   //表示服务器希望接收下一帧数据，说明之前发送的数据成功
                                AddNo(s_nSdlcSendno);                   //发送帧号+1
                                pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
                                s_nSdlcWriteDataLen = 0;                //没有数据等待发送
                                pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                            } else {
                                sendframetype = MDM_SDLC_SEND_I;
                                break;
                            }
                        }
                        sendframetype = MDM_SDLC_SEND_RR;
                        break;
                    case SDLC_FRAME_TYPE_INVALILD:
                        s_nRecInvalids++;
                        mdmprint("RECEIVE ERR FRAME[%d]\n\r", s_nRecInvalids);
                        break;
                    default:
                        break;
                }
            }
            mdm_get_sdlc_status();
            if (s_stMdmSdlcs.nStatus == MDM_SDLCS_RR) {
                switch (sendframetype) {
                    case MDM_SDLC_SEND_I:
                        if ((ret = mdm_send_I()) != MDM_OK)
                            return ret;
                        if (s_nSdlcSendno == 0)
                            s_nSendI0s++;
                        else
                            s_nSendI0s = 0;
                        break;
                    case MDM_SDLC_SEND_RR:
                        if ((ret = mdm_send_RR()) != MDM_OK)
                            return ret;
                        break;
					case MDM_SDLC_SEND_REJ:
						if ((ret = mdm_send_REJ()) != MDM_OK)
							return ret;
                        break;
                    default: //如果是UA帧则在分析时已经发送
                        break;
                }
            } else {
                mdmprint("CONNECT BREAK OFF [status %d]\n\r", s_stMdmSdlcs.nStatus);
                return MDM_ERR_SDLC_STATUS;
            }
            break;
        default:
            return MDM_ERR_SDLC_STATUS;
    }
    pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
    if ((s_nSendUAs >= 3) || (s_nSendI0s >= 3))
        s_stMdmSdlcs.nErr = MDM_ERR_ADAPT_LOWVOL;
    if (s_nRecInvalids >= 15)
        s_stMdmSdlcs.nErr = MDM_ERR_ADAPT_RECSIGNAL;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    return MDM_OK;
}

static void *sdlc_pthread(void *args)
{
    static int s_nNewDialFlag = 0;
    int cmd, ret;
    char ATCMD[MAXLEN_DIALNUM + 5] = { 0 };

    mdmprint("SDLC PTHREAD START\n\r");
    while (1) {
        pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
        cmd = s_stMdmSdlcs.nCtrl;
        pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);

        switch (cmd) {
            case MDM_SDLCC_INIT:
                //请收、发缓冲区，收、发帧号清零
                mdmprint("SDLC PTHREAD INITING\n\r");
                pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
                memset(s_stSdlcWriteBuff, 0, sizeof(s_stSdlcWriteBuff));        //用户发送缓冲区
                s_nSdlcWriteDataLen = 0;
                memset(s_stSdlcReadBuff, 0, sizeof(s_stSdlcReadBuff));          //用户接收缓冲区
                s_nSdlcReadDataLen = 0;
                memset(s_stSdlcMdmPortBuff, 0, sizeof(s_stSdlcMdmPortBuff));    //串口收发缓冲区
                s_nSdlcMdmPortBufLenf = 0;
                s_stMdmSdlcs.nStatus = MDM_SDLCS_INITED;
                s_stMdmSdlcs.nCtrl = MDM_SDLCC_DONE;
                s_stMdmSdlcs.nErr = MDM_OK;
                pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                mdmprint("SDLC PTHREAD INITED\n\r");
                break;
            case MDM_SDLCC_DIAL:
                mdmprint("SDLC PTHREAD DIALING\n\r");
                pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
                sprintf(ATCMD, "ATDT%s\r", s_stMdmSdlcs.szDialNum);
                mdm_port_at_cmd_process(ATCMD, NULL, 0, 0);
                pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
                s_stMdmSdlcs.nStatus = MDM_SDLCS_DIALED;
                s_stMdmSdlcs.nCtrl = MDM_SDLCC_DONE;
                s_nNewDialFlag = 1;
                mdm_get_datetime(s_stMdmSdlcs.szDialTime);
                pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                mdmprint("SDLC PTHREAD DIALED\n\r");
                break;
            case MDM_SDLCC_HUANGUP:
                mdmprint("SDLC PTHREAD HUNGUPING\n\r");
                mdm_sdlc_hangup();
                pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
                s_stMdmSdlcs.nStatus = MDM_SDLCS_CLOSED;
                s_stMdmSdlcs.nCtrl = MDM_SDLCC_DONE;
                pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
                mdmprint("SDLC PTHREAD HUNGUPED\n\r");
                break;
            default:
                break;
        }

        if (s_stMdmSdlcs.nStatus >= MDM_SDLCS_DIALED) {
            ret = mdm_sdlc_serv();
            pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
            if (s_stMdmSdlcs.nErr == MDM_OK)
                s_stMdmSdlcs.nErr = ret;
            pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
            if ((s_stMdmSdlcs.nErr != MDM_OK) || (s_stMdmSdlcs.nStatus == MDM_SDLCS_RR)
                || (s_stMdmSdlcs.nStatus < MDMSTATUS_NORETURN_AFTERPREDIAL)) {
                if (mdm_db_statistics(s_stMdmSdlcs, s_nNewDialFlag) == MDM_OK)
                    if (g_stMdmCfg.nParamAdaptFlag)
                        mdm_adapt_auto(s_stMdmSdlcs, s_nNewDialFlag);
                s_nNewDialFlag = 0;
            }
        }
        usleep(10 * 1000);
    }
}

//初始化同步服务线程
static int mdm_sdlc_pthread_init(void)
{
    if (!s_nMdmSdlcPThreadID) {
        s_stMdmSdlcs.nStatus = MDM_SDLCS_CLOSED;
        s_stMdmSdlcs.nCtrl = MDM_SDLCC_INIT;
        s_stMdmSdlcs.nErr = MDM_OK;
        pthread_rwlock_init(&s_stMdmSdlcs.rwLock, NULL);
        if (pthread_create(&s_nMdmSdlcPThreadID, NULL, sdlc_pthread, NULL) != 0) {
            s_nMdmSdlcPThreadID = 0;
            return MDM_ERR_SDLC_PTHREAD_CREAT;
        }
    }
    return MDM_OK;
}

int mdm_sdlc_serv_init(void)
{
    int oldstatus, nowstatus;
    int timeout = 10; //初始化超时定位10S
    long int timeticks;

    pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
    oldstatus = s_stMdmSdlcs.nStatus;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    if (oldstatus == MDM_SDLCS_INITED)
        return MDM_OK;
    mdm_sdlc_pthread_init();
    pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
    s_stMdmSdlcs.nCtrl = MDM_SDLCC_INIT;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    timeticks = mdm_get_time();

    /******************判断是否初始化完成**********************/
    while (1) {
        pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
        nowstatus = s_stMdmSdlcs.nStatus;
        pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
        if (nowstatus != oldstatus) {
            if (nowstatus == MDM_SDLCS_INITED)
                break;
            else
                return MDM_ERR_SDLCSERV_INIT_FAIL;
        }
        if (mdm_get_time() - timeticks > timeout * TICKS_PERSECOND)
            return MDM_ERR_SDLCSERV_INIT_TIMEOUT;
        usleep(10 * 1000);
    }
    return MDM_OK;
}
int mdm_sdlc_serv_dial(const char *dialno)
{
    int oldstatus, nowstatus;
    int timeout = 10; //拨号超时定位10S
    long int timeticks;

    pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
    oldstatus = s_stMdmSdlcs.nStatus;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    if (oldstatus == MDM_SDLCS_DIALED)
        return MDM_OK;
    pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
    memset(s_stMdmSdlcs.szDialNum, 0, sizeof(s_stMdmSdlcs.szDialNum));
    memcpy(s_stMdmSdlcs.szDialNum, dialno, strlen(dialno));
    s_stMdmSdlcs.nCtrl = MDM_SDLCC_DIAL;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    timeticks = mdm_get_time();
    /******************判断是否拨号完成**********************/
    while (1) {
        pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
        nowstatus = s_stMdmSdlcs.nStatus;
        pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
        if (nowstatus != oldstatus) {
            if (nowstatus == MDM_SDLCS_DIALED)
                break;
            else
                return MDM_ERR_SDLC_DAIL_FAIL;
        }
        if (mdm_get_time() - timeticks > timeout * TICKS_PERSECOND)
            return MDM_ERR_SDLC_DAIL_TIMEOUT;
        usleep(10 * 1000);
    }
    return MDM_OK;
}

int mdm_sdlc_serv_write(const char *pszdata, int ndatalen)
{
    mdmprint("call %s %d \n\r", __func__, __LINE__);
    int timeout = 0, status = 0, writedatalen = -1;
    long int timeticks;

    pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
    status = s_stMdmSdlcs.nStatus;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    if (status != MDM_SDLCS_RR)
        return MDM_ERR_SDLC_NODIAL;
    timeout = g_stMdmCfg.nDataTimeOut;
    if (timeout < (ndatalen / 100 + 10))
        timeout = ndatalen / 100 + 10;
    timeticks = mdm_get_time();
    mdmprint("call %s %d timeout %d\n\r", __func__, __LINE__, timeout);
    if (s_nSdlcWriteDataLen + ndatalen > MAXLEN_SDLCDATA)
        return MDM_ERR_SDLC_WRITE_BUFERR;
    pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
    memcpy(s_stSdlcWriteBuff + s_nSdlcWriteDataLen, pszdata, ndatalen);
    s_nSdlcWriteDataLen += ndatalen;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);

    while (1) {
        pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
        writedatalen = s_nSdlcWriteDataLen;
        status = s_stMdmSdlcs.nStatus;
        pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
        if (!writedatalen)
            break;
        if (status != MDM_SDLCS_RR)
            return MDM_ERR_SDLC_DISCONNECT;
        if (mdm_get_time() - timeticks > timeout * TICKS_PERSECOND)
            return MDM_ERR_SDLC_WRITE_TIMEOUT;
        usleep(10 * 1000);
    }
    return MDM_OK;
}

int mdm_sdlc_serv_read(char *pszdata, int *ndatalen, int ntimeout)
{
    int status = 0, datalen = -1;
    long int timeticks;

    timeticks = mdm_get_time();
    pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
    status = s_stMdmSdlcs.nStatus;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    mdmprint("call %s %d SdlnStatus %d ntimeout %d\n\r", __func__, __LINE__, status, ntimeout);
    if ((status == MDM_SDLCS_CLOSED) || (status == MDM_SDLCS_INITED))
        return MDM_ERR_SDLC_NODIAL;

    while (1) {
        pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
        datalen = s_nSdlcReadDataLen;
        status = s_stMdmSdlcs.nStatus;
        pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);

        if (datalen) {
            pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
            memcpy(pszdata, s_stSdlcReadBuff, datalen);
            *ndatalen = datalen;
            s_nSdlcReadDataLen -= datalen;
            pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
            break;
        }
        if (status != MDM_SDLCS_RR)
            return MDM_ERR_SDLC_DISCONNECT;
        if (mdm_get_time() - timeticks > ntimeout * TICKS_PERSECOND)
            return MDM_ERR_SDLC_READ_TIMEOUT;
        usleep(10 * 1000);
    }
    return MDM_OK;
}

int mdm_sdlc_serv_hangup(void)
{
    int oldstatus, nowstatus;
    int timeout = 10; //挂断超时定位10S
    long int timeticks;

    pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
    oldstatus = s_stMdmSdlcs.nStatus;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    mdmprint("call %s %d nStatus %d\n\r", __func__, __LINE__, oldstatus);
    if (oldstatus == MDM_SDLCS_CLOSED)
        return MDM_OK;
    pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
    s_stMdmSdlcs.nCtrl = MDM_SDLCC_HUANGUP;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    timeticks = mdm_get_time();

    /******************判断是否挂断完成**********************/
    while (1) {
        pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
        nowstatus = s_stMdmSdlcs.nStatus;
        pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
        if (nowstatus != oldstatus) {
            if (nowstatus == MDM_SDLCS_CLOSED)
                break;
            else
                return MDM_ERR_SDLC_HUNGUP_FAIL;
        }
        if (mdm_get_time() - timeticks > timeout * TICKS_PERSECOND)
            return MDM_ERR_SDLC_HUNGUP_TIMEOUT;
        usleep(10 * 1000);
    }
    return MDM_OK;
}

int mdm_sdlc_serv_getstatus(void)
{
    int status;

    pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
    status = s_stMdmSdlcs.nStatus;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    /* 与旧的接口返回值兼容*/
    if ((status == MDM_SDLCS_CLOSED) || (status == MDM_SDLCS_INITED))
        status = MDMSTATUS_NOPREDIAL;
    else if (status == MDM_SDLCS_RR)
        status = MDMSTATUS_CONNECT_AFTERPREDIAL;
    else if (status > MDM_SDLCS_INITED)
        status = MDMSTATUS_NORETURN_AFTERPREDIAL;
    return status;
}

int mdm_sdlc_serv_readlen(void)
{
    int datalen = 0;

    pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
    datalen = s_nSdlcReadDataLen;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    return datalen;
}

int mdm_sdlc_serv_clrbuff(void)
{
    int status = MDM_SDLCS_RR + 1;

    pthread_rwlock_rdlock(&s_stMdmSdlcs.rwLock);
    status = s_stMdmSdlcs.nStatus;
    pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    if (status == MDM_SDLCS_RR) {
        pthread_rwlock_wrlock(&s_stMdmSdlcs.rwLock);
        memset(s_stSdlcReadBuff, 0, sizeof(s_stSdlcReadBuff));
        s_nSdlcReadDataLen = 0;
        pthread_rwlock_unlock(&s_stMdmSdlcs.rwLock);
    }
    return MDM_OK;
}
