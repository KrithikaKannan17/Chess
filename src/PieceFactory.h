#ifndef PIECEFACTORY_H
#define PIECEFACTORY_H

#include "Piece.h"
#include "pieces/King.h"
#include "pieces/Queen.h"
#include "pieces/Rook.h"
#include "pieces/Bishop.h"
#include "pieces/Knight.h"
#include "pieces/Pawn.h"
#include <memory>
#include <cctype>

using namespace std;

class PieceFactory {// PieceFactory class
public:
    static unique_ptr<Piece> initPiece(PieceType type, Color color) { // init piece
        if (type == PieceType::KING) { // if king
            return make_unique<King>(color); // return king
        } else if (type == PieceType::QUEEN) { // if queen
            return make_unique<Queen>(color); // return queen
        } else if (type == PieceType::ROOK) { // if rook
            return make_unique<Rook>(color); // return rook
        } else if (type == PieceType::BISHOP) { // if bishop
            return make_unique<Bishop>(color); // return bishop
        } else if (type == PieceType::KNIGHT) { // if knight
            return make_unique<Knight>(color); // return knight
        } else if (type == PieceType::PAWN) { // if pawn
            return make_unique<Pawn>(color); // return pawn
        } else { // if other
            return nullptr; // return nullptr
        }
    }
    
    static unique_ptr<Piece> initPieceFromChar(char sym) { // init piece from char
        Color color = (sym >= 'A' && sym <= 'Z') ? Color::WHITE : Color::BLACK; // get color
        char upperSymbol = toupper(sym); // get upper symbol
        
        PieceType type; // piece type
        if (upperSymbol == 'K') { // if king
            type = PieceType::KING; // set type to king
        } else if (upperSymbol == 'Q') { // if queen
            type = PieceType::QUEEN; // set type to queen
        } else if (upperSymbol == 'R') { // if rook
            type = PieceType::ROOK; // set type to rook
        } else if (upperSymbol == 'B') { // if bishop
            type = PieceType::BISHOP; // set type to bishop
        } else if (upperSymbol == 'N') { // if knight
            type = PieceType::KNIGHT; // set type to knight
        } else if (upperSymbol == 'P') { // if pawn
            type = PieceType::PAWN; // set type to pawn
        } else { // if other
            return nullptr; // return nullptr
        }
        
        return initPiece(type, color); // return piece
    }
    
    static void setPieces(class Board& board); // set pieces
};

#endif 
