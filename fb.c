/* Implement frame buffer from fb.h here. */

#include "fb.h"

#define MAILBOX 0x2000B880
#define GPU 0x40000000
#define BUF_WIDTH 1280
#define BUF_HEIGHT 960
#define BUF_DEPTH 4 

const int MAILBOX_FULL = (1<<31);
const int MAILBOX_EMPTY = (1<<30);

//Defining the mailbox!
//Including the padding so that the offsets match up with the mailbox format
typedef struct {
	unsigned int read;
	unsigned int padding[3];
	unsigned int peek;
	unsigned int sender;
	unsigned int status;
	unsigned int configuration;
	unsigned int write;
} mailbox_t;


//Frame buffer configuration structure
typedef struct {
	unsigned int width;
	unsigned int height;
	unsigned int virtual_width;
	unsigned int virtual_height;
	unsigned int pitch;
	unsigned int depth;
	unsigned int x_offset;
	unsigned int y_offset;
	unsigned int pointer;
	unsigned int size;
	unsigned int double_buffer;	//Either a 1 or a 0, depending on whether or not we're double-buffering!
} fb_config_t;

char * buf;

//Forces gcc to align the struct so that the address of the first field of the configuration
//ends in 0x0
static volatile fb_config_t config __attribute__ ((aligned(16)));
volatile mailbox_t * mb = (mailbox_t *)MAILBOX;
int mb_read();
void mb_write();

int fb_init(){
	config.width = BUF_WIDTH;
	config.height = BUF_HEIGHT;
	config.virtual_width = BUF_WIDTH;
	config.virtual_height = 2*BUF_HEIGHT;
	config.depth = 8*BUF_DEPTH;	//for a total of 32
	config.x_offset = 0;
	config.y_offset = 0;
	config.pointer = 0;
	config.pitch = 0;
	config.size = 0;
	
	mb_write();
	return mb_read();
}

void fb_set_double_buffered(unsigned int on){
	config.double_buffer = on;
}

void mb_write(){
	while(mb->status & MAILBOX_FULL){}
	mb->write = ((unsigned int)&config) + GPU + 1;
}

int mb_read(){
	while(1){
		while(mb->status & MAILBOX_EMPTY){}
		int val = mb->read;
		if( (val & 0xF) == 1){
			return 1;
		}
		return 0;
	}
}

unsigned fb_byte_depth(){
	return config.depth / 8;
}

unsigned fb_height(){
	return config.height;
}

unsigned fb_width(){
	return config.width;
}

volatile char * fb_writeable_buffer(){
	if(config.y_offset == config.height || config.double_buffer == 0)
		return (char*) config.pointer;
	return (char*) (config.pointer + config.width*config.height*config.depth/8);
}

const char * fb_display(){
	if(config.y_offset == config.height || config.double_buffer == 0)
		return (char*) (config.pointer + config.width*config.height*config.depth/8);
	return (char*) config.pointer;

}

void fb_clear(){
	unsigned int * buffer = (unsigned int *) fb_writeable_buffer();
	for(int i = 0; i < (fb_width() * fb_height()); ++i){
		buffer[i] = 0x00000000;
	}
}

void fb_draw() {
	if(config.y_offset == 0)
		config.y_offset = config.height;
	else
		config.y_offset = 0;
	mb_write();
	mb_read();
}

