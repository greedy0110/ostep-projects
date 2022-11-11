#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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
                } else {
                    // should execute another program.
                    //TODO: path support
                    if (access(argv[0], X_OK) == 0) {
                        // argv[0] is an execuatable.
                        pid_t rc = fork();
                        if (rc == -1) {
                            //TODO: print error
                            printf("error occured");
                        } else if (rc == 0) {
                            // child
                            rc = execv(argv[0], argv);
                            if (rc == -1) {
                                //TODO: print error
                                printf("error occured");
                            }
                        } else {
                            // parent
                            // rc is the child process id.
                            // parent should wait for the child to finish.
                            if (waitpid(rc, NULL, 0) == -1) {
                                //TODO: print error
                                printf("error occured");
                            }
                        }
                    }
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
