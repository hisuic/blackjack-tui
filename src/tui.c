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

static void draw_box(int start_y, int start_x, int width, int height) {
    for (int x = start_x + 1; x < start_x + width - 1; ++x) {
        mvaddch(start_y, x, ACS_HLINE);
        mvaddch(start_y + height - 1, x, ACS_HLINE);
    }

    for (int y = start_y + 1; y < start_y + height - 1; ++y) {
        mvaddch(y, start_x, ACS_VLINE);
        mvaddch(y, start_x + width - 1, ACS_VLINE);
    }

    mvaddch(start_y, start_x, ACS_ULCORNER);
    mvaddch(start_y, start_x + width - 1, ACS_URCORNER);
    mvaddch(start_y + height - 1, start_x, ACS_LLCORNER);
    mvaddch(start_y + height - 1, start_x + width - 1, ACS_LRCORNER);
}

static void render_hand_box(int start_y, int start_x, int width, int height, const CardCollection *hand, int score, const char *title, bool hide_first_card) {
    draw_box(start_y, start_x, width, height);

    if (hide_first_card) {
        mvprintw(start_y + 1, start_x + 2, "%s (Score: ?)", title);
    } else {
        mvprintw(start_y + 1, start_x + 2, "%s (Score: %d)", title, score);
    }

    int max_card_rows = height - 3; // title + cards + border
    int cards_to_draw = hand->count;
    if (cards_to_draw > max_card_rows) {
        cards_to_draw = max_card_rows;
    }

    for (int i = 0; i < cards_to_draw; ++i) {
        if (hide_first_card && i == 0) {
            mvprintw(start_y + 2 + i, start_x + 2, "[?]");
        } else {
            Card card = hand->cards[i];
            bool is_red = (card.suit == HEARTS || card.suit == DIAMONDS);
            if (is_red) {
                attron(COLOR_PAIR(2));
            }
            mvprintw(start_y + 2 + i, start_x + 2, "[%s %s]", RANKS[card.rank], SUITS[card.suit]);
            if (is_red) {
                attroff(COLOR_PAIR(2));
            }
        }
    }
}

void render_game(const GameState *state) {
    clear();

    bool hide_dealer_card = (state->phase == PHASE_PLAYER_TURN);
    int cols;
    int screen_rows;
    getmaxyx(stdscr, screen_rows, cols);
    (void)screen_rows;

    int side_margin = 2;
    int column_gap = 4;
    int usable_width = cols - (side_margin * 2) - column_gap;
    int column_width = usable_width / 2;
    if (column_width < 18) {
        column_width = 18;
    }

    int total_width = (column_width * 2) + column_gap;
    int start_x = (cols - total_width) / 2;
    if (start_x < 1) {
        start_x = 1;
    }

    int hand_y = 2;
    int player_x = start_x;
    int dealer_x = start_x + column_width + column_gap;
    int hand_box_height = 13; // top/bottom border + title + 10 card rows
    int status_y = hand_y + hand_box_height + 1;
    int message_y = status_y + 2;
    int controls_y = message_y + 2;

    // Render hands
    render_hand_box(hand_y, player_x, column_width, hand_box_height, &state->player_hand, state->player_score, "Player", false);
    render_hand_box(hand_y, dealer_x, column_width, hand_box_height, &state->dealer_hand, state->dealer_score, "Dealer", hide_dealer_card);

    // Render money and bet
    mvprintw(status_y, player_x, "Money: $%d", state->player_money);
    mvprintw(status_y, dealer_x, "Bet: $%d", state->current_bet);

    // Render message
    mvprintw(message_y, player_x, "%s", state->message);

    // Render instructions
    if (state->phase == PHASE_BETTING) {
        mvprintw(controls_y, player_x, "Bet Amount: 1:$10  2:$50  3:$100  4:$500");
        mvprintw(controls_y + 1, player_x, "Actions: (c) Clear  (a) All In  (b) Deal  (q) Quit  (?) Help");
    } else {
        mvprintw(controls_y, player_x, "(h) Hit  (s) Stand  (d) Double  (q) Quit  (?) Help");
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
    mvprintw(10, 4, "(d) - Double: Double the bet, take one card, then stand.");
    mvprintw(11, 4, "(q) - Quit: Exit the game.");
    mvprintw(12, 4, "(?) - Help: Show this help screen.");

    mvprintw(15, 2, "Press any key to return to the game.");
    refresh();
}

int get_input() {
    return getch();
}
