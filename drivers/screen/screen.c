#include "screen.h"
#include "../keyboard/keyboard.h" // We need this for the hardware cursor update!

// ~*~*~*~ POLICE SECRETS! (Internal State) ~*~*~*~
static int cursor_x = 0;
static int cursor_y = 0;
static volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
static uint16_t attr = 0x0F << 8;

static void update_vga_cursor() {
    update_cursor(cursor_x, cursor_y);
}

// ... screen_init, screen_clear, screen_newline, etc. are all the same ...
void screen_init() {
    cursor_x = 0;
    cursor_y = 0;
    for (int i = 0; i < 25 * 80; i++) {
        vga[i] = attr | ' ';
    }
    update_vga_cursor();
}

void screen_clear() {
    cursor_x = 0;
    cursor_y = 0;
    for (int i = 0; i < 25 * 80; i++) {
        vga[i] = attr | ' ';
    }
    update_vga_cursor();
}

void screen_newline() {
    cursor_x = 0;
    cursor_y++;
    if (cursor_y >= 25) {
        screen_clear(); 
    }
    update_vga_cursor();
}

void screen_print_char(char c) {
    if (c == '\n') {
        screen_newline();
        return;
    }
    if (cursor_x >= 80) {
        screen_newline();
    }
    vga[cursor_y * 80 + cursor_x] = attr | c;
    cursor_x++;
    update_vga_cursor();
}

void screen_print_string(const char* str) {
    while (*str) {
        screen_print_char(*str++);
    }
}


// ✨ TEACHING THE PAW-LICE HOW TO ERASE! ✨
void screen_backspace() {
    // We only erase if we're not at the very start of the line!
    if (cursor_x > 0) {
        cursor_x--; // Move the cursor position back
        vga[cursor_y * 80 + cursor_x] = attr | ' '; // Put a blank space there
        update_vga_cursor(); // Move the blinking cursor to the new spot!
    }
}