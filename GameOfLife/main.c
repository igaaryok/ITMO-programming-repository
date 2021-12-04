#include <stdio.h>
#include <string.h>
#include <stdlib.h>


FILE *inputFile;
FILE *outputFile;
char *outputFilePath;       // Путь до дирректории вывода
int max_iter = -1;          // Максимальное число поколений, по умолчанию "00". Значение "-1" соответствует бесконечности; Указывая "-1", вы рискуете лишьться своего компьютера
short int dump_freq = 1;    // Частота поколений, по умолчанию: "1"
char outputFileName[50];    // Переменная для хранения полного пути вывода
char allPath[100];          // Путь до директории, в которую печатать ответ
int upper;                  // Индекс верхнего соседа
int left;                   // Индекс левого соседа
int lower;                  // Индекс нижнего соседа
int right;                  // Индекс правого соседа
int neighborsCounter;       // Количество соседей клетки


typedef struct {
    char bfType[2];         // Тип файла растрового изображения должен быть BMP (2 байта)
    unsigned int bfSize;    // Размер файла растрового изображения в байтах (4 байта)
    short bfReserved1;      // Зарезервированное слово растрового файла, должно быть 0 (2 байта)
    short bfReserved2;      // Зарезервированное слово растрового файла, должно быть 0 (2 байта)
    int bfOffBits;          // Начальная позиция данных растрового изображения относительно растрового изображения (4 байта)
} BITMAPFILEHEADER;


typedef struct {
    int biSize;            // Количество байтов, занимаемых этой структурой (4 байта)
    int biWidth;           // Ширина растрового изображения в пикселях (4 байта)
    int biHeight;          // Высота растрового изображения в пикселях (4 байта)
    short biPlanes;        // Уровень целевого устройства должен быть 1 (2 байта)
    short biBitCount;      // Количество битов, необходимых для каждого пикселя, должно быть 1 (два цвета), 4 (16 цветов), 8 (256 цветов), 24 (истинный цвет) или 32 (улучшенный истинный цвет) (2 байта)
    int biCompression;     // Тип сжатия растрового изображения должен быть 0 (без сжатия), 1 (BI_RLE8  Тип сжатия) или одно из 2 (тип сжатия BI_RLE4)) (4 байта)
    int biSizeImage;       // Размер растрового изображения в байтах (4 байта)
    int biXPelsPerMeter;   // Разрешение растрового изображения по горизонтали, количество пикселей на метр (4 байта)
    int biYPelsPerMeter;   // Вертикальное разрешение растрового изображения, пикселей на метр (4 байта)
    int biClrUsed;         // Количество цветов в таблице цветов, фактически используемых растровым изображением (4 байта)
    int biClrImportant;    // Количество важных цветов в процессе отображения растрового изображения (4 байта)
} BITMAPINFOHEADER;


typedef struct {
    int rgbBlue;           // Синий
    int rgbGreen;          // Зеленый
    int rgbRed;            // Красный
    int rgbReserved;
} RGBQUAD;


BITMAPFILEHEADER getBmpHeader(FILE *file) {
    BITMAPFILEHEADER bmpHeader;
    fread(&bmpHeader.bfType, 1, 2, file);
    fread(&bmpHeader.bfSize, 4, 1, file);
    fread(&bmpHeader.bfReserved1, 2, 1, file);
    fread(&bmpHeader.bfReserved2, 2, 1, file);
    fread(&bmpHeader.bfOffBits, 4, 1, file);
    return bmpHeader;
}


BITMAPINFOHEADER getBmpInfoHeader(FILE *file) {
    BITMAPINFOHEADER infoHeader;
    fread(&infoHeader.biSize, 4, 1, file);
    fread(&infoHeader.biWidth, 4, 1, file);
    fread(&infoHeader.biHeight, 4, 1, file);
    fread(&infoHeader.biPlanes, 2, 1, file);
    fread(&infoHeader.biBitCount, 2, 1, file);
    fread(&infoHeader.biCompression, 4, 1, file);
    fread(&infoHeader.biSizeImage, 4, 1, file);
    fread(&infoHeader.biXPelsPerMeter, 4, 1, file);
    fread(&infoHeader.biYPelsPerMeter, 4, 1, file);
    fread(&infoHeader.biClrUsed, 4, 1, file);
    fread(&infoHeader.biClrImportant, 4, 1, file);
    return infoHeader;
}


void createOutputBmp(FILE *file, BITMAPFILEHEADER header, BITMAPINFOHEADER infoHeader, RGBQUAD **cells) {
    fwrite(&header.bfType, 1, 2, file);
    fwrite(&header.bfSize, 4, 1, file);
    fwrite(&header.bfReserved1, 2, 1, file);
    fwrite(&header.bfReserved2, 2, 1, file);
    fwrite(&header.bfOffBits, 4, 1, file);
    fwrite(&infoHeader.biSize, 4, 1, file);
    fwrite(&infoHeader.biWidth, 4, 1, file);
    fwrite(&infoHeader.biHeight, 4, 1, file);
    fwrite(&infoHeader.biPlanes, 2, 1, file);
    fwrite(&infoHeader.biBitCount, 2, 1, file);
    fwrite(&infoHeader.biCompression, 4, 1, file);
    fwrite(&infoHeader.biSizeImage, 4, 1, file);
    fwrite(&infoHeader.biXPelsPerMeter, 4, 1, file);
    fwrite(&infoHeader.biYPelsPerMeter, 4, 1, file);
    fwrite(&infoHeader.biClrUsed, 4, 1, file);
    fwrite(&infoHeader.biClrImportant, 4, 1, file);


    for (int i = infoHeader.biHeight - 1; i >= 0; i--) {
        for (int j = 0; j < infoHeader.biWidth; j++) {
            putc(cells[i][j].rgbBlue, file);
            putc(cells[i][j].rgbGreen, file);
            putc(cells[i][j].rgbRed, file);
        }
    }
}


RGBQUAD **readRGBQUAD(FILE *file, BITMAPINFOHEADER infoHeader) {
    RGBQUAD **cell = (RGBQUAD **) malloc(infoHeader.biHeight * sizeof(RGBQUAD *));
    for (int i = 0; i < infoHeader.biHeight; ++i)
        cell[i] = (RGBQUAD *) malloc(infoHeader.biWidth * sizeof(RGBQUAD));
    for (int i = infoHeader.biHeight - 1; i >= 0; i--) {
        for (int j = 0; j < infoHeader.biWidth; j++) {
            cell[i][j].rgbBlue = getc(file);
            cell[i][j].rgbGreen = getc(file);
            cell[i][j].rgbRed = getc(file);
        }
    }
    return cell;
}


RGBQUAD **RGBQUADtoMonochrome(int **in, BITMAPINFOHEADER infoHeader) {
    RGBQUAD **cell = (RGBQUAD **) malloc(infoHeader.biHeight * sizeof(RGBQUAD *));
    for (int i = 0; i < infoHeader.biHeight; i++) {
        cell[i] = (RGBQUAD *) malloc(infoHeader.biWidth * sizeof(RGBQUAD));
    }
    for (int i = infoHeader.biHeight - 1; i >= 0; i--) {
        for (int j = 0; j < infoHeader.biWidth; j++) {
            if (in[i][j] == 1) {
                cell[i][j].rgbBlue = 0;
                cell[i][j].rgbGreen = 0;
                cell[i][j].rgbRed = 0;
            } else {
                cell[i][j].rgbBlue = 255;
                cell[i][j].rgbGreen = 255;
                cell[i][j].rgbRed = 255;
            }
        }
    }
    return cell;
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        char inputFileNameInIf[50];
        char outputFileNameInIf[50];
        char max_iter_if[50];
        char dump_freq_if[50];
        printf("Enter the name(location) of the input bmp file\n");
        scanf("%s", inputFileNameInIf);
        printf("Enter the name(location) of the output bmp file\n");
        scanf("%s", outputFileNameInIf);
        printf("Enter max iter or '-1' to skip\n");
        scanf("%s", max_iter_if);
        printf("Enter dump freq or '-1' to skip\n");
        scanf("%s", dump_freq_if);
        inputFile = fopen(inputFileNameInIf, "r");
        outputFilePath = outputFileNameInIf;
        if (strcmp(max_iter_if, "-1") != 0) {
            max_iter = atoi(max_iter_if);
        }
        if (strcmp(dump_freq_if, "-1") != 0) {
            dump_freq = atoi(dump_freq_if);
        }
    } else {
        int index = 0;
        while (index < argc) {
            if (strcmp(argv[index], "--input") == 0) {
                inputFile = fopen(argv[index + 1], "r");
                index++;
            }
            if (strcmp(argv[index], "--output") == 0) {
                outputFilePath = argv[index + 1];
                index++;
            }
            if (strcmp(argv[index], "--max_iter") == 0) {
                max_iter = atoi(argv[index + 1]);
                index++;
            }
            if (strcmp(argv[index], "--dump_freq") == 0) {
                dump_freq = atoi(argv[index + 1]);
                index++;
            }
            index++;
        }
    }


    if (inputFile == NULL){
        printf("ERROR: input file doesn't exist in this folder");
        return 0;
    }


    BITMAPFILEHEADER bmpHeader = getBmpHeader(inputFile);
    BITMAPINFOHEADER bmpInfoHeader = getBmpInfoHeader(inputFile);
    int **previousGeneration = (int **) malloc(bmpInfoHeader.biHeight * sizeof(int *));
    RGBQUAD **rgb = readRGBQUAD(inputFile, bmpInfoHeader);


    for (int i = 0; i < bmpInfoHeader.biHeight; ++i)
        previousGeneration[i] = (int *) malloc(bmpInfoHeader.biWidth * sizeof(int));
    for (int i = 0; i < bmpInfoHeader.biHeight; i++) {
        for (int j = 0; j < bmpInfoHeader.biWidth; j++) {
            previousGeneration[i][j] = (rgb[i][j].rgbBlue == 0 && rgb[i][j].rgbGreen == 0 && rgb[i][j].rgbRed == 0) ? 1
                                                                                                                    : 0;
        }
    }
    int **newGeneration = (int **) malloc(bmpInfoHeader.biHeight * sizeof(int *));
    for (int i = 0; i < bmpInfoHeader.biHeight; ++i) {
        newGeneration[i] = (int *) malloc(bmpInfoHeader.biWidth * sizeof(int));
    }
    for (int i = 0; i < bmpInfoHeader.biHeight; i++) {
        for (int j = 0; j < bmpInfoHeader.biWidth; j++) {
            newGeneration[i][j] = (rgb[i][j].rgbBlue == 0 && rgb[i][j].rgbGreen == 0 && rgb[i][j].rgbRed == 0) ? 1 : 0;
        }
    }
    for (int k = 0; (k < max_iter) || (max_iter == -1); k++) {
        for (int i = 0; i < bmpInfoHeader.biHeight; i++) {
            upper = i - 1;
            lower = i + 1;
            if (i == 0)
                upper = bmpInfoHeader.biHeight - 1;
            if (i == bmpInfoHeader.biHeight - 1)
                lower = 0;
            for (int j = 0; j < bmpInfoHeader.biWidth; j++) {
                left = j - 1;
                right = j + 1;
                if (j == 0)
                    left = bmpInfoHeader.biWidth - 1;
                if (j == bmpInfoHeader.biWidth - 1)
                    right = 0;
                neighborsCounter = previousGeneration[upper][left] + previousGeneration[upper][j] +
                                   previousGeneration[upper][right] + previousGeneration[i][left] +
                                   previousGeneration[i][right] +
                                   previousGeneration[lower][left] + previousGeneration[lower][j] +
                                   previousGeneration[lower][right];
                if (previousGeneration[i][j] == 1) {
                    if (neighborsCounter < 2 || neighborsCounter > 3)
                        newGeneration[i][j] = 0;
                    else
                        newGeneration[i][j] = 1;
                } else {
                    if (neighborsCounter == 3)
                        newGeneration[i][j] = 1;

                    else
                        newGeneration[i][j] = 0;
                }
            }
        }
        for (int i = 0; i < bmpInfoHeader.biHeight; i++)
            for (int j = 0; j < bmpInfoHeader.biWidth; j++)
                previousGeneration[i][j] = newGeneration[i][j];
        if (k % dump_freq == 0) {
            printf("%d\n", k);
            sprintf(outputFileName, "%d.bmp", k + 1);
            strcpy(allPath, outputFilePath);
            strcat(allPath, outputFileName);
            outputFile = fopen(allPath, "w");
            createOutputBmp(outputFile, bmpHeader, bmpInfoHeader,
                            RGBQUADtoMonochrome(previousGeneration, bmpInfoHeader));
            fclose(outputFile);
            fclose(inputFile);
        }
    }
}
