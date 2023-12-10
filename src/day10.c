#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "util.h"
#include "days.h"

typedef struct {
    char* tiles;
    size_t width;
    size_t height;
} grid;

typedef struct {
    int32_t x;
    int32_t y;
} i32vec2;

#define get_tile(grid, x, y) grid.tiles[(y) * grid.width + (x)]
#define i32vec_eq(a, b) ((a).x == (b).x && (a).y == (b).y)

i32vec2 find_start(grid grid) {
    for (int32_t y = 0; y < grid.height; y++)
        for (int32_t x = 0; x < grid.width; x++)
            if (get_tile(grid, x, y) == 'S')
                return (i32vec2) {
                    .x = x,
                    .y = y,
                };
    fputs("Starting position not found!\n", stderr);
    exit(1);
}

struct step {
    i32vec2 pos;
    struct step* previous;
    char tile;
};

bool can_go_left_from_s(grid grid, i32vec2 pos) {
    assert(get_tile(grid, pos.x, pos.y) == 'S');
    if (pos.x == 0) return false;
    char tile = get_tile(grid, pos.x - 1, pos.y);
    return tile == 'F' || tile == 'L' || tile == '-';
}
bool can_go_right_from_s(grid grid, i32vec2 pos) {
    assert(get_tile(grid, pos.x, pos.y) == 'S');
    if (pos.x + 1 == grid.width) return false;
    char tile = get_tile(grid, pos.x + 1, pos.y);
    return tile == 'J' || tile == '7' || tile == '-';
}
bool can_go_up_from_s(grid grid, i32vec2 pos) {
    assert(get_tile(grid, pos.x, pos.y) == 'S');
    if (pos.y == 0) return false;
    char tile = get_tile(grid, pos.x, pos.y - 1);
    return tile == '7' || tile == 'F' || tile == '|';
}
bool can_go_down_from_s(grid grid, i32vec2 pos) {
    assert(get_tile(grid, pos.x, pos.y) == 'S');
    if (pos.y + 1 == grid.height) return false;
    char tile = get_tile(grid, pos.x, pos.y + 1);
    return tile == 'J' || tile == 'L' || tile == '|';
}

static bool go(grid grid, struct step step, bool* visited, i32vec2* loop, size_t* loop_len, i32vec2 first_s_conn, int depth) {
    assert(step.pos.x >= 0 && step.pos.x < grid.width && step.pos.y >= 0 && step.pos.y < grid.height);
    char tile = get_tile(grid, step.pos.x, step.pos.y);
    if (tile == '.') return false;

    if (tile == 'S' && step.previous && !(step.previous->pos.x == first_s_conn.x && step.previous->pos.y == first_s_conn.y)) 
        return true;

    bool* already_visited = &visited[step.pos.y * grid.width + step.pos.x];
    if (*already_visited) return false;
    *already_visited = true;

    if (step.pos.x > 0 && (tile == 'J' || tile == '-' || tile == '7')) {
        struct step next = (struct step) { .pos = { .x = step.pos.x - 1, .y = step.pos.y }, .previous = &step, tile = tile };
        if (go(grid, next, visited, loop, loop_len, first_s_conn, depth + 1))
            goto success;
    }
    if (tile == 'S' && can_go_left_from_s(grid, step.pos)) {
        struct step next = (struct step) { .pos = { .x = step.pos.x - 1, .y = step.pos.y }, .previous = &step, tile = tile };
        if (go(grid, next, visited, loop, loop_len, next.pos, depth + 1))
            goto success;
    }

    if (step.pos.y > 0 && (tile == 'J' || tile == '|' || tile == 'L')) {
        struct step next = (struct step) { .pos = { .x = step.pos.x, .y = step.pos.y - 1 }, .previous = &step, tile = tile };
        if (go(grid, next, visited, loop, loop_len, first_s_conn, depth + 1))
            goto success;
    }
    if (tile == 'S' && can_go_up_from_s(grid, step.pos)) {
        struct step next = (struct step) { .pos = { .x = step.pos.x, .y = step.pos.y - 1 }, .previous = &step, tile = tile };
        if (go(grid, next, visited, loop, loop_len, next.pos, depth + 1))
            goto success;
    }

    if (step.pos.x + 1 < grid.width && (tile == 'F' || tile == '-' || tile == 'L')) {
        struct step next = (struct step) { .pos = { .x = step.pos.x + 1, .y = step.pos.y }, .previous = &step, tile = tile };
        if (go(grid, next, visited, loop, loop_len, first_s_conn, depth + 1))
            goto success;
    }
    if (tile == 'S' && can_go_right_from_s(grid, step.pos)) {
        struct step next = (struct step) { .pos = { .x = step.pos.x + 1, .y = step.pos.y }, .previous = &step, tile = tile };
        if (go(grid, next, visited, loop, loop_len, next.pos, depth + 1))
            goto success;
    }

    if (step.pos.y + 1 < grid.height && (tile == 'F' || tile == '|' || tile == '7')) {
        struct step next = (struct step) { .pos = { .x = step.pos.x, .y = step.pos.y + 1 }, .previous = &step, tile = tile };
        if (go(grid, next, visited, loop, loop_len, first_s_conn, depth + 1))
            goto success;
    }
    if (tile == 'S' && can_go_down_from_s(grid, step.pos)) {
        struct step next = (struct step) { .pos = { .x = step.pos.x, .y = step.pos.y + 1 }, .previous = &step, tile = tile };
        if (go(grid, next, visited, loop, loop_len, next.pos, depth + 1))
            goto success;
    }

    *already_visited = false;
    return false;
success:
    loop[(*loop_len)++] = step.pos;
    return true;
}

i32vec2* find_loop(grid grid, i32vec2 start, size_t* loop_len) {
    *loop_len = 0;
    i32vec2* loop = malloc(grid.height * grid.width * sizeof(i32vec2));
    bool* visited = calloc(grid.height * grid.width, sizeof(bool));
    struct step step = { .pos = start, .tile = get_tile(grid, start.x, start.y), .previous = NULL };
    if (go(grid, step, visited, loop, loop_len, (i32vec2) { .x  = -1, .y = -1 }, 0)) {
        free(visited);
        return loop;
    }
    free(visited);
    return NULL;
}

 uint32_t* build_obstructions_to_outside_map(grid grid, bool* loop_map) {
    size_t map_buf_size = grid.height * grid.width * sizeof(uint32_t);
    uint32_t* map = malloc(map_buf_size);
    memset(map, 0xFF, map_buf_size);
    uint32_t* dst = malloc(map_buf_size);
    memcpy(dst, map, map_buf_size);
    for (;;) {
        for (;;) {
            bool something_changed = false;
            for (size_t y = 1; y < grid.height - 1; y++) {
                for (size_t x = 1; x < grid.width - 1; x++) {
                    if (get_tile(grid, x, y) != '.') continue;
                    size_t index = y * grid.width + x;
                    uint32_t* dst_entry = &dst[index];
                    if (*dst_entry == 0) continue;
                    if (map[y * grid.width + x + 1] == 0 || map[y * grid.width + x - 1] == 0
                            || map[(y - 1) * grid.width + x] == 0 || map[(y + 1) * grid.width + x] == 0) {
                        something_changed = true;
                        *dst_entry = 0;
                    }
                }
            }
            if (!something_changed) break;
            memcpy(map, dst, map_buf_size);
        }
        bool something_changed = false;
        for (size_t y = 0; y < grid.height; y++) {
            for (size_t x = 0; x < grid.width; x++) {
                uint32_t* dst_entry = &dst[y * grid.width + x];
                bool is_part_of_loop = loop_map[y * grid.width + x];
                if ((x == 0 || y == 0 || x + 1 == grid.width || y + 1 == grid.height)) {
                    something_changed |= is_part_of_loop != *dst_entry;
                    *dst_entry = is_part_of_loop;
                    continue;
                }
                uint32_t new = min(
                    min(map[y * grid.width + x + 1], map[y * grid.width + x - 1]),
                    min(map[(y + 1) * grid.width + x], map[(y - 1) * grid.width + x])
                );
                if (new == UINT32_MAX) continue;
                new += is_part_of_loop;
                if (*dst_entry != new) {
                    *dst_entry = new;
                    something_changed = true;
                }
            }
        }
        if (!something_changed) break;
        memcpy(map, dst, map_buf_size);
    }
    for (size_t y = 0; y < grid.height; y++) {
        for (size_t x = 0; x < grid.width; x++) {
            uint32_t obstructions = map[y * grid.width + x];
            if (obstructions == UINT32_MAX) putc('.', stdout);
            else printf("%u", obstructions);
        }
        putc('\n', stdout);
    }
    free(dst);
    return map;
}

bool is_part_of_loop(i32vec2* loop, size_t loop_len, i32vec2 pos) {
    for (size_t i = 0; i < loop_len; i++)
        if (i32vec_eq(loop[i], pos)) return true;
    return false;
}

struct uint64_day_result day10(struct string_view input) {
    struct uint64_day_result result = {0};

    size_t height = 0;
    for (size_t i = 0; i < input.len; i++)
        height += input.ptr[i] == '\n';

    size_t width = 0;
    while (width < input.len && input.ptr[width] != '\n') width++;

    char* tiles = malloc(width * height);

    size_t y = 0;
    while (input.len > 0) {
        struct string_view line = chop_until(&input, '\n');
        if (line.len == 0) break;
        assert(line.len == width);
        for (size_t x = 0; x < line.len; x++) {
            char ch = line.ptr[x];
            assert(ch == '.' || ch == '|' || ch == 'F' || ch == 'L' || ch == '-' || ch == 'J' || ch == '7' || ch == 'S');
            tiles[y * width + x] = ch;
        }
        y++;
    }
    grid grid = {
        .tiles = tiles,
        .width = width,
        .height = y,
    };

    i32vec2 start = find_start(grid);

    size_t loop_len;
    i32vec2* loop = find_loop(grid, start, &loop_len);
    if (!loop) {
        fputs("No loop found!", stderr);
        exit(1);
    }
    result.part_one = loop_len / 2;

    bool* loop_map = calloc(grid.width * grid.height, sizeof(*loop_map));
    for (size_t i = 0; i < loop_len; i++) {
        i32vec2 pos = loop[i];
        loop_map[pos.y * grid.width + pos.x] = true;
    }
    uint32_t* obstructions_map = build_obstructions_to_outside_map(grid, loop_map);
    puts("");
    uint32_t i = 0;
    for (size_t y = 0; y < grid.height; y++) {
        for (size_t x = 0; x < grid.width; x++) {
            char tile = get_tile(grid, x, y);
            if (loop_map[y * grid.width + x]) {
                i++;
                printf("\033[38;2;128;255;128m");
                putc(tile, stdout);
                fputs("\033[0m", stdout);
                continue;
            }
            fputs("\033[38;2;255;128;128m", stdout);
            putc(obstructions_map[y * grid.width + x] & 1 ? 'I' : 'O', stdout);
            fputs("\033[0m", stdout);
            result.part_two += (obstructions_map[y * grid.width + x] & 1) && !loop_map[y * grid.width + x];
        }
        putc('\n', stdout);
    }

    free(obstructions_map);
    free(loop_map);
    free(loop);
    free(grid.tiles);
    return result;
}

