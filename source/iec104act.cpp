#include "../include/iec104act.h"

void  fCopyMsv ( byte *msvS, byte *msvD, int col ) {
    for ( int i = 1; i<=col; i++ )
        *msvD++ = *msvS++;
}

TAPCI NumlessControl ( const NLCtype control_type ) {
    TAPCI res;
    res.sb = 0x68;
    res.len = 4;
    switch ( control_type ) {
    case STARTDT_act:
        res.CF.hole = 0 | CF_U | aSTARTDT;
        return res;
        break;
    case STARTDT_con:
        res.CF.hole = 0 | CF_U | cSTARTDT;
        return res;
        break;
    case STOPDT_act:
        res.CF.hole = 0 | CF_U | aSTOPDT;
        return res;
        break;
    case STOPDT_con:
        res.CF.hole = 0 | CF_U | cSTOPDT;
        return res;
        break;
    default : return res;
    }
}

TAPCI NumControl ( const word ReadASDUcnt ) {
    TAPCI res;
    res.sb = 0x68;
    res.len = 4;
    res.CF.hole = 0 | CF_S;
    res.CF.S.nrS = ReadASDUcnt;
    return res;
}

TAPCI msgTESTFR_con() {
    TAPCI res;
    res.sb = 0x68;
    res.len = 4;
    res.CF.hole = 0 | CF_U|cTESTFR;
    return res;
}

TAPCI msgTESTFR_act() {
    TAPCI res;
    res.sb = 0x68;
    res.len = 4;
    res.CF.hole = 0 | CF_U|aTESTFR;
    return res;
}

int get_APDUtype ( cardinal CtrlF ) {
    if ( ( CtrlF << 31 ) == 0 ) return CF_I;
    switch ( ( CtrlF << 30 ) >> 30 ) {
    case 1 : return CF_S; break;
    case 3 : return CF_U; break;
    default : return -1;
    }
}

