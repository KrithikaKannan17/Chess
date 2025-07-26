#include "Piece.h"
#include "Board.h"
#include <algorithm>
#include <cmath>

using namespace std;

bool Piece::isOwn(const Position& pos, const Board& board) const { // is own
    const Piece* newPiece = board.getPiece(pos); // get piece
    return newPiece != nullptr && newPiece->getColor() == color; // return piece is not null and piece color is color
}

bool Piece::isEnemy(const Position& pos, const Board& board) const { // is enemy
    const Piece* newPiece = board.getPiece(pos); // get piece
    return newPiece != nullptr && newPiece->getColor() != color; // return piece is not null and piece color is not color
}

bool Piece::isEmpty(const Position& pos, const Board& board) const { // is empty
    return board.getPiece(pos) == nullptr; // return piece is null
}

bool Piece::isPathClear(const Position& init, const Position& dest, const Board& board) const { // is path clear
    int rd = getDirection(init.r, dest.r); // get row direction
    int cd = getDirection(init.c, dest.c); // get column direction
    
    if (rd == 0 && cd == 0) { // if row direction and column direction are 0
        return false;
    }
    
    int currRd = init.r + rd; // get current row direction
    int currCd = init.c + cd; // get current column direction
    
    while (currRd != dest.r || currCd != dest.c) { // while current row direction is not equal to destination row direction or current column direction is not equal to destination column direction
        Position current(currRd, currCd); // get current position
        
        if (!current.isValid()) { // if current position is not valid
            return false;
        }
        
        if (!isEmpty(current, board)) { // if current position is not empty
            return false;
        }

        currRd += rd; // get current row direction
        currCd += cd; // get current column direction
    }
    
    return true;            
}

bool Piece::leaveCheckPieces(const Position& init, const Position& dest, const Board& board) const { // leave check pieces
    return false; // return false
} 
