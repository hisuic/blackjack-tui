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

#endif // GAME_H
