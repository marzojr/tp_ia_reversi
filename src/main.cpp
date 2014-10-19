#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <fstream>
#include "reversi.h"
#include "minmax.h"


void printFormatAndExit(char * program){
	printf("(1) %s -f [fileName] [\"white\" | \"black\"] \n", program);
	printf("(2) %s -s [board string] [\"white\" | \"black\"]\n", program);
	exit(0);
}

int main(int argc, char ** argv){

	// Sanitize inputs and generate initial state
	int expectedArgs = 1;
	const char * argReadType = argv[expectedArgs++];
	const char * argReadData = argv[expectedArgs++];
	const char * argColor = argv[expectedArgs++];
	if (argc == 1) printFormatAndExit(argv[0]);
	else if (argc != expectedArgs){
		fprintf(stderr, "ERROR: Expected %i arguments, got %i!\n", expectedArgs, argc-1);
		printFormatAndExit(argv[0]);
	}
	//       Read play color
	reversi::Occupancy_t myColor, oppColor;
	if (strlen(argColor) != 5){
		fprintf(stderr, "ERROR: Expected play color \"white\" or \"black\", instead got \"%s\"!\n", argColor);
		printFormatAndExit(argv[0]);
	} else{
		char argColorUpper[6];
		for (size_t i = 0; i < 6; i++) argColorUpper[i] = toupper(argColor[i]);
		if (strcmp(argColorUpper, "WHITE")){
			myColor  = reversi::Occupancy_t::WHITE;
			oppColor = reversi::Occupancy_t::BLACK;
		} else if (strcmp(argColorUpper, "BLACK")){
			myColor  = reversi::Occupancy_t::BLACK;
			oppColor = reversi::Occupancy_t::WHITE;
		} else{
			fprintf(stderr, "ERROR: Expected a color \"white\" or \"black\", instead got \"%s\"", argColor);
			printFormatAndExit(argv[0]);
		}
	}
	//       Read input type
	enum { FROM_FILE, FROM_STRING} readType;
	if (strcmp(argReadType, "-f") == 0) readType = FROM_FILE;
	else if (strcmp(argReadType, "-s") == 0) readType = FROM_STRING;
	else{
		fprintf(stderr, "ERROR: Expected a read type of \"-f\" or \"-s\", instead got \"%s\"!\n", argReadType);
		printFormatAndExit(argv[0]);
	}
	//       Read input data and produce starting state
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

	printf("%s\n", startingState->toString().c_str());

	// Compute the next state using min-max
	reversi::Movement_t movement;
	minmax::computeMinmax(startingState, &movement, myColor, oppColor);

	// Prints out the movement to the screen (launch.sh will redirect it to move.txt)
	printf("%s\n", movement.toString().c_str());
	
	// Free resources and exit
	delete startingState;
	return 0;
}
