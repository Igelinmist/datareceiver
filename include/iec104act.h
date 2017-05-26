#ifndef IEC104ACT_H
#define IEC104ACT_H
#include "iec104types.h"

const int CF_I  =  0x0;
const int CF_S  =  0x1;
const int CF_U  =  0x3;
const int aSTARTDT = 0x04;
const int cSTARTDT = 0x08;
const int aSTOPDT = 0x10;
const int cSTOPDT = 0x20;
const int aTESTFR = 0x40;
const int cTESTFR = 0x80;

enum NLCtype {STARTDT_act,STARTDT_con,STOPDT_act,STOPDT_con,TESTFR_act,TESTFR_con};

/* Фукция формирования команды управления без нумерации */
TAPCI NumlessControl ( const NLCtype control_type );  
/* Фукция формирования команды управления с нумерацией */
TAPCI NumControl(const word ReadASDUcnt); 
TAPCI msgTESTFR_con();
TAPCI msgTESTFR_act();
int get_APDUtype(cardinal CtrlF);

#endif
