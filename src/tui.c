#include "tui.h"
#include <ncurses.h>
#include <stdbool.h>

static const char *SUITS[] = {"♥", "♦", "♣", "♠"};
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
    mvprintw(start_y, start_x, "%s's Hand (Score: %d)", title, score);
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

void render_game(const CardCollection *player_hand, const CardCollection *dealer_hand, int player_score, int dealer_score, const char *message, bool hide_dealer_card) {
    clear();

    // Render hands
    render_hand(2, 2, player_hand, player_score, "Player", false);
    render_hand(2, 30, dealer_hand, dealer_score, "Dealer", hide_dealer_card);

    // Render message
    mvprintw(15, 2, "%s", message);

    // Render instructions
    mvprintw(17, 2, "(h) Hit  (s) Stand  (q) Quit");

    refresh();
}


int get_input() {
    return getch();
}
