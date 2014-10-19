#ifndef _REVERSI_H_
#define _REVERSI_H_

#include <iosfwd>
#include <string>
#include <cstdint>
#include <vector>
#include <utility>

namespace reversi{
	typedef int8_t boardCoord_t;

	enum class Occupancy_t : char {
		BLACK = -1, EMPTY = 0, WHITE = 1
	};
	
	struct Movement_t{
		boardCoord_t x, y;
		std::string toString() const;
	};

	class State_t{
		Occupancy_t board[8][8];
	private:
		void init(std::istream & conf);

	public:
		State_t(const char * conf);
		State_t(std::istream & conf);
		State_t(const State_t * parent, const Movement_t * movement, Occupancy_t color);
		std::string toString() const;
		std::string toString(bool flipMain, bool flipRev) const;
		void expand(std::vector<Movement_t> & actions, Occupancy_t myColor, Occupancy_t oppColor) const;
	};
}

#endif //_REVERSI_H_

