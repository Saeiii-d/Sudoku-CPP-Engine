# 🧩 Cross-Platform Modern Sudoku C++ Engine

A terminal-based Sudoku game written in C++ with object-oriented components, cross-platform terminal input handling, puzzle generation, unique-solution validation, save/load support, player statistics, hints, undo, and a non-blocking game loop.

---

## ✨ Key Features

- **Non-Blocking Timer and Input Loop:** The game periodically updates the timer while polling keyboard input without requiring the player to press Enter.
- **Solution-Based Move Validation:** Player moves are validated against the generated solution, and the puzzle generator checks that the puzzle has a unique solution.
- **Object-Oriented Architecture:** Organizes the application into focused classes such as `GameEngine`, `SudokuBoard`, `UserManager`, and `Player`, separating board logic, player management, and game orchestration.- **Object-Oriented Architecture:** Refactored into highly cohesive classes (`GameEngine`, `SudokuBoard`, `UserManager`, and `Player`) separating data logic from the rendering pipeline.
- **Local Player Profiles:** Supports registration and login for multiple players, keeping track of scores, win/loss history, and hints used.
- **Local Credential Obfuscation:** Passwords are transformed with a simple reversible byte operation and encoded as hexadecimal text before local storage. Legacy readable records are migrated when possible. This is intended only for a student project and is not production-grade password security.
- **Undo History Stack:** Uses `std::stack` to restore the most recent accepted move while also reversing its score and session correct-move contribution.
- **Validated Save/Load System:** Saves the remaining time, mistake count, session move counters, and the initial, current, and solution grids. Loaded saves are validated before being applied to the active board.

---

## 🛠️ Architectural Overview

- **`Player`**: Stores player credentials, scores, wins, losses, hints, and move statistics.
- **`UserManager`**: Handles registration, login, loading and saving player data, and leaderboard sorting.
- **`SudokuBoard`**: Stores puzzle grids, validates values, solves puzzles using backtracking, and checks solution uniqueness.
- **`GameEngine`**: Manages rendering, input, timing, hints, undo operations, saving games, and the main game loop.
- **`CustomTermUI`**: Provides terminal input and display operations for Windows and POSIX-compatible systems.

---

## 🚀 Getting Started

### Prerequisites

Make sure you have a standard C++ compiler installed (e.g., GCC/MinGW for Windows, Clang for macOS, or G++ for Linux).

### Compilation

Navigate to the root directory containing the source files and execute:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp -o SudokuGame
```

## Running the game

```bash
# Windows
SudokuGame.exe

# Linux / macOS
./SudokuGame
```

## 🎮 How to Play

- **Navigation**: Use W, A, S, and D. Arrow keys are supported on Windows terminals.
- **Insertion**: Simply press numbers 1 through 9 on an empty cell (.).
- **Undo**: Press U to step backward into your match history.
- **Hint**: Press H to automatically solve an empty cell (Max 10 per account).
- **Save Game**: Press G during a game to save the current puzzle state, remaining time, mistakes, and session move counters.
- **Pause / Resume**: Press Esc to pause or resume the active game and timer.

---

## 🤝 Connect with Me

If you have any questions, suggestions, or just want to collaborate, feel free to reach out!

[![LinkedIn](https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/saeidkhazaei/)
[![Medium](https://img.shields.io/badge/Medium-12100E?style=for-the-badge&logo=medium&logoColor=white)](https://medium.com/@saeiiid.khazaei)
