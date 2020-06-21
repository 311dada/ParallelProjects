#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 1024


int is_white_space(char ch)
{
    if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\f' || ch == '\t' || ch == '\v')
        return 1;
    return 0;
}


int count_words(char* line)
{
    char ch=' ';
    char pre=' ';
    char temp;
    int count=0;

    for (int i = 0; i < strlen(line); i++)
    {
        ch = line[i];
        if (is_white_space(ch) && (!is_white_space(pre)))
            ++count;
        pre = ch;
    }

    if (!is_white_space(ch))
        ++count;

    return count;
}


int main(int argc, char** argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char* line;
    int line_num = 0;
    int to_send;
    int line_len;
    int global_count = 0;
    int local_count = 0;
    line = (char*)malloc(MAX_LINE);

    if (!world_rank)
    {
        FILE *in;
        in = fopen("../big_file/big_100.txt", "r");
        while(fgets(line, MAX_LINE, in) != NULL)
        {
            to_send = line_num % (world_size - 1) + 1;
            line_len = strlen(line);
            MPI_Send(&line_len, 1, MPI_INT, to_send, 0, MPI_COMM_WORLD);
            MPI_Send(line, line_len + 1, MPI_CHAR, to_send, 1, MPI_COMM_WORLD);
            line_num++;
        }

        // Send terminal signal
        int terminal = -1;
        for (int i = 1; i < world_size; i++)
        {
            MPI_Send(&terminal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    else
    {
        while (1)
        {
            MPI_Recv(&line_len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (line_len == -1)
                break;
            
            MPI_Recv(line, line_len + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            local_count += count_words(line);
        }
    }
    
    MPI_Reduce(&local_count)


    MPI_Finalize();
    return 0;
}