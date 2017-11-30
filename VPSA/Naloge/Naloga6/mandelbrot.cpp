#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pgm.h"

/*double dtime(){
	double tseconds = 0.0;
	struct timeval mytime;

	gettimeofday(&mytime, (struct timezone*) 0);
	tseconds = (double) (mytime.tv_sec + mytime.tv_usec * 1.0e-6);
	return (tseconds);

}*/

void mandelbrotCPU(PGMData *I){
	float x0,y0,x,y,xtemp;
	int i,j;
	int color;
	int iter;
	int max_iteration=1000;   //stevilo iteracij
	
	//za vsak piksel v sliki
	for(i=0; i<I->height; i++)
		for(j=0; j<I->width; j++)
		{
			x0 = (float)j/I->width*(float)3.5-(float)2.5; //zacetna vrednost
			y0 = (float)i/I->height*(float)2.0-(float)1.0;
			x = 0;
			y = 0;
			iter = 0;
			while((x*x+y*y <= 4) && (iter < max_iteration))
			{ //ponavljamo, dokler ne izpolnemo enega izmed pogojev
				xtemp = x*x-y*y+x0;
				y = 2*x*y+y0;
				x = xtemp;
				iter++;
			}
			color = (int)(iter/(float)max_iteration*(float)I->max_gray); //pobarvamo piksel z ustrezno barvo
			I->image[i*I->width+j] = I->max_gray-color;
		}
}


int main(void) 
{
	double start;
	PGMData slika;

	slika.height=1000;
	slika.width=1000;
	slika.max_gray=255;

	slika.image=(int *)malloc(slika.height*slika.width*sizeof(int));
	
	//start=dtime();
	
	mandelbrotCPU(&slika);
	
	//printf("Time CPU: %d\n",dtime()-start);
	
	writePGM("mandelbort.pgm",&slika);
	return 0;
}
