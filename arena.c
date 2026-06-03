// 先做最簡單的版本：Arena Allocator
// Arena 的概念極其簡單：

// 一開始跟系統要一大塊記憶體（比如 1MB）
// 維護一個指標指向「目前用到哪裡」
// 每次有人要記憶體，就從那個指標往後切一塊給他，指標往後移
// 不支援個別 free，整塊一起釋放
// 需要三個函式：

// arena_init(Arena *a, size_t size)：跟系統要一塊 size bytes 的記憶體
// arena_alloc(Arena *a, size_t size)：從 arena 切一塊給呼叫者，回傳指標；空間不夠回傳 NULL
// arena_free(Arena *a)：把整塊還給系統

// main 裡面用 arena 配置幾個 int 和一個結構，印出來驗證正確，最後 arena_free。

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *buffer;    // 那塊大記憶體
    size_t size;     // 總大小
    size_t offset;   // 目前用到哪裡
} Arena;

void arena_init(Arena *a, size_t size) {
    a->buffer = malloc(size);
    if (a->buffer == NULL) {
        fprintf(stderr, "malloc 失敗\n");
        exit(1);
    }
    a->size = size;
    a->offset = 0;
}

void *arena_alloc(Arena *a, size_t size) {
    // 先把 offset 對齊到 8 的倍數
    size_t aligned_offset = (a->offset + 7) & ~7;
    
    if (aligned_offset + size > a->size) {
        return NULL;
    }
    
    void *ptr = a->buffer + aligned_offset;
    a->offset = aligned_offset + size;  // 下次從這裡繼續
    return ptr;
}

void arena_free(Arena *a) {
    free(a->buffer); // 把整塊還給系統
    a->buffer = NULL;
    a->size = 0;
    a->offset = 0;
}

typedef struct {
    int x;
    int y;
} Point;

int main() {
    Arena arena;
    arena_init(&arena, 1024); // 初始化 arena，1KB 大小

    int *a = arena_alloc(&arena, sizeof(int));
    int *b = arena_alloc(&arena, sizeof(int));
    Point *p = arena_alloc(&arena, sizeof(Point));

    if (a == NULL || b == NULL || p == NULL) {
        fprintf(stderr, "arena_alloc 失敗\n");
        arena_free(&arena);
        return 1;
    }

    *a = 42;
    *b = 84;
    p->x = 10;
    p->y = 20;

    printf("a: %d\n", *a);
    printf("b: %d\n", *b);
    printf("Point: (%d, %d)\n", p->x, p->y);

    printf("a 的位址: %p\n", (void *)a);
    printf("b 的位址: %p\n", (void *)b);
    printf("p 的位址: %p\n", (void *)p);

    arena_free(&arena); // 釋放整塊記憶體
    return 0;
}
