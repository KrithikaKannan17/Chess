#include "TextDisplay.h"
#include <iostream>
#include <iomanip>
#include <cctype>
#include <string>

using namespace std;

TextDisplay::TextDisplay(ostream& out, bool coords, bool unicode, bool assignment) 
    : output(out), showCoordinates(coords), useUnicode(unicode), assignmentStyle(assignment) {
}

void TextDisplay::displayBoard(const Board& board) {
    if (assignmentStyle) {
        drawAssignmentBoard(board);
    } else {
        drawBoard(board);
    }
}

void TextDisplay::drawBoard(const Board& board) {
    // Top border with file coordinates  
    if (showCoordinates) {
        output << "       ";  // 7 spaces to align coordinates with center of squares
        for (char c = 'a'; c <= 'h'; ++c) {
            output << "    " << c << "    ";  // Perfect centering: 4 + letter + 4 = 9 chars
        }
        output << endl;
    }
    
    // Top border with thick Unicode characters
    output << "  ╔";
    for (int i = 0; i < 8; ++i) {
        output << "═════════";
        if (i < 7) output << "╦";
    }
    output << "╗" << endl;
    
    // Draw board from rank 8 to rank 1 (top to bottom display)
    for (int row = 7; row >= 0; --row) {
        // Top spacing line for each rank
        output << "  ║";
        for (int col = 0; col < 8; ++col) {
            output << "         ║";
        }
        output << endl;
        
        // Middle line with pieces
        if (showCoordinates) {
            output << (row + 1) << " ║";
        } else {
            output << "  ║";
        }
        
        for (int col = 0; col < 8; ++col) {
            const Piece* piece = board.getPiece(row, col);
            
            if (piece) {
                char symbol = piece->getSymbol();
                output << "    " << symbol << "    ║";  // Perfect centering: 4 spaces + letter + 4 spaces
            } else {
                // Empty square with clean spacing
                output << "         ║";
            }
        }
        
        // Right rank number
        if (showCoordinates) {
            output << " " << (row + 1);
        }
        output << endl;
        
        // Bottom spacing line for each rank
        output << "  ║";
        for (int col = 0; col < 8; ++col) {
            output << "         ║";
        }
        output << endl;
        
        // Horizontal separator between ranks (except after last row)
        if (row > 0) {
            output << "  ╠";
            for (int i = 0; i < 8; ++i) {
                output << "═════════";
                if (i < 7) output << "╬";
            }
            output << "╣" << endl;
        }
    }
    
    // Bottom border
    output << "  ╚";
    for (int i = 0; i < 8; ++i) {
        output << "═════════";
        if (i < 7) output << "╩";
    }
    output << "╝" << endl;
    
    // Bottom file coordinates
    if (showCoordinates) {
        output << "       ";  // 7 spaces to align coordinates with center of squares
        for (char c = 'a'; c <= 'h'; ++c) {
            output << "    " << c << "    ";  // Perfect centering: 4 + letter + 4 = 9 chars
        }
        output << endl;
    }
    
    output << endl;
    
    // Enhanced current player info with thick Unicode box
    Color currentPlayer = board.getCurrentPlayer();
    string playerStr = (currentPlayer == Color::WHITE) ? "White to move" : "Black to move";
    string statusStr = "";
    
    if (board.isInCheck(currentPlayer)) {
        statusStr = " (IN CHECK!)";
    }
    
    string fullStr = playerStr + statusStr;
    int boxWidth = fullStr.length() + 4;
    
    // Top border
    output << "╔";
    for (int i = 0; i < boxWidth - 2; ++i) {
        output << "═";
    }
    output << "╗" << endl;
    
    // Content
    output << "║ " << fullStr << " ║" << endl;
    
    // Bottom border
    output << "╚";
    for (int i = 0; i < boxWidth - 2; ++i) {
        output << "═";
    }
    output << "╝" << endl;
}

void TextDisplay::drawAssignmentBoard(const Board& board) {
    // Draw board from rank 8 to rank 1 (like in assignment)
    for (int row = 7; row >= 0; --row) {
        output << (row + 1) << " ";
        
        for (int col = 0; col < 8; ++col) {
            const Piece* piece = board.getPiece(row, col);
            
            if (piece) {
                output << piece->getSymbol();
            } else {
                output << "-";
            }
        }
        output << endl;
    }
    
    // File coordinates at bottom
    output << "  abcdefgh" << endl;
}

void TextDisplay::notifyMove(const Move& move) { // notify move
    string info = move.toAlgebraic();
    if (move.isCap) { // if capture
        info += " (CAPTURE)";
    }
    if (move.isEnp) { // if en passant
        info += " (EN PASSANT)";
    }
    if (move.isCast) { // if castling
        info += " (CASTLING)";
    }
    if (move.isProm) { // if promotion
        info += " (PROMOTION to " + string(1, pieceTypeToChar(move.pProm, Color::WHITE)) + ")";
    }
    output << "Move: " << info << endl;
}

void TextDisplay::notifyStateChange(GameState state) { // notify state change
    if (state == GameState::PLAYING) { // if playing
        return; // return
    }
    if (state == GameState::CHECK) { // if check
        output << endl;
        output << "CHECK!" << endl;
    }
    if (state == GameState::CHECKMATE_WHITE_WINS) { // if checkmate white wins
        output << endl;
        output << "CHECKMATE! WHITE WINS!" << endl;
    } if (state == GameState::CHECKMATE_BLACK_WINS) { // if checkmate black wins
        output << endl;
        output << "CHECKMATE! BLACK WINS!" << endl;
    } if (state == GameState::DRAW_STALEMATE) { // if draw stalemate
        output << endl;
        output << "STALEMATE! GAME IS A DRAW" << endl;
    } if (state == GameState::DRAW_INSUFFICIENT_MATERIAL) { // if draw insufficient material    
        output << endl;
        output << "DRAW: Insufficient material" << endl;
    } if (state == GameState::DRAW_REPETITION) { // if draw repetition
        output << endl; 
        output << "DRAW: Threefold repetition" << endl;
    } if (state == GameState::DRAW_50_MOVE_RULE) { // if draw 50 move rule
        output << endl;
        output << "DRAW: Fifty-move rule" << endl;
    }
}

void TextDisplay::notifyNoMove(const Move& move, const string& errmsg) { // notify no move
    output << "Invalid move: " << move.toAlgebraic() << " - " << errmsg << endl;
}

void TextDisplay::notifyCheck(Color player) { // notify check
    output << colorToString(player) << " IS IN CHECK!" << endl;
}

void TextDisplay::notifyCheckmate(Color loser) { // notify checkmate
    output << endl;
    output << "CHECKMATE! " << colorToString(loser) << " WINS!" << endl;
    output << endl;
}

void TextDisplay::notifyDraw(const string& errmsg) { // notify draw
    output << "DRAW: " << errmsg << endl;
}

void TextDisplay::displayGameInfo(const Board& board) { // display game info
    output << endl;
    output << "GAME INFORMATION" << endl;
    
    string info = "Current Player: " + colorToString(board.getCurrentPlayer()); // current player
    output << info << endl; // output current player
    
    GameState state = board.getGameState(); // get game state
    string stateInfo; // state info
    if (state == GameState::PLAYING) { // if playing
        stateInfo = "Playing"; // set state info to playing
    } else if (state == GameState::CHECK) { // if check
        stateInfo = "Check"; // set state info to check
    } else if (state == GameState::CHECKMATE_WHITE_WINS) { // if checkmate white wins
        stateInfo = "White wins"; // set state info to white wins
    } else if (state == GameState::CHECKMATE_BLACK_WINS) { // if checkmate black wins   
        stateInfo = "Black wins"; // set state info to black wins
    } else { // if draw
        stateInfo = "Draw"; // set state info to draw
    }
    info = "Game State: " + stateInfo; // game state info
    output << info << endl; // output game state info   
    
    auto history = board.getMoveHistory(); // get move history
    int correctMoves = (history.size() / 2) + 1; // get correct moves
    info = "Move Number: " + to_string(correctMoves); // get move number
    output << info << endl; // output move number
}

void TextDisplay::displayMoveHistory(const vector<Move>& moves) { // display move history
    if (moves.empty()) { // if no moves played yet
        output << "No moves played yet" << endl; // output no moves played yet
        return; // return
    }
    
    output << "MOVE HISTORY" << endl; // output move history
    
    for (size_t i = 0; i < moves.size(); i += 2) { // loop through moves
        int delta = (i / 2) + 1; // get delta
        string info = to_string(delta) + ". " + moves[i].toAlgebraic(); // get move info
        
        if (i + 1 < moves.size()) { // if there is a next move
            info += " " + moves[i + 1].toAlgebraic(); // get next move info
        }
        
        output << info << endl; // output move info
    }
}

void TextDisplay::drawCoordinates() { // draw showCoordinates
    if (!showCoordinates) return; // if not show showCoordinates
    
    output << "    "; // output empty
            for (char file = 'a'; file <= 'h'; file++) { // loop through files
        output << "   " << file << "   "; // output file
    }
    output << endl; // output new line
}

char TextDisplay::getPieceSymbol(const Piece* piece) { // get piece symbol
    if (!piece) return ' '; // if no piece
    return piece->getSymbol(); // return piece symbol
}

char TextDisplay::getSquareBackground(int row, int col) { // get square background
    char background; 
    if ((row + col) % 2 == 0) { // if row + col is even
        background = '.'; // set background to dot
    } else { // if row + col is odd
        background = ' '; // set background to space
    }
    return background;
}


string TextDisplay::getUnicodePiece(const Piece* piece) { // get unicode piece
    if (!piece) { // if no piece
        return " "; // return space
    }
    
    PieceType type = piece->getType(); // get piece type
    Color color = piece->getColor(); // get piece color
    if (color == Color::WHITE) { // if white
        if (type == PieceType::KING) { // if king
            return "♔"; // return king
        } else if (type == PieceType::QUEEN) { // if queen
            return "♕"; // return queen
        } else if (type == PieceType::ROOK) { // if rook
            return "♖"; // return rook
        } else if (type == PieceType::BISHOP) { // if bishop
            return "♗"; // return bishop
        } else if (type == PieceType::KNIGHT) { // if knight
            return "♘"; // return knight
        } else if (type == PieceType::PAWN) { // if pawn
            return "♙"; // return pawn
        } else { // if other
            return "?"; // return question mark
        }
    } else { // if black
        if (type == PieceType::KING) { // if king
            return "♚"; // return king
        } else if (type == PieceType::QUEEN) { // if queen
            return "♛"; // return queen
        } else if (type == PieceType::ROOK) { // if rook
            return "♜"; // return rook
        } else if (type == PieceType::BISHOP) { // if bishop
            return "♝"; // return bishop
        } else if (type == PieceType::KNIGHT) { // if knight
            return "♞"; // return knight
        } else if (type == PieceType::PAWN) { // if pawn
            return "♟"; // return pawn
        } else { // if other    
            return "?"; // return question mark
        }
    }
}

string TextDisplay::getASCIIPiece(const Piece* piece) { // get ascii piece
    if (!piece) { // if no piece
        return " "; // return space
    }
    
         PieceType type = piece->getType(); // get piece type
    Color color = piece->getColor();
    
    if (color == Color::WHITE) { // if white
        if (type == PieceType::KING) { // if king
            return "K"; // return king
        } else if (type == PieceType::QUEEN) { // if queen
            return "Q"; // return queen
        } else if (type == PieceType::ROOK) { // if rook
            return "R"; // return rook
        } else if (type == PieceType::BISHOP) { // if bishop
            return "B"; // return bishop
        } else if (type == PieceType::KNIGHT) { // if knight
            return "N"; // return knight
        } else if (type == PieceType::PAWN) { // if pawn
            return "P"; // return pawn
        } else { // if other
            return "?"; // return question mark
        }
    } else { // if black
        if (type == PieceType::KING) { // if king
            return "k"; // return king
        } else if (type == PieceType::QUEEN) { // if queen
            return "q"; // return queen
        } else if (type == PieceType::ROOK) { // if rook
            return "r"; // return rook
        } else if (type == PieceType::BISHOP) { // if bishop
            return "b"; // return bishop
        } else if (type == PieceType::KNIGHT) { // if knight
            return "n"; // return knight
        } else if (type == PieceType::PAWN) { // if pawn
            return "p"; // return pawn
        } else { // if other
            return "?"; // return question mark
        }
    }
}   

void TextDisplay::printBorder() { // print border
    output << endl; // output new line
} 
