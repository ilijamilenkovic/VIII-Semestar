#include <stdio.h>
#include <cuda.h>
#define BLOCKCOUNT 256
#define BLOCKSIZE 256
#define N 20
#define p 1.15f


__global__ void calculate(int* devA, int* devB, int*devC){
    __shared__ int localA[BLOCKSIZE];
    __shared__ int localB[BLOCKSIZE];

    int pid = blockIdx.x*blockDim.x + threadIdx.x;
    if(threadIdx.x == 0){
        localA[threadIdx.x] = devA[pid];
        localB[threadIdx.x] = devB[pid];

        localA[threadIdx.x+1] = devA[pid+1];
        localB[threadIdx.x+1] = devB[pid+1];

        localA[threadIdx.x+2] = devA[pid+2];
        localB[threadIdx.x+2] = devB[pid+2];
    }
    else{
        //jer je nulti vec pribavio dev[pid] i dev[pid+1]
        localA[threadIdx.x+2] = devA[pid+2]; 
        localA[threadIdx.x+2] = devB[pid+2]; 
    }
    __syncthreads();

    devC[pid] = (localA[threadIdx.x] + localA[threadIdx.x+1] + localA[threadIdx.x+2])*p + (localB[threadIdx.x] + localB[threadIdx.x+1] + localB[threadIdx.x+2]) * (1-p);

}


int main(int argc, char** argv){
    int* hostA, *hostB, *hostC;
    int* devA, *devB, *devC;

    hostA = new int[N];
    hostB = new int[N];
    hostC = new int[N-2];

    cudaMalloc((void**)&devA, N*sizeof(int));
    cudaMalloc((void**)&devB, N*sizeof(int));
    cudaMalloc((void**)&devC, (N-2)*sizeof(int));

    for (int i = 0; i < N; i++)
    {
        A[i] = i*i;
        B[i] = i;
    }
    


    cudaMemcpy(devA, hostA, N*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(devB, hostB, N*sizeof(int), cudaMemcpyHostToDevice);

    calculate<<<256,256>>>(devA,devB,devC);

    cudaMemcpy(hostC, devC, (N-2)*sizeof(int), cudaMemcpyDeviceToHost);

    cudaFree(devA);
    cudaFree(devB);
    cudaFree(devC);


    delete hostA;
    delete hostB;
    delete hostC;


}
