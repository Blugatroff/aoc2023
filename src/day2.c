#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "days.h"

enum color {
    red,
    green,
    blue,
};

bool parse_color(struct string_view str, enum color* color) {
    if (string_view_eq_cstring(str, "red")) { *color = red; return false; }
    if (string_view_eq_cstring(str, "green")) { *color = green; return false; }
    if (string_view_eq_cstring(str, "blue")) { *color = blue; return false; }
    return true;
}

struct reveal {
    uint32_t reds;
    uint32_t greens;
    uint32_t blues;
};

struct game {
    uint32_t id;
    uint32_t max_reds;
    uint32_t max_greens;
    uint32_t max_blues;
};

bool parse_reveal(struct string_view str, struct reveal* reveal) {
    memset(reveal, 0, sizeof(*reveal));
    while (str.len > 0) {
        struct string_view cube_str = chop_until(&str, ',');
        chop_while(&cube_str, is_space);

        struct string_view n_cubes_str = chop_until(&cube_str, ' ');
        uint32_t n_cubes;
        if (parse_uint32(n_cubes_str.ptr, n_cubes_str.len, &n_cubes))
            return true;
        enum color color;
        if (parse_color(cube_str, &color))
            return true;
        switch (color) {
            case red: reveal->reds += n_cubes; break;
            case green: reveal->greens += n_cubes; break;
            case blue: reveal->blues += n_cubes; break;
        }
    }
    return false;
}

bool parse_line(struct string_view line, struct game* game) {
    line = drop_prefix(line, string_view_from_cstr("Game "));
    struct string_view game_id_str = chop_until(&line, ':');
    if (parse_uint32(game_id_str.ptr, game_id_str.len, &game->id))
        return true;
    chop_while(&line, is_space);

    while (line.len > 0) {
        struct string_view revealed_subset = chop_until(&line, ';');
        chop_while(&revealed_subset, is_space);
        struct reveal revealed;
        if (parse_reveal(revealed_subset, &revealed))
            return true;
        game->max_reds = max(game->max_reds, revealed.reds);
        game->max_greens = max(game->max_greens, revealed.greens);
        game->max_blues = max(game->max_blues, revealed.blues);
    }
    return false;
}

struct uint64_day_result day2(struct string_view input) {
    struct uint64_day_result result = {0};
    while (input.len > 0) {
        struct string_view line = chop_until(&input, '\n');
        if (line.len == 0) continue;
        struct game game = {0};
        if (parse_line(line, &game)) {
            fputs("Failed to parse line: \"", stderr);
            fwrite_all(line.ptr, line.len, stderr);
            fputs("\"\n", stderr);
            exit(1);
        }
        if (game.max_reds <= 12 && game.max_greens <= 13 && game.max_blues <= 14)
            result.part_one += game.id;
        result.part_two += game.max_reds * game.max_greens * game.max_blues;
    }
    return result;
}

