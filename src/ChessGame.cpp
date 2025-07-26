#include "ChessGame.h"
#include <iostream>
#include <sstream>
#include "GraphicalDisplay.h"
#include <cstdlib>
#include <thread>
#include <chrono>
#include <random>
#include <fstream>

using namespace std;

// ChessGame class
const int WINDOW_SIZE = 500;
    
// constructor
ChessGame::ChessGame(bool graphics, bool greenTiles, bool assignmentStyle) : gameActive(false), Setup(false), ifCustomSetup(false), useGraphics(graphics), ifGreenTiles(greenTiles), ifAssignmentStyle(assignmentStyle) {
    board = make_unique<Board>(); // board
    textDisplay = make_unique<TextDisplay>(cout, true, false, assignmentStyle);
    
    if (useGraphics) { // if use graphics
        try { 
            graphicalDisplay = make_unique<GraphicalDisplay>(WINDOW_SIZE, greenTiles); // graphical display

        } catch (const exception& e) {
            cerr << "Failed to initialize graphics: " << e.what() << endl;
            cerr << "Falling back to text-only mode." << endl;
            useGraphics = false; // use graphics
        }
    }
    

    if (textDisplay) { // if text display
        board->attach(textDisplay.get()); // attach text display
    }
    if (useGraphics && graphicalDisplay) { // if use graphics and graphical display
        board->attach(graphicalDisplay.get()); // attach graphical display
    }
    

    scores[Color::WHITE] = 0.0; // scores
    scores[Color::BLACK] = 0.0; // scores
}

void ChessGame::run() { // run
    string command; 
    while (getline(cin, command)) { 
        if (command == "quit" || command == "exit") { // if input is quit or exit
            break; // break
        }
        if (!readInstructions(command)) { // if read instructions
            cout << "Invalid command. Type 'help' for commands." << endl; // invalid command
        }
    }
    
    displayFinalScore(); // display final score
}

bool ChessGame::readInstructions(const string& instruction) { // read instructions
    vector<string> tokens = tokenize(instruction); // tokens
    if (tokens.empty()) return false; // if tokens is empty
    
    string command = tokens[0]; 
    if (command == "help") { // if command is help
        printHelp(); 
        return true; 
    } else if (command == "score") { // if command is score
        printScore(); 
        return true; 
    } else if (command == "game" && tokens.size() == 3) { 
        PlayerType white = parsePlayerType(tokens[1]); // white
        PlayerType black = parsePlayerType(tokens[2]); // black
        newGame(white, black); // new game
        return true; // return true
    } else if (command == "move") { // if command is move
        if (tokens.size() >= 3) {
            string promotion = (tokens.size() > 3) ? tokens[3] : ""; // promotion
            return doMove(tokens[1], tokens[2], promotion); // do move
        }
        return false; // return false
    } else if (command == "resign") {
        resign(); // resign
        return true; // return true
    } else if (command == "setup") { // if command is setup
        startSetup(); // start setup
        return true; // return true
    } else if (command == "default") { // if command is default
        ifCustomSetup = false; // has custom setup
        board->setup(); // setup
        cout << "Board reset to standard starting position." << endl; // board reset to standard starting position
        notifyDisplays(); // notify displays
        return true; // return true
    } else if (command == "theme" && tokens.size() == 2) { // if command is theme and tokens size is 2
        switchDisplayMode(tokens[1]); // switch display mode
        return true;
    } else if (command == "display" && tokens.size() == 2) { // if command is display and tokens size is 2
        switchDisplayMode(tokens[1]); // switch display mode
        return true; // return true
    } 
    else if (command == "save" && tokens.size() == 3) { // if command is save and tokens size is 3
        string formatString = tokens[1]; // format
        string filename = tokens[2]; // filename
        if (formatString == "fen") { // if format is fen
            return saveGameToFEN(filename); // save game to FEN
        } else { // if format is not fen
            cout << "Invalid format. Use 'fen'." << endl; // invalid format
            return false; // return false
        }
    } else if (command == "load" && tokens.size() == 3) { // if command is load and tokens size is 3
        string formatString = tokens[1]; // format
        string filename = tokens[2]; // filename
        if (formatString == "fen") { // if format is fen
            return loadGameFromFEN(filename); // load game from FEN
        } else { // if format is not fen
            cout << "Invalid format. Use 'fen'." << endl; // invalid format
            return false; // return false
        }
    }
    
    if (Setup) { // if in setup mode
        if (command == "done") { // if command is done
            leaveSetupMode(); // leave setup mode
            return true; // return true
        } else if (command == "+" && tokens.size() == 3) { // if command is + and tokens size is 3
            bool result = addPiece(tokens[1][0], tokens[2]); // add piece
            if (result) notifyDisplays();
            return result;
        } else if (command == "-" && tokens.size() == 2) { // if command is - and tokens size is 2
            bool result = delPiece(tokens[1]); // delete piece
            if (result) notifyDisplays();
            return result;
        } else if (command == "=" && tokens.size() == 2) {      
            Color color; // color
            if (tokens[1] == "white") { // if tokens[1] is white
                color = Color::WHITE; // color
            } else { // if tokens[1] is not white
                color = Color::BLACK; // color
            }
            bool result = decideTurn(color); // decide turn
            if (result) notifyDisplays();
            return result;
        }
    }
    
    return false;
}

void ChessGame::newGame(PlayerType whiteType, PlayerType blackType) {
    if (whiteType == PlayerType::HUMAN) { // if white is human
        white = make_unique<Human>(Color::WHITE); // white player
    } else { // if white is not human
        white = make_unique<Computer>(Color::WHITE, whiteType); // white player
    }
    
    if (blackType == PlayerType::HUMAN) { // if black is human
        black = make_unique<Human>(Color::BLACK); // black player
    } else { // if black is not human
        black = make_unique<Computer>(Color::BLACK, blackType); // black player
    }
    
    if (ifCustomSetup) { // if has custom setup
        cout << "Starting game with custom setup." << endl; // starting game with custom setup
    } else { // if has not custom setup
        board->setup(); // setup
    }
    
    gameActive = true; // game active
    
    notifyDisplays(); // notify displays
    runGameLoop(); // run game loop
}

void ChessGame::runGameLoop() { // run game loop
    while (gameActive) { // while game active
        Color currentPlayer = board->getCurrentPlayer(); // current player color
        Player* currentPlayerPtr; // current player
        if (currentPlayer == Color::WHITE) { // if current player color is white
            currentPlayerPtr = white.get(); // current player
        } else {
            currentPlayerPtr = black.get(); // current player
        } // current player
        
        vector<Move> legalMoves = board->getAllLegalMoves(currentPlayer); // legal moves
        
        if (legalMoves.empty()) { // if legal moves is empty
            if (board->isInCheck(currentPlayer)) { // if current player color is in check
                Color WinnerColor = oppositeColor(currentPlayer); // winner
                cout << "\nCheckmate! " << colorToString(WinnerColor) << " wins!" << endl; // checkmate
                scores[WinnerColor] += 1.0; // scores
            } else { // if current player color is not in check
                cout << "\nStalemate! The game is a draw." << endl; // stalemate
                scores[Color::WHITE] += 0.5; // scores
                scores[Color::BLACK] += 0.5; // scores
            }
            gameActive = false; // game active
            cout << "\nGAME OVER" << endl; // game over
            break; // break
        }
        
        if (board->isDraw()) { // if board is draw
            cout << "\nDraw!" << endl; // draw
            scores[Color::WHITE] += 0.5; // scores
            scores[Color::BLACK] += 0.5; // scores
            gameActive = false; // game active
            cout << "\nGAME OVER" << endl; // game over
            break; // break
        }
        
        Move chosenMove = currentPlayerPtr->makeMove(*board); // selected move
        
        if (!chosenMove.init.isValid()) { // if selected move is not valid
            Color WinnerColor = oppositeColor(currentPlayer); 
            cout << "\n" << colorToString(currentPlayer) << " resigns! " 
                      << colorToString(WinnerColor) << " wins!" << endl; 
            scores[WinnerColor] += 1.0; 
            gameActive = false; 
            cout << "\nGAME OVER" << endl; 
            break; 
        }
        
        if (board->move(chosenMove)) {
            notifyDisplays(); // notify displays
            
            GameState currentState = board->getGameState(); // game state
            if (currentState == GameState::CHECKMATE_WHITE_WINS || 
                currentState == GameState::CHECKMATE_BLACK_WINS ||
                currentState == GameState::DRAW_STALEMATE ||
                currentState == GameState::DRAW_INSUFFICIENT_MATERIAL ||
                currentState == GameState::DRAW_50_MOVE_RULE ||
                currentState == GameState::DRAW_REPETITION) {
                
                if (currentState == GameState::CHECKMATE_WHITE_WINS) {
                    scores[Color::WHITE] += 1.0; // scores
                    cout << "\nCheckmate! White wins!" << endl; // checkmate
                } else if (currentState == GameState::CHECKMATE_BLACK_WINS) {
                    scores[Color::BLACK] += 1.0; // scores
                    cout << "\nCheckmate! Black wins!" << endl; // checkmate
                } else if (currentState == GameState::DRAW_STALEMATE) {
                    scores[Color::WHITE] += 0.5; // scores
                    scores[Color::BLACK] += 0.5; // scores
                    cout << "\nStalemate! The game is a draw." << endl; // stalemate
                } else {
                    scores[Color::WHITE] += 0.5; // scores
                    scores[Color::BLACK] += 0.5; // scores
                    cout << "\nDraw!" << endl; // draw
                }
                
                gameActive = false; // game active
                cout << "\nGAME OVER" << endl; // game over
                break; // break
            }
        } else {
            cout << "Move execution failed. Please try again." << endl; // move execution failed
        }
    }
    
    cout << "Type 'game human human' to start a new game." << endl; // type 'game human human' to start a new game
}

void ChessGame::resign() {
    if (!gameActive) {
        cout << "No active game." << endl; // no active game
        return; // return
    }
    
                Color resPlayer = board->getCurrentPlayer(); // resigning player
    Color WinnerColor = oppositeColor(resPlayer); // winner
    
    cout << colorToString(resPlayer) << " resigns. " << colorToString(WinnerColor) << " wins!" << endl; // resigns
    
    scores[WinnerColor] += 1.0; // scores
    gameActive = false; // game active
}

void ChessGame::startSetup() {
    if (gameActive) { // if game active
        cout << "Cannot enter setup mode during an active game." << endl; // cannot enter setup mode during an active game
        return; // return
    }
    
                Setup = true; // in setup mode
    ifCustomSetup = false; // has custom setup
    board->clear(); // clear
    cout << "Entering setup mode. Use '+', '-', '=' commands, then 'done'." << endl; // entering setup mode. Use '+', '-', '=' commands, then 'done'.
}

void ChessGame::leaveSetupMode() {
    if (!board->isValidSetup()) { // if board is not valid setup
        cout << "Invalid setup. Must have exactly one king per side, no pawns on back ranks, and no kings in check." << endl; // invalid setup
        return; // return
    }
    
    Setup = false; // in setup mode
    ifCustomSetup = true; // has custom setup
    notifyDisplays(); // show final board state
    cout << "Setup complete." << endl; // setup complete
}

bool ChessGame::addPiece(char piece, const string& position) {
    Position pos = Position::fromAlgebraic(position); // position
    if (!pos.isValid()) return false; // if position is not valid
    
    auto piecePtr = PieceFactory::initPieceFromChar(piece); // piece pointer
    if (!piecePtr) return false; // if piece pointer is not valid
    
    board->setPiece(pos, std::move(piecePtr)); // set piece
    return true; // return true
}

bool ChessGame::delPiece(const string& position) {
    Position pos = Position::fromAlgebraic(position); // position
    if (!pos.isValid()) return false; // if position is not valid
    
    board->removePiece(pos); // remove piece
    return true; // return true
}

bool ChessGame::decideTurn(Color color) {
    board->setCurrentPlayer(color); // set current player
    cout << "Turn set to " << colorToString(color) << endl; // turn set to
    return true; // return true
}

bool ChessGame::doMove(const string& from, const string& to, const string& promotion) {
    if (!gameActive) { // if game active
        cout << "No active game. Start a new game first." << endl; // no active game
        return false; // return false
    }
    
    PieceType promotionPiece = parsePromotionType(promotion); // promotion piece
    bool success = board->move(from, to, promotionPiece); // success
    
    if (success) { // if success
        const auto& currMoveHistory = board->getMoveHistory(); // move history
        if (!currMoveHistory.empty()) { // if move history is not empty
        }
        
        notifyDisplays(); // notify displays
        
        GameState currentState = board->getGameState(); // game state
        if (currentState == GameState::CHECKMATE_WHITE_WINS || 
            currentState == GameState::CHECKMATE_BLACK_WINS ||
            currentState == GameState::DRAW_STALEMATE ||
            currentState == GameState::DRAW_INSUFFICIENT_MATERIAL ||
            currentState == GameState::DRAW_50_MOVE_RULE ||
            currentState == GameState::DRAW_REPETITION) {
            
            if (currentState == GameState::CHECKMATE_WHITE_WINS) {
                scores[Color::WHITE] += 1.0; // scores
            } else if (currentState == GameState::CHECKMATE_BLACK_WINS) {
                scores[Color::BLACK] += 1.0; // scores
            } else {
                scores[Color::WHITE] += 0.5; // scores
                scores[Color::BLACK] += 0.5; // scores
            }
            
            gameActive = false; // game active
            cout << "\nGAME OVER" << endl; // game over
            cout << "Type 'game human human' to start a new game." << endl; // type 'game human human' to start a new game
        }
    }
    
    return success; // return success
}

bool ChessGame::doAlgoMove() {
    // Placeholder for algorithm-based move generation
    // Currently returns false indicating no algorithmic move was made
    return false; // return false
}

void ChessGame::notifyDisplays() {
    cout << "\033[2J\033[H"; // clear screen
    
    if (textDisplay) { // if text display
        textDisplay->displayBoard(*board);
    }
    
    if (useGraphics && graphicalDisplay) {
        graphicalDisplay->renderBoard(*board); // render board
    }
}

void ChessGame::switchDisplayMode(const string& mode) {
    if (mode == "brown") {
        ifGreenTiles = false; // green tiles enabled
        if (useGraphics && graphicalDisplay) { // if use graphics and graphical display
            board->detach(graphicalDisplay.get()); // detach graphical display
            graphicalDisplay = make_unique<GraphicalDisplay>(WINDOW_SIZE, false); // graphical display
            board->attach(graphicalDisplay.get()); // attach graphical display
            notifyDisplays(); // notify displays
        }
        cout << "Switched to brown tile theme" << endl; // switched to brown tile theme
    } else if (mode == "green") { // if mode is green
        ifGreenTiles = true; // green tiles enabled
        if (useGraphics && graphicalDisplay) { // if use graphics and graphical display
            board->detach(graphicalDisplay.get());
            graphicalDisplay = make_unique<GraphicalDisplay>(WINDOW_SIZE, true); // graphical display
            board->attach(graphicalDisplay.get()); // attach graphical display
            notifyDisplays(); // notify displays
        }
        cout << "Switched to green tile theme" << endl;
    } else if (mode == "standard") { // if mode is standard
        ifAssignmentStyle = false; // assignment style enabled
        if (textDisplay) { // if text display
            board->detach(textDisplay.get()); // detach text display
            textDisplay = make_unique<TextDisplay>(cout, true, false, false); // text display
            board->attach(textDisplay.get()); // attach text display
            notifyDisplays(); // notify displays
        }
        cout << "Switched to standard text display" << endl; // switched to standard text display
    } else if (mode == "assignment") { // if mode is assignment
        ifAssignmentStyle = true; // assignment style enabled
        if (textDisplay) {
            board->detach(textDisplay.get()); // detach text display
            textDisplay = make_unique<TextDisplay>(cout, true, false, true); // text display
            board->attach(textDisplay.get()); // attach text display
            notifyDisplays(); // notify displays
        }
        cout << "Switched to assignment text display" << endl; // switched to assignment text display
    } else {
        cout << "Unknown display mode: " << mode << endl; // unknown display mode
    }
}

void ChessGame::printHelp() {
    cout << "Available commands:\n"; // available commands
    cout << "  game <white> <black>\n"; // game <white> <black>
    cout << "  move <initial> <final>\n"; // move <initial> <final>
    cout << "  score\n"; // score
    cout << "  resign\n"; // resign
    cout << "  setup\n"; // setup
    cout << "  default\n"; // default
    cout << "  help\n"; // help
    cout << "  quit\n"; // quit
    cout << "\nFile Operations:\n"; // file operations
    cout << "  save fen <filename>\n"; // save fen <filename>
    cout << "  load fen <filename>\n"; // load fen <filename>
    
    cout << "\nDisplay Options:\n"; // display options
    cout << "  theme brown\n"; // theme brown
    cout << "  theme green\n"; // theme green
    cout << "  display standard\n"; // display standard
    cout << "  display assignment\n"; // display assignment

    cout << "\nSetup mode commands:\n"; // setup mode commands
    cout << "  + <piece> <square>\n"; // + <piece> <square>
    cout << "  - <square>\n"; // - <square>
    cout << "  = <color>\n"; // = <color>
    cout << "  done\n"; // done
    cout << "\nComputer Levels:\n"; // computer levels
    cout << "  computer1\n";
    cout << "  computer2\n";
    cout << "  computer3\n";
}

void ChessGame::printScore() { // print score
    cout << "Current Score:" << endl; // current score
    cout << "White: " << scores[Color::WHITE] << endl; // white
    cout << "Black: " << scores[Color::BLACK] << endl; // black
}

void ChessGame::displayFinalScore() { // display final score
    cout << "Final Score:" << endl; // final score
    cout << "White: " << scores[Color::WHITE] << endl; // white
    cout << "Black: " << scores[Color::BLACK] << endl; // black
}

bool ChessGame::isGameOver() { // is game over
    return !gameActive;
}

bool ChessGame::saveGameToFEN(const string& filename) {
    ofstream inputFile(filename); // file
    if (!inputFile.is_open()) { // if file is not open
        cout << "Error: Could not open file " << filename << " for writing." << endl; // error
        return false; // return false
    }
    
    inputFile << board->toFEN() << endl; // to FEN
    inputFile.close(); // close file
    
    cout << "Position saved to " << filename << endl; // position saved to
    return true; // return true
}

bool ChessGame::loadGameFromFEN(const string& filename) {
    ifstream inputFile(filename); // file
    if (!inputFile.is_open()) { // if file is not open
        cout << "Error: Could not open file " << filename << " for reading." << endl; // error
        return false; // return false
    }
    
    string fenString; // fen string
    if (!getline(inputFile, fenString)) { // if fen string is not valid
        cout << "Error: Could not read FEN string from file." << endl; // error
        inputFile.close(); // close file
        return false; // return false
    }
    
    if (!board->loadFromFEN(fenString)) { // if board is not valid
        cout << "Error: Invalid FEN string in file." << endl; // error
        inputFile.close(); // close file
        return false; // return false
    }
    
    inputFile.close(); // close file
    notifyDisplays(); // notify displays
    cout << "Position loaded from " << filename << endl; // position loaded from
    return true; // return true
}

unique_ptr<Player> ChessGame::createPlayer(PlayerType type, Color color) { // create player
    if (type == PlayerType::HUMAN) { // if type is human
        return make_unique<Human>(color); // human player
    } else {
        return make_unique<Computer>(color, type); // computer player
    }
}

PlayerType ChessGame::parsePlayerType(const string& playerInput) { // parse player type
    if (playerInput == "human") return PlayerType::HUMAN; // human
    if (playerInput == "computer1") return PlayerType::COMPUTER_LEVEL1; // computer1
    if (playerInput == "computer2") return PlayerType::COMPUTER_LEVEL2; // computer2
    if (playerInput == "computer3") return PlayerType::COMPUTER_LEVEL3; // computer3
    return PlayerType::HUMAN; // human
}

PieceType ChessGame::parsePromotionType(const string& promotionInput) {
    if (promotionInput == "Q") return PieceType::QUEEN; // queen
    if (promotionInput == "R") return PieceType::ROOK; // rook
    if (promotionInput == "B") return PieceType::BISHOP; // bishop
    if (promotionInput == "N") return PieceType::KNIGHT;
    return PieceType::QUEEN;
}

vector<string> ChessGame::tokenize(const string& input) { // tokenize                
    vector<string> inputTokens; // tokens
    istringstream iss(input); // input string stream
    string token; // token
    while (iss >> token) { // while token is not empty
        inputTokens.push_back(token); // add token to tokens
    }
    return inputTokens; // return tokens
}

bool ChessGame::isValidPosition(const string& position) { // is valid position
    return position.length() == 2 && position[0] >= 'a' && position[0] <= 'h' && 
           position[1] >= '1' && position[1] <= '8'; // return true if position is valid
}

Move Human::selectMove(const Board& board, const vector<Move>& legalMoves) { // select move
    string inputString; // input
    
    while (true) { // while true
        cout << "Enter Input or press help: "; // enter input or press help
        
        if (!getline(cin, inputString)) { // if input is not valid
            return Move(); // return move
        }
        
        if (inputString == "resign") { // if input is resign
            return Move(); // return move
        }
        
        vector<string> inputTokens; // tokens
        istringstream iss(inputString); // input string stream
        string token; // token
        while (iss >> token) { // while token is not empty
            inputTokens.push_back(token); // add token to tokens
        }
        
        if (inputTokens.size() >= 3 && inputTokens[0] == "move") { // if tokens size is greater than or equal to 3 and tokens[0] is move
            Position from = Position::fromAlgebraic(inputTokens[1]); // from
            Position to = Position::fromAlgebraic(inputTokens[2]); // to
            
            if (from.isValid() && to.isValid()) { // if from is valid and to is valid
                for (const Move& move : legalMoves) { // for each move in legal moves
                    if (move.init == from && move.finalpos == to) { // if move init is from and move finalpos is to
                        if (move.isProm) { // if move is promotion
                            PieceType promotionPiece = PieceType::QUEEN; // promotion piece
                            
                            if (inputTokens.size() >= 4) { // if tokens size is greater than or equal to 4
                                if (inputTokens[3] == "Q") promotionPiece = PieceType::QUEEN; // queen
                                else if (inputTokens[3] == "R") promotionPiece = PieceType::ROOK; // rook
                                else if (inputTokens[3] == "B") promotionPiece = PieceType::BISHOP; // bishop
                                else if (inputTokens[3] == "N") promotionPiece = PieceType::KNIGHT; // knight
                            } else { // if tokens size is less than 4
                                string promotionInput; // promotion input
                                while (true) {
                                    cout << "Pawn promotion! Choose piece (Q=Queen, R=Rook, B=Bishop, N=Knight): "; // pawn promotion! Choose piece (Q=Queen, R=Rook, B=Bishop, N=Knight):
                                    if (!getline(cin, promotionInput)) { // if promotion input is not valid
                                        return Move(); // return move
                                    }
                                    
                                    if (promotionInput == "Q" || promotionInput == "q") { // if promotion input is queen
                                        promotionPiece = PieceType::QUEEN; // promotion piece
                                        break; // break
                                    } else if (promotionInput == "R" || promotionInput == "r") { // if promotion input is rook
                                        promotionPiece = PieceType::ROOK; // promotion piece
                                        break; // break
                                    } else if (promotionInput == "B" || promotionInput == "b") { // if promotion input is bishop
                                        promotionPiece = PieceType::BISHOP; // promotion piece
                                        break; // break
                                    } else if (promotionInput == "N" || promotionInput == "n") { // if promotion input is knight
                                        promotionPiece = PieceType::KNIGHT; // promotion piece
                                        break; // break
                                    } else { // if promotion input is not valid
                                        cout << "Invalid choice. Please enter Q, R, B, or N." << endl; // invalid choice. Please enter Q, R, B, or N.
                                    }
                                }
                            }
                            
                            for (const Move& promMove : legalMoves) { // for each move in legal moves
                                if (promMove.init == from && promMove.finalpos == to && 
                                    promMove.isProm && promMove.pProm == promotionPiece) { // if move init is from and move finalpos is to and move is promotion and move promotion piece is promotion piece
                                    return promMove; // return move
                                }
                            }
                        }
                        return move; // return move
                    }
                }
                cout << "Invalid move. Please try a legal move." << endl; // invalid move. Please try a legal move.
            } else {
                cout << "Invalid position format. Use algebraic notation (e.g., move e2 e4)." << endl; // invalid position format. Use algebraic notation (e.g., move e2 e4).
            }
        } else {
            cout << "invalid" << endl; // invalid
        }
    }
}

Move Human::parseInput(const string& input, const vector<Move>& legalMoves) { // parse input
    return Move(); // return move
}

    Move Computer::selectMove(const Board& board, const vector<Move>& legalMoves) { // select move
    if (type == PlayerType::COMPUTER_LEVEL1) { // if type is computer level 1
        return Rmove(legalMoves); // return select random move
    } else if (type == PlayerType::COMPUTER_LEVEL2) { // if type is computer level 2
        return R2Move(board, legalMoves); // return select level 2 move
    } else if (type == PlayerType::COMPUTER_LEVEL3) { // if type is computer level 3
        return R3Move(board, legalMoves); // return select level 3 move
    } else { // if type is not computer level 1, 2, or 3
        return Rmove(legalMoves); // return select random move
    }
}

Move Computer::Rmove(const vector<Move>& legalMoves) { // select random move
    if (legalMoves.empty()) return Move(); // if legal moves is empty
    
    this_thread::sleep_for(chrono::milliseconds(WINDOW_SIZE)); // sleep for WINDOW_SIZE milliseconds
    
    random_device device; // random device
    mt19937 gen(device()); // mt19937
    uniform_int_distribution<> dis(0, legalMoves.size() - 1); // uniform int distribution
    
    Move chosenMove = legalMoves[dis(gen)]; // selected move
    
    cout << "\n" << colorToString(color) << " (Computer Level 1) plays: " 
              << chosenMove.init.toAlgebraic() << " " << chosenMove.finalpos.toAlgebraic() << endl; // color to string (color) (Computer Level 1) plays: selected move init to algebraic and selected move finalpos to algebraic
    
    return chosenMove; // return selected move
}

Move Computer::R2Move(const Board& board, const vector<Move>& legalMoves) { // select level 2 move
    if (legalMoves.empty()) return Move(); // if legal moves is empty
    
    this_thread::sleep_for(chrono::milliseconds(750)); // sleep for 750 milliseconds
    
    vector<Move> captureMoves; // capture moves
    vector<Move> checkMoves; // check moves
    vector<Move> regularMoves; // regular moves
    
    for (const Move& move : legalMoves) { // for each move in legal moves
        if (move.isCap) { // if move is capture
            captureMoves.push_back(move); // add move to capture moves
        } else if (isCheck(board, move)) { // if move gives check
            checkMoves.push_back(move); // add move to check moves
        } else {
            regularMoves.push_back(move); // add move to regular moves
        }
    }
    
    Move chosenMove; // selected move
    string strategy; // strategy
    if (!captureMoves.empty()) { // if capture moves is not empty
        random_device device; // random device
        mt19937 gen(device()); // mt19937
        uniform_int_distribution<> dis(0, captureMoves.size() - 1); // uniform int distribution
        chosenMove = captureMoves[dis(gen)]; // selected move
        strategy = "capture"; // strategy
    } else if (!checkMoves.empty()) { // if check moves is not empty
        random_device device; // random device
        mt19937 gen(device()); // mt19937
        uniform_int_distribution<> dis(0, checkMoves.size() - 1); // uniform int distribution
        chosenMove = checkMoves[dis(gen)]; // selected move
        strategy = "check"; // strategy
    } else {
        random_device device; // random device
        mt19937 gen(device()); // mt19937
        uniform_int_distribution<> dis(0, regularMoves.size() - 1); // uniform int distribution
        chosenMove = regularMoves[dis(gen)]; // selected move
        strategy = "development"; // strategy
    }
    
    cout << "\n" << colorToString(color) << " (Computer Level 2) plays: " 
              << chosenMove.init.toAlgebraic() << " " << chosenMove.finalpos.toAlgebraic() 
              << " (" << strategy << ")" << endl; // color to string (color) (Computer Level 2) plays: selected move init to algebraic and selected move finalpos to algebraic and strategy
    
    return chosenMove; // return selected move
}

Move Computer::R3Move(const Board& board, const vector<Move>& legalMoves) { // select level 3 move
    if (legalMoves.empty()) return Move(); // if legal moves is empty
    
    this_thread::sleep_for(chrono::milliseconds(1000)); // sleep for 1000 milliseconds
    
    vector<Move> safeCaptureMoves; // safe capture moves
    vector<Move> safeCheckMoves; // safe check moves
    vector<Move> safeMoveMoves; // safe move moves
    vector<Move> riskyMoveMoves; // risky move moves
    
    for (const Move& move : legalMoves) { // for each move in legal moves
        bool isSafe = !noCapture(board, move); // is safe
        
        if (move.isCap && isSafe) { // if move is capture and is safe
            safeCaptureMoves.push_back(move); // add move to safe capture moves
        } else if (isCheck(board, move) && isSafe) { // if move gives check and is safe
            safeCheckMoves.push_back(move); // add move to safe check moves
        } else if (isSafe) { // if is safe
            safeMoveMoves.push_back(move); // add move to safe move moves
        } else {
            riskyMoveMoves.push_back(move); // add move to risky move moves
        }
    }
    
    Move chosenMove; // selected move
    string strategy; // strategy
    if (!safeCaptureMoves.empty()) { // if safe capture moves is not empty
        random_device device; // random device
        mt19937 gen(device()); // mt19937
        uniform_int_distribution<> dis(0, safeCaptureMoves.size() - 1); // uniform int distribution
        chosenMove = safeCaptureMoves[dis(gen)]; // selected move
        strategy = "safe capture"; // strategy
    } else if (!safeCheckMoves.empty()) { // if safe check moves is not empty
        random_device device; // random device
        mt19937 gen(device()); // mt19937
        uniform_int_distribution<> dis(0, safeCheckMoves.size() - 1); // uniform int distribution
        chosenMove = safeCheckMoves[dis(gen)]; // selected move
        strategy = "safe check"; // strategy
    } else if (!safeMoveMoves.empty()) { // if safe move moves is not empty
        random_device device; // random device
        mt19937 gen(device()); // mt19937
        uniform_int_distribution<> dis(0, safeMoveMoves.size() - 1); // uniform int distribution
        chosenMove = safeMoveMoves[dis(gen)]; // selected move
        strategy = "safe move"; // strategy
    } else {
        random_device device; // random device
        mt19937 gen(device()); // mt19937
        uniform_int_distribution<> dis(0, riskyMoveMoves.size() - 1); // uniform int distribution
        chosenMove = riskyMoveMoves[dis(gen)]; // selected move
        strategy = "forced move"; // strategy
    }
    
    cout << "\n" << colorToString(color) << " (Computer Level 3) plays: " 
              << chosenMove.init.toAlgebraic() << " " << chosenMove.finalpos.toAlgebraic() 
              << " (" << strategy << ")" << endl; // color to string (color) (Computer Level 3) plays: selected move init to algebraic and selected move finalpos to algebraic and strategy
    return chosenMove; // return selected move
}

int Computer::isSlay(const Board& board, const Move& move) { // evaluate move
    int currentScore = 0; // current score
    
    if (move.isCap) { // if move is capture
        const Piece* capturedPiece = board.getPiece(move.finalpos); // captured piece
        if (capturedPiece) { // if captured piece is not empty
            currentScore += capturedPiece->getValue() / 10; // add captured piece value to score
        }
    }
    
    if (isCheck(board, move)) { // if move gives check
        currentScore += 30; // add 30 to score
    }
    
    if (noCapture(board, move)) { // if move avoids capture
        const Piece* movingPiece = board.getPiece(move.init); // moving piece
        if (movingPiece) { // if moving piece is not empty
            currentScore -= movingPiece->getValue() / 20; // subtract moving piece value from score
        }
    }
    
    int bonusCenter = 0; // bonus center
    if ((move.finalpos.r >= 3 && move.finalpos.r <= 4) && (move.finalpos.c >= 3 && move.finalpos.c <= 4)) { // if move finalpos is in center
        bonusCenter = 20; // add 20 to center bonus
    } else if ((move.finalpos.r >= 2 && move.finalpos.r <= 5) && (move.finalpos.c >= 2 && move.finalpos.c <= 5)) {
        bonusCenter = 10; // add 10 to center bonus
    }
    currentScore += bonusCenter; // add center bonus to score
    
    if (move.pType == PieceType::KING && board.getMoveHistory().size() < 10) { // if move type is king and move history size is less than 10
        currentScore -= 15; // subtract 15 from score
    }
    
    if ((move.pType == PieceType::KNIGHT || move.pType == PieceType::BISHOP) && 
        board.getMoveHistory().size() < 16) { // if move type is knight or bishop and move history size is less than 16
        currentScore += 15; // add 15 to score
    }
    
    return currentScore; // return score
}

bool Computer::isCheck(const Board& board, const Move& move) { // gives check
    auto copyBoard = board.createCopy(); // board copy
    if (copyBoard->applyMoveToBoard(move)) { // if move is applied to board
        Color opponent = oppositeColor(color); // opponent color
        return copyBoard->isInCheck(opponent); // return true if opponent is in check
    }
    return false; // return false
}

bool Computer::noCapture(const Board& board, const Move& move) { // avoids capture
    auto copyBoard = board.createCopy(); // board copy
    if (copyBoard->applyMoveToBoard(move)) { // if move is applied to board
        Color opponent = oppositeColor(color); // opponent color
        return copyBoard->isSquareAttacked(move.finalpos, opponent); // return true if opponent is square attacked
    }
    return false;
} 
