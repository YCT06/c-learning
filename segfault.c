#include <stdio.h>

int main(void) {
    int *p = NULL;
    printf("%d\n", *p);  // 對 NULL 指標解參考
    return 0;
}