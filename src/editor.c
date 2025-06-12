#include "editor.h"
#include "terminal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define COMMAND_MAX_SIZE 256
#define CLIPBOARD_MAX_SIZE 4096

Editor* editor_create(void) {
    Editor* ed = malloc(sizeof(Editor));
    if (!ed) return NULL;

    ed->buffer = buffer_create(1024);
    if (!ed->buffer) {
        free(ed);
        return NULL;
    }

    ed->filename = NULL;
    ed->cursor_x = 0;
    ed->cursor_y = 0;
    ed->mode = MODE_NORMAL;
    ed->command_buffer = malloc(COMMAND_MAX_SIZE);
    ed->clipboard = malloc(CLIPBOARD_MAX_SIZE);
    ed->dirty = 0;

    terminal_get_size(&ed->screen_rows, &ed->screen_cols);
    ed->screen_rows -= 1;

    return ed;
}

void editor_destroy(Editor* ed) {
    if (ed) {
        buffer_destroy(ed->buffer);
        free(ed->filename);
        free(ed->command_buffer);
        free(ed->clipboard);
        free(ed);
    }
}

static void editor_execute_command(Editor* ed) {
    if (strcmp(ed->command_buffer, "q") == 0) {
        if (!ed->dirty) {
            terminal_cleanup();
            exit(0);
        }
    } else if (strcmp(ed->command_buffer, "w") == 0) {
        editor_save_file(ed);
    } else if (strcmp(ed->command_buffer, "wq") == 0) {
        editor_save_file(ed);
        terminal_cleanup();
        exit(0);
    }
    ed->mode = MODE_NORMAL;
}

void editor_process_keypress(Editor* ed) {
    int c = terminal_read_key();
    if (c == -1) return;

    switch (ed->mode) {
        case MODE_NORMAL:
            switch (c) {
                case 'i':
                    ed->mode = MODE_INSERT;
                    break;
                case ':':
                    ed->mode = MODE_COMMAND;
                    ed->command_buffer[0] = '\0';
                    break;
                case 'h': case 'j': case 'k': case 'l':
                    editor_move_cursor(ed, c);
                    break;
                case 'x':
                    editor_delete_char(ed);
                    break;
                case 'y':
                    if (terminal_read_key() == 'y') {
                        editor_copy_line(ed);
                    }
                    break;
                case 'p':
                    editor_paste(ed);
                    break;
            }
            break;

        case MODE_INSERT:
            if (c == KEY_ESC) {
                ed->mode = MODE_NORMAL;
            } else if (c == KEY_BACKSPACE) {
                editor_delete_char(ed);
            } else {
                editor_insert_char(ed, c);
            }
            break;

        case MODE_COMMAND:
            if (c == KEY_ENTER) {
                editor_execute_command(ed);
            } else if (c == KEY_ESC) {
                ed->mode = MODE_NORMAL;
            } else {
                size_t len = strlen(ed->command_buffer);
                if (len < COMMAND_MAX_SIZE - 1) {
                    ed->command_buffer[len] = c;
                    ed->command_buffer[len + 1] = '\0';
                }
            }
            break;
    }
}

void editor_render(Editor* ed) {
    terminal_clear_screen();

    char* line = malloc(ed->screen_cols + 1);
    if (!line) return;
    size_t line_pos = 0;
    size_t current_row = 0;

    for (size_t i = 0; i < buffer_size(ed->buffer); i++) {
        char c = buffer_char_at(ed->buffer, i);
        if (c == '\n' || line_pos == ed->screen_cols) {
            line[line_pos] = '\0';
            terminal_move_cursor(current_row, 0);
            terminal_write(line);
            current_row++;
            line_pos = 0;
            if (c == '\n') continue;
        }
        line[line_pos++] = c;
    }

    if (line_pos > 0) {
        line[line_pos] = '\0';
        terminal_move_cursor(current_row, 0);
        terminal_write(line);
    }

    free(line);


    terminal_move_cursor(ed->screen_rows, 0);
    switch (ed->mode) {
        case MODE_NORMAL:
            terminal_write("NORMAL");
            break;
        case MODE_INSERT:
            terminal_write("INSERT");
            break;
        case MODE_COMMAND:
            terminal_write(":");
            terminal_write(ed->command_buffer);
            break;
    }

    terminal_move_cursor(ed->cursor_y, ed->cursor_x);
}

void editor_open_file(Editor* ed, const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return;

    char c;
    while ((c = fgetc(fp)) != EOF) {
        buffer_insert_char(ed->buffer, c);
    }

    fclose(fp);
    ed->filename = strdup(filename);
}

void editor_save_file(Editor* ed) {
    if (!ed->filename) return;

    FILE* fp = fopen(ed->filename, "w");
    if (!fp) return;

    char* contents = malloc(buffer_size(ed->buffer) + 1);
    if (!contents) {
        fclose(fp);
        return;
    }

    buffer_get_contents(ed->buffer, contents);
    fprintf(fp, "%s", contents);
    free(contents);
    fclose(fp);
    ed->dirty = 0;
}

void editor_insert_char(Editor* ed, char c) {
    buffer_insert_char(ed->buffer, c);
    if (c == '\n') {
        ed->cursor_y++;
        ed->cursor_x = 0;
    } else {
        ed->cursor_x++;
    }
    ed->dirty = 1;
}

void editor_delete_char(Editor* ed) {
    if (ed->cursor_x > 0) {
        buffer_delete_char(ed->buffer);
        ed->cursor_x--;
        ed->dirty = 1;
    }
}

void editor_move_cursor(Editor* ed, int key) {
    switch (key) {
        case 'h':
            if (ed->cursor_x > 0) ed->cursor_x--;
            break;
        case 'l':
            if (ed->cursor_x < ed->screen_cols - 1) ed->cursor_x++;
            break;
        case 'k':
            if (ed->cursor_y > 0) ed->cursor_y--;
            break;
        case 'j':
            if (ed->cursor_y < ed->screen_rows - 1) ed->cursor_y++;
            break;
    }
}

void editor_copy_line(Editor* ed) {
    // 简单实现：仅复制当前行
    size_t pos = 0;
    size_t line_start = 0;
    size_t line_end = buffer_size(ed->buffer);

    // 找到当前行的开始和结束位置
    for (size_t i = 0; i < buffer_size(ed->buffer); i++) {
        if (pos == ed->cursor_y) {
            line_start = i;
            while (i < buffer_size(ed->buffer) && 
                   buffer_char_at(ed->buffer, i) != '\n') {
                i++;
            }
            line_end = i;
            break;
        }
        if (buffer_char_at(ed->buffer, i) == '\n') pos++;
    }

    // 复制行内容到剪贴板
    size_t len = 0;
    for (size_t i = line_start; i < line_end && len < CLIPBOARD_MAX_SIZE - 1; i++) {
        ed->clipboard[len++] = buffer_char_at(ed->buffer, i);
    }
    ed->clipboard[len] = '\0';
}

void editor_paste(Editor* ed) {
    // 在当前位置插入剪贴板内容
    for (size_t i = 0; ed->clipboard[i] != '\0'; i++) {
        editor_insert_char(ed, ed->clipboard[i]);
    }
}