// In drivers/keyboard/keyboard.h
#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <stdint.h>

// ~*~*~*~ SPECIAL KEY CODES! ~*~*~*~
#define KEY_F1      0x101
#define KEY_UP      0x102
#define KEY_DOWN    0x103
#define KEY_LEFT    0x104
#define KEY_RIGHT   0x105

// ✨✨✨ ADD THESE NEW SPELLS! ✨✨✨
#define KEY_F2      0x106
#define KEY_F3      0x107


void keyboard_handler();
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void update_cursor(int x, int y);
void set_keyboard_handler(void (*handler)(int keycode));

#endif