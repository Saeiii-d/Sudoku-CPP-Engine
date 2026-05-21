#include <bits/stdc++.h>
#include "CustomTermUI.hpp"

using namespace std;

// Defines & Constants
const int BOARD_SIZE = 9;
const int SUBGRID = 3;
const int MAX_PASS_LEN = 20;
const string SAVE_DIR = "./";
const string LEADERBOARD_FILE = "leaderboard.txt";

// ANSI Colors (Using the custom class constants/strings)
const string RED = "\x1b[31m";
const string GREEN = "\x1b[32m";
const string YELLOW = "\x1b[33m";
const string BLUE = "\x1b[34m";
const string LIGHT_BLUE = "\033[94m";
const string CYAN = "\033[36m";
const string PURPLE = "\x1b[35m";
const string WHITE = "\x1b[37m";
const string LIGHT_ORANGE = "\x1b[38;5;214m";
const string RESET = "\x1b[0m";
const string BOLD = "\x1b[1m";
const string BROWN = "\x1b[38;2;164;93;2m";
const string DARK_BROWN = "\x1b[38;2;125;73;5m";
const string LIGHT_BROWN = "\x1b[38;2;242;140;8m";
const string BACK_LIGHERT_BROWN = "\x1b[48;2;249;188;108m";

// Utility functions leveraging CustomTermUI
int getMidCol() { 
    int r, c; 
    CustomTermUI::getTerminalSize(r, c); 
    return c / 2; 
}

int getMidRow() { 
    int r, c; 
    CustomTermUI::getTerminalSize(r, c); 
    return r / 2; 
}

// Structure to track undo moves
struct Move {
    int row;
    int col;
    int oldValue;
    int newValue;
};

// Player Class
class Player {
public:
    string name;
    string passwordHash; 
    int hints = 0;
    int score = 0;
    int wins = 0;
    int losses = 0;
    int correctMoves = 0;
    int incorrectMoves = 0;
    int totalTime = 0;

    static string hashPassword(const string& pass) {
        string hashed = pass;
        for (size_t i = 0; i < hashed.length(); i++) {
            hashed[i] = (hashed[i] ^ 0x5A) + 3;
        }
        return hashed;
    }
};

// UserManager Class
class UserManager {
private:
    vector<Player> players;
    string currentUser;

public:
    UserManager() { currentUser = ""; }

    string getCurrentUser() const { return currentUser; }
    void setCurrentUser(const string& name) { currentUser = name; }
    bool isLoggedIn() const { return !currentUser.empty(); }
    void logout() { currentUser = ""; }
    vector<Player>& getPlayers() { return players; }

    Player* getCurrentPlayerPtr() {
        for (auto& p : players) {
            if (p.name == currentUser) return &p;
        }
        return nullptr;
    }

    void loadLeaderboard() {
        players.clear();
        ifstream file(LEADERBOARD_FILE);
        if (!file.is_open()) return;

        Player p;
        while (file >> p.name >> p.passwordHash >> p.score >> p.wins >> p.losses >> p.totalTime >> p.correctMoves >> p.incorrectMoves >> p.hints) {
            players.push_back(p);
        }
        file.close();
    }

    void saveLeaderboard() {
        ofstream file(LEADERBOARD_FILE);
        if (!file.is_open()) return;

        for (const auto& p : players) {
            file << p.name << " " << p.passwordHash << " " << p.score << " "
                 << p.wins << " " << p.losses << " " << p.totalTime << " "
                 << p.correctMoves << " " << p.incorrectMoves << " " << p.hints << "\n";
        }
        file.close();
    }

    bool registerUser(const string& username, const string& password) {
        for (const auto& p : players) {
            if (p.name == username) return false;
        }
        Player newPlayer;
        newPlayer.name = username;
        newPlayer.passwordHash = Player::hashPassword(password);
        players.push_back(newPlayer);
        saveLeaderboard();
        return true;
    }

    bool loginUser(const string& username, const string& password) {
        string targetHash = Player::hashPassword(password);
        for (auto& p : players) {
            if (p.name == username && p.passwordHash == targetHash) {
                currentUser = username;
                return true;
            }
        }
        return false;
    }

    void sortLeaderboard() {
        sort(players.begin(), players.end(), [](const Player& a, const Player& b) {
            if (a.score != b.score) return a.score > b.score;
            return a.totalTime < b.totalTime;
        });
    }
};

// SudokuBoard Class
class SudokuBoard {
private:
    int currentGrid[BOARD_SIZE][BOARD_SIZE];
    int initialGrid[BOARD_SIZE][BOARD_SIZE];
    int solutionGrid[BOARD_SIZE][BOARD_SIZE];

public:
    int getCell(int r, int c) const { return currentGrid[r][c]; }
    void setCell(int r, int c, int val) { currentGrid[r][c] = val; }
    int getInitialCell(int r, int c) const { return initialGrid[r][c]; }
    void setInitialCell(int r, int c, int val) { initialGrid[r][c] = val; }
    int getSolutionCell(int r, int c) const { return solutionGrid[r][c]; }
    void setSolutionCell(int r, int c, int val) { solutionGrid[r][c] = val; }

    bool isValidPose(int row, int col, int num, int grid[BOARD_SIZE][BOARD_SIZE]) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (grid[row][i] == num || grid[i][col] == num)
                return false;

            int startRow = (row / SUBGRID) * SUBGRID;
            int startCol = (col / SUBGRID) * SUBGRID;
            if (grid[startRow + i / SUBGRID][startCol + i % SUBGRID] == num)
                return false;
        }
        return true;
    }

    bool solveSudoku(int grid[BOARD_SIZE][BOARD_SIZE]) {
        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                if (grid[row][col] == 0) {
                    for (int num = 1; num <= BOARD_SIZE; num++) {
                        if (isValidPose(row, col, num, grid)) {
                            grid[row][col] = num;
                            if (solveSudoku(grid)) return true;
                            grid[row][col] = 0;
                        }
                    }
                    return false;
                }
            }
        }
        return true;
    }

    int solutionsCounter(int grid[BOARD_SIZE][BOARD_SIZE], int& count) {
        int row = -1, col = -1;
        bool isEmpty = true;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (grid[i][j] == 0) {
                    row = i;
                    col = j;
                    isEmpty = false;
                    break;
                }
            }
            if (!isEmpty) break;
        }

        if (isEmpty) {
            count++;
            return count;
        }

        for (int num = 1; num <= BOARD_SIZE; num++) {
            if (isValidPose(row, col, num, grid)) {
                grid[row][col] = num;
                solutionsCounter(grid, count);
                if (count > 1) {
                    grid[row][col] = 0;
                    return count;
                }
                grid[row][col] = 0;
            }
        }
        return count;
    }

    void generateFullSudoku() {
        memset(currentGrid, 0, sizeof(currentGrid));
        for (int i = 0; i < 2 * BOARD_SIZE; i++) {
            int row = rand() % BOARD_SIZE;
            int col = rand() % BOARD_SIZE;
            int num = rand() % BOARD_SIZE + 1;
            if (currentGrid[row][col] == 0 && isValidPose(row, col, num, currentGrid)) {
                currentGrid[row][col] = num;
            }
        }
        solveSudoku(currentGrid);
        memcpy(solutionGrid, currentGrid, sizeof(currentGrid));
    }

    void removeElements(int elementsToRemove) {
        while (elementsToRemove > 0) {
            int row = rand() % BOARD_SIZE;
            int col = rand() % BOARD_SIZE;
            if (currentGrid[row][col] != 0) {
                int temp = currentGrid[row][col];
                currentGrid[row][col] = 0;

                int backup[BOARD_SIZE][BOARD_SIZE];
                memcpy(backup, currentGrid, sizeof(currentGrid));

                int count = 0;
                solutionsCounter(backup, count);

                if (count != 1) {
                    currentGrid[row][col] = temp;
                } else {
                    elementsToRemove--;
                }
            }
        }
    }

    void initBoard(int difficulty) {
        generateFullSudoku();
        int toRemove = (difficulty == 1) ? 20 : ((difficulty == 2) ? 30 : 40);
        removeElements(toRemove);
        memcpy(initialGrid, currentGrid, sizeof(currentGrid));
    }

    bool isFull() {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (currentGrid[i][j] == 0) return false;
            }
        }
        return true;
    }
};

// GameEngine Class
class GameEngine {
private:
    UserManager userManager;
    SudokuBoard board;
    stack<Move> undoStack;
    int cursorRow = 0;
    int cursorCol = 0;
    bool isGamePaused = false;

    int currentSessionCorrectMoves = 0;
    int currentSessionIncorrectMoves = 0;

    void displaySudoku() {
        int window_col = getMidCol() * 2;
        int window_row = getMidRow() * 2;
        int board_height = BOARD_SIZE * 2 + 1; 
        int board_width = (BOARD_SIZE * 4) + (BOARD_SIZE / SUBGRID) - 1;
        int start_row = (window_row - board_height) / 2;
        int start_col = (window_col - board_width) / 2;

        CustomTermUI::moveCursor(start_row, start_col);
        printf("%s╔", BROWN.c_str());
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("═══");
            if ((j + 1) % SUBGRID == 0 && j != BOARD_SIZE - 1) printf("╦");
            else if (j != BOARD_SIZE - 1) printf("╤");
        }
        printf("╗%s\n", RESET.c_str());

        for (int i = 0; i < BOARD_SIZE; i++) {
            CustomTermUI::moveCursor(start_row + 1 + (i * 2), start_col);
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (j % SUBGRID == 0) printf("%s║%s", BROWN.c_str(), RESET.c_str());
                else printf("%s│%s", BROWN.c_str(), RESET.c_str());

                if (i == cursorRow && j == cursorCol) {
                    printf("%s%s %d %s", BACK_LIGHERT_BROWN.c_str(), DARK_BROWN.c_str(), board.getCell(i, j), RESET.c_str());
                } else if (board.getCell(i, j) == 0) {
                    printf(" %s.%s ", YELLOW.c_str(), RESET.c_str());
                } else {
                    if (board.getInitialCell(i, j) != 0) {
                        printf(" %s%d%s ", WHITE.c_str(), board.getCell(i, j), RESET.c_str());
                    } else {
                        printf(" %s%d%s ", LIGHT_BLUE.c_str(), board.getCell(i, j), RESET.c_str());
                    }
                }
            }
            printf("%s║%s\n", BROWN.c_str(), RESET.c_str());

            if (i < BOARD_SIZE - 1) {
                CustomTermUI::moveCursor(start_row + 2 + (i * 2), start_col);
                if ((i + 1) % SUBGRID == 0) {
                    printf("%s╠", BROWN.c_str());
                    for (int j = 0; j < BOARD_SIZE; j++) {
                        printf("═══");
                        if ((j + 1) % SUBGRID == 0 && j != BOARD_SIZE - 1) printf("╬");
                        else if (j != BOARD_SIZE - 1) printf("╪");
                    }
                    printf("╣%s\n", RESET.c_str());
                } else {
                    printf("%s╟", BROWN.c_str());
                    for (int j = 0; j < BOARD_SIZE; j++) {
                        printf("───");
                        if ((j + 1) % SUBGRID == 0 && j != BOARD_SIZE - 1) printf("╫");
                        else if (j != BOARD_SIZE - 1) printf("┼");
                    }
                    printf("╢%s\n", RESET.c_str());
                }
            }
        }

        CustomTermUI::moveCursor(start_row + (BOARD_SIZE * 2), start_col);
        printf("%s╚", BROWN.c_str());
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("═══");
            if ((j + 1) % SUBGRID == 0 && j != BOARD_SIZE - 1) printf("╩");
            else if (j != BOARD_SIZE - 1) printf("╧");
        }
        printf("╝%s\n", RESET.c_str());
    }

    void moveCursor(int key) {
        if (key == 72 || key == 'w' || key == 'W') { if (cursorRow > 0) cursorRow--; } 
        else if (key == 80 || key == 's' || key == 'S') { if (cursorRow < BOARD_SIZE - 1) cursorRow++; } 
        else if (key == 75 || key == 'a' || key == 'A') { if (cursorCol > 0) cursorCol--; } 
        else if (key == 77 || key == 'd' || key == 'D') { if (cursorCol < BOARD_SIZE - 1) cursorCol++; } 
    }

    void updateLiveHUD(int remainingTime, int mistakes) {
        int sideRow = getMidRow() - 5;
        int sideCol = 2;

        CustomTermUI::moveCursor(sideRow, sideCol);
        printf("%sRemaining Time: %02d:%02d%s   ", CYAN.c_str(), remainingTime / 60, remainingTime % 60, RESET.c_str());
        CustomTermUI::moveCursor(sideRow + 1, sideCol);
        printf("%sMistakes: %d / 5%s", RED.c_str(), mistakes, RESET.c_str());

        CustomTermUI::moveCursor(sideRow + 3, sideCol);
        printf("Actions:");
        CustomTermUI::moveCursor(sideRow + 4, sideCol);
        printf(" - Move: Arrow keys / WASD");
        CustomTermUI::moveCursor(sideRow + 5, sideCol);
        printf(" - Insert: Numbers (1-9)");
        CustomTermUI::moveCursor(sideRow + 6, sideCol);
        printf(" - Undo: U");
        CustomTermUI::moveCursor(sideRow + 7, sideCol);
        printf(" - Hint: H");
        CustomTermUI::moveCursor(sideRow + 8, sideCol);
        printf(" - Save: G");
        CustomTermUI::moveCursor(sideRow + 9, sideCol);
        printf(" - Pause/Resume: Esc");
        CustomTermUI::moveCursor(sideRow + 10, sideCol);
        printf(" - Quit: Q");
    }

    void requestHint() {
        Player* cp = userManager.getCurrentPlayerPtr();
        if (!cp) return;

        if (cp->hints >= 10) {
            CustomTermUI::moveCursor(getMidRow() + 10, 2);
            printf("%sNo hints left! (Max 10)%s", RED.c_str(), RESET.c_str());
            CustomTermUI::sleepMs(1000);
            return;
        }

        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board.getCell(i, j) == 0) {
                    int correctVal = board.getSolutionCell(i, j);
                    undoStack.push({i, j, 0, correctVal});
                    board.setCell(i, j, correctVal);
                    
                    cp->hints++;
                    cp->score++;
                    currentSessionCorrectMoves++;
                    userManager.saveLeaderboard();
                    return;
                }
            }
        }
    }

    void undoMove() {
        if (undoStack.empty()) return;
        Move last = undoStack.top();
        undoStack.pop();
        board.setCell(last.row, last.col, last.oldValue);
    }

    void saveGame(int mistakes, int remainingTime) {
        CustomTermUI::clearScreen();
        string filename;
        CustomTermUI::moveCursor(getMidRow() - 1, getMidCol() - 25);
        printf("Enter save file name (without extension): ");
        cin >> filename;
        filename += ".txt";

        ofstream file(filename);
        if (!file.is_open()) {
            printf("Failed to create save file.\n");
            CustomTermUI::sleepMs(1000);
            return;
        }

        file << userManager.getCurrentUser() << "\n" 
             << remainingTime << " " << mistakes << " " 
             << currentSessionCorrectMoves << " " << currentSessionIncorrectMoves << "\n";

        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) file << board.getInitialCell(i, j) << " ";
            file << "\n";
        }
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) file << board.getCell(i, j) << " ";
            file << "\n";
        }
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) file << board.getSolutionCell(i, j) << " ";
            file << "\n";
        }
        file.close();
        printf("Game successfully saved to %s\n", filename.c_str());
        CustomTermUI::sleepMs(1500);
    }

    void gameOverScreen(bool won, int finalRemainingTime) {
        CustomTermUI::clearScreen();
        Player* cp = userManager.getCurrentPlayerPtr();
        if (!cp) return;

        if (won) cp->wins++;
        else cp->losses++;
        cp->totalTime += (180 - finalRemainingTime);
        
        cp->correctMoves += currentSessionCorrectMoves;
        cp->incorrectMoves += currentSessionIncorrectMoves;
        userManager.saveLeaderboard();

        CustomTermUI::moveCursor(getMidRow() - 4, getMidCol() - 15);
        if (won) printf("%s🎉 CONGRATULATIONS! YOU WON! 🎉%s", GREEN.c_str(), RESET.c_str());
        else printf("%s💀 GAME OVER! YOU LOST! 💀%s", RED.c_str(), RESET.c_str());

        CustomTermUI::moveCursor(getMidRow() - 2, getMidCol() - 10);
        printf("Player: %s", cp->name.c_str());
        CustomTermUI::moveCursor(getMidRow() - 1, getMidCol() - 10);
        printf("Correct Moves: %d", currentSessionCorrectMoves);
        CustomTermUI::moveCursor(getMidRow(), getMidCol() - 10);
        printf("Incorrect Moves: %d", currentSessionIncorrectMoves);
        CustomTermUI::moveCursor(getMidRow() + 1, getMidCol() - 10);
        printf("Final Score From This Game: %d", currentSessionCorrectMoves - currentSessionIncorrectMoves);

        CustomTermUI::moveCursor(getMidRow() + 3, getMidCol() - 15);
        printf("Press 'M' to return to Main Menu...");
        while (true) {
            char ch = CustomTermUI::getChar();
            if (ch == 'm' || ch == 'M') return;
        }
    }

    void playGame(int remainingTime, int mistakes) {
        time_t lastTime = time(NULL);
        while (!board.isFull() && mistakes < 5 && remainingTime > 0) {
            if (!isGamePaused) {
                displaySudoku();
                updateLiveHUD(remainingTime, mistakes);
                CustomTermUI::setCursorVisibility(false);
            }

            while (!CustomTermUI::isKeyboardHit()) {
                time_t now = time(NULL);
                if (now - lastTime >= 1) {
                    if (!isGamePaused) {
                        remainingTime -= (now - lastTime);
                        if (remainingTime <= 0) break;
                        updateLiveHUD(remainingTime, mistakes);
                    }
                    lastTime = now;
                }
                CustomTermUI::sleepMs(50); 
            }

            if (remainingTime <= 0) break;

            int key = CustomTermUI::getChar();

            if (key == 0 || key == 224) {
                key = CustomTermUI::getChar(); 
                moveCursor(key);
                continue;
            }

            if (key == 27) { 
                isGamePaused = !isGamePaused;
                CustomTermUI::clearScreen();
                if (isGamePaused) {
                    CustomTermUI::moveCursor(getMidRow(), getMidCol() - 10);
                    printf("%s⏸️ GAME PAUSED ⏸️%s", YELLOW.c_str(), RESET.c_str());
                    CustomTermUI::moveCursor(getMidRow() + 1, getMidCol() - 15);
                    printf("Press Escape again to Resume.");
                }
            } else if (!isGamePaused) {
                if (key == 'g' || key == 'G') {
                    saveGame(mistakes, remainingTime);
                    return;
                } else if (key == 'u' || key == 'U') {
                    undoMove();
                } else if (key == 'h' || key == 'H') {
                    requestHint();
                } else if (key == 'q' || key == 'Q') {
                    gameOverScreen(false, remainingTime);
                    return;
                } else if (key >= '1' && key <= '9') {
                    int num = key - '0';
                    if (board.getInitialCell(cursorRow, cursorCol) != 0) {
                        continue; 
                    }

                    if (board.getSolutionCell(cursorRow, cursorCol) == num) {
                        undoStack.push({cursorRow, cursorCol, board.getCell(cursorRow, cursorCol), num});
                        board.setCell(cursorRow, cursorCol, num);
                        currentSessionCorrectMoves++;
                        Player* cp = userManager.getCurrentPlayerPtr();
                        if (cp) {
                            cp->score++;
                        }
                    } else {
                        mistakes++;
                        currentSessionIncorrectMoves++;
                        Player* cp = userManager.getCurrentPlayerPtr();
                        if (cp) {
                            if (cp->score > 0) cp->score--;
                        }
                    }
                    userManager.saveLeaderboard();
                } else {
                    moveCursor(key);
                }
            }
        }

        if (board.isFull() && mistakes < 5 && remainingTime > 0) {
            gameOverScreen(true, remainingTime);
        } else {
            gameOverScreen(false, remainingTime);
        }
    }

    void selectDifficulty() {
        int diff = 1;
        while (true) {
            CustomTermUI::clearScreen();
            CustomTermUI::moveCursor(getMidRow() - 2, getMidCol() - 10);
            printf("%sSelect Difficulty:%s", BROWN.c_str(), RESET.c_str());
            CustomTermUI::moveCursor(getMidRow() - 1, getMidCol() - 8);
            printf("%s %s Easy %s", (diff == 1 ? "➜" : " "), (diff == 1 ? GREEN.c_str() : WHITE.c_str()), RESET.c_str());
            CustomTermUI::moveCursor(getMidRow(), getMidCol() - 8);
            printf("%s %s Medium %s", (diff == 2 ? "➜" : " "), (diff == 2 ? YELLOW.c_str() : WHITE.c_str()), RESET.c_str());
            CustomTermUI::moveCursor(getMidRow() + 1, getMidCol() - 8);
            printf("%s %s Hard %s", (diff == 3 ? "➜" : " "), (diff == 3 ? RED.c_str() : WHITE.c_str()), RESET.c_str());

            int key = CustomTermUI::getChar();
            if (key == 72 || key == 'w') { if (diff > 1) diff--; }
            else if (key == 80 || key == 's') { if (diff < 3) diff++; }
            else if (key == 13) break; 
        }
        board.initBoard(diff);
        while (!undoStack.empty()) undoStack.pop();

        currentSessionCorrectMoves = 0;
        currentSessionIncorrectMoves = 0;

        playGame(180, 0);
    }

    void registerMenu() {
        CustomTermUI::clearScreen();
        string user, pass, confirm;
        CustomTermUI::moveCursor(getMidRow() - 3, getMidCol() - 15);
        printf("%s📝 Register New Account%s", PURPLE.c_str(), RESET.c_str());
        CustomTermUI::moveCursor(getMidRow() - 1, getMidCol() - 15);
        printf("Username: "); cin >> user;
        CustomTermUI::moveCursor(getMidRow(), getMidCol() - 15);
        printf("Password: "); cin >> pass;
        CustomTermUI::moveCursor(getMidRow() + 1, getMidCol() - 15);
        printf("Confirm Password: "); cin >> confirm;

        if (pass != confirm || pass.length() < 4) {
            CustomTermUI::moveCursor(getMidRow() + 3, getMidCol() - 20);
            printf("%sPasswords mismatch or too short (Min 4 chars).%s", RED.c_str(), RESET.c_str());
            CustomTermUI::sleepMs(1500);
            return;
        }

        if (userManager.registerUser(user, pass)) {
            CustomTermUI::moveCursor(getMidRow() + 3, getMidCol() - 15);
            printf("%sRegistration Successful!%s", GREEN.c_str(), RESET.c_str());
        } else {
            CustomTermUI::moveCursor(getMidRow() + 3, getMidCol() - 15);
            printf("%sUsername already exists!%s", RED.c_str(), RESET.c_str());
        }
        CustomTermUI::sleepMs(1500);
    }

    void loginMenu() {
        CustomTermUI::clearScreen();
        string user, pass;
        CustomTermUI::moveCursor(getMidRow() - 2, getMidCol() - 15);
        printf("%s🔑 User Login%s", BROWN.c_str(), RESET.c_str());
        CustomTermUI::moveCursor(getMidRow(), getMidCol() - 15);
        printf("Username: "); cin >> user;
        CustomTermUI::moveCursor(getMidRow() + 1, getMidCol() - 15);
        printf("Password: "); cin >> pass;

        if (userManager.loginUser(user, pass)) {
            CustomTermUI::moveCursor(getMidRow() + 3, getMidCol() - 15);
            printf("%sLogin successful! Welcome back.%s", GREEN.c_str(), RESET.c_str());
        } else {
            CustomTermUI::moveCursor(getMidRow() + 3, getMidCol() - 15);
            printf("%sInvalid credentials.%s", RED.c_str(), RESET.c_str());
        }
        CustomTermUI::sleepMs(1500);
    }

    void loadGameMenu() {
        CustomTermUI::clearScreen();
        string filename;
        CustomTermUI::moveCursor(getMidRow() - 2, getMidCol() - 20);
        printf("Enter save filename to load (e.g. game.txt): ");
        cin >> filename;

        ifstream file(filename);
        if (!file.is_open()) {
            printf("Save file not found.\n");
            CustomTermUI::sleepMs(1500);
            return;
        }

        string fileUser;
        int remainingTime, mistakes;
        file >> fileUser >> remainingTime >> mistakes >> currentSessionCorrectMoves >> currentSessionIncorrectMoves;

        if (fileUser != userManager.getCurrentUser()) {
            printf("This save file belongs to another user!\n");
            file.close();
            CustomTermUI::sleepMs(1500);
            return;
        }

        int val;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) { file >> val; board.setInitialCell(i, j, val); }
        }
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) { file >> val; board.setCell(i, j, val); }
        }
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) { file >> val; board.setSolutionCell(i, j, val); }
        }
        file.close();

        printf("Game state loaded successfully.\n");
        CustomTermUI::sleepMs(1000);
        while (!undoStack.empty()) undoStack.pop();
        playGame(remainingTime, mistakes);
    }

    void displayLeaderboard() {
        CustomTermUI::clearScreen();
        userManager.sortLeaderboard();
        auto players = userManager.getPlayers();

        int startRow = getMidRow() - 5;
        int startCol = getMidCol() - 45;

        CustomTermUI::moveCursor(startRow, startCol);
        printf("%s╔═════════════╤═════════════╤══════════╤═══════╤═════════╤════════════╤═════════════╤══════════════╗%s\n", BROWN.c_str(), RESET.c_str());
        CustomTermUI::moveCursor(startRow + 1, startCol);
        printf("%s║    Rank     │    Name     │  Score   │ Wins  │ Losses  │  Correct   │  Incorrect  │  Total Time  ║%s\n", BROWN.c_str(), RESET.c_str());
        CustomTermUI::moveCursor(startRow + 2, startCol);
        printf("%s╠═════════════╪═════════════╪══════════╪═══════╪═════════╪════════════╪═════════════╪══════════════╣%s\n", BROWN.c_str(), RESET.c_str());

        int rank = 1;
        for (const auto& p : players) {
            CustomTermUI::moveCursor(startRow + 2 + rank, startCol);
            printf("%s║      %d      │ %-11s │ %-8d │ %-5d │ %-7d │ %-10d │ %-11d │ %-12d ║%s\n",
                   BROWN.c_str(), rank, p.name.c_str(), p.score, p.wins, p.losses, p.correctMoves, p.incorrectMoves, p.totalTime, RESET.c_str());
            rank++;
            if (rank > 10) break;
        }
        printf("\n\nPress 'M' to return to menu...");
        while (true) {
            char ch = CustomTermUI::getChar();
            if (ch == 'm' || ch == 'M') return;
        }
    }

public:
    void initEngine() {
        srand(time(NULL));
        userManager.loadLeaderboard();
    }

    void run() {
        initEngine();
        int choice = 1;

        while (true) {
            CustomTermUI::clearScreen();
            CustomTermUI::setCursorVisibility(true);
            int maxOptions = userManager.isLoggedIn() ? 5 : 3;

            CustomTermUI::moveCursor(2, getMidCol() - 10);
            printf("%s%s✨ MODERN SUDOKU C++ ENGINE ✨%s", BOLD.c_str(), YELLOW.c_str(), RESET.c_str());

            if (!userManager.isLoggedIn()) {
                CustomTermUI::moveCursor(getMidRow() - 1, getMidCol() - 8);
                printf("%s Login%s", (choice == 1 ? "➜ " : "  "), RESET.c_str());
                CustomTermUI::moveCursor(getMidRow(), getMidCol() - 8);
                printf("%s Register%s", (choice == 2 ? "➜ " : "  "), RESET.c_str());
                CustomTermUI::moveCursor(getMidRow() + 1, getMidCol() - 8);
                printf("%s Exit%s", (choice == 3 ? "➜ " : "  "), RESET.c_str());
            } else {
                CustomTermUI::moveCursor(getMidRow() - 4, getMidCol() - 12);
                printf("%sActive Session: %s%s%s", LIGHT_BROWN.c_str(), BOLD.c_str(), userManager.getCurrentUser().c_str(), RESET.c_str());

                CustomTermUI::moveCursor(getMidRow() - 2, getMidCol() - 10);
                printf("%s New Game%s", (choice == 1 ? "➜ " : "  "), RESET.c_str());
                CustomTermUI::moveCursor(getMidRow() - 1, getMidCol() - 10);
                printf("%s Load Game%s", (choice == 2 ? "➜ " : "  "), RESET.c_str());
                CustomTermUI::moveCursor(getMidRow(), getMidCol() - 10);
                printf("%s Leaderboard%s", (choice == 3 ? "➜ " : "  "), RESET.c_str());
                CustomTermUI::moveCursor(getMidRow() + 1, getMidCol() - 10);
                printf("%s Logout%s", (choice == 4 ? "➜ " : "  "), RESET.c_str());
                CustomTermUI::moveCursor(getMidRow() + 2, getMidCol() - 10);
                printf("%s Exit%s", (choice == 5 ? "➜ " : "  "), RESET.c_str());
            }

            int key = CustomTermUI::getChar();
            if (key == 72 || key == 'w') { if (choice > 1) choice--; }
            else if (key == 80 || key == 's') { if (choice < maxOptions) choice++; }
            else if (key == 13) { 
                if (!userManager.isLoggedIn()) {
                    if (choice == 1) loginMenu();
                    else if (choice == 2) registerMenu();
                    else { 
                        CustomTermUI::setCursorVisibility(true); 
                        CustomTermUI::clearScreen(); 
                        exit(0); 
                    }
                } else {
                    if (choice == 1) selectDifficulty();
                    else if (choice == 2) loadGameMenu();
                    else if (choice == 3) displayLeaderboard();
                    else if (choice == 4) userManager.logout();
                    else { 
                        CustomTermUI::setCursorVisibility(true); 
                        CustomTermUI::clearScreen(); 
                        exit(0); 
                    }
                }
                choice = 1; 
            }
        }
    }
};

int main() {
    CustomTermUI::init();
    GameEngine engine;
    engine.run();
    return 0;
}