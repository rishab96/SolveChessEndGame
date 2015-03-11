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

void drawField(int nrow, int ncol, int width, int height, int x, int y);
void add_battleship(int start_row, int start_col, int end_row, int end_col);
void fire(int row, int col);
void draw_dimensions();
void fieldInit();

void notmain() {
	fieldInit();
	unsigned int white = gfx_compute_color(128, 128, 128);
	int y = 10;
	y += 2*font_height();
	drawField(NUM_COLS, NUM_ROWS, TILE_SIZE, TILE_SIZE, START_POS, START_POS);
	int corners[4];
	int i;
	for(i = 0; i < 4; ++i){
		while(1){
			char c;
			if(keyboard_has_char()){
				c = keyboard_read_char();
				gfx_draw_letter(white, 0, 0, c);
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
	add_battleship(corners[0], corners[1], corners[2], corners[3]);
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
    }
    else {
        end_x = start_x + TILE_SIZE - 2*SHIP_SPACE;
        end_y = start_y + (end_row-start_row) * TILE_SIZE - 2*SHIP_SPACE;
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

    //draw second half of X, this part is not working!!! wtf
    volatile int new_start_y = start_y + TILE_SIZE;
    volatile int new_end_y = end_y - TILE_SIZE;
    gfx_draw_line(red, start_x, end_x, new_start_y, new_end_y, 0);
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
