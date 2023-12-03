#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "days.h"

struct symbol {
    int32_t x;
    int32_t y;
    char symbol;
    bool used;
};

struct num {
    int32_t x;
    int32_t y;
    uint32_t value;
    uint32_t len;
};

struct tile {
    struct num* num;
    bool used;
    char symbol;
};

bool is_adjacent_to_symbol(int32_t x, int32_t y, struct tile* board, size_t width, size_t height) {
    for (int32_t xo = x - 1; xo <= x + 1; xo++) {
        if (xo < 0) continue;
        for (int32_t yo = y - 1; yo <= y + 1; yo++) {
            if (yo < 0) continue;
            if (yo >= height) continue;
            struct tile* tile = &board[yo * width + xo];
            if (tile->symbol != '.' && !is_digit(tile->symbol))
                return true;
        }
    }
    return false;
}

struct tile* is_adjacent_to_star(int32_t x, int32_t y, struct tile* board, size_t width, size_t height, int32_t* star_x, int32_t* star_y) {
    for (int32_t xo = x - 1; xo <= x + 1; xo++) {
        if (xo < 0) continue;
        for (int32_t yo = y - 1; yo <= y + 1; yo++) {
            if (yo < 0) continue;
            if (yo >= height) continue;
            struct tile* tile = &board[yo * width + xo];
            if (tile->symbol == '*') {
                *star_x = xo;
                *star_y = yo;
                return tile;
            }
        }
    }
    return NULL;
}

struct tile* find_adjacent_number(int32_t x, int32_t y, struct tile* board, size_t width, size_t height) {
    for (int32_t xo = x - 1; xo <= x + 1; xo++) {
        if (xo < 0) continue;
        for (int32_t yo = y - 1; yo <= y + 1; yo++) {
            if (yo < 0) continue;
            if (yo >= height) continue;
            struct tile* tile = &board[yo * width + xo];
            if (tile->num) return tile;
        }
    }
    return NULL;
}

void print_grid(struct tile* board, size_t width, size_t height) {
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            fputc(board[y * width + x].symbol, stderr);
        }
        fputc('\n', stderr);
    }
}

struct day2_result day3(struct string_view input) {
    struct day2_result result = {0};

    struct num* nums = calloc(sizeof(*nums), input.len);
    size_t n_nums = 0;

    struct tile* board = calloc(sizeof(*board), input.len);

    size_t width = 0;
    size_t height = 0;

    for (uint32_t y = 0; input.len > 0; y++) {
        struct string_view line = chop_until(&input, '\n');
        if (line.len == 0) continue;
        height++;

        if (width == 0) width = line.len;

        for (size_t i = 0; i < line.len; i++) {
            board[y * width + i].symbol = line.ptr[i];
            board[y * width + i].used = false;
        }

        struct string_view orig_line = line;

        while (line.len > 0) {
            chop_while(&line, is_period);
            if (line.len == 0) break;
            char next_char = line.ptr[0];
            uint32_t x = line.ptr - orig_line.ptr;
            if (!is_digit(next_char)) {
                line.ptr++;
                line.len--;
                continue;
            }
            struct string_view num_str = chop_while(&line, is_digit);
            uint32_t num;
            if (parse_uint32(num_str.ptr, num_str.len, &num)) {
                fputs("Failed to parse line: \"", stderr);
                fwrite_all(orig_line.ptr, orig_line.len, stderr);
                fputs("\"\n", stderr);
                exit(1);
            }

            for (size_t i = 0; i < num_str.len; i++) 
                board[y * width + x + i].num = &nums[n_nums];
            nums[n_nums++] = (struct num) {
                .x = x,
                .y = y,
                .value = num,
                .len = num_str.len,
            };
        }
    }

    for (size_t i = 0; i < n_nums; i++) {
        struct num num = nums[i];
        for (int32_t j = 0; j < num.len; j++) {
            if (is_adjacent_to_symbol(num.x + j, num.y, board, width, height)) {
                result.part_one += num.value;
                break;
            }
        }
    }

    for (size_t i = 0; i < n_nums; i++) {
        struct num* num1_ptr = &nums[i];
        struct num num1 = *num1_ptr;
        for (int32_t j = 0; j < num1.len; j++) {
            int32_t star_x, star_y;
            struct tile* star_tile = is_adjacent_to_star(num1.x + j, num1.y, board, width, height, &star_x, &star_y);
            if (star_tile == NULL) continue;
            if (star_tile->used) continue;

            struct tile* num_tile = find_adjacent_number(star_x, star_y, board, width, height);
            if (num_tile == NULL) continue;
            if (num1_ptr == num_tile->num) continue;

            num_tile->used = true;
            result.part_two += num1.value * num_tile->num->value;
            break;
        }
    }

    free(board);
    free(nums);
    return result;
}

