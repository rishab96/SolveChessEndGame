/* Functions for a simple graphics library for the bare metal
 * Raspberry Pi.
 *
 * Author: Philip Levis <pal@cs.stanford.edu>
 * Date: August 14 2014
 */ 

#include "gfx.h"
#include "fb.h"
#include "system.h"
#include "font.h"

extern int __text_start__;
extern int __text_end__;

void gfx_init() {
	fb_init();
}

char * gfx_buffer(){
	return ((char*) fb_writeable_buffer());
}

void gfx_set_double_buffered(unsigned int on){
	fb_set_double_buffered(on);
}

unsigned int gfx_compute_color(unsigned char red,
                               unsigned char green,
                               unsigned char blue) {
	return(0xFF000000 | blue<<16 | green<<8 | red);
}

unsigned int gfx_compute_shade(unsigned char red,
															 unsigned char green,
															 unsigned char blue,
															 unsigned char alpha){
	return( alpha<<24 | blue << 16 | green <<8 | red);
}

unsigned int gfx_get_width() {
	return fb_width();
}

unsigned int gfx_get_height() {
	return fb_height();
}

void gfx_plot(unsigned int color,
              unsigned int x,
              unsigned int y) {
	if(x >= fb_width() || y >= fb_height())
		return;
	unsigned int buf = (unsigned int)fb_writeable_buffer();
	buf += ((y*fb_width()) + x)*fb_byte_depth();
	PUT32(buf, color);
}

unsigned int gfx_letter_height() {
	return font_height();
}

unsigned int gfx_letter_width() {
	return font_width();
}

unsigned int gfx_draw_letter(unsigned int color,
                             unsigned int x,
                             unsigned int y,
                             char letter) {
	int buflen = font_buflen();
	int width = font_width();
	if(letter < ' ' || letter > '~')
		return 0;
	char buffer[buflen];
	font_ascii(letter, buffer, buflen);
	unsigned int (*pixels)[width] = (unsigned int (*)[width])buffer;
	unsigned int yval;
	for(yval = 0; yval < font_height(); ++yval){
		unsigned int xval;
		for(xval = 0; xval < font_width(); ++xval){
			unsigned int pixelval = pixels[yval][xval];
			unsigned int rgb[3];
			int i;
			for(i = 0; i < 3; ++i){
				rgb[i] = (pixelval >> (i*8)) & 0xFF;
				rgb[i] *= (color >> (i*8)) & 0xFF;
				rgb[i] = rgb[i] >> 8;
			}
			pixelval = (pixelval & 0xFF000000) | rgb[0] | (rgb[1] << 8) | (rgb[2] << 16);
			gfx_plot(pixelval, x+xval, y+yval);
		}
	}
	return 1;
}
  
unsigned int gfx_draw_string(unsigned int color,
                             unsigned int x,
                             unsigned int y,
                             char* str) {
	char * p = str;
	int  tempx = x;
	while(p){
		if(gfx_draw_letter(color, tempx, y, *p))
			tempx += (gfx_letter_width() + 1);
		else
			return 0;
		++p;
	}
	return 1;
}

void gfx_clear(){
	fb_clear();
}

void gfx_draw(){
	fb_draw();
}

void gfx_program_visualize(unsigned int x, unsigned int y, unsigned int width, unsigned int length){
	int xpos;
	int ypos;
	int count = 0;
	unsigned int * address = (unsigned int *) 0x8000;
	for(ypos = 0; ypos < length; ++ypos){
		for(xpos = 0; xpos < width; ++xpos){
			if(address == (unsigned int *)&__text_end__)
				return;
			unsigned int val = *address;
			gfx_plot(val, x+xpos, y+ypos);
			++address;
		}
	}
}

void gfx_draw_line_aliased(unsigned int color, int x0, int x1, int y0, int y1){
		int x;
		for(x = x0; x < x1; ++x){
			int y = (int) (y1-y0) * (x-x0) / (x1 - x0) + y0;
			gfx_plot(color, x, y);
		}
}

int ipart(float x){
	return (int) x;
}

int round(float x){
	return ipart(x+0.5);
}

float fpart(float x){
	if(x < 0)
		return 1-(x-(int)x);
	return x-(int)x;
}

float rfpart(float x){
	return 1-fpart(x);
}

float abs(float x){
	if(x >= 0)
		return x;
	return -x;
}

void gfx_plot_alpha(unsigned int color, float alpha, int x, int y){
	unsigned char alphaval = alpha*255;
	unsigned int colorval = (0xFFFFFF | alphaval<<24) & color;
	gfx_plot(colorval, x, y);
}

void gfx_draw_line_anti(unsigned int color, int x0, int x1, int y0, int y1){
	unsigned int steep = abs(y1-y0) > abs(x1 - x0);
	if(steep){
		int tx0 = x0;
		x0 = y0;
		y0 = tx0;
		int tx1 = x1;
		x1 = y1;
		y1 = tx1;
	}
	if(x0 > x1){
		int tx0 = x0;
		x0 = x1;
		x1 = tx0;
		int ty0 = y0;
		y0 = y1;
		y1 = ty0;
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	float gradient = dy/dx;

	float xend = round(x0);
	float yend = y0 + gradient*(xend - x0);
	float xgap = rfpart(x0+0.5);
	float xpxl1 = xend;
	float ypxl1 = ipart(yend);
	if(steep){
		gfx_plot_alpha(color, rfpart(yend)*xgap, ypxl1, xpxl1);
		gfx_plot_alpha(color, fpart(yend)*xgap, ypxl1+1, xpxl1);
	}
	else{
		gfx_plot_alpha(color, rfpart(yend)*xgap, xpxl1, ypxl1);
		gfx_plot_alpha(color, fpart(yend)*xgap, xpxl1, ypxl1+1);
	}
	float intery = yend + gradient;

	xend = round(x1);
	yend = y1+gradient*(xend - x1);
	xgap = fpart(x1+0.5);
	float xpxl2 = xend;
	float ypxl2 = ipart(yend);
	if(steep){
		gfx_plot_alpha(color, rfpart(yend)*xgap, ypxl2, xpxl2);
		gfx_plot_alpha(color, fpart(yend)*xgap, ypxl2+1, ypxl2);
	}
	else{
		gfx_plot_alpha(color, rfpart(yend)*xgap, xpxl2, ypxl2);
		gfx_plot_alpha(color,fpart(yend)*xgap, xpxl2, ypxl2+1);
	}
	int x;
	for(x = xpxl1+1; x < xpxl2; ++x){
		if(steep){
			gfx_plot_alpha(color, rfpart(intery), ipart(intery), x);
			gfx_plot_alpha(color, fpart(intery), ipart(intery) + 1, x);
		}
		else{
			gfx_plot_alpha(color, rfpart(intery), x, ipart(intery));
			gfx_plot_alpha(color, fpart(intery), x, ipart (intery) + 1);
		}
		intery += gradient;
	}
}

void gfx_draw_line(unsigned int color, int x0, int x1, int y0, int y1, unsigned int aliased){
	if(x0 > x1){
			int temp = x0;
			x0 = x1;
			x1 = temp;
	}
	if(y0 > y1){
		int temp = y0;
		y0 = y1;
		y1 = temp;
	}
	if(aliased == 1)
		gfx_draw_line_aliased(color, x0, x1, y0, y1);	
	else
		gfx_draw_line_anti(color, x0, x1, y0, y1);
}

void gfx_draw_rect(unsigned int color, int x, int y, int width, int height){
	int xpos;
	int ypos;
	for(xpos = x; xpos < (x+width); ++xpos){
		for(ypos = y; ypos < (y+height); ++ypos){
			gfx_plot(color, xpos, ypos);
		}
	}	
}

/*
 * Copyright (c) 2014 Stanford University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the Stanford University nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL STANFORD
 * UNIVERSITY OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
