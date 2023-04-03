#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "matrix.h"
#include "hashset.h"

#define DEFAULT_ITERS 100
#define OUTPUT_ON 1
#define HASHSET_DIVISOR 5

typedef struct args_t{
    cell_t** matrix;
    hashset_t hashset;
    listnode_t* list;
    int rows;
    int cols;
}* args_t;

int getArg(char* arg, int argc, char* argv[]);
int applyGameOfLifeToCell(int key, Status status, void* args);
int updateValueOfCell(int key, Status status, void* args);
int addListNodeToHashSet(int key, void* args);

/* 
    The first command-line argument specifies the input file
    The second command-line argument specifies the number of threads to be used
    The third command-line argument specifies output file

    Optional command-line arguments:
        -iter: Specifies the number of iterations to be performed
        -output: Speicifies if the program should produce an output file
*/
int main(int argc, char *argv[]){
    // Checking the number of command-line arguments
    if(argc < 4){
        printf("Usage: %s <input file> <number of threads> <output file> (-iter <number of iterations>) (-output <0/1>)\n", argv[0]);
        return 1;
    }

    // Reading the matrix rows and columns from the input file
    int rows, cols;
    readRowsCols(argv[1], &rows, &cols);

    // Reading the number of threads
    int num_threads = atoi(argv[2]);

    #ifndef SERIAL
        // Setting the number of threads to the OpenMP environment
        omp_set_num_threads(num_threads);
    #endif

    // Creating the hashset and a list of cells to be added to the hashset 
    hashset_t hashset = createHashSet((rows * cols) / HASHSET_DIVISOR, num_threads);
    listnode_t list = NULL;

    // Reading the matrix from the input file
    cell_t** matrix = parseMatrix(argv[1], hashset, &list);

    // Reading the number of iterations
    int iterations = DEFAULT_ITERS;
    int arg_index = getArg("-iter", argc, argv);
    if(arg_index != -1){
        iterations = atoi(argv[arg_index]);
    }

    // Creating the arguments for the applyGameOfLifeToCell function
    args_t args = (args_t) malloc(sizeof(struct args_t));
    args->matrix = matrix;
    args->hashset = hashset;
    args->list = &list;
    args->rows = rows;
    args->cols = cols;

    // Getting all the items to be added to the hashset
    applyList(&list, addListNodeToHashSet, (void*) hashset);
    freeList(&list);

    // Apply the Game of Life algorithm for the specified number of iterations
    for(int i = 0; i < iterations; i++){
        // Applying the algorithm
        applyHashSet(hashset, applyGameOfLifeToCell, (void*) args);
        applyHashSet(hashset, updateValueOfCell, (void*) args);
        applyList(&list, addListNodeToHashSet, (void*) hashset);
        freeList(&list);
    }

    // Reading the output flag
    int output = OUTPUT_ON;
    arg_index = getArg("-output", argc, argv);
    if(arg_index != -1){
        output = atoi(argv[arg_index]);
    }

    // Writing the matrix to the output file
    if(output){
        writeMatrix(argv[3], matrix, rows, cols);
    }

    // Freeing the matrix
    freeMatrix(matrix, rows, cols, num_threads);
    
    return 0;
}

/*
    Returns the next index of the argument in the command-line arguments
    If the argument is not found, returns -1
*/
int getArg(char* arg, int argc, char* argv[]){
    for(int i = 0; i < argc; i++){
        // If the argument is found
        if(!strcmp(arg, argv[i])){
            // Checking if the next argument is in range
            if(i + 1 >= argc){
                printf("Option %s used but not value was given\n", arg);
                exit(1);
            }

            return i + 1;
        }
    }

    return -1;
}

/*
    Function to be passed to the hashset apply function
    It applies the Game of Life algorithm to the cell
*/
int applyGameOfLifeToCell(int key, Status status, void* args){
    // Getting the arguments
    args_t arguments = (args_t) args;
    cell_t** matrix = arguments->matrix;
    hashset_t hashset = arguments->hashset;
    listnode_t* list = arguments->list;
    int rows = arguments->rows;
    int cols = arguments->cols;
    int x = key / cols;
    int y = key % cols;

    // Applying the rules
    matrix[x][y]->next_val = applyRules(matrix, rows, cols, x, y);
    
    // If the cell will be alive in the next iteration, add all its neighbors to the hashset
    if(matrix[x][y]->next_val == ALIVE){
        addAllNeighbors(list, rows, cols, x, y);
        return 1;
    }

    // Else, if the cell will be dead in the next iteration, set it to inactive
    setActiveHashSet(hashset, key, INACTIVE);
    return 1;
}

/*
    Function to be passed to the hashset apply function
    It udpates the value of the cell
*/
int updateValueOfCell(int key, Status status, void* args){
    // Getting the arguments
    args_t arguments = (args_t) args;
    cell_t** matrix = arguments->matrix;
    int cols = arguments->cols;
    int x = key / cols;
    int y = key % cols;

    // Updating the value of the cell
    matrix[x][y]->curr_val = matrix[x][y]->next_val;

    if(status == INACTIVE){
        return 0;
    }

    return 1;
}

/*
    Function that will be passed to the list apply function
    it adds all the nodes of the list to the hashset
*/
int addListNodeToHashSet(int key, void* args){
    // Getting the arguments
    hashset_t hashset = (hashset_t) args;

    // Adding the node to the hashset
    addHashSet(hashset, key, ACTIVE);

    return 1;
}