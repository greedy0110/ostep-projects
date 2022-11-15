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
// int main(int argc, char const *argv[]) {
//     asserttrim("", "");
//     asserttrim("a", "a");
//     asserttrim(" a", "a");
//     asserttrim("a ", "a");
//     asserttrim(" a ", "a");
//     return 0;
// }

void
asserttokenizer(char *raw_input, char *expected[], int expected_cnt) {
    char *token, *string, *tofree, *tokens[128];
    int cnt = 0;
    tofree = string = strdup(raw_input);
    assert(string != NULL);

    while ((token = strsep(&string, " \t\n\v\f\r")) != NULL) {
        // multiple whitespace will show up as multiple empty fields.
        // skip them.
        if (*token == '\0') continue;
        tokens[cnt++] = token;
    }

    assert(cnt == expected_cnt);
    for (int i = 0; i < cnt; i++) {
        assert(strcmp(tokens[i], expected[i]) == 0);
    }

    free(tofree);
}

int 
main(int argc, char const *argv[]) {
    char *expected[] = {"a"};
    asserttokenizer("a", expected, 1);
    asserttokenizer(" a", expected, 1);
    asserttokenizer("   a", expected, 1); // tab
    asserttokenizer("     a     ", expected, 1); // space, tab

    char *expected2[] = {"a", "b"};
    asserttokenizer("a b", expected2, 2);
    asserttokenizer("a  b", expected2, 2);
    asserttokenizer("   a     b    ", expected2, 2);
    return 0;
}
