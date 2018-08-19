#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "pgm.h"

#define REPETITIONS 10
#define ITERATIONS 50

int getPixelCPU(PGMData *input,int y, int x)
{
    if( x<0 || x>=input->width )
        return 0;
    if( y<0 || y>=input->height )
        return 0;
    return input->image[y*input->width+x];
}

int returnIndex(PGMData *input, int x, int y){
    if( x<0 || x>=input->width )
        return 0;
    if( y<0 || y>=input->height )
        return 0;
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
    PGMData slikaInput, slikaCPU, final;
    
    readPGM("image.pgm",&slikaInput);
    slikaCPU.height=slikaInput.height;
    slikaCPU.width=slikaInput.width;
    slikaCPU.max_gray=slikaInput.max_gray;
    
    final.height=slikaInput.height;
    final.width=slikaInput.width - ITERATIONS;
    final.max_gray=slikaInput.max_gray;
    
    //CPU del
    slikaCPU.image=(int *)malloc(slikaCPU.height*slikaCPU.width*sizeof(int));
    final.image=(int *)malloc(final.height*final.width*sizeof(int));
    int *kumulativa = (int *) malloc(slikaCPU.height*slikaCPU.width*sizeof(int));
    int *removes = (int *) malloc(slikaCPU.height*sizeof(int));
    
    sobelCPU(&slikaInput, &slikaCPU);
    
    
    // Izračun kumulativ
    int down_left = 2147483647, down = 2147483647, down_right = 2147483647;
    for(int i = slikaCPU.height - 1; i >= 0; i--){
        for (int j = 0; j < slikaCPU.width; j++) {
            kumulativa[returnIndex(&slikaCPU, j, i)] = slikaCPU.image[returnIndex(&slikaCPU, j, i)];
            
            down = slikaCPU.image[returnIndex(&slikaCPU, j, i)];
            if(j - 1 >= 0){
                down_left = slikaCPU.image[returnIndex(&slikaCPU, j - 1, i + 1)];
            }
            if(j + 1 >= slikaCPU.width){
                down_right = slikaCPU.image[returnIndex(&slikaCPU, j + 1, i + 1)];
            }
            kumulativa[returnIndex(&slikaCPU, j, i)] += min(min(down, down_left), down_right);
            down_left = 2147483647, down = 2147483647, down_right = 2147483647;
        }
    }
    
    // Iskanje in odstranitev šiva
    int min_number = 2147483647, min_index = 0, replace_index;
    down_left = 2147483647, down = 2147483647, down_right = 2147483647;
    
    for (int iteration = 0; iteration < ITERATIONS; iteration++) {
        // Najdi min vrednost v prvi vrstici
        for (int j = 0; j < slikaCPU.width - iteration; j++) {
            if(kumulativa[returnIndex(&slikaCPU, j, 0)] < min_number){
                min_number = kumulativa[returnIndex(&slikaCPU, j, 0)];
                min_index = j;
            }
        }
        removes[0] = min_index;
        for(int i = 1; i < slikaCPU.height; i++){
            down = kumulativa[returnIndex(&slikaCPU, min_index, i)];
            
            if(min_index - 1 >= 0){
                down_left = kumulativa[returnIndex(&slikaCPU, min_index - 1, i)];
            }
            if(min_index + 1 <= slikaCPU.width){
                down_right = kumulativa[returnIndex(&slikaCPU, min_index + 1, i)];
            }
            min_number = min(min(down, down_left), down_right);
            
            if(min_number == down_left){
                min_index--;
            } else if(min_number == down_right){
                min_index++;
            }
            
            removes[i] = min_index;
            down_left = 2147483647, down = 2147483647, down_right = 2147483647;
        }
        for(int i = 0; i < slikaCPU.height; i++){
            for(int j = removes[i] + 1; j < slikaCPU.width; j++){
                replace_index = returnIndex(&slikaCPU, j, i);
                kumulativa[replace_index - 1] = kumulativa[replace_index];
                slikaInput.image[replace_index - 1] = slikaInput.image[replace_index];
            }
        }
        slikaCPU.width -= 1;
    }
    
    printf("%d %d %d\n", slikaInput.width, slikaCPU.width, final.width);
    
    for (int i = 0; i < final.width; i++) {
        for (int j = 0; j < final.height; j++) {
            final.image[returnIndex(&final, i, j)] = slikaInput.image[returnIndex(&final, i, j)];
        }
    }
    
    writePGM("seam.pgm", &final);
    
    
    //writePGM("sobelCPU.pgm",&slikaCPU);
    free(kumulativa);
    free(removes);
    
    return 0;
}

