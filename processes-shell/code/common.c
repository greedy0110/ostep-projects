#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "common.h"

// malloc trimmed str so clean-up is up to the caller.
void trim(char **trimmed, const char *src) {
    assert(*trimmed == NULL);
    int src_len = strlen(src);
    *trimmed = (char*)malloc(sizeof(char)* (src_len + 1));
    int i, ti = 0, si = 0, sri = src_len-1;
    while (si < src_len && isspace(src[si++]));
    // src[si-1] is the first non-whitespace letter or end.
    if (si == src_len && isspace(src[si-1]) == 1) return;

    while (0 <= sri && isspace(src[sri--]));
    // src[sri+1] is the last non-whitespace letter.

    // src[si-1..sri+1] is what we want. (inclusive)
    for (i = si-1; i <= sri+1; i++)
        (*trimmed)[ti++] = src[i];
    (*trimmed)[ti] = '\0';
}

void asserttrim(char *input, const char *expected) {
    char *string = NULL;
    trim(&string, input);
    printf("%s %s\n", string, input);
    assert(strcmp(string, expected) == 0);
    free(string);
}

// test code
int main(int argc, char const *argv[]) {
    asserttrim("", "");
    asserttrim("a", "a");
    asserttrim(" a", "a");
    asserttrim("a ", "a");
    asserttrim(" a ", "a");
    return 0;
}
