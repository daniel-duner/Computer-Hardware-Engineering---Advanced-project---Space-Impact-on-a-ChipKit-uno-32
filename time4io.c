#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void){
    int sw= ((PORTD & 0xf00)>> 8);
    return  sw;
}
int getbtns (void){
    int btn= ((PORTD & 0xe0)>> 5);
    return btn;
}
