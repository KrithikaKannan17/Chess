#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H

#include "Board.h"
#include <iostream>
#include <string>

using namespace std;

class TextDisplay : public Observer {
    ostream& output;
    bool showCoordinates;
    bool useUnicode;
    bool assignmentStyle;
    
    void drawBoard(const Board& board);
    void drawAssignmentBoard(const Board& board);
    void drawCoordinates();
    char getPieceSymbol(const Piece* piece);
    char getSquareBackground(int row, int col);
    string getUnicodePiece(const Piece* piece);
    string getASCIIPiece(const Piece* piece);
    void printBorder();
    
public:
    TextDisplay(ostream& out = cout, bool coords = true, bool unicode = false, bool assignment = false);
    ~TextDisplay() override = default;
    
    void notifyMove(const Move& move) override;
    void notifyStateChange(GameState state) override;
    void notifyNoMove(const Move& move, const string& errmsg) override;
    void notifyCheck(Color player) override;
    void notifyCheckmate(Color loser) override;
    void notifyDraw(const string& errmsg) override;
    
    void displayBoard(const Board& board);
    void displayGameInfo(const Board& board);
    void displayMoveHistory(const vector<Move>& moves);
    
    void setShowCoordinates(bool show) { showCoordinates = show; }
    void setUseUnicode(bool unicode) { useUnicode = unicode; }
};

#endif 
