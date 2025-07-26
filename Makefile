CXX = g++-14
CXXFLAGS = -g -std=c++20 -Wall -MMD -Werror=vla
SRC_DIR = src
PIECES_DIR = $(SRC_DIR)/pieces

EXEC = chess

OBJECTS = \
	$(SRC_DIR)/main.o \
	$(SRC_DIR)/Board.o \
	$(SRC_DIR)/Piece.o \
	$(SRC_DIR)/PieceFactory.o \
	$(SRC_DIR)/TextDisplay.o \
	$(SRC_DIR)/ChessGame.o \
	$(PIECES_DIR)/King.o \
	$(PIECES_DIR)/Queen.o \
	$(PIECES_DIR)/Rook.o \
	$(PIECES_DIR)/Bishop.o \
	$(PIECES_DIR)/Knight.o \
	$(PIECES_DIR)/Pawn.o \
	$(SRC_DIR)/GraphicalDisplay.o \
	$(SRC_DIR)/window.o

DEPENDS = ${OBJECTS:.o=.d}

all: ${EXEC}

${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC} -lX11

-include ${DEPENDS}

.PHONY: clean

clean:
	rm -f ${OBJECTS} ${EXEC} ${DEPENDS}