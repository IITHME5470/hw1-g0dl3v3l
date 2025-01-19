#include <stdio.h>
#include <stdlib.h>

// Input and output file paths
char *inputFile = "input.in";
char *outPutFileASCI = "../q1/ASCI";   // Path for ASCII output files
char *outPutFileBinary = "../q1/binary"; // Path for binary output files

// Function declarations
double** create_array(int n);
void print_to_file(double** array, int n, int format_flag);
void process(int format_flag);
void memClear(double** array, int n);

int main() {
    int format_flag;
    // Prompt user to enter format flag
    printf("Enter format flag (ASCII - 0, Binary - 1): ");
    scanf("%d", &format_flag);

    // Process the file based on the format flag
    process(format_flag);

    return 0;
}

// Function to process the input file and create output files
void process(int format_flag) {
    FILE* file = fopen(inputFile, "r");

    if (!file) {
        perror("Error opening file");
        return;
    }

    int n;
    while (fscanf(file, "%d", &n) == 1) {
        // Create a 2D array of size n x n
        double** array = create_array(n);
        
        // Write the array to the file in the specified format
        print_to_file(array, n, format_flag);
        
        // Free allocated memory
        memClear(array, n);
    }
    fclose(file);
}

// Function to free allocated memory for the 2D array
void memClear(double** array, int n) {
    int i;
    for (i = 0; i < n; i++) {
        free(array[i]);  // Free each row
    }
    free(array);  // Free the array of pointers
}

// Function to create a 2D array of size n x n and initialize values
double** create_array(int n) {
    // Allocate memory for the 2D array
    double** array = (double**)malloc(sizeof(double*) * n);
    int i;
    for (i = 0; i < n; i++) {
        array[i] = (double*)malloc(sizeof(double) * n);
        int j;
        for (j = 0; j < n; j++) {
            array[i][j] = i + j;  // Initialize values based on row and column indices
        }
    }
    return array;
}

// Function to write the 2D array to a file in ASCII or binary format
void print_to_file(double** array, int n, int format_flag) {
    char FileName[100];
    FILE* file;

    // Writing to an ASCII file
    if (format_flag == 0) {
        printf("Created File: %s/array_%06d_asc.out\n", outPutFileASCI, n);
        sprintf(FileName, "%s/array_%06d_asc.out", outPutFileASCI, n);
        file = fopen(FileName, "w");

        if (!file) {
            perror("Error opening ASCII file");
            return;
        }
        int i;
        for (i = 0; i < n; i++) {
            int j;
            for (j = 0; j < n; j++) {
                fprintf(file, "%.15lf ", array[i][j]);  // Write values with high precision
            }
            fprintf(file, "\n");
        }

    // Writing to a binary file
    } else if (format_flag == 1) {
        printf("Created File: %s/array_%06d_bin.out\n", outPutFileBinary, n);
        sprintf(FileName, "%s/array_%06d_bin.out", outPutFileBinary, n);
        file = fopen(FileName, "wb");

        if (!file) {
            perror("Error opening binary file");
            return;
        }
        int i;
        for (i = 0; i < n; i++) {
            fwrite(array[i], sizeof(double), n, file);  // Write array data in binary format
        }

    } else {
        printf("Invalid format flag\n");
        return;
    }

    fclose(file);  // Close the output file
}
