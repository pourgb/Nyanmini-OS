#include "basic_storage.h"
#include "utils/string.h"
#include "../screen/screen.h" // The correct phonebook!

// Shhh, it's a secret! This is our treasure chest!
static BasicProgram saved_programs[MAX_PROGRAMS];

void storage_init() {
    for (int i = 0; i < MAX_PROGRAMS; i++) {
        saved_programs[i].is_used = 0;
    }
}

int storage_save_program(const char* name, const char* code) {
    int update_slot = -1; 
    int empty_slot = -1;  

    for (int i = 0; i < MAX_PROGRAMS; i++) {
        if (saved_programs[i].is_used) {
            if (strcmp(saved_programs[i].name, name) == 0) {
                update_slot = i;
                break; 
            }
        } else if (empty_slot == -1) {
            empty_slot = i;
        }
    }

    if (update_slot != -1) {
        strncpy(saved_programs[update_slot].code, code, MAX_PROG_LEN);
        return update_slot;
    } else if (empty_slot != -1) {
        saved_programs[empty_slot].is_used = 1;
        strncpy(saved_programs[empty_slot].name, name, MAX_PROG_NAME);
        strncpy(saved_programs[empty_slot].code, code, MAX_PROG_LEN);
        return empty_slot;
    }
    return -1;
}

BasicProgram* storage_get_program(int index) {
    if (index >= 0 && index < MAX_PROGRAMS && saved_programs[index].is_used) {
        return &saved_programs[index];
    }
    return 0;
}

void storage_list_programs() {
    for (int i = 0; i < MAX_PROGRAMS; i++) {
        if (saved_programs[i].is_used) {
            char num[4];
            itoa(i, num);
            screen_print_string(num); // <-- Use the formal police name!
            screen_print_string(": "); // <-- Use the formal police name!
            screen_print_string(saved_programs[i].name); // <-- Use the formal police name!
            screen_print_string("\n"); // <-- Use the formal police name!
        }
    }
}

void storage_dump_state() {
    screen_newline(); // <-- Use the formal police name!
    screen_print_string("--- Storage State Dump ---\n");
    for (int i = 0; i < MAX_PROGRAMS; i++) {
        char num[4];
        itoa(i, num);
        screen_print_string("Slot ");
        screen_print_string(num);
        screen_print_string(": ");

        if (saved_programs[i].is_used) {
            screen_print_string("USED - Name: \"");
            screen_print_string(saved_programs[i].name);
            screen_print_string("\"\n");
        } else {
            screen_print_string("EMPTY\n");
        }
    }
    screen_print_string("--- End Dump ---\n");
}