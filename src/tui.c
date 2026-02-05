#include "tui.h"
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>

static const char *SUITS[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
static const char *RANKS[] = {"?", "?", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
enum ColorPairId {
    CP_TEXT = 1,
    CP_CARD_RED = 2,
    CP_STATUS_SUCCESS = 3,
    CP_STATUS_INFO = 4,
    CP_TITLE = 5,
    CP_PANEL_BORDER = 6,
    CP_PANEL_HEADING = 7,
    CP_STATUS_LOSS = 8
};
static const char *TITLE_ART[6] = {
    "__________.__                 __        ____.              __     _______________ ___.___ ",
    "\\______   \\  | _____    ____ |  | __   |    |____    ____ |  | __ \\__    ___/    |   \\   |",
    " |    |  _/  | \\__  \\ _/ ___\\|  |/ /   |    \\__  \\ _/ ___\\|  |/ /   |    |  |    |   /   |",
    " |    |   \\  |__/ __ \\\\  \\___|    </\\__|    |/ __ \\\\  \\___|    <    |    |  |    |  /|   |",
    " |______  /____(____  /\\___  >__|_ \\________(____  /\\___  >__|_ \\   |____|  |______/ |___|",
    "        \\/          \\/     \\/     \\/             \\/     \\/     \\/                         "
};

void init_tui() {
    initscr();            // Start curses mode
    cbreak();             // Line buffering disabled
    noecho();             // Don't echo() while we do getch
    keypad(stdscr, TRUE); // Enable function keys
    curs_set(0);          // Hide the cursor
    start_color();        // Enable color
    use_default_colors();
    init_pair(CP_TEXT, COLOR_WHITE, -1);
    init_pair(CP_CARD_RED, COLOR_RED, -1);
    init_pair(CP_STATUS_SUCCESS, COLOR_GREEN, -1);
    init_pair(CP_STATUS_INFO, COLOR_YELLOW, -1);
    init_pair(CP_TITLE, COLOR_CYAN, -1);
    init_pair(CP_PANEL_BORDER, COLOR_BLUE, -1);
    init_pair(CP_PANEL_HEADING, COLOR_MAGENTA, -1);
    init_pair(CP_STATUS_LOSS, COLOR_RED, -1);
    bkgd(COLOR_PAIR(CP_TEXT));
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
    attron(COLOR_PAIR(CP_PANEL_BORDER));
    draw_box(start_y, start_x, width, height);
    attroff(COLOR_PAIR(CP_PANEL_BORDER));

    attron(COLOR_PAIR(CP_PANEL_HEADING) | A_BOLD);
    if (hand->count == 0) {
        mvprintw(start_y + 1, start_x + 2, "%s (Score: -)", title);
    } else if (hide_first_card) {
        mvprintw(start_y + 1, start_x + 2, "%s (Score: ?)", title);
    } else {
        mvprintw(start_y + 1, start_x + 2, "%s (Score: %d)", title, score);
    }
    attroff(COLOR_PAIR(CP_PANEL_HEADING) | A_BOLD);

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
                attron(COLOR_PAIR(CP_CARD_RED));
            }
            mvprintw(start_y + 2 + i, start_x + 2, "[%s %s]", RANKS[card.rank], SUITS[card.suit]);
            if (is_red) {
                attroff(COLOR_PAIR(CP_CARD_RED));
            }
        }
    }
}

static void render_title_art(int cols) {
    attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
    for (int i = 0; i < 6; ++i) {
        int line_len = (int)strlen(TITLE_ART[i]);
        int title_x = (cols - line_len) / 2;
        if (title_x < 1) {
            title_x = 1;
        }
        mvprintw(1 + i, title_x, "%s", TITLE_ART[i]);
    }
    attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);
}

static void render_betting_controls(int start_y, int start_x, int width) {
    attron(COLOR_PAIR(CP_PANEL_BORDER));
    draw_box(start_y, start_x, width, 7);
    attroff(COLOR_PAIR(CP_PANEL_BORDER));
    attron(COLOR_PAIR(CP_PANEL_HEADING) | A_BOLD);
    mvprintw(start_y + 1, start_x + 2, "Bet Amount");
    mvprintw(start_y + 4, start_x + 2, "Actions");
    attroff(COLOR_PAIR(CP_PANEL_HEADING) | A_BOLD);
    mvprintw(start_y + 2, start_x + 4, "[1] +$10   [2] +$50   [3] +$100   [4] +$500   [a] All In   [c] Clear");
    mvprintw(start_y + 5, start_x + 4, "[b] Deal   [q] Quit   [?] Help");
}

static void render_turn_controls(int start_y, int start_x, int width) {
    attron(COLOR_PAIR(CP_PANEL_BORDER));
    draw_box(start_y, start_x, width, 3);
    attroff(COLOR_PAIR(CP_PANEL_BORDER));
    mvprintw(start_y + 1, start_x + 2, "[h] Hit   [s] Stand   [d] Double   [q] Quit   [?] Help");
}

static int status_color_for_message(const char *message) {
    if (strstr(message, "Dealer busts") != NULL) {
        return CP_STATUS_SUCCESS;
    }
    if (strstr(message, "bust") != NULL || strstr(message, "busted") != NULL ||
        strstr(message, "Dealer wins") != NULL || strstr(message, "Game Over") != NULL) {
        return CP_STATUS_LOSS;
    }
    if (strstr(message, "win") != NULL || strstr(message, "Blackjack") != NULL || strstr(message, "push") != NULL) {
        return CP_STATUS_SUCCESS;
    }
    return CP_STATUS_INFO;
}

static void render_status_message(int start_y, int start_x, int width, const char *message) {
    attron(COLOR_PAIR(CP_PANEL_BORDER));
    draw_box(start_y, start_x, width, 3);
    attroff(COLOR_PAIR(CP_PANEL_BORDER));
    int color_pair = status_color_for_message(message);
    attron(COLOR_PAIR(color_pair) | A_BOLD);
    mvprintw(start_y + 1, start_x + 2, "%s", message);
    attroff(COLOR_PAIR(color_pair) | A_BOLD);
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

    render_title_art(cols);

    int title_height = 6;
    int hand_y = 1 + title_height + 1;
    int player_x = start_x;
    int dealer_x = start_x + column_width + column_gap;
    int hand_box_height = 13; // top/bottom border + title + 10 card rows
    int status_y = hand_y + hand_box_height + 1;
    int message_y = status_y + 2;
    int controls_y = message_y + 4;
    int controls_x = start_x;
    int controls_width = total_width;

    // Render hands
    render_hand_box(hand_y, player_x, column_width, hand_box_height, &state->player_hand, state->player_score, "Player", false);
    render_hand_box(hand_y, dealer_x, column_width, hand_box_height, &state->dealer_hand, state->dealer_score, "Dealer", hide_dealer_card);

    // Render money and bet
    attron(COLOR_PAIR(CP_PANEL_HEADING) | A_BOLD);
    mvprintw(status_y, player_x, "Money: $%d", state->player_money);
    mvprintw(status_y, dealer_x, "Bet: $%d", state->current_bet);
    attroff(COLOR_PAIR(CP_PANEL_HEADING) | A_BOLD);

    // Render message
    render_status_message(message_y, controls_x, controls_width, state->message);

    // Render instructions
    if (state->phase == PHASE_BETTING) {
        render_betting_controls(controls_y, controls_x, controls_width);
    } else {
        render_turn_controls(controls_y, controls_x, controls_width);
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
