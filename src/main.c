#include "tui.h"
#include "game.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for sleep
#include <locale.h>
#include <stdio.h>

GameState* init_game_state() {
    GameState *state = malloc(sizeof(GameState));
    state->deck = create_deck();
    shuffle_deck(&state->deck);
    state->player_hand = init_hand();
    state->dealer_hand = init_hand();
    state->player_money = 1000; // Starting money
    state->current_bet = 0;
    state->phase = PHASE_BETTING;
    strcpy(state->message, "Welcome! Place your bet.");
    return state;
}

void play_game(GameState *state) {
    bool running = true;
    while(running) {
        if (state->player_money <= 0) {
            strcpy(state->message, "Game Over! You have no money left. (r) Reset, (q) Quit");
            render_game(state);
            int ch_end;
            do {
                ch_end = get_input();
            } while (ch_end != 'r' && ch_end != 'q');

            if (ch_end == 'q') {
                running = false;
                continue;
            } else if (ch_end == 'r') {
                // Reset game state
                state->player_money = 1000;
                state->current_bet = 0;
                free(state->deck.cards);
                state->deck = create_deck();
                shuffle_deck(&state->deck);
            }
        }

        // Reset hands
        state->player_hand.count = 0;
        state->dealer_hand.count = 0;

        // Reshuffle deck if less than 1/4 of cards remain
        if (state->deck.count < 13) {
            free(state->deck.cards);
            state->deck = create_deck();
            shuffle_deck(&state->deck);
        }

        // Betting phase
        state->phase = PHASE_BETTING;
        state->current_bet = 0;
        strcpy(state->message, "Place your bet.");

        while (state->phase == PHASE_BETTING) {
            render_game(state);
            int ch = get_input();
            int bet_amount = 0;

            switch(ch) {
                case '1': bet_amount = 10; break;
                case '2': bet_amount = 50; break;
                case '3': bet_amount = 100; break;
                case '4': bet_amount = 500; break;
                case 'c':
                    state->current_bet = 0;
                    break;
                case 'a':
                    state->current_bet = state->player_money;
                    break;
                case 'b':
                    if (state->current_bet > 0 && state->current_bet <= state->player_money) {
                        state->phase = PHASE_PLAYER_TURN;
                    }
                    break;
                case 'q':
                    running = false;
                    state->phase = -1; //dummy phase
                    break;
            }

            if (bet_amount > 0) {
                if (state->current_bet + bet_amount <= state->player_money) {
                    state->current_bet += bet_amount;
                }
            }
             sprintf(state->message, "Your bet: $%d", state->current_bet);
        }
        if(!running) break;

        // Initial deal
        state->phase = PHASE_PLAYER_TURN;
        deal_card(&state->deck, &state->player_hand);
        deal_card(&state->deck, &state->dealer_hand);
        deal_card(&state->deck, &state->player_hand);
        deal_card(&state->deck, &state->dealer_hand);

        state->player_score = calculate_score(&state->player_hand);
        state->dealer_score = calculate_score(&state->dealer_hand);

        bool player_blackjack = (state->player_score == 21 && state->player_hand.count == 2);
        bool dealer_blackjack = (state->dealer_score == 21 && state->dealer_hand.count == 2);

        if (player_blackjack || dealer_blackjack) {
            if (player_blackjack && dealer_blackjack) {
                strcpy(state->message, "Both have Blackjack! Push.");
            } else if (player_blackjack) {
                int payout = (state->current_bet * 3) / 2;
                state->player_money += payout;
                strcpy(state->message, "Blackjack! You win 3:2.");
            } else {
                state->player_money -= state->current_bet;
                strcpy(state->message, "Dealer has Blackjack!");
            }
            strcat(state->message, " (p) Play Again, (q) Quit");
            state->phase = PHASE_ROUND_OVER;
        } else {
            strcpy(state->message, "Your turn. (h) Hit, (s) Stand.");
        }
        
        bool round_over = false;

        while(!round_over) {
            render_game(state);
            int ch = get_input();

            if (state->phase == PHASE_PLAYER_TURN) {
                if (ch == 'h') {
                    deal_card(&state->deck, &state->player_hand);
                    state->player_score = calculate_score(&state->player_hand);
                    if (state->player_score > 21) {
                        strcpy(state->message, "You busted! Dealer wins.");
                        state->phase = PHASE_DEALER_TURN;
                    }
                } else if (ch == 's') {
                    strcpy(state->message, "Dealer's turn.");
                    state->phase = PHASE_DEALER_TURN;
                } else if (ch == 'q') {
                    round_over = true;
                    running = false;
                } else if (ch == '?' || ch == 'H') {
                    render_help();
                    get_input();
                }
            }            
            if (state->phase == PHASE_DEALER_TURN) {
                render_game(state);

                while (state->dealer_score < 17 ||
                       (state->dealer_score == 17 && is_soft_17(&state->dealer_hand))) {
                    deal_card(&state->deck, &state->dealer_hand);
                    state->dealer_score = calculate_score(&state->dealer_hand);
                    strcpy(state->message, "Dealer is hitting...");
                    render_game(state);
                    sleep(1);
                }

                if (state->player_score > 21) {
                    state->player_money -= state->current_bet;
                } else if (state->dealer_score > 21) {
                    strcpy(state->message, "Dealer busts! You win!");
                    state->player_money += state->current_bet;
                } else if (state->player_score > state->dealer_score) {
                    strcpy(state->message, "You win!");
                    state->player_money += state->current_bet;
                } else if (state->dealer_score > state->player_score) {
                    strcpy(state->message, "Dealer wins!");
                    state->player_money -= state->current_bet;
                } else {
                    strcpy(state->message, "It's a push!");
                }
                
                strcat(state->message, " (p) Play Again, (q) Quit");
                state->phase = PHASE_ROUND_OVER;
            }
            if (state->phase == PHASE_ROUND_OVER) {
                render_game(state);
                
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

    free(state->deck.cards);
    free(state->player_hand.cards);
    free(state->dealer_hand.cards);
    free(state);
}

int main() {
    setlocale(LC_ALL, "");
    init_tui();
    GameState *state = init_game_state();
    play_game(state);
    end_tui();
    return 0;
}
