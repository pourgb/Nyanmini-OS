#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN    0x02
#define ATTR_SYSTEM    0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE   0x20

typedef struct {
    uint8_t jmp[3]; char oem[8]; uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster; uint16_t reserved_sectors;
    uint8_t num_fats; uint16_t root_dir_entries;
    uint16_t total_sectors_short; uint8_t media_descriptor;
    uint16_t sectors_per_fat; uint16_t sectors_per_track;
    uint16_t num_heads; uint32_t hidden_sectors;
    uint32_t total_sectors_long; uint8_t drive_num;
    uint8_t reserved; uint8_t boot_sig; uint32_t volume_id;
    char volume_label[11]; char fs_type[8];
} __attribute__((packed)) FAT16_BootSector;

typedef struct {
    char filename[8]; char ext[3]; uint8_t attributes;
    uint8_t reserved; uint8_t create_time_tenths;
    uint16_t create_time; uint16_t create_date;
    uint16_t access_date; uint16_t first_cluster_high;
    uint16_t modify_time; uint16_t modify_date;
    uint16_t first_cluster_low; uint32_t file_size;
} __attribute__((packed)) FAT16_DirectoryEntry;

void fat16_init();
void fat16_format(); 
void fat16_list_root();
int fat16_create_entry(const char* name, uint8_t attributes);
int fat16_delete_entry(const char* name);

// ✨✨✨ THE ULTIMATE MAGIC! READING AND WRITING FILE CONTENT! ✨✨✨
uint32_t fat16_read_file(const char* name, uint8_t* buffer);
uint32_t fat16_write_file(const char* name, const uint8_t* buffer, uint32_t length);

#endif