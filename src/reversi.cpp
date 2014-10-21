#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include "reversi.h"

namespace reversi{
	std::string Movement_t::toString() const{
		std::stringstream ss;
		ss << static_cast<int>(x) << ',' << static_cast<int>(y);
		return ss.str();
	}

	State_t::State_t(const State_t * parent, const Movement_t * movement, Occupancy_t color){

		// First, duplicate the parent.
		memcpy(board, parent->board, 8 * 8 * sizeof(Occupancy_t));

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
	void State_t::expand(Movement_t * actionsB, size_t &actionsBCnt, Movement_t * actionsW, size_t &actionsWCnt) const{
	
		// Loads in the hard-coded placement matrices
		/* Defines expandAttributes -- {x_0, x_1, inc_x, inc_y, size, trinary_representation}
		 *     and expandPlacement3 ... expandPlacement8 -- A matrix that, for a given trinary representation, 
		 *           represents {W, W, W, B, B, B}, where W or B are a position that can have that color in it, 
		 *           and a value of -1 should be ignored */
		const uint8_t (*b)[8] = reinterpret_cast<const uint8_t (*)[8]>(board);
		#include "expand.def"
		const static int(*expandPlacement[9])[6] = {
			NULL, NULL, NULL, expandPlacement3, expandPlacement4, expandPlacement5, expandPlacement6, expandPlacement7, expandPlacement8
		};

		// Stores which positions can be of each color
		uint8_t possibleW[8][8];
		uint8_t possibleB[8][8];
		memset(possibleW, 0x00, 8 * 8 * sizeof(bool));
		memset(possibleB, 0x00, 8 * 8 * sizeof(bool));

		// Determine placement positions
		for (size_t i = 0; i < 38; i++){
			const int x0 = expandAttributes[i][0];
			const int y0 = expandAttributes[i][1];
			const int incx = expandAttributes[i][2];
			const int incy = expandAttributes[i][3];
			const int segmentSize = expandAttributes[i][4];
			const int segmentTrinary = expandAttributes[i][5];
			const int (&values)[6] = expandPlacement[segmentSize][segmentTrinary];
			if (values[0] != -1){
				assert(y0 + incy*values[0] < 8 && x0 + incx*values[0] < 8);
				possibleW[y0 + incy*values[0]][x0 + incx*values[0]] = true;
				if (values[1] != -1){
					assert(y0 + incy*values[1] < 8 && x0 + incx*values[1] < 8);
					possibleW[y0 + incy*values[1]][x0 + incx*values[1]] = true;
					if (values[2] != -1){
						assert(y0 + incy*values[2] < 8 && x0 + incx*values[2] < 8);
						possibleW[y0 + incy*values[2]][x0 + incx*values[2]] = true;
					}
				}
			}
			if (values[3] != -1){
				assert(y0 + incy*values[3] < 8 && x0 + incx*values[3] < 8);
				possibleB[y0 + incy*values[3]][x0 + incx*values[3]] = true;	
				if (values[4] != -1){
					assert(y0 + incy*values[4] < 8 && x0 + incx*values[4] < 8);
					possibleB[y0 + incy*values[4]][x0 + incx*values[4]] = true;
					if (values[5] != -1){
						assert(y0 + incy*values[5] < 8 && x0 + incx*values[5] < 8);
						possibleB[y0 + incy*values[5]][x0 + incx*values[5]] = true;
					}
				}
			}
		}

		actionsWCnt = 0;
		for (size_t y = 0; y < 8; y++){
			for (size_t x = 0; x < 8; x++){
				if (possibleW[y][x]){
					actionsW[actionsWCnt].y = y;
					actionsW[actionsWCnt].x = x;
					actionsWCnt++;
		}}}
		actionsBCnt = 0;
		for (size_t y = 0; y < 8; y++){
			for (size_t x = 0; x < 8; x++){
				if (possibleB[y][x]){
					actionsB[actionsBCnt].y = y;
					actionsB[actionsBCnt].x = x;
					actionsBCnt++;
		}}}

	}
}

int reversi::State_t::score() const {
	int total = 0;
	for (unsigned ii = 0; ii < 8; ii++)
		for (unsigned jj = 0; jj < 8; jj++)
			if (board[ii][jj] == Occupancy_t::WHITE) total++;
			else if (board[ii][jj] == Occupancy_t::BLACK) total--;
	return total;
}

