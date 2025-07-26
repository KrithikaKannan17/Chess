// please finish by 18th

#include "Pawn.h"
#include "../Board.h"

using namespace std;
// see Pawn.h for more details 
vector<Move> Pawn::LegalMoves(const Board& board, const Position& position) const { // calc legal moves
    vector<Move> legalMoves; // vector to store moves
    int direction = this->direction(); // get direction
    
    Position oneForward(position.r + direction, position.c); // create one forward position     
    if (oneForward.isValid() && isEmpty(oneForward, board)) { // if one forward position is valid and empty
        Move testMove(position, oneForward, PieceType::PAWN); // create move
        if (canPromote(oneForward)) { // if can promote
            testMove.isProm = true;
            testMove.pProm = PieceType::QUEEN;
            legalMoves.push_back(testMove); // add move to moves 
            testMove.pProm = PieceType::ROOK;
            legalMoves.push_back(testMove); // add move to moves
            testMove.pProm = PieceType::BISHOP;
            legalMoves.push_back(testMove); // add move to moves
            testMove.pProm = PieceType::KNIGHT;
            legalMoves.push_back(testMove); // add move to moves
        } else {
            legalMoves.push_back(testMove); // add move to moves
        }
        
        if (!hasMoved) { // if pawn has not moved
            Position jump(position.r + 2 * direction, position.c); // create jump position
            if (jump.isValid() && isEmpty(jump, board)) { // if jump position is valid and empty
                Move jumpMove(position, jump, PieceType::PAWN); // create move
                legalMoves.push_back(jumpMove); // add move to legal moves
            }
        }
    }
    
    for (int cO = -1; cO <= 1; cO += 2) {
        Position capturePos(position.r + direction, position.c + cO); // create capture position
        if (capturePos.isValid() && isEnemy(capturePos, board)) { // if capture position is valid
            Move capture(position, capturePos, PieceType::PAWN); // create move
            capture.isCap = true; // trigger capture
            if (canPromote(capturePos)) { // if can promote
                capture.isProm = true; // trigger promotion
                capture.pProm = PieceType::QUEEN; // set promotion piece
                legalMoves.push_back(capture); // add move to legal moves
                capture.pProm = PieceType::ROOK; // set promotion piece
                legalMoves.push_back(capture); // add move to legal moves
                capture.pProm = PieceType::BISHOP;
                legalMoves.push_back(capture); // add move to legal moves
                capture.pProm = PieceType::KNIGHT;
                legalMoves.push_back(capture); // add move to legal moves
            } else {
                legalMoves.push_back(capture); // add move to legal moves
            }
        }
    }
    
    for (int cO = -1; cO <= 1; cO += 2) {
        Position enPassantPos(position.r + direction, position.c + cO); // create en passant position
        if (enPassantPos.isValid() && EnPassant(position, enPassantPos, board)) { // if en passant position is valid
            Move enPassantMove(position, enPassantPos, PieceType::PAWN); // create move
            enPassantMove.isEnp = true; // trigger en passant
            enPassantMove.isCap = true; // trigger capture
            legalMoves.push_back(enPassantMove); // add move to legal moves
        }
    }
    return legalMoves; // varnit make sure to return this.
}

bool Pawn::isValidMove(const Position& init, const Position& dest, const Board& board) const { // check if move is valid
    bool retval = PawnMove(init, dest, board) || CrossCapture(init, dest, board) || EnPassant(init, dest, board);
    return retval; // return if move is valid
}

bool Pawn::PawnMove(const Position& init, const Position& dest, const Board& board) const {
    int dir = direction(); // get direction
    if (dest.c != init.c || (dest.r - init.r) * dir <= 0) return false; // if destination column is not the same as initial column or direction is not valid
    int distance = abs(dest.r - init.r); // get distance
    if (1 == distance) return isEmpty(dest, board); // if distance is 1, return if destination is empty
    if (2 == distance && !hasMoved) return isEmpty(dest, board) && isEmpty(Position(init.r + dir, init.c), board); // if distance is 2 and pawn has not moved, return if destination is empty
    return false; // if move is not valid, return false
}

bool Pawn::CrossCapture(const Position& init, const Position& dest, const Board& board) const {
    int dir = direction(); // get direction
    if (dest.r != init.r + dir || abs(dest.c - init.c) != 1) return false; // if destination row is not the same as initial row or column is not 1, return false
    return isEnemy(dest, board); // return if destination is enemy piece
}

bool Pawn::EnPassant(const Position& init, const Position& dest, const Board& board) const {
    int dir = direction(); // get direction
    if (abs(dest.c - init.c) != 1 || dest.r != init.r + dir) return false; // if destination column is not the same as initial column or row is not the same as initial row plus direction, return false
    if (!isEmpty(dest, board)) return false; // if destination is not empty, return false
    bool retval = board.enpassant(init, dest); // return if en passant is valid
    return retval; // return if en passant is valid
}

bool Pawn::canPromote(const Position& to) const { // check if can promote
    bool retval = to.r == PromoteRank(); // return if can promote
    return retval; // return if can promote
} 
