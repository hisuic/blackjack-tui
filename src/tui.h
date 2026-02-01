#ifndef TUI_H
#define TUI_H

#include "game.h"

// Initializes the TUI
void init_tui();

// Cleans up the TUI
void end_tui();

// Renders the entire game state
void render_game(const CardCollection *player_hand, const CardCollection *dealer_hand, int player_score, int dealer_score, const char *message, bool hide_dealer_card);

// Gets a character from user input
int get_input();

#endif // TUI_H
