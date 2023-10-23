#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define MAX_SIZE 2048
#define MAX_GEN 2001
#define MAX_NEIGHBORS 8

typedef struct {
    float **grid;
} Generation;

Generation *InitGeneration(void);
void AddInitialCells(Generation *generation);
int GetNeighbors(float **grid, size_t i, size_t j);
bool IsAlive(float **grid, size_t i, size_t j);
float AverageNeighbors(int nCells);
void CellUpdate(float **grid, float **newGrid, size_t i, size_t j, int nCells);
void NewGeneration(Generation *newGeneration, Generation *generation);
bool CheckGeneration(Generation *newGeneration, Generation *generation);
long long TotalLivingCells(Generation *generation);
void PrintGrid(Generation *generation);
void FreeGeneration(Generation *generation);

int main(int argc, char **argv) {
    Generation *generation = InitGeneration();
    if(generation == NULL) {
        return -1;
    }

    AddInitialCells(generation);

    size_t i;
    long long totalLivingCells;
    printf("** Rainbow Game of Life\nCondição inicial: %lld\n", TotalLivingCells(generation));
    for(i = 1; i < (MAX_GEN - 1); i++) {
        //PrintGrid(generation);

        Generation *newGeneration = InitGeneration();
        if(newGeneration == NULL) {
            return -1;
        }

        NewGeneration(newGeneration, generation);

        totalLivingCells = TotalLivingCells(newGeneration);
        //printf("\nGeneration: %zu\nTotal Living Cells: %lld\n", (size_t)i, totalLivingCells);
        printf("Geração %zu: %lld\n", (size_t)i, totalLivingCells);

        FreeGeneration(generation);
        generation = newGeneration;

        if(i == (MAX_GEN - 1)) {
            FreeGeneration(newGeneration);
        } /*else {
            sleep(1);
            printf("\033c");
        }*/
    }
    //PrintGrid(generation);
    printf("Última geração (%zu iterações): %lld células vivas\n", (size_t)(MAX_GEN - 1), totalLivingCells);

    FreeGeneration(generation);
    return 0;
}

Generation *InitGeneration(void) {
    Generation *generation = (Generation *)malloc(sizeof(Generation));
    if(generation == NULL) {
        return NULL;
    }

    generation->grid = (float **)malloc(MAX_SIZE * sizeof(float *));
    if(generation->grid == NULL) {
        free(generation);
        return NULL;
    }

    size_t i, j;
    for(i = 0; i < MAX_SIZE; i++) {
        generation->grid[i] = (float *)malloc(MAX_SIZE * sizeof(float));
        if(generation->grid[i] == NULL) {
            for(j = 0; j < i; j++) {
                free(generation->grid[j]);
            }

            free(generation->grid);
            free(generation);
            return NULL;
        }

        for(j = 0; j < MAX_SIZE; j++) {
            generation->grid[i][j] = 0.0;
        }
    }

    return generation;
}

void AddInitialCells(Generation *generation) {
    size_t i = 1, j = 1;
    generation->grid[i][j + 1] = 1.0;
    generation->grid[i + 1][j + 2] = 1.0;
    generation->grid[i + 2][j] = 1.0;
    generation->grid[i + 2][j + 1] = 1.0;
    generation->grid[i + 2][j + 2] = 1.0;

    i = 10; j = 30;
    generation->grid[i][j + 1] = 1.0;
    generation->grid[i][j + 2] = 1.0;
    generation->grid[i + 1][j] = 1.0;
    generation->grid[i + 1][j + 1] = 1.0;
    generation->grid[i + 2][j + 1] = 1.0;
}

int GetNeighbors(float **grid, size_t i, size_t j) {
    int nCells = 0;

    int x, y;
    for(x = -1; x <= 1; x++) {
        for(y = -1; y <= 1; y++) {
            if(!(x == 0 && y == 0)) {
                //Periodic Boundary
                if(grid[(i + x + MAX_SIZE) % MAX_SIZE][(j + y + MAX_SIZE) % MAX_SIZE] == 1.0) {
                    nCells++;
                }
            }
        }
    }

    return nCells;
}

bool IsAlive(float **grid, size_t i, size_t j) {
    return (grid[i][j] == 1.0);
}

float AverageNeighbors(int nCells) {
    return ((float)nCells / MAX_NEIGHBORS);
}

void CellUpdate(float **grid, float **newGrid, size_t i, size_t j, int nCells) {
    if(IsAlive(grid, i, j) == true) {
        if(nCells >= 2 && nCells <= 3) {
            newGrid[i][j] = 1.0;
        } else {
            newGrid[i][j] = 0.0;
        }
    } else if(AverageNeighbors(nCells) > 0.0 && nCells == 3) {
        newGrid[i][j] = 1.0;
    }
}

bool CheckGeneration(Generation *newGeneration, Generation *generation) {
    size_t i, j;

    for(i = 0; i < MAX_SIZE; i++) {
        for(j = 0; j < MAX_SIZE; j++) {
            if((newGeneration->grid[i][j] != generation->grid[i][j])) {
                return false;
            }
        }
    }

    return true;
}

void NewGeneration(Generation *newGeneration, Generation *generation) {
    size_t i, j;

    for(i = 0; i < MAX_SIZE; i++) {
        for(j = 0; j < MAX_SIZE; j++) {
            CellUpdate(generation->grid, newGeneration->grid, i, j, GetNeighbors(generation->grid, i, j));
        }
    }
}

long long TotalLivingCells(Generation *generation) {
    size_t i, j;
    long long totalCels = 0;

    for(i = 0; i < MAX_SIZE; ++i) {
        for(j = 0; j < MAX_SIZE; ++j) {
            if(generation->grid[i][j] > 0.0) {
                totalCels++;
            }
        }
    }

    return totalCels;
}

void PrintGrid(Generation *generation) {
    size_t i, j;
    for(i = 0; i < MAX_SIZE; ++i) {
        for(j = 0; j < MAX_SIZE; ++j) {
            if(generation->grid[i][j] == 1.0) {
                printf("\033[1;31m%.0f\033[0m ", generation->grid[i][j]);
            } else {
                printf("%.0f ", generation->grid[i][j]);
            }
        }
        printf("\n");
    }
}

void FreeGeneration(Generation *generation) {
    if(generation) {
        size_t i;
        for(i = 0; i < MAX_SIZE; i++) {
            free(generation->grid[i]);
        }

        free(generation->grid);
        free(generation);
    }
}