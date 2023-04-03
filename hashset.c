#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "hashset.h"

struct listnode{
    int key;
    Status status;
    struct listnode* next;
};

struct hashset{
    int size;
    int num_elements;
    listnode_t* buckets;
};

// Hash function 
int hash(hashset_t hashset, int key){
    return key % hashset->size;
}

// Create a hashset
hashset_t createHashSet(int size, int num_threads){

    #ifndef SERIAL
        // Setting the number of threads to the OpenMP environment
        omp_set_num_threads(num_threads);
    #endif

    hashset_t hashset = (hashset_t) malloc(sizeof(struct hashset));
    hashset->size = size;
    hashset->buckets = (listnode_t*) calloc(size, sizeof(listnode_t));
    return hashset;
}

// Add a key to the hashset, if the key already exists, do nothing
void addHashSet(hashset_t hashset, int key, Status status){
    int index = hash(hashset, key);

    // Check if the key already exists
    for(listnode_t node = hashset->buckets[index]; node != NULL; node = node->next){
        if(node->key == key) return;
    }

    // Add the key to the hashset
    listnode_t newnode = (listnode_t) malloc(sizeof(struct listnode));
    newnode->key = key;
    newnode->status = status;
    newnode->next = hashset->buckets[index];
    hashset->buckets[index] = newnode;

    return;
}

// Remove a key from the hashset and free the memory
void removeHashSet(hashset_t hashset, int key){
    #ifndef SERIAL
        #pragma omp critical
        {
    #endif

    int index = hash(hashset, key);

    // Find the key in the list
    listnode_t node = hashset->buckets[index], prev = NULL;
    while(node != NULL){
        if(node->key == key){
            if(prev == NULL){
                hashset->buckets[index] = node->next;
            }else{
                prev->next = node->next;
            }
            free(node);
            break;
        }
        prev = node;
        node = node->next;
    }

    #ifndef SERIAL
        }
    #endif

    return;
}

void setActiveHashSet(hashset_t hashset, int key, Status status){
    int index = hash(hashset, key);

    // Check if the key already exists
    for(listnode_t node = hashset->buckets[index]; node != NULL; node = node->next){
        if(node->key == key){
            node->status = status;
            return;
        }
    }

    return;
}

// Apply a function to each key in the hashset
void applyHashSet(hashset_t hashset, int (*func)(int, Status, void*), void* arg){

    // Iterate through the buckets
    #ifndef SERIAL
        #pragma omp parallel for schedule(static)
    #endif
    for(int i = 0; i < hashset->size; i++){
        // Iterate through the list
        listnode_t node = hashset->buckets[i], next;

        while(node != NULL){
            next = node->next;

            // If the function returns 0, the node has to be removed from the list
            if(func(node->key, node->status, arg) == 0){
                removeHashSet(hashset, node->key);
            }

            node = next;
        }
    }

    return;
}

// Print the hashset
void printHashSet(hashset_t hashset){
    for(int i = 0; i < hashset->size; i++){
        printf("Bucket %d: ", i);
        for(listnode_t node = hashset->buckets[i]; node != NULL; node = node->next){
            printf("%d-%s ", node->key, node->status == INACTIVE ? "INACTIVE" : "ACTIVE");
        }
        printf("\n");
    }
}

// Free the hashset
void freeHashSet(hashset_t hashset){
    #ifndef SERIAL
        #pragma omp parallel for schedule(static)
    #endif
    for(int i = 0; i < hashset->size; i++){
        listnode_t node = hashset->buckets[i];
        while(node != NULL){
            listnode_t tmp = node;
            node = node->next;
            free(tmp);
        }
    }
    free(hashset->buckets);
    free(hashset);
}


listnode_t createListNode(int key){
    listnode_t node = (listnode_t) malloc(sizeof(struct listnode));
    node->key = key;
    node->status = ACTIVE;
    node->next = NULL;
    return node;
}

void addListNode(listnode_t* head, int key){
    #ifndef SERIAL
        #pragma omp critical
        {
    #endif

    listnode_t node = createListNode(key);
    node->next = *head;
    *head = node;

    #ifndef SERIAL
        }
    #endif
}

void freeList(listnode_t* head){
    listnode_t node = *head, next;
    while(node != NULL){
        next = node->next;
        free(node);
        node = next;
    }

    *head = NULL;
}

/*
    Apply a function to each element in the list and remove the item afterwards
*/
void applyList(listnode_t* head, int (*func)(int, void*), void* arg){
    listnode_t node = *head;

    while(node != NULL){
        func(node->key, arg);
        node = node->next;
    }

    return;
}

void printList(listnode_t head){
    for(listnode_t node = head; node != NULL; node = node->next){
        printf("%d ", node->key);
    }
    printf("\n");
}