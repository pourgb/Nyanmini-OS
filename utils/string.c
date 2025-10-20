#include "string.h"
#include <stdint.h>

int strlen(const char* str) {
    int len = 0;
    while (str && str[len]) len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, int n) {
    while (n > 0 && *s1 && (*s1 == *s2)) { s1++; s2++; n--; }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void strncpy(char* dest, const char* src, int n) {
    // We can even remove the debug prints now that we found the bug!
    if (!dest || !src) {
        return;
    }

    int i = 0;
    // This loop copies the source string, but makes sure not to go past the buffer limit!
    while (i < n - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    // This makes suwe the string is ALWAYS null-terminated! No padding needed!
    dest[i] = '\0';
}

void itoa(int n, char* buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    int i = 0;
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    buf[i] = '\0';
    for (int j = 0; j < i / 2; j++) {
        char temp = buf[j];
        buf[j] = buf[i - j - 1];
        buf[i - j - 1] = temp;
    }
}

int atoi(const char* str) {
    int result = 0;
    int sign = 1;
    while (*str == ' ') str++;
    if (*str == '-') { sign = -1; str++; }
    else if (*str == '+') { str++; }
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return sign * result;
}

int starts_with(const char* str, const char* prefix) {
    while (*prefix) {
        if (*str != *prefix) return 0;
        str++;
        prefix++;
    }
    return 1;
}

void memcpy(void* dest, const void* src, int n) {
    char* csrc = (char*)src;
    char* cdest = (char*)dest;
    for (int i = 0; i < n; i++) {
        cdest[i] = csrc[i];
    }
}

int memcmp(const void* s1, const void* s2, int n) {
    const unsigned char* p1 = s1;
    const unsigned char* p2 = s2;
    for (int i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }
    return 0; // They match! Nya~!
}

void* memset(void* ptr, int value, int num) {
    unsigned char* p = ptr;
    unsigned char c = (unsigned char)value;
    for (int i = 0; i < num; i++) {
        p[i] = c;
    }
    return ptr;
}