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

int joker_card_index(char card) {
    if (card <= '9') return card - '2' + 1;
    switch (card) {
        case 'A': return 12;
        case 'K': return 11;
        case 'Q': return 10;
        case 'J': return 0;
        case 'T': return 9;
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

struct hand_with_bid {
    char hand[5];
    uint32_t bid;
    enum hand_type type;
};

int compare_hands(const void* va, const void* vb) {
    const struct hand_with_bid* a = (struct hand_with_bid*)va;
    const struct hand_with_bid* b = (struct hand_with_bid*)vb;
    if (a->type != b->type) return b->type - a->type;
    for (size_t i = 0; i < 5; i++)
        if (a->hand[i] != b->hand[i]) return card_index(a->hand[i]) - card_index(b->hand[i]);
    return 0;
}

int joker_compare_hands(const void* va, const void* vb) {
    const struct hand_with_bid* a = (struct hand_with_bid*)va;
    const struct hand_with_bid* b = (struct hand_with_bid*)vb;
    if (a->type != b->type) return b->type - a->type;
    for (size_t i = 0; i < 5; i++)
        if (a->hand[i] != b->hand[i]) return joker_card_index(a->hand[i]) - joker_card_index(b->hand[i]);
    return 0;
}

char cards[] = { 'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2' };

void go(struct hand_with_bid* hand, size_t n_jokers, struct hand_with_bid* unjokered, size_t* n_unjokered) {
    if (n_jokers == 0) {
        struct hand_with_bid* permut_hand = &unjokered[(*n_unjokered)++];
        permut_hand->bid = hand->bid;
        memcpy(permut_hand->hand, hand->hand, 5);
        return;
    }
    size_t joker_index = 0;
    while (hand->hand[joker_index] != 'J') joker_index++;
    for (size_t replacement_card = 0; replacement_card < sizeof(cards); replacement_card++) {
        struct hand_with_bid permut_hand = { .bid = hand->bid, .type = hand->type };
        memcpy(permut_hand.hand, hand->hand, 5);
        permut_hand.hand[joker_index] = cards[replacement_card];
        go(&permut_hand, n_jokers - 1, unjokered, n_unjokered);
    }
}

enum hand_type best_type_with_jokers(struct hand_with_bid hand) {
    size_t n_jokers = 0;
    size_t n_permutations = 1;
    for (size_t i = 0; i < 5; i++) {
        if (hand.hand[i] == 'J') {
            n_jokers++; 
            n_permutations *= sizeof(cards);
        }
    }
    if (n_jokers == 0) return hand.type;
    struct hand_with_bid* unjokered = calloc(n_permutations, sizeof(struct hand_with_bid));
    size_t n_unjokered = 0; 
    go(&hand, n_jokers, unjokered, &n_unjokered);

    for (size_t i = 0; i < n_unjokered; i++) {
        unjokered[i].type = hand_type(unjokered[i].hand);
        memcpy(unjokered[i].hand, hand.hand, 5);
    }

    qsort(unjokered, n_unjokered, sizeof(*unjokered), compare_hands);

    enum hand_type best_type = unjokered[n_unjokered - 1].type;
    free(unjokered);
    return best_type;
}

struct uint64_day_result day7(struct string_view input) {
    struct uint64_day_result result = {0};

    size_t n_lines = 0;
    for (size_t i = 0; i < input.len; i++)
        n_lines += input.ptr[i] == '\n';

    struct hand_with_bid hands[n_lines];
    size_t n_hands = 0;

    while (input.len > 0) {
        struct string_view line = chop_until(&input, '\n');
        if (line.len == 0) continue;
        struct string_view hand_str = chop_until(&line, ' ');
        struct string_view bid_str = chop_until(&line, '\n');
        uint32_t bid;
        if (parse_uint32(bid_str.ptr, bid_str.len, &bid))
            goto failed_parsing;
        assert(hand_str.len == 5);
        struct hand_with_bid* hand = &hands[n_hands++];
        hand->bid = bid;
        memcpy(hand->hand, hand_str.ptr, 5);
        hand->type = hand_type(hand_str.ptr);
    }

    qsort(hands, n_hands, sizeof(*hands), compare_hands);
    for (size_t i = 0; i < n_hands; i++)
        result.part_one += (i + 1) * hands[i].bid;

    for (size_t i = 0; i < n_hands; i++)
        hands[i].type = best_type_with_jokers(hands[i]);
    qsort(hands, n_hands, sizeof(*hands), joker_compare_hands);
    for (size_t i = 0; i < n_hands; i++)
        result.part_two += (i + 1) * hands[i].bid;

    return result;
failed_parsing:
    fputs("Failed to parse\n", stderr);
    exit(1);
}


