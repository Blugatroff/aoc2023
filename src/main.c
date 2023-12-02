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
        printf("part one: %lu\n", day1_part_one(input));
        printf("part two: %lu\n", day1_part_two(input));
    } else if (day == 2) {
        puts("TODO");
    }

    free(input_buf);
    return 0;
}

