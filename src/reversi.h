#ifndef _REVERSI_H_
#define _REVERSI_H_

#include <iosfwd>
namespace reversi{
	typedef uint8_t boardCoord_t;

	enum class Occupancy_t : char {
		BLACK, EMPTY, WHITE
	};	
	
	struct Movement_t{
		boardCoord_t x, y;
		std::string toString() const;
	};

	class State_t{
		Occupancy_t board[8][8];
		State_t(const State_t * parent, const Movement_t * movement);
	public:
		State_t(const char * conf);
		State_t(std::istream & conf);
		std::string toString() const;
		void expand(Movement_t * movement, State_t * state, size_t &count) const;
	};
}

#endif //_REVERSI_H_

