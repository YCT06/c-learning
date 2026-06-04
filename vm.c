#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// 指令集
typedef enum
{
    OP_CONSTANT, // 把一個常數放進 stack
    OP_ADD,      // 加法
    OP_SUBTRACT, // 減法
    OP_MULTIPLY, // 乘法
    OP_DIVIDE,   // 除法
    OP_NEGATE,   // 取負數
    OP_RETURN,   // 結束執行，印出結果
} OpCode;

// bytecode 陣列
typedef struct
{
    uint8_t *code;     // 指令序列
    double *constants; // 常數表
    int count;         // 指令數量
    int capacity;      // 容量
    int const_count;   // 常數數量
} Chunk;

// stack
#define STACK_MAX 256

// 物件型別
typedef enum
{
    OBJ_STRING,
} ObjType;

// 所有物件的共同 header
typedef struct Obj
{
    ObjType type;
    int marked;       // GC 用：這個物件有沒有被標記
    struct Obj *next; // 所有物件串成一條鏈，GC 用來追蹤
} Obj;

// 字串物件
typedef struct
{
    Obj obj; // 必須放第一個，這樣可以安全地在 Obj* 和 ObjString* 之間轉換
    char *chars;
    int length;
} ObjString;

typedef struct
{
    Chunk *chunk;
    uint8_t *ip;
    double stack[STACK_MAX];
    double *stack_top;
    Obj *objects; // 新增：所有物件的鏈
} VM;

// Chunk 的初始化、寫入指令、加入常數、釋放記憶體

void chunk_init(Chunk *chunk)
{
    chunk->code = NULL;
    chunk->constants = NULL;
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->const_count = 0;
}

void chunk_write(Chunk *chunk, uint8_t byte)
{
    if (chunk->count >= chunk->capacity)
    {
        chunk->capacity = chunk->capacity < 8 ? 8 : chunk->capacity * 2;
        chunk->code = realloc(chunk->code, chunk->capacity);
    }
    chunk->code[chunk->count++] = byte;
}

int chunk_add_constant(Chunk *chunk, double value)
{
    chunk->constants = realloc(chunk->constants,
                               (chunk->const_count + 1) * sizeof(double));
    chunk->constants[chunk->const_count] = value;
    return chunk->const_count++; // 回傳這個常數的 index
}

void chunk_free(Chunk *chunk)
{
    free(chunk->code);
    free(chunk->constants);
    chunk_init(chunk);
}

// VM 的初始化和 stack 操作

void vm_init(VM *vm, Chunk *chunk)
{
    vm->chunk = chunk;
    vm->ip = chunk->code;      // ip 指向第一條指令
    vm->stack_top = vm->stack; // stack 從底部開始
    vm->objects = NULL;        // 初始化物件鏈
}

void push(VM *vm, double value)
{
    *vm->stack_top = value;
    vm->stack_top++;
}

double pop(VM *vm)
{
    vm->stack_top--;
    return *vm->stack_top;
}

// 建立字串物件，並把它加入物件鏈

ObjString *new_string(VM *vm, const char *chars, int length)
{
    ObjString *string = malloc(sizeof(ObjString));
    string->obj.type = OBJ_STRING;
    string->obj.marked = 0;
    string->chars = malloc(length + 1);
    memcpy(string->chars, chars, length);
    string->chars[length] = '\0';
    string->length = length;

    // 插入物件鏈開頭
    string->obj.next = vm->objects;
    vm->objects = (Obj *)string;

    return string;
}

// 實作GC

// Mark 階段：標記 stack 上的所有字串物件
void mark(VM *vm)
{
    (void)vm;
    // 這個簡化版只標記 stack 上的物件
    // 真實 GC 還要標記全域變數、upvalue 等
    // 我們的 stack 目前放的是 double，先印個訊息示意
    printf("[GC] mark 階段\n");
}

// Sweep 階段：釋放未標記的物件
void sweep(VM *vm)
{
    Obj **object = &vm->objects;
    while (*object != NULL)
    {
        if (!(*object)->marked)
        {
            // 這個物件沒有被標記，釋放它
            Obj *unreached = *object;
            *object = unreached->next; // 從鏈上移除

            // 根據型別釋放
            if (unreached->type == OBJ_STRING)
            {
                ObjString *str = (ObjString *)unreached;
                printf("[GC] 釋放字串: %s\n", str->chars);
                free(str->chars);
                free(str);
            }
        }
        else
        {
            // 還活著，清除標記，準備下次 GC
            (*object)->marked = 0;
            object = &(*object)->next;
        }
    }
}

void collect_garbage(VM *vm)
{
    printf("[GC] 開始回收\n");
    mark(vm);
    sweep(vm);
    printf("[GC] 回收完成\n");
}

// 執行迴圈

void vm_run(VM *vm)
{
    while (1)
    {
        printf("stack: [ ");
        for (double *slot = vm->stack; slot < vm->stack_top; slot++)
        {
            printf("%g ", *slot);
        }
        printf("]\n");

        uint8_t instruction = *vm->ip++; // 讀指令，ip 往後移

        switch (instruction)
        {
        case OP_CONSTANT:
        {
            uint8_t index = *vm->ip++; // 讀常數的 index
            double value = vm->chunk->constants[index];
            push(vm, value);
            break;
        }
        case OP_ADD:
        {
            double b = pop(vm);
            double a = pop(vm);
            push(vm, a + b);
            break;
        }
        case OP_SUBTRACT:
        {
            double b = pop(vm);
            double a = pop(vm);
            push(vm, a - b);
            break;
        }
        case OP_MULTIPLY:
        {
            double b = pop(vm);
            double a = pop(vm);
            push(vm, a * b);
            break;
        }
        case OP_DIVIDE:
        {
            double b = pop(vm);
            double a = pop(vm);
            push(vm, a / b);
            break;
        }
        case OP_NEGATE:
        {
            push(vm, -pop(vm));
            break;
        }
        case OP_RETURN:
        {
            printf("結果: %g\n", pop(vm));
            return;
        }
        }
    }
}

// main 裡面手動寫 bytecode

int main(void)
{
    Chunk chunk;
    chunk_init(&chunk);

    // 建立常數
    int c1 = chunk_add_constant(&chunk, 1);
    int c2 = chunk_add_constant(&chunk, 2);
    int c3 = chunk_add_constant(&chunk, 3);

    // 寫入 bytecode
    chunk_write(&chunk, OP_CONSTANT);
    chunk_write(&chunk, c1);
    chunk_write(&chunk, OP_CONSTANT);
    chunk_write(&chunk, c2);
    chunk_write(&chunk, OP_CONSTANT);
    chunk_write(&chunk, c3);
    chunk_write(&chunk, OP_MULTIPLY);
    chunk_write(&chunk, OP_ADD);
    chunk_write(&chunk, OP_RETURN);

    VM vm;
    vm_init(&vm, &chunk);

    // 建立幾個字串物件
    ObjString *s1 = new_string(&vm, "hello", 5);
    ObjString *s2 = new_string(&vm, "world", 5);
    ObjString *s3 = new_string(&vm, "gc test", 7);

    printf("建立了三個字串: %s, %s, %s\n", s1->chars, s2->chars, s3->chars);

    // 假設 s2 還在用（手動標記），s1 和 s3 沒有人用
    s2->obj.marked = 1;

    // 觸發 GC
    collect_garbage(&vm);

    // 跑 VM
    vm_run(&vm);

    // 清理剩下的物件
    collect_garbage(&vm);

    chunk_free(&chunk);
    return 0;
}
