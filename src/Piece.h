#ifndef PIECE_H
#define PIECE_H

#include "types.h"
#include <memory>
#include <vector>

using namespace std;

// piece values
const int PAWN_VALUE = 100;
const int KNIGHT_VALUE = 320;
const int BISHOP_VALUE = 330;
const int ROOK_VALUE = 500;
const int QUEEN_VALUE = 900;
const int KING_VALUE = 20000;

class Board;

class Piece {
    PieceType type; // piece type

protected:
    Color color; // color
    bool hasMoved;
    
    virtual bool isValidMove(const Position& init, const Position& dest, const Board& board) const = 0; // is valid move
    
    bool isValidPos(const Position& pos) const { // is valid position
        return pos.isValid(); // return position is valid
    }
    
    bool isOwn(const Position& pos, const Board& board) const; // is own
    bool isEnemy(const Position& pos, const Board& board) const; // is enemy
    bool isEmpty(const Position& pos, const Board& board) const; // is empty
    
    bool isPathClear(const Position& init, const Position& dest, const Board& board) const; // is path clear
    
    static int getDirection(int from, int to) { // get direction
        if (from < to) {
            return 1; // if from is less than to
        }
        if (from > to) {
            return -1; // if from is greater than to
        }
        return 0; // return 0
    }
    
public:
    Piece(Color c, PieceType t) : type(t), color(c), hasMoved(false) {} // constructor
    virtual ~Piece() = default; // destructor
    
    virtual vector<Move> LegalMoves(const Board& board, const Position& position) const = 0; // legal moves
    virtual unique_ptr<Piece> clone() const = 0; // clone
    
    Color getColor() const { return color; } // get color
    PieceType getType() const { return type; } // get type
    bool getHasMoved() const { return hasMoved; } // get has moved
    void setHasMoved(bool moved) { hasMoved = moved; } // set has moved
    
    char getSymbol() const { // get symbol
        return pieceTypeToChar(type, color); // return piece type to char
    }
    
    virtual int getValue() const { // get value
        if (type == PieceType::PAWN) { // if pawn
            return PAWN_VALUE; // return pawn value
        } else if (type == PieceType::KNIGHT) { // if knight
            return KNIGHT_VALUE; // return knight value
        } else if (type == PieceType::BISHOP) { // if bishop
            return BISHOP_VALUE; // return bishop value
        } else if (type == PieceType::ROOK) { // if rook
            return ROOK_VALUE; // return rook value
        } else if (type == PieceType::QUEEN) { // if queen
            return QUEEN_VALUE; // return queen value
        } else if (type == PieceType::KING) { // if king
            return KING_VALUE; // return king value
        } else { // if other
            return 0; // return 0
        }
    }   
    
    bool isLegal(const Position& init, const Position& dest, const Board& board) const { // is legal
        if (!isValidPos(dest)) {
            return false; // return false
        }
        if (init == dest) {
            return false; // return false
        }
        if (isOwn(dest, board)) {
            return false; // return false
        }
        
        return isValidMove(init, dest, board); // return is valid move
    }
    
    bool leaveCheckPieces(const Position& init, const Position& dest, const Board& board) const; // leave check pieces
};

#endif 
