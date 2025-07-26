#ifndef KING_H
#define KING_H

#include "../Piece.h"

using namespace std;

class King : public Piece {
    bool KingMove(const Position& init, const Position& dest) const; // check if move is a king move
    bool CastlingMove(const Position& init, const Position& dest) const; // check if move is a castling move    

protected:
    bool isValidMove(const Position& init, const Position& dest, const Board& board) const override; // check if move is valid
    
public:
    King(Color c) : Piece(c, PieceType::KING) {} // constructor
    
    vector<Move> LegalMoves(const Board& board, const Position& position) const override; // calc legal moves
    unique_ptr<Piece> clone() const override {
        auto cloned = make_unique<King>(color); // clone piece
        cloned->setHasMoved(hasMoved); // set has moved
        return cloned; // return cloned piece
    }
};

#endif 
