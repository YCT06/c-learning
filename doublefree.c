#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *p = malloc(sizeof(int));
    *p = 42;
    free(p);
    free(p);  // 同一塊記憶體 free 兩次
    return 0;
}