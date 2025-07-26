#include "King.h"
#include "../Board.h"

using namespace std;
// see King.h for more details 
vector<Move> King::LegalMoves(const Board& board, const Position& position) const { // calc legal moves 
    vector<Move> legalMoves; // vector to store legal moves
    for (int rO = -1; rO <= 1; ++rO) { // loop all
        for (int cO = -1; cO <= 1; ++cO) { // loop all 
            if (rO == 0 && cO == 0) { // if the offset is 0
                continue;
            }
            Position finalPos(position.r + rO, position.c + cO); // calculate new position  
            if (finalPos.isValid() && !isOwn(finalPos, board)) { // if the final position is valid and not own piece
                Move move(position, finalPos, PieceType::KING); // create move
                if (isEnemy(finalPos, board))  move.isCap = true; // set capture to true   
                legalMoves.push_back(move); // add move to legal moves
            }
        }
    }
    
    if (!hasMoved) { // if the king has not moved
        if (board.castle(color, true)) { // if right
            Move kingSideCastle(position, Position(position.r, 6), PieceType::KING); // create castle move
            kingSideCastle.isCast = true; // trigger castling
            legalMoves.push_back(kingSideCastle); // add castle move to legal moves
        }
        if (board.castle(color, false)) { // if left
            Move queenSideCastle(position, Position(position.r, 2), PieceType::KING); // create castle move
            queenSideCastle.isCast = true; // trigger castling
            legalMoves.push_back(queenSideCastle); // add castle move to legal moves
        }
    }
    // return legal moves
    return legalMoves;
}

bool King::isValidMove(const Position& init, const Position& dest, const Board& board) const { // check if move is valid
    if (KingMove(init, dest))  return true; // if move is a king move
    else if (CastlingMove(init, dest)) { // if move is a castling move
        bool side = (dest.c > init.c); // check if king side
        return board.castle(color, side); // return if castle is valid
    }
    return false; // if move is not a king or castling move, return false
}

bool King::KingMove(const Position& init, const Position& dest) const { // check if move is a king move
    int rd = abs(dest.r - init.r); // calculate row difference
    int cd = abs(dest.c - init.c); // calculate column difference
    return (rd <= 1 && cd <= 1) && (rd + cd > 0); // return if move is a king move
}

bool King::CastlingMove(const Position& init, const Position& dest) const { // check if move is a castling move
    if (hasMoved || init.r != dest.r) return false; // if king has moved or row is not the same, return false
    int cd = dest.c - init.c; // calculate column difference
    bool retval = 2 == abs(cd); // return if move is a castling move
    return retval;
}
