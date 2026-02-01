#ifndef GAME_H
#define GAME_H

// Represents a card suit
typedef enum {
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
} Suit;

// Represents a card rank
typedef enum {
    TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
    JACK, QUEEN, KING, ACE
} Rank;

// Represents a single card
typedef struct {
    Suit suit;
    Rank rank;
} Card;

// Represents a collection of cards (e.g., a hand or a deck)
typedef struct {
    Card *cards;
    int count;
    int capacity;
} CardCollection;

// Initializes a hand with a default capacity
CardCollection init_hand();

// Creates a standard 52-card deck
CardCollection create_deck();

// Shuffles a card collection
void shuffle_deck(CardCollection *deck);

// Deals a card from the deck to a hand
void deal_card(CardCollection *deck, CardCollection *hand);

// Calculates the score of a hand
int calculate_score(const CardCollection *hand);

#endif // GAME_H
