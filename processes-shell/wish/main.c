#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

const char* PROMPT = "wish> ";
char paths[128][256] = {
    "/bin"
}; // 32KB
int num_path = 1;

// return 0 when the execution has occured.
int execute(char *fullpath, char *argv[]) {
    if (access(fullpath, X_OK) == 0) {
        // argv[0] is an execuatable.
        pid_t rc = fork();
        if (rc == -1) {
            //TODO: print error
            printf("fork error occured");
            return 1;
        } else if (rc == 0) {
            // child
            rc = execv(fullpath, argv);
            if (rc == -1) {
                //TODO: print error
                printf("execv error occured");
                return 1;
            }
        } else {
            // parent
            // rc is the child process id.
            // parent should wait for the child to finish.
            if (waitpid(rc, NULL, 0) == -1) {
                //TODO: print error
                printf("waitpid error occured");
            }

            return 0; // launch success
        }
    }
    return 1;
}

int main(int argc, char const *argv[]) {

    if (argc == 1) {
        // interact mode
        char *line = NULL;
        size_t linecap = 0;
        ssize_t linelen = 0;
        do {
            if (linelen != 0) {
                line[linelen-1] = '\0'; // the return of getline is a new line. so replace it.

                // parse
                char *token;
                char *argv[10]; // FIXME: 10?
                int argc = 0;
                while ((token = strsep(&line, " ")) != NULL) {
                    argv[argc++] = token; 
                }
                argv[argc] = NULL;
                
                // argv[0] == command, argv[1..] == args

                // builtin command
                if (strcmp(argv[0], "exit") == 0) {
                    exit(0);
                } else if (strcmp(argv[0], "cd") == 0) {
                    if (argc != 2) {
                        //TODO: print error
                        printf("cd error occured");
                        continue;
                    }
                    if (chdir(argv[1]) != 0) {
                        //TODO: print error
                        printf("chdir error occured");
                    }
                } else if (strcmp(argv[0], "path") == 0) {
                    int i;
                    for (i = 1; i < argc; i++) {
                        strcpy(paths[i-1], argv[i]);
                    } 
                    num_path = argc - 1;
                } else {
                    // should execute another program.
                    // path support
                    char fullpath[1024];
                    int i;
                    for (i = 0; i<num_path; i++) {
                        strcpy(fullpath, paths[i]);
                        strcat(fullpath, "/");
                        strcat(fullpath, argv[0]);
                        if (execute(fullpath, argv) == 0) {
                            // execution success
                            break;
                        }
                    }
                }
            }
            printf("%s", PROMPT);
        } while ((linelen = getline(&line, &linecap, stdin)) > 0);
    } else if (argc == 2) {
        // batch mode
        //TODO:
    } else {
        // wrong
        //TODO:
        exit(1);
    }

    return 0;
}
