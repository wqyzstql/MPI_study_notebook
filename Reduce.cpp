#include <cstdio>
#include <mpi.h>
#include <cstring>
#include <chrono>
#include <iostream>

#define min(a,b) (a<b)?a:b

double f(double x) {
    return x * x;
}

int main() {
    int comm_sz, my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    double a, b, Inia=-1, Inib=-1;
    int n = 100000;
    if(my_rank==0)
        scanf_s("%lf%lf", &Inia, &Inib);
    auto tstart = std::chrono::high_resolution_clock::now();
    MPI_Allreduce(&Inia, &a, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&Inib, &b, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    double stride = 1.0 * (b - a) / n;
    double approx = (f(a) + f(b)) / 2;
    int step = n / comm_sz;
    int start = my_rank * step;
    int end = (my_rank+1) * step;
    //printf("%d %d %d\n", my_rank, start, end);
    if (n < end) n = end;
    for (int i = start + 1; i <= end; i++)
        approx += f(a + i * stride);
    approx = approx * stride;
    double sum = 0;
    MPI_Reduce(&approx, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if(my_rank == 0){
        printf("%.5lf\n", sum);
        auto tend = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_sycl = tend - tstart;
        std::cout << "MPI code time: " << duration_sycl.count() << " seconds" << std::endl;
    }
    MPI_Finalize();
    return 0;
}

