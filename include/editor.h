#ifndef EITOR_EDITOR_H
#define EITOR_EDITOR_H

#include "buffer.h"

typedef enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_COMMAND
} EditorMode;

typedef struct {
    GapBuffer* buffer;
    char* filename;
    size_t cursor_x;
    size_t cursor_y;
    size_t screen_rows;
    size_t screen_cols;
    EditorMode mode;
    char* command_buffer;
    char* clipboard;
    int dirty;
} Editor;

Editor* editor_create(void);
void editor_destroy(Editor* ed);
void editor_process_keypress(Editor* ed);
void editor_render(Editor* ed);
void editor_open_file(Editor* ed, const char* filename);
void editor_save_file(Editor* ed);
void editor_insert_char(Editor* ed, char c);
void editor_delete_char(Editor* ed);
void editor_move_cursor(Editor* ed, int key);
void editor_copy_line(Editor* ed);
void editor_paste(Editor* ed);

#endif