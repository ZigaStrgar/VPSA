
#include <stdio.h>

__global__ void scepec(void);

int main(int argc, const char * argv[]){

scepec <<<1,1>>>();

printf("Pravkar sem poklical in izvedel scepec!");
return 0;
}

__global__ void scepec(void){

}
