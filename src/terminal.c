#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

#ifdef _WIN32
static HANDLE hStdin, hStdout;
static DWORD original_mode;
#else
static struct termios original_termios;
#endif

void terminal_init(void) {
#ifdef _WIN32
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hStdin, &original_mode);
    DWORD mode = original_mode;
    mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    //mode &= ~(ENABLE_LINE_INPUT);
    SetConsoleMode(hStdin, mode);
#else
    tcgetattr(STDIN_FILENO, &original_termios);
    struct termios raw = original_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
#endif
}

void terminal_cleanup(void) {
#ifdef _WIN32
    SetConsoleMode(hStdin, original_mode);
#else
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
#endif
}

int terminal_read_key(void) {
#ifdef _WIN32
    if (_kbhit()) {
        int c = _getch();
        if (c == 0 || c == 224) {
            switch (_getch()) {
                case 75: return KEY_LEFT;
                case 77: return KEY_RIGHT;
                case 72: return KEY_UP;
                case 80: return KEY_DOWN;
            }
        }
        return c;
    }
    return -1;
#else
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\x1b') {
            char seq[3];
            if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
            if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': return KEY_UP;
                    case 'B': return KEY_DOWN;
                    case 'C': return KEY_RIGHT;
                    case 'D': return KEY_LEFT;
                }
            }
        }
        return c;
    }
    return -1;
#endif
}

void terminal_get_size(size_t* rows, size_t* cols) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hStdout, &csbi);
    *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    *rows = ws.ws_row;
    *cols = ws.ws_col;
#endif
}

void terminal_clear_screen(void) {
#ifdef _WIN32
    COORD coord = {0, 0};
    DWORD written;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hStdout, &csbi);
    FillConsoleOutputCharacter(hStdout, ' ',
        csbi.dwSize.X * csbi.dwSize.Y, coord, &written);
    FillConsoleOutputAttribute(hStdout,
        csbi.wAttributes,
        csbi.dwSize.X * csbi.dwSize.Y, coord, &written);
    SetConsoleCursorPosition(hStdout, coord);
#else
    terminal_write("\x1b[2J");
    terminal_write("\x1b[H");
#endif
}

void terminal_move_cursor(size_t row, size_t col) {
#ifdef _WIN32
    COORD coord = {col, row};
    SetConsoleCursorPosition(hStdout, coord);
#else
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%zu;%zuH", row + 1, col + 1);
    terminal_write(buf);
#endif
}

void terminal_set_cursor_visible(int visible) {
#ifdef _WIN32
    CONSOLE_CURSOR_INFO cursor_info;
    GetConsoleCursorInfo(hStdout, &cursor_info);
    cursor_info.bVisible = visible;
    SetConsoleCursorInfo(hStdout, &cursor_info);
#else
    terminal_write(visible ? "\x1b[?25h" : "\x1b[?25l");
#endif
}

void terminal_write(const char* str) {
#ifdef _WIN32
    DWORD written;
    WriteConsole(hStdout, str, strlen(str), &written, NULL);
#else
    write(STDOUT_FILENO, str, strlen(str));
#endif
}

void terminal_write_escape(const char* str) {
#ifdef _WIN32
#else
    terminal_write(str);
#endif
}

void delete_cursor_character()
{
#ifdef _WIN32
    const char *text = "\0";
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole,&csbi);
    COORD pos = csbi.dwCursorPosition;
    WriteConsoleOutputCharacter(hConsole,text,strlen(text),pos,&written);
#else
    // TODO: Linux Delete.
#endif
}