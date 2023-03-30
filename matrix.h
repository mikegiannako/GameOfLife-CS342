#ifndef MATRIX_H
#define MATRIX_H

#define DEAD 0
#define ALIVE 1

int countNeighbors(int** matrix, int rows, int cols, int row, int col);
int applyRules(int** matrix, int rows, int cols, int row, int col);
void readRowsCols(char* filename, int* rows, int* cols);
int** parseMatrix(char* filename);
void writeMatrix(char* filename, int** matrix, int rows, int cols);
void freeMatrix(int** matrix, int rows);

#endif // MATRIX_H