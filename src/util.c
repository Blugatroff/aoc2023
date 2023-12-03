#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "util.h"

bool is_space(char c) { return c == ' '; }
bool is_period(char c) { return c == '.'; }
bool is_digit(char c) { return isdigit(c); }

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

struct string_view drop_prefix(struct string_view str, struct string_view prefix) {
    if (!starts_with(str, prefix))
        return str;
    return (struct string_view) {
        .ptr = str.ptr + prefix.len,
        .len = str.len - prefix.len,
    };
}

struct string_view string_view_from_cstr(char* str) {
    return (struct string_view) {
        .ptr = str,
        .len = strlen(str),
    };
}

struct string_view chop_while(struct string_view* str, bool predicate(char)) {
    struct string_view orig = *str;
    while (str->len > 0 && predicate(str->ptr[0])) {
        str->len--;
        str->ptr++;
    }
    orig.len = str->ptr - orig.ptr;
    return orig;
}

bool string_view_eq(struct string_view a, struct string_view b) {
    return memcmp(a.ptr, b.ptr, a.len > b.len ? b.len : a.len) == 0;
}

bool string_view_eq_cstring(struct string_view str, char* cstr) {
    return string_view_eq(str, string_view_from_cstr(cstr));
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

#define PARSE_TYPE(type) \
int parse_##type(char* str, size_t str_len, type##_t* v) { \
    type##_t max = 0; \
    max = ~max; \
    *v = 0; \
    if (str_len == 0) return -1; \
    for (;;) { \
        char c = *str; \
        if (c >= '0' && c <= '9') { \
            type##_t digit = c - '0'; \
            if (digit != 0) { \
                type##_t limit = max - digit + 1; \
                if (*v >= limit) return -1; /* Would adding this digit overflow v? */ \
            } \
            *v += digit; \
        } else return -1; \
        str++; \
        str_len--; \
        if (str_len == 0) { \
            return 0; \
        } \
        type##_t shifted = *v * 10; \
        if (shifted / 10 != *v) /* did it overflow? */ \
            return -1; \
        *v = shifted; \
    } \
}

PARSE_TYPE(uint8)
PARSE_TYPE(uint16)
PARSE_TYPE(uint32)
PARSE_TYPE(uint64)

#define PARSE_SIGNED(type) \
int parse_##type(char* str, size_t str_len, type##_t* v) { \
    if (str_len == 0) return -1; \
    u##type##_t uv; \
    type##_t multiplier = 1; \
    if (*str == '-') { \
        str++; \
        str_len--; \
        multiplier = -1; \
    } \
    if (parse_u##type(str, str_len, &uv) < 0) return -1; \
    if ((uv & ((u##type##_t)1 << (sizeof(u##type##_t) * 8 - 1))) != 0 /* is the first bit a one? */ \
            && uv != ((u##type##_t)1 << (sizeof(u##type##_t) * 8 - 1))) /* and is it not exactly -type_MIN? (i.e. 128 in case of int8_t) */ \
        return -1; \
    *v = uv * multiplier; \
    return 0; \
}

PARSE_SIGNED(int64)


