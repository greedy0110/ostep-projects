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

// test code
// int 
// main(int argc, char const *argv[]) {
//     char *expected[] = {"a"};
//     asserttokenizer("a", expected, 1);
//     asserttokenizer(" a", expected, 1);
//     asserttokenizer("   a", expected, 1); // tab
//     asserttokenizer("     a     ", expected, 1); // space, tab

//     char *expected2[] = {"a", "b"};
//     asserttokenizer("a b", expected2, 2);
//     asserttokenizer("a  b", expected2, 2);
//     asserttokenizer("   a     b    ", expected2, 2);
//     return 0;
// }

// int 
// tokenize_single_command_with_red(char *cmd, char ***ret_tokens, int *cnt_tokens) {
//     // 1. split by ">"
//     // 2. split by whitespaces

//     int cnt = 0;
//     char **tokens = (char**)malloc(sizeof(char*) * 128);
//     if (tokens == NULL) return 1;

//     // 1. find the locate of ">".
//     // 2. split by whitespace and add tokens until that pointer.
//     // 3. add the token
//     // 4. repeat from the pointer.

//     char *redirect = NULL;
//     int pos = 0;
//     while ((redirect = strstr(cmd + pos, ">")) != NULL) {
        
//     }

//     while ((token = strsep(&string, ">")) != NULL) {
//         // multiple whitespace will show up as multiple empty fields.
//         // skip them.
//         if (*token == '\0') continue;
//         tokens[cnt++] = token;
//     }

//     return 0;
// }

void
assert_single_command_parsing(char *raw_input, char *expected[], int expected_cnt) {
    printf("test: %s\n", raw_input);

    char *token, *string, *tofree, *tokens[128];
    int cnt = 0;
    tofree = string = strdup(raw_input);
    assert(string != NULL);

    // ">" -> ""
    // "ls>" -> "ls", "" 
    // "ls" -> "ls" 
    // "ls>ls" -> "ls", "ls" 
    // ">>" -> "", ""
    while ((token = strsep(&string, ">")) != NULL) {
        printf("token: %s %d\n", token, cnt);
        // multiple whitespace will show up as multiple empty fields.
        // skip them.
        if (*token == '\0') {
            if (string != NULL) tokens[cnt++] = ">";
            continue;
        } else {
            char *inner;
            while ((inner = strsep(&token, " \t\n\v\f\r")) != NULL) {
                // multiple whitespace will show up as multiple empty fields.
                // skip them.
                if (*inner == '\0') continue;
                tokens[cnt++] = inner;
            }
            // add ">" if there is the next.
            if (string != NULL) {
                tokens[cnt++] = ">";
            }
        }
    }

    printf("expected : %d %d\n", cnt, expected_cnt);
    assert(cnt == expected_cnt);
    for (int i = 0; i < cnt; i++) {
        assert(strcmp(tokens[i], expected[i]) == 0);
    }

    free(tofree);
}

int 
main(int argc, char const *argv[]) {
    char *expected[] = {"ls", "-la", "/tmp", ">", "output"};
    assert_single_command_parsing("ls -la /tmp > output", expected, 5);
    assert_single_command_parsing("ls -la /tmp>output", expected, 5);

    char *expected2[] = {">", "output"};
    assert_single_command_parsing(">output", expected2, 2);
    char *expected3[] = {">"};
    assert_single_command_parsing(">", expected3, 1);
    char *expected4[] = {">", ">"};
    assert_single_command_parsing(">>", expected4, 2);
    char *e6[] = {">", ">", ">"};
    assert_single_command_parsing(">>>", e6, 3);

    char *expected5[] = {"ls", "-la", "/tmp"};
    assert_single_command_parsing("ls -la /tmp", expected5, 3);
    return 0;
}