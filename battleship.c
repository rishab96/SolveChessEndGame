#include "gfx.h"
#include "font.h"

void drawField(int nrow, int ncol, int width, int height, int x, int y);

void notmain() {
	gfx_init();
	gfx_set_double_buffered(0);
	unsigned int white = gfx_compute_color(128, 128, 128);
	int y = 10;
	gfx_draw_string(white, 10, y,"Welcome to Battleship!"); 
	y += 2*font_height();
	drawField(4, 4, 10, 10, 30, 30);
}

void drawField(int nrow, int ncol, int width, int height, int x, int y){
	int i;
	for(i = 0; i <= ncol; ++i){
		gfx_draw_line(0xFFFFFFFF, x+i*width, x+i*width, y, y+height*nrow, 0);
	}
	int j;
	for(j = 0; j <= nrow; ++j){
		gfx_draw_line(0xFFFFFFFF, x,  x+width*ncol, y+j*height, y+j*height, 0);
	}
}
