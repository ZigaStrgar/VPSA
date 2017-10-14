#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double *Random(int n);

double **Matrix(double *A, int n, int r);

double *Max(double *A, int n);

int main() {
    int n, r;
    clock_t start, stop;
    
    srand((unsigned) time(NULL));
    
    printf("Vnesi n: ");
    scanf("%d", &n);
    printf("Vnesi r: ");
    scanf("%d", &r);
    
    start = clock();
    double *vector = Random(n);
    stop = clock();
    
    double timer = (double) (stop-start) / CLOCKS_PER_SEC;
    
    double **array = Matrix(vector, n, r);
    double *max = Max(vector, n);
    
    printf("\n1D: \n");
    for (int i = 0; i < n; i++)
        printf("%f ", vector[i]);
    
    int cols = n / r;
    if (n % r != 0)
        cols++;
    
    printf("\n2D: \n");
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < cols; j++)
            printf("%f ", array[i][j]);
        printf("\n");
    }

    for (int k = 0; k < r; k++)
        free(array[k]);
     
    free(array);
    free(vector);
    
    printf("\nNajvecja vrednost: %f na naslovu: %p", *max, max);
    printf("\nCas generiranja nakljucnih stevil: %f s\n", timer);
}

double *Random(int n){
    double *vector= (double *) malloc(n * sizeof(double));
    
    for (int i = 0; i < n; i++) {
        vector[i] = ((double) rand() / (RAND_MAX));
    }
    
    return vector;
}

double **Matrix(double *A, int n, int r){
    double **array;
    int cols = n / r;
    if (n % r != 0)
        cols++;
    
    array = (double **) malloc(r * sizeof(double *));
    
    for (int i = 0; i < r; i++)
        array[i] = (double *) calloc(cols, sizeof(double));
    
    int i = 0;
    while (i < n) {
        array[i / cols][i % cols] = A[i];
        i++;
    }
    
    return array;
}

double *Max(double *A, int n) {
    double max = A[0];
    int max_index = 0;
    for (int i = 0; i < n; ++i) {
        if (A[i] > max) {
            max = A[i];
            max_index = i;
        }
    }
    return A + max_index;
}
