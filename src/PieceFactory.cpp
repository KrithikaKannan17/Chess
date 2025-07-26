#include "PieceFactory.h"
#include "Board.h"

using namespace std;

void PieceFactory::setPieces(Board& board) { // set pieces
    board.clear(); // clear board
    
    board.setPiece(Position(0, 0), initPiece(PieceType::ROOK, Color::WHITE)); // set rook
    board.setPiece(Position(0, 1), initPiece(PieceType::KNIGHT, Color::WHITE)); // set knight
    board.setPiece(Position(0, 2), initPiece(PieceType::BISHOP, Color::WHITE)); // set bishop
    board.setPiece(Position(0, 3), initPiece(PieceType::QUEEN, Color::WHITE)); // set queen
    board.setPiece(Position(0, 4), initPiece(PieceType::KING, Color::WHITE)); // set king
    board.setPiece(Position(0, 5), initPiece(PieceType::BISHOP, Color::WHITE)); // set bishop
    board.setPiece(Position(0, 6), initPiece(PieceType::KNIGHT, Color::WHITE)); // set knight
    board.setPiece(Position(0, 7), initPiece(PieceType::ROOK, Color::WHITE)); // set rook
    
    for (int col = 0; col < 8; ++col) { // loop through columns
        board.setPiece(Position(1, col), initPiece(PieceType::PAWN, Color::WHITE)); // set pawn
    }
    
    for (int col = 0; col < 8; ++col) { // loop through columns
        board.setPiece(Position(6, col), initPiece(PieceType::PAWN, Color::BLACK)); // set pawn
    }
    
    board.setPiece(Position(7, 0), initPiece(PieceType::ROOK, Color::BLACK)); // set rook
    board.setPiece(Position(7, 1), initPiece(PieceType::KNIGHT, Color::BLACK)); // set knight
    board.setPiece(Position(7, 2), initPiece(PieceType::BISHOP, Color::BLACK)); // set bishop
    board.setPiece(Position(7, 3), initPiece(PieceType::QUEEN, Color::BLACK)); // set queen
    board.setPiece(Position(7, 4), initPiece(PieceType::KING, Color::BLACK)); // set king
    board.setPiece(Position(7, 5), initPiece(PieceType::BISHOP, Color::BLACK)); // set bishop
    board.setPiece(Position(7, 6), initPiece(PieceType::KNIGHT, Color::BLACK)); // set knight
    board.setPiece(Position(7, 7), initPiece(PieceType::ROOK, Color::BLACK)); // set rook
} 
