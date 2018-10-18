#ifndef _VERSION_H_
#define _VERSION_H_

const char g_VerInfo[] = "3.0.2";

#if defined E90||defined E90G
const char _g_sVer[] = "1999v4.1C";
const char _g_sAppName[] = "mainctrl";
const char _g_sExtName[] = "";
#endif

#if (MULTIAPP==2)
const int _g_nFramLen = 2048;
const char _g_sMachineType[2+1] = "\x85\x10";
const unsigned char _g_nSerio = 1;
const char _g_sVer[15+1] = "v1.000";
const char _g_sAppName[16+1] = "Ö÷¿Ø";
#endif
#endif
