#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include "days.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        puts("Usage: ./aoc <day>");
        return 1;
    }
    uint8_t day;
    if (parse_uint8(argv[1], strlen(argv[1]), &day)) {
        fputs("Failed to parse day!\n", stderr);
        return 1;
    }
    struct string_view input;
    exit_on_error(read_whole_stdin(&input.ptr, &input.len));
    char* input_buf = input.ptr;

    if (day == 1) {
        struct uint64_day_result result = day1(input);
        printf("part one: %lu\n", result.part_one);
        printf("part two: %lu\n", result.part_two);
    } else if (day == 2) {
        struct uint64_day_result result = day2(input);
        printf("part one: %lu\n", result.part_one);
        printf("part two: %lu\n", result.part_two);
    } else if (day == 3) {
        struct uint64_day_result result = day3(input);
        printf("part one: %lu\n", result.part_one);
        printf("part two: %lu\n", result.part_two);
    } else if (day == 4) {
        struct uint64_day_result result = day4(input);
        printf("part one: %lu\n", result.part_one);
        printf("part two: %lu\n", result.part_two);
    } else {
        fprintf(stderr, "Day %hhu does not exist or is unimplemented\n", day);
    }

    free(input_buf);
    return 0;
}

