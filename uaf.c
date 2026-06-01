#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *p = malloc(sizeof(int));
    *p = 42;
    printf("free 之前: %d\n", *p);

    free(p);

    printf("free 之後: %d\n", *p);  // use-after-free
    return 0;
}
