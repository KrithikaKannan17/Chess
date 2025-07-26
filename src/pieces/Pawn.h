#ifndef PAWN_H
#define PAWN_H

#include "../Piece.h"

using namespace std;

class Pawn : public Piece {
    bool PawnMove(const Position& init, const Position& dest, const Board& board) const; // check if move is a pawn move
    bool CrossCapture(const Position& init, const Position& dest, const Board& board) const; // check if move is a cross capture
    bool EnPassant(const Position& init, const Position& dest, const Board& board) const; // check if move is an en passant
    bool canPromote(const Position& to) const; // check if can promote
    
    int direction() const { // get direction
        if (Color::WHITE == color) return 1; // if color is white, return 1
        return -1; // return -1
    }
    
    int StartRank() const { // get start rank
        if (Color::WHITE == color) return 1; // if color is white, return 1
        return 6; // return 6
    }
    
    int PromoteRank() const { // get promote rank
        if (Color::WHITE == color) return 7; // if color is white, return 7 as temp
        return 0; // return 0
    }

protected:
    bool isValidMove(const Position& init, const Position& dest, const Board& board) const override; // check if move is valid
    
public:
    Pawn(Color c) : Piece(c, PieceType::PAWN) {} // constructor
    vector<Move> LegalMoves(const Board& board, const Position& position) const override; // calc legal moves
    unique_ptr<Piece> clone() const override { // clone piece
        auto cloned = make_unique<Pawn>(color); // clone piece
        cloned->setHasMoved(hasMoved); // set has moved
        return cloned; // return cloned piece
    }
};

#endif 
