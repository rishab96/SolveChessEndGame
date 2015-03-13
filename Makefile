ARM = arm-none-eabi
GCC = $(ARM)-gcc
LD  = $(ARM)-ld
AS  = $(ARM)-as
OD  = $(ARM)-objdump
OCP = $(ARM)-objcopy
TTY = /dev/tty.SLAB_USBtoUART

# Setting DEFAULT sets the default program that make
# will build and install. You can also build specific
# ones to build (e.g. 'make lines-2d.bin').
DEFAULT = battleship.bin

LIB_OBJS = start.o cstart.o helpers.o led.o gpio.o timer.o system.o gprof.o 
LIB_OBJS += fb.o keyboard.o interrupts_asm.o handlers.o gfx.o font.o buffer.o
CFLAGS  = -g -ffreestanding -nostdlib -nostartfiles -O0
LDFLAGS = -T memmap
LDFLAGS += -L/Users/zackherbst/bin/arm-none-eabi/bin/../lib/gcc/arm-none-eabi/4.8.3/

LIBS = -lgcc

.PRECIOUS: %.o %.elf

all: $(DEFAULT) 

install: all
	rpi-install $(TTY) $(DEFAULT)

%.o: %.c
	$(ARM)-gcc $(CFLAGS) -c -o $@ $^

%.o: %.s
	$(AS) $^ -o $@

%.elf: $(LIB_OBJS) %.o
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)
	$(OD) -D $@ > $@.list

%.bin: %.elf
	$(OCP) -O binary $^ $@

clean:
	rm -f *.bin *.elf *.list *~ *.o \#*
