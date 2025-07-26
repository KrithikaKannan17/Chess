#include "Knight.h"
#include "../Board.h"

using namespace std;
// see Knight.h for more details
vector<Move> Knight::LegalMoves(const Board& board, const Position& position) const { // calc legal moves
    vector<Move> legalMoves; // vector to store legal moves
    
    const int knightMoves[8][2] = { // direction vector to store the 8 directions of knight's movement
        {2, 1}, // up 2 right 1
        {2, -1}, // up 2 left 1
        {-2, 1}, // down 2 right 1
        {-2, -1}, // down 2 left 1
        {1, 2}, // right 2 up 1
        {1, -2}, // right 2 down 1
        {-1, 2}, // left 2 up 1
        {-1, -2} // left 2 down 1
    };
    
    for (int i = 0; i < 8; i++) { // loop all possible directions
        Position finalPos(position.r + knightMoves[i][0], position.c + knightMoves[i][1]); // calculate new position
        if (finalPos.isValid() && !isOwn(finalPos, board)) { // if new position is valid and not own piece
            Move move(position, finalPos, PieceType::KNIGHT); // create move
            if (isEnemy(finalPos, board)) { // if enemy piece
                move.isCap = true; // trigger capture
            }
            legalMoves.push_back(move); // add move to legal moves
        }
    }
    return legalMoves; // return legal moves
}

bool Knight::isValidMove(const Position& init, const Position& dest, const Board& board) const { // check if move is valid
    return KnightMove(init, dest); // check if move is valid
}

bool Knight::KnightMove(const Position& init, const Position& dest) const { // check if move is valid
    int rd = abs(dest.r - init.r); // row difference
    int cd = abs(dest.c - init.c); // column difference
    bool retval = (rd == 2 && cd == 1) || (rd == 1 && cd == 2); // check if move is valid
    return retval; // return true if move is valid
}
