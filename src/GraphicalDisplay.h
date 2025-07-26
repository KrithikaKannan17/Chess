#ifndef GRAPHICALDISPLAY_H
#define GRAPHICALDISPLAY_H

#include "Board.h"
#include <map>
#include <string>
#include <utility>

using namespace std;

class Xwindow; // Xwindow class

class GraphicalDisplay : public Observer { // GraphicalDisplay class
    unique_ptr<Xwindow> window; // window
    int boardSize; // board size
    int squareSize; // square size
    map<PieceType, pair<string, string>> pieceSymbols; // piece symbols
    int lightSquareColor; // light square color
    int darkSquareColor; // dark square color
    int highlightColor; // highlight color
    int borderColor; // border color
    
    void initializeWindow(); // initialize window
    void drawSquare(int row, int col, int color); // draw square
    void drawPieceLetter(const Piece* piece, int row, int col); // draw piece as letter
    void drawCoordinates(); // draw coordinates
    void clearSquare(int row, int col); // clear square

    void setupPieceSymbols(); // setup piece symbols
    string getUnicodeChessPiece(const Piece* piece); // get unicode chess piece
    pair<int, int> boardToPixel(int row, int col); // board to pixel
    pair<int, int> pixelToBoard(int x, int y); // pixel to board
    void setupColors(bool greenTiles = false); // setup colors
    int getSquareColor(int row, int col); // get square color

public:
    GraphicalDisplay(int size = 500, bool greenTiles = false); // constructor
    ~GraphicalDisplay() override; // destructor

    void notifyMove(const Move& move) override;
    void notifyStateChange(GameState state) override;
    void notifyNoMove(const Move& move, const string& errmsg) override;
    void notifyCheck(Color player) override;
    void notifyCheckmate(Color loser) override; // notify checkmate
    void notifyDraw(const string& errmsg) override; // notify draw
    
    void renderBoard(const Board& board); // render board
    void highlightSquare(const Position& pos, int color); // highlight square
    void displayMessage(const string& message); // display message
};

#endif 
