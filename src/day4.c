#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "days.h"

void failed_to_parse(struct string_view line) {
    fputs("Failed to parse line: \"", stderr);
    exit_on_error(fwrite_all(line.ptr, line.len, stderr));
    fputs("\".\n", stderr);
    exit(1);
}

struct card {
    struct card* next;
    uint32_t id;
    uint32_t n_winning;
    uint32_t n_having;
    uint32_t instances;
    uint32_t cards[];
};

size_t count_lines(struct string_view str) {
    size_t n = 0;
    for (size_t i = 0; i < str.len; i++)
        if (str.ptr[i] == '\n') n++;
    return n;
}

struct uint64_day_result day4(struct string_view input) {
    struct uint64_day_result result = {0};
    
    const size_t n_lines = count_lines(input);
    struct card* cards = malloc(sizeof(*cards) * n_lines + sizeof(uint32_t) * (input.len / 2));

    struct card* card = cards;
    card->next = card;

    for (;;) {
        if (input.len == 0) {
            card->next = NULL;
            break;
        }
        struct string_view line = chop_until(&input, '\n');
        if (line.len == 0) continue;
        card = card->next;

        struct string_view orig_line = line;
        line = drop_prefix(line, string_view_from_cstr("Card "));
        chop_while(&line, is_whitespace);

        struct string_view card_id_str = chop_until(&line, ':');
        if (parse_uint32(card_id_str.ptr, card_id_str.len, &card->id))
            failed_to_parse(orig_line);

        while (line.len > 0 && line.ptr[0] != '|') {
            chop_while(&line, is_whitespace);
            struct string_view n_str = chop_until(&line, ' ');
            uint32_t* n = card->cards + card->n_winning;
            if (parse_uint32(n_str.ptr, n_str.len, n))
                failed_to_parse(orig_line);
            card->n_winning++;
        }

        chop_until(&line, '|');

        while (line.len > 0) {
            chop_while(&line, is_whitespace);
            struct string_view n_str = chop_until(&line, ' ');
            uint32_t* n = card->cards + card->n_winning + card->n_having;
            if (parse_uint32(n_str.ptr, n_str.len, n))
                failed_to_parse(orig_line);
            card->n_having++;
        }

        card->instances = 1;
        card->next = (struct card*)(card->cards + card->n_winning + card->n_having);
    }


    card = cards;
    while (card) {
        uint32_t card_value = 0;
        for (size_t i = 0; i < card->n_having; i++) {
            uint32_t having = card->cards[card->n_winning + i];
            for (size_t j = 0; j < card->n_winning; j++) {
                if (having == card->cards[j]) {
                    if (card_value == 0) card_value = 1;
                    else card_value <<= 1;
                    break;
                }
            }
        }
        result.part_one += card_value;
        card = card->next;
    }

    card = cards;
    for (card = cards; card; card = card->next) {
        if (card->instances == 0) continue;
        uint32_t n_matches = 0;
        for (size_t i = 0; i < card->n_having; i++) {
            uint32_t having = card->cards[card->n_winning + i];
            for (size_t j = 0; j < card->n_winning; j++) {
                if (having == card->cards[j]) {
                    n_matches++;
                    break;
                }
            }
        }
        struct card* next_card = card->next;
        for (size_t i = 0; i < n_matches && next_card; i++) {
            next_card->instances += card->instances;
            next_card = next_card->next;
        }
        result.part_two += card->instances;
    }

    return result;
}

