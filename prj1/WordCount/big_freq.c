#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


int is_white_space(char ch)
{
    if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\f' || ch == '\t' || ch == '\v')
        return 1;
    return 0;
}


int get_file_size(char *filename)
{
    struct stat statbuf;
    stat(filename,&statbuf);
    int size=statbuf.st_size;

    return size;
}

void adjust_offset(FILE *f, int *offset)
{
    int ch;
    int little_offset = 0;
    while (1)
    {
        ch = fgetc(f);
        
        if (ch == EOF || is_white_space(ch))
            break;
        ++little_offset;
    }
    if (is_white_space(ch))
        ++little_offset;
    *offset = *offset + little_offset;
}

int count_words(FILE *f, int start, int offset)
{
    fseek(f,(long)start, 0);
    char ch=' ';
    char pre=' ';
    char temp;
    int count=0;

    for (int i = 0; i < offset; i++)
    {
        ch = fgetc(f);
        if (is_white_space(ch) && (!is_white_space(pre)))
            ++count;
        pre = ch;
    }

    if (!is_white_space(ch))
        ++count;

    return count;
}


int main(int argc, char **argv)
{

    int offset = 0;
    int start = 0;

    int global_count = 0;
    int local_count = 0;
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    double begin, end;
    MPI_Barrier(MPI_COMM_WORLD);
    begin = MPI_Wtime();

    if (!world_rank)
    {
        int size;
        long block;
        int next_start=0;
        int next_offset=0;
        size = get_file_size("./big_file/big_100.txt");

        FILE *in;
        in = fopen("./big_file/big_100.txt", "r");

        block = size / world_size;
        offset = block;
        fseek(in, block, 1);
        adjust_offset(in, &offset);
        size -= offset;
        next_start = offset;

        // Send to slaves
        for (int i = 1; i < world_size; i++)
        {
            if (i == world_size - 1)
                block = size;
            else
                block = size / (world_size - i);
            next_offset = block;
            fseek(in, block, 1);
            adjust_offset(in, &next_offset);
            
            // Send start and offset
            MPI_Send(&next_start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&next_offset, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

            // Update start and size
            next_start += next_offset;
            size -= next_offset;
        }
        
        local_count = count_words(in, start, offset);
        fclose(in);
    }

    else
    {
        MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        FILE *in;
        in = fopen("./big_file/big_100.txt", "r");
        local_count = count_words(in, start, offset);
        fclose(in);
    }

    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (!world_rank)
    {
        end = MPI_Wtime();
        printf("total words count: %d\n", global_count);
        printf("time: %f seconds\n", end - begin);
    }
    
    MPI_Finalize();
    
    
    return 0;
}