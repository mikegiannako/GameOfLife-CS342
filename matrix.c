#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "matrix.h"

cell_t** createMatrix(int rows, int cols){
    // Allocating memory for the matrix
    cell_t** matrix = (cell_t**) malloc(rows * sizeof(cell_t*));

    // Allocating memory for each row
    for(int i = 0; i < rows; i++){
        matrix[i] = (cell_t*) malloc(cols * sizeof(cell_t));

        // Allocating memory for each cell
        for(int j = 0; j < cols; j++){
            matrix[i][j] = (cell_t) malloc(sizeof(struct cell));
            matrix[i][j]->curr_val = DEAD;
            matrix[i][j]->next_val = DEAD;
        }
    }

    return matrix;
}

/*
    Counts the number of neighbors of a cell in the matrix
    The cell is specified by the row and column
*/
int countNeighbors(cell_t** matrix, int rows, int cols, int row, int col){
    int count = 0;

    // Iteterating over the 3x3 neighborhood of the cell
    for(int i = row - 1; i <= row + 1; i++){
        for(int j = col - 1; j <= col + 1; j++){
            // Checking if the cell is outside the matrix
            if(i < 0 || i >= rows || j < 0 || j >= cols){
                continue;
            }

            // Checking if the cell is the cell itself
            if(i == row && j == col) continue;

            // Adding the value of the cell to the count
            count += matrix[i][j]->curr_val;
        }
    }

    return count;
}

/*
    Adds all the neighbors of a cell to the hashset
*/
void addAllNeighbors(listnode_t* list, int rows, int cols, int row, int col){
    // Iteterating over the 3x3 neighborhood of the cell
    for(int i = row - 1; i <= row + 1; i++){
        for(int j = col - 1; j <= col + 1; j++){
            // Checking if the cell is outside the matrix
            if(i < 0 || i >= rows || j < 0 || j >= cols){
                continue;
            }

            addListNode(list, i * cols + j);
        }

    }

    return;
}

/*
    Applies the rules of the game of life to a cell in the matrix (doesn't change the value of the cell)
    Returns the new value of the cell, which is either ALIVE or DEAD
*/
int applyRules(cell_t** matrix, int rows, int cols, int row, int col){
    // Counting the number of neighbors
    int neighbors = countNeighbors(matrix, rows, cols, row, col);

    // Applying the rules

    // If the cell is alive
    if(matrix[row][col]->curr_val == ALIVE){
        // If the cell has less than 2 or more than 3 neighbors, it dies
        if(neighbors < 2 || neighbors > 3){
            return DEAD;
        }
    }
    // If the cell is dead
    else{
        // If the cell has exactly 3 neighbors, it becomes alive
        if(neighbors == 3){
            return ALIVE;
        }
    }

    // If none of the rules apply, the cell remains the same
    return matrix[row][col]->curr_val;
}


/* Read rows and columns from input file */
void readRowsCols(char* filename, int* rows, int* cols){
    // File to be read
    FILE* file = fopen(filename, "r");

    // Reading the number of rows and columns
    if(fscanf(file, "%d %d", rows, cols) != 2){
        exit(-1);
    }

    // Closing the file
    fclose(file);

    return;
}

/* 
    Reads the specified file and parses it to read the input matrix
    The input format is the following:

    8 7 
    | | | | | | |*|
    | | | | | |*|*|
    | |*| | | |*|*|
    | | | | | | | |
    | | | | | | |*|
    | | |*|*| | |*|
    | | |*|*| | | |
    | | | | | | | |

    The first line contains the number of rows and columns of the matrix
    The following lines contain the matrix, where * represents a 1 and space represents a 0
    Each cell is separated by a '|' character
*/
cell_t** parseMatrix(char* filename, hashset_t hashset, listnode_t* list){
    // File to be read
    FILE* file = fopen(filename, "r");

    if(file == NULL) {
        printf("Failed to open file: %s\n", filename);
        exit(1);
    }

    // Reading the number of rows and columns
    int rows, cols;
    if(fscanf(file, "%d %d", &rows, &cols) != 2){
        exit(-1);
    }

    // Allocating memory for the matrix based on the numbers we just read
    cell_t** matrix = createMatrix(rows, cols);

    int buffer_size = /* columns + separators */ cols * 2 + 1 
                    + /*\n*/ 1 
                    + /*\0*/ 1
                    + /*\r*/ 1;

    // Allocating memory for the line that will be read
    char* line = malloc(buffer_size * sizeof(char));

    // Skipping the first line
    if(fgets(line, buffer_size, file) == NULL){
        exit(-1);
    }

    // Reading the matrix
    for(int i = 0; i < rows; i++){
        // Reading the line
        if(fgets(line, buffer_size, file) == NULL){
            exit(-1);
        }


        // Parsing the line
        for(int j = 0; j < cols; j++){
            // Reading the character at the 1 (first separator of the line)
            //                            + j (column number) * 2 (separator + character)
            if(line[1 + j * 2] == '*'){
                matrix[i][j]->curr_val = ALIVE;
                addAllNeighbors(list, rows, cols, i, j);
            }
            else matrix[i][j]->curr_val = DEAD;
        }
    }

    // Closing the file and returning the matrix
    fclose(file);
    return matrix;
}

void writeMatrix(char* filename, cell_t** matrix, int rows, int cols){
    // File to be written
    FILE* file = fopen(filename, "w+");

    // Writing the number of rows and columns
    fprintf(file, "%d %d\n", rows, cols);

    // Writing the matrix
    for(int i = 0; i < rows; i++){
        // Writing the separator
        fprintf(file, "|");
        
        // For each cell, print the character and a separator
        for(int j = 0; j < cols; j++){
            fprintf(file, "%c|", matrix[i][j]->curr_val == ALIVE ? '*' : ' ');
        }

        // Writing the new line
        fprintf(file, "\n");
    }

    // Closing the file
    fclose(file);

    return;
}

void printMatrix(cell_t** matrix, int rows, int cols){
    // Writing the number of rows and columns
    printf("%d %d\n", rows, cols);

    // Writing the matrix
    for(int i = 0; i < rows; i++){
        // Writing the separator
        printf("|");
        
        // For each cell, print the character and a separator
        for(int j = 0; j < cols; j++){
            printf("%c|", matrix[i][j]->curr_val == ALIVE ? '*' : ' ');
        }

        // Writing the new line
        printf("\n");
    }

    return;
}

void freeMatrix(cell_t** matrix, int rows, int cols, int num_threads){
    // Freeing the memory allocated for each row
    #ifndef SERIAL
        #pragma omp parallel for schedule(static) num_threads(num_threads)
    #endif
    for(int i = 0; i < rows; i++){
        #ifndef SERIAL
            #pragma omp parallel for schedule(static) num_threads(num_threads)
        #endif
        for(int j = 0; j < cols; j++){
            free(matrix[i][j]);
        }
        
        free(matrix[i]);
    }


    // Freeing the memory allocated for the matrix
    free(matrix);

    return;
}