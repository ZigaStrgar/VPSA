#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "pgm.h"
#include "omp.h"

#define REPETITIONS 10
#define ITERATIONS 300
#define MAX 2147483647

int getPixelCPU(PGMData *input,int y, int x)
{
    if( x<0 || x>=input->width )
        return 0;
    if( y<0 || y>=input->height )
        return 0;
    return input->image[y*input->width+x];
}

int returnIndex(PGMData *input, int x, int y){
    return y * input->width + x;
}

int min(int a, int b){
    return a < b ? a : b;
}

void sobelCPU(PGMData *input, PGMData *output)
{
    int i, j;
    int Gx, Gy;
    int tempPixel;
    
    //za vsak piksel v sliki
    for(i=0; i<(input->height); i++)
        for(j=0; j<(input->width); j++)
        {
            Gx = -getPixelCPU(input,i-1,j-1)-2*getPixelCPU(input,i-1,j)-
            getPixelCPU(input,i-1,j+1)+getPixelCPU(input,i+1,j-1)+
            2*getPixelCPU(input,i+1,j)+getPixelCPU(input,i+1,j+1);
            Gy = -getPixelCPU(input,i-1,j-1)-2*getPixelCPU(input,i,j-1)-
            getPixelCPU(input,i+1,j-1)+getPixelCPU(input,i-1,j+1)+
            2*getPixelCPU(input,i,j+1)+getPixelCPU(input,i+1,j+1);
            tempPixel = sqrt((float)(Gx*Gx+Gy*Gy));
            if( tempPixel>255 )
                output->image[i*output->width+j] = 255;
            else
                output->image[i*output->width+j] = tempPixel;
        }
}


int main(int argc, char *argv[])
{
    PGMData original, sobel, final, kumulativa;
    
    readPGM("/Users/zigastrgar/Code/College/VPSA/Seminarska/Seminarska/image.pgm",&original);
    
    sobel.height=original.height;
    sobel.width=original.width;
    sobel.max_gray=original.max_gray;
    
    kumulativa.height=original.height;
    kumulativa.width=original.width;
    kumulativa.max_gray=original.max_gray;
    
    final.height=original.height;
    final.width=original.width - ITERATIONS;
    final.max_gray=original.max_gray;
    
    //CPU del
    sobel.image=(int *)malloc(sobel.height*sobel.width*sizeof(int));
    final.image=(int *)malloc(final.height*final.width*sizeof(int));
    kumulativa.image = (int *) malloc(sobel.height*sobel.width*sizeof(int));
    
    //sobelCPU(&original, &sobel);
    
    double start = omp_get_wtime();

    int left = MAX, right = MAX, center = MAX, minimum = MAX, minimum_index = -1, index;
    
    for (int iteration = 0; iteration < ITERATIONS; iteration++) {
        //kumulativa.image = (int *) malloc(sobel.height*sobel.width*sizeof(int));
        sobelCPU(&original, &sobel);
        
        for (int i = 0; i < sobel.height * sobel.width; i++) {
            kumulativa.image[i] = sobel.image[i];
        }
        
        // Izračunaj kumulativo
        for (int y = sobel.height - 2; y >= 0; y--) {
            for (int x = 0; x < sobel.width; x++) {
                index = returnIndex(&sobel, x, y + 1);
                center = kumulativa.image[index];
                
                if(x - 1 >= 0){
                    left = kumulativa.image[index - 1];
                }
                
                if(x + 1 < sobel.width){
                    right = kumulativa.image[index + 1];
                }
                
                minimum = min(min(left, center), right);
                
                kumulativa.image[returnIndex(&sobel, x, y)] += minimum;
                
                left = center = right = minimum = MAX;
            }
        }
        
        // Dobi šiv
        // 1. v prvi vrstici minimum
        // 2. iz tega minimuma glej spodnjo vrstico in delaj šiv
        for (int x = 0; x < sobel.width; x++) {
            if(kumulativa.image[x] < minimum){
                minimum = kumulativa.image[x];
                minimum_index = x;
            }
        }
        
        sobel.image[minimum_index] = -1;
        
        for (int y = 1; y < sobel.height; y++) {
            index = returnIndex(&sobel, minimum_index, y);
            center = kumulativa.image[index];
            
            if(minimum_index - 1 >= 0){
                left = kumulativa.image[index - 1];
            }
            
            if(minimum_index + 1 < sobel.width){
                right = kumulativa.image[index + 1];
            }
            
            minimum_index += left < center ? (left < right ? -1 : 1) : (center < right ? 0 : 1);
            
            sobel.image[returnIndex(&sobel, minimum_index, y)] = -1;
            
            left = center = right = minimum = MAX;
        }
        
        // Odstrani šiv
        for(int i = 0, j = 0; i < sobel.height * sobel.width; i++){
            if(sobel.image[i] != -1){
                sobel.image[j] = sobel.image[i];
                //kumulativa.image[j] = sobel.image[i];
                original.image[j] = original.image[i];
                j++;
            }
        }

    /*
        float max = 0;
        for (int i = 0; i < sobel.height * sobel.width; i++) {
            if(max < kumulativa.image[i]){
                max = kumulativa.image[i];
            }
        }
        
        for (int i = 0; i < sobel.height * sobel.width; i++) {
            kumulativa.image[i] = (int)(kumulativa.image[i] / max * 255);
        }
        */
        
        //writePGM("/Users/zigastrgar/Code/College/VPSA/Seminarska/Seminarska/kumulativa.pgm", &kumulativa);
        
        sobel.width -= 1;
        original.width -= 1;
        //kumulativa.width -= 1;
    }
    
    for (int i = 0; i < final.height * final.width; i++) {
        final.image[i] = original.image[i];
    }
    
    printf("Time CPU: %.20g\n", omp_get_wtime() - start);
    
    writePGM("/Users/zigastrgar/Code/College/VPSA/Seminarska/Seminarska/seam.pgm", &final);
    
    writePGM("/Users/zigastrgar/Code/College/VPSA/Seminarska/Seminarska/sobelC.pgm", &sobel);

    //free(kumulativa);
    
    return 0;
}
