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
	Occupancy_t oppositeColor(Occupancy_t color);
	
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
		void expand(std::vector<Movement_t> & actionsB, std::vector<Movement_t> & actionsW) const ;
		const Occupancy_t(*getBoard() const)[8][8];
	};
}

#endif //_REVERSI_H_

