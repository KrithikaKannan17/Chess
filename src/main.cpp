#include "ChessGame.h"
#include <iostream>
#include <string>

using namespace std;

void printUsage(const string& Name) { // print usage
    cout << "Usage: " << Name << " [options]\n"; // print usage
    cout << "Options:\n"; // print options
    cout << "  -text        Text-only mode (no graphics)\n"; // print text only mode
    cout << "  -graphics    Enable graphical display (default)\n"; // print graphics
    cout << "  -green       Use green tile theme for graphics\n"; // print green tile theme
    cout << "  -default     Use assignment-style simple text display\n"; // print assignment style
    cout << "  -help        Show this help message\n"; // print help
    cout << "\nCommands during gameplay:\n"; // print commands during gameplay
    cout << "  game white-human black-human    Start new game\n"; // print game
    cout << "    Players: human, computer[1-4]\n"; // print players
    cout << "  move from to [promotion]        Make a move\n"; // print move
    cout << "  resign                          Resign current game\n"; // print resign
    cout << "  setup                           Enter setup mode\n"; // print setup
    cout << "  help                            Show help\n"; // print help
    cout << "  quit                            Exit program\n"; // print quit
    cout << "\nSetup mode commands:\n"; // print setup mode commands
    cout << "  + piece square                  Place piece on square\n"; // print place piece on square
    cout << "  - square                        Remove piece from square\n"; // print remove piece from square
    cout << "  = color                         Set next player to move\n"; // print set next player to move
    cout << "  done                            Exit setup mode\n"; // print done
    cout << "\nExamples:\n"; // print examples
    cout << "  game human computer2            Human vs Level 2 computer\n"; // print human vs level 2 computer
    cout << "  move e2 e4                      Move pawn from e2 to e4\n"; // print move pawn from e2 to e4
    cout << "  move e7 e8 Q                    Promote pawn to queen\n";
    cout << "  + K e1                          Place white king on e1\n";
}

int main(int argc, char* argv[]) {
    bool graphics = true;
    bool greenTiles = false;
    bool assignmentStyle = false;
    
    for (int i = 1; i < argc; ++i) {
        string argument = argv[i];
        if (argument == "-text") {
            graphics = false;
        } else if (argument == "-graphics") {
            graphics = true;
        } else if (argument == "-green") {
            greenTiles = true;
        } else if (argument == "-default") {
            assignmentStyle = true;
        } else if (argument == "-help" || argument == "--help" || argument == "-h") {
            printUsage(argv[0]);
            return 0;
        } else {
            cerr << "Unknown option: " << argument << endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    try {
        cout << "Varnit and Krithika welcome you to VKChess\n";
        cout << "you know the rules already, if you dont, just type 'help'\n";
        
        ChessGame game(graphics, greenTiles, assignmentStyle);
        game.run();
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown error occurred." << endl;
        return 1;
    }
    
    return 0;
} 
