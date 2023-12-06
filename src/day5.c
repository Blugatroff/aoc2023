#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"
#include "days.h"

bool parse_uint32s(struct string_view str, uint32_t* values, size_t* n_values) {
    while (str.len > 0) {
        chop_while(&str, is_space);
        struct string_view num_str = chop_until(&str, ' ');
        if (parse_uint32(num_str.ptr, num_str.len, &values[*n_values]))
            return true;
        *n_values += 1;
    }
    return false;
}

size_t n_lines_until_empty_line(struct string_view str) {
    size_t n = 0;
    while (str.len > 0) {
        struct string_view line = chop_until(&str, '\n');
        if (line.len == 0) return n;
        n++;
    }
    return n;
}

struct mapping {
    uint32_t dst_start;
    uint32_t src_start;
    uint32_t length;
};

struct mapping* parse_maps(struct string_view* str, size_t* n_maps) {
    size_t maps_cap = n_lines_until_empty_line(*str);
    struct mapping* maps = calloc(maps_cap, sizeof(*maps));

    chop_until(str, '\n'); // map header
    while (str->len > 0) {
        struct string_view line = chop_until(str, '\n');
        chop_while(&line, is_whitespace);
        if (line.len == 0) break;
        size_t zero = 0;
        
        struct mapping* map = &maps[*n_maps];
        if (parse_uint32s(line, (uint32_t*)map, &zero)) {
            free(maps);
            return NULL;
        }
        *n_maps += 1;
    }
    return maps;
}

int32_t use_mapping(int32_t v, struct mapping* maps, size_t n_maps) {
    for (size_t k = 0; k < n_maps; k++) {
        struct mapping mapping = maps[k];
        if (mapping.src_start <= v && v - mapping.src_start < mapping.length) {
            return v + (mapping.dst_start - mapping.src_start);
            break;
        }
    }
    return v;
}
int32_t use_mapping_reverse(int32_t v, struct mapping* maps, size_t n_maps) {
    for (size_t k = 0; k < n_maps; k++) {
        struct mapping mapping = maps[k];
        if (mapping.dst_start <= v && v - mapping.dst_start < mapping.length) {
            return v + (mapping.src_start - mapping.dst_start);
            break;
        }
    }
    return v;
}

uint32_t lowest_seed_location(uint32_t* seeds, size_t n_seeds, struct mapping** maps, size_t* map_sizes, size_t n_maps) {
    uint32_t lowest_seed_location = UINT32_MAX;
    for (size_t i = 0; i < n_seeds; i++) {
        int32_t seed = seeds[i];
        for (size_t j = 0; j < n_maps; j++)
            seed = use_mapping(seed, maps[j], map_sizes[j]);
        lowest_seed_location = min(lowest_seed_location, seed);
    }
    return lowest_seed_location;
}

size_t max_points_of_interest(struct mapping** maps, size_t* map_sizes, size_t n_maps) {
    size_t max_points_of_interest = 0;
    for (size_t i = n_maps; i > 0; i--)
        max_points_of_interest += max_points_of_interest + map_sizes[i - 1] * 2;
    return max_points_of_interest;
}

void determine_points_of_interest(
        uint32_t* points_of_interest,
        size_t* n_points_of_interest,
        uint32_t* seeds,
        size_t n_seeds,
        struct mapping** maps,
        size_t* map_sizes,
        size_t n_maps
) {
    for (size_t i = n_maps; i > 0; i--) {
        size_t previous_n_points_of_interest = *n_points_of_interest;
        for (size_t j = 0; j < previous_n_points_of_interest; j++) {
            uint32_t new = use_mapping_reverse(points_of_interest[j], maps[i - 1], map_sizes[i - 1]);
            points_of_interest[(*n_points_of_interest)++] = new;
        }
        for (size_t j = 0; j < map_sizes[i - 1]; j++) {
            struct mapping map = maps[i - 1][j];
            points_of_interest[(*n_points_of_interest)++] = map.src_start;
            points_of_interest[(*n_points_of_interest)++] = map.src_start + map.length - 1;
        }
    }

    size_t filtered_n_points_of_interest = 0;
    for (size_t i = 0; i < *n_points_of_interest; i++) {
        uint32_t p = points_of_interest[i];
        for (size_t j = 0; j + 1 < n_seeds; j += 2) {
            uint32_t start = seeds[j];
            uint32_t len = seeds[j + 1];
            if (p >= start && p < start + len)
                points_of_interest[filtered_n_points_of_interest++] = p;
        }
    }
    *n_points_of_interest = filtered_n_points_of_interest;
}

static void failed_to_parse() {
    fputs("Failed to parse first line\n", stderr);
    exit(1);
}

struct uint64_day_result day5(struct string_view input) {
    struct uint64_day_result result = {0};

    struct string_view first_line = chop_until(&input, '\n');
    if (first_line.len == 0)
        failed_to_parse();

    first_line = drop_prefix(first_line, string_view_from_cstr("seeds: "));

    uint32_t seeds[first_line.len / 2];
    size_t n_seeds = 0;

    if (parse_uint32s(first_line, seeds, &n_seeds))
        failed_to_parse();

    while (input.len > 0) {
        if (*input.ptr != '\n') break;
        input.ptr++;
        input.len--;
    }
    const size_t n_maps = 7;
    struct mapping* maps[n_maps];
    size_t map_sizes[n_maps];
    memset(map_sizes, 0, sizeof(*map_sizes) * n_maps);

    for (size_t i = 0; i < n_maps; i++)
        maps[i] = parse_maps(&input, &map_sizes[i]);

    result.part_one = lowest_seed_location(seeds, n_seeds, maps, map_sizes, n_maps);
    
    uint32_t points_of_interest[max_points_of_interest(maps, map_sizes, n_maps)];
    size_t n_points_of_interest = 0;
    determine_points_of_interest(points_of_interest, &n_points_of_interest, seeds, n_seeds, maps, map_sizes, n_maps);
    result.part_two = lowest_seed_location(points_of_interest, n_points_of_interest, maps, map_sizes, n_maps);

    for (size_t i = 0; i < n_maps; i++)
        free(maps[i]);
    return result;
}

