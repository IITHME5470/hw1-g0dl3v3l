#include <stdio.h>
#include <stdlib.h>

char * inputFile = "input.in";
char* outPutFileASCI = "../q1/ASCI";
char* outPutFileBinary = "../q1/binary" ;


double** create_array(int n);
void print_to_file(double**array , int n, int format_flag);
void process(int format_flag);
void memClear(double** array , int n);



int main(){
    int format_flag;
    printf("Enter format flag (ASCII - 0, Binary - 1): ");
    scanf("%d",&format_flag);
    //printf("\n");
    process(format_flag);
}


void process(int format_flag){
    FILE* file = fopen(inputFile,"r");

    if(!file){
        perror("Error opening file");
        return ;
    }

    int n;
    int NumLine;
    int row = 0;
    while(fscanf(file , "%d" , &n) == 1){
        double** array = create_array(n);
        print_to_file(array, n, format_flag); 
        memClear(array,n);
    }
    fclose(file);
}

void memClear(double** array , int n){
    for (int i=0; i<n; i++) {
            free(array[i]);
        }
        free(array);

}

double** create_array(int n){
    double** array = (double**)malloc(sizeof(double*)*n);
    int i;
    int j;
    for(i = 0; i< n;i++){
        array[i] = (double*)malloc(sizeof(double)*n);
        for(j = 0 ; j<n;j++){
            
            array[i][j] = i+j;
        }
    }
    return array;
}


void print_to_file(double**array , int n, int format_flag){
    char FileName[100];
    FILE* file;
    //writing to ASCII file
    if(format_flag == 0){
        printf("Created File : %s/array_%06d_asc.out\n" ,outPutFileASCI,n);
        sprintf(FileName , "%s/array_%06d_asc.out" ,outPutFileASCI,n);
        file = fopen(FileName , "w");
        if(!file){
            perror("Error opening file");
            return;
        }

        int i;
        int j;
        for(i = 0;i< n;i++){
            for(j = 0 ; j< n ;j++){
                fprintf(file,"%.15lf ",array[i][j]);
            }
            fprintf(file,"\n");
        }


    // writing to binnary file
    }else if (format_flag == 1) {  
        printf("Created File : %s/array_%06d_bin.out\n" ,outPutFileASCI,n);
        sprintf(FileName , "%s/array_%06d_bin.out" ,outPutFileBinary,n);
        file = fopen(FileName , "wb");
        for (int i=0; i<n; i++) {
            fwrite(array[i], sizeof(double), n, file);
        }
    }else{
        printf("Invalid format flag\n");
        return;
    }
    fclose(file);
    return;
}