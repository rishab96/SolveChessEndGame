/* Fill in this application for printing the message
   GREETING PROFESSOR FALKEN

   HELLO

   A STRANGE GAME.
   THE ONLY WINNING MOVE IS
   NOT TO PLAY.

   HOW ABOUT A NICE GAME OF CHESS?
*/

#include "gfx.h"
#include "font.h"

void notmain() {
	gfx_init();
	gfx_set_double_buffered(0);
	unsigned int white = gfx_compute_color(128, 128, 128);
	int y = 10;
	gfx_draw_string(white, 10, y,"GREETING PROFESSOR FALKEN"); 
	y += 2*font_height();
	gfx_draw_string(white, 10, y, "HELLO");
	y += 2*font_height();
	gfx_draw_string(white, 10, y, "A STRANGE GAME.");
	y += font_height();
	gfx_draw_string(white, 10, y, "THE ONLY WINNING MOVE IS");
	y += font_height();
	gfx_draw_string(white, 10, y, "NOT TO PLAY.");
	y += 2*font_height();
	gfx_draw_string(white, 10, y, "HOW ABOUT A NICE GAME OF CHESS?");
	gfx_program_visualize(10, 200, 100, 100);
	gfx_draw_line(0xFFFFFFFF, 300, 400, 350, 450, 0);
	gfx_draw_line(0xFFFFFFFF, 350, 450, 450, 550, 1);	
	gfx_draw_rect(0xFFFFFFFF, 0, 300, 100, 100);
}
