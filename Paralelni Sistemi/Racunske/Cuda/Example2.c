#include "book.h"
#define N 10

__global__ void add(int* a, int* b, int* c)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x; //blockDim = number of threads per dimension
    if (tid < N)
        c[tid] = a[tid] + b[tid];

}



int main(void)
{
    int a[N], b[N], c[N];
    int* dev_a;
    int* dev_b;
    int* dev_c;

    //mem alloc
    HANDLE_ERROR(cudaMalloc((void**)& dev_a, N * sizeof(int)));
    HANDLE_ERROR(cudaMalloc((void**)& dev_b, N * sizeof(int)));
    HANDLE_ERROR(cudaMalloc((void**)& dev_c, N * sizeof(int)));

    //initizalization of arrays
    for (int i=0; i<N; i++) 
    {
        a[i] = -i;
        b[i] = i * i;
    }

    //copying arrays from host to device
    HANDLE_ERROR(cudaMemcpy(dev_a, a, N*sizeof(int), cudaMemcpyHostToDevice));
    HANDLE_ERROR(cudaMemcpy(dev_b, b, N * sizeof(int), cudaMemcpyHostToDevice));

    //kernel call
    add<<<N,1>>>(dev_a, dev_b, dev_c);

    //copying result array from device to host
    HANDLE_ERROR(cudaMemcpy(c, dev_c, N*sizeof(int), cudaMemcpyDeviceToHost));

    cudaFree(dev_a);
    cudaFree(dev_b);
    cudaFree(dev_c);
    return 0;
}

