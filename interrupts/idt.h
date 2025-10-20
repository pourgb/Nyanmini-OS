#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// This struct is the same, uwu
struct IDTGate {
    uint16_t low_offset;
    uint16_t selector;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t high_offset;
} __attribute__((packed));

// This pointer is also the same!
struct IDTPointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// ✨ FIXED! ✨ This now correctly matches the order of 'pusha'!
typedef struct registers {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha
    uint32_t int_no, err_code;                      // Pushed by our ISR stub
    uint32_t eip, cs, eflags, useresp, ss;          // Pushed by the CPU automatically
} registers_t;

// This is still our main setup function!
void idt_init();

#endif