#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void read_text_from_file(const char* fn, char** text) {
    FILE *fp = fopen(fn, "r");

    if (fp == NULL) {
        fprintf(stderr, "%s does not exist.", fn);
        exit(1);
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fprintf(stderr, "fseek failed (END)");
        exit(1);
    }
    long fsize = ftell(fp);
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "fseek failed (SET)");
        exit(1);
    }

    *text = (char*)malloc(fsize);
    fread(*text, 1, fsize, fp);

    fclose(fp);
}

// return the length of addition to the buffer.
int write_int_to_buffer(int n, char* buffer, int index) {
    assert(n > 0);

    // 123 -> num[0] = 3, num[1] = 2, num[2] = 1
    // 10 -> num[0] = 0, num[1] = 1
    uint8_t num[40] = {0};
    int n_index = 0;
    while (n > 0) {
        num[n_index++] = n % 10;
        n /= 10;
    }

    for (int i = n_index - 1; i >= 0; i--) {
        buffer[index++] = num[i] + '0';
    }

    return n_index;
}

// i use char** to pass the address that this function generates.
void compress(char* source) {
    // source : a plain ASCII string
    int n = strlen(source);
    int cnt = 1;
    char prev = source[0];
    for (int i = 1; i < n; i++) {
        char cur = source[i];
        if (prev == cur) {
            cnt++;
        } else {
            fwrite(&cnt, sizeof cnt, 1, stdout);
            fwrite(&prev, sizeof prev, 1, stdout);
            prev = cur;
            cnt = 1;
        }
    }
    fwrite(&cnt, sizeof cnt, 1, stdout);
    fwrite(&prev, sizeof prev, 1, stdout);
}

void print(char* text) {
    // To write out an integer in binary format.
    fwrite(text, sizeof(char), strlen(text), stdout);
}

void compress_file(const char* fn) {
    char *source;
    read_text_from_file(fn, &source);
    compress(source);
}

int main(int argc, char const *argv[]) {
    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i=1; i<argc; i++) {
        compress_file(argv[i]);
    }
    return 0;
}
