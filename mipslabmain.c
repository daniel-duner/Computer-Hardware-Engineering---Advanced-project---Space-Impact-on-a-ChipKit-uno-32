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
//int mytime = 0x5957;
int startMapCount= 0;
int end = 1;

int main(void) {

    while(1) {
        set_init();
        display_init();
        labinit(); /* Do any lab-specific initialization */
        intro_screen();
        clear_game();
        select_menu();

        run_map();

        clr_game();
        update_map();
        start_pos();
        display_image(game);
        update_game(projectiles);
        while (startMapCount == 900) {
            run_map();
            startMapCount++;
            display_game(game);
        }
        update_score();
        while (end!=0) {
            run_map();
            run_projectile();
            run_enemies();
            run_Control();

            clr_game();
            update_enemies();
            paint_life();
            update_score();
            game_clock();
            update_map();
            move(ship_placementX, ship_placementY, shipChoice, 22);
            update_game(projectiles);
            display_image(game);

        }
        game_over();
        score_board();


    }
	return 0;
}


