#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "matrix.h"

#define DEFAULT_ITERS 100
#define OUTPUT_ON 1

int getArg(char* arg, int argc, char* argv[]);
int** applyGameOfLife(int** matrix, int rows, int cols, int num_threads);

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

    // Reading the matrix from the input file
    int** matrix = parseMatrix(argv[1]);

    // Reading the number of threads
    int num_threads = atoi(argv[2]);

    // Setting the number of threads to the OpenMP environment
    omp_set_num_threads(num_threads);

    // Reading the number of iterations
    int iterations = DEFAULT_ITERS;
    int arg_index = getArg("-iter", argc, argv);
    if(arg_index != -1){
        iterations = atoi(argv[arg_index]);
    }

    // Apply the Game of Life algorithm for the specified number of iterations
    for(int i = 0; i < iterations; i++){
        // Applying the algorithm
        matrix = applyGameOfLife(matrix, rows, cols, num_threads);
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
    Applies the Game of Life algorithm to the matrix
    The algorithm is applied in parallel using OpenMP
*/
int** applyGameOfLife(int** matrix, int rows, int cols, int num_threads){
    // Creating a new matrix to store the new values
    int** new_matrix = createMatrix(rows, cols);

    // Applying the algorithm
    #pragma omp parallel for num_threads(num_threads) collapse(2) shared(matrix, new_matrix, rows, cols)
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            new_matrix[i][j] = applyRules(matrix, rows, cols, i, j);
        }
    }

    // Freeing the old matrix
    freeMatrix(matrix, rows);

    return new_matrix;
}