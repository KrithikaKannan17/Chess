#ifndef ROOK_H
#define ROOK_H

#include "../Piece.h"

using namespace std;

class Rook : public Piece {
    bool RookMove(const Position& init, const Position& dest) const;

protected:
    bool isValidMove(const Position& init, const Position& dest, const Board& board) const override;
    
public:
    Rook(Color c) : Piece(c, PieceType::ROOK) {}
    
    vector<Move> LegalMoves(const Board& board, const Position& position) const override;
    unique_ptr<Piece> clone() const override {
        auto cloned = make_unique<Rook>(color);
        cloned->setHasMoved(hasMoved);
        return cloned;
    }
};

#endif
