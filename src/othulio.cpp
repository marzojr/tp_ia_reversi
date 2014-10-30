#include <cstdio>
#include <climits>
#include <algorithm>
#include <iostream>

#include "ai.h"
#include "game.h"
#include "board.h"

#include <sys/time.h>
#include <unistd.h>


	class TTimer
	{
		public:
			TTimer(); //"The Genesis of time."

			void start(); //"At it's alpha, it learns to walk;"

			void pause(); //"Still doesn't pause time...Time pauses to no one."

			double get_elapsed_time() { return m_elapsed_time; } //"I am out of lines"

			double get_elapsed_total_time() { return m_elapsed_total_time; } //"K thx bai"

		private:

			struct timeval m_t0;
			struct timeval m_tS;
			struct timeval m_tF;
			double m_elapsed_time;
			double m_elapsed_total_time;
	};

	TTimer::TTimer() //"The origin of time;"
	{
		m_elapsed_time = 0;
		m_elapsed_total_time = 0;

		gettimeofday(&m_t0,NULL);
		m_tS = m_t0;
		m_tF = m_t0;
	}

	void TTimer::start() //"Time begins to flow."
	{
		gettimeofday(&m_tS,NULL);
	}

	void TTimer::pause() //"Time cannot be controlled, therefore this does not actually pause time."
	{
		gettimeofday(&m_tF,NULL);

		long long seconds;
		long long useconds;

		seconds  = m_tF.tv_sec  - m_tS.tv_sec;
		useconds = m_tF.tv_usec - m_tS.tv_usec;

		m_elapsed_time = seconds + useconds/1000000.0;
		m_elapsed_total_time += m_elapsed_time;
	}

///////////
//DEFINES//
///////////

#define MAX_DEPTH 6
#define board_size 8

//Definicoes de constantes de avaliacao
#define WON_VALUE 9001
#define MARKER_VALUE -1
#define MOBILITY_VALUE 2
#define Z_VALUE 99
#define C_VALUE -8
#define A_VALUE 8
#define B_VALUE 6
#define X_VALUE -24
#define F_VALUE -4
#define G_VALUE -3
#define D_VALUE 7
#define E_VALUE 4
#define M_VALUE 0

/////////////////////
//VARIAVEIS GLOBAIS//
/////////////////////

int my_number;
short int pos_matrix[8][8] = {{Z_VALUE, C_VALUE, A_VALUE, B_VALUE, B_VALUE, A_VALUE, C_VALUE, Z_VALUE},
				{C_VALUE, X_VALUE, F_VALUE, G_VALUE, G_VALUE, F_VALUE, X_VALUE, C_VALUE},
				{A_VALUE, F_VALUE, D_VALUE, E_VALUE, E_VALUE, D_VALUE, F_VALUE, A_VALUE},
				{B_VALUE, G_VALUE, E_VALUE, M_VALUE, M_VALUE, E_VALUE, G_VALUE, B_VALUE},
				{B_VALUE, G_VALUE, E_VALUE, M_VALUE, M_VALUE, E_VALUE, G_VALUE, B_VALUE},
				{A_VALUE, F_VALUE, D_VALUE, E_VALUE, E_VALUE, D_VALUE, F_VALUE, A_VALUE},
				{C_VALUE, X_VALUE, F_VALUE, G_VALUE, G_VALUE, F_VALUE, X_VALUE, C_VALUE},
				{Z_VALUE, C_VALUE, A_VALUE, B_VALUE, B_VALUE, A_VALUE, C_VALUE, Z_VALUE}};

/* Mapeamento do nome das posicoes
	{'Z','C','A','B','B','A','C','Z'}
	{'C','X','F','G','G','F','X','C'}
	{'A','F','D','E','E','D','F','A'}
	{'B','G','E','M','M','E','G','B'}
	{'B','G','E','M','M','E','G','B'}
	{'A','F','D','E','E','D','F','A'}
	{'C','X','F','G','G','F','X','C'}
	{'Z','C','A','B','B','A','C','Z'}
*/

///////////////
//DECLARACOES//
///////////////

struct GraphNode;

bool terminal_test(GraphNode *node);
short int minimax(GraphNode *node, short int alpha, short int beta);

//////////////////////
//ESTRUTURA DO GRAFO//
//////////////////////

struct GraphNode{

	int won;	//Indica qual jogador wooooooon nesse estado (0 caso nenhum ganhou)
	short int score; //score atual do estado
	short int filhos; //numero de nos filhos (largura)
	short int profundidade; //numero de jogadas (profundidade)


	Game* game_state; //estado do jogo nesse estado
	GraphNode* next; //nos filhos das jogadas subsequentes
	short int* v_filhos; //score dos filhos
	Coord* c_filhos; //coordenada da jogada dos filhos

	//funciones
	void init_filhos();
	void yuri(); //Destroyer of things
	short int calc_score();
	Coord find_node(short int v);
};

////////////////////////
//METODOS DA ESTRUTURA//
////////////////////////

//Calcula o score do no
short int GraphNode::calc_score(){

	char square_type;
	struct Coord c;
	short int actual_marker_value;
	short int actual_mobility_value;
	short int pecas = -4;

	//Conta numero de jogadas ate o momento (pecas - 4)
	for (c.y = 0; c.y < board_size; ++c.y) {
		for (c.x = 0; c.x < board_size; ++c.x) {
			pecas += (gameSquareType(game_state,c) ? 1 : 0);
		}
	}

	//Altera os valores de acordo com o periodo do jogo
	actual_marker_value = (pecas < 15) ? MARKER_VALUE : -MARKER_VALUE; //Evaporacao (early) X Numero de Pecas (late)
	actual_mobility_value = ((pecas/10)+1) * MOBILITY_VALUE; //Melhor no lategame (?)

	//Verifica se vai ganhar ou perder
	score += (won == my_number) ? WON_VALUE : ((won) ? -WON_VALUE : 0);

	//Avaliacao Gulosa
	for (c.y = 0; c.y < board_size; ++c.y) {
		for (c.x = 0; c.x < board_size; ++c.x) {
			square_type = gameSquareType(game_state,c);
			score += (my_number == square_type) ? actual_marker_value :
				  ((square_type) ? -actual_marker_value : 0);
		}
	}

	//Avaliacao Posicional Estrategica
	for (c.y = 0; c.y < board_size; ++c.y) {
		for (c.x = 0; c.x < board_size; ++c.x) {
			square_type = gameSquareType(game_state,c);
			score += (square_type == my_number) ? pos_matrix[c.y][c.x] :
				  ((square_type) ? -pos_matrix[c.y][c.x] : 0);
		}
	}

	//Mobilidade
	bool flip;
	for (c.y = 0; c.y < board_size; ++c.y) {
		for (c.x = 0; c.x < board_size; ++c.x) {
			flip = gameNumFlips(game_state, c);
			score += ((gameCurrentPlayer(game_state) == my_number) && flip) ? actual_mobility_value :
				  (flip ? -actual_mobility_value : 0);
		}
	}

	return score;
}

//Gera e inicializa os nos filhos
void GraphNode::init_filhos(){

	short int i;

	bool positions[board_size][board_size];

	struct Coord c;

	for (c.y = 0; c.y < board_size; ++c.y) {
		for (c.x = 0; c.x < board_size; ++c.x) {
			positions[c.y][c.x] = gameNumFlips(game_state, c) ? 1 : 0; //Se a posicao e valida, aumente o numero de filhos
			filhos += positions[c.y][c.x];
		}
	}

	next = new GraphNode[filhos]; //Cria os nos com as proximas jogadas
	v_filhos = new short int[filhos];
	c_filhos = new Coord[filhos];
	i = 0;

	for (c.y = 0; c.y < board_size; ++c.y) {
		for (c.x = 0; c.x < board_size; ++c.x) {
			if(positions[c.y][c.x]){
				next[i].game_state = gameCopy(game_state); //copia o tabuleiro
				next[i].won = gamePlayCurrent(next[i].game_state, c); //faz a jogada

				//Ajusta os parametros do filho
				next[i].profundidade = profundidade+1;
				next[i].filhos = 0;
				next[i].score = 0;
				i ++;
			}
		}
	}
}

//Encontra a coordenada que gerou o filho com valor 'v'
Coord GraphNode::find_node(short int v){

	for(int i=0; i<filhos; i++){
		if(v_filhos[i] == v)
			return c_filhos[i];
	}
	std::cerr << "ERRO! No de score " << v << " nao encontrado no grafo MIN-MAX!" << std::endl;
	Coord c;
	c.y = -1; c.x = -1;
	return c;
}

//Destroyer of things
void GraphNode::yuri(){
	if(filhos){
		delete [] next;
		delete [] v_filhos;
		delete [] c_filhos;
	}

	if(game_state){
		gameDestruct(game_state);
		game_state = NULL;
	}

	filhos = 0;
}

///////////////////
//FUNCOES GLOBAIS//
///////////////////

//Testa se e o ultimo no a ser avaliado
bool terminal_test(GraphNode *node){
	return( node->profundidade >= MAX_DEPTH || node->won );
}

//Algoritmo do MiniMax
short int minimax(GraphNode *node, short int alpha, short int beta){

	int i;

	if(terminal_test(node)){
		return node->calc_score();
	}

	node->init_filhos();

	short int v;
	if( gameCurrentPlayer(node->game_state) == my_number ){
		v = SHRT_MIN;
		for(i=0; i<node->filhos; i++){
			v = std::max( v, minimax(&(node->next[i]), alpha, beta) );

			node->v_filhos[i] = v;
			node->c_filhos[i] = gameLastCoord(node->next[i].game_state);
			node->next[i].yuri();

			if( v >= beta ){
				break;
			}
			alpha = std::max( alpha, v );
		}
	}else{
		v = SHRT_MAX;
		for(i=0; i<node->filhos; i++){
			v = std::min( v, minimax(&(node->next[i]), alpha, beta) );

			node->v_filhos[i] = v;
			node->c_filhos[i] = gameLastCoord(node->next[i].game_state);
			node->next[i].yuri();

			if( v <= alpha ){
				break;
			}
			beta = std::min( beta, v );
		}
	}

	for(; i<node->filhos; i++){
		node->next[i].yuri();
	}

	node->score = v;
	return v;
}

//Funcao Principal
Coord aiPlay(Game const* game)
{
	Coord c;     //Coordenadas enviadas como resposta
	short int v; //Valor da funcao de avaliacao

	TTimer t;

	t.start();

	//Cria no raiz
	GraphNode *root = new GraphNode;
	//Init R00t node
	root->profundidade = 0;
	root->filhos = 0;
	root->score = 0;
	root->won = 0;
	root->game_state = gameCopy(game);

	my_number = gameCurrentPlayer(game);

	//Roda MiniMax
	v = minimax(root,SHRT_MIN,SHRT_MAX);
	c = root->find_node(v);

	t.pause();

	//Libera memoria
	root->yuri();
	delete root;

	//Ao final, retornar a coordenada da próxima jogada
	return c;
}

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <fstream>
#include <cmath>
#include <functional>
#include "reversi.h"

void runOthulio(
	const reversi::Occupancy_t myColor,
	const reversi::Occupancy_t (*startBoard)[8][8],
	reversi::Movement_t &result
);
#define OTHULIO_NOMAIN
#ifndef OTHULIO_NOMAIN

int main(int argc, char ** argv){
	const char * argReadData = argv[1];
	const char * argColor = argv[2];
	if(argc != 3){
		fprintf(stderr, "ERROR: Invalid number of arguments!\n");
		exit(-1);
	}
	// Load in the board using deep diver format
	reversi::State_t * startingState = 0;
	std::ifstream input(argReadData, std::ifstream::in);
	if (!input.is_open()){
		fprintf(stderr, "ERROR: Unable to open board definition file \"%s\" for reading.\n", argReadData);
		exit(-1);
	} else{
		startingState = new reversi::State_t(input);
		input.close();
	}
	
	// Compute the player color
	reversi::Occupancy_t myColor, oppColor;
	if (strlen(argColor) != 5){
		fprintf(stderr, "ERROR: Expected play color \"white\" or \"black\", instead got \"%s\"!\n", argColor);
		exit(-1);
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
			exit(-1);
		}
	}
	
	const reversi::Occupancy_t (*startBoard)[8][8] = startingState->getBoard();
	reversi::Movement_t movement;
	runOthulio(myColor, startBoard, movement);
	printf("%i,%i\n", movement.x, movement.y);
}
#endif

void runOthulio(
	const reversi::Occupancy_t myColor,
	const reversi::Occupancy_t (*startBoard)[8][8], 
	reversi::Movement_t &move
){
	// Create the game structure expected by othulio
	//       Defining values for black and white
	#define BVAL 1
	#define WVAL 2
	//       Create the board
	struct Board b;
	b.width = 8;
	b.height = 8;
	b.array = (int*)malloc(64*sizeof(int*));
	for(size_t y = 0; y < 8; y++){
		for(size_t x = 0; x < 8; x++){
			if((*startBoard)[y][x]  == reversi::Occupancy_t::EMPTY) b.array[y*8 + x] = 0;
			else if((*startBoard)[y][x] == reversi::Occupancy_t::BLACK) b.array[y*8 + x] = BVAL;
			else b.array[y*8 + x] = WVAL;
		}
	}
	b.last = coord(0, 0);
	
	Game game;
	game.turn = myColor == reversi::Occupancy_t::BLACK ? BVAL : WVAL;
	game.board = &b;
	
	// Run othulio
	Coord result = aiPlay(&game);
	move.x = result.x;
	move.y = result.y;
}


