// 需要三個函式：

// allocator_init：malloc 一大塊，整塊當成一個大的 FreeBlock 放進 free list
// allocator_alloc：從 free list 找第一個夠大的塊，切出來給呼叫者
// allocator_free：把那塊記憶體變成 FreeBlock，插回 free list 開頭

#include <stdio.h>
#include <stdlib.h>

typedef struct FreeBlock
{
    size_t size;
    struct FreeBlock *next;
} FreeBlock;

typedef struct
{
    char *buffer;
    size_t size;
    FreeBlock *free_list;
} Allocator;

void allocator_init(Allocator *a, size_t size)
{
    a->buffer = malloc(size);
    if (a->buffer == NULL)
    {
        fprintf(stderr, "malloc 失敗\n");
        exit(1);
    }
    a->size = size;
    a->free_list = (FreeBlock *)a->buffer; // 整塊當成一個大的 FreeBlock
    a->free_list->size = size;
    a->free_list->next = NULL;
}

void *allocator_alloc(Allocator *a, size_t size)
{
    FreeBlock *prev = NULL;
    FreeBlock *current = a->free_list;

    while (current)
    {
        if (current->size >= size + sizeof(FreeBlock))
        {
            size_t aligned_size = (size + 7) & ~7;
            char *allocated = (char *)current + sizeof(FreeBlock);
            size_t remaining_size = current->size - aligned_size - sizeof(FreeBlock);

            if (remaining_size > sizeof(FreeBlock))
            {
                FreeBlock *new_block = (FreeBlock *)(allocated + aligned_size);
                new_block->size = remaining_size;
                new_block->next = current->next;

                if (prev)
                {
                    prev->next = new_block;
                }
                else
                {
                    a->free_list = new_block;
                }
            }
            else
            {
                if (prev)
                {
                    prev->next = current->next;
                }
                else
                {
                    a->free_list = current->next;
                }
            }
            return allocated;
        }
        prev = current;
        current = current->next;
    }
    return NULL; // 沒有足夠空間
}

void allocator_free(Allocator *a, void *ptr)
{
    FreeBlock *block = (FreeBlock *)((char *)ptr - sizeof(FreeBlock)); // 找到對應的 FreeBlock
    block->next = a->free_list;                                        // 插回 free list 開頭
    a->free_list = block;
}

int main()
{
    Allocator allocator;
    allocator_init(&allocator, 1024); // 初始化 allocator，分配 1KB 的記憶體

    int *a = (int *)allocator_alloc(&allocator, sizeof(int));
    int *b = (int *)allocator_alloc(&allocator, sizeof(int));
    if (a == NULL || b == NULL)
    {
        fprintf(stderr, "分配失敗\n");
        return 1;
    }

    *a = 42;
    *b = 84;

    typedef struct
    {
        int x;
        int y;
    } Point;

    Point *p = (Point *)allocator_alloc(&allocator, sizeof(Point));
    if (p == NULL)
    {
        fprintf(stderr, "分配失敗\n");
        return 1;
    }
    p->x = 10;
    p->y = 20;

    printf("a: %d\n", *a);
    printf("b: %d\n", *b);
    printf("Point: (%d, %d)\n", p->x, p->y);

    allocator_free(&allocator, a);
    allocator_free(&allocator, b);
    allocator_free(&allocator, p);
    free(allocator.buffer);

    return 0;
}
