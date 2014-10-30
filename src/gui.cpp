#include "gui.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Allow enabling a "graphical user-interface" with gcc -DTEXTGUI
#ifdef TEXTGUI
#define ANSI_CONTROL_CODES 1
#define INTERACTIVE_INPUT 1
#else
#define ANSI_CONTROL_CODES 0
#define INTERACTIVE_INPUT 0
#endif

#if INTERACTIVE_INPUT
// The following is NOT STANDARD C and will not port to other operating systems.
#include <termios.h>
void disableInputBuffering() {
	struct termios term;
	tcgetattr(0, &term);
	term.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(0, TCSANOW, &term);
	setbuf(stdin, NULL);
}
void enableInputBuffering() {
	struct termios term;
	tcgetattr(0, &term);
	term.c_lflag |= ICANON | ECHO;
	tcsetattr(0, TCSAFLUSH, &term);
	setbuf(stdin, NULL);
}
#endif

void guiInit() {
#if INTERACTIVE_INPUT
	atexit(enableInputBuffering);
	disableInputBuffering();
#endif
}

#if ANSI_CONTROL_CODES
#define ANSIESC "\x1B["
/** This function prints out an ANSI control sequences for reseting to default colors,
*   then clearing the screen and moving the cursor to top-left corner. **/
void guiClearScreen(void) { printf(ANSIESC "0m" ANSIESC "2J" ANSIESC "1;1H"); }

/** This function prints out an ANSI color sequence for coloring the text output that follows. **/
static void printColor(enum Color color) {
	printf(ANSIESC);
	switch (color) {
	  case COLOR_EMPTY: printf("0;1;42;30m"); break; // grey on green
	  case COLOR_PLAYER1: printf("0;42;30m"); break; // black on green
	  case COLOR_PLAYER1_HILIGHT: printf("0;5;42;30m"); break; // black on bright green
	  case COLOR_PLAYER2: printf("0;1;42;37m"); break; // white on green
	  case COLOR_PLAYER2_HILIGHT: printf("1;5;42;37m"); break; // white on bright green
	  case COLOR_COORD: printf("0m"); break; // default color
	  case COLOR_OTHER: printf("0m"); break; // 0 resets back to default color (usually non-bright white)
	}
}
#else
void guiClearScreen(void) { putchar('\n'); }
static void printColor() {}
#endif

static void printMarker(int marker, bool hilight) {
	/*switch (marker) {
	  case 0: printColor(COLOR_EMPTY); putchar('-'); break;
	  case 1: printColor(hilight ? COLOR_PLAYER1_HILIGHT : COLOR_PLAYER1); putchar('1'); break;
	  case 2: printColor(hilight ? COLOR_PLAYER2_HILIGHT : COLOR_PLAYER2); putchar('2'); break;
	  default: printColor(COLOR_OTHER); putchar('?'); break;
	}*/
}

/** Print the X coordinate axel. **/
static void printCoordX(int x, int x2) {
	printf("   ");
	for (; x <= x2; ++x) {
		printf("%c ", 'a' + x);
	}
	puts("  ");
}

void guiPrintBoard(struct Game const* game, struct Coord cursor, enum Color cursorColor) {
	size_t width = gameWidth(game), height = gameHeight(game);
#if ANSI_CONTROL_CODES
	printf(ANSIESC "1;1H"); // Move cursor to top-left corner
#endif
	/*printColor(COLOR_COORD);
	printCoordX(0, width - 1);
	unsigned int p1score = 0, p2score = 0;
	for (int y = 0; y < (int)height; ++y) {
		printf("%c ", '1' + y);
		for (int x = 0; x < (int)width; ++x) {
			struct Coord pos = coord(x, y);
			bool cursorIsHere = (x == cursor.x && y == cursor.y);
			if (cursorIsHere) { printColor(cursorColor); putchar('>'); }
			else if (x == 0) { printColor(COLOR_EMPTY); putchar(' '); }
			int type = gameSquareType(game, pos);
			if (type == 1) ++p1score;
			if (type == 2) ++p2score;
			printMarker(type, gameSquareHilighted(game, pos));
			if (cursorIsHere) {
				printColor(cursorColor); putchar('<');
			} else if (x + 1 != cursor.x || y != cursor.y) {
				putchar(' ');
			}
		}
		printColor(COLOR_COORD); printf(" %c\n", '1' + y);
	}
	printCoordX(0, width - 1);
	printColor(COLOR_OTHER);
	printf("Score: %u - %u\n", p1score, p2score);*/
}

struct Coord guiHumanPlay(struct Game* game) {
	struct Coord c = gameLastCoord(game);
#if INTERACTIVE_INPUT
	int width = gameWidth(game), height = gameHeight(game);
	int ch;
	do {
		guiPrintBoard(game, c, gameCurrentPlayer(game));
		printf("\nInput coordinates: %c%c" ANSIESC "K", 'a' + c.x, '1' + c.y);
		ch = getchar();
		if (ch == EOF) { puts("EOF from keyboard!"); exit(EXIT_FAILURE); }
		// Handle escape sequences
		if (ch == '\x1B') {
			ch = getchar();
			// Two times ESC key = exit program
			if (ch == '\x1B') { puts("\nExiting..."); exit(EXIT_SUCCESS); }
			// Special key sequences begin with ESC[
			if (ch == '[') {
				switch (getchar()) {
				  case 'A': // Up
					if (c.y > 0) --c.y;
					break;
				  case 'B': // Down
					if (c.y < height - 1) ++c.y;
					break;
				  case 'C': // Right
					if (c.x < width - 1) ++c.x;
					break;
				  case 'D': // Left
					if (c.x > 0) --c.x;
					break;
				}
			}
		}
	} while (ch != '\n' || gameNumFlips(game, c) == 0);
	putchar('\n');
#else
	while (printf("Input coordinates: "), c.x = getchar() - 'a', c.y = getchar() - '1', gameNumFlips(game, c) == 0) {
		while (getchar() != '\n');
		guiPrintBoard(game, c, (Color)gameCurrentPlayer(game));
		puts("Invalid input, try again");
	}
#endif
	return c;
}
