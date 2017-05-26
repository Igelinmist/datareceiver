#include "../include/kpi.h"
#include "../include/assistent.h"
#include "../include/iec104act.h"

const int cT0 = 0x0001;// Константы расположения битов в глобальном флаге (флаге контроля временных интервалов)
const int cT1 = 0x0002;
const int cT2 = 0x0004;
const int cT3 = 0x0008;

cKPI::cKPI(const int prmT0, const int prmT1, const int prmT2, const int prmT3, const int prmK, const int prmW, cAssistent* prmAssist)
{
    ivT0 = prmT0;
    ivT1 = prmT1;
    ivT2 = prmT2;
    ivT3 = prmT3;
    fK = prmK;
    fW = prmW;
    pTCPassist = prmAssist;
}

void cKPI::ResetProcessVars()
{
    TimerT0 = ivT0;
    TimerT1 = ivT1;
    TimerT2 = ivT2;
    TimerT3 = ivT3;
    fFlag &= ~0x000F;
    Ack = 0;
    preW = 0;
    Nr = 0;
    Ns = 0;
}

cp56time2a cKPI::get_cp56time_now()
{
    time_t rawtime;
    struct tm * timeinfo;
    cp56time2a iec_tm;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    iec_tm.mday = timeinfo->tm_mday;
    iec_tm.month = timeinfo->tm_mon+1;
    iec_tm.year = timeinfo->tm_year+1900-2000;//2000 отнимается, поскольку для передачи года 7 бит
    iec_tm.hour = timeinfo->tm_hour;
    iec_tm.min = timeinfo->tm_min;
    iec_tm.msec = timeinfo->tm_sec * 1000;
    iec_tm.wday = timeinfo->tm_wday;
    iec_tm.su = timeinfo->tm_isdst;

    return iec_tm;
}


void cKPI::LeadUp(time_outs pTO)
{
    switch ( pTO ) {
    case T0:
        fFlag |=cT0; //взводим бит контроля временного интервала
        TimerT0 = ivT0; // счетчик секунд на максимум
        break;
    case T1:
        fFlag |=cT1;
        TimerT1 = ivT1;
        break;
    case T2:
        fFlag |=cT2;
        TimerT2 = ivT2;
        break;
    case T3:
        fFlag |=cT3;
        TimerT3 = ivT3;
        break;
    default: ;
    }
}

void cKPI::Drop(time_outs pTO)
{
    switch ( pTO ) {
    case T0:
        // Сбрасываем флаг
        fFlag &=~cT0;
        // Тайм-аут на максимум
        TimerT0 = ivT0;
        break;
    case T1:
        fFlag &=~cT1;
        TimerT1 = ivT1;
        break;
    case T2:
        fFlag &=~cT2;
        TimerT2 = ivT2;
        break;
    case T3:
        fFlag &=~cT3;
        TimerT3 = ivT3;
        break;
    default: ;
    }
}

int cKPI::OnTimer()
{
    // Уменьшаем запас времени таймаутов, если они на контроле
    if ( fFlag & cT0 )
        TimerT0--;
    if ( fFlag & cT1 )
        TimerT1--;
    if ( fFlag & cT2 )
        TimerT2--;
    if ( fFlag & cT3 )
        TimerT3--;
    if ( ( fFlag & cT1 ) && ( TimerT1<=0 ) ) {
        log(InfIEC, "Timer T1 was out");
        syslog(LOG_NOTICE,"<%d> Timer T1 was out. Init reconnection after 30s.",cfg_num);
        return -1;
    }
    if ( ( fFlag & cT2 ) && ( TimerT2<=0 ) )  {
        S_confirm();
    }
    if ( ( fFlag & cT3 ) && ( TimerT3<=0 ) )
        TESTFR_send();
    return 0;
}

void cKPI::StopDT()
{
    TAPCI nlc;
    nlc = NumlessControl ( STOPDT_act );
    pTCPassist->PushWriteBuf(&nlc,6);
}

void cKPI::S_confirm()
{
    TAPCI nc;
    nc = NumControl ( Nr );
    pTCPassist->PushWriteBuf(&nc,6);
    log(InfIEC,"<<< S(%d)",Nr);
    Drop(T2);
    preW = 0; //Сбрасываем число неподтвержденных APDU
}

void cKPI::TESTFR_send()
{
    TAPCI nlc = msgTESTFR_act();
    pTCPassist->PushWriteBuf(&nlc,6);
    log(InfIEC,"<<< TESTFR_act");
    Drop ( T3 );
    LeadUp ( T1 );
}

void cKPI::TESTFR_confirm()
{
    TAPCI nlc = msgTESTFR_con();
    pTCPassist->PushWriteBuf(&nlc,6);
}

void cKPI::AskPolling(int Couse)
{
    struct {
        TAPCI APCI;
        ASDU_C_IC_NA_1 cmd;
    }__attribute__ ( ( packed ) ) IECmes;
    memset ( &IECmes,0,sizeof ( IECmes ) );
    IECmes.APCI.sb = 104;
    IECmes.APCI.len = 14;
    IECmes.APCI.CF.I.idI = 0;
    IECmes.APCI.CF.I.nrI = Nr;
    IECmes.APCI.CF.I.nsI = Ns;
    // Заголовок  type == 0x64  num == 1  sq == 0
    IECmes.cmd.ASDUhead.type = C_IC_NA_1;
    IECmes.cmd.ASDUhead.num = 1;
    IECmes.cmd.ASDUhead.sq = 0; //классификатор переменной структуры
    IECmes.cmd.ASDUhead.cause = Couse; //причина передачи 20 - общий опрос 6 - активация
    IECmes.cmd.IOA.ioa = 0; //адрес объекта информации при данном опросе = 0
    IECmes.cmd.IOA.ioa_h = 0;
    IECmes.cmd.QOI = 20; //описатель запрос 20-опрос станции общий
    pTCPassist->PushWriteBuf(&IECmes, sizeof ( IECmes ));
    Ns++;
}

void cKPI::OnOpenIECexchange()
{
    ResetProcessVars();
    AskPolling();
}

void cKPI::ParseMessage(byte* bytes)
{
    int PtB=0;
    int i;
    TAPCI *pAPCI;
    TASDU_HEAD *pASDU_HEAD;
    TIEC_M_ME_TF_1 *pIEC_M_ME_TF_1;
    TIEC_M_ME_NC_1 *pIEC_M_ME_NC_1;
    TIEC_M_SP_NA_1 *pIEC_M_SP_NA_1;
    TIEC_M_SP_TB_1 *pIEC_M_SP_TB_1;
    TIEC_IOA *pIEC_IOA;
    fdata tmpData;

    pAPCI = ( TAPCI * ) &bytes[PtB];
    PtB+=sizeof ( TAPCI );

    LeadUp(T3); //любой входящий ASDU (пере)взводит T3

    switch ( get_APDUtype ( pAPCI->CF.hole ) ) {
    case CF_U:
        switch ( pAPCI->CF.hole & 0xFC ) {
        case cSTARTDT:
            log(InfIEC,"\t>>> %s","STARTDT_con" );
            syslog(LOG_NOTICE,"<%d> IEC104 exchange UP",cfg_num);
            OnOpenIECexchange();
            break;
        case aSTARTDT:
            log(InfIEC,"\t>>> %s","STARTDT_act" );
            break;
        case cSTOPDT:
            log(InfIEC,"\t>>> %s","STOPDT_con" );
            break;
        case aSTOPDT:
            log(InfIEC,"\t>>> %s","STOPTDT_act" );
            break;
        case cTESTFR:
            Drop(T1); //Получение ответа на тестирование сбрасывает Т1
            log(InfIEC,"\t>>> %s","TESTFR_con" );
            break;
        case aTESTFR:
            log(InfIEC,"\t>>> %s","TESTFR_act" );
            TESTFR_confirm();
            log(InfIEC,"<<< %s","TESTFR_con" );
            break;
        };
        break;

    case CF_I:

        if (pAPCI->CF.I.nsI < Nr) { /* Означает потерю сообщений. Отключаться не будем, скомпенсируем. */
            log(InfDebug, "Compensated losses for %d APDU",Nr-pAPCI->CF.I.nsI);
            Nr = pAPCI->CF.I.nsI;
        }
        else {
            if (pAPCI->CF.I.nsI > Nr) {
                log(InfDebug, "Compensated bad enumeration for APDU (%d)",Nr-pAPCI->CF.I.nsI);
                Nr = pAPCI->CF.I.nsI;
            }
            else log(InfIEC,"\t>>> I(%d,%d)",pAPCI->CF.I.nrI,pAPCI->CF.I.nsI );
        }

        if (pAPCI->CF.I.nrI == Ns) {
            Ack = Ns;
            Drop(T1); /* в I-APDU мы получаем подтверждение кол-ва переданных нами APDU, поэтому сбрасываем T1 */
        }

        pASDU_HEAD= ( TASDU_HEAD * ) &bytes[PtB];
        PtB += sizeof ( TASDU_HEAD );
        log(InfIEC," APDU type = %d ___ cause(%d)",pASDU_HEAD->type,pASDU_HEAD->cause);
        switch ( pASDU_HEAD->type ) {
            case M_SP_NA_1 : /* телесигналы без метки времени*/
                for ( i= 1; i<=pASDU_HEAD->num; i++) {
                    pIEC_IOA = ( TIEC_IOA * ) & bytes[PtB];
                    PtB += sizeof ( TIEC_IOA );
                    pIEC_M_SP_NA_1 = (TIEC_M_SP_NA_1 * )&bytes[PtB];
                    PtB += sizeof(TIEC_M_SP_NA_1);
                    tmpData.CAA = pASDU_HEAD->ca;
                    tmpData.iec_addr = pIEC_IOA->ioa;
                    tmpData.value = 1.0*pIEC_M_SP_NA_1->nfo.nfo_dtl.sp;
                    tmpData.dttm = get_cp56time_now();
                    tmpData.qd = pIEC_M_SP_NA_1->nfo.nfo_hole&~1;
                    if (pTCPassist->IsInList(tmpData.iec_addr)) {
                        pTCPassist->PushData(tmpData);
                        }
                    log(InfData, "АСДУ(%d) addr = %d; value = %d",
                        pASDU_HEAD->ca,
                        pIEC_IOA->ioa,
                        pIEC_M_SP_NA_1->nfo.nfo_dtl.sp);
                }
                break;
            case M_SP_TB_1  : /* Телесигналы с меткой времени */
                for ( i= 1; i<=pASDU_HEAD->num; i++) {
                    pIEC_IOA = ( TIEC_IOA * ) & bytes[PtB];
                    PtB += sizeof ( TIEC_IOA );
                    pIEC_M_SP_TB_1 = (TIEC_M_SP_TB_1 * )&bytes[PtB];
                    PtB += sizeof(TIEC_M_SP_TB_1);
                    tmpData.CAA = pASDU_HEAD->ca;
                    tmpData.iec_addr = pIEC_IOA->ioa;
                    tmpData.value = 1.0*pIEC_M_SP_TB_1->nfo.nfo_dtl.sp;
                    tmpData.dttm = pIEC_M_SP_TB_1->time;
                    tmpData.qd = pIEC_M_SP_TB_1->nfo.nfo_hole&~1;
                    if (pTCPassist->IsInList(tmpData.iec_addr)) {
                        pTCPassist->PushData(tmpData);
                        }
                    log(InfData, "АСДУ(%d) addr = %d; value = %d",
                        pASDU_HEAD->ca,
                        pIEC_IOA->ioa,
                        pIEC_M_SP_TB_1->nfo.nfo_dtl.sp);
                }
                break;
            case M_ME_NC_1 :/* Телеизмерения без метки времени M_ME_NC_1*/
                for ( i = 1; i<=pASDU_HEAD->num; i++ ) {
                    pIEC_IOA = ( TIEC_IOA * ) & bytes[PtB];
                    PtB += sizeof ( TIEC_IOA );
                    pIEC_M_ME_NC_1 =(TIEC_M_ME_NC_1 *)&bytes[PtB];
                    PtB +=sizeof(TIEC_M_ME_NC_1);
                    tmpData.CAA = pASDU_HEAD->ca;
                    tmpData.iec_addr = pIEC_IOA->ioa;
                    tmpData.value = pIEC_M_ME_NC_1->mv;
                    tmpData.dttm = get_cp56time_now();
                    tmpData.qd = pIEC_M_ME_NC_1->sw.sw_hole;

                    if (pTCPassist->IsInList(tmpData.iec_addr)) {
                        pTCPassist->PushData(tmpData);
                        }

                        log(InfData,"++++++++++++++++ АСДУ(%d) addr = %d; value = %8.3f sw = %d",
                            pASDU_HEAD->ca,
                            pIEC_IOA->ioa,
                            pIEC_M_ME_NC_1->mv,
                            pIEC_M_ME_NC_1->sw);

                }
                break;
            case M_ME_TF_1 : /* Измеренные величины с меткой времени */
                for ( i = 1; i<=pASDU_HEAD->num; i++ ) {
                    pIEC_IOA = ( TIEC_IOA * ) & bytes[PtB];
                    PtB += sizeof ( TIEC_IOA );
                    pIEC_M_ME_TF_1 = ( TIEC_M_ME_TF_1 * ) & bytes[PtB];
                    PtB +=sizeof ( TIEC_M_ME_TF_1 );
                    tmpData.CAA = pASDU_HEAD->ca;
                    tmpData.iec_addr = pIEC_IOA->ioa;
                    tmpData.value = pIEC_M_ME_TF_1->mv;
                    tmpData.dttm = pIEC_M_ME_TF_1->time;
                    tmpData.qd = pIEC_M_ME_TF_1->sw_hole;
                    if (pTCPassist->IsInList(tmpData.iec_addr))
                        pTCPassist->PushData(tmpData);
                    log(InfData,"АСДУ(%d) addr = %d; value = %8.3f; timestamp: %.2d:%.2d:%.2d.%.3d  %.2d.%.2d.%d SU = %d",
                        pASDU_HEAD->ca,
                        pIEC_IOA->ioa,
                        pIEC_M_ME_TF_1->mv,
                        pIEC_M_ME_TF_1->time.hour,
                        pIEC_M_ME_TF_1->time.min,
                        (int)(pIEC_M_ME_TF_1->time.msec/1000),
                        pIEC_M_ME_TF_1->time.msec%1000,
                        pIEC_M_ME_TF_1->time.mday,
                        pIEC_M_ME_TF_1->time.month,
                        pIEC_M_ME_TF_1->time.year,
                        pIEC_M_ME_TF_1->time.su
                        );
                }
                break;
            case C_IC_NA_1  :
                log(InfIEC,"Общий опрос подтвержден");
                break;
        }
        LeadUp(T2); /*Каждая APDU (пере)взводит T2. Взводим таймер до отправки подтверждения */
        preW++;
        // Обработка APDU
        if ( Nr<0x7fff ) Nr++;
        else Nr = 0;
        if (preW>=fW)    {
            S_confirm();
            pTCPassist->ForceSendBuf();
        }
        break;
    case CF_S:
        log (InfIEC, "\t>>> S(%d) ",pAPCI->CF.S.nrS);
        if (pAPCI->CF.S.nrS == Ns) {
            Ack = Ns;
            Drop(T1);
        }
        break;
    }
}




