#include "buffer.h"

volatile int buffer[SIZE];
volatile int head = 0;
volatile int tail = 0;

int remove(){
  int val = buffer[head];
  head = (head+1)%SIZE;
  return val;
}

int peek(){
	return buffer[head];
}

void insert(int val){
  int next_tail = (tail+1)%SIZE;
  if(next_tail == head) return;   //the buffer is full
  buffer[tail] = val;
  tail = next_tail;
}

int has_elements(){
  return (tail != head);
}


