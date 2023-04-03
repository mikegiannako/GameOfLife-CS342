#ifndef MATRIX_H
#define MATRIX_H

#include "hashset.h"

typedef struct cell{
    int curr_val;
    int next_val;
}* cell_t;



#define DEAD 0
#define ALIVE 1

// Function prototypes

// Creates a matrix of size rows x cols
cell_t** createMatrix(int rows, int cols);

// Adds the neighbors of a cell to the hashset (and the cell itself)
void addAllNeighbors(listnode_t* list, int rows, int cols, int row, int col);

// Applies the rules of the game of life to a cell and returns the new value
// (Fuction doesn't change the value of the cell)
int applyRules(cell_t** matrix, int rows, int cols, int row, int col);

// Reads the number of rows and columns from the first line of the file
void readRowsCols(char* filename, int* rows, int* cols);

// Reads the matrix from the file
cell_t** parseMatrix(char* filename, hashset_t hashset, listnode_t* list);

// Writes the matrix to the file
void writeMatrix(char* filename, cell_t** matrix, int rows, int cols);

// Frees the memory allocated for the matrix
void freeMatrix(cell_t** matrix, int rows, int cols, int num_threads);

// Prints the matrix to the screen
void printMatrix(cell_t** matrix, int rows, int cols);

#endif // MATRIX_H