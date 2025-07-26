#include "Rook.h"
#include "../Board.h"

using namespace std;
// see Rook.h for more details 
vector<Move> Rook::LegalMoves(const Board& board, const Position& position) const { // calc legal moves
    vector<Move> legalMoves; // vector to store legal moves
    
    const int dirs[4][2] = { // direction vector to store the 4 directions of rook's movement
        {1, 0}, // up
        {-1, 0}, // down
        {0, 1}, // right
        {0, -1} // left
    };
    
    for (int dir = 0; dir < 4; dir++) { // loop all possible directions
        int rd = dirs[dir][0]; // row direction
        int cd = dirs[dir][1]; // column direction
        
        for (int delta = 1; delta <= 7; ++delta) { // loop all possible distances
            Position finalPos(position.r + delta * rd, position.c + delta * cd); // calculate final position
            
            if (!finalPos.isValid()) break; // if invalid position
            
            if (isEmpty(finalPos, board)) { // if empty square
                legalMoves.emplace_back(position, finalPos, PieceType::ROOK); // add move to legal moves
            } else if (isEnemy(finalPos, board)) { // if enemy piece
                Move capture(position, finalPos, PieceType::ROOK); // create capture move
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

bool Rook::isValidMove(const Position& init, const Position& dest, const Board& board) const { // check if move is valid
    return RookMove(init, dest) && isPathClear(init, dest, board); // check if move is valid
}

bool Rook::RookMove(const Position& init, const Position& dest) const { // check if move is valid
    if (init.r == dest.r && init.c != dest.c) { // if move is horizontal
        return true; // return true
    } else if (init.c == dest.c && init.r != dest.r) { // if move is vertical
        return true; // return true
    }
    return false; // return false
}
