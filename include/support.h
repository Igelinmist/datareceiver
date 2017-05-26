#ifndef SUPPORT_H
#define SUPPORT_H
/* Include standard headers */
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/stat.h>
// #include <sys/event.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <syslog.h>
#include <vector>
#include <set>
#include <fstream>
#include "iec104types.h"


using namespace std;

#define TRUE                    1
#define FALSE                   0
#define NLISTEN                 5               /* max waiting connections */
#define NSMB                    5               /* number shared memory bufs */
#define SMBUFSZ                 256             /* size of shared memory buf */

typedef int SOCKET;
//typedef void ( *tofunc_t )( void * );
typedef set<int> set_t;
typedef void (*ERROR_LOG)(const char*,...);

extern char *program_name;              /* for error */
extern int InfoContent; /* Уровень информативности сообщений. 1 бит - только APDU, 2 бит - добавляет данные, 3 - сообщения для отладки,
4 - потоки байтов*/
extern bool flDaemon;
extern set_t tagset;
extern set_t::iterator t_iter;
extern const char *udpHost;
extern int udpPort;
extern int cfg_num;


const int InfIEC = 0x0001;// Константы расположения битов в  флаге уровня информативности
const int InfData = 0x0002;
const int InfDebug = 0x0004;
const int InfByte = 0x0008;

enum InfoStatus{stIEC,stData,stDebug,stByte};

#ifdef __SVR4
#define bzero(b,n)      memset( ( b ), 0, ( n ) )
#endif

#define INIT() ( program_name = \
strrchr ( argv[ 0 ], '/' ) ) ? \
program_name++ : \
( program_name = argv[ 0 ] )

#define EXIT(s) exit( s )

#define CLOSE(s) if ( close( s ) ) error( 1, errno, \
"ошибка close " )

#define set_errno(e) errno = ( e )

#define isvalidsock(s) ( ( s ) >= 0 )

/*Структура инициализации интерфейса к КП*/
struct kpi_struct {
     int caASDU;
     char addr_ip[15];
     int addr_port;
     int T0;
     int T1;
     int T2;
     int T3;
     int k;
     int w;
};

/*Максимальное количество данных в посылке UDP*/
const int MaxDataPtr = 255;
#pragma pack(push,1)
struct  fdata{
    int CAA; /*Common ASDU Address*/
    int iec_addr; /*Адрес МЭК-104*/
    float value;
    byte qd; /*Описатель качества*/
    cp56time2a dttm;
};
/*структура цепочки данных*/
struct DataChain {
    int DataPtr;/*количество данных в цепочке (указатель на последнее записанное данное)*/
    fdata DataChain[MaxDataPtr];
};
#pragma pack(pop)

void PushData(fdata);

void InitDataChain();

void SendData();

char *ToLower(char *InpStr);

char *AllTrim(const char *src);

char* toLowerCase(char* str);

void delspc(const char *inStr, char *outStr); /*Процедура удаления пробелов*/

void error( int, int, const char*, ... ); /*Печать сообщения + ошибка + потенциальный выход*/

void log(const int, const char* , ...);

void init_udp_connection(const char *udp_host="localhost", int udp_port = 26666);

void sub_for_n(char *dest, char *sours, char c, short n); /* Процедура поиска подстроки по номеру с заданным разделителем подстрок */

timeval get_check_tm(const timeval tm, const timeval interval); /* дать контрольное время (срабатываение таймера)*/

timeval get_est_tm(const timeval tm1, const timeval tm2);  /*дать разницу во времени tm2-tm1*/




#endif  /* SUPPORT_H*/
