#ifndef HASHSET_H
#define HASHSET_H

typedef enum status {INACTIVE, ACTIVE} Status;
typedef struct hashset* hashset_t;
typedef struct listnode* listnode_t;

// Create a hashset
hashset_t createHashSet(int size, int num_threads);

// Add a key to the hashset, if the key already exists, do nothing
void addHashSet(hashset_t hashset, int key, Status status);\

// Sets the status of the key to ACTIVE
void setActiveHashSet(hashset_t hashset, int key, Status status);

// Apply a function to each key in the hashset, after the function is applied,
// the key is removed from the hashset based on the return value of the function
void applyHashSet(hashset_t hashset, int (*func)(int, Status, void*), void* arg);

// Print the hashset
void printHashSet(hashset_t hashset);

// Free the hashset
void freeHashSet(hashset_t hashset);

void addListNode(listnode_t* head, int key);
void freeList(listnode_t* head);
void applyList(listnode_t* head, int (*func)(int, void*), void* arg);
void printList(listnode_t head);

#endif // HASHSET_H