#ifndef CUSTOM_TERM_UI_HPP
#define CUSTOM_TERM_UI_HPP

#include <iostream>
#include <string>
#include <cstdio>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
#endif

class CustomTermUI {
public:
    // Initializes the terminal settings at application startup
    static void init() {
        #if defined(_WIN32) || defined(_WIN64)
            // Enable ANSI Virtual Terminal Processing for Windows 10/11
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut != INVALID_HANDLE_VALUE) {
                DWORD dwMode = 0;
                if (GetConsoleMode(hOut, &dwMode)) {
                    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                    SetConsoleMode(hOut, dwMode);
                }
            }
        #else
            // Native ANSI escape codes are supported by default on Unix/Linux/macOS
        #endif
    }

    // Clears the entire screen and moves the cursor to home position
    static void clearScreen() {
        std::cout << "\033[2J\033[H" << std::flush;
    }

    // Moves the cursor to the specified row and column coordinates
    static void moveCursor(int row, int col) {
        std::cout << "\033[" << row << ";" << col << "H" << std::flush;
    }

    // Toggles terminal cursor visibility
    static void setCursorVisibility(bool visible) {
        if (visible) {
            std::cout << "\033[?25h" << std::flush;
        } else {
            std::cout << "\033[?25l" << std::flush;
        }
    }

    // Sets foreground text color using 24-bit RGB (True Color)
    static void setTextColorRGB(int r, int g, int b) {
        std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m" << std::flush;
    }

    // Sets background cell color using 24-bit RGB (True Color)
    static void setBackgroundColorRGB(int r, int g, int b) {
        std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m" << std::flush;
    }

    // Resets text colors and formatting styles back to terminal defaults
    static void resetColors() {
        std::cout << "\033[0m" << std::flush;
    }

    // Retrieves current window dimensions (total rows and columns)
    static void getTerminalSize(int &rows, int &cols) {
        #if defined(_WIN32) || defined(_WIN64)
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
                cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            } else {
                cols = 80; rows = 25; // Fallback defaults
            }
        #else
            struct winsize w;
            if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
                cols = w.ws_col;
                rows = w.ws_row;
            } else {
                cols = 80; rows = 25; // Fallback defaults
            }
        #endif
    }

    // Non-blocking keyboard state check (Equivalent to _kbhit)
    static bool isKeyboardHit() {
        #if defined(_WIN32) || defined(_WIN64)
            return _kbhit() != 0;
        #else
            // Low-level asynchronous select polling for POSIX streams
            struct timeval tv = {0L, 0L};
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(STDIN_FILENO, &fds);
            return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
        #endif
    }

    // Reads a single character instantly without waiting for Enter (Equivalent to _getch)
    static int getChar() {
        #if defined(_WIN32) || defined(_WIN64)
            return _getch();
        #else
            // Switch terminal state temporarily to Raw Mode to bypass line buffering
            struct termios oldt, newt;
            int ch;
            tcgetattr(STDIN_FILENO, &oldt);
            newt = oldt;
            newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical processing and echo
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
            ch = getchar();
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore original attributes
            return ch;
        #endif
    }

    // Suspends thread execution for a specified duration in milliseconds
    static void sleepMs(size_t milliseconds) {
        #if defined(_WIN32) || defined(_WIN64)
            Sleep(milliseconds);
        #else
            usleep(milliseconds * 1000);
        #endif
    }
};

#endif // CUSTOM_TERM_UI_HPP