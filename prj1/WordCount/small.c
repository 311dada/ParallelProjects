#include <mpi.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#define MAX_FILE 100


char* add_head(char* filename, char* head)
{
    char* file_path;
    file_path = (char*)malloc(MAX_FILE);
    strcpy(file_path, head);
    strcat(file_path, filename);
    return file_path;
}



int is_white_space(char ch)
{
    if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\f' || ch == '\t' || ch == '\v')
        return 1;
    return 0;
}


int count_words(char* filename)
{
    FILE *in;
    in = fopen(filename, "r");
    char ch=' ';
    char pre=' ';
    char temp;
    int count=0;

    while((temp=fgetc(in)) != EOF)
    {
        ch = temp;
        if (is_white_space(ch) && (!is_white_space(pre)))
            ++count;
        pre = ch;
    }

    if (!is_white_space(ch))
        ++count;

    fclose(in);
    return count;
}



int main(int argc, char** argv)
{
    struct dirent *de;

    DIR *dr = opendir("./small_file");

    char **files = NULL;
    int file_num = 0;
    char *filename;
    int file_len;
    

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int to_proc_num=0;
    int to_send;
    int global_count=0;
    int local_count=0;

    double start, end;
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    if (world_rank)
        filename = (char*)malloc(MAX_FILE);

    if (!world_rank)
    {
        
        // Distribute tasks
        while ((de = readdir(dr)) != NULL) 
        {
            if (de->d_name[0] == '.')
                continue;
            to_send = file_num % world_size;
            filename = add_head(de->d_name, "small_file/");
            if (to_send)
            {
                file_len = strlen(filename);
                MPI_Send(&file_len, 1, MPI_INT, to_send, 0, MPI_COMM_WORLD);
                MPI_Send(filename, file_len + 1, MPI_CHAR, to_send, 1, MPI_COMM_WORLD);
            }
                
            else
            {
                to_proc_num++;
                files = (char**)realloc(files, to_proc_num * sizeof(char*));
                files[to_proc_num - 1] = strdup(filename);
            }
            ++file_num;
            free(filename);
        }

        // Send terminal signal
        int terminal = -1;
        for (int i = 1; i < world_size; i++)
        {
            MPI_Send(&terminal, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        for (int i = 0; i < to_proc_num; i++)
        {
            local_count += count_words(files[i]);
        }
    }

    
    // Process tasks
    else
    {
        while (1)
        {
            MPI_Recv(&file_len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            if (file_len == -1)
                break;
            MPI_Recv(filename, file_len + 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            local_count += count_words(filename);
        }
        
    }

    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    if (!world_rank)
    {
        for (int i = 0; i < to_proc_num; i++)
        {
            free(files[i]);
        }
        free(files);
        printf("total word count: %d\n", global_count);
        printf("time: %f seconds\n", end - start);
        closedir(dr);
    }

    else
    {
        free(filename);
    }
    MPI_Finalize();

    return 0;
}