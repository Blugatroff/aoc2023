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
    char* name;
};

struct mapping* parse_maps(struct string_view* str, char* name, size_t* n_maps) {
    size_t maps_cap = n_lines_until_empty_line(*str);
    struct mapping* maps = calloc(maps_cap, sizeof(*maps));

    struct string_view seed_to_soil_line = chop_until(str, '\n');
    seed_to_soil_line = drop_prefix(seed_to_soil_line, string_view_from_cstr(name));
    seed_to_soil_line = drop_prefix(seed_to_soil_line, string_view_from_cstr(": "));

    while (str->len > 0) {
        struct string_view line = chop_until(str, '\n');
        chop_while(&line, is_whitespace);
        if (line.len == 0) break;
        size_t zero = 0;
        
        struct mapping* map = &maps[*n_maps];
        map->name = name;
        if (parse_uint32s(line, (uint32_t*)map, &zero)) {
            free(maps);
            return NULL;
        }
        *n_maps += 1;
    }
    return maps;
}


uint32_t lowest_seed_location(uint32_t* seeds, size_t n_seeds, struct mapping** maps, size_t* map_sizes, size_t n_maps) {
    uint32_t lowest_seed_location = UINT32_MAX;
    for (size_t i = 0; i < n_seeds; i++) {
        int32_t seed = seeds[i];
        for (size_t j = 0; j < n_maps; j++) {
            struct mapping* map = maps[j];
            const size_t map_size = map_sizes[j];
            for (size_t k = 0; k < map_size; k++) {
                struct mapping mapping = map[k];
                if (mapping.src_start <= seed && seed - mapping.src_start < mapping.length) {
                    seed += mapping.dst_start - mapping.src_start;
                    break;
                }
            }
        }
        lowest_seed_location = min(lowest_seed_location, seed);
    }
    return lowest_seed_location;
}

int compare_uint32(const void* a, const void* b) {
    return *(uint32_t*)a - *(uint32_t*)b;
}

struct uint64_day_result day5(struct string_view input) {
    struct uint64_day_result result = {0};

    struct string_view first_line = chop_until(&input, '\n');
    if (first_line.len == 0)
        goto failed_parsing_first_line;

    first_line = drop_prefix(first_line, string_view_from_cstr("seeds: "));

    uint32_t* seeds = calloc(first_line.len / 2, sizeof(*seeds));
    size_t n_seeds = 0;

    if (parse_uint32s(first_line, seeds, &n_seeds))
        goto failed_parsing_first_line;

    while (input.len > 0) {
        if (*input.ptr != '\n') break;
        input.ptr++;
        input.len--;
    }

    size_t n_seed_to_soil_map = 0;
    struct mapping* seed_to_soil_map = parse_maps(&input, "seed-to-soil", &n_seed_to_soil_map);

    size_t n_soil_to_fertilizer_map = 0;
    struct mapping* soil_to_fertilizer_map = parse_maps(&input, "soil-to-fertilizer", &n_soil_to_fertilizer_map);

    size_t n_fertilizer_to_water_map = 0;
    struct mapping* fertilizer_to_water_map = parse_maps(&input, "fertilizer-to-water", &n_fertilizer_to_water_map);

    size_t n_water_to_light_map = 0;
    struct mapping* water_to_light_map = parse_maps(&input, "water-to-light", &n_water_to_light_map);

    size_t n_light_to_temperature_map = 0;
    struct mapping* light_to_temperature_map = parse_maps(&input, "light-to-temperature", &n_light_to_temperature_map);

    size_t n_temperature_to_humidity_map = 0;
    struct mapping* temperature_to_humidity_map = parse_maps(&input, "temperature_to_humidity_maps", &n_temperature_to_humidity_map);
    
    size_t n_humidity_to_location_map = 0;
    struct mapping* humidity_to_location_map = parse_maps(&input, "humidity-to-location", &n_humidity_to_location_map);

    struct mapping* maps[] = { seed_to_soil_map, soil_to_fertilizer_map, fertilizer_to_water_map, water_to_light_map, light_to_temperature_map, temperature_to_humidity_map, humidity_to_location_map };
    size_t map_sizes[] = { n_seed_to_soil_map, n_soil_to_fertilizer_map, n_fertilizer_to_water_map, n_water_to_light_map, n_light_to_temperature_map, n_temperature_to_humidity_map, n_humidity_to_location_map };
    const size_t n_maps = sizeof(maps) / sizeof(void*);

    result.part_one = lowest_seed_location(seeds, n_seeds, maps, map_sizes, n_maps);
 
    free(humidity_to_location_map);
    free(temperature_to_humidity_map);
    free(light_to_temperature_map);
    free(water_to_light_map);
    free(fertilizer_to_water_map);
    free(soil_to_fertilizer_map);
    free(seed_to_soil_map);
    free(seeds);
    return result;

failed_parsing_first_line:
    fputs("Failed to parse first line\n", stderr);
    exit(1);
}

