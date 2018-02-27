/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from mipslabfunc.c */
void display_image(uint8_t array[]);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
void display_game(uint8_t array[]);
void move(int x, int y, int array[],int arrayLength);
void test(void);
void set_coordinate(int x, int y, uint8_t array[], int setClr, int arraySize);
int get_coordinate(int x, int y, uint8_t arr[], int arraySize);

uint8_t spi_send_recv(uint8_t data);

void clear_game(void);
void clr_game(void);
extern int ship_placementY;
extern int ship_placementX;
void start_pos(void);
void create_projectile(int startX, int startY, int faction);
void move_projectiles(void);
void update_game(uint8_t  arr[]);
void paint_map(void);
void move_map(void);
void update_map(void);
void paint_life(void);

//Enemies
void create_enemy(int x, int y, int enemyChar[], int arrayLength, int enemyStat[]);  //new
void update_enemies(void);
void spawn_enemy(void);



//
extern int gamecount;       //count for spawning enemies NEW
extern int number_of_enemies;
extern int TIE1_alive;
extern int pause;
extern int spawnEnemyCount;



//counters
extern int commandCount;
extern int projectileCount;
extern int createProjectileCount;
extern int mapCount;
extern int buttonCount;
extern int secCount;
extern int createMapCount;
extern int randCount;
extern int moveEnemiesCount;
extern int stopMove;




void run_projectile(void);
void run_control(void);
void run_map(void);
void run_enemies(void);

/* Declare lab-related functions from mipslabfunc.c */
char * itoaconv( int num );
int nextprime( int inval );
void quicksleep(int cyc);
void tick( unsigned int * timep );


/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern uint8_t game[128*4];
extern uint8_t map[144];
extern uint8_t projectiles[128*4];
extern uint8_t font[128*8];
extern uint8_t life[10];
extern int ship[22];
extern int shipChoice[22];
extern uint8_t cloud_1[13];
extern uint8_t cloud_2[7];
extern uint8_t cloud_3[13];
extern uint8_t cloud_4[16];
extern int lives;
extern uint8_t select_ship[58];


extern uint8_t enemies[4*164];
extern int TIE1[32]; //new
// [0] = x coord, [1] = y-coord, [2] = om 1 alive om 0 död, [3] hp
extern int enemy_placement1[4];
extern int enemy_placement2[4];
void check_enemy_placement(void);
void dmg(uint8_t dealer[], uint8_t receiver[], int character[], int characterLength);
void kill_enemy(int x, int y, int enemyChar[], int arrayLength, int enemyStat[]);
void select_menu(void);


//void start_random_counter(void);
int random(int mod);






/* Declare bitmap array containing icon */
/* Declare text buffer for display output */
extern char textbuffer[4][16];

/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string( char *, int );
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);

//Initializing
void set_init(void);
