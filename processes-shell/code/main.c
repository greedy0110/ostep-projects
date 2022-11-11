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

void programe_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

// return 0 when the execution has occured.
int execute(char *fullpath, char *argv[]) {
    if (access(fullpath, X_OK) == 0) {
        // argv[0] is an execuatable.
        pid_t rc = fork();
        if (rc == -1) {
            // printf("fork error occured");
            programe_error();
            return 1;
        } else if (rc == 0) {
            // child
            rc = execv(fullpath, argv);
            if (rc == -1) {
                // printf("execv error occured");
                programe_error();
                return 1;
            }
        } else {
            // parent
            // rc is the child process id.
            // parent should wait for the child to finish.
            if (waitpid(rc, NULL, 0) == -1) {
                // printf("waitpid error occured");
                programe_error();
                return 1;
            }

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
    while ((token = strsep(&raw_line, " ")) != NULL) {
        argv[argc++] = token; 
    }
    argv[argc] = NULL;
    
    // argv[0] == command, argv[1..] == args

    // builtin command
    if (strcmp(argv[0], "exit") == 0) {
        if (argc != 1) {
            programe_error();
        } else {
            exit(0);
        }
    } else if (strcmp(argv[0], "cd") == 0) {
        if (argc != 2) {
            // printf("cd error occured");
            programe_error();
            return;
        }
        if (chdir(argv[1]) != 0) {
            // printf("chdir error occured");
            programe_error();
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
            if (execute(fullpath, argv) == 0) {
                // execution success
                fail = 0;
                break;
            }
        }

        if (fail) {
            programe_error();
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

    return 0;
}
