#ifndef TYPES_H
#define TYPES_H
#include <string>
#include <iostream>
using namespace std;

enum class Color { // color enum
    WHITE, BLACK // white and black
};

enum class PieceType { // piece type enum
    KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN
};

enum class GameState { // game state enum
    PLAYING, CHECK, CHECKMATE_WHITE_WINS, CHECKMATE_BLACK_WINS, DRAW_STALEMATE, DRAW_INSUFFICIENT_MATERIAL, DRAW_50_MOVE_RULE, DRAW_REPETITION
};

class Position { // position struct
    public:
    int r;
    int c;
    Position() : r(-1), c(-1) {} // constructor
    Position(int r, int c) : r(r), c(c) {} // constructor
    
    bool isValid() const { // check if position is valid
        bool retval = r >= 0 &&
                      r < 8 &&
                      c >= 0 &&
                      c < 8; // check if row and column are within bounds
        return retval;
    }
    bool operator==(const Position& other) const { // overloaded == operator
        if (r == other.r && c == other.c) {
            return true;
        }
        return false;
    }
    
    bool operator!=(const Position& other) const { // check if position is not equal to another position
        if (!(other == *this)) { // if position is not equal to another position    
            return true;
        }
        return false;
    }
    
    string toAlgebraic() const { // convert position to algebraic notation
        if (!isValid()) {
            return "invalid";
        }
        string retval = string(1, 'a' + c) + string(1, '1' + r); // convert position to algebraic notation
        return retval;
    }
    
    static Position fromAlgebraic(const string& init) { // convert algebraic notation to position
        if (init.length() != 2) {
            return Position(); // return invalid position
        }
        int c = init[0] - 'a'; // convert column to integer
        int r = init[1] - '1'; // convert row to integer
        if (c >= 0 && c < 8 && r >= 0 && r < 8) {
            Position retval(r, c); // create position
            return retval;
        }
        return Position(); // return invalid position
    }
};

class Move { // move struct
    public:
    Position init; // initial position
    Position finalpos; // final position
    PieceType pType; // piece type
    PieceType pProm; // promotion piece
    bool isCap; // is capture
    bool isCast; // is castling
    bool isEnp; // is en passant
    bool isProm; // is promotion
    
    Move() : pType(PieceType::PAWN), pProm(PieceType::QUEEN), isCap(false), isCast(false), isEnp(false), isProm(false) {} // constructor
             
    Move(Position f, Position t, PieceType piece) 
        : init(f), finalpos(t), pType(piece), pProm(PieceType::QUEEN),
          isCap(false), isCast(false), isEnp(false), isProm(false) {} // constructor
    
    string toAlgebraic() const {
        string retval = init.toAlgebraic() + " " + finalpos.toAlgebraic();
        if (isProm) {
            retval += " ";
            if (pProm == PieceType::QUEEN) {
                retval += "Q";
            } else if (pProm == PieceType::ROOK) {
                retval += "R";
            } else if (pProm == PieceType::BISHOP) {
                retval += "B";
            } else if (pProm == PieceType::KNIGHT) {
                retval += "N";
            } else {
                retval += "Q";
            }
        }
        return retval;
    }
};

inline Color oppositeColor(Color c) { // get opposite color
    if (c == Color::WHITE) {
        return Color::BLACK; // return black
    } else {
        return Color::WHITE; // return white
    }
}

inline string colorToString(Color c) { // convert color to string
    if (c == Color::WHITE) {
        return "White"; // return white
    } else {
        return "Black"; // return black
    }
}

inline char pieceTypeToChar(PieceType type, Color color) { // convert piece type to character
    char baseChar; // base character
    if (type == PieceType::KING) { // king
        baseChar = 'K';
    } else if (type == PieceType::QUEEN) { // queen
        baseChar = 'Q';
    } else if (type == PieceType::ROOK) { // rook
        baseChar = 'R';
    } else if (type == PieceType::BISHOP) { // bishop
        baseChar = 'B';
    } else if (type == PieceType::KNIGHT) { // knight
        baseChar = 'N';
    } else if (type == PieceType::PAWN) { // pawn
        baseChar = 'P';
    } else { // invalid piece type
        baseChar = '?';
    }
    if (color == Color::WHITE) { // white
        return baseChar;
    } else { // black
        return baseChar + 'a' - 'A'; // convert to lowercase
    }
}

#endif 
