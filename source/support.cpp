#include "../include/support.h"

bool flReadyUDP=false;
int udp_sockfd;
struct sockaddr_in udp_sin;
DataChain DC;

char *ToLower(char *InpStr)
{
    int len = strlen(InpStr);
    for (int i=0;i<len;i++)
        if (isupper(InpStr[i])) InpStr[i]=tolower(InpStr[i]);
    return InpStr;
}

char *AllTrim(const char *src)
{

    int len = strlen(src);
    char *res = new char [len * sizeof(char)];
    char *tmpres = res;
    for (;*src;*src++)
        if (*src!=' ') {
            *tmpres = *src;
            *tmpres++;
        }
    *tmpres = 0;
    return res;
}

void delspc(const char *inStr, char *outStr)
{
    while (*inStr)
    {
        if (*inStr!=' ')
        {
            *outStr=*inStr;
            outStr++;
        }
        inStr++;
    }
    *outStr = '\0';
}

char* toLowerCase(char* str)
{
    int differ = 'A'-'a';
    char ch;
    int ii = strlen(str);
    for (int i=0; i <ii;i++)
    {
        strncpy(&ch,str+i,1);
        if (ch>='A' && ch<='Z')
        {
            ch = ch-differ;
            memcpy(str+i,&ch,1);
        }
    }
    return str;
}

/* error - print a diagnostic and optionally exit */
void error( int status, int err, const char *fmt, ... )
{
    va_list ap;

    va_start( ap, fmt );
    fprintf( stderr, "%s: ", program_name );
    vfprintf( stderr, fmt, ap );
    va_end( ap );
    if ( err )
        fprintf( stderr, ": %s (%d)\n", strerror( err ), err );
    if ( status )
        EXIT( status );
}

void init_udp_connection(const char* udp_host, int udp_port)
{
    memset(&udp_sin, 0, sizeof(udp_sin));
    udp_sin.sin_family = AF_INET;
    udp_sin.sin_port = htons(udp_port);
    inet_pton(AF_INET, udp_host, &udp_sin.sin_addr);
    if ((udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) != -1) {
        flReadyUDP = true;
    } else {
        error(1,10,"error init udp socket");
    }
}

void log(const int InfSt, const char* fmt, ...)
{
    va_list ap;
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    char buf[512];
    char tmp_str[300];
    timeinfo = localtime ( &rawtime );
    va_start( ap, fmt );
    vsprintf( tmp_str,fmt, ap);
    va_end( ap );
    sprintf( buf,"%.2d.%.2d.%d %.2d:%.2d:%.2d > %s\n", timeinfo->tm_mday, timeinfo->tm_mon+1,timeinfo->tm_year+1900,
             timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,tmp_str);
    if (InfoContent &  InfSt) {
        if (!flDaemon) {
            printf("%s",buf);
        }
    }
}

void sub_for_n(char *dest, char *sours, char c, short n)
{
    short j=0, k=0;
    int i=0;
    char s;
    do
    {
        s=sours[i];
        if ((j==n)&&(c!=s)) dest[k++]=sours[i];
        if ((c==s)||(c<' '))j++;
        i++;
    } while ((i<=0xFFFF)&&(s!='\0')&&(j<=n));
    if (i<=0xFFFF)dest[k]='\0';
}

timeval get_check_tm(const timeval tm, const timeval interval)
{
    timeval last_tm;
    int rs = tm.tv_sec / 60 * 60+60;//выравнивание по началу следующей минуты

    last_tm.tv_sec = rs + interval.tv_sec;
    last_tm.tv_usec = tm.tv_usec + interval.tv_usec;
    if (last_tm.tv_usec >= 1000000) {
        last_tm.tv_sec++;
        last_tm.tv_usec -=1000000;
    }
    return last_tm;
}

timeval get_est_tm(const timeval tm1, const timeval tm2) /*дать разницу во времени tm2-tm1*/
{
    timeval est_tm;
    if (tm2.tv_usec>=tm1.tv_usec) {
        est_tm.tv_sec = tm2.tv_sec - tm1.tv_sec;
        est_tm.tv_usec = tm2.tv_usec - tm1.tv_usec;
    }
    else
    {
        est_tm.tv_sec = tm2.tv_sec - 1 - tm1.tv_sec;
        est_tm.tv_usec = tm2.tv_usec + 1000000 - tm1.tv_usec;
    }
    return est_tm;
}

void PushData(fdata SD)
{
//     log(InfDebug,"Вход в процедуру %s","PushData");
    if (SD.dttm.year<=0) return;
    DC.DataChain[DC.DataPtr] = SD;
    DC.DataPtr++;
    if (DC.DataPtr==MaxDataPtr)  SendData();
//     log(InfDebug,"Выход из процедуры %s","PushData");
}

void SendData()
{
//     log(InfDebug,"Вход в процедуру %s","SendData");
    if (flReadyUDP && (DC.DataPtr>0)) {
        if (sendto(udp_sockfd, &DC, sizeof(DC), 0, (struct sockaddr *) &udp_sin, sizeof(udp_sin)) != 0)
            error(0,0,"error while sending UDP");
    }
    InitDataChain();
//     log(InfDebug,"Выход из процедуры %s","SendData");
}

void InitDataChain()
{
    DC.DataPtr = 0;
    memset(DC.DataChain,0,sizeof(DC.DataChain));
}
