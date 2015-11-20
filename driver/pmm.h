//====================================================================
//    File: PMM.h
//
//    This file is used in conjunction with PMM.c to set the core
//    voltage level of a device. To set a core voltage level, call
//    SetVCore(level). See RF project(s) for example usage.
//
//    Version 1.0 first
//    07/14/07
//
//====================================================================

#ifndef __PMM
#define __PMM

void SetVCore(unsigned char level);
void SetVCoreUp(unsigned char level);
void SetVCoreDown(unsigned char level);

#endif                          /* __PMM */
