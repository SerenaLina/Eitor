#include "editor.h"
#include "terminal.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    terminal_init();

    Editor* ed = editor_create();
    if (!ed) {
        fprintf(stderr, "Failed to create editor\n");
        return 1;
    }

    if (argc > 1) {
        editor_open_file(ed, argv[1]);
    }

    while (1) {
        editor_render(ed);
        editor_process_keypress(ed);
    }

    editor_destroy(ed);
    terminal_cleanup();
    return 0;
}