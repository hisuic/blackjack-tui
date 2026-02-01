# TUI Blackjack

[日本語版](./README.ja.md)

Simple terminal Blackjack built with C and ncurses.

## About this project
- A TUI game that lets you play Blackjack in the terminal.
- Uses a single 52-card deck and automatically reshuffles when the deck gets low.
- Playable with keyboard input only.

## Usage
### Build
```bash
make
```

### Run
```bash
./blackjack
```

### Controls
- Betting phase
  - `1` $10 / `2` $50 / `3` $100 / `4` $500
  - `c` clear bet
  - `a` all-in
  - `b` deal (start the round)
- During play
  - `h` hit (draw a card)
  - `s` stand (end your turn)
  - `d` double (double bet + draw one card and end)
  - `?` help
  - `q` quit
- After the round
  - `p` play again
  - `q` quit

## Rules (project-specific)
### Basics
- The goal is to get a higher total than the dealer without going over 21.
- Number cards are their value, J/Q/K are worth 10, A is worth 1 or 11.
- Starting bankroll is $1000.

### Dealer behavior
- Hits on totals under 17.
- Also hits on soft 17 (A counted as 11 for a total of 17).
- Keeps the first card face-down during the player's turn.

### Outcomes and payouts
- Blackjack (21 with the first two cards) pays 3:2.
- Ties are a push.
- Player bust is an immediate loss.

### Available / unavailable actions
- Double down: available (first two cards only, if you have enough bankroll).
- Split: not available.
- Surrender: not available.
- Insurance: not available.

### Deck
- Single 52-card deck.
- Automatically switches to a fresh deck and shuffles when fewer than 13 cards remain.

## Notes for play
- You cannot start a round with a $0 bet.
- If your bankroll reaches $0, you get "Game Over" and can `r` to reset or `q` to quit.
