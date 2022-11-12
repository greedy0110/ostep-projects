#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include "common.h"

const char* PROMPT = "wish> ";
char paths[128][256] = {
    "/bin"
}; // 32KB
int num_path = 1;

void programe_error() {
    // perror("Error: ");
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

// return 0 when the execution has occured.
int execute(char *fullpath, char *argv[], char* red_fn) { //TODO: red_fn must be trimmed.
    if (access(fullpath, X_OK) == 0) {
        // argv[0] is an execuatable.
        pid_t rc = fork();
        FILE *fp = NULL;
        if (rc == -1) {
            // printf("fork error occured");
            // programe_error();
            return 1;
        } else if (rc == 0) {
            if (red_fn != NULL) {
                // printf("ch : %s\n", red_fn);
                if (freopen(red_fn, "w", stdout) == NULL) {
                    // programe_error();
                    return 1;
                }
                if (freopen(red_fn, "w", stderr) == NULL) {
                    // programe_error();
                    return 1;
                }
            }

            // child
            rc = execv(fullpath, argv);

            if (rc == -1) {
                // printf("execv error occured");
                // programe_error();
                return 1;
            }
        } else {
            // parent
            // rc is the child process id.
            // parent should wait for the child to finish.
            if (waitpid(rc, NULL, 0) == -1) {
                // printf("waitpid error occured");
                // programe_error();
                return 1;
            }

            if (fp != NULL)
                fclose(fp);

            return 0; // launch success
        }
    }
    return 1;
}

void parse_command_execute(char *raw_line) {
    // parse
    char *token;
    char *argv[10]; // FIXME: 10?
    int argc = 0;

    while ((token = strsep(&raw_line, ">")) != NULL) { //TODO: too simple
        argv[argc++] = token; 
        // printf("%s\n", token);
    }

    char *command_line = strdup(argv[0]);
    if (command_line == NULL) {
        programe_error();
        return;
    }

    char *red_fn = NULL; // redirection file name
    //TODO: free?
    if (argc == 1) {
        argc = 0;
    } else if (argc == 2) {
        // 2 for redirection
        int argc = 0;
        trim(&red_fn, argv[1]);
        // printf("%s\n", red_fn);

        if (strlen(red_fn) == 0) {
            programe_error();
            return;
        }

        char *trimmed = strdup(red_fn);
        while (strsep(&trimmed, " ") != NULL) { //TODO: too simple
            argc++;
        }
        // printf("%s\n", red_fn);

        if (argc >= 2) {
            programe_error();
            return;
        }
    } else {
        programe_error();
        return;
    }

    argc = 0;

    while ((token = strsep(&command_line, " ")) != NULL) { //TODO: too simple
        argv[argc++] = token; 
    }
    argv[argc] = NULL;
    
    // argv[0] == command, argv[1..] == args

    // builtin command
    if (strcmp(argv[0], "exit") == 0) {
        if (argc != 1) {
            programe_error();
            return;
        } else {
            exit(0);
        }
    } else if (strcmp(argv[0], "cd") == 0) {
        if (argc != 2) {
            // printf("cd error occured");
            programe_error();
            return;
        } else if (chdir(argv[1]) != 0) {
            // printf("chdir error occured");
            programe_error();
            return;
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
        int fail = 1;
        for (i = 0; i<num_path; i++) {
            strcpy(fullpath, paths[i]);
            strcat(fullpath, "/");
            strcat(fullpath, argv[0]);
            if (execute(fullpath, argv, red_fn) == 0) {
                // execution success
                fail = 0;
                break;
            }
        }

        if (fail) {
            programe_error();
            return;
        }
    }
}

int main(int argc, char const *argv[]) {

    FILE *in_fp;
    if (argc == 1) {
        // interact mode
        in_fp = stdin;
    } else if (argc == 2) {
        // batch mode
        in_fp = fopen(argv[1], "r");
        if (in_fp == NULL) {
            // printf("open error occured");
            exit(1);
        }
    } else {
        // wrong
        // printf("argc error occured");
        exit(1);
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen = 0;
    do {
        if (linelen != 0) {
            line[linelen-1] = '\0'; // the return of getline is a new line. so replace it.
            parse_command_execute(line);
        }
        if (argc == 1) {
            // interact mode only
            printf("%s", PROMPT);
        }
    } while ((linelen = getline(&line, &linecap, in_fp)) > 0);
    // getline uses malloc if linep is NULL, and realloc if linep is not null.
    free(line); 

    return 0;
}
