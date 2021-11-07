#include <stdio.h>
#include <string.h>
#include <ctype.h>

int lines(char *location) {
    FILE *file = fopen(location, "r");
    int line_count = 1;
    while (!feof(file)) {
        if (fgetc(file) == '\n') line_count++;
    }
    return line_count;

}


int bytes(char *location) {
    FILE *file = fopen(location, "rb");
    int bin_count = 0;
    while (fgetc(file) != EOF) {
        bin_count++;
    }
    return bin_count;


}

int words(char *location) {
    FILE *file = fopen(location, "rb");
    int word_count = 0;
    char first_read_letter = '\0';
    while (!feof(file)) {
        char second_read_letter = fgetc(file);
        if (!isspace(second_read_letter)) {
            if (first_read_letter == '\0' || isspace(first_read_letter)) word_count++;
        }
        first_read_letter = second_read_letter;
    }
    return word_count;
}


int main(int argc, char **argv) {
    if (argc == 3) {
        FILE *file = fopen(argv[2], "r");
        if (file == 0) {
            printf("Warning: no file '%s'", argv[2]);
        } else if ((strcmp(argv[1], "--lines") == 0) || (strcmp(argv[1], "-l") == 0)) {
            printf("Number of lines: %d", lines(argv[2]));

        } else if ((strcmp(argv[1], "--bytes") == 0) || (strcmp(argv[1], "-c") == 0)) {
            printf("File size: %d bytes", bytes(argv[2]));
        } else if ((strcmp(argv[1], "--words") == 0) || (strcmp(argv[1], "-w") == 0)) {
            printf("Number of words: %d", words(argv[2]));
        } else {
            printf("Wrong format: [--lines/--words/--bytes] [File name]");
        }
    } else {
        printf("Wrong format: [--lines/--words/--bytes] [File name]");
    }

    return 0;
}

