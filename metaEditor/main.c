#include "stdlib.h"
#include "stdio.h"
#include "string.h"

size_t sizeOfFile(const char *file_name) {
    size_t fileSize = 0;
    FILE *mp3File = fopen(file_name, "rb");
    if (mp3File == NULL)
        fileSize = -1;
    else {
        fseek(mp3File, 0, SEEK_END);
        fileSize = ftell(mp3File);
        fclose(mp3File);
    }
    return fileSize;
}

void show(char *file) {
    char temp[10];
    FILE *mp3File = fopen(file, "rb");
    fread(temp, 1, 10, mp3File);
    char *bites = (char *) malloc(sizeof(char) * sizeOfFile(file));
    fread(bites, 1, sizeOfFile(file) - 10, mp3File);
    int i = 0;
    while (bites[i] != 0) {
        char name[5] = {bites[i], bites[i + 1], bites[i + 2], bites[i + 3], '\0'};
        printf("%s: ", name);
        i += 4;
        int size = bites[i] * 256 * 256 * 256 + bites[i + 1] * 256 * 256 + bites[i + 2] * 256 + bites[i + 3];
        i += 6;
        for (int j = 0; j < size; j++, i++)
            printf("%c", bites[i]);
        printf("\n");
    }
    fclose(mp3File);
    free(bites);
}

void get(char *file, char frame[5]) {
    char *bites = (char *) malloc(sizeof(char) * (sizeOfFile(file) + 1));
    FILE *mp3File = fopen(file, "rb");
    fread(bites, 1, sizeOfFile(file), mp3File);
    int i = 10;
    while (bites[i] != 0) {
        int size = 0;
        if (bites[i] == frame[0] && bites[i + 1] == frame[1] && bites[i + 2] == frame[2] && bites[i + 3] == frame[3]) {
            printf("%c", bites[i]);
            printf("%c", bites[i + 1]);
            printf("%c", bites[i + 2]);
            printf("%c: ", bites[i + 3]);
            i += 4;
            size = bites[i] * 256 * 256 * 256 + bites[i + 1] * 256 * 256 + bites[i + 2] * 256 + bites[i + 3];
            i += 6;
            for (int j = 0; j < size; i++, j++)
                printf("%c", bites[i]);
            break;
        } else {
            i += 4;
            int size = bites[i] * 256 * 256 * 256 + bites[i + 1] * 256 * 256 + bites[i + 2] * 256 + bites[i + 3];
            i += size + 6;
        }
    }
    fclose(mp3File);
    free(bites);
}

void set(char *file, char frame[5], char *value) {
    FILE *fmp3File = fopen(file, "rb");
    char *bites = (char *) malloc(sizeof(char) * sizeOfFile(file));
    fread(bites, 1, sizeOfFile(file), fmp3File);
    for (int i = 0; i < sizeOfFile(file) - 4; i++) {
        if (bites[i] == frame[0] && bites[i + 1] == frame[1] && bites[i + 2] == frame[2] && bites[i + 3] == frame[3]) {
            FILE *newMp3File = fopen("temp.mp3", "ab");
            fwrite(bites, 1, i + 10, newMp3File);
            i += 7;
            int size = bites[i];
            fwrite(value, 1, size, newMp3File);
            fseek(fmp3File, i + size + 3, SEEK_SET);
            char temp[1000] = {0};
            int cnt;
            while (!feof(fmp3File)) {
                cnt = fread(&temp, 1, 1000 * sizeof(char), fmp3File);
                fwrite(&temp, 1, cnt * sizeof(char), newMp3File);
            }
            fclose(fmp3File);
            fclose(newMp3File);
            newMp3File = fopen("temp.mp3", "rb");
            fmp3File = fopen(file, "wb");
            fseek(newMp3File, 0, SEEK_SET);
            while (!feof(newMp3File)) {
                cnt = fread(&temp, 1, 1000 * sizeof(char), newMp3File);
                fwrite(&temp, 1, cnt * sizeof(char), fmp3File);
            }
            fclose(newMp3File);
            remove("temp.mp3");
            break;
        }
    }
    free(bites);
}

int main(int argc, char *argv[]) {
    int isShow = 0;
    int isGet = 0;
    int isSet = 0;
    char frame[5] = {0};
    char *value = (char *) malloc(sizeof(char) * 60);
    for (int i = 0; i < 60; i++)
        value[i] = 0;
    char *file = (char *) malloc(sizeof(char) * 9);
    for (int i = 0; i < argc; i++) {
        if (argv[i][2] == 'f') {
            for (int j = 0; j < 8; j++)
                file[j] = argv[i][j + 11];
            file[8] = '\0';
        }
        if (!strcmp("--show", argv[i]))
            isShow = 1;
        if (argv[i][2] == 'g') {
            isGet = 1;
            file[8] = '\0';
            for (int j = 0; j < 4; j++)
                frame[j] = argv[i][j + 6];
            frame[4] = '\0';
        }
        if (argv[i][2] == 's' && argv[i][3] == 'e') {
            isSet = 1;
            for (int j = 0; j < 4; j++)
                frame[j] = argv[i][j + 6];
            frame[4] = '\0';
            int c = 8;
            for (int j = 0; j < strlen(argv[i + 1]); j++, c++) {
                if (argv[i + 1][c] >= 'a' && argv[i + 1][c] <= 'z' || argv[i + 1][c] >= 'A' && argv[i + 1][c] <= 'Z'
                    || argv[i + 1][c] >= '0' && argv[i + 1][c] <= '9' || argv[i][c] == ' ')
                    value[j] = argv[i + 1][c];
                else break;
            }
            value[c] = '\0';
        }
    }
    if (isShow) show(file);
    if (isGet) get(file, frame);
    if (isSet) set(file, frame, value);
    return 0;
}