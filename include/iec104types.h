#ifndef IEC104TYPES_H
#define IEC104TYPES_H

// Типы APDU
#define APDU_TYPE_Err  -1
#define APDU_TYPE_I    0
#define APDU_TYPE_S    1
#define APDU_TYPE_U    3

typedef  unsigned char byte;
typedef unsigned short word;
typedef unsigned int cardinal;


// Типы ASDU
typedef enum _ASDU_TYPE {
    ASDU_UNKNOWN =   0, // Неизвестный ASDU
    C_IC_NA_1    = 100, // 0x64 - Команда опроса
    C_CS_NA_1    = 103, // 0x67 - Команда синхронизации времени
    C_SC_NA_1    =  45, // 0x2D - Однопозиционная команда
    C_DC_NA_1    =  46, // 0x2E - Двухпозиционная команда
    C_SE_NB_1    =  49, // 0x31 - Команда уставки, масштабированное значение
    C_SE_NC_1    =  50, // 0x32 - Команда уставки, короткое число с плавающей запятой
    C_SC_TA_1    =  58, // 0x3A - Одноэлнементная команда с меткой времени CP56Время2а
    C_SE_TB_1    =  62, // 0x3E - Команда уставки, масштабированное значение с меткой времени CP56Время2а
    C_SE_TC_1    =  63, // 0x3F - Команда уставки, короткое число с плавающей запятой с меткой времени CP56Время2а
    C_TS_NA_1    = 104, // 0x68 - Команда тестирования
    C_TS_TA_1    = 107, // 0x6B - Тестовая команда с меткой времени CP56Время2а
    KL_HIST      = 136, // 0x88 - Запрос истории (KLogic)
    M_SP_NA_1    =   1, // 0x01 - single point information with quality description
    M_SP_TB_1    =  30, // 0x1E - Одноэлементная информация с меткой времени CP56Время2а
    M_ME_NC_1    =  13, // 0x0D - short floating point measured value
    M_ME_TF_1    =  36, // 0x24 - Значение измеряемой величины, короткий формат с плавающей запятой с меткой времени CP56Время2а
    M_ME_NA_1    =   9, // 0x09 - Значение измеряемой величины, нормализованное значение
    M_ME_NB_1    =  11, // 0x0B - Значение измеряемой величины, масштабированное значение
    M_ME_TD_1    =  34, // 0x22 - Значение измеряемой величины, нормализованное значение с меткой времени CP56Время2а
    M_ME_TE_1    =  35, // 0x23 - Значение измеряемой величины, масштабированное значение с меткой времени CP56Время2а
    M_ME_ND_1    =  21, // 0x15 - Значение измеряемой величины, нормализованное значение без описателя качества
    M_DP_NA_1    =  3,  // 0x03 - Двухэлементная информация без метки времени
    M_DP_TB_1    =  31, // 0x1F - Двухэлементная информация с меткой времени CP56Время2а
    M_IT_NA_1    =  15, // 0x0F - Интегральная сумма
    M_IT_TB_1    =  37, // 0x25 - Интегральная сумма с меткой времени CP56Время2а
    ASDU_ERROR   =  255 // Неверный ASDU
} __attribute__ ((packed)) ASDU_TYPE;


// Идентификаторы причины передачи
//       <0>              // по умолчанию
//       <1>              // периодически,циклически
//       <2>              // фоновое сканирование
#define CAUSE_SPORADIC 3 // спорадически
//       <4>              // сообщение о инициализации
//       <5>              // запрос или запрашиваемое
#define CAUSE_ACTIVATION 6 // активация
#define CAUSE_ACTIVATION_CON 7 // подтверждение активации
//       <8>              // деактивация
//       <9>              // подтверждение деактивации
#define CAUSE_ACTIVATION_END 10 // завершение активации
//       <11>             // обратная информация, вызванная удаленной командой
//       <12>             // обратная информация, вызванная местной командой
//       <13>             // передача файлов
#define CAUSE_COMMON_ANSWER 20 // ответ на общий опрос
//       <21>             // ответ на опрос группы 1
//       <22>             // ответ на опрос группы 2
//       <23>             // ответ на опрос группы 3
//       <24>             // ответ на опрос группы 4
//       <25>             // ответ на опрос группы 5
//       <26>             // ответ на опрос группы 6
//       <27>             // ответ на опрос группы 7
//       <28>             // ответ на опрос группы 8
//       <29>             // ответ на опрос группы 9
//       <30>             // ответ на опрос группы 10
//       <31>             // ответ на опрос группы 11
//       <32>             // ответ на опрос группы 12
//       <33>             // ответ на опрос группы 13
//       <34>             // ответ на опрос группы 14
//       <35>             // ответ на опрос группы 15
//       <36>             // ответ на опрос группы 16
//       <37>             // ответ на общий запрос счетчиков
//       <38>             // ответ на запрос группы счетчиков 1
//       <39>             // ответ на запрос группы счетчиков 2
//       <40>             // ответ на запрос группы счетчиков 3
//       <41>             // ответ на запрос группы счетчиков 4
#define CAUSE_KL_HIST_ANSWER 48 // ответ на запрос истории

#define IEC_POLL 0x14 // признак общего опроса станции
#define KL_POLL 0x40 // признак общего опроса станции KLogicMngr'ом

typedef struct _APCI {
    byte sb;/*стартовый байт*/
    byte len;/*длина ASDU + поле контроля 4 байта*/
    union  {
        struct {
            unsigned int idI : 1; // id = APDU_TYPE_I
            unsigned int nsI : 15;
            unsigned int dummyI  : 1;
            unsigned int nrI  : 15;
        } I;
        struct {
            unsigned int idS : 2; // id = APDU_TYPE_S
            unsigned int dummyS : 15;
            unsigned int nrS  : 15;
        } S;
        cardinal  hole;/*поле контроля цельным значением*/
    } CF;

} __attribute__ ((packed)) TAPCI;

// Data unit identifier block - ASDU header
typedef struct _ASDU_HEAD {
    byte type;    /* type identification */
    byte num  :7; /* number of information objects */
    byte sq   :1; /* sequenced/not sequenced address */
    byte cause:6; /* cause of transmission */
    byte pn   :1; /* positive/negative app. confirmation */
    byte t    :1; /* test */
    byte cause_h:8; /* cause of transmission  - station addr*/  // Originator Address
    byte ca;      /* common address of ASDU */
    byte ca_h;
} __attribute__ ((packed)) TASDU_HEAD;

/* Information Object Address */
typedef struct _IEC_IOA {
    short        ioa;
    byte        ioa_h;  /* 0 */
} __attribute__ ((packed))
TIEC_IOA;

/* CP56Time2a timestamp */
typedef struct {
    word       msec;
    byte        min     :6;
    byte        res1    :1;
    byte        iv      :1;
    byte        hour    :5;
    byte        res2    :2;
    byte        su      :1;
    byte        mday    :5;
    byte        wday    :3;
    byte        month   :4;
    byte        res3    :4;
    byte        year    :7;
    byte        res4    :1;
} __attribute__ ((packed)) cp56time2a;

// M_SP_NA_1 - single point information with quality description
// 1 - Одноэлементная информация
typedef struct {
    union {
        struct {
            byte    sp  :1; /* single point information */
            byte    res :3;
            byte    bl  :1; /* blocked/not blocked */
            byte    sb  :1; /* substituted/not substituted */
            byte    nt  :1; /* not topical/topical */
            byte    iv  :1; /* valid/invalid */
        } nfo_dtl;
        byte nfo_hole;
    } nfo;
} __attribute__ ((packed)) TIEC_M_SP_NA_1;

// M_DP_NA_1
// 3 - Двухэлементная информация без метки времени
typedef struct _IEC_M_DP_NA_1 {
    byte    dpi :2; /* Двухэлементная информация (0-3)*/
    byte    res :2;
    byte    bl  :1; /* blocked/not blocked */
    byte    sb  :1; /* substituted/not substituted */
    byte    nt  :1; /* not topical/topical */
    byte    iv  :1; /* valid/invalid */
} __attribute__ ((packed)) IEC_M_DP_NA_1;

// M_ME_NA_1 - normalized measured value
// 9 - Значение измеряемой величины, нормализованное значение
typedef struct {
    short       mv; /* normalized value */
    byte        ov  :1; /* overflow/no overflow */
    byte        res :3;
    byte        bl  :1; /* blocked/not blocked */
    byte        sb  :1; /* substituted/not substituted */
    byte        nt  :1; /* not topical/topical */
    byte        iv  :1; /* valid/invalid */
} __attribute__ ((packed)) IEC_M_ME_NA_1;

// M_ME_NB_1 - scaled measured value
// 11 - Значение измеряемой величины, масштабированное значение
typedef struct _IEC_M_ME_NB_1 {
    short       mv; /* scaled value */
    byte        ov  :1; /* overflow/no overflow */
    byte        res :3;
    byte        bl  :1; /* blocked/not blocked */
    byte        sb  :1; /* substituted/not substituted */
    byte        nt  :1; /* not topical/topical */
    byte        iv  :1; /* valid/invalid */
} __attribute__ ((packed)) IEC_M_ME_NB_1;

// M_ME_NC_1 - short floating point measured value
// 13 - Значение измеряемой величины, короткий формат с плавающей запятой
typedef struct {
    float       mv;
    union {
        struct {
            byte        ov  :1; /* overflow/no overflow */
            byte        res :3;
            byte        bl  :1; /* blocked/not blocked */
            byte        sb  :1; /* substituted/not substituted */
            byte        nt  :1; /* not topical/topical */
            byte        iv  :1; /* valid/invalid */
        } sw_dtl;
        byte sw_hole;
    } sw;
} __attribute__ ((packed))  TIEC_M_ME_NC_1;

// M_IT_NA_1
// 15 - Интегральная сумма
typedef struct _IEC_M_IT_NA_1 {
    int         mv;
    byte        sq  :5; /* номер последовательности */
    byte        cy  :1; /* за соответствующий период интегрирования не произошло переполнение счетчика/за соответствующий период интегрирования произошло переполнение счетчика */
    byte        ca  :1; /* после последнего считывания счетчик не был установлен/после последнего считывания счетчик был установлен */
    byte        iv  :1; /* valid/invalid */
} __attribute__ ((packed)) IEC_M_IT_NA_1;

// M_ME_ND_1 - normalized measured value without quality descriptor
// 21 - Значение измеряемой величины, нормализованное значение без описателя качества
typedef struct _IEC_M_ME_ND_1 {
    short       mv; /* normalized value */
} __attribute__ ((packed)) IEC_M_ME_ND_1;

// M_SP_TB_1 - single point information with quality description and time tag
// 30 - Одноэлементная информация с меткой времени CP56Время2а
typedef struct {
    union {
        struct {
            byte        sp  :1; /* single point information */
            byte        res :3;
            byte        bl  :1; /* blocked/not blocked */
            byte        sb  :1; /* substituted/not substituted */
            byte        nt  :1; /* not topical/topical */
            byte        iv  :1; /* valid/invalid */
        } nfo_dtl;
        byte nfo_hole;
    } nfo;
    cp56time2a  time;
} __attribute__ ((packed)) TIEC_M_SP_TB_1;

// M_DP_TB_1
// 31 - Двухэлементная информация с меткой времени CP56Время2а
typedef struct _IEC_M_DP_TB_1 {
    byte    dpi :2; /* Двухэлементная информация (0-3)*/
    byte    res :2;
    byte    bl  :1; /* blocked/not blocked */
    byte    sb  :1; /* substituted/not substituted */
    byte    nt  :1; /* not topical/topical */
    byte    iv  :1; /* valid/invalid */
    cp56time2a  time;
} __attribute__ ((packed)) IEC_M_DP_TB_1;

/* M_ME_TD_1 -  normalized measured value with time tag */
// 34 - Значение измеряемой величины, нормализованное значение с меткой времени CP56Время2а
typedef struct _IEC_M_ME_TD_1 {
    short       mv; /*  normalized value */
    byte        ov  :1; /* overflow/no overflow */
    byte        res :3;
    byte        bl  :1; /* blocked/not blocked */
    byte        sb  :1; /* substituted/not substituted */
    byte        nt  :1; /* not topical/topical */
    byte        iv  :1; /* valid/invalid */
    cp56time2a  time;
} __attribute__ ((packed)) IEC_M_ME_TD_1;

/* M_ME_TE_1 - scaled measured value with time tag*/
// 35 - Значение измеряемой величины, масштабированное значение с меткой времени CP56Время2а
typedef struct _IEC_M_ME_TE_1 {
    short       mv; /* scaled value */
    byte        ov  :1; /* overflow/no overflow */
    byte        res :3;
    byte        bl  :1; /* blocked/not blocked */
    byte        sb  :1; /* substituted/not substituted */
    byte        nt  :1; /* not topical/topical */
    byte        iv  :1; /* valid/invalid */
    cp56time2a  time;
} __attribute__ ((packed)) IEC_M_ME_TE_1;

// M_ME_TF_1 - short floating point measurement value and time tag
// 36 - Значение измеряемой величины, короткий формат с плавающей запятой с меткой времени CP56Время2а
typedef struct {
    float       mv;
    union {
        struct {
            byte        ov  :1; /* overflow/no overflow */
            byte        res :3;
            byte        bl  :1; /* blocked/not blocked */
            byte        sb  :1; /* substituted/not substituted */
            byte        nt  :1; /* not topical/topical */
            byte        iv  :1; /* valid/invalid */
        } sw_dtl;
        byte sw_hole;
    };

    cp56time2a  time;
} __attribute__ ((packed)) TIEC_M_ME_TF_1;

// M_IT_TB_1
// 37 - Интегральная сумма с меткой времени CP56Время2а
typedef struct _IEC_M_IT_TB_1 {
    int         mv;
    byte        sq  :5; /* номер последовательности */
    byte        cy  :1; /* за соответствующий период интегрирования не произошло переполнение счетчика/за соответствующий период интегрирования произошло переполнение счетчика */
    byte        ca  :1; /* после последнего считывания счетчик не был установлен/после последнего считывания счетчик был установлен */
    byte        iv  :1; /* valid/invalid */
    cp56time2a  time;
} __attribute__ ((packed)) IEC_M_IT_TB_1;


// C_IC_NA_1 - Команда опроса
typedef struct _ASDU_C_IC_NA_1 {
    TASDU_HEAD ASDUhead; // Заголовок  type == 0x64  num == 1  sq == 0
    TIEC_IOA IOA; // 0
    byte QOI; // описатель запроса  QOI == 20 - общий опрос станции; 64 - опрос KLogicMngr'ом
} __attribute__ ((packed)) ASDU_C_IC_NA_1;

// C_CS_NA_1 - Команда синхронизации времени
typedef struct _ASDU_C_CS_NA_1 {
    TASDU_HEAD ASDUhead; // Заголовок  type == 0x67  num == 1  sq == 0
    TIEC_IOA IOA; // 0
    cp56time2a time; // время
} __attribute__ ((packed)) ASDU_C_CS_NA_1;

// C_SC_NA_1 - Однопозиционная команда
typedef struct _ASDU_C_SC_NA_1 {
    TASDU_HEAD ASDUhead; // Заголовок  type == 0x2D  num == 1  sq == 0
    TIEC_IOA IOA; // адрес
//  struct SCO
//  {
    byte SCS : 1; // 0 - ВЫКЛ, 1 - ВКЛ
    byte res : 1; // 0
    // QOC : 6;  - указатель команд
    byte QU  : 5; // 0 - нет дополнительного определения
    byte SE  : 1; // 0 - исполнение, 1 - выбор
//  };
} __attribute__ ((packed)) ASDU_C_SC_NA_1;

// C_SC_TA_1 - Одноэлементная команда с меткой времени CP56Время2а
typedef struct _ASDU_C_SC_TA_1 {
    ASDU_C_SC_NA_1 iec_C_SC_NA_1;
    cp56time2a time;
} __attribute__ ((packed)) ASDU_C_SC_TA_1;

// C_DC_NA_1 - двухпозиционная команда
typedef struct _ASDU_C_DC_NA_1 {
    TASDU_HEAD ASDUhead; // Заголовок  type == 0x2E  num == 1  sq == 0
    TIEC_IOA IOA; // адрес
    byte DCS : 2; // 0 - неразрешенная 1 - ВЫКЛ 2 - ВКЛ 3 - неразрешенная
    byte QU  : 5; // 0 - нет дополнительного определения
    byte SE  : 1; // 0 - исполнение, 1 - выбор
} __attribute__ ((packed)) ASDU_C_DC_NA_1;

// C_SE_NB_1 - Команда уставки, масштабированное значение
typedef struct _ASDU_C_SE_NB_1 {
    TASDU_HEAD ASDUhead; // Заголовок  type == 0x31  num == 1  sq == 0
    TIEC_IOA IOA; // адрес
    short int NVA; // значение
//  struct // Указатель команды уставки
//  {
    byte QL : 7; // 0
    byte SE : 1; // 0 - исполнение, 1 - выбор
//  };
} __attribute__ ((packed)) ASDU_C_SE_NB_1;

// C_SE_TB_1 - Команда уставки, масштабированное значение с меткой времени CP56Время2а
typedef struct _ASDU_C_SE_TB_1 {
    ASDU_C_SE_NB_1 iec_C_SE_NB_1;
    cp56time2a time;
} __attribute__ ((packed)) ASDU_C_SE_TB_1;

// C_SE_NC_1 - Команда уставки, короткое число с плавающей запятой
typedef struct _ASDU_C_SE_NC_1 {
    TASDU_HEAD ASDUhead; // Заголовок  type == 0x32  num == 1  sq == 0
    TIEC_IOA IOA; // адрес
    float mv; // значение
//  struct  // Указатель команды уставки
//  {
    byte QL : 7; // 0
    byte SE : 1; // 0 - исполнение, 1 - выбор
//  };
} __attribute__ ((packed)) ASDU_C_SE_NC_1;

// C_SE_TC_1 - Команда уставки, короткое число с плавающей запятой с меткой времени CP56Время2а
typedef struct _ASDU_C_SE_TC_1 {
    ASDU_C_SE_NC_1 iec_C_SE_NC_1;
    cp56time2a time;
} __attribute__ ((packed)) ASDU_C_SE_TC_1;

// C_TS_NA_1 - Команда тестирования
typedef struct _ASDU_C_TS_NA_1 {
    TASDU_HEAD ASDUhead; // Заголовок  type == 0x68  num == 1  sq == 0
    TIEC_IOA IOA; // адрес = 0
    short FSP; // Фиксированный образец теста = 0x55AA
} __attribute__ ((packed)) ASDU_C_TS_NA_1;

// C_TS_TA_1 - Тестовая команда с меткой времени CP56Время2а
typedef struct _ASDU_C_TS_TA_1 {
    TASDU_HEAD ASDUhead; // Заголовок  type == 0x6B  num == 1  sq == 0
    TIEC_IOA IOA; // адрес объекта информации
    short TSC; // Счетчик тестовой последовательности
    cp56time2a time;
} __attribute__ ((packed)) ASDU_C_TS_TA_1;

// KL_HIST - Запрос истории по параметру
typedef struct _ASDU_KL_HIST {
    TASDU_HEAD ASDUhead; // Заголовок type == 0x88  num == 1  sq == 0
    TIEC_IOA IOA; // адрес объекта информации
    short MaxQuan; // Максимальное кол-во точек
    cp56time2a time; // Начальное время
} __attribute__ ((packed)) ASDU_KL_HIST;

#endif // #ifndef IEC104TYPES_H
