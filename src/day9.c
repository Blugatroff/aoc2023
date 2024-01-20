#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "days.h"

int32_t extrapolate(int32_t* nums, size_t n_nums) {
    bool all_zero = true;
    for (size_t i = 0; all_zero && i < n_nums; i++)
        all_zero &= nums[i] == 0;
    if (all_zero) return 0;

    int32_t last = nums[n_nums - 1];
    for (size_t i = 0; i + 1 < n_nums; i++)
        nums[i] = nums[i + 1] - nums[i];

    return last + extrapolate(nums, n_nums - 1);
}

int32_t extrapolate_backwards(int32_t* nums, size_t n_nums) {
    bool all_zero = true;
    for (size_t i = 0; all_zero && i < n_nums; i++)
        all_zero &= nums[i] == 0;
    if (all_zero) return 0;

    int32_t first = nums[0];
    for (size_t i = 0; i + 1 < n_nums; i++)
        nums[i] = nums[i + 1] - nums[i];

    return first - extrapolate_backwards(nums, n_nums - 1);
}

struct uint64_day_result day9(struct string_view input) {
    struct uint64_day_result result = {0};

    while (input.len > 0) {
        struct string_view line = chop_until(&input, '\n');
        if (line.len == 0) continue;

        int32_t nums[sizeof(int32_t) * line.len / 2];
        size_t n_nums = 0;

        while (line.len > 0) {
            struct string_view n_str = chop_until(&line, ' ');
            if (n_str.len == 0) n_str = line;

            if (parse_int32(n_str.ptr, n_str.len, &nums[n_nums++])) {
                fprintf(stderr, "Failed to parse line: ");
                fwrite_all(line.ptr, line.len, stderr);
                fputs("\n", stderr);
            }
        }
        int32_t nums_copy[n_nums];
        memcpy(nums_copy, nums, n_nums * sizeof(int32_t));

        result.part_one += extrapolate(nums, n_nums);
        result.part_two += extrapolate_backwards(nums_copy, n_nums);
    }

    return result;
}

