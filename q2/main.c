#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>


int NumVec = 4;          // number of vectors
int matID[4] = {3,5,50,80};  // the matrix id are derived from the size of the matrix

char* inputFilePath = "../q2/inputfiles";
char* time_log = "../q2/logs/timings";

void process(int NumVec , int matID);  //performs the business logic
void* readInput(int num_args , ...);
bool isEigenVector(double** matrix ,  double*vector, int n , double*lambda);
void fileWrite(int matID, int vectorNumber , double eigenVal);
void matrixVectorProduct(double** matrix, double* vector, int matID, double* product);
void CalTime(int matID, double totalTime, int iterations);

int main(){
    for(int i =0  ; i<sizeof(matID)/sizeof(int) ; i++){
        process(NumVec ,matID[i]);
    }
    return 0;
}
void process(int NunVec , int matID){
    double** matrix = (double**)readInput(1,matID);
    
    clock_t start, end;
    double totalTime = 0.0;


    for(int i = 0 ; i<NumVec;i++){
        double * vector = (double*)readInput(2, matID , i+1);
        double eigenVal ;

        start = clock();
        bool isEigen = isEigenVector(matrix , vector , matID,&eigenVal);

        // End timing
        end = clock();
        totalTime += ((double)(end - start)) / CLOCKS_PER_SEC; // Calculate total time in seconds


        if (isEigen) {
                // Print to screen
                printf("vec_%06d_%06d.in : Yes : %.6f\n", matID, i+1, eigenVal);
                // Append to file
                fileWrite(matID, i+1, eigenVal);
            } else {
                // Print to screen
                printf("vec_%06d_%06d.in : Not an eigenvector\n", matID, i+1);
            }

        free(vector);
    }
    CalTime(matID, totalTime, NumVec);

    for (int i=0; i<matID; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void * readInput(int num_args , ...){
    va_list args;
    va_start(args ,num_args);

    // process matrix file reading
    if(num_args == 1){
        int n = va_arg(args , int);  // matrix ID
        // memory allocation
        double** matrix = (double**)malloc(sizeof(double*) * n);
        for (int i = 0; i < n; i++) {
            matrix[i] = (double*)malloc(sizeof(double) * n);
        }

        // reading from file
        char* fileName = (char*)malloc(sizeof(char)*150);
        //printf("%smat_%06d.in\n", inputFilePath, n);
        sprintf(fileName, "%s/mat_%06d.in", inputFilePath, n);
        FILE* file = fopen(fileName, "r");

        if (!file) {
            perror("Error opening matrix file");
            return NULL;
        }

        // Reading line by line
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
    // process vector file reading
    else if(num_args == 2){
         int n = va_arg(args , int);  // matrix ID
         int vectorNumber = va_arg(args , int);

         // Memory allocation
        double* vector = (double*)malloc(sizeof(double) * n);

        // Filename
        char* fileName = (char*)malloc(sizeof(char)*150);
        //printf("%svec_%06d_%06d.in\n", inputFilePath, n, vectorNumber);
        sprintf(fileName, "%s/vec_%06d_%06d.in", inputFilePath, n, vectorNumber);
        FILE* file = fopen(fileName, "r");

        if (!file) {
            perror("Error opening vector file");
            return NULL;
        }
        // Reading the file
        char* line = NULL;
        size_t lenLen = 0;
        int row = 0;

        if (getline(&line, &lenLen, file) == -1) {
            perror("Error reading the file");
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

    // Compute the matrix-vector product: product = matrix * vector
    matrixVectorProduct(matrix, vector, n, MatVecProduct);

    // Find the first non-zero element in the vector
    int nonZeroIndex = -1;
    for (int i = 0; i < n; i++) {
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

    // Calculate the potential eigenvalue using the first non-zero element
    double potentialLambda = MatVecProduct[nonZeroIndex] / vector[nonZeroIndex];

    // Verify the eigenvalue condition for all other elements
    for (int i = nonZeroIndex + 1; i < n; i++) {
        if (vector[i] != 0) {
            double difference = fabs(MatVecProduct[i] - potentialLambda * vector[i]);
            if (difference > 1e-6) { // Allow for floating-point tolerance
                free(MatVecProduct);
                return false;
            }
        }
    }

    // Assign the calculated eigenvalue as a double and clean up
    *lambda = potentialLambda;
    free(MatVecProduct);
    return true;
}

void matrixVectorProduct(double** matrix, double* vector, int matID, double* product){
    for (int i = 0; i < matID; i++) {  // Loop through rows of the matrix
        double sum = 0; // Temporary variable to store the sum
        for (int j = 0; j < matID; j++) {  // Loop through columns of the matrix
            sum += matrix[i][j] * vector[j];
        }
        product[i] = sum; // Assign the computed value
    }
}

void fileWrite(int matID, int vectorNumber, double eigenVal) {
    // Construct the filename

    char* fileName = (char*)malloc(sizeof(char)*150);
    sprintf(fileName, "%s/vec_%06d_%06d.in", inputFilePath, matID, vectorNumber);
    // Open the file in append mode
    FILE* outfile = fopen(fileName, "a");
    if (!outfile) {
        perror("Error opening file");
        return;
    }

    // Append the eigenvalue to the file
    fprintf(outfile, "\nEigenvalue: %.6f\n", eigenVal);

    // Close the file
    fclose(outfile);
}

// Function to log the average execution time
void CalTime(int matID, double totalTime, int iterations) {
    char fileName[150];
    sprintf(fileName, "%s/average_time.log",time_log);

    FILE* file = fopen(fileName, "a");
    if (!file) {
        perror("Error opening timing file");
        return;
    }

    double avgTime = totalTime / iterations;
    fprintf(file, "MatrixID: %06d AvgTime: %.6f seconds\n", matID, avgTime);

    fclose(file);
}