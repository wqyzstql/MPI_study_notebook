#include <cstdio>
#include <mpi.h>
#include <cstring>
#include <iostream>

using namespace std;

int main() {
    int comm_sz, my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int data_count = 0, bin_count = 0;
    double data[100] = { 0 }, local_data[100] = { 0 };
    double min_meas = 2147483647, max_meas =-1;
    double bin_width = 0;
    if (my_rank == 0) {
        data_count = 8, bin_count = 5;
        for (int i = 0; i < data_count; i++) {
            int tmp = rand() % 40 + 10;
            data[i] = 1.0 * tmp / 10;
            min_meas = min(min_meas, data[i]);
            max_meas = max(max_meas, data[i]);
            //cout << data[i] << endl;
        }
        bin_width = (max_meas - min_meas) / bin_count;
    }
    int array_of_blocklength[5] = {1, 1, 1, 1, 1};
    MPI_Aint addr[5] = { 0 };
    MPI_Get_address(&data_count, &addr[0]);
    MPI_Get_address(&bin_count, &addr[1]);
    MPI_Get_address(&min_meas, &addr[2]);
    MPI_Get_address(&max_meas, &addr[3]);
    MPI_Get_address(&bin_width, &addr[4]);
    MPI_Aint array_of_displacements[5] = { 0 };
    for (int i = 1; i < 5; i++)array_of_displacements[i] = addr[i] - addr[0];
    MPI_Datatype array_of_types[5] = { MPI_INT, MPI_INT, MPI_DOUBLE , MPI_DOUBLE, MPI_DOUBLE };
    MPI_Datatype INPUT_CONST;
    MPI_Type_create_struct(5, array_of_blocklength, array_of_displacements, array_of_types, &INPUT_CONST);
    MPI_Type_commit(&INPUT_CONST);
    MPI_Bcast(&data_count, 1, INPUT_CONST, 0, MPI_COMM_WORLD);
    MPI_Type_free(&INPUT_CONST);
    int local_n = data_count / comm_sz;
    MPI_Scatter(data, local_n, MPI_DOUBLE, local_data, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    //cout << my_rank << " " << local_n << " " << max_meas << " " << min_meas << endl;
    //for (int i = 0; i < local_n; i++) cout << "my_rank:" << my_rank << " data[" << i << "] is" << local_data[i] << endl;
    int bin_sum[5] = { 0 }, answer[5] = { 0 };
    for (int i = 0; i < local_n; i++) {
        int sign = (local_data[i] - min_meas) / bin_width;
        //cout << max_meas << " " << min_meas << " " << bin_width << endl;
        if (local_data[i] == max_meas) sign--;
        bin_sum[sign]++;
    }
    MPI_Reduce(bin_sum, answer, bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {
        for (int i = 0; i < bin_count; i++)
            cout << answer[i] << " ";
    }
    MPI_Finalize();
    return 0;
}

