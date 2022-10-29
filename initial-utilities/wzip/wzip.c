#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void aggregate_files_content(int num_files, const char* fn[], char **text) {
    FILE *fps[num_files];
    long fsizes[num_files];
    long total_size = 0;
    for (int i = 0; i<num_files; i++) {
        fps[i] = fopen(fn[i], "r");
        if (fps[i] == NULL) {
            fprintf(stderr, "%s does not exist.", fn[i]);
            exit(1);
        }

        if (fseek(fps[i], 0, SEEK_END) != 0) {
            fprintf(stderr, "fseek failed (END)");
            exit(1);
        }
        fsizes[i] = ftell(fps[i]);
        total_size += fsizes[i];
        if (fseek(fps[i], 0, SEEK_SET) != 0) {
            fprintf(stderr, "fseek failed (SET)");
            exit(1);
        }
    }

    *text = (char*)malloc(total_size);
    long offset = 0;

    for (int i = 0; i<num_files; i++) {
        fread(*text + offset, 1, fsizes[i], fps[i]);
        fclose(fps[i]);
        offset += fsizes[i];
    }
}

int main(int argc, char const *argv[]) {
    if (argc == 1) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    char *source;
    //FIXME: split the array to argv[1..] to only pass the files list.
    // Is it a good way to do it?
    aggregate_files_content(argc-1, argv + 1, &source);
    compress(source);

    return 0;
}
