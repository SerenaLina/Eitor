#include "buffer.h"
#include "editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "terminal.h"

#define INITIAL_GAP_SIZE 128

GapBuffer* buffer_create(size_t initial_size) {
    GapBuffer* gb = malloc(sizeof(GapBuffer));
    if (!gb) return NULL;

    size_t size = initial_size + INITIAL_GAP_SIZE;
    gb->buffer = malloc(size);
    if (!gb->buffer) {
        free(gb);
        return NULL;
    }

    gb->size = size;
    gb->gap_start = 0;
    gb->gap_size = INITIAL_GAP_SIZE;

    return gb;
}

void buffer_destroy(GapBuffer* gb) {
    if (gb) {
        free(gb->buffer);
        free(gb);
    }
}

static void expand_gap(GapBuffer* gb) {
    size_t new_size = gb->size * 2;
    char* new_buffer = malloc(new_size);
    if (!new_buffer) return;

    memcpy(new_buffer, gb->buffer, gb->gap_start);

    size_t after_gap = gb->gap_start + gb->gap_size;
    size_t after_gap_size = gb->size - after_gap;
    memcpy(new_buffer + new_size - after_gap_size,
           gb->buffer + after_gap,
           after_gap_size);

    free(gb->buffer);
    gb->buffer = new_buffer;
    gb->gap_size = new_size - (gb->size - gb->gap_size);
    gb->size = new_size;
}

void buffer_move_gap_to(GapBuffer* gb, size_t pos) {
    if (pos == gb->gap_start) return;

    if (pos < gb->gap_start) {
        size_t move_size = gb->gap_start - pos;
        memmove(gb->buffer + pos + gb->gap_size,
                gb->buffer + pos,
                move_size);
    } else {
        size_t after_gap = gb->gap_start + gb->gap_size;
        size_t move_size = pos - gb->gap_start;
        memmove(gb->buffer + gb->gap_start,
                gb->buffer + after_gap,
                move_size);
        pos = pos - move_size;
    }

    gb->gap_start = pos;
}

void buffer_insert_char(GapBuffer* gb, char c) {
    if (gb->gap_size == 0) { 
        expand_gap(gb);
    }

    gb->buffer[gb->gap_start++] = c;
    gb->gap_size--;
}

void buffer_delete_char(GapBuffer* gb) {
    if (gb->gap_start > 0) {
        delete_cursor_character();
        gb->gap_start--;
        gb->gap_size++;
    }
}

size_t buffer_size(const GapBuffer* gb) {
    return gb->size - gb->gap_size;
}

char buffer_char_at(const GapBuffer* gb, size_t pos) {
    if (pos >= gb->gap_start) {
        pos += gb->gap_size;
    }
    return gb->buffer[pos];
}

void buffer_get_contents(const GapBuffer* gb, char* dest) {
    memcpy(dest, gb->buffer, gb->gap_start);
    size_t after_gap = gb->gap_start + gb->gap_size;
    memcpy(dest + gb->gap_start,
           gb->buffer + after_gap,
           gb->size - after_gap);
}