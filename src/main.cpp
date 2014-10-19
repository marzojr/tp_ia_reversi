#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <fstream>
#include "reversi.h"
#include "minmax.h"


void printFormatAndExit(void){
	printf("(1) ./reversi [\"white\" | \"black\"] -f [fileName]\n");
	printf("(2) ./reversi [\"white\" | \"black\"] -s [board string]\n");
	exit(0);
}
int main(int argc, char ** argv){

	// Sanitize inputs and generate initial state
	size_t expectedArgs = 1;
	const char * argColor = argv[expectedArgs++];
	const char * argReadType = argv[expectedArgs++];
	const char * argReadData = argv[expectedArgs++];
	if (argc == 1) printFormatAndExit();
	else if (argc > expectedArgs){
		fprintf(stderr, "ERROR: Expected %i arguments, got %i!\n", expectedArgs);
		printFormatAndExit();
	}
	//       Read play color
	enum { WHITE_START, BLACK_START } startColor;
	if (strlen(argColor) != 5){
		fprintf(stderr, "ERROR: Expected play color \"white\" or \"black\", instead got \"%s\"!\n", argColor);
		printFormatAndExit();
	}
	else{
		char argColorUpper[6];
		for (size_t i = 0; i < 6; i++) argColorUpper[i] = toupper(argColor[i]);
		if (strcmp(argColorUpper, "WHITE")) startColor = WHITE_START;
		else if (strcmp(argColorUpper, "BLACK")) startColor = BLACK_START;
		else{
			fprintf(stderr, "ERROR: Expected a color \"white\" or \"black\", instead got \"%s\"", argColor);
			printFormatAndExit();
		}
	}
	//       Read input type
	enum { FROM_FILE, FROM_STRING} readType;
	if (strcmp(argReadType, "-f") == 0) readType = FROM_FILE;
	else if (strcmp(argReadType, "-s") == 0) readType = FROM_STRING;
	else{
		fprintf(stderr, "ERROR: Expected a read type of \"-f\" or \"-s\", instead got \"%s\"!\n", argReadType);
		printFormatAndExit();
	}
	//       Read input data and produce starting state
	reversi::State_t * startingState;
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
	minmax::computeMinmax(startingState, &movement);

	// Prints out the movement to the screen (launch.sh will redirect it to move.txt)
	printf("%s\n", movement.toString().c_str());
	
	// Free resources and exit
	if (startingState) delete startingState;
	return 0;
}