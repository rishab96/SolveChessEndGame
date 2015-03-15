#include "keyboard.h"
#include "system.h"
#include "gpio.h"
#include "led.h"
#include "buffer.h"
#include "gfx.h"
#include "font.h"
#include "ps2.h"


/* Don't forget to mark any variables touched by interrupts as volatile! */

/* This file should define an interrupt handler for when there is a 
   falling edge on GPIO pin 23. It should read GPIO pin 24 in that
   handler to read in a bit of a PS2 scan code. If it successfully
   receives a scan code, the code should be put into a ring buffer.
   A call to keyboard_has_char() removes scan codes from the ring
   buffer until no remain (returning 0) or a character is read
   (storing the character and returning 1). */

#define INTERRUPT_ENABLE_1  0x2000b210
#define INTERRUPT_ENABLE_2  0x2000b214
#define INTERRUPT_DISABLE_1 0x2000b21c
#define INTERRUPT_DISABLE_2 0x2000b220

#define GPLEV0              0x20200034
#define GPLEV1              0x20200038
#define GPEDS0              0x20200040
#define GPEDS1              0x20200044

volatile int status;
volatile int pinval;
volatile int count;
volatile int shift;
volatile unsigned int up;

/* Initialize the keyboard driver.
   This should set up the keyboard GPIO pins and enable GPIO interrupts
   for them. It should not enable global interrupts: leave that to
   your main program. */
void keyboard_init() {
  gpio_init();
 
  PUT32(INTERRUPT_DISABLE_1, 0xffffffff); 
  PUT32(INTERRUPT_DISABLE_2, 0xffffffff);

  // Put code to configure which GPIO events to trigger interrupts below
 // gpio_set_pullup(GPIO_PIN23);
  gpio_set_input(GPIO_PIN23); 
  gpio_detect_falling_edge(GPIO_PIN23); 
  gpio_set_input(GPIO_PIN24);
  //gpio_set_pullup(GPIO_PIN24);
  // Bit 52 in IRQ registers enables/disables all GPIO interrupts
  // Bit 52 is in the second register, so subtract 32 for index
  PUT32(INTERRUPT_ENABLE_2, (1 << (52 - 32))); 
}

void int_handler(unsigned int pc){
	//Check the events from the 23rd pin
	led_toggle();
  gpio_check_and_clear_event(GPIO_PIN23);
  if(status){
		if(count < 8){
	    pinval |= (gpio_pin_read(24))<<count;
		}
		else
			gpio_pin_read(24);
		++count;
//        gfx_draw_letter(0xFFFFFFFF, 250+count*font_width(),250, '0' +gpio_pin_read(24)); 
		if(count == 9){
			insert(pinval);
			count = 0;
			status = 0;
			pinval = 0;
		}
  }
  if(count == 0 && gpio_pin_read(24) == 0){
    status = 1;
  }
}

/* Return 1 if there are characters to read, 0 if no characters to read.
   This call should process PS2 scan codes in the ring buffer, returning
   after the first scan code that results in a pressed character. This
   keeps the interrupt handler simple (just put scan codes in ring
   buffer) and puts the complex logic (handling shift, caps lock, etc.)
   in this call.

   This call should not assume that it is followed by keyboard_read_char().
   That is, if a program calls keyboard_has_chars() twice, this should
   not result in characters being lost.*/
unsigned keyboard_has_char() {
	if(!has_elements())
		return 0;
	if(peek() == 0xF0){
		remove();
		//set some flag for when we've read the up
		up = (up == 0) ? 1 : 0;
		return 0;
	}
	else{	//There is a character
		return 1;
	}
}

/* Return the next ASCII character pressed. If keyboard_has_char() is
false, the return value of this function is undefined. This
   function can assume that keyboard_has_char() was called before it
   and that call returned true. This call does not block. */
char keyboard_read_char() {
	//If we read the shift character or caps lock
	int val = remove();
	if(up || val == 0xE0){
		return '\0';
	}
	if(val == 0x12 || val == 0x59 || val == 0x58){
		shift = (shift == 0) ? 1 : 0;
		return '\0';
	}
	if(val == 0x5A)
		return '\n';
	if(val == 0x66)
		return '\b';
	if(shift)
		return shift_lookup_table[val];
	else
		return lookup_table[val];
}

char get_char(int scan_code) {
   return lookup_table[scan_code];
    }
