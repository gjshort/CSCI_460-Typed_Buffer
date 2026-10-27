#ifndef CDLL_H
#define CDLL_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Outlines the components and functions for a circular, doubly linked list.
*
* Griffin Short
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*/

/*
* Defines the nodes in the list. Each node contains an integer
* value and pointers to the next and last nodes.
*/
struct Node {
    struct Node* next;
    struct Node* last;
    int value;
};

/*
* Returns the number of nodes in the list.
*
* Takes in double pointers to the head and tail.
*/
int getSize(struct Node** headPtr, struct Node** tailPtr) {
    if (*headPtr == NULL) {
        return 0;
    }
    // Count nodes from head until you return to head
    else {
        struct Node* curr = *headPtr;
        int count = 0;
        do {
            curr = curr->next;
            count++;
        } while (curr != *headPtr);
        return count;
    }
}

/*
* Adds a new node to the front of the list.
*
* Takes in the value for the new node, and
* double pointers to the head and tail.
*/
void addToFront(int newVal, struct Node** headPtr, struct Node** tailPtr) {
    // If empty list, create first node.
    if (*headPtr == NULL) {
        struct Node* nextNode = *headPtr;
        *headPtr = (struct Node*)(malloc(sizeof(struct Node)));
        if (*headPtr == NULL) {
            printf("Could not allocate for new node\n");
            exit(EXIT_FAILURE);
        }
        // Head and tail are both the only node. It points in both directions to itself
        *tailPtr = *headPtr;
        (*headPtr)->value = newVal; 
        (*headPtr)->last = *tailPtr;
        (*tailPtr)->next = *headPtr;
    }
    // If non-empty list
    else {
        struct Node* nextNode = *headPtr;
        *headPtr = (struct Node*)(malloc(sizeof(struct Node)));
        if (*headPtr == NULL) {
            printf("Could not allocate for new node\n");
            exit(EXIT_FAILURE);
        }
        // Insert at front of list (new head), update pointers of next node and tail
        (*headPtr)->value = newVal;
        (*headPtr)->next = nextNode;
        (*headPtr)->last = *tailPtr;
        (*tailPtr)->next = *headPtr;
        nextNode->last = *headPtr;
    }
}

/*
* Adds a new node to the end of the list.
*
* Takes in the value for the new node, and
* double pointers to the head and tail.
*/
void addToEnd(int newVal, struct Node** headPtr, struct Node** tailPtr) {
    // If empty list, create first node.
    if (*headPtr == NULL) {
        struct Node* nextNode = *headPtr;
        *headPtr = (struct Node*)(malloc(sizeof(struct Node)));
        if (*headPtr == NULL) {
            printf("Could not allocate for new node\n");
            exit(EXIT_FAILURE);
        }
        // Head and tail are both the only node. It points in both directions to itself
        *tailPtr = *headPtr;
        (*headPtr)->value = newVal;
        (*headPtr)->last = *tailPtr;
        (*tailPtr)->next = *headPtr;
    }
    // If non-empty list
    else {
        struct Node* lastNode = *tailPtr;
        *tailPtr = (struct Node*)(malloc(sizeof(struct Node)));
        if (*tailPtr == NULL) {
            printf("Could not allocate for new node\n");
            exit(EXIT_FAILURE);
        }
        // Insert at end of list (new tail), update pointers of next node and head
        (*tailPtr)->value = newVal;
        (*tailPtr)->last = lastNode;
        (*tailPtr)->next = *headPtr;
        (*headPtr)->last = *tailPtr;
        lastNode->next = *tailPtr;
    }
}

/*
* Adds a new node to the specified slot in the list.
*
* Takes in the value and location for the new node, and
* double pointers to the head and tail.
*/
void insert(int slot, int newVal, struct Node** headPtr, struct Node** tailPtr) {
    if (slot <= 1 || *headPtr == NULL) {
        addToFront(newVal, headPtr, tailPtr);
    }
    else if (slot >= getSize(headPtr, tailPtr)) {
        addToEnd(newVal, headPtr, tailPtr);
    }
    else {
        struct Node* curr = *headPtr;
        int position = 1;
        // Walk through list to specified slot
        while (position < slot) {
            curr = curr->next;
            position++;
        }
        struct Node* newNode = (struct Node*)(malloc(sizeof(struct Node)));
        if (newNode == NULL) {
            printf("Could not allocate for new node\n");
            exit(EXIT_FAILURE);
        }
        // Insert new node and update adjacent pointers
        newNode->value = newVal;
        newNode->last = curr->last;
        newNode->next = curr;
        newNode->last->next = newNode;
        curr->last = newNode;
    }
}

/*
* Removes the node from the front of the list.
*
* Takes in double pointers to the head and tail.
*/
void removeFirst(struct Node** headPtr, struct Node** tailPtr) {
    if (*headPtr == NULL) {
        printf("Can't remove from empty list/n");
    }
    else if (getSize(headPtr, tailPtr) == 1) {
        free(*headPtr);
        *headPtr = NULL;
        *tailPtr = NULL;
    }
    else {
        // Next node becomes head
        struct Node* temp = *headPtr;
        *headPtr = (*headPtr)->next;
        (*headPtr)->last = *tailPtr;
        (*tailPtr)->next = *headPtr;
        free(temp);
    }

}

/*
* Removes the node from the end of the list.
*
* Takes in double pointers to the head and tail.
*/
void removeLast(struct Node** headPtr, struct Node** tailPtr) {
    if (*headPtr == NULL) {
        printf("Can't remove from empty list/n");
    }
    else if (getSize(headPtr, tailPtr) == 1) {
        free(*headPtr);
        *headPtr = NULL;
        *tailPtr = NULL;
    }
    else {
        // Second to last node becomes tail
        struct Node* temp = *tailPtr;
        *tailPtr = (*tailPtr)->last;
        (*tailPtr)->next = *headPtr;
        (*headPtr)->last = *tailPtr;
        free(temp);
    }
}

/*
* Removes the node at the specified slot in the list
*
* Takes in the location of the node to be removed, and
* double pointers to the head and tail.
*/
void removeSlot(int slot, struct Node** headPtr, struct Node** tailPtr) {
    if (*headPtr == NULL) {
        printf("Can't remove from empty list\n");
    }
    else if (slot == 1) {
        removeFirst(headPtr, tailPtr);
    }
    else if (slot >= getSize(headPtr, tailPtr)) {
        removeLast(headPtr, tailPtr);
    }
    else {
        struct Node* curr = *headPtr;
        int position = 1;
        // Walk through list to specified slot
        while (position < slot) {
            curr = curr->next;
            position++;
        }
        // Remove all pointers to node and deallocate it
        curr->last->next = curr->next;
        curr->next->last = curr->last;
        free(curr);
    }
}

/*
* Removes the first node with the given value in the list.
* Returns 0 if no node was found with the given value.
* Returns 1 if a node was removed.
*
* Takes in the value of the node to be removed, and
* double pointers to the head and tail.
*/
bool removeValue(int target, struct Node** headPtr, struct Node** tailPtr) {
    if (*headPtr == NULL) {
        printf("Can't remove from empty list\n");
        return 0;
    }
    else {
        struct Node* curr = *headPtr;
        int position = 1;
        // Walk list until remove found value or reached end of list
        do {
            if (curr->value == target) {
                removeSlot(position, headPtr, tailPtr);
                return 1;
            }
            curr = curr->next;
            position++;
        } while (curr != *headPtr);
        return 0;
    }
}

/*
* Retrieves the value at the specified slot in the list
*
* Takes in the location of the node to be accessed, a pointer
* to the return value's location, and double pointers 
* to the head and tail.
*/
bool at(int slot, int *return_val, struct Node** headPtr, struct Node** tailPtr) {
    if (*headPtr == NULL) {
        printf("Can't remove from empty list\n");
        return 0;
    }
    else {
        struct Node* curr = *headPtr;
        int position = 1;
        // Walk through list to specified slot
        while (position < slot) {
            curr = curr->next;
            position++;
        }
        *return_val = curr->value;
        return 1;
    }
}

/*
* Prints the values of the nodes in the list.
*
* Takes in a double pointer to the head.
*/
void printList(struct Node** headPtr) {
    if (*headPtr == NULL) {
        printf("Empty List\n");
    }
    else {
        printf("-------------\n");
        struct Node* curr = *headPtr;
        // Walk list and print node values
        do {
            printf("%d\n", curr->value);
            curr = curr->next;
        } while (curr != *headPtr);
        printf("-------------\n");
    }
}

/*
* Deallocates all heap memory used during the
* creation of the nodes in list.
*/
void destroyList(struct Node** headPtr, struct Node** tailPtr) {
    if (*headPtr == NULL) {
        printf("Can't destroy empty list\n");
    }
    else {
        (*tailPtr)->next = NULL; // Detach tail's next from head
        struct Node* curr;
        // Walk list by make next node new head and removing current head
        while (*headPtr != NULL) {
            curr = *headPtr;
            *headPtr = (*headPtr)->next;
            free(curr);
        }
        *headPtr = NULL;
        *tailPtr = NULL;
    }
}

#endif