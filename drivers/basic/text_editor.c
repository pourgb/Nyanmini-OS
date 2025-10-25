#include "text_editor.h"
#include "../fs/fat16.h"
#include "../commands/commands.h"
#include "../keyboard/keyboard.h"
#include "../screen/screen.h"
#include "utils/string.h"
#include <stdint.h>

#define EDITOR_BUFFER_SIZE 4096
static char editor_buffer[EDITOR_BUFFER_SIZE];
static char current_filename[32];
static int buffer_len = 0;
static int cursor_pos = 0;
static volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
static uint16_t attr = 0x0F << 8;

// Forward declare so the F-keys can use it
void text_editor_handle_key(int keycode);

void save_file() {
    // The buffer might not have a null terminator, so we can't use strlen!
    fat16_write_file(current_filename, (uint8_t*)editor_buffer, buffer_len);
    // The write function will tell us if it worked or not!
}

void exit_editor() {
    set_keyboard_handler(command_handle_key);
    do_clear();
    print_prompt();
}

void editor_redraw_screen() {
    for(int i=80;i<24*80;i++)vga[i]=attr|' ';
    int x=0,y=1;
    for(int i=0;i<buffer_len;i++){
        if(editor_buffer[i]=='\n'){y++;x=0;}
        else{vga[y*80+x]=attr|editor_buffer[i];x++;if(x>=80){y++;x=0;}}
    }
}

void text_editor_handle_key(int keycode) {
    switch(keycode){
        case KEY_F1: save_file(); exit_editor(); return;
        case KEY_F2: save_file(); return;
        case KEY_F3: exit_editor(); return;
        case KEY_LEFT: if(cursor_pos>0)cursor_pos--;break;
        case KEY_RIGHT: if(cursor_pos<buffer_len)cursor_pos++;break;
        case KEY_DOWN: { 
            int current_col = 0;
            int i = cursor_pos;
            while (i > 0 && editor_buffer[i-1] != '\n') {
                i--;
                current_col++;
            }

            int next_line_pos = cursor_pos;
            while (next_line_pos < buffer_len && editor_buffer[next_line_pos] != '\n') {
                next_line_pos++;
            }
            if (next_line_pos < buffer_len) { // found end of current line
                next_line_pos++; // move to start of next line
                cursor_pos = next_line_pos;
                int col_count = 0;
                while (cursor_pos < buffer_len && editor_buffer[cursor_pos] != '\n' && col_count < current_col) {
                    cursor_pos++;
                    col_count++;
                }
            }
            break;
        }
        case KEY_UP: {
            int current_col = 0;
            int i = cursor_pos;
            while (i > 0 && editor_buffer[i-1] != '\n') {
                i--;
                current_col++;
            }

            if (i > 0) { // if not on the first line
                int prev_line_end = i - 1;
                int prev_line_start = prev_line_end;
                while (prev_line_start > 0 && editor_buffer[prev_line_start-1] != '\n') {
                    prev_line_start--;
                }
                cursor_pos = prev_line_start;
                int col_count = 0;
                while (cursor_pos < prev_line_end && col_count < current_col) {
                    cursor_pos++;
                    col_count++;
                }
            }
            break;
        }
        default:
            if(keycode=='\b'){
                if(cursor_pos>0){
                    for(int i=cursor_pos;i<=buffer_len;i++) editor_buffer[i-1]=editor_buffer[i];
                    cursor_pos--; buffer_len--; editor_redraw_screen();
                }
            }else if(keycode > 0 && keycode < 0xFF && buffer_len<EDITOR_BUFFER_SIZE-1){
                for(int i=buffer_len;i>=cursor_pos;i--) editor_buffer[i+1]=editor_buffer[i];
                editor_buffer[cursor_pos]=(char)keycode;
                cursor_pos++; buffer_len++; editor_redraw_screen();
            }
    }
    int cursor_x=0, cursor_y=1;
    for(int i=0;i<cursor_pos;i++){
        if(editor_buffer[i]=='\n'){cursor_y++;cursor_x=0;}
        else{cursor_x++;if(cursor_x>=80){cursor_y++;cursor_x=0;}}
    }
    update_cursor(cursor_x,cursor_y);
}

void enter_text_editor(const char* filename) {
    do_clear();
    memset(editor_buffer, 0, EDITOR_BUFFER_SIZE);
    buffer_len = 0;
    cursor_pos = 0;
    strncpy(current_filename, filename, 32);

    uint32_t bytes_read = fat16_read_file(filename, (uint8_t*)editor_buffer);
    if (bytes_read > 0) {
        buffer_len = bytes_read;
    } else {
        fat16_create_entry(filename, ATTR_ARCHIVE);
    }
    
    int pos=0; const char*title="--- NYANMINI TEXT EDITOR (F1:Save&Exit F2:Save F3:Exit) ---";
    while(*title) vga[pos++] = attr | *title++;
    
    editor_redraw_screen();
    update_cursor(0,1);
    set_keyboard_handler(text_editor_handle_key);
}
