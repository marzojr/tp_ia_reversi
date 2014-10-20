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
	inline Occupancy_t oppositeColor(Occupancy_t color){
		return static_cast<Occupancy_t>(-static_cast<char>(color));
	}

	struct Movement_t{
		boardCoord_t x, y;
		std::string toString() const;
		Movement_t() {}
		Movement_t(boardCoord_t _x, boardCoord_t _y) : x(_x), y(_y) {}
		Movement_t(const Movement_t & other) : x(other.x), y(other.y) {}
		Movement_t & operator=(const Movement_t & rhs) {
			if (this != &rhs){
				x = rhs.x; y = rhs.y;
			}
			return *this;
		}
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
		void expand(std::vector<Movement_t> & actionsB, std::vector<Movement_t> & actionsW) const ;
		const Occupancy_t(*getBoard() const)[8][8];
	};
}

#endif //_REVERSI_H_

