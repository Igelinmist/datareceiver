#include "../include/assistent.h"
#include "../include/kpi.h"
#include "../include/iec104act.h"

cAssistent::cAssistent(char* host, int port)
{
    strcpy(fHost,host);
    fPort = port;
    interval_tm.tv_sec = 1;
    interval_tm.tv_usec = 0;
    interval_sd.tv_sec = TS_period;
    interval_sd.tv_usec = 0;
    select_tm.tv_sec = 0;
    select_tm.tv_usec = 10000;
    flen = 0;
    InitDataChain();
}

void cAssistent::SetConnectedKPI(cKPI *prmKPI)
{
    fKPI = prmKPI;
}

void cAssistent::PushWriteBuf(const void* buf, int len)
{
    byte *chain = new byte[len];
    memcpy(chain,buf,len);
    for (int i = 0; i < len; i++) {
            printf("%02x ", chain[i]);
            wBuf.push(chain[i]);}
    delete [] chain;
}

void cAssistent::PushReadBuf(const void* buf, int len)
{
    byte *chain = new byte[len];
    memcpy(chain,buf,len);
    for (int i = 0; i < len; i++)
        rBuf.push(chain[i]);
    delete [] chain;
}

int cAssistent::OpenTCPSocket()
{
    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port  = htons ( fPort );
    peer.sin_addr.s_addr = inet_addr ( fHost );
    fS = socket ( AF_INET,SOCK_STREAM,0 );

    if ( !isvalidsock ( fS ) ) {
        printf("Bad socket\n");
        syslog(LOG_ERR,"<%d> Unable to create socket with error:%m",cfg_num);
        return 1;
    }
    if ( connect ( fS, ( struct sockaddr * ) &peer,sizeof ( peer ) ) ==-1 ) {
        syslog(LOG_ERR,"<%d> Unable connect to socket with error:%m",cfg_num);
        close ( fS );
        return 1; // ошибка при подключении к TCP-серверу. Возможно сервер еще поднимется
    }
    //printf("try set nonblock sock\n");
    //fcntl(fS, F_SETFL, O_NONBLOCK);
    //printf("set nonblock sock\n");
    return 0;
}

void cAssistent::init_udp_connection(const char *udp_host, int udp_port)
{
    memset(&fudp_sin, 0, sizeof(fudp_sin));
    fudp_sin.sin_family = AF_INET;
    fudp_sin.sin_port = htons(udp_port);
    inet_pton(AF_INET, udp_host, &fudp_sin.sin_addr);
    fudp_sockfd = socket( PF_INET, SOCK_DGRAM, 0 );
    if (!isvalidsock(fudp_sockfd)) error(1,10,"error init udp socket");

}

void cAssistent::InitDataChain()
{
    fDC.DataPtr = 0;
    memset(fDC.DataChain,0,sizeof(fDC.DataChain));//Заполнение нулями массива структур данных
}

void cAssistent::SendData()
{
    int sendres;

    if (fDC.DataPtr>0) {
        sendres = sendto(fudp_sockfd, &fDC, sizeof(fDC), 0, (struct sockaddr *) &fudp_sin, sizeof(fudp_sin));
    }
    InitDataChain();
}

int cAssistent::SendBuff()
{
    log(InfDebug,"Entering SendBuff");
    int rc,len;
    len = wBuf.size();
    byte *buf = new byte[len];

    for (int i = 0; i < len; i++) {
        buf[i] = wBuf.front();
        log(InfDebug,"%.2x ",buf[i]);
        wBuf.pop();
    }
    log(InfDebug,"\n");
    rc = send(fS,buf,len,0);
    if (rc==-1) {
        syslog(LOG_ERR,"<%d> Error while sending data:%m",cfg_num);
        return -1;
    }
    log(InfDebug,"Sent %d byte",rc);
    delete [] buf;
    return 0;
    log(InfDebug,"Exit(0) SendBuff");
}


void cAssistent::Init_tm()
{
    gettimeofday ( &cur_tm, NULL );
    check_tm = get_check_tm ( cur_tm, interval_tm );
    check_sd = get_check_tm ( cur_tm, interval_sd );
}

int cAssistent::GetSocketMsg()
{
    log(InfDebug,"Entering GetSocketMsg");
    int rc;
    int res = 0;
    byte ReadBuff[MaxBufLen];
    do {
        rc = recv(fS,ReadBuff,MaxBufLen,0);
        if (rc==-1) {
            syslog(LOG_ERR,"<%d> Error while receiving data:%m",cfg_num);
            log(InfDebug,"Exit(-1) GetSocketMsg");
            return -1;
        }
        if (rc==0) {
            log(InfDebug,"Exit(-1) GetSocketMsg");
            return -1;
        }
        res +=rc;
        PushReadBuf(ReadBuff,rc);
    } while (rc==MaxBufLen);
    log(InfDebug,"Exit(%d) GetSocketMsg",res);
    return res;
}

void cAssistent::AllocateASDU()
{
    /*
    Разобрать FIFO очередь входящих сообщений на ASDU. Для обнаруженных и полных ASDU
     вызвать метод kpi по обработке
    */
    byte buf[255];
    unsigned int i;

    if (flen > 0) {
        if (flen>rBuf.size())
            return;
        buf[0] = 0x68;
        buf[1] = flen;
        for (i = 2; i<flen+2; i++) {
            buf[i] = rBuf.front();
            rBuf.pop();
        }
        flen = 0;
        fKPI->ParseMessage(buf);
    }
    while (rBuf.size()>=6) {
        buf[0] = rBuf.front();
        rBuf.pop();
        if (buf[0] != 0x68)
            continue;
        buf[1] = rBuf.front();
        rBuf.pop();
        flen = buf[1];
        if (flen>rBuf.size())
            return;
        for (i = 2; i<flen+2; i++) {
            buf[i] = rBuf.front();
            rBuf.pop();
        }
        flen = 0;
        fKPI->ParseMessage(buf);
    }

}

void cAssistent::StopDT_req()
{
    log(InfIEC,"<<< %s","STOPDT_act" );
    TAPCI nlc = NumlessControl ( STOPDT_act );
    int rc = send(fS,&nlc,6,MSG_DONTWAIT);
    if (rc==-1) {
        log(InfDebug,"Result StopDT_req = %d",rc);
    }
    log(InfDebug,"Result StopDT_req = %d",rc);
    log(InfDebug,"Exit StopDT_req");
}

void cAssistent::DropSocket()
{
    log(InfIEC,"--- %s","DropSocket" );
    ClearBuffs();
    shutdown(fS,2);
    close(fS);
    log(InfDebug,"Exit DropSocket");
}

int cAssistent::StartDT_req()
{
    //printf("start_dt req");
    TAPCI nlc = NumlessControl ( STARTDT_act );
    int rc = send(fS,&nlc,6,0);
    if (rc < 0) {
        log(InfIEC, "Error on STARTDT_act (%d)", rc);
        return -1;
    }
    log(InfIEC,"<<< %s","STARTDT_act" );
    return 0;
}

int cAssistent::mainloop(int& pState)
{
    if (pState!=NORMAL_PROLONGATION) {
        if (OpenTCPSocket()==0) init_udp_connection(fUDPHost,fUDPPort);
        else {
            //printf("socket not opend\n");
            return -1; /* Сокет не открыт, поэтому выходим для ожидания*/
        }

        Init_tm();
        ClearBuffs();
        if (StartDT_req()!=0) return -1;
    }
    int CicleCnt = 0;
    while (1) {
        FD_ZERO ( &rmask ); /* инициализация (очистка) множества */
        FD_SET ( fS,&rmask );
        tmp_tm = select_tm;
        sres = select ( fS+1,&rmask, NULL, NULL,&tmp_tm );
        if ( sres < 0 ) {
            syslog(LOG_ERR,"<%d> Error while SELECT (descriptor event intercept):%m",cfg_num);
            log(InfDebug, "Error while recv select %d(%d)", sres, errno);
            DropSocket();
            return -1;
        }

        if ((sres>0)&&(FD_ISSET(fS,&rmask))) {
            if (GetSocketMsg()!=-1)
                AllocateASDU();
            else {
                DropSocket();
                return -1;
            }
        }
        if (!wBuf.empty()) {
            if (SendBuff()==-1) {
                DropSocket();
                return -1;
            }
        }
        /*блок контроля таймера*/
        gettimeofday ( &cur_tm, NULL );
        if ( timercmp ( &cur_tm, &check_tm,> ) ) { /*если перешли в следующую секунду*/
            check_tm.tv_sec++; /*передвигаем границу контроля*/
            /*Сигнализируем интерфейса к КП о необходимости проверки таймаутов*/
            if (fKPI->OnTimer()==-1) {
                return -1;
            }
        }
        if ( timercmp ( &cur_tm, &check_sd,> ) ) { /*если перешли отметку времени для сохранения*/
            check_sd.tv_sec = check_sd.tv_sec+TS_period; /*передвигаем границу сохренения*/
            SendData();
            PrepareTagSet();
            CicleCnt++;
            if (CicleCnt==60*60/TS_period) {
                CicleCnt = 0;
                fKPI->AskPolling(1);
            }
            //return 0xf5; /* Выходим из цикла для отправки данных */
        }

        pState = NORMAL_PROLONGATION;
    }
    printf("Exit\n");
    return 0;
}

void cAssistent::ClearBuffs()
{
    while (rBuf.size()>0) rBuf.pop();
    while (wBuf.size()>0) wBuf.pop();
}

void cAssistent::ForceSendBuf()
{
    SendBuff();
}

cAssistent::~cAssistent()
{
    DropSocket();
}

void cAssistent::PrepareTagSet()
{
    // fDBprov->FillTagSet(fCAA,fTagSet);
    // Инициализировать множество целых адресов МЭК, на которые мы подписаны
}

bool cAssistent::IsInList(int addr)
{
    if (fTagSet.count(addr)==0) return false;
    else return true;
}

void cAssistent::PushData(fdata SD)
{
//     log(InfDebug,"Вход в процедуру %s","PushData");
    fDC.DataChain[fDC.DataPtr] = SD;
    fDC.DataPtr++;
    if (fDC.DataPtr==MaxDataPtr)  SendData();
//     log(InfDebug,"Выход из процедуры %s","PushData");
}





