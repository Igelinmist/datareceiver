#ifndef TKPI_H
#define TKPI_H
#include "support.h"

enum time_outs {T0,T1,T2,T3,T4};

class cAssistent;//Forward declaration

class cKPI
{
private:
    int ivT0, ivT1, ivT2, ivT3, fK, fW;
    int TimerT0,TimerT1,TimerT2,TimerT3;
    word fFlag; /* Глобальная переменная флагов. Первые четыре - срабатывание таймаутов*/
    int Nr,Ns;
    cAssistent *pTCPassist;
    int preW;
    int Ack;

    void LeadUp ( time_outs pTO ); /* Ввзвести флаг контроля T.. */
    void Drop ( time_outs pTO ); /* Сбросить контроль интервала T..*/
    void S_confirm();
    void TESTFR_send();
    void TESTFR_confirm();

    cp56time2a get_cp56time_now();
public:
    cKPI(const int prmT0,const int prmT1,const int prmT2,const int prmT3,const int prmK,const int prmW,cAssistent *prmAssist);
    void AskPolling(int Couse = 6);
    int OnTimer();
    void ResetProcessVars();
    void StopDT();
    void OnOpenIECexchange();
    void ParseMessage(byte *bytes);
};

#endif // TKPI_H
