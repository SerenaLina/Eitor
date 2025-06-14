#include "editor.h"
#include "terminal.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif
int main(int argc, char* argv[]) {
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0 & (~ENABLE_ECHO_INPUT);
    SetConsoleMode(hStdin,mode); //Unable ENABLE_ECHO_INPUT mode to avoid displaying profile
#else
    //TODO:Linux.
#endif
    char* filename;
    if(argc != 1)
    {
        filename = argv[1];
        FILE *fp = fopen(filename,"w");
        fclose(fp);
    }
    else{
        filename = "ULTIMATE_ANSWER_42"; //Anyone knows that?:)
        FILE *fp = fopen(filename,"w");
        fclose(fp);
    }

    terminal_init();
#ifdef _WIN32
    SetConsoleMode(hStdin,0);
#else
    //TODO:Linux(maybe not,Linux maybe dont have this problem.)
#endif
    Editor* ed = editor_create();
    if (!ed) {
        fprintf(stderr, "Failed to create editor\n");
        return 1;
    }
    
    if (argc > 1) {
        editor_open_file(ed, argv[1]);
    }
    else{
        ed ->filename = filename;
    }
    editor_render(ed);
    while (1) {
        //editor_render(ed); //Do not render all time.
        editor_process_keypress(ed);
    }

    editor_destroy(ed);
    terminal_cleanup();
    return 0;
}