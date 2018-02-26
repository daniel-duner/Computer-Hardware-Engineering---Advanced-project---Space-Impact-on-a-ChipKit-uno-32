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



int main(void) {
            set_init();
            display_init();
            labinit(); /* Do any lab-specific initialization */
   while(1){
        display_string(0, "--Space Impact--");
        display_string(1, "    To start");
        display_string(2, "Press any button");
        display_update();
        if(getbtns()){
            break;
        }
    }

    run_map();

    clr_game();
    update_map();
    start_pos();
    update_game(projectiles);
    display_image(game);
    delay(100);

    while(1) {
        run_map();
        run_projectile();
        run_Control();
        run_enemies();

        clr_game();
        update_enemies();
        paint_life();
        update_map();
        move(ship_placementX,ship_placementY,ship,22);
        update_game(projectiles);
        display_image(game);
       }

	return 0;
}
