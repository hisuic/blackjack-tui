#include "tui.h"
#include <ncurses.h>
#include <stdbool.h>

static const char *SUITS[] = {"H", "D", "C", "S"};
static const char *RANKS[] = {"?", "?", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

void init_tui() {
    initscr();            // Start curses mode
    cbreak();             // Line buffering disabled
    noecho();             // Don't echo() while we do getch
    keypad(stdscr, TRUE); // Enable function keys
    curs_set(0);          // Hide the cursor
    start_color();        // Enable color
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
}

void end_tui() {
    endwin(); // End curses mode
}

static void render_hand(int start_y, int start_x, const CardCollection *hand, int score, const char *title, bool hide_first_card) {
    if (hide_first_card) {
        mvprintw(start_y, start_x, "%s's Hand (Score: ?)", title);
    } else {
        mvprintw(start_y, start_x, "%s's Hand (Score: %d)", title, score);
    }
    for (int i = 0; i < hand->count; ++i) {
        if (hide_first_card && i == 0) {
            mvprintw(start_y + 1 + i, start_x, "[?]");
        } else {
            Card card = hand->cards[i];
            bool is_red = (card.suit == HEARTS || card.suit == DIAMONDS);
            if (is_red) {
                attron(COLOR_PAIR(2));
            }
            mvprintw(start_y + 1 + i, start_x, "[%s %s]", RANKS[card.rank], SUITS[card.suit]);
            if (is_red) {
                attroff(COLOR_PAIR(2));
            }
        }
    }
}

void render_game(const GameState *state) {
    clear();

    bool hide_dealer_card = (state->phase == PHASE_PLAYER_TURN);

    // Render hands
    render_hand(2, 2, &state->player_hand, state->player_score, "Player", false);
    render_hand(2, 30, &state->dealer_hand, state->dealer_score, "Dealer", hide_dealer_card);

    // Render money and bet
    mvprintw(13, 2, "Money: $%d", state->player_money);
    mvprintw(13, 30, "Bet: $%d", state->current_bet);

    // Render message
    mvprintw(15, 2, "%s", state->message);

    // Render instructions
    if (state->phase == PHASE_BETTING) {
        mvprintw(17, 2, "1: $10 | 2: $50 | 3: $100 | 4: $500      (c) Clear | (a) All In | (b) Deal");
    } else {
        mvprintw(17, 2, "(h) Hit  (s) Stand  (q) Quit  (?) Help");
    }

    refresh();
}

void render_help() {
    clear();
    mvprintw(2, 2, "== Blackjack Help ==");
    mvprintw(4, 2, "The goal of Blackjack is to beat the dealer's hand without going over 21.");
    mvprintw(5, 2, "Face cards (J, Q, K) are worth 10. Aces are worth 1 or 11.");
    
    mvprintw(7, 2, "Controls:");
    mvprintw(8, 4, "(h) - Hit: Take another card.");
    mvprintw(9, 4, "(s) - Stand: Hold your total and end your turn.");
    mvprintw(10, 4, "(q) - Quit: Exit the game.");
    mvprintw(11, 4, "(?) - Help: Show this help screen.");

    mvprintw(14, 2, "Press any key to return to the game.");
    refresh();
}

int get_input() {
    return getch();
}
