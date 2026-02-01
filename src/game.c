#include "game.h"
#include <stdlib.h>
#include <time.h>

CardCollection init_hand() {
    CardCollection hand;
    hand.count = 0;
    hand.capacity = 10; // Max cards in a hand
    hand.cards = malloc(sizeof(Card) * hand.capacity);
    return hand;
}

CardCollection create_deck() {
    CardCollection deck;
    deck.count = 52;
    deck.capacity = 52;
    deck.cards = malloc(sizeof(Card) * deck.capacity);

    int i = 0;
    for (int suit = HEARTS; suit <= SPADES; suit++) {
        for (int rank = TWO; rank <= ACE; rank++) {
            deck.cards[i].suit = suit;
            deck.cards[i].rank = rank;
            i++;
        }
    }
    return deck;
}

void shuffle_deck(CardCollection *deck) {
    srand(time(NULL));
    for (int i = deck->count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
}

void deal_card(CardCollection *deck, CardCollection *hand) {
    if (deck->count > 0) {
        // For simplicity, we assume the hand has enough capacity.
        // A more robust implementation would reallocate if needed.
        hand->cards[hand->count] = deck->cards[deck->count - 1];
        deck->count--;
        hand->count++;
    }
}

int calculate_score(const CardCollection *hand) {
    int score = 0;
    int ace_count = 0;

    for (int i = 0; i < hand->count; i++) {
        Rank rank = hand->cards[i].rank;
        if (rank >= JACK && rank <= KING) {
            score += 10;
        } else if (rank == ACE) {
            ace_count++;
            score += 11;
        } else {
            score += rank;
        }
    }

    // Adjust for Aces
    while (score > 21 && ace_count > 0) {
        score -= 10;
        ace_count--;
    }

    return score;
}
