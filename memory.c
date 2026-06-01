#include <stdio.h>
#include <stdlib.h>

void 函式B(void) {
    int b = 0;
    printf("函式B 的區域變數位址: %p\n", (void *)&b);
}

void 函式A(void) {
    int a = 0;
    printf("函式A 的區域變數位址: %p\n", (void *)&a);
    函式B();
}

int main(void) {
    int m = 0;
    printf("main  的區域變數位址: %p\n", (void *)&m);
    函式A();
    return 0;
}