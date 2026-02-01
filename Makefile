CC=gcc
CFLAGS=-Wall -Wextra -std=c11
LDFLAGS=-lncurses

SRC=src/main.c src/game.c src/tui.c
OBJ=$(SRC:.c=.o)
TARGET=blackjack

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
