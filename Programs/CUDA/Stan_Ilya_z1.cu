#include <iostream>
#include <cmath>
#define N 4

using namespace std;

/*
Задача:
 Решить СЛАУ (Ax = f, A - матрица N*N) итерационным методом.
 Выбранный метод - Метод простой итерации (Simple iteration method / SIM):  x(k+1) = A'* x(k) + t*f, где
 t = const, 0 < t < 2/||A|| (Нашёл теорему: А=А*>0, 0 < тау < 2/||A|| => SIM сходится).
 A' = E - t*A

 Алгоритм, описанный здесь, сходится не для всех А. Проверок на сходимость нет. Если не сходится, то будет ломаться
или бесконечно считать итерации.

 Глобальные функции-ядра помечены G_.
 У функций, использующих редукцию, написано (reduction)
 (Использовал reduce3)
 У функций, использующих ядра, написано uses G_<название ядра>

 Используемые нормы: для поиска тау - Эйлерова норма, для eps (x(k+1) - x(k)) - L1 норма (сумма модулей).
*/

//_______________________________functions (prototypes)

__global__ void G_alphaMulVec(float *vec, unsigned long int pitch, float mul); // alpha*vector

__global__ void G_vecSubVec(float *resVec, float *vec); // vector-vector

__global__ void G_vecPlusVec(float *resVec, float *vec); // vector+vector

__global__ void G_E_plus_A(float A[N * N], unsigned long int pitch); // E+A

__global__ void G_vecMulVec(float *v1, float *v2, float *res); // vector*vector=res (reduction)

void matrMulVec(float A[N * N], unsigned long int pitch, float vec[N], float *res); // A*vector=res, uses vecMulVec

__global__ void G_absSumOfVecElems(float *vecIn, double *vecOut); // abs(v0)+abs(v1)+abs(v2)+...=res; (reduction)

__global__ void G_squaredEulerNorm(float *In, unsigned long int pitch, float *vecOut); // ||A||^2, Euler (reduction)

__global__ void G_sumOfVecElems(float *vecIn, float *vecOut); // v0+v1+v2+...=res; (reduction)

float coeffSIM(float *A, unsigned long int pitch); // coeff. for simple iter. method = 2/(||A|| + 1), uses G_squaredEulerNorm and G_sumOfVecElems

void coutMatr(float A[N * N]);

//__________________________________________________main()

int main()
{
    // Host init.

    double *eps, H_eps = 1; // eps- on device, H_eps= on Host
    float A[N * N] = {5, 2, 2, 0,
                      3, 10, 4, 0,
                      3, 4, 18, 0,
                      2, 0, 4, 7}; // ||A||^2 = 576 = 24*24 => tau = 0.08 = 2/(24+1)
    // answer: (16/35, 1/5, 11/70, -54/245)
    unsigned long int pitch;
    float f[N] = {3, 4, 5, 0};
    float x0[N] = {1, 0, 1, 2}; // any x0
    float tau;

    // Device init.

    float *D_A, *Df, *Dx0, *Dtmp; // pointers on Device

    cudaMallocPitch((void **)&D_A, &pitch, N * sizeof(float), N);
    cudaMemcpy2D(D_A, pitch, A, N * sizeof(float), N * sizeof(float), N, cudaMemcpyDefault);
    pitch /= sizeof(float); // pitch - ammount of floats in a raw

    cudaMalloc((void **)&Df, N * sizeof(float));
    cudaMemcpy(Df, f, N * sizeof(float), cudaMemcpyDefault);

    cudaMalloc((void **)&Dtmp, N * sizeof(float));

    cudaMalloc((void **)&Dx0, N * sizeof(float));
    cudaMemcpy(Dx0, x0, N * sizeof(float), cudaMemcpyDefault);

    cudaMallocHost((void **)&eps, sizeof(double));
    // if eps in managed => time=1.7 ms. If eps in host => time=0.48 ms. If eps on device => time=0.53 ms. (1660 ti)

    //  x(k+1) = A'* x(k) + t*f

    tau = coeffSIM(D_A, pitch);

    G_alphaMulVec<<<N, N>>>(D_A, pitch, -tau); // -xA
    G_E_plus_A<<<1, N>>>(D_A, pitch);          // E-xA = E+(-xA) = A'
    G_alphaMulVec<<<1, N>>>(Df, 0, tau);

    // ready to start SIM

    // output of new Matrix А'
    cudaMemcpy2D(A, N * sizeof(float), D_A, pitch * sizeof(float), N * sizeof(float), N, cudaMemcpyDefault);
    cout << "Matrix A' = E - tA:\n";
    coutMatr(A);

    cudaEvent_t Ev1, Ev2; // time measurement
    cudaEventCreate(&Ev1);
    cudaEventRecord(Ev1, 0);

    // iterations

    while (H_eps > 0.001) // tmp = x(k+1), x0 = x(k)
    {
        matrMulVec(D_A, pitch, Dx0, Dtmp);
        G_vecPlusVec<<<1, N>>>(Dtmp, Df);
        G_vecSubVec<<<1, N>>>(Dx0, Dtmp);

        G_absSumOfVecElems<<<1, N>>>(Dx0, eps); // new epsilon
        cudaMemcpy(&H_eps, eps, sizeof(double), cudaMemcpyDefault);
        cudaMemcpy(Dx0, Dtmp, N * sizeof(float), cudaMemcpyDeviceToDevice); // x(k)<->x(k+1)
    }

    cudaEventCreate(&Ev2); // time measurement
    cudaEventRecord(Ev2, 0);
    cudaEventSynchronize(Ev2);
    cudaEventElapsedTime(&tau, Ev1, Ev2);
    cudaEventDestroy(Ev1);
    cudaEventDestroy(Ev2);
    cout << "\ntime: " << tau << endl;

    // result output

    cudaMemcpy(x0, Dx0, N * sizeof(float), cudaMemcpyDefault);
    cout << "____________\n";
    for (int i = 0; i < N; ++i)
    {
        cout << x0[i] << endl;
    }
    cout << "____________\n";

    // frees

    cout << "CUDA_ERROR: " << cudaGetErrorString(cudaGetLastError()) << endl;
    cudaFree(D_A);
    cudaFree(Dx0);
    cudaFree(Dtmp);
    cudaFreeHost(eps);
    cudaFree(Df);

    return 0;
}

//___________________________________________functions (definitions)

__global__ void G_alphaMulVec(float *vec, unsigned long int pitch, float mul)
{
    vec[pitch * blockIdx.x + threadIdx.x] *= mul; // Nxblocks, Nxthreads
}

__global__ void G_vecSubVec(float *resVec, float *vec)
{
    resVec[threadIdx.x] -= vec[threadIdx.x];
}

__global__ void G_vecPlusVec(float *resVec, float *vec)
{
    resVec[threadIdx.x] += vec[threadIdx.x];
}

__global__ void G_E_plus_A(float A[N * N], unsigned long int pitch)
{
    unsigned int tid = threadIdx.x;
    ++A[pitch * tid + tid];
}

__global__ void G_vecMulVec(float *v1, float *v2, float *res)
{
    __shared__ float shMass[N];
    unsigned int tid = threadIdx.x;
    shMass[tid] = v1[tid] * v2[tid];
    __syncthreads();
    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1)
    {
        if (tid < s)
        {
            shMass[tid] += shMass[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0)
        *res = shMass[0];
}

void matrMulVec(float A[N * N], unsigned long int pitch, float vec[N], float *res)
{
    for (int i = 0; i < N; i++)
        G_vecMulVec<<<1, N>>>(&A[pitch * i], vec, &res[i]);
}

__global__ void G_absSumOfVecElems(float *vecIn, double *vecOut)
{
    __shared__ float shMass[N];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    shMass[tid] = abs(vecIn[i]);
    __syncthreads();
    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1)
    {
        if (tid < s)
        {
            shMass[tid] += shMass[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0)
        vecOut[blockIdx.x] = shMass[0];
}

__global__ void G_squaredEulerNorm(float *In, unsigned long int pitch, float *vecOut)
{
    __shared__ float shMass[N];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * pitch + threadIdx.x;
    shMass[tid] = In[i] * In[i];
    __syncthreads();
    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1)
    {
        if (tid < s)
        {
            shMass[tid] += shMass[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0)
        vecOut[blockIdx.x] = shMass[0];
}

__global__ void G_sumOfVecElems(float *vecIn, float *vecOut)
{
    __shared__ float shMass[N];
    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    shMass[tid] = vecIn[i];
    __syncthreads();
    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1)
    {
        if (tid < s)
        {
            shMass[tid] += shMass[tid + s];
        }
        __syncthreads();
    }
    if (tid == 0)
        vecOut[blockIdx.x] = shMass[0];
}

float coeffSIM(float *A, unsigned long int pitch)
{
    float *D_coeff, coeff = 0;
    cudaMalloc((void **)&D_coeff, N * sizeof(float));
    G_squaredEulerNorm<<<N, N>>>(A, pitch, D_coeff);
    G_sumOfVecElems<<<1, N>>>(D_coeff, D_coeff);

    cudaMemcpy(&coeff, D_coeff, sizeof(float), cudaMemcpyDefault);
    cudaFree(D_coeff);
    coeff = 2 / (sqrt(coeff) + 1);
    return coeff; // tau < 2/||A|| (if A=A*>0 => simple iter. will converge (theory))
}

void coutMatr(float A[N * N])
{
    for (int j = 0; j < N; j++)
    {
        for (int i = 0; i < N; i++)
        {
            printf("%.3f  ", A[j * N + i]);
        }
        cout << endl;
    }
}
