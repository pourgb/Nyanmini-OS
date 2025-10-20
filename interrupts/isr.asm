bits 32
section .text

; ✨✨✨ THE MAGIC MACROS! ✨✨✨
; We're telling the whole world about our cute little ISRs!

; For interrupts that DON'T have an error code pushed by the CPU
%macro ISR_NOERRCODE 1
    global isr%1
    isr%1:
        cli
        push 0      ; Push a dummy error code so the stack is always the same, nya~
        push %1     ; Push the interrupt number!
        jmp isr_common_stub
%endmacro

; For interrupts that DO have an error code!
%macro ISR_ERRCODE 1
    global isr%1
    isr%1:
        cli
        ; The CPU pushes the error code for us, how nice!
        push %1     ; Push the interrupt number!
        jmp isr_common_stub
%endmacro


; --- Here are all the ISRs you pwomised in your C code! Ready for duty! ---
; Exceptions (ISRs 0-31)
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

; IRQs (Hardware Interrupts! beep boop!)
ISR_NOERRCODE 32  ; Timer! Tick-tock!
ISR_NOERRCODE 33  ; Keyboard! clacky clacky!


; --- The Simplest, Safest, Most Purr-fect Common Stub ---
extern isr_handler
isr_common_stub:
    pusha           ; Save all registers
    
    mov eax, esp    ; Get a pointer to the registers on the stack

    push eax        ; Push the pointer as an argument for our C function
    call isr_handler
    pop eax         ; Clean up the argument

    popa            ; Restore all registers
    
    add esp, 8      ; Clean up error code & int number
    iret            ; Return from interrupt