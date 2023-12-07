#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef UTIL_H
#define UTIL_H

#define max(a, b) ((a) > (b) ? (a) : (b));
#define min(a, b) ((a) < (b) ? (a) : (b));

bool is_space(char c);
bool is_period(char c);
bool is_digit(char c);
bool is_whitespace(char c);

int read_whole_stdin(char** buf, size_t* len);

struct string_view {
    char* ptr;
    size_t len;
};

struct string_view chop_until(struct string_view* str, char delim);

bool starts_with(struct string_view str, struct string_view prefix);

struct string_view drop_prefix(struct string_view str, struct string_view prefix);

struct string_view string_view_from_cstr(char* str);

struct string_view chop_while(struct string_view* str, bool predicate(char));


bool string_view_eq(struct string_view a, struct string_view b);

bool string_view_eq_cstring(struct string_view str, char* cstr);

int fwrite_all(const char* buf, size_t len, FILE* file);

void exit_on_error(int error);

void* handle_alloc_failure(void* ptr);

int parse_uint8(char* str, size_t str_len, uint8_t* v);
int parse_int8(char* str, size_t str_len, int8_t* v);
int parse_uint16(char* str, size_t str_len, uint16_t* v);
int parse_int16(char* str, size_t str_len, int16_t* v);
int parse_uint32(char* str, size_t str_len, uint32_t* v);
int parse_int32(char* str, size_t str_len, int32_t* v);
int parse_uint64(char* str, size_t str_len, uint64_t* v);
int parse_int64(char* str, size_t str_len, int64_t* v);

#endif
