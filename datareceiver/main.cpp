#include <iostream>
#include <getopt.h>
#include "../include/assistent.h"
#include "../include/support.h"
#include "../include/kpi.h"

char *program_name;              /* for error */
bool flDaemon = false;
int cfg_num;
int InfoContent;

cAssistent *pTCP_assist;
cKPI *pKPI;
kpi_struct KpiInit;

/* Перехват сигнала на останов с клавиатуры Ctrl-C */
void onintr(int sig);

int main(int argc, char **argv)
{
    int opt;
    char c_cfg[10];
    char udpDestHost[16];
    int udpDestPort = 26666;
    InfoContent = 1;

    struct {
        bool s, a;
    } args = {false, false};

    INIT();

    //******** Обработка командной строки
    while ((opt = getopt(argc, argv, "s:a:i:d")) != -1) {
        switch (opt) {
        case 's':
            printf("IEC server address:%s\n",optarg);
            strcpy(KpiInit.addr_ip,optarg);
            args.s = true;
            break;
        case 'a':
            printf("Common ASDU address:%s\n",optarg);
            KpiInit.caASDU = atoi(optarg);
            args.a = true;
            break;
        case 'i':
            printf("InfoLevel:%s\n",optarg);
            InfoContent = atoi(optarg);
            break;
        case 'd':
            printf("Daemon mode\n");
            flDaemon = true;
            break;
        }
    }
    if (!(args.s&&args.a)) {
        printf("Заданы не все необходимые аргументы (Server IP (s), Common ASDU address (a))\n");
        exit(0);
    }

    KpiInit.T0 = 30;
    KpiInit.T1 = 15;
    KpiInit.T2 = 10;
    KpiInit.T3 = 20;
    KpiInit.k = 12;
    KpiInit.w = 8;
    // KpiInit.caASDU = 81; // Общий адрес АСДУ перезадать как у фактического
    // strcpy(KpiInit.addr_ip, "192.168.70.16"); //  Перезадать и правильно присвоить
    // strcpy(KpiInit.addr_ip, "10.194.10.26"); //  Перезадать и правильно присвоить
    KpiInit.addr_port = 2404;
    strcpy(udpDestHost, "10.194.10.26"); // Перезадать и правильно присвоить
    udpDestPort = 26666; // Перезадать

    // ******** Создать Асистента (провайдер TCP обмена)
    cAssistent assist=cAssistent(KpiInit.addr_ip,KpiInit.addr_port);
    pTCP_assist = &assist;

    assist.SetUdpAddr(udpDestHost,udpDestPort);
    assist.SetCommonAsduAddr(KpiInit.caASDU);

    // Создать менеджера обмена по МЭК-104
    cKPI kpi = cKPI(KpiInit.T0,KpiInit.T1,KpiInit.T2,KpiInit.T3,KpiInit.k,KpiInit.w,pTCP_assist);
    pKPI = &kpi;
    assist.SetConnectedKPI(pKPI);
    signal (SIGINT, onintr);

    /* Демонизация */
    if (flDaemon) {
        int pid;
        pid = fork(); /* Раздваиваем процесс */
        switch (pid) {
        case 0:
            setsid();
            chdir("/");
            close(0);
            close(1);
            close(2);
            break;
            //                 exit(0);
        case -1:
            printf("Error: unable to fork\n");
            break;

        default:
            printf("Success: process %d went to background\n", pid);
            log(InfDebug, "Process %d went to background", pid);
            exit(0);
            //                 break;
        }
    }
    openlog("datareceiver", LOG_PID, LOG_USER);
    char StartStr[200];
    assist.PrepareTagSet();
    int State = FIRST_PASS;

    do {
        switch (assist.mainloop(State)) {
            /*case 0xf5:
                SendData();
                if (dbprovider.FillTagSet(KpiInit.caASDU)!=0)
                    log(0,0,"Unable to update paramset due to bad dbconnection");
                State = NORMAL_PROLONGATION;
                break;*/
        case -1:
            sleep(KpiInit.T0);
            pTCP_assist->ClearBuffs();
            // Перезапуск программы
            // Можно прописать перезапуск
            // sprintf(StartStr,"/usr/home/vasiliy/bin/datareceiver -b %s -c %s -d",db_host,c_cfg);
            // system(StartStr);
            exit(0xf5);
            break;
        }
    } while (1); // Выход только по -INT
    closelog();
    return 0;
}

void onintr(int sig)
{
    // Добавить действия по корректному завершению
    pTCP_assist->StopDT_req();
    pTCP_assist->~cAssistent();
    log(InfDebug, "Program stoped by INT signal");
    exit(0);
}
