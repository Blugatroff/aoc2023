#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"
#include "days.h"

struct game {
    uint64_t duration;
    uint64_t record;
};


uint8_t dec_len(uint64_t n) {
    uint8_t dec_len = 1;
    while (n /= 10) dec_len++;
    return dec_len;
}

uint64_t dekern(uint64_t* nums, size_t stride, size_t n_nums) {
    uint64_t res = 0;
    for (size_t i = 0; i < n_nums; i++) {
        uint64_t n = *(uint64_t*)((char*)nums + i * stride);
        size_t dl = dec_len(n);
        for (size_t j = 0; j < dl; j++)
            res *= 10;
        res += n;
    }
    return res;
}

inline uint32_t num_beating_games(struct game game) {
    uint64_t n = 0;
    for (uint64_t time_pressing = 1; time_pressing < game.duration; time_pressing++)
        n += time_pressing * (game.duration - time_pressing) > game.record;
    return n;
}

struct uint64_day_result day6(struct string_view input) {
    struct uint64_day_result result = {0};

    struct string_view first_line = chop_until(&input, '\n');
    if (first_line.len == 0)
        goto failed_parsing;
    first_line = drop_prefix(first_line, string_view_from_cstr("Time: "));
    struct game* games = calloc(first_line.len / 2, sizeof(*games));
    size_t n_games = 0;
    while (first_line.len > 0) {
        chop_while(&first_line, is_whitespace);
        struct string_view num_str = chop_while(&first_line, is_digit);
        if (parse_uint64(num_str.ptr, num_str.len, &games[n_games++].duration))
            goto failed_parsing;
    }

    struct string_view second_line = chop_until(&input, '\n');
    if (second_line.len == 0)
        goto failed_parsing;
    second_line = drop_prefix(second_line, string_view_from_cstr("Distance: "));
    size_t n_race_records = 0;
    while (second_line.len > 0) {
        chop_while(&second_line, is_whitespace);
        struct string_view num_str = chop_while(&second_line, is_digit);
        assert(n_race_records < n_games);
        if (parse_uint64(num_str.ptr, num_str.len, &games[n_race_records++].record))
            goto failed_parsing;
    }

    result.part_one = 1;
    for (size_t i = 0; i < n_games; i++)
        result.part_one *= num_beating_games(games[i]);

    uint64_t duration = dekern((uint64_t*)games, sizeof(*games), n_games);
    uint64_t record = dekern((uint64_t*)&games->record, sizeof(*games), n_games);

    result.part_two = num_beating_games((struct game) {
        .duration = duration,
        record = record
    });

    free(games);
    return result;

failed_parsing:
    fputs("Failed to parse\n", stderr);
    exit(1);
}

