// 規格：手刻 hash table
// 支援兩個操作：

// insert：插入一個 key-value pair（key 是字串，value 是 int）
// get：用 key 查詢 value，找不到回傳 -1

// 內部用陣列 + linked list 處理碰撞（chaining），初始桶數（bucket）設 8。

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry {
    char *key;
    int value;
    struct Entry *next;  // 同一個桶裡的下一個
} Entry;

typedef struct {
    Entry **buckets;  // 指標陣列，每個桶是一條 linked list
    size_t count;     // 桶的數量
} HashTable;

size_t hash(const char *key, size_t count) {
    size_t h = 0;
    while (*key) {
        h = h * 31 + (unsigned char)*key;
        key++;
    }
    return h % count;
}

void init(HashTable *ht) {
    ht->count = 8; // 初始桶數
    ht->buckets = calloc(ht->count, sizeof(Entry*)); // 初始化桶陣列
}

void insert(HashTable *ht, const char *key, int value) {
    size_t index = hash(key, ht->count);
    Entry *current = ht->buckets[index];
 
    // 檢查是否已經有這個 key
    while (current) {
        if (strcmp(current->key, key) == 0) {
            current->value = value; // 更新值
            return;
        }
        current = current->next;
    }

    // 沒有找到，插入新 entry
    Entry *new_entry = malloc(sizeof(Entry));
    if (new_entry == NULL) {
        fprintf(stderr, "malloc 失敗\n");
        return;
    }
    new_entry->key = strdup(key); // 複製 key 字串
    new_entry->value = value;
    new_entry->next = ht->buckets[index]; // 插入到桶的頭部
    ht->buckets[index] = new_entry;
}

int get(const HashTable *ht, const char *key) {
    size_t index = hash(key, ht->count);
    Entry *current = ht->buckets[index];

    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value; // 找到 key，回傳值
        }
        current = current->next;
    }
    return -1; // 沒有找到
}
  
void free_table(HashTable *ht) {
    for (size_t i = 0; i < ht->count; i++) {
        Entry *current = ht->buckets[i];
        while (current) {
            Entry *next = current->next;
            free(current->key); // 釋放 key 字串
            free(current);      // 釋放 entry
            current = next;
        }
    }
    free(ht->buckets); // 釋放桶陣列
}

int main() {
    HashTable ht;
    init(&ht);

    insert(&ht, "apple", 1);
    insert(&ht, "banana", 2);
    insert(&ht, "orange", 3);

    printf("apple: %d\n", get(&ht, "apple"));   // 輸出 1
    printf("banana: %d\n", get(&ht, "banana")); // 輸出 2
    printf("grape: %d\n", get(&ht, "grape"));   // 輸出 -1

    free_table(&ht);
    return 0;
}
