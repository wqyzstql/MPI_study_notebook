#include <cstdio>
#include <mpi.h>
#include <cstring>

const int MAX_STRING = 100;

int main(){
    char greeting[MAX_STRING];
    int comm_sz, my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank != 0){
        //printf("Greetings from process %d of %d!\n", my_rank, comm_sz);
        sprintf_s(greeting, "Working from process %d of %d!", my_rank, comm_sz);
        MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }//MPI_Send: msg_p, msg_size: +1 means '\0', msg_type, dest, tag, comm
    else {
        printf("Greetings from process %d of %d!\n", my_rank, comm_sz);
        for (int q = 1; q < comm_sz; q++) {
            MPI_Status status;
            //use MPI_Status to store the source and tag of recved msg
            MPI_Recv(greeting, MAX_STRING, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            printf("%s\n", greeting);
            //printf("%d %d\n", status.MPI_SOURCE, status.MPI_TAG);
            int number;
            MPI_Get_count(&status, MPI_CHAR, &number);
            //use MPI_Get_count to get the count of recved msg
            printf("%d\n", number);
        }//MPI_Recv: msg_p, msg_size, msg_type, source, tag, comm, status_error
        //ANY_SOURCE: break the dependency
    }
    MPI_Finalize();
    return 0;
}
/*
*   recv.comm = send.comm
*   recv.tag = send.tag
*   dest = r && src = q
*   recv_type = send_type
*   recv_size >= send_size
*/
