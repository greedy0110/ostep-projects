#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void wcat_single_file(char const * fname) {
    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        printf("wcat: cannot open file\n");
        exit(1);
    }

    char buf[1024] = {0};
    while (fgets(buf, 1024, fp) != NULL) {
        printf("%s", buf);
    }

    fclose(fp);
}

int main(int argc, char const *argv[])
{
    if (argc == 1) {
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        wcat_single_file(argv[i]);
    }

    return 0;
}
