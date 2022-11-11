#include <stdio.h>
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
                // parse
                printf("echo : %s", line);
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
