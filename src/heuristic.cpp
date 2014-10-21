#include "heuristic.h"
#include <cstdio> // [DEBUG] 

double Heuristic_t::eval(
	const reversi::State_t * state,
	const reversi::Movement_t * actionsB, size_t actionsBCnt,
	const reversi::Movement_t * actionsW, size_t actionsWCnt,
	reversi::Occupancy_t color
){
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
	double hCountVal;
	if (countW + countB == 0) hCountVal = 0;
	else hCountVal = ((double)countW - (double)countB) / (countW + countB);
	

	// 2) Boards weighing different positions
	static const size_t wBoardCnt = 2;
	static const int wBoard[wBoardCnt][8][8] = 
	{
		// [Board 1] Source: http://www.samsoft.org.uk/reversi/strategy.htm
		#define COST_A  99
		#define COST_B  -8
		#define COST_C   8
		#define COST_D   6
		#define COST_E -24
		#define COST_F  -4
		#define COST_G  -3
		#define COST_H   7
		#define COST_I   4
		#define COST_J   0
		{
			{ COST_A, COST_B, COST_C, COST_D, COST_D, COST_C, COST_B, COST_A },
			{ COST_B, COST_E, COST_F, COST_G, COST_G, COST_F, COST_E, COST_B },
			{ COST_C, COST_F, COST_H, COST_I, COST_I, COST_H, COST_F, COST_C },
			{ COST_D, COST_G, COST_I, COST_J, COST_J, COST_I, COST_G, COST_D },
			{ COST_D, COST_G, COST_I, COST_J, COST_J, COST_I, COST_G, COST_D },
			{ COST_C, COST_F, COST_H, COST_I, COST_I, COST_H, COST_F, COST_C },
			{ COST_B, COST_E, COST_F, COST_G, COST_G, COST_F, COST_E, COST_B },
			{ COST_A, COST_B, COST_C, COST_D, COST_D, COST_C, COST_B, COST_A },
		},
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

		// [Board 2] Source: http://kartikkukreja.wordpress.com/2013/03/30/heuristic-function-for-reversiothello/
		#define COST_A 20
		#define COST_B -3
		#define COST_C 11
		#define COST_D  8
		#define COST_E -7
		#define COST_F -4
		#define COST_G  1
		#define COST_H  2
		#define COST_I  2
		#define COST_J -3
		{
			{ COST_A, COST_B, COST_C, COST_D, COST_D, COST_C, COST_B, COST_A },
			{ COST_B, COST_E, COST_F, COST_G, COST_G, COST_F, COST_E, COST_B },
			{ COST_C, COST_F, COST_H, COST_I, COST_I, COST_H, COST_F, COST_C },
			{ COST_D, COST_G, COST_I, COST_J, COST_J, COST_I, COST_G, COST_D },
			{ COST_D, COST_G, COST_I, COST_J, COST_J, COST_I, COST_G, COST_D },
			{ COST_C, COST_F, COST_H, COST_I, COST_I, COST_H, COST_F, COST_C },
			{ COST_B, COST_E, COST_F, COST_G, COST_G, COST_F, COST_E, COST_B },
			{ COST_A, COST_B, COST_C, COST_D, COST_D, COST_C, COST_B, COST_A },
		}
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
	int wBoardCostW[wBoardCnt];
	int wBoardCostB[wBoardCnt];
	for (size_t i = 0; i < wBoardCnt; i++){
		wBoardCostW[i] = 0;
		wBoardCostB[i] = 0;
		for (size_t y = 0; y < 8; y++){
			for (size_t x = 0; x < 8; x++){
				if (board[y][x] == reversi::Occupancy_t::WHITE) 
					wBoardCostW[i] += wBoard[i][y][x];
				else if (board[y][x] == reversi::Occupancy_t::BLACK) 
					wBoardCostB[i] += wBoard[i][y][x];
			}
		}
	}
	double hWBoard[wBoardCnt];
	for (size_t i = 0; i < wBoardCnt; i++){
		if (wBoardCostW[i] + wBoardCostB[i] == 0) hWBoard[i] = 0.0;
		else hWBoard[i] = (double) (wBoardCostW[i] - wBoardCostB[i]) / (wBoardCostW[i] + wBoardCostB[i]);
	}

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

	// 5) Fudge factor to make minmax select corners
	double cornerMastery = 0.0;
	const double cornerBonus = 1.0;
	if (board[0][0] == reversi::Occupancy_t::WHITE) cornerMastery += cornerBonus;
	else if (board[0][0] == reversi::Occupancy_t::BLACK) cornerMastery -= cornerBonus;
	if (board[0][7] == reversi::Occupancy_t::WHITE) cornerMastery += cornerBonus;
	else if (board[0][7] == reversi::Occupancy_t::BLACK) cornerMastery -= cornerBonus;
	if (board[7][0] == reversi::Occupancy_t::WHITE) cornerMastery += cornerBonus;
	else if (board[7][0] == reversi::Occupancy_t::BLACK) cornerMastery -= cornerBonus;
	if (board[7][7] == reversi::Occupancy_t::WHITE) cornerMastery += cornerBonus;
	else if (board[7][7] == reversi::Occupancy_t::BLACK) cornerMastery -= cornerBonus;

	// 6) Fudge factor to try harder to not yield corners
	double cornerCloseness = 0.0, cornerClosenessW = 0.0, cornerClosenessB = 0.0;
	const double borderPenalty = 1.0, innerPenalty = 2.0;
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
	cornerCloseness = cornerClosenessW - cornerClosenessB;

	// Inverts the heuristic values if we are actually placing a black piece
	if (color == reversi::Occupancy_t::BLACK){
		hCountVal *= -1;
		hFrontierSize *= -1;
		cornerCloseness *= -1;
		cornerMastery *= -1;
		for (size_t i = 0; i < wBoardCnt; i++){
			hWBoard[i] *= -1;
		}
	}

	// Does a harmonic average of all heuristics adding one (so we avoid sign flippage)
	static const double weightCount = 10.0, weightFrontier = 75.0, weightCloseness = 5000.0, weightCorner = 10000.0;
	static const double weightBoard[wBoardCnt] = {10.0, 10.0};
	double hVal = weightCount * hCountVal + weightFrontier * hFrontierSize + weightCloseness * cornerCloseness + weightCorner * cornerMastery;
	for (size_t i = 0; i < wBoardCnt; i++) hVal += weightBoard[i] * hWBoard[i];
	return hVal;
}

