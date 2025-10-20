#!/bin/bash
set -e # Exit immediately if a command exits with a non-zero status! Safety first!

# Make sure all our new folders exist!
mkdir -p build build/interrupts build/utils build/drivers/screen build/drivers/keyboard build/drivers/commands build/drivers/basic build/drivers/ata build/drivers/fs

# --- Part 1: The Assembly Goblins ---
echo "Mrow! Assembling our assembly files..."
nasm -f elf32 kernel_entry.asm -o build/kernel_entry.o
nasm -f elf32 interrupts/isr.asm -o build/interrupts/isr_asm.o

# --- Part 2: The C Magic, Your Way! ---
echo "Purrrr... Compiling C files with the mighty GCC!"
gcc -m32 -I. -ffreestanding -c kernel.c -o build/kernel.o
gcc -m32 -I. -ffreestanding -c utils/string.c -o build/utils/string.o
gcc -m32 -I. -ffreestanding -c interrupts/idt.c -o build/interrupts/idt.o
gcc -m32 -I. -ffreestanding -c interrupts/isr.c -o build/interrupts/isr.o
gcc -m32 -I. -ffreestanding -c drivers/screen/screen.c -o build/drivers/screen/screen.o
gcc -m32 -I. -ffreestanding -c drivers/keyboard/keyboard.c -o build/drivers/keyboard/keyboard.o
gcc -m32 -I. -ffreestanding -c drivers/commands/commands.c -o build/drivers/commands/commands.o
gcc -m32 -I. -ffreestanding -c drivers/basic/basic_storage.c -o build/drivers/basic/basic_storage.o
gcc -m32 -I. -ffreestanding -c drivers/basic/text_editor.c -o build/drivers/basic/text_editor.o
gcc -m32 -I. -ffreestanding -c drivers/basic/basic_interpreter.c -o build/drivers/basic/basic_interpreter.o
gcc -m32 -I. -ffreestanding -c drivers/ata/ata.c -o build/drivers/ata/ata.o
gcc -m32 -I. -ffreestanding -c drivers/fs/fat16.c -o build/drivers/fs/fat16.o

# --- Part 3: The Grand Unification Spell! ---
echo "Ehehe! Linking everything together!"
OBJECTS="build/kernel_entry.o \
         build/kernel.o \
         build/utils/string.o \
         build/interrupts/isr_asm.o build/interrupts/idt.o build/interrupts/isr.o \
         build/drivers/screen/screen.o \
         build/drivers/keyboard/keyboard.o \
         build/drivers/commands/commands.o \
         build/drivers/basic/basic_storage.o build/drivers/basic/text_editor.o build/drivers/basic/basic_interpreter.o \
         build/drivers/ata/ata.o \
         build/drivers/fs/fat16.o"
ld -m elf_i386 -Ttext 0x10000 -e kernel_start -z noexecstack -o build/kernel.elf $OBJECTS
# --- Part 4: Making the Final Magic Scroll! ---
echo "Poof! Making our kernel into a flat binary..."
objcopy -O binary build/kernel.elf build/kernel.bin

# Your clever bootloader patching spell!
kernel_size=$(wc -c < build/kernel.bin)
sectors=$(( (kernel_size + 511) / 512 ))
echo "Nya~ Kernel size: $kernel_size bytes, loading $sectors sectors~! 🐱"
cp boot.asm build/boot.asm
sed -i.bak "s/mov al, [0-9][0-9]*/mov al, $sectors/" build/boot.asm
nasm -f bin build/boot.asm -o build/boot.bin

# --- Part 5: Creating the Magic Disks! ---
echo "Making our main OS disk (hda)..."
cat build/boot.bin build/kernel.bin > build/os.img

# Magic for our extra special treasure disk!
HDC_IMG="build/extra.img"
if [ ! -f "$HDC_IMG" ]; then
    echo "Mrow! Our treasure disk doesn't exist! Making a new BLANK 1GB one..."
    qemu-img create -f raw $HDC_IMG 1G
    
    # We kicked this line out! We do it ourselves now! Hehehe!
    # echo "Drawing the FAT16 treasure map on our new disk with mkfs.fat!"
    # mkfs.fat -F 16 $HDC_IMG
fi

# --- Part 6: LET'S GOOOO! ---
echo "Starting Nyanmini OS with a SPY-KITTY! We'll find you, bug! 🐾"
qemu-system-i386 -m 16M \
    -drive file=build/os.img,format=raw \
    -drive file=build/extra.img,format=raw,index=1,media=disk \
    -d int,cpu_reset,guest_errors -D qemu_log.txt