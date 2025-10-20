#include "commands.h"
#include "../basic/text_editor.h" // New editor!
#include "../basic/basic_interpreter.h"
#include "../fs/fat16.h"
#include "../keyboard/keyboard.h"
#include "../screen/screen.h"
#include "utils/string.h"
#include <stdint.h>

#define MAX_CMD_LEN 256
#define FILE_BUFFER_SIZE 4096
static char command_buffer[MAX_CMD_LEN];
static uint8_t file_content_buffer[FILE_BUFFER_SIZE];
static int cmd_len = 0;

void commands_init() { print_prompt(); }
void print_prompt() { screen_print_string("\n> "); }
void do_clear() { screen_clear(); }

void do_help() {
    screen_print_string("\n--- Nyanmini OS Help ---");
    screen_print_string("\n/help          - Shows this message");
    screen_print_string("\n/clear         - Clears the screen");
    screen_print_string("\n/ls            - Lists files and directories");
    screen_print_string("\n/md <name>     - Creates a directory");
    screen_print_string("\n/rd <name>     - Removes a directory");
    screen_print_string("\n/fmake <file>  - Creates a file");
    screen_print_string("\n/fdelete <file>- Deletes a file");
    screen_print_string("\n/edit <file>   - Opens a file in the text editor");
    screen_print_string("\n/grab <file>   - Prints the content of a file"); // ✨ My new command! ✨
    screen_print_string("\n/basicrun <file.bas> - Runs a BASIC program from a file");
}

void process_command() {
    command_buffer[cmd_len] = '\0';

    if (strcmp(command_buffer, "/help") == 0) { do_help(); }
    else if (strcmp(command_buffer, "/clear") == 0) { do_clear(); }
    else if (strcmp(command_buffer, "/ls") == 0) { fat16_list_root(); }
    else if (strcmp(command_buffer, "/format_hdc") == 0) { fat16_format(); }
    else if (strcmp(command_buffer, "/fatinit") == 0) { fat16_init(); }
    else if (strncmp(command_buffer, "/md ", 4) == 0) { fat16_create_entry(command_buffer + 4, ATTR_DIRECTORY); }
    else if (strncmp(command_buffer, "/rd ", 4) == 0) { fat16_delete_entry(command_buffer + 4); }
    else if (strncmp(command_buffer, "/fmake ", 7) == 0) { fat16_create_entry(command_buffer + 7, ATTR_ARCHIVE); }
    else if (strncmp(command_buffer, "/fdelete ", 9) == 0) { fat16_delete_entry(command_buffer + 9); }
    
    else if (strncmp(command_buffer, "/edit ", 6) == 0) {
        cmd_len = 0;
        enter_text_editor(command_buffer + 6);
        return;
    }
    // ✨✨✨ NYA-HA-HA! HERE IT IS! ✨✨✨
    else if (strncmp(command_buffer, "/grab ", 6) == 0) {
        const char* filename = command_buffer + 6;
        memset(file_content_buffer, 0, FILE_BUFFER_SIZE); // Clean our paws first!
        uint32_t bytes_read = fat16_read_file(filename, file_content_buffer);

        if (bytes_read > 0) {
            // We have to be suuuper sure it's a pwoper string for printing!
            // So we put a little '\0' at the end, just in case!
            if (bytes_read < FILE_BUFFER_SIZE) {
                file_content_buffer[bytes_read] = '\0';
            } else {
                file_content_buffer[FILE_BUFFER_SIZE - 1] = '\0';
            }
            screen_print_string("\n--- Content of ");
            screen_print_string(filename);
            screen_print_string(" ---\n");
            screen_print_string((const char*)file_content_buffer);
            screen_print_string("\n--- End of file ---\n");
        } else {
            screen_print_string("\nMrow! Couldn't read the file! Maybe it's empty or doesn't exist? o.O");
        }
    }
    else if (strncmp(command_buffer, "/basicrun ", 10) == 0) {
        const char* filename = command_buffer + 10;
        int len = strlen(filename);
        if (len < 4 || strcmp(filename + len - 4, ".bas") != 0) {
            screen_print_string("\nMrow! Must be a .bas file! >.<");
        } else {
            memset(file_content_buffer, 0, FILE_BUFFER_SIZE);
            uint32_t bytes_read = fat16_read_file(filename, file_content_buffer);
            if (bytes_read > 0) {
                screen_print_string("\nRunning '");
                screen_print_string(filename);
                screen_print_string("'...\n");
                interpret_program((const char*)file_content_buffer);
            } else {
                screen_print_string("\nMrow! Couldn't read the file!");
            }
        }
    }
    
    else if (cmd_len > 0) {
        screen_print_string("\nMrow? Unknown command!");
    }

    cmd_len = 0;
    print_prompt();
}

void command_handle_key(int keycode) {
    if (keycode == '\n') { process_command(); }
    else if (keycode == '\b') { if (cmd_len > 0) { cmd_len--; screen_backspace(); } }
    else if (keycode > 0 && keycode < 0xFF) { if (cmd_len < MAX_CMD_LEN - 1) { command_buffer[cmd_len++] = (char)keycode; screen_print_char((char)keycode); } }
}