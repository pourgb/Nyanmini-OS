#ifndef BASIC_STORAGE_H
#define BASIC_STORAGE_H

// How many programs we can remember! Let's say... 10!
#define MAX_PROGRAMS 10 
// How big a program can be! 4KB should be enough for lots of chaos! >:3
#define MAX_PROG_LEN 4096 
#define MAX_PROG_NAME 32

// This is like a little label for each program!
typedef struct {
    char name[MAX_PROG_NAME];
    char code[MAX_PROG_LEN];
    int is_used; // Is this spot taken?
} BasicProgram;

// Our pwomises to the rest of the OS!
void storage_init();
int storage_save_program(const char* name, const char* code);
void storage_list_programs();
BasicProgram* storage_get_program(int index);

// ✨ OUR NEW MAGIC FLASHLIGHT PROMISE! ✨
void storage_dump_state(); 

#endif