// In interrupts/isr.c

#include "isr.h"
#include "../drivers/keyboard/keyboard.h"
#include <stdint.h> 

// ~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
// ✨✨✨ THE BIG SECRET! ✨✨✨
// We moved this whole function UP HERE! Now when the compiler
// reads isr_handler, it has ALREADY read this! It can't get
// confused! Nya-ha-ha! WE'RE SMARTER THAN YOU, GWEMLIN!
// ~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~
void pic_send_eoi(uint8_t irq) {
    if(irq >= 8) {
        // This says "I got it!" to the second contwowwew
        __asm__ __volatile__("outb %0, $0xA0" : : "a"((uint8_t)0x20));
    }
    // This says "I got it!" to the main contwowwew
    __asm__ __volatile__("outb %0, $0x20" : : "a"((uint8_t)0x20));
}


// Now the handler is down here, whewe it can't cause twouble!
void isr_handler(registers_t *regs) {
    // Is it the keyboawd? Mrow?
    if (regs->int_no == 33) {
        keyboard_handler();
    }
    
    // If it was any piece of hawdwawe, we MUST tell it we're done!
    if (regs->int_no >= 32 && regs->int_no < 48) {
        pic_send_eoi(regs->int_no - 32);
    }
}