#include "basic_interpreter.h"
#include "utils/string.h"
#include "../screen/screen.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> // ✨ THE MAGIC DICTIONARY FOR 'NULL'! ✨

#define MAX_VARS 32
#define MAX_STRING_SPACE 1024

typedef enum {
    TYPE_INT,
    TYPE_CHAR,
    TYPE_STRING
} VarType;

typedef struct {
    char name[16];
    VarType type;
    union {
        int int_val;
        char char_val;
        char* str_val;
    };
} Variable;

static Variable variables[MAX_VARS];
static int var_count = 0;
static char string_space[MAX_STRING_SPACE];
static int string_space_ptr = 0;

static const char* next_line(const char* p) { while (*p && *p != '\n') p++; if (*p == '\n') p++; return p; }

static Variable* find_variable(const char* name, int len) {
    char clean_name[17];
    int clean_len = len > 16 ? 16 : len;
    strncpy(clean_name, name, clean_len);
    clean_name[clean_len] = '\0';
    
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

static void set_int_variable(const char* name, int len, int value) {
    Variable* v = find_variable(name, len);
    if (v) {
        v->type = TYPE_INT;
        v->int_val = value;
    } else if (var_count < MAX_VARS) {
        strncpy(variables[var_count].name, name, len);
        variables[var_count].name[len] = '\0';
        variables[var_count].type = TYPE_INT;
        variables[var_count].int_val = value;
        var_count++;
    }
}

static void set_char_variable(const char* name, int len, char value) {
    Variable* v = find_variable(name, len);
    if (v) {
        v->type = TYPE_CHAR;
        v->char_val = value;
    } else if (var_count < MAX_VARS) {
        strncpy(variables[var_count].name, name, len);
        variables[var_count].name[len] = '\0';
        variables[var_count].type = TYPE_CHAR;
        variables[var_count].char_val = value;
        var_count++;
    }
}

static void set_string_variable(const char* name, int len, const char* value) {
    int str_len = strlen(value);
    if (string_space_ptr + str_len + 1 >= MAX_STRING_SPACE) {
        screen_print_string("MROW! Out of string space! >.<");
        return;
    }
    char* str_ptr = &string_space[string_space_ptr];
    strncpy(str_ptr, value, str_len + 1);
    
    string_space_ptr += str_len + 1;

    Variable* v = find_variable(name, len);
    if (v) {
        v->type = TYPE_STRING;
        v->str_val = str_ptr;
    } else if (var_count < MAX_VARS) {
        strncpy(variables[var_count].name, name, len);
        variables[var_count].name[len] = '\0';
        variables[var_count].type = TYPE_STRING;
        variables[var_count].str_val = str_ptr;
        var_count++;
    }
}

static int evaluate_expression(const char** expr_ptr); 

static int parse_operand(const char** expr_ptr) {
    while (**expr_ptr == ' ') (*expr_ptr)++;
    if (**expr_ptr >= '0' && **expr_ptr <= '9') {
        int v = atoi(*expr_ptr);
        while (**expr_ptr >= '0' && **expr_ptr <= '9') (*expr_ptr)++;
        return v;
    } else {
        const char* start = *expr_ptr;
        while ((**expr_ptr >= 'a' && **expr_ptr <= 'z') || (**expr_ptr >= 'A' && **expr_ptr <= 'Z')) (*expr_ptr)++;
        Variable* v = find_variable(start, *expr_ptr - start);
        if (v && v->type == TYPE_INT) {
            return v->int_val;
        }
        return 0;
    }
}

static int evaluate_expression(const char** expr_ptr) {
    return parse_operand(expr_ptr);
}

typedef enum { OP_EQ, OP_GT, OP_LT, OP_GE, OP_LE, OP_INVALID } Operator;

static Operator parse_operator(const char** pc) {
    while (**pc == ' ') (*pc)++;
    if (**pc == '=' && *(*pc + 1) == '=') { *pc += 2; return OP_EQ; }
    if (**pc == '>') { 
        if (*(*pc + 1) == '=') { *pc += 2; return OP_GE; }
        (*pc)++; return OP_GT; 
    }
    if (**pc == '<') { 
        if (*(*pc + 1) == '=') { *pc += 2; return OP_LE; }
        (*pc)++; return OP_LT; 
    }
    return OP_INVALID;
}

void interpret_program(const char* code) {
    var_count = 0;
    string_space_ptr = 0;
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
                    if (v->type == TYPE_INT) {
                        char num_str[12]; itoa(v->int_val, num_str);
                        screen_print_string(num_str);
                    } else if (v->type == TYPE_CHAR) {
                        screen_print_char(v->char_val);
                    } else if (v->type == TYPE_STRING) {
                        screen_print_string(v->str_val);
                    }
                } else { screen_print_string("Mrow? Unknown var!\n"); }
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
            set_int_variable(name_start, name_end - name_start, val);
        }
        else if (starts_with(pc, "CHAR ")) {
            pc += 5; while (*pc == ' ') pc++;
            const char* name_start = pc;
            while (*pc && *pc != ' ' && *pc != '=') pc++;
            const char* name_end = pc;
            if (find_variable(name_start, name_end - name_start)) {
                screen_print_string("Mrow! You already have a toy named that! >.<\n"); return;
            }
            while (*pc == ' ' || *pc == '=') pc++;
            if (*pc == '\'') {
                pc++;
                char val = *pc;
                pc++;
                if (*pc == '\'') pc++;
                set_char_variable(name_start, name_end - name_start, val);
            } else {
                screen_print_string("MROW! Invalid char literal! >.<");
            }
        }
        else if (starts_with(pc, "STRING ")) {
            pc += 7; while (*pc == ' ') pc++;
            const char* name_start = pc;
            while (*pc && *pc != ' ' && *pc != '=') pc++;
            const char* name_end = pc;
            if (find_variable(name_start, name_end - name_start)) {
                screen_print_string("Mrow! You already have a toy named that! >.<\n"); return;
            }
            while (*pc == ' ' || *pc == '=') pc++;
            if (*pc == '"') {
                pc++;
                const char* str_start = pc;
                while (*pc && *pc != '"') pc++;
                char temp = *pc;
                *((char*)pc) = '\0';
                set_string_variable(name_start, name_end - name_start, str_start);
                *((char*)pc) = temp;
                if (*pc == '"') pc++;
            } else {
                screen_print_string("MROW! Invalid string literal! >.<");
            }
        }
        else if (starts_with(pc, "VAR ")) {
            pc += 4; while (*pc == ' ') pc++;
            const char* name_start = pc;
            while (*pc && *pc != ' ' && *pc != '=') pc++;
            const char* name_end = pc;
            Variable* v = find_variable(name_start, name_end - name_start);
            if (!v) {
                screen_print_string("Mrow? I don't have a toy with that name! o.O\n"); return;
            }
            while (*pc == ' ' || *pc == '=') pc++;
            if (v->type == TYPE_INT) {
                v->int_val = evaluate_expression(&pc);
            } else {
                screen_print_string("MROW! Can only assign to INT variables for now! >.<");
            }
        }
        else if (starts_with(pc, "IF ")) {
            pc += 3; // Skip "IF "
            
            // --- PARSE LEFT OPERAND ---
            const char* left_str_val = NULL;
            int left_str_len = 0;
            int left_int_val = 0;
            VarType left_type;

            while (*pc == ' ') pc++;
            if (*pc == '"') {
                pc++;
                left_str_val = pc;
                while (*pc && *pc != '"') pc++;
                left_str_len = pc - left_str_val;
                if (*pc == '"') pc++;
                left_type = TYPE_STRING;
            } else {
                const char* expr_start = pc;
                const char* var_start = pc;
                while (*pc && *pc != ' ' && *pc != '=' && *pc != '>' && *pc != '<') pc++;
                Variable* v = find_variable(var_start, pc - var_start);
                if (v && v->type == TYPE_STRING) {
                    left_str_val = v->str_val;
                    left_str_len = strlen(left_str_val);
                    left_type = TYPE_STRING;
                } else {
                    pc = expr_start;
                    left_int_val = evaluate_expression(&pc);
                    left_type = TYPE_INT;
                }
            }

            // --- PARSE OPERATOR ---
            Operator op = parse_operator(&pc);

            // --- PARSE RIGHT OPERAND ---
            const char* right_str_val = NULL;
            int right_str_len = 0;
            int right_int_val = 0;
            VarType right_type;

            while (*pc == ' ') pc++;
            if (*pc == '"') {
                pc++;
                right_str_val = pc;
                while (*pc && *pc != '"') pc++;
                right_str_len = pc - right_str_val;
                if (*pc == '"') pc++;
                right_type = TYPE_STRING;
            } else {
                const char* expr_start = pc;
                const char* var_start = pc;
                while (*pc && *pc != ' ' && *pc != '\n' && !starts_with(pc, "THEN:")) pc++;
                Variable* v = find_variable(var_start, pc - var_start);
                if (v && v->type == TYPE_STRING) {
                    right_str_val = v->str_val;
                    right_str_len = strlen(right_str_val);
                    right_type = TYPE_STRING;
                } else {
                    pc = expr_start;
                    right_int_val = evaluate_expression(&pc);
                    right_type = TYPE_INT;
                }
            }

            // --- CHECK FOR THEN: ---
            while (*pc == ' ') pc++;
            if (!starts_with(pc, "THEN:")) {
                screen_print_string("MROW! Missing THEN: in IF statement! >.<");
                pc = next_line(pc);
                continue;
            }
            pc += 5; // Skip "THEN:"
            while (*pc == ' ') pc++;

            // --- EVALUATE CONDITION ---
            bool condition_met = false;
            if (left_type == TYPE_STRING || right_type == TYPE_STRING) {
                if (left_type != TYPE_STRING || right_type != TYPE_STRING) {
                    screen_print_string("MROW! Cannot compare string to number! >.<");
                } else if (op != OP_EQ) {
                    screen_print_string("MROW! Only '==' is supported for string comparison! >.<");
                } else {
                    condition_met = (left_str_len == right_str_len) && (strncmp(left_str_val, right_str_val, left_str_len) == 0);
                }
            } else {
                // Numeric comparison
                switch (op) {
                    case OP_EQ: condition_met = (left_int_val == right_int_val); break;
                    case OP_GT: condition_met = (left_int_val > right_int_val); break;
                    case OP_LT: condition_met = (left_int_val < right_int_val); break;
                    case OP_GE: condition_met = (left_int_val >= right_int_val); break;
                    case OP_LE: condition_met = (left_int_val <= right_int_val); break;
                    default:
                        screen_print_string("MROW! Invalid operator in IF statement! >.<");
                }
            }

            if (!condition_met) {
                pc = next_line(pc);
            }
            continue;
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
            if (!v || v->type != TYPE_INT) {
                screen_print_string("Mrow? I can't find that toy or it's not an INT! o.O\n"); return;
            }

            while(*pc == ' ') pc++;
            int amount = evaluate_expression(&pc); 

            if (op_type == 'A') v->int_val += amount;
            else if (op_type == 'S') v->int_val -= amount;
            else if (op_type == 'M') v->int_val *= amount;
            else if (op_type == 'D') {
                if (amount == 0) {
                    screen_print_string("MROW! Don't divide by zero! >.<"); return;
                }
                v->int_val /= amount;
            }
        }
        else if (starts_with(pc, "GOTO")) {
            pc += 4; while (*pc == ' ') pc++; int target_line = atoi(pc);
            const char* scan = code; int found = 0;
            while (*scan) {
                if (atoi(scan) == target_line) { pc = scan; found = 1; break; }
                scan = next_line(scan);
            }
            if (!found) { screen_print_string("MROW! Bad GOTO! >.<"); return; }
            continue;
        } else if (starts_with(pc, "END")) {
            screen_print_string("Pwogram finished! Nya~!"); return;
        }
        pc = next_line(pc);
    }
}