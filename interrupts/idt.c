#include "idt.h" 
#include <stdint.h>

// These promises are all here and ready!
extern void isr0(); extern void isr1(); extern void isr2(); extern void isr3();
extern void isr4(); extern void isr5(); extern void isr6(); extern void isr7();
extern void isr8(); extern void isr9(); extern void isr10(); extern void isr11();
extern void isr12(); extern void isr13(); extern void isr14(); extern void isr15();
extern void isr16(); extern void isr17(); extern void isr18(); extern void isr19();
extern void isr20(); extern void isr21(); extern void isr22(); extern void isr23();
extern void isr24(); extern void isr25(); extern void isr26(); extern void isr27();
extern void isr28(); extern void isr29(); extern void isr30(); extern void isr31();
extern void isr32(); extern void isr33();

struct IDTGate idt[256];
struct IDTPointer idt_ptr;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].low_offset = base & 0xFFFF;
    idt[num].high_offset = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void pic_remap() {
    __asm__ __volatile__("outb %0, $0x20" : : "a"((uint8_t)0x11));
    __asm__ __volatile__("outb %0, $0xA0" : : "a"((uint8_t)0x11));
    __asm__ __volatile__("outb %0, $0x21" : : "a"((uint8_t)0x20));
    __asm__ __volatile__("outb %0, $0xA1" : : "a"((uint8_t)0x28));
    __asm__ __volatile__("outb %0, $0x21" : : "a"((uint8_t)0x04));
    __asm__ __volatile__("outb %0, $0xA1" : : "a"((uint8_t)0x02));
    __asm__ __volatile__("outb %0, $0x21" : : "a"((uint8_t)0x01));
    __asm__ __volatile__("outb %0, $0xA1" : : "a"((uint8_t)0x01));
    __asm__ __volatile__("outb %0, $0x21" : : "a"((uint8_t)0x0));
    __asm__ __volatile__("outb %0, $0xA1" : : "a"((uint8_t)0x0));
}

void idt_init() {
    idt_ptr.limit = (sizeof(struct IDTGate) * 256) - 1;
    idt_ptr.base = (uint32_t)&idt;

    for(int i=0; i<256; ++i) { idt_set_gate(i, 0, 0, 0); }

    // ✨✨✨ THE COMPLETE, FULL, NOT-SLEEPY STAIRCASE! ✨✨✨
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
    idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
    idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);
    idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
    idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E);
    idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
    idt_set_gate(32, (uint32_t)isr32, 0x08, 0x8E); // Timer
    idt_set_gate(33, (uint32_t)isr33, 0x08, 0x8E); // Keyboard!

    pic_remap();
    
    __asm__ __volatile__("outb %0, $0x21" : : "a"((uint8_t)0xFD));
    __asm__ __volatile__("lidt %0" : : "m"(idt_ptr));
}