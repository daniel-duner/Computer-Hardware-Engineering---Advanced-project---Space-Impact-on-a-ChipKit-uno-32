/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */


/* Declare a helper function which is local to this file */
static void num32asc( char * s, int );

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)
int secCount = 0;
int buttonCount=0;
int projectileCount = 0;
int createProjectileCount = 0;
int mapCount = 0;
int createMapCount=0;
int lives = 3;
int randCount = 0;
int spawnEnemyCount = -10;
int moveEnemiesCount = 0;
int points = 0;
int dmgCount =0;
int min = 0;
int sec = 0;
int startClock=0;
int scoreCount=0;
int takeLife = 0;
int gameOver = 1;
int lvl = 3;
int LEDlight =0;


//ALREADY EXISTING FUNCTIONS
/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}
/* tick:
   Add 1 to time in memory, at location pointed to by parameter.
   Time is stored as 4 pairs of 2 NBCD-digits.
   1st pair (most significant byte) counts days.
   2nd pair counts hours.
   3rd pair counts minutes.
   4th pair (least significant byte) counts seconds.
   In most labs, only the 3rd and 4th pairs are used. */
void tick( unsigned int * timep )
{
  /* Get current value, store locally */
  register unsigned int t = * timep;
  t += 1; /* Increment local copy */

  /* If result was not a valid BCD-coded time, adjust now */

  if( (t & 0x0000000f) >= 0x0000000a ) t += 0x00000006;
  if( (t & 0x000000f0) >= 0x00000060 ) t += 0x000000a0;
  /* Seconds are now OK */

  if( (t & 0x00000f00) >= 0x00000a00 ) t += 0x00000600;
  if( (t & 0x0000f000) >= 0x00006000 ) t += 0x0000a000;
  /* Minutes are now OK */

  if( (t & 0x000f0000) >= 0x000a0000 ) t += 0x00060000;
  if( (t & 0x00ff0000) >= 0x00240000 ) t += 0x00dc0000;
  /* Hours are now OK */

  if( (t & 0x0f000000) >= 0x0a000000 ) t += 0x06000000;
  if( (t & 0xf0000000) >= 0xa0000000 ) t = 0;
  /* Days are now OK */

  * timep = t; /* Store new value */
}
/*DISPLAY*/
void display_debug( volatile int * const addr )
{
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;

	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_image(uint8_t array[]) {
	int i, j;

	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0 & 0xF);
		spi_send_recv(0x10 | ((0 >> 4) & 0xF));

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 128; j++)
            spi_send_recv(~array[i*128+j]);
	}
}

void display_update(void) {
    int i, j, k;
    int c;
    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(0x0);
        spi_send_recv(0x10);

        DISPLAY_CHANGE_TO_DATA_MODE;

        for(j = 0; j < 16; j++) {
            c = textbuffer[i][j];
            if(c & 0x80)
                continue;

            for(k = 0; k < 8; k++)
                spi_send_recv(font[c*8 + k]);
        }
    }
}




//INITIALIZING
/*
This will set the peripheral bus clock to the same frequency
as the sysclock. That means 80 MHz, when the microcontroller
is running at 80 MHz. Changed 2017, as recommended by Axel.
*/
void set_init(void){

    SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
    SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
    while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
    OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
    while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
    SYSKEY = 0x0;  /* Lock OSCCON */

    /* Set up output pins */
    AD1PCFG = 0xFFFF;
    ODCE = 0x0;
    TRISECLR = 0xFF;
    PORTE = 0x0;

    /* Output pins for display signals */
    PORTF = 0xFFFF;
    PORTG = (1 << 9);
    ODCF = 0x0;
    ODCG = 0x0;
    TRISFCLR = 0x70;
    TRISGCLR = 0x200;

    /* Set up input pins */
    TRISDSET = (1 << 8);
    TRISFSET = (1 << 1);

    /* Set up SPI as master */
    SPI2CON = 0;
    SPI2BRG = 4;
    /* SPI2STAT bit SPIROV = 0; */
    SPI2STATCLR = 0x40;
    /* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
    /* SPI2CON bit MSTEN = 1; */
    SPI2CONSET = 0x20;
    /* SPI2CON bit ON = 1; */
    SPI2CONSET = 0x8000;
}
//Activates the display
void display_init(void) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    quicksleep(10);
    DISPLAY_ACTIVATE_VDD;
    quicksleep(1000000);

    spi_send_recv(0xAE);
    DISPLAY_ACTIVATE_RESET;
    quicksleep(10);
    DISPLAY_DO_NOT_RESET;
    quicksleep(10);

    spi_send_recv(0x8D);
    spi_send_recv(0x14);

    spi_send_recv(0xD9);
    spi_send_recv(0xF1);

    DISPLAY_ACTIVATE_VBAT;
    quicksleep(10000000);

    spi_send_recv(0xA1);
    spi_send_recv(0xC8);

    spi_send_recv(0xDA);
    spi_send_recv(0x20);

    spi_send_recv(0xAF);
}

//Checks TMR2 and ativates counters for time,
//enemy spawning and also the random functions counter
void user_isr( void ) {
    IFS(0)=0;
    secCount++;
    end_game();
    //seconds
    if (secCount== 10 && end!=0){
        if(spawnEnemyCount >= 0){
            sec++;
            if(sec == 30){
                lvl= 2;
                moveEnemiesCount = 0;
            }
        }
        secCount = 0;
        spawnEnemyCount++;
    }
    //random counter
    if (randCount == 735) {
        randCount = 0;
    }
    //for LED
    if(LEDlight == 17){
        LEDlight = 0;
    }

    if(secCount == 8 || secCount == 16){
        PORTE = PORTE >>1  | 0;
    }

    randCount++;
    return;
}

//Welcome screen
void intro_screen(void){
    while(1){
        display_string(0, "--Space Impact--");
        display_string(1, "    To start");
        display_string(2, "Press btn 3 or 4");
        display_update();
        if(getbtns()& 6){
            break;
        }
    }
}

//in game clock
void game_clock(void){
    if (startClock ==0){
        sec = 0;
        min = 0;
        startClock=1; //initiera klockan
    }
    if(sec == 59){
        min++;
        sec=0;
    }
    if(min==59){
        min = 0;
    }
    int i,j,k;
    k = sec/10;
    i=0;
    j=0;
    for (i = 3*sec-k*30; i < 3+3*sec-k*30; i++) {
        game[65+j] |= numbers[i];       //placera klockan centralt
        j++;
    }
    i=0;
    j=0;
    for (i = 3 * k; i < 3 + 3 * k; i++) {
        game[61+ j] |= numbers[i];
        j++;
    }
    game[59]|= 20;
    k = min/10;
    i=0;
    j=0;
    for (i = 3*min-k*30; i < 3+3*min-k*30; i++) {
        game[55+j] |= numbers[i];
        j++;
    }
    i=0;
    j=0;
    for (i = 3 * k; i < 3 + 3 * k; i++) {
        game[51+ j] |= numbers[i];
        j++;
    }
}

//Score board
void score_board(void){
    if (end == 0) {
            highscore[scoreCount] = points;
            scoreCount++;
        if (scoreCount == 3) {
            scoreCount = 0;
        }
        int i, j, k;

        for (i = 0; i < 2;i++){
            int move;
            for(k = 1;k <3;k++){
                if(highscore[i] < highscore[k]){
                    move = highscore[i];
                    highscore[i] = highscore[k];
                    highscore[k] = move;
                }
            }

        }
            clr_game();
            while (1) {
                //Skriver ut score
                for (i = 47; i < 35 + 47; i++) {
                    game[i] |= score_text[i - 47];
                }
                j=0;

                for (k = 0; k < 3; k++) {
                    //antalet ental och antalet titoal
                    int tens = highscore[k] / 10;
                    int ones = highscore[k] % 10;
                    //antalet highscore (var de skrivs ut)

                        //ritar ut ental, samt ordningssiffra
                        j = 0;
                        for (i = 3 * tens; i < 3 + 3 * tens; i++) {
                            game[183+j+(k * 128)] |= numbers[3+j+(3*k)];
                            game[187+(k * 128)]=32;
                            game[193 + j + (k * 128)] |= numbers[i];
                            j++;
                        }
                        //ritar ut heltal
                        j = 0;
                        for (i = 3 * ones; i < 3 + 3 * ones; i++) {
                            game[198 + j + (k * 128)] |= numbers[i];
                            j++;
                        }


                }

                display_game(game);
                delay(100);
                if (getbtns() & 1) {
                    reset_game();
                    break;
                }
            }


    }

}

// end game if either lives are zero or time ends
void end_game(void){
    if(min == 1 || lives == 0){
        end = 0;
    }
}

//returns a semi-random number depends on counters
//parameter is the amount of random numbers 2 equals 0,1 as returns
int rand(int mod){
    int random = (randCount+createProjectileCount)%mod;
    randCount = 0;
    return random;
}

//Updates the score the in game score counter
void update_score(void){
    int i,j,k;
    k = points/10;
    i=0;
    j=0;
    for (i = 3*points-k*30; i < 3+3*points-k*30; i++) {
        game[120+j] |= numbers[i];
        j++;
    }
    i=0;
    j=0;
    for (i = 3 * k; i < 3 + 3 * k; i++) {
        game[116+ j] |= numbers[i];
        j++;
    }
}

//used to show a black background screen and white text
void display_game(uint8_t array[]) {
    int i, j;

    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(0 & 0xF);
        spi_send_recv(0x10 | ((0 >> 4) & 0xF));

        DISPLAY_CHANGE_TO_DATA_MODE;

        for(j = 0; j < 128; j++)
            spi_send_recv(array[i*128+j]);
    }
}

//clear the game board "tun on all pixels"
void clear_game(){
    int i, j;
    for (i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        spi_send_recv(0x22);
        spi_send_recv(i);

        spi_send_recv(0 & 0xF);
        spi_send_recv(0x10 | ((0 >> 4) & 0xF));

        DISPLAY_CHANGE_TO_DATA_MODE;

        for (j = 0; j < 128; j++) {
            spi_send_recv(0);

        }
    }
}

//clear the game board "set the game array to 0"
void clr_game(){
    int i = 0;
    for (i; i< 1024; i++){
        game[i] = 0;
    }
}

//menu for selecting player ship
void select_menu(void){
    while(1){
        int i;
        for(i = 0; i < 58;i++){
            game[i] = select_ship[i];
        }
        if(buttonCount < 100) {
            buttonCount= 100;
            //move up button 2
            if ((getbtns() & 0x2) == 2) {
                set_coordinate(5,24,game,0,128);
                set_coordinate(6,24,game,0,128);
                set_coordinate(5,25,game,0,128);
                set_coordinate(6,25,game,0,128);
                set_coordinate(5,14,game,1,128);
                set_coordinate(6,14,game,1,128);
                set_coordinate(5,15,game,1,128);
                set_coordinate(6,15,game,1,128);
                menu_ship(30, 14, ship, ship2);
                for (i = 0; i < 22; i++){
                    shipChoice[i] = ship[i];
                }
            }
            //move down button 3
            if ((getbtns() & 0x4)) {
                {
                    set_coordinate(5,14,game,0,128);
                    set_coordinate(6,14,game,0,128);
                    set_coordinate(5,15,game,0,128);
                    set_coordinate(6,15,game,0,128);
                    set_coordinate(5,24,game,1,128);
                    set_coordinate(6,24,game,1,128);
                    set_coordinate(5,25,game,1,128);
                    set_coordinate(6,25,game,1,128);
                    menu_ship(30, 24, ship2, ship);
                    for (i = 0; i < 22; i++){
                        shipChoice[i] = ship2[i];
                    }

                }
            }
        }
        buttonCount++;
        if (buttonCount== 110){
            buttonCount = 0;
        }

        display_game(game);
        if ((getbtns() & 0x1) == 1) {
            startClock = 0;
            spawnEnemyCount = -10;
            break;
        }
    }

}

//moves the ship in the menu section
void menu_ship(int x, int y, int show[], int remove[]){
    int i;
    for (i = 0; i < 11;i++) {
        set_coordinate((ship_placementX+remove[i]),(ship_placementY+remove[i+11]), game,0, 128);
    }
    ship_placementX = x;
    ship_placementY = y;
    for (i = 0; i < 11;i++){
        set_coordinate((x+show[i]),(y+show[i+(11)]),game,1, 128);
    }
}

//paints the map depending on time, clouds or city
void paint_map(void){
    int i,k,r;
    r = rand(4);

    switch (r){
        case 0:
            if (sec <30){
                for (k=0;k<13;k++){
                    map[128+k] = cloud_1[k];
                }
            } else{
                for (k=0;k<15;k++){
                    map[128+k] = stad1[k];
                }
            }

            break;
        case 1:
            if(sec <30){
            for (k=0;k<7;k++){
                map[128+5+k] = cloud_2[k];
            }
            }else{
                for (k=0;k<15;k++){
                    map[128+k] = stad2[k];
                }
            }
            break;
        case 2:
            if(sec < 30) {
                for (k = 0; k < 13; k++) {
                    map[128 + k] = cloud_3[k];
                }
            }else{
                for (k=0;k<15;k++){
                    map[128+k] = stad3[k];
                }
            }
            break;
        case 3:
            if(sec < 30) {
                for (k = 0; k < 16; k++) {
                    map[128 + k] = cloud_4[k];
                }
            }else{
                for (k=0;k<15;k++){
                    map[128+k] = stad4[k];
                }
            }
            break;

    }

}

//makes the map move to the left
void move_map(void){
    int i,k;
    for (i = 0; i < 144; i++) {
        map[i] = map[i + 1];
    }
    map[143] = 0;

}

//puts the map on the game array
void update_map(void){
    int i,k=0;
    for (i = 128*3; i<128*4;i++) {
        game[i] |= map[k];
        k++;
    }
}

//creates enemies on the enmeies array
void create_enemy(int x, int y, int enemyChar[], int arrayLength, int enemyStat[]){
    enemyStat[0] = x;
    enemyStat[1] = y;
    enemyStat[2] = 1;
    enemyStat[3] = 10-(2*lvl);
    int i;
    for (i = 0; i < arrayLength/2;i++){
        set_coordinate(x+enemyChar[i],y+enemyChar[i+arrayLength/2],enemies,1,164);
    }


}

// clear a bitmap, sets it's values to zero
void clr_bitmap(uint8_t arr[], int size){
    int i;
    for (i = 0; i < size*4;i++){
        arr[i] = 0;
    }

}
// clear a bitmap, sets it's values to zero
void clear_bitmap(int arr[], int size){
    int i;
    for (i = 0; i < size*4;i++){
        arr[i] = 0;
    }

}

//makes the enemy move and also update it's position
void move_enemy(int enemyChar[], int arrayLength, int enemyStat[]){
    int i;
    if(sec < 30) {
        for (i = 0; i < arrayLength / 2; i++) {
            set_coordinate(enemyStat[0] + enemyChar[i], enemyStat[1] + enemyChar[i + arrayLength / 2], enemies, 0, 164);
        }
        enemyStat[0] -= 1;
        if (enemyStat[0] == 5) {
            enemyStat[2] = 0;
        }
        if (enemyStat[2] == 1) {
            for (i = 0; i < arrayLength / 2; i++) {
                set_coordinate(enemyStat[0] + enemyChar[i], enemyStat[1] + enemyChar[i + arrayLength / 2], enemies, 1,
                               164);
            }
        }
    }
    if(sec > 30) {
        for (i = 0; i < arrayLength / 2; i++) {
            set_coordinate(enemyStat[0] + enemyChar[i], enemyStat[1] + enemyChar[i + arrayLength / 2], enemies, 0, 164);
        }
        enemyStat[0] -= 1;
        if (enemyStat[0] == 5) {
            enemyStat[2] = 0;
        }
        if (enemyStat[2] == 1) {
            for (i = 0; i < arrayLength / 2; i++) {
                set_coordinate(enemyStat[0] + enemyChar[i], enemyStat[1] + enemyChar[i + arrayLength / 2], enemies, 1,
                               164);
            }
        }
    }


}

//kills of an enemy also makes points increase on a kill and also check end_game
void kill_enemy(int enemyChar[], int arrayLength, int enemyStat[]){
        int i;
        if(enemy_placement1[2] == 0){
            for (i = 0; i < arrayLength / 2; i++) {
                set_coordinate(enemyStat[0] + enemyChar[i], enemyStat[1] + enemyChar[i + arrayLength / 2], enemies, 0, 164);
            }
        /*for (i = 164*2; i < 164*3; i++ ) {
            enemies[i] = 0;
        }*/
        for (i = 0;i < 3;i++){
            enemyStat[i] = 0;
        }
            enemyStat[3] = 6;
            points++;
            end_game();
        }

        if(enemy_placement2[2] == 0) {
            for (i = 0; i < arrayLength / 2; i++) {
                set_coordinate(enemyStat[0] + enemyChar[i], enemyStat[1] + enemyChar[i + arrayLength / 2], enemies, 0, 164);
            }
            /*for (i = 164; i < 164 * 2; i++) {
                enemies[i] = 0;
            }*/

            for (i = 0;i < 3;i++){
                enemyStat[i] = 0;
        }
            enemyStat[3] = 6;
            points++;
            end_game();
    }

}

//handles the dmg checking on the ship and on the enemies
void dmg(uint8_t dealer[], int receiver[], int character[], int characterLength) {

    //ENEMY DMG
    int i, j;
    for (i = 0; i < 5; i++) {
        if (get_coordinate(receiver[0] - 18, receiver[1] + i, dealer, 128) == 1) {
            if (receiver[3] != 0) {
                receiver[3]--;
            }
            // tar in x och y värde från receiver kollar koordinaterna för dealer sätter den till 0
            set_coordinate(receiver[0] - 17, receiver[1] + i, dealer, 0, 128);
            set_coordinate(receiver[0] - 18, receiver[1] + i, dealer, 0, 128);
            set_coordinate(receiver[0] - 19, receiver[1] + i, dealer, 0, 128);
        }
    }

    if (receiver[3] == 0) {
        dmgCount = 1;
        receiver[2] = 0;
        kill_enemy(character, characterLength, receiver);
        update_score();
        dmgCount = 0;
    }

    //SHIP DMG
    for (j = 0; j < 11;j++){
         for (i = 0; i < 5; i++) {
            if (ship_placementX+18 == receiver[0] && ship_placementY+1 == receiver[1] + i && takeLife == 0) {
                lives--;
                if(lives == 0){
                    gameOver = 0;
                }
                end_game();
                takeLife = 100;
            }
         }
    }
    if(takeLife > 0){
        takeLife--;
    }


}

//puts the enemies on the game array
void update_enemies(void){
    int i,k;
    for (i = 0; i <4;i++) {
        for (k = 0 + 128 * i; k < 128 + (128 * i); k++) {
            game[k] |= enemies[k + 18 + (36 * i)];
        }
    }

}

//paints the player ship lives top left-corner
void paint_life(void){
    int i;
    for (i = 0; i < 1+(3*lives);i++){
        game[i] |= life[i];
    }
}

//checks a coordinate on a bitmap and returns 1 if the light is on or a zero if it is off (occupied or not occupied)
int get_coordinate(int x, int y, uint8_t arr[], int arraySize) {
    int coordinate;
    short part = 0;
    if (y > 0) {
        part = y / 8;
        y = y -(part*8);
    }
    coordinate = part * arraySize + x;
    coordinate = arr[coordinate] >> y && 0x1;
    return coordinate;
}

//moves the ship and changes it's position values
void move(int x, int y, int array[], int arrayLength){
    int i;
    for (i = 0; i < arrayLength/2;i++) {
        set_coordinate((ship_placementX+array[i]),(ship_placementY+array[i+arrayLength/2]), game,0, 128);
    }
    ship_placementX = x;
    ship_placementY = y;
    for (i = 0; i < arrayLength/2;i++){
        set_coordinate((x+array[i]),(y+array[i+(arrayLength/2)]),game,1, 128);
    }
}

//Puts the ship in it's start position
void start_pos(void){
    move(4,16,shipChoice,22);
}

//the function that lights or turns a pixel off on the display
void set_coordinate(int x, int y, uint8_t array[], int setClr, int arraySize){
    short part = 0;
    if (y > 0) {
        part = y/8;
    }
    if(setClr == 1){
        array[part * arraySize + x] |= 1 << (y - part * 8);
    }
    if(setClr == 0){
        array[part * arraySize + x] &= ~(1 << (y - part * 8));
    }
}

//makes the projectiles move
void move_projectiles(void){
    int i,j;
    for (j=0; j < 4;j++) {
        for (i = 127+(j*128); i > 128*j; i--) {
            projectiles[i] = projectiles[i - 1];
            projectiles[i-1] = 0;
        }
    }

}

//updates the game with a given array
void update_game(uint8_t arr[]){
    int i;
    for (i = 0; i<128*4;i++) {
        game[i] |= arr[i];
    }

}

//create projectiles, that are put on the projectile bitmap, depending on the coordinat of the ship
void create_projectile(int startX, int startY, int faction){
        set_coordinate(startX, startY, projectiles, 1, 128);
        set_coordinate(startX+1, startY, projectiles, 1,128);
        PORTE = PORTE <<1  | 1;

}

/*RUN*/
//turn on and handle counters that make the map move and that create objects on the map
void run_map(void){
   if (mapCount == 15){
        move_map();
        mapCount=0;
    }
    mapCount++;
    if(createMapCount == 300){
        paint_map();
        createMapCount=0;
    }
    createMapCount++;

}

//updates the projectiles, depending on a cooldown and a button push
void run_projectile(void){
    if(projectileCount == 5){
        move_projectiles();
        projectileCount=0;
    }
    projectileCount++;
    if(createProjectileCount < 1 && spawnEnemyCount >= -8){
        createProjectileCount = 1;
        if ((getbtns() & 0x1) == 1) {
            create_projectile(ship_placementX+5,ship_placementY+1,1);
        }
    }
    if (createProjectileCount >= 1){
        createProjectileCount++;
    }

    if (createProjectileCount == 100){
        createProjectileCount = 0;
    }
}

//check the buttons for controlling the ship
void run_Control(void){
    if(buttonCount < 100) {
        buttonCount= 100;
        //move up button 2
        if ((getbtns() & 0x2) == 2 && ship_placementY > 4) {
            move(ship_placementX, ship_placementY - 1, ship, 22);
        }
        //move down button 3
        if ((getbtns() & 0x4) == 4 && ship_placementY < 28) {
            {
                move(ship_placementX, ship_placementY + 1, ship, 22);
            }
        }
    }
    buttonCount++;
    if (buttonCount== 110){
        buttonCount = 0;
    }
}

//make enemies spawn
void run_enemies(void){
    if (spawnEnemyCount == 2){
        if(sec < 30){
        if (enemy_placement1[2]==0) {
            create_enemy(150, 18, TIE1, 32, enemy_placement1);
        }
        if (enemy_placement2[2]==0) {
            create_enemy(150, 8, TIE1, 32, enemy_placement2);

        }
        }
        if(sec > 30) {
            if (enemy_placement1[2] == 0) {
                create_enemy(150, 18, missile, 42, enemy_placement1);
            }
            if (enemy_placement2[2] == 0) {
                create_enemy(150, 8, missile, 42, enemy_placement2);

            }
        }
        spawnEnemyCount = 0;
    }
    if (moveEnemiesCount == lvl) {
        {
            if (enemy_placement1[2] == 1) {
                if(sec < 30){
                    move_enemy(TIE1, 32, enemy_placement1);
                }
                if(sec > 30){
                    move_enemy(missile, 42, enemy_placement1);
                }
            }
            if (enemy_placement2[2] == 1) {
                if(sec < 30){
                    move_enemy(TIE1, 32, enemy_placement2);
                }
                if(sec > 30){
                    move_enemy(missile, 42, enemy_placement2);
                }
            }
            if(sec < 30) {
                dmg(projectiles, enemy_placement1, TIE1, 32);
                dmg(projectiles, enemy_placement2, TIE1, 32);
            }
            if(sec > 30) {
                dmg(projectiles, enemy_placement1, missile, 42);
                dmg(projectiles, enemy_placement2, missile, 42);
            }
            moveEnemiesCount = 0;
        }
    }
    moveEnemiesCount++;
}

// reset game
void reset_game(void){
    clr_bitmap(map,36);
    clr_bitmap(enemies,164);
    clr_bitmap(projectiles,128);
    clear_bitmap(enemy_placement1,1);
    clear_bitmap(enemy_placement2,1);
    clr_game();
    secCount = 0;
    buttonCount=0;
    projectileCount = 0;
    createProjectileCount = 0;
    mapCount = 0;
    createMapCount=0;
    lives = 3;
    randCount = 0;
    moveEnemiesCount = 0;
    dmgCount =0;
    min = 0;
    sec = 0;
    ship_placementY=0;
    ship_placementX=0;
    startMapCount= 0;
    points = 0;
    end = 1;
    enemy_placement1[3] = 1;
    enemy_placement2[3] = 1;
    lvl = 3;
}

//end screen
void game_over(void){
    //if (gameOver >= 0) {
    int i,k;

        while (k<300) {
            display_string(0, "");
            display_string(1, "    GAME OVER");
            display_string(2, "    ");
            display_update();
            i++;
            if(i == 10){
                k++;
                i = 0;
            }
        }
    //}
}




/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n )
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}

/*
 * itoa
 *
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 *
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 *
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 *
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 *
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 *
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 *
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 *
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";

  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}
