#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int create(FILE *fileArc, FILE *fileAdd, const char *fileName) {
    if (fileAdd == NULL)return 1;
    char symbol = fgetc(fileAdd);
    int fileSize = 0;
    while (!feof(fileAdd)) {
        ++fileSize;
        fgetc(fileAdd);
    }
    fwrite(fileName, 12, 1, fileArc);
    fwrite(&fileSize, sizeof(int), 1, fileArc);
    fseek(fileAdd, 1, SEEK_SET);
    for (int i = 0; i < fileSize; i++) {
        fputc(symbol, fileArc);
        symbol = fgetc(fileAdd);
    }
    fclose(fileAdd);
    return 0;
}


int extract(FILE *fileArc, const char *arcName) {
    if (fileArc == NULL){
        return 1;
    }
    printf("From %s:\n", arcName);
    while (!feof(fileArc)) {
        char fileName[128] = {0};
        int fileSize;
        FILE *outputFile;
        fread(fileName, 12, 1, fileArc);
        if (strstr(fileName, ".") == NULL) {
            return 0;
        }
        fread(&fileSize, 4, 1, fileArc);
        outputFile = fopen(fileName, "wb");
        for (int i = 0; i < fileSize; i++) {
            fputc(fgetc(fileArc), outputFile);
        }
        printf("%s was extracted\n", fileName);
    }
    return 0;
}


void list(FILE *fileArc, const char *arcName){
    printf("List of %s:\n", arcName);
    while (!feof(fileArc)) {
        char fileName[128] = {0};
        int fileSize;
        fread(fileName, 12, 1, fileArc);
        if (strstr(fileName, ".") == NULL) {
            return;
        }
        printf("%s\n", fileName);
        fread(&fileSize, 4, 1, fileArc);
        for (int i = 0; i < fileSize; i++) {
            fseek(fileArc, 1, SEEK_CUR);
        }
    }
}


int main(int argc, char const *argv[]) {
    FILE *arcFile;
    const char *arcName;
    int i = 1;
    while (i < argc) {
        if (!strcmp(argv[i], "--file")) {
            arcName = argv[i + 1];
            ++i;
        }
        if (!strcmp(argv[i], "--create")) {
            arcFile = fopen(arcName, "wb");
            for (int j = i + 1; j < argc; j++) {
                if (strstr(argv[j], ".") == NULL) {
                    break;
                } else {
                    FILE *file = fopen(argv[j], "rb");
                    if (create(arcFile, file, argv[j])) {
                        printf("Can't open file: %s\n", argv[j]);
                        return 1;
                    } else{
                        printf("File %s added to %s\n", argv[j], arcName);
                    }
                }
            }
        }
        if (!strcmp(argv[i], "--extract")) {
            arcFile = fopen(arcName, "rb+");
            if (extract(arcFile, arcName)){
                printf("Can't open arc: %s\n", arcName);
            } else{
                printf("___SUCCESSFUL___\n");
            }
        }
        if (!strcmp(argv[i], "--list")) {
            arcFile = fopen(arcName, "rb+");
            list(arcFile, arcName);
        }
        ++i;
    }
    return 0;
}
