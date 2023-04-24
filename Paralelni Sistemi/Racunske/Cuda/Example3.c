#include <stdio.h>
#define SIZE 16
#define BLOCKSIZE 4

__device__ int compare(int a, int b) 
{
  if (a == b) 
     return 1;
  return 0;
}

__global__ void compute(int* d_in_array, int* d_out_array){
    int tid = threadIdx.x;
    d_out_array[tid] = 0;
    if(tid < SIZE){
        for (int i=0; i<SIZE/BLOCKSIZE; i++) 
        {
            int val = d_in[i*BLOCKSIZE + threadIdx.x];  
            d_out[threadIdx.x] += compare(val, 6);
        }
    }
}

__host__ void outer_compute(int* in_arr, int* out_arr){
    //Alociranje memorije na uredjaju
    int *d_in_array, *d_out_array;

    cudaMalloc((void**)& d_in_array, SIZE * sizeof(int));
    cudaMalloc((void**)& d_out_array, SIZE * sizeof(int));

    cudaMemcpy(d_in_array, in_arr, SIZE * sizeof(int), cudaMemcpyHostToDevice);
    // do the computation
    compute<<<1,BLOCKSIZE>>>(d_in_array, d_out_array);
    

    cudaMemcpy(out_arr, d_out_arr, SIZE * sizeof(int), cudaMemcpyDeviceToHost);


}

int main(int argc, char** argv){
    int *in_array, *out_array;
    //inicijalizaija
    int sum = 0;


    outer_compute(in_array, out_array);

    
    for (int i = 0; i < BLOCKSIZE; i++)
    {
        sum+=out_array;
    }

    printf ("Result = %d\n",sum);

}