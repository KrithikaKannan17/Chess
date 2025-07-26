#ifndef BISHOP_H
#define BISHOP_H

#include "../Piece.h"

using namespace std; 

class Bishop : public Piece { // inherit from Piece
    bool BishopMove(const Position& init, const Position& dest) const; // check if move is valid

protected:
    bool isValidMove(const Position& init, const Position& dest, const Board& board) const override; // check if move is valid
    
public:
    Bishop(Color c) : Piece(c, PieceType::BISHOP) {} // constructor
    
    vector<Move> LegalMoves(const Board& board, const Position& position) const override; // calculate legal moves
    unique_ptr<Piece> clone() const override { // clone piece
        auto tmp = make_unique<Bishop>(color);
        tmp->setHasMoved(hasMoved); // set has moved
        return tmp; // return cloned piece
    }
};

#endif
