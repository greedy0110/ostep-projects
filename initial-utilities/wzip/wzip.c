#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void read_text_from_file(char* fn, char** text) {
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
void compress(char* source, char** compressed) {
    // source : a plain ASCII string
    int n = strlen(source);
    *compressed = (char*)malloc(sizeof(char) * (n*2 + 1));
    memset(*compressed, 0,sizeof(char) * (2*n + 1));

    int c_index = 0, cnt = 1;
    char prev = source[0];
    for (int i = 1; i < n; i++) {
        char cur = source[i];
        if (prev == cur) {
            cnt++;
        } else {
            // write cnt and prev
            int add = write_int_to_buffer(cnt, *compressed, c_index);
            c_index += add;
            (*compressed)[c_index++] = prev;
            prev = cur;
            cnt = 1;
        }
    }
    // write cnt and prev
    int add = write_int_to_buffer(cnt, *compressed, c_index);
    c_index += add;

    // this doesn't work
    // *compressed[c_index++] = prev;
    // it does work! wtf!
    (*compressed)[c_index++] = prev;
}

void print(char* text) {
    // To write out an integer in binary format.
    fwrite(text, sizeof(char), strlen(text), stdout);
}

void compress_file(char* fn) {
    char *source, *compressed;
    read_text_from_file(fn, &source);
    compress(source, &compressed);
    print(compressed);
}

int main(int argc, char const *argv[]) {
    compress_file("test.txt");
    return 0;
}
