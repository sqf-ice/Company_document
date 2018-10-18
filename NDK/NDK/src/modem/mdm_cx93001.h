/*
 * 新大陆公司 版权所有(c) 2011-2015
 *
 * modem模块-cx93001芯片
 * 作    者：    产品开发部
 * 日    期：    2013-12-10
 * 版    本：    V1.00
 * 最后修改人：
 * 最后修改日期：
 */

#ifndef MDM_CX93001_INCLUDE_H
#define MDM_CX93001_INCLUDE_H


#define VOL_OF_ABSENT_LINE_cx93001      1.8             //小于该值表示：电话线未插好
#define VOL_OF_LINE_IN_USE_Cx93001      12.0            //小于该值表示：检测到并机，使得线压过低


int mdm_sdlc_init_cx93001(void);
int mdm_sdlc_dial_cx93001(const char *dialno);
int mdm_sdlc_hangup_cx93001(void);
int mdm_asyn_init_cx93001(void);
int mdm_asyn_dial_cx93001(const char *dialno);
int mdm_asyn_hangup_cx93001(void);

#endif
