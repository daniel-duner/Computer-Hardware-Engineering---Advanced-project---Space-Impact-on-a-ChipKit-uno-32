/* mipslabmain.c

   This file written 2015 by Axel Isaksson,
   modified 2015, 2017 by F Lundevall

   Latest update 2017-04-21 by F Lundevall
   
   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
//x värden fram till hälften 11 värden, resten y värden alltså {x1,x2,...y1,y2}
int ship_placementY=0;
int ship_placementX=0;
int mytime = 0x5957;
int count = 0;


void user_isr( void )
{
    IFS(0)=0;
    /*
    count++;
    if(count == 5){
        move_ship(ship_placementX+1,ship_placementY+1,1);
        display_image(game);
        delay (100);
        move_ship(ship_placementX,ship_placementY+1,0);
        display_image(game);
    }
    if (count==10) {
        ate += 5;
        int i;
        move_ship(ship_placementX-1,ship_placementY-1,1);
        display_image(game);
        delay (100);
        move_ship(ship_placementX-1,ship_placementY-1,0);
        display_image(game);
        count=0;
    }
    if(count = 19){
        clr_game();
        display_image(game);
    }/**/
    return;
}

int main(void) {
            labinit();
            clear_game();
            // display_string(0, "--Space Impact");
            //display_update();
            // move_ship(64,12,1);
            set_init();
            display_init();
            //display_string(0, "KTH/ICT lab");
            //display_string(1, "in Computer");
            //display_string(2, "Engineering");
            //display_string(3, "Welcome!");
            //display_update();
            //display_game(0, game);
             /* Do any lab-specific initialization */
    while(1) {

       }

	return 0;
}
