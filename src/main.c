#include "tui.h"
#include "game.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

    char message[100] = "Your turn. (h) Hit, (s) Stand.";
    bool game_over = false;
    bool player_turn = true;

    while (!game_over) {
        render_game(&player_hand, &dealer_hand, player_score, dealer_score, message, player_turn);
        int ch = get_input();

        if (player_turn) {
            if (ch == 'h') {
                deal_card(&deck, &player_hand);
                player_score = calculate_score(&player_hand);
                if (player_score > 21) {
                    strcpy(message, "You busted! Dealer wins.");
                    player_turn = false;
                }
            } else if (ch == 's') {
                player_turn = false;
                strcpy(message, "Dealer's turn.");
            } else if (ch == 'q') {
                game_over = true;
                strcpy(message, "Quitting...");
            }
        }
        
        // Simple exit for now after player turn
        if (!player_turn) {
            // Dealer logic will go here
            get_input(); // Wait for keypress before exiting
            game_over = true;
        }
    }

    cleanup(&deck, &player_hand, &dealer_hand);
    return 0;
}
