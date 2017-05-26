#ifndef TASSISTENT_H
#define TASSISTENT_H
#include <queue>
#include "support.h"

const int TS_period = 20;
const int MaxBufLen = 255;
const int FIRST_PASS = 0xf9;
const int NORMAL_PROLONGATION = 0xf3;
const int REFRESH = 0xf5;


class cKPI;//Forward declaration

class cDBProvider;//Forward declaration

class cAssistent
{
private:
    std::queue <byte> rBuf, wBuf;
    SOCKET fS;
    char fHost[16];
    int fUDPPort;
    char fUDPHost[16];
    int fPort;
    timeval cur_tm; /* время текущее для таймера*/
    timeval check_tm; /* время проверки таймаутов МЭК */
    timeval check_sd; /* время проверки времени сохранения */
    timeval interval_tm; /* интервал таймера*/
    timeval interval_sd; /* интервал сохранения данных*/
    timeval select_tm; /* интервал действия select в режиме ожидания событий на дескрипторах*/
    timeval tmp_tm;
    int sres; /* переменная для получения результата select */
    fd_set rmask; /* маска дескрипторов для чтения, проверяется select */
    fd_set wmask; /* маска дескрипторов для записи, проверяется  select */
    cKPI *fKPI;
    unsigned int flen; /*Ожидаемая длина ASDU. Если 0, то последнее сообщение получено полностью*/
    struct sockaddr_in fudp_sin;
    int fudp_sockfd;
    DataChain fDC;
    set_t fTagSet;
    int fCAA;

    int SendBuff();
    int GetSocketMsg();
    void AllocateASDU();
    void DropSocket();
    void InitDataChain();

    void SendData();
public:
    cAssistent(char* host,int port);
    ~cAssistent();
    void ClearBuffs();
    void SetConnectedKPI(cKPI *prmKPI);
    int OpenTCPSocket();
    void init_udp_connection(const char *udp_host="localhost", int udp_port = 26666);
    /* Основной цикл тайминга, опроса сокета и передачи данных.
    Возвращает: 0xf5 при необходимости  перечитать тэги,
    -1 при ошибке, 0 "нормальный" выход */
    int mainloop(int &pState);
    void Init_tm();
    void PushWriteBuf(const void *buf,int len);
    void PushReadBuf(const void *buf,int len);
    void StopDT_req();
    void ForceSendBuf();
    int StartDT_req();
    void PrepareTagSet();
    void SetCommonAsduAddr(int caa) {fCAA = caa;}
    void SetUdpAddr(const char* pHost,int pPort){strcpy(fUDPHost,pHost);fUDPPort = pPort;}
    bool IsInList(int addr);
    void PushData(fdata);
};

#endif // TASSISTENT_H
