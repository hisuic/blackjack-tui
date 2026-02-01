#ifndef TUI_H
#define TUI_H

#include <stdbool.h>
#include "game.h"

// Initializes the TUI
void init_tui();

// Cleans up the TUI
void end_tui();

// Renders the entire game state
void render_game(const GameState *state);

// Renders the help screen
void render_help();

// Gets a character from user input
int get_input();

#endif // TUI_H
