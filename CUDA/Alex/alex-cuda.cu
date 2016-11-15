#include <stdio.h>

const int N = 25;
const int blocksize = 25;

__global__
void hello(char *a)
{
 
}

int main()
{
 char a[N] = "Hello World, for Dr. Li!";
 

 char *ad;
 
 const int csize = N*sizeof(char);


 printf("%s\n", a);

 cudaMalloc( (void**)&ad, csize );

 cudaMemcpy( ad, a, csize, cudaMemcpyHostToDevice );
 dim3 dimBlock(blocksize, 1 );
 dim3 dimGrid( 1, 1 );
 hello<<<dimGrid, dimBlock>>>(ad);
 cudaMemcpy( a, ad, csize, cudaMemcpyDeviceToHost );
 cudaFree( ad );

 printf("%s\n", a);
 return EXIT_SUCCESS;
}
