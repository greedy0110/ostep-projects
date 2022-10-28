#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void read_text_from_file(char* fn, char* text) {
    FILE *fp = fopen(fn, "r");

    fclose(fp);
}

// i use char** to pass the address that this function generates.
void compress(char* source, char** compressed) {
    // source : a plain ASCII string
    int n = strlen(source);
    *compressed = (char*)malloc(sizeof(char)*n*2);
    memset(*compressed, 0,sizeof(char)*2*n);
}

void print(char* text) {
    // To write out an integer in binary format.
    fwrite(text, sizeof(char), strlen(text), stdout);
}

int main(int argc, char const *argv[]) {
    /* code */

    char* source = "aaaaaaaaaabbbb";
    char* compressed = NULL;

    // what i want here, pass the memory address which allocates in compress function.
    compress(source, &compressed);

    assert(strcmp(compressed, "10a4b") == 0);

    free(compressed);
    return 0;
}
