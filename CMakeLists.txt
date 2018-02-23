cmake_minimum_required(VERSION 3.8)
project(Projekt)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        mipslab.h
        mipslabdata.c
        mipslabfunc.c
        mipslabmain.c
        mipslabwork.c
        stubs.c
        time4io.c)

add_executable(Projekt ${SOURCE_FILES})