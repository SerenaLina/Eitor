#ifndef EITOR_BUFFER_H
#define EITOR_BUFFER_H

#include <stddef.h>

typedef struct {
    char* buffer;
    size_t size;
    size_t gap_start;
    size_t gap_size;
} GapBuffer;

GapBuffer* buffer_create(size_t initial_size);
void buffer_destroy(GapBuffer* gb);
void buffer_insert_char(GapBuffer* gb, char c);
void buffer_delete_char(GapBuffer* gb);
void buffer_move_gap_to(GapBuffer* gb, size_t pos);
size_t buffer_size(const GapBuffer* gb);
char buffer_char_at(const GapBuffer* gb, size_t pos);
void buffer_get_contents(const GapBuffer* gb, char* dest);

#endif