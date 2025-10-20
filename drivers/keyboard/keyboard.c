#include "keyboard.h"
#include <stdint.h>

static void (*keyboard_callback)(int keycode);
static int shift_held = 0;

// ~*~*~*~ Hardware & Cursor Magic (Unchanged) ~*~*~*~
static inline uint8_t inb(uint16_t port){uint8_t r;__asm__ __volatile__("inb %1, %0":"=a"(r):"Nd"(port));return r;}
static inline void outb(uint16_t p, uint8_t v){__asm__ __volatile__("outb %0, %1"::"a"(v),"Nd"(p));}
void enable_cursor(uint8_t s, uint8_t e){outb(0x3D4,0x0A);outb(0x3D5,(inb(0x3D5)&0xC0)|s);outb(0x3D4,0x0B);outb(0x3D5,(inb(0x3D5)&0xE0)|e);}
void update_cursor(int x, int y){uint16_t p=y*80+x;outb(0x3D4,0x0F);outb(0x3D5,(uint8_t)(p&0xFF));outb(0x3D4,0x0E);outb(0x3D5,(uint8_t)((p>>8)&0xFF));}

// ~*~*~*~ THE NEW, PERFECT, UNSCRIBBLED-ON MAPS! ~*~*~*~
const unsigned char scancode_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'','`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0,
    '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// Nya~ Adding the shifted map to avoid gremlins!!! 🥺✨
const unsigned char scancode_map_shifted[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0,
    ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0,
    '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void set_keyboard_handler(void (*handler)(int keycode)) {
    keyboard_callback = handler;
}

void keyboard_handler() {
    uint8_t scancode = inb(0x60);
    // Nya~ Debug print scancode as two hex digits at VGA bottom-left!
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    char hex_digits[] = "0123456789ABCDEF";
    vga[24*80 + 0] = (0x0F << 8) | hex_digits[(scancode >> 4) & 0xF]; // High nibble
    vga[24*80 + 1] = (0x0F << 8) | hex_digits[scancode & 0xF];       // Low nibble
    vga[24*80 + 2] = (0x0F << 8) | ' '; // Space for clarity
    // Nya~ Debug print keycode at VGA bottom-middle!
    int keycode = 0;
    if (scancode == 0x2A || scancode == 0x36) {
        shift_held = 1; return;
    }
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_held = 0; return;
    }
    if (scancode >= 0x80) return;

    switch(scancode) {
        case 0x3B: keycode = KEY_F1;    break;
        case 0x3C: keycode = KEY_F2;    break;
        case 0x3D: keycode = KEY_F3;    break;
        case 0x48: keycode = KEY_UP;    break;
        case 0x50: keycode = KEY_DOWN;  break;
        case 0x4B: keycode = KEY_LEFT;  break;
        case 0x4D: keycode = KEY_RIGHT; break;
        default: break;
    }

    if (keycode != 0) {
        vga[24*80 + 40] = (0x0F << 8) | hex_digits[(keycode >> 4) & 0xF];
        vga[24*80 + 41] = (0x0F << 8) | hex_digits[keycode & 0xF];
        vga[24*80 + 42] = (0x0F << 8) | ' ';
        if (keyboard_callback) keyboard_callback(keycode);
        return;
    }

    char c = 0;
    if (shift_held) {
        c = scancode_map_shifted[scancode];
    } else {
        c = scancode_map[scancode];
    }
    
    if (c != 0) {
        keycode = c;
        vga[24*80 + 40] = (0x0F << 8) | hex_digits[(keycode >> 4) & 0xF];
        vga[24*80 + 41] = (0x0F << 8) | hex_digits[keycode & 0xF];
        vga[24*80 + 42] = (0x0F << 8) | ' ';
        if (keyboard_callback) keyboard_callback(c);
    }
}