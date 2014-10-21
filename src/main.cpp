#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <fstream>
#include <cmath>
#include <functional>
#include "reversi.h"
#include "minmax.h"
#include "heuristic.h"

// extern void generateExpandTables(void);

// [DEBUG] Print out the possible movements for each player given the board
void printDebug(reversi::State_t * state, reversi::Occupancy_t color){
	
	// Print out the board
	printf("%s\n", state->toString().c_str());

	// Print out the possible movements
	reversi::Movement_t movementW[64], movementB[64];
	size_t movementBCnt, movementWCnt;
	state->expand(movementB, movementBCnt, movementW, movementWCnt);
	printf("White movements:\n");
	for (size_t i = 0; i < movementWCnt; i++) printf("%s\n", movementW[i].toString().c_str());
	printf("Black movements:\n");
	for (size_t i = 0; i < movementBCnt; i++) printf("%s\n", movementB[i].toString().c_str());
	printf("Done printing configurations!\n");

	// Print out the heuristic for the base state
	Heuristic_t H;
	printf("Heuristic value was %f\n", 
		H.eval(state, movementBCnt, movementWCnt, color));	
}

// Compete returns -1 if black wins, 0 if it's a tie, and 1 if white wins
int compete(Heuristic_t &HB, Heuristic_t &HW){
	
	reversi::State_t state("........\n........\n........\n...WB...\n...BW...\n........\n........\n........");
	reversi::Occupancy_t myColor = reversi::Occupancy_t::BLACK;
	reversi::Occupancy_t opColor = reversi::Occupancy_t::WHITE;
	reversi::Movement_t movement;
				
	for(size_t i = 0; i < 60; i++){
		minmax::computeMinmax(&state, myColor == reversi::Occupancy_t::WHITE ? HW : HB, &movement, myColor, opColor);
		if(movement.x != -1 && movement.y != -1) {
			reversi::State_t newstate(&state, &movement, myColor);
			state = newstate;
		}
		myColor = reversi::oppositeColor(myColor);
		opColor = reversi::oppositeColor(opColor);
	}

	int count = state.score();
	if(count == 0) return 0;
	else return count > 0 ? 1 : -1;
}

void printFormatAndExit(const char * program){
	printf("(1) %s -f [fileName] [\"white\" | \"black\"] \n", program);
	printf("(2) %s -s [board string] [\"white\" | \"black\"]\n", program);
	printf("(3) %s -c [HB Coin Count] [HB Relative Frontier Size] [HB Closeness] [HB Board weight] [HW Coin Count] [HW Relative Frontier Size] [HW Closeness] [HW Board weight]\n", program);
	printf("(4) %s -search [H Coin Count] [H Relative Frontier Size] [H Closeness] [H Board weight]\n", program);
	exit(0);
}

int main(int argc, char ** argv){

	// [CODE_GEN]
	//generateExpandTables();

	//Heuristic_t HB, HW;
	//HB.set()
	//compete(HB, HW);
	//return 0;

	// Sanitize inputs and generate initial state
	const char * argProgram = argv[0];
	const char * argRunType = argv[1];
	//       Single play mode	
	int expectedArgs0 = 2;
	const char * argReadData = argv[expectedArgs0++];
	const char * argColor = argv[expectedArgs0++];
	//       Competition
	int expectedArgs1 = 2;
	const char * argHBCoinCount = argv[expectedArgs1++];
	const char * argHBFrontier = argv[expectedArgs1++];
	const char * argHBCloseness = argv[expectedArgs1++];
	const char * argHBBoard = argv[expectedArgs1++];
	const char * argHWCoinCount = argv[expectedArgs1++];
	const char * argHWFrontier = argv[expectedArgs1++];
	const char * argHWCloseness = argv[expectedArgs1++];
	const char * argHWBoard = argv[expectedArgs1++];
	//       Parameter search
	int expectedArgs2 = 2;
	const char * argHCoinCount = argv[expectedArgs2++];
	const char * argHFrontier = argv[expectedArgs2++];
	const char * argHCloseness = argv[expectedArgs2++];
	const char * argHBoard = argv[expectedArgs2++];	
	if (argc == 1) printFormatAndExit(argProgram);
	else if (argc != expectedArgs0 && argc != expectedArgs1 && argc != expectedArgs2){
		fprintf(stderr, "ERROR: Expected %i, %i, or %i arguments, got %i!\n", expectedArgs0, expectedArgs1, expectedArgs2, argc-1);
		printFormatAndExit(argProgram);
	
	//      Competition run type
	} if (strcmp(argRunType, "-c") == 0){

		// Blindly read heuristic parameters
		const double HBCoinCount = atof(argHBCoinCount);
		const double HBFrontier = atof(argHBFrontier);
		const double HBCloseness = atof(argHBCloseness);
		const double HBBoard = atof(argHBBoard);
		const double HWCoinCount = atof(argHWCoinCount);
		const double HWFrontier = atof(argHWFrontier);
		const double HWCloseness = atof(argHWCloseness);
		const double HWBoard = atof(argHWBoard);
		
		// Create heuristic functions
		Heuristic_t HB, HW;
		HB.set(HBCoinCount, HBFrontier, HBCloseness, HBBoard);
		HW.set(HWCoinCount, HWFrontier, HWCloseness, HWBoard);

		// Run competitions
		int score = compete(HB, HW) - compete(HW, HB);
		printf("%i\n", score);
	
	//      Parameter search		
	} else if(strcmp(argRunType, "-search") == 0){
			
		double HCoinCount = atof(argHCoinCount);
		double HFrontier = atof(argHFrontier);
		double HCloseness = atof(argHCloseness);
		double HBoard = atof(argHBoard);
		const double searchFraction = 0.02;		

		Heuristic_t HB, HW;
		std::function<void(Heuristic_t &, double)>  setH1 = [&](Heuristic_t & H, double val){ 
			H.set(HCoinCount, val, HCloseness, HBoard); };
		std::function<void(Heuristic_t &, double)>  setH2 = [&](Heuristic_t & H, double val){ 
			H.set(HCoinCount, HFrontier, val, HBoard); };
		std::function<void(Heuristic_t &, double)> setH3 = [&](Heuristic_t & H, double val){ 
			H.set(HCoinCount, HFrontier, HCloseness, val); };
		auto updateParam = [&](double &initialValue, std::function<void(Heuristic_t &, double)> & setH){
			int score;
			Heuristic_t HB, HW;
			double selectedCandidate, candidate0 = initialValue, candidate1 = initialValue;
			for(;;){			

				candidate0 *= 1 - searchFraction;
				candidate1 *= 1 + searchFraction;
				setH(HB, candidate0);
				setH(HW, candidate1);
				score = compete(HB, HW) - compete(HW, HB);
		
				if(score == 0) continue;				
				else if(score > 0) selectedCandidate = candidate1;
				else selectedCandidate = candidate0;
				break;
			}
			setH(HB, selectedCandidate);
			setH(HW, initialValue);
			score = compete(HB, HW) - compete(HW, HB);
			if(score <= 0) initialValue = selectedCandidate; 
		};
		for(size_t i = 0;; i++){
			updateParam(HFrontier, setH1);
			updateParam(HCloseness, setH2);
			updateParam(HBoard, setH3);	 		
			printf("(%zu) HCoinCount: %f; HFrontier: %f; HCloseness: %f; HBoard %f\n", 
				i, HCoinCount, HFrontier, HCloseness, HBoard);	
		}


	//      Single run type
	} else {
		// Read the read type
		enum { FROM_FILE, FROM_STRING} readType;
		if (strcmp(argRunType, "-f") == 0) readType = FROM_FILE;
		else if (strcmp(argRunType, "-s") == 0) readType = FROM_STRING;
		else{
			fprintf(stderr, "ERROR: Expected a read type of \"-f\" or \"-s\", instead got \"%s\"!\n", argRunType);
			printFormatAndExit(argProgram);
		}
		// Read play color
		reversi::Occupancy_t myColor, oppColor;
		if (strlen(argColor) != 5){
			fprintf(stderr, "ERROR: Expected play color \"white\" or \"black\", instead got \"%s\"!\n", argColor);
			printFormatAndExit(argProgram);
		} else{
			char argColorUpper[6];
			for (size_t i = 0; i < 6; i++) argColorUpper[i] = toupper(argColor[i]);
			if (!strcmp(argColorUpper, "WHITE")){
				myColor  = reversi::Occupancy_t::WHITE;
				oppColor = reversi::Occupancy_t::BLACK;
			} else if (!strcmp(argColorUpper, "BLACK")){
				myColor  = reversi::Occupancy_t::BLACK;
				oppColor = reversi::Occupancy_t::WHITE;
			} else{
				fprintf(stderr, "ERROR: Expected a color \"white\" or \"black\", instead got \"%s\"", argColor);
				printFormatAndExit(argProgram);
			}
		}
		// Read input data and produce starting state
		reversi::State_t * startingState = 0;
		if (readType == FROM_FILE){
			std::ifstream input(argReadData, std::ifstream::in);
			if (!input.is_open()){
				fprintf(stderr, "ERROR: Unable to open board definition file \"%s\" for reading.\n", argReadData);
				exit(-1);
			} else{
				startingState = new reversi::State_t(input);
				input.close();
			}
		} else{
			startingState = new reversi::State_t(argReadData);
		}

		// [DEBUG] Print out the possible movements for each player given the board and compute the heuristic value
		// printDebug(startingState, myColor);	

		// Compute the next state using min-max
		Heuristic_t H;
		reversi::Movement_t movement;
		minmax::computeMinmax(startingState, H, &movement, myColor, oppColor);

		// Prints out the movement to the screen (launch.sh will redirect it to move.txt)
		printf("%s\n", movement.toString().c_str());
	
		// Free resources
		delete startingState;
	}

	return 0;
}
