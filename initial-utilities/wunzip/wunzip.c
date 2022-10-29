#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void unzip_single(const char* fn) {
    // read binary content from the file.
    FILE *fp = fopen(fn, "r");
    if (fp == NULL) {
        fprintf(stderr, "%s does not exist.", fn);
        exit(1);
    }

    int cnt;
    char ch;
    while (fread(&cnt, sizeof(int), 1, fp) != 0) { // repeat until it reaches the eof.
        fread(&ch, sizeof(char), 1, fp);
        for (int i = 0; i< cnt; i++) printf("%c", ch);
    }

}

void unzip(int numf, const char* fn[]) {
    for (int i = 0; i < numf; i++) {
        unzip_single(fn[i]);
    }
}

int main(int argc, char const *argv[])
{
    if (argc == 1) {
        printf("wunzip: file1 [file2 ...]\n");
        exit(1);
    }

    unzip(argc-1, argv+1);

    return 0;
}
