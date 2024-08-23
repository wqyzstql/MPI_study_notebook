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
    double a, b;
    int n = 10000000;
    if (my_rank == 0) 
        scanf_s("%lf%lf", &a, &b);
    int array_of_blocklength[2] = {1, 1};
    MPI_Aint array_of_displacements[2] = { 0 };
    MPI_Aint a_addr, b_addr;
    MPI_Get_address(&a, &a_addr);
    MPI_Get_address(&b, &b_addr);
    array_of_displacements[1] = b_addr - a_addr;
    MPI_Datatype array_of_types[2] = { MPI_DOUBLE, MPI_DOUBLE };
    MPI_Datatype input_a_b;
    MPI_Type_create_struct(2, array_of_blocklength, array_of_displacements, array_of_types, &input_a_b);
    MPI_Type_commit(&input_a_b);
    MPI_Bcast(&a, 1, input_a_b, 0, MPI_COMM_WORLD);
    MPI_Type_free(&input_a_b);
    auto tstart = std::chrono::high_resolution_clock::now();
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
