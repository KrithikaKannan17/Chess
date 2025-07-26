#ifndef BOARD_H
#define BOARD_H

#include "types.h"
#include "Piece.h"
#include "PieceFactory.h"
#include <array>
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

class Observer;

// Board class
class Board {
    array<array<unique_ptr<Piece>, 8>, 8> squares; // squares
    vector<Observer*> observers; // observers
    Color currentPlayer; // current player
    GameState gameState; // game state
    Position enPassantTarget; // en passant target
    int halfmoveClock; // halfmove clock
    int fullmoveNumber; // fullmove number
    vector<Move> moveHistory; // move history
    
    bool executeMove(const Move& move); // execute move
    bool wouldBeInCheckAfterMove(const Move& move) const; // would be in check after move
    void updateCastlingAvailability(); // update castling availability
    bool isPathClearForCastling(Color player, bool kingSide) const; // is path clear for castling
    void updateEnPassantTarget(const Move& move); // update en passant target
    bool isInsufficientMaterial() const; // is insufficient material
    bool is50MoveRule() const; // is 50 move rule
    bool isThreefoldRepetition() const; // is threefold repetition
    Move createMove(const Position& from, const Position& to, PieceType promotion = PieceType::QUEEN) const; // create move
    bool isPromotion(const Move& move) const; // is promotion
    bool isCastling(const Move& move) const; // is castling
    
public:
    Board(); // constructor
    ~Board() = default; // destructor
    
    Board(const Board& other); // copy constructor
    Board& operator=(const Board& other);
    unique_ptr<Board> createCopy() const; // create copy        
    
    void attach(Observer* observer); // attach observer
    void detach(const Observer* observer); // detach observer
    void notifyObservers(); // notify observers
    void notifyMove(const Move& move); // notify move
    void notifyStateChange(GameState state); // notify state change
    void notifyNoMove(const Move& move, const string& errmsg); // notify no move
    void notifyCheck(Color player); // notify check
    void notifyCheckmate(Color loser); // notify checkmate
    void notifyDraw(const string& errmsg); // notify draw
    
    bool move(const string& from, const string& to, PieceType promotion = PieceType::QUEEN); // move
    bool move(const Move& move); // move
    bool isValidMove(const Position& from, const Position& to) const; // is valid move
    bool isValidMove(const Move& move) const; // is valid move
    
    const Piece* getPiece(const Position& pos) const; // get piece
    const Piece* getPiece(int row, int col) const; // get piece
    void setPiece(const Position& pos, unique_ptr<Piece> piece); // set piece
    void removePiece(const Position& pos); // remove piece
    
    bool isInCheck(Color player) const; // is in check
    bool isCheckmate(Color player) const; // is checkmate
    bool isStalemate(Color player) const; // is stalemate
    bool isDraw() const; // is draw
    Color getCurrentPlayer() const { return currentPlayer; } // get current player
    void setCurrentPlayer(Color player) { currentPlayer = player; } // set current player
    GameState getGameState() const { return gameState; } // get game state
    const vector<Move>& getMoveHistory() const { return moveHistory; } // get move history
    
    void setup(); // setup
    void clear(); // clear
    bool isValidSetup() const; // is valid setup
    
    vector<Move> getAllLegalMoves(Color player) const; // get all legal moves
    vector<Move> getLegalMoves(const Position& pos) const; // get legal moves
    
    bool castle(Color player, bool kingSide) const; // castle
    bool enpassant(const Position& from, const Position& to) const; // enpassant
    
    void display() const; // display
    friend ostream& operator<<(ostream& os, const Board& board); // operator<<
    
    Position findKing(Color player) const; // find king
    bool isSquareAttacked(const Position& pos, Color byPlayer) const; // is square attacked
    
    bool isSquareAttackedByPawn(const Position& pos, Color byPlayer) const; // is square attacked by pawn
    bool isSquareAttackedByRook(const Position& pos, Color byPlayer) const; // is square attacked by rook
    bool isSquareAttackedByBishop(const Position& pos, Color byPlayer) const; // is square attacked by bishop
    bool isSquareAttackedByQueen(const Position& pos, Color byPlayer) const; // is square attacked by queen
    bool isSquareAttackedByKnight(const Position& pos, Color byPlayer) const; // is square attacked by knight
    bool isSquareAttackedByKing(const Position& pos, Color byPlayer) const; // is square attacked by king
    
    void switchPlayer(); // switch player
    
    string toFEN() const; // to FEN
    bool loadFromFEN(const string& fen); // load from FEN
    
    bool applyMoveToBoard(const Move& move); // apply move to board
};

// Observer class
class Observer {
public:
    virtual ~Observer() = default; // destructor
    virtual void notifyMove(const Move& move) = 0; // notify move
    virtual void notifyStateChange(GameState state) = 0; // notify state change
    virtual void notifyNoMove(const Move& move, const string& errmsg) = 0; // notify no move
    virtual void notifyCheck(Color player) = 0; // notify check
    virtual void notifyCheckmate(Color loser) = 0; // notify checkmate
    virtual void notifyDraw(const string& errmsg) = 0; // notify draw
};

#endif 
