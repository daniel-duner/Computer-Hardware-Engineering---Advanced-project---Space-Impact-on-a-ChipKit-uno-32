/* mipslabmain.c

   This file written 2015 by Axel Isaksson,
   modified 2015, 2017 by F Lundevall

   Latest update 2017-04-21 by F Lundevall
   
   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
/*int shipY[] = {0,0,0,0,1,1,1,0,0,0,0};
int shipX[] = {0,1,3,4,2,3,5,0,1,3,4};
int ship_placementY=16;
int ship_placementX=4;
*/
int mytime = 0x5957;
int count;
int coordinate = 0;
void move_ship(int x, int y){
}

void set_coordinate(int x, int y){
    short offset = 0;
    if (y > 0) { offset = y / 8; }
    game[offset * 128 + x] |= 1 << (y - offset * 8);
};
void user_isr( void )
{
    count++;
    IFS(0)=0;
    if (count==10) {
        coordinate += 5;
        display_image(game);
        count=0;
    }
    if(coordinate == 15){
        clear_game();
        coordinate = 0;
    }
    return;/**/
}

int main(void) {
       // display_string(0, "--Space Impact");
        //display_update();

            set_init();
            display_init();
            int i;
            int a[7] = {20,20,8,28,20,28,8};
            for (i = 0; i < 7;i++){
                game[i+256]=a[i];
            }
            //display_string(0, "KTH/ICT lab");
            //display_string(1, "in Computer");
            //display_string(2, "Engineering");
            //display_string(3, "Welcome!");
            //display_update();
            //display_game(0, game);
            labinit(); /* Do any lab-specific initialization */
    while(1) {


       }

	return 0;
}
