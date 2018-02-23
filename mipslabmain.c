/* mipslabmain.c

   This file written 2015 by Axel Isaksson,
   modified 2015, 2017 by F Lundevall

   Latest update 2017-04-21 by F Lundevall
   
   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int main(void) {
     set_init();
	
	display_init();
	display_string(0, "KTH/ICT lab");
	display_string(1, "in Computer");
	display_string(2, "Engineering");
	display_string(3, "Welcome!");
	display_update();
	
	labinit(); /* Do any lab-specific initialization */


	return 0;
}
