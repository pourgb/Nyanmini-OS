; =================================================================
; kernel_entry.asm - THE NEW, SIMPLIFIED KERNEL ENTRY
; =================================================================
bits 32
global kernel_start
extern main

; The bootloader jumps here!
; By the time we arrive, we are already in 32-bit mode,
; and all the segment registers (DS, ES, SS, etc.) and the
; stack pointer (ESP) have been set up for us!
kernel_start:
    ; Time to hand off to the C part of our kernel!
    call main

    ; If main ever returns, we'll just chill here forever.
    cli
.hlt:
    hlt
    jmp .hlt