#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

// --- Public Paw-lice Functions! ---

void screen_init();
void screen_clear();
void screen_print_char(char c);
void screen_print_string(const char* str);
void screen_newline();

// ✨ THE NEW ERASER POWER! ✨
void screen_backspace();

#endif