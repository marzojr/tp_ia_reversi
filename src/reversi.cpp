#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "reversi.h"

namespace reversi{
	std::string Movement_t::toString() const{
		std::stringstream ss;
		ss << static_cast<int>(x) << ',' << static_cast<int>(y);
		return ss.str();
	}
	
	State_t::State_t(const State_t * parent, const Movement_t * movement, Occupancy_t color){
		// First, duplicate the parent.
		for (unsigned ii = 0; ii < 8; ii++){
			for (unsigned jj = 0; jj < 8; jj++){
				board[ii][jj] = parent->board[ii][jj];
			}
		}
		board[movement->y][movement->x] = color;
		Occupancy_t otherclr = oppositeColor(color);
		// Radiate outwards from point looking for contiguous runs of the
		// opposing color followed by the source color.
		static const int incX[8] = { 1, 1, 0, -1, -1, -1, 0, 1};
		static const int incY[8] = { 0, 1, 1, 1, 0, -1, -1, -1};
		for (unsigned ii = 0; ii < 8; ii++){
			const int dX = incX[ii], dY = incY[ii];
			int cX = movement->x + dX, cY = movement->y + dY;
			while (cX >= 0 && cX < 8 && cY >= 0 && cY < 8 && parent->board[cY][cX] == otherclr) {
				cX += dX;
				cY += dY;
			}
			if (cX >= 0 && cX < 8 && cY >= 0 && cY < 8 && parent->board[cY][cX] == color) {
				cX = movement->x + dX;
				cY = movement->y + dY;
				while (cX >= 0 && cX < 8 && cY >= 0 && cY < 8 && parent->board[cY][cX] == otherclr) {
					board[cY][cX] = color;
					cX += dX;
					cY += dY;
				}
			}
		}
	}

	void State_t::init(std::istream & conf){
		for (unsigned i = 0; i < 8; i++){
			for (unsigned j = 0; j < 8; j++){
				
				// Load next character from stream
				if (conf.eof()){
					fprintf(stderr, "ERROR: Unexpected end of input! Expected an 8*8 board, got only %i valid inputs.\n", i*8 + j);
					exit(-1);
				}
				char c; conf >> c;
				
				// Decode string into a board value and register it
				switch (c){
				case 'w': case 'W': board[i][j] = Occupancy_t::WHITE; break;
				case 'b': case 'B': board[i][j] = Occupancy_t::BLACK; break;
				case '.': board[i][j] = Occupancy_t::EMPTY; break;
				case '\r': case '\n': case ' ': break;
				default: 
					fprintf(stderr, "ERROR: Unexpected character '%c' in input stream!\n", c);
					exit(-1);
				}
			}
		}

	}

	State_t::State_t(const char * conf){
		std::istringstream ss(conf);
		init(ss);
	}

	State_t::State_t(std::istream & conf){
		init(conf);
	}

	std::string State_t::toString() const{
		std::stringstream ss;
		for (size_t i = 0; i < 8; i++){
			for (size_t j = 0; j < 8; j++){
				switch (board[i][j]){
				case Occupancy_t::BLACK: ss << 'B'; break;
				case Occupancy_t::WHITE: ss << 'W'; break;
				case Occupancy_t::EMPTY: ss << '.'; break;
				default: ss << '!'; break;
				}
			}
			ss << '\n';
		}
		return ss.str();
	}

	std::string State_t::toString(bool flipMain, bool flipRev) const{
		std::stringstream ss;
		if (flipMain && flipRev){
			// Flipped along both diagonals (180 degree rotation)
			for (size_t i = 0; i < 8; i++){
				for (size_t j = 0; j < 8; j++){
					switch (board[7 - i][7 - j]){
					case Occupancy_t::BLACK: ss << 'B'; break;
					case Occupancy_t::WHITE: ss << 'W'; break;
					case Occupancy_t::EMPTY: ss << '.'; break;
					}
				}
			}
		} else if (flipMain){
			// Flipped along main diagonal.
			for (size_t i = 0; i < 8; i++){
				for (size_t j = 0; j < 8; j++){
					switch (board[j][i]){
					case Occupancy_t::BLACK: ss << 'B'; break;
					case Occupancy_t::WHITE: ss << 'W'; break;
					case Occupancy_t::EMPTY: ss << '.'; break;
					}
				}
			}
		} else if(flipRev){
			// Flipped along reverse diagonal
			for (size_t i = 0; i < 8; i++){
				for (size_t j = 0; j < 8; j++){
					switch (board[7 - j][7 - i]){
					case Occupancy_t::BLACK: ss << 'B'; break;
					case Occupancy_t::WHITE: ss << 'W'; break;
					case Occupancy_t::EMPTY: ss << '.'; break;
					}
				}
			}
		} else {
			// Unflipped. Different from toString to reduce length.
			for (size_t i = 0; i < 8; i++){
				for (size_t j = 0; j < 8; j++){
					switch (board[i][j]){
					case Occupancy_t::BLACK: ss << 'B'; break;
					case Occupancy_t::WHITE: ss << 'W'; break;
					case Occupancy_t::EMPTY: ss << '.'; break;
					}
				}
			}
		}
		return ss.str();
	}
	
	// Returns an immutable pointer to the board
	const Occupancy_t (*State_t::getBoard() const)[8][8] {
		return &board;
	}

	// Expand computes all moves that can be done on the current board, through 8 passes
	void State_t::expand(std::vector<Movement_t> & actionsB, std::vector<Movement_t> & actionsW) const{

		// We define the starting x and y, as well as the increment for every row/column/diagonal in each direction
		static const int startX[8][16] = {
			{ 0, 0, 0, 0, 0, 0, 0, 0, -1}, // E (>) 
			{ 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, -1}, // SE 
			{ 0, 1, 2, 3, 4, 5, 6, 7, -1 }, // S (V)
			{ 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, -1 }, // SW
			{ 7, 7, 7, 7, 7, 7, 7, 7, -1 }, // W (<)
			{ 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, -1 }, // NW
			{ 0, 1, 2, 3, 4, 5, 6, 7, -1 }, // N (^)
			{ 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, -1 }, // NE
		};
		static const int startY[8][16] = {
			{ 0, 1, 2, 3, 4, 5, 6, 7, -1 }, // E (>)
			{ 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, -1 }, // SE
			{ 0, 0, 0, 0, 0, 0, 0, 0, -1 }, // S (V)
			{ 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, -1 }, // SW
			{ 0, 1, 2, 3, 4, 5, 6, 7, -1 }, // W (<)
			{ 7, 7, 7, 7, 7, 7, 7, 6, 5, 4, 3, 2, 1, -1 }, // NW
			{ 7, 7, 7, 7, 7, 7, 7, -1 }, // N(^)
			{ 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, -1 }, // NE
		};
		static const int incX[8] = { 1, 1, 0, -1, -1, -1, 0, 1};
		static const int incY[8] = { 0, 1, 1, 1, 0, -1, -1, -1};

		// Auxiliary where we store if a position can be black or white
		uint8_t possibleBoard[8][8];
		memset(possibleBoard, 0x00, 8 * 8 * sizeof(uint8_t));
		const uint8_t possibleW = 0x01;
		const uint8_t possibleB = 0x02;

		// Iterate over each direction and row/column/diagonal, computing placements
		for (size_t i = 0; i < 8; i++){
			for (size_t j = 0; ; j++){
				
				// Load the start of the new search
				int x = startX[i][j];
				int y = startY[i][j];
				const int ix = incX[i];
				const int iy = incY[i];

				// If the sentinel value was found, all elements in this direction were searched
				if (x == -1 || y == -1) break;

				/* Compute empty cells as follows:
				 * 1) If the current cell is empty, store its address
				 * 2) Store the opposite color to that of the previous cell; Initially set to empty
				 * 3) If the color of the current cell is the same as the previous, and neither is empty, the last empty cell can be this color
				 */
				uint8_t * lastEmpty = NULL;
				Occupancy_t placeColor = Occupancy_t::EMPTY;
				do{
					Occupancy_t color = board[y][x];
					if (color == Occupancy_t::EMPTY) lastEmpty = &possibleBoard[y][x];
					else if (color == placeColor && lastEmpty != NULL){
						*lastEmpty |= (placeColor == Occupancy_t::WHITE) ? possibleW : possibleB;
						lastEmpty = NULL;
					}
					placeColor = oppositeColor(color);
					x += ix;
					y += iy;
				} while (0 <= x && x < 8 && 0 <= y && y < 8);				
			}
		}

		// Pass over the possibleBloard and store the possible movements
		Movement_t movement;
		for (size_t y = 0; y < 8; y++){
			movement.y = y;
			for (size_t x = 0; x < 8; x++){
				movement.x = x;
				if (possibleBoard[y][x] & possibleW) actionsW.push_back(movement);
				if (possibleBoard[y][x] & possibleB) actionsB.push_back(movement);
			}
		}
	}
}

