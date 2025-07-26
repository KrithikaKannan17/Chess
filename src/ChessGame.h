#ifndef CHESSGAME_H
#define CHESSGAME_H

#include "Board.h"
#include "TextDisplay.h"
#include "GraphicalDisplay.h"
#include "types.h"
#include <memory>
#include <string>
#include <map>
#include <fstream>

using namespace std;

enum class PlayerType { // player type
    HUMAN,
    COMPUTER_LEVEL1,
    COMPUTER_LEVEL2,
    COMPUTER_LEVEL3
};

class Player { // player

protected:
    Color color;
    PlayerType type;
    
public:
    Player(Color c, PlayerType t) : color(c), type(t) {} // constructor
    virtual ~Player() = default; // destructor
    
    Move makeMove(const Board& board) { // make move
        vector<Move> legalMoves = board.getAllLegalMoves(color);
        if (legalMoves.empty()) { // if there are no legal moves
            return Move(); // return empty move
        }
        
        return selectMove(board, legalMoves);
    }
    
    Color getColor() const { return color; } // get color
    PlayerType getType() const { return type; } // get type
    virtual bool isHuman() const { return type == PlayerType::HUMAN; } // is human
    
    virtual Move selectMove(const Board& board, const vector<Move>& legalMoves) = 0;
};

class Human : public Player { // human player
    Move parseInput(const string& input, const vector<Move>& legalMoves); // parse input

protected:
    Move selectMove(const Board& board, const vector<Move>& legalMoves) override;

public:
    Human(Color c) : Player(c, PlayerType::HUMAN) {} // constructor
};

class Computer : public Player { // computer player
    Move Rmove(const vector<Move>& legalMoves); // select random move
    Move R2Move(const Board& board, const vector<Move>& legalMoves); // select level 2 move
    Move R3Move(const Board& board, const vector<Move>& legalMoves); // select level 3 move
    int isSlay(const Board& board, const Move& move); // evaluate move
    bool isCheck(const Board& board, const Move& move); // gives check
    bool noCapture(const Board& board, const Move& move); // avoids capture

protected:
    Move selectMove(const Board& board, const vector<Move>& legalMoves) override; // select move

public:
    Computer(Color c, PlayerType level) : Player(c, level) {} // constructor  
    bool isHuman() const override { return false; }
};

class ChessGame {
    unique_ptr<Board> board; // board
    unique_ptr<Player> white; // white player
    unique_ptr<Player> black; // black player
    unique_ptr<TextDisplay> textDisplay; // text display
    unique_ptr<GraphicalDisplay> graphicalDisplay; // graphical display
    bool gameActive; // game active
    bool Setup; // in setup mode
    bool ifCustomSetup; // has custom setup
    bool useGraphics; // use graphics
    bool ifGreenTiles; // green tiles enabled
    bool ifAssignmentStyle; // assignment style enabled
    map<Color, double> scores; // scores
    
    void initializeGame(); // initialize game
    void gameLoop(); // game loop
    void processPlayerInput(); // process player input
    void updateGameState(); // update game state
    void concludeGame(); // conclude game
    bool isGameOver(); // is game over
    unique_ptr<Player> createPlayer(PlayerType type, Color color); // create player
    PlayerType parsePlayerType(const string& playerInput); // parse player type
    PieceType parsePromotionType(const string& promotionInput);
    void updateScore(); // update score
    vector<string> tokenize(const string& input); // tokenize
    bool isValidPosition(const string& position); // is valid position
    void displayFinalScore(); // display final score
    
public:
    ChessGame(bool graphics = true, bool greenTiles = false, bool assignmentStyle = false); // constructor
    ~ChessGame() = default; // destructor
    
    void run(); // run
    
    bool readInstructions(const string& instruction); // read instructions
    
    void newGame(PlayerType whiteType, PlayerType blackType); // new game
    void runGameLoop(); // run game loop
    void resign(); // resign
    void startSetup(); // start setup
    void leaveSetupMode(); // leave setup mode
    
    bool addPiece(char piece, const string& position); // add piece
    bool delPiece(const string& position); // delete piece
    bool decideTurn(Color color); // decide turn
    
    bool doMove(const string& from, const string& to, 
                    const string& promotion = ""); // do move
    bool doAlgoMove(); // do algo move
    
    bool saveGameToFEN(const string& filename); // save game to FEN
    bool loadGameFromFEN(const string& filename); // load game from FEN
    
    void notifyDisplays(); // notify displays
    void switchDisplayMode(const string& mode); // switch display mode
    void printHelp(); // print help
    void printScore(); // print score
};

#endif 
