#include "tui.h"
#include "game.h"
#include <stdbool.h>

void cleanup(CardCollection *deck, CardCollection *player_hand, CardCollection *dealer_hand) {
    free(deck->cards);
    free(player_hand->cards);
    free(dealer_hand->cards);
    end_tui();
}

int main() {
    init_tui();

    CardCollection deck = create_deck();
    shuffle_deck(&deck);

    CardCollection player_hand = init_hand();
    CardCollection dealer_hand = init_hand();

    // Initial deal
    deal_card(&deck, &player_hand);
    deal_card(&deck, &dealer_hand);
    deal_card(&deck, &player_hand);
    deal_card(&deck, &dealer_hand);

    int player_score = calculate_score(&player_hand);
    int dealer_score = calculate_score(&dealer_hand);

    render_game(&player_hand, &dealer_hand, player_score, dealer_score, "Welcome to Blackjack!", true);

    get_input(); // Wait for user to press a key

    cleanup(&deck, &player_hand, &dealer_hand);

    return 0;
}
