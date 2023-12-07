#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"
#include "days.h"

int card_index(char card) {
    if (card <= '9') return card - '2';
    switch (card) {
        case 'A': return 12;
        case 'K': return 11;
        case 'Q': return 10;
        case 'J': return 9;
        case 'T': return 8;
    }
    return -1;
}

void fill_distrib(const char hand[5], uint8_t distrib[13]) {
    for (size_t i = 0; i < 5; i++)
        distrib[card_index(hand[i])]++;
}
bool distrib_contains(uint8_t distrib[13], uint8_t n_cards, uint8_t n_pairs) {
    for (size_t i = 0; i < 13; i++)
        if (distrib[i] == n_cards)
            if (--n_pairs == 0) return true;
    return false;
}

bool is_five_of_a_kind(const char hand[5]) {
    char a = hand[0];
    return a == hand[1] && a == hand[2] && a == hand[3] && a == hand[4];
}
bool is_four_of_a_kind(const char hand[5]) {
    uint8_t distrib[13] = { 0 };
    fill_distrib(hand, distrib);
    return distrib_contains(distrib, 4, 1);
}
bool is_full_house(const char hand[5]) {
    uint8_t distrib[13] = { 0 };
    fill_distrib(hand, distrib);
    return distrib_contains(distrib, 3, 1) && distrib_contains(distrib, 2, 1);
}
bool is_three_of_a_kind(const char hand[5]) {
    uint8_t distrib[13] = { 0 };
    fill_distrib(hand, distrib);
    return distrib_contains(distrib, 3, 1) && distrib_contains(distrib, 1, 1);
}
bool is_two_pair(const char hand[5]) {
    uint8_t distrib[13] = { 0 };
    fill_distrib(hand, distrib);
    return distrib_contains(distrib, 2, 2);
}
bool is_one_pair(const char hand[5]) {
    uint8_t distrib[13] = { 0 };
    fill_distrib(hand, distrib);
    return distrib_contains(distrib, 2, 1) && distrib_contains(distrib, 1, 3);
}
bool is_high_card(const char hand[5]) {
    uint8_t distrib[13] = { 0 };
    fill_distrib(hand, distrib);
    return distrib_contains(distrib, 1, 5);
}

enum hand_type {
    five_of_a_kind,
    four_of_a_kind,
    full_house,
    three_of_a_kind,
    two_pair,
    one_pair,
    high_card,
};

enum hand_type hand_type(const char hand[5]) {
    if (is_five_of_a_kind(hand)) return five_of_a_kind;
    if (is_four_of_a_kind(hand)) return four_of_a_kind;
    if (is_full_house(hand)) return full_house;
    if (is_three_of_a_kind(hand)) return three_of_a_kind;
    if (is_two_pair(hand)) return two_pair;
    if (is_one_pair(hand)) return one_pair;
    if (is_high_card(hand)) return high_card;
    fwrite_all(hand, 5, stdout);
    exit(7);
}

int compare_hands(const void* va, const void* vb) {
    const char* a = *(char**)va;
    const char* b = *(char**)vb;
    enum hand_type a_ty = hand_type(a);
    enum hand_type b_ty = hand_type(b);
    if (a_ty != b_ty) return b_ty - a_ty;
    for (size_t i = 0; i < 5; i++)
        if (a[i] != b[i]) return card_index(a[i]) - card_index(b[i]);
    return 0;
}

struct hand_with_pid {
    char* hand;
    uint32_t bid;
};

struct uint64_day_result day7(struct string_view input) {
    struct uint64_day_result result = {0};

    size_t n_lines = 0;
    for (size_t i = 0; i < input.len; i++)
        n_lines += input.ptr[i] == '\n';

    struct hand_with_pid hands[n_lines];
    size_t n_hands = 0;

    while (input.len > 0) {
        struct string_view line = chop_until(&input, '\n');
        if (line.len == 0) continue;
        struct string_view hand = chop_until(&line, ' ');
        struct string_view bid_str = chop_until(&line, '\n');
        uint32_t bid;
        if (parse_uint32(bid_str.ptr, bid_str.len, &bid))
            goto failed_parsing;
        assert(hand.len == 5);
        hands[n_hands++] = ((struct hand_with_pid) {
            .hand = hand.ptr,
            .bid = bid,
        });
    }

    qsort(hands, n_hands, sizeof(*hands), compare_hands);
    for (size_t i = 0; i < n_hands; i++) {
        result.part_one += (i + 1) * hands[i].bid;
    }

    return result;
failed_parsing:
    fputs("Failed to parse\n", stderr);
    exit(1);
}


