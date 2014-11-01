#ifndef BCAST_H
#define BCAST_H

#include <mpich/mpi.h>
#include <math.h>

#define TAG_MY_BCAST 301


int bcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    //init
    MPI_Status status;
    MPI_Comm comm_copy;
    MPI_Group group;
    int comm_size;
    int* ranks;
    int proc_rank;
    MPI_Comm_group(comm, &group);
    MPI_Group_size(group, &comm_size);
    ranks = new int[comm_size];
    for(int i = 0; i < comm_size; ++i) {
        ranks[i] = i;
    }
    //set root as 0
    ranks[0] = root;
    ranks[root] = 0;

    MPI_Group_incl(group, comm_size, ranks, &group);
    MPI_Comm_create(comm, group, &comm_copy);

    MPI_Comm_rank (comm_copy, &proc_rank);	/* get current process id */


    printf("Process %d start broadcast\n", proc_rank);

    //count process num for send
    int steps_count = (int)log2(comm_size);
    int border = 1 << steps_count;
    int tmp_rank = proc_rank | border;
    int i = 0; // pointer on first right 1
    while ((tmp_rank | (1 << i)) != tmp_rank) {
        ++i;
    }
    int start_step = steps_count - i;
    printf("Process %d: start_step = %d\n", proc_rank, start_step);
    if (start_step != 0) {
        int sender_id = proc_rank & ~(1 << i);
        printf("Process %d: sender_id = %d\n", proc_rank, sender_id);
        MPI_Recv(buffer, count, datatype, sender_id, TAG_MY_BCAST, comm_copy, &status);
    }
    int cur_step = start_step;
    printf("Process %d: i = %d\n", proc_rank, i);

    while(cur_step < steps_count) {
        int curr_reciver_id = proc_rank | (1 << (i - cur_step - 1 + start_step));
        printf("Process %d: curr_reciver_id = %d\n", proc_rank, curr_reciver_id);
        MPI_Send(buffer, count, datatype, curr_reciver_id, TAG_MY_BCAST, comm_copy);
        ++cur_step;
    }
    printf("Process %d: finished\n", proc_rank);
    return 0;
}

#endif // BCAST_H
