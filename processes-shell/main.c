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
int execute(char *fullpath, int argc, char *argv[], char* red_fn) { //TODO: red_fn must be trimmed.
    if (access(fullpath, X_OK) == 0) {
        // argv[0] is an execuatable.
        pid_t rc = fork();
        FILE *fp = NULL;
        if (rc == -1) {
            return 1;
        } else if (rc == 0) {
            if (red_fn != NULL) {
                if (freopen(red_fn, "w", stdout) == NULL) {
                    return 1;
                }
                if (freopen(red_fn, "w", stderr) == NULL) {
                    return 1;
                }
            }

            argv[argc] = NULL;
            // child
            rc = execv(fullpath, argv);

            if (rc == -1) {
                return 1;
            }
        } else {
            // parent
            // rc is the child process id.
            // parent should wait for the child to finish.
            if (waitpid(rc, NULL, 0) == -1) {
                return 1;
            }

            if (fp != NULL)
                fclose(fp);

            return 0; // launch success
        }
    }
    return 1;
}

int
vaild_redirection(char **tokens, int cnt_tokens, char **red_fn) {
    // - the number of ">" tokens MUST be 0 or 1.
    // - if ">" token presents.
    //   - left part MUST be provided.
    //   - right part MUST be provided. 
    //   - the size of the right part MUST be 1. (the redirection file name)
    // execute the left part. if the right part presents, the result of the execution should redriect.

    int cnt_red_sym = 0;
    int red_index = -1;
    for (int i=0; i<cnt_tokens; i++) {
        if (strcmp(tokens[i], ">") == 0) {
            cnt_red_sym++;
            red_index = i;
        }
    }

    if (cnt_red_sym == 0) {
        red_fn = NULL;
        return 0;
    }
    if (cnt_red_sym > 1) return 1; // which is invalid.

    if (red_index == 0) return 1; // red symbol can't place firstly.
    if (red_index == cnt_tokens-1) return 1; // can't place lastly neither.
    if (red_index != cnt_tokens-2) return 1; // the size of right part must be 1.

    *red_fn = tokens[cnt_tokens-1];

    return 0;
}

// raw_line is the one executable command. it may include redirection ('>').
// e.g. "echo hello", "echo hello > output" ...
int 
parse_command_execute(char *raw_line) {
    // e.g. ls -la /tmp > output
    // split raw input into tokens. ["ls", "-la", "/tmp", ">", "output"]
    // evaluate redirection syntax is valid.

    char **tokens = NULL;
    int cnt_tokens = 0;

    if (tokenize_single_command(raw_line, &tokens, &cnt_tokens) != 0)
        return 1;

    if (cnt_tokens == 0) // ignore an empty command
        return 0;

    char *red_fn = NULL;
    if (vaild_redirection(tokens, cnt_tokens, &red_fn) != 0)
        return 1;

    int exe_argc = 0;
    if (red_fn == NULL) exe_argc = cnt_tokens;
    else exe_argc = cnt_tokens - 2;

    // builtin command
    if (strcmp(tokens[0], "exit") == 0) {
        if (exe_argc != 1) {
            return 1;
        } else {
            exit(0);
        }
    } else if (strcmp(tokens[0], "cd") == 0) {
        if (exe_argc != 2) {
            return 1;
        } else if (chdir(tokens[1]) != 0) {
            return 1;
        }
    } else if (strcmp(tokens[0], "path") == 0) {
        int i;
        for (i = 1; i < exe_argc; i++) {
            strcpy(paths[i-1], tokens[i]);
        } 
        num_path = exe_argc - 1;
    } else {
        // should execute another program.
        // path support
        char fullpath[1024];
        int i;
        int fail = 1;
        for (i = 0; i<num_path; i++) {
            strcpy(fullpath, paths[i]);
            strcat(fullpath, "/");
            strcat(fullpath, tokens[0]);
            if (execute(fullpath, exe_argc, tokens, red_fn) == 0) {
                // execution success
                fail = 0;
                break;
            }
        }

        if (fail) {
            return 1;
        }
    }

    return 0;
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
            programe_error();
            exit(1);
        }
    } else {
        // wrong
        // printf("argc error occured");
        programe_error();
        exit(1);
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen = 0;
    do {
        if (linelen != 0) {
            line[linelen-1] = '\0'; // the return of getline is a new line. so replace it.

            // split '&' and parse and execute
            char *single_cmd, *indicator, *tofree;
            tofree = indicator = strdup(line);
            if (indicator == NULL) {
                programe_error();
                continue;
            }

            pid_t children[10];
            int cnt_children = 0;
            int cnt_cmd = 0;
            memset(children, 0, sizeof(pid_t)*10);
            while ((single_cmd = strsep(&indicator, "&")) != NULL) {
                cnt_cmd++;
                if (*single_cmd == '\0') continue;
                if (indicator == NULL && cnt_cmd == 1) {
                    // no parallel.
                    if (parse_command_execute(single_cmd) != 0) {
                        programe_error();
                    }
                    break;
                }

                pid_t child = fork();
                if (child == -1) {
                    programe_error();
                } else if (child == 0) {
                    // the child process
                    if (parse_command_execute(single_cmd) != 0) {
                        programe_error();
                        exit(1);
                    }
                    exit(0); // the child should end.
                } else {
                    children[cnt_children++] = child;
                }
            }

            for (int i=0; i<cnt_children; i++) {
                if (waitpid(children[i], NULL, 0) == -1) {
                    programe_error();
                    break;
                }
            }

            free(tofree);
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
