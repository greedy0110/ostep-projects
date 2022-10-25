#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void grep_single_fd(char const *term, FILE *fp) {
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
        if (strstr(line, term) != NULL) {
            fwrite(line, linelen, 1, stdout);
        }
    }
}

void grep_single_file(char const *term, char const *fn) {
    FILE *fp = fopen(fn, "r");
    if (fp == NULL) {
        printf("wgrep: cannot open file\n");
        exit(1);
    }

    grep_single_fd(term, fp);
}

int main(int argc, char const *argv[])
{
    if (argc == 1) {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }

    if (argc == 2) {
        grep_single_fd(argv[1], stdin);
    }

    for (int i = 2; i < argc; i++) {
        grep_single_file(argv[1], argv[i]);
    }

    /* code */
    return 0;
}
