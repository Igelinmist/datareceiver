cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

set (PROJECT datareceiver)
project (${PROJECT})

set (HEADERS
        ../include/iec104act.h
        ../include/iec104types.h
        ../include/support.h
        ../include/assistent.h
        ../include/kpi.h)

set (SOURCES
        ../source/iec104act.cpp
        ../source/support.cpp
        ../source/assistent.cpp
        ../source/kpi.cpp
        main.cpp)

include_directories(../include ../source /usr/local/include)

add_executable (${PROJECT} ${HEADERS} ${SOURCES})
