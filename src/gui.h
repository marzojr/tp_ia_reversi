#ifndef TKK_AS_C_GUI_H
#define TKK_AS_C_GUI_H

#include "game.h"

enum Color {
	COLOR_EMPTY = 0,
	COLOR_PLAYER1 = 1,
	COLOR_PLAYER2 = 2,
	COLOR_COORD,
	COLOR_OTHER,
	COLOR_PLAYER1_HILIGHT = -1,
	COLOR_PLAYER2_HILIGHT = -2
};

void guiInit();
void guiClearScreen(void);
void guiPrintBoard(struct Game const* game, struct Coord cursor, enum Color cursorColor);
struct Coord guiHumanPlay(struct Game* game);

#endif
