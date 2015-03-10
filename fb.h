#ifndef FB_H_INCLUDED
#define FB_H_INCLUDED

int fb_init();

/* Return a pointer to the off-screen buffer. */

volatile char* fb_writeable_buffer();
unsigned int fb_byte_depth();
unsigned int fb_height();
unsigned int fb_width();

/* Clear the off-scren buffer.*/
void fb_clear();

/* Swap the on-screen and off-screen buffers. */
void fb_draw();

/* Return a pointer to the on-screen buffer. */
const char* fb_display();

/* Sets the buffer as double- or single-buffered. */
void fb_set_double_buffered(unsigned int on);
#endif
