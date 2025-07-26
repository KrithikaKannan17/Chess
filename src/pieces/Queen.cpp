#include "Queen.h"
#include "../Board.h"

using namespace std;
// see Queen.h for more details
vector<Move> Queen::LegalMoves(const Board& board, const Position& position) const { // calc legal moves
    vector<Move> legalMoves; // vector to store legal moves
    // 8 directions: up, down, right, left, up right, up left, down right, down left
    const int dirs[8][2] = { // direction vector to store the 8 directions of queen's movement
        {1, 0}, // up
        {-1, 0}, // down
        {0, 1}, // right
        {0, -1}, // left
        {1, 1}, // up right
        {1, -1}, // up left
        {-1, 1}, // down right
        {-1, -1} // down left
    };
    
    for (int dir = 0; dir < 8; dir++) { // loop all possible directions
        int rd = dirs[dir][0]; // row direction
        int cd = dirs[dir][1]; // column direction
        
        for (int delta = 1; delta <= 7; ++delta) { // loop all possible distances
            Position finalPos(position.r + delta * rd, position.c + delta * cd); // calculate final position
            if (!finalPos.isValid()) break; // if invalid position
            if (isEmpty(finalPos, board)) { // if empty square
                legalMoves.emplace_back(position, finalPos, PieceType::QUEEN); // add move to legal moves
            } else if (isEnemy(finalPos, board)) { // if enemy piece
                Move capture(position, finalPos, PieceType::QUEEN); // create capture move
                capture.isCap = true; // trigger capture
                legalMoves.push_back(capture); // add capture move to legal moves
                break;
            } else {
                break; // if not empty or enemy piece, break
            }
        }
    }
    return legalMoves; // return legal moves
}

bool Queen::isValidMove(const Position& init, const Position& dest, const Board& board) const { // check if move is valid
    return QueenMove(init, dest) && isPathClear(init, dest, board); // check if move is valid
}

bool Queen::QueenMove(const Position& init, const Position& dest) const { // check if move is valid
    int rd = abs(dest.r - init.r); // row difference
    int cd = abs(dest.c - init.c); // column difference
    bool retval = (rd == 0 && cd > 0) || (cd == 0 && rd > 0) || (rd == cd && rd > 0); // check if move is valid
    return retval; // return true if move is valid
}
