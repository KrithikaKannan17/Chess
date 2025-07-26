# Chess Game

A feature-rich chess game implementation in C++ with both text and graphical interfaces.

## Features

- **Dual Display Modes**: Text-based and graphical (X11) interfaces
- **Multiple Player Types**: 
  - Human players
  - Computer players with 4 difficulty levels
- **Game Modes**:
  - Standard chess gameplay
  - Setup mode for custom positions
  - FEN notation support
- **Display Options**:
  - Green tile theme for graphics
  - Assignment-style simple text display
  - Standard graphical board
- **Complete Chess Rules Implementation**:
  - All piece movements (King, Queen, Rook, Bishop, Knight, Pawn)
  - Special moves (castling, en passant, pawn promotion)
  - Check and checkmate detection
  - Stalemate detection

## Requirements

- **Compiler**: g++ with C++20 support (g++-14 or later)
- **Graphics**: X11 development libraries (for graphical mode)
- **Operating System**: Linux/Unix systems with X11

### Installing Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install g++ libx11-dev
```

On macOS (with Homebrew and XQuartz):
```bash
brew install gcc
# Install XQuartz from https://www.xquartz.org/
```

## Building

Use the provided Makefile to build the project:

```bash
make
```

This will create an executable named `chess`.

To clean build artifacts:
```bash
make clean
```

## Usage

### Command Line Options

```bash
./chess [options]
```

**Available Options:**
- `-text`: Text-only mode (no graphics)
- `-graphics`: Enable graphical display (default)
- `-green`: Use green tile theme for graphics
- `-default`: Use assignment-style simple text display
- `-help`: Show help message

### Game Commands

Once the program is running, use these commands:

#### Starting a Game
```
game white-player black-player
```
**Player Types:**
- `human`: Human player
- `computer1`: Computer level 1 (easiest)
- `computer2`: Computer level 2
- `computer3`: Computer level 3
- `computer4`: Computer level 4 (hardest)

**Examples:**
```
game human computer2        # Human vs Level 2 computer
game human human           # Two human players
game computer1 computer3   # Computer vs computer
```

#### Making Moves
```
move from to [promotion]
```
**Examples:**
```
move e2 e4          # Move pawn from e2 to e4
move e7 e8 Q        # Promote pawn to queen
move e1 g1          # Castle kingside (if legal)
```

#### Other Commands
- `resign`: Resign the current game
- `setup`: Enter setup mode
- `help`: Show help during gameplay
- `quit`: Exit the program

### Setup Mode

Enter setup mode to create custom board positions:

```
setup
```

**Setup Commands:**
- `+ piece square`: Place piece on square (e.g., `+ K e1` for white king on e1)
- `- square`: Remove piece from square (e.g., `- e4`)
- `= color`: Set next player to move (`= white` or `= black`)
- `done`: Exit setup mode

**Piece Notation:**
- `K/k`: King (white/black)
- `Q/q`: Queen (white/black)
- `R/r`: Rook (white/black)
- `B/b`: Bishop (white/black)
- `N/n`: Knight (white/black)
- `P/p`: Pawn (white/black)

## File Structure

```
src/
├── main.cpp              # Program entry point
├── ChessGame.cpp/h       # Main game logic and player management
├── Board.cpp/h           # Chess board representation and game rules
├── Piece.cpp/h           # Base piece class
├── PieceFactory.cpp/h    # Factory for creating pieces
├── TextDisplay.cpp/h     # Text-based display
├── GraphicalDisplay.cpp/h # X11 graphical display
├── window.cpp/h          # X11 window management
├── types.h               # Common type definitions
└── pieces/               # Individual piece implementations
    ├── King.cpp/h
    ├── Queen.cpp/h
    ├── Rook.cpp/h
    ├── Bishop.cpp/h
    ├── Knight.cpp/h
    └── Pawn.cpp/h
```

## Examples

### Starting a Human vs Computer Game
```bash
./chess
game human computer2
move e2 e4
# Computer makes its move automatically
move d7 d5
```

### Text-Only Mode
```bash
./chess -text
game human human
move e2 e4
move e7 e5
```

### Custom Setup
```bash
./chess
setup
+ K e1
+ k e8
+ R a1
+ r a8
= white
done
game human computer1
```

## Development

The project uses modern C++20 features and follows object-oriented design principles:

- **Polymorphism**: Base `Piece` class with derived piece types
- **Factory Pattern**: `PieceFactory` for piece creation
- **Observer Pattern**: Display classes observe board state
- **RAII**: Proper resource management with smart pointers

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Troubleshooting

### Graphics Not Working
- Ensure X11 libraries are installed
- On macOS, make sure XQuartz is running
- Try text mode with `-text` flag

### Compilation Errors
- Verify g++ version supports C++20
- Install X11 development headers
- Check that all source files are present

### Game Rules Questions
- The game implements standard FIDE chess rules
- All special moves (castling, en passant, promotion) are supported
- Use `help` command during gameplay for quick reference 
