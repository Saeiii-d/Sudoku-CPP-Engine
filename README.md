# 🧩 Cross-Platform Modern Sudoku C++ Engine

A highly optimized, feature-rich, and object-oriented Sudoku game designed for the terminal. Built completely from scratch using modern **C++ (OOP)** and cross-platform terminal wrappers, this project demonstrates robust game-loop design, real-time asynchronous multi-threading event management, and secure dynamic memory management.

---

## ✨ Key Features

* **Real-Time Asynchronous Timer:** Unlike sequential terminal games where the clock updates only upon user action, this engine features a non-blocking game loop that dynamically updates the remaining time second-by-second while waiting for inputs.
* **100% Deadlock-Free Logic:** Player moves are cross-referenced directly with a pre-calculated mathematically verified unique solution grid, preventing any logical deadlocks during runtime.
* **Object-Oriented Architecture:** Refactored into highly cohesive classes (`GameEngine`, `SudokuBoard`, `UserManager`, and `Player`) separating data logic from the rendering pipeline.
* **Secure Session Manager:** Supports registration and login for multiple players, keeping track of scores, win/loss history, and hints used.
* **Password Security:** Credentials are encrypted using a customized Caesar-XOR cipher combination instead of plain-text text files.
* **Smart History Stack (Undo System):** Implements `std::stack` for lightweight and boundless move history tracking.
* **Persistent State Saves:** Allows active sessions to save games, recording precise remaining times, applied grids, and matching session-specific correct/incorrect move history.

---

## 🛠️ Architectural Overview

The project is structured under high-cohesion design patterns:
* **`Player`**: Encapsulates statistics data metrics and secure credential routines.
* **`UserManager`**: Governs persistence algorithms (loading/saving leaderboards), session state-machines, and dynamic ranking sorting routines.
* **`SudokuBoard`**: Handles algorithmic back-tracking solver constraints, unique matrix cell mutation validation, and sub-grid analysis.
* **`GameEngine`**: Coordinates cross-platform console color buffers, HUD coordinate placements, non-blocking hardware interrupt capturing, and chronological event sequences.

---

## 🚀 Getting Started

### Prerequisites
Make sure you have a standard C++ compiler installed (e.g., GCC/MinGW for Windows, Clang for macOS, or G++ for Linux).

### Compilation
Navigate to the root directory containing the source files and execute:

```bash
g++ -O3 src/main.cpp -o SudokuGame
```

## Running the game

```bash
# Windows
SudokuGame.exe

# Linux / macOS
./SudokuGame
```

## 🎮 How to Play

**Navigation**: Use Arrow keys or W, A, S, D to traverse the grid.
**Insertion**: Simply press numbers 1 through 9 on an empty cell (.).
**Undo**: Press U to step backward into your match history.
**Hint**: Press H to automatically solve an empty cell (Max 10 per account).
**Save Game**: Press G mid-match to dump current arrays into a save file.
**Pause Menu**: Hit Esc anytime to halt execution blocks.

---

## 🤝 Connect with Me

If you have any questions, suggestions, or just want to collaborate, feel free to reach out!

[![LinkedIn](https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/saeidkhazaei/)
[![Medium](https://img.shields.io/badge/Medium-12100E?style=for-the-badge&logo=medium&logoColor=white)](https://medium.com/@saeiiid.khazaei)