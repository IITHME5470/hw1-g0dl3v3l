#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

int NumVec = 4;              // Number of vectors to process
int matID[4] = {3, 5, 50, 80};  // Matrix IDs derived from the size of matrices

char* inputFilePath = "../q2/inputfiles";  // Directory for input files
char* time_log = "../q2/logs/timings";      // Directory for timing logs

// Function declarations
void process(int NumVec, int matID);  
void* readInput(int num_args, ...);
bool isEigenVector(double** matrix, double* vector, int n, double* lambda);
void fileWrite(int matID, int vectorNumber, double eigenVal);
void matrixVectorProduct(double** matrix, double* vector, int matID, double* product);
void CalTime(int matID, double totalTime, int iterations);

int main() {
    int i;
    // Process each matrix ID in the matID array
    for (i = 0; i < sizeof(matID) / sizeof(int); i++) {
        process(NumVec, matID[i]);
    }
    return 0;
}

// Function to process each matrix and its corresponding vectors
void process(int NunVec, int matID) {
    // Read the matrix from the input file
    double** matrix = (double**)readInput(1, matID);
    
    clock_t start, end;
    double totalTime = 0.0;

    int i;
    for (i = 0; i < NumVec; i++) {
        // Read the vector from the input file
        double* vector = (double*)readInput(2, matID, i + 1);
        double eigenVal;

        // Start timing the eigenvector check
        start = clock();
        bool isEigen = isEigenVector(matrix, vector, matID, &eigenVal);
        end = clock();

        // Calculate the total time taken
        totalTime += ((double)(end - start)) / CLOCKS_PER_SEC; 

        // Output results to the console
        if (isEigen) {
            printf("vec_%06d_%06d.in : Yes : %.6f\n", matID, i + 1, eigenVal);
            // Append to file (commented)
             fileWrite(matID, i + 1, eigenVal);
        } else {
            printf("vec_%06d_%06d.in : Not an eigenvector\n", matID, i + 1);
        }

        free(vector);  // Free allocated memory for the vector
    }

    // Log the average computation time for the matrix
    CalTime(matID, totalTime, NumVec);

    // Free allocated memory for the matrix
    for (i = 0; i < matID; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Function to read input matrix/vector files dynamically using variable arguments
void* readInput(int num_args, ...) {
    va_list args;
    va_start(args, num_args);

    // Case 1: Read a matrix from the file
    if (num_args == 1) {
        int n = va_arg(args, int);  // Matrix ID

        // Allocate memory for the matrix
        double** matrix = (double**)malloc(sizeof(double*) * n);
        int i;
        for (i = 0; i < n; i++) {
            matrix[i] = (double*)malloc(sizeof(double) * n);
        }

        // Construct file path
        char* fileName = (char*)malloc(sizeof(char) * 150);
        sprintf(fileName, "%s/mat_%06d.in", inputFilePath, n);
        FILE* file = fopen(fileName, "r");

        if (!file) {
            perror("Error opening matrix file");
            return NULL;
        }

        // Read matrix from file line by line
        char* line = NULL;
        size_t Linelen = 0;
        int row = 0;

        while (getline(&line, &Linelen, file) != -1 && row < n) {
            char* token;
            int col = 0;
            token = strtok(line, ",");
            while (token && col < n) {
                matrix[row][col++] = atof(token);
                token = strtok(NULL, ",");
            }
            row++;
        }

        // Cleanup
        free(line);
        free(fileName);
        fclose(file);

        return matrix;
    } 
    // Case 2: Read a vector from the file
    else if (num_args == 2) {
        int n = va_arg(args, int);  // Matrix ID
        int vectorNumber = va_arg(args, int);

        // Allocate memory for the vector
        double* vector = (double*)malloc(sizeof(double) * n);

        // Construct file path
        char* fileName = (char*)malloc(sizeof(char) * 150);
        sprintf(fileName, "%s/vec_%06d_%06d.in", inputFilePath, n, vectorNumber);
        FILE* file = fopen(fileName, "r");

        if (!file) {
            perror("Error opening vector file");
            return NULL;
        }

        // Read vector from file
        char* line = NULL;
        size_t lenLen = 0;
        int row = 0;

        if (getline(&line, &lenLen, file) == -1) {
            perror("Error reading vector file");
            free(vector);
            free(fileName);
            fclose(file);
            return NULL;
        }

        char* token;
        token = strtok(line, ",");
        while (token && row < n) {
            vector[row++] = atof(token);
            token = strtok(NULL, ",");
        }

        free(fileName);
        fclose(file);
        return vector;
    }
    return NULL;
}

// Function to check if a vector is an eigenvector of a matrix
bool isEigenVector(double** matrix, double* vector, int n, double* lambda) {
    // Allocate memory for the matrix-vector product
    double* MatVecProduct = (double*)malloc(sizeof(double) * n);
    if (!MatVecProduct) {
        perror("Memory allocation failed");
        return false;
    }

    // Compute the matrix-vector product
    matrixVectorProduct(matrix, vector, n, MatVecProduct);

    // Find the first non-zero element in the vector
    int nonZeroIndex = -1;
    int i;
    for (i = 0; i < n; i++) {
        if (vector[i] != 0) {
            nonZeroIndex = i;
            break;
        }
    }

    // If no non-zero element is found, vector is not an eigenvector
    if (nonZeroIndex == -1) {
        free(MatVecProduct);
        return false;
    }

    // Calculate the potential eigenvalue
    double potentialLambda = MatVecProduct[nonZeroIndex] / vector[nonZeroIndex];

    // Verify the eigenvalue condition for all elements
    for (i = nonZeroIndex + 1; i < n; i++) {
        if (vector[i] != 0) {
            double difference = fabs(MatVecProduct[i] - potentialLambda * vector[i]);
            if (difference > 1e-6) { 
                free(MatVecProduct);
                return false;
            }
        }
    }

    *lambda = potentialLambda;  // Assign the computed eigenvalue
    free(MatVecProduct);
    return true;
}

// Function to multiply matrix and vector
void matrixVectorProduct(double** matrix, double* vector, int matID, double* product) {
    int i;
    for (i = 0; i < matID; i++) { 
        double sum = 0;  
        int j;
        for (j = 0; j < matID; j++) {
            sum += matrix[i][j] * vector[j];
        }
        product[i] = sum; 
    }
}

// Function to append the eigenvalue result to a file
void fileWrite(int matID, int vectorNumber, double eigenVal) {
    char fileName[150];
    sprintf(fileName, "%s/vec_%06d_%06d.in", inputFilePath, matID, vectorNumber);
    FILE* outfile = fopen(fileName, "a");
    if (!outfile) {
        perror("Error opening file");
        return;
    }

    fprintf(outfile, "\nEigenvalue: %.6f\n", eigenVal);
    fclose(outfile);
}

// Function to log average execution time
void CalTime(int matID, double totalTime, int iterations) {
    char fileName[150];
    sprintf(fileName, "%s/average_time.log", time_log);
    FILE* file = fopen(fileName, "a");
    if (!file) {
        perror("Error opening timing file");
        return;
    }

    double avgTime = totalTime / iterations;
    fprintf(file, "MatrixID: %06d AvgTime: %.6f seconds\n", matID, avgTime);

    fclose(file);
}
