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
        
        // Dealer's turn
        if (!player_turn && !game_over) {
            // Reveal dealer's hand
            render_game(&player_hand, &dealer_hand, player_score, dealer_score, message, false);

            while (dealer_score < 17) {
                deal_card(&deck, &dealer_hand);
                dealer_score = calculate_score(&dealer_hand);
                render_game(&player_hand, &dealer_hand, player_score, dealer_score, "Dealer is hitting...", false);
                get_input(); // Pause for effect
            }

            if (player_score > 21) {
                // Player already busted
            } else if (dealer_score > 21) {
                strcpy(message, "Dealer busts! You win!");
            } else if (player_score > dealer_score) {
                strcpy(message, "You win!");
            } else if (dealer_score > player_score) {
                strcpy(message, "Dealer wins!");
            } else {
                strcpy(message, "It's a push!");
            }
            
            strcat(message, " (p) Play Again, (q) Quit");
            render_game(&player_hand, &dealer_hand, player_score, dealer_score, message, false);
            
            int ch_end = get_input();
            if (ch_end == 'p') {
                // Reset for a new game (simplified)
                // This will be improved later
                game_over = true; // For now, just exit
            } else {
                game_over = true;
            }
        }
    }

    cleanup(&deck, &player_hand, &dealer_hand);
    return 0;
}
