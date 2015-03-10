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
	gfx_set_double_buffered(1);
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
	gfx_draw();

}
