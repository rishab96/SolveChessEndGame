#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#define SIZE 128

extern volatile int buffer[SIZE];
extern volatile int head;
extern volatile int tail;


int remove();
void insert(int val);
int has_elements();
int peek();
#endif
