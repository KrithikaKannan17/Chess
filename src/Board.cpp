#include "Board.h"
#include "PieceFactory.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cctype>

using namespace std;
// Board class
Board::Board() : currentPlayer(Color::WHITE), 
                 gameState(GameState::PLAYING),
                 enPassantTarget(-1, -1),
                 halfmoveClock(0),
                 fullmoveNumber(1) { // constructor with default values
    for (int rowNum = 0; rowNum < 8; ++rowNum) { // initialize squares
        for (int colNum = 0; colNum < 8; ++colNum) {
            squares[rowNum][colNum] = nullptr; // initialize squares
        }
    }
}

// copy constructor
Board::Board(const Board& other) // copy constructor with default values
    : currentPlayer(other.currentPlayer), // copy current player
      gameState(other.gameState), // copy game state
      enPassantTarget(other.enPassantTarget), // copy en passant target
      halfmoveClock(other.halfmoveClock), // copy halfmove clock
      fullmoveNumber(other.fullmoveNumber), // copy fullmove number
      moveHistory(other.moveHistory) { // copy move history

    for (int rowNum = 0; rowNum < 8; ++rowNum) { // initialize squares
        for (int colNum = 0; colNum < 8; ++colNum) {
            if (other.squares[rowNum][colNum]) { // if square is not empty
                squares[rowNum][colNum] = other.squares[rowNum][colNum]->clone(); // copy square
            } else {
                squares[rowNum][colNum] = nullptr; // initialize square
            }
        }
    }
}

// assignment operator
Board& Board::operator=(const Board& other) {
    if (this != &other) {
        currentPlayer = other.currentPlayer; // copy current player
        gameState = other.gameState; // copy game state
        enPassantTarget = other.enPassantTarget; // copy en passant target
        halfmoveClock = other.halfmoveClock; // copy halfmove clock
        fullmoveNumber = other.fullmoveNumber;
        moveHistory = other.moveHistory;

        for (int rowNum = 0; rowNum < 8; ++rowNum) { // initialize squares
            for (int colNum = 0; colNum < 8; ++colNum) {
                if (other.squares[rowNum][colNum]) { // if square is not empty
                    squares[rowNum][colNum] = other.squares[rowNum][colNum]->clone(); // copy square
                } else {
                    squares[rowNum][colNum] = nullptr; // initialize square
                }
            }
        }

        observers.clear(); // clear observers
    }
    return *this;
}

// create copy
unique_ptr<Board> Board::createCopy() const {
    return make_unique<Board>(*this); // create copy
}

// attach observer
void Board::attach(Observer* observer) {
    if (observer) {
        observers.push_back(observer); // add observer
    }
}

// detach observer
void Board::detach(const Observer* observer) {
    observers.erase(
        remove(observers.begin(), observers.end(), observer), // remove observer
        observers.end() // remove observer
    );
}

// get piece
const Piece* Board::getPiece(const Position& pos) const {
    if (!pos.isValid()) return nullptr;
    return squares[pos.r][pos.c].get(); // get piece
}

// get piece
const Piece* Board::getPiece(int row, int col) const {
    return getPiece(Position(row, col)); // get piece
}

// set piece
void Board::setPiece(const Position& pos, unique_ptr<Piece> piece) {
    if (!pos.isValid()) return;
    squares[pos.r][pos.c] = std::move(piece); // set piece
}

// remove piece
void Board::removePiece(const Position& pos) {
    if (!pos.isValid()) return;
    squares[pos.r][pos.c] = nullptr; // remove piece
}

// move
bool Board::move(const string& from, const string& to, PieceType promotion) {
    Position fromPos = Position::fromAlgebraic(from); // from position
    Position toPos = Position::fromAlgebraic(to); // to position
    
    if (!fromPos.isValid() || !toPos.isValid()) {
        return false; // invalid move
    }
    
    Move moveObj = createMove(fromPos, toPos, promotion); // create move
    
    if (!moveObj.init.isValid()) {
        return false; // invalid move
    }
    
    return move(moveObj); // move
}

// move
bool Board::move(const Move& move) {
    if (!isValidMove(move)) {
        notifyNoMove(move, "Invalid move"); // notify no move
        return false; // invalid move
    }
    
    if (executeMove(move)) {
        moveHistory.push_back(move); // add move to move history
        
        updateEnPassantTarget(move); // update en passant target
        
        switchPlayer(); // switch player
        notifyObservers(); // notify observers
        
        notifyMove(move); // notify move
        notifyStateChange(gameState); // notify state change
        
        return true; // valid move
    }
    
    return false; // invalid move
}

// is valid move
bool Board::isValidMove(const Position& from, const Position& to) const {
    const Piece* piece = getPiece(from); // get piece
    if (!piece) return false; // invalid move
    if (piece->getColor() != currentPlayer) return false; // invalid move
    
    if (!piece->isLegal(from, to, *this)) return false; // invalid move
    
    Move move = createMove(from, to); // create move
    
    if (!move.init.isValid()) return false;
    
    if (wouldBeInCheckAfterMove(move)) return false;
    
    return true;
}

// is valid move
bool Board::isValidMove(const Move& move) const {
    return isValidMove(move.init, move.finalpos); // is valid move
}

// is in check
bool Board::isInCheck(Color player) const {
    Position kingPos = findKing(player); // find king
    if (!kingPos.isValid()) return false; // invalid move
    
    return isSquareAttacked(kingPos, oppositeColor(player)); // is square attacked
}

// is checkmate
bool Board::isCheckmate(Color player) const {
    if (!isInCheck(player)) return false; // is not in check
    
    std::vector<Move> legalMoves = getAllLegalMoves(player);
    return legalMoves.empty();
}

// is stalemate
bool Board::isStalemate(Color player) const {
    if (isInCheck(player)) return false;   // is not in check
    
    std::vector<Move> legalMoves = getAllLegalMoves(player);
    return legalMoves.empty();
}

// is draw
bool Board::isDraw() const {
    return isStalemate(currentPlayer) || 
           isInsufficientMaterial() || // is insufficient material
           is50MoveRule() || // is 50 move rule
           isThreefoldRepetition(); // is threefold repetition
}

// setup
void Board::setup() {
    clear();
    PieceFactory::setPieces(*this); // set pieces
    currentPlayer = Color::WHITE; // set current player
    gameState = GameState::PLAYING; // set game state
    enPassantTarget = Position(-1, -1); // set en passant target
    halfmoveClock = 0; // set halfmove clock
    fullmoveNumber = 1; // set fullmove number
    moveHistory.clear(); // clear move history
    
    notifyObservers(); // notify observers
}

// clear
void Board::clear() {
    for (int rowNum = 0; rowNum < 8; ++rowNum) { // initialize squares
        for (int colNum = 0; colNum < 8; ++colNum) {
            squares[rowNum][colNum] = nullptr; // initialize square
        }
    }
}

// is valid setup
bool Board::isValidSetup() const {
    int whiteKingNum = 0, blackKingNum = 0; // initialize white kings and black kings
    bool pawnsOnBackRanks = false; // initialize pawns on back ranks
    
    for (int rowNum = 0; rowNum < 8; ++rowNum) {
        for (int colNum = 0; colNum < 8; ++colNum) {
            const Piece* piece = getPiece(rowNum, colNum);
            if (!piece) continue;
            
            if (piece->getType() == PieceType::KING) {
                if (piece->getColor() == Color::WHITE) whiteKingNum++; // increment white kings
                else blackKingNum++; // increment black kings
            }
            
            if (piece->getType() == PieceType::PAWN && (rowNum == 0 || rowNum == 7)) {
                pawnsOnBackRanks = true; // set pawns on back ranks
            }
        }
    }
    
    if (whiteKingNum != 1 || blackKingNum != 1 || pawnsOnBackRanks) {
        return false; // invalid setup
    }
    
    return !isInCheck(Color::WHITE) && !isInCheck(Color::BLACK); // is valid setup
}

// get all legal moves
vector<Move> Board::getAllLegalMoves(Color player) const {
    vector<Move> legalMoves; // initialize legal moves
    
    for (int rowNum = 0; rowNum < 8; ++rowNum) { // initialize squares
        for (int colNum = 0; colNum < 8; ++colNum) { // initialize squares
            const Piece* piece = getPiece(Position(rowNum, colNum)); // get piece
            if (piece && piece->getColor() == player) { // if piece is not empty and color is player
                vector<Move> pieceMoves = getLegalMoves(Position(rowNum, colNum)); // get legal moves
                legalMoves.insert(legalMoves.end(), pieceMoves.begin(), pieceMoves.end()); // insert legal moves
            }
        }
    }
    
    return legalMoves; // get all legal moves
}

// get legal moves
vector<Move> Board::getLegalMoves(const Position& pos) const {
    const Piece* piece = getPiece(pos); // get piece
    if (!piece) return {}; // invalid move
    
    vector<Move> pseudoLegalMoves = piece->LegalMoves(*this, pos); // get pseudo legal moves
    vector<Move> legalMoves; // initialize legal moves
    
    for (const Move& move : pseudoLegalMoves) { // for each move
        if (!wouldBeInCheckAfterMove(move)) { // if move is not in check
            legalMoves.push_back(move); // add move to legal moves
        }
    }
    
    return legalMoves; // get legal moves
}

// display
void Board::display() const {
    cout << "  abcdefgh\n"; // display board
    for (int rowNum = 7; rowNum >= 0; --rowNum) { // display board
        cout << (rowNum + 1) << " "; // display row
        for (int colNum = 0; colNum < 8; ++colNum) { // display board
            const Piece* piece = getPiece(rowNum, colNum);
            if (piece) { // if piece is not empty
                cout << piece->getSymbol(); // display piece
            } else { // if piece is empty
                if ((rowNum + colNum) % 2 == 1) {
                    cout << ' '; // display empty square
                } else {
                    cout << '_'; // display empty square
                }
            }
        }
        cout << " " << (rowNum + 1) << "\n"; // display row
    }
    cout << "  abcdefgh\n"; // display board
    
    if (isInCheck(currentPlayer)) { // if current player is in check
        cout << colorToString(currentPlayer) << " is in check!\n"; // display check
    }
}

// operator<<
std::ostream& operator<<(std::ostream& os, const Board& board) {
    board.display();
    return os; // display board
}

// find king
Position Board::findKing(Color player) const {
    for (int rowNum = 0; rowNum < 8; ++rowNum) { // initialize squares
        for (int colNum = 0; colNum < 8; ++colNum) { // initialize squares
            const Piece* piece = getPiece(rowNum, colNum); // get piece
            if (piece && piece->getType() == PieceType::KING && piece->getColor() == player) { // if piece is not empty and type is king and color is player
                return Position(rowNum, colNum); // return position
            }
        }
    }
    return Position(-1, -1); // invalid move
}

// is square attacked
bool Board::isSquareAttacked(const Position& pos, Color byPlayer) const {
    return isSquareAttackedByPawn(pos, byPlayer) || // is square attacked by pawn
           isSquareAttackedByRook(pos, byPlayer) || // is square attacked by rook
           isSquareAttackedByBishop(pos, byPlayer) || // is square attacked by bishop
           isSquareAttackedByQueen(pos, byPlayer) || // is square attacked by queen
           isSquareAttackedByKnight(pos, byPlayer) || // is square attacked by knight
           isSquareAttackedByKing(pos, byPlayer); // is square attacked by king
}

// is square attacked by pawn
bool Board::isSquareAttackedByPawn(const Position& pos, Color byPlayer) const {
    int attackDir; // initialize attack direction
    if (byPlayer == Color::WHITE) { // if player is white   
        attackDir = 1; // set attack direction to 1
    } else {
        attackDir = -1; // set attack direction to -1
    }
    
    Position leftAttack(pos.r - attackDir, pos.c - 1); // left attack
    Position rightAttack(pos.r - attackDir, pos.c + 1); // right attack
    
    if (leftAttack.isValid()) {
        const Piece* piece = getPiece(leftAttack); // get piece
        if (piece && piece->getType() == PieceType::PAWN && piece->getColor() == byPlayer) { // if piece is not empty and type is pawn and color is player
            return true; // return true
        }
    }
    if (rightAttack.isValid()) {
        const Piece* piece = getPiece(rightAttack); // get piece
        if (piece && piece->getType() == PieceType::PAWN && piece->getColor() == byPlayer) { // if piece is not empty and type is pawn and color is player
            return true; // return true
        }
    }
    
    return false; // return false
}

// is square attacked by rook
bool Board::isSquareAttackedByRook(const Position& pos, Color byPlayer) const {
    const int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}; // directions
    
    for (int dirNum = 0; dirNum < 4; dirNum++) {
        int rowDir = directions[dirNum][0]; // row direction
        int colDir = directions[dirNum][1]; // column direction
        
        for (int dist = 1; dist < 8; dist++) {
            Position checkPos(pos.r + dist * rowDir, pos.c + dist * colDir); // check position
            if (!checkPos.isValid()) break; // if check position is invalid
            
            const Piece* piece = getPiece(checkPos); // get piece
            if (piece) {
                if (piece->getColor() == byPlayer && piece->getType() == PieceType::ROOK) {
                    return true; // return true
                }
                break; // break
            }
        }
    }
    
    return false; // return false
}

// is square attacked by bishop
bool Board::isSquareAttackedByBishop(const Position& pos, Color byPlayer) const {
    const int directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}; // directions    
    
    for (int dirNum = 0; dirNum < 4; dirNum++) {
        int rowDir = directions[dirNum][0]; // row direction
        int colDir = directions[dirNum][1]; // column direction
        
        for (int dist = 1; dist < 8; dist++) {
            Position checkPos(pos.r + dist * rowDir, pos.c + dist * colDir); // check position
            if (!checkPos.isValid()) break; // if check position is invalid
            
            const Piece* piece = getPiece(checkPos); // get piece
            if (piece) {
                if (piece->getColor() == byPlayer && piece->getType() == PieceType::BISHOP) {
                    return true; // return true
                }
                break; // break
            }
        }
    }
    
    return false; // return false
}

// is square attacked by queen
bool Board::isSquareAttackedByQueen(const Position& pos, Color byPlayer) const {
    const int directions[8][2] = { // directions
        {0, 1}, {0, -1}, {1, 0}, {-1, 0}, // directions
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // directions
    };
    
    for (int d = 0; d < 8; d++) {
        int rowDir = directions[d][0]; // row direction
        int colDir = directions[d][1]; // column direction
        
        for (int dist = 1; dist < 8; dist++) {
            Position checkPos(pos.r + dist * rowDir, pos.c + dist * colDir); // check position
            if (!checkPos.isValid()) break; // if check position is invalid
            
            const Piece* piece = getPiece(checkPos); // get piece
            if (piece) {
                if (piece->getColor() == byPlayer && piece->getType() == PieceType::QUEEN) {
                    return true; // return true
                }
                break; // break
            }
        }
    }
    
    return false; // return false
}

// is square attacked by knight
bool Board::isSquareAttackedByKnight(const Position& pos, Color byPlayer) const {
    const int knightMoves[8][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, // knight moves
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2} // knight moves
    };
    
    for (int moveNum = 0; moveNum < 8; moveNum++) {
        Position knightPos(pos.r + knightMoves[moveNum][0], pos.c + knightMoves[moveNum][1]); // knight position
        if (knightPos.isValid()) { // if knight position is valid
            const Piece* piece = getPiece(knightPos); // get piece
            if (piece && piece->getType() == PieceType::KNIGHT && piece->getColor() == byPlayer) {
                return true; // return true
            }
        }
    }
    
    return false;
}

// is square attacked by king
bool Board::isSquareAttackedByKing(const Position& pos, Color byPlayer) const {
    const int kingMoves[8][2] = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0}, // king moves
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // king moves
    };
    
    for (int moveNum = 0; moveNum < 8; moveNum++) {
        Position kingPos(pos.r + kingMoves[moveNum][0], pos.c + kingMoves[moveNum][1]); // king position
        if (kingPos.isValid()) { // if king position is valid
            const Piece* piece = getPiece(kingPos); // get piece
            if (piece && piece->getType() == PieceType::KING && piece->getColor() == byPlayer) {
                return true; // return true
            }
        }
    }
    
    return false;
}

// switch player
void Board::switchPlayer() {
    currentPlayer = oppositeColor(currentPlayer); // switch player
    if (currentPlayer == Color::WHITE) { // if current player is white
        fullmoveNumber++; // increment fullmove number
    }
}

// notify observers
void Board::notifyObservers() {
    if (isCheckmate(currentPlayer)) { // if current player is checkmate
        gameState = (currentPlayer == Color::WHITE) ? 
            GameState::CHECKMATE_BLACK_WINS : GameState::CHECKMATE_WHITE_WINS; // set game state
        
        notifyCheckmate(currentPlayer);
    } else if (isStalemate(currentPlayer)) { // if current player is stalemate
        gameState = GameState::DRAW_STALEMATE; // set game state
        
        notifyDraw("Stalemate"); // notify draw
    } else if (isInCheck(currentPlayer)) { // if current player is in check
        gameState = GameState::CHECK; // set game state
        
        notifyCheck(currentPlayer); // notify check
    } else { // if current player is not in check, stalemate, or checkmate
        gameState = GameState::PLAYING; // set game state
    }
    notifyStateChange(gameState); // notify state change
}

// execute move
bool Board::executeMove(const Move& move) {
    const Piece* piece = getPiece(move.init); // get piece
    if (!piece) return false; // if piece is empty
    
    if (move.isCast) { // if move is castling
        bool kingSide = (move.finalpos.c > move.init.c); // if king side
        
        int RookCol; // initialize rook from column
        if (kingSide) { // if king side
            RookCol = 7; // rook from column
        } else { // if queen side
            RookCol = 0; // rook from column
        }
        int RookToCol; // initialize rook to column
        if (kingSide) { // if king side
            RookToCol = 5; // rook to column
        } else { // if queen side
            RookToCol = 3; // rook to column
        }
        
        Position rookFrom(move.init.r, RookCol); // rook from position
        Position rookTo(move.init.r, RookToCol); // rook to position
        
        // Execute castling
        auto king = std::move(squares[move.init.r][move.init.c]); // get piece
        squares[move.init.r][move.init.c] = nullptr; // set piece
        squares[move.finalpos.r][move.finalpos.c] = std::move(king); // set piece
        
        auto rook = std::move(squares[rookFrom.r][rookFrom.c]); // get piece
        squares[rookFrom.r][rookFrom.c] = nullptr; // set piece
        squares[rookTo.r][rookTo.c] = std::move(rook); // set piece
        
        return true; // return true
    }
    
    if (move.isEnp) { // if move is en passant
        const Piece* movingPiece = getPiece(move.init); // get piece
        if (!movingPiece) return false; // if piece is empty
        
        Position capturedPawnPos; // captured pawn position
        if (movingPiece->getColor() == Color::WHITE) { // if moving piece is white
            capturedPawnPos = Position(move.finalpos.r - 1, move.finalpos.c); // captured pawn position
        } else { // if moving piece is black
            capturedPawnPos = Position(move.finalpos.r + 1, move.finalpos.c); // captured pawn position
        }
        
        if (capturedPawnPos.isValid()) {
            squares[capturedPawnPos.r][capturedPawnPos.c] = nullptr; // set piece to nullptr
        }
        
        auto pawn = std::move(squares[move.init.r][move.init.c]); // get piece
        pawn->setHasMoved(true); // set has moved
        squares[move.finalpos.r][move.finalpos.c] = std::move(pawn); // set piece
        
        return true; // return true
    }
    
    auto movingPiece = std::move(squares[move.init.r][move.init.c]); // get piece
    if (!movingPiece) return false; // if piece is empty
    
    movingPiece->setHasMoved(true); // set has moved
    
    if (move.isProm) { // if move is promotion
        movingPiece = PieceFactory::initPiece(move.pProm, movingPiece->getColor()); // init piece
    }
    squares[move.finalpos.r][move.finalpos.c] = std::move(movingPiece); // set piece
    
    return true;
}

// would be in check after move
bool Board::wouldBeInCheckAfterMove(const Move& move) const {
    const Piece* movingPiece = getPiece(move.init); // get piece
    if (!movingPiece) return false; // if piece is empty
    
    Color movingPlayerColor = movingPiece->getColor(); // get moving player color
    
    auto boardCopy = createCopy(); // create copy
    
    if (!boardCopy->applyMoveToBoard(move)) { // if move is not valid
        return true; // return true
    }
    
    Position kingPos = boardCopy->findKing(movingPlayerColor); // find king
    if (!kingPos.isValid()) { // if king position is invalid
        return true; // return true
    }
    
    bool inCheck = boardCopy->isSquareAttacked(kingPos, oppositeColor(movingPlayerColor)); // is square attacked
    
    return inCheck; // return in check
}

// is insufficient material
bool Board::isInsufficientMaterial() const {
    return false; // return false
}

// is 50 move rule
bool Board::is50MoveRule() const {
    return halfmoveClock >= 100; // return halfmove clock is greater than or equal to 100
}

// is threefold repetition
bool Board::isThreefoldRepetition() const {
    return false;
}

// apply move to board
bool Board::applyMoveToBoard(const Move& move) {
    const Piece* piece = getPiece(move.init); // get piece
    if (!piece) return false; // if piece is empty
    
    if (move.isCast) { // if move is castling
        bool kingSide = (move.finalpos.c > move.init.c); // if king side
        
        int RookCol; // initialize rook from column
        if (kingSide) { // if king side
            RookCol = 7; // rook from column
        } else { // if queen side
            RookCol = 0; // rook from column
        }
        int RookToCol; // initialize rook to column
        if (kingSide) { // if king side
            RookToCol = 5; // rook to column
        } else { // if queen side
            RookToCol = 3; // rook to column
        }
        
        Position rookFrom(move.init.r, RookCol); // rook from position
        Position rookTo(move.init.r, RookToCol); // rook to position
        
        auto king = std::move(squares[move.init.r][move.init.c]); // get piece
        king->setHasMoved(true); // set has moved
        squares[move.finalpos.r][move.finalpos.c] = std::move(king); // set piece
        
        auto rook = std::move(squares[rookFrom.r][rookFrom.c]); // get piece
        rook->setHasMoved(true); // set has moved
        squares[rookTo.r][rookTo.c] = std::move(rook); // set piece
        
        return true; // return true
    }
    
    if (move.isEnp) { // if move is en passant
        Position capturedPawnPos; // captured pawn position
        if (piece->getColor() == Color::WHITE) { // if piece is white
            capturedPawnPos = Position(move.finalpos.r - 1, move.finalpos.c); // captured pawn position
        } else { // if piece is black
            capturedPawnPos = Position(move.finalpos.r + 1, move.finalpos.c); // captured pawn position
        }
        
        if (capturedPawnPos.isValid()) {
            squares[capturedPawnPos.r][capturedPawnPos.c] = nullptr; // set piece to nullptr
        }
    }
    
    auto movingPiece = std::move(squares[move.init.r][move.init.c]); // get piece
    if (!movingPiece) return false; // if piece is empty
    
    movingPiece->setHasMoved(true); // set has moved
    
    if (move.isProm) { // if move is promotion
        movingPiece = PieceFactory::initPiece(move.pProm, movingPiece->getColor()); // init piece       
    }
    
    squares[move.finalpos.r][move.finalpos.c] = std::move(movingPiece); // set piece
    
    return true; // return true
}

// create move
Move Board::createMove(const Position& from, const Position& to, PieceType promotion) const {
    const Piece* piece = getPiece(from); // get piece
    if (!piece) { // if piece is empty
        Move invalidMove; // invalid move
        invalidMove.init = Position(-1, -1); // invalid move
        return invalidMove; // return invalid move
    }
    
    Move move(from, to, piece->getType()); // move
    
    move.isCap = (getPiece(to) != nullptr); // is capture
    
    if (piece->getType() == PieceType::PAWN) { // if piece is pawn
        int promotionRankCount;
        if (piece->getColor() == Color::WHITE) { // if piece is white
            promotionRankCount = 7; // promotion rank count
        } else { // if piece is black
            promotionRankCount = 0; // promotion rank count
        }
        if (to.r == promotionRankCount) { // if to rank is promotion rank
            move.isProm = true; // is promotion
            move.pProm = promotion; // promotion type
        }
    }
    
    if (piece->getType() == PieceType::KING) { // if piece is king
        if (from.r == to.r && abs(to.c - from.c) == 2) { // if from rank is to rank and to column is from column plus or minus 2
            move.isCast = true; // is castling
        }
    }
    
    if (piece->getType() == PieceType::PAWN) { // if piece is pawn
        int directionCount;
        if (piece->getColor() == Color::WHITE) { // if piece is white
            directionCount = 1; // direction count
        } else { // if piece is black
            directionCount = -1; // direction count
        }
        
        if (abs(to.c - from.c) == 1 && to.r == from.r + directionCount) {
            if (getPiece(to) == nullptr && enPassantTarget.isValid() && to == enPassantTarget) {
                move.isEnp = true; // is en passant
                move.isCap = true; // is capture
            }
        }
    }
    
    return move; // return move
}

// castle
bool Board::castle(Color player, bool kingSide) const {
    Position kingPos = findKing(player); // find king
    if (!kingPos.isValid()) return false; // if king position is invalid
    
    const Piece* king = getPiece(kingPos); // get piece
    if (!king || king->getHasMoved()) return false;
    
    if (isInCheck(player)) return false; // if player is in check
    
        int rookCol; // rook column
        if (kingSide) { // if king side
            rookCol = 7; // rook column
        } else { // if queen side
            rookCol = 0; // rook column
        }
        int kingTargetCol; // king target column
        if (kingSide) { // if king side
            kingTargetCol = 6; // king target column
        } else { // if queen side
            kingTargetCol = 2; // king target column
        }
    
    Position rookPos(kingPos.r, rookCol); // rook position
    const Piece* rook = getPiece(rookPos); // get piece
    
    if (!rook || rook->getType() != PieceType::ROOK || rook->getHasMoved()) { // if rook is empty or rook is not rook or rook has moved
        return false; // return false
    }
    
    int startCol = min(kingPos.c, rookCol); // start column
    int endCol = max(kingPos.c, rookCol); // end column
    
    for (int col = startCol + 1; col < endCol; col++) {
        if (getPiece(Position(kingPos.r, col)) != nullptr) { // if piece is not empty
            return false; // return false
        }
    }
    
    int kingStart = kingPos.c; // king start column
    int kingEnd = kingTargetCol; // king end column
    int directionCount;
    if (kingEnd > kingStart) { // if king end column is greater than king start column 
        directionCount = 1; // direction count
    } else {
        directionCount = -1; // direction count
    }
    
    for (int col = kingStart; col != kingEnd + directionCount; col += directionCount) {
        Position checkPos(kingPos.r, col); // check position
        if (isSquareAttacked(checkPos, oppositeColor(player))) { // if square is attacked by opposite color
            return false; // return false
        }
    }
    
    return true; // return true
}

// enpassant
bool Board::enpassant(const Position& from, const Position& to) const {
    const Piece* piece = getPiece(from); // get piece
    if (!piece || piece->getType() != PieceType::PAWN) { // if piece is empty or piece is not pawn
        return false; // return false
    }
    
    int directionCount;
    if (piece->getColor() == Color::WHITE) { // if piece is white
        directionCount = 1; // direction count
    } else { // if piece is black
        directionCount = -1; // direction count
    }
    
    if (abs(to.c - from.c) != 1 || to.r != from.r + directionCount) { // if to column is not from column plus or minus 1 or to rank is not from rank plus or minus direction count
        return false; // return false
    }
    
    if (getPiece(to) != nullptr) { // if piece is not empty
        return false; // return false
    }
    
    return enPassantTarget.isValid() && to == enPassantTarget; // return en passant target is valid and to is en passant target
}

// update en passant target
void Board::updateEnPassantTarget(const Move& move) {
    enPassantTarget = Position(-1, -1); // en passant target
    
    if (move.pType == PieceType::PAWN) { // if piece is pawn
        int distance = abs(move.finalpos.r - move.init.r); // distance
        
        if (distance == 2) { // if distance is 2
            int targetRank = (move.init.r + move.finalpos.r) / 2; // target rank
            enPassantTarget = Position(targetRank, move.finalpos.c); // en passant target
        }
    }
}

// to FEN
string Board::toFEN() const {
    string fenStr = ""; // FEN string
    
    for (int rankNum = 7; rankNum >= 0; --rankNum) {
        int emptyNum = 0;
        for (int fileNum = 0; fileNum < 8; ++fileNum) {
            const Piece* piece = getPiece(rankNum, fileNum); // get piece
            if (piece) { // if piece is not empty
                if (emptyNum > 0) { // if empty count is greater than 0
                    fenStr += to_string(emptyNum); // add empty count to FEN
                    emptyNum = 0; // empty count
                }
                fenStr += piece->getSymbol(); // add piece symbol to FEN
            } else {
                emptyNum++; // empty count
            }
        }
        
        if (emptyNum > 0) { // if empty count is greater than 0
            fenStr += to_string(emptyNum); // add empty count to FEN
        }
        if (rankNum > 0) { // if rank number is greater than 0
            fenStr += "/"; // add "/" to FEN
        }
    }
    
    // Add current player
    fenStr += " "; // add " " to FEN
    fenStr += (currentPlayer == Color::WHITE) ? "w" : "b"; // add player to FEN
    
    // Add castling availability
    fenStr += " "; // add " " to FEN
    string castleStr = ""; // castling
    
    const Piece* whiteKing = getPiece(0, 4); // get piece
    if (whiteKing && whiteKing->getType() == PieceType::KING && !whiteKing->getHasMoved()) { // if white king is not empty and white king is king and white king has not moved
        const Piece* kingsideRook = getPiece(0, 7); // get piece
        if (kingsideRook && kingsideRook->getType() == PieceType::ROOK && !kingsideRook->getHasMoved()) { // if kingside rook is not empty and kingside rook is rook and kingside rook has not moved
            castleStr += 'K'; // add 'K' to castling
        }
        const Piece* queensideRook = getPiece(0, 0); // get piece
        if (queensideRook && queensideRook->getType() == PieceType::ROOK && !queensideRook->getHasMoved()) { // if queenside rook is not empty and queenside rook is rook and queenside rook has not moved
            castleStr += 'Q'; // add 'Q' to castling
        }
    }
    
    const Piece* blackKing = getPiece(7, 4); // get piece
    if (blackKing && blackKing->getType() == PieceType::KING && !blackKing->getHasMoved()) { // if black king is not empty and black king is king and black king has not moved
        const Piece* rookBlackKingside = getPiece(7, 7); // get piece
        if (rookBlackKingside && rookBlackKingside->getType() == PieceType::ROOK && !rookBlackKingside->getHasMoved()) { // if kingside rook is not empty and kingside rook is rook and kingside rook has not moved
            castleStr += 'k'; // add 'k' to castling
        }
        const Piece* rookBlackQueenside = getPiece(7, 0); // get piece
        if (rookBlackQueenside && rookBlackQueenside->getType() == PieceType::ROOK && !rookBlackQueenside->getHasMoved()) { // if queenside rook is not empty and queenside rook is rook and queenside rook has not moved
            castleStr += 'q'; // add 'q' to castling 
        }
    }
    
    if (castleStr.empty()) { // if  castle string is empty
        castleStr = "-"; // castle string
    }
    fenStr += castleStr; // add castle string to FEN
    fenStr += ' '; // add ' ' to FEN
    
    if (enPassantTarget.isValid()) { // if en passant target is valid
        fenStr += enPassantTarget.toAlgebraic(); // add en passant target to FEN
    } else { // if en passant target is not valid
        fenStr += '-'; // add '-' to FEN
    }
    fenStr += ' '; // add ' ' to FEN   
    
    fenStr += to_string(halfmoveClock); // add halfmove clock to FEN
    
    fenStr += " "; // add " " to FEN
    fenStr += to_string(fullmoveNumber); // add fullmove number to FEN
    
    return fenStr; // return FEN string
}

// load from FEN   
bool Board::loadFromFEN(const string& fen) {
    vector<string> partsVec; // parts vector
    istringstream iss(fen); // input string stream
    string part; // part
    
    while (getline(iss, part, ' ')) { // while getting line
        partsVec.push_back(part); // add part to parts vector
    }
    
    if (partsVec.size() < 6) return false; // if parts size is less than 6
    
    // Clear board
    for (int rowNum = 0; rowNum < 8; ++rowNum) { // clear board
        for (int colNum = 0; colNum < 8; ++colNum) { // clear board
            squares[rowNum][colNum] = nullptr; // clear board
        }
    }
    
    string piecePlacementStr = partsVec[0]; // piece placement string
    int rankNum = 7; // rank number
    int fileNum = 0; // file number
    
    for (char c : piecePlacementStr) {
        if (c == '/') { // if c is '/'
            rankNum--; // rank number
            fileNum = 0; // file number
        } else if (isdigit(c)) {
            fileNum += c - '0' - 1; // advance file number
        } else {
            auto piece = PieceFactory::initPieceFromChar(c); // create piece
            if (piece) { // if piece is not empty
                setPiece(Position(rankNum, fileNum), std::move(piece)); // set piece
            }
            fileNum++; // file number
        }
    }
    
    if (partsVec[1] == "w") { // if parts[1] is "w"
        currentPlayer = Color::WHITE; // current player
    } else if (partsVec[1] == "b") { // if parts[1] is "b"
        currentPlayer = Color::BLACK; // current player
    } else {
        return false; // return false
    }
    
    string castleStr = partsVec[2]; // castle string
    
    bool whiteKingsideCastling = castleStr.find('K') != string::npos; // white kingside castling
    bool whiteQueensideCastling = castleStr.find('Q') != string::npos; // white queenside castling
    bool blackKingsideCastling = castleStr.find('k') != string::npos; // black kingside castling
    bool blackQueensideCastling = castleStr.find('q') != string::npos; // black queenside castling
        
    const Piece* whiteKing = getPiece(0, 4); // get piece
    if (whiteKing && whiteKing->getType() == PieceType::KING) { // if white king is not empty and white king is king
        if (whiteKingsideCastling || whiteQueensideCastling) { // if white kingside castling or white queenside castling
            const_cast<Piece*>(whiteKing)->setHasMoved(false); // set has moved
        }
    }
        
    const Piece* blackKing = getPiece(7, 4); // get piece
    if (blackKing && blackKing->getType() == PieceType::KING) { // if black king is not empty and black king is king
        if (blackKingsideCastling || blackQueensideCastling) { // if black kingside castling or black queenside castling
            const_cast<Piece*>(blackKing)->setHasMoved(false); // set has moved
        }
    }
        
    if (whiteKingsideCastling) { // if white kingside castling
        const Piece* rook = getPiece(0, 7); // get piece
        if (rook && rook->getType() == PieceType::ROOK) { // if rook is not empty and rook is rook
            const_cast<Piece*>(rook)->setHasMoved(false); // set has moved
        }
    }
        
    if (whiteQueensideCastling) { // if white queenside castling
        const Piece* rook = getPiece(0, 0); // get piece
        if (rook && rook->getType() == PieceType::ROOK) { // if rook is not empty and rook is rook
            const_cast<Piece*>(rook)->setHasMoved(false); // set has moved
        }
    }
        
    if (blackKingsideCastling) { // if black kingside castling
        const Piece* rook = getPiece(7, 7); // get piece
        if (rook && rook->getType() == PieceType::ROOK) { // if rook is not empty and rook is rook
            const_cast<Piece*>(rook)->setHasMoved(false); // set has moved
        }
    }
        
    if (blackQueensideCastling) { // if black queenside castling
        const Piece* rook = getPiece(7, 0); // get piece
        if (rook && rook->getType() == PieceType::ROOK) { // if rook is not empty and rook is rook
            const_cast<Piece*>(rook)->setHasMoved(false); // set has moved
        }
    }
    
    if (partsVec[3] != "-") { // if parts[3] is not "-"
        enPassantTarget = Position::fromAlgebraic(partsVec[3]); // en passant position
    } else { // if parts[3] is "-"
        enPassantTarget = Position(-1, -1); // en passant position
    }
    
    try {
        halfmoveClock = stoi(partsVec[4]); // halfmove number
    } catch (const exception&) { // if exception
        halfmoveClock = 0; // halfmove number
    }
    
    try {       
        fullmoveNumber = stoi(partsVec[5]); // fullmove number
    } catch (const exception&) { // if exception
        fullmoveNumber = 1; // fullmove number
    }
    
    gameState = GameState::PLAYING; // game state
    notifyObservers(); // notify observers
    
    return true; // return true
} 

// notify move
void Board::notifyMove(const Move& move) {
    for (auto observer : observers) { // for each observer
        observer->notifyMove(move); // notify move
    }
}

// notify state change
void Board::notifyStateChange(GameState state) {
    for (auto observer : observers) { // for each observer
        observer->notifyStateChange(state); // notify state change
    }
}

// notify no move
void Board::notifyNoMove(const Move& move, const string& errmsg) {
    for (auto observer : observers) { // for each observer
        observer->notifyNoMove(move, errmsg); // notify no move
    }
}

// notify check
void Board::notifyCheck(Color player) {
    for (auto observer : observers) { // for each observer
        observer->notifyCheck(player); // notify check
    }
}

// notify checkmate
void Board::notifyCheckmate(Color loser) {
    for (auto observer : observers) { // for each observer
        observer->notifyCheckmate(loser); // notify checkmate
    }
}

// notify draw
void Board::notifyDraw(const string& errmsg) {
    for (auto observer : observers) { // for each observer
        observer->notifyDraw(errmsg); // notify draw
    }
} 
