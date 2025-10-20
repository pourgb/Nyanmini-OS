; =================================================================
; boot.asm - THE NEW, SUPER-STRONG GHOST TRAP!
; =================================================================
bits 16
org 0x7c00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov [boot_drive], dl

    ; Enable A20 Gate
    in al, 0x92
    or al, 2
    out 0x92, al

    mov si, stage1_msg
    call print_string_16

    ; --- Load Kernel from disk to 0x10000 ---
    mov dl, [boot_drive]
    mov ax, 0x1000  ; ES:BX -> 0x1000:0x0000 -> physical 0x10000
    mov es, ax
    mov bx, 0x0000

    mov ah, 0x02
    mov al, 40      ; Let's load a bit more just to be safe, run.sh will patch this
    mov ch, 0
    mov cl, 2
    mov dh, 0
    int 0x13
    jc disk_error

    ; --- Switch to Protected Mode ---
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to flush the CPU pipeline and load our code segment selector
    jmp 0x08:p_mode_start

bits 32
p_mode_start:
    ; --- We are now in 32-bit Protected Mode! ---
    ; Set up all our data segments with the data selector (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up a safe stack pointer
    mov esp, 0x90000

    ; Jump to the kernel's main entry point!
    jmp 0x10000

disk_error:
    mov si, disk_err_msg
    call print_string_16
    cli
    hlt

print_string_16:
    mov ah, 0x0e
.print_char:
    lodsb
    cmp al, 0
    je .retp
    int 0x10
    jmp .print_char
.retp:
    ret

stage1_msg db 'stage 1',0
disk_err_msg db 'disk read failed',0
boot_drive: db 0

; --- GDT (Global Descriptor Table) ---
gdt_start:
    ; Null Descriptor
    dq 0x0000000000000000

    ; Code Segment: Base=0, Limit=4GB, Granularity=4KB, 32-bit, Read/Execute
gdt_code:
    dw 0xFFFF    ; Limit (low)
    dw 0x0000    ; Base (low)
    db 0x00      ; Base (mid)
    db 0x9A      ; Access (Present, DPL 0, Executable, Read/Write)
    db 0xCF      ; Granularity (4K pages, 32-bit), Limit (high)
    db 0x00      ; Base (high)

    ; Data Segment: Base=0, Limit=4GB, Granularity=4KB, 32-bit, Read/Write
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92      ; Access (Present, DPL 0, Data, Read/Write)
    db 0xCF
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510-($-$$) db 0
dw 0xAA55