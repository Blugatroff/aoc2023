#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"
#include "days.h"

void check_name_validity(char* name) {
    for (size_t i = 0; i < 3; i++)
        assert('A' <= name[i] && name[i] <= 'Z');
}

uint16_t node_name_to_index(char* name) {
    check_name_validity(name);
    uint16_t index = 0;
    for (size_t i = 0; i < 3; i++) {
        index *= 'Z' - 'A';
        index += name[i] - 'A';
    }
    return index;
}

struct node {
    char name[4];
    uint16_t left;
    uint16_t right;
};

uint64_t gcd(uint64_t a, uint64_t b) {
    for (;;) {
        if (a == 0) return b;
        b %= a;
        if (b == 0) return a;
        a %= b;
    }
}

uint64_t lcm(uint64_t a, uint64_t b) {
    uint64_t t = gcd(a, b);
    return t == 0 ? 0 : (a / t * b);
}

uint64_t lcms(uint64_t* nums, size_t count, size_t stride) {
    assert(count > 0);
    uint64_t m = *nums;
    for (size_t i = 1; i < count; i++)
        m = lcm(m, *(uint64_t*)((uint8_t*)nums + stride * i));
    return m;
}

struct uint64_day_result day8(struct string_view input) {
    struct uint64_day_result result = {0};

    size_t n_lines = 0;
    for (size_t i = 0; i < input.len; i++)
        n_lines += input.ptr[i] == '\n';

    struct string_view first_line = chop_until(&input, '\n');
    char steps[first_line.len];
    const size_t n_steps = first_line.len;
    for (size_t i = 0; i < n_steps; i++) {
        assert(first_line.ptr[i] == 'L' || first_line.ptr[i] == 'R');
        steps[i] = first_line.ptr[i];
    }

    const size_t a = 'Z' - 'A';
    const size_t max_nodes = a * a * a;
    assert(n_lines < max_nodes);
    struct node* nodes = calloc(max_nodes, sizeof(*nodes));

    size_t n_cursors = 0;
    while (input.len > 0) {
        struct string_view line = chop_until(&input, '\n');
        if (line.len == 0) continue;

        struct string_view node_name = chop_until(&line, ' ');
        assert(node_name.len == 3);

        chop_until(&line, '(');
        struct string_view left_node = chop_until(&line, ',');
        assert(left_node.len == 3);

        chop_until(&line, ' ');
        struct string_view right_node = chop_until(&line, ')');
        assert(right_node.len == 3);
        uint16_t index = node_name_to_index(node_name.ptr);
        nodes[index] = (struct node) {
            .left = node_name_to_index(left_node.ptr),
            .right = node_name_to_index(right_node.ptr),
        };
        nodes[index].name[0] = node_name.ptr[0];
        nodes[index].name[1] = node_name.ptr[1];
        nodes[index].name[2] = node_name.ptr[2];
        nodes[index].name[3] = 0;
        n_cursors += node_name.ptr[2] == 'A';
    }

    uint16_t end = node_name_to_index("ZZZ");
    uint16_t current = node_name_to_index("AAA");
    for (result.part_one = 0; current != end; result.part_one++) {
        size_t step_index = result.part_one % n_steps;
        struct node* node = &nodes[current];
        char step = steps[step_index];
        current = step == 'R' ? node->right : node->left;
    }

    struct cursor {
        uint16_t start;
        uint16_t current;
    };
    struct cursor* cursors = calloc(n_cursors, sizeof(*cursors));
    n_cursors = 0;

    for (size_t i = 0; i < max_nodes; i++) {
        struct node* node = &nodes[i];
        if (node->name[2] == 'A')
            cursors[n_cursors++] = (struct cursor) {
                .start = i,
                .current = i,
            };
    }
    
    struct { uint64_t steps; bool reached; }* steps_until_z = calloc(n_cursors, sizeof(*steps_until_z));
    for (;;) {
        for (result.part_two = 0; ; result.part_two++) {
            uint64_t step_index = result.part_two % n_steps;

            for (size_t i = 0; i < n_cursors; i++) {
                if (steps_until_z[i].reached) continue;
                struct cursor* cursor = &cursors[i];
                struct node* node = &nodes[cursor->current];
                char step = steps[step_index];
                cursor->current = step == 'R' ? node->right : node->left;

                steps_until_z[i].steps++;
                steps_until_z[i].reached = nodes[cursor->current].name[2] == 'Z';
            }

            bool all_reached_end = true;
            for (size_t i = 0; i < n_cursors && all_reached_end; i++)
                all_reached_end = all_reached_end && steps_until_z[i].reached;
            if (all_reached_end) {
                result.part_two = lcms((uint64_t*)steps_until_z, n_cursors, sizeof(*steps_until_z));
                goto done;
            }
        }
    }

done:
    free(steps_until_z);
    free(cursors);
    free(nodes);
    return result;
}

