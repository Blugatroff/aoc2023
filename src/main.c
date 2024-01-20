#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "util.h"
#include "days.h"

struct uint64_day_result (*days[])() = { day1, day2, day3, day4, day5, day6, day7, day8, day9, day10 };

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
    const size_t n_days = sizeof(days) / sizeof(*days);
    if (day > n_days || day == 0)
        goto unimplemented;
    struct uint64_day_result (*day_f)() = days[day - 1];
    if (day_f == NULL)
        goto unimplemented;

    struct string_view input;
    exit_on_error(read_whole_stdin(&input.ptr, &input.len));
    char* input_buf = input.ptr;

    struct uint64_day_result result = day_f(input);
    printf("part one: %lu\n", result.part_one);
    printf("part two: %lu\n", result.part_two);

    free(input_buf);
    return 0;
unimplemented:
    fprintf(stderr, "Day %hhu does not exist or is unimplemented\n", day);
    free(input_buf);
    return 1;
}

