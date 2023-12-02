#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

int read_whole_stdin(char** buf, size_t* len);

struct string_view {
    char* ptr;
    size_t len;
};

struct string_view chop_until(struct string_view* str, char delim);

bool starts_with(struct string_view str, struct string_view prefix);

int fwrite_all(char* buf, size_t len, FILE* file);

void exit_on_error(int error);

void* handle_alloc_failure(void* ptr);

int parse_uint8(char* str, size_t str_len, uint8_t* v);
int parse_int8(char* str, size_t str_len, int8_t* v);
int parse_uint64(char* str, size_t str_len, uint64_t* v);
int parse_int64(char* str, size_t str_len, int64_t* v);

