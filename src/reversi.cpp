#include <sstream>
#include <cstdio>
#include <cstdlib>
#include "reversi.h"

namespace reversi{
	
	std::string Movement_t::toString() const{
		std::stringstream ss;
		ss << x << ',' << y;
		return ss.str();
	}
	
	State_t::State_t(const State_t * parent, const Movement_t * movement){

	}

	void State_t::init(std::istream & conf){
		for (size_t i = 0; i < 8; i++){
			for (size_t j = 0; j < 8; j++){
				
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

	void State_t::expand(Movement_t * movement, State_t * state, size_t &count) const{

	}
}

