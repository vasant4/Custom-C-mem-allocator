#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;


void insert_head(Node** head, int val) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    
    new_node->data = val;
    new_node->next = *head;
    *head = new_node;
}

void delete_by_value(Node** head, int val) {
    Node** current = head;

    
    while (*current != NULL) {
        if ((*current)->data == val) {
            Node* to_delete = *current;
            *current = (*current)->next; 
            free(to_delete);
            return;
        }
        current = &(*current)->next;
    }
}

void print(Node* head) {
    Node* current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

int main(void) {
    Node* head = NULL;

    printf("Initial empty list:\n");
    print(head);
    delete_by_value(&head, 99);

    insert_head(&head, 30);
    insert_head(&head, 20);
    insert_head(&head, 10);
    
    printf("\nAfter inserting 10, 20, 30 at the head:\n");
    print(head);

    printf("\nDeleting 20 (middle):\n");
    delete_by_value(&head, 20);
    print(head);

    printf("\nDeleting 10 (head):\n");
    delete_by_value(&head, 10);
    print(head);

    printf("\nDeleting 99 (absent):\n");
    delete_by_value(&head, 99);
    print(head);

    delete_by_value(&head, 30);

    return 0;
}