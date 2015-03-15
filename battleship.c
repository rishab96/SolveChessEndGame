#include "system.h"
#include "timer.h"
#include "gpio.h"
#include "fb.h"
#include "keyboard.h"
#include "buffer.h"
#include "gfx.h"
#include "font.h"

#define FIELD_COLOR 0xFFFFFFFF
#define NUM_ROWS 12
#define NUM_COLS 12
#define TILE_SIZE 50
#define START_POS 50
#define SHIP_SPACE 10
#define FLASH_TIME 500000
#define MAX_SCORE 14
#define CLOCK 21
#define DATA 12

//holds the grid of ships
int field[144];
//holds the locations fired
int fired[144];

void drawField(int nrow, int ncol, int width, int height, int x, int y);
void add_battleship(int start_row, int start_col, int end_row, int end_col);
void fire(int row, int col);
void draw_dimensions();
void fieldInit();
void add_ships();
void ask_for_ship(int size);
void play_game();
void ask_to_fire();

void notmain() {
	fieldInit();
	//play_game();
  unsigned int value = 1;
	gpio_set_output(DATA);
	gpio_set_output(CLOCK);
	gpio_pin_write(DATA, 1);
	gpio_pin_write(CLOCK, 1);
	while(1){
		if(keyboard_has_char()){
			unsigned int scancode = remove();
			int i;
			//Write start bit
			/*gpio_pin_write(12, 0);
			timer_wait_for(FLASH_TIME);
			gpio_pin_write(21, 0);
			timer_wait_for(FLASH_TIME);
			gpio_pin_write(12, 1);*/
			for(i = 0; i < 8; ++i){
				gpio_pin_write(CLOCK, 0);
				int value = scancode & 1;
				timer_wait_for(FLASH_TIME);
				gpio_pin_write(DATA, value);
				scancode = scancode>>1;
				timer_wait_for(FLASH_TIME);
				gpio_pin_write(CLOCK, 1);
				timer_wait_for(FLASH_TIME);
			}
/*			//Write parity (garbage) & end bit
			int j;
			for(j = 0; j < 2; ++j){
				gpio_pin_write(12, 0);
				gpio_pin_write(21, 0);
				gpio_pin_write(12, 1);
			}*/
		}

	}
	/*unsigned int white = gfx_compute_color(128, 128, 128);
	int y = 10;
	y += 2*font_height();
	play_game();*/
}

void play_game() {
    //draw grid and ask fro ships
	drawField(NUM_COLS, NUM_ROWS, TILE_SIZE, TILE_SIZE, START_POS, START_POS);
    add_ships();
    int over = 0;
    int previous_score = 0;
    //play the game until max score reached
    while(!over) {
        //fire
        ask_to_fire();
        int current_score = get_score();
        //check if hit, and alert user on result
        if(current_score == MAX_SCORE) {
            gfx_draw_string(FIELD_COLOR, 700, 500, "you lost!   ");
            over = 1;
        }
        else if(current_score > previous_score) {
            gfx_draw_string(FIELD_COLOR, 700, 500, "they hit!   ");
        }
        else {
            gfx_draw_string(FIELD_COLOR, 700, 500, "they missed!  ");
        }
        //update score
        previous_score = current_score;
  }
}

//methods asks the user where to fire
void ask_to_fire() {
       gfx_draw_string(FIELD_COLOR, 700, 0, "Where to fire?")    ; 
        int corners[4];
        int i=0;
        for(i = 0; i < 2; ++i){
            while(1){
                char c;
                if(keyboard_has_char()){
                    c = keyboard_read_char();
                    gfx_draw_letter(FIELD_COLOR, 700, (i+1)*font_height(), c);
                    if(c >= 97){
                        corners[i] = c - 'a' + 10;
                    }
                    else{
                        corners[i] = c - '0';
                    }
                    if(corners[i] >= 0 && corners[i] <= 0xC)
                        break;
                }
            }
      } 
    //fire at this location
      fire(corners[0], corners[1]);
    //set the fired location to true
    fired[corners[0] * NUM_COLS + corners[1]] = 1;
    //clear
    for(i = 0; i < 4; ++i){
          gfx_draw_letter(FIELD_COLOR, 700, (i+1)*font_height(), ' ');
    }
    
}
void fieldInit(){
  gpio_init();
  led_init();
  timer_init();
  fb_init();
  keyboard_init();
  gfx_init();
  system_enable_interrupts();
	gfx_set_double_buffered(0);

    //fill field and fires with 0s
    int i;
    for(i=0; i < 143; i++) {
        field[i] = 0;
        fired[i] = 0;
    }
}

void drawField(int nrow, int ncol, int width, int height, int x, int y){
	//add the dimensions to screen
    draw_dimensions();
    int i;
	for(i = 0; i <= ncol; ++i){
		gfx_draw_line(0xFFFFFFFF, x+i*width, x+i*width, y, y+height*nrow, 0);
	}
	int j;
	for(j = 0; j <= nrow; ++j){
		gfx_draw_line(0xFFFFFFFF, x,  x+width*ncol, y+j*height, y+j*height, 0);
	}
}

void ask_for_ship(int size) {
        int corners[4];
        int i=0;
        for(i = 0; i < 2; ++i){
            while(1){
                char c;
                if(keyboard_has_char()){
                    c = keyboard_read_char();
                    gfx_draw_letter(FIELD_COLOR, 700, (i+1)*font_height(), c);
                    if(c >= 97){
                        corners[i] = c - 'a' + 10;
                    }
                    else{
                        corners[i] = c - '0';
                    }
                    if(corners[i] >= 0 && corners[i] <= 0xC)
                        break;
                }
            }
        }gfx_draw_string(FIELD_COLOR, 700, 0, "vert(v) or hor(h)?");
        int horizontal = 0;
        int vertical = 0;
        while(1) {
            char c1;
            if(keyboard_has_char()) {
                c1 = keyboard_read_char();
			    if(c1 == 'h') {
                    horizontal = 1;
                }
                else if(c1 == 'v') { 
                    vertical = 1;
                }
                if(horizontal ==1 || vertical ==1) {
                    break;
                }
            }    
        }
        if(horizontal) {
            add_battleship(corners[0], corners[1], corners[0], corners[1]+size);
    }
    else {
        add_battleship(corners[0], corners[1], corners[0]+size, corners[1]);
    }  
    //clear
    for(i = 0; i < 4; ++i){
            gfx_draw_letter(FIELD_COLOR, 700, (i+1)*font_height(), ' ');
    }
        
}
//asks the user to put in the 4 starting ships
void add_ships() {
        int ships_to_place[4] = {5,4,3,2};
        //ask for 1st ship
		gfx_draw_string(FIELD_COLOR, 700, 0, "Where to put Ship of size 5?");
            //wait for valid input
        ask_for_ship(ships_to_place[0]);
        gfx_draw_string(FIELD_COLOR, 700, 0, "Where to put Ship of size 4?");
        ask_for_ship(ships_to_place[1]);
        gfx_draw_string(FIELD_COLOR, 700, 0, "Where to put Ship of size 3?")    ;
        ask_for_ship(ships_to_place[2]);
        gfx_draw_string(FIELD_COLOR, 700, 0, "Where to put Ship of size 2?")        ;
        ask_for_ship(ships_to_place[3]);
}

//adds a battleship to the field
void add_battleship(int start_row, int start_col, int end_row, int end_col) {
    volatile int start_x = START_POS + (start_col * TILE_SIZE) + SHIP_SPACE;
    volatile int start_y = START_POS + (start_row *TILE_SIZE) + SHIP_SPACE;
    volatile int end_x = 0;
    volatile int end_y = 0; 
    //determine if it is placed horizontally or vertically
    if(start_row == end_row) {
        end_x = start_x + (end_col-start_col) * TILE_SIZE - 2*SHIP_SPACE;
        end_y = start_y + TILE_SIZE - 2*SHIP_SPACE;
        int pos;
        for(pos = start_col; pos <= end_col; pos++) {
            field[start_row*NUM_COLS + pos] = 1;
        } 
    }
    else {
        end_x = start_x + TILE_SIZE - 2*SHIP_SPACE;
        end_y = start_y + (end_row-start_row) * TILE_SIZE - 2*SHIP_SPACE;
        int pos;
        for(pos= start_row; pos <= end_row; pos++) {
            field[pos*NUM_COLS + start_col] = 1;
        }
    }
    
    int width = end_x - start_x;
    int height = end_y - start_y;
    gfx_draw_rect(FIELD_COLOR, start_x, start_y, width, height);
}

void fire(int row, int col) {
    //draw a red X on the square
    volatile int start_x = START_POS + (col * TILE_SIZE);
    volatile int start_y = START_POS + (row *TILE_SIZE);
    int red = gfx_compute_color(255,0,0);

    //draw first half
    volatile int end_x = start_x + TILE_SIZE;
    volatile int end_y = start_y + TILE_SIZE;
    
    gfx_draw_line(red, start_x, end_x, start_y, end_y, 0);

    volatile int new_start_y = start_y + TILE_SIZE;
    volatile int new_end_y = end_y - TILE_SIZE;
    gfx_draw_line(red, start_x, end_x, new_start_y, new_end_y, 0);
}

//gets your score
int get_score() {
    int i;
    int score = 0;    
    for(i=0; i < 144; i++) {
        int val = field[i];
        int fire = fired[i];
        if(val && fire) {
            score++;
            }
        }
    return score;
}
void draw_dimensions() {
    //draw left corner
    gfx_draw_letter(FIELD_COLOR, START_POS/2, START_POS, '1');
    gfx_draw_letter(FIELD_COLOR, 3*START_POS/2, START_POS/2, '1');
    
    //draw other corners
    gfx_draw_letter(FIELD_COLOR, START_POS/2 + TILE_SIZE *NUM_ROWS, START_POS/2, 'C');
    gfx_draw_letter(FIELD_COLOR, START_POS/2,  START_POS/2 + TILE_SIZE *NUM_COLS, 'C');
    gfx_draw_letter(FIELD_COLOR, 3*START_POS/2 + TILE_SIZE *NUM_ROWS, 3*START_POS/2, '1');
    gfx_draw_letter(FIELD_COLOR, 3*START_POS/2,  3*START_POS/2 + TILE_SIZE *NUM_COLS, '1');
    gfx_draw_letter(FIELD_COLOR, START_POS/2 + TILE_SIZE*NUM_ROWS,  3*START_POS/2 + TILE_SIZE *    NUM_COLS, 'C');
    gfx_draw_letter(FIELD_COLOR, 3*START_POS/2 + TILE_SIZE*NUM_ROWS,  START_POS/2 + TILE_SIZE * NUM_COLS, 'C');
}
