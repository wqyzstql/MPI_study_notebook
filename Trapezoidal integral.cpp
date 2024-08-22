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
    auto start = std::chrono::high_resolution_clock::now();
    double a = 0, b = 2;
    int n = 1234567;
    int comm_sz, my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if (my_rank != 0) {
        double stride = 1.0 * (b - a) / n;
        double approx = (f(a) + f(b)) / 2;
        int step = n / (comm_sz - 1);
        int start = (my_rank - 1) * step;
        int end = my_rank * step;
        if (n < end) n = end;

        for (int i = start + 1; i <= end; i++)
            approx += f(a + i * stride);
        approx = approx * stride;
        MPI_Send(&approx, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        //printf("Thread %d is ok\n", my_rank);
    }
    else {
        double sum = 0;
        for (int i = 1; i < comm_sz; i++) {
            double approx;
            MPI_Recv(&approx, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += approx;
        }
        printf("%.5lf\n", sum);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_sycl = end - start;
        std::cout << "MPI code time: " << duration_sycl.count() << " seconds" << std::endl;
    }
    MPI_Finalize();
    return 0;
}
