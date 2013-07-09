#ifndef _INPUT_H
#define _INPUT_H

int input_start(void);
void input_register(int key, void (*callback)(int), int data);
void input_poll(void);
void input_set(int k);
void input_stop(void);

#endif
