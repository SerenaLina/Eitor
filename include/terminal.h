#ifndef EITOR_TERMINAL_H
#define EITOR_TERMINAL_H

#include <stddef.h>

#define KEY_ESC     27
#define KEY_ENTER   13
#define KEY_BACKSPACE 127
#define KEY_LEFT    1000
#define KEY_RIGHT   1001
#define KEY_UP      1002
#define KEY_DOWN    1003

void terminal_init(void);
void terminal_cleanup(void);
int terminal_read_key(void);
void terminal_get_size(size_t* rows, size_t* cols);
void terminal_clear_screen(void);
void terminal_move_cursor(size_t row, size_t col);
void terminal_set_cursor_visible(int visible);
void terminal_refresh_screen(void);

void terminal_write(const char* str);
void terminal_write_escape(const char* str);

#endif