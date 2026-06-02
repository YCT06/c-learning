// 規格：手刻 linked list
// 需要支援這些操作：

// push_back：把一個 int 加到串列尾端
// print：把所有元素印出來
// free_list：釋放整條串列的所有 node

// main 裡面加入 1、2、3、4、5，印出來，然後 free 掉。

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int value;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
} LinkedList;

void init(LinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
}

void push_back(LinkedList *list, int value) {
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "malloc 失敗\n");
        return;
    }
    new_node->value = value;
    new_node->next = NULL;

    if (list->tail == NULL) { // 空串列
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }
}

void print_list(const LinkedList *list) {
    Node *current = list->head;
    while (current != NULL) {
        printf("%d ", current->value);
        current = current->next;
    }
    printf("\n");
}

void free_list(LinkedList *list) {
    Node *current = list->head;
    while (current != NULL) {
        Node *next = current->next;
        free(current);
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
}

int main() {
    LinkedList list;
    init(&list);

    push_back(&list, 1);
    push_back(&list, 2);
    push_back(&list, 3);
    push_back(&list, 4);
    push_back(&list, 5);

    print_list(&list);
    free_list(&list);

    return 0;
}
