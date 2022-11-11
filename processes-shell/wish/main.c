#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* PROMPT = "wish> ";

int main(int argc, char const *argv[]) {
    if (argc == 1) {
        // interact mode
        char *line = NULL;
        size_t linecap = 0;
        ssize_t linelen = 0;
        do {
            if (linelen != 0) {
                line[linelen-1] = '\0'; // the return of getline is a new line. so earse it.

                // parse
                char *token;
                char *argv[10]; // FIXME: 10?
                int argc = 0;
                while ((token = strsep(&line, " ")) != NULL) {
                    argv[argc++] = token; 
                }
                argv[argc] = NULL;
                
                // argv[0] == command, argv[1..] == args
                if (strcmp(argv[0], "exit") == 0) {
                    exit(0);
                }
            }
            printf("%s", PROMPT);
        } while ((linelen = getline(&line, &linecap, stdin)) > 0);
    } else if (argc == 2) {
        // batch mode
    } else {
        // wrong
        exit(1);
    }

    return 0;
}
