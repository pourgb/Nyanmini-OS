#include "basic_interpreter.h"
#include "utils/string.h"
#include "../screen/screen.h"
#include <stdint.h>
#include <stddef.h> // ✨ THE MAGIC DICTIONARY FOR 'NULL'! ✨

#define MAX_VARS 32
typedef struct { char name[16]; int value; } Variable;
static Variable variables[MAX_VARS];
static int var_count = 0;

static const char* next_line(const char* p) { while (*p && *p != '\n') p++; if (*p == '\n') p++; return p; }

// I taught this function how to clean names REALLY well! No more confusion!
static Variable* find_variable(const char* name, int len) {
    char clean_name[17];
    int clean_len = len > 16 ? 16 : len;
    strncpy(clean_name, name, clean_len);
    clean_name[clean_len] = '\0'; // The proper '\0' bow!
    
    // This loop trims any sleepy whitespace from the end!
    while (clean_len > 0 && (clean_name[clean_len-1] == ' ' || clean_name[clean_len-1] == '\t' || clean_name[clean_len-1] == '\r' || clean_name[clean_len-1] == '\n')) {
        clean_name[--clean_len] = '\0';
    }

    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, clean_name) == 0) {
            return &variables[i];
        }
    }
    return NULL;
}

static void set_variable(const char* name, int len, int value) {
    char clean_name[17];
    int clean_len = len > 16 ? 16 : len;
    strncpy(clean_name, name, clean_len);
    clean_name[clean_len] = '\0';
    while (clean_len > 0 && (clean_name[clean_len-1] == ' ' || clean_name[clean_len-1] == '\t' || clean_name[clean_len-1] == '\r' || clean_name[clean_len-1] == '\n')) {
        clean_name[--clean_len] = '\0';
    }
    Variable* v = find_variable(clean_name, clean_len);
    if (v) {
        v->value = value;
    } else if (var_count < MAX_VARS) {
        strncpy(variables[var_count].name, clean_name, 16);
        variables[var_count].value = value;
        var_count++;
    }
}

static int evaluate_expression(const char** expr_ptr); // We need to promise this exists!

// This function can now correctly tell the difference between a number and a toy's name!
static int parse_operand(const char** expr_ptr) {
    while (**expr_ptr == ' ') (*expr_ptr)++; // Skip sleepy spaces
    if (**expr_ptr >= '0' && **expr_ptr <= '9') {
        int v = atoi(*expr_ptr);
        while (**expr_ptr >= '0' && **expr_ptr <= '9') (*expr_ptr)++;
        return v;
    } else {
        const char* start = *expr_ptr;
        while ((**expr_ptr >= 'a' && **expr_ptr <= 'z') || (**expr_ptr >= 'A' && **expr_ptr <= 'Z')) (*expr_ptr)++;
        Variable* v = find_variable(start, *expr_ptr - start);
        return v ? v->value : 0; // If we can't find it, we'll just say 0 for now!
    }
}

// And this function orchestrates the whole thing! It's the brain of the math magic!
static int evaluate_expression(const char** expr_ptr) {
    int left = parse_operand(expr_ptr);
    while (**expr_ptr) {
        while (**expr_ptr == ' ') (*expr_ptr)++; // Skip more sleepy spaces
        char op = **expr_ptr;
        if (op != '+' && op != '-' && op != '*' && op != '/') break;
        (*expr_ptr)++;
        int right = parse_operand(expr_ptr);
        if (op == '+') left += right;
        else if (op == '-') left -= right;
        else if (op == '*') left *= right;
        else if (op == '/') { if (right != 0) left /= right; else return 0; }
    }
    return left;
}

void interpret_program(const char* code) {
    var_count = 0;
    const char* pc = code;
    while (*pc) {
        while (*pc == ' ' || *pc == '\t' || *pc == '\n') pc++;
        if (!*pc) break;
        if (*pc >= '0' && *pc <= '9') { while (*pc >= '0' && *pc <= '9') pc++; while (*pc == ' ') pc++; }
        if (starts_with(pc, "PRINT")) {
            pc += 5; while (*pc == ' ') pc++;
            if (*pc == '"') {
                pc++; const char* s = pc; while (*pc && *pc != '"') pc++;
                char o = *pc; *((char*)pc) = '\0'; screen_print_string(s); *((char*)pc) = o; if (*pc == '"') pc++;
            } else {
                const char* var_start = pc;
                while (*pc && *pc != '\n' && *pc != ' ' && *pc != '\t') pc++;
                Variable* v = find_variable(var_start, pc - var_start);
                if (v) {
                    char num_str[12]; itoa(v->value, num_str);
                    screen_print_string(num_str);
                } else { screen_print_string("Mrow? Unknown var!"); }
            }
            screen_print_string("\n");
        } 
        else if (starts_with(pc, "INT ")) {
            pc += 4; while (*pc == ' ') pc++;
            const char* name_start = pc;
            while (*pc && *pc != ' ' && *pc != '=') pc++;
            const char* name_end = pc;
            if (find_variable(name_start, name_end - name_start)) {
                screen_print_string("Mrow! You already have a toy named that! >.<\n"); return;
            }
            while (*pc == ' ' || *pc == '=') pc++;
            int val = evaluate_expression(&pc);
            set_variable(name_start, name_end - name_start, val);
        }
        else if (starts_with(pc, "VAR ")) {
            pc += 4; while (*pc == ' ') pc++;
            const char* name_start = pc;
            while (*pc && *pc != ' ' && *pc != '=') pc++;
            const char* name_end = pc;
            if (!find_variable(name_start, name_end - name_start)) {
                screen_print_string("Mrow? I don't have a toy with that name! o.O\n"); return;
            }
            while (*pc == ' ' || *pc == '=') pc++;
            int val = evaluate_expression(&pc);
            set_variable(name_start, name_end - name_start, val);
        }
        // ~*~*~*~*~ HELLO, PUNCHY NEW COMMANDS! ~*~*~*~*~
        else if (starts_with(pc, "ADD ") || starts_with(pc, "SUB ") || starts_with(pc, "MUL ") || starts_with(pc, "DIV ")) {
            char op_type = pc[0]; // 'A', 'S', 'M', or 'D'!
            pc += 4; // Skip "ADD ", etc.
            while(*pc == ' ') pc++;

            const char* name_start = pc;
            while(*pc && *pc != ' ') pc++;
            const char* name_end = pc;

            Variable* v = find_variable(name_start, name_end - name_start);
            if (!v) {
                screen_print_string("Mrow? I can't find that toy! o.O\n"); return;
            }

            while(*pc == ' ') pc++;
            int amount = evaluate_expression(&pc); // We can reuse our big brain function here! Ehehe!

            if (op_type == 'A') v->value += amount;
            else if (op_type == 'S') v->value -= amount;
            else if (op_type == 'M') v->value *= amount;
            else if (op_type == 'D') {
                if (amount == 0) {
                    screen_print_string("MROW! Don't divide by zero! >.<\n"); return;
                }
                v->value /= amount;
            }
        }
        else if (starts_with(pc, "GOTO")) {
            pc += 4; while (*pc == ' ') pc++; int target_line = atoi(pc);
            const char* scan = code; int found = 0;
            while (*scan) {
                if (atoi(scan) == target_line) { pc = scan; found = 1; break; }
                scan = next_line(scan);
            }
            if (!found) { screen_print_string("MROW! Bad GOTO! >.<\n"); return; }
            continue;
        } else if (starts_with(pc, "END")) {
            screen_print_string("Pwogram finished! Nya~!\n"); return;
        }
        pc = next_line(pc);
    }
}