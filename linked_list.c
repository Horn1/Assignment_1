#include "linked_list.h"
#include "memory_manager.h"
#include <stdio.h>

// Initialize the linked list and memory
void list_init(Node** head, size_t size) {
    *head = NULL; // Start with an empty list
    mem_init(size); // Initialize memory pool
}

// Insert a new node at the end of the list
void list_insert(Node** head, uint16_t data) {
    Node* new_node = (Node*)mem_alloc(sizeof(Node));  // Allocate memory for the new node
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }
    new_node->data = data;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;  // If the list is empty, make the new node the head
    } else {
        Node* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;  // Traverse to the end of the list
        }
        temp->next = new_node;  // Append new node
    }
}


// Insert a new node after a given node
void list_insert_after(Node* prev_node, uint16_t data) {
    if (prev_node == NULL) {
        fprintf(stderr, "Previous node cannot be NULL.\n");
        return;
    }
    Node* new_node = (Node*)mem_alloc(sizeof(Node));  // Allocate memory for the new node
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }
    new_node->data = data;
    new_node->next = prev_node->next;  // Point the new node to the next node
    prev_node->next = new_node;
}

// Insert a new node before a given node
void list_insert_before(Node** head, Node* next_node, uint16_t data) {
    if (next_node == NULL) {
        fprintf(stderr, "Next node cannot be NULL.\n");
        return;
    }
    Node* new_node = (Node*)mem_alloc(sizeof(Node));  // Allocate memory for the new node
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }
    new_node->data = data;  // Set the data for the new node
    if (*head == next_node) {
        // Insert at the beginning
        new_node->next = *head;
        *head = new_node;
    } else {
        // Find the node before next_node
        Node* current = *head;
        while (current != NULL && current->next != next_node) {
            current = current->next;
        }
        if (current == NULL) {
            fprintf(stderr, "The given next node is not present in the list.\n");
            mem_free(new_node);
            return;
        }
        new_node->next = next_node;  // Insert the new node before next_node
        current->next = new_node;  // Update the next pointer of the previous node
    }
}

// Delete a node with given data from the list
void list_delete(Node** head, uint16_t data) {
    Node* current = *head;
    Node* prev = NULL;

    while (current != NULL) {
        if (current->data == data) {
            if (prev == NULL) {
                // Deleting the head node
                *head = current->next;
            } else {
                prev->next = current->next;
            }
            mem_free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Search for a node with given data in the list
Node* list_search(Node** head, uint16_t data) {
    Node* current = *head;
    while (current != NULL) {
        if (current->data == data)
            return current;
        current = current->next;
    }
    return NULL; // Return NULL if the node is not found
}

// Display the list
void list_display(Node** head) {
    Node* current = *head;
    printf("[");
    while (current != NULL) {  // Traverse the list
        printf("%u", current->data);
        if (current->next != NULL) printf(", ");
        current = current->next;  // Move to the next node
    }
    printf("]");
}

// Display nodes from start_node to end_node
void list_display_range(Node** head, Node* start_node, Node* end_node) {
    Node* current = *head;
    int in_range = (start_node == NULL);  // Start printing when start_node is NULL
    int printed = 0;

    printf("[");
    while (current != NULL) {
        if (!in_range && current == start_node) {
            in_range = 1;  // Start printing from start_node
        }
        if (in_range) {
            if (printed) {
                printf(", ");
            }
            printf("%u", current->data);
            printed = 1;

            if (current == end_node) {
                break;
            }
        }
        current = current->next;
    }
    printf("]");
}

// Count the number of nodes in the list
int list_count_nodes(Node** head) {
    int count = 0;
    Node* current = *head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// Cleanup the list and free memory
void list_cleanup(Node** head) {
    Node* current = *head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        mem_free(temp);
    }
    *head = NULL;
    mem_deinit();  // Cleanup memory
}
