#include "fat16.h"
#include "../ata/ata.h"
#include "../screen/screen.h" 
#include "utils/string.h"

#define HDC_DRIVE 1 
static uint8_t ata_buffer[512];
static uint8_t fat_buffer[512]; // A separate buffer for the FAT is safer!
static FAT16_BootSector boot_sector;
static uint32_t fat_start_sector;
static uint32_t root_dir_start_sector;
static uint32_t data_start_sector;
static uint32_t sectors_per_root_dir;
static uint32_t sectors_per_cluster;

// --- Helper Functions (These are all perfect and stable) ---
static void parse_fat_name(const char* in, char* out) { for(int i=0;i<11;i++){out[i]=' ';} int j=0; for(int i=0;in[i]!=0&&in[i]!='.'&&j<8;i++){out[j++]=(in[i]>='a'&&in[i]<='z')?in[i]-32:in[i];} const char* ext=0; for(int i=0;in[i]!=0;i++){if(in[i]=='.'){ext=&in[i+1];break;}} if(ext){j=8; for(int i=0;ext[i]!=0&&j<11;i++){out[j++]=(ext[i]>='a'&&ext[i]<='z')?ext[i]-32:ext[i];}} }
static int find_entry_by_name(const char* name, FAT16_DirectoryEntry* entry_out, uint32_t* sector_out, int* index_out) { char fn[11]; parse_fat_name(name,fn); for(uint32_t s=0;s<sectors_per_root_dir;s++){ uint32_t cs=root_dir_start_sector+s; ata_read_sector(HDC_DRIVE,cs,ata_buffer); FAT16_DirectoryEntry* e=(FAT16_DirectoryEntry*)ata_buffer; for(int i=0;i<16;i++){ if(e[i].filename[0]==0){return -1;} if((uint8_t)e[i].filename[0]==0xE5){continue;} if(memcmp(e[i].filename,fn,11)==0){ memcpy(entry_out, &e[i], 32); *sector_out=cs;*index_out=i;return 0;}}} return -1;}
void fat16_init() { ata_read_sector(HDC_DRIVE,0,ata_buffer); memcpy(&boot_sector,ata_buffer,sizeof(FAT16_BootSector)); if(boot_sector.bytes_per_sector==0){screen_print_string("\nDISK READ FAILED!");for(;;);} fat_start_sector=boot_sector.reserved_sectors; root_dir_start_sector=fat_start_sector+(boot_sector.num_fats*boot_sector.sectors_per_fat); sectors_per_root_dir=(boot_sector.root_dir_entries*32)/boot_sector.bytes_per_sector; data_start_sector=root_dir_start_sector+sectors_per_root_dir; sectors_per_cluster = boot_sector.sectors_per_cluster; screen_print_string("\nFAT16 map found! Nya~!"); }
int fat16_create_entry(const char* name, uint8_t attributes) { FAT16_DirectoryEntry entry; uint32_t fs; int fi; if(find_entry_by_name(name,&entry,&fs,&fi)==0){return -1;} char fn[11]; parse_fat_name(name,fn); for(uint32_t s=0;s<sectors_per_root_dir;s++){ uint32_t cs=root_dir_start_sector+s; ata_read_sector(HDC_DRIVE,cs,ata_buffer); FAT16_DirectoryEntry* e=(FAT16_DirectoryEntry*)ata_buffer; for(int i=0;i<16;i++){ if(e[i].filename[0]==0||(uint8_t)e[i].filename[0]==0xE5){ memcpy(e[i].filename,fn,11); e[i].attributes=attributes; memset(&e[i].reserved,0,sizeof(FAT16_DirectoryEntry)-11); e[i].first_cluster_low = 0; e[i].file_size = 0; ata_write_sector(HDC_DRIVE,cs,ata_buffer); ata_flush(); return 0;}}} screen_print_string("\nMrow! Root full!");return -1;}

void fat16_format() {
    screen_print_string("\nMrow! Formatting hdb...");
    FAT16_BootSector new_bs;
    memset(&new_bs, 0, sizeof(FAT16_BootSector));

    new_bs.jmp[0] = 0xEB; new_bs.jmp[1] = 0x3C; new_bs.jmp[2] = 0x90;
    memcpy(new_bs.oem, "NYANMINI", 8);
    new_bs.bytes_per_sector = 512;
    new_bs.sectors_per_cluster = 64;
    new_bs.reserved_sectors = 32;
    new_bs.num_fats = 2;
    new_bs.root_dir_entries = 512;
    new_bs.media_descriptor = 0xF8;
    new_bs.sectors_per_fat = 128;
    new_bs.sectors_per_track = 32;
    new_bs.num_heads = 64;
    new_bs.total_sectors_long = 2097152;
    new_bs.drive_num = 0x80;
    new_bs.boot_sig = 0x29;
    new_bs.volume_id = 0x12345678;
    memcpy(new_bs.volume_label, "NYANDRIVE", 11);
    memcpy(new_bs.fs_type, "FAT16   ", 8);
    
    uint8_t* bs_ptr = (uint8_t*)&new_bs;
    bs_ptr[510] = 0x55;
    bs_ptr[511] = 0xAA;

    ata_write_sector(HDC_DRIVE, 0, (uint8_t*)&new_bs);

    uint8_t empty_sector[512];
    memset(empty_sector, 0, 512);

    uint32_t fat_start = new_bs.reserved_sectors;
    uint32_t total_fat_sectors = new_bs.num_fats * new_bs.sectors_per_fat;
    for(uint32_t i = 0; i < total_fat_sectors; i++) {
        ata_write_sector(HDC_DRIVE, fat_start + i, empty_sector);
    }
    
    uint32_t root_start = fat_start + total_fat_sectors;
    uint32_t root_sectors = (new_bs.root_dir_entries * 32) / new_bs.bytes_per_sector;
    for(uint32_t i = 0; i < root_sectors; i++) {
        ata_write_sector(HDC_DRIVE, root_start + i, empty_sector);
    }
    
    ata_flush();
    screen_print_string("\nDone! Nya~!");
}

int fat16_delete_entry(const char* name) {
    FAT16_DirectoryEntry entry; uint32_t sector; int index;
    if (find_entry_by_name(name, &entry, &sector, &index) != 0) {
        screen_print_string("\nMrow! Can't find that to delete!"); return -1;
    }
    ata_read_sector(HDC_DRIVE, sector, ata_buffer);
    FAT16_DirectoryEntry* entries = (FAT16_DirectoryEntry*)ata_buffer;
    entries[index].filename[0] = 0xE5;
    ata_write_sector(HDC_DRIVE, sector, ata_buffer);
    ata_flush();
    return 0;
}

void fat16_list_root() { 
    screen_print_string("\nListing files:\n"); 
    for(uint32_t s=0; s<sectors_per_root_dir; s++){ 
        ata_read_sector(HDC_DRIVE,root_dir_start_sector+s,ata_buffer); 
        FAT16_DirectoryEntry* e=(FAT16_DirectoryEntry*)ata_buffer; 
        for(int i=0;i<16;i++){ 
            if(e[i].filename[0]==0) {screen_newline();return;} 
            // ✨✨✨ THE STRONGER SPELL! ✨✨✨
            if((uint8_t)e[i].filename[0] == 0xE5){ continue; } 
            if(e[i].attributes & ATTR_VOLUME_ID){continue;} 
            char n[9],x[4]; memcpy(n,e[i].filename,8); memcpy(x,e[i].ext,3); 
            n[8]=0; x[3]=0; 
            for(int j=7;j>=0&&n[j]==' ';j--)n[j]=0; for(int j=2;j>=0&&x[j]==' ';j--)x[j]=0; 
            if(e[i].attributes&ATTR_DIRECTORY){screen_print_string("[");screen_print_string(n);screen_print_string("]");} 
            else {screen_print_string(n); if(x[0]!=0){screen_print_char('.');screen_print_string(x);}} 
            screen_newline();
        }
    } 
    screen_newline(); 
}

static uint16_t fat16_get_next_cluster(uint16_t cluster) {
    uint32_t offset = cluster * 2;
    uint32_t sector = fat_start_sector + (offset / 512);
    uint32_t entry_offset = offset % 512;
    ata_read_sector(HDC_DRIVE, sector, fat_buffer);
    return *(uint16_t*)&fat_buffer[entry_offset];
}

uint32_t fat16_read_file(const char* name, uint8_t* buffer) {
    FAT16_DirectoryEntry entry; uint32_t sector; int index;
    if (find_entry_by_name(name, &entry, &sector, &index) != 0) { return 0; }
    if (entry.attributes & ATTR_DIRECTORY) { return 0; }

    uint32_t bytes_read = 0;
    uint16_t cluster = entry.first_cluster_low;
    while (cluster < 0xFFF8 && cluster != 0) {
        uint32_t lba = data_start_sector + (cluster - 2) * sectors_per_cluster;
        for (uint32_t i = 0; i < sectors_per_cluster; i++) {
            if (bytes_read >= entry.file_size) break;
            ata_read_sector(HDC_DRIVE, lba + i, buffer + bytes_read);
            bytes_read += 512;
        }
        cluster = fat16_get_next_cluster(cluster);
    }
    return entry.file_size;
}

static uint16_t fat16_find_free_cluster() {
    for (uint32_t sector = 0; sector < boot_sector.sectors_per_fat; sector++) {
        ata_read_sector(HDC_DRIVE, fat_start_sector + sector, fat_buffer);
        uint32_t start_index = (sector == 0) ? 2 : 0; 
        for (uint32_t i = start_index; i < 256; i++) {
            if ( ((uint16_t*)fat_buffer)[i] == 0x0000 ) {
                return (sector * 256) + i;
            }
        }
    }
    return 0;
}


static void fat16_set_fat_entry(uint16_t cluster, uint16_t value) {
    uint32_t offset = cluster * 2;
    uint32_t sector = fat_start_sector + (offset / 512);
    uint32_t entry_offset = offset % 512;
    ata_read_sector(HDC_DRIVE, sector, fat_buffer);
    ((uint16_t*)fat_buffer)[entry_offset / 2] = value;
    ata_write_sector(HDC_DRIVE, sector, fat_buffer);
}

uint32_t fat16_write_file(const char* name, const uint8_t* buffer, uint32_t length) {
    FAT16_DirectoryEntry entry; uint32_t sector; int index;
    if (find_entry_by_name(name, &entry, &sector, &index) != 0) {
        screen_print_string("\nMrow! File not found to write to!"); return 0;
    }

    uint16_t start_cluster = fat16_find_free_cluster();
    if (start_cluster == 0) {
        screen_print_string("\nMrow! Disk is full!"); return 0;
    }

    uint16_t current_cluster = start_cluster;
    uint32_t bytes_written = 0;
    uint32_t cluster_size = sectors_per_cluster * 512;

    while (bytes_written < length) {
        uint32_t lba = data_start_sector + (current_cluster - 2) * sectors_per_cluster;
        uint32_t num_sectors_to_write = 0;
        if(length - bytes_written >= cluster_size) {
            num_sectors_to_write = sectors_per_cluster;
        } else {
            num_sectors_to_write = (length - bytes_written + 511) / 512;
        }

        for (uint32_t i = 0; i < num_sectors_to_write; i++) {
            ata_write_sector(HDC_DRIVE, lba + i, (uint8_t*)buffer + bytes_written);
            bytes_written += 512;
        }

        if (bytes_written >= length) {
            fat16_set_fat_entry(current_cluster, 0xFFFF);
        } else {
            uint16_t next_cluster = fat16_find_free_cluster();
            if (next_cluster == 0) { break; }
            fat16_set_fat_entry(current_cluster, next_cluster);
            current_cluster = next_cluster;
        }
    }
    
    ata_read_sector(HDC_DRIVE, sector, ata_buffer);
    FAT16_DirectoryEntry* entries = (FAT16_DirectoryEntry*)ata_buffer;
    entries[index].first_cluster_low = start_cluster;
    entries[index].file_size = length;
    ata_write_sector(HDC_DRIVE, sector, ata_buffer);

    ata_flush();
    return length;
}