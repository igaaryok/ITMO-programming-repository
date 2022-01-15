#include <stdio.h>
#include <string.h>
#include <stdlib.h>


const unsigned int BASE = 1000000000;

typedef struct {
    unsigned int list[35];
} uint1024_t;

void rev(const char *const c) {
    if (*c) {
        rev(c + 1);
        putchar(*c);
    }
}

uint1024_t uint1024_t_from_uint(unsigned int x) {
    uint1024_t x_1024_t;
    memset(x_1024_t.list, 0, sizeof(uint1024_t));
    if (x > BASE) {
        x_1024_t.list[34] = x % BASE;
        x_1024_t.list[33] = x / BASE;
    } else {
        x_1024_t.list[34] = x;
    }
    return x_1024_t;
}

void printf_value(uint1024_t x) {
    short numberBegin = 0;
    for (int i = 0; i < 35; ++i) {
        short nullCount = 9;
        if (!numberBegin) {
            if (x.list[i] == 0)continue;
        }
        if (numberBegin) {
            unsigned int j = x.list[i];
            while (j != 0) {
                nullCount--;
                j /= 10;
            }
            while (nullCount != 0) {
                printf("0");
                nullCount--;
            }
        }
        printf("%u", x.list[i]);
        numberBegin = 1;
    }
    if (!numberBegin)printf("0");
    printf("\n");
}

uint1024_t add_op(uint1024_t x, uint1024_t y) {
    uint1024_t sum;
    memset(sum.list, 0, sizeof(sum));
    for (int i = 34; i >= 0; --i) {
        sum.list[i] += (x.list[i] + y.list[i]) % BASE;
        sum.list[i - 1] = (x.list[i] + y.list[i]) / BASE;
    }
    return sum;
}

uint1024_t subtr_op(uint1024_t x, uint1024_t y) {
    uint1024_t subtr;
    memset(subtr.list, 0, sizeof(subtr));
    unsigned int next_digit = 0;
    for (int i = 34; i != -1; i--) {
        unsigned long current_subtr = (x.list[i] + BASE) - (y.list[i] + next_digit);
        subtr.list[i] = current_subtr % BASE;
        if (current_subtr >= BASE) {
            next_digit = 0;
        } else {
            next_digit = 1;
        }
    }
    return subtr;
}

uint1024_t mult_op(uint1024_t num, uint1024_t y) {
    uint1024_t mult;
    memset(mult.list, 0, sizeof(mult));
    for (int i = 34; i >= 0; i--) {
        unsigned long next_digit = 0;
        for (int j = 34; j >= 0; j--) {
            if ((i + j - 34) < 0 || (i + j - 34) > 34) {
                break;
            }
            unsigned long current_mult = mult.list[i + j - 34] + num.list[i] * y.list[j] + next_digit;
            mult.list[i + j - 34] = current_mult % BASE;
            next_digit = current_mult / BASE;
        }
    }
    return mult;
}

void put_digit(uint1024_t *x, char val[], int numbers, int index, int valInd) {
    char digit[numbers];
    for (int i = 0; i < numbers; i++) {
        digit[i] = val[valInd++];
    }
    x->list[index] = atoi(digit);
}


void scanf_value(uint1024_t *x, char val[]) {
    int length = strlen(val);
    int needed = length / 9 + (length % 9 ? 1 : 0);
    int numbers = length % 9 + (!(length % 9) ? 9 : 0);
    put_digit(x, val, numbers, 35 - needed, 0);
    int counter = 0;
    for (int i = 36 - needed; i < 35; i++) {
        put_digit(x, val, 9, i, numbers + (counter * 9));
        counter++;
    }
}


int main(int argc, char *argv[]) {
    uint1024_t x = uint1024_t_from_uint(0);
    uint1024_t y = uint1024_t_from_uint(0);
    uint1024_t z = uint1024_t_from_uint(0);
    scanf_value(&x, argv[1]);
    scanf_value(&y, argv[2]);
    printf("First number from uint1024_t:\n");
    printf_value(x);
    printf("Second number from uint1024_t:\n");
    printf_value(y);
    z = add_op(x, y);
    printf("Summary:\n");
    printf_value(z);
    z = subtr_op(x, y);
    printf("Subtr:\n");
    printf_value(z);
    z = mult_op(x, y);
    printf("Mult:\n");
    printf_value(z);
}