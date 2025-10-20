#ifndef ATA_H
#define ATA_H

#include <stdint.h>

// Our magic spell to read one 512-byte cookie crumb (sector) from the disk!
// drive: 0 for hda, 1 for hdc
// lba: which crumb to get
// buffer: a safe place to put the crumb!
void ata_read_sector(uint8_t drive, uint32_t lba, uint8_t* buffer);
void ata_write_sector(uint8_t drive, uint32_t lba, uint8_t* buffer);
void ata_flush();

#endif