#include <stdint.h>
#include "util.h"

uint64_t day1_part_one(struct string_view input);
uint64_t day1_part_two(struct string_view input);

struct day2_result {
    uint64_t part_one;
    uint64_t part_two;
};

struct day2_result day2(struct string_view input);

