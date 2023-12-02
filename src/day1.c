#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "days.h"

uint64_t day1_part_one(struct string_view input) {
    uint64_t sum = 0;
    while (input.len > 0) {
        struct string_view line = chop_until(&input, '\n');
        if (line.len == 0) continue;

        char first = -1;
        for (size_t i = 0; i < line.len; i++) {
            if (isdigit(line.ptr[i])) {
                first = line.ptr[i] - '0';
                break;
            }
        }
        if (first == -1) {
            fputs("Failed to find digit in calibration value: ", stderr);
            fwrite_all(line.ptr, line.len, stderr);
            fputs("\n", stderr);
            exit(1);
        }

        char last = -1;
        for (size_t i = line.len; i > 0; i--) {
            if (isdigit(line.ptr[i - 1])) {
                last = line.ptr[i - 1] - '0';
                break;
            }
        }
        sum += first * 10 + last;
    }
    return sum;
}

bool try_match(struct string_view str, uint8_t* output) {
    static char* zero_to_nine[] = { "", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
    char ch = str.ptr[0];
    if (isdigit(ch)) {
        *output = ch - '0';
        return true;
    }
    for (uint8_t i = 1; i < 10; i++) {
        char* n_str_ptr = zero_to_nine[i];
        struct string_view n_str = { .ptr = n_str_ptr, .len = strlen(n_str_ptr) };
        if (starts_with(str, n_str)) {
            *output = i;
            return true;
        }
    }
    return false;
}

uint64_t day1_part_two(struct string_view input) {
    static char* zero_to_nine[] = { "", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" };
    uint64_t sum = 0;
    while (input.len > 0) {
        struct string_view line = chop_until(&input, '\n');
        if (line.len == 0) continue;

        uint8_t first = -1;
        for (size_t i = 0;; i++) {
            if (i == line.len) goto failed;
            struct string_view shifted = { .ptr = line.ptr + i, .len = line.len - i };
            if (try_match(shifted, &first)) break;
        }
        uint8_t last = -1;
        for (size_t i = line.len - 1;; i--) {
            struct string_view shifted = { .ptr = line.ptr + i, .len = line.len - i };
            if (try_match(shifted, &last)) break;
            if (i == 0) goto failed;
        }
        sum += first * 10 + last;
        continue;
    failed:
        fputs("Failed to find digit in calibration value: ", stderr);
        fwrite_all(line.ptr, line.len, stderr);
        fputs("\n", stderr);
        exit(1);
    }
    return sum;
}
