// 規格：

// 初始容量 4 個 int
// 提供一個 push 函式，可以把 int 加進去
// 容量滿了自動用 realloc 擴大成兩倍
// 最後把所有元素印出來，然後 free 掉

#include <stdio.h>
#include <stdlib.h> 

typedef struct {
    int *data;      // 動態陣列的指標
    size_t size;    // 已使用的元素數量
    size_t capacity; // 總容量
} DynArray;

void init(DynArray *arr) {
    arr->data = malloc(4 * sizeof(int)); // 初始容量為 4
    arr->size = 0;
    arr->capacity = 4;
}

void push(DynArray *arr, int value) {
    if (arr->size >= arr->capacity) {
        size_t new_capacity = arr->capacity * 2;
        int *tmp = realloc(arr->data, new_capacity * sizeof(int));
        if (tmp == NULL) {
            fprintf(stderr, "realloc 失敗\n");
            return;  // arr->capacity 根本還沒改，不用還原
        }
        arr->data = tmp;
        arr->capacity = new_capacity;
    }
    arr->data[arr->size++] = value;
}

void free_array(DynArray *arr) {
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

int main() {
    DynArray arr;
    init(&arr);

    // 加入一些元素
    for (int i = 0; i < 10; i++) {
        push(&arr, i);
    }

    // 印出所有元素
    for (size_t i = 0; i < arr.size; i++) {
        printf("%d ", arr.data[i]);
    }
    printf("\n");

    // 釋放資源
    free_array(&arr);
    return 0;
}
