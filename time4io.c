#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"
//
// Created by molin on 2018-02-09.
//
int getsw(void){
    int swdata= ((PORTD & 0xf00)>> 8);
    return  swdata;
}
int getbtns (void){
    int btndata= ((PORTD & 0xe0)>> 5);
    return btndata;
}
