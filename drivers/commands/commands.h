#ifndef COMMANDS_H
#define COMMANDS_H
#include <stdint.h>

void command_handle_key(int keycode);
void commands_init();
void print_prompt(); 
void do_clear();

#endif