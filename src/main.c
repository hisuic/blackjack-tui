#include "tui.h"
#include "game.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tui.h"
#include "game.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for sleep

void play_game() {
    CardCollection deck = create_deck();
    shuffle_deck(&deck);

    CardCollection player_hand = init_hand();
    CardCollection dealer_hand = init_hand();

    bool running = true;
    while(running) {
        // Reset hands
        player_hand.count = 0;
        dealer_hand.count = 0;

        // Reshuffle deck if less than 1/4 of cards remain
        if (deck.count < 13) {
            free(deck.cards);
            deck = create_deck();
            shuffle_deck(&deck);
        }

        // Initial deal
        deal_card(&deck, &player_hand);
        deal_card(&deck, &dealer_hand);
        deal_card(&deck, &player_hand);
        deal_card(&deck, &dealer_hand);

        int player_score = calculate_score(&player_hand);
        int dealer_score = calculate_score(&dealer_hand);
        char message[100] = "Your turn. (h) Hit, (s) Stand.";
        bool player_turn = true;
        bool round_over = false;

        while(!round_over) {
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
                                    strcpy(message, "Dealer's turn.");
                                    player_turn = false;
                                } else if (ch == 'q') {
                                    round_over = true;
                                    running = false;
                                } else if (ch == '?' || ch == 'H') {
                                    render_help();
                                    get_input();
                                }
                            }            
            if (!player_turn) {
                render_game(&player_hand, &dealer_hand, player_score, dealer_score, message, false);
                sleep(1);

                while (dealer_score < 17) {
                    deal_card(&deck, &dealer_hand);
                    dealer_score = calculate_score(&dealer_hand);
                    render_game(&player_hand, &dealer_hand, player_score, dealer_score, "Dealer is hitting...", false);
                    sleep(1);
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
                
                int ch_end;
                do {
                    ch_end = get_input();
                } while (ch_end != 'p' && ch_end != 'q');
                
                if (ch_end == 'q') {
                    running = false;
                }
                round_over = true;
            }
        }
    }

    free(deck.cards);
    free(player_hand.cards);
    free(dealer_hand.cards);
}

int main() {
    init_tui();
    play_game();
    end_tui();
    return 0;
}

