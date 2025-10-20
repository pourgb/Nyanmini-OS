#ifndef STRING_H
#define STRING_H

int strlen(const char* str);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, int n);
void strncpy(char* dest, const char* src, int n);
void itoa(int n, char* buf);
int atoi(const char* str); // Nya~ Added atoi!
int starts_with(const char* str, const char* prefix); // Nya~ Added starts_with!
void memcpy(void* dest, const void* src, int n); // Our new, dumber copy machine!
int memcmp(const void* s1, const void* s2, int n);
void* memset(void* ptr, int value, int num);

#endif