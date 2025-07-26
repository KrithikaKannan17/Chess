#include "GraphicalDisplay.h"
#include "window.h"
#include <iostream>
#include <map>

using namespace std;

GraphicalDisplay::GraphicalDisplay(int size, bool greenTiles) : boardSize(size), squareSize(size / 8) { // constructor
    window = make_unique<Xwindow>(size + 80, size + 80); // window
    setupColors(greenTiles); // setup colors
    setupPieceSymbols(); // setup piece symbols

    window->setFont("-*-*-bold-r-*-*-24-*-*-*-*-*-*-*"); // set font        
}

GraphicalDisplay::~GraphicalDisplay() { // destructor

}

void GraphicalDisplay::notifyMove(const Move& move) {
    // Observer pattern implementation - intentionally empty
    // Graphics display updates are handled through renderBoard() calls
}

void GraphicalDisplay::notifyStateChange(GameState state) {
    // Observer pattern implementation - intentionally empty  
    // State changes are handled through specific notify methods like notifyCheckmate()
}

void GraphicalDisplay::notifyNoMove(const Move& move, const string& errmsg) {
    // Observer pattern implementation - intentionally empty
    // Invalid moves are handled by the game logic, no visual feedback needed
}

void GraphicalDisplay::notifyCheck(Color player) {
    // Observer pattern implementation - intentionally empty
    // Check indication is handled through board rendering
}

void GraphicalDisplay::notifyCheckmate(Color loser) { // notify checkmate       
    displayMessage("Checkmate!");       
}

void GraphicalDisplay::notifyDraw(const string& errmsg) { // notify draw
    displayMessage("Draw: " + errmsg);
}

void GraphicalDisplay::renderBoard(const Board& board) { // render board
    window->clear();

    window->fillRectangle(0, 0, boardSize + 80, boardSize + 80, Xwindow::BoardBorder); // fill rectangle

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            drawSquare(row, col, getSquareColor(row, col)); // draw square
            
            const Piece* piece = board.getPiece(row, col); // get piece
            if (piece) {
                drawPieceLetter(piece, row, col); // draw piece as letter
            }
        }
    }

    drawCoordinates(); // draw coordinates
    
    window->flush(); // flush
}

void GraphicalDisplay::highlightSquare(const Position& pos, int color) { // highlight square
    if (pos.isValid()) {
        auto [x, y] = boardToPixel(pos.r, pos.c); // board to pixel
        window->fillRectangle(x, y, squareSize, squareSize, color); // fill rectangle
        window->flush(); // flush
    }
}

void GraphicalDisplay::displayMessage(const string& message) { // display message
    window->drawString(40, boardSize + 70, message, Xwindow::White);
    window->flush();
}

void GraphicalDisplay::drawSquare(int row, int col, int color) { // draw square
    auto [x, y] = boardToPixel(row, col); // board to pixel
    window->fillRectangle(x, y, squareSize, squareSize, color); // fill rectangle
}



void GraphicalDisplay::drawCoordinates() {
    for (int col = 0; col < 8; ++col) {
        char file = 'a' + col;
        auto [x, y] = boardToPixel(0, col);
        
        window->drawStringCentered(x, boardSize + 40, squareSize, 25, string(1, file), Xwindow::White);
        
        window->drawStringCentered(x, 5, squareSize, 25, string(1, file), Xwindow::White);
    }
    
    for (int row = 0; row < 8; ++row) {
        char rank = '1' + row;
        auto [x, y] = boardToPixel(row, 0);
        
        window->drawStringCentered(5, y, 25, squareSize, string(1, rank), Xwindow::White);
        
        window->drawStringCentered(boardSize + 35, y, 25, squareSize, string(1, rank), Xwindow::White);
    }
}

void GraphicalDisplay::clearSquare(int row, int col) {
    drawSquare(row, col, getSquareColor(row, col));
}

pair<int, int> GraphicalDisplay::boardToPixel(int row, int col) {
    return {col * squareSize + 30, (7 - row) * squareSize + 30};
}

pair<int, int> GraphicalDisplay::pixelToBoard(int x, int y) {
    int col = (x - 30) / squareSize; 
    int row = 7 - ((y - 30) / squareSize);
    return {row, col};
}

void GraphicalDisplay::setupColors(bool greenTiles) {
    lightSquareColor = Xwindow::LightSquare;
    darkSquareColor = Xwindow::DarkSquare;
    highlightColor = Xwindow::HighlightYellow;
    borderColor = Xwindow::BoardBorder;

    if (greenTiles) {
        lightSquareColor = Xwindow::LightSquareGreen;
        darkSquareColor = Xwindow::DarkSquareGreen;
    }
}

void GraphicalDisplay::setupPieceSymbols() {
    pieceSymbols[PieceType::KING] = {"K", "K"};
    pieceSymbols[PieceType::QUEEN] = {"Q", "Q"};
    pieceSymbols[PieceType::ROOK] = {"R", "R"};
    pieceSymbols[PieceType::BISHOP] = {"B", "B"};
    pieceSymbols[PieceType::KNIGHT] = {"N", "N"};
    pieceSymbols[PieceType::PAWN] = {"P", "P"};
}

string GraphicalDisplay::getUnicodeChessPiece(const Piece* piece) {
    PieceType type = piece->getType();
    Color color = piece->getColor();
    
    if (pieceSymbols.find(type) != pieceSymbols.end()) {
        return (color == Color::WHITE) ? pieceSymbols[type].first : pieceSymbols[type].second;
    }
    
    return string(1, piece->getSymbol());
}

int GraphicalDisplay::getSquareColor(int row, int col) {
    return ((row + col) % 2 == 1) ? lightSquareColor : darkSquareColor;
}



void GraphicalDisplay::drawPieceLetter(const Piece* piece, int row, int col) {
    auto [x, y] = boardToPixel(row, col);
    
    char symbol = piece->getSymbol();
    string letter(1, symbol);
    
    int textColor = (piece->getColor() == Color::WHITE) ? Xwindow::PieceWhite : Xwindow::PieceBlack;
    
    // Center the letter in the square
    window->drawStringCentered(x, y, squareSize, squareSize, letter, textColor);
}