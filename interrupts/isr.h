#ifndef ISR_H
#define ISR_H

#include "idt.h" // We need to know what registers_t is!

// This is the main C function that our assembly gremlins will call!
void isr_handler(registers_t *regs);

#endif