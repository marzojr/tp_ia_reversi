#include "heuristic.h"

double Heuristic_t::eval(
	const reversi::State_t * state,
	const std::vector<reversi::Movement_t> & actionsB,
	const std::vector<reversi::Movement_t> & actionsW, 
	reversi::Occupancy_t color
){
	return 42;
	// Makes an alias for the board being worked on
	/*const reversi::Occupancy_t (&board)[8][8] = *state->getBoard();

	// 1) Difference in disk counts
	size_t countW = 0, countB = 0;
	for (size_t y = 0; y < 8; y++){
		for (size_t x = 0; x < 8; x++){
			countW += board[y][x] == reversi::Occupancy_t::WHITE;
			countB += board[y][x] == reversi::Occupancy_t::BLACK;
		}
	}
	

	// 2) Boards weighing different positions
	const size_t wBoardCnt = 2;
	const int wBoard[wBoardCnt][8][8] = 
	{
		// [Board 1] Source: http://www.samsoft.org.uk/reversi/strategy.htm
		#define COST_A 99
		#define COST_B -8
		#define COST_C 8
		#define COST_D 6
		#define COST_E -24
		#define COST_F -4
		#define COST_G -3
		#define COST_H 7
		#define COST_I 4
		#define COST_J 0
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

		// [Board 2] Source: http://kartikkukreja.wordpress.com/2013/03/30/heuristic-function-for-reversiothello/
		#define COST_A 20
		#define COST_B -3
		#define COST_C 11
		#define COST_D 8
		#define COST_E -7
		#define COST_F -4
		#define COST_G 1
		#define COST_H 2
		#define COST_I 2
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

	};
	int wBoardCost[wBoardCnt];
	for (size_t i = 0; i < wBoardCnt; i++){
		for (size_t y = 0; y < 8; y++){
			for (size_t x = 0; x < 8; x++){
				if (board[y][x] == reversi::Occupancy_t::WHITE) wBoardCost[i] += wBoard[i][y][x];
				else if (board[y][x] == reversi::Occupancy_t::BLACK) wBoardCost[i] -= wBoard[i][y][x];
	}}}*/

	// 3) Relative Frontier sizes
	size_t frontierSizeW = actionsW.size();
	size_t frontierSizeB = actionsB.size();

	/* 4) Unflipable coins/stability
	*    A coin can be stable (weight 1) if it is unflippable:
	*        It is a corner
	*        It is a side, and
	*    A coin can be unstable (weight -1) if it can be flipped in the next round
	*    A coin can be semi-stable, if it can't be flipped in the next round
	* Is this actually computable deterministically in polynomial time?
	*/


	//printf("CountW: %i; CountB: %i\n", countW, countB);
	//printf("Board cost (0): %i; (1): %i\n", wBoardCost[0], wBoardCost[1]);



	return 42;
}

