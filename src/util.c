#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int read_whole_stdin(char** buf, size_t* len) {
    size_t buf_size = 128;
    *buf = handle_alloc_failure(malloc(buf_size));
    *len = 0;
    for (;;) {
        if (*len == buf_size) {
            *buf = realloc(*buf, buf_size *= 2);
        }
        size_t read = fread(*buf + *len, 1, buf_size - *len, stdin); 
        if (read == 0) {
            if (feof(stdin)) break;
            int error = ferror(stdin);
            if (error != 0) return error;
        }
        *len += read;
    }
    return 0;
}

struct string_view chop_until(struct string_view* str, char delim) {
    struct string_view chopped = { .ptr = str->ptr, .len = 0 };
    while (str->len > 0) {
        if (*str->ptr == delim) {
            str->ptr++;
            str->len--;
            return chopped;
        }
        chopped.len++;
        str->len--;
        str->ptr++;
    }
    return chopped;
}

bool starts_with(struct string_view str, struct string_view prefix) {
    if (prefix.len > str.len) return false;
    return 0 == memcmp(str.ptr, prefix.ptr, prefix.len);
}

int fwrite_all(char* buf, size_t len, FILE* file) {
    size_t res;
    while (len > 0 && (res = fwrite(buf, 1, len, file))) {
        buf += res;
        len -= res;
    }
    return ferror(file);
}

void exit_on_error(int error) {
    if (error) {
        fputs(strerror(error), stderr);
        fputs("\n", stderr);
        exit(1);
    }
}

void* handle_alloc_failure(void* ptr) {
    if (ptr == NULL) {
        fputs("Failed to allocate\n", stderr);
        exit(1);
    }
    return ptr;
}

