#ifndef GFX_H_INCLUDED
#define GFX_H_INCLUDED

/* Functions for a simple bare metal Raspberry Pi graphics
 * library. Builds on the framebuffer library fb.[ch] for frame buffer
 * access and configuration; trying to use both simultaneously
 * is discouraged.
 *
 * This is a simple double-buffering library. You can draw on a
 * writeable buffer with gfx_block and other commands, then swap it in
 * as the display buffer with gfx_draw.  The function gfx_clear clears
 * the writeable buffer.
 *
 * Author: Philip Levis <pal@cs.stanford.edu>
 * Date: August 14 2014
 */ 

void gfx_init();

unsigned int gfx_compute_color(unsigned char red,
                               unsigned char green,
                               unsigned char blue);

unsigned int gfx_get_width();
unsigned int gfx_get_height();

// Plot a single point
void gfx_plot(unsigned int color,
              unsigned int x,
              unsigned int y);

unsigned int gfx_draw_letter(unsigned int color,
                             unsigned int x,
                             unsigned int y,
                             char letter);

unsigned int gfx_draw_string(unsigned int color,
                             unsigned int x,
                             unsigned int y,
                             char* str);

unsigned int gfx_letter_height();
unsigned int gfx_letter_width();

void gfx_clear();
void gfx_draw();
void gfx_set_double_buffered(unsigned int on);

void gfx_program_visualize(unsigned int x, unsigned int y, unsigned int width, unsigned int length);

void gfx_draw_line(unsigned int color, int x0, int x1, int y0, int y1, unsigned int aliased);

void gfx_draw_rect(unsigned int color, int x, int y, int width, int height);

#endif

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
