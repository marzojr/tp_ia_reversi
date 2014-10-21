#include "heuristic.h"
#include <cstdio> // [DEBUG] 

void Heuristic_t::set(
	const double weightCount, const double weightFrontier, const double weightCloseness,
	const double weightBoard)
{
	this->weightCount = weightCount;
	this->weightFrontier = weightFrontier;  
	this->weightCloseness = weightCloseness; 
	this->weightBoard = weightBoard; 
}

double Heuristic_t::eval(
	const reversi::State_t * state, size_t actionsBCnt, size_t actionsWCnt,
	reversi::Occupancy_t color
) const {
	// Makes an alias for the board being worked on
	const reversi::Occupancy_t (&board)[8][8] = *state->getBoard();

	/* All heuristics are computed on a [-1, 1] range, assuming we are optimizing for white 
	 * We later invert the score if we are actually optimizing for black */

	// 1) Difference in disk counts
	size_t countW = 0, countB = 0;
	for (size_t y = 0; y < 8; y++){
		for (size_t x = 0; x < 8; x++){
			countW += board[y][x] == reversi::Occupancy_t::WHITE;
			countB += board[y][x] == reversi::Occupancy_t::BLACK;
		}
	}
	double hCountVal = (double)countW - (double)countB;
	

	// 2) Boards weighing different positions
	static const int wBoard[8][8] = 
	{
		// [Board 1] Source: http://www.samsoft.org.uk/reversi/strategy.htm
		#define COST_A  99+20
		#define COST_B  -8+-3
		#define COST_C   8+11
		#define COST_D   6+8
		#define COST_E -24+-7
		#define COST_F  -4+-4
		#define COST_G  -3+1
		#define COST_H   7+2
		#define COST_I   4+2
		#define COST_J   0+3
		{ COST_A, COST_B, COST_C, COST_D, COST_D, COST_C, COST_B, COST_A },
		{ COST_B, COST_E, COST_F, COST_G, COST_G, COST_F, COST_E, COST_B },
		{ COST_C, COST_F, COST_H, COST_I, COST_I, COST_H, COST_F, COST_C },
		{ COST_D, COST_G, COST_I, COST_J, COST_J, COST_I, COST_G, COST_D },
		{ COST_D, COST_G, COST_I, COST_J, COST_J, COST_I, COST_G, COST_D },
		{ COST_C, COST_F, COST_H, COST_I, COST_I, COST_H, COST_F, COST_C },
		{ COST_B, COST_E, COST_F, COST_G, COST_G, COST_F, COST_E, COST_B },
		{ COST_A, COST_B, COST_C, COST_D, COST_D, COST_C, COST_B, COST_A },
		#undef COST_A 
		#undef COST_B 
		#undef COST_C 
		#undef COST_D 
		#undef COST_E 
		#undef COST_F 
		#undef COST_G 
		#undef COST_H 
		#undef COST_I 
		#undef COST_J 
	};
	int wBoardCostW = 0;
	int wBoardCostB = 0;
	for (size_t y = 0; y < 8; y++){
		for (size_t x = 0; x < 8; x++){
			if (board[y][x] == reversi::Occupancy_t::WHITE) 
				wBoardCostW += wBoard[y][x];
			else if (board[y][x] == reversi::Occupancy_t::BLACK) 
				wBoardCostB += wBoard[y][x];
		}
	}

	double hWBoard;
	if (wBoardCostW + wBoardCostB == 0) hWBoard = 0.0;
	else hWBoard = (double) (wBoardCostW - wBoardCostB) / (wBoardCostW + wBoardCostB);

	// 3) Relative Frontier sizes
	size_t frontierSizeW = actionsWCnt;
	size_t frontierSizeB = actionsBCnt;
	double hFrontierSize;
	if (frontierSizeW + frontierSizeB == 0) hFrontierSize = 0.0;
	else hFrontierSize = ((double)frontierSizeW - (double)frontierSizeB) / (frontierSizeW + frontierSizeB);

	/* 4) Unflipable coins/stability
	*    A coin can be stable (weight 1) if it is unflippable:
	*        It is a corner

	*        It is a side, and
	*    A coin can be unstable (weight -1) if it can be flipped in the next round
	*    A coin can be semi-stable, if it can't be flipped in the next round
	* Is this actually computable deterministically in polynomial time?
	*/

	// 5) Fudge factor to try harder to not yield corners
	double cornerClosenessW = 0.0, cornerClosenessB = 0.0;
	const double borderPenalty = -1.0, innerPenalty = -2.0;
	if (board[0][0] == reversi::Occupancy_t::EMPTY){
		cornerClosenessW += borderPenalty * (board[0][1] == reversi::Occupancy_t::WHITE);
		cornerClosenessW += innerPenalty  * (board[1][1] == reversi::Occupancy_t::WHITE);
		cornerClosenessW += borderPenalty * (board[1][0] == reversi::Occupancy_t::WHITE);
		cornerClosenessB += borderPenalty * (board[0][1] == reversi::Occupancy_t::BLACK);
		cornerClosenessB += innerPenalty  * (board[1][1] == reversi::Occupancy_t::BLACK);
		cornerClosenessB += borderPenalty * (board[1][0] == reversi::Occupancy_t::BLACK);
	}
	if (board[0][7] == reversi::Occupancy_t::EMPTY){
		cornerClosenessW += borderPenalty * (board[0][6] == reversi::Occupancy_t::WHITE);
		cornerClosenessW += innerPenalty  * (board[1][6] == reversi::Occupancy_t::WHITE);
		cornerClosenessW += borderPenalty * (board[1][7] == reversi::Occupancy_t::WHITE);
		cornerClosenessB += borderPenalty * (board[0][6] == reversi::Occupancy_t::BLACK);
		cornerClosenessB += innerPenalty  * (board[1][6] == reversi::Occupancy_t::BLACK);
		cornerClosenessB += borderPenalty * (board[1][7] == reversi::Occupancy_t::BLACK);
	}
	if (board[7][0] == reversi::Occupancy_t::EMPTY){
		cornerClosenessW += borderPenalty * (board[7][1] == reversi::Occupancy_t::WHITE);
		cornerClosenessW += innerPenalty  * (board[6][1] == reversi::Occupancy_t::WHITE);
		cornerClosenessW += borderPenalty * (board[6][0] == reversi::Occupancy_t::WHITE);
		cornerClosenessB += borderPenalty * (board[7][1] == reversi::Occupancy_t::BLACK);
		cornerClosenessB += innerPenalty  * (board[6][1] == reversi::Occupancy_t::BLACK);
		cornerClosenessB += borderPenalty * (board[6][0] == reversi::Occupancy_t::BLACK);
	}
	if (board[7][7] == reversi::Occupancy_t::EMPTY){
		cornerClosenessW += borderPenalty * (board[6][7] == reversi::Occupancy_t::WHITE);
		cornerClosenessW += innerPenalty  * (board[6][6] == reversi::Occupancy_t::WHITE);
		cornerClosenessW += borderPenalty * (board[7][6] == reversi::Occupancy_t::WHITE);
		cornerClosenessB += borderPenalty * (board[6][7] == reversi::Occupancy_t::BLACK);
		cornerClosenessB += innerPenalty  * (board[6][6] == reversi::Occupancy_t::BLACK);
		cornerClosenessB += borderPenalty * (board[7][6] == reversi::Occupancy_t::BLACK);
	}
	double hCornerCloseness = cornerClosenessW - cornerClosenessB;

	// Inverts the heuristic values if we are actually placing a black piece
	if (color == reversi::Occupancy_t::BLACK){
		hCountVal *= -1;
		hFrontierSize *= -1;
		hCornerCloseness *= -1;
		hWBoard *= -1;
	}
	
	// [DEBUG] Print out the heuristic values
	/*fprintf(stderr, "countW: %zu; countB: %zu\n", countW, countB);
	fprintf(stderr, "wBoardCostW: %i; wBoardCostB: %i\n", wBoardCostW, wBoardCostB);
	fprintf(stderr, "frontierSizeW: %zu; frontierSizeB: %zu\n", frontierSizeW, frontierSizeB);
	fprintf(stderr, "cornerClosenessW: %f; cornerClosenessB:  %f\n", cornerClosenessW, cornerClosenessB);	
	*/

	// Does a harmonic average of all heuristics adding one (so we avoid sign flippage)
	double hVal = weightCount * hCountVal + weightFrontier * hFrontierSize + 
		weightCloseness * hCornerCloseness + weightBoard * hWBoard;
	return hVal;
}

