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
secCount = 0;
buttonCount=0;
projectileCount = 0;
createProjectileCount = 0;  
mapCount = 0;
createMapCount=0;
lives = 3;
randCount = 0;
spawnEnemyCount = 0;
moveEnemiesCount = 0;
stopMove=0;
points= 0;
void update_score(void){
        int i;
        if(points < 10) {
            for (i = points * 3; i < 3 + 3 * points; i++) {
                game[i + 120] |= numbers[i];
            }
        for(i = 0; i < 3;i++){
             game[i+116] |= numbers[i];
            }
        }


        


        if(points > 10 && points < 20) {
        for (i = points * 3; i < 3 + 3 * points; i++) {
            game[i + 120] |= numbers[i];
        }
        for(i = 3; i < 6;i++){
            game[i+116] |= numbers[i];
        }
    }
}


/*
 * returns a semi-random number depends on counters
 * parameter is the amount of random numbmbers 2 equals 0,1 as returns
 */
int rand(int mod){
    int random = (randCount+createProjectileCount)%mod;
    randCount = 0;
    return random;
}


/* TIMER*/
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






void user_isr( void )
{
    IFS(0)=0;
    secCount++;
    if (secCount= 10){
        secCount =0;
    }
    if (randCount == 10) {
        randCount = 0;
    }
    randCount++;
    return;
}
/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/

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
//initializes everything that was in main;
void set_init(void){
    /*
   This will set the peripheral bus clock to the same frequency
   as the sysclock. That means 80 MHz, when the microcontroller
   is running at 80 MHz. Changed 2017, as recommended by Axel.
 */
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







//light up the game "board" on the screen
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

void select_menu(void){
    int i;
    if(buttonCount < 100) {
        buttonCount= 100;
        //move up button 2
        move(30,14,ship,22);
        move(30,24,ship2,22);
        if ((getbtns() & 0x2) == 2) {
            set_coordinate(5,24,game,0,128);
            set_coordinate(6,24,game,0,128);
            set_coordinate(5,25,game,0,128);
            set_coordinate(6,25,game,0,128);
            set_coordinate(5,14,game,1,128);
            set_coordinate(6,14,game,1,128);
            set_coordinate(5,15,game,1,128);
            set_coordinate(6,15,game,1,128);
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


}

//skapar kartan i map arrayen, lägger till ett max 16 punkter längst bort
void paint_map(void){
    int i,k,r;
    r = rand(4);

    switch (r){
        case 0:
            for (k=0;k<13;k++){
                map[128+k] = cloud_1[k];
            }
            break;
        case 1:
            for (k=0;k<7;k++){
                map[128+5+k] = cloud_2[k];
            }
            break;
        case 2:
            for (k=0;k<13;k++){
                map[128+k] = cloud_3[k];
            }
            break;
        case 3:
            for (k=0;k<16;k++){
                map[128+k] = cloud_4[k];
            }
            break;

    }

}
//gör så att map arrayen rör sig åt vänster
void move_map(void){
    int i,k;
    for (i = 0; i < 144; i++) {
        map[i] = map[i + 1];
    }
    map[143] = 0;
    
}
void update_map(void){
    int i,k=0;
    for (i = 128*3; i<128*4;i++) {
        game[i] |= map[k];
        k++;
    }
}



//Enemies
//Ritar ut fiender på en array
void create_enemy(int x, int y, int enemyChar[], int arrayLength, int enemyStat[]){
    enemyStat[0] = x;
    enemyStat[1] = y;
    enemyStat[2] = 1;
    enemyStat[3] = 4;
    int i;
    for (i = 0; i < arrayLength/2;i++){
        set_coordinate(x+enemyChar[i],y+enemyChar[i+arrayLength/2],enemies,1,164);
    }


}
void move_enemy(int enemyChar[], int arrayLength, int enemyStat[]){
    int i;
    for (i = 0; i < arrayLength / 2; i++) {
        set_coordinate(enemyStat[0] + enemyChar[i], enemyStat[1] + enemyChar[i + arrayLength / 2], enemies, 0, 164);
    }
    enemyStat[0] -=1;
    if (enemyStat[0] == 5){
        enemyStat[2] = 0;
    }
    if(enemyStat[2] == 1) {
        for (i = 0; i < arrayLength / 2; i++) {
            set_coordinate(enemyStat[0] + enemyChar[i], enemyStat[1] + enemyChar[i + arrayLength / 2], enemies, 1, 164);
        }
    }

}

void kill_enemy(int enemyChar[], int arrayLength, int enemyStat[]){
        int i;
        stopMove=1;
        if(enemy_placement1[2] == 0)
        for (i = 164*2; i < 164*3; i++ ) {
            enemies[i] = 0;
        }
        if(enemy_placement2[2] == 0)
        for (i = 164; i < 164*2; i++ ) {
            enemies[i] = 0;
        }
        stopMove = 0;
        for (i = 0;i < 3;i++){
        enemyStat[i] = 0;
    }
   // enemyStat[2]= 0;


}

void check_enemy_placement(void){
       if (enemy_placement1[2] == 1){ 
           enemy_placement1[0]--;     
       }                              
       if (enemy_placement1[0] == 5 ){
           enemy_placement1[2] = 0;   
       }
       if (enemy_placement2[2] == 1){
          enemy_placement2[0]--;
      }
      if (enemy_placement2[0] == 5 ){
          enemy_placement2[2] = 0;
      }


}
//rör på fienderna ANVÄNDS INTE ATM
/*void move_enemies(){
    int i,k;

    for (i=0; i < 4;i++){
        for(k=0+164*i;k<164+164*i;k++){
            enemies[k] = enemies[k+1];
        }
        enemies[163+(i*164)]=0;
        //enemies[0+163*i] = 0;
    }
    check_enemy_placement();

}*/

//ritar in enemies på game
void update_enemies(void){
    int i,k;
    for (i = 0; i <4;i++) {
        for (k = 0 + 128 * i; k < 128 + (128 * i); k++) {
            game[k] |= enemies[k + 18 + (36 * i)];
        }
    }

 }

//lägger in map i game, så att


void dmg(uint8_t dealer[], int receiver[], int character[], int characterLength){
    if(get_coordinate(receiver[0],receiver[1],dealer,128) == 1){
        receiver[3]--;
        // tar in x och y värde från receiver kollar koordinaterna för dealer sätter den till 0
        set_coordinate(receiver[0],receiver[1],dealer,0,128);
        set_coordinate(receiver[0]-1,receiver[1],dealer,0,128);
        set_coordinate(receiver[0]+1,receiver[1],dealer,0,128);

    }
    if(receiver[3] == 0){
        kill_enemy(character,characterLength,receiver);
    }
}


void paint_life(void){
    int i;
 /* switch(lives){
        case 1:
            life[2] = 6;
            life[3] = 6;
            for(i=4;i < 10;i++){
               life[i] = 0;
            }
            break;
        case 2:
            for (i =2; i < 7; i=+3) {
                life[i] = 6;
                life[i+1] = 6;
            }
            for(i=6;i < 10;i++){
                life[i] = 0;
            }
            break;
        case 3:
            for (i = 2; i < 11; i=+3) {
                life[i] = 6;
                life[i+1] = 6;
    }
            break;

    }          */
    for (i = 0; i < 10;i++){
        game[i] |= life[i];
    }        
}


//kollar en given coordinat, på en give array ich returnerar värdet 1 eller 0 beroende på om pixeln är av eller på
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
// ritar ut skeppet, utgår ifrån en skeppets tidigare koordinat
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
// sätter skeppets start position
void start_pos(void){
    move(4,16,shipChoice,22);
}
//tänder/släcker pixel (koordinatsystem)
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
//rör på prjektilerna från skeppet, åt höger
void move_projectiles(void){
    int i,j;
    for (j=0; j < 4;j++) {
        for (i = 127+(j*128); i > 128*j; i--) {
            projectiles[i] = projectiles[i - 1];
            projectiles[i-1] = 0;
        }
    }

}
//uppdaterar spelet med en given uint8_t array som är 128*4, såsom projectiles
void update_game(uint8_t arr[]){
    int i;
    for (i = 0; i<128*4;i++) {
        game[i] |= arr[i];
    }

}
//skapar projektiler från skeppets främre punkt
void create_projectile(int startX, int startY, int faction){
        set_coordinate(startX, startY, projectiles, 1, 128);
        set_coordinate(startX+1, startY, projectiles, 1,128);
}

/*RUN*/
//uppdaterar kartan, när mapCount är 15 rör sig kartan, skapar
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

//uppdaterar projektil karta efter knapptryck, samt gör att den rör sig
void run_projectile(void){
    if(projectileCount == 10){
        move_projectiles();
        projectileCount=0;
    }
    projectileCount++;
    if(createProjectileCount < 1){

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

//kollar styrknappar
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

//kollar spawn enemies
void run_enemies(void){
    if (spawnEnemyCount == 500){
        if (enemy_placement1[2]==0) {
            create_enemy(150, 18, TIE1, 32, enemy_placement1);
        }
        if (enemy_placement2[2]==0) {
            create_enemy(150, 8, TIE1, 32, enemy_placement2);

        }
        spawnEnemyCount = 0;
    }
    if (moveEnemiesCount == 5){
        //if(stopMove == 0){
           if(enemy_placement1[2] == 1){
               move_enemy(TIE1, 32, enemy_placement1);
           }
            if(enemy_placement2[2] == 1){
                move_enemy(TIE1, 32, enemy_placement2);
            }
       // }
        dmg(projectiles,enemy_placement1,TIE1,32);
        dmg(projectiles,enemy_placement2,TIE1,32);
        moveEnemiesCount=0;
    }
    moveEnemiesCount++;
    spawnEnemyCount++;
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



