#ifndef _REVERSI_H_
#define _REVERSI_H_

#include <iosfwd>
#include <string>
#include <stdint.h>

namespace reversi{
	typedef uint8_t boardCoord_t;

	enum class Occupancy_t : char {
		BLACK = -1, EMPTY = 0, WHITE = 1
	};	
	
	struct Movement_t{
		boardCoord_t x, y;
		std::string toString() const;
	};

	class State_t{
		Occupancy_t board[8][8];
		State_t(const State_t * parent, const Movement_t * movement);
	private:
		void init(std::istream & conf);

	public:
		State_t(const char * conf);
		State_t(std::istream & conf);
		std::string toString() const;
		void expand(Movement_t * movement, State_t * state, size_t &count) const;
	};
}

#endif //_REVERSI_H_

