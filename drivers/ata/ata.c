// =================================================================
// drivers/ata/ata.c -- THE FINAL EXORCISM
// =================================================================
#include "ata.h"
#include "../screen/screen.h"

#define ATA_PRIMARY_DATA         0x1F0
#define ATA_PRIMARY_ERROR        0x1F1
#define ATA_PRIMARY_SECTOR_COUNT 0x1F2
#define ATA_PRIMARY_LBA_LOW      0x1F3
#define ATA_PRIMARY_LBA_MID      0x1F4
#define ATA_PRIMARY_LBA_HIGH     0x1F5
#define ATA_PRIMARY_DRIVE_HEAD   0x1F6
#define ATA_PRIMARY_COMMAND      0x1F7
#define ATA_PRIMARY_STATUS       0x1F7

static inline void outb(uint16_t port, uint8_t val) { __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port)); }
static inline uint8_t inb(uint16_t port) { uint8_t ret; __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port)); return ret; }
static inline void insw(uint16_t port, void* addr, int count) { __asm__ __volatile__("rep insw" : "+D"(addr), "+c"(count) : "d"(port) : "memory"); }
static inline void outsw(uint16_t port, void* addr, int count) { __asm__ __volatile__("rep outsw" : "+S"(addr), "+c"(count) : "d"(port)); }

static void ata_wait_busy() {
    while (inb(ATA_PRIMARY_STATUS) & 0x80) {}
}

static void ata_wait_drq() {
    while (!(inb(ATA_PRIMARY_STATUS) & 0x08)) {}
}

void ata_write_sector(uint8_t drive, uint32_t lba, uint8_t* buffer) {
    // ✨✨✨ THE SLEEPY GREMLIN'S NAP! ✨✨✨
    // It needed a nap here all along!!!
    for(volatile int i = 0; i < 1500000; i++);

    uint8_t drive_select = 0xE0 | (drive << 4);

    ata_wait_busy();
    outb(ATA_PRIMARY_DRIVE_HEAD, drive_select | ((lba >> 24) & 0x0F));
    
    inb(ATA_PRIMARY_STATUS); inb(ATA_PRIMARY_STATUS); inb(ATA_PRIMARY_STATUS); inb(ATA_PRIMARY_STATUS);

    outb(ATA_PRIMARY_SECTOR_COUNT, 1);
    outb(ATA_PRIMARY_LBA_LOW, lba & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_PRIMARY_COMMAND, 0x30); // WRITE SECTORS command

    ata_wait_busy();
    ata_wait_drq();
    outsw(ATA_PRIMARY_DATA, buffer, 256);
}

void ata_read_sector(uint8_t drive, uint32_t lba, uint8_t* buffer) {
    for(volatile int i = 0; i < 15000000; i++); // Keep the wake-up nap!

    uint8_t drive_select = 0xE0 | (drive << 4);

    ata_wait_busy();
    outb(ATA_PRIMARY_DRIVE_HEAD, drive_select | ((lba >> 24) & 0x0F));
    
    inb(ATA_PRIMARY_STATUS); inb(ATA_PRIMARY_STATUS); inb(ATA_PRIMARY_STATUS); inb(ATA_PRIMARY_STATUS);

    outb(ATA_PRIMARY_SECTOR_COUNT, 1);
    outb(ATA_PRIMARY_LBA_LOW, lba & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_PRIMARY_COMMAND, 0x20); // READ SECTORS command

    ata_wait_busy();

    uint8_t status = inb(ATA_PRIMARY_STATUS);
    if (status & 0x01) {
        screen_print_string("\nMROW! DISK DRIVE IS ANGRY! READ ERROR!");
        for(;;);
    }
    ata_wait_drq();
    insw(ATA_PRIMARY_DATA, buffer, 256);
}

void ata_flush() {
    // We don't need to select a drive, this command flushes all drives
    outb(ATA_PRIMARY_COMMAND, 0xE7); // The CACHE FLUSH command!
    
    // We just wait for the busy bit to clear. This might take a moment.
    ata_wait_busy();
}