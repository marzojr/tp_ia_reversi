# Compiladores
CXX = g++
CC = gcc

# Compactadores
ZIP = zip
TAR = tar

# Conversor odt -> pdf
ODT2PDF = unoconv

# Diretórios
DIRS=src

# Arquivos fonte
SRCSCXX := $(wildcard $(DIRS)/*.cpp)
SRCSH := $(wildcard $(DIRS)/*.h)

# Arquivos de objeto
OBJECTS := $(SRCSCXX:%.cpp=%.o)
BIN := reversi
DISTFILE := tp_ia_reversi

# Documentação
SRCDOCS := $(wildcard *.odt)
EXTRADIST := compila.sh launch.sh othello.sh src/opening.dat
DOCS := $(SRCDOCS:%.odt=%.pdf)

# Variáveis para compilação
CXXFLAGS = -O3 -s -std=c++0x -Wall -Wextra -Wcast-qual -pedantic -MMD
CPPFLAGS = 
INCFLAGS = 
LDFLAGS = -Wl,-rpath,/usr/local/lib
LIBS = 

# Alvos
all: $(BIN)
	
docs: $(DOCS)

time: CFLAGS += -DLOGTIME
time: CPPFLAGS += -DLOGTIME
time: $(BIN)

zip: docs
	rm -f $(DISTFILE).zip
	$(ZIP) -9 $(DISTFILE).zip Makefile $(SRCSCXX) $(SRCSH) $(DOCS) $(EXTRADIST)

clean:
	rm -f $(DIRS)/*.o *~ $(DIRS)/*~ $(DIRS)/*.d $(BIN) *.zip *.tar.gz

distclean: clean
	rm -f *.pdf

.PHONY: all clean distclean time zip tar docs

# Regras de construção
.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

$(BIN): $(OBJECTS)
	$(CXX) -o $(BIN) $(OBJECTS) $(LDFLAGS) $(LIBS)

%.pdf: %.odt
	$(ODT2PDF) -f pdf $<

# Dependências
-include $(OBJECTS:%.o=%.d)
