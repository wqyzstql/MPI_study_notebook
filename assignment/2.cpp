#include <cstdio>
#include <mpi.h>
#include <cstring>
#include <iostream>
#include <chrono>
#include <time.h>

using namespace std;

int main(int argc, char* argv) {
    srand((unsigned)(long long)argv);
    int comm_sz, my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    long long n = 0 , hit = 0 ,sum = 0;
    if (my_rank == 0) {
        n = 1000000000;
        n /= comm_sz;
        for (int i = 1; i < comm_sz; i++)
            MPI_Ssend(&n, 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD);
    }
    else MPI_Recv(&n, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    auto tstart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        double offset = rand() % 20;
        offset /= 10;
        double x = -1 + offset;
        offset = rand() % 20;
        offset /= 10;
        double y = -1 + offset;
        //cout << x << " " << y << endl;
        double dis = x * x + y * y;
        if (dis < 1)    hit++;
    }
    MPI_Reduce(&hit, &sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {
        cout << 4.0 * sum / (n * comm_sz);
        auto tend = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration_sycl = tend - tstart;
        std::cout << "MPI code time: " << duration_sycl.count() << " seconds" << std::endl;
    }
        
    MPI_Finalize();
    return 0;
}

